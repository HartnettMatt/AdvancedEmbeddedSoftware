/*----------------------------------------------------------------------*
 *  HC‑SR04 functional test                                             *
 *  Reads distance every 100 ms for 10 s (100 samples).                 *
 *----------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include "driver_hcsr04.h"
#include "driver_hcsr04_interface.h"

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


    hcsr04_handle_t handle;
    uint32_t echo_us;
    float    dist_m;
    int      ret;

    DRIVER_HCSR04_LINK_INIT(&handle, hcsr04_handle_t);
    DRIVER_HCSR04_LINK_TRIG_INIT(&handle,       hcsr04_interface_trig_init);
    DRIVER_HCSR04_LINK_TRIG_DEINIT(&handle,     hcsr04_interface_trig_deinit);
    DRIVER_HCSR04_LINK_TRIG_WRITE(&handle,      hcsr04_interface_trig_write);
    DRIVER_HCSR04_LINK_ECHO_INIT(&handle,       hcsr04_interface_echo_init);
    DRIVER_HCSR04_LINK_ECHO_DEINIT(&handle,     hcsr04_interface_echo_deinit);
    DRIVER_HCSR04_LINK_ECHO_WRITE(&handle,      hcsr04_interface_echo_read);
    DRIVER_HCSR04_LINK_TIMESTAMP_READ(&handle,  hcsr04_interface_timestamp_read);
    DRIVER_HCSR04_LINK_DELAY_US(&handle,        hcsr04_interface_delay_us);
    DRIVER_HCSR04_LINK_DELAY_MS(&handle,        hcsr04_interface_delay_ms);
    DRIVER_HCSR04_LINK_DEBUG_PRINT(&handle,     hcsr04_interface_debug_print);

    ret = hcsr04_init(&handle);
    if (ret) {
        fprintf(stderr, "HC‑SR04 init failed (%d)\n", ret);
        return 1;
    }
    printf("HC-SR04 initialised - reading 100 samples at 10 Hz");

    for (int i = 0; i < 50; i++) {
        if (hcsr04_read(&handle, &echo_us, &dist_m) == 0) {
            if(echo_us > 1000){
                echo_us = echo_us - 1000;
                dist_m = dist_m - (0.17);
            }
            printf("Sample %3d: %7u µs  =  %6.1f cm\n",
                   i, echo_us, dist_m * 100.0f);
        } else {
            fprintf(stderr, "Read error at sample %d\n", i);
        }
        usleep(100000);            // 100 ms
    }

    hcsr04_deinit(&handle);
    printf("Done.");
    return 0;
}
