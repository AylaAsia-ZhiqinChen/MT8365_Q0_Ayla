LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH    := arm
ARM_CPU := cortex-a53
CPU     := generic

MMC_SLOT         := 1

PLATFORM_SECURITY_VERSION:=21
DEFINES += PLATFORM_SECURITY_VERSION=$(PLATFORM_SECURITY_VERSION)
MTK_AVB20_SUPPORT:=yes
AVB_SHA256_CRYPTO_HW_SUPPORT:=no

# mtk gpt version
GPT_VER := 1
DEFINES += GPT_VER=$(GPT_VER)

# set kernel decompress size limit (200MB)
KERNEL_DECOMPRESS_SIZE := 0x0c800000
DEFINES += KERNEL_DECOMPRESS_SIZE=$(KERNEL_DECOMPRESS_SIZE)
DEFINES += CFG_DTB_EARLY_LOADER_SUPPORT=1

# choose one of following value -> 1: disabled/ 2: permissive /3: enforcing
SELINUX_STATUS := 3

# overwrite SELINUX_STATUS value with PRJ_SELINUX_STATUS, if defined. it's by project variable.
ifdef PRJ_SELINUX_STATUS
	SELINUX_STATUS := $(PRJ_SELINUX_STATUS)
endif

ifeq (yes,$(strip $(MTK_BUILD_ROOT)))
SELINUX_STATUS := 2
DEFINES += MTK_BUILD_ROOT
endif

ifeq (yes,$(strip $(BUILD_MTK_LDVT)))
SELINUX_STATUS := 1
endif

DEFINES += PLATFORM=\"$(PLATFORM)\"

DEFINES += SELINUX_STATUS=$(SELINUX_STATUS)

DEFINES += PERIPH_BLK_BLSP=1
DEFINES += WITH_CPU_EARLY_INIT=0 WITH_CPU_WARM_BOOT=0 \
	   MMC_SLOT=$(MMC_SLOT)

# provide PMIC full (cold) reset functionality
MTK_PMIC_FULL_RESET := yes
ifeq ($(MTK_PMIC_FULL_RESET), yes)
	DEFINES += MTK_PMIC_FULL_RESET
endif

MTK_3LEVEL_PAGETABLE := yes
ifeq ($(MTK_3LEVEL_PAGETABLE), yes)
    DEFINES += MTK_3LEVEL_PAGETABLE
endif

MTK_LM_MODE := yes
ifeq ($(MTK_LM_MODE), yes)
DEFINES += MTK_LM_MODE
endif

PLAT_DBG_INFO_LIB_SUPPORT := yes
ifeq ($(PLAT_DBG_INFO_LIB_SUPPORT), yes)
	DEFINES += PLAT_DBG_INFO_LIB_SUPPORT=1
	PLAT_DBG_INFO_LIB_SUPPORT := 1
	MODULES += lib/plat_dbg_info
endif

ifeq ($(MTK_SECURITY_SW_SUPPORT), yes)
	DEFINES += MTK_SECURITY_SW_SUPPORT
endif

ifeq ($(MTK_SEC_FASTBOOT_UNLOCK_SUPPORT), yes)
	DEFINES += MTK_SEC_FASTBOOT_UNLOCK_SUPPORT
ifeq ($(MTK_SEC_FASTBOOT_UNLOCK_KEY_SUPPORT), yes)
	DEFINES += MTK_SEC_FASTBOOT_UNLOCK_KEY_SUPPORT
endif
endif

ifneq ($(MTK_PRODUCT_LINE_PLATFORM),)
    DEFINES += MTK_PRODUCT_LINE_PLATFORM=\"$(MTK_PRODUCT_LINE_PLATFORM)\"
endif

ifeq ($(MTK_KERNEL_POWER_OFF_CHARGING),yes)
#Fastboot support off-mode-charge 0/1
#1: charging mode, 0:skip charging mode
DEFINES += MTK_OFF_MODE_CHARGE_SUPPORT
endif

MTK_PMIC_POWER_OFF := yes
ifeq ($(MTK_PMIC_POWER_OFF), yes)
	DEFINES += MTK_PMIC_POWER_OFF
endif

ifeq ($(MTK_MLC_NAND_SUPPORT),yes)
DEFINES += MTK_MLC_NAND_SUPPORT
endif

ifeq ($(MTK_TLC_NAND_SUPPORT),yes)
DEFINES += MTK_TLC_NAND_SUPPORT
endif

ifeq ($(MTK_NAND_UBIFS_SUPPORT),yes)
DEFINES += MTK_NAND_UBIFS_SUPPORT
endif

ifeq ($(MTK_NAND_MTK_FTL_SUPPORT ),yes)
DEFINES += MTK_NAND_MTK_FTL_SUPPORT
endif

ifeq ($(MTK_COMBO_NAND_SUPPORT),yes)
DEFINES += MTK_COMBO_NAND_SUPPORT
endif

ifeq ($(MNTL_SUPPORT),yes)
DEFINES += MNTL_SUPPORT
endif

ifeq ($(MTK_DUAL_DISPLAY_SUPPORT), 2)
CFLAGS += -DMTK_DUAL_DISPLAY_SUPPORT=2
OBJS += $(LOCAL_DIR)/external_display.o
endif

DEFINES += $(PLATFORM)

DEFINES += MTK_SMC_K32_SUPPORT

KEDUMP_MINI := yes

ARCH_HAVE_MT_RAMDUMP := no

$(info libshowlogo new path ------- $(LOCAL_DIR)/../../../../../bootable/bootloader/lk/lib/libshowlogo)
INCLUDES += -I$(LOCAL_DIR)/include \
            -I$(LOCAL_DIR)/include/platform \
            -I$(LOCAL_DIR)/../../../../../bootable/bootloader/lk/lib/libshowlogo \
            -I$(LK_TOP_DIR)/app/mt_boot/ \
            -Icustom/$(FULL_PROJECT)/lk/include/target \
            -Icustom/$(FULL_PROJECT)/lk/lcm/inc \
            -Icustom/$(FULL_PROJECT)/lk/inc \
            -Icustom/$(FULL_PROJECT)/common \
            -Icustom/$(FULL_PROJECT)/kernel/dct/ \
            -I$(BUILDDIR)/include/dfo \
            -I$(LOCAL_DIR)/../../dev/lcm/inc

INCLUDES += -I$(DRVGEN_OUT)/inc

OBJS += \
	$(LOCAL_DIR)/bitops.o \
	$(LOCAL_DIR)/mt_gpio.o \
	$(LOCAL_DIR)/mt_disp_drv.o \
	$(LOCAL_DIR)/mt_gpio_init.o \
	$(LOCAL_DIR)/mt_i2c.o \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/uart.o \
	$(LOCAL_DIR)/interrupts.o \
	$(LOCAL_DIR)/timer.o \
	$(LOCAL_DIR)/debug.o \
	$(LOCAL_DIR)/boot_mode.o \
	$(LOCAL_DIR)/load_image.o \
	$(LOCAL_DIR)/atags.o \
	$(LOCAL_DIR)/addr_trans.o \
	$(LOCAL_DIR)/factory.o \
	$(LOCAL_DIR)/mtk_key.o \
	$(LOCAL_DIR)/mt_logo.o\
	$(LOCAL_DIR)/boot_mode_menu.o\
	$(LOCAL_DIR)/env.o\
	$(LOCAL_DIR)/mt_pmic_wrap_init.o\
	$(LOCAL_DIR)/upmu_common.o\
	$(LOCAL_DIR)/mt_pmic.o\
	$(LOCAL_DIR)/mt_pmic_dlpt.o\
	$(LOCAL_DIR)/mtk_wdt.o\
	$(LOCAL_DIR)/mt_rtc.o\
        $(LOCAL_DIR)/mt_usbphy.o\
        $(LOCAL_DIR)/mt_usb.o\
        $(LOCAL_DIR)/mt_ssusb_qmu.o\
        $(LOCAL_DIR)/mt_mu3d_hal_qmu_drv.o\
	$(LOCAL_DIR)/mt_leds.o\
	$(LOCAL_DIR)/sec_policy.o\
	$(LOCAL_DIR)/ddp_manager.o\
	$(LOCAL_DIR)/ddp_path.o\
	$(LOCAL_DIR)/ddp_ovl.o\
	$(LOCAL_DIR)/ddp_rdma.o\
	$(LOCAL_DIR)/ddp_misc.o\
	$(LOCAL_DIR)/ddp_info.o\
	$(LOCAL_DIR)/ddp_dither.o\
	$(LOCAL_DIR)/ddp_dump.o\
	$(LOCAL_DIR)/ddp_dsi.o\
	$(LOCAL_DIR)/ddp_dpi.o\
	$(LOCAL_DIR)/primary_display.o\
	$(LOCAL_DIR)/disp_lcm.o\
	$(LOCAL_DIR)/ddp_pwm.o\
	$(LOCAL_DIR)/pwm.o \
	$(LOCAL_DIR)/fpc_sw_repair2sw.o \
	$(LOCAL_DIR)/mt_latch.o \
	$(LOCAL_DIR)/aee_platform_debug.o \
	$(LOCAL_DIR)/sec_otp.o

ifeq ($(MTK_SECURITY_SW_SUPPORT), yes)
OBJS +=	$(LOCAL_DIR)/img_auth_stor.o
endif

ifeq ($(DEVICE_TREE_SUPPORT), yes)
OBJS +=	$(LOCAL_DIR)/device_tree.o
endif

# SETTING of USBPHY type
#OBJS += $(LOCAL_DIR)/mt_usbphy_d60802.o
#OBJS += $(LOCAL_DIR)/mt_usbphy_e60802.o

ifeq ($(MTK_EMMC_SUPPORT),yes)
  OBJS +=	$(LOCAL_DIR)/partition.o
	OBJS +=	$(LOCAL_DIR)/msdc_utils.o
	OBJS +=	$(LOCAL_DIR)/msdc.o
	OBJS +=	$(LOCAL_DIR)/mmc_core.o
	OBJS += $(LOCAL_DIR)/mmc_test.o
	OBJS += $(LOCAL_DIR)/mmc_common_inter.o
	OBJS += $(LOCAL_DIR)/msdc_dma.o
	OBJS += $(LOCAL_DIR)/msdc_irq.o
	OBJS +=	$(LOCAL_DIR)/efi.o
	OBJS +=	$(LOCAL_DIR)/mt_get_dl_info.o
endif

ifneq ($(MTK_EMMC_SUPPORT),yes)
ifeq ($(MTK_COMBO_NAND_SUPPORT),yes)
INCLUDES += -I$(LOCAL_DIR)/nandx/include/internal \
	    -I$(LOCAL_DIR)/nandx/include/lk
	CONFIG_NANDX_MK_LK := y
	include $(LOCAL_DIR)/nandx/Nandx.mk
else
ifeq ($(MTK_TLC_NAND_SUPPORT),yes)
	OBJS +=$(LOCAL_DIR)/mtk_nand.o
	OBJS +=$(LOCAL_DIR)/bmt.o
	OBJS +=$(LOCAL_DIR)/partition_nand.o
endif
endif
#	OBJS +=$(LOCAL_DIR)/mt_partition.o
	OBJS +=$(LOCAL_DIR)/partition_setting.o
endif

ifeq ($(MTK_USB2JTAG_SUPPORT),yes)
OBJS += $(LOCAL_DIR)/mt_usb2jtag.o
DEFINES += MTK_USB2JTAG_SUPPORT
endif

ifeq ($(MTK_MT8193_SUPPORT),yes)
#OBJS +=$(LOCAL_DIR)/mt8193_init.o
#OBJS +=$(LOCAL_DIR)/mt8193_ckgen.o
#OBJS +=$(LOCAL_DIR)/mt8193_i2c.o
endif

ifeq ($(FASTBOOT_WHOLE_FLASH_SUPPORT), yes)
DEFINES += FASTBOOT_WHOLE_FLASH_SUPPORT
endif

ifeq ($(MTK_KERNEL_POWER_OFF_CHARGING),yes)
OBJS +=$(LOCAL_DIR)/mt_kernel_power_off_charging.o
DEFINES += MTK_KERNEL_POWER_OFF_CHARGING
endif

ifeq ($(MTK_BQ24261_SUPPORT),yes)
OBJS +=$(LOCAL_DIR)/bq24261.o
endif

ifeq ($(MTK_BQ24196_SUPPORT),yes)
OBJS +=$(LOCAL_DIR)/bq24196.o
DEFINES += MTK_BQ24196_SUPPORT
endif

ifeq ($(MTK_BQ24296_SUPPORT),yes)
OBJS +=$(LOCAL_DIR)/bq24296.o
DEFINES += MTK_BQ24296_SUPPORT
endif

ifeq ($(MTK_RT9466_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/rt9466.o
	DEFINES += MTK_RT9466_SUPPORT
	DEFINES += SWCHR_POWER_PATH
endif

ifeq ($(MTK_VPU_SUPPORT), yes)
	ifneq ($(MTK_VPU_LOAD_IMAGE), yes)
		OBJS += $(LOCAL_DIR)/mt_vpu.o
	else
		DEFINES += MTK_VPU_LOAD_IMAGE
	endif
	DEFINES += MTK_VPU_SUPPORT
endif

# MTK IN-HOUSE TEE Secure chunk memory with CMA support
ifeq ($(filter-out yes, \
       $(if $(MTK_IN_HOUSE_TEE_SUPPORT),$(MTK_IN_HOUSE_TEE_SUPPORT),no) \
       $(if $(MTK_SEC_VIDEO_PATH_SUPPORT),$(MTK_SEC_VIDEO_PATH_SUPPORT),no) \
       $(if $(MTK_WVDRM_L1_SUPPORT),$(MTK_WVDRM_L1_SUPPORT),no)),)
DEFINES += MTK_SHARED_SECURE_POOL_SUPPORT
DEFINES += MTK_SHARED_SECURE_POOL_SIZE=$(if $(PRJ_SHARED_SECURE_POOL_SIZE),$(PRJ_SHARED_SECURE_POOL_SIZE),0x7000000)
endif

ifeq ($(MTK_NCP1854_SUPPORT),yes)
OBJS +=$(LOCAL_DIR)/ncp1854.o
endif
ifeq ($(DUMMY_AP),yes)
OBJS +=$(LOCAL_DIR)/dummy_ap.o
OBJS +=$(LOCAL_DIR)/spm_md_mtcmos.o
endif

ifeq ($(MTK_SECURITY_SW_SUPPORT), yes)
ifeq ($(CUSTOM_SEC_AUTH_SUPPORT), yes)
LIBSEC := -L$(LOCAL_DIR)/lib --start-group -lsec
else
LIBSEC := -L$(LOCAL_DIR)/lib --start-group -lsec -lcrypto
endif
ifeq ($(MNTL_SUPPORT), yes)
ifeq ($(MTK_ARM64), yes)
LIBSEC_PLAT := -lsecplat -ldevinfo -lmntl_core_lk_arm64 --end-group
else
LIBSEC_PLAT := -lsecplat -ldevinfo -lmntl_core_lk_arm32 --end-group
endif
else
LIBSEC_PLAT := -lsecplat -ldevinfo --end-group
endif
else
LIBSEC := -L$(LOCAL_DIR)/lib
ifeq ($(MNTL_SUPPORT), yes)
ifeq ($(MTK_ARM64), yes)
LIBSEC_PLAT := -lsecplat -ldevinfo -lscp -lmntl_core_lk_arm64
else
LIBSEC_PLAT := -lsecplat -ldevinfo -lscp -lmntl_core_lk_arm32
endif
else
LIBSEC_PLAT := -lsecplat -ldevinfo
endif
endif

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

MTK_AEE_PLATFORM_DEBUG_SUPPORT := yes
DEFINES += MTK_AEE_PLATFORM_DEBUG_SUPPORT

ifeq ($(MTK_DM_VERITY_OFF),yes)
DEFINES += MTK_DM_VERITY_OFF
endif

ifeq ($(MTK_USERIMAGES_USE_F2FS), yes)
DEFINES += MTK_USERIMAGES_USE_F2FS
endif

MTK_GIC_VERSION := v3
MBLOCK_LIB_SUPPORT := yes
ifeq ($(MBLOCK_LIB_SUPPORT), yes)
	DEFINES += MBLOCK_LIB_SUPPORT=2
	MBLOCK_LIB_SUPPORT := 2
	MODULES += lib/mblock
	INCLUDES += -I$(LOCAL_DIR)/../../lib/mblock
endif

OBJS += $(LOCAL_DIR)/dm_verity_status.o

MTK_RC_TO_VENDOR ?= yes
MTK_SMC_ID_MGMT := yes

ifeq ($(MTK_FBTOOL_SUPPORT), yes)
DEFINES += MTK_FBTOOL_SUPPORT
endif

SYSTEM_AS_ROOT=no