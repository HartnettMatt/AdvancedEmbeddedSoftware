// Some of this code was copied from the libgpiod examples:
// https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/

#define GPIOD_API_VERSION 2
#define GPIO_CHIP "/dev/gpiochip0"
#define MOTOR_RIGHT_1_OFFSET 15
#define MOTOR_RIGHT_2_OFFSET 18
#define MOTOR_LEFT_1_OFFSET 23
#define MOTOR_LEFT_2_OFFSET 24
#include <errno.h>
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// This function does all of the setup necessary to control a GPIO line
// This function was copied from libgpiod
static struct gpiod_line_request *
request_output_line(const char *chip_path, unsigned int offset, enum gpiod_line_value value, const char *consumer) {
  struct gpiod_request_config *req_cfg = NULL;
  struct gpiod_line_request *request = NULL;
  struct gpiod_line_settings *settings;
  struct gpiod_line_config *line_cfg;
  struct gpiod_chip *chip;
  int ret;

  chip = gpiod_chip_open(chip_path);
  if (!chip)
    return NULL;

  settings = gpiod_line_settings_new();
  if (!settings)
    goto close_chip;

  gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
  gpiod_line_settings_set_output_value(settings, value);

  line_cfg = gpiod_line_config_new();
  if (!line_cfg)
    goto free_settings;

  ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
                                            settings);
  if (ret)
    goto free_line_config;

  if (consumer) {
    req_cfg = gpiod_request_config_new();
    if (!req_cfg)
      goto free_line_config;

    gpiod_request_config_set_consumer(req_cfg, consumer);
  }

  request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

  gpiod_request_config_free(req_cfg);

free_line_config:
  gpiod_line_config_free(line_cfg);

free_settings:
  gpiod_line_settings_free(settings);

close_chip:
  gpiod_chip_close(chip);

  return request;
}

// Currently just run through a power on self test of motor control forever
int main(void) {
  static const char *const chip_path = GPIO_CHIP;
  static const unsigned int mr1_line_offset = MOTOR_RIGHT_1_OFFSET;
  static const unsigned int mr2_line_offset = MOTOR_RIGHT_2_OFFSET;
  static const unsigned int ml1_line_offset = MOTOR_LEFT_1_OFFSET;
  static const unsigned int ml2_line_offset = MOTOR_LEFT_2_OFFSET;

  enum gpiod_line_value value = GPIOD_LINE_VALUE_INACTIVE;
  struct gpiod_line_request *mr1_request;
  struct gpiod_line_request *mr2_request;
  struct gpiod_line_request *ml1_request;
  struct gpiod_line_request *ml2_request;

  mr1_request = request_output_line(chip_path, mr1_line_offset, value,
                                    "motor-control");
  mr2_request = request_output_line(chip_path, mr2_line_offset, value,
                                    "motor-control");
  ml1_request = request_output_line(chip_path, ml1_line_offset, value,
                                    "motor-control");
  ml2_request = request_output_line(chip_path, ml2_line_offset, value,
                                    "motor-control");
  if (!mr1_request) {
    fprintf(stderr, "failed to request line mr1: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }
  if (!mr2_request) {
    fprintf(stderr, "failed to request line mr2: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }
  if (!ml1_request) {
    fprintf(stderr, "failed to request line ml1: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }
  if (!ml2_request) {
    fprintf(stderr, "failed to request line ml2: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }


  printf("BEGINNING POWER ON SELF TEST");
  sleep(1);
  printf("RIGHT MOTOR FORWARD");
  gpiod_line_request_set_value(mr1_request, mr1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(mr2_request, mr2_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  sleep(1);
  printf("RIGHT MOTOR BACKWARD");
  gpiod_line_request_set_value(mr1_request, mr1_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  gpiod_line_request_set_value(mr2_request, mr2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  sleep(1);
  printf("RIGHT MOTOR STOP");
  gpiod_line_request_set_value(mr1_request, mr1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(mr2_request, mr2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  sleep(1);
  printf("LEFT MOTOR FORWARD");
  gpiod_line_request_set_value(ml1_request, ml1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(ml2_request, ml2_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  sleep(1);
  printf("LEFT MOTOR BACKWARD");
  gpiod_line_request_set_value(ml1_request, ml1_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  gpiod_line_request_set_value(ml2_request, ml2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  sleep(1);
  printf("LEFT MOTOR STOP");
  gpiod_line_request_set_value(ml1_request, ml1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(ml2_request, ml2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  sleep(1);
  printf("BOTH MOTOR FORWARD");
  gpiod_line_request_set_value(mr1_request, mr1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(mr2_request, mr2_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  gpiod_line_request_set_value(ml1_request, ml1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(ml2_request, ml2_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  sleep(1);
  printf("BOTH MOTOR BACKWARD");
  gpiod_line_request_set_value(mr1_request, mr1_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  gpiod_line_request_set_value(mr2_request, mr2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(ml1_request, ml1_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  gpiod_line_request_set_value(ml2_request, ml2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  sleep(1);
  printf("BOTH MOTOR ALTERNATING 1");
  gpiod_line_request_set_value(mr1_request, mr1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(mr2_request, mr2_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  gpiod_line_request_set_value(ml1_request, ml1_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  gpiod_line_request_set_value(ml2_request, ml2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  sleep(1);
  printf("BOTH MOTOR ALTERNATING 2");
  gpiod_line_request_set_value(mr1_request, mr1_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  gpiod_line_request_set_value(mr2_request, mr2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(ml1_request, ml1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(ml2_request, ml2_line_offset, GPIOD_LINE_VALUE_ACTIVE);
  sleep(1);
  printf("BOTH MOTOR STOP");
  gpiod_line_request_set_value(mr1_request, mr1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(mr2_request, mr2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(ml1_request, ml1_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  gpiod_line_request_set_value(ml2_request, ml2_line_offset, GPIOD_LINE_VALUE_INACTIVE);
  sleep(1);

  

  gpiod_line_request_release(mr1_request);
  gpiod_line_request_release(mr2_request);
  gpiod_line_request_release(ml1_request);
  gpiod_line_request_release(ml2_request);

  return EXIT_SUCCESS;
}
