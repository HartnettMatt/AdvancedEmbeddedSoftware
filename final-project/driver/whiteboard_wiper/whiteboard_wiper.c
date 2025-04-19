/*----------------------------------------------------------------------*
 *  HC‑SR04 functional test                                             *
 *  Reads distance every 100 ms for 10 s (100 samples).                 *
 *----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*
 *  HC‑SR04 functional test                                             *
 *  Reads distance every 100 ms for 10 s (100 samples).                 *
 *----------------------------------------------------------------------*/

#include "AdvancedEmbeddedSoftware/final-project/driver/whiteboard_wiper/inc/hcsr04.h"
#include "AdvancedEmbeddedSoftware/final-project/driver/whiteboard_wiper/inc/motor.h"
#include "AdvancedEmbeddedSoftware/final-project/driver/whiteboard_wiper/motor.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <math.h>
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

    // Init motor
    if(motor_init() != 0){
        goto motor_fail;
    }
    // Init sensor
    if(hcsr04_init() != 0){
        goto hcsr04_fail;
    }

    uint32_t echo_time;
    float dist_m;
    float wall_dist_m;

    // Calibrate wall distance
    for(int i = 0; i < CAL_CYCLES; i++){
        if(hcsr04_read(&echo_time, &dist_m) != 0){
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
    printf("Calibrated wall distance = %6.1f cm", dist_m * 100.0f);

    // Start control loop
    motor_forward_start();

    while(1){
        if(hcsr04_read(&echo_time, &dist_m) != 0){
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

    hcsr04_deinit();
    motor_deinit();
    printf("Done.");
    return 0;

    cleanup:
        printf("Cleaning up");
        motor_stop();
    cal_fail:
        hcsr04_deinit();
    hcsr04_fail:
        motor_deinit();
    motor_fail:
        return 1;
}
