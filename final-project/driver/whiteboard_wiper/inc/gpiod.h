/**
 * @file gpiod.h
 * @brief GPIO line request interface.
 * @details
 * This code is adapted from the libgpiod example repository:
 * https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/
 */

#ifndef GPIOD_H
#define GPIOD_H

#ifndef GPIO_CHIP
#define GPIO_CHIP "/dev/gpiochip0"
#endif // GPIO_CHIP

#include <errno.h>
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Request a GPIO line for output.
 *
 * Opens the specified GPIO chip and configures a single line at the given
 * offset as an output, setting its initial value.
 *
 * @param chip_path Path to the GPIO chip device (e.g., "/dev/gpiochip0").
 * @param offset    Zero-based index of the line within the GPIO chip.
 * @param value     Initial output value: 0 (low) or 1 (high).
 * @param consumer  String label identifying the consumer of this line.
 *
 * @return On success, returns a pointer to an allocated gpiod_line_request
 *         structure. On failure, returns NULL and errno is set.
 */
struct gpiod_line_request * request_output_line(const char *chip_path, unsigned int offset, enum gpiod_line_value value, const char *consumer);

/**
 * @brief Request a GPIO line for input.
 *
 * Opens the specified GPIO chip and configures a single line at the given
 * offset as an input.
 *
 * @param chip_path Path to the GPIO chip device (e.g., "/dev/gpiochip0").
 * @param offset    Zero-based index of the line within the GPIO chip.
 * @param consumer  String label identifying the consumer of this line.
 *
 * @return On success, returns a pointer to an allocated gpiod_line_request
 *         structure. On failure, returns NULL and errno is set.
 */
struct gpiod_line_request * request_input_line(const char *chip_path, unsigned int offset, const char *consumer);

#endif // GPIOD_H