
ifeq (yes, $(strip $(MTK_RECOVERY_RAMDISK_SPLIT)))

### recovery-vendor.img
MTK_RECOVERY_VENDOR_IMAGE := $(PRODUCT_OUT)/recovery-vendor.img
.PHONY: mtk_recovery_vendor_image
droidcore: mtk_recovery_vendor_image
mtk_recovery_vendor_image: $(MTK_RECOVERY_VENDOR_IMAGE)

TARGET_OUT_RECOVERY_VENDOR := $(PRODUCT_OUT)/recovery-vendor
MTK_RECOVERY_VENDOR_IMAGE_FILES := $(filter $(TARGET_OUT_RECOVERY_VENDOR)/%,$(ALL_DEFAULT_INSTALLED_MODULES))

MTK_RECOVERY_KERNEL_TARGET := $(call intermediates-dir-for,PACKAGING,recovery-mtk)/kernel
$(MTK_RECOVERY_KERNEL_TARGET):
	$(hide) mkdir -p $(dir $@)
	$(hide) touch $@

$(MTK_RECOVERY_VENDOR_IMAGE): mtk_recovery_intermediates := $(call intermediates-dir-for,PACKAGING,recovery-mtk)
$(MTK_RECOVERY_VENDOR_IMAGE): mtk_recovery_kernel_target := $(MTK_RECOVERY_KERNEL_TARGET)
$(MTK_RECOVERY_VENDOR_IMAGE): $(MKBOOTFS) $(MKBOOTIMG) $(MTK_RECOVERY_VENDOR_IMAGE_FILES) $(MTK_RECOVERY_KERNEL_TARGET) $(recovery_fstab) $(INSTALLED_DEFAULT_PROP_TARGET) | $(MINIGZIP)
	$(call pretty,"Target mtk_recovery_vendor_image: $@")
	$(hide) mkdir -p $(mtk_recovery_intermediates)
	$(hide) mkdir -p $(TARGET_OUT_RECOVERY_VENDOR)/etc
	$(hide) cp -f $(recovery_fstab) $(TARGET_OUT_RECOVERY_VENDOR)/etc/recovery.fstab
	$(hide) cat $(PRODUCT_OUT)/system/etc/prop.default > $(TARGET_OUT_RECOVERY_VENDOR)/default.prop
	$(hide) $(MKBOOTFS) -d $(TARGET_OUT) $(TARGET_OUT_RECOVERY_VENDOR) | $(MINIGZIP) > $(mtk_recovery_intermediates)/ramdisk-vendor.img
	$(hide) $(MKBOOTIMG) --kernel $(mtk_recovery_kernel_target) --ramdisk $(mtk_recovery_intermediates)/ramdisk-vendor.img --output $@


### recovery-ramdisk.img
MTK_RECOVERY_RAMDISK_IMAGE := $(PRODUCT_OUT)/recovery-ramdisk.img
.PHONY: mtk_recovery_ramdisk_image
droidcore: mtk_recovery_ramdisk_image
mtk_recovery_ramdisk_image: $(MTK_RECOVERY_RAMDISK_IMAGE)

TARGET_OUT_RECOVERY_RAMDISK := $(PRODUCT_OUT)/recovery-ramdisk
$(MTK_RECOVERY_RAMDISK_IMAGE): mtk_recovery_kernel_target := $(MTK_RECOVERY_KERNEL_TARGET)
$(MTK_RECOVERY_RAMDISK_IMAGE): $(MKBOOTFS) $(MKBOOTIMG) $(INSTALLED_RECOVERYIMAGE_TARGET) $(MTK_RECOVERY_KERNEL_TARGET) | $(MINIGZIP)
	$(call pretty,"Target mtk_recovery_ramdisk_image: $@")
	$(hide) mkdir -p $(TARGET_OUT_RECOVERY_RAMDISK)
	$(hide) rsync -a $(TARGET_RECOVERY_OUT) $(TARGET_OUT_RECOVERY_RAMDISK)
	$(hide) rm -f $(TARGET_RECOVERY_ROOT_OUT)/etc/recovery.fstab
	$(hide) $(MKBOOTFS) -d $(TARGET_OUT) $(TARGET_RECOVERY_ROOT_OUT) | $(MINIGZIP) > $(TARGET_OUT_RECOVERY_RAMDISK)/ramdisk-recovery.img
	$(hide) $(MKBOOTIMG) --kernel $(mtk_recovery_kernel_target) --ramdisk $(TARGET_OUT_RECOVERY_RAMDISK)/ramdisk-recovery.img $(BOARD_MKBOOTIMG_ARGS) --base $(BOARD_KERNEL_BASE) --output $@


### recovery-kernel.img
MTK_RECOVERY_KERNEL_IMAGE := $(PRODUCT_OUT)/recovery-kernel.img
.PHONY: mtk_recovery_kernel_image
droidcore: mtk_recovery_kernel_image
mtk_recovery_kernel_image: $(MTK_RECOVERY_KERNEL_IMAGE)

ifeq (true,$(BOARD_AVB_ENABLE))
$(MTK_RECOVERY_KERNEL_IMAGE): $(AVBTOOL) $(BOARD_AVB_BOOT_KEY_PATH)
endif

$(MTK_RECOVERY_KERNEL_IMAGE): mtk_recovery_intermediates := $(call intermediates-dir-for,PACKAGING,recovery-mtk)
$(MTK_RECOVERY_KERNEL_IMAGE): $(MKBOOTIMG) $(INSTALLED_KERNEL_TARGET)
	$(call pretty,"Target mtk_recovery_kernel_image: $@")
	$(hide) $(MKBOOTIMG) --kernel $(INSTALLED_KERNEL_TARGET) $(BOARD_MKBOOTIMG_ARGS) --base $(BOARD_KERNEL_BASE) --cmdline $(BOARD_KERNEL_CMDLINE) --output $@
	$(if $(filter true,$(BOARD_AVB_ENABLE)), \
            $(hide) $(AVBTOOL) add_hash_footer --image $@ --partition_size $(BOARD_RECOVERYIMAGE_PARTITION_SIZE) --partition_name recovery $(INTERNAL_AVB_RECOVERY_SIGNING_ARGS) $(BOARD_AVB_RECOVERY_ADD_HASH_FOOTER_ARGS))


endif ### MTK_RECOVERY_RAMDISK_SPLIT=yes
