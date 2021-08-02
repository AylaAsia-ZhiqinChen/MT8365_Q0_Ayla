.PHONY: superimage
superimage:

ifeq (no,$(MTK_BSP_PACKAGE))

ifeq (true,$(PRODUCT_BUILD_SUPER_PARTITION))
ifneq ($(BOARD_SUPER_PARTITION_SIZE),)
ifneq (true,$(PRODUCT_RETROFIT_DYNAMIC_PARTITIONS))
ifeq (true,$(BOARD_BUILD_SUPER_IMAGE_BY_DEFAULT))

.PHONY: otapackage mtksuperimage
otapackage: mtksuperimage
mtksuperimage: $(BUILT_TARGET_FILES_PACKAGE)
	$(call build-superimage-target,$(INSTALLED_SUPERIMAGE_TARGET),\
           $(call intermediates-dir-for,PACKAGING,superimage_debug)/misc_info.txt)

endif
endif
endif
endif
endif
