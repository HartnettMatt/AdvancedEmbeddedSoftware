/**
 * @file hcsr04.h
 * @brief HC-SR04 ultrasonic sensor interface.
 * @details
 * Provides initialization, single-shot measurement, and cleanup functions
 * for the HC-SR04 ultrasonic distance sensor using libgpiod and the
 * libdriver_hcsr04 core driver. Clamps spurious reflections >1000 µs.
 */

#ifndef HCSR04_H
#define HCSR04_H

#include <stdint.h>

#ifndef GPIO_CHIP
#define GPIO_CHIP "/dev/gpiochip0"
#endif // GPIO_CHIP
#define TRIG_GPIO_OFFSET      17
#define ECHO_GPIO_OFFSET      27

/**
 * @brief Initialize the HC-SR04 sensor.
 *
 * Sets up GPIO lines and links the low-level interface functions
 * to the libdriver_hcsr04 core driver.
 *
 * @param chip_path Path to the GPIO chip device (e.g., GPIO_CHIP).
 * @return 0 on success, non-zero on failure.
 */
int init_hcsr04(void);

/**
 * @brief Perform a single distance measurement.
 *
 * Calls the libdriver read routine, then clamps the echo time to handle
 * spurious reflections: if the measured echo time exceeds 1000 µs,
 * it is reduced by 1000 µs and the calculated distance is adjusted by
 * subtracting 0.17 m.
 *
 * @param[out] echo_time_us Raw echo pulse duration in microseconds,
 *                          after clamping.
 * @param[out] distance_m   Calculated distance in meters,
 *                          after clamping.
 * @return 0 on success, non-zero on error.
 */
int read_hcsr04(uint32_t *echo_time_us, float *distance_m);

/**
 * @brief Deinitialize the HC-SR04 sensor.
 *
 * Releases GPIO lines and cleans up internal state.
 */
void deinit_hcsr04(void);

#endif // HCSR04_H
