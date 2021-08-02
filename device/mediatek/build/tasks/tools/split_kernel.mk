
name := $(TARGET_PRODUCT)-kernel_target_files-$(FILE_NAME_TAG)
intermediates := $(call intermediates-dir-for,PACKAGING,kernel_target_files)
$(MTK_SPLIT_IMAGES_DIR)/krn.target_files.zip: intermediates := $(intermediates)
$(MTK_SPLIT_IMAGES_DIR)/krn.target_files.zip: zip_root := $(intermediates)/$(name)
ifeq ($(BOARD_USES_RECOVERY_AS_BOOT),true)
$(MTK_SPLIT_IMAGES_DIR)/krn.target_files.zip: PRIVATE_RECOVERY_OUT := BOOT
else
$(MTK_SPLIT_IMAGES_DIR)/krn.target_files.zip: PRIVATE_RECOVERY_OUT := RECOVERY
endif
ifdef BOARD_PREBUILT_DTBOIMAGE
ifeq ($(strip $(MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET)),)
$(warning MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET is not defined)
endif
endif
$(MTK_SPLIT_IMAGES_DIR)/krn.target_files.zip: \
	    $(filter $(TARGET_OUT_VENDOR)/lib/modules/%,$(INTERNAL_VENDORIMAGE_FILES)) \
	    $(INSTALLED_KERNEL_TARGET) \
	    $(INSTALLED_DTBIMAGE_TARGET) \
	    $(INSTALLED_DTBOIMAGE_TARGET) \
	    $(MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET) \
	    $(BOARD_PREBUILT_DTBOIMAGE) \
	    $(BOARD_PREBUILT_RECOVERY_DTBOIMAGE) \
	    $(SOONG_ZIP) \
	    | $(ACP)
	@echo "Package target files: $@"
	$(hide) rm -rf $@ $(zip_root).list $(zip_root)
	$(hide) mkdir -p $(dir $@) $(zip_root)
ifneq (,$(INSTALLED_RECOVERYIMAGE_TARGET)$(filter true,$(BOARD_USES_RECOVERY_AS_BOOT)))
	@# Components of the recovery image
	$(hide) mkdir -p $(zip_root)/$(PRIVATE_RECOVERY_OUT)
ifdef INSTALLED_KERNEL_TARGET
	$(hide) cp $(INSTALLED_KERNEL_TARGET) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/kernel
endif
ifdef BOARD_INCLUDE_RECOVERY_DTBO
ifdef BOARD_PREBUILT_RECOVERY_DTBOIMAGE
	$(hide) cp $(BOARD_PREBUILT_RECOVERY_DTBOIMAGE) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/recovery_dtbo
else
	$(hide) cp $(BOARD_PREBUILT_DTBOIMAGE) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/recovery_dtbo
endif
endif
ifdef BOARD_INCLUDE_DTB_IN_BOOTIMG
	$(hide) cp $(INSTALLED_DTBIMAGE_TARGET) $(zip_root)/$(PRIVATE_RECOVERY_OUT)/dtb
endif
endif # INSTALLED_RECOVERYIMAGE_TARGET defined or BOARD_USES_RECOVERY_AS_BOOT is true
	@# Components of the boot image
	$(hide) mkdir -p $(zip_root)/BOOT
	@# If we are using recovery as boot, this is already done when processing recovery.
ifneq ($(BOARD_USES_RECOVERY_AS_BOOT),true)
ifdef INSTALLED_KERNEL_TARGET
	$(hide) cp $(INSTALLED_KERNEL_TARGET) $(zip_root)/BOOT/kernel
endif
ifdef INSTALLED_DTBIMAGE_TARGET
	$(hide) cp $(INSTALLED_DTBIMAGE_TARGET) $(zip_root)/BOOT/dtb
endif
endif # BOARD_USES_RECOVERY_AS_BOOT
ifdef BUILDING_VENDOR_IMAGE
	@# Contents of the vendor image
	$(hide) $(call package_files-copy-root, \
	    $(TARGET_OUT_VENDOR)/lib/modules,$(zip_root)/VENDOR/lib/modules)
endif
ifdef BOARD_PREBUILT_DTBOIMAGE
	$(hide) mkdir -p $(zip_root)/PREBUILT_IMAGES
ifdef MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET
	$(hide) cp $(patsubst %.img, %-verified.img, $(INSTALLED_DTBOIMAGE_TARGET)) $(zip_root)/PREBUILT_IMAGES/dtbo.img
endif
endif # BOARD_PREBUILT_DTBOIMAGE
	$(hide) find $(zip_root) -print | sort >$(zip_root).list
	$(hide) $(SOONG_ZIP) -d -o $@ -C $(zip_root) -l $(zip_root).list

.PHONY: krn_images
krn_images: $(MTK_SPLIT_IMAGES_DIR)/krn.target_files.zip
krn_images: $(MTK_SPLIT_IMAGES_DIR)/krn.rsc.xml
	@echo $@: $^

