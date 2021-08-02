ifeq ($(strip $(MTK_FM_SUPPORT)), yes)

# FM Configuration
FM_CHIP_ID := $(patsubst consys_%,%,$(patsubst CONSYS_%,%,$(strip $(MTK_COMBO_CHIP))))

FM_SOC_CHIPS  := 6580 6570 0633
FM_6627_CHIPS := 6572 6582 6592 8127 6752 0321 0335 0337 6735 8163 6755 0326 6757 6763 6739 6625
FM_6630_CHIPS := 6630 8167
FM_6631_CHIPS := 6758 6759 6771 6775 6765 6761 3967 6797 6768 6785 8168
FM_6632_CHIPS := 6632
FM_6635_CHIPS := 6779 6885

ifneq ($(filter $(FM_SOC_CHIPS), $(FM_CHIP_ID)),)
FM_CHIP := soc
else ifneq ($(filter $(FM_6627_CHIPS), $(FM_CHIP_ID)),)
FM_CHIP := mt6627
else ifneq ($(filter $(FM_6630_CHIPS), $(FM_CHIP_ID)),)
FM_CHIP := mt6630
FM_CFG_FILE_2 := yes
else ifneq ($(filter $(FM_6631_CHIPS), $(FM_CHIP_ID)),)
FM_CHIP := mt6631
else ifneq ($(filter $(FM_6632_CHIPS), $(FM_CHIP_ID)),)
FM_CHIP := mt6632
else ifneq ($(filter $(FM_6635_CHIPS), $(FM_CHIP_ID)),)
    ifeq ($(strip $(MTK_CONSYS_ADIE)), MT6631)
        FM_CHIP := mt6631
    else
        FM_CHIP := mt6635
    endif
else
    ifeq ($(strip $(MTK_COMBO_CHIP)), MT6632)
        FM_CHIP := mt6632
    else ifeq ($(strip $(MTK_COMBO_CHIP)), MT6627)
        FM_CHIP := mt6627
    else ifeq ($(strip $(MTK_COMBO_CHIP)), MT6630)
        FM_CHIP := mt6630
        FM_CFG_FILE_2 := yes
    endif
endif

PRODUCT_PROPERTY_OVERRIDES += persist.vendor.connsys.fm_chipid=$(FM_CHIP)

ifeq ($(strip $(MTK_FM_TX_SUPPORT)), yes)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.connsys.fm_tx_support=1
endif

ifeq ($(strip $(MTK_FM_SHORT_ANTENNA_SUPPORT)), yes)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.connsys.fm_short_antenna_support=1
endif

ifeq ($(strip $(MTK_FM_50KHZ_SUPPORT)), yes)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.connsys.fm_50khz_support=1
endif

# lib
PRODUCT_PACKAGES += libfmjni
PRODUCT_PACKAGES += libfmtxjni
PRODUCT_PACKAGES_ENG += autofm

# hidl
#PRODUCT_PACKAGES += fm_hidl_service

# kernel module
PRODUCT_PACKAGES += fmradio_drv.ko

# cust cfg
FM_CFG_PATH := vendor/mediatek/proprietary/hardware/connectivity/fmradio/config/$(FM_CHIP)

FM_PATCH_FILE_1 := $(FM_CHIP)_fm_v1_patch.bin
FM_COEFF_FILE_1 := $(FM_CHIP)_fm_v1_coeff.bin
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_PATCH_FILE_1):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_PATCH_FILE_1):mtk
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_COEFF_FILE_1):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_COEFF_FILE_1):mtk

ifeq ($(strip $(FM_CFG_FILE_2)), yes)
FM_PATCH_FILE_2 := $(FM_CHIP)_fm_v2_patch.bin
FM_COEFF_FILE_2 := $(FM_CHIP)_fm_v2_coeff.bin
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_PATCH_FILE_2):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_PATCH_FILE_2):mtk
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_COEFF_FILE_2):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_COEFF_FILE_2):mtk
endif

ifeq ($(strip $(MTK_FM_TX_SUPPORT)), yes)
FM_TX_PATCH_FILE_1 := $(FM_CHIP)_fm_v1_patch_tx.bin
FM_TX_COEFF_FILE_1 := $(FM_CHIP)_fm_v1_coeff_tx.bin
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_TX_PATCH_FILE_1):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_TX_PATCH_FILE_1):mtk
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_TX_COEFF_FILE_1):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_TX_COEFF_FILE_1):mtk

ifeq ($(strip $(FM_CFG_FILE_2)), yes)
FM_TX_PATCH_FILE_2 := $(FM_CHIP)_fm_v2_patch_tx.bin
FM_TX_COEFF_FILE_2 := $(FM_CHIP)_fm_v2_coeff_tx.bin
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_TX_PATCH_FILE_2):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_TX_PATCH_FILE_2):mtk
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_TX_COEFF_FILE_2):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_TX_COEFF_FILE_2):mtk
endif
endif

FM_CFG_FILE := fm_cust.cfg
PRODUCT_COPY_FILES += $(FM_CFG_PATH)/$(FM_CFG_FILE):$(TARGET_COPY_OUT_VENDOR)/firmware/$(FM_CFG_FILE):mtk

endif
