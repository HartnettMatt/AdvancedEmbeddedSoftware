/**
 * @file motor.c
 * @brief Motor control implementation using libgpiod.
 */
#include "motor.h"
#include <stdio.h>
#include <errno.h>

static struct gpiod_line_request *mr1_req = NULL;
static struct gpiod_line_request *mr2_req = NULL;
static struct gpiod_line_request *ml1_req = NULL;
static struct gpiod_line_request *ml2_req = NULL;

int motor_init(void)
{
    enum gpiod_line_value inactive = GPIOD_LINE_VALUE_INACTIVE;
    char *chip_path = GPIO_CHIP;
    unsigned int right1_offset = MOTOR_RIGHT_1_OFFSET;
    unsigned int right2_offset = MOTOR_RIGHT_2_OFFSET;
    unsigned int left1_offset = MOTOR_LEFT_1_OFFSET;
    unsigned int left2_offset = MOTOR_LEFT_2_OFFSET;

    mr1_req = request_output_line(chip_path, right1_offset, inactive, "motor");
    if (!mr1_req) goto fail;
    mr2_req = request_output_line(chip_path, right2_offset, inactive, "motor");
    if (!mr2_req) goto fail;
    ml1_req = request_output_line(chip_path, left1_offset, inactive, "motor");
    if (!ml1_req) goto fail;
    ml2_req = request_output_line(chip_path, left2_offset, inactive, "motor");
    if (!ml2_req) goto fail;

    return 0;

fail:
    perror("motor_init");
    if (mr1_req) gpiod_line_request_release(mr1_req);
    if (mr2_req) gpiod_line_request_release(mr2_req);
    if (ml1_req) gpiod_line_request_release(ml1_req);
    if (ml2_req) gpiod_line_request_release(ml2_req);
    return -1;
}

void motor_forward_start(void)
{
    gpiod_line_request_set_value(mr1_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(mr2_req, 0, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(ml1_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml2_req, 0, GPIOD_LINE_VALUE_ACTIVE);
}

void motor_stop(void)
{
    gpiod_line_request_set_value(mr1_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(mr2_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml1_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml2_req, 0, GPIOD_LINE_VALUE_INACTIVE);
}

void motor_backward_start(void)
{
    gpiod_line_request_set_value(mr1_req, 0, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(mr2_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml1_req, 0, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(ml2_req, 0, GPIOD_LINE_VALUE_INACTIVE);
}

void motor_turn_cw_start(void)
{
    gpiod_line_request_set_value(mr1_req, 0, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(mr2_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml1_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml2_req, 0, GPIOD_LINE_VALUE_ACTIVE);
}

void motor_turn_ccw_start(void)
{
    gpiod_line_request_set_value(mr1_req, 0, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(mr2_req, 0, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(ml1_req, 0, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(ml2_req, 0, GPIOD_LINE_VALUE_INACTIVE);
}

void motor_deinit(void)
{
    gpiod_line_request_release(mr1_req);
    gpiod_line_request_release(mr2_req);
    gpiod_line_request_release(ml1_req);
    gpiod_line_request_release(ml2_req);
}
