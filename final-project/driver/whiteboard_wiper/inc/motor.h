/**
 * @file motor.h
 * @brief Motor control interface using libgpiod.
 * @details
 * Provides initialization and control functions for a dual-motor setup.
 * Some code adapted from libgpiod examples:
 * https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/tree/examples
 */


#ifndef MOTOR_H
#define MOTOR_H

#ifndef GPIO_CHIP
#define GPIO_CHIP "/dev/gpiochip0"
#endif // GPIO_CHIP

#define MOTOR_RIGHT_1_OFFSET 15
#define MOTOR_RIGHT_2_OFFSET 18
#define MOTOR_LEFT_1_OFFSET 23
#define MOTOR_LEFT_2_OFFSET 24

#include "gpiod.h"


/**
 * @brief Initialize motor control lines.
 *
 * Requests four GPIO lines for motor control.
 *
 * @return 0 on success, -1 on failure (errno set).
 */
int motor_init(void);

/**
 * @brief Start both motors moving forward.
 */
void motor_forward_start(void);

/**
 * @brief Stop both motors.
 */
void motor_stop(void);

/**
 * @brief Start both motors moving backward.
 */
void motor_backward_start(void);

/**
 * @brief Start a clockwise turn.
 */
void motor_turn_cw_start(void);

/**
 * @brief Start a counterclockwise turn.
 */
void motor_turn_ccw_start(void);

/**
 * @brief Start a counterclockwise turn.
 */
void motor_deinit(void);

#endif // MOTOR_H