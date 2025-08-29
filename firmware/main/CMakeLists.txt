idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES spi_flash
                       REQUIRES driver
                       REQUIRES led_strip
                       # REQUIRES kodediy__kode_bq25896  # Commented out - not used
                       # REQUIRES ble_conn_mgr           # Commented out - not used
                       # REQUIRES ble_services           # Commented out - not used
                       # REQUIRES nvs_flash              # Commented out - not used
                       INCLUDE_DIRS "")
