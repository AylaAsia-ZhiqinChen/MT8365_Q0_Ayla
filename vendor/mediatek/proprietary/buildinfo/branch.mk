ifeq ($(MTK_BSP_PACKAGE),yes)
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.mediatek.version.branch=alps-mp-q0.mp3
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mediatek.version.branch=alps-mp-q0.mp3
else
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.mediatek.version.branch=alps-mp-q0.mp3
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mediatek.version.branch=alps-mp-q0.mp3
endif
