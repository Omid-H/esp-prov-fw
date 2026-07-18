idf_component_register(
    SRCS "factory_reset.cpp"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES nvs_flash esp_driver_gpio
)
