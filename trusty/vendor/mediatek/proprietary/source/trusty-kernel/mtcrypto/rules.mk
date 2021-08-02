LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

ifeq ($(CFG_CRYPTO_MODULE),HACC)
	MODULE_CFLAGS += -DCRYPTO_MODULE_HACC
endif

MODULE_SRCS := \
	$(LOCAL_DIR)/mtcrypto.c \
	$(LOCAL_DIR)/mtcrypto_hwrng.c \
	$(LOCAL_DIR)/mtcrypto_hwkey.c \

MODULE_DEPS := \
	lib/trusty \
	interface/hwrng \
	interface/hwkey \

include make/module.mk
