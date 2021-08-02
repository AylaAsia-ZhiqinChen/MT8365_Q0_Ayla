
ifeq (user, $(strip $(TARGET_BUILD_VARIANT)))

# root lk.img
TARGET_OUT_MTKROOT := $(PRODUCT_OUT)/mtk_root_lk
TARGET_LKROOT_OUT := $(TARGET_OUT_INTERMEDIATES)/BOOTLOADER_ROOT_OBJ
TARGET_LKROOT_ROOT_OUT := $(if $(filter /% ~%,$(TARGET_LKROOT_OUT)),,$(LK_ROOT_DIR)/)$(TARGET_LKROOT_OUT)

BUILT_MTK_LKROOT_FILES := $(TARGET_LKROOT_OUT)/build-$(LK_PROJECT)/lk.img
INSTALLED_MTK_LKROOT_FILES := $(TARGET_OUT_MTKROOT)/lk.img

LK_MAKE_OPTION_ROOT := $(if $(SHOW_COMMANDS),NOECHO=) $(if $(LK_CROSS_COMPILE),TOOLCHAIN_PREFIX=$(LK_CROSS_COMPILE)) BOOTLOADER_OUT=$(TARGET_LKROOT_ROOT_OUT) ROOTDIR=$(LK_ROOT_DIR) MTK_BUILD_ROOT=yes

.KATI_RESTAT: $(BUILT_MTK_LKROOT_FILES)
$(BUILT_MTK_LKROOT_FILES): $(LK_MAKE_DEPENDENCIES)
	$(hide) mkdir -p $(dir $@)
	$(MAKE) -C $(LK_DIR) $(LK_MAKE_OPTION_ROOT) $(LK_PROJECT)


$(INSTALLED_MTK_LKROOT_FILES): $(BUILT_MTK_LKROOT_FILES)
	$(hide) mkdir -p $(dir $@)
	$(hide) cp -f $< $@

.PHONY: mtk_root_lk
mtk_root_lk: $(INSTALLED_MTK_LKROOT_FILES)
ifeq (yes, $(strip $(MTK_BUILD_ROOT)))
droidcore: mtk_root_lk
endif


endif ### user mode



ifeq (eng, $(strip $(TARGET_BUILD_VARIANT)))

### root boot.img
MTK_ROOT_BOOT_IMAGE := $(PRODUCT_OUT)/mtk_root_boot.img
MTK_ROOT_BOOT_IMAGE_FILES := $(PRODUCT_OUT)/kernel_root
INSTALLED_KERNELROOT_TARGET := $(MTK_ROOT_BOOT_IMAGE_FILES)

KERNELROOT_OUT ?= $(TARGET_OUT_INTERMEDIATES)/KERNEL_ROOT_OBJ
KERNELROOT_ROOT_OUT := $(if $(filter /% ~%,$(KERNELROOT_OUT)),,$(KERNEL_ROOT_DIR)/)$(KERNELROOT_OUT)
KERNELROOT_MAKE_OPTION := O=$(KERNELROOT_ROOT_OUT) ARCH=$(KERNEL_TARGET_ARCH) TARGET_BUILD_VARIANT=user CROSS_COMPILE=$(TARGET_KERNEL_CROSS_COMPILE) $(TARGET_KERNEL_CLANG_COMPILE) ROOTDIR=$(KERNEL_ROOT_DIR) $(if $(strip $(SHOW_COMMANDS)),V=1)
ifeq ($(KERNEL_TARGET_ARCH), arm64)
  ifeq ($(MTK_APPENDED_DTB_SUPPORT), yes)
    KERNELROOT_ZIMAGE_OUT := $(KERNELROOT_OUT)/arch/$(KERNEL_TARGET_ARCH)/boot/Image.gz-dtb
  else
    KERNELROOT_ZIMAGE_OUT := $(KERNELROOT_OUT)/arch/$(KERNEL_TARGET_ARCH)/boot/Image.gz
  endif
else
  ifeq ($(MTK_APPENDED_DTB_SUPPORT), yes)
    KERNELROOT_ZIMAGE_OUT := $(KERNELROOT_OUT)/arch/$(KERNEL_TARGET_ARCH)/boot/zImage-dtb
  else
    KERNELROOT_ZIMAGE_OUT := $(KERNELROOT_OUT)/arch/$(KERNEL_TARGET_ARCH)/boot/zImage
  endif
endif
BUILT_KERNELROOT_TARGET := $(KERNELROOT_ZIMAGE_OUT).bin

KERNELROOT_DEFCONFIG := $(strip $(subst _debug_defconfig,_defconfig,$(KERNEL_DEFCONFIG)))

TARGET_KERNELROOT_CONFIG := $(KERNELROOT_OUT)/.config
$(TARGET_KERNELROOT_CONFIG): $(KERNEL_MAKE_DEPENDENCIES)
	$(hide) mkdir -p $(dir $@)
	$(MAKE) -C $(KERNEL_DIR) $(KERNELROOT_MAKE_OPTION) $(KERNELROOT_DEFCONFIG)
	$(hide) echo "CONFIG_MTK_ROOT_DEVICE=y" >> $@
	$(MAKE) -C $(KERNEL_DIR) $(KERNELROOT_MAKE_OPTION) oldconfig

.KATI_RESTAT: $(KERNELROOT_ZIMAGE_OUT)
$(KERNELROOT_ZIMAGE_OUT): $(TARGET_KERNELROOT_CONFIG) $(KERNEL_MAKE_DEPENDENCIES)
	$(hide) mkdir -p $(dir $@)
	$(MAKE) -C $(KERNEL_DIR) $(KERNELROOT_MAKE_OPTION)
	$(hide) $(call fixup-kernel-cmd-file,$(KERNELROOT_OUT)/arch/$(KERNEL_TARGET_ARCH)/boot/compressed/.piggy.xzkern.cmd)


ifeq ($(strip $(MTK_HEADER_SUPPORT)), yes)
$(BUILT_KERNELROOT_TARGET): $(KERNELROOT_ZIMAGE_OUT) $(TARGET_KERNELROOT_CONFIG) $(LINUX_KERNEL_VERSION)/Android.mk | $(HOST_OUT_EXECUTABLES)/mkimage$(HOST_EXECUTABLE_SUFFIX)
	$(hide) $(HOST_OUT_EXECUTABLES)/mkimage$(HOST_EXECUTABLE_SUFFIX) $< KERNEL 0xffffffff > $@
else
$(BUILT_KERNELROOT_TARGET): $(KERNELROOT_ZIMAGE_OUT) $(TARGET_KERNELROOT_CONFIG) $(LINUX_KERNEL_VERSION)/Android.mk | $(ACP)
	$(copy-file-to-target)
endif

$(INSTALLED_KERNELROOT_TARGET): $(BUILT_KERNELROOT_TARGET) $(LINUX_KERNEL_VERSION)/Android.mk | $(ACP)
	$(copy-file-to-target)


$(MTK_ROOT_BOOT_IMAGE): $(MKBOOTIMG) $(AVBTOOL) $(MTK_ROOT_BOOT_IMAGE_FILES) $(BOARD_AVB_BOOT_KEY_PATH) $(TARGET_KERNELROOT_CONFIG)
	$(call pretty,"Target mkt_root_boot image: $@")
	$(hide) $(MKBOOTIMG) --kernel $(MTK_ROOT_BOOT_IMAGE_FILES) --base $(BOARD_KERNEL_BASE) --cmdline "$(INTERNAL_KERNEL_CMDLINE)" $(INTERNAL_MKBOOTIMG_VERSION_ARGS) $(BOARD_MKBOOTIMG_ARGS) --output $@
	$(hide) $(call assert-max-image-size,$@,$(call get-hash-image-max-size,$(BOARD_BOOTIMAGE_PARTITION_SIZE)))
	$(hide) $(AVBTOOL) add_hash_footer \
          --image $@ \
          --partition_size $(BOARD_BOOTIMAGE_PARTITION_SIZE) \
          --partition_name boot $(INTERNAL_AVB_BOOT_SIGNING_ARGS) \
          $(BOARD_AVB_BOOT_ADD_HASH_FOOTER_ARGS)

.PHONY: mtk_root_boot
mtk_root_boot: $(MTK_ROOT_BOOT_IMAGE) $(INSTALLED_KERNELROOT_TARGET)
ifeq (yes, $(strip $(MTK_BUILD_ROOT)))
droidcore: mtk_root_boot
endif



### root image --> eng_system.img
MTK_ENG_SYSTEM_IMAGE := $(PRODUCT_OUT)/eng_system.img
ifeq (yes, $(strip $(MTK_BUILD_ROOT)))
droidcore: mtk_eng_system_image
endif
.PHONY: mtk_eng_system_image
mtk_eng_system_image: $(MTK_ENG_SYSTEM_IMAGE)

TARGET_OUT_MTK_ENG_SYSTEM := $(PRODUCT_OUT)/eng_system

mtk_eng_system_intermediates := $(call intermediates-dir-for,PACKAGING,mtk_eng_system)
mtk_eng_system_mount_point := system
mtk_eng_system_partition_size := $(BOARD_ENG_SYSTEMIMAGE_PARTITION_SIZE)
mtk_eng_system_staging_dir := $(mtk_eng_system_intermediates)/$(mtk_eng_system_mount_point)
mtk_eng_system_extfs_sparse_flag := $(INTERNAL_USERIMAGES_SPARSE_EXT_FLAG)

$(MTK_ENG_SYSTEM_IMAGE): PRIVATE_INTERMEDIATES := $(mtk_eng_system_intermediates)
$(MTK_ENG_SYSTEM_IMAGE): PRIVATE_MOUNT_POINT := $(mtk_eng_system_mount_point)
$(MTK_ENG_SYSTEM_IMAGE): PRIVATE_PARTITION_SIZE := $(mtk_eng_system_partition_size)
$(MTK_ENG_SYSTEM_IMAGE): PRIVATE_FILE_SYSTEM_TYPE := ext4
$(MTK_ENG_SYSTEM_IMAGE): PRIVATE_EXTFS_SPARSE_FLAG := $(mtk_eng_system_extfs_sparse_flag)

$(MTK_ENG_SYSTEM_IMAGE): $(TARGET_ROOT_OUT)/init $(TARGET_OUT)/bin/adbd $(TARGET_OUT_FAKE)/selinux_policy-timestamp $(INSTALLED_DEFAULT_PROP_TARGET) $(INTERNAL_USERIMAGES_DEPS)
	$(call pretty,"Target mtk_eng_system_image image: $@")
	$(hide) mkdir -p $(TARGET_OUT_MTK_ENG_SYSTEM)/root/system
	$(hide) mkdir -p $(TARGET_OUT_MTK_ENG_SYSTEM)/root/vendor
	$(hide) mkdir -p $(TARGET_OUT_MTK_ENG_SYSTEM)/system/etc
	$(hide) mkdir -p $(TARGET_OUT_MTK_ENG_SYSTEM)/system/bin
	$(hide) rsync -a $(TARGET_ROOT_OUT)/init $(TARGET_OUT_MTK_ENG_SYSTEM)/root
	$(hide) rsync -a $(TARGET_OUT)/etc/prop.default $(TARGET_OUT_MTK_ENG_SYSTEM)/system/etc
	$(hide) rsync -a $(TARGET_OUT)/etc/selinux $(TARGET_OUT_MTK_ENG_SYSTEM)/system/etc
	$(hide) rsync -a $(TARGET_OUT)/bin/adbd $(TARGET_OUT_MTK_ENG_SYSTEM)/system/bin
	$(hide) mkdir -p $(PRIVATE_INTERMEDIATES)
	$(hide) echo "# For all accepted properties, see BuildImage() in tools/releasetools/build_image.py" > $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "mount_point=$(PRIVATE_MOUNT_POINT)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "partition_name=$(PRIVATE_MOUNT_POINT)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "fs_type=$(PRIVATE_FILE_SYSTEM_TYPE)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "partition_size=$(PRIVATE_PARTITION_SIZE)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "ext_mkuserimg=$(notdir $(MKEXTUSERIMG))" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "extfs_sparse_flag=$(PRIVATE_EXTFS_SPARSE_FLAG)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "selinux_fc=$(SELINUX_FC)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "system_root_image=true" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "ramdisk_dir=$(TARGET_OUT_MTK_ENG_SYSTEM)/root" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) PATH=$(foreach p,$(INTERNAL_USERIMAGES_BINARY_PATHS),$(p):)$$PATH \
          build/make/tools/releasetools/build_image.py \
          $(TARGET_OUT_MTK_ENG_SYSTEM)/system $(PRIVATE_INTERMEDIATES)/image_info.txt $@ $(TARGET_OUT_MTK_ENG_SYSTEM)


### root image --> eng_vendor.img
MTK_ENG_VENDOR_IMAGE := $(PRODUCT_OUT)/eng_vendor.img
ifeq (yes, $(strip $(MTK_BUILD_ROOT)))
droidcore: mtk_eng_vendor_image
endif
.PHONY: mtk_eng_vendor_image
mtk_eng_vendor_image: $(MTK_ENG_VENDOR_IMAGE)

TARGET_OUT_MTK_ENG_VENDOR := $(PRODUCT_OUT)/eng_vendor

mtk_eng_vendor_intermediates := $(call intermediates-dir-for,PACKAGING,mtk_eng_vendor)
mtk_eng_vendor_mount_point := vendor
mtk_eng_vendor_partition_size := $(BOARD_ENG_VENDORIMAGE_PARTITION_SIZE)
mtk_eng_vendor_extfs_sparse_flag := $(INTERNAL_USERIMAGES_SPARSE_EXT_FLAG)

$(MTK_ENG_VENDOR_IMAGE): PRIVATE_INTERMEDIATES := $(mtk_eng_vendor_intermediates)
$(MTK_ENG_VENDOR_IMAGE): PRIVATE_MOUNT_POINT := $(mtk_eng_vendor_mount_point)
$(MTK_ENG_VENDOR_IMAGE): PRIVATE_PARTITION_SIZE := $(mtk_eng_vendor_partition_size)
$(MTK_ENG_VENDOR_IMAGE): PRIVATE_FILE_SYSTEM_TYPE := ext4
$(MTK_ENG_VENDOR_IMAGE): PRIVATE_STAGING_DIR := $(mtk_eng_vendor_staging_dir)
$(MTK_ENG_VENDOR_IMAGE): PRIVATE_EXTFS_SPARSE_FLAG := $(mtk_eng_vendor_extfs_sparse_flag)

$(MTK_ENG_VENDOR_IMAGE): $(INSTALLED_VENDOR_DEFAULT_PROP_TARGET) $(TARGET_OUT_FAKE)/selinux_policy-timestamp $(INTERNAL_USERIMAGES_DEPS)
	$(call pretty,"Target mtk_eng_vendor_image image: $@")
	$(hide) mkdir -p $(TARGET_OUT_MTK_ENG_VENDOR)/etc
	$(hide) mkdir -p $(TARGET_OUT_MTK_ENG_VENDOR)/lib
	$(hide) rsync -a $(TARGET_OUT_VENDOR)/default.prop $(TARGET_OUT_MTK_ENG_VENDOR)
	$(hide) rsync -a $(TARGET_OUT_VENDOR)/etc/selinux $(TARGET_OUT_MTK_ENG_VENDOR)/etc
	$(hide) mkdir -p $(PRIVATE_INTERMEDIATES)
	$(hide) echo "# For all accepted properties, see BuildImage() in tools/releasetools/build_image.py" > $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "mount_point=$(PRIVATE_MOUNT_POINT)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "partition_name=$(PRIVATE_MOUNT_POINT)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "fs_type=$(PRIVATE_FILE_SYSTEM_TYPE)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "partition_size=$(PRIVATE_PARTITION_SIZE)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "ext_mkuserimg=$(notdir $(MKEXTUSERIMG))" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "extfs_sparse_flag=$(PRIVATE_EXTFS_SPARSE_FLAG)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) echo "selinux_fc=$(SELINUX_FC)" >> $(PRIVATE_INTERMEDIATES)/image_info.txt
	$(hide) PATH=$(foreach p,$(INTERNAL_USERIMAGES_BINARY_PATHS),$(p):)$$PATH \
          build/make/tools/releasetools/build_image.py \
          $(TARGET_OUT_MTK_ENG_VENDOR) $(PRIVATE_INTERMEDIATES)/image_info.txt $@ $(TARGET_OUT_MTK_ENG_VENDOR)


### eng mode prop
ifeq (yes, $(strip $(MTK_BUILD_ROOT)))
droidcore: mtk_eng_prop
endif
.PHONY: mtk_eng_prop
mtk_eng_prop: $(INSTALLED_DEFAULT_PROP_TARGET) $(INSTALLED_VENDOR_DEFAULT_PROP_TARGET)


endif ### eng mode

