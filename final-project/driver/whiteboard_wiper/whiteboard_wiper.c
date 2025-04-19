/**
 * @file main.c
 * @brief Main control loop for the whiteboard wiper application.
 * @author Matt Hartnett
 * @details
 * Sets up real‑time scheduling and CPU affinity, initializes the motor and
 * HC‑SR04 ultrasonic sensor, calibrates the target wall distance over a
 * fixed number of samples, then enters a continuous control loop:
 *  - Drives the motor forward
 *  - Monitors distance to the wall
 *  - If deviation beyond a threshold is detected, stops, reverses,
 *    turns around, and resumes forward motion
 *  - Responds to SIGINT (Ctrl+C) to cleanly exit the loop and deinitialize
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include "whiteboard_wiper.h"

static volatile sig_atomic_t keep_running = 1;

static void handle_sigint(int sig)
{
    (void)sig;            // unused
    keep_running = 0;
}

int main(void)
{
    // Increase scheduler priority and lock to a single core
    struct sched_param p;
    p.sched_priority = 80;
    if (sched_setscheduler(0, SCHED_FIFO, &p) < 0) {
        perror("sched_setscheduler");
    }
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);    // pin to core 0
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0)
        perror("sched_setaffinity");

    printf("Start init procedure...\n");
    // Init motor
    if(motor_init() != 0){
        goto motor_fail;
    }
    // Init sensor
    if(init_hcsr04() != 0){
        goto hcsr04_fail;
    }

    // Test motors
    printf("Testing motors...\n");
    motor_forward_start();
    usleep(10);
    motor_stop();
    usleep(10);
    motor_backward_start();
    usleep(10);
    motor_stop();

    uint32_t echo_time;
    float dist_m;
    float wall_dist_m;

    // Calibrate wall distance
    printf("Calibrating wall distance\n");
    for(int i = 0; i < CAL_CYCLES; i++){
        if(read_hcsr04(&echo_time, &dist_m) != 0){
            goto cal_fail;
        }
        wall_dist_m = wall_dist_m + dist_m;
        usleep(100000);            // 100 ms
        if (!keep_running) {
            // Interrupted during calibration
            goto cleanup;
        }
    }
    wall_dist_m = wall_dist_m / CAL_CYCLES;
    printf("Calibrated wall distance = %6.1f cm\n", dist_m * 100.0f);

    // Start control loop
    motor_forward_start();

    while(1){
        if(read_hcsr04(&echo_time, &dist_m) != 0){
            goto cleanup;
        }
        if(fabs(dist_m - wall_dist_m) > WALL_RANGE){
            // Edge of wall detected, turn around
            motor_stop();
            usleep(10);
            motor_backward_start();
            usleep(REVERSE_TIME); // 100 ms
            if (!keep_running) {
                // Interrupted during motor control loop
                goto cleanup;
            }
            motor_stop();
            usleep(10);
            motor_turn_cw_start();
            usleep(TURNAROUND_TIME); // 100 ms
            if (!keep_running) {
                // Interrupted during motor control loop
                goto cleanup;
            }
            motor_stop();
            usleep(10);
            motor_forward_start();
        }
        usleep(100000);            // 100 ms
        if (!keep_running) {
            // Interrupted during motor control loop
            goto cleanup;
        }
    }

    deinit_hcsr04();
    motor_deinit();
    printf("Done.\n");
    return 0;

    cleanup:
        printf("Cleaning up\n");
        motor_stop();
    cal_fail:
        deinit_hcsr04();
    hcsr04_fail:
        motor_deinit();
    motor_fail:
        return 1;
}
