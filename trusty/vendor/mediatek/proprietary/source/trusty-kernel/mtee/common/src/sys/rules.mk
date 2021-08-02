#
# Copyright (c) 2015 MediaTek Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

LOCAL_DIR := $(GET_LOCAL_DIR)

# custom settings
MTK_TEE_CFG_FOLDER := $(ANDROIDMKROOT)/vendor/mediatek/proprietary/trustzone/custom/build/project
PRJ_CHIP := $(MTK_TEE_CFG_FOLDER)/$(PLATFORM).mk
PRJ_MF := $(MTK_TEE_CFG_FOLDER)/$(TARGET).mk

-include $(PRJ_CHIP)
-include $(PRJ_MF)

ifeq ($(PLATFORM), mt8176)
-include $(MTK_TEE_CFG_FOLDER)/mt8173.mk
endif

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += \
                $(LOCAL_DIR)/../../include

MODULE_SRCS := $(LOCAL_DIR)/debug.c \
               $(LOCAL_DIR)/memcfg.c \
               $(LOCAL_DIR)/mtee_mutex.c \
               $(LOCAL_DIR)/mtee_semaphore.c \
               $(LOCAL_DIR)/mtee_thread.c \
               $(LOCAL_DIR)/tz_utils.c \
               $(LOCAL_DIR)/malloc.c \
               $(LOCAL_DIR)/mtee_utils.c \
               $(LOCAL_DIR)/mtee_mem.c \

MODULE_CFLAGS += \
        -DMEMSIZE=$(MEMSIZE) \
        -DFBSIZE=$(MTEE_FBSIZE) \
        -DBOOTSHARE=$(MTEE_BOOTSHARE) \
        -DSECURE_FUNC_STACK_NUM=$(MTEE_SECURE_FUNC_STACK_NUM) \
        -DSECURE_FUNC_STACK_SIZE=$(MTEE_SECURE_FUNC_STACK_SIZE) \
        -DTZMEM_RELEASECM_SIZE=$(MTEE_TZMEM_RELEASECM_SIZE) \

MODULE_DEPS := prebuilt/trusty-kernel/mtee/$(PLATFORM)

include make/module.mk
