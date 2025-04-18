/*----------------------------------------------------------------------*
 *  LibDriver HC‑SR04  ↔  libgpiod v2.2 port layer                      *
 *  GPIO mapping (BCM numbering):                                       *
 *      Trig → GPIO 17  (physical pin 3)                                *
 *      Echo → GPIO 27  (physical pin 5)                                *
 *----------------------------------------------------------------------*/
#include <errno.h>
#include <gpiod.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "driver_hcsr04_interface.h"

#define TRIG_GPIO 17
#define ECHO_GPIO 27
#define GPIO_CHIP "/dev/gpiochip0"

static struct gpiod_line_request *trig_req = NULL;
static struct gpiod_line_request *echo_req = NULL;

// This function does all of the setup necessary to control a GPIO line
// This function was copied from libgpiod
static struct gpiod_line_request *
request_output_line(const char *chip_path, unsigned int offset, enum gpiod_line_value value, const char *consumer) {
    struct gpiod_request_config *req_cfg = NULL;
    struct gpiod_line_request *request = NULL;
    struct gpiod_line_settings *settings;
    struct gpiod_line_config *line_cfg;
    struct gpiod_chip *chip;
    int ret;

    chip = gpiod_chip_open(chip_path);
    if (!chip)
        return NULL;

    settings = gpiod_line_settings_new();
    if (!settings)
        goto close_chip;

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, value);

    line_cfg = gpiod_line_config_new();
    if (!line_cfg)
        goto free_settings;

    ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
                                              settings);
    if (ret)
        goto free_line_config;

    if (consumer) {
        req_cfg = gpiod_request_config_new();
        if (!req_cfg)
            goto free_line_config;

        gpiod_request_config_set_consumer(req_cfg, consumer);
    }

    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    gpiod_request_config_free(req_cfg);

free_line_config:
    gpiod_line_config_free(line_cfg);

free_settings:
    gpiod_line_settings_free(settings);

close_chip:
    gpiod_chip_close(chip);

    return request;
}
// This function does all of the setup necessary to read a GPIO line
// This function was copied from libgpiod
static struct gpiod_line_request *request_input_line(const char *chip_path,
						     unsigned int offset,
						     const char *consumer)
{
	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings;
	struct gpiod_line_config *line_cfg;
	struct gpiod_chip *chip;
	int ret;

	chip = gpiod_chip_open(chip_path);
	if (!chip)
		return NULL;

	settings = gpiod_line_settings_new();
	if (!settings)
		goto close_chip;

	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

	line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto free_settings;

	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
						  settings);
	if (ret)
		goto free_line_config;

	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;

		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
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
    trig_req = request_output_line(GPIO_CHIP, TRIG_GPIO, GPIOD_LINE_VALUE_INACTIVE, "hcsr04-trig");
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
    return gpiod_line_request_set_value(trig_req, TRIG_GPIO, value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE) ? 1 : 0;
}

/**
 * @brief Initialise the Echo line (input).
 * @return 0 on success, 1 on failure.
 */
uint8_t hcsr04_interface_echo_init(void)
{
     echo_req = request_input_line(GPIO_CHIP, ECHO_GPIO, "hcsr04-echo");
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
    enum gpiod_line_value val;
    val = gpiod_line_request_get_value(echo_req, ECHO_GPIO);
    *value = val == GPIOD_LINE_VALUE_ACTIVE ? 1 : 0;
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

void hcsr04_interface_delay_us(uint32_t us) { usleep(us); }

void hcsr04_interface_delay_ms(uint32_t ms) { usleep((useconds_t)ms * 1000); }