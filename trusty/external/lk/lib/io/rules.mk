LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := \

ifeq (false,$(call TOBOOL,$(WITHOUT_CONSOLE)))
MODULE_DEPS += lib/cbuf
MODULE_SRCS += $(LOCAL_DIR)/console.c
endif

MODULE_SRCS += \
   $(LOCAL_DIR)/io.c \

include make/module.mk
