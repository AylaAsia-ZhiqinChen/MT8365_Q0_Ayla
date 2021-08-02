###########################################################
## Generic definitions
###########################################################
# Remove $@ if error occurs
.DELETE_ON_ERROR:

# Turn off suffix build rules built into make
.SUFFIXES:

.PHONY: FORCE
FORCE:

SHELL          := /bin/bash
.DEFAULT_GOAL  := all

TINYSYS_ADSP   := tinysys-adsp
TINYSYS_LOADER := tinysys-loader
TINYSYS_NAME   := $(TINYSYS_ADSP)

###########################################################
## Parameter control
###########################################################
PROJECT := $(strip $(PROJECT))
ifeq ($(PROJECT),)
  $(error $(TINYSYS_NAME): project name is required)
endif

# Verbosity control
# Add 'V=1' with make or 'showcommands' when building Android
V ?= 1
ifeq ($(V),1)
hide :=
else
hide := @
endif

###########################################################
## Common directory locations and generic variables
###########################################################
#SOURCE_DIR         := $(shell pwd)/..
SOURCE_DIR         := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))/..

ifeq ($(strip $(O)),)
O := $(SOURCE_DIR)/out
endif
BUILT_DIR          := $(O)/$(PROJECT)

DRIVERS_COMMON_DIR := $(SOURCE_DIR)/drivers
BUILD_DIR          := $(SOURCE_DIR)/build
TOOLS_DIR          := $(SOURCE_DIR)/tools
RTOS_SRC_DIR       := $(SOURCE_DIR)/kernel/FreeRTOS/Source
RTOS_SRC_PLUS_DIR  := $(SOURCE_DIR)/kernel/FreeRTOS-Plus/Source
BASE_DIR           := $(SOURCE_DIR)/project
#DO_SERVICE_DIR     := $(SOURCE_DIR)/middleware/DoService
TINYSYS_PACK_BIN   := $(O)/hifixdsp.bin

MKIMAGE            := $(TOOLS_DIR)/mkimage
OBJSIZE            := $(TOOLS_DIR)/objsize
DUMPBIN            := $(TOOLS_DIR)/dumpbin
MCHECK             := $(TOOLS_DIR)/memoryReport.py

###########################################################
## Generic build flow
###########################################################
# Common functions and utilities
include $(BUILD_DIR)/definitions.mk

# Initialize the environment for each processor
include $(BUILD_DIR)/main.mk

ifeq (1,$(V))
  $(info $(TINYSYS_NAME): PROCESSORS=$(PROCESSORS))
  $(info $(TINYSYS_NAME): PROJECT=$(PROJECT))
  $(info $(TINYSYS_NAME): PLATFORM=$(PLATFORM))
  $(info $(TINYSYS_NAME): O=$(O))
  $(info $(TINYSYS_NAME): CONFIG_DIR=$(CONFIG_DIR))
  $(info $(TINYSYS_NAME): SOURCE_DIR=$(SOURCE_DIR))
  $(info $(TINYSYS_NAME): ALL_ADSP_BINS=$(ALL_ADSP_BINS))
endif

###########################################################
## Build targets
###########################################################
TINYSYS_ADSP_BIN := $(ALL_ADSP_BINS)
all: adsp
adsp: $(TINYSYS_ADSP_BIN)
pack: $(TINYSYS_PACK_BIN)

ifneq (,$(TINYSYS_PACK_BIN))
$(TINYSYS_PACK_BIN): $(TINYSYS_ADSP_BIN)
	@mkdir -p $(dir $@)
	$(warning input parameters : $(NUM_LOAD_BINS) $(O) $(BUILT_DIR))
else
$(TINYSYS_PACK_BIN): ;
endif

clean:
	rm -rf $(TINYSYS_ADSP_BIN) $(O)
