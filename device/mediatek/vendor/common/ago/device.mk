#From O, project with less than 1G memory, must set prop ro.config.low_ram=true
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_config_max_dram_size=$(CUSTOM_CONFIG_MAX_DRAM_SIZE)

ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
    AGO_TARGET_DIR := $(TOPDIR)device/mediatek/vendor/common/ago/build
    # To not pick-up LiveWallpapers, inherit from generic.mk NOT full_base.mk
    ifeq ($(strip $(MTK_TB_WIFI_3G_MODE)),WIFI_ONLY)
        $(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony.mk)
    else
        $(call inherit-product, $(SRC_TARGET_DIR)/product/generic.mk)
    endif
    $(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)
    PRODUCT_PROPERTY_OVERRIDES += ro.vendor.gmo.ram_optimize=1
    PRODUCT_COPY_FILES += device/mediatek/vendor/common/fstab.enableswap_ago:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.enableswap:mtk
    # Add MtkLauncher3 to replace Launcher3 when vendor code is available
    ifneq ($(wildcard vendor/mediatek/proprietary/packages/apps/Launcher3/Android.mk),)
        PRODUCT_PACKAGES += MtkLauncher3GoIconRecents
    else
        PRODUCT_PACKAGES += Launcher3GoIconRecents
    endif

    # Adjust LMKD to aviod continuous kill ADJ0
    PRODUCT_PROPERTY_OVERRIDES += \
        ro.lmk.use_minfree_levels=false \
        ro.lmk.upgrade_pressure=0 \

    # For the new devices shipped we would use go_handheld_core_hardware.xml and
    # previously launched devices should continue using handheld_core_hardware.xml
    PRODUCT_COPY_FILES += frameworks/native/data/etc/go_handheld_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/handheld_core_hardware.xml
    AGO_DRAM_LIMIT := $(shell printf "%d" 0x80000000)
    CUR_DRAM_SIZE := $(shell printf "%d" $(CUSTOM_CONFIG_MAX_DRAM_SIZE))
    ifeq (0x20000000,$(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)))
        ifneq ($(filter yes,$(BUILD_AGO_GMS) $(MTK_GMO_RAM_OPTIMIZE)),)
            $(call inherit-product, $(SRC_TARGET_DIR)/product/go_defaults_512.mk)
            PRODUCT_COPY_FILES += device/mediatek/vendor/common/ago/init/init.ago_512.rc:$(MTK_TARGET_VENDOR_RC)/init.ago.rc
            #PRODUCT_PROPERTY_OVERRIDES += ro.lmk.critical_upgrade=true
            #PRODUCT_PROPERTY_OVERRIDES += ro.lmk.upgrade_pressure=30
            #PRODUCT_PROPERTY_OVERRIDES += ro.lmk.downgrade_pressure=50
            PRODUCT_PROPERTY_OVERRIDES += ro.lmk.thrashing_min_score_adj=300
            PRODUCT_PROPERTY_OVERRIDES += \
                ro.zram.mark_idle_delay_mins=2 \
                ro.zram.first_wb_delay_mins=5 \
                ro.zram.periodic_wb_delay_hours=24
        endif
    else ifeq ($(shell test $(CUR_DRAM_SIZE) -le $(AGO_DRAM_LIMIT) && echo true), true)
        PRODUCT_PROPERTY_OVERRIDES += \
            ro.zram.mark_idle_delay_mins=30 \
            ro.zram.first_wb_delay_mins=120 \
            ro.zram.periodic_wb_delay_hours=24
        ifneq ($(filter yes,$(BUILD_AGO_GMS) $(MTK_GMO_RAM_OPTIMIZE)),)
            $(call inherit-product, $(SRC_TARGET_DIR)/product/go_defaults.mk)
        endif
        PRODUCT_COPY_FILES += device/mediatek/vendor/common/ago/init/init.ago_default.rc:$(MTK_TARGET_VENDOR_RC)/init.ago.rc
    endif
    ifeq ($(strip $(MTK_K64_SUPPORT)), no)
        PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote32
    endif
#    DEVICE_PACKAGE_OVERLAYS += device/mediatek/vendor/common/overlay/ago

#    ifneq ($(wildcard vendor/mediatek/proprietary/packages/apps/SystemUI/Android.mk),)
#        PRODUCT_ENFORCE_RRO_TARGETS += MtkSystemUI
#    else
#        PRODUCT_ENFORCE_RRO_TARGETS += SystemUI
#    endif

ifneq ($(PLATFORM_VERSION),R)
    # Force android.hardware.cas@1.1 to be lazy for AGO project
    PRODUCT_PACKAGES += android.hardware.cas@1.1-service-lazy
endif

    # For overriding AOSP Files application
#    ifeq (yes,$(strip $(BUILD_AGO_GMS)))
#        ifneq ($(wildcard vendor/mediatek/proprietary/packages/apps/DocumentsUI/Android.mk),)
#            PRODUCT_ENFORCE_RRO_TARGETS += MtkDocumentsUI
#        else
#            PRODUCT_ENFORCE_RRO_TARGETS += DocumentsUI
#        endif
#    endif

    # Include Audio Go Audio Package
    $(call inherit-product-if-exists, frameworks/base/data/sounds/AudioPackageGo.mk)

    # Reduces GC frequency of foreground apps by 50%.
    # This change will increase RAM usage by at least a few MB and is not recommended for 512MB devices.
    ifneq (0x20000000,$(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)))
    PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.foreground-heap-growth-multiplier=2.0
    endif
    # Add f2fs property to enable f2fs service
    PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_f2fs_enable=1
    # Filesystem management tools
    PRODUCT_PACKAGES += fsck.f2fs mkfs.f2fs

    # HWUI
    PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.hwui.path_cache_size=0
    PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.hwui.text_small_cache_width=512
    PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.hwui.text_small_cache_height=256
    PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.hwui.disable_asset_atlas=true

    # For GSI invisible mkd properties
    PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
         ro.lmk.kill_heaviest_task=false

    PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
         pm.dexopt.downgrade_after_inactive_days=10

    PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
         pm.dexopt.shared=quicken


    # Disable fast starting window in GMO project
    PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.mtk_perf_fast_start_win=0

    # Add ago vm heap parameter
    PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapgrowthlimit=128m
    PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=256m

    # Disable USAP pool
    PRODUCT_PRODUCT_PROPERTIES += persist.device_config.runtime_native.usap_pool_enabled=false

    #Images for LCD test in factory mode
    PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/common/factory/res/images/lcd_test_00_gmo.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_00.png:mtk
    PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/common/factory/res/images/lcd_test_01_gmo.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_01.png:mtk
    PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/common/factory/res/images/lcd_test_02_gmo.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_02.png:mtk
else
    #non-ago settings
    # Add for Automatic Setting for heapgrowthlimit & heapsize
    RESOLUTION_HXW := $(shell expr $(LCM_HEIGHT) \* $(LCM_WIDTH))

    ifeq ($(shell test $(RESOLUTION_HXW) -ge 0 && test $(RESOLUTION_HXW) -lt 3500000 && echo true), true)
        PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapgrowthlimit=256m
        PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=512m
    endif

    ifeq ($(shell test $(RESOLUTION_HXW) -ge 3500000 && echo true), true)
        PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapgrowthlimit=384m
        PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=768m
    endif

    # $(call inherit-product, ($SRC_TARGET_DIR)/product/full_base.mk)

    # Handheld core hardware: Default
    PRODUCT_COPY_FILES += frameworks/native/data/etc/handheld_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/handheld_core_hardware.xml

    PRODUCT_COPY_FILES += device/mediatek/vendor/common/fstab.enableswap:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.enableswap:mtk
    PRODUCT_COPY_FILES += device/mediatek/vendor/common/ago/init/init.ago_default.rc:$(MTK_TARGET_VENDOR_RC)/init.ago.rc
    # Add the 1GB overlays (to enable pinning on 1GB but not 512)
#    DEVICE_PACKAGE_OVERLAYS += device/mediatek/vendor/common/overlay/ago/ago_1gb

    # Add MtkLauncher3 to replace Launcher3 when vendor code is available
    ifneq ($(wildcard vendor/mediatek/proprietary/packages/apps/Launcher3/Android.mk),)
        PRODUCT_PACKAGES += MtkLauncher3QuickStep
    endif
    # QuickSearchBox AOSP code will be replaced by MTK subject to availablity.
    ifneq ($(wildcard vendor/mediatek/proprietary/packages/apps/QuickSearchBox/Android.mk),)
        PRODUCT_PACKAGES += MtkQuickSearchBox
    endif
    # F2FS filesystem
    PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_f2fs_enable=0
    $(call inherit-product-if-exists, frameworks/base/data/sounds/AllAudio.mk)
endif

