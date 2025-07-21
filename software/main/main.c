#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/rmt.h"
#include "led_strip.h"

#define I2C_MASTER_SCL_IO 33
#define I2C_MASTER_SDA_IO 36
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

#define BQ25896_ADDR 0x6B
#define BQ27220_ADDR 0xAA

#define LED_RMT_CHANNEL RMT_CHANNEL_0
#define LED_GPIO 30
#define NUM_LEDS 8

static const char *TAG = "MAIN";

led_strip_t *strip;

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

void battery_task(void *arg) {
    uint8_t buf[2];
    while (1) {
        if (bq_read(BQ27220_ADDR, 0x04, buf, 2) == ESP_OK) {
            uint16_t voltage = buf[1] << 8 | buf[0];
            ESP_LOGI(TAG, "Voltage: %.3f V", voltage / 1000.0);
        }

        if (bq_read(BQ27220_ADDR, 0x2C, buf, 2) == ESP_OK) {
            uint16_t soc = buf[1] << 8 | buf[0];
            ESP_LOGI(TAG, "State of Charge: %d %%", soc);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void led_task(void *arg) {
    while (1) {
        for (int i = 0; i < NUM_LEDS; i++) {
            strip->set_pixel(strip, i, 0, 150, 0);
        }
        strip->refresh(strip, 100);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    ESP_LOGI(TAG, "Starting app...");

    i2c_master_init();

    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(LED_GPIO, LED_RMT_CHANNEL);
    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
    strip = led_strip_new_rmt_ws2812(&config);
    strip->clear(strip, 100);

    xTaskCreate(battery_task, "battery_task", 2048, NULL, 5, NULL);
    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
}