# mt8167 platform boardconfig

# Use the common part
include device/mediatek/vendor/common/BoardConfig.mk

# Use the non-open-source part, if present
-include vendor/mediatek/mt8167/BoardConfigVendor.mk

ifneq ($(TARGET_SUPPORTS_64_BIT_APPS), true)
TARGET_ARCH := arm
TARGET_CPU_VARIANT := cortex-a53
TARGET_2ND_CPU_VARIANT := cortex-a53
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH_VARIANT := armv8-a
else
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=
TARGET_CPU_VARIANT := cortex-a53
TARGET_2ND_CPU_VARIANT := cortex-a53
TARGET_CPU_SMP := true
TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv8-a
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi

TARGET_IS_64_BIT := true

endif

ifeq ($(MTK_K64_SUPPORT), yes)
KERNEL_TARGET_ARCH:= arm64
KERNEL_TARGET_STRIP:= $(KERNEL_CROSS_COMPILE)strip
else
KERNEL_TARGET_ARCH:= arm
KERNEL_TARGET_STRIP:= $(KERNEL_CROSS_COMPILE)strip
endif

# disable system as root by platform
# will move it to common/device.mk after all platform ready
BOARD_BUILD_SYSTEM_ROOT_IMAGE := false

ARCH_ARM_HAVE_TLS_REGISTER := true
TARGET_BOARD_PLATFORM := mt8167
TARGET_USERIMAGES_USE_EXT4 := true
TARGET_NO_FACTORYIMAGE:=true

# MTK, Nick Ko, 20140305, Add Display {
TARGET_FORCE_HWC_FOR_VIRTUAL_DISPLAYS := true
NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3
TARGET_RUNNING_WITHOUT_SYNC_FRAMEWORK := false
# temporarily avoid ALPS01799495 failed issue in BSP load
ifneq ($(strip $(MTK_BASIC_PACKAGE)),yes)
    VSYNC_EVENT_PHASE_OFFSET_NS := -5000000
    SF_VSYNC_EVENT_PHASE_OFFSET_NS := -5000000
    #PRESENT_TIME_OFFSET_FROM_VSYNC_NS := 0
else
    #VSYNC_EVENT_PHASE_OFFSET_NS := 0
    #SF_VSYNC_EVENT_PHASE_OFFSET_NS := 0
    #PRESENT_TIME_OFFSET_FROM_VSYNC_NS := 0
endif
MTK_HWC_SUPPORT := yes
MTK_HWC_VERSION := 2.0.0
# MTK, Nick Ko, 20140305, Add Display }

ifneq (,$(filter $(strip $(MTK_LCM_PHYSICAL_ROTATION)), 90 270))
    SF_PRIMARY_DISPLAY_ORIENTATION := $(MTK_LCM_PHYSICAL_ROTATION)
endif

TARGET_USES_HWC2 := true
TARGET_USES_HWC2ON1ADAPTER := false

BOARD_CONNECTIVITY_VENDOR := MediaTek
BOARD_USES_MTK_AUDIO := true

ifeq ($(MTK_GPS_SUPPORT), yes)
  BOARD_GPS_LIBRARIES := true
else
  BOARD_GPS_LIBRARIES := false
endif

ifeq ($(strip $(BOARD_CONNECTIVITY_VENDOR)), MediaTek)
BOARD_MEDIATEK_USES_GPS := true
endif

ifeq ($(MTK_WLAN_SUPPORT), yes)
BOARD_WLAN_DEVICE := MediaTek
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_mt66xx
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_mt66xx
WIFI_DRIVER_FW_PATH_PARAM := /dev/wmtWifi
WIFI_DRIVER_FW_PATH_STA:=STA
WIFI_DRIVER_FW_PATH_AP:=AP
WIFI_DRIVER_FW_PATH_P2P:=P2P
WIFI_DRIVER_STATE_CTRL_PARAM := /dev/wmtWifi
WIFI_DRIVER_STATE_ON := 1
WIFI_DRIVER_STATE_OFF := 0
ifneq ($(strip $(MTK_BSP_PACKAGE)),yes)
MTK_WIFI_CHINESE_SSID := yes
endif
ifeq ($(strip $(MTK_BSP_PACKAGE)),yes)
MTK_WIFI_GET_IMSI_FROM_PROPERTY := yes
endif
endif

# mkbootimg header, which is used in LK
BOARD_KERNEL_BASE = 0x40000000
ifneq ($(MTK_K64_SUPPORT), yes)
BOARD_KERNEL_OFFSET = 0x00008000
else
BOARD_KERNEL_OFFSET = 0x00080000
endif
BOARD_RAMDISK_OFFSET = 0x15000000
BOARD_TAGS_OFFSET = 0x14000000
TARGET_USES_64_BIT_BINDER := true
ifneq ($(MTK_K64_SUPPORT), yes)
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,32N2
else
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,64N2
endif
BOARD_MKBOOTIMG_ARGS := --kernel_offset $(BOARD_KERNEL_OFFSET) --ramdisk_offset $(BOARD_RAMDISK_OFFSET) --tags_offset $(BOARD_TAGS_OFFSET)
BOARD_MKBOOTIMG_ARGS += --header_version 2
BOARD_MKBOOTIMG_ARGS += --dtb_offset $(BOARD_TAGS_OFFSET)
BOARD_INCLUDE_DTB_IN_BOOTIMG := true
BOARD_PREBUILT_DTBIMAGE_DIR := $(MTK_PTGEN_PRODUCT_OUT)/obj/PACKAGING/dtb

ifeq ($(strip $(MTK_DTBO_UPGRADE_FROM_ANDROID_O)), no)
ifeq ($(strip $(MTK_DTBO_FEATURE)),yes)
BOARD_INCLUDE_RECOVERY_DTBO := true
BOARD_PREBUILT_DTBOIMAGE := $(MTK_PTGEN_PRODUCT_OUT)/obj/PACKAGING/dtboimage/dtbo.img
endif
endif

# partition

partition_file = $(notdir $(wildcard device/mediatek/mt8167/partition/partition_dynamic.xml))
ifeq ($(strip $(partition_file)), partition_dynamic.xml)
define get-partition-size
$(shell device/mediatek/build/build/tools/partition/get-partition-size.py device/mediatek/mt8167/partition/partition_dynamic.xml $(1) $(2))
endef
BOARD_SUPER_PARTITION_SIZE := $(call get-partition-size,super)
BOARD_SYSTEMIMAGE_PARTITION_SIZE := $(call get-partition-size,system)
BOARD_VENDORIMAGE_PARTITION_SIZE := $(call get-partition-size,vendor)
BOARD_USERDATAIMAGE_PARTITION_SIZE := $(call get-partition-size,userdata)
BOARD_BOOTIMAGE_PARTITION_SIZE := $(call get-partition-size,boot)
BOARD_DTBOIMG_PARTITION_SIZE := $(call get-partition-size,dtbo)
BOARD_CACHEIMAGE_PARTITION_SIZE := $(call get-partition-size,cache)
BOARD_RECOVERYIMAGE_PARTITION_SIZE := $(call get-partition-size,recovery)
endif

pathmap_INCL += trustzone:$(MTK_PATH_SOURCE)/trustzone/mtee/source/common/include
pathmap_INCL += trustzone-uree:$(MTK_PATH_SOURCE)/external/trustzone/mtee/include

#SELinux Policy File Configuration
ifeq ($(strip $(MTK_BASIC_PACKAGE)), yes)
BOARD_SEPOLICY_DIRS += \
        device/mediatek/mt8167/sepolicy/basic
endif
ifeq ($(strip $(MTK_BSP_PACKAGE)), yes)
BOARD_SEPOLICY_DIRS += \
        device/mediatek/mt8167/sepolicy/basic \
        device/mediatek/mt8167/sepolicy/bsp
endif
ifneq ($(strip $(MTK_BASIC_PACKAGE)), yes)
ifneq ($(strip $(MTK_BSP_PACKAGE)), yes)
BOARD_SEPOLICY_DIRS += \
        device/mediatek/mt8167/sepolicy/basic \
        device/mediatek/mt8167/sepolicy/bsp \
        device/mediatek/mt8167/sepolicy/full
endif
endif

# IMG GPU
MTK_GPU_VERSION ?= rgx clark 1.10ED

# Create vendor partition
TARGET_COPY_OUT_VENDOR := vendor
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
TARGET_RECOVERY_FSTAB := $(MTK_PTGEN_PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/etc/fstab.$(MTK_PLATFORM_DIR)



ifneq ($(strip $(MTK_COMBO_NAND_SUPPORT)), yes)
BOARD_BUILD_SYSTEM_ROOT_IMAGE = no
BOARD_MAIN_SIZE := $(call get-partition-size,super)
BOARD_SUPER_PARTITION_GROUPS := main
endif

# add product to vbmeta_system partitions
ifeq ($(PRODUCT_USE_DYNAMIC_PARTITIONS), true)
BOARD_AVB_VBMETA_SYSTEM := system product
endif

# ODM image
ifneq ($(filter $(MAKECMDGOALS),custom_images),)
ifeq (yes,$(strip $(TARGET_COPY_OUT_ODM)))
PRODUCT_CUSTOM_IMAGE_MAKEFILES += device/mediatek/mt8167/odm/odm.mk
endif
endif

# config.fs
TARGET_FS_CONFIG_GEN += device/mediatek/mt8167/config.fs

-include device/mediatek/build/core/target_brm_platform.mk
TARGET_KERNEL_USE_CLANG ?= true
