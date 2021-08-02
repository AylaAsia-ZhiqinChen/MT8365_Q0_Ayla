LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)
MODULE_USER := true

MODULE_SRCS := \
	$(LOCAL_DIR)/bio.c \
	$(LOCAL_DIR)/rand.c \
	$(LOCAL_DIR)/sscanf.c \
	$(LOCAL_DIR)/time.c \

MODULE_DEPS := \
	lib/rng

include make/module.mk
