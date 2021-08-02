# Use the non-open-source part, if present
-include vendor/mediatek/aiot8362am3_bsp/BoardConfigVendor.mk

# Use the 8167 common part
include device/mediatek/mt8167/BoardConfig.mk

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(MTK_PROJECT_FOLDER)/bluetooth

#W1923 Android Q new feature UDC
BOARD_ROOT_EXTRA_FOLDERS += metadata
BOARD_USES_METADATA_PARTITION := true

#Config partition size
-include $(MTK_PTGEN_OUT)/partition_size.mk
ifneq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
endif
BOARD_FLASH_BLOCK_SIZE := 4096

#Config data partition for F2FS
BOARD_USERDATAIMAGE_FILE_SYSTEM_TYPE := f2fs

# This ensures the needed build tools are available.
# TODO: make non-linux builds happy with external/f2fs-tool; system/extras/f2fs_utils
ifeq ($(HOST_OS),linux)
TARGET_USERIMAGES_USE_F2FS := true
endif

MTK_INTERNAL_CDEFS := $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),$(foreach v,$(shell echo $($(t)) | tr '[a-z]' '[A-Z]'),-D$(v))))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)=\"$(strip $($(t)))\"))

MTK_GLOBAL_CFLAGS += $(MTK_INTERNAL_CDEFS)

ifneq ($(MTK_K64_SUPPORT), yes)
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,32N2
else
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,64N2
endif

MTK_PTGEN_CHIP := MT8167

# Enable product partition
ifeq ($(strip $(MTK_DYNAMIC_PARTITION_SUPPORT)),yes)
TARGET_COPY_OUT_PRODUCT := product
BOARD_PRODUCTIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_MAIN_PARTITION_LIST := vendor product system
endif

# partition
ifeq ($(strip $(MTK_DYNAMIC_PARTITION_SUPPORT)),yes)
define get-partition-size
$(shell device/mediatek/build/build/tools/partition/get-partition-size.py device/mediatek/mt8167/partition/partition_dynamic.xml $(1) $(2))
endef
else
define get-partition-size
$(shell device/mediatek/build/build/tools/partition/get-partition-size.py device/mediatek/mt8167/partition/partition.xml $(1) $(2))
endef
endif

BOARD_MAIN_SIZE := $(call get-partition-size,super)
BOARD_SUPER_PARTITION_SIZE := $(call get-partition-size,super)
BOARD_SYSTEMIMAGE_PARTITION_SIZE := $(call get-partition-size,system)
BOARD_VENDORIMAGE_PARTITION_SIZE := $(call get-partition-size,vendor)
BOARD_USERDATAIMAGE_PARTITION_SIZE := $(call get-partition-size,userdata)
BOARD_BOOTIMAGE_PARTITION_SIZE := $(call get-partition-size,boot)
BOARD_DTBOIMG_PARTITION_SIZE := $(call get-partition-size,dtbo)
BOARD_CACHEIMAGE_PARTITION_SIZE := $(call get-partition-size,cache)
BOARD_RECOVERYIMAGE_PARTITION_SIZE := $(call get-partition-size,recovery)

#BOARD_MTK_SYSTEM_SIZE_KB :=2359296
BOARD_MTK_SYSTEM_SIZE_KB :=1257472
BOARD_MTK_VENDOR_SIZE_KB :=409600
BOARD_MTK_CACHE_SIZE_KB :=114688

ifeq ($(strip $(MTK_WLAN_SUPPORT)),yes)
ifeq ($(strip $(MTK_COMBO_CHIP)),MT7668)
WIFI_DRIVER_FW_PATH_PARAM :=
WIFI_DRIVER_FW_PATH_STA:=
WIFI_DRIVER_FW_PATH_AP:=
WIFI_DRIVER_FW_PATH_P2P:=
WIFI_DRIVER_STATE_CTRL_PARAM :=
WIFI_DRIVER_STATE_ON :=
WIFI_DRIVER_STATE_OFF :=
WIFI_DRIVER_MODULE_PATH := /vendor/lib/modules/wlan_drv_gen4_mt7668.ko
WIFI_DRIVER_MODULE_NAME := wlan_drv_gen4_mt7668
WIFI_HIDL_FEATURE_DISABLE_AP := true
endif
endif

-include device/mediatek/build/core/soong_config.mk
