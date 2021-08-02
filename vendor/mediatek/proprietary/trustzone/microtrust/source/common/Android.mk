# Copyright (c) 2015-2017 MICROTRUST Incorporated
# All rights reserved
#
# This file and software is confidential and proprietary to MICROTRUST Inc.
# Unauthorized copying of this file and software is strictly prohibited.
# You MUST NOT disclose this file and software unless you get a license
# agreement from MICROTRUST Incorporated.

ifeq ($(MICROTRUST_TEE_SUPPORT), yes)
LOCAL_PATH := $(call my-dir)

MICROTRUST_TEE_VERSION ?= 300
$(info "common\Anroid.mk:MICROTRUST_TEE_VERSION=$(MICROTRUST_TEE_VERSION)")

ALL_MICROTRUST_MK = $(call all-makefiles-under, $(LOCAL_PATH)/$(MICROTRUST_TEE_VERSION))
ALL_MICROTRUST_MK += $(LOCAL_PATH)/interfaces/Android.mk

$(info "ALL_MICROTRUST_MK=$(ALL_MICROTRUST_MK)")

include $(ALL_MICROTRUST_MK)

endif
