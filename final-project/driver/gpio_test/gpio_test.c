#define GPIOD_API_VERSION 2
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE_OFFSET 15

int main(void) {
    struct gpiod_chip *chip;
    struct gpiod_line_request *req;
    int ret;
    int current = 0;  // initial output value
    unsigned int offset = GPIO_LINE_OFFSET; // single line offset

    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("Failed to open chip");
        return 1;
    }

    req = gpiod_chip_request_lines(chip, "gpio_test", &offset, 1, current);
    if (!req) {
        perror("Failed to request line");
        gpiod_chip_close(chip);
        return 1;
    }

    while (1) {
        current = !current;
        ret = gpiod_line_request_set_value(req, 0, current);
        if (ret < 0) {
            perror("Failed to set line value");
            break;
        }
        printf("GPIO set to %d\n", current);
        sleep(1);
    }

    // Release the line request and close the chip.
    gpiod_line_request_release(req);
    gpiod_chip_close(chip);
    return 0;
}
