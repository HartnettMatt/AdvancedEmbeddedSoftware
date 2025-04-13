###############################################################################
# Makefile for "gpio_test"
#
# Usage:
#  make                                (build for native)
#  make clean                          (remove object files and the "blink_gpio" binary)
#  make CROSS_COMPILE=arm-linux-gnueabihf- (build for Raspberry Pi cross-compile)
#
# Author: Matt Hartnett
###############################################################################

# If CROSS_COMPILE is not passed in, it defaults to empty (native build)
CROSS_COMPILE ?=

# The compiler and linker commands
CC      := $(CROSS_COMPILE)gcc
CFLAGS  := -Wall -Werror $(shell pkg-config --cflags libgpiod) -DGPIOD_API_VERSION=2
LDFLAGS := $(shell pkg-config --libs libgpiod)

# The target application and its object files
TARGET  := gpio_test
OBJS    := gpio_test.o

###############################################################################
# Default target: builds the blink_gpio application
###############################################################################
all: $(TARGET)

###############################################################################
# Rules to build the target application
###############################################################################
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

###############################################################################
# Clean target: remove build artifacts
###############################################################################
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
