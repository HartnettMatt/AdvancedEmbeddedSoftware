# Debug Notes

This is simply a file for me (Matt Hartnett) to collect all of my debugging information to ensure I don't lose anything or solve the same problem twice.


## Date: 4/12/2025
**Goal:** Finish sprint 1 by using GPIO to control motors
**Notes:**
* `gpiodetect` output: <br>
`gpiochip0 [pinctrl-bcm2835] (54 lines)` <br>
`gpiochip1 [brcmvirt-gpio] (2 lines)` <br>
`gpiochip2 [raspberrypi-exp-gpio] (8 lines)` <br>
* `gpioinfo --version` output:<br>
`gpioinfo (libgpiod) v2.21`
* `gpioinfo -c 0` output: <br>
`line 0: "ID_SDA" input` <br>
`line 1: "ID_SCL" input` <br>
`line 2: "ID_GPIO2" input` <br>
`line 3: "ID_GPIO3" input` <br>
...
* GPIO SUCCESS! Example commands that works as expected:
* `gpioset -t500ms GPIO2=1`: toggles GPIO2 every 500ms
* `gpioset -t500ms STATUS_LED=1`: toggles the status LED every 500ms
* `gpioget -c 0 2`: reads the value of GPIO2