#include <debug.h>
#include <delay_timer.h>
#include <mmio.h>
#include <mt_spm.h>
#include <mt_spm_idle.h>
#include <mt_spm_internal.h>
#include <mt_spm_reg.h>
#include <mt_spm_resource_req.h>
#include <platform.h>
#include <platform_def.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**************************************
 * Config and Parameter
 **************************************/
#define CNTCV_L			(SYSTIMER_BASE + 0x8)
#define CNTCV_H			(SYSTIMER_BASE + 0xc)
#define SYSTEM_BOOTING_120S	(0x5CFBB600) /* 0x5CFBB600 / 13M = 120s */
#define SPM_SYSCLK_SETTLE	99

/**************************************
 * Define and Declare
 **************************************/
const char *wakeup_src_str[32] = {
	[0] = "R12_PCM_TIMER",
	[2] = "R12_KP_F32K_WAKEUP_EVENT_2_1",
	[3] = "R12_WDT_32K_EVENT_B",
	[4] = "R12_APXGPT_EVENT_B",
	[5] = "R12_CONN2AP_WAKEUP_B",
	[6] = "R12_EINT_EVENT_B",
	[7] = "R12_CONN_WDT_IRQ",
	[8] = "R12_IRRX_WAKEUP",
	[9] = "R12_LOW_BATTERY_IRQ_B",
	[11] = "R12_DSP_TIMER_EVENT_B",
	[12] = "R12_DSP_WAKEUP_B",
	[13] = "R12_PCM_WDT_WAKEUP_B",
	[14] = "R12_USB_CONNECT",
	[15] = "R12_USB_POWERDWN_B",
	[16] = "R12_SYS_TIMER_EVENT_B",
	[17] = "R12_EINT_SECURE",
	[18] = "R12_NIC_IRQ",
	[19] = "R12_UART0_IRQ_B",
	[20] = "R12_AFE_IRQ_MCU_B",
	[21] = "R12_THERM_CTRL_EVENT_B",
	[22] = "R12_SYS_CIRQ_B",
	[23] = "R12_DSPWDT_IRQ_B",
	[24] = "R12_CSYSPWREQ_B",
	[25] = "R12_MSDC2_WAKEUP",
	[27] = "R12_SEJ",
};

enum {
	CG_INFRA1 = 0,
	CG_INFRA0,
	CG_INFRA2,
	CG_INFRA3,
	CG_INFRA4,
	CG_MMSYS0,
	CG_MMSYS1,
	PWR_CAM,
	PWR_MFG,
	PWR_VENC,
	PWR_VDEC,
	PWR_APU,
	NR_GRPS,
};


/**************************************
 * Function and API
 **************************************/

int spm_for_gps_flag;

int __spm_is_last_online_cpu(uint32_t cpu)
{
	uint32_t i, pwr_status, cpux_pwr_mask, cpux_pwr = 0;

	pwr_status = mmio_read_32(PWR_STATUS);
	for (i = 0; i < PLATFORM_CLUSTER0_CORE_COUNT; i++) {
		if (i == cpu)
			continue;

		cpux_pwr_mask = PWR_STATUS_MP0_CPU0 << i;
		cpux_pwr |= (pwr_status & cpux_pwr_mask);
	}

	return cpux_pwr == 0 ? 1 : 0;
}

int __spm_is_idle_blocked_by_clk(struct pwr_ctrl *pwrctrl)
{
	uint32_t i, clks[NR_GRPS] = { 0 }, idle_masks[NR_GRPS];

	idle_masks[CG_INFRA1] = pwrctrl->infra1_idle_mask;
	idle_masks[CG_INFRA0] = pwrctrl->infra0_idle_mask;
	idle_masks[CG_INFRA2] = pwrctrl->infra2_idle_mask;
	idle_masks[CG_INFRA3] = pwrctrl->infra3_idle_mask;
	idle_masks[CG_INFRA4] = pwrctrl->infra4_idle_mask;
	idle_masks[CG_MMSYS0] = pwrctrl->mmsys0_idle_mask;
	idle_masks[CG_MMSYS1] = pwrctrl->mmsys1_idle_mask;
	idle_masks[PWR_CAM] = pwrctrl->cam_idle_mask;
	idle_masks[PWR_MFG] = pwrctrl->mfg_idle_mask;
	idle_masks[PWR_VENC] = pwrctrl->venc_idle_mask;
	idle_masks[PWR_VDEC] = pwrctrl->vdec_idle_mask;
	idle_masks[PWR_APU] = pwrctrl->apu_idle_mask;

	clks[CG_INFRA1] = ~spm_get_infra1_sta();
	clks[CG_INFRA0] = ~spm_get_infra0_sta();
	clks[CG_INFRA2] = ~spm_get_infra2_sta();
	clks[CG_INFRA3] = ~spm_get_infra3_sta();

	if (mmio_read_32(PWR_STATUS_2ND) & PWR_STA_DISP) {
		clks[CG_MMSYS0] = ~spm_get_mmsys_con0();
		clks[CG_MMSYS1] = ~spm_get_mmsys_con1();
	}

	clks[PWR_CAM] = ~spm_get_cam_pow_con();
	clks[PWR_MFG] = ~spm_get_mfg_pow_con();
	clks[PWR_VENC] = ~spm_get_venc_pow_con();
	clks[PWR_VDEC] = ~spm_get_vdec_pow_con();
	clks[PWR_APU] = ~spm_get_apu_pow_con();

	for (i = 0; i < NR_GRPS; i++)
		if (clks[i] & idle_masks[i])
			return 1;

	return 0;
}

int __spm_does_system_boot_120s(void)
{
	uint32_t cntcv_l, cntcv_h;

	cntcv_h = mmio_read_32(CNTCV_H);
	cntcv_l = mmio_read_32(CNTCV_L);

	if (cntcv_h == 0 && cntcv_l <= SYSTEM_BOOTING_120S)
		return 0;

	return 1;
}

void __spm_clean_idle_block_cnt(struct pwr_ctrl *pwrctrl)
{
	pwrctrl->by_swt = 0;
	pwrctrl->by_boot = 0;
	pwrctrl->by_cpu = 0;
	pwrctrl->by_clk = 0;
	pwrctrl->by_gpt = 0;
}

uint32_t __spm_set_sysclk_settle(void)
{
	uint32_t settle;

	mmio_write_32(SPM_CLK_SETTLE, SPM_SYSCLK_SETTLE);
	settle = mmio_read_32(SPM_CLK_SETTLE);

	return settle;
}

enum WAKE_REASON __spm_output_wake_reason(const struct wake_status *wakesta,
					  const struct pcm_desc *pcmdesc)
{
	uint64_t mpidr = read_mpidr();
	uint32_t cpu = platform_get_core_pos(mpidr);
	int i;

	/* assert_pc shows 0 when normal */
	for (i = 31; i >= 0; i--) {
		if (wakesta->r12 & (1U << i)) {
			INFO("cpu%u: wake up by %s, assert %u, timeout = %u\n",
			     cpu, wakeup_src_str[i], wakesta->assert_pc,
			     wakesta->timer_out);
			break;
		} else if (wakesta->r12 == 0) {
			INFO("cpu%u: r12 = 0x%x? assert %u, timeout = %u\n",
			     cpu, wakesta->r12, wakesta->assert_pc,
			     wakesta->timer_out);
			break;
		}
	}

	INFO("r13 = 0x%x, debug_flag = 0x%x 0x%x, ddren_sta = 0x%x\n",
	     wakesta->r13, wakesta->debug_flag, wakesta->debug_flag1,
	     wakesta->ddren_sta);
	INFO("r12 = 0x%x, r12_ext = 0x%x, raw_sta = 0x%x, idle_sta = 0x%x\n",
	     wakesta->r12, wakesta->r12_ext, wakesta->raw_sta,
	     wakesta->idle_sta);
	INFO("req_sta = 0x%x, event_reg = 0x%x, isr = 0x%x, rsv_6 = 0x%x\n",
	     wakesta->req_sta, wakesta->event_reg, wakesta->isr,
	     wakesta->rsv_6);
	INFO("raw_ext_sta = 0x%x, wake_misc = 0x%x, wake_event_mask = 0x%x\n",
	     wakesta->raw_ext_sta, wakesta->wake_misc,
	     wakesta->wake_event_mask);

	return 0;
}

int __spm_get_spmfw_idx(void)
{
	return __spmfw_idx;
}

void __spm_set_cpu_status(int cpu)
{
	if (cpu >= 0 && cpu < 4) {
		mmio_write_32(ROOT_CPUTOP_ADDR, MP0_CPUTOP_PWR_CON);
		mmio_write_32(ROOT_CORE_ADDR, MP0_CPU0_PWR_CON + (cpu * 0x4));
	} else if (cpu >= 4 && cpu < 8) {
		mmio_write_32(ROOT_CPUTOP_ADDR, MP1_CPUTOP_PWR_CON);
		mmio_write_32(ROOT_CORE_ADDR, MP1_CPU0_PWR_CON + ((cpu - 4) * 0x4));
	} else {
		ERROR("%s: error cpu number %d\n", __func__, cpu);
	}
}

void __spm_reset_and_init_pcm(const struct pcm_desc *pcmdesc)
{
	uint32_t con1;

	mmio_write_32(SPMC_DORMANT_ENABLE, 0);

	/* disable r0 and r7 to control power */
	mmio_write_32(PCM_PWR_IO_EN, 0);

	/* disable pcm timer after leaving FW */
	mmio_write_32(PCM_CON1, SPM_REGWR_CFG_KEY | (mmio_read_32(PCM_CON1) & ~PCM_TIMER_EN_LSB));

	/* reset PCM */
	mmio_write_32(PCM_CON0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	mmio_write_32(PCM_CON0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
	if ((mmio_read_32(PCM_FSM_STA) & 0x7fffff) != PCM_FSM_STA_DEF)
		ERROR("reset pcm(PCM_FSM_STA=0x%x)\n", mmio_read_32(PCM_FSM_STA));

	/* init PCM_CON0 (disable event vector) */
	mmio_write_32(PCM_CON0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | EN_IM_SLEEP_DVS_LSB);

	/* init PCM_CON1 */
	con1 = mmio_read_32(PCM_CON1);
	con1 = con1 | SPM_REGWR_CFG_KEY | MIF_APBEN_LSB |
	       (pcmdesc->replace ? 0 : IM_NONRP_EN_LSB) |
	       SPM_SRAM_ISOINT_B_LSB | EVENT_LOCK_EN_LSB |
	       SCP_APB_INTERNAL_EN_LSB | MCUPM_APB_INTERNAL_EN_LSB;
	mmio_write_32(PCM_CON1, con1);
}

void __spm_kick_im_to_fetch(const struct pcm_desc *pcmdesc)
{
	uint32_t ptr, len, con0;

	/* tell IM where is PCM code (use slave mode if code existed) */
	ptr = (unsigned int)(unsigned long)pcmdesc->base;
	len = pcmdesc->size - 1;
	if (mmio_read_32(PCM_IM_PTR) != ptr || mmio_read_32(PCM_IM_LEN) != len || pcmdesc->sess > 2) {
		mmio_write_32(PCM_IM_PTR, ptr);
		mmio_write_32(PCM_IM_LEN, len);
	} else {
		mmio_write_32(PCM_CON1, mmio_read_32(PCM_CON1) | SPM_REGWR_CFG_KEY | IM_SLAVE_LSB);
	}

	/* kick IM to fetch (only toggle IM_KICK) */
	con0 = mmio_read_32(PCM_CON0) & ~(IM_KICK_L_LSB | PCM_KICK_L_LSB);
	mmio_write_32(PCM_CON0, con0 | SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | IM_KICK_L_LSB);
	mmio_write_32(PCM_CON0, con0 | SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
}

void __spm_init_pcm_register(void)
{
	uint32_t pcm_pwr_io_en;

	/* init r0 with POWER_ON_VAL0 */
	mmio_write_32(PCM_REG_DATA_INI, mmio_read_32(SPM_POWER_ON_VAL0));
	pcm_pwr_io_en = mmio_read_32(PCM_PWR_IO_EN);
	pcm_pwr_io_en |= PCM_RF_SYNC_R0;
	mmio_write_32(PCM_PWR_IO_EN, pcm_pwr_io_en);
	pcm_pwr_io_en &= ~PCM_RF_SYNC_R0;
	mmio_write_32(PCM_PWR_IO_EN, pcm_pwr_io_en);

	/* init r7 with POWER_ON_VAL1 */
	mmio_write_32(PCM_REG_DATA_INI, mmio_read_32(SPM_POWER_ON_VAL1));
	pcm_pwr_io_en = mmio_read_32(PCM_PWR_IO_EN);
	pcm_pwr_io_en |= PCM_RF_SYNC_R7;
	mmio_write_32(PCM_PWR_IO_EN, pcm_pwr_io_en);
	pcm_pwr_io_en &= ~PCM_RF_SYNC_R7;
	mmio_write_32(PCM_PWR_IO_EN, pcm_pwr_io_en);
}

void __spm_init_event_vector(const struct pcm_desc *pcmdesc)
{
	/* init event vector register */
	mmio_write_32(PCM_EVENT_VECTOR0, pcmdesc->vec0);
	mmio_write_32(PCM_EVENT_VECTOR1, pcmdesc->vec1);
	mmio_write_32(PCM_EVENT_VECTOR2, pcmdesc->vec2);
	mmio_write_32(PCM_EVENT_VECTOR3, pcmdesc->vec3);
	mmio_write_32(PCM_EVENT_VECTOR4, pcmdesc->vec4);
	mmio_write_32(PCM_EVENT_VECTOR5, pcmdesc->vec5);
	mmio_write_32(PCM_EVENT_VECTOR6, pcmdesc->vec6);
	mmio_write_32(PCM_EVENT_VECTOR7, pcmdesc->vec7);
	mmio_write_32(PCM_EVENT_VECTOR8, pcmdesc->vec8);
	mmio_write_32(PCM_EVENT_VECTOR9, pcmdesc->vec9);
	mmio_write_32(PCM_EVENT_VECTOR10, pcmdesc->vec10);
	mmio_write_32(PCM_EVENT_VECTOR11, pcmdesc->vec11);
	mmio_write_32(PCM_EVENT_VECTOR12, pcmdesc->vec12);
	mmio_write_32(PCM_EVENT_VECTOR13, pcmdesc->vec13);
	mmio_write_32(PCM_EVENT_VECTOR14, pcmdesc->vec14);
	mmio_write_32(PCM_EVENT_VECTOR15, pcmdesc->vec15);
}

void __spm_src_req_update(const struct pwr_ctrl *pwrctrl, unsigned int resource_usage)
{
	uint8_t spm_apsrc_req = (resource_usage & SPM_RESOURCE_DRAM)    ? 1 : pwrctrl->spm_apsrc_req;
	uint8_t spm_ddren_req = (resource_usage & SPM_RESOURCE_DRAM)    ? 1 : pwrctrl->spm_ddren_req;
	uint8_t spm_vrf18_req = (resource_usage & SPM_RESOURCE_MAINPLL) ? 1 : pwrctrl->spm_vrf18_req;
	uint8_t spm_infra_req = (resource_usage & SPM_RESOURCE_MAINPLL) ? 1 : pwrctrl->spm_infra_req;
	uint8_t spm_f26m_req  = (resource_usage & SPM_RESOURCE_CK_26M)  ? 1 : pwrctrl->spm_f26m_req;

	/* SPM_SRC_REQ */
	mmio_write_32(SPM_SRC_REQ,
		((spm_apsrc_req & 0x1) << 0) |
		((spm_f26m_req & 0x1) << 1) |
		((spm_infra_req & 0x1) << 3) |
		((spm_vrf18_req & 0x1) << 4) |
		((spm_ddren_req & 0x1) << 7) |
		((pwrctrl->spm_rsv_src_req & 0x7) << 8) |
		((pwrctrl->spm_ddren_2_req & 0x1) << 11) |
		((pwrctrl->cpu_md_dvfs_sop_force_on & 0x1) << 16));
}

void __spm_set_power_control(const struct pwr_ctrl *pwrctrl)
{
	/* Auto-gen Start */

	/* SPM_AP_STANDBY_CON */
	mmio_write_32(SPM_AP_STANDBY_CON,
		((pwrctrl->wfi_op & 0x1) << 0) |
		((pwrctrl->mp0_cputop_idle_mask & 0x1) << 1) |
		((pwrctrl->mp1_cputop_idle_mask & 0x1) << 2) |
		((pwrctrl->mcusys_idle_mask & 0x1) << 4) |
		((pwrctrl->mm_mask_b & 0x3) << 16) |
		((pwrctrl->md_ddr_en_0_dbc_en & 0x1) << 18) |
		((pwrctrl->md_ddr_en_1_dbc_en & 0x1) << 19) |
		((pwrctrl->md_mask_b & 0x3) << 20) |
		((pwrctrl->sspm_mask_b & 0x1) << 22) |
		((pwrctrl->scp_mask_b & 0x1) << 23) |
		((pwrctrl->srcclkeni_mask_b & 0x1) << 24) |
		((pwrctrl->md_apsrc_1_sel & 0x1) << 25) |
		((pwrctrl->md_apsrc_0_sel & 0x1) << 26) |
		((pwrctrl->conn_ddr_en_dbc_en & 0x1) << 27) |
		((pwrctrl->conn_mask_b & 0x1) << 28) |
		((pwrctrl->conn_apsrc_sel & 0x1) << 29) |
		((pwrctrl->conn_srcclkena_sel_mask & 0x3) << 30));

	/* Set spm to connsys ack wait value and set hw umask*/
	if ((pwrctrl->conn_mask_b & 0x1) || (pwrctrl->md_mask_b & 0x1))
		mmio_write_32(SPM2CONN_ACK_WAIT_CYCLE, SPM2CONN_ACK_WAIT_CYCLE_VALUE);

	/* SPM_SRC_REQ */
	mmio_write_32(SPM_SRC_REQ,
		((pwrctrl->spm_apsrc_req & 0x1) << 0) |
		((pwrctrl->spm_f26m_req & 0x1) << 1) |
		((pwrctrl->spm_infra_req & 0x1) << 3) |
		((pwrctrl->spm_vrf18_req & 0x1) << 4) |
		((pwrctrl->spm_ddren_req & 0x1) << 7) |
		((pwrctrl->spm_rsv_src_req & 0x7) << 8) |
		((pwrctrl->spm_ddren_2_req & 0x1) << 11) |
		((pwrctrl->cpu_md_dvfs_sop_force_on & 0x1) << 16));

	/* SPM_SRC_MASK */
	mmio_write_32(SPM_SRC_MASK,
		((pwrctrl->csyspwreq_mask & 0x1) << 0) |
		((pwrctrl->ccif0_md_event_mask_b & 0x1) << 1) |
		((pwrctrl->ccif0_ap_event_mask_b & 0x1) << 2) |
		((pwrctrl->ccif1_md_event_mask_b & 0x1) << 3) |
		((pwrctrl->ccif1_ap_event_mask_b & 0x1) << 4) |
		((pwrctrl->ccif2_md_event_mask_b & 0x1) << 5) |
		((pwrctrl->ccif2_ap_event_mask_b & 0x1) << 6) |
		((pwrctrl->ccif3_md_event_mask_b & 0x1) << 7) |
		((pwrctrl->ccif3_ap_event_mask_b & 0x1) << 8) |
		((pwrctrl->md_srcclkena_0_infra_mask_b & 0x1) << 9) |
		((pwrctrl->md_srcclkena_1_infra_mask_b & 0x1) << 10) |
		((pwrctrl->conn_srcclkena_infra_mask_b & 0x1) << 11) |
		((pwrctrl->ufs_infra_req_mask_b & 0x1) << 12) |
		((pwrctrl->srcclkeni_infra_mask_b & 0x1) << 13) |
		((pwrctrl->md_apsrc_req_0_infra_mask_b & 0x1) << 14) |
		((pwrctrl->md_apsrc_req_1_infra_mask_b & 0x1) << 15) |
		((pwrctrl->conn_apsrcreq_infra_mask_b & 0x1) << 16) |
		((pwrctrl->ufs_srcclkena_mask_b & 0x1) << 17) |
		((pwrctrl->md_vrf18_req_0_mask_b & 0x1) << 18) |
		((pwrctrl->md_vrf18_req_1_mask_b & 0x1) << 19) |
		((pwrctrl->ufs_vrf18_req_mask_b & 0x1) << 20) |
		((pwrctrl->gce_vrf18_req_mask_b & 0x1) << 21) |
		((pwrctrl->conn_infra_req_mask_b & 0x1) << 22) |
		((pwrctrl->gce_apsrc_req_mask_b & 0x1) << 23) |
		((pwrctrl->disp0_apsrc_req_mask_b & 0x1) << 24) |
		((pwrctrl->disp1_apsrc_req_mask_b & 0x1) << 25) |
		((pwrctrl->mfg_req_mask_b & 0x1) << 26) |
		((pwrctrl->vdec_req_mask_b & 0x1) << 27) |
		((pwrctrl->mcu_apsrcreq_infra_mask_b & 0x1) << 28));

	/* SPM_SRC2_MASK */
	mmio_write_32(SPM_SRC2_MASK,
		((pwrctrl->md_ddr_en_0_mask_b & 0x1) << 0) |
		((pwrctrl->md_ddr_en_1_mask_b & 0x1) << 1) |
		((pwrctrl->conn_ddr_en_mask_b & 0x1) << 2) |
		((pwrctrl->ddren_md32_apsrc_req_mask_b & 0x1) << 3) |
		((pwrctrl->ddren_scp_apsrc_req_mask_b & 0x1) << 4) |
		((pwrctrl->disp0_ddren_mask_b & 0x1) << 5) |
		((pwrctrl->disp1_ddren_mask_b & 0x1) << 6) |
		((pwrctrl->gce_ddren_mask_b & 0x1) << 7) |
		((pwrctrl->ddren_emi_self_refresh_ch0_mask_b & 0x1) << 8) |
		((pwrctrl->ddren_emi_self_refresh_ch1_mask_b & 0x1) << 9) |
		((pwrctrl->mcu_apsrc_req_mask_b & 0x1) << 10) |
		((pwrctrl->mcu_ddren_mask_b & 0x1) << 11));

	/* SPM_WAKEUP_EVENT_MASK */
	mmio_write_32(SPM_WAKEUP_EVENT_MASK,
		((pwrctrl->spm_wakeup_event_mask & 0xffffffff) << 0));

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	mmio_write_32(SPM_WAKEUP_EVENT_EXT_MASK,
		((pwrctrl->spm_wakeup_event_ext_mask & 0xffffffff) << 0));

	/* SPM_SRC3_MASK */
	mmio_write_32(SPM_SRC3_MASK,
		((pwrctrl->md_ddr_en_2_0_mask_b & 0x1) << 0) |
		((pwrctrl->md_ddr_en_2_1_mask_b & 0x1) << 1) |
		((pwrctrl->conn_ddr_en_2_mask_b & 0x1) << 2) |
		((pwrctrl->ddren2_md32_apsrc_req_mask_b & 0x1) << 3) |
		((pwrctrl->ddren2_scp_apsrc_req_mask_b & 0x1) << 4) |
		((pwrctrl->disp0_ddren2_mask_b & 0x1) << 5) |
		((pwrctrl->disp1_ddren2_mask_b & 0x1) << 6) |
		((pwrctrl->gce_ddren2_mask_b & 0x1) << 7) |
		((pwrctrl->ddren2_emi_self_refresh_ch0_mask_b & 0x1) << 8) |
		((pwrctrl->ddren2_emi_self_refresh_ch1_mask_b & 0x1) << 9) |
		((pwrctrl->mcu_ddren_2_mask_b & 0x1) << 10));

	/* SPARE_SRC_REQ_MASK */
	mmio_write_32(SPARE_SRC_REQ_MASK,
		((pwrctrl->spare1_ddren_mask_b & 0x1) << 0) |
		((pwrctrl->spare1_apsrc_req_mask_b & 0x1) << 1) |
		((pwrctrl->spare1_vrf18_req_mask_b & 0x1) << 2) |
		((pwrctrl->spare1_infra_req_mask_b & 0x1) << 3) |
		((pwrctrl->spare1_srcclkena_mask_b & 0x1) << 4) |
		((pwrctrl->spare1_ddren2_mask_b & 0x1) << 5) |
		((pwrctrl->spare2_ddren_mask_b & 0x1) << 8) |
		((pwrctrl->spare2_apsrc_req_mask_b & 0x1) << 9) |
		((pwrctrl->spare2_vrf18_req_mask_b & 0x1) << 10) |
		((pwrctrl->spare2_infra_req_mask_b & 0x1) << 11) |
		((pwrctrl->spare2_srcclkena_mask_b & 0x1) << 12) |
		((pwrctrl->spare2_ddren2_mask_b & 0x1) << 13));

	/* MP0_CPU0_WFI_EN */
	mmio_write_32(MP0_CPU0_WFI_EN,
		((pwrctrl->mp0_cpu0_wfi_en & 0x1) << 0));

	/* MP0_CPU1_WFI_EN */
	mmio_write_32(MP0_CPU1_WFI_EN,
		((pwrctrl->mp0_cpu1_wfi_en & 0x1) << 0));

	/* MP0_CPU2_WFI_EN */
	mmio_write_32(MP0_CPU2_WFI_EN,
		((pwrctrl->mp0_cpu2_wfi_en & 0x1) << 0));

	/* MP0_CPU3_WFI_EN */
	mmio_write_32(MP0_CPU3_WFI_EN,
		((pwrctrl->mp0_cpu3_wfi_en & 0x1) << 0));

	/* MP1_CPU0_WFI_EN */
	mmio_write_32(MP1_CPU0_WFI_EN,
		((pwrctrl->mp1_cpu0_wfi_en & 0x1) << 0));

	/* MP1_CPU1_WFI_EN */
	mmio_write_32(MP1_CPU1_WFI_EN,
		((pwrctrl->mp1_cpu1_wfi_en & 0x1) << 0));

	/* MP1_CPU2_WFI_EN */
	mmio_write_32(MP1_CPU2_WFI_EN,
		((pwrctrl->mp1_cpu2_wfi_en & 0x1) << 0));

	/* MP1_CPU3_WFI_EN */
	mmio_write_32(MP1_CPU3_WFI_EN,
		((pwrctrl->mp1_cpu3_wfi_en & 0x1) << 0));
	/* Auto-gen End */
}

void __spm_set_wakeup_event(const struct pwr_ctrl *pwrctrl)
{
	uint32_t val, mask, isr;

	if (pwrctrl->timer_val_cust == 0)
		val = pwrctrl->timer_val ? pwrctrl->timer_val : PCM_TIMER_MAX;
	else
		val = pwrctrl->timer_val_cust;

	mmio_write_32(PCM_TIMER_VAL, val);
	mmio_write_32(PCM_CON1, mmio_read_32(PCM_CON1) | SPM_REGWR_CFG_KEY | PCM_TIMER_EN_LSB);

	/* unmask AP wakeup source */
	if (pwrctrl->wake_src_cust == 0)
		mask = pwrctrl->wake_src;
	else
		mask = pwrctrl->wake_src_cust;

	if (pwrctrl->csyspwreq_mask)
		mask &= ~WAKE_SRC_R12_CSYSPWREQ_B;
	mmio_write_32(SPM_WAKEUP_EVENT_MASK, ~mask);

	/* unmask SPM ISR (keep TWAM setting) */
	isr = mmio_read_32(SPM_IRQ_MASK) & SPM_TWAM_IRQ_MASK_LSB;
	mmio_write_32(SPM_IRQ_MASK, isr | ISRM_RET_IRQ_AUX);
}

void __spm_set_pcm_flags(struct pwr_ctrl *pwrctrl)
{
	/* set PCM flags and data */
	if (pwrctrl->pcm_flags_cust_clr != 0)
		pwrctrl->pcm_flags &= ~pwrctrl->pcm_flags_cust_clr;
	if (pwrctrl->pcm_flags_cust_set != 0)
		pwrctrl->pcm_flags |= pwrctrl->pcm_flags_cust_set;
	if (pwrctrl->pcm_flags1_cust_clr != 0)
		pwrctrl->pcm_flags1 &= ~pwrctrl->pcm_flags1_cust_clr;
	if (pwrctrl->pcm_flags1_cust_set != 0)
		pwrctrl->pcm_flags1 |= pwrctrl->pcm_flags1_cust_set;

	mmio_write_32(SPM_SW_FLAG, pwrctrl->pcm_flags);
	mmio_write_32(SPM_SW_RSV_2, pwrctrl->pcm_flags1);
}

void __spm_kick_pcm_to_run(void)
{
	uint32_t con0, pcm_pwr_io_en;

	/* init register to match PCM expectation */
	mmio_write_32(SPM_MAS_PAUSE_MASK_B, 0xffffffff);
	mmio_write_32(SPM_MAS_PAUSE2_MASK_B, 0xffffffff);
	mmio_write_32(PCM_REG_DATA_INI, 0);

	/* enable r0 and r7 to control power */
	mmio_write_32(PCM_PWR_IO_EN, PCM_PWRIO_EN_R0 | PCM_PWRIO_EN_R7);

	/* enable r0 and r7 to control power */
	pcm_pwr_io_en = (mmio_read_32(PCM_PWR_IO_EN) & (~0xff)) |
			 PCM_PWRIO_EN_R0 | PCM_PWRIO_EN_R7;
	mmio_write_32(PCM_PWR_IO_EN, pcm_pwr_io_en);

	INFO("Check IM ready\n");
	/* check IM ready */
	while ((mmio_read_32(PCM_FSM_STA) & (0x7 << 7)) != (0x4 << 7))
		;
	INFO("Check IM ready done\n");

	/* kick PCM to run (only toggle PCM_KICK) */
	con0 = mmio_read_32(PCM_CON0) & ~(IM_KICK_L_LSB | PCM_KICK_L_LSB);
	mmio_write_32(PCM_CON0, con0 | SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_KICK_L_LSB);
	mmio_write_32(PCM_CON0, con0 | SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
}


void __spm_get_wakeup_status(struct wake_status *wakesta)
{
	/* get PC value if PCM assert (pause abort) */
	wakesta->assert_pc = mmio_read_32(PCM_REG_DATA_INI);

	/* get wakeup event */
	wakesta->r12 = mmio_read_32(SPM_SW_RSV_0);        /* backup of PCM_REG12_DATA */
	wakesta->r12_ext = mmio_read_32(PCM_REG12_EXT_DATA);
	wakesta->raw_sta = mmio_read_32(SPM_WAKEUP_STA);
	wakesta->raw_ext_sta = mmio_read_32(SPM_WAKEUP_EXT_STA);
	wakesta->wake_misc = mmio_read_32(SPM_BSI_D0_SR);   /* backup of SPM_WAKEUP_MISC */
	wakesta->wake_event_mask = mmio_read_32(SPM_WAKEUP_EVENT_MASK);

	/* get sleep time */
	wakesta->timer_out = mmio_read_32(SPM_BSI_D1_SR);   /* backup of PCM_TIMER_OUT */

	/* get other SYS and co-clock status */
	wakesta->r13 = mmio_read_32(PCM_REG13_DATA);
	wakesta->idle_sta = mmio_read_32(SUBSYS_IDLE_STA);
	wakesta->req_sta = mmio_read_32(SRC_REQ_STA);
	wakesta->ddren_sta = mmio_read_32(SRC_DDREN_STA);

	/* get debug flag for PCM execution check */
	wakesta->debug_flag = mmio_read_32(SPM_SW_DEBUG);
	wakesta->debug_flag1 = mmio_read_32(WDT_LATCH_SPARE0_FIX);

	/* get special pattern (0xf0000 or 0x10000) if sleep abort */
	wakesta->event_reg = mmio_read_32(SPM_BSI_D2_SR);   /* PCM_EVENT_REG_STA */

	/* get ISR status */
	wakesta->isr = mmio_read_32(SPM_IRQ_STA);

	/* get ddren req */
	wakesta->rsv_6 = mmio_read_32(SPM_SW_RSV_6);
}

void __spm_clean_after_wakeup(void)
{
	/*
	 * Copy SPM_WAKEUP_STA to SPM_SW_RSV_0
	 * before clear SPM_WAKEUP_STA
	 *
	 * CPU dormant driver @kernel will copy  edge-trig IRQ pending
	 * (recorded @SPM_SW_RSV_0) to GIC
	 */
	mmio_write_32(SPM_SW_RSV_0, mmio_read_32(SPM_WAKEUP_STA) | mmio_read_32(SPM_SW_RSV_0));

	/* [Vcorefs] can not switch back to POWER_ON_VAL0 here,
	 *  the FW stays in VCORE DVFS which use r0 to Ctrl MEM
	 */
	/* disable r0 and r7 to control power */
	/* mmio_write_32(PCM_PWR_IO_EN, 0); */

	/* clean CPU wakeup event */
	mmio_write_32(SPM_CPU_WAKEUP_EVENT, 0);

	/* [Vcorefs] not disable pcm timer here, due to the
	 * following vcore dvfs will use it for latency check
	 */
	/* clean PCM timer event */
	/* mmio_write_32(PCM_CON1, SPM_REGWR_CFG_KEY | (mmio_read_32(PCM_CON1) & ~PCM_TIMER_EN_LSB)); */

	/* clean wakeup event raw status (for edge trigger event) */
	mmio_write_32(SPM_WAKEUP_EVENT_MASK, ~0);

	/* clean ISR status (except TWAM) */
	mmio_write_32(SPM_IRQ_MASK, mmio_read_32(SPM_IRQ_MASK) | ISRM_ALL_EXC_TWAM);
	mmio_write_32(SPM_IRQ_STA, ISRC_ALL_EXC_TWAM);
	mmio_write_32(SPM_SWINT_CLR, PCM_SW_INT_ALL);
}

void __spm_set_pcm_wdt(int en)
{
	/* enable PCM WDT (normal mode) to start count if needed */
	if (en) {
		uint32_t con1;

		con1 = mmio_read_32(PCM_CON1) & ~(PCM_WDT_WAKE_MODE_LSB);
		mmio_write_32(PCM_CON1, SPM_REGWR_CFG_KEY | con1);

		if (mmio_read_32(PCM_TIMER_VAL) > PCM_TIMER_MAX)
			mmio_write_32(PCM_TIMER_VAL, PCM_TIMER_MAX);
		mmio_write_32(PCM_WDT_VAL, mmio_read_32(PCM_TIMER_VAL) + PCM_WDT_TIMEOUT);
		mmio_write_32(PCM_CON1, con1 | SPM_REGWR_CFG_KEY | PCM_WDT_EN_LSB);
	} else {
		mmio_write_32(PCM_CON1, SPM_REGWR_CFG_KEY | (mmio_read_32(PCM_CON1) &
		~PCM_WDT_EN_LSB));
	}

}

void __spm_send_cpu_wakeup_event(void)
{
	/* clear PCM_REG_DATA_INI which is pcm assert address */
	mmio_write_32(PCM_REG_DATA_INI, 0);

	/* FIXME: Workaround for spmfw v4.6 or later version */
	mmio_write_32(SW2SPM_MAILBOX_1, 1);

	mmio_write_32(SPM_CPU_WAKEUP_EVENT, 1);
	/* SPM will clear SPM_CPU_WAKEUP_EVENT */
}
