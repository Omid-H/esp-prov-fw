idf_component_register(
    SRCS "status_led.cpp"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES wifi_prov esp_driver_gpio
)
