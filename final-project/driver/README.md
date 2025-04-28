# Final Project Driver
Whiteboard Wiper Robot Driver
=============================

Overview
--------
This firmware drives a robot that wipes a whiteboard. On power‑up the program starts automatically, calibrates its ultrasonic distance sensor to the wall in front of it, and then enters a loop in which the robot moves forward until it detects the edge of the wall, reverses a short distance, rotates 180 °, and continues in the opposite direction. The loop runs until the program receives SIGINT (Ctrl+C).

Key Features
------------
* Deterministic real‑time behaviour via POSIX SCHED_FIFO scheduler and CPU core pinning.
* Self‑calibration of the HC‑SR04 ultrasonic distance sensor to accommodate different starting positions.
* Closed‑loop control that maintains distance to the wall within *WALL_RANGE* centimetres.
* Safe signal handling for graceful shutdown.
* GPIO access implemented with libgpiod for portable control of motors and sensor pins.

Build
-----
The code targets Buildroot and requires:
* libgpiod ≥ 2.0
* pthreads (usually provided by glibc)

Run
---
```bash
./whiteboard_wiper
```

Program Flow
------------
1. **System setup**
   * Elevate to SCHED_FIFO priority 80.
   * Pin to CPU 0 to reduce context‑switch jitter.
2. **Hardware initialisation**
   * `motor_init()` configures the motor driver GPIOs.
   * `init_hcsr04()` configures the trigger and echo lines of the HC‑SR04 sensor.
3. **Calibration**
   * Sample the distance sensor `CAL_CYCLES` times, average the result, and store it as *wall_dist_m*.
4. **Motion loop**
   * Drive forward.
   * Every 100 ms read the current distance.
   * If |distance – wall_dist_m| > *WALL_RANGE*, execute:
     1. Stop.
     2. Reverse for `REVERSE_TIME`.
     3. Turn clockwise for `TURNAROUND_TIME`.
     4. Resume forward motion.
5. **Shutdown**
   * On SIGINT or any error, stop the motors, release GPIO lines, and exit.

Configuration
-------------
Timing constants (`CAL_CYCLES`, `WALL_RANGE`, `REVERSE_TIME`, `TURNAROUND_TIME`) and GPIO pin assignments are defined in the header files. Adjust them to match your hardware.
