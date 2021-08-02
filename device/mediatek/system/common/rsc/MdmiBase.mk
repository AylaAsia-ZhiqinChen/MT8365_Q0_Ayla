# Based on DMC framework
include device/mediatek/system/common/rsc/DmcBase.mk

# ----------------------------------------------------------------------------
# Modem Analysis Public Interface(MAPI)
# ----------------------------------------------------------------------------
MTK_RSC_SYSTEM_PROPERTIES += \
    ro.vendor.system.mtk_mdmi_support=1

MTK_RSC_MODULES += \
    mdmi_redirector \
    mdmi_redirector_ctrl \
    libVzw_mdmi_debug \
    libVzw_mdmi_lte \
    libVzw_mdmi_gsm \
    libVzw_mdmi_umts \
    libVzw_mdmi_wifi \
    libVzw_mdmi_ims \
    libVzw_mdmi_commands \
    libVzw_mdmi_wcdma \
    libVzw_mdmi_hsupa \
    libVzw_mdmi_hsdpa \
    libVzw_mdmi_embms
