
name := $(TARGET_PRODUCT)-vendor_target_files-$(FILE_NAME_TAG)
intermediates := $(call intermediates-dir-for,PACKAGING,vendor_target_files)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: intermediates := $(intermediates)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: zip_root := $(intermediates)/$(name)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: PRIVATE_OTA_TOOLS := $(built_ota_tools)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: PRIVATE_RECOVERY_API_VERSION := $(RECOVERY_API_VERSION)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: PRIVATE_RECOVERY_FSTAB_VERSION := $(RECOVERY_FSTAB_VERSION)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: PRIVATE_TOOL_EXTENSIONS := $(tool_extensions)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: PRIVATE_TOOL_EXTENSION := $(tool_extension)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: $(updater_dep)
ifeq ($(BOARD_USES_RECOVERY_AS_BOOT),true)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: PRIVATE_RECOVERY_OUT := BOOT
else
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: PRIVATE_RECOVERY_OUT := RECOVERY
endif
ifeq ($(BOARD_USES_RECOVERY_AS_BOOT),true)
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: DEBUG_RAMDISK_SYNC_DIR := $(TARGET_RECOVERY_ROOT_OUT)
else
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: DEBUG_RAMDISK_SYNC_DIR := $(TARGET_RAMDISK_OUT)
endif
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: DEBUG_RAMDISK_ROOT_DIR := $(PRODUCT_OUT)/debug_ramdisk
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: PRIVATE_DEBUG_RAMDISK_OUT := BOOT_DEBUG/RAMDISK$(patsubst debug_ramdisk%,%,$(TARGET_COPY_OUT_DEBUG_RAMDISK))
ifeq ($(AB_OTA_UPDATER),true)
  ifdef OSRELEASED_DIRECTORY
    $(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: $(TARGET_OUT_OEM)/$(OSRELEASED_DIRECTORY)/product_id
    $(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: $(TARGET_OUT_OEM)/$(OSRELEASED_DIRECTORY)/product_version
    $(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: $(TARGET_OUT_ETC)/$(OSRELEASED_DIRECTORY)/system_version
  endif
endif

MTK_PTGEN_OTA_SCATTER_PERL := device/mediatek/build/build/tools/ptgen/ota_scatter.pl
MTK_PTGEN_OTA_UPDATE_PYTHON := device/mediatek/build/build/tools/ptgen/ota_update_list.py

ifdef MTK_TARGET_PROJECT
  $(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: \
	    $(MTK_PTGEN_OTA_SCATTER_PERL) \
	    $(MTK_PTGEN_OTA_UPDATE_PYTHON) \
	    $(MTK_SCATTER_FILE_NAME) \
	    $(wildcard vendor/mediatek/proprietary/scripts/releasetools/*.py) \
	    $(MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET)
ifneq ($(support_split_build),true)
  # avoid race condition in recovery/root/res
  $(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: \
	    | $(MTK_SPLIT_IMAGES_DIR)/sys.target_files.zip
endif
endif
$(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip: \
	    $(INTERNAL_RAMDISK_FILES) \
	    $(INTERNAL_DEBUG_RAMDISK_FILES) \
	    $(INTERNAL_ROOT_FILES) \
	    $(INTERNAL_BOOTIMAGE_FILES) \
	    $(INSTALLED_RADIOIMAGE_TARGET) \
	    $(INSTALLED_RECOVERYIMAGE_TARGET) \
	    $(INTERNAL_USERDATAIMAGE_FILES) \
	    $(INTERNAL_CACHEIMAGE_FILES) \
	    $(INTERNAL_VENDORIMAGE_FILES) $(INSTALLED_FILES_FILE_VENDOR) $(DEPMOD) $(BOARD_VENDOR_KERNEL_MODULES) \
	    $(INSTALLED_ODMIMAGE_TARGET) \
	    $(INSTALLED_DTBOIMAGE_TARGET) \
	    $(INSTALLED_ANDROID_INFO_TXT_TARGET) \
	    $(INSTALLED_KERNEL_TARGET) \
	    $(INSTALLED_DTBIMAGE_TARGET) \
	    $(INSTALLED_2NDBOOTLOADER_TARGET) \
	    $(BOARD_PREBUILT_DTBOIMAGE) \
	    $(BOARD_PREBUILT_RECOVERY_DTBOIMAGE) \
	    $(BOARD_RECOVERY_ACPIO) \
	    $(PRODUCT_VENDOR_BASE_FS_PATH) \
	    $(PRODUCT_ODM_BASE_FS_PATH) \
	    $(LPMAKE) \
	    $(SELINUX_FC) \
	    $(APKCERTS_FILE) \
	    $(SOONG_APEX_KEYS_FILE) \
	    $(SOONG_ZIP) \
	    $(HOST_OUT_EXECUTABLES)/fs_config \
	    $(HOST_OUT_EXECUTABLES)/imgdiff \
	    $(HOST_OUT_EXECUTABLES)/bsdiff \
	    $(HOST_OUT_EXECUTABLES)/care_map_generator \
	    $(BUILD_IMAGE_SRCS) \
	    $(BUILT_VENDOR_MATRIX) \
	    | $(ACP)
	@echo "Package target files: $@"
	$(call create-system-vendor-symlink)
	$(call create-system-product-symlink)
	$(call create-system-product_services-symlink)
	$(call create-vendor-odm-symlink)
	$(hide) rm -rf $@ $(zip_root).list $(zip_root)
	$(hide) mkdir -p $(dir $@) $(zip_root)
ifneq (,$(INSTALLED_RECOVERYIMAGE_TARGET)$(filter true,$(BOARD_USES_RECOVERY_AS_BOOT)))
	@# Components of the recovery image
	$(hide) mkdir -p $(zip_root)/$(PRIVATE_RECOVERY_OUT)
	$(hide) $(call package_files-copy-root, \
	    $(TARGET_RECOVERY_ROOT_OUT),$(zip_root)/$(PRIVATE_RECOVERY_OUT)/RAMDISK)
ifdef INSTALLED_KERNEL_TARGET
	$(hide) cp $(INSTALLED_KERNEL_TARGET) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/kernel
endif
ifdef INSTALLED_2NDBOOTLOADER_TARGET
	$(hide) cp $(INSTALLED_2NDBOOTLOADER_TARGET) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/second
endif
ifdef BOARD_INCLUDE_RECOVERY_DTBO
ifdef BOARD_PREBUILT_RECOVERY_DTBOIMAGE
	$(hide) cp $(BOARD_PREBUILT_RECOVERY_DTBOIMAGE) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/recovery_dtbo
else
	$(hide) cp $(BOARD_PREBUILT_DTBOIMAGE) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/recovery_dtbo
endif
endif
ifdef BOARD_INCLUDE_RECOVERY_ACPIO
	$(hide) cp $(BOARD_RECOVERY_ACPIO) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/recovery_acpio
endif
ifdef INSTALLED_DTBIMAGE_TARGET
	$(hide) cp $(INSTALLED_DTBIMAGE_TARGET) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/dtb
endif
ifdef INTERNAL_KERNEL_CMDLINE
	$(hide) echo "$(INTERNAL_KERNEL_CMDLINE)" > $(zip_root)/$(PRIVATE_RECOVERY_OUT)/cmdline
endif
ifdef BOARD_KERNEL_BASE
	$(hide) echo "$(BOARD_KERNEL_BASE)" > $(zip_root)/$(PRIVATE_RECOVERY_OUT)/base
endif
ifdef BOARD_KERNEL_PAGESIZE
	$(hide) echo "$(BOARD_KERNEL_PAGESIZE)" > $(zip_root)/$(PRIVATE_RECOVERY_OUT)/pagesize
endif
endif # INSTALLED_RECOVERYIMAGE_TARGET defined or BOARD_USES_RECOVERY_AS_BOOT is true
	@# Components of the boot image
	$(hide) mkdir -p $(zip_root)/BOOT
	$(hide) mkdir -p $(zip_root)/ROOT
	$(hide) $(call package_files-copy-root, \
	    $(TARGET_ROOT_OUT),$(zip_root)/ROOT)
	@# If we are using recovery as boot, this is already done when processing recovery.
ifneq ($(BOARD_USES_RECOVERY_AS_BOOT),true)
ifneq ($(BOARD_BUILD_SYSTEM_ROOT_IMAGE),true)
	$(hide) $(call package_files-copy-root, \
	    $(TARGET_RAMDISK_OUT),$(zip_root)/BOOT/RAMDISK)
endif
ifdef INSTALLED_KERNEL_TARGET
	$(hide) cp $(INSTALLED_KERNEL_TARGET) $(zip_root)/BOOT/kernel
endif
ifdef INSTALLED_2NDBOOTLOADER_TARGET
	$(hide) cp $(INSTALLED_2NDBOOTLOADER_TARGET) $(zip_root)/BOOT/second
endif
ifdef INSTALLED_DTBIMAGE_TARGET
	$(hide) cp $(INSTALLED_DTBIMAGE_TARGET) $(zip_root)/BOOT/dtb
endif
ifdef INTERNAL_KERNEL_CMDLINE
	$(hide) echo "$(INTERNAL_KERNEL_CMDLINE)" > $(zip_root)/BOOT/cmdline
endif
ifdef BOARD_KERNEL_BASE
	$(hide) echo "$(BOARD_KERNEL_BASE)" > $(zip_root)/BOOT/base
endif
ifdef BOARD_KERNEL_PAGESIZE
	$(hide) echo "$(BOARD_KERNEL_PAGESIZE)" > $(zip_root)/BOOT/pagesize
endif
endif # BOARD_USES_RECOVERY_AS_BOOT
	$(hide) $(foreach t,$(INSTALLED_RADIOIMAGE_TARGET),\
	            mkdir -p $(zip_root)/RADIO; \
	            cp $(t) $(zip_root)/RADIO/$(notdir $(t));)
#ifdef BUILDING_SYSTEM_IMAGE
#	@# Contents of the system image
#	$(hide) $(call package_files-copy-root, \
#	    $(SYSTEMIMAGE_SOURCE_DIR),$(zip_root)/SYSTEM)
#endif
ifdef BUILDING_USERDATA_IMAGE
	@# Contents of the data image
	$(hide) $(call package_files-copy-root, \
	    $(TARGET_OUT_DATA),$(zip_root)/DATA)
endif
ifdef BUILDING_VENDOR_IMAGE
	@# Contents of the vendor image
	$(hide) $(call package_files-copy-root, \
	    $(TARGET_OUT_VENDOR),$(zip_root)/VENDOR)
endif
#ifdef BUILDING_PRODUCT_IMAGE
#	@# Contents of the product image
#	$(hide) $(call package_files-copy-root, \
#	    $(TARGET_OUT_PRODUCT),$(zip_root)/PRODUCT)
#endif
#ifdef BUILDING_PRODUCT_SERVICES_IMAGE
#	@# Contents of the product_services image
#	$(hide) $(call package_files-copy-root, \
#	    $(TARGET_OUT_PRODUCT_SERVICES),$(zip_root)/PRODUCT_SERVICES)
#endif
ifdef BUILDING_ODM_IMAGE
	@# Contents of the odm image
	$(hide) $(call package_files-copy-root, \
	    $(TARGET_OUT_ODM),$(zip_root)/ODM)
endif
#ifdef BUILDING_SYSTEM_OTHER_IMAGE
#	@# Contents of the system_other image
#	$(hide) $(call package_files-copy-root, \
#	    $(TARGET_OUT_SYSTEM_OTHER),$(zip_root)/SYSTEM_OTHER)
#endif
	@# Extra contents of the OTA package
	$(hide) mkdir -p $(zip_root)/OTA
	$(hide) cp $(INSTALLED_ANDROID_INFO_TXT_TARGET) $(zip_root)/OTA/
ifneq ($(AB_OTA_UPDATER),true)
ifneq ($(built_ota_tools),)
	$(hide) mkdir -p $(zip_root)/OTA/bin
	$(hide) cp $(PRIVATE_OTA_TOOLS) $(zip_root)/OTA/bin/
endif
endif
	@# Files that do not end up in any images, but are necessary to
	@# build them.
	$(hide) mkdir -p $(zip_root)/META
	$(hide) cp $(APKCERTS_FILE) $(zip_root)/META/apkcerts.txt
	$(hide) cp $(SOONG_APEX_KEYS_FILE) $(zip_root)/META/apexkeys.txt
ifneq ($(tool_extension),)
	$(hide) cp $(PRIVATE_TOOL_EXTENSION) $(zip_root)/META/
endif
	$(hide) echo "$(PRODUCT_OTA_PUBLIC_KEYS)" > $(zip_root)/META/otakeys.txt
	$(hide) cp $(SELINUX_FC) $(zip_root)/META/file_contexts.bin
	$(hide) echo "recovery_api_version=$(PRIVATE_RECOVERY_API_VERSION)" > $(zip_root)/META/misc_info.txt
	$(hide) echo "fstab_version=$(PRIVATE_RECOVERY_FSTAB_VERSION)" >> $(zip_root)/META/misc_info.txt
ifdef BOARD_FLASH_BLOCK_SIZE
	$(hide) echo "blocksize=$(BOARD_FLASH_BLOCK_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_BOOTIMAGE_PARTITION_SIZE
	$(hide) echo "boot_size=$(BOARD_BOOTIMAGE_PARTITION_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
ifeq ($(INSTALLED_RECOVERYIMAGE_TARGET),)
	$(hide) echo "no_recovery=true" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_INCLUDE_RECOVERY_DTBO
	$(hide) echo "include_recovery_dtbo=true" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_INCLUDE_RECOVERY_ACPIO
	$(hide) echo "include_recovery_acpio=true" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_RECOVERYIMAGE_PARTITION_SIZE
	$(hide) echo "recovery_size=$(BOARD_RECOVERYIMAGE_PARTITION_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
ifdef TARGET_RECOVERY_FSTYPE_MOUNT_OPTIONS
	@# TARGET_RECOVERY_FSTYPE_MOUNT_OPTIONS can be empty to indicate that nothing but defaults should be used.
	$(hide) echo "recovery_mount_options=$(TARGET_RECOVERY_FSTYPE_MOUNT_OPTIONS)" >> $(zip_root)/META/misc_info.txt
else
	$(hide) echo "recovery_mount_options=$(DEFAULT_TARGET_RECOVERY_FSTYPE_MOUNT_OPTIONS)" >> $(zip_root)/META/misc_info.txt
endif
	$(hide) echo "tool_extensions=$(PRIVATE_TOOL_EXTENSIONS)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "default_system_dev_certificate=$(DEFAULT_SYSTEM_DEV_CERTIFICATE)" >> $(zip_root)/META/misc_info.txt
ifdef PRODUCT_EXTRA_RECOVERY_KEYS
	$(hide) echo "extra_recovery_keys=$(PRODUCT_EXTRA_RECOVERY_KEYS)" >> $(zip_root)/META/misc_info.txt
endif
	$(hide) echo 'mkbootimg_args=$(BOARD_MKBOOTIMG_ARGS)' >> $(zip_root)/META/misc_info.txt
	$(hide) echo 'mkbootimg_version_args=$(INTERNAL_MKBOOTIMG_VERSION_ARGS)' >> $(zip_root)/META/misc_info.txt
	$(hide) echo "multistage_support=1" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "blockimgdiff_versions=3,4" >> $(zip_root)/META/misc_info.txt
ifneq ($(OEM_THUMBPRINT_PROPERTIES),)
	# OTA scripts are only interested in fingerprint related properties
	$(hide) echo "oem_fingerprint_properties=$(OEM_THUMBPRINT_PROPERTIES)" >> $(zip_root)/META/misc_info.txt
endif
#ifneq ($(PRODUCT_SYSTEM_BASE_FS_PATH),)
#	$(hide) cp $(PRODUCT_SYSTEM_BASE_FS_PATH) \
#	  $(zip_root)/META/$(notdir $(PRODUCT_SYSTEM_BASE_FS_PATH))
#endif
ifneq ($(PRODUCT_VENDOR_BASE_FS_PATH),)
	$(hide) cp $(PRODUCT_VENDOR_BASE_FS_PATH) \
	  $(zip_root)/META/$(notdir $(PRODUCT_VENDOR_BASE_FS_PATH))
endif
#ifneq ($(PRODUCT_PRODUCT_BASE_FS_PATH),)
#	$(hide) cp $(PRODUCT_PRODUCT_BASE_FS_PATH) \
#	  $(zip_root)/META/$(notdir $(PRODUCT_PRODUCT_BASE_FS_PATH))
#endif
#ifneq ($(PRODUCT_PRODUCT_SERVICES_BASE_FS_PATH),)
#	$(hide) cp $(PRODUCT_PRODUCT_SERVICES_BASE_FS_PATH) \
#	  $(zip_root)/META/$(notdir $(PRODUCT_PRODUCT_SERVICES_BASE_FS_PATH))
#endif
ifneq ($(PRODUCT_ODM_BASE_FS_PATH),)
	$(hide) cp $(PRODUCT_ODM_BASE_FS_PATH) \
	  $(zip_root)/META/$(notdir $(PRODUCT_ODM_BASE_FS_PATH))
endif
#ifneq (,$(filter address, $(SANITIZE_TARGET)))
	# We need to create userdata.img with real data because the instrumented libraries are in userdata.img.
	$(hide) echo "userdata_img_with_data=true" >> $(zip_root)/META/misc_info.txt
#endif
ifeq ($(BOARD_USES_FULL_RECOVERY_IMAGE),true)
	$(hide) echo "full_recovery_image=true" >> $(zip_root)/META/misc_info.txt
endif
ifeq ($(BOARD_AVB_ENABLE),true)
	$(hide) echo "avb_enable=true" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_key_path=$(BOARD_AVB_KEY_PATH)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_algorithm=$(BOARD_AVB_ALGORITHM)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_args=$(BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_boot_add_hash_footer_args=$(BOARD_AVB_BOOT_ADD_HASH_FOOTER_ARGS)" >> $(zip_root)/META/misc_info.txt
ifdef BOARD_AVB_BOOT_KEY_PATH
	$(hide) echo "avb_boot_key_path=$(BOARD_AVB_BOOT_KEY_PATH)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_boot_algorithm=$(BOARD_AVB_BOOT_ALGORITHM)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_boot_rollback_index_location=$(BOARD_AVB_BOOT_ROLLBACK_INDEX_LOCATION)" >> $(zip_root)/META/misc_info.txt
endif # BOARD_AVB_BOOT_KEY_PATH
	$(hide) echo "avb_recovery_add_hash_footer_args=$(BOARD_AVB_RECOVERY_ADD_HASH_FOOTER_ARGS)" >> $(zip_root)/META/misc_info.txt
ifdef BOARD_AVB_RECOVERY_KEY_PATH
	$(hide) echo "avb_recovery_key_path=$(BOARD_AVB_RECOVERY_KEY_PATH)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_recovery_algorithm=$(BOARD_AVB_RECOVERY_ALGORITHM)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_recovery_rollback_index_location=$(BOARD_AVB_RECOVERY_ROLLBACK_INDEX_LOCATION)" >> $(zip_root)/META/misc_info.txt
endif # BOARD_AVB_RECOVERY_KEY_PATH
ifneq (,$(strip $(BOARD_AVB_VBMETA_SYSTEM)))
	$(hide) echo "avb_vbmeta_system=$(BOARD_AVB_VBMETA_SYSTEM)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_system_args=$(BOARD_AVB_MAKE_VBMETA_SYSTEM_IMAGE_ARGS)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_system_key_path=$(BOARD_AVB_VBMETA_SYSTEM_KEY_PATH)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_system_algorithm=$(BOARD_AVB_VBMETA_SYSTEM_ALGORITHM)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_system_rollback_index_location=$(BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX_LOCATION)" >> $(zip_root)/META/misc_info.txt
endif # BOARD_AVB_VBMETA_SYSTEM
ifneq (,$(strip $(BOARD_AVB_VBMETA_VENDOR)))
	$(hide) echo "avb_vbmeta_vendor=$(BOARD_AVB_VBMETA_VENDOR)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_vendor_args=$(BOARD_AVB_MAKE_VBMETA_SYSTEM_IMAGE_ARGS)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_vendor_key_path=$(BOARD_AVB_VBMETA_VENDOR_KEY_PATH)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_vendor_algorithm=$(BOARD_AVB_VBMETA_VENDOR_ALGORITHM)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_vbmeta_vendor_rollback_index_location=$(BOARD_AVB_VBMETA_VENDOR_ROLLBACK_INDEX_LOCATION)" >> $(zip_root)/META/misc_info.txt
endif # BOARD_AVB_VBMETA_VENDOR_KEY_PATH
endif # BOARD_AVB_ENABLE
ifdef BOARD_BPT_INPUT_FILES
	$(hide) echo "board_bpt_enable=true" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "board_bpt_make_table_args=$(BOARD_BPT_MAKE_TABLE_ARGS)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "board_bpt_input_files=$(BOARD_BPT_INPUT_FILES)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_BPT_DISK_SIZE
	$(hide) echo "board_bpt_disk_size=$(BOARD_BPT_DISK_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
	$(call generate-userimage-prop-dictionary, $(zip_root)/META/misc_info.txt)
ifneq ($(INSTALLED_RECOVERYIMAGE_TARGET),)
#ifdef BUILDING_SYSTEM_IMAGE
#	$(hide) PATH=$(foreach p,$(INTERNAL_USERIMAGES_BINARY_PATHS),$(p):)$$PATH MKBOOTIMG=$(MKBOOTIMG) \
#	    build/make/tools/releasetools/make_recovery_patch $(zip_root) $(zip_root)
#endif # BUILDING_SYSTEM_IMAGE
endif
ifeq ($(AB_OTA_UPDATER),true)
	@# When using the A/B updater, include the updater config files in the zip.
	$(hide) cp $(TOPDIR)system/update_engine/update_engine.conf $(zip_root)/META/update_engine_config.txt
	$(hide) for part in $(AB_OTA_PARTITIONS); do \
	  echo "$${part}" >> $(zip_root)/META/ab_partitions.txt; \
	done
	$(hide) for conf in $(AB_OTA_POSTINSTALL_CONFIG); do \
	  echo "$${conf}" >> $(zip_root)/META/postinstall_config.txt; \
	done
	@# Include the build type in META/misc_info.txt so the server can easily differentiate production builds.
	$(hide) echo "build_type=$(TARGET_BUILD_VARIANT)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "ab_update=true" >> $(zip_root)/META/misc_info.txt
ifdef OSRELEASED_DIRECTORY
	$(hide) cp $(TARGET_OUT_OEM)/$(OSRELEASED_DIRECTORY)/product_id $(zip_root)/META/product_id.txt
	$(hide) cp $(TARGET_OUT_OEM)/$(OSRELEASED_DIRECTORY)/product_version $(zip_root)/META/product_version.txt
	$(hide) cp $(TARGET_OUT_ETC)/$(OSRELEASED_DIRECTORY)/system_version $(zip_root)/META/system_version.txt
endif
endif
ifeq ($(BREAKPAD_GENERATE_SYMBOLS),true)
	@# If breakpad symbols have been generated, add them to the zip.
	$(hide) $(ACP) -r $(TARGET_OUT_BREAKPAD) $(zip_root)/BREAKPAD
endif
#ifdef BOARD_PREBUILT_VENDORIMAGE
#	$(hide) mkdir -p $(zip_root)/IMAGES
#	$(hide) cp $(INSTALLED_VENDORIMAGE_TARGET) $(zip_root)/IMAGES/
#endif
#ifdef BOARD_PREBUILT_PRODUCTIMAGE
#	$(hide) mkdir -p $(zip_root)/IMAGES
#	$(hide) cp $(INSTALLED_PRODUCTIMAGE_TARGET) $(zip_root)/IMAGES/
#endif
#ifdef BOARD_PREBUILT_PRODUCT_SERVICESIMAGE
#	$(hide) mkdir -p $(zip_root)/IMAGES
#	$(hide) cp $(INSTALLED_PRODUCT_SERVICESIMAGE_TARGET) $(zip_root)/IMAGES/
#endif
#ifdef BOARD_PREBUILT_BOOTIMAGE
#	$(hide) mkdir -p $(zip_root)/IMAGES
#	$(hide) cp $(INSTALLED_BOOTIMAGE_TARGET) $(zip_root)/IMAGES/
#endif
ifdef BOARD_PREBUILT_ODMIMAGE
	$(hide) mkdir -p $(zip_root)/IMAGES
	$(hide) cp $(INSTALLED_ODMIMAGE_TARGET) $(zip_root)/IMAGES/
endif
ifdef BOARD_PREBUILT_DTBOIMAGE
	$(hide) mkdir -p $(zip_root)/PREBUILT_IMAGES
ifdef MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET
	$(hide) cp $(patsubst %.img, %-verified.img, $(INSTALLED_DTBOIMAGE_TARGET)) $(zip_root)/PREBUILT_IMAGES/dtbo.img
endif
	$(hide) echo "has_dtbo=true" >> $(zip_root)/META/misc_info.txt
ifeq ($(BOARD_AVB_ENABLE),true)
	$(hide) echo "dtbo_size=$(BOARD_DTBOIMG_PARTITION_SIZE)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_dtbo_add_hash_footer_args=$(BOARD_AVB_DTBO_ADD_HASH_FOOTER_ARGS)" >> $(zip_root)/META/misc_info.txt
ifdef BOARD_AVB_DTBO_KEY_PATH
	$(hide) echo "avb_dtbo_key_path=$(BOARD_AVB_DTBO_KEY_PATH)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_dtbo_algorithm=$(BOARD_AVB_DTBO_ALGORITHM)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_dtbo_rollback_index_location=$(BOARD_AVB_DTBO_ROLLBACK_INDEX_LOCATION)" \
	    >> $(zip_root)/META/misc_info.txt
endif # BOARD_AVB_DTBO_KEY_PATH
endif # BOARD_AVB_ENABLE
endif # BOARD_PREBUILT_DTBOIMAGE
	$(call dump-dynamic-partitions-info,$(zip_root)/META/misc_info.txt)
	@# The radio images in BOARD_PACK_RADIOIMAGES will be additionally copied from RADIO/ into
	@# IMAGES/, which then will be added into <product>-img.zip. Such images must be listed in
	@# INSTALLED_RADIOIMAGE_TARGET.
	$(hide) $(foreach part,$(BOARD_PACK_RADIOIMAGES), \
	    echo $(part) >> $(zip_root)/META/pack_radioimages.txt;)
	@# Run fs_config on all the system, vendor, boot ramdisk,
	@# and recovery ramdisk files in the zip, and save the output
#ifdef BUILDING_SYSTEM_IMAGE
#	$(hide) $(call fs_config,$(zip_root)/SYSTEM,system/) > $(zip_root)/META/filesystem_config.txt
#endif
ifdef BUILDING_VENDOR_IMAGE
	$(hide) $(call fs_config,$(zip_root)/VENDOR,vendor/) > $(zip_root)/META/vendor_filesystem_config.txt
endif
#ifdef BUILDING_PRODUCT_IMAGE
#	$(hide) $(call fs_config,$(zip_root)/PRODUCT,product/) > $(zip_root)/META/product_filesystem_config.txt
#endif
#ifdef BUILDING_PRODUCT_SERVICES_IMAGE
#	$(hide) $(call fs_config,$(zip_root)/PRODUCT_SERVICES,product_services/) > $(zip_root)/META/product_services_filesystem_config.txt
#endif
ifdef BUILDING_ODM_IMAGE
	$(hide) $(call fs_config,$(zip_root)/ODM,odm/) > $(zip_root)/META/odm_filesystem_config.txt
endif
	@# ROOT always contains the files for the root under normal boot.
#	$(hide) $(call fs_config,$(zip_root)/ROOT,) > $(zip_root)/META/root_filesystem_config.txt
ifeq ($(BOARD_USES_RECOVERY_AS_BOOT),true)
	@# BOOT/RAMDISK exists and contains the ramdisk for recovery if using BOARD_USES_RECOVERY_AS_BOOT.
	$(hide) $(call fs_config,$(zip_root)/BOOT/RAMDISK,) > $(zip_root)/META/boot_filesystem_config.txt
endif
ifneq ($(BOARD_BUILD_SYSTEM_ROOT_IMAGE),true)
	@# BOOT/RAMDISK also exists and contains the first stage ramdisk if not using BOARD_BUILD_SYSTEM_ROOT_IMAGE.
	$(hide) $(call fs_config,$(zip_root)/BOOT/RAMDISK,) > $(zip_root)/META/boot_filesystem_config.txt
endif
ifneq ($(INSTALLED_RECOVERYIMAGE_TARGET),)
	$(hide) $(call fs_config,$(zip_root)/RECOVERY/RAMDISK,) > $(zip_root)/META/recovery_filesystem_config.txt
endif
#ifdef BUILDING_SYSTEM_OTHER_IMAGE
#	$(hide) $(call fs_config,$(zip_root)/SYSTEM_OTHER,system/) > $(zip_root)/META/system_other_filesystem_config.txt
#endif
	@# Metadata for compatibility verification.
#	$(hide) cp $(BUILT_SYSTEM_MATRIX) $(zip_root)/META/system_matrix.xml
#	$(hide) cp $(BUILT_ASSEMBLED_FRAMEWORK_MANIFEST) $(zip_root)/META/system_manifest.xml
#ifdef BUILT_ASSEMBLED_VENDOR_MANIFEST
#	$(hide) cp $(BUILT_ASSEMBLED_VENDOR_MANIFEST) $(zip_root)/META/vendor_manifest.xml
#endif
ifdef BUILT_VENDOR_MATRIX
	$(hide) cp $(BUILT_VENDOR_MATRIX) $(zip_root)/META/vendor_matrix.xml
endif
ifneq ($(BOARD_SUPER_PARTITION_GROUPS),)
	$(hide) echo "super_partition_groups=$(BOARD_SUPER_PARTITION_GROUPS)" > $(zip_root)/META/dynamic_partitions_info.txt
	@# Remove 'vendor' from the group partition list if the image is not available. This should only
	@# happen to AOSP targets built without vendor.img. We can't remove the partition from the
	@# BoardConfig file, as it's still needed elsewhere (e.g. when creating super_empty.img).
	$(foreach group,$(BOARD_SUPER_PARTITION_GROUPS), \
	    $(eval _group_partition_list := $(BOARD_$(call to-upper,$(group))_PARTITION_LIST)) \
	    $(if $(INSTALLED_VENDORIMAGE_TARGET),,$(eval _group_partition_list := $(filter-out vendor,$(_group_partition_list)))) \
	    echo "$(group)_size=$(BOARD_$(call to-upper,$(group))_SIZE)" >> $(zip_root)/META/dynamic_partitions_info.txt; \
	    $(if $(_group_partition_list), \
	        echo "$(group)_partition_list=$(_group_partition_list)" >> $(zip_root)/META/dynamic_partitions_info.txt;))
endif # BOARD_SUPER_PARTITION_GROUPS
# MTK
ifdef BUILDING_RAMDISK_IMAGE
	$(hide) mkdir -p $(zip_root)/$(PRIVATE_DEBUG_RAMDISK_OUT)
	$(hide) touch $(zip_root)/$(PRIVATE_DEBUG_RAMDISK_OUT)/force_debuggable
	$(hide) rsync -a $(DEBUG_RAMDISK_SYNC_DIR)/ $(zip_root)/BOOT_DEBUG/RAMDISK
	$(hide) $(call package_files-copy-root, \
	    $(DEBUG_RAMDISK_ROOT_DIR),$(zip_root)/BOOT_DEBUG/RAMDISK)
endif
ifdef INSTALLED_KERNEL_TARGET
	$(hide) cp $(INSTALLED_KERNEL_TARGET) $(zip_root)/BOOT_DEBUG/kernel
endif
ifdef INSTALLED_2NDBOOTLOADER_TARGET
	$(hide) cp $(INSTALLED_2NDBOOTLOADER_TARGET) $(zip_root)/BOOT_DEBUG/second
endif
ifdef BOARD_INCLUDE_RECOVERY_DTBO
ifdef BOARD_PREBUILT_RECOVERY_DTBOIMAGE
	$(hide) cp $(BOARD_PREBUILT_RECOVERY_DTBOIMAGE) $(zip_root)/BOOT_DEBUG/recovery_dtbo
else
	$(hide) cp $(BOARD_PREBUILT_DTBOIMAGE) $(zip_root)/BOOT_DEBUG/recovery_dtbo
endif
endif
ifdef BOARD_INCLUDE_RECOVERY_ACPIO
	$(hide) cp $(BOARD_RECOVERY_ACPIO) $(zip_root)/BOOT_DEBUG/recovery_acpio
endif
ifdef INSTALLED_DTBIMAGE_TARGET
	$(hide) cp $(INSTALLED_DTBIMAGE_TARGET) $(zip_root)/BOOT_DEBUG/dtb
endif
ifdef INTERNAL_KERNEL_CMDLINE
	$(hide) echo "$(INTERNAL_KERNEL_CMDLINE)" > $(zip_root)/BOOT_DEBUG/cmdline
endif
ifdef BOARD_KERNEL_BASE
	$(hide) echo "$(BOARD_KERNEL_BASE)" > $(zip_root)/BOOT_DEBUG/base
endif
ifdef BOARD_KERNEL_PAGESIZE
	$(hide) echo "$(BOARD_KERNEL_PAGESIZE)" > $(zip_root)/BOOT_DEBUG/pagesize
endif
ifneq ($(BOARD_AVB_BOOT_KEY_PATH),)
	$(hide) echo "avb_debug_boot_key_path=$(BOARD_AVB_DEBUG_BOOT_KEY_PATH)" >> $(zip_root)/META/misc_info.txt
	$(hide) echo "avb_debug_boot_algorithm=SHA256_RSA2048" >> $(zip_root)/META/misc_info.txt
endif
ifeq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
ifeq ($(INTERNAL_SYSTEM_OTHER_PARTITION_SIZE),)
ifeq ($(PRODUCT_RETROFIT_DYNAMIC_PARTITIONS),true)
	$(hide) $(if $(BOARD_SUPER_PARTITION_SYSTEM_DEVICE_SIZE),echo "system_other_size=$(BOARD_SUPER_PARTITION_SYSTEM_DEVICE_SIZE)" >> $(zip_root)/META/misc_info.txt)
else
	$(hide) $(if $(BOARD_SYSTEMIMAGE_PARTITION_SIZE),echo "system_other_size=$(BOARD_SYSTEMIMAGE_PARTITION_SIZE)" >> $(zip_root)/META/misc_info.txt)
endif
endif
endif
ifneq ($(notdir $(BOARD_PREBUILT_DTBOIMAGE)),dtbo.img)
	$(hide) echo "dtbo_image_name=$(notdir $(BOARD_PREBUILT_DTBOIMAGE))" >> $(zip_root)/META/misc_info.txt
endif
ifeq (yes, $(strip $(MAIN_VBMETA_IN_BOOT)))
	$(hide) echo "avb_main_vbmeta_in_boot=true" >> $(zip_root)/META/misc_info.txt
endif # MAIN_VBMETA_IN_BOOT
ifdef MTK_PLATFORM_DIR
	$(hide) echo "mtk_platform_dir=$(MTK_PLATFORM_DIR)" >> $(zip_root)/META/misc_info.txt
endif
ifdef MTK_BASE_PROJECT
	$(hide) echo "mtk_base_project=$(MTK_BASE_PROJECT)" >> $(zip_root)/META/misc_info.txt
endif
ifdef MTK_TARGET_PROJECT
	$(hide) echo "mtk_target_project=$(MTK_TARGET_PROJECT)" >> $(zip_root)/META/misc_info.txt
endif
ifneq ($(wildcard $(IMAGE_LIST_TXT)),)
	$(hide) echo "mtk_sign_image_version=v2" >> $(zip_root)/META/misc_info.txt
else
	$(hide) echo "mtk_sign_image_version=v1" >> $(zip_root)/META/misc_info.txt
endif
# MNTL
ifeq ($(TARGET_USERIMAGES_USE_MNTL), true)
	$(hide) echo "use_mntl=$(TARGET_USERIMAGES_USE_MNTL)" >> $(zip_root)/META/misc_info.txt
ifdef BOARD_FLASH_PAGE_SIZE
	$(hide) echo "mntl_page_size=$(BOARD_FLASH_PAGE_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_SYSTEMIMAGE_START_SECTOR
	$(hide) echo "mntl_systemimage_start_sector=$(BOARD_SYSTEMIMAGE_START_SECTOR)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_VENDORIMAGE_START_SECTOR
	$(hide) echo "mntl_vendorimage_start_sector=$(BOARD_VENDORIMAGE_START_SECTOR)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_CACHEIMAGE_START_SECTOR
	$(hide) echo "mntl_cacheimage_start_sector=$(BOARD_CACHEIMAGE_START_SECTOR)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_USERDATAIMAGE_START_SECTOR
	$(hide) echo "mntl_userdataimage_start_sector=$(BOARD_USERDATAIMAGE_START_SECTOR)" >> $(zip_root)/META/misc_info.txt
endif
endif
ifdef MTK_TARGET_PROJECT
# ota_scatter.txt
	$(PERL) $(MTK_PTGEN_OTA_SCATTER_PERL) $(MTK_SCATTER_FILE_NAME) $(zip_root)/OTA/ota_scatter.txt
ifeq ($(AB_OTA_UPDATER),true)
# ab_partitions.txt
	PYTHONPATH=vendor/mediatek/proprietary/scripts/releasetools MAIN_VBMETA_IN_BOOT="$(MAIN_VBMETA_IN_BOOT)" AB_OTA_UPDATER="$(AB_OTA_UPDATER)" BOARD_SUPER_PARTITION_PARTITION_LIST="$(BOARD_SUPER_PARTITION_PARTITION_LIST)" python device/mediatek/build/build/tools/ptgen/ab_partitions.py $(MTK_SCATTER_FILE_NAME) > $(zip_root)/META/ab_partitions.txt
else
# ota_update_list.txt
	PYTHONPATH=vendor/mediatek/proprietary/scripts/releasetools MAIN_VBMETA_IN_BOOT="$(MAIN_VBMETA_IN_BOOT)" AB_OTA_UPDATER="$(AB_OTA_UPDATER)" BOARD_SUPER_PARTITION_PARTITION_LIST="$(BOARD_SUPER_PARTITION_PARTITION_LIST)" AB_OTA_PARTITIONS="$(AB_OTA_PARTITIONS)" python $(MTK_PTGEN_OTA_UPDATE_PYTHON) $(MTK_SCATTER_FILE_NAME) $(zip_root)/OTA/ota_update_list.txt
	$(hide) touch $(zip_root)/META/ab_partitions.txt
endif
	$(hide) mkdir -p $(zip_root)/DATA
	$(hide) mkdir -p $(zip_root)/META_TEMP
ifdef BUILT_VENDOR_MANIFEST
	$(hide) cp $(BUILT_VENDOR_MANIFEST) $(zip_root)/META_TEMP/built_vendor_manifest.xml
endif
	$(hide) mkdir -p $(zip_root)/IMAGES
	$(hide) if [ -e $(PRODUCT_OUT)/PGPT ]; then cp -f $(PRODUCT_OUT)/PGPT $(zip_root)/IMAGES/PGPT; fi
endif
# MTK
	@# Zip everything up, preserving symlinks and placing META/ files first to
	@# help early validation of the .zip file while uploading it.
	$(hide) find $(zip_root)/META | sort >$(zip_root).list
	$(hide) find $(zip_root) -path $(zip_root)/META -prune -o -print | sort | sed -e 's/\[/\\\[/g' -e 's/\]/\\\]/g' >>$(zip_root).list
	$(hide) $(SOONG_ZIP) -d -o $@ -C $(zip_root) -l $(zip_root).list


MTK_SPLIT_VENDOR_DIST_FILES := \
	$(MTK_NOT_UPGRADABLE_IMAGE_TARGET) \
	$(INSTALLED_FILES_FILE_RAMDISK) \
	$(INSTALLED_FILES_FILE_DEBUG_RAMDISK) \
	$(INSTALLED_FILES_FILE_VENDOR) \
	$(INSTALLED_FILES_FILE_ODM)

ifeq ($(TARGET_USERIMAGES_USE_MNTL), true)
MTK_SPLIT_VENDOR_DIST_FILES += \
	$(PRODUCT_OUT)/ftl_gpt.bin
endif

name := $(TARGET_PRODUCT)-vendor_dist_files-$(FILE_NAME_TAG)
intermediates := $(call intermediates-dir-for,PACKAGING,vendor_dist_files)
$(MTK_SPLIT_IMAGES_DIR)/vnd.dist_files.zip: intermediates := $(intermediates)
$(MTK_SPLIT_IMAGES_DIR)/vnd.dist_files.zip: zip_root := $(intermediates)/$(name)
$(MTK_SPLIT_IMAGES_DIR)/vnd.dist_files.zip: $(MTK_SCATTER_FILE_NAME) $(MTK_CGEN_AP_Editor_DB)
$(MTK_SPLIT_IMAGES_DIR)/vnd.dist_files.zip: $(MTK_SPLIT_VENDOR_DIST_FILES)
$(MTK_SPLIT_IMAGES_DIR)/vnd.dist_files.zip: \
	    $(filter $(TARGET_OUT_TESTCASES)/%,$(ALL_DEFAULT_INSTALLED_MODULES)) \
	    $(SOONG_ZIP) \
	    | $(ACP)
	@echo "Package target files: $@"
	$(hide) rm -rf $@ $(zip_root).list $(zip_root)
	$(hide) mkdir -p $(dir $@) $(zip_root)
ifdef MTK_SCATTER_FILE_NAME
	$(hide) cp -f $(MTK_SCATTER_FILE_NAME) $(zip_root)/
endif
ifdef MTK_CGEN_AP_Editor_DB
	$(hide) cp -f $(MTK_CGEN_AP_Editor_DB) $(zip_root)/
ifdef MTK_CGEN_AP_Editor_DB_Enum_File
	$(hide) cp -f $(MTK_CGEN_AP_Editor_DB_Enum_File) $(zip_root)/
endif
endif
ifneq ($(strip $(MTK_SPLIT_VENDOR_DIST_FILES)),)
	$(hide) cp -f $(MTK_SPLIT_VENDOR_DIST_FILES) $(zip_root)/
endif
	$(hide) mkdir -p $(patsubst $(PRODUCT_OUT)/%,$(zip_root)/%,$(TARGET_OUT_TESTCASES))
	$(hide) $(call package_files-copy-root, \
	    $(TARGET_OUT_TESTCASES),$(patsubst $(PRODUCT_OUT)/%,$(zip_root)/%,$(TARGET_OUT_TESTCASES)))
	$(hide) find $(zip_root) -print | sort >$(zip_root).list
	$(hide) $(SOONG_ZIP) -d -o $@ -C $(zip_root) -l $(zip_root).list


MTK_SPLIT_VENDOR_OTATOOLS :=

MTK_SPLIT_VENDOR_OTATOOLS_DEPS := \
  $(BOARD_AVB_KEY_PATH) \
  $(BOARD_AVB_BOOT_KEY_PATH) \
  $(BOARD_AVB_RECOVERY_KEY_PATH) \
  $(BOARD_AVB_VENDOR_KEY_PATH) \
  $(BOARD_AVB_VBMETA_VENDOR_KEY_PATH) \
  $(wildcard vendor/mediatek/proprietary/custom/$(MTK_PLATFORM_DIR)/security/cert_config/*.txt) \
  $(wildcard vendor/mediatek/proprietary/custom/$(MTK_PLATFORM_DIR)/security/cert_config/cert1/*.der) \
  $(wildcard vendor/mediatek/proprietary/custom/$(MTK_PLATFORM_DIR)/security/cert_config/cert2_key/*.pem) \
  $(wildcard vendor/mediatek/proprietary/custom/$(MTK_BASE_PROJECT)/security/image_auth/*.ini)

$(MTK_SPLIT_IMAGES_DIR)/vnd.otatools.zip: zip_root := $(call intermediates-dir-for,PACKAGING,vendor_otatools)/vendor_otatools
ifeq ($(TARGET_USERIMAGES_USE_MNTL), true)
$(MTK_SPLIT_IMAGES_DIR)/vnd.otatools.zip: $(HOST_OUT_EXECUTABLES)/mkmntl$(HOST_EXECUTABLE_SUFFIX)
endif
$(MTK_SPLIT_IMAGES_DIR)/vnd.otatools.zip: $(MTK_SPLIT_VENDOR_OTATOOLS) $(MTK_SPLIT_VENDOR_OTATOOLS_DEPS) $(SOONG_ZIP)
	$(hide) rm -rf $@ $(zip_root)
	$(hide) mkdir -p $(dir $@) $(zip_root)/releasetools
	$(call copy-files-with-structure,$(MTK_SPLIT_VENDOR_OTATOOLS),,$(zip_root))
ifeq ($(TARGET_USERIMAGES_USE_MNTL), true)
	$(hide) mkdir -p $(zip_root)/bin
	$(hide) cp -f $(HOST_OUT_EXECUTABLES)/mkmntl$(HOST_EXECUTABLE_SUFFIX) $(zip_root)/bin/
endif
	$(hide) $(SOONG_ZIP) -o $@ -C $(zip_root) -D $(zip_root) \
	  -C . $(addprefix -f ,$(MTK_SPLIT_VENDOR_OTATOOLS_DEPS))

$(MTK_SPLIT_IMAGES_DIR)/ProjectConfig.mk: $(MTK_TARGET_PROJECT_FOLDER)/ProjectConfig.mk
	@mkdir -p $(dir $@)
	$(hide) cp -f $< $@

.PHONY: vnd_images mtk_images
vnd_images: $(MTK_SPLIT_IMAGES_DIR)/vnd.target_files.zip
vnd_images: $(MTK_SPLIT_IMAGES_DIR)/vnd.dist_files.zip
vnd_images: $(MTK_SPLIT_IMAGES_DIR)/vnd.otatools.zip
vnd_images: $(MTK_SPLIT_IMAGES_DIR)/vnd.rsc.xml
vnd_images: $(MTK_SPLIT_IMAGES_DIR)/usr.rsc.xml
vnd_images: $(MTK_SPLIT_IMAGES_DIR)/ProjectConfig.mk
	@echo $@: $^

# required by sign_target_files_apks
#vnd_images: $(INTERNAL_USERIMAGES_DEPS)

mtk_images: $(MTK_SPLIT_IMAGES_DIR)/ProjectConfig.mk

