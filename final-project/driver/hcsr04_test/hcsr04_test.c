/*----------------------------------------------------------------------*
 *  HC‑SR04 functional test                                             *
 *  Reads distance every 100 ms for 10 s (100 samples).                 *
 *----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*
 *  HC‑SR04 functional test                                             *
 *  Reads distance every 100 ms for 10 s (100 samples).                 *
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include "driver_hcsr04.h"
#include "driver_hcsr04_interface.h"

int main(void)
{
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
    puts("HC‑SR04 initialised — reading 100 samples at 10 Hz…");

    for (int i = 0; i < 100; ++i) {
        if (hcsr04_read(&handle, &echo_us, &dist_m) == 0) {
            printf("Sample %3d: %7u µs  ≈  %6.1f cm\n",
                   i + 1, echo_us, dist_m * 100.0f);
        } else {
            fprintf(stderr, "Read error at sample %d\n", i + 1);
        }
        usleep(100000);            // 100 ms
    }

    hcsr04_deinit(&handle);
    puts("Done.");
    return 0;
}
