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

static unsigned int mr1_offset = MOTOR_RIGHT_1_OFFSET;
static unsigned int mr2_offset = MOTOR_RIGHT_2_OFFSET;
static unsigned int ml1_offset = MOTOR_LEFT_1_OFFSET;
static unsigned int ml2_offset = MOTOR_LEFT_2_OFFSET;

int motor_init(void)
{
    enum gpiod_line_value inactive = GPIOD_LINE_VALUE_INACTIVE;
    char *chip_path = GPIO_CHIP;

    mr1_req = request_output_line(chip_path, mr1_offset, inactive, "motor mr1");
    if (!mr1_req) goto fail;
    mr2_req = request_output_line(chip_path, mr2_offset, inactive, "motor mr2");
    if (!mr2_req) goto fail;
    ml1_req = request_output_line(chip_path, ml1_offset, inactive, "motor ml1");
    if (!ml1_req) goto fail;
    ml2_req = request_output_line(chip_path, ml2_offset, inactive, "motor ml2");
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
    gpiod_line_request_set_value(mr1_req, mr1_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(mr2_req, mr2_offset, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(ml1_req, ml1_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml2_req, ml2_offset, GPIOD_LINE_VALUE_ACTIVE);
}

void motor_stop(void)
{
    gpiod_line_request_set_value(mr1_req, mr1_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(mr2_req, mr2_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml1_req, ml1_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml2_req, ml2_offset, GPIOD_LINE_VALUE_INACTIVE);
}

void motor_backward_start(void)
{
    gpiod_line_request_set_value(mr1_req, mr1_offset, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(mr2_req, mr2_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml1_req, ml1_offset, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(ml2_req, ml2_offset, GPIOD_LINE_VALUE_INACTIVE);
}

void motor_turn_cw_start(void)
{
    gpiod_line_request_set_value(mr1_req, mr1_offset, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(mr2_req, mr2_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml1_req, ml1_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(ml2_req, ml2_offset, GPIOD_LINE_VALUE_ACTIVE);
}

void motor_turn_ccw_start(void)
{
    gpiod_line_request_set_value(mr1_req, mr1_offset, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(mr2_req, mr2_offset, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(ml1_req, ml1_offset, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_request_set_value(ml2_req, ml2_offset, GPIOD_LINE_VALUE_INACTIVE);
}

void motor_deinit(void)
{
    gpiod_line_request_release(mr1_req);
    gpiod_line_request_release(mr2_req);
    gpiod_line_request_release(ml1_req);
    gpiod_line_request_release(ml2_req);
}
