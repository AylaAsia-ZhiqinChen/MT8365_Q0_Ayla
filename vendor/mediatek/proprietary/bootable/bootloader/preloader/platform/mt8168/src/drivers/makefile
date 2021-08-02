###################################################################
# flag
###################################################################

MOD_SRC  =
MOD_OBJ  = $(PRELOADER_OUT)/obj

###################################################################
# source files
# - add your file here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
###################################################################

MOD_SRC  := \
	circbuf.c \
	rtc.c \
	msdc.c \
	msdc_dma.c \
	msdc_irq.c \
	msdc_utils.c \
	i2c.c \
	keypad.c \
	usbd.c \
	usbphy.c \
	usbtty.c \
	timer.c \
	wdt.c \
	platform.c \
	pll.c \
	device_apc.c \
	gpio.c \
	uart.c \
	pmic_wrap_init.c \
	mtk_regulator_core.c \
	pmic.c \
	pmic_initial_setting.c \
	pmic_efuse.c \
	mt6357.c \
	battery.c \
	charging_bat.c \
	clkbuf_ctl.c \
	hl7593.c \
	fan53526.c \
	rt5738.c \
	memory_test.s \
	sync_write.c \
	spm_mtcmos.c \
	hotplug.s \
	gpu_freq.c \
	mtk_ptp.c \
	mtk_auxadc.c \
	dramc_actiming_api.c \
	dramc_basic_api.c \
	dramc_calibration_api.c \
	dramc_cbt_api.c \
	dramc_engine_api.c \
	dramc_gating_api.c \
	dramc_initial_api.c \
	dramc_main.c \
	dramc_mr_api.c \
	dramc_rxcal_api.c \
	dramc_txcal_api.c \
	dramc_power.c \
	dramc_eyescan_api.c \
	emi.c \
	hal_io.c \
	mt_mem.c \
	dramc_ddr_reserve.c \

ifeq ("$(MTK_GPT_SCHEME_SUPPORT)","yes")
  MOD_SRC  += efi.c
endif

ifneq ("$(MTK_EMMC_SUPPORT)","yes")
  MOD_SRC  += cust_part.c
ifeq ("$(MTK_COMBO_NAND_SUPPORT)","yes")
  CONFIG_NANDX_MK_PRELOADER := y
  include nandx/Nandx.mk
else
ifeq ("$(MTK_TLC_NAND_SUPPORT)","yes")
  MOD_SRC  += partition_nand.c
	MOD_SRC  += nand.c
	MOD_SRC  +=	bmt.c
endif
endif
endif

ifeq ("$(MTK_BQ24296_SUPPORT)","yes")
	MOD_SRC  +=	bq24296.c
endif

ifeq ("$(MTK_EMMC_SUPPORT)","yes")
	MOD_SRC  += mmc_common_inter.c
	MOD_SRC  += mmc_core.c
	MOD_SRC  += mmc_test.c
	ifeq ("$(MTK_EMMC_RPMB_SUPPORT)","yes")
		MOD_SRC += mmc_rpmb.c
	endif
endif

ifeq ("$(PL_MODE)","DUMMY_AP")
	MOD_SRC  += gpio_init.c
endif

ifeq ("$(PL_MODE)","TINY")
	MOD_SRC  += gpio_init.c
endif

###################################################################
# target
###################################################################

include $(COMMON_FILE_MK)

###################################################################
# trigger emigen
###################################################################
$(MOD_OBJ)/emi.o: $(EMIGEN_H_OUT)/custom_emi.h
