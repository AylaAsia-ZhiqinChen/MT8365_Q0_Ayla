BOARD_CONNECTIVITY_VENDOR := MediaTek
BOARD_USES_MTK_AUDIO := true
BOARD_VNDK_VERSION := current

ifneq ($(strip $(MTK_BOARD_VENDOR_COMMON_DEFINED)),yes)
  MTK_PROJECT_NAME := $(SYS_BASE_PROJECT)
  MTK_PATH_SOURCE := vendor/mediatek/proprietary
  MTK_ROOT := vendor/mediatek/proprietary
  MTK_PATH_COMMON := vendor/mediatek/proprietary/custom/common
  MTK_PATH_CUSTOM := .
  #MTK_PATH_CUSTOM_PLATFORM := vendor/mediatek/proprietary/custom/$(call to-lower,$(MTK_PLATFORM))
  TARGET_BOARD_KERNEL_HEADERS :=
  TARGET_BOARD_KERNEL_HEADERS += device/mediatek/common/kernel-headers


  # TODO: remove MTK_PATH_PLATFORM
  #MTK_PATH_PLATFORM := $(MTK_PATH_SOURCE)/platform/$(call to-lower,$(MTK_PLATFORM))
endif

#SELinux Policy File Configuration
ifeq ($(strip $(MTK_BASIC_PACKAGE)), yes)
BOARD_PLAT_PUBLIC_SEPOLICY_DIR := \
        device/mediatek/sepolicy/basic/plat_public
BOARD_PLAT_PRIVATE_SEPOLICY_DIR := \
        device/mediatek/sepolicy/basic/plat_private
BOARD_PREBUILTS_BASIC_PUBLIC_PLAT_DIRS := \
        device/mediatek/sepolicy/basic/prebuilts/api/26.0/plat_public
BOARD_PREBUILTS_BASIC_PRIVATE_PLAT_DIRS := \
        device/mediatek/sepolicy/basic/prebuilts/api/26.0/plat_private
BOARD_COMPAT_MAPPING_CIL_DIRS := \
        device/mediatek/sepolicy/basic/private/compat/26.0/26.0.cil
BOARD_COMPAT_MAPPING_IGNORE_CIL_DIRS := \
        device/mediatek/sepolicy/basic/private/compat/26.0/26.0.ignore.cil
endif
ifeq ($(strip $(MTK_BSP_PACKAGE)), yes)
BOARD_PLAT_PUBLIC_SEPOLICY_DIR := \
        device/mediatek/sepolicy/basic/plat_public \
        device/mediatek/sepolicy/bsp/plat_public
BOARD_PLAT_PRIVATE_SEPOLICY_DIR := \
        device/mediatek/sepolicy/basic/plat_private \
        device/mediatek/sepolicy/bsp/plat_private
BOARD_PREBUILTS_BSP_PUBLIC_PLAT_DIRS := \
        device/mediatek/sepolicy/basic/prebuilts/api/26.0/plat_public \
        device/mediatek/sepolicy/bsp/prebuilts/api/26.0/plat_public
BOARD_PREBUILTS_BSP_PRIVATE_PLAT_DIRS := \
        device/mediatek/sepolicy/basic/prebuilts/api/26.0/plat_private \
        device/mediatek/sepolicy/bsp/prebuilts/api/26.0/plat_private
BOARD_COMPAT_MAPPING_CIL_DIRS := \
        device/mediatek/sepolicy/bsp/private/compat/26.0/26.0.cil
BOARD_COMPAT_MAPPING_IGNORE_CIL_DIRS := \
        device/mediatek/sepolicy/bsp/private/compat/26.0/26.0.ignore.cil
endif
ifneq ($(strip $(MTK_BASIC_PACKAGE)), yes)
ifneq ($(strip $(MTK_BSP_PACKAGE)), yes)
BOARD_PLAT_PUBLIC_SEPOLICY_DIR := \
        device/mediatek/sepolicy/basic/plat_public \
        device/mediatek/sepolicy/bsp/plat_public \
        device/mediatek/sepolicy/full/plat_public
BOARD_PLAT_PRIVATE_SEPOLICY_DIR := \
        device/mediatek/sepolicy/basic/plat_private \
        device/mediatek/sepolicy/bsp/plat_private \
        device/mediatek/sepolicy/full/plat_private
BOARD_PREBUILTS_FULL_PUBLIC_PLAT_DIRS := \
        device/mediatek/sepolicy/basic/prebuilts/api/26.0/plat_public \
        device/mediatek/sepolicy/bsp/prebuilts/api/26.0/plat_public \
        device/mediatek/sepolicy/full/prebuilts/api/26.0/plat_public
BOARD_PREBUILTS_FULL_PRIVATE_PLAT_DIRS := \
        device/mediatek/sepolicy/basic/prebuilts/api/26.0/plat_private \
        device/mediatek/sepolicy/bsp/prebuilts/api/26.0/plat_private \
        device/mediatek/sepolicy/full/prebuilts/api/26.0/plat_private
BOARD_COMPAT_MAPPING_CIL_DIRS := \
        device/mediatek/sepolicy/full/private/compat/26.0/26.0.cil
BOARD_COMPAT_MAPPING_IGNORE_CIL_DIRS := \
        device/mediatek/sepolicy/full/private/compat/26.0/26.0.ignore.cil
endif
endif

# Define MTK ota and secure boot tool extension
#TARGET_RELEASETOOLS_EXTENSIONS := vendor/mediatek/proprietary/scripts/releasetools
#SECURITY_SIG_TOOL := vendor/mediatek/proprietary/scripts/sign-image/sign_image.sh
#SECURITY_IMAGE_PATH := vendor/mediatek/proprietary/custom/$(MTK_PLATFORM_DIR)/security/cert_config/img_list.txt

ALLOW_MISSING_DEPENDENCIES := true
BUILD_BROKEN_DUP_RULES := true
#BUILD_BROKEN_PHONY_TARGETS := true
#BUILD_BROKEN_ANDROIDMK_EXPORTS := true
#BUILD_BROKEN_ENG_DEBUG_TAGS := true
BUILD_BROKEN_USES_NETWORK := true
SKIP_BOOT_JARS_CHECK := true

#ifneq ($(strip $(SYSTEM_AS_ROOT)), no)
#BOARD_BUILD_SYSTEM_ROOT_IMAGE ?= true
#endif

ifeq ($(PRODUCT_USE_DYNAMIC_PARTITIONS), true)
# add default super partition size here, will be overwritten by partition_size.mk
BOARD_SUPER_PARTITION_SIZE := 6442450944
BOARD_BUILD_SUPER_IMAGE_BY_DEFAULT := true
endif

# Add MTK compile options to wrap MTK's modifications on AOSP.
ifneq ($(strip $(MTK_BOARD_CONFIG_AOSP_ENH_DEFINED)),yes)
  ifneq ($(strip $(MTK_BASIC_PACKAGE)),yes)
      MTK_GLOBAL_CFLAGS += -DMTK_AOSP_ENHANCEMENT
    endif
endif

ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
     BOARD_MTK_GMO_SYSTEM_SIZE_KB := 1400832
endif

ifeq ($(BUILD_GMS),yes)
  ifeq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
    DONT_DEXPREOPT_PREBUILTS := false
  else
    ifeq ($(strip $(YUNTIAN_DEXPREOPT_PREBUILTS)), yes)
      DONT_DEXPREOPT_PREBUILTS := false
    else
      DONT_DEXPREOPT_PREBUILTS := true
    endif
  endif
else
  ifeq ($(TARGET_BUILD_VARIANT),userdebug)
    DEX_PREOPT_DEFAULT := nostripping
  endif
endif

ifeq (yes,$(BUILD_MTK_LDVT))
MTK_RELEASE_GATEKEEPER := no
endif

ifneq ($(wildcard vendor/mediatek/internal/system/core/init),)
  ifneq ($(strip $(MTK_BASIC_PACKAGE)), yes)
    ifeq ($(strip $(TARGET_BUILD_VARIANT)),user)
      ifeq ($(strip $(SYSTEM_AS_ROOT)),yes)
        BOARD_ROOT_EXTRA_FOLDERS += eng
      endif
    endif
  endif
endif

# A/B System updates
ifeq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
BOARD_USES_RECOVERY_AS_BOOT := true
TARGET_NO_RECOVERY := true
AB_OTA_UPDATER := true

 # A/B OTA partitions
AB_OTA_PARTITIONS := \
boot \
system \
lk \
preloader

# Install odex files into the other system image
BOARD_USES_SYSTEM_OTHER_ODEX := true

# A/B OTA dexopt update_engine hookup
AB_OTA_POSTINSTALL_CONFIG += \
    RUN_POSTINSTALL_system=true \
    POSTINSTALL_PATH_system=system/bin/otapreopt_script \
    FILESYSTEM_TYPE_system=ext4 \
    POSTINSTALL_OPTIONAL_system=true
endif

ifeq ($(strip $(MTK_BSP_PACKAGE)), yes)
    #Define MTK Recovery UI
    MTK_RECOVERY_MEDIUM_RES := yes
endif

#settings for main vbmeta
BOARD_AVB_ENABLE ?= true

ifneq ($(strip $(BOARD_AVB_ENABLE)), true)
    # if avb2.0 is not enabled
    #$(call inherit-product, build/target/product/verity.mk)
else
    BOARD_AVB_ALGORITHM ?= SHA256_RSA2048
    BOARD_AVB_KEY_PATH ?= device/mediatek/system/common/oem_prvk.pem
    BOARD_AVB_ROLLBACK_INDEX ?= 0

    ifeq ($(PRODUCT_USE_DYNAMIC_PARTITIONS), true)
        BOARD_AVB_VBMETA_SYSTEM := system
        BOARD_AVB_VBMETA_SYSTEM_KEY_PATH := device/mediatek/system/common/system_prvk.pem
        BOARD_AVB_VBMETA_SYSTEM_ALGORITHM := SHA256_RSA2048
        BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX := 0
        BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX_LOCATION := 2
    else
        #settings for system, which is configured as chain partition
        BOARD_AVB_SYSTEM_KEY_PATH := device/mediatek/system/common/system_prvk.pem
        BOARD_AVB_SYSTEM_ALGORITHM := SHA256_RSA2048
        BOARD_AVB_SYSTEM_ROLLBACK_INDEX := 0
        BOARD_AVB_SYSTEM_ROLLBACK_INDEX_LOCATION := 2
    endif

    BOARD_AVB_SYSTEM_ADD_HASHTREE_FOOTER_ARGS := --hash_algorithm sha256
    BOARD_AVB_PRODUCT_ADD_HASHTREE_FOOTER_ARGS := --hash_algorithm sha256
endif

TARGET_USES_64_BIT_BINDER := true

TARGET_SYSTEM_PROP += device/mediatek/system/common/system.prop

#Add MTK's hook
ifndef MTK_TARGET_PROJECT
-include device/mediatek/build/core/base_rule_hook.mk
-include vendor/mediatek/build/core/base_rule_hook.mk
-include vendor/mediatek/build/core/rpgen.mk
endif

# Use the connectivity Boardconfig
include device/mediatek/system/common/connectivity/BoardConfig.mk

