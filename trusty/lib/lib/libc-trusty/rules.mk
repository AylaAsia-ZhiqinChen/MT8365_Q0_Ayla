LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

WITH_CUSTOM_MALLOC := true
WITHOUT_CONSOLE := true

GLOBAL_INCLUDES := $(LOCAL_DIR)/include $(LKROOT)/include $(GLOBAL_INCLUDES)

MODULE_SRCS := \
	$(LOCAL_DIR)/abort.c \
	$(LOCAL_DIR)/assert.c \
	$(LOCAL_DIR)/atexit.c \
	$(LOCAL_DIR)/exit.c \
	$(LOCAL_DIR)/ipc.c \
	$(LOCAL_DIR)/malloc.c \
	$(LOCAL_DIR)/mman.c \
	$(LOCAL_DIR)/stdio.c \
	$(LOCAL_DIR)/time.c \
	$(LOCAL_DIR)/libc_init.c \
	$(LOCAL_DIR)/libc_fatal.c \

include $(LOCAL_DIR)/arch/$(ARCH)/rules.mk

# dlmalloc does arithmatic on null pointers to calculate padding.
MODULE_COMPILEFLAGS += -Wno-null-pointer-arithmetic

MODULE_DEPS := \
	lib/libc \

ifeq ($(call TOBOOL,$(CLANGBUILD)), true)
MODULE_DEPS += lib/rt
endif

include make/module.mk
