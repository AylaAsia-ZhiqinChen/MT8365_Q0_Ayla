


######################################################

# PRODUCT_COPY_FILES overwrite
# Please add flavor project's PRODUCT_COPY_FILES here.
# It will overwrite base project's PRODUCT_COPY_FILES.

PRODUCT_COPY_FILES += $(LOCAL_PATH)/handheld_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/handheld_core_hardware.xml

# overlay has priorities. high <-> low.
DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay
DEVICE_PACKAGE_OVERLAYS += device/mediatek/common/overlay/wifi
DEVICE_PACKAGE_OVERLAYS += device/mediatek/common/overlay/sd_in_ex_otg
DEVICE_PACKAGE_OVERLAYS += device/mediatek/common/overlay/navbar

# dual input charger support
ifeq ($(strip $(MTK_DUAL_INPUT_CHARGER_SUPPORT)),yes)
  PRODUCT_PROPERTY_OVERRIDES += ro.mtk_diso_support=true
endif

#######################################################

# PRODUCT_COPY_FILES += $(LOCAL_PATH)/egl.cfg:$(TARGET_COPY_OUT_VENDOR)/lib/egl/egl.cfg:mtk
# PRODUCT_COPY_FILES += $(LOCAL_PATH)/ueventd.mt8168.rc:root/ueventd.mt8168.rc

PRODUCT_COPY_FILES += $(LOCAL_PATH)/factory_init.project.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/factory_init.project.rc
PRODUCT_COPY_FILES += $(LOCAL_PATH)/init.project.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.project.rc
PRODUCT_COPY_FILES += $(LOCAL_PATH)/meta_init.project.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/meta_init.project.rc
PRODUCT_COPY_FILES += $(LOCAL_PATH)/init.recovery.mt8168.rc:root/init.recovery.mt8168.rc

ifeq ($(MTK_SMARTBOOK_SUPPORT),yes)
PRODUCT_COPY_FILES += $(LOCAL_PATH)/sbk-kpd.kl:system/usr/keylayout/sbk-kpd.kl:mtk \
                      $(LOCAL_PATH)/sbk-kpd.kcm:system/usr/keychars/sbk-kpd.kcm:mtk
endif

# Add FlashTool needed files
#PRODUCT_COPY_FILES += $(LOCAL_PATH)/EBR1:EBR1
#ifneq ($(wildcard $(LOCAL_PATH)/EBR2),)
#  PRODUCT_COPY_FILES += $(LOCAL_PATH)/EBR2:EBR2
#endif
#PRODUCT_COPY_FILES += $(LOCAL_PATH)/MBR:MBR
#PRODUCT_COPY_FILES += $(LOCAL_PATH)/MT8168_Android_scatter.txt:MT8168_Android_scatter.txt




### copy it in common/device.mk
# alps/vendor/mediatek/proprietary/external/GeoCoding/Android.mk

# alps/vendor/mediatek/proprietary/frameworks-ext/native/etc/Android.mk

# touch related file for CTS
ifeq ($(strip $(CUSTOM_KERNEL_TOUCHPANEL)),generic)
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.xml
else
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.faketouch.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.faketouch.xml
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.multitouch.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.xml
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.xml
endif

# USB OTG
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml

# GPS relative file
ifeq ($(MTK_GPS_SUPPORT),yes)
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml
else
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.xml
endif

# alps/external/libnfc-opennfc/open_nfc/hardware/libhardware/modules/nfcc/nfc_hal_microread/Android.mk
# PRODUCT_COPY_FILES += external/libnfc-opennfc/open_nfc/hardware/libhardware/modules/nfcc/nfc_hal_microread/driver/open_nfc_driver.ko:$(TARGET_COPY_OUT_VENDOR)/lib/open_nfc_driver.ko:mtk

# alps/frameworks/av/media/libeffects/factory/Android.mk
PRODUCT_COPY_FILES += frameworks/av/media/libeffects/data/audio_effects.conf:system/etc/audio_effects.conf

# alps/mediatek/config/$project
PRODUCT_COPY_FILES += $(LOCAL_PATH)/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml

# Set default USB interface
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += persist.service.acm.enable=0
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.mount.fs=EXT4

# Set density
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=213

# audio decoder priority OMX > C2
PRODUCT_PROPERTY_OVERRIDES += debug.stagefright.omx_default_rank.sw-audio=7

PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapgrowthlimit=128m
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=256m

# meta tool
PRODUCT_PROPERTY_OVERRIDES += ro.mediatek.chip_ver=S01
PRODUCT_PROPERTY_OVERRIDES += ro.mediatek.platform=MT8168

# wifi hw test tool
PRODUCT_PACKAGES += libwifitest
PRODUCT_PACKAGES += wifitesttool

# GPU NN
PRODUCT_PACKAGES += android.hardware.neuralnetworks@1.2-service-gpunn
PRODUCT_PACKAGES += armnn_app.config
PRODUCT_PACKAGES += libarmnn

# set Telephony property - SIM count
#SIM_COUNT := 2
#PRODUCT_PROPERTY_OVERRIDES += ro.telephony.sim.count=$(SIM_COUNT)
#PRODUCT_PROPERTY_OVERRIDES += persist.radio.default.sim=0
#PRODUCT_PROPERTY_OVERRIDES += persist.radio.multisim.config=dsds

# Audio Related Resource
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/aiot8365p2_64_bsp/factory/res/sound/testpattern1.wav:$(TARGET_COPY_OUT_VENDOR)/res/sound/testpattern1.wav:mtk
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/aiot8365p2_64_bsp/factory/res/sound/ringtone.wav:$(TARGET_COPY_OUT_VENDOR)/res/sound/ringtone.wav:mtk

# Keyboard layout
PRODUCT_COPY_FILES += device/mediatek/mt8168/ACCDET.kl:system/usr/keylayout/ACCDET.kl:mtk
PRODUCT_COPY_FILES += $(LOCAL_PATH)/mtk-kpd.kl:system/usr/keylayout/mtk-kpd.kl:mtk

# Microphone
PRODUCT_COPY_FILES += $(LOCAL_PATH)/android.hardware.microphone.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.microphone.xml

# Camera
PRODUCT_COPY_FILES += $(LOCAL_PATH)/android.hardware.camera.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.xml

# Audio Policy
PRODUCT_COPY_FILES += $(LOCAL_PATH)/audio_policy.conf:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy.conf:mtk

# Mixer Paths
PRODUCT_COPY_FILES += $(LOCAL_PATH)/mixer_paths.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths.xml:mtk

#Images for LCD test in factory mode
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/aiot8365p2_64_bsp/factory/res/images/lcd_test_00.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_00.png:mtk
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/aiot8365p2_64_bsp/factory/res/images/lcd_test_01.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_01.png:mtk
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/aiot8365p2_64_bsp/factory/res/images/lcd_test_02.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_02.png:mtk

#For PowerAQ config file
ifeq (yes,$(strip $(MTK_POWERAQ_SUPPORT)))
  PRODUCT_COPY_FILES += $(LOCAL_PATH)/element_pool.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/element_pool.cfg:mtk
  PRODUCT_COPY_FILES += $(LOCAL_PATH)/page_list.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/page_list.cfg:mtk
  PRODUCT_PACKAGES += toybox1
  PRODUCT_PACKAGES += ppccli
endif

#For GMO to reduce runtime memroy usage
ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))

PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.hwui.path_cache_size=0
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.hwui.text_small_cache_width=512
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.hwui.text_small_cache_height=256
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.hwui.disable_asset_atlas=true


# Disable fast starting window in GMO project
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.mtk_perf_fast_start_win=0


#Images for LCD test in factory mode
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/common/factory/res/images/lcd_test_00_gmo.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_00.png:mtk
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/common/factory/res/images/lcd_test_01_gmo.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_01.png:mtk
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/common/factory/res/images/lcd_test_02_gmo.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_02.png:mtk

PRODUCT_COPY_FILES += device/mediatek/common/fstab.enableswap_ago:root/fstab.enableswap

PRODUCT_PROPERTY_OVERRIDES += ro.config.low_ram=true

#end of For GMO to reduce runtime memroy usage
endif

ifeq (yes,$(strip $(MTK_SEC_VIDEO_PATH_SUPPORT)))
  PRODUCT_PROPERTY_OVERRIDES += ro.mtk_sec_video_path_support=1
  PRODUCT_PROPERTY_OVERRIDES += ro.mtk_venc_inhouse_ready=1
endif

ifdef OPTR_SPEC_SEG_DEF
  ifneq ($(strip $(OPTR_SPEC_SEG_DEF)),NONE)
    OPTR := $(word 1,$(subst _,$(space),$(OPTR_SPEC_SEG_DEF)))
    SPEC := $(word 2,$(subst _,$(space),$(OPTR_SPEC_SEG_DEF)))
    SEG  := $(word 3,$(subst _,$(space),$(OPTR_SPEC_SEG_DEF)))
    DEVICE_PACKAGE_OVERLAYS += device/mediatek/common/overlay/operator/$(OPTR)/$(SPEC)/$(SEG)
  endif
endif
ifneq (yes,$(strip $(MTK_TABLET_PLATFORM)))
  ifeq (480,$(strip $(LCM_WIDTH)))
    ifeq (854,$(strip $(LCM_HEIGHT)))
      DEVICE_PACKAGE_OVERLAYS += device/mediatek/common/overlay/FWVGA
    endif
  endif
  ifeq (540,$(strip $(LCM_WIDTH)))
    ifeq (960,$(strip $(LCM_HEIGHT)))
      DEVICE_PACKAGE_OVERLAYS += device/mediatek/common/overlay/qHD
    endif
  endif
endif
ifeq (yes,$(strip $(MTK_GMO_ROM_OPTIMIZE)))
  DEVICE_PACKAGE_OVERLAYS += device/mediatek/common/overlay/slim_rom
endif
ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
  DEVICE_PACKAGE_OVERLAYS += device/mediatek/common/overlay/slim_ram
endif


# media_profiles.xml for media profile support
PRODUCT_COPY_FILES += device/mediatek/mt8168/media_profiles_mt8168.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml:mtk
#PRODUCT_PROPERTY_OVERRIDES += media.settings.xml=/vendor/etc/media_profiles.xml

# dual input charger support
ifeq ($(strip $(MTK_DUAL_INPUT_CHARGER_SUPPORT)),yes)
  PRODUCT_PROPERTY_OVERRIDES += ro.mtk_diso_support=true
endif

# hybrid encode support
  PRODUCT_PROPERTY_OVERRIDES += ro.mtk_hybrid_encode_support=true

# add CarBTDemo
PRODUCT_PACKAGES += CarBTDemo

# wlan
ifeq ($(MTK_WLAN_SUPPORT), yes)
  ifeq ($(MTK_COMBO_CHIP), MT7663)
    PRODUCT_PACKAGES += wifitest
  endif
endif

# Bluetooth MESH
PRODUCT_PACKAGES += \
    MeshTestProvisioner \

# BT
PRODUCT_PACKAGES += \
    libbluetooth_mtk \
    libbluetooth_mtk_pure \
    libbluetooth_hw_test \
    autobt

#Add BT mesh config file
PRODUCT_COPY_FILES += $(LOCAL_PATH)/mtk_bt_mesh.conf:$(TARGET_OUT)/system/etc/bluetooth/mtk_bt_mesh.conf

# inherit 8163 platform
$(call inherit-product, device/mediatek/mt8168/device.mk)

$(call inherit-product-if-exists, vendor/mediatek/libs/$(MTK_TARGET_PROJECT)/device-vendor.mk)

# $(call inherit-product, frameworks/native/build/tablet-dalvik-heap.mk)
