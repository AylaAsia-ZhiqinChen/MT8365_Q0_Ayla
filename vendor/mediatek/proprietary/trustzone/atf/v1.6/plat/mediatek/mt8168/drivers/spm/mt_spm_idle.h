#ifndef __MT_SPM_IDLE__H__
#define __MT_SPM_IDLE__H__

uint64_t spm_idle_check_secure_cg(uint64_t x1, uint64_t x2, uint64_t x3);
uint32_t spm_get_infra1_sta(void);
uint32_t spm_get_infra0_sta(void);
uint32_t spm_get_infra2_sta(void);
uint32_t spm_get_infra3_sta(void);
uint32_t spm_get_infra4_sta(void);
uint32_t spm_get_mmsys_con0(void);
uint32_t spm_get_mmsys_con1(void);
uint32_t spm_get_cam_pow_con(void);
uint32_t spm_get_mfg_pow_con(void);
uint32_t spm_get_venc_pow_con(void);
uint32_t spm_get_vdec_pow_con(void);
uint32_t spm_get_apu_pow_con(void);

#endif /* __MT_SPM_IDLE__H__ */
