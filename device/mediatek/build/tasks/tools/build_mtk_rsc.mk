
mtk_rsc_parameter_variables := \
	MTK_RSC_APKS \
	MTK_RSC_MODULES \
	MTK_RSC_COPY_FILES \
	MTK_RSC_SYSTEM_PROPERTIES \
	MTK_RSC_VENDOR_PROPERTIES \
	MTK_RSC_XML_OPTR \


$(call clear-var-list, $(mtk_rsc_parameter_variables))

MTK_RSC_MAKEFILE := $(firstword $(wildcard $(MTK_TARGET_PROJECT_FOLDER)/rsc/$(mtk_rsc_name)/RuntimeSwitch.mk $(MTK_PROJECT_FOLDER)/rsc/$(mtk_rsc_name)/RuntimeSwitch.mk))
ifndef MTK_RSC_MAKEFILE
$(error Fail to find RuntimeSwitch.mk for $(mtk_rsc_name))
endif

MTK_RSC_LOCAL_PATH := $(patsubst %/,%,$(dir $(MTK_RSC_MAKEFILE)))
MTK_RSC_NAME := $(mtk_rsc_name)
ifeq ($(support_split_build),true)
MTK_RSC_PROJECT_NAME := $(MTK_RSC_NAME)
else ifeq ($(MTK_RSC_NAME),default)
MTK_RSC_PROJECT_NAME := $(MTK_TARGET_PROJECT)
else
MTK_RSC_PROJECT_NAME := $(MTK_TARGET_PROJECT)[$(MTK_RSC_NAME)]
endif


include $(MTK_RSC_MAKEFILE)


MTK_RSC_RELATIVE_DIR := $(if $(PRODUCT_MTK_RSC_ROOT_PATH),/$(PRODUCT_MTK_RSC_ROOT_PATH)/$(MTK_RSC_PROJECT_NAME))
ifeq (1,$(words $(CURRENT_RSC_NAMES)))
  MTK_RSC_PROP_RELATIVE_DIR :=
else
  MTK_RSC_PROP_RELATIVE_DIR := $(MTK_RSC_RELATIVE_DIR)
endif

MTK_RSC_SYSTEM_PROPERTIES += ro.mediatek.rsc_name=$(MTK_RSC_NAME)
MTK_RSC_SYSTEM_PROPERTIES += ro.sys.current_rsc_path=/system$(MTK_RSC_RELATIVE_DIR)
MTK_RSC_SYSTEM_PROPERTIES += ro.product.current_rsc_path=/$(TARGET_COPY_OUT_PRODUCT)$(MTK_RSC_RELATIVE_DIR)
MTK_RSC_VENDOR_PROPERTIES += ro.vendor.mediatek.rsc_name=$(MTK_RSC_NAME)
MTK_RSC_VENDOR_PROPERTIES += ro.vendor.sys.current_rsc_path=/system$(MTK_RSC_RELATIVE_DIR)
MTK_RSC_VENDOR_PROPERTIES += ro.vendor.vnd.current_rsc_path=/$(TARGET_COPY_OUT_VENDOR)$(MTK_RSC_RELATIVE_DIR)
MTK_RSC_SYSTEM_FINAL_PROPERTIES := $(call uniq-pairs-by-first-component,$(call collapse-pairs,$(MTK_RSC_SYSTEM_PROPERTIES)),=)
MTK_RSC_VENDOR_FINAL_PROPERTIES := $(call uniq-pairs-by-first-component,$(call collapse-pairs,$(MTK_RSC_VENDOR_PROPERTIES)),=)

MTK_RSC_XML_NAME.$(MTK_RSC_NAME) := $(MTK_RSC_PROJECT_NAME)
ifeq ($(MTK_RSC_NAME),$(word 1,$(CURRENT_RSC_NAMES)))
MTK_RSC_XML_INDEX.$(MTK_RSC_NAME) := 0
else
MTK_RSC_XML_INDEX.$(MTK_RSC_NAME) := $(strip $(foreach i,1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20,$(if $(filter $(word $(i),$(wordlist 2,999,$(CURRENT_RSC_NAMES))),$(MTK_RSC_NAME)),$(i))))
endif
ifneq ($(MTK_RSC_XML_OPTR),)
MTK_RSC_XML_OPTR.$(MTK_RSC_NAME) := $(MTK_RSC_XML_OPTR)
else ifneq ($(OPTR_SPEC_SEG_DEF),)
MTK_RSC_XML_OPTR.$(MTK_RSC_NAME) := $(OPTR_SPEC_SEG_DEF)
else
MTK_RSC_XML_OPTR.$(MTK_RSC_NAME) := NONE
endif

include device/mediatek/build/tasks/tools/check_mtk_rsc.mk


#$(foreach f,$(MTK_RSC_SHARED_LIBRARIES),\
#  $(eval pair := $(subst :,$(space),$(f)))\
#  $(eval m := $(word 1,$(pair)))\
#  $(eval i := $(word 2,$(pair)))\
#  $(eval n := $(word 3,$(pair)))\
#  $(eval o := $(word 4,$(pair)))\
#  $(if $(filter SYSTEM,$(i)),\
#    $(eval p := $(addprefix $(TARGET_OUT)$(MTK_RSC_RELATIVE_DIR)/,$(n)))\
#    $(eval q := $(addprefix $(TARGET_OUT)$(MTK_RSC_RELATIVE_DIR)/,$(o)))\
#  ,\
#    $(eval p := $(addprefix $(TARGET_OUT_$(i))$(MTK_RSC_RELATIVE_DIR)/,$(n)))\
#    $(eval q := $(addprefix $(TARGET_OUT_$(i))$(MTK_RSC_RELATIVE_DIR)/,$(o)))\
#  )\
#  $(eval mtk_rsc_override_modules += $(m))\
#  $(eval mtk_rsc_required_modules += $(m).mtk_rsc.$(MTK_RSC_NAME))\
#  $(eval $(call mtk-rsc-add-shared-library,$(m),.mtk_rsc.$(MTK_RSC_NAME),$(p),$(q)))\
#)


#$(foreach f,$(MTK_RSC_EXECUTABLES),\
#  $(eval pair := $(subst :,$(space),$(f)))\
#  $(eval m := $(word 1,$(pair)))\
#  $(eval i := $(word 2,$(pair)))\
#  $(eval n := $(word 3,$(pair)))\
#  $(eval o := $(word 4,$(pair)))\
#  $(if $(filter SYSTEM,$(i)),\
#    $(eval p := $(addprefix $(TARGET_OUT)$(MTK_RSC_RELATIVE_DIR)/,$(n)))\
#    $(eval q := $(addprefix $(TARGET_OUT)$(MTK_RSC_RELATIVE_DIR)/,$(o)))\
#  ,\
#    $(eval p := $(addprefix $(TARGET_OUT_$(i))$(MTK_RSC_RELATIVE_DIR)/,$(n)))\
#    $(eval q := $(addprefix $(TARGET_OUT_$(i))$(MTK_RSC_RELATIVE_DIR)/,$(o)))\
#  )\
#  $(eval mtk_rsc_override_modules += $(m))\
#  $(eval mtk_rsc_required_modules += $(m).mtk_rsc.$(MTK_RSC_NAME))\
#  $(eval $(call mtk-rsc-add-executable,$(m),.mtk_rsc.$(MTK_RSC_NAME),$(p),$(q)))\
#)


#$(foreach f,$(MTK_RSC_JAVA_LIBRARIES),\
#  $(eval pair := $(subst :,$(space),$(f)))\
#  $(eval m := $(word 1,$(pair)))\
#  $(eval i := $(word 2,$(pair)))\
#  $(eval n := $(word 3,$(pair)))\
#  $(if $(filter SYSTEM,$(i)),\
#    $(eval p := $(addprefix $(TARGET_OUT)$(MTK_RSC_RELATIVE_DIR)/,$(n)))\
#  ,\
#    $(eval p := $(addprefix $(TARGET_OUT_$(i))$(MTK_RSC_RELATIVE_DIR)/,$(n)))\
#  )\
#  $(eval mtk_rsc_override_modules += $(m))\
#  $(eval mtk_rsc_required_modules += $(m).mtk_rsc.$(MTK_RSC_NAME))\
#  $(eval $(call mtk-rsc-add-java-library,$(m),.mtk_rsc.$(MTK_RSC_NAME),$(p)))\
#)


$(foreach f,$(MTK_RSC_APKS),\
  $(eval pair := $(subst :,$(space),$(f)))\
  $(eval m := $(word 1,$(pair)))\
  $(eval i := $(word 2,$(pair)))\
  $(eval n := $(word 3,$(pair)))\
  $(if $(filter SYSTEM,$(i)),\
    $(eval p := $(addprefix $(TARGET_OUT)$(MTK_RSC_RELATIVE_DIR)/,$(n)))\
  ,\
    $(eval p := $(addprefix $(TARGET_OUT_$(i))$(MTK_RSC_RELATIVE_DIR)/,$(n)))\
  )\
  $(if $(filter overlay,$(n)),\
    $(eval o := false)\
  ,\
    $(eval o :=)\
  )\
  $(eval mtk_rsc_override_modules += $(m))\
  $(eval mtk_rsc_required_modules += $(m).mtk_rsc.$(MTK_RSC_NAME))\
  $(eval $(call mtk-rsc-add-package,$(m),.mtk_rsc.$(MTK_RSC_NAME),$(p),$(o)))\
)
ALL_MODULES.$(m).REQUIRED_FROM_TARGET := $(ALL_MODULES.$(m).REQUIRED_FROM_TARGET) $(m).mtk_rsc.$(MTK_RSC_NAME)


ifeq ($(support_split_build),true)
# build_mssi_mtk_rsc.mk
else

include $(CLEAR_VARS)
LOCAL_MODULE := system.ro.prop.mtk_rsc.$(MTK_RSC_NAME)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
intermediates := $(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE))
$(intermediates)/gen.prop: MTK_RSC_SYSTEM_RO_PROPERTIES := $(filter ro.%,$(MTK_RSC_SYSTEM_FINAL_PROPERTIES))
$(intermediates)/gen.prop: $(MTK_RSC_MAKEFILES)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(hide) $(foreach line,$(MTK_RSC_SYSTEM_RO_PROPERTIES), echo "$(line)" >> $@;)
	@touch $@
LOCAL_PREBUILT_MODULE_FILE := $(intermediates)/gen.prop
LOCAL_MODULE_PATH := $(TARGET_OUT)$(MTK_RSC_PROP_RELATIVE_DIR)
LOCAL_INSTALLED_MODULE_STEM := ro.prop
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE := system.rw.prop.mtk_rsc.$(MTK_RSC_NAME)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
intermediates := $(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE))
$(intermediates)/gen.prop: MTK_RSC_SYSTEM_RW_PROPERTIES := $(filter-out ro.%,$(MTK_RSC_SYSTEM_FINAL_PROPERTIES))
$(intermediates)/gen.prop: $(MTK_RSC_MAKEFILES)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(hide) $(foreach line,$(MTK_RSC_SYSTEM_RW_PROPERTIES), echo "$(line)" >> $@;)
	@touch $@
LOCAL_PREBUILT_MODULE_FILE := $(intermediates)/gen.prop
LOCAL_MODULE_PATH := $(TARGET_OUT)$(MTK_RSC_PROP_RELATIVE_DIR)
LOCAL_INSTALLED_MODULE_STEM := rw.prop
include $(BUILD_PREBUILT)

endif#support_split_build


include $(CLEAR_VARS)
LOCAL_MODULE := vendor.ro.prop.mtk_rsc.$(MTK_RSC_NAME)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
intermediates := $(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE))
$(intermediates)/gen.prop: MTK_RSC_VENDOR_RO_PROPERTIES := $(filter ro.%,$(MTK_RSC_VENDOR_FINAL_PROPERTIES))
$(intermediates)/gen.prop: $(MTK_RSC_MAKEFILES)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(hide) $(foreach line,$(MTK_RSC_VENDOR_RO_PROPERTIES), echo "$(line)" >> $@;)
	@touch $@
LOCAL_PREBUILT_MODULE_FILE := $(intermediates)/gen.prop
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)$(MTK_RSC_PROP_RELATIVE_DIR)
LOCAL_INSTALLED_MODULE_STEM := ro.prop
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE := vendor.rw.prop.mtk_rsc.$(MTK_RSC_NAME)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
intermediates := $(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE))
$(intermediates)/gen.prop: MTK_RSC_VENDOR_RW_PROPERTIES := $(filter-out ro.%,$(MTK_RSC_VENDOR_FINAL_PROPERTIES))
$(intermediates)/gen.prop: $(MTK_RSC_MAKEFILES)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(hide) $(foreach line,$(MTK_RSC_VENDOR_RW_PROPERTIES), echo "$(line)" >> $@;)
	@touch $@
LOCAL_PREBUILT_MODULE_FILE := $(intermediates)/gen.prop
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)$(MTK_RSC_PROP_RELATIVE_DIR)
LOCAL_INSTALLED_MODULE_STEM := rw.prop
include $(BUILD_PREBUILT)
