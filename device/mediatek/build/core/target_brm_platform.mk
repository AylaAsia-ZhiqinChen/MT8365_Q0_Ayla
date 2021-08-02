# TARGET_BRM_PLATFORM determines which platform-dependent binaries are used

ifeq (,$(TARGET_BRM_PLATFORM))

ifneq (,$(filter mt6753 mt6737t,$(TARGET_BOARD_PLATFORM)))
  TARGET_BRM_PLATFORM := mt6735
else ifneq (,$(filter mt6737m,$(TARGET_BOARD_PLATFORM)))
  TARGET_BRM_PLATFORM := mt6735m
else ifneq (,$(filter mt6750,$(TARGET_BOARD_PLATFORM)))
  TARGET_BRM_PLATFORM := mt6755
else
  TARGET_BRM_PLATFORM := $(TARGET_BOARD_PLATFORM)
endif

endif
