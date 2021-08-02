# Use the non-open-source part, if present
-include vendor/mediatek/aiot8365p2_bsp/BoardConfigVendor.mk

# Use the 8168 common part
include device/mediatek/mt8168/BoardConfig.mk

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(MTK_TARGET_PROJECT_FOLDER)/bluetooth

# Android Q new feature UDC
BOARD_ROOT_EXTRA_FOLDERS += metadata
BOARD_USES_METADATA_PARTITION := true

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

# fosmod_hdd_support begin
# limit fsck_msdos memory usage to 400MB
TARGET_FSCK_MSDOS_MEMORY_LIMITATION = 409600
# fosmod_hdd_support end

#A-GO
MALLOC_SVELTE := true
BOARD_MTK_SYSTEM_SIZE_KB :=2621440
BOARD_MTK_VENDOR_SIZE_KB :=409600
BOARD_MTK_CACHE_SIZE_KB :=114688

ifeq ($(MTK_WLAN_SUPPORT), yes)
ifeq ($(MTK_COMBO_CHIP), MT7663)
WIFI_DRIVER_FW_PATH_PARAM :=
WIFI_DRIVER_FW_PATH_STA:=
WIFI_DRIVER_FW_PATH_AP:=
WIFI_DRIVER_FW_PATH_P2P:=
WIFI_DRIVER_STATE_CTRL_PARAM :=
WIFI_DRIVER_STATE_ON :=
WIFI_DRIVER_STATE_OFF :=
WIFI_DRIVER_MODULE_PATH := /vendor/lib/modules/wlan_drv_gen4_mt7663.ko
WIFI_DRIVER_MODULE_NAME := wlan_drv_gen4_mt7663
endif
endif

-include device/mediatek/build/core/soong_config.mk
