idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES 
        driver 
        esp_wifi 
        esp_event 
        esp_netif 
        nvs_flash 
        esp_bt 
        esp_gap_ble 
        esp_gatts 
        led_strip 
        esp_http_client 
        json 
        freertos
)