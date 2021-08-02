# Copyright (C) 2018 MICROTRUST Incorporated
# All rights reserved
#
# This file and software is confidential and proprietary to MICROTRUST Inc.
# Unauthorized copying of this file and software is strictly prohibited.
# You MUST NOT disclose this file and software unless you get a license
# agreement from MICROTRUST Incorporated.

TARGET = 91dba524a6e24909acc48abd7163121a.ta

SRC_C = \
drv_entry.c \
drv_i2c_gpio_api.c \
manifest.c \

MY_CFLAGS = $(CFLAGS_SYS_IRQ) $(CFLAGS_SYS_IO) $(CFLAGS_PF_UT_DRV)

MY_CFLAGS += -Iinclude
MY_CFLAGS += -Iprivate
MY_CFLAGS += -I$(UT_SDK_HOME)/teei_test/include

MY_LDFLAGS = $(LDFLAGS_SYS_IRQ) $(LDFLAGS_SYS_IO) $(LDFLAGS_PF_UT_DRV)

MODE = shared

include $(UT_SDK_HOME)/build/prog.mk
