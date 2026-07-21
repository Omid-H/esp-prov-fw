/*
 * SPDX-FileCopyrightText: 2026 Nariman Ziaie
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on Espressif Systems network provisioning examples.
 */

#ifndef WIFI_PROV_H
#define WIFI_PROV_H

#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_PROV_SEC2_USERNAME "smartlamp"
#define EXAMPLE_PROV_SEC2_PWD "Lamp3232"

typedef enum {
    LED_STATE_PROVISIONING, // waiting for credentials (fast blink: 100ms ON / 100ms OFF)
    LED_STATE_CONNECTING,   // connecting to AP (medium blink: 500ms ON / 500ms OFF)
    LED_STATE_CONNECTED,    // connected to Wi-Fi (slow heartbeat: 100ms ON / 1900ms OFF)
    LED_STATE_ERROR         // error state (rapid double-blink, then pause)
} led_status_t;

void set_led_status(led_status_t status);

/**
 * @brief Initializes NVS, network interfaces, and handles the complete 
 * Wi-Fi provisioning lifecycle. Blocks until connected to an AP.
 */
void init_wifi_provisioning(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_PROV_H
