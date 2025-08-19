#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "led_strip.h"

#define I2C_MASTER_SCL_IO 21
#define I2C_MASTER_SDA_IO 22
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

#define BQ25896_ADDR 0x6B
#define BQ27220_ADDR 0xAA

#define LED_GPIO 18
#define LED_COUNT 16  // Configurable LED count
#define LED_RMT_CHANNEL RMT_CHANNEL_0

static const char *TAG = "MAIN";

led_strip_handle_t strip;

// LED color patterns
typedef struct {
    uint8_t r, g, b;
} rgb_color_t;

// Predefined color patterns
rgb_color_t colors[] = {
    {255, 0, 0},     // Red
    {0, 255, 0},     // Green
    {0, 0, 255},     // Blue
    {255, 255, 0},   // Yellow
    {255, 0, 255},   // Magenta
    {0, 255, 255},   // Cyan
    {255, 255, 255}, // White
    {128, 0, 128},   // Purple
    {255, 165, 0},   // Orange
    {128, 128, 128}  // Gray
};

#define COLOR_COUNT (sizeof(colors) / sizeof(colors[0]))

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

esp_err_t bq_read(uint8_t dev_addr, uint8_t reg, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void led_strip_init() {
    ESP_LOGI(TAG, "Initializing LED strip with %d LEDs on GPIO %d", LED_COUNT, LED_GPIO);
    
    // LED strip configuration
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = LED_COUNT,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    // RMT specific configuration
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &strip));
    ESP_LOGI(TAG, "LED strip initialized successfully");
}

void set_led_pattern(uint8_t pattern_type) {
    switch (pattern_type) {
        case 0: // Solid color
            for (int i = 0; i < LED_COUNT; i++) {
                led_strip_set_pixel(strip, i, 255, 0, 0); // Red
            }
            break;
            
        case 1: // Rainbow pattern
            for (int i = 0; i < LED_COUNT; i++) {
                uint8_t hue = (i * 255) / LED_COUNT;
                // Simple HSV to RGB conversion
                uint8_t r, g, b;
                if (hue < 85) {
                    r = 255 - hue * 3;
                    g = hue * 3;
                    b = 0;
                } else if (hue < 170) {
                    hue -= 85;
                    r = 0;
                    g = 255 - hue * 3;
                    b = hue * 3;
                } else {
                    hue -= 170;
                    r = hue * 3;
                    g = 0;
                    b = 255 - hue * 3;
                }
                led_strip_set_pixel(strip, i, r, g, b);
            }
            break;
            
        case 2: // Alternating colors
            for (int i = 0; i < LED_COUNT; i++) {
                if (i % 2 == 0) {
                    led_strip_set_pixel(strip, i, 255, 0, 0); // Red
                } else {
                    led_strip_set_pixel(strip, i, 0, 0, 255); // Blue
                }
            }
            break;
            
        case 3: // Color wave
            for (int i = 0; i < LED_COUNT; i++) {
                uint8_t color_index = (i + (esp_timer_get_time() / 100000)) % COLOR_COUNT;
                led_strip_set_pixel(strip, i, 
                    colors[color_index].r,
                    colors[color_index].g,
                    colors[color_index].b);
            }
            break;
            
        default:
            // Turn off all LEDs
            for (int i = 0; i < LED_COUNT; i++) {
                led_strip_set_pixel(strip, i, 0, 0, 0);
            }
            break;
    }
    
    led_strip_refresh(strip);
}

void app_main() {
    ESP_LOGI(TAG, "Starting app...");

    i2c_master_init();
    led_strip_init();
    
    ESP_LOGI(TAG, "LED strip with %d LEDs configured on GPIO %d", LED_COUNT, LED_GPIO);

    // Main loop
    uint8_t buf[2];
    uint8_t current_pattern = 0;
    uint8_t pattern_count = 4; // Number of available patterns
    
    while (1) {
        // Battery monitoring
        if (bq_read(BQ27220_ADDR, 0x04, buf, 2) == ESP_OK) {
            uint16_t voltage = buf[1] << 8 | buf[0];
            ESP_LOGI(TAG, "Voltage: %.3f V", voltage / 1000.0);
        }

        if (bq_read(BQ27220_ADDR, 0x2C, buf, 2) == ESP_OK) {
            uint16_t soc = buf[1] << 8 | buf[0];
            ESP_LOGI(TAG, "State of Charge: %d %%", soc);
        }

        // LED pattern cycling
        ESP_LOGI(TAG, "Setting LED pattern %d", current_pattern);
        set_led_pattern(current_pattern);
        
        // Cycle through patterns
        current_pattern = (current_pattern + 1) % pattern_count;
        
        vTaskDelay(pdMS_TO_TICKS(3000)); // 3 second delay between patterns
    }
}