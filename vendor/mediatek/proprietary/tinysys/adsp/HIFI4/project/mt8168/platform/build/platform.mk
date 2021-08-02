###################################################################
# Common options for all projects
###################################################################
CFG_XTENSA_ERRATUM_572 = yes
CFG_MPU_SUPPORT = yes
CFG_MPU_DEBUG = no
CFG_MPU_LOW_POWER_CHECK = yes
CFG_DYNAMIC_DEBUG = yes
CFG_MTK_HEAP_SUPPORT = yes
CFG_MTK_HEAP_MALLOC_TRACE = no
CFG_MTK_HEAP_MALLOC_DEBUG = no
CFG_MALLOC_TRACE = no
CFG_MALLOC_DEBUG = no
CFG_CLIB_MALLOC_DEBUG = no


###################################################################
# Optional ProjectConfig.mk used by project
###################################################################
-include $(PROJECT_DIR)/ProjectConfig.mk
-include $(PROJECT_DIR)/AudioConfig.mk
$(call stash_config_options,$(PROJECT_DIR)/AudioConfig.mk)

###################################################################
# Mandatory platform-specific resources
###################################################################
INCLUDES += \
  $(DRIVERS_PLATFORM_DIR)/main/inc \
  $(RTOS_SRC_PLUS_DIR)/FreeRTOS-Plus-CLI

C_FILES += \
  $(DRIVERS_PLATFORM_DIR)/main/src/main.c \
  $(DRIVERS_PLATFORM_DIR)/main/src/platform.c

ifeq ($(CFG_AUDIO_LOG_DEBUG),yes)
CFLAGS  += -DAUDIO_LOG_DEBUG
endif

ifeq ($(CFG_MTK_AUDIO_FRAMEWORK_SUPPORT),yes)
CFLAGS += -DMTK_AUDIO_FRAMEWORK_SUPPORT
endif

###################################################################
# Heap size config
###################################################################
# default heap size
ifeq ($(configDTCM_HEAP_SIZE),)
CFLAGS += -DconfigTOTAL_HEAP_SIZE='( ( size_t ) ( 192 * 1024 ) )'
else
CFLAGS += -DconfigTOTAL_HEAP_SIZE='$(configDTCM_HEAP_SIZE)'
endif

###################################################################
# Resources determined by configuration options
###################################################################
ifeq ($(CFG_CACHE_SUPPORT), yes)
INCLUDES += $(DRIVERS_PLATFORM_DIR)/cache/inc
C_FILES  += $(DRIVERS_PLATFORM_DIR)/cache/src/hal_cache.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/cache/src/hal_cache_internal.c
endif

ifeq ($(CFG_SYSTIMER_SUPPORT),yes)
C_FILES  += $(DRIVERS_PLATFORM_DIR)/systimer/systimer.c
endif

ifeq ($(CFG_UART_SUPPORT),yes)
INCLUDES += $(DRIVERS_PLATFORM_DIR)/uart/inc
C_FILES  += $(DRIVERS_PLATFORM_DIR)/uart/src/uart.c
endif

ifeq ($(CFG_TRAX_SUPPORT),yes)
INCLUDES += $(DRIVERS_PLATFORM_DIR)/trax/inc
C_FILES  += $(DRIVERS_PLATFORM_DIR)/trax/src/adsp_trax.c
endif

ifeq ($(CFG_IPC_SUPPORT),yes)
INCLUDES += $(DRIVERS_COMMON_DIR)/ipi/common
NORMAL_SECTION_C_FILES  += $(DRIVERS_COMMON_DIR)/ipi/common/adsp_ipi_queue.c
INCLUDES += $(DRIVERS_PLATFORM_DIR)/ipi/inc
NORMAL_SECTION_C_FILES  += $(DRIVERS_PLATFORM_DIR)/ipi/src/adsp_ipi.c
ifeq ($(CFG_AUDIO_IPC_UT),yes)
CFLAGS   += -DFAKE_HOST_IPC_UT
endif
endif

ifeq ($(CFG_LOGGER_SUPPORT),yes)
INCLUDES += $(DRIVERS_PLATFORM_DIR)/logger/inc
C_FILES  += $(DRIVERS_PLATFORM_DIR)/logger/src/logger.c
endif

ifeq ($(CFG_WDT_SUPPORT),yes)
INCLUDES += $(DRIVERS_COMMON_DIR)/wdt/inc
C_FILES  += $(DRIVERS_COMMON_DIR)/wdt/src/wdt.c
endif

ifeq ($(CFG_DMA_SUPPORT),yes)
INCLUDES += $(DRIVERS_PLATFORM_DIR)/dma/inc
C_FILES  += $(SOURCE_DIR)/drivers/common/dma/src/dma.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/dma/src/dma_api.c
endif

ifeq ($(CFG_AP_AWAKE_SUPPORT),yes)
ifneq ($(CFG_WAKELOCK_SUPPORT),yes)
$(error CFG_AP_AWAKE_SUPPORT needs to set CFG_WAKELOCK_SUPPORT to yes)
endif
INCLUDES += $(DRIVERS_PLATFORM_DIR)/ap_awake/inc
C_FILES  += $(DRIVERS_PLATFORM_DIR)/ap_awake/src/ap_awake.c
endif

ifeq ($(CFG_WAKELOCK_SUPPORT),yes)
INCLUDES += $(SOURCE_DIR)/kernel/FreeRTOS/Source/include
INCLUDES += $(SOURCE_DIR)/kernel/service/HIFI4/include
C_FILES  += $(SOURCE_DIR)/kernel/service/HIFI4/src/wakelock.c
endif

ifeq ($(CFG_VCORE_DVFS_SUPPORT),yes)
#INCLUDES += $(SOURCE_DIR)/kernel/service/common/include/
INCLUDES += $(DRIVERS_PLATFORM_DIR)/dvfs/inc
C_FILES  += $(DRIVERS_PLATFORM_DIR)/dvfs/src/dvfs_common.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/dvfs/src/dvfs.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/dvfs/src/sleep.c
endif

ifeq ($(CFG_MTK_AUDIO_TUNNELING_SUPPORT),yes)
CFG_AUDIO_SUPPORT = yes
endif

ifeq ($(CFG_DMIC_SUPPORT),yes)
CFLAGS  += -DAUDIO_DMIC_SUPPORT
endif

ifeq ($(CFG_DMIC_SUPPORT),yes)
CFLAGS  += -DAUDIO_DMIC_SUPPORT
endif

ifeq ($(CFG_AUDIO_SUPPORT),yes)
INCLUDES += $(DRIVERS_COMMON_DIR)/audio/utility/
INCLUDES += $(DRIVERS_COMMON_DIR)/audio/framework
INCLUDES += $(DRIVERS_COMMON_DIR)/audio/tasks
INCLUDES += $(DRIVERS_PLATFORM_DIR)/include/audio
INCLUDES += $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant
C_FILES  += $(DRIVERS_COMMON_DIR)/audio/framework/audio.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/audio/aud_drv/afe_drv_pcm.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/audio/aud_drv/afe_drv_reg_rw.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/audio/aud_drv/afe_drv_ops_implement.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/audio/aud_drv/afe_drv_ops_dispatcher.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/audio/aud_drv/afe_drv_api_if.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/audio/aud_drv/afe_drv_misc.c
NORMAL_SECTION_C_FILES  += $(DRIVERS_COMMON_DIR)/audio/framework/audio_messenger_ipi.c
C_FILES  += $(DRIVERS_COMMON_DIR)/audio/tasks/audio_task.c
NORMAL_SECTION_C_FILES  += $(DRIVERS_COMMON_DIR)/audio/tasks/audio_task_top_ctrl.c
C_FILES  += $(DRIVERS_COMMON_DIR)/audio/tasks/audio_task_io_buf_ctrl.c

### va task utility
ifeq ($(CFG_AUDIO_DSP_STATE),yes)
CFLAGS += -DDSP_STATE_SUPPORT
INCLUDES  += $(DRIVERS_PLATFORM_DIR)/dsp_state
C_FILES  += $(DRIVERS_PLATFORM_DIR)/dsp_state/dsp_state.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/dsp_state/dsp_state_implement.c
endif #CFG_AUDIO_DSP_STATE

INCLUDES  += $(DRIVERS_PLATFORM_DIR)/va_state
C_FILES  += $(DRIVERS_PLATFORM_DIR)/va_state/va_state.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/va_state/va_state_implement.c

### utility

ifeq ($(CFG_AUDIO_DEMO),yes)
C_FILES  += $(DRIVERS_COMMON_DIR)/audio/afe_drv_demo/audio_process.c
C_FILES  += $(DRIVERS_COMMON_DIR)/audio/afe_drv_demo/afe_drv_api_demo_synced_api.c
ifeq ($(CFG_AUDIO_IPC_UT),yes)
C_FILES  += $(DRIVERS_COMMON_DIR)/audio/afe_drv_demo/audio_ipc_ut_demo.c
endif
endif #CFG_AUDIO_DEMO

ifeq ($(CFG_HW_RES_MGR),yes)
INCLUDES  += $(DRIVERS_PLATFORM_DIR)/hw_res_mgr
C_FILES  += $(DRIVERS_PLATFORM_DIR)/hw_res_mgr/hw_res_mgr.c
C_FILES  += $(DRIVERS_PLATFORM_DIR)/hw_res_mgr/hw_res_mgr_implement.c
endif #CFG_HW_RES_MGR

ifeq ($(CFG_MTK_AUDIO_FRAMEWORK_SUPPORT),yes)
INCLUDES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/audio_task_va.c

INCLUDES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_process
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_process/vad_process.c
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_process/preproc_process.c
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_process/chdemux_process.c
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_process/ww_process.c
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_process/va_process.c

ifeq ($(CFG_AUDIO_VA_VAD_DUMMY),yes)
CFLAGS += -DVA_DUMMY_VAD_SUPPORT
INCLUDES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_dummy \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/vad_dummy
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_dummy/vad_dummy.c \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/vad_dummy/mtk_vad_dummy_adaptor.c
endif #CFG_AUDIO_VA_VAD_DUMMY

ifeq ($(CFG_VA_MTK_VAD_SUPPORT),yes)
CFLAGS += -DVA_MTK_VAD_SUPPORT
INCLUDES += \
  $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/vad \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/vad
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/vad/mtk_vad_adaptor.c
LIBFLAGS += -L$(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/vad/ -lswVAD
endif #CFG_VA_MTK_VAD_SUPPORT

ifeq ($(CFG_AUDIO_VA_AEC_DUMMY),yes)
CFLAGS += -DVA_DUMMY_AEC_SUPPORT
INCLUDES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_dummy \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/aec_dummy
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_dummy/aec_dummy.c \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/aec_dummy/mtk_aec_dummy_adaptor.c
endif #CFG_AUDIO_VA_AEC_DUMMY

ifeq ($(CFG_AUDIO_VA_PREPROC_MTKFFP),yes)
CFLAGS += -DVA_MTK_PREPROC_SUPPORT
INCLUDES += \
  $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/effp \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/effp
NORMAL_SECTION_C_FILES += \
  $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/effp/effp_mic_config.c \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/effp/mtk_effp_adaptor.c
NORMAL_SECTION_LIBS += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/effp/libeffp.normal.a
LIBFLAGS += -lm
endif #CFG_AUDIO_VA_PREPROC_MTKFFP

ifeq ($(CFG_AUDIO_VA_WW_DUMMY),yes)
CFLAGS += -DVA_DUMMY_WW_SUPPORT
INCLUDES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_dummy \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/ww_dummy
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_dummy/ww_dummy.c \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/ww_dummy/mtk_ww_dummy_adaptor.c
endif #CFG_AUDIO_VA_WW_DUMMY

ifeq ($(CFG_VA_MTK_WW_LITE_SUPPORT),yes)
CFLAGS += -DVA_MTK_WW_LITE_SUPPORT
INCLUDES += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/wwe_lite_xatx/
INCLUDES += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/wwe_lite/
INCLUDES += $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/wwe_lite/
ifeq ($(CFG_VA_WW_SYS_HW), DSP_SYS_HW_DRAM)
NORMAL_SECTION_C_FILES += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/wwe_lite_xatx/wwe_lite_model.c \
	$(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/wwe_lite/mtk_wwe_lite_adaptor.c
NORMAL_SECTION_LIBS += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/wwe_lite/libvow_p2_test.a
else
LIBFLAGS += -L$(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/wwe_lite/ -lvow_p2_test
C_FILES  += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/mtk-algo/wwe_lite_xatx/wwe_lite_model.c \
	$(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_mtk/wwe_lite/mtk_wwe_lite_adaptor.c
endif
LIBFLAGS += -lm
endif #CFG_VA_MTK_WW_LITE_SUPPORT

ifeq ($(CFG_AMZ_WW_LITE_SUPPORT),yes)
CFLAGS  += -DDISABLE_DIAGNOSTIC_PRAGMAS
INCLUDES += \
  $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/thirdparty-algo/amazon/wwe \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_thirdparty/amazon/wwe

ifeq ($(CFG_VA_WW_SYS_HW), DSP_SYS_HW_DRAM)
NORMAL_SECTION_C_FILES += \
  $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/thirdparty-algo/amazon/wwe/U_100k.en-US.alexa.c \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_thirdparty/amazon/wwe/amz_wwe_adaptor.c
NORMAL_SECTION_LIBS += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/thirdparty-algo/amazon/wwe/libpryon_lite-U.a
else
C_FILES += \
  $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/thirdparty-algo/amazon/wwe/U_100k.en-US.alexa.c \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_thirdparty/amazon/wwe/amz_wwe_adaptor.c
LIBFLAGS += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/thirdparty-algo/amazon/wwe/libpryon_lite-U.a
endif
endif #CFG_AMZ_WW_LITE_SUPPORT

ifeq ($(CFG_GOOGLE_WW_LITE_SUPPORT),yes)
INCLUDES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_thirdparty/google/wwe_lite/
INCLUDES += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/thirdparty-algo/google/wwe_lite/
C_FILES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/voiceassistant/va_thirdparty/google/wwe_lite/google_wwe_adaptor.c \
  $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/thirdparty-algo/google/wwe_lite/google_ww_en_all.c
LIBFLAGS += $(SOURCE_DIR)/../license/prebuilt/HIFI4/mt8168/thirdparty-algo/google/wwe_lite/libmt8168_google_hotword_dsp_api.a
endif

ifeq ($(CFG_AUDIO_DEBUG_DUMP),yes)
CFLAGS += -DDUMP_TO_HOST_SUPPORT
INCLUDES += \
  $(DRIVERS_COMMON_DIR)/audio/tasks/audio_task_dump_helper/
C_FILES  += $(DRIVERS_COMMON_DIR)/audio/tasks/audio_task_dump_helper/audio_dump_helper.c
endif #CFG_AUDIO_DEBUG_DUMP

ifeq ($(CFG_AUDIO_DYNAMIC_LOAD_MODEL),yes)
CFLAGS += -DDYNAMIC_LOAD_MODEL
endif #CFG_AUDIO_DYNAMIC_LOAD_MODEL

endif #CFG_MTK_AUDIO_FRAMEWORK_SUPPORT

endif #CFG_AUDIO_SUPPORT

ifeq ($(CFG_CADENCE_VFPU_V320),yes)
CADENCE_VFPU_V320_DIR := $(SOURCE_DIR)/middleware/cadence/vfpu/v3_2_0
INCLUDES += \
  $(CADENCE_VFPU_V320_DIR)/include \
  $(CADENCE_VFPU_V320_DIR)/include_private
C_FILES += \
  $(CADENCE_VFPU_V320_DIR)/fft_real_32x32_hifi4.c \
  $(CADENCE_VFPU_V320_DIR)/fft_real_x32_twd_512.c \
  $(CADENCE_VFPU_V320_DIR)/scl_alogn_32x32_hifi4.c \
  $(CADENCE_VFPU_V320_DIR)/vec_add32x32_hifi4.c \
  $(CADENCE_VFPU_V320_DIR)/vec_bexp16_hifi4.c \
  $(CADENCE_VFPU_V320_DIR)/vec_bexp32_hifi4.c \
  $(CADENCE_VFPU_V320_DIR)/fft_cplx_32x32_hifi4.c \
  $(CADENCE_VFPU_V320_DIR)/fft_cplx_x32_twd_256.c \
  $(CADENCE_VFPU_V320_DIR)/fft_cplx_inner_32x32_hifi4.c \
  $(CADENCE_VFPU_V320_DIR)/log_table.c \
  $(CADENCE_VFPU_V320_DIR)/scl_logn_32x32_hifi4.c
endif #CFG_CADENCE_VFPU_V320

ifeq ($(CFG_CLI_SUPPORT),yes)
C_FILES += $(RTOS_SRC_PLUS_DIR)/FreeRTOS-Plus-CLI/FreeRTOS_CLI.c
C_FILES += $(DRIVERS_COMMON_DIR)/cli/UARTCommandConsole.c
C_FILES += $(DRIVERS_COMMON_DIR)/cli/os_cli.c
endif

ifeq ($(CFG_MPU_SUPPORT),yes)
C_FILES  += $(DRIVERS_COMMON_DIR)/mpu/mpu.c
endif

ifeq ($(CFG_MTK_HEAP_SUPPORT),yes)
INCLUDES += $(DRIVERS_COMMON_DIR)/mem_mgt/inc
C_FILES  += $(DRIVERS_COMMON_DIR)/mem_mgt/src/mtk_heap.c
endif

ifeq ($(CFG_MALLOC_DEBUG),yes)
C_FILES += $(DRIVERS_COMMON_DIR)/mem_mgt/src/heap_4_debug.c
else
C_FILES += $(RTOS_SRC_DIR)/portable/MemMang/heap_4.c
endif

C_FILES += $(DRIVERS_COMMON_DIR)/mem_mgt/src/malloc_trace.c
ifeq ($(CFG_CLIB_MALLOC_DEBUG),yes)
LDFLAGS += -Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,calloc -Wl,-wrap,realloc
C_FILES += $(DRIVERS_COMMON_DIR)/mem_mgt/src/malloc_wrap.c
endif

ifeq ($(CFG_HW_SEMA_SUPPORT),yes)
C_FILES += $(DRIVERS_PLATFORM_DIR)/hw_semaphore/hw_semaphore.c
endif

ifeq ($(CFG_TICKLESS_SUPPORT),yes)
CFLAGS += -DconfigUSE_TICKLESS_IDLE=1
C_FILES += $(DRIVERS_PLATFORM_DIR)/tickless/tickless.c
else
CFLAGS += -DconfigUSE_TICKLESS_IDLE=0
endif

C_FILES += $(DRIVERS_PLATFORM_DIR)/interrupt/interrupt.c
C_FILES += $(DRIVERS_COMMON_DIR)/exception/adsp_excep.c
C_FILES += $(DRIVERS_COMMON_DIR)/printf/mtk_printf.c

ifeq ($(CFG_LIB_RENAME_SECTION_TO_DRAM),yes)
#INCLUDES += $(PLATFORM_DIR)/middleware/lib/inc
#NORMAL_SECTION_LIBS += $(PLATFORM_DIR)/middleware/lib/example1/libexample1.a
#NORMAL_SECTION_LIBS += $(PLATFORM_DIR)/middleware/lib/example2/libexample2.a
endif

ifeq ($(CFG_MTK_PDCT_SUPPORT),yes)
LIBFLAGS += -L$(PLATFORM_DIR)/middleware/secure_lib -lpdct
endif

ifeq ($(CFG_SPM_SUPPORT),yes)
INCLUDES += \
  $(DRIVERS_PLATFORM_DIR)/spm/
C_FILES += \
  $(DRIVERS_PLATFORM_DIR)/spm/spm.c
endif

ifeq ($(CFG_DSP_CLK_SUPPORT),yes)
INCLUDES += \
   $(DRIVERS_PLATFORM_DIR)/dsp_clk/
C_FILES += \
   $(DRIVERS_PLATFORM_DIR)/dsp_clk/dsp_clk.c
endif

###################################################################
# Optional CompilerOption.mk used by project
###################################################################
-include $(PROJECT_DIR)/CompilerOption.mk
