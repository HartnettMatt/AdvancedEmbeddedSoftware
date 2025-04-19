/**
 * @file hcsr04.c
 * @brief HC-SR04 ultrasonic sensor implementation.
 * @details
 * Implements initialization, single-shot measurement with clamping, and
 * deinitialization for the HC-SR04 sensor using libgpiod and the
 * libdriver_hcsr04 core driver.
 */

#include "hcsr04.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <gpiod.h>
#include "driver_hcsr04.h"
#include "driver_hcsr04_interface.h"
#include "gpiod.h"

// Internal libgpiod line requests
static struct gpiod_line_request *trig_req = NULL;
static struct gpiod_line_request *echo_req = NULL;

//------------------------------------------------------------------------------
// driver_hcsr04_interface implementations
uint8_t hcsr04_interface_trig_init(void) {
    trig_req = request_output_line(GPIO_CHIP,
                                   TRIG_GPIO_OFFSET,
                                   GPIOD_LINE_VALUE_INACTIVE,
                                   "hcsr04-trig");
    return (trig_req == NULL);
}

uint8_t hcsr04_interface_trig_deinit(void) {
    if (trig_req) {
        gpiod_line_request_release(trig_req);
        trig_req = NULL;
    }
    return 0;
}

uint8_t hcsr04_interface_trig_write(uint8_t value) {
    if (!trig_req)
        return 1;
    return gpiod_line_request_set_value(
               trig_req, TRIG_GPIO_OFFSET,
               value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE)
               ? 1 : 0;
}

uint8_t hcsr04_interface_echo_init(void) {
    echo_req = request_input_line(GPIO_CHIP,
                                  ECHO_GPIO_OFFSET,
                                  "hcsr04-echo");
    return (echo_req == NULL);
}

uint8_t hcsr04_interface_echo_deinit(void) {
    if (echo_req) {
        gpiod_line_request_release(echo_req);
        echo_req = NULL;
    }
    return 0;
}

uint8_t hcsr04_interface_echo_read(uint8_t *value) {
    if (!echo_req)
        return 1;
    enum gpiod_line_value val = gpiod_line_request_get_value(
        echo_req, ECHO_GPIO_OFFSET);
    *value = (val == GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
    return 0;
}

uint8_t hcsr04_interface_timestamp_read(hcsr04_time_t *t) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0)
        return 1;
    t->microsecond = (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
    t->millisecond = (uint32_t)(t->microsecond / 1000);
    return 0;
}

void hcsr04_interface_debug_print(const char *const fmt, ...) {
#ifdef HCSR04_ENABLE_DEBUG_PRINT
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
#endif
}

void hcsr04_interface_delay_us(uint32_t us) {
    usleep(us);
}

void hcsr04_interface_delay_ms(uint32_t ms) {
    usleep((useconds_t)ms * 1000);
}

//------------------------------------------------------------------------------
// Public API
static hcsr04_handle_t _hcsr04_handle;

int init_hcsr04(void) {
    char *chip_path = GPIO_CHIP;
    DRIVER_HCSR04_LINK_INIT(&_hcsr04_handle, hcsr04_handle_t);
    DRIVER_HCSR04_LINK_TRIG_INIT(&_hcsr04_handle, hcsr04_interface_trig_init);
    DRIVER_HCSR04_LINK_TRIG_DEINIT(&_hcsr04_handle, hcsr04_interface_trig_deinit);
    DRIVER_HCSR04_LINK_TRIG_WRITE(&_hcsr04_handle, hcsr04_interface_trig_write);
    DRIVER_HCSR04_LINK_ECHO_INIT(&_hcsr04_handle, hcsr04_interface_echo_init);
    DRIVER_HCSR04_LINK_ECHO_DEINIT(&_hcsr04_handle, hcsr04_interface_echo_deinit);
    DRIVER_HCSR04_LINK_ECHO_WRITE(&_hcsr04_handle, hcsr04_interface_echo_read);
    DRIVER_HCSR04_LINK_TIMESTAMP_READ(&_hcsr04_handle, hcsr04_interface_timestamp_read);
    DRIVER_HCSR04_LINK_DELAY_US(&_hcsr04_handle, hcsr04_interface_delay_us);
    DRIVER_HCSR04_LINK_DELAY_MS(&_hcsr04_handle, hcsr04_interface_delay_ms);
    DRIVER_HCSR04_LINK_DEBUG_PRINT(&_hcsr04_handle, hcsr04_interface_debug_print);

    return hcsr04_init(&_hcsr04_handle);
}

int hcsr04_read(uint32_t *echo_time_us, float *distance_m) {
    uint32_t raw_us;
    float raw_m;
    int ret = hcsr04_read(&_hcsr04_handle, &raw_us, &raw_m);
    if (ret)
        return ret;
    // clamp reflections >1000us
    if (raw_us > 1000U) {
        raw_us -= 1000U;
        raw_m  -= 0.17f;
    }
    *echo_time_us = raw_us;
    *distance_m   = raw_m;
    return 0;
}

void deinit_hcsr04(void) {
    hcsr04_deinit(&_hcsr04_handle);
}
