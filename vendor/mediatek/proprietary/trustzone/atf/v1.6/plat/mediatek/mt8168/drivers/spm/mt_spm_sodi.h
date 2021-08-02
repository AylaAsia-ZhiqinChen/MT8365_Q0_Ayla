#ifndef __MT_SPM_SODI_H__
#define __MT_SPM_SODI_H__

void spm_sodi_args(uint64_t x1, uint64_t x2, uint64_t x3);
void spm_sodi(void);
void spm_sodi_finish(void);
int spm_can_sodi_enter(void);
int spm_is_sodi_resume(void);

enum sodi_smc_cmd {
	SODI_SMC_CMD_0,
	SODI_SMC_CMD_1,
	SODI_SMC_CMD_2,
	NUM_SODI_SMC_CMD,
};

#endif /* __MT_SPM_SODI_H__ */
