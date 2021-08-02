#
# Copyright (c) 2013-2018, Google, Inc. All rights reserved
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

EXTRA_LINKER_SCRIPTS += $(LOCAL_DIR)/trusty_apps.ld

MODULE := $(LOCAL_DIR)

MODULE_SRCS := \
	$(LOCAL_DIR)/trusty.c \
	$(LOCAL_DIR)/trusty_app.c \
	$(LOCAL_DIR)/syscall.c \
	$(LOCAL_DIR)/handle.c \
	$(LOCAL_DIR)/handle_set.c \
	$(LOCAL_DIR)/uctx.c \
	$(LOCAL_DIR)/ipc.c \
	$(LOCAL_DIR)/ipc_msg.c \
	$(LOCAL_DIR)/iovec.c \
	$(LOCAL_DIR)/uuid.c

ifeq (true,$(call TOBOOL,$(WITH_TRUSTY_IPC)))
GLOBAL_DEFINES += WITH_TRUSTY_IPC=1

# check if we want to disable wait_any support
ifeq (false, $(call TOBOOL,$(WITH_NO_WAIT_ANY_SUPPORT)))
GLOBAL_DEFINES += WITH_WAIT_ANY_SUPPORT=1
endif

MODULE_SRCS += \
	$(LOCAL_DIR)/vqueue.c \
	$(LOCAL_DIR)/smcall.c \
	$(LOCAL_DIR)/trusty_virtio.c \
	$(LOCAL_DIR)/tipc_dev.c \
	$(LOCAL_DIR)/tipc_dev_ql.c

ifneq (true,$(call TOBOOL,$(WITH_CUSTOM_TRUSTY_IPC_CONFIG)))
MODULE_SRCS += \
	$(LOCAL_DIR)/tipc_config.c
endif
endif

GLOBAL_INCLUDES += \
	$(LOCAL_DIR)/include \

MODULE_DEPS += \
	lib/syscall \
	lib/version \

ifeq ($(call TOBOOL,$(CLANGBUILD)), true)
MODULE_DEPS += lib/rt
endif

GLOBAL_DEFINES += \
	WITH_SYSCALL_TABLE=1 \

include make/module.mk
