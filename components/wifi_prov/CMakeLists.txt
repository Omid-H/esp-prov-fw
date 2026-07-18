idf_component_register(
    SRCS "wifi_prov.c"
    INCLUDE_DIRS "include"
    REQUIRES nvs_flash esp_wifi esp_event network_provisioning qrcode
)
