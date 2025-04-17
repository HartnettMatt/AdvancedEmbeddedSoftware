/*----------------------------------------------------------------------*
 *  LibDriver HC‑SR04  ↔  libgpiod v2.2 port layer                      *
 *  GPIO mapping (BCM numbering):                                       *
 *      Trig → GPIO 17  (physical pin 3)                                 *
 *      Echo → GPIO 27  (physical pin 5)                                 *
 *----------------------------------------------------------------------*/

#include <gpiod.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "driver_hcsr04_interface.h"

#define TRIG_GPIO 17
#define ECHO_GPIO 27

static struct gpiod_line_request *trig_req = NULL;
static struct gpiod_line_request *echo_req = NULL;

/**
 * @brief Request a single GPIO line with libgpiod v2 and
 *        cleanly free all intermediate objects on error.
 *
 * @param chip_path  Path to gpiochip device (e.g. "/dev/gpiochip0").
 * @param offset     BCM GPIO number.
 * @param direction  GPIOD_LINE_DIRECTION_{INPUT,OUTPUT}.
 * @param value      Initial output value (ignored for inputs).
 * @param consumer   Consumer label or NULL.
 * @return Pointer to a gpiod_line_request on success, NULL on error.
 */
static struct gpiod_line_request *
request_line(const char *chip_path, unsigned int offset,
             enum gpiod_line_direction direction,
             enum gpiod_line_value value,
             const char *consumer)
{
    struct gpiod_request_config *req_cfg = NULL;
    struct gpiod_line_request  *request  = NULL;
    struct gpiod_line_settings *settings = NULL;
    struct gpiod_line_config   *line_cfg = NULL;
    struct gpiod_chip          *chip     = NULL;
    int ret;

    chip = gpiod_chip_open(chip_path);
    if (!chip)
        return NULL;

    settings = gpiod_line_settings_new();
    if (!settings)
        goto close_chip;

    gpiod_line_settings_set_direction(settings, direction);
    if (direction == GPIOD_LINE_DIRECTION_OUTPUT)
        gpiod_line_settings_set_output_value(settings, value);

    line_cfg = gpiod_line_config_new();
    if (!line_cfg)
        goto free_settings;

    ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
    if (ret)
        goto free_line_cfg;

    if (consumer) {
        req_cfg = gpiod_request_config_new();
        if (!req_cfg)
            goto free_line_cfg;

        gpiod_request_config_set_consumer(req_cfg, consumer);
    }

    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    gpiod_request_config_free(req_cfg);

free_line_cfg:
    gpiod_line_config_free(line_cfg);

free_settings:
    gpiod_line_settings_free(settings);

close_chip:
    gpiod_chip_close(chip);
    return request;
}

/**
 * @brief Initialise the Trig line (output, LOW).
 * @return 0 on success, 1 on failure.
 */
uint8_t hcsr04_interface_trig_init(void)
{
    trig_req = request_line(CHIP_DEV, TRIG_GPIO,
                            GPIOD_LINE_DIRECTION_OUTPUT,
                            GPIOD_LINE_VALUE_INACTIVE,
                            "hcsr04-trig");
    return (trig_req == NULL);
}

/**
 * @brief Release the Trig line.
 * @return Always 0.
 */
uint8_t hcsr04_interface_trig_deinit(void)
{
    if (trig_req) {
        gpiod_line_request_release(trig_req);
        trig_req = NULL;
    }
    return 0;
}

/**
 * @brief Drive the Trig line high or low.
 * @param value 0 = LOW, non‑zero = HIGH.
 * @return 0 on success, 1 on error.
 */
uint8_t hcsr04_interface_trig_write(uint8_t value)
{
    if (!trig_req) return 1;
    struct gpiod_line *line = gpiod_line_request_get_line(trig_req, 0);
    return gpiod_line_set_value(line, value ? 1 : 0) ? 1 : 0;
}

/**
 * @brief Initialise the Echo line (input).
 * @return 0 on success, 1 on failure.
 */
uint8_t hcsr04_interface_echo_init(void)
{
    echo_req = request_line(CHIP_DEV, ECHO_GPIO,
                            GPIOD_LINE_DIRECTION_INPUT,
                            GPIOD_LINE_VALUE_INACTIVE,
                            "hcsr04-echo");
    return (echo_req == NULL);
}

/**
 * @brief Release the Echo line.
 * @return Always 0.
 */
uint8_t hcsr04_interface_echo_deinit(void)
{
    if (echo_req) {
        gpiod_line_request_release(echo_req);
        echo_req = NULL;
    }
    return 0;
}

/**
 * @brief Read current logic level on Echo line.
 * @param[out] value 0 = LOW, 1 = HIGH.
 * @return 0 on success, 1 on error.
 */
uint8_t hcsr04_interface_echo_read(uint8_t *value)
{
    if (!echo_req) return 1;
    struct gpiod_line *line = gpiod_line_request_get_line(echo_req, 0);
    int v = gpiod_line_get_value(line);
    if (v < 0) return 1;
    *value = (uint8_t)v;
    return 0;
}

/**
 * @brief Return a monotonic timestamp with µs resolution.
 * @param[out] t Filled with current time.
 * @return 0 on success, 1 on error.
 */
uint8_t hcsr04_interface_timestamp_read(hcsr04_time_t *t)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0) return 1;
    t->microsecond = (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
    t->millisecond = (uint32_t)(t->microsecond / 1000ULL);
    return 0;
}

/**
 * @brief Optional printf‑style debug output (compile‑time gated).
 */
void hcsr04_interface_debug_print(const char *const fmt, ...)
{
#ifdef HCSR04_ENABLE_DEBUG_PRINT
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
#endif
}