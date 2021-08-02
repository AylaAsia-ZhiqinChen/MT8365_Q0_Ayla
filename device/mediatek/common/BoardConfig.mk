#this is platform common Boardconfig
ifneq ($(strip $(TARGET_COPY_OUT_ODM)),yes)
TARGET_COPY_OUT_ODM := vendor/odm
else
TARGET_COPY_OUT_ODM := odm
endif
ifneq ($(MTK_GLOBAL_C_INCLUDES)$(MTK_GLOBAL_CFLAGS)$(MTK_GLOBAL_CONLYFLAGS)$(MTK_GLOBAL_CPPFLAGS)$(MTK_GLOBAL_LDFLAGS),)
$(info *** MTK-specific global flags are changed)
$(info *** MTK_GLOBAL_C_INCLUDES: $(MTK_GLOBAL_C_INCLUDES))
$(info *** MTK_GLOBAL_CFLAGS: $(MTK_GLOBAL_CFLAGS))
$(info *** MTK_GLOBAL_CONLYFLAGS: $(MTK_GLOBAL_CONLYFLAGS))
$(info *** MTK_GLOBAL_CPPFLAGS: $(MTK_GLOBAL_CPPFLAGS))
$(info *** MTK_GLOBAL_LDFLAGS: $(MTK_GLOBAL_LDFLAGS))
$(error bailing...)
endif

MTK_GLOBAL_C_INCLUDES:=
MTK_GLOBAL_CFLAGS:=
MTK_GLOBAL_CONLYFLAGS:=
MTK_GLOBAL_CPPFLAGS:=
MTK_GLOBAL_LDFLAGS:=

# Use the non-open-source part, if present
-include vendor/mediatek/common/BoardConfigVendor.mk

# Use the connectivity Boardconfig
include device/mediatek/common/connectivity/BoardConfig.mk

# Use ago BoardConfig, if present
-include device/mediatek/common/ago/BoardConfig.mk

ifeq ($(PRODUCT_USE_DYNAMIC_PARTITIONS), true)
BOARD_BUILD_SUPER_IMAGE_BY_DEFAULT := true
endif

ifneq ($(strip $(SYSTEM_AS_ROOT)), no)
BOARD_BUILD_SYSTEM_ROOT_IMAGE ?= true
endif

# for flavor build base project assignment
ifeq ($(strip $(MTK_BASE_PROJECT)),)
  MTK_PROJECT_NAME := $(subst full_,,$(TARGET_PRODUCT))
else
  MTK_PROJECT_NAME := $(MTK_BASE_PROJECT)
endif
MTK_PROJECT := $(MTK_PROJECT_NAME)
MTK_PATH_SOURCE := vendor/mediatek/proprietary
MTK_ROOT := vendor/mediatek/proprietary
MTK_PATH_COMMON := vendor/mediatek/proprietary/custom/common
MTK_PATH_CUSTOM := vendor/mediatek/proprietary/custom/$(MTK_PROJECT)
MTK_PATH_CUSTOM_PLATFORM := vendor/mediatek/proprietary/custom/$(MTK_PLATFORM_DIR)
ifeq ($(strip $(MTK_K64_SUPPORT)),yes)
KERNEL_CROSS_COMPILE := $(PWD)/prebuilts/gcc/$(HOST_PREBUILT_TAG)/aarch64/aarch64-linux-gnu-6.3.1/bin/aarch64-linux-gnu-
else
KERNEL_CROSS_COMPILE := $(PWD)/prebuilts/gcc/$(HOST_PREBUILT_TAG)/arm/arm-linux-androideabi-4.9/bin/arm-linux-androideabi-
endif
TARGET_BOARD_KERNEL_HEADERS :=
ifneq ($(strip $(MTK_PLATFORM)),)
TARGET_BOARD_KERNEL_HEADERS += device/mediatek/$(MTK_PLATFORM_DIR)/kernel-headers
endif
TARGET_BOARD_KERNEL_HEADERS += device/mediatek/common/kernel-headers

MTK_GLOBAL_C_INCLUDES += $(TOPDIR)vendor/mediatek/proprietary/hardware/audio/common/include
MTK_GLOBAL_C_INCLUDES += $(MTK_PATH_CUSTOM)/cgen/cfgdefault $(MTK_PATH_CUSTOM)/cgen/cfgfileinc $(MTK_PATH_CUSTOM)/cgen/inc $(MTK_PATH_CUSTOM)/cgen
ifneq ($(strip $(MTK_PLATFORM)),)
MTK_GLOBAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/cgen/cfgdefault $(MTK_PATH_CUSTOM_PLATFORM)/cgen/cfgfileinc $(MTK_PATH_CUSTOM_PLATFORM)/cgen/inc $(MTK_PATH_CUSTOM_PLATFORM)/cgen
endif
MTK_GLOBAL_C_INCLUDES += $(MTK_PATH_COMMON)/cgen/cfgdefault $(MTK_PATH_COMMON)/cgen/cfgfileinc $(MTK_PATH_COMMON)/cgen/inc $(MTK_PATH_COMMON)/cgen

# Add MTK compile options to wrap MTK's modifications on AOSP.
ifneq ($(strip $(MTK_BASIC_PACKAGE)),yes)
ifneq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
  MTK_GLOBAL_CFLAGS += -DMTK_AOSP_ENHANCEMENT
endif
endif

# TODO: remove MTK_PATH_PLATFORM
MTK_PATH_PLATFORM := $(MTK_PATH_SOURCE)/platform/$(MTK_PLATFORM_DIR)
GOOGLE_RELEASE_RIL := no

ifeq ($(strip $(CUSTOM_BUILD_VERNO)),)
  CUSTOM_BUILD_VERNO_HDR := $(shell echo $(firstword $(BUILD_NUMBER)) | cut -b 1-15)
else
  CUSTOM_BUILD_VERNO_HDR := $(shell echo $(firstword $(CUSTOM_BUILD_VERNO)) | cut -b 1-15)
endif

#Enable HWUI by default
USE_OPENGL_RENDERER := true

#SELinux Policy File Configuration
ifeq ($(strip $(MTK_BASIC_PACKAGE)), yes)
BOARD_SEPOLICY_DIRS := \
        device/mediatek/sepolicy/basic/non_plat
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
BOARD_26.0_NONPLAT_FILE := \
        device/mediatek/sepolicy/basic/prebuilts/api/26.0/nonplat_sepolicy.cil
endif
ifeq ($(strip $(MTK_BSP_PACKAGE)), yes)
BOARD_SEPOLICY_DIRS := \
        device/mediatek/sepolicy/basic/non_plat \
        device/mediatek/sepolicy/bsp/non_plat
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
BOARD_26.0_NONPLAT_FILE := \
        device/mediatek/sepolicy/bsp/prebuilts/api/26.0/nonplat_sepolicy.cil
endif
ifneq ($(strip $(MTK_BASIC_PACKAGE)), yes)
ifneq ($(strip $(MTK_BSP_PACKAGE)), yes)
BOARD_SEPOLICY_DIRS := \
        device/mediatek/sepolicy/basic/non_plat \
        device/mediatek/sepolicy/bsp/non_plat \
        device/mediatek/sepolicy/full/non_plat
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
BOARD_26.0_NONPLAT_FILE := \
        device/mediatek/sepolicy/full/prebuilts/api/26.0/nonplat_sepolicy.cil
endif
endif

BOARD_SEPOLICY_DIRS += $(wildcard device/mediatek/sepolicy/secure)

#widevine data migration for OTA upgrade from O to P
ifneq ($(call math_lt,$(PRODUCT_SHIPPING_API_LEVEL),28),)
BOARD_SEPOLICY_DIRS += $(wildcard device/mediatek/sepolicy/bsp/ota_upgrade)
endif


ifneq ($(MTK_BUILD_IGNORE_IMS_REPO),yes)
ifdef CUSTOM_MODEM
  ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
    MTK_MODEM_MODULE_MAKEFILES := $(foreach item,$(CUSTOM_MODEM),$(firstword $(wildcard vendor/mediatek/proprietary/modem/$(patsubst %_prod,%,$(item))/Android.mk vendor/mediatek/proprietary/modem/$(item)/Android.mk)))
  else
    MTK_MODEM_MODULE_MAKEFILES := $(foreach item,$(CUSTOM_MODEM),$(firstword $(wildcard vendor/mediatek/proprietary/modem/$(patsubst %_prod,%,$(item))_prod/Android.mk vendor/mediatek/proprietary/modem/$(item)/Android.mk)))
  endif
  MTK_MODEM_APPS_SEPOLICY_DIRS :=
  $(foreach f,$(MTK_MODEM_MODULE_MAKEFILES),\
    $(if $(strip $(MTK_MODEM_APPS_SEPOLICY_DIRS)),,\
      $(eval MTK_MODEM_APPS_SEPOLICY_DIRS := $(wildcard $(patsubst %/Android.mk,%/sepolicy/q0,$(f))))\
    )\
  )
BOARD_SEPOLICY_DIRS += $(MTK_MODEM_APPS_SEPOLICY_DIRS)
endif
endif

# Include an expanded selection of fonts
EXTENDED_FONT_FOOTPRINT := true

# To disable AOSP rild
ENABLE_VENDOR_RIL_SERVICE := true

ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
     BOARD_MTK_GMO_SYSTEM_SIZE_KB := 1400832
     BOARD_MTK_GMO_VENDOR_SIZE_KB := 417792
     BOARD_MTK_GMO_CACHE_SIZE_KB  := 114688
endif

# add default super partition size here, will be overwritten by partition_size.mk
BOARD_SUPER_PARTITION_SIZE := 6442450944

# ptgen
PERL := /usr/bin/perl
# Add MTK's MTK_PTGEN_OUT definitions
ifeq (,$(strip $(OUT_DIR)))
  ifeq (,$(strip $(OUT_DIR_COMMON_BASE)))
    MTK_PTGEN_OUT_DIR := $(TOPDIR)out
  else
    MTK_PTGEN_OUT_DIR := $(OUT_DIR_COMMON_BASE)/$(notdir $(PWD))
  endif
else
    MTK_PTGEN_OUT_DIR := $(strip $(OUT_DIR))
endif
# Change flavor progect to base project
MTK_PTGEN_PRODUCT_OUT := $(MTK_PTGEN_OUT_DIR)/target/product/$(TARGET_DEVICE)

ifneq ($(CALLED_FROM_SETUP),true)
ifneq ($(strip $(MTK_TARGET_PROJECT)), $(strip $(TARGET_DEVICE)))
$(shell mkdir -p $(OUT_DIR)/target/product && ln -sfn $(TARGET_DEVICE) $(OUT_DIR)/target/product/$(MTK_TARGET_PROJECT))
endif
endif

MTK_PTGEN_OUT := $(MTK_PTGEN_PRODUCT_OUT)/obj/PTGEN
MTK_PTGEN_MK_OUT := $(MTK_PTGEN_PRODUCT_OUT)/obj/PTGEN
MTK_PTGEN_TMP_OUT := $(MTK_PTGEN_PRODUCT_OUT)/obj/PTGEN_TMP

TARGET_CUSTOM_OUT := $(MTK_PTGEN_PRODUCT_OUT)/custom

#Add MTK's Recovery fstab definitions
TARGET_RECOVERY_FSTAB := $(MTK_PTGEN_PRODUCT_OUT)/system/vendor/etc/fstab.$(MTK_PLATFORM_DIR)

# Define MTK Recovery updater
ifneq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
    TARGET_RECOVERY_UPDATER_LIBS := librecovery_updater_mtk
    TARGET_RECOVERY_UPDATER_EXTRA_LIBS := libpartition
endif

ifeq ($(strip $(MTK_BSP_PACKAGE)), yes)
  # Define MTK Recovery UI
    MTK_RECOVERY_MEDIUM_RES := yes
    TARGET_RECOVERY_UI_LIB := librecovery_ui_mtk
endif

# Define MTK ota and secure boot tool extension
TARGET_RELEASETOOLS_EXTENSIONS := vendor/mediatek/proprietary/scripts/releasetools
SECURITY_SIG_TOOL := vendor/mediatek/proprietary/scripts/sign-image/sign_image.sh
SECURITY_IMAGE_PATH := vendor/mediatek/proprietary/custom/$(MTK_PLATFORM_DIR)/security/cert_config/img_list.txt

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

ALLOW_MISSING_DEPENDENCIES ?= true
BUILD_BROKEN_DUP_RULES := true
#BUILD_BROKEN_PHONY_TARGETS := true
#BUILD_BROKEN_ANDROIDMK_EXPORTS := true
#BUILD_BROKEN_ENG_DEBUG_TAGS := true
BUILD_BROKEN_USES_NETWORK := true

DEVICE_MANIFEST_FILE += device/mediatek/$(MTK_PLATFORM_DIR)/manifest.xml
DEVICE_MATRIX_FILE := device/mediatek/$(MTK_PLATFORM_DIR)/compatibility_matrix.xml

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
lk

# Install odex files into the other system image
BOARD_USES_SYSTEM_OTHER_ODEX := true

# A/B OTA dexopt update_engine hookup
AB_OTA_POSTINSTALL_CONFIG += \
    RUN_POSTINSTALL_system=true \
    POSTINSTALL_PATH_system=system/bin/otapreopt_script \
    FILESYSTEM_TYPE_system=ext4 \
    POSTINSTALL_OPTIONAL_system=true
endif

#Add MTK's hook
-include device/mediatek/build/core/base_rule_hook.mk
-include vendor/mediatek/build/core/base_rule_hook.mk
-include vendor/mediatek/build/core/base_rule_jack.mk
-include vendor/mediatek/build/core/rpgen.mk
