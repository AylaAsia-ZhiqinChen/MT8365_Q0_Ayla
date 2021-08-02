# This project is created only for generating ADSP default image. Project-based
# configuration here should be minimized, because one ADSP default image
# project belongs to a specific platform.

CFG_MTK_AUDIODSP_SUPPORT := yes

###################################################################
# ADSP internal feature options
###################################################################
CFG_AUDIO_SUPPORT = yes
CFG_FPGA = no
CFG_HIFI4_A = yes
CFG_HIFI4_DUAL_CORE = no
CFG_ASSERT_SUPPORT = yes
CFG_GPIO_SUPPORT = no
CFG_AUXADC_SUPPORT = no
CFG_XGPT_SUPPORT = no
CFG_SYSTIMER_SUPPORT = yes
CFG_CLK_PM_SUPPORT = no
CFG_I2C_SUPPORT = no
CFG_PMIC_SUPPORT = no
CFG_PWM_SUPPORT = no
CFG_EINT_SUPPORT = no
CFG_UART_SUPPORT = yes
CFG_TRAX_SUPPORT = no
CFG_TASK_MONITOR = no
# CFG_MTK_APUART_SUPPORT
# Do not use this with eng load or log may mix together and hard to recognzie
# Do not use this on lower power, it keeps infra always on
CFG_CACHE_SUPPORT = no
CFG_IPC_SUPPORT = yes
CFG_ONCHIP_IPI_SUPPORT = no
CFG_PRINT_TIMESTAMP = yes
CFG_KERNEL_TIMESTAMP_SUPPORT = no
CFG_LOGGER_SUPPORT = yes
CFG_WDT_SUPPORT = yes
CFG_DMA_SUPPORT = no
CFG_RECOVERY_SUPPORT = no
CFG_HEAP_GUARD_SUPPORT = no
CFG_VCORE_DVFS_SUPPORT = no
CFG_RAMDUMP_SUPPORT = no
CFG_FREERTOS_TRACE_SUPPORT = no
CFG_IRQ_MONITOR_SUPPORT = no
CFG_IPI_STAMP_SUPPORT = no
CFG_AP_AWAKE_SUPPORT = no
CFG_WAKELOCK_SUPPORT = no
CFG_CLI_SUPPORT = yes
CFG_HW_SEMA_SUPPORT = yes
CFG_DSP_SEND_IRQ_SUPPORT = no
CFG_PWM_BUCK_SUPPORT = no
CFG_LIB_RENAME_SECTION_TO_DRAM = no
CFG_TICKLESS_SUPPORT = yes
CFG_SPM_SUPPORT = yes
CFG_DSP_CLK_SUPPORT = yes
CFG_MTK_PDCT_SUPPORT = yes

###################################################################
# HIFI4_A address layout
###################################################################
CFG_HIFI4_DTCM_ADDRESS = 0x1e000000
CFG_HIFI4_DTCM_SIZE    = 0x060000
CFG_HIFI4_ITCM_ADDRESS = 0x40000000
CFG_HIFI4_ITCM_SIZE    = 0x018000
CFG_HIFI4_DRAM_ADDRESS = 0x40020000
CFG_HIFI4_DRAM_SIZE    = 0x600000
CFG_HIFI4_DRAM_RESERVE_CACHE_START = 0x40620000
CFG_HIFI4_DRAM_RESERVE_CACHE_SIZE  = 0x600000
CFG_HIFI4_DRAM_SHARED_NONCACHE_START = 0x40c20000
CFG_HIFI4_DRAM_SHARED_NONCACHE_SIZE  = 0xc00000

# dram-cpu-view is uncertain
CFG_HIFI4_DTCM_CPU_VIEW = 0x1e000000
CFG_HIFI4_ITCM_CPU_VIEW = 0x1e100000
CFG_HIFI4_DRAM_DSP_VIEW = 0x40020001
CFG_HIFI4_BOOTUP_ADDR_DSP_VIEW = 0x40000000


