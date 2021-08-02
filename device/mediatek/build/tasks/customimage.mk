# device/mediatek/common/BoardConfig.mk
ifeq ($(strip $(MTK_CIP_SUPPORT)),yes)
  TARGET_OUT_MTK_CIP := $(PRODUCT_OUT)/custom
else
  TARGET_OUT_MTK_CIP := $(PRODUCT_OUT)/system
endif


# MTK_CARRIEREXPRESS_PACK without CIP
INTERNAL_CUSTOMIMAGE_FILES :=
ifneq ($(filter-out no,$(strip $(MSSI_MTK_CARRIEREXPRESS_PACK))),)
include device/mediatek/build/tasks/tools/build_cip_usp_info.mk
$(INTERNAL_SYSTEMIMAGE_FILES): $(INTERNAL_CUSTOMIMAGE_FILES)
endif#MTK_CARRIEREXPRESS_PACK
