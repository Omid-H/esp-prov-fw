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

/**
 * @brief Initializes NVS, network interfaces, and handles the complete 
 * Wi-Fi provisioning lifecycle. Blocks until connected to an AP.
 */
void init_wifi_provisioning(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_PROV_H
