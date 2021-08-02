LOCAL_PATH := $(call my-dir)

ifneq ($(TARGET_DEVICE), gobo)

ifeq ($(is_sdk_build),true)
$(shell if [ ! -e $(HOST_OUT)/sdk/$(TARGET_PRODUCT)/sdk_deps.mk ]; then mkdir -p $(HOST_OUT)/sdk/$(TARGET_PRODUCT) && echo "-include device/mediatek/build/core/sdk_deps.mk" > $(HOST_OUT)/sdk/$(TARGET_PRODUCT)/sdk_deps.mk; fi)
endif

ifdef TARGET_2ND_ARCH

define mtk-add-vndk-lib
include $$(CLEAR_VARS)
LOCAL_MODULE := $(1)$(2)
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PREBUILT_MODULE_FILE := $$(call intermediates-dir-for,SHARED_LIBRARIES,$(1))/$(1).so
LOCAL_MULTILIB := 64
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := $(1).so
LOCAL_MODULE_SUFFIX := .so
LOCAL_VENDOR_MODULE := $(3)
LOCAL_MODULE_RELATIVE_PATH := $(4)
include $$(BUILD_PREBUILT)

include $$(CLEAR_VARS)
LOCAL_MODULE := $(1)$(2)
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PREBUILT_MODULE_FILE := $$(call intermediates-dir-for,SHARED_LIBRARIES,$(1),,,2ND_)/$(1).so
LOCAL_MULTILIB := 32
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := $(1).so
LOCAL_MODULE_SUFFIX := .so
LOCAL_VENDOR_MODULE := $(3)
LOCAL_MODULE_RELATIVE_PATH := $(4)
include $$(BUILD_PREBUILT)

ALL_MODULES.$(1).REQUIRED_FROM_TARGET := $$(strip $$(ALL_MODULES.$(1).REQUIRED_FROM_TARGET) $(1)$(2))
ALL_MODULES.$(1)$$(TARGET_2ND_ARCH_MODULE_SUFFIX).REQUIRED_FROM_TARGET := $$(strip $$(ALL_MODULES.$(1)$$(TARGET_2ND_ARCH_MODULE_SUFFIX).REQUIRED_FROM_TARGET) $(1)$(2))
TARGET_DEPENDENCIES_ON_SHARED_LIBRARIES := $$(filter-out $(1)$(2):%,$$(TARGET_DEPENDENCIES_ON_SHARED_LIBRARIES))
2ND_TARGET_DEPENDENCIES_ON_SHARED_LIBRARIES := $$(filter-out $(1)$(2)$$(TARGET_2ND_ARCH_MODULE_SUFFIX):%,$$(2ND_TARGET_DEPENDENCIES_ON_SHARED_LIBRARIES))
endef

else

define mtk-add-vndk-lib
include $$(CLEAR_VARS)
LOCAL_MODULE := $(1)$(2)
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PREBUILT_MODULE_FILE := $$(call intermediates-dir-for,SHARED_LIBRARIES,$(1))/$(1).so
LOCAL_MULTILIB := 32
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := $(1).so
LOCAL_MODULE_SUFFIX := .so
LOCAL_VENDOR_MODULE := $(3)
LOCAL_MODULE_RELATIVE_PATH := $(4)
include $$(BUILD_PREBUILT)

ALL_MODULES.$(1).REQUIRED_FROM_TARGET := $$(strip $$(ALL_MODULES.$(1).REQUIRED_FROM_TARGET) $(1)$(2))
TARGET_DEPENDENCIES_ON_SHARED_LIBRARIES := $$(filter-out $(1)$(2):%,$$(TARGET_DEPENDENCIES_ON_SHARED_LIBRARIES))
endef

endif

# clone from system/lib to vendor/lib
AOSP_VNDK_EXT_LIBRARIES :=

# clone from system/lib to system/lib/vndk-sp

ifneq ($(BUILD_QEMU_IMAGES),true)

ifndef BOARD_VNDK_VERSION
AOSP_VNDK_SP_LIBRARIES := \
    android.hardware.graphics.allocator@2.0 \
    android.hardware.graphics.mapper@2.0 \
    android.hardware.graphics.common@1.0 \
    android.hardware.renderscript@1.0 \
    android.hidl.base@1.0 \
    android.hidl.memory@1.0 \
    libRSCpuRef \
    libRSDriver \
    libRS_internal \
    libbacktrace \
    libbase \
    libbcinfo \
    libblas \
    libc++ \
    libcompiler_rt \
    libcutils \
    libft2 \
    libhardware \
    libhidlbase \
    libhidlmemory \
    libhidltransport \
    libhwbinder \
    libion \
    liblzma \
    libpng \
    libunwind \
    libutils \

endif

else

AOSP_VNDK_SP_LIBRARIES ?=

endif


# clone from vendor/lib to system/lib
MTK_VNDK_EXT_LIBRARIES := \


#
$(foreach lib,$(AOSP_VNDK_EXT_LIBRARIES),$(eval $(call mtk-add-vndk-lib,$(lib),_vnd,true)))
$(foreach lib,$(AOSP_VNDK_SP_LIBRARIES),$(eval $(call mtk-add-vndk-lib,$(lib),.vndk-sp,,vndk-sp)))
$(foreach lib,$(MTK_VNDK_EXT_LIBRARIES),$(eval $(call mtk-add-vndk-lib,$(lib),_fwk)))
endif


# hard code from add_img_to_target_files.py
ADD_IMG_TO_TARGET_FILES_IMAGES := \
  boot.img \
  recovery.img \
  system.img \
  vendor.img \
  product.img \
  product_services.img \
  odm.img \
  system_other.img \
  userdata.img \
  cache.img \
  dtbo.img \
  vbmeta_system.img \
  vbmeta_vendor.img \
  vbmeta.img \
  super_empty.img \
  super.img

ADD_IMG_TO_TARGET_FILES_IMAGES += \
  boot-debug.img \
  mntl.img

# hard code from sign-image/SignTool.pl
MTK_VERIFIED_BOOT_V1_ORIGINAL_IMAGES := lk.img logo.bin secro.img odmdtbo.img dtbo.img
MTK_VERIFIED_BOOT_V2_ORIGINAL_IMAGES :=
MTK_SECURE_DOWNLOAD_ORIGINAL_IMAGES := system.img system_other.img userdata.img efuse.img vendor.img odm.img vbmeta.img vbmeta_system.img vbmeta_vendor.img super.img
ifneq ($(BOARD_AVB_ENABLE),true)
  MTK_VERIFIED_BOOT_V1_ORIGINAL_IMAGES += boot.img recovery.img
else
  MTK_SECURE_DOWNLOAD_ORIGINAL_IMAGES += boot.img recovery.img
endif
MTK_SECURE_DOWNLOAD_ORIGINAL_IMAGES += trustzone.bin mobicore.bin tz.img tee.img
MTK_SECURE_DOWNLOAD_ORIGINAL_IMAGES += MBR EBR1 EBR2
MTK_SECURE_DOWNLOAD_ORIGINAL_IMAGES += cache.img custom.img
MTK_SECURE_DOWNLOAD_ORIGINAL_IMAGES += $(MTK_VERIFIED_BOOT_V1_ORIGINAL_IMAGES)

MTK_PTGEN_OTA_SCATTER_PERL := device/mediatek/build/build/tools/ptgen/ota_scatter.pl
MTK_PTGEN_OTA_UPDATE_PYTHON := device/mediatek/build/build/tools/ptgen/ota_update_list.py

# x.y -> x-verified.y; needed by FlashTool and OTA
MTK_VERIFIED_BOOT_CURRENT_IMAGES :=
# x.y or x-verified.y -> x-sign.y; needed by FlashTool
MTK_SECURE_DOWNLOAD_CURRENT_IMAGES :=
# x.y; needed by OTA
MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES :=
# x.y; needed by FlashTool, not needed by OTA
MTK_NOT_UPGRADABLE_CURRENT_IMAGES :=
#
MTK_SCATTER_ORIGINAL_IMAGES :=
MTK_SCATTER_FILE_NAME :=
MTK_SPLIT_VENDOR_BOARD_PACK_RADIOIMAGES :=
MTK_SPLIT_VENDOR_INSTALLED_RADIOIMAGE_TARGET :=
MTK_SPLIT_VENDOR_AB_OTA_PARTITIONS :=
MTK_SPLIT_VENDOR_OTA_TXT_TARGET :=


ifdef MTK_PLATFORM_DIR

ifdef MTK_PTGEN_CHIP
  MTK_SCATTER_FILE_NAME := $(PRODUCT_OUT)/$(MTK_PTGEN_CHIP)_Android_scatter.txt
  # MSI build will not run ptgen
  ifneq ($(wildcard $(MTK_SCATTER_FILE_NAME)),)
    MTK_SCATTER_ORIGINAL_IMAGES := $(sort $(filter-out file_name: NONE,$(shell cat $(MTK_SCATTER_FILE_NAME) | grep "file_name:")))
  else
    include device/mediatek/$(TARGET_BOARD_PLATFORM)/partition/Android.mk
  endif
endif

IMAGE_LIST_TXT := $(wildcard vendor/mediatek/proprietary/custom/$(MTK_PLATFORM_DIR)/security/cert_config/img_list.txt)
ifneq (,$(IMAGE_LIST_TXT))
  IMAGE_LIST := $(shell sed -n '/=/ p' $(IMAGE_LIST_TXT))
  MTK_VERIFIED_BOOT_V2_ORIGINAL_IMAGES := $(foreach IMAGE_PAIR,$(IMAGE_LIST),$(firstword $(subst =, ,$(IMAGE_PAIR))))
  ifeq ($(BOARD_AVB_ENABLE),true)
    MTK_VERIFIED_BOOT_V2_ORIGINAL_IMAGES := $(filter-out boot.img recovery.img,$(MTK_VERIFIED_BOOT_V2_ORIGINAL_IMAGES))
  endif
  MTK_VERIFIED_BOOT_CURRENT_IMAGES := $(sort $(filter $(MTK_SCATTER_ORIGINAL_IMAGES),$(MTK_VERIFIED_BOOT_V2_ORIGINAL_IMAGES)))
  MTK_SECURE_DOWNLOAD_CURRENT_IMAGES :=
else
  MTK_VERIFIED_BOOT_CURRENT_IMAGES := $(sort $(filter $(MTK_SCATTER_ORIGINAL_IMAGES),$(MTK_VERIFIED_BOOT_V1_ORIGINAL_IMAGES)))
  MTK_SECURE_DOWNLOAD_CURRENT_IMAGES := $(sort $(filter $(MTK_SCATTER_ORIGINAL_IMAGES),$(MTK_SECURE_DOWNLOAD_ORIGINAL_IMAGES)))
endif

ifdef PRELOADER_TARGET_PRODUCT
  MTK_NOT_UPGRADABLE_CURRENT_IMAGES += preloader_$(PRELOADER_TARGET_PRODUCT).bin
  ifeq ($(BUILD_PRELOADER),yes)
    ifeq (yes,$(BOARD_BUILD_SBOOT_DIS_PL))
      MTK_NOT_UPGRADABLE_CURRENT_IMAGES += preloader_$(PRELOADER_TARGET_PRODUCT)_SBOOT_DIS.bin
    endif
    ifeq (yes,$(BOARD_BUILD_RPMB_KEY_PL))
      MTK_NOT_UPGRADABLE_CURRENT_IMAGES += preloader_$(PRELOADER_TARGET_PRODUCT)_RPMB_KEY.bin
    endif
  endif
  ifeq ($(AB_OTA_UPDATER),true)
    ifeq ($(MTK_EMMC_SUPPORT).$(MTK_UFS_SUPPORT),yes.yes)
      MTK_SPLIT_VENDOR_AB_OTA_PARTITIONS += preloader_ufs preloader_emmc
    else
      MTK_SPLIT_VENDOR_AB_OTA_PARTITIONS += preloader
    endif
  else
    # rule from build_preloader.mk
    ifeq ($(MTK_EMMC_SUPPORT),yes)
      ifneq ($(wildcard $(PRELOADER_EMMC_HEADER_BLOCK)),)
        MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES += preloader_emmc.img
      else
        MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES += preloader.img
      endif
    endif
    ifeq ($(MTK_UFS_SUPPORT),yes)
      ifneq ($(wildcard $(PRELOADER_UFS_HEADER_BLOCK)),)
        MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES += preloader_ufs.img
      else
        MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES += preloader.img
      endif
    endif
  endif
endif
ifdef INSTALLED_SECROIMAGE_TARGET
  IMAGE_NAME := $(notdir $(INSTALLED_SECROIMAGE_TARGET))
  ifneq ($(filter $(MTK_SCATTER_ORIGINAL_IMAGES),$(IMAGE_NAME)),)
    ifneq ($(filter $(MTK_VERIFIED_BOOT_CURRENT_IMAGES),$(IMAGE_NAME)),)
      # V1: secro.img -> secro-verified.img -> vnd.target_files.zip
    else
      # V2: secro.img -> vnd.dist_files.zip
      MTK_NOT_UPGRADABLE_CURRENT_IMAGES += $(IMAGE_NAME)
    endif
  endif
endif
ifeq ($(AB_OTA_UPDATER),true)
  MTK_SPLIT_VENDOR_AB_OTA_PARTITIONS += $(shell PYTHONPATH=vendor/mediatek/proprietary/scripts/releasetools AB_OTA_UPDATER="$(AB_OTA_UPDATER)" python device/mediatek/build/build/tools/ptgen/ab_partitions.py $(MTK_SCATTER_FILE_NAME))
  MTK_SPLIT_VENDOR_AB_OTA_PARTITIONS += $(BOARD_SUPER_PARTITION_PARTITION_LIST)
endif

MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES += $(filter-out $(ADD_IMG_TO_TARGET_FILES_IMAGES) odmdtbo.img $(MTK_VERIFIED_BOOT_CURRENT_IMAGES) $(MTK_SECURE_DOWNLOAD_CURRENT_IMAGES) $(MTK_NOT_UPGRADABLE_CURRENT_IMAGES),$(MTK_SCATTER_ORIGINAL_IMAGES))


intermediates := $(call intermediates-dir-for,PACKAGING,mtk_signed_image)

MTK_SPLIT_VENDOR_BOARD_PACK_RADIOIMAGES := $(filter-out $(ADD_IMG_TO_TARGET_FILES_IMAGES) $(MTK_NOT_UPGRADABLE_CURRENT_IMAGES),$(MTK_SCATTER_ORIGINAL_IMAGES))
ifneq (,$(IMAGE_LIST_TXT))
  $(foreach IMAGE_NAME,$(filter-out $(ADD_IMG_TO_TARGET_FILES_IMAGES),$(MTK_VERIFIED_BOOT_CURRENT_IMAGES)),\
    $(eval MTK_SPLIT_VENDOR_BOARD_PACK_RADIOIMAGES += $(IMAGE_NAME))\
    $(eval MTK_SPLIT_VENDOR_INSTALLED_RADIOIMAGE_TARGET += $(intermediates)/$(IMAGE_NAME))\
  )
else
  $(foreach IMAGE_NAME,$(filter-out $(ADD_IMG_TO_TARGET_FILES_IMAGES),$(MTK_SECURE_DOWNLOAD_CURRENT_IMAGES)),\
    $(eval MTK_SPLIT_VENDOR_BOARD_PACK_RADIOIMAGES += $(IMAGE_NAME))\
    $(if $(filter odmdtbo.img,$(IMAGE_NAME)),,\
      $(eval SIGN_IMAGE_NAME := $(basename $(IMAGE_NAME))-sign$(suffix $(IMAGE_NAME)))\
      $(eval MTK_SPLIT_VENDOR_INSTALLED_RADIOIMAGE_TARGET += $(PRODUCT_OUT)/$(SIGN_IMAGE_NAME))\
      $(if $(filter $(MTK_VERIFIED_BOOT_CURRENT_IMAGES),$(IMAGE_NAME)),\
        $(eval MTK_SPLIT_VENDOR_INSTALLED_RADIOIMAGE_TARGET += $(intermediates)/$(IMAGE_NAME))\
      ,\
        $(eval MTK_SPLIT_VENDOR_INSTALLED_RADIOIMAGE_TARGET += $(PRODUCT_OUT)/$(IMAGE_NAME))\
      )\
    )\
  )
endif

MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES := $(sort $(MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES))
$(foreach IMAGE_NAME,$(MTK_NORMAL_UPGRADABLE_CURRENT_IMAGES),\
  $(eval MTK_SPLIT_VENDOR_BOARD_PACK_RADIOIMAGES += $(IMAGE_NAME))\
  $(eval MTK_SPLIT_VENDOR_INSTALLED_RADIOIMAGE_TARGET += $(PRODUCT_OUT)/$(IMAGE_NAME))\
)


intermediates := $(call intermediates-dir-for,PACKAGING,vendor_target_files)

MTK_SPLIT_VENDOR_OTA_TXT_TARGET += $(intermediates)/OTA/ota_scatter.txt
.KATI_RESTAT: $(intermediates)/OTA/ota_scatter.txt
$(intermediates)/OTA/ota_scatter.txt: $(MTK_SCATTER_FILE_NAME) $(MTK_PTGEN_OTA_SCATTER_PERL)
	$(hide) mkdir -p $(dir $@)
	$(PERL) $(MTK_PTGEN_OTA_SCATTER_PERL) $< $@.tmp
	$(hide) if ! cmp -s $@.tmp $@; then mv $@.tmp $@; else rm $@.tmp; fi

ifneq ($(AB_OTA_UPDATER),true)
MTK_SPLIT_VENDOR_OTA_TXT_TARGET += $(intermediates)/OTA/ota_update_list.txt
.KATI_RESTAT: $(intermediates)/OTA/ota_update_list.txt
$(intermediates)/OTA/ota_update_list.txt: $(MTK_SCATTER_FILE_NAME) $(MTK_PTGEN_OTA_UPDATE_PYTHON)
	$(hide) mkdir -p $(dir $@)
	PYTHONPATH=vendor/mediatek/proprietary/scripts/releasetools MAIN_VBMETA_IN_BOOT="$(MAIN_VBMETA_IN_BOOT)" AB_OTA_UPDATER="$(AB_OTA_UPDATER)" BOARD_SUPER_PARTITION_PARTITION_LIST="$(BOARD_SUPER_PARTITION_PARTITION_LIST)" AB_OTA_PARTITIONS="$(AB_OTA_PARTITIONS)" python $(MTK_PTGEN_OTA_UPDATE_PYTHON) $< $@.tmp
	$(hide) if ! cmp -s $@.tmp $@; then mv $@.tmp $@; else rm $@.tmp; fi
endif

endif#MTK_PLATFORM_DIR


  BOARD_PACK_RADIOIMAGES := $(BOARD_PACK_RADIOIMAGES) $(MTK_SPLIT_VENDOR_BOARD_PACK_RADIOIMAGES)
  INSTALLED_RADIOIMAGE_TARGET := $(INSTALLED_RADIOIMAGE_TARGET) $(MTK_SPLIT_VENDOR_INSTALLED_RADIOIMAGE_TARGET)
  ifeq ($(AB_OTA_UPDATER),true)
    AB_OTA_PARTITIONS := $(AB_OTA_PARTITIONS) $(MTK_SPLIT_VENDOR_AB_OTA_PARTITIONS)
  endif
