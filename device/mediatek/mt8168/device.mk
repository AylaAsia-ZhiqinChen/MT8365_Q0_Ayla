# Do not copy rc files before this line !!
# RC files should goto /vendor since O-MR1
MTK_RC_TO_VENDOR = yes
ifeq ($(strip $(MTK_RC_TO_VENDOR)), yes)
  MTK_TARGET_VENDOR_RC = $(TARGET_COPY_OUT_VENDOR)/etc/init/hw
  PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.vendor.rc=/vendor/etc/init/hw/
  PRODUCT_COPY_FILES += device/mediatek/mt8168/ueventd.mt8168.rc:$(TARGET_COPY_OUT_VENDOR)/ueventd.rc
else
  MTK_TARGET_VENDOR_RC = root
  PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.vendor.rc=/
  PRODUCT_COPY_FILES += device/mediatek/mt8168/ueventd.mt8168.rc:root/ueventd.mt8168.rc
endif

ifeq ($(strip $(MTK_GPS_SUPPORT)), yes)
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml
else
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.xml
endif

PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml
endif

PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.software.midi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.midi.xml

ifneq ($(strip $(CUSTOM_KERNEL_STEP_COUNTER)),)
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml
endif


PRODUCT_PACKAGES += libI420colorconvert
PRODUCT_PACKAGES += vpud
PRODUCT_PACKAGES += v4l2vcodec-ut
PRODUCT_PACKAGES += libvcodec_utility
PRODUCT_PACKAGES += libvcodec_oal
PRODUCT_PACKAGES += libh264dec_xa.ca7
PRODUCT_PACKAGES += libh264dec_xb.ca7
PRODUCT_PACKAGES += libmp4dec_sa.ca7
PRODUCT_PACKAGES += libmp4dec_sb.ca7
PRODUCT_PACKAGES += libmp4enc_sa.ca7
PRODUCT_PACKAGES += libvp8dec_xa.ca7
PRODUCT_PACKAGES += libmp4enc_xa.ca7
PRODUCT_PACKAGES += libmp4enc_xb.ca7
PRODUCT_PACKAGES += libh264enc_sa.ca7
PRODUCT_PACKAGES += libh264enc_sb.ca7
PRODUCT_PACKAGES += libvc1dec_sa.ca7
PRODUCT_PACKAGES += libvideoeditorplayer
PRODUCT_PACKAGES += libvideoeditor_osal
PRODUCT_PACKAGES += libvideoeditor_3gpwriter
PRODUCT_PACKAGES += libvideoeditor_mcs
PRODUCT_PACKAGES += libvideoeditor_core
PRODUCT_PACKAGES += libvideoeditor_stagefrightshells
PRODUCT_PACKAGES += libvideoeditor_videofilters
PRODUCT_PACKAGES += libvideoeditor_jni
PRODUCT_PACKAGES += audio.primary.default
PRODUCT_PACKAGES += audio.primary.mt8168
PRODUCT_PACKAGES += sound_trigger.primary.mt8168
PRODUCT_PACKAGES += audio_policy.stub
PRODUCT_PACKAGES += local_time.default
PRODUCT_PACKAGES += libaudiocustparam
PRODUCT_PACKAGES += libh264dec_xa.ca9
PRODUCT_PACKAGES += libh264dec_xb.ca9
PRODUCT_PACKAGES += libh264dec_customize
PRODUCT_PACKAGES += libmp4dec_sa.ca9
PRODUCT_PACKAGES += libmp4dec_sb.ca9
PRODUCT_PACKAGES += libmp4dec_customize
PRODUCT_PACKAGES += libvp8dec_xa.ca9
PRODUCT_PACKAGES += libmp4enc_xa.ca9
PRODUCT_PACKAGES += libmp4enc_xb.ca9
PRODUCT_PACKAGES += libh264enc_sa.ca9
PRODUCT_PACKAGES += libh264enc_sb.ca9
PRODUCT_PACKAGES += libvcodec_oal
PRODUCT_PACKAGES += libvc1dec_sa.ca9
PRODUCT_PACKAGES += liblic_s263
PRODUCT_PACKAGES += init.factory.rc
PRODUCT_PACKAGES += libaudio.primary.default
PRODUCT_PACKAGES += audio_policy.default
PRODUCT_PACKAGES += audio_policy.mt8168
PRODUCT_PACKAGES += libaudio.a2dp.default
PRODUCT_PACKAGES += libMtkVideoTranscoder
PRODUCT_PACKAGES += libMtkOmxCore
PRODUCT_PACKAGES += libMtkOmxOsalUtils
PRODUCT_PACKAGES += libMtkOmxVdecEx
PRODUCT_PACKAGES += libMtkOmxVenc
PRODUCT_PACKAGES += libaudiodcrflt
PRODUCT_PACKAGES += libaudiosetting
PRODUCT_PACKAGES += librtp_jni
PRODUCT_PACKAGES += mfv_ut
PRODUCT_PACKAGES += libstagefrighthw
PRODUCT_PACKAGES += libstagefright_memutil
PRODUCT_PACKAGES += factory.ini
PRODUCT_PACKAGES += libmtdutil
PRODUCT_PACKAGES += libminiui
PRODUCT_PACKAGES += factory
PRODUCT_PACKAGES += drvbd
PRODUCT_PACKAGES += libaudio.usb.default
PRODUCT_PACKAGES += audio.usb.default
PRODUCT_PACKAGES += audio.usb.mt8168
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += AccountAndSyncSettings
PRODUCT_PACKAGES += DeskClock
PRODUCT_PACKAGES += AlarmProvider
PRODUCT_PACKAGES += Bluetooth
PRODUCT_PACKAGES += Calculator
PRODUCT_PACKAGES += Calendar
endif
PRODUCT_PACKAGES += CertInstaller
ifeq ($(strip $(MTK_OMADRM_SUPPORT)), yes)
  PRODUCT_PACKAGES += DrmProvider
endif
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += Email
PRODUCT_PACKAGES += FusedLocation
PRODUCT_PACKAGES += TelephonyProvider
PRODUCT_PACKAGES += Exchange2
endif
PRODUCT_PACKAGES += LatinIME
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += Music
PRODUCT_PACKAGES += MusicFX
PRODUCT_PACKAGES += Protips
PRODUCT_PACKAGES += QuickSearchBox
endif
PRODUCT_PACKAGES += Settings
PRODUCT_PACKAGES += Sync
PRODUCT_PACKAGES += SystemUI
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += Updater
PRODUCT_PACKAGES += CalendarProvider
PRODUCT_PACKAGES += batterywarning
endif
PRODUCT_PACKAGES += SyncProvider
PRODUCT_PACKAGES += Launcher3
PRODUCT_PACKAGES += disableapplist.txt
PRODUCT_PACKAGES += resmonwhitelist.txt
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += MTKThermalManager
endif
PRODUCT_PACKAGES += libmtcloader
PRODUCT_PACKAGES += thermal_manager
PRODUCT_PACKAGES += thermald
PRODUCT_PACKAGES += thermal
PRODUCT_PACKAGES += thermal.mt8168
PRODUCT_PACKAGES += thermalloadalgod
PRODUCT_PACKAGES += CellConnService
ifneq ($(strip $(OPTR_SPEC_SEG_DEF)),NONE)
    PRODUCT_PACKAGES += MTKAndroidSuiteDaemon
endif
PRODUCT_PACKAGES += libthermalalgo
PRODUCT_PACKAGES += libthha
PRODUCT_PACKAGES += libfmjni
PRODUCT_PACKAGES += libfmmt6616
PRODUCT_PACKAGES += libfmmt6626
PRODUCT_PACKAGES += libfmmt6620
PRODUCT_PACKAGES += libfmmt6628
PRODUCT_PACKAGES += libfmmt6627
PRODUCT_PACKAGES += libfmmt6630
PRODUCT_PACKAGES += libfmar1000
PRODUCT_PACKAGES += libfmcust
PRODUCT_PACKAGES += fm_cust.cfg
PRODUCT_PACKAGES += mt6620_fm_cust.cfg
PRODUCT_PACKAGES += mt6627_fm_cust.cfg
PRODUCT_PACKAGES += mt6628_fm_rom.bin
PRODUCT_PACKAGES += mt6628_fm_v1_patch.bin
PRODUCT_PACKAGES += mt6628_fm_v1_coeff.bin
PRODUCT_PACKAGES += mt6628_fm_v2_patch.bin
PRODUCT_PACKAGES += mt6628_fm_v2_coeff.bin
PRODUCT_PACKAGES += mt6628_fm_v3_patch.bin
PRODUCT_PACKAGES += mt6628_fm_v3_coeff.bin
PRODUCT_PACKAGES += mt6628_fm_v4_patch.bin
PRODUCT_PACKAGES += mt6628_fm_v4_coeff.bin
PRODUCT_PACKAGES += mt6628_fm_v5_patch.bin
PRODUCT_PACKAGES += mt6628_fm_v5_coeff.bin
PRODUCT_PACKAGES += mt6627_fm_v1_patch.bin
PRODUCT_PACKAGES += mt6627_fm_v1_coeff.bin
PRODUCT_PACKAGES += mt6627_fm_v2_patch.bin
PRODUCT_PACKAGES += mt6627_fm_v2_coeff.bin
PRODUCT_PACKAGES += mt6627_fm_v3_patch.bin
PRODUCT_PACKAGES += mt6627_fm_v3_coeff.bin
PRODUCT_PACKAGES += mt6627_fm_v4_patch.bin
PRODUCT_PACKAGES += mt6627_fm_v4_coeff.bin
PRODUCT_PACKAGES += mt6627_fm_v5_patch.bin
PRODUCT_PACKAGES += mt6627_fm_v5_coeff.bin
PRODUCT_PACKAGES += mt6630_fm_v1_patch.bin
PRODUCT_PACKAGES += mt6630_fm_v1_coeff.bin
PRODUCT_PACKAGES += mt6630_fm_v2_patch.bin
PRODUCT_PACKAGES += mt6630_fm_v2_coeff.bin
PRODUCT_PACKAGES += mt6630_fm_v3_patch.bin
PRODUCT_PACKAGES += mt6630_fm_v3_coeff.bin
PRODUCT_PACKAGES += mt6630_fm_v4_patch.bin
PRODUCT_PACKAGES += mt6630_fm_v4_coeff.bin
PRODUCT_PACKAGES += mt6630_fm_v5_patch.bin
PRODUCT_PACKAGES += mt6630_fm_v5_coeff.bin
PRODUCT_PACKAGES += calib.dat
PRODUCT_PACKAGES += param.dat
PRODUCT_PACKAGES += sensors.dat
PRODUCT_PACKAGES += sensors.mt8168
PRODUCT_PACKAGES += libhwm
PRODUCT_PACKAGES += lights.default
PRODUCT_PACKAGES += lights.mt8168
$(foreach custom_hal_msensorlib,$(CUSTOM_HAL_MSENSORLIB),$(eval PRODUCT_PACKAGES += lib$(custom_hal_msensorlib)))
PRODUCT_PACKAGES += meta_tst
PRODUCT_PACKAGES += dhcp6c
PRODUCT_PACKAGES += dhcp6ctl
PRODUCT_PACKAGES += dhcp6c.conf
PRODUCT_PACKAGES += dhcp6cDNS.conf
PRODUCT_PACKAGES += dhcp6s
PRODUCT_PACKAGES += dhcp6s.conf
PRODUCT_PACKAGES += dhcp6c.script
PRODUCT_PACKAGES += dhcp6cctlkey
PRODUCT_PACKAGES += libblisrc
PRODUCT_PACKAGES += libifaddrs
PRODUCT_PACKAGES += libbluetoothdrv
PRODUCT_PACKAGES += libbluetooth_mtk
PRODUCT_PACKAGES += libbluetoothem_mtk
PRODUCT_PACKAGES += libbluetooth_relayer
PRODUCT_PACKAGES += libmeta_bluetooth
PRODUCT_PACKAGES += libmobilelog_jni
PRODUCT_PACKAGES += libaudio.r_submix.default
PRODUCT_PACKAGES += audio.r_submix.default
PRODUCT_PACKAGES += audio.r_submix.mt8168
PRODUCT_PACKAGES += libaudio.usb.default
PRODUCT_PACKAGES += libnbaio
PRODUCT_PACKAGES += libaudioflinger
PRODUCT_PACKAGES += libmeta_audio
PRODUCT_PACKAGES += liba3m
PRODUCT_PACKAGES += libja3m
PRODUCT_PACKAGES += libmmprofile
PRODUCT_PACKAGES += libmmprofile_jni
PRODUCT_PACKAGES += libtvoutjni
PRODUCT_PACKAGES += libtvoutpattern
PRODUCT_PACKAGES += libmtkhdmi_jni
PRODUCT_PACKAGES += libmtkcam_modulefactory_custom
PRODUCT_PACKAGES += libmtkcam_modulefactory_drv
PRODUCT_PACKAGES += libmtkcam_modulefactory_aaa
PRODUCT_PACKAGES += libmtkcam_modulefactory_feature
PRODUCT_PACKAGES += libmtkcam_modulefactory_utils
PRODUCT_PACKAGES += libcam_platform
PRODUCT_PACKAGES += camerahalserver
PRODUCT_PACKAGES += android.hardware.camera.provider@2.4-service-mediatek
PRODUCT_PACKAGES += android.hardware.camera.provider@2.4-impl-mediatek
PRODUCT_PACKAGES += libmtkcam_device1
PRODUCT_PACKAGES += libmtkcam_device3
PRODUCT_PACKAGES += camera.default
PRODUCT_PACKAGES += camera.mt8168
PRODUCT_PACKAGES += liblog
PRODUCT_PACKAGES += shutdown
PRODUCT_PACKAGES += WIFI_RAM_CODE_MT6630
PRODUCT_PACKAGES += muxreport
PRODUCT_PACKAGES += mtkrild
PRODUCT_PACKAGES += mtk-ril
PRODUCT_PACKAGES += librilmtk
PRODUCT_PACKAGES += libutilrilmtk
PRODUCT_PACKAGES += gsm0710muxd
PRODUCT_PACKAGES += md_minilog_util
PRODUCT_PACKAGES += wbxml
PRODUCT_PACKAGES += wappush
PRODUCT_PACKAGES += thememap.xml
PRODUCT_PACKAGES += libBLPP.so
PRODUCT_PACKAGES += rc.fac
PRODUCT_PACKAGES += mtkGD
PRODUCT_PACKAGES += libdrm
PRODUCT_PACKAGES += libdrm.vendor
PRODUCT_PACKAGES += thermalindicator

# GPU collection
ifeq ($(strip $(MTK_GPU_SUPPORT)), yes)
  PRODUCT_PACKAGES += libgpu_aux
  PRODUCT_PACKAGES += libOpenCL
  PRODUCT_PACKAGES += libGLES_mali
  PRODUCT_PACKAGES += gralloc.mt8168
  PRODUCT_PACKAGES += libged.so
  ifneq ($(MTK_BASIC_PACKAGE), yes)
    PRODUCT_PACKAGES += libgas.so
  endif
  PRODUCT_PACKAGES += libgpud
  PRODUCT_PACKAGES += libRSDriver_mtk
  PRODUCT_PACKAGES += rs2spir
  PRODUCT_PACKAGES += spir2cl
  PRODUCT_PACKAGES += libGLES_meow
  PRODUCT_PACKAGES += libMEOW_trace

  PRODUCT_PROPERTY_OVERRIDES += ro.opengles.version=196610

  PRODUCT_PACKAGES += android.hardware.graphics.allocator@2.0-impl
  PRODUCT_PACKAGES += android.hardware.graphics.mapper@2.0-impl-2.1
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml
  ifneq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
    PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.vulkan.level-1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.level.xml
    PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.vulkan.version-1_1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.version.xml
    PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.vulkan.compute-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.compute.xml
    PRODUCT_PACKAGES += android.hardware.graphics.allocator@2.0-service
  endif

  PRODUCT_PROPERTY_OVERRIDES += ro.hardware.egl=mali
  PRODUCT_PACKAGES += memtrack.mt8168
  PRODUCT_PACKAGES += android.hardware.memtrack@1.0-impl
  ifneq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
    PRODUCT_PACKAGES += android.hardware.memtrack@1.0-service
  endif
  PRODUCT_PROPERTY_OVERRIDES += ro.hardware.egl=mali
else
  PRODUCT_PACKAGES += libgpu_aux
  PRODUCT_PACKAGES += libOpenCL
  PRODUCT_PACKAGES += ged_srv
  PRODUCT_PACKAGES += libged.so
  ifneq ($(MTK_BASIC_PACKAGE), yes)
    PRODUCT_PACKAGES += libgas.so
  endif
  PRODUCT_PACKAGES += libRSDriver_mtk
  PRODUCT_PACKAGES += rs2spir
  PRODUCT_PACKAGES += spir2cl

  PRODUCT_PROPERTY_OVERRIDES += ro.opengles.version=196609
  PRODUCT_PACKAGES += android.hardware.graphics.allocator@2.0-impl
  PRODUCT_PACKAGES += android.hardware.graphics.mapper@2.0-impl
  PRODUCT_PACKAGES += android.hardware.graphics.allocator@2.0-service

  PRODUCT_PACKAGES += libEGL_swiftshader
  PRODUCT_PACKAGES += libGLESv1_CM_swiftshader
  PRODUCT_PACKAGES += libGLESv2_swiftshader
endif

PRODUCT_PACKAGES += libFraunhoferAAC
PRODUCT_PACKAGES += audiocmdservice_atci
PRODUCT_PACKAGES += libMtkOmxAudioEncBase
PRODUCT_PACKAGES += libMtkOmxAmrEnc
PRODUCT_PACKAGES += libMtkOmxAwbEnc
PRODUCT_PACKAGES += libMtkOmxAacEnc
PRODUCT_PACKAGES += libMtkOmxVorbisEnc
PRODUCT_PACKAGES += libMtkOmxAdpcmEnc
PRODUCT_PACKAGES += libMtkOmxMp3Dec
PRODUCT_PACKAGES += libMtkOmxGsmDec
PRODUCT_PACKAGES += libMtkOmxAacDec
PRODUCT_PACKAGES += libMtkOmxG711Dec
PRODUCT_PACKAGES += libMtkOmxVorbisDec
PRODUCT_PACKAGES += libMtkOmxAudioDecBase
PRODUCT_PACKAGES += libMtkOmxAdpcmDec
PRODUCT_PACKAGES += libMtkOmxWmaDec
PRODUCT_PACKAGES += libMtkOmxRawDec
PRODUCT_PACKAGES += libMtkOmxAMRNBDec
PRODUCT_PACKAGES += libMtkOmxAMRWBDec
PRODUCT_PACKAGES += libvoicerecognition_jni
PRODUCT_PACKAGES += libvoicerecognition
PRODUCT_PACKAGES += libphonemotiondetector_jni
PRODUCT_PACKAGES += libphonemotiondetector
PRODUCT_PACKAGES += libmotionrecognition
PRODUCT_PACKAGES += libasf
PRODUCT_PACKAGES += libasfextractor
PRODUCT_PACKAGES += audio.primary.default
PRODUCT_PACKAGES += audio_policy.stub
PRODUCT_PACKAGES += audio_policy.default
PRODUCT_PACKAGES += libaudio.primary.default
PRODUCT_PACKAGES += libaudio.a2dp.default
PRODUCT_PACKAGES += audio.a2dp.default
PRODUCT_PACKAGES += libaudio-resampler
PRODUCT_PACKAGES += local_time.default
PRODUCT_PACKAGES += libaudiocustparam
PRODUCT_PACKAGES += libaudiocomponentengine
PRODUCT_PACKAGES += libaudiocomponentengine_vendor
PRODUCT_PACKAGES += libaudiodcrflt
PRODUCT_PACKAGES += libaudiosetting
PRODUCT_PACKAGES += librtp_jni
PRODUCT_PACKAGES += libmatv_cust
PRODUCT_PACKAGES += libmtkplayer
PRODUCT_PACKAGES += libatvctrlservice
PRODUCT_PACKAGES += matv
PRODUCT_PACKAGES += libMtkOmxApeDec
PRODUCT_PACKAGES += libMtkOmxFlacDec
PRODUCT_PACKAGES += ppp_dt
PRODUCT_PACKAGES += power.mt8168

PRODUCT_PACKAGES += android.hardware.power@1.3-impl-mediatek
PRODUCT_PACKAGES += vendor.mediatek.hardware.power@2.1-impl
PRODUCT_PACKAGES += vendor.mediatek.hardware.mtkpower@1.0-impl
PRODUCT_PACKAGES += vendor.mediatek.hardware.mtkpower@1.0-service

PRODUCT_PACKAGES += vendor.mediatek.hardware.power@2.0-impl
ifneq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
    PRODUCT_PACKAGES += vendor.mediatek.hardware.power@2.0-service
else
    PRODUCT_PACKAGES += vendor.mediatek.hardware.power@2.0-impl_vendor
endif
PRODUCT_PACKAGES += libdiagnose
PRODUCT_PACKAGES += mnld
PRODUCT_PACKAGES += libmnlp
PRODUCT_PACKAGES += libmnlp_mt6628
PRODUCT_PACKAGES += libmnlp_mt6620
PRODUCT_PACKAGES += libmnlp_mt3332
PRODUCT_PACKAGES += libmnlp_mt8168
PRODUCT_PACKAGES += gps.mt8168
PRODUCT_PACKAGES += libmnl.a
PRODUCT_PACKAGES += libsupl.a
PRODUCT_PACKAGES += libhotstill.a
PRODUCT_PACKAGES += libagent.a
PRODUCT_PACKAGES += libsonivox
PRODUCT_PACKAGES += iAmCdRom.iso
PRODUCT_PACKAGES += libmemorydumper
PRODUCT_PACKAGES += memorydumper
PRODUCT_PACKAGES += libvt_custom
PRODUCT_PACKAGES += DcxoSetCap
PRODUCT_PACKAGES += libamrvt
PRODUCT_PACKAGES += libvtmal
PRODUCT_PACKAGES += libipsec_ims
PRODUCT_PACKAGES += racoon
PRODUCT_PACKAGES += libipsec
PRODUCT_PACKAGES += libpcap
PRODUCT_PACKAGES += mtpd
PRODUCT_PACKAGES += netcfg
PRODUCT_PACKAGES += pppd
PRODUCT_PACKAGES += pppd_via
PRODUCT_PACKAGES += pppd_dt
PRODUCT_PACKAGES += dhcpcd
PRODUCT_PACKAGES += dhcpcd.conf
PRODUCT_PACKAGES += dhcpcd-run-hooks
PRODUCT_PACKAGES += 20-dns.conf
PRODUCT_PACKAGES += 95-configured
PRODUCT_PACKAGES += radvd
PRODUCT_PACKAGES += radvd.conf
PRODUCT_PACKAGES += dnsmasq
PRODUCT_PACKAGES += netd
PRODUCT_PACKAGES += ndc
PRODUCT_PACKAGES += libiprouteutil
PRODUCT_PACKAGES += libnetlink
PRODUCT_PACKAGES += tc
PRODUCT_PACKAGES += e2fsck
PRODUCT_PACKAGES += libext2_blkid
PRODUCT_PACKAGES += libext2_e2p
PRODUCT_PACKAGES += libext2_com_err
PRODUCT_PACKAGES += libext2fs
PRODUCT_PACKAGES += libext2_uuid
PRODUCT_PACKAGES += mke2fs
PRODUCT_PACKAGES += tune2fs
PRODUCT_PACKAGES += badblocks
PRODUCT_PACKAGES += resize2fs
PRODUCT_PACKAGES += resize.f2fs
PRODUCT_PACKAGES += libnvram
PRODUCT_PACKAGES += libnvram_daemon_callback
PRODUCT_PACKAGES += libfile_op
ifneq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
    PRODUCT_PACKAGES += nvram_agent_binder
endif
PRODUCT_PACKAGES += nvram_daemon
PRODUCT_PACKAGES += vendor.mediatek.hardware.nvram@1.1
PRODUCT_PACKAGES += vendor.mediatek.hardware.nvram@1.1-impl
PRODUCT_PACKAGES += vendor.mediatek.hardware.nvram@1.1-service
PRODUCT_PACKAGES += make_ext4fs
PRODUCT_PACKAGES += sdcard
PRODUCT_PACKAGES += libext
PRODUCT_PACKAGES += libext4
PRODUCT_PACKAGES += libext6
PRODUCT_PACKAGES += libxtables
PRODUCT_PACKAGES += libip4tc
PRODUCT_PACKAGES += libip6tc
PRODUCT_PACKAGES += ipod
PRODUCT_PACKAGES += libipod
PRODUCT_PACKAGES += gatord
PRODUCT_PACKAGES += fuelgauged
PRODUCT_PACKAGES += fuelgauged_nvram
ifeq ($(MTK_GAUGE_VERSION), 30)
PRODUCT_PACKAGES += libfgauge_gm30
else
PRODUCT_PACKAGES += libfgauge
endif
PRODUCT_PACKAGES += android.hardware.health@2.0-service
PRODUCT_PACKAGES += android.hardware.health@2.0-service.override
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += boot_logo_updater
PRODUCT_PACKAGES += boot_logo
endif
PRODUCT_PACKAGES += bootanimation
ifneq (,$(filter yes, $(MTK_KERNEL_POWER_OFF_CHARGING)))
PRODUCT_PACKAGES += kpoc_charger
endif
PRODUCT_PACKAGES += libtvoutjni
PRODUCT_PACKAGES += libtvoutpattern
PRODUCT_PACKAGES += libmtkhdmi_jni
PRODUCT_PACKAGES += libhissage.so
PRODUCT_PACKAGES += libhpe.so
PRODUCT_PACKAGES += sdiotool
PRODUCT_PACKAGES += superumount
PRODUCT_PACKAGES += libsched
PRODUCT_PACKAGES += fsck_msdos_mtk
PRODUCT_PACKAGES += cmmbsp
PRODUCT_PACKAGES += libcmmb_jni
PRODUCT_PACKAGES += robotium
PRODUCT_PACKAGES += libc_malloc_debug_mtk
PRODUCT_PACKAGES += dpfd
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += SchedulePowerOnOff
PRODUCT_PACKAGES += BatteryWarning
PRODUCT_PACKAGES += libpq_cust
PRODUCT_PACKAGES += libpq_cust_mtk
PRODUCT_PACKAGES += libPQjni
PRODUCT_PACKAGES += libPQDCjni
PRODUCT_PACKAGES += MiraVision
PRODUCT_PACKAGES += libMiraVision_jni
endif
PRODUCT_PACKAGES += dmlog
PRODUCT_PACKAGES += mtk_msr.ko
PRODUCT_PACKAGES += ext4_resize
PRODUCT_PACKAGES += send_bug
ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += met-cmd
PRODUCT_PACKAGES += libmet-tag
PRODUCT_PACKAGES += met_log_d
PRODUCT_PACKAGES += met.ko
PRODUCT_PACKAGES += met_plf.ko
PRODUCT_PACKAGES += trace-cmd
PRODUCT_PACKAGES += libMtkOmxRawDec
PRODUCT_PACKAGES += libperfservice
PRODUCT_PACKAGES += libperfservicenative
PRODUCT_PACKAGES += libpowerhal
PRODUCT_PACKAGES += libpowerhalctl
PRODUCT_PACKAGES += libpowerhalctl_vendor
PRODUCT_PACKAGES += libpowerhalwrap
PRODUCT_PACKAGES += libpowerhalwrap_vendor
PRODUCT_PACKAGES += libperfctl
PRODUCT_PACKAGES += libperfctl_vendor
PRODUCT_PACKAGES += power_native_test_v_2_0
PRODUCT_PACKAGES += power_app_cfg.xml
PRODUCT_PACKAGES += powerscntbl.xml
PRODUCT_PACKAGES += powerboosttbl.cfg
PRODUCT_PACKAGES += powercontable.xml
PRODUCT_PACKAGES += fstb.cfg
endif
PRODUCT_PACKAGES += Videos
PRODUCT_PACKAGES += sn
PRODUCT_PACKAGES += lcdc_screen_cap
PRODUCT_PACKAGES += libJniAtvService
PRODUCT_PACKAGES += GoogleKoreanIME

ifndef MTK_TB_WIFI_3G_MODE
  PRODUCT_PACKAGES += Mms
else
  ifeq ($(strip $(MTK_TB_WIFI_3G_MODE)), 3GDATA_SMS)
    PRODUCT_PACKAGES += Mms
  endif
endif

PRODUCT_PACKAGES += libjni_koreanime.so
PRODUCT_PACKAGES += wpa_supplicant
PRODUCT_PACKAGES += wpa_cli
PRODUCT_PACKAGES += wpa_supplicant.conf
PRODUCT_PACKAGES += wpa_supplicant_overlay.conf
PRODUCT_PACKAGES += p2p_supplicant_overlay.conf
PRODUCT_PACKAGES += hostapd
PRODUCT_PACKAGES += hostapd_cli
PRODUCT_PACKAGES += lib_driver_cmd_mt66xx.a
PRODUCT_PACKAGES += CallLogBackup
PRODUCT_PACKAGES += libacdk

# camtool
PRODUCT_PACKAGES += libcam.hal3a.cctsvr
PRODUCT_PACKAGES += camtool
PRODUCT_PACKAGES += jpegtool

PRODUCT_PACKAGES += md_ctrl

ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
PRODUCT_PACKAGES += hwcomposer.mt8168
endif
PRODUCT_PACKAGES += \
                android.hardware.graphics.composer@2.1-impl \
                android.hardware.graphics.composer@2.1-service

ifeq ($(strip $(MTK_CCCI_PERMISSION_CHECK_SUPPORT)),yes)
PRODUCT_PACKAGES += permission_check
PRODUCT_PROPERTY_OVERRIDES += persist.md.perm.checked=to_upgrade
endif

ifeq ($(strip $(BUILD_MTK_LDVT)),yes)
PRODUCT_PACKAGES += ts_uvvf
endif

PRODUCT_PACKAGES += fstab.mt8168
PRODUCT_PACKAGES += fstab.mt8168.ramdisk

#effect, gesture, imageTransform
#PRODUCT_PACKAGES += libjni_effects
#PRODUCT_PACKAGES += libjni_gesture
#PRODUCT_PACKAGES += libjni_imagetransform

ifeq ($(strip $(MTK_WFD_HDCP_TX_SUPPORT)), yes)
  PRODUCT_PACKAGES += libstagefright_hdcp
  PRODUCT_PACKAGES += liburee_meta_drmkeyinstall_v2
endif

ifeq ($(strip $(MTK_WFD_HDCP_RX_SUPPORT)), yes)
  PRODUCT_PACKAGES += libstagefright_hdcprx
  PRODUCT_PACKAGES += liburee_meta_drmkeyinstall_v2
endif

########################### 1 - EMMC #####################################
######## /dev/block/platform/mtk-msdc.0/11230000.MSDC0/by-name/kb   ######
######## /dev/block/platform/mtk-msdc.0/11230000.MSDC0/by-name/dkb  ######
########################### 2 - NAND #####################################
######################### /dev/kb   ######################################
######################### /dev/dkb  ######################################
########################### 3 - 8168 EMMC ################################
######## /dev/block/platform/mtk-msdc.0/11230000.MSDC0/by-name/kb   ######
######## /dev/block/platform/mtk-msdc.0/11230000.MSDC0/by-name/dkb  ######

ifeq ($(strip $(MTK_DRM_KEY_MNG_SUPPORT)),yes)
  PRODUCT_PACKAGES += kisd
  PRODUCT_PACKAGES += liburee_meta_drmkeyinstall_v2

  ifeq ($(strip $(MTK_EMMC_SUPPORT)), yes)
    ifeq ($(strip $(MTK_PLATFORM)), MT8168)
      #ADDITIONAL_DEFAULT_PROPERTIES += ro.mtk_key_manager_kb_path=4
      PRODUCT_PROPERTY_OVERRIDES += ro.mtk_key_manager_kb_path=4
    else
      #ADDITIONAL_DEFAULT_PROPERTIES += ro.mtk_key_manager_kb_path=1
      PRODUCT_PROPERTY_OVERRIDES += ro.mtk_key_manager_kb_path=1
    endif
  else
    #ADDITIONAL_DEFAULT_PROPERTIES += ro.mtk_key_manager_kb_path=2
    PRODUCT_PROPERTY_OVERRIDES += ro.mtk_key_manager_kb_path=2
  endif

endif

ifeq ($(strip $(MTK_APP_GUIDE)),yes)
  PRODUCT_PACKAGES += ApplicationGuide
endif

ifeq ($(strip $(MTK_FLV_PLAYBACK_SUPPORT)), yes)
  PRODUCT_PACKAGES += libflv
  PRODUCT_PACKAGES += libflvextractor
endif

ifneq ($(strip $(foreach value,$(DFO_NVRAM_SET),$(filter yes,$($(value))))),)
  PRODUCT_PACKAGES += featured
  PRODUCT_PACKAGES += libdfo
  PRODUCT_PACKAGES += libdfo_jni
endif

ifeq ($(strip $(MTK_CDS_EM_SUPPORT)), yes)
  PRODUCT_PACKAGES += CDS_INFO
endif

ifeq ($(strip $(MTK_WLAN_SUPPORT)), yes)
  ifeq ($(strip $(MTK_PLATFORM)), MT8127)
    PRODUCT_PACKAGES += WIFI_RAM_CODE_8127
  else
    ifeq ($(strip $(MTK_PLATFORM)), MT6571)
      PRODUCT_PACKAGES += WIFI_RAM_CODE_6571
    else
      ifeq ($(strip $(MTK_PLATFORM)), MT6752)
        PRODUCT_PACKAGES += WIFI_RAM_CODE_6752
      else
          ifeq ($(strip $(MTK_PLATFORM)), MT8168)
            PRODUCT_PACKAGES += WIFI_RAM_CODE_8168
          else
            PRODUCT_PACKAGES += WIFI_RAM_CODE_SOC
          endif
      endif
    endif
  endif
endif

ifeq ($(TARGET_BUILD_VARIANT),user)
  PRODUCT_DEFAULT_PROPERTY_OVERRIDES += persist.sys.usb.config=mtp
else
  PRODUCT_DEFAULT_PROPERTY_OVERRIDES += persist.sys.usb.config=adb
endif

ifeq ($(strip $(MTK_EMMC_SUPPORT)), yes)
  PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.mount.fs=EXT4
else
  ifeq ($(strip $(MTK_NAND_UBIFS_SUPPORT)), yes)
    PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.mount.fs=UBIFS
  else
    PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.mount.fs=YAFFS
  endif
endif

ifeq ($(strip $(MTK_DATAUSAGE_SUPPORT)), yes)
  ifeq ($(strip $(MTK_DATAUSAGELOCKSCREENCLIENT_SUPPORT)), yes)
    PRODUCT_PACKAGES += DataUsageLockScreenClient
  endif
endif

ifeq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
  PRODUCT_PACKAGES += SDKGallery
else
  PRODUCT_PACKAGES += Gallery2
  PRODUCT_PACKAGES += Gallery2Root
  PRODUCT_PACKAGES += Gallery2Drm
  PRODUCT_PACKAGES += Gallery2Gif
  PRODUCT_PACKAGES += Gallery2Pq
  PRODUCT_PACKAGES += Gallery2PqTool
  PRODUCT_PACKAGES += Gallery2Raw
  PRODUCT_PACKAGES += Gallery2SlowMotion
  PRODUCT_PACKAGES += Gallery2StereoEntry
  PRODUCT_PACKAGES += Gallery2StereoCopyPaste
  PRODUCT_PACKAGES += Gallery2StereoBackground
  PRODUCT_PACKAGES += Gallery2StereoFancyColor
  PRODUCT_PACKAGES += Gallery2StereoRefocus
  PRODUCT_PACKAGES += Gallery2PhotoPicker
endif

ifneq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
  PRODUCT_PACKAGES += Provision
endif

ifeq ($(strip $(HAVE_CMMB_FEATURE)), yes)
  PRODUCT_PACKAGES += CMMBPlayer
endif

ifeq ($(strip $(MTK_DATA_TRANSFER_APP)), yes)
  PRODUCT_PACKAGES += DataTransfer
endif

ifeq ($(strip $(MTK_MDM_APP)),yes)
  PRODUCT_PACKAGES += MediatekDM
endif

ifeq ($(strip $(MTK_VT3G324M_SUPPORT)),yes)
  PRODUCT_PACKAGES += libmtk_vt_client
  PRODUCT_PACKAGES += libmtk_vt_em
  PRODUCT_PACKAGES += libmtk_vt_utils
  PRODUCT_PACKAGES += libmtk_vt_service
  PRODUCT_PACKAGES += libmtk_vt_swip
  PRODUCT_PACKAGES += vtservice
endif

ifeq ($(strip $(MTK_OOBE_APP)),yes)
  PRODUCT_PACKAGES += OOBE
endif

ifeq ($(strip $(MTK_MEDIA3D_APP)), yes)
  PRODUCT_PACKAGES += Media3D
endif

ifdef MTK_WEATHER_PROVIDER_APP
  ifneq ($(strip $(MTK_WEATHER_PROVIDER_APP)), no)
    PRODUCT_PACKAGES += MtkWeatherProvider
  endif
endif

ifeq ($(strip $(MTK_ENABLE_VIDEO_EDITOR)),yes)
  PRODUCT_PACKAGES += VideoEditor
endif

ifeq ($(strip $(MTK_CALENDAR_IMPORTER_APP)), yes)
  PRODUCT_PACKAGES += CalendarImporter
endif

ifeq ($(strip $(MTK_LOG2SERVER_APP)), yes)
  PRODUCT_PACKAGES += Log2Server
  PRODUCT_PACKAGES += Excftpcommonlib
  PRODUCT_PACKAGES += Excactivationlib
  PRODUCT_PACKAGES += Excadditionnallib
  PRODUCT_PACKAGES += Excmaillib
endif

ifeq ($(strip $(MTK_CAMERA_APP)), yes)
  PRODUCT_PACKAGES += CameraOpen
else
  PRODUCT_PACKAGES += Camera
  PRODUCT_PACKAGES += Panorama
  PRODUCT_PACKAGES += NativePip
  PRODUCT_PACKAGES += SlowMotion
  PRODUCT_PACKAGES += CameraRoot
endif

ifeq ($(strip $(MTK_VIDEO_FAVORITES_WIDGET_APP)), yes)
  ifneq ($(strip $(MTK_TABLET_PLATFORM)), yes)
    PRODUCT_PACKAGES += VideoFavorites
    PRODUCT_PACKAGES += libjtranscode
  endif
endif

ifeq ($(strip $(MTK_VIDEOWIDGET_APP)),yes)
  PRODUCT_PACKAGES += MtkVideoWidget
endif

ifeq ($(strip $(MTK_RCSE_SUPPORT)), yes)
  PRODUCT_PACKAGES += Rcse
  PRODUCT_PACKAGES += Provisioning
endif

ifeq ($(strip $(MTK_GPS_SUPPORT)), yes)
  PRODUCT_PACKAGES += BGW
  PRODUCT_PROPERTY_OVERRIDES += bgw.current3gband=0
endif

ifeq ($(strip $(MTK_STEREO3D_WALLPAPER_APP)), yes)
  PRODUCT_PACKAGES += Stereo3DWallpaper
endif

ifeq ($(strip $(MTK_GPS_SUPPORT)), yes)
  ifeq ($(strip $(MTK_GPS_CHIP)), MTK_GPS_MT6620)
    PRODUCT_PROPERTY_OVERRIDES += gps.solution.combo.chip=1
  endif
  ifeq ($(strip $(MTK_GPS_CHIP)), MTK_GPS_MT6628)
    PRODUCT_PROPERTY_OVERRIDES += gps.solution.combo.chip=1
  endif
  ifeq ($(strip $(MTK_GPS_CHIP)), MTK_GPS_MT3332)
    PRODUCT_PROPERTY_OVERRIDES += gps.solution.combo.chip=0
  endif
endif

ifeq ($(strip $(MTK_NAND_UBIFS_SUPPORT)),yes)
  PRODUCT_PACKAGES += mkfs_ubifs
  PRODUCT_PACKAGES += ubinize
  PRODUCT_PACKAGES += mtdinfo
  PRODUCT_PACKAGES += ubiupdatevol
  PRODUCT_PACKAGES += ubirmvol
  PRODUCT_PACKAGES += ubimkvol
  PRODUCT_PACKAGES += ubidetach
  PRODUCT_PACKAGES += ubiattach
  PRODUCT_PACKAGES += ubinfo
  PRODUCT_PACKAGES += ubiformat
endif

ifeq ($(strip $(MTK_LIVEWALLPAPER_APP)), yes)
  PRODUCT_PACKAGES += LiveWallpapers
  PRODUCT_PACKAGES += LiveWallpapersPicker
  PRODUCT_PACKAGES += MagicSmokeWallpapers
  PRODUCT_PACKAGES += VisualizationWallpapers
  PRODUCT_PACKAGES += Galaxy4
  PRODUCT_PACKAGES += HoloSpiralWallpaper
  PRODUCT_PACKAGES += NoiseField
  PRODUCT_PACKAGES += PhaseBeam
endif

ifeq ($(strip $(MTK_SNS_SUPPORT)), yes)
  PRODUCT_PACKAGES += SNSCommon
  PRODUCT_PACKAGES += SnsContentProvider
  PRODUCT_PACKAGES += SnsWidget
  PRODUCT_PACKAGES += SnsWidget24
  PRODUCT_PACKAGES += SocialStream
  ifeq ($(strip $(MTK_SNS_KAIXIN_APP)), yes)
    PRODUCT_PACKAGES += KaiXinAccountService
  endif
  ifeq ($(strip $(MTK_SNS_RENREN_APP)), yes)
    PRODUCT_PACKAGES += RenRenAccountService
  endif
  ifeq ($(strip $(MTK_SNS_FACEBOOK_APP)), yes)
    PRODUCT_PACKAGES += FacebookAccountService
  endif
  ifeq ($(strip $(MTK_SNS_FLICKR_APP)), yes)
    PRODUCT_PACKAGES += FlickrAccountService
  endif
  ifeq ($(strip $(MTK_SNS_TWITTER_APP)), yes)
    PRODUCT_PACKAGES += TwitterAccountService
  endif
  ifeq ($(strip $(MTK_SNS_SINAWEIBO_APP)), yes)
    PRODUCT_PACKAGES += WeiboAccountService
  endif
endif

ifeq ($(strip $(MTK_DATADIALOG_APP)), yes)
  PRODUCT_PACKAGES += DataDialog
endif

ifeq ($(strip $(MTK_DATA_TRANSFER_APP)), yes)
  PRODUCT_PACKAGES += DataTransfer
endif

ifeq ($(strip $(MTK_FM_SUPPORT)), yes)
  PRODUCT_PACKAGES += FMRadio
endif

ifeq ($(strip $(MTK_CAM_LOMO_SUPPORT)), yes)
  PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_cam_lomo_support=1
endif
ifeq (MT6620_FM,$(strip $(MTK_FM_CHIP)))
  PRODUCT_PROPERTY_OVERRIDES += fmradio.driver.chip=1
endif

ifeq (MT6626_FM,$(strip $(MTK_FM_CHIP)))
  PRODUCT_PROPERTY_OVERRIDES += fmradio.driver.chip=2
endif

ifeq (MT6628_FM,$(strip $(MTK_FM_CHIP)))
  PRODUCT_PROPERTY_OVERRIDES += fmradio.driver.chip=3
endif

ifeq ($(strip $(MTK_BT_SUPPORT)), yes)
  PRODUCT_PROPERTY_OVERRIDES += \
        ro.btstack=blueangel
  PRODUCT_PACKAGES += libbtem
  PRODUCT_PACKAGES += libextem
  PRODUCT_PACKAGES += auto_pair_blacklist.conf
  PRODUCT_PACKAGES += btconfig.xml
  PRODUCT_PACKAGES += MtkBt
  PRODUCT_PACKAGES += libbtcusttable
  PRODUCT_PACKAGES += libbtcust
  PRODUCT_PACKAGES += libmtkbtextadp
  PRODUCT_PACKAGES += libextpbap
  PRODUCT_PACKAGES += libextavrcp
  PRODUCT_PACKAGES += libextopp
  PRODUCT_PACKAGES += libextsys
  PRODUCT_PACKAGES += libextftp
  PRODUCT_PACKAGES += libmtkbtextadpa2dp
  PRODUCT_PACKAGES += libmtka2dp
  PRODUCT_PACKAGES += libextbip
  PRODUCT_PACKAGES += libextbpp
  PRODUCT_PACKAGES += libexthid
  PRODUCT_PACKAGES += libextsimap
  PRODUCT_PACKAGES += libextjsr82
  PRODUCT_PACKAGES += libbtsession
  PRODUCT_PACKAGES += libmtkbtextpan
  PRODUCT_PACKAGES += libextmap
  PRODUCT_PACKAGES += libmtkbtextspp
  PRODUCT_PACKAGES += libexttestmode
  PRODUCT_PACKAGES += libextopp_jni
  PRODUCT_PACKAGES += libexthid_jni
  PRODUCT_PACKAGES += libextpan_jni
  PRODUCT_PACKAGES += libextftp_jni
  PRODUCT_PACKAGES += libextbpp_jni
  PRODUCT_PACKAGES += libextbip_jni
  PRODUCT_PACKAGES += libextpbap_jni
  PRODUCT_PACKAGES += libextavrcp_jni
  PRODUCT_PACKAGES += libextsimap_jni
  PRODUCT_PACKAGES += libextdun_jni
  PRODUCT_PACKAGES += libextmap_jni
  PRODUCT_PACKAGES += libextsys_jni
  PRODUCT_PACKAGES += libextadvanced_jni
  PRODUCT_PACKAGES += btlogmask
  PRODUCT_PACKAGES += btconfig
  PRODUCT_PACKAGES += libbtpcm
  PRODUCT_PACKAGES += libbtsniff
  PRODUCT_PACKAGES += mtkbt
  PRODUCT_PACKAGES += bluetooth.blueangel
  PRODUCT_PACKAGES += audio.a2dp.blueangel
endif

ifeq ($(strip $(MTK_DT_SUPPORT)),yes)
  ifneq ($(strip $(EVDO_DT_SUPPORT)),yes)
    ifeq ($(strip $(MTK_MDLOGGER_SUPPORT)),yes)
      PRODUCT_PACKAGES += ExtModemLog
      PRODUCT_PACKAGES += libextmdlogger_ctrl_jni
      PRODUCT_PACKAGES += libextmdlogger_ctrl
      PRODUCT_PACKAGES += extmdlogger
    endif
  endif
endif

ifneq ($(strip $(MTK_LCM_PHYSICAL_ROTATION)),)
  PRODUCT_PROPERTY_OVERRIDES += ro.sf.hwrotation=$(MTK_LCM_PHYSICAL_ROTATION)
endif

ifeq ($(strip $(MTK_FM_TX_SUPPORT)), yes)
  PRODUCT_PACKAGES += FMTransmitter
endif

ifeq ($(strip $(MTK_SOUNDRECORDER_APP)),yes)
  PRODUCT_PACKAGES += SoundRecorder
endif

ifeq ($(strip $(MTK_WEATHER3D_WIDGET)), yes)
  ifneq ($(strip $(MTK_TABLET_PLATFORM)), yes)
    PRODUCT_PACKAGES += Weather3DWidget
  endif
endif

ifeq ($(strip $(MTK_LOCKSCREEN_TYPE)),2)
  PRODUCT_PACKAGES += MtkWallPaper
endif

ifneq ($(strip $(MTK_LOCKSCREEN_TYPE)),)
  PRODUCT_PROPERTY_OVERRIDES += curlockscreen=$(MTK_LOCKSCREEN_TYPE)
endif

ifeq ($(strip $(MTK_OMA_DOWNLOAD_SUPPORT)),yes)
  PRODUCT_PACKAGES += Browser
  PRODUCT_PACKAGES += DownloadProvider
endif

ifeq ($(strip $(MTK_WIFI_P2P_SUPPORT)),yes)
  PRODUCT_PACKAGES += WifiContactSync
  PRODUCT_PACKAGES += WifiP2PWizardy
  PRODUCT_PACKAGES += FileSharingServer
  PRODUCT_PACKAGES += FileSharingClient
  PRODUCT_PACKAGES += UPnPAV
  PRODUCT_PACKAGES += WifiWsdsrv
  PRODUCT_PACKAGES += bonjourExplorer
endif

ifeq ($(strip $(CUSTOM_KERNEL_TOUCHPANEL)),generic)
  PRODUCT_PACKAGES += Calibrator
endif

ifeq ($(strip $(MTK_FILEMANAGER_APP)), yes)
  PRODUCT_PACKAGES += FileManager
endif

ifneq ($(findstring OP03, $(strip $(OPTR_SPEC_SEG_DEF))),)
  PRODUCT_PACKAGES += SimCardAuthenticationService
endif

ifeq ($(strip $(MTK_NFC_OMAAC_SUPPORT)),yes)
  PRODUCT_PACKAGES += SmartcardService
  PRODUCT_PACKAGES += org.simalliance.openmobileapi
  PRODUCT_PACKAGES += org.simalliance.openmobileapi.xml
  PRODUCT_PACKAGES += libassd
endif

ifeq ($(strip $(MTK_APKINSTALLER_APP)), yes)
  PRODUCT_PACKAGES += APKInstaller
endif

ifeq ($(strip $(MTK_SMSREG_APP)), yes)
  PRODUCT_PACKAGES += SmsReg
endif

ifeq ($(strip $(MTK_STEREO3D_WALLPAPER_APP)), yes)
  PRODUCT_PACKAGES += Stereo3DWallpaper
endif

ifeq ($(strip $(MTK_WEATHER3D_WIDGET)), yes)
  PRODUCT_PACKAGES += Weather3DWidget
endif

ifeq ($(MTK_BACKUPANDRESTORE_APP),yes)
  PRODUCT_PACKAGES += BackupAndRestore
endif

ifeq ($(strip $(MTK_BWC_SUPPORT)), yes)
  PRODUCT_PACKAGES += libbwc
endif

ifeq ($(strip $(MTK_DT_SUPPORT)),yes)
  PRODUCT_PACKAGES += ip-up
  PRODUCT_PACKAGES += ip-down
  PRODUCT_PACKAGES += ppp_options
  PRODUCT_PACKAGES += chap-secrets
  PRODUCT_PACKAGES += init.gprs-pppd
endif

# OEM Unlock reporting
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    ro.oem_unlock_supported=1


ifeq (yes,$(strip $(MTK_FD_SUPPORT)))
  # Only support the format: n.m (n:1 or 1+ digits, m:Only 1 digit) or n (n:integer)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.radio.fd.counter=15
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.radio.fd.off.counter=5
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.radio.fd.r8.counter=15
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.radio.fd.off.r8.counter=5
endif

ifeq ($(strip $(MTK_COMBO_SUPPORT)), yes)
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.connsys.chipid=-1
  PRODUCT_PROPERTY_OVERRIDES += service.wcn.driver.ready=no
endif

#################################################
#Widevine DRM part
ifeq ($(strip $(MTK_WVDRM_SUPPORT)),yes)
  #both L1 and L3 library
  PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=true
  PRODUCT_PACKAGES += com.google.widevine.software.drm.xml
  PRODUCT_PACKAGES += com.google.widevine.software.drm
  PRODUCT_PACKAGES += libdrmmtkutil
  PRODUCT_PACKAGES += libdrmwvmplugin
  PRODUCT_PACKAGES += libwvm
  PRODUCT_PACKAGES += libdrmdecrypt
  ifeq ($(strip $(MTK_WVDRM_L1_SUPPORT)),yes)
    #PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_in_house_tee_support=true
    PRODUCT_PACKAGES += libWVStreamControlAPI_L1
    PRODUCT_PACKAGES += libwvdrm_L1
    PRODUCT_PACKAGES += lib_uree_mtk_crypto
  else
    PRODUCT_PACKAGES += libWVStreamControlAPI_L3
    PRODUCT_PACKAGES += libwvdrm_L3
  endif
else
  PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=false
endif

###Modular drm
ifeq ($(strip $(MTK_WVDRM_SUPPORT)),yes)
  #Mock modular drm plugin for cts
  PRODUCT_PACKAGES += libmockdrmcryptoplugin
  #both L1 and L3 library
  PRODUCT_PACKAGES += libwvdrmengine
  ifeq ($(strip $(MTK_WVDRM_L1_SUPPORT)),yes)
    PRODUCT_PACKAGES += lib_uree_mtk_modular_drm
    PRODUCT_PACKAGES += liboemcrypto
  endif
endif

################################################

# Use PlayReady Drm
ifeq ($(strip $(MTK_DRM_PLAYREADY_SUPPORT)),yes)
    PRODUCT_PACKAGES += libplayready
    PRODUCT_PACKAGES += libplayreadydrmplugin
endif

ifeq ($(strip $(MTK_IN_HOUSE_TEE_SUPPORT)),yes)
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_in_house_tee_support=true
ifneq ($(filter-out no, $(MTK_DRM_PLAYREADY_SUPPORT) $(MTK_WVDRM_L1_SUPPORT)), )
    PRODUCT_PACKAGES += securetime
endif
endif

# For secure camera path
ifeq ($(strip $(MTK_CAM_SECURITY_SUPPORT)),yes)
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_sec_cam_path_support=1
endif

#For camera isp
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.camera.isp-version.major=3

#################################################
#OMA DRM part
ifeq ($(strip $(MTK_OMADRM_SUPPORT)), yes)
  PRODUCT_PACKAGES += libdrmmtkutil
  #Media framework reads this property
  PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=true
  PRODUCT_PACKAGES += libdcfdecoderjni
  PRODUCT_PACKAGES += libdrmmtkplugin
  PRODUCT_PACKAGES += drm_chmod
endif
ifeq ($(strip $(MTK_CTA_SET)), yes)
  PRODUCT_PACKAGES += libdrmmtkutil
  #Media framework reads this property
  PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=true
  PRODUCT_PACKAGES += libdcfdecoderjni
  ifneq ($(strip $(BUILD_GMS)), yes)
    PRODUCT_PACKAGES += libdrmctaplugin
    PRODUCT_PACKAGES += DataProtection
  else
    ifneq ($(strip $(MTK_GMO_ROM_OPTIMIZE)), yes)
      PRODUCT_PACKAGES += libdrmctaplugin
      PRODUCT_PACKAGES += DataProtection
    endif
  endif
endif

################################################

ifeq (yes,$(strip $(MTK_FM_SUPPORT)))
  PRODUCT_PROPERTY_OVERRIDES += fmradio.driver.enable=1
else
  PRODUCT_PROPERTY_OVERRIDES += fmradio.driver.enable=0
endif





ifeq ($(strip $(HAVE_SRSAUDIOEFFECT_FEATURE)),yes)
  PRODUCT_PACKAGES += SRSTruMedia
  PRODUCT_PACKAGES += libsrsprocessing
endif

ifeq ($(strip $(MTK_DOLBY_DAP_SUPPORT)), yes)
  PRODUCT_PACKAGES += libdseffect \
                      Ds \
                      DsUI \
                      dolby_ds
endif

ifeq ($(strip $(MTK_AUDIO_DDPLUS_SUPPORT)), yes)
    PRODUCT_PACKAGES += libstagefright_soft_ddpdec
endif

ifeq ($(strip $(MTK_WEATHER_WIDGET_APP)), yes)
  PRODUCT_PACKAGES += MtkWeatherWidget
endif

ifeq ($(strip $(MTK_WFD_SINK_SUPPORT)), yes)
  PRODUCT_PACKAGES += MtkFloatMenu
endif

ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
ifeq ($(strip $(MTK_EMMC_SUPPORT)), yes)
PRODUCT_PACKAGES += rpmb_svc
endif
endif

PRODUCT_BRAND := alps
PRODUCT_MANUFACTURER := alps

# for USB Accessory Library/permission
# Mark for early porting in JB
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml
PRODUCT_PACKAGES += com.android.future.usb.accessory

# System property for MediaTek ANR pre-dump.
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.mtk-stack-trace-file=/data/anr/mtk_traces.txt

ifeq ($(strip $(MTK_WLAN_SUPPORT)),yes)
  PRODUCT_PROPERTY_OVERRIDES += mediatek.wlan.chip=$(MTK_WLAN_CHIP)
  PRODUCT_PROPERTY_OVERRIDES += mediatek.wlan.module.postfix="_"$(shell echo $(strip $(MTK_WLAN_CHIP)) | tr A-Z a-z)
endif

ifeq ($(strip $(MTK_RILD_READ_IMSI)),yes)
  PRODUCT_PROPERTY_OVERRIDES += ril.read.imsi=1
endif

ifeq ($(strip $(MTK_FACTORY_RESET_PROTECTION_SUPPORT)),yes)
  PRODUCT_PROPERTY_OVERRIDES += ro.frp.pst=/dev/block/platform/bootdevice/by-name/frp
endif

PRODUCT_COPY_FILES += device/mediatek/mt8168/init.mt8168.rc:$(MTK_TARGET_VENDOR_RC)/init.mt8168.rc
PRODUCT_COPY_FILES += device/mediatek/mt8168/factory_init.rc:$(MTK_TARGET_VENDOR_RC)/factory_init.rc
PRODUCT_COPY_FILES += device/mediatek/mt8168/meta_init.rc:$(MTK_TARGET_VENDOR_RC)/meta_init.rc

PRODUCT_COPY_FILES += device/mediatek/mt8168/init.modem.rc:$(MTK_TARGET_VENDOR_RC)/init.modem.rc
PRODUCT_COPY_FILES += device/mediatek/mt8168/meta_init.modem.rc:$(MTK_TARGET_VENDOR_RC)/meta_init.modem.rc

ifeq ($(strip $(MTK_SENSORS_1_0)),yes)
PRODUCT_COPY_FILES += device/mediatek/mt8168/init.sensor_1_0.rc:$(MTK_TARGET_VENDOR_RC)/init.sensor_1_0.rc
endif

PRODUCT_COPY_FILES += device/mediatek/mt8168/init.mt8168.usb.rc:$(MTK_TARGET_VENDOR_RC)/init.mt8168.usb.rc
PRODUCT_COPY_FILES += $(LOCAL_PATH)/thermal.conf:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/thermal.conf:mtk
PRODUCT_COPY_FILES += $(LOCAL_PATH)/thermal.off.conf:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/.thermal_policy_00:mtk
PRODUCT_COPY_FILES += $(LOCAL_PATH)/partition_permission.sh:$(TARGET_COPY_OUT_VENDOR)/etc/partition_permission.sh:mtk
PRODUCT_COPY_FILES += $(LOCAL_PATH)/ht120.mtc:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/.ht120.mtc:mtk
PRODUCT_COPY_FILES += $(LOCAL_PATH)/throttle.sh:$(TARGET_COPY_OUT_VENDOR)/etc/throttle.sh:mtk
PRODUCT_COPY_FILES += device/mediatek/common/fstab.enableswap:root/fstab.enableswap

PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml:mtk
PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_performance.xml:mtk
ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
    PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_mediatek_audio_gmo.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml
    PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_google_audio_gmo.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml
    PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_google_video_gmo.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video_le.xml
    PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_c2_gmo.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
else
    PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_mediatek_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml
    PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml
    PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_video_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video_le.xml
    PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_c2.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
endif
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/hardware/omx/mediacodec/android.hardware.media.omx@1.0-service.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/android.hardware.media.omx@1.0-service.rc


ifeq ($(strip $(MTK_WVDRM_L1_SUPPORT)),yes)
    ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
        PRODUCT_COPY_FILES += device/mediatek/mt8168/mtk_omx_core_wvl1_gmo.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
    else
        PRODUCT_COPY_FILES += device/mediatek/mt8168/mtk_omx_core_wvl1.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
    endif
    PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_mediatek_video_wvl1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_video.xml:mtk
else
    ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
        PRODUCT_COPY_FILES += device/mediatek/mt8168/mtk_omx_core_gmo.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
    else
        PRODUCT_COPY_FILES += device/mediatek/mt8168/mtk_omx_core.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
    endif
    PRODUCT_COPY_FILES += device/mediatek/mt8168/media_codecs_mediatek_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_video.xml:mtk
endif

ifeq ($(strip $(MTK_WVDRM_L1_SUPPORT)),yes)
PRODUCT_PROPERTY_OVERRIDES += ro.mtk_venc_inhouse_ready=1
endif

DEVICE_PACKAGE_OVERLAYS += device/mediatek/mt8168/overlay

#Audio config
PRODUCT_COPY_FILES += device/mediatek/mt8168/audio_device.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_device.xml:mtk

#Audio Tuning Tool config
ifneq ($(MTK_AUDIO_TUNING_TOOL_VERSION),)
  ifneq ($(strip $(MTK_AUDIO_TUNING_TOOL_VERSION)),V1)
    MTK_AUDIO_PARAM_DIR_LIST += device/mediatek/mt8168/audio_param
  endif
endif

PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.mediatek.platform=MT8168
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mediatek.platform=MT8168

ifeq ($(strip $(MNTL_SUPPORT)), yes)
PRODUCT_PACKAGES += mnb.img
PRODUCT_PACKAGES += mntl_gpt_resize
PRODUCT_PACKAGES += mntl_link
endif

ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
ifeq ($(strip $(MTK_IN_HOUSE_TEE_SUPPORT)),yes)
PRODUCT_PACKAGES += keystore.mt8168
PRODUCT_PACKAGES += gatekeeper.mt8168
endif
endif

# whether MTK_VPU_SUPPORT = yes or not
PRODUCT_PACKAGES += cam_vpu1.img
PRODUCT_PACKAGES += cam_vpu2.img
PRODUCT_PACKAGES += cam_vpu3.img

PRODUCT_PACKAGES += fpsgo-user.ko fpsgo-eng.ko
PRODUCT_PACKAGES += fpsgo.ko

# add vintf utility
PRODUCT_PACKAGES += \
    vintf

ifeq ($(MTK_VOW_SUPPORT),yes)
PRODUCT_PACKAGES += \
    android.hardware.soundtrigger@2.0-impl
endif

ifneq ($(strip $(FPGA_EARLY_PORTING)),yes)
# Bluetooth HAL
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-impl-mediatek \
    android.hardware.bluetooth@1.0-service-mediatek

# vendor hidl process merging together to save memory
ifeq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
    PRODUCT_PACKAGES += merged_hal_service
endif

# light HAL
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-impl-mediatek \
    android.hardware.light@2.0-service-mediatek

# sensor HAL HIDL
ifeq ($(strip $(MTK_SENSORS_1_0)),yes)
PRODUCT_PACKAGES += \
    android.hardware.sensors@2.0-service-mediatek
endif

# APE service HIDL
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.mtkcodecservice@1.1-impl \
    vendor.mediatek.hardware.mtkcodecservice@1.1-service

# thermal HIDL
PRODUCT_PACKAGES += \
    android.hardware.thermal@1.0-impl
ifneq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
PRODUCT_PACKAGES += \
    android.hardware.thermal@1.0-service
endif
endif

# Usb HAL
PRODUCT_PACKAGES += \
    android.hardware.usb@1.1-service-mediatek

# HDMI
ifeq ($(strip $(MTK_HDMI_SUPPORT)), yes)
    PRODUCT_PACKAGES += libjni_hdmi


    PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_tb_hdmi=ite66121,hidl

    #HDMI Service HIDL
    PRODUCT_PACKAGES += \
      vendor.mediatek.hardware.hdmi@1.0-service \
      vendor.mediatek.hardware.hdmi@1.0-impl

    DEVICE_MANIFEST_FILE += device/mediatek/common/project_manifest/manifest_hdmi.xml

endif

# thermal HIDL
PRODUCT_PACKAGES += \
    android.hardware.thermal@1.0-impl
PRODUCT_PACKAGES += \
    android.hardware.thermal@1.0-service

# mtk av enhance
PRODUCT_PACKAGES += libmtkavenhancements
PRODUCT_PACKAGES += libmtkmkvextractor
PRODUCT_PACKAGES += libmtkmpeg2extractor

# hybrid encode support
  PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_hybrid_encode_support=true

# NeuroPilot NN SDK
ifeq ($(strip $(MTK_NN_SDK_SUPPORT)), yes)
    PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_nn.option=B,E,F,Z
    PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_nn_quant_preferred=1
    PRODUCT_PACKAGES += android.hardware.neuralnetworks@1.2-service-neuron-ann
    PRODUCT_PACKAGES += libnir_neon_driver
    PRODUCT_PACKAGES += armnn_app.config
endif

# HIFI4DSP image
ifeq ($(strip $(BUILD_WITH_XTENSA)), yes)
  PRODUCT_PACKAGES += tinysys-hifi4dsp
else
  ifneq ($(wildcard vendor/mediatek/proprietary/tinysys/adsp/adsp_imgs_3rd_party/mt8168/$(MTK_TARGET_PROJECT)/hifi4dsp_load.bin),)
    PRODUCT_COPY_FILES += vendor/mediatek/proprietary/tinysys/adsp/adsp_imgs_3rd_party/mt8168/$(MTK_TARGET_PROJECT)/hifi4dsp_load.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/hifi4dsp_load.bin:mtk
  endif
endif

# gpu HAL HIDL
PRODUCT_PACKAGES += \
    vendor.mediatek.hardware.gpu@1.0-impl \
    vendor.mediatek.hardware.gpu@1.0-service

#inherit common platform
$(call inherit-product, device/mediatek/vendor/common/device.mk)

#inherit prebuilt
$(call inherit-product-if-exists, vendor/mediatek/mt8168/device-vendor.mk)

PRODUCT_PACKAGES += \
    android.hardware.soundtrigger@2.2-impl

###############################################yuntian start#################################################################################
include yuntian/build/device-yuntian.mk
###############################################yuntian end#################################################################################
