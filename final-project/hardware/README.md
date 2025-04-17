# Final Project Hardware

Here is all of the hardware for the whiteboard wiper robot.

### Directory Structure:

hardware/ <br>
├── 3D_Printing # 3D Printing files and designs <br>
│ ├── *.FCStd # Part's Free CAD file, used for editing <br>
│ ├── *.stl # Part's STL export, imported into slicer <br>
│ └── *.gcode # Part's sliced file (specific to Matt's 3D printer, DO NOT USE ELSEWHERE) <br>
├── Spice # Circuit simulations<br>
│ ├── *.asc # Design's LTSpice file, used for editing <br>
│ ├── *.log # Design's log file <br>
│ └── *.raw # Design's raw file <br>
├── Docs # Important documentation<br>
│ ├── HCS04.pdf # Specifications for distance sensor <br>
│ ├── MJE200_D-1811381.pdf # Specifications for NPN <br>
│ └── MJE210_D-2316039.pdf # Specifications for PNP <br>
├── raspberryPiPinout.png # Pinout
image, [Source](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)<br>
└── README.md # Project documentation (this file) <br>

## Raspberry Pi Pinout:

![Raspberry Pi Pinout](./raspberryPiPinout.png "Raspberry Pi Pinout")

## Important pins:

| Hardware Pin | Pin Label | Net Name    | Connection         |
|--------------|-----------|-------------|--------------------|
| 4            | 5V Power  | VCC         | Power cable red    |
| 6            | Ground    | GND         | Power cable black  |
| 10           | GPIO 15   | RM_1        | Right motor orange |
| 12           | GPIO 18   | RM_2        | Right motor green  |
| 16           | GPIO 23   | LM_1        | Left motor blue    |
| 18           | GPIO 24   | LM_2        | Left motor yellow  |
| 3            | GPIO 2    | HCSR04_TRIG | HCSR04 (#TBD)      |
| 5            | GPIO 3    | HCSR04_ECHO | HCSR04 (#TBD)      |

### Notes:

* If motor mount v2:
    * Tighten tolerances around hex holes
    * Add 3rd mount point for PCB
    * Add indent for motor screws