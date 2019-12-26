# Name: Makefile_STM
# Author: Daniel Nery Silva de Oliveira
# ThundeRatz Robotics Team
# 06/2019

# Cube file name without .ioc extension
PROJECT_NAME = vl53l1
VERSION := 1

TARGET_BOARD := target_$(PROJECT_NAME)_$(VERSION)

DEVICE_FAMILY  := STM32F3xx
DEVICE_TYPE    := STM32F303xx
DEVICE_DEF     := STM32F303xC
DEVICE         := STM32F303RC

# Linker script file without .ld extension
# Find it on cube folder after code generation
DEVICE_LD_FILE := STM32F303RCTx_FLASH

# Lib dir
LIB_DIR  := lib

# Cube Directory
CUBE_DIR := cube

# Config Files Directory
CFG_DIR := cfg

# Default values, can be set on the command line or here
DEBUG   ?= 1
VERBOSE ?= 0