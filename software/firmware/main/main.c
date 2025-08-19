#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"
#include "led_strip.h"
#include "firebase_app.h"
#include "cJSON.h"

static const char *TAG = "LUMA_LANTERN";

// WiFi configuration - these are now defaults that can be overridden
#define DEFAULT_WIFI_SSID "YOUR_WIFI_SSID"
#define DEFAULT_WIFI_PASS "YOUR_WIFI_PASSWORD"
#define WIFI_MAXIMUM_RETRY 5

// NVS keys for WiFi credentials
#define NVS_NAMESPACE "wifi_config"
#define NVS_SSID_KEY "ssid"
#define NVS_PASSWORD_KEY "password"

// Bluetooth configuration
#define DEVICE_NAME "Luma_Lantern"
#define MANUFACTURER_DATA_LEN 17
#define SERVICE_UUID 0x00FF
#define CHAR_UUID 0xFF01

// LED Strip configuration
#define LED_STRIP_LED_COUNT 60
#define LED_STRIP_RMT_CHANNEL RMT_CHANNEL_0
#define LED_STRIP_GPIO 18

// Firebase configuration
#define FIREBASE_HOST "your-project.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-secret"

// Event group to signal when we are connected
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

// WiFi credentials storage
static char stored_ssid[33];  // Max 32 chars + null terminator
static char stored_password[65]; // Max 64 chars + null terminator

// Function prototypes
static void wifi_init_sta(void);
static void load_wifi_credentials(void);
static void save_wifi_credentials(const char* ssid, const char* password);
static void update_wifi_credentials(const char* ssid, const char* password);

// Load WiFi credentials from NVS
static void load_wifi_credentials(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    
    if (err == ESP_OK) {
        size_t ssid_len = sizeof(stored_ssid);
        size_t password_len = sizeof(stored_password);
        
        // Try to read stored credentials
        err = nvs_get_str(nvs_handle, NVS_SSID_KEY, stored_ssid, &ssid_len);
        if (err == ESP_OK) {
            err = nvs_get_str(nvs_handle, NVS_PASSWORD_KEY, stored_password, &password_len);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Loaded WiFi credentials from NVS: SSID=%s", stored_ssid);
            } else {
                ESP_LOGW(TAG, "Failed to load password from NVS: %s", esp_err_to_name(err));
                strcpy(stored_ssid, DEFAULT_WIFI_SSID);
                strcpy(stored_password, DEFAULT_WIFI_PASS);
            }
        } else {
            ESP_LOGW(TAG, "Failed to load SSID from NVS: %s", esp_err_to_name(err));
            strcpy(stored_ssid, DEFAULT_WIFI_SSID);
            strcpy(stored_password, DEFAULT_WIFI_PASS);
        }
        nvs_close(nvs_handle);
    } else {
        ESP_LOGW(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        strcpy(stored_ssid, DEFAULT_WIFI_SSID);
        strcpy(stored_password, DEFAULT_WIFI_PASS);
    }
}

// Save WiFi credentials to NVS
static void save_wifi_credentials(const char* ssid, const char* password)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    
    if (err == ESP_OK) {
        // Save SSID
        err = nvs_set_str(nvs_handle, NVS_SSID_KEY, ssid);
        if (err == ESP_OK) {
            // Save password
            err = nvs_set_str(nvs_handle, NVS_PASSWORD_KEY, password);
            if (err == ESP_OK) {
                // Commit changes
                err = nvs_commit(nvs_handle);
                if (err == ESP_OK) {
                    ESP_LOGI(TAG, "WiFi credentials saved to NVS successfully");
                    // Update stored credentials
                    strcpy(stored_ssid, ssid);
                    strcpy(stored_password, password);
                } else {
                    ESP_LOGE(TAG, "Failed to commit NVS: %s", esp_err_to_name(err));
                }
            } else {
                ESP_LOGE(TAG, "Failed to save password to NVS: %s", esp_err_to_name(err));
            }
        } else {
            ESP_LOGE(TAG, "Failed to save SSID to NVS: %s", esp_err_to_name(err));
        }
        nvs_close(nvs_handle);
    } else {
        ESP_LOGE(TAG, "Failed to open NVS for writing: %s", esp_err_to_name(err));
    }
}

// Update WiFi credentials and reconnect
static void update_wifi_credentials(const char* ssid, const char* password)
{
    ESP_LOGI(TAG, "Updating WiFi credentials to: %s", ssid);
    
    // Save new credentials
    save_wifi_credentials(ssid, password);
    
    // Disconnect current WiFi
    esp_wifi_disconnect();
    
    // Wait a moment for disconnect to complete
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // Reconnect with new credentials
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    
    // Copy new credentials
    strcpy((char*)wifi_config.sta.ssid, ssid);
    strcpy((char*)wifi_config.sta.password, password);
    
    // Set new configuration
    esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "WiFi configuration updated, attempting to connect...");
        esp_wifi_connect();
    } else {
        ESP_LOGE(TAG, "Failed to update WiFi configuration: %s", esp_err_to_name(err));
    }
}

// WiFi event handler
static void event_handler(void* arg, esp_event_base_t event_base,
                         int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_wifi_event_group != NULL) {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "WiFi disconnected, attempting to reconnect...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        if (s_wifi_event_group != NULL) {
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        }
    }
}

// WiFi initialization
static void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_got_ip));

    // Use stored credentials or defaults
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    
    // Copy stored credentials
    strcpy((char*)wifi_config.sta.ssid, stored_ssid);
    strcpy((char*)wifi_config.sta.password, stored_password);
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi initialization finished with SSID: %s", stored_ssid);

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to WiFi SSID: %s", stored_ssid);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to WiFi SSID: %s", stored_ssid);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

// Bluetooth GAP callback
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising start failed");
        } else {
            ESP_LOGI(TAG, "Advertising start successfully");
        }
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        if (param->auth_cmpl.success) {
            ESP_LOGI(TAG, "Authentication success, address type = %d", param->auth_cmpl.addr_type);
        } else {
            ESP_LOGE(TAG, "Authentication failed, reason = 0x%x", param->auth_cmpl.fail_reason);
        }
        break;
    default:
        break;
    }
}

// Bluetooth GATTS callback
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GATTS_WRITE_EVT:
        if (param->write.len > 0) {
            ESP_LOGI(TAG, "Received data: %.*s", param->write.len, param->write.value);
            // Handle received data for lantern control
            handle_lantern_control(param->write.value, param->write.len);
        }
        break;
    default:
        break;
    }
}

// LED Strip instance
static led_strip_handle_t led_strip;

// Initialize LED strip
static void led_strip_init(void)
{
    ESP_LOGI(TAG, "Create LED strip object with RMT channel");
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO,
        .max_leds = LED_STRIP_LED_COUNT,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
}

// Set LED color
static void set_led_color(uint32_t color)
{
    for (int i = 0; i < LED_STRIP_LED_COUNT; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, color));
    }
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

// Handle lantern control commands
static void handle_lantern_control(uint8_t *data, size_t len)
{
    cJSON *json = cJSON_Parse((char*)data);
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return;
    }

    // Check for WiFi configuration update
    cJSON *type = cJSON_GetObjectItem(json, "type");
    if (type && cJSON_IsString(type) && strcmp(type->valuestring, "wifi_config") == 0) {
        cJSON *wifi_ssid = cJSON_GetObjectItem(json, "wifi_ssid");
        cJSON *wifi_password = cJSON_GetObjectItem(json, "wifi_password");
        
        if (wifi_ssid && wifi_password && cJSON_IsString(wifi_ssid) && cJSON_IsString(wifi_password)) {
            ESP_LOGI(TAG, "Received WiFi configuration update: SSID=%s", wifi_ssid->valuestring);
            update_wifi_credentials(wifi_ssid->valuestring, wifi_password->valuestring);
        }
    } else {
        // Handle regular lantern control
        cJSON *color = cJSON_GetObjectItem(json, "color");
        cJSON *brightness = cJSON_GetObjectItem(json, "brightness");
        cJSON *mode = cJSON_GetObjectItem(json, "mode");

        if (color && cJSON_IsString(color)) {
            // Parse hex color string
            uint32_t color_value = strtol(color->valuestring + 1, NULL, 16);
            set_led_color(color_value);
            ESP_LOGI(TAG, "Set LED color to 0x%06x", color_value);
        }

        if (brightness && cJSON_IsNumber(brightness)) {
            // Adjust brightness (implement PWM dimming)
            ESP_LOGI(TAG, "Set brightness to %d%%", (int)brightness->valuedouble);
        }

        if (mode && cJSON_IsString(mode)) {
            if (strcmp(mode->valuestring, "rainbow") == 0) {
                // Start rainbow animation
                ESP_LOGI(TAG, "Start rainbow mode");
            } else if (strcmp(mode->valuestring, "breathing") == 0) {
                // Start breathing animation
                ESP_LOGI(TAG, "Start breathing mode");
            }
        }
    }

    cJSON_Delete(json);
}

// Firebase polling task
static void firebase_poll_task(void *pvParameters)
{
    while (1) {
        // Poll Firebase for updates every 5 seconds
        ESP_LOGI(TAG, "Polling Firebase for updates...");
        
        // TODO: Implement Firebase REST API calls to check for updates
        // This would include checking for color changes, friend permissions, etc.
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// Main function
void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Load stored WiFi credentials
    load_wifi_credentials();

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    // Initialize Bluetooth
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "esp_bt_controller_init failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "esp_bt_controller_enable failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "esp_bluedroid_init failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "esp_bluedroid_enable failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "esp_ble_gatts_register_callback failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "esp_ble_gap_register_callback failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_app_register(0);
    if (ret) {
        ESP_LOGE(TAG, "esp_ble_gatts_app_register failed: %s", esp_err_to_name(ret));
        return;
    }

    // Initialize LED strip
    led_strip_init();

    // Create Firebase polling task
    xTaskCreate(firebase_poll_task, "firebase_poll", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Luma Lantern initialized successfully!");
    ESP_LOGI(TAG, "WiFi credentials stored in NVS - will persist across power cycles");
    
    // Set initial LED color (warm white)
    set_led_color(0xFFF5E6);
}