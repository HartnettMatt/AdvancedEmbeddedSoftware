#!/bin/bash
#Script to clean buildroot
#Author: Matt Hartnett

# Change to the buildroot directory
cd ./buildroot || { echo "Failed to change directory to ./buildroot"; exit 1; }

# Run make distclean
make distclean