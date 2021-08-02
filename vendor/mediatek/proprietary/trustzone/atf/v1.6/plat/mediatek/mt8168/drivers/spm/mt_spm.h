#ifndef __MT_SPM_H__
#define __MT_SPM_H__

#include <stdio.h>
#include <libc/stdint.h>

#define true 1
#define false 0

#define SPM_FW_NO_RESUME 1

#define EVENT_VEC(event, resume, imme, pc)	\
	(((pc) << 16) |				\
	 (!!(imme) << 7) |			\
	 (!!(resume) << 6) |			\
	 ((event) & 0x3f))

extern uint32_t __spmfw_idx;
extern uint32_t is_ext_buck;

enum DRAM_DRAM_TYPE_T {
	TYPE_PCDDR3 = 0,
	TYPE_PCDDR4,
	TYPE_LPDDR3,
	TYPE_LPDDR4,
	TYPE_LPDDR4X,
	TYPE_LPDDR4P,
	TYPE_MAX,
};

enum _SPM_FIRMWARE_STATUS_ {
	SPM_FIRMWARE_STATUS_NOT_LOADED = 0,
	SPM_FIRMWARE_STATUS_LOADED,
	SPM_FIRMWARE_STATUS_LOADED_KICKED,
};

enum {
	SPM_ARGS_SPMFW_IDX = 0,
	SPM_ARGS_SPMFW_INIT,
	SPM_ARGS_SUSPEND,
	SPM_ARGS_SUSPEND_FINISH,
	SPM_ARGS_SODI,
	SPM_ARGS_SODI_FINISH,
	SPM_ARGS_DPIDLE,
	SPM_ARGS_DPIDLE_FINISH,
	SPM_ARGS_PCM_WDT,
	SPM_ARGS_NUM,
};

enum WAKE_REASON {
	WR_NONE = 0,
	WR_UART_BUSY = 1,
	WR_PCM_ASSERT = 2,
	WR_PCM_TIMER = 3,
	WR_WAKE_SRC = 4,
	WR_UNKNOWN = 5,
};

void spm_lock_init(void);
void spm_lock_get(void);
void spm_lock_release(void);
void spm_boot_init(void);

void spm_suspend_args(uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4);
uint64_t spm_load_firmware_status(void);
void spm_irq0_handler(uint64_t x1);
void spm_ap_mdsrc_req(uint64_t x1);
void spm_pwr_ctrl_args(uint64_t x1, uint64_t x2, uint64_t x3);
uint64_t spm_get_pwr_ctrl_args(uint64_t x1, uint64_t x2, uint64_t x3);
void spm_dcs_s1_setting(uint64_t x1, uint64_t x2);
void spm_dummy_read(uint64_t x1, uint64_t x2);
void spm_args(uint64_t x1, uint64_t x2, uint64_t x3);

void spm_pcm_wdt(uint64_t enable, uint64_t time);
void spm_suspend(void);
void spm_suspend_finish(void);

void spm_legacy_sleep_wfi(uint64_t x1, uint64_t x2, uint64_t x3);
void spm_config_spmfw_base(uint64_t addr, uint64_t size);

void spm_set_bootaddr(unsigned long bootaddr);
const char *spm_get_firmware_version(void);
#endif /* __MT_SPM_H__ */
