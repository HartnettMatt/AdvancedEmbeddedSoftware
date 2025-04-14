#define GPIOD_API_VERSION 2
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE_OFFSET 15

int main(void) {
    struct gpiod_chip *chip;
    struct gpiod_line_request *req;
    struct gpiod_request_config *req_config;
    struct gpiod_line_config *line_config;
    struct gpiod_line_settings *line_settings;
    int ret;
    int current = 0;  // initial output value
    unsigned int offset = GPIO_LINE_OFFSET; // single line offset

    // Init chip, request, line config, and line settings
    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("Failed to init chip");
        return 1;
    }

    req_config = gpiod_request_config_new();
    if (!req_config) {
        perror("Failed to init request");
        return 1;
    }

    line_config = gpiod_line_config_new();
    if (!line_config) {
        perror("Failed to init line");
        return 1;
    }

    line_settings = gpiod_line_settings_new();
    if (!line_settings) {
        perror("Failed to init line settings");
        return 1;
    }

    // Configure line settings
    if(!gpiod_line_settings_set_direction(line_settings, GPIOD_LINE_DIRECTION_OUTPUT)){
        perror("Failed to set line output direction");
        return 1;
    }
    
    if(!gpiod_line_config_add_line_settings(line_config, &offsets, 1, line_settings)){
        perror("Failed to configure line settings");
        return 1;
    }

    req = gpiod_chip_request_lines(chip, req_config, line_config);
    if (!req) {
        perror("Failed to request line");
        gpiod_chip_close(chip);
        return 1;
    }

    while (1) {
        current = !current;
        ret = gpiod_line_request_set_value(req, offset, current);
        if (ret < 0) {
            perror("Failed to set line value");
            break;
        }
        printf("GPIO set to %d\n", current);
        sleep(1);
    }

    // Release the line request and close the chip.
    gpiod_line_settings_free(line_settings);
    gpiod_line_request_release(req);
    gpiod_line_config_free(line_config)
    gpiod_request_config_free(req_config);
    gpiod_chip_close(chip);
    return 0;
}
