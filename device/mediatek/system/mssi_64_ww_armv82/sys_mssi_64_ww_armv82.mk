SYS_TARGET_PROJECT := mssi_64_ww_armv82
SYS_BASE_PROJECT := mssi_64_ww_armv82
ifndef SYS_TARGET_PROJECT_FOLDER
SYS_TARGET_PROJECT_FOLDER := $(LOCAL_PATH)
endif
SYS_PROJECT_FOLDER := $(SYS_TARGET_PROJECT_FOLDER)

include $(SYS_TARGET_PROJECT_FOLDER)/SystemConfig.mk
include $(wildcard $(SYS_TARGET_PROJECT_FOLDER)/RuntimeSwitchConfig.mk)

$(call inherit-product, $(SYS_TARGET_PROJECT_FOLDER)/device.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/handheld_system.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/handheld_product.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony_system.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony_product.mk)

PRODUCT_LOCALES := en_US zh_CN zh_TW es_ES pt_BR ru_RU fr_FR de_DE tr_TR vi_VN ms_MY in_ID th_TH it_IT ar_EG hi_IN bn_IN ur_PK fa_IR pt_PT nl_NL el_GR hu_HU tl_PH ro_RO cs_CZ ko_KR km_KH iw_IL my_MM pl_PL es_US bg_BG hr_HR lv_LV lt_LT sk_SK uk_UA de_AT da_DK fi_FI nb_NO sv_SE en_GB hy_AM zh_HK et_EE ja_JP kk_KZ sr_RS sl_SI ca_ES
PRODUCT_MANUFACTURER := alps
PRODUCT_NAME := sys_mssi_64_ww_armv82
PRODUCT_DEVICE := $(strip $(SYS_BASE_PROJECT))
PRODUCT_MODEL := mssi_64_ww_armv82
PRODUCT_POLICY := android.policy_phone
PRODUCT_BRAND := alps
PRODUCT_SHIPPING_API_LEVEL := 29
ifdef MTK_TARGET_PROJECT
PRODUCT_SHIPPING_API_LEVEL :=
endif

