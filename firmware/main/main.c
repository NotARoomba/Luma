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
#include "driver/i2c.h"

static const char *TAG = "WS2812B_LED";

// LED strip configuration
#define LED_STRIP_LED_NUMBERS 24
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000) // 10MHz resolution, 1 tick = 0.1us
#define LED_GPIO_PIN          18  // GPIO 18 as shown in pinout diagram

// I2C configuration for BQ25896 PMIC
#define I2C_MASTER_SCL_IO     21  // GPIO 21 (Pin 33) - I2C_SCL
#define I2C_MASTER_SDA_IO     22  // GPIO 22 (Pin 36) - I2C_SDA
#define I2C_MASTER_NUM        I2C_NUM_0
#define I2C_MASTER_FREQ_HZ    400000  // 400kHz I2C frequency
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

// LED strip handle
static led_strip_handle_t led_strip;

// BQ25896 PMIC configuration
#define BQ25896_I2C_ADDR 0x6B  // BQ25896 I2C address
static bool bq25896_initialized = false;

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

// Function to initialize I2C bus for BQ25896 PMIC
static esp_err_t i2c_master_init(void)
{
    ESP_LOGI(TAG, "Initializing I2C master...");
    
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "I2C master initialized successfully");
    return ESP_OK;
}

// Function to read BQ25896 register
static esp_err_t bq25896_read_register(uint8_t reg_addr, uint8_t *data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BQ25896_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BQ25896_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Function to write BQ25896 register
static esp_err_t bq25896_write_register(uint8_t reg_addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BQ25896_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Function to initialize BQ25896 PMIC
static esp_err_t bq25896_pmic_init(void)
{
    ESP_LOGI(TAG, "Starting BQ25896 PMIC initialization");
    
    // Initialize I2C bus first
    esp_err_t ret = i2c_master_init();
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Test communication by reading device information register (REG14)
    uint8_t device_info;
    ret = bq25896_read_register(0x14, &device_info);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to communicate with BQ25896: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Check if device is BQ25896 (PN bits 5-3 should be 000)
    uint8_t pn = (device_info >> 3) & 0x07;
    if (pn != 0) {
        ESP_LOGW(TAG, "Unexpected device PN: %d (expected 0 for BQ25896)", pn);
    }
    
    ESP_LOGI(TAG, "BQ25896 communication established successfully");
    ESP_LOGI(TAG, "Device Info: 0x%02X (PN=%d, TS_PROFILE=%d, DEV_REV=%d)", 
             device_info, pn, (device_info >> 2) & 0x01, device_info & 0x03);
    
    bq25896_initialized = true;
    return ESP_OK;
}

// Function to read and log battery status
static void log_battery_status(void)
{
    if (!bq25896_initialized) {
        ESP_LOGW(TAG, "BQ25896 not initialized, cannot read battery status");
        return;
    }
    
    // Read battery voltage (REG0E)
    uint8_t batv_reg;
    esp_err_t ret = bq25896_read_register(0x0E, &batv_reg);
    if (ret == ESP_OK) {
        // Battery voltage = 2.304V + (BATV * 20mV)
        float battery_voltage = 2.304f + (batv_reg * 0.020f);
        ESP_LOGI(TAG, "Battery Voltage: %.3fV", battery_voltage);
    } else {
        ESP_LOGE(TAG, "Failed to read battery voltage: %s", esp_err_to_name(ret));
    }
    
    // Read system voltage (REG0F)
    uint8_t sysv_reg;
    ret = bq25896_read_register(0x0F, &sysv_reg);
    if (ret == ESP_OK) {
        // System voltage = 2.304V + (SYSV * 20mV)
        float system_voltage = 2.304f + (sysv_reg * 0.020f);
        ESP_LOGI(TAG, "System Voltage: %.3fV", system_voltage);
    } else {
        ESP_LOGE(TAG, "Failed to read system voltage: %s", esp_err_to_name(ret));
    }
    
    // Read VBUS voltage (REG11)
    uint8_t vbus_reg;
    ret = bq25896_read_register(0x11, &vbus_reg);
    if (ret == ESP_OK) {
        // VBUS voltage = 2.6V + (VBUSV * 100mV)
        float vbus_voltage = 2.6f + (vbus_reg * 0.1f);
        ESP_LOGI(TAG, "VBUS Voltage: %.3fV", vbus_voltage);
    } else {
        ESP_LOGE(TAG, "Failed to read VBUS voltage: %s", esp_err_to_name(ret));
    }
    
    // Read charge current (REG12)
    uint8_t ichg_reg;
    ret = bq25896_read_register(0x12, &ichg_reg);
    if (ret == ESP_OK) {
        // Charge current = ICHGR * 50mA
        float charge_current = ichg_reg * 0.050f;
        ESP_LOGI(TAG, "Charge Current: %.3fA", charge_current);
    } else {
        ESP_LOGE(TAG, "Failed to read charge current: %s", esp_err_to_name(ret));
    }
    
    // Read status register (REG0B)
    uint8_t status_reg;
    ret = bq25896_read_register(0x0B, &status_reg);
    if (ret == ESP_OK) {
        uint8_t vbus_stat = (status_reg >> 5) & 0x07;
        uint8_t chrg_stat = (status_reg >> 3) & 0x03;
        uint8_t pg_stat = (status_reg >> 2) & 0x01;
        uint8_t vsys_stat = status_reg & 0x01;
        
        const char* vbus_status[] = {"No Input", "USB Host SDP", "Adapter (3.25A)", "Unknown", "Unknown", "Unknown", "Unknown", "OTG"};
        const char* charge_status[] = {"Not Charging", "Pre-charge", "Fast Charging", "Charge Done"};
        
        ESP_LOGI(TAG, "VBUS Status: %s", vbus_status[vbus_stat]);
        ESP_LOGI(TAG, "Charge Status: %s", charge_status[chrg_stat]);
        ESP_LOGI(TAG, "Power Good: %s", pg_stat ? "Yes" : "No");
        ESP_LOGI(TAG, "VSYS Regulation: %s", vsys_stat ? "Active" : "Inactive");
    } else {
        ESP_LOGE(TAG, "Failed to read status register: %s", esp_err_to_name(ret));
    }
    
    // Read fault register (REG0C)
    uint8_t fault_reg;
    ret = bq25896_read_register(0x0C, &fault_reg);
    if (ret == ESP_OK) {
        if (fault_reg == 0) {
            ESP_LOGI(TAG, "No faults detected");
        } else {
            ESP_LOGW(TAG, "Faults detected: 0x%02X", fault_reg);
            if (fault_reg & 0x80) ESP_LOGW(TAG, "  - Watchdog fault");
            if (fault_reg & 0x40) ESP_LOGW(TAG, "  - Boost fault");
            if (fault_reg & 0x30) ESP_LOGW(TAG, "  - Charge fault");
            if (fault_reg & 0x08) ESP_LOGW(TAG, "  - Battery fault");
            if (fault_reg & 0x07) ESP_LOGW(TAG, "  - NTC fault");
        }
    } else {
        ESP_LOGE(TAG, "Failed to read fault register: %s", esp_err_to_name(ret));
    }
}

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

// Function to create a rainbow effect
static void rainbow_effect(void)
{
    uint32_t colors[][3] = {
        {255, 0, 0},     // Red
        {255, 127, 0},   // Orange
        {255, 255, 0},   // Yellow
        {0, 255, 0},     // Green
        {0, 0, 255},     // Blue
        {75, 0, 130},    // Indigo
        {148, 0, 211}    // Violet
    };
    
    for (int color = 0; color < 7; color++) {
        for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++) {
            ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 
                colors[color][0], colors[color][1], colors[color][2]));
        }
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Function to create a chasing effect
static void chasing_effect(void)
{
    for (int pos = 0; pos < LED_STRIP_LED_NUMBERS; pos++) {
        clear_all_leds();
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, pos, 0, 255, 0)); // Green
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

// Battery monitoring task
static void battery_monitor_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting battery monitoring task");
    
    while (1) {
        // Log battery status every 10 seconds
        log_battery_status();
        ESP_LOGI(TAG, "--- Battery Status Log Complete ---");
        vTaskDelay(pdMS_TO_TICKS(10000)); // 10 second delay
    }
}

// LED control task
static void led_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting LED control task");
    
    while (1) {
        // Turn on all LEDs with white color
        ESP_LOGI(TAG, "Setting all LEDs to white");
        set_all_leds_color(255, 255, 255);
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // Turn on all LEDs with red color
        ESP_LOGI(TAG, "Setting all LEDs to red");
        set_all_leds_color(255, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // Turn on all LEDs with green color
        ESP_LOGI(TAG, "Setting all LEDs to green");
        set_all_leds_color(0, 255, 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // Turn on all LEDs with blue color
        ESP_LOGI(TAG, "Setting all LEDs to blue");
        set_all_leds_color(0, 0, 255);
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // Rainbow effect
        ESP_LOGI(TAG, "Starting rainbow effect");
        rainbow_effect();
        
        // Chasing effect
        ESP_LOGI(TAG, "Starting chasing effect");
        chasing_effect();
        
        // Clear all LEDs
        ESP_LOGI(TAG, "Clearing all LEDs");
        clear_all_leds();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    printf("ESP32 WS2812B LED Controller\n");
    printf("Controlling 5 WS2812B LEDs on GPIO %d\n", LED_GPIO_PIN);

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

    // Initialize LED strip
    led_strip_init();
    
    // Initialize BQ25896 PMIC
    esp_err_t ret = bq25896_pmic_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BQ25896 initialization failed, continuing without battery monitoring");
    } else {
        // Create battery monitoring task
        xTaskCreate(battery_monitor_task, "battery_monitor", 4096, NULL, 3, NULL);
        ESP_LOGI(TAG, "Battery monitoring task started");
    }
    
    // Create LED control task
    xTaskCreate(led_task, "led_task", 4096, NULL, 5, NULL);
    
    printf("LED control task started. LEDs will begin cycling through patterns.\n");
    if (ret == ESP_OK) {
        printf("Battery monitoring active - status will be logged every 10 seconds.\n");
    }
}
