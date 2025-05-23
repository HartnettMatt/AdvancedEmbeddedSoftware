###############################################################################
# Makefile for "hcsr04_test"
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
CFLAGS  += -Wall -Werror
# Include /usr/include for hcsr04 library
LIBS    += -lgpiod -ldriver_hcsr04 -pthread -D_GNU_SOURCE -I$(STAGING_DIR)/usr/include

# The target application and its object files
SRCS := hcsr04_test.c hcsr04_port.c
OBJS := $(SRCS:.c=.o)

TARGET := hcsr04_test

###############################################################################
# Default target: builds the blink_gpio application
###############################################################################
all: $(TARGET)

###############################################################################
# Rules to build the target application
###############################################################################
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

###############################################################################
# Clean target: remove build artifacts
###############################################################################
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
