/*
 * SPDX-FileCopyrightText: 2026 Nariman Ziaie
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on Espressif Systems network provisioning examples.
 */

#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the factory reset button GPIO and spawns the monitoring task.
 */
void init_factory_reset_monitor(void);

#ifdef __cplusplus
}
#endif

#endif // FACTORY_RESET_H
