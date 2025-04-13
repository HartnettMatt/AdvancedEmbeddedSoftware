// Ensure the API version is set before any headers are processed:
#define GPIOD_API_VERSION 2
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE_OFFSET 17  // Using offset 17

int main(void) {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    struct gpiod_line_request_config config = {
        .consumer = "gpio_test",
        .request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT,
        .flags = 0
    };
    int ret, val = 0;

    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("Failed to open chip");
        return 1;
    }

    line = gpiod_chip_get_line(chip, GPIO_LINE_OFFSET);
    if (!line) {
        perror("Failed to get line");
        gpiod_chip_close(chip);
        return 1;
    }

    ret = gpiod_line_request(line, &config, 0);
    if (ret < 0) {
        perror("Failed to request line as output");
        gpiod_chip_close(chip);
        return 1;
    }

    while (1) {
        ret = gpiod_line_request_set_value(line, val);
        if (ret < 0) {
            perror("Failed to set line value");
            break;
        }
        printf("GPIO set to %d\n", val);
        val = !val;
        sleep(1);
    }

    gpiod_line_request_release(line);
    gpiod_chip_close(chip);
    return 0;
}
