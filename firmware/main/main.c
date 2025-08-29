/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "led_strip.h"
#include "driver/gpio.h"
  #include "soc/soc.h"
    #include "soc/rtc_cntl_reg.h"
// #include "driver/i2c.h"
// #include "kode_bq25896.h"
// #include "esp_ble_conn_mgr.h"
// #include "nvs_flash.h"

static const char *TAG = "WS2812B_LED";

// LED strip configuration
#define LED_STRIP_LED_NUMBERS 20
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000) // 10MHz resolution, 1 tick = 0.1us
#define LED_GPIO_PIN          18  // GPIO 18 as shown in pinout diagram

// I2C configuration for BQ25896 PMIC (commented out)
// #define I2C_MASTER_SCL_IO     21  // GPIO 21 (Pin 33) - I2C_SCL
// #define I2C_MASTER_SDA_IO     22  // GPIO 22 (Pin 36) - I2C_SDA
// #define I2C_MASTER_NUM        I2C_NUM_0

// BLE configuration (commented out)
// #define BLE_DEVICE_NAME       "Luma"
// #define BLE_BROADCAST_DATA    "LED Monitor"

// LED color configuration - toggle between warm orange and minecraft blue
#define USE_WARM_ORANGE       1  // Set to 1 for warm orange, 0 for minecraft blue

#if USE_WARM_ORANGE
    #define LED_RED           255
    #define LED_GREEN         100
    #define LED_BLUE          0
    #define COLOR_NAME        "Warm Orange"
#else
    #define LED_RED           5
    #define LED_GREEN         0
    #define LED_BLUE          159
    #define COLOR_NAME        "Minecraft Blue"
#endif

// LED strip handle
static led_strip_handle_t led_strip;

// BQ25896 PMIC handles (commented out)
// static i2c_master_bus_handle_t i2c_bus = NULL;
// static bq25896_handle_t bq_handle = NULL;
// static bool bq25896_initialized = false;

// Function to initialize LED strip
static void led_strip_init(void)
{
    ESP_LOGI(TAG, "Initializing LED strip...");
    
    // LED strip general configuration
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO_PIN,   // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_NUMBERS, // The number of LEDs in the strip
        .led_model = LED_MODEL_WS2812,    // LED strip model, it determines the bit timing
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color component format is G-R-B
        .flags = {
            .invert_out = false, // don't invert the output signal
        }
    };

    // LED strip RMT configuration
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency: 10MHz
        .mem_block_symbols = 64,               // the memory size of each RMT channel, in words (4 bytes)
        .flags = {
            .with_dma = false, // DMA feature is available on chips like ESP32-S3/P4
        }
    };

    // LED Strip object handle
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
}

// Function to initialize BQ25896 PMIC
// static esp_err_t bq25896_pmic_init(void)
// {
//     ESP_LOGI(TAG, "Starting BQ25896 PMIC initialization");
    
//     // Initialize I2C bus
//     i2c_master_bus_config_t i2c_bus_config = {
//         .clk_source = I2C_CLK_SRC_DEFAULT,
//         .i2c_port = I2C_MASTER_NUM,
//         .scl_io_num = I2C_MASTER_SCL_IO,
//         .sda_io_num = I2C_MASTER_SDA_IO,
//         .glitch_ignore_cnt = 7,
//         .flags.enable_internal_pullup = true,
//     };

//     esp_err_t ret = i2c_new_master_bus(&i2c_bus_config, &i2c_bus);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(ret));
//         return ret;
//     }
//     ESP_LOGI(TAG, "I2C bus initialized successfully");
    
//     // Initialize BQ25896 PMIC with default configuration
//     ret = bq25896_init(i2c_bus, &bq_handle);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to initialize BQ25896: %s", esp_err_to_name(ret));
//         i2c_del_master_bus(i2c_bus);
//         return ret;
//     }
//     ESP_LOGI(TAG, "BQ25896 initialized successfully with default configuration");
    
//     bq25896_initialized = true;
//     return ESP_OK;
// }

// // Function to initialize BLE connection manager
// static esp_err_t ble_conn_mgr_init(void)
// {
//     ESP_LOGI(TAG, "Initializing BLE connection manager...");
    
//     // Add delay to allow power to stabilize after brownout
//     vTaskDelay(pdMS_TO_TICKS(1000));
//     ESP_LOGI(TAG, "Power stabilization delay completed");
    
//     // Configure BLE connection manager
//     esp_ble_conn_config_t config = {
//         .device_name = BLE_DEVICE_NAME,
//         .broadcast_data = BLE_BROADCAST_DATA
//     };
    
//     // Initialize BLE connection manager
//     esp_err_t ret = esp_ble_conn_init(&config);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to initialize BLE connection manager: %s", esp_err_to_name(ret));
//         ESP_LOGE(TAG, "This may be due to power supply issues or RF calibration problems");
//         return ret;
//     }
    
//     // Add delay before starting BLE
//     vTaskDelay(pdMS_TO_TICKS(500));
    
//     // Start BLE connection manager
//     ret = esp_ble_conn_start();
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to start BLE connection manager: %s", esp_err_to_name(ret));
//         return ret;
//     }
    
//     ESP_LOGI(TAG, "BLE connection manager initialized successfully");
//     ESP_LOGI(TAG, "Device name: %s", BLE_DEVICE_NAME);
//     ESP_LOGI(TAG, "Broadcast data: %s", BLE_BROADCAST_DATA);
    
//     return ESP_OK;
// }

// Function to log battery status
// static void log_battery_status(void)
// {
//     if (!bq25896_initialized) {
//         ESP_LOGW(TAG, "BQ25896 not initialized, cannot read battery status");
//         return;
//     }
    
//     // Read battery voltage
//     uint16_t battery_voltage_mv;
//     esp_err_t ret = bq25896_get_battery_voltage(bq_handle, &battery_voltage_mv);
//     if (ret == ESP_OK) {
//         ESP_LOGI(TAG, "Battery Voltage: %.3fV", battery_voltage_mv / 1000.0f);
//     } else {
//         ESP_LOGE(TAG, "Failed to read battery voltage: %s", esp_err_to_name(ret));
//     }
    
//     // Read system voltage
//     uint16_t system_voltage_mv;
//     ret = bq25896_get_system_voltage(bq_handle, &system_voltage_mv);
//     if (ret == ESP_OK) {
//         ESP_LOGI(TAG, "System Voltage: %.3fV", system_voltage_mv / 1000.0f);
//     } else {
//         ESP_LOGE(TAG, "Failed to read system voltage: %s", esp_err_to_name(ret));
//     }
    
//     // Read VBUS voltage
//     uint16_t vbus_voltage_mv;
//     ret = bq25896_get_vbus_voltage(bq_handle, &vbus_voltage_mv);
//     if (ret == ESP_OK) {
//         ESP_LOGI(TAG, "VBUS Voltage: %.3fV", vbus_voltage_mv / 1000.0f);
//     } else {
//         ESP_LOGE(TAG, "Failed to read VBUS voltage: %s", esp_err_to_name(ret));
//     }
    
//     // Read charge current
//     uint16_t charge_current_ma;
//     ret = bq25896_get_charge_current(bq_handle, &charge_current_ma);
//     if (ret == ESP_OK) {
//         ESP_LOGI(TAG, "Charge Current: %.3fA", charge_current_ma / 1000.0f);
//     } else {
//         ESP_LOGE(TAG, "Failed to read charge current: %s", esp_err_to_name(ret));
//     }
    
//     // Read charge status
//     bq25896_chrg_stat_t charge_status;
//     ret = bq25896_get_charging_status(bq_handle, &charge_status);
//     if (ret == ESP_OK) {
//         const char* status_strings[] = {
//             "Not Charging", "Pre-charge", "Fast Charging", "Charge Done"
//         };
//         ESP_LOGI(TAG, "Charge Status: %s", status_strings[charge_status]);
//     } else {
//         ESP_LOGE(TAG, "Failed to read charge status: %s", esp_err_to_name(ret));
//     }
    
//     // Read VBUS status
//     bq25896_vbus_stat_t vbus_status;
//     ret = bq25896_get_vbus_status(bq_handle, &vbus_status);
//     if (ret == ESP_OK) {
//         const char* vbus_strings[] = {
//             "No Input", "USB Host SDP", "Adapter (3.25A)", "Unknown", 
//             "Unknown", "Unknown", "Unknown", "OTG"
//         };
//         ESP_LOGI(TAG, "VBUS Status: %s", vbus_strings[vbus_status]);
//     } else {
//         ESP_LOGE(TAG, "Failed to read VBUS status: %s", esp_err_to_name(ret));
//     }
    
//     // Read power good status
//     bq25896_pg_stat_t power_good;
//     ret = bq25896_get_pg_status(bq_handle, &power_good);
//     if (ret == ESP_OK) {
//         ESP_LOGI(TAG, "Power Good: %s", power_good ? "Yes" : "No");
//     } else {
//         ESP_LOGE(TAG, "Failed to read power good status: %s", esp_err_to_name(ret));
//     }
    
//     // Read VSYS regulation status
//     bq25896_vsys_stat_t vsys_status;
//     ret = bq25896_get_vsys_status(bq_handle, &vsys_status);
//     if (ret == ESP_OK) {
//         ESP_LOGI(TAG, "VSYS Regulation: %s", vsys_status ? "Active" : "Inactive");
//     } else {
//         ESP_LOGE(TAG, "Failed to read VSYS status: %s", esp_err_to_name(ret));
//     }
    
//     // Read fault status
//     bq25896_watchdog_fault_t wd_fault;
//     bq25896_boost_fault_t boost_fault;
//     bq25896_chrg_fault_t chrg_fault;
//     bq25896_bat_fault_t bat_fault;
//     bq25896_ntc_fault_t ntc_fault;
    
//     ret = bq25896_get_watchdog_fault(bq_handle, &wd_fault);
//     if (ret == ESP_OK && wd_fault) {
//         ESP_LOGW(TAG, "Watchdog fault detected");
//     }
    
//     ret = bq25896_get_boost_fault(bq_handle, &boost_fault);
//     if (ret == ESP_OK && boost_fault) {
//         ESP_LOGW(TAG, "Boost fault detected");
//     }
    
//     ret = bq25896_get_charge_fault(bq_handle, &chrg_fault);
//     if (ret == ESP_OK && chrg_fault != BQ25896_CHRG_FAULT_NORMAL) {
//         const char* fault_strings[] = {
//             "Normal", "Input Fault", "Thermal Shutdown", "Timer Expired"
//         };
//         ESP_LOGW(TAG, "Charge fault: %s", fault_strings[chrg_fault]);
//     }
    
//     ret = bq25896_get_battery_fault(bq_handle, &bat_fault);
//     if (ret == ESP_OK && bat_fault) {
//         ESP_LOGW(TAG, "Battery overvoltage fault detected");
//     }
    
//     ret = bq25896_get_ntc_fault(bq_handle, &ntc_fault);
//     if (ret == ESP_OK && ntc_fault != BQ25896_NTC_FAULT_NORMAL) {
//         const char* ntc_strings[] = {
//             "Normal", "Unknown", "TS Warm", "TS Cool", "Unknown", "TS Cold", "TS Hot"
//         };
//         ESP_LOGW(TAG, "NTC fault: %s", ntc_strings[ntc_fault]);
//     }
// }

// Function to set all LEDs to a specific color
static void set_all_leds_color(uint32_t red, uint32_t green, uint32_t blue)
{
    for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, red, green, blue));
    }
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

// Function to clear all LEDs
static void clear_all_leds(void)
{
    set_all_leds_color(0, 0, 0);
}

// Rainbow and chasing effects removed - using single color mode only

// Battery monitoring task
// static void battery_monitor_task(void *pvParameters)
// {
//     ESP_LOGI(TAG, "Starting battery monitoring task");
    
//     while (1) {
//         // Log battery status every 10 seconds
//         log_battery_status();
//         ESP_LOGI(TAG, "--- Battery Status Log Complete ---");
//         vTaskDelay(pdMS_TO_TICKS(10000)); // 10 second delay
//     }
// }

// LED control task
static void led_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting LED control task");
    ESP_LOGI(TAG, "Using color: %s (R:%d, G:%d, B:%d)", COLOR_NAME, LED_RED, LED_GREEN, LED_BLUE);
    
    // Clear all LEDs first to remove any residual state
    ESP_LOGI(TAG, "Clearing all LEDs before starting");
    clear_all_leds();
    vTaskDelay(pdMS_TO_TICKS(500)); // Brief delay to ensure clear is visible
    
    // Set LEDs once to the selected color (reduced brightness)
    ESP_LOGI(TAG, "Setting all LEDs to %s", COLOR_NAME);
    // Reduce brightness to lower power consumption and get proper orange
    set_all_leds_color(LED_RED/2, LED_GREEN/2, LED_BLUE/2);
    ESP_LOGI(TAG, "LEDs set to constant color, entering idle state");
    
    while (1) {
        // Just keep the task alive - LEDs remain on
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 second idle delay
    }
}

void app_main(void)
{
    printf("ESP32 WS2812B LED Controller - Single Color Mode\n");
    printf("Controlling %d WS2812B LEDs on GPIO %d\n", LED_STRIP_LED_NUMBERS, LED_GPIO_PIN);
    printf("LED Color: %s (R:%d, G:%d, B:%d)\n", COLOR_NAME, LED_RED, LED_GREEN, LED_BLUE);
    
    // Disable brownout detector to prevent resets (TEMPORARY FIX)
    // WARNING: This masks power supply issues - fix hardware instead!
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
        ESP_LOGW(TAG, "Brownout detector disabled - fix power supply issues!");

    // NVS initialization (commented out - not needed for LED only)
    // esp_err_t ret = nvs_flash_init();
    // ... NVS code commented out ...

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    // Add delay to allow power to stabilize
    ESP_LOGI(TAG, "Allowing power to stabilize...");
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // Initialize LED strip
    led_strip_init();
    
    // BLE and BQ25896 initialization (commented out)
    // ret = ble_conn_mgr_init();
    // ret = bq25896_pmic_init();
    // ... BLE and battery monitoring code commented out ...
    
    // Create LED control task with lower priority to reduce power spikes
    xTaskCreate(led_task, "led_task", 4096, NULL, 3, NULL);
    
    printf("LED control task started. LEDs will begin cycling through patterns.\n");
}
