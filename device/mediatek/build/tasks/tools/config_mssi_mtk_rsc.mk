
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

MTK_RSC_MODULES += \
	system.ro.prop.mtk_rsc.$(MSSI_MTK_RSC_NAME) \
	system.rw.prop.mtk_rsc.$(MSSI_MTK_RSC_NAME)

MTK_RSC_MODULES += \
	rsc.xml

$(foreach f,$(MTK_RSC_COPY_FILES),\
  $(eval pair := $(subst :,$(space),$(f)))\
  $(eval src := $(word 1,$(pair)))\
  $(eval img := $(word 2,$(pair)))\
  $(eval dst := $(word 3,$(pair)))\
  $(eval own := $(word 4,$(pair)))\
  $(eval PRODUCT_COPY_FILES += $(src):$(TARGET_COPY_OUT_$(img))$(MSSI_MTK_RSC_RELATIVE_DIR)/$(dst)$(if $(own),:$(own)))\
)

PRODUCT_PACKAGES += $(MTK_RSC_MODULES)

$(foreach f,$(MTK_RSC_APKS),\
  $(eval pair := $(subst :,$(space),$(f)))\
  $(eval m := $(word 1,$(pair)))\
  $(eval MTK_RSC_OVERRIDE_APKS := $(MTK_RSC_OVERRIDE_APKS) $(m))\
)

MSSI_MTK_RSC_MAKEFILES := $(MSSI_MTK_RSC_MAKEFILES) $(MSSI_MTK_RSC_MAKEFILE)

