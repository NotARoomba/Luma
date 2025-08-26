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

static const char *TAG = "WS2812B_LED";

// LED strip configuration
#define LED_STRIP_LED_NUMBERS 1
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000) // 10MHz resolution, 1 tick = 0.1us
#define LED_GPIO_PIN          18  // GPIO 18 as shown in pinout diagram

// LED strip handle
static led_strip_handle_t led_strip;

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
    
    // Create LED control task
    xTaskCreate(led_task, "led_task", 4096, NULL, 5, NULL);
    
    printf("LED control task started. LEDs will begin cycling through patterns.\n");
}
