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
	emi.c \
	dramc_pi_basic_api.c \
	dramc_pi_calibration_api.c \
	dramc_pi_main.c \
	Hal_io.c \
	timer.c \
	wdt.c \
	platform.c \
	pll.c \
	device_apc.c \
	gpio.c \
	uart.c \
 	pmic_wrap_init.c \
	pmic_6392.c \
	memory.c \
	memory_test.s \
	mt_xo.c \
	sync_write.c \
	spm_mtcmos.c \
	hotplug.s \
	gpu_freq.c \
	mtk_ptp.c \
	mtk_auxadc.c \

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
