/* Wi-Fi Provisioning Manager Example

    SPDX-FileCopyrightText: 2026 Nariman Ziaie
    SPDX-License-Identifier: Apache-2.0

    Based on Espressif Systems network provisioning examples.

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include <network_provisioning/manager.h>

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
#include <network_provisioning/scheme_ble.h>
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
#include <network_provisioning/scheme_softap.h>
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
#include "qrcode.h"
#include "wifi_prov.h"

static const char *TAG = "app";

#if CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE


/* This salt,verifier has been generated for username = "smartlamp" and password = "Lamp3232"
 * IMPORTANT NOTE: For production cases, this must be unique to every device
 * and should come from device manufacturing partition.*/
static const char sec2_salt[] = {
    0x50, 0x13, 0xb2, 0x4f, 0x64, 0x0e, 0x4c, 0xd9, 0x01, 0x72, 0xc4, 0x75, 0x9f, 0xf8, 0x19, 0x2b
};

static const char sec2_verifier[] = {
    0x7d, 0xf9, 0xeb, 0xb1, 0xcb, 0x40, 0xea, 0x19, 0x6d, 0x27, 0xcc, 0x04, 0x4f, 0x68, 0x28, 0x68, 
    0x07, 0xa9, 0xf0, 0x9f, 0x81, 0x68, 0xef, 0xe6, 0x06, 0x56, 0x33, 0xe1, 0xdf, 0xad, 0xe7, 0x4a, 
    0xb9, 0x7d, 0x47, 0xac, 0x0e, 0xf9, 0x1c, 0x65, 0x2c, 0xb1, 0xc2, 0xa4, 0x9e, 0x3c, 0xb2, 0x8d, 
    0x62, 0x44, 0xce, 0x39, 0xac, 0x0a, 0x9b, 0xe1, 0x55, 0x06, 0x5e, 0x5d, 0x35, 0x13, 0x65, 0x3b, 
    0x08, 0xeb, 0x81, 0xc1, 0xdd, 0xb8, 0xcd, 0xfc, 0xb1, 0x5f, 0x5e, 0x32, 0x1d, 0x94, 0xa0, 0xa7, 
    0x4b, 0xa8, 0xbc, 0xc9, 0x81, 0xef, 0xb5, 0x22, 0xc0, 0x14, 0x23, 0xfe, 0x95, 0x30, 0xbf, 0xa9, 
    0x61, 0x23, 0x57, 0x2d, 0x40, 0x17, 0x14, 0xc3, 0x61, 0xb3, 0x1a, 0xa7, 0x9b, 0x8d, 0x0f, 0xf4, 
    0x9f, 0xad, 0x4f, 0xd1, 0x4b, 0xd5, 0xfd, 0xd0, 0x0a, 0xbb, 0x2b, 0xbd, 0xe7, 0x56, 0x7e, 0x24, 
    0x97, 0xdb, 0x2a, 0x46, 0x90, 0xdb, 0xe3, 0xba, 0x6e, 0x72, 0x89, 0x85, 0xc1, 0x74, 0x6a, 0xef, 
    0xb7, 0xab, 0xec, 0xa0, 0x2e, 0x46, 0x04, 0xd9, 0x29, 0x3a, 0x1d, 0x0b, 0xfb, 0xd4, 0x9d, 0x90, 
    0xa7, 0xd4, 0xcd, 0x45, 0x06, 0xeb, 0x1a, 0x2d, 0x98, 0x2c, 0x3d, 0x98, 0x1d, 0x49, 0xc1, 0xd7, 
    0x35, 0xbf, 0xa8, 0x56, 0x64, 0x7f, 0x28, 0x19, 0x7a, 0xc7, 0x1a, 0xdb, 0x6c, 0xd4, 0xb1, 0xf4, 
    0x7e, 0x23, 0x20, 0xa3, 0x0f, 0x2e, 0x55, 0xcb, 0x37, 0x51, 0xc1, 0xd8, 0xb4, 0x4e, 0xc3, 0x3d, 
    0x3b, 0x6c, 0xfc, 0xe6, 0x4f, 0xe1, 0x8e, 0x6f, 0xd0, 0xb1, 0x55, 0x3b, 0xea, 0x6a, 0xdb, 0x40, 
    0xa0, 0xca, 0x6e, 0x02, 0x76, 0x48, 0x67, 0x35, 0x4c, 0x7f, 0x82, 0x1a, 0xef, 0xab, 0xce, 0x02, 
    0xeb, 0xab, 0xe8, 0x86, 0x27, 0xcd, 0x2f, 0xb4, 0x03, 0x08, 0x3b, 0x6b, 0x4f, 0x06, 0x73, 0xa2, 
    0x3f, 0x70, 0x5f, 0xbd, 0x08, 0x31, 0x1e, 0x6b, 0x45, 0xb1, 0x50, 0xc8, 0xee, 0x99, 0xb7, 0xb1, 
    0x88, 0xea, 0x15, 0x6d, 0xbb, 0xb3, 0x21, 0x85, 0x52, 0x0a, 0x6a, 0x25, 0xfc, 0x11, 0x54, 0x77, 
    0xf5, 0x4f, 0xc5, 0x5b, 0x5c, 0x99, 0x43, 0xd4, 0xc4, 0x73, 0x13, 0x1b, 0xad, 0x7c, 0x24, 0x94, 
    0x15, 0x61, 0x8d, 0xac, 0x90, 0x2b, 0xa8, 0x87, 0x74, 0x35, 0xfe, 0xe1, 0xa5, 0x31, 0x35, 0x54, 
    0x30, 0x4b, 0x68, 0x13, 0xfb, 0x94, 0x4a, 0x66, 0x02, 0xc3, 0x64, 0xc6, 0x36, 0xb5, 0x5a, 0x94, 
    0xa3, 0xbf, 0x73, 0x03, 0x01, 0x38, 0x5a, 0x53, 0x16, 0x7a, 0xdd, 0x25, 0xc7, 0xae, 0x2a, 0x86, 
    0x6c, 0xdc, 0xb0, 0x28, 0x4e, 0xb1, 0xa4, 0x74, 0x71, 0x9c, 0xf4, 0x83, 0xa0, 0xe9, 0x6d, 0x42, 
    0x18, 0x52, 0x30, 0x83, 0xb5, 0x6e, 0x3c, 0x8f, 0x6d, 0xbe, 0x98, 0xe2, 0xe4, 0x12, 0x5d, 0x61
};
#endif

static esp_err_t example_get_sec2_salt(const char **salt, uint16_t *salt_len)
{
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded salt");
    *salt = sec2_salt;
    *salt_len = sizeof(sec2_salt);
    return ESP_OK;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}

static esp_err_t example_get_sec2_verifier(const char **verifier, uint16_t *verifier_len)
{
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded verifier");
    *verifier = sec2_verifier;
    *verifier_len = sizeof(sec2_verifier);
    return ESP_OK;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
    /* This code needs to be updated with appropriate implementation to provide verifier */
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}
#endif

/* Signal Wi-Fi events on this event-group */
const int WIFI_CONNECTED_EVENT = BIT0;
static EventGroupHandle_t wifi_event_group;

#define PROV_QR_VERSION "v1"
#define PROV_TRANSPORT_SOFTAP "softap"
#define PROV_TRANSPORT_BLE "ble"
#define QRCODE_BASE_URL "https://espressif.github.io/esp-jumpstart/qrcode.html"

/* Event handler for catching system events */
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == NETWORK_PROV_EVENT)
    {
        switch (event_id)
        {
        case NETWORK_PROV_START:
            ESP_LOGI(TAG, "Provisioning started");
            set_led_status(LED_STATE_PROVISIONING);
            break;
        case NETWORK_PROV_WIFI_CRED_RECV:
        {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
            ESP_LOGI(TAG, "Received Wi-Fi credentials"
                          "\n\tSSID     : %s\n\tPassword : %s",
                     (const char *)wifi_sta_cfg->ssid,
                     (const char *)wifi_sta_cfg->password);
            set_led_status(LED_STATE_CONNECTING);
            break;
        }
        case NETWORK_PROV_WIFI_CRED_FAIL:
        {
            network_prov_wifi_sta_fail_reason_t *reason = (network_prov_wifi_sta_fail_reason_t *)event_data;
            ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
                          "\n\tPlease reset to factory and retry provisioning",
                     (*reason == NETWORK_PROV_WIFI_STA_AUTH_ERROR) ? "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
            set_led_status(LED_STATE_ERROR);
#ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
            /* Reset the state machine on provisioning failure.
             * This is enabled by the CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE configuration.
             * It allows the provisioning manager to retry the provisioning process
             * based on the number of attempts specified in wifi_conn_attempts. After attempting
             * the maximum number of retries, the provisioning manager will reset the state machine
             * and the provisioning process will be terminated.
             */
            network_prov_mgr_reset_wifi_sm_state_on_failure();
#endif
            break;
        }
        case NETWORK_PROV_WIFI_CRED_SUCCESS:
            ESP_LOGI(TAG, "Provisioning successful");
            break;
        case NETWORK_PROV_END:
            /* De-initialize manager once provisioning is finished */
            esp_err_t err = network_prov_mgr_deinit();
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to de-initialize provisioning manager: %s", esp_err_to_name(err));
            }
            break;
        default:
            break;
        }
    }
    else if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
            esp_wifi_connect();
            break;
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "SoftAP transport: Connected!");
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "SoftAP transport: Disconnected!");
            break;
#endif
        default:
            break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        set_led_status(LED_STATE_CONNECTED);
        /* Signal main application to continue execution */
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_EVENT);
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
    }
    else if (event_base == PROTOCOMM_TRANSPORT_BLE_EVENT)
    {
        switch (event_id)
        {
        case PROTOCOMM_TRANSPORT_BLE_CONNECTED:
            ESP_LOGI(TAG, "BLE transport: Connected!");
            break;
        case PROTOCOMM_TRANSPORT_BLE_DISCONNECTED:
            ESP_LOGI(TAG, "BLE transport: Disconnected!");
            break;
        default:
            break;
        }
#endif
    }
    else if (event_base == PROTOCOMM_SECURITY_SESSION_EVENT)
    {
        switch (event_id)
        {
        case PROTOCOMM_SECURITY_SESSION_SETUP_OK:
            ESP_LOGI(TAG, "Secured session established!");
            break;
        case PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS:
            ESP_LOGE(TAG, "Received invalid security parameters for establishing secure session!");
            break;
        case PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH:
            ESP_LOGE(TAG, "Received incorrect username and/or PoP for establishing secure session!");
            break;
        default:
            break;
        }
    }
}

static void wifi_init_sta(void)
{
    /* Start Wi-Fi in station mode */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    const char *ssid_prefix = "PROV_";
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X",
             ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}

/* Handler for the optional provisioning endpoint registered by the application.
 * The data format can be chosen by applications. Here, we are using plain ascii text.
 * Applications can choose to use other formats like protobuf, JSON, XML, etc.
 * Note that memory for the response buffer must be allocated using heap as this buffer
 * gets freed by the protocomm layer once it has been sent by the transport layer.
 */
esp_err_t custom_prov_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                   uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    if (inbuf)
    {
        ESP_LOGI(TAG, "Received data: %.*s", inlen, (char *)inbuf);
    }
    char response[] = "SUCCESS";
    *outbuf = (uint8_t *)strdup(response);
    if (*outbuf == NULL)
    {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    *outlen = strlen(response) + 1; /* +1 for NULL terminating byte */

    return ESP_OK;
}

static void wifi_prov_print_qr(const char *name, const char *username, const char *pop, const char *transport)
{
    if (!name || !transport)
    {
        ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
        return;
    }
    char payload[150] = {0};
    if (pop)
    {
#if CONFIG_EXAMPLE_PROV_SECURITY_VERSION_1
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\""
                                           ",\"pop\":\"%s\",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, pop, transport);
#elif CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\""
                                           ",\"username\":\"%s\",\"pop\":\"%s\",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, username, pop, transport);
#endif
    }
    else
    {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\""
                                           ",\"transport\":\"%s\",\"network\":\"wifi\"}",
                 PROV_QR_VERSION, name, transport);
    }
    // TODO: Add the network protocol type to the QR code payload
#ifdef CONFIG_EXAMPLE_PROV_SHOW_QR
    ESP_LOGI(TAG, "Scan this QR code from the provisioning application for Provisioning.");
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    esp_qrcode_generate(&cfg, payload);
#endif /* CONFIG_EXAMPLE_PROV_SHOW_QR */
    ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL, payload);
}

#ifdef CONFIG_EXAMPLE_PROV_ENABLE_APP_CALLBACK
void wifi_prov_app_callback(void *user_data, wifi_prov_cb_event_t event, void *event_data)
{
    /**
     * This is blocking callback, any configurations that needs to be set when a particular
     * provisioning event is triggered can be set here.
     */
    switch (event)
    {
    case WIFI_PROV_SET_STA_CONFIG:
    {
        /**
         * Wi-Fi configurations can be set here before the Wi-Fi is enabled in
         * STA mode.
         */
        wifi_config_t *wifi_config = (wifi_config_t *)event_data;
        (void)wifi_config;
        break;
    }
    default:
        break;
    }
}

const wifi_prov_event_handler_t wifi_prov_event_handler = {
    .event_cb = wifi_prov_app_callback,
    .user_data = NULL,
};
#endif /* EXAMPLE_PROV_ENABLE_APP_CALLBACK */

void init_wifi_provisioning(void)
{
    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* NVS partition was truncated and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    /* Initialize TCP/IP */
    ESP_ERROR_CHECK(esp_netif_init());

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_event_group = xEventGroupCreate();

    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_register(NETWORK_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_TRANSPORT_BLE_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_SECURITY_SESSION_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    /* Initialize Wi-Fi including netif with default config */
    esp_netif_create_default_wifi_sta();
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
    esp_netif_create_default_wifi_ap();
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Configuration for the provisioning manager */
    network_prov_mgr_config_t config = {
#ifdef CONFIG_EXAMPLE_RESET_PROV_MGR_ON_FAILURE
        .network_prov_wifi_conn_cfg = {
            .wifi_conn_attempts = CONFIG_EXAMPLE_PROV_MGR_CONNECTION_CNT,
        },
#endif
    /* What is the Provisioning Scheme that we want ?
     * network_prov_scheme_softap or network_prov_scheme_ble */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        .scheme = network_prov_scheme_ble,
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
        .scheme = network_prov_scheme_softap,
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
#ifdef CONFIG_EXAMPLE_PROV_ENABLE_APP_CALLBACK
        .app_event_handler = wifi_prov_event_handler,
#endif /* EXAMPLE_PROV_ENABLE_APP_CALLBACK */

    /* Any default scheme specific event handler that you would
     * like to choose. Since our example application requires
     * neither BT nor BLE, we can choose to release the associated
     * memory once provisioning is complete, or not needed
     * (in case when device is already provisioned). Choosing
     * appropriate scheme specific event handler allows the manager
     * to take care of this automatically. This can be set to
     * NETWORK_PROV_EVENT_HANDLER_NONE when using network_prov_scheme_softap*/
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        .scheme_event_handler = NETWORK_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
                                    .scheme_event_handler = NETWORK_PROV_EVENT_HANDLER_NONE
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
    };

    /* Initialize provisioning manager with the
     * configuration parameters set above */
    ESP_ERROR_CHECK(network_prov_mgr_init(config));

    bool provisioned = false;
#ifdef CONFIG_EXAMPLE_RESET_PROVISIONED
    network_prov_mgr_reset_wifi_provisioning();
#else
    /* Let's find out if the device is provisioned */
    ESP_ERROR_CHECK(network_prov_mgr_is_wifi_provisioned(&provisioned));

#endif
    /* If device is not yet provisioned start provisioning service */
    if (!provisioned)
    {
        ESP_LOGI(TAG, "Starting provisioning");

        /* What is the Device Service Name that we want
         * This translates to :
         *     - Wi-Fi SSID when scheme is network_prov_scheme_softap
         *     - device name when scheme is network_prov_scheme_ble
         */
        char service_name[12];
        get_device_service_name(service_name, sizeof(service_name));

#ifdef CONFIG_EXAMPLE_PROV_SECURITY_VERSION_1
        /* What is the security level that we want (0, 1, 2):
         *      - NETWORK_PROV_SECURITY_0 is simply plain text communication.
         *      - NETWORK_PROV_SECURITY_1 is secure communication which consists of secure handshake
         *          using X25519 key exchange and proof of possession (pop) and AES-CTR
         *          for encryption/decryption of messages.
         *      - NETWORK_PROV_SECURITY_2 SRP6a based authentication and key exchange
         *        + AES-GCM encryption/decryption of messages
         */
        network_prov_security_t security = NETWORK_PROV_SECURITY_1;

        /* Do we want a proof-of-possession (ignored if Security 0 is selected):
         *      - this should be a string with length > 0
         *      - NULL if not used
         */
        const char *pop = "Lamp3232";

        /* This is the structure for passing security parameters
         * for the protocomm security 1.
         */
        network_prov_security1_params_t *sec_params = pop;

        const char *username = NULL;

#elif CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
        network_prov_security_t security = NETWORK_PROV_SECURITY_2;
        /* The username must be the same one, which has been used in the generation of salt and verifier */

#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
        /* This pop field represents the password that will be used to generate salt and verifier.
         * The field is present here in order to generate the QR code containing password.
         * In production this password field shall not be stored on the device */
        const char *username = EXAMPLE_PROV_SEC2_USERNAME;
        const char *pop = EXAMPLE_PROV_SEC2_PWD;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
        /* The username and password shall not be embedded in the firmware,
         * they should be provided to the user by other means.
         * e.g. QR code sticker */
        const char *username = NULL;
        const char *pop = NULL;
#endif
        /* This is the structure for passing security parameters
         * for the protocomm security 2.
         * If dynamically allocated, sec2_params pointer and its content
         * must be valid till NETWORK_PROV_END event is triggered.
         */
        network_prov_security2_params_t sec2_params = {};

        ESP_ERROR_CHECK(example_get_sec2_salt(&sec2_params.salt, &sec2_params.salt_len));
        ESP_ERROR_CHECK(example_get_sec2_verifier(&sec2_params.verifier, &sec2_params.verifier_len));

        network_prov_security2_params_t *sec_params = &sec2_params;
#endif
        /* What is the service key (could be NULL)
         * This translates to :
         *     - Wi-Fi password when scheme is network_prov_scheme_softap
         *          (Minimum expected length: 8, maximum 64 for WPA2-PSK)
         *     - simply ignored when scheme is network_prov_scheme_ble
         */
        const char *service_key = NULL;

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        /* This step is only useful when scheme is network_prov_scheme_ble. This will
         * set a custom 128 bit UUID which will be included in the BLE advertisement
         * and will correspond to the primary GATT service that provides provisioning
         * endpoints as GATT characteristics. Each GATT characteristic will be
         * formed using the primary service UUID as base, with different auto assigned
         * 12th and 13th bytes (assume counting starts from 0th byte). The client side
         * applications must identify the endpoints by reading the User Characteristic
         * Description descriptor (0x2901) for each characteristic, which contains the
         * endpoint name of the characteristic */
        uint8_t custom_service_uuid[] = {
            /* LSB <---------------------------------------
             * ---------------------------------------> MSB */
            0xb4,
            0xdf,
            0x5a,
            0x1c,
            0x3f,
            0x6b,
            0xf4,
            0xbf,
            0xea,
            0x4a,
            0x82,
            0x03,
            0x04,
            0x90,
            0x1a,
            0x02,
        };

        /* If your build fails with linker errors at this point, then you may have
         * forgotten to enable the BT stack or BTDM BLE settings in the SDK (e.g. see
         * the sdkconfig.defaults in the example project) */
        network_prov_scheme_ble_set_service_uuid(custom_service_uuid);
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

        /* An optional endpoint that applications can create if they expect to
         * get some additional custom data during provisioning workflow.
         * The endpoint name can be anything of your choice.
         * This call must be made before starting the provisioning.
         */
        network_prov_mgr_endpoint_create("custom-data");

        /* Do not stop and de-init provisioning even after success,
         * so that we can restart it later. */
#ifdef CONFIG_EXAMPLE_REPROVISIONING
        network_prov_mgr_disable_auto_stop(1000);
#endif
        /* Start provisioning service */
        ESP_ERROR_CHECK(network_prov_mgr_start_provisioning(security, (const void *)sec_params, service_name, service_key));

        /* The handler for the optional endpoint created above.
         * This call must be made after starting the provisioning, and only if the endpoint
         * has already been created above.
         */
        network_prov_mgr_endpoint_register("custom-data", custom_prov_data_handler, NULL);

        /* Uncomment the following to wait for the provisioning to finish and then release
         * the resources of the manager. Since in this case de-initialization is triggered
         * by the default event loop handler, we don't need to call the following */
        // network_prov_mgr_wait();
        // network_prov_mgr_deinit();
        /* Print QR code for provisioning */
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        wifi_prov_print_qr(service_name, username, pop, PROV_TRANSPORT_BLE);
#else  /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
        wifi_prov_print_qr(service_name, username, pop, PROV_TRANSPORT_SOFTAP);
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
    }
    else
    {
        ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");

        /* We don't need the manager as device is already provisioned,
         * so let's release it's resources */
        ESP_ERROR_CHECK(network_prov_mgr_deinit());

        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        /* Start Wi-Fi station */
        wifi_init_sta();
    }

    /* Block execution here until background events signal a successful IP assignment */
    ESP_LOGI(TAG, "Waiting for Wi-Fi connection link to stabilize...");
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_EVENT, true, true, portMAX_DELAY);

    ESP_LOGI(TAG, "Wi-Fi link established successfully. Handing control to app_main.");
}
