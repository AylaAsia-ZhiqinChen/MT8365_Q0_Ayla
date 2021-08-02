# Use the non-open-source part, if present
-include vendor/mediatek/m3520_pos/BoardConfigVendor.mk

# Use the 8168 common part
include device/mediatek/mt8168/BoardConfig.mk

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(MTK_TARGET_PROJECT_FOLDER)/bluetooth

#Config partition size
-include $(MTK_PTGEN_OUT)/partition_size.mk
ifneq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
endif
BOARD_FLASH_BLOCK_SIZE := 4096

MTK_INTERNAL_CDEFS := $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),$(foreach v,$(shell echo $($(t)) | tr '[a-z]' '[A-Z]'),-D$(v))))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)=\"$(strip $($(t)))\"))

MTK_GLOBAL_CFLAGS += $(MTK_INTERNAL_CDEFS)

ifneq ($(MTK_K64_SUPPORT), yes)
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,32N2
else
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,64N2
endif

#A-GO
MALLOC_SVELTE := true
BOARD_MTK_SYSTEM_SIZE_KB :=2621440
BOARD_MTK_VENDOR_SIZE_KB :=409600
BOARD_MTK_CACHE_SIZE_KB :=114688

-include device/mediatek/build/core/soong_config.mk
