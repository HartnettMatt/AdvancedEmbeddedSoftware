#include <gpiod.h>
#include <unistd.h>
#include <stdio.h>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE 17  // GPIO17

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int val = 0;
    int ret;

    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("Open chip failed");
        return 1;
    }

    line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (!line) {
        perror("Get line failed");
        gpiod_chip_close(chip);
        return 1;
    }

    // Explicitly call the version 2 function, providing a flags parameter (set to 0).
    ret = gpiod_line_request_output_flags(line, "blink_gpio", 0, 0);
    if (ret < 0) {
        perror("Request line as output failed");
        gpiod_chip_close(chip);
        return 1;
    }

    while (1) {
        ret = gpiod_line_set_value(line, val);
        if (ret < 0) {
            perror("Set line value failed");
            break;
        }
        printf("GPIO set to %d\n", val);
        val = !val;
        sleep(1);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}
