/*
 * SPDX-FileCopyrightText: 2026 Nariman Ziaie
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on Espressif Systems network provisioning examples.
 */

#ifndef STATUS_LED_H
#define STATUS_LED_H

#include "wifi_prov.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the status LED GPIO and spawns the background blinking task.
 */
void init_status_led(void);

#ifdef __cplusplus
}
#endif

#endif // STATUS_LED_H
