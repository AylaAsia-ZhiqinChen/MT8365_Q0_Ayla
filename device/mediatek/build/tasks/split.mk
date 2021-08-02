# namespace
MTK_SPLIT_IMAGES_DIR := $(PRODUCT_OUT)/images
MTK_SPLIT_IMAGES_PREFIX :=
ifdef SYS_TARGET_PROJECT
  MTK_SPLIT_IMAGES_PREFIX += sys.
endif
ifdef MTK_TARGET_PROJECT
  MTK_SPLIT_IMAGES_PREFIX += vnd.
  ifneq ($(wildcard vendor/mediatek/kernel_modules),)
    ifneq ($(wildcard $(LINUX_KERNEL_VERSION)),)
      MTK_SPLIT_IMAGES_PREFIX += krn.
    endif
  endif
endif

include device/mediatek/build/tasks/tools/split_system.mk device/mediatek/build/tasks/tools/split_vendor.mk device/mediatek/build/tasks/tools/split_kernel.mk

# RSC
$(MTK_SPLIT_IMAGES_DIR)/sys.rsc.xml: $(call intermediates-dir-for,ETC,rsc.xml)/sys.rsc.xml
	@mkdir -p $(dir $@)
	$(hide) cp -f $< $@

$(MTK_SPLIT_IMAGES_DIR)/vnd.rsc.xml: $(call intermediates-dir-for,ETC,rsc.xml)/vnd.rsc.xml
	@mkdir -p $(dir $@)
	$(hide) cp -f $< $@

$(MTK_SPLIT_IMAGES_DIR)/krn.rsc.xml: $(call intermediates-dir-for,ETC,rsc.xml)/krn.rsc.xml
	@mkdir -p $(dir $@)
	$(hide) cp -f $< $@

$(MTK_SPLIT_IMAGES_DIR)/usr.rsc.xml: $(call intermediates-dir-for,ETC,rsc.xml)/usr.rsc.xml
	@mkdir -p $(dir $@)
	$(hide) cp -f $< $@

.PHONY: mtk_images
mtk_images: $(foreach n,$(MTK_SPLIT_IMAGES_PREFIX),$(MTK_SPLIT_IMAGES_DIR)/$(n)target_files.zip)
mtk_images: $(foreach n,$(filter vnd.,$(MTK_SPLIT_IMAGES_PREFIX)),$(MTK_SPLIT_IMAGES_DIR)/$(n)dist_files.zip)
mtk_images: $(foreach n,$(MTK_SPLIT_IMAGES_PREFIX) usr.,$(MTK_SPLIT_IMAGES_DIR)/$(n)rsc.xml)
mtk_images: $(foreach n,$(filter sys. vnd.,$(MTK_SPLIT_IMAGES_PREFIX)),$(MTK_SPLIT_IMAGES_DIR)/$(n)otatools.zip)
	@echo $@: $^

