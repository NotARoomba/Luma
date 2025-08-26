idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES spi_flash
                       REQUIRES driver
                       REQUIRES led_strip
                       REQUIRES kodediy__kode_bq25896
                       INCLUDE_DIRS "")
