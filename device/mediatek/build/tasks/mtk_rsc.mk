# check-rsc-config
intermediates := $(call intermediates-dir-for,FAKE,check-rsc-config)
$(intermediates)/rsc.txt: PRIVATE_COMMAND := $(if $(filter yes,$(DISABLE_MTK_CONFIG_CHECK)),-)python device/mediatek/build/build/tools/check_kernel_config.py -r $(intermediates)/rsc.txt -k $(KERNEL_CONFIG_FILE) -p $(MTK_PROJECT_NAME)
$(intermediates)/rsc.txt: $(MTK_RSC_MAKEFILES) $(KERNEL_CONFIG_FILE) device/mediatek/build/build/tools/check_kernel_config.py
	@mkdir -p $(dir $@)
	@echo PRODUCT_MTK_RSC_NAMES=$(strip $(CURRENT_RSC_NAMES)) >$@
	$(PRIVATE_COMMAND)
