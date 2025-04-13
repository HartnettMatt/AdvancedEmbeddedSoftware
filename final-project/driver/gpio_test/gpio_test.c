#define GPIOD_API_VERSION 2
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE_OFFSET 17

int main(void) {
    struct gpiod_chip *chip;
    struct gpiod_line_bulk bulk;
    int offsets[1] = { GPIO_LINE_OFFSET };
    int ret;
    int current = 0;
    int val_array[1];

    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("Failed to open chip");
        return 1;
    }

    ret = gpiod_chip_get_lines(chip, offsets, 1, &bulk);
    if (ret < 0) {
        perror("Failed to get line bulk");
        gpiod_chip_close(chip);
        return 1;
    }

    ret = gpiod_line_request_bulk_output(&bulk, "gpio_test", &current);
    if (ret < 0) {
        perror("Failed to request bulk output");
        gpiod_chip_close(chip);
        return 1;
    }

    while (1) {
        current = !current;
        val_array[0] = current;
        ret = gpiod_line_set_value_bulk(&bulk, val_array);
        if (ret < 0) {
            perror("Failed to set line bulk value");
            break;
        }
        printf("GPIO set to %d\n", current);
        sleep(1);
    }

    // Release the lines and close the chip.
    gpiod_line_release_bulk(&bulk);
    gpiod_chip_close(chip);
    return 0;
}
