#define GPIOD_API_VERSION 2
#include <gpiod.h>
#include <unistd.h>
#include <stdio.h>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE 17  // Use GPIO17

int main(void) {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    struct gpiod_line_request *req;
    int ret, val = 0;

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

    req = gpiod_line_request_output_flags(line, "gpio_test", 0, 0);
    if (!req) {
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

    gpiod_line_request_release(req);
    gpiod_chip_close(chip);

    return 0;
}
