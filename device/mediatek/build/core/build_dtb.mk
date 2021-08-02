ifneq ($(words $(LOCAL_SRC_FILES)),1)
$(error LOCAL_SRC_FILES $(LOCAL_SRC_FILES) should be one file)
endif

ifneq (true,$(strip $(TARGET_NO_KERNEL)))
ifneq ($(LINUX_KERNEL_VERSION),)

ifeq (,$(KERNEL_OUT))
include $(LINUX_KERNEL_VERSION)/kenv.mk
endif

my_kernel_dts := $(KERNEL_OUT)/arch/$(KERNEL_TARGET_ARCH)/boot/dts/$(LOCAL_SRC_FILES)
$(my_kernel_dts): $(LOCAL_SRC_FILES)
	$(hide) mkdir -p $(dir $@)
	$(hide) cp -f $< $@

my_kernel_dtb_stem := $(patsubst %.dts,%.dtb,$(LOCAL_SRC_FILES))
my_kernel_dtb := $(KERNEL_OUT)/arch/$(KERNEL_TARGET_ARCH)/boot/dts/$(my_kernel_dtb_stem)

$(my_kernel_dtb): KOUT := $(KERNEL_OUT)
$(my_kernel_dtb): OPTS := $(KERNEL_MAKE_OPTION) $(my_kernel_dtb_stem)
$(my_kernel_dtb): $(KERNEL_ZIMAGE_OUT) $(my_kernel_dts) $(LOCAL_ADDITIONAL_DEPENDENCIES)
	$(PREBUILT_MAKE_PREFIX)$(MAKE) -C $(KOUT) $(OPTS)

#UFDT_TOOL := $(KERNEL_OUT)/scripts/dtc/ufdt_apply_overlay
#$(my_kernel_dtb).merge: $(UFDT_TOOL) $(1) $(my_kernel_dtb)
#	$(UFDT_TOOL) $(1) $(my_kernel_dtb) $@

MTK_DTBOIMAGE_DTB := $(strip $(MTK_DTBOIMAGE_DTB) $(my_kernel_dtb))

endif#TARGET_NO_KERNEL
endif#LINUX_KERNEL_VERSION

LOCAL_ADDITIONAL_DEPENDENCIES :=
LOCAL_SRC_FILES :=
