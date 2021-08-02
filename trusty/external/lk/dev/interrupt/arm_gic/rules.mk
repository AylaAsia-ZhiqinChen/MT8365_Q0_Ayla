LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

GLOBAL_INCLUDES += \
	$(LOCAL_DIR)/include

ifeq (1, $(ARM_GIC_V3_SUPPORT))
MODULE_SRCS += \
	$(LOCAL_DIR)/arm_gic_v3.c
else
MODULE_SRCS += \
	$(LOCAL_DIR)/arm_gic.c
endif

include make/module.mk
