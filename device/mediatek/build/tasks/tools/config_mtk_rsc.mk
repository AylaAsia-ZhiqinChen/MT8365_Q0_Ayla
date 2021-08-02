
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
ifndef support_split_build
ifdef SYS_TARGET_PROJECT
support_split_build := true
else ifneq ($(wildcard $(MTK_TARGET_PROJECT_FOLDER)/vnd_*.mk),)
support_split_build := true
endif
endif
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

MTK_RSC_MODULES += \
	vendor.ro.prop.mtk_rsc.$(MTK_RSC_NAME) \
	vendor.rw.prop.mtk_rsc.$(MTK_RSC_NAME)

ifneq ($(support_split_build),true)
MTK_RSC_MODULES += \
	check-rsc-config \
	rsc.xml \
	system.ro.prop.mtk_rsc.$(MTK_RSC_NAME) \
	system.rw.prop.mtk_rsc.$(MTK_RSC_NAME)
endif

$(foreach f,$(MTK_RSC_COPY_FILES),\
  $(eval pair := $(subst :,$(space),$(f)))\
  $(eval src := $(word 1,$(pair)))\
  $(eval img := $(word 2,$(pair)))\
  $(eval dst := $(word 3,$(pair)))\
  $(eval own := $(word 4,$(pair)))\
  $(eval PRODUCT_COPY_FILES += $(src):$(TARGET_COPY_OUT_$(img))$(MTK_RSC_RELATIVE_DIR)/$(dst)$(if $(own),:$(own)))\
)

PRODUCT_PACKAGES += $(MTK_RSC_MODULES)

$(foreach f,$(MTK_RSC_APKS),\
  $(eval pair := $(subst :,$(space),$(f)))\
  $(eval m := $(word 1,$(pair)))\
  $(eval MTK_RSC_OVERRIDE_APKS := $(MTK_RSC_OVERRIDE_APKS) $(m))\
)

MTK_RSC_MAKEFILES := $(MTK_RSC_MAKEFILES) $(MTK_RSC_MAKEFILE)

