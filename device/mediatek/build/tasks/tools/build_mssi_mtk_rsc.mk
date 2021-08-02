
mssi_mtk_rsc_parameter_variables := \
	MTK_RSC_APKS \
	MTK_RSC_MODULES \
	MTK_RSC_COPY_FILES \
	MTK_RSC_SYSTEM_PROPERTIES \
	MTK_RSC_VENDOR_PROPERTIES \
	MTK_RSC_XML_OPTR \


$(call clear-var-list, $(mssi_mtk_rsc_parameter_variables))

MSSI_MTK_RSC_MAKEFILE := $(firstword $(wildcard $(SYS_TARGET_PROJECT_FOLDER)/rsc/$(mssi_mtk_rsc_name)/RuntimeSwitch.mk $(SYS_PROJECT_FOLDER)/rsc/$(mssi_mtk_rsc_name)/RuntimeSwitch.mk))
ifndef MSSI_MTK_RSC_MAKEFILE
$(error Fail to find RuntimeSwitch.mk for $(mssi_mtk_rsc_name))
endif

MSSI_MTK_RSC_LOCAL_PATH := $(patsubst %/,%,$(dir $(MSSI_MTK_RSC_MAKEFILE)))
MSSI_MTK_RSC_NAME := $(mssi_mtk_rsc_name)
MSSI_MTK_RSC_PROJECT_NAME := $(MSSI_MTK_RSC_NAME)


include $(MSSI_MTK_RSC_MAKEFILE)


MSSI_MTK_RSC_RELATIVE_DIR := $(if $(PRODUCT_MSSI_MTK_RSC_ROOT_PATH),/$(PRODUCT_MSSI_MTK_RSC_ROOT_PATH)/$(MSSI_MTK_RSC_PROJECT_NAME))
ifeq (1,$(words $(CURRENT_MSSI_RSC_NAMES)))
  MSSI_MTK_RSC_PROP_RELATIVE_DIR :=
else
  MSSI_MTK_RSC_PROP_RELATIVE_DIR := $(MSSI_MTK_RSC_RELATIVE_DIR)
endif

MTK_RSC_SYSTEM_PROPERTIES += ro.mediatek.rsc_name=$(MSSI_MTK_RSC_NAME)
MTK_RSC_SYSTEM_PROPERTIES += ro.sys.current_rsc_path=/system$(MSSI_MTK_RSC_RELATIVE_DIR)
MTK_RSC_SYSTEM_PROPERTIES += ro.product.current_rsc_path=/$(TARGET_COPY_OUT_PRODUCT)$(MSSI_MTK_RSC_RELATIVE_DIR)
MTK_RSC_SYSTEM_FINAL_PROPERTIES := $(call uniq-pairs-by-first-component,$(call collapse-pairs,$(MTK_RSC_SYSTEM_PROPERTIES)),=)

MSSI_MTK_RSC_XML_NAME.$(MSSI_MTK_RSC_NAME) := $(MSSI_MTK_RSC_PROJECT_NAME)
ifeq ($(MSSI_MTK_RSC_NAME),$(word 1,$(CURRENT_MSSI_RSC_NAMES)))
MSSI_MTK_RSC_XML_INDEX.$(MSSI_MTK_RSC_NAME) := 0
else
MSSI_MTK_RSC_XML_INDEX.$(MSSI_MTK_RSC_NAME) := $(strip $(foreach i,1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20,$(if $(filter $(word $(i),$(wordlist 2,999,$(CURRENT_MSSI_RSC_NAMES))),$(MSSI_MTK_RSC_NAME)),$(i))))
endif

include device/mediatek/build/tasks/tools/check_mtk_rsc.mk


$(foreach f,$(MTK_RSC_APKS),\
  $(eval pair := $(subst :,$(space),$(f)))\
  $(eval m := $(word 1,$(pair)))\
  $(eval i := $(word 2,$(pair)))\
  $(eval n := $(word 3,$(pair)))\
  $(if $(filter SYSTEM,$(i)),\
    $(eval p := $(addprefix $(TARGET_OUT)$(MSSI_MTK_RSC_RELATIVE_DIR)/,$(n)))\
  ,\
    $(eval p := $(addprefix $(TARGET_OUT_$(i))$(MSSI_MTK_RSC_RELATIVE_DIR)/,$(n)))\
  )\
  $(if $(filter overlay,$(n)),\
    $(eval o := false)\
  ,\
    $(eval o :=)\
  )\
  $(eval mtk_rsc_override_modules += $(m))\
  $(eval mtk_rsc_required_modules += $(m).mtk_rsc.$(MSSI_MTK_RSC_NAME))\
  $(eval $(call mtk-rsc-add-package,$(m),.mtk_rsc.$(MSSI_MTK_RSC_NAME),$(p),$(o)))\
)
ALL_MODULES.$(m).REQUIRED_FROM_TARGET := $(ALL_MODULES.$(m).REQUIRED_FROM_TARGET) $(m).mtk_rsc.$(MSSI_MTK_RSC_NAME)

include $(CLEAR_VARS)
LOCAL_MODULE := system.ro.prop.mtk_rsc.$(MSSI_MTK_RSC_NAME)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
intermediates := $(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE))
$(intermediates)/gen.prop: MTK_RSC_SYSTEM_RO_PROPERTIES := $(filter ro.%,$(MTK_RSC_SYSTEM_FINAL_PROPERTIES))
$(intermediates)/gen.prop: $(MSSI_MTK_RSC_MAKEFILES)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(hide) $(foreach line,$(MTK_RSC_SYSTEM_RO_PROPERTIES), echo "$(line)" >> $@;)
	@touch $@
LOCAL_PREBUILT_MODULE_FILE := $(intermediates)/gen.prop
LOCAL_MODULE_PATH := $(TARGET_OUT)$(MSSI_MTK_RSC_PROP_RELATIVE_DIR)
LOCAL_INSTALLED_MODULE_STEM := ro.prop
ifeq ($(MSSI_MTK_RSC_NAME),default)
  LOCAL_POST_INSTALL_CMD := cp -f $(LOCAL_PREBUILT_MODULE_FILE) $(TARGET_OUT)/$(LOCAL_INSTALLED_MODULE_STEM)
endif
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE := system.rw.prop.mtk_rsc.$(MSSI_MTK_RSC_NAME)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
intermediates := $(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE))
$(intermediates)/gen.prop: MTK_RSC_SYSTEM_RW_PROPERTIES := $(filter-out ro.%,$(MTK_RSC_SYSTEM_FINAL_PROPERTIES))
$(intermediates)/gen.prop: $(MSSI_MTK_RSC_MAKEFILES)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(hide) $(foreach line,$(MTK_RSC_SYSTEM_RW_PROPERTIES), echo "$(line)" >> $@;)
	@touch $@
LOCAL_PREBUILT_MODULE_FILE := $(intermediates)/gen.prop
LOCAL_MODULE_PATH := $(TARGET_OUT)$(MSSI_MTK_RSC_PROP_RELATIVE_DIR)
LOCAL_INSTALLED_MODULE_STEM := rw.prop
ifeq ($(MSSI_MTK_RSC_NAME),default)
  LOCAL_POST_INSTALL_CMD := cp -f $(LOCAL_PREBUILT_MODULE_FILE) $(TARGET_OUT)/$(LOCAL_INSTALLED_MODULE_STEM)
endif
include $(BUILD_PREBUILT)

