#include <arch_helpers.h>
#include <debug.h>
#include <mmio.h>
#include <mt_spm.h>
#include <mt_spm_internal.h>
#include <mt_spm_reg.h>
#include <mt_spm_vcorefs.h>
#include <plat_pm.h>
#include <platform.h>
#include <platform_def.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* for internal debug */
#define SPM_BYPASS_SYSPWREQ     1

static struct wake_status spm_wakesta; /* record last wakesta */
static wake_reason_t spm_wake_reason = WR_NONE;

/**************************************

 * SW code for suspend
 **************************************/
#define SPM_SYSCLK_SETTLE       99	/* 3ms */

#if defined(CFG_MICROTRUST_TEE_SUPPORT)
#define WAKE_SRC_FOR_SUSPEND \
	(WAKE_SRC_R12_PCM_TIMER | \
	WAKE_SRC_R12_SSPM_WDT_EVENT_B | \
	WAKE_SRC_R12_KP_IRQ_B | \
	WAKE_SRC_R12_CONN2AP_SPM_WAKEUP_B | \
	WAKE_SRC_R12_EINT_EVENT_B | \
	WAKE_SRC_R12_CONN_WDT_IRQ_B | \
	WAKE_SRC_R12_CCIF0_EVENT_B | \
	WAKE_SRC_R12_SSPM_SPM_IRQ_B | \
	WAKE_SRC_R12_SCP_SPM_IRQ_B | \
	WAKE_SRC_R12_SCP_WDT_EVENT_B | \
	WAKE_SRC_R12_USB_CDSC_B | \
	WAKE_SRC_R12_USB_POWERDWN_B | \
	WAKE_SRC_R12_SYS_TIMER_EVENT_B | \
	WAKE_SRC_R12_EINT_EVENT_SECURE_B | \
	WAKE_SRC_R12_CCIF1_EVENT_B | \
	WAKE_SRC_R12_MD2AP_PEER_EVENT_B | \
	WAKE_SRC_R12_MD1_WDT_B | \
	WAKE_SRC_R12_CLDMA_EVENT_B)
#else
#define WAKE_SRC_FOR_SUSPEND \
	(WAKE_SRC_R12_PCM_TIMER | \
	WAKE_SRC_R12_SSPM_WDT_EVENT_B | \
	WAKE_SRC_R12_KP_IRQ_B | \
	WAKE_SRC_R12_CONN2AP_SPM_WAKEUP_B | \
	WAKE_SRC_R12_EINT_EVENT_B | \
	WAKE_SRC_R12_CONN_WDT_IRQ_B | \
	WAKE_SRC_R12_CCIF0_EVENT_B | \
	WAKE_SRC_R12_SSPM_SPM_IRQ_B | \
	WAKE_SRC_R12_SCP_SPM_IRQ_B | \
	WAKE_SRC_R12_SCP_WDT_EVENT_B | \
	WAKE_SRC_R12_USB_CDSC_B | \
	WAKE_SRC_R12_USB_POWERDWN_B | \
	WAKE_SRC_R12_SYS_TIMER_EVENT_B | \
	WAKE_SRC_R12_EINT_EVENT_SECURE_B | \
	WAKE_SRC_R12_CCIF1_EVENT_B | \
	WAKE_SRC_R12_MD2AP_PEER_EVENT_B | \
	WAKE_SRC_R12_MD1_WDT_B | \
	WAKE_SRC_R12_CLDMA_EVENT_B | \
	WAKE_SRC_R12_SEJ_WDT_GPT_B)
#endif

static struct pwr_ctrl suspend_ctrl = {
	.wake_src = WAKE_SRC_FOR_SUSPEND,

	/* Auto-gen Start */

	/* SPM_AP_STANDBY_CON */
	.wfi_op = WFI_OP_AND,
	.mp0_cputop_idle_mask = 0,
	.mp1_cputop_idle_mask = 0,
	.mcusys_idle_mask = 0,
	.mm_mask_b = 0,
	.md_ddr_en_0_dbc_en = 0x1,
	.md_ddr_en_1_dbc_en = 0,
	.md_mask_b = 0x1,
	.sspm_mask_b = 0x1,
	.scp_mask_b = 0x1,
	.srcclkeni_mask_b = 0x1,
	.md_apsrc_1_sel = 0,
	.md_apsrc_0_sel = 0,
	.conn_ddr_en_dbc_en = 0x1,
	.conn_mask_b = 0x1,
	.conn_apsrc_sel = 0,

	/* SPM_SRC_REQ */
	.spm_apsrc_req = 0,
	.spm_f26m_req = 0,
	.spm_infra_req = 0,
	.spm_vrf18_req = 0,
	.spm_ddren_req = 0,
	.spm_rsv_src_req = 0,
	.spm_ddren_2_req = 0,
	.cpu_md_dvfs_sop_force_on = 0,

	/* SPM_SRC_MASK */
#if SPM_BYPASS_SYSPWREQ
	.csyspwreq_mask = 0x1,
#endif
	.ccif0_md_event_mask_b = 0x1,
	.ccif0_ap_event_mask_b = 0x1,
	.ccif1_md_event_mask_b = 0x1,
	.ccif1_ap_event_mask_b = 0x1,
	.ccif2_md_event_mask_b = 0x1,
	.ccif2_ap_event_mask_b = 0x1,
	.ccif3_md_event_mask_b = 0x1,
	.ccif3_ap_event_mask_b = 0x1,
	.md_srcclkena_0_infra_mask_b = 0x1,
	.md_srcclkena_1_infra_mask_b = 0,
	.conn_srcclkena_infra_mask_b = 0,
	.ufs_infra_req_mask_b = 0,
	.srcclkeni_infra_mask_b = 0,
	.md_apsrc_req_0_infra_mask_b = 0x1,
	.md_apsrc_req_1_infra_mask_b = 0x1,
	.conn_apsrcreq_infra_mask_b = 0x1,
	.ufs_srcclkena_mask_b = 0,
	.md_vrf18_req_0_mask_b = 0,
	.md_vrf18_req_1_mask_b = 0,
	.ufs_vrf18_req_mask_b = 0,
	.gce_vrf18_req_mask_b = 0,
	.conn_infra_req_mask_b = 0x1,
	.gce_apsrc_req_mask_b = 0,
	.disp0_apsrc_req_mask_b = 0,
	.disp1_apsrc_req_mask_b = 0,
	.mfg_req_mask_b = 0,
	.vdec_req_mask_b = 0,

	/* SPM_SRC2_MASK */
	.md_ddr_en_0_mask_b = 0x1,
	.md_ddr_en_1_mask_b = 0,
	.conn_ddr_en_mask_b = 0x1,
	.ddren_sspm_apsrc_req_mask_b = 0x1,
	.ddren_scp_apsrc_req_mask_b = 0x1,
	.disp0_ddren_mask_b = 0x1,
	.disp1_ddren_mask_b = 0x1,
	.gce_ddren_mask_b = 0x1,
	.ddren_emi_self_refresh_ch0_mask_b = 0,
	.ddren_emi_self_refresh_ch1_mask_b = 0,

	/* SPM_WAKEUP_EVENT_MASK */
	.spm_wakeup_event_mask = 0xF1782218,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	.spm_wakeup_event_ext_mask = 0xFFFFFFFF,

	/* SPM_SRC3_MASK */
	.md_ddr_en_2_0_mask_b = 0x1,
	.md_ddr_en_2_1_mask_b = 0,
	.conn_ddr_en_2_mask_b = 0x1,
	.ddren2_sspm_apsrc_req_mask_b = 0x1,
	.ddren2_scp_apsrc_req_mask_b = 0x1,
	.disp0_ddren2_mask_b = 0,
	.disp1_ddren2_mask_b = 0,
	.gce_ddren2_mask_b = 0,
	.ddren2_emi_self_refresh_ch0_mask_b = 0,
	.ddren2_emi_self_refresh_ch1_mask_b = 0,

	/* MP0_CPU0_WFI_EN */
	.mp0_cpu0_wfi_en = 1,

	/* MP0_CPU1_WFI_EN */
	.mp0_cpu1_wfi_en = 1,

	/* MP0_CPU2_WFI_EN */
	.mp0_cpu2_wfi_en = 1,

	/* MP0_CPU3_WFI_EN */
	.mp0_cpu3_wfi_en = 1,

	/* MP1_CPU0_WFI_EN */
	.mp1_cpu0_wfi_en = 1,

	/* MP1_CPU1_WFI_EN */
	.mp1_cpu1_wfi_en = 1,

	/* MP1_CPU2_WFI_EN */
	.mp1_cpu2_wfi_en = 1,

	/* MP1_CPU3_WFI_EN */
	.mp1_cpu3_wfi_en = 1,

	/* Auto-gen End */

	.md_ddr_en_0_dbc_len = 154,
};

struct spm_lp_scen __spm_suspend = {
	.pwrctrl = &suspend_ctrl,
};

static void spm_set_sysclk_settle(void)
{
	__uint32_t settle;

	/* SYSCLK settle = MD SYSCLK settle but set it again for MD PDN */
	mmio_write_32(SPM_CLK_SETTLE, SPM_SYSCLK_SETTLE);
	settle = mmio_read_32(SPM_CLK_SETTLE);

	INFO("md_settle = %u, settle = %u\n", SPM_SYSCLK_SETTLE, settle);
}

void spm_suspend_args(__uint64_t x1, __uint64_t x2, __uint64_t x3)
{
	struct pwr_ctrl *pwrctrl;

	pwrctrl = __spm_suspend.pwrctrl;
	pwrctrl->pcm_flags = x1;
	pwrctrl->pcm_flags1 = x2;
	pwrctrl->timer_val = x3;

#if 0
	/* for gps only case */
	if (pwrctrl->pcm_flags & SPM_FLAG_DIS_ULPOSC_OFF)
		spm_for_gps_flag = 1;
	else
		spm_for_gps_flag = 0;
#endif
}

void go_to_sleep_before_wfi_no_resume(void)
{
	struct pwr_ctrl *pwrctrl;
	uint64_t mpidr = read_mpidr();
	__uint32_t cpu = platform_get_core_pos(mpidr);

	pwrctrl = __spm_suspend.pwrctrl;

	/*
	 * 1) Setup scenario setting
	 * 2) Set CPU to SPM wakeup event, trigger SPM FW scenario switch
	 */
	__spm_set_cpu_status(cpu);

	__spm_set_power_control(pwrctrl);

	__spm_set_wakeup_event(pwrctrl);

	__spm_sync_vcore_dvfs_power_control(pwrctrl, __spm_vcorefs.pwrctrl);

	__spm_sync_mc_dsr_power_control(pwrctrl, NULL);

	__spm_set_pcm_flags(pwrctrl);

	if (!pwrctrl->wdt_disable) {
		__spm_set_pcm_wdt(1);
		mmio_write_32(SUSPEND_FOOTPRINT, mmio_read_32(SUSPEND_FOOTPRINT) | SPM_WDT_ENABLE_FOOTPRINT);
	}

	__spm_send_cpu_wakeup_event();

}

void go_to_sleep_before_wfi(void)
{
	struct pcm_desc *pcmdesc = NULL;
	struct pwr_ctrl *pwrctrl;
	uint64_t mpidr = read_mpidr();
	__uint32_t cpu = platform_get_core_pos(mpidr);
	int spmfw_idx = __spm_get_spmfw_idx();

	if (dyna_load_pcm[spmfw_idx].ready) {
		pcmdesc = &(dyna_load_pcm[spmfw_idx].desc);
	} else {
		INFO("firmware is not ready!!!\n");
		return;
	}
	pwrctrl = __spm_suspend.pwrctrl;

	INFO("Online CPU is %d, suspend FW ver. is %s\n",
			cpu, pcmdesc->version);

	INFO("sec = %u, wakesrc = 0x%x (%u)(%u)\n",
			pwrctrl->timer_val, pwrctrl->wake_src,
			is_cpu_pdn(pwrctrl->pcm_flags),
			is_infra_pdn(pwrctrl->pcm_flags));

	__spm_set_cpu_status(cpu);
	spm_set_sysclk_settle();
	__spm_reset_and_init_pcm(pcmdesc);
	__spm_kick_im_to_fetch(pcmdesc);
	__spm_init_pcm_register();
	__spm_init_event_vector(pcmdesc);

	__spm_sync_vcore_dvfs_power_control(pwrctrl, __spm_vcorefs.pwrctrl);

	__spm_sync_mc_dsr_power_control(pwrctrl, NULL);

	__spm_set_power_control(pwrctrl);
	__spm_set_wakeup_event(pwrctrl);

	if (!pwrctrl->wdt_disable) {
		__spm_set_pcm_wdt(1);
		mmio_write_32(SUSPEND_FOOTPRINT, mmio_read_32(SUSPEND_FOOTPRINT) | SPM_WDT_ENABLE_FOOTPRINT);
	}

	__spm_kick_pcm_to_run(pwrctrl);
}

static void go_to_sleep_after_wfi(void)
{
	struct pcm_desc *pcmdesc = NULL;
	struct pwr_ctrl *pwrctrl;
	int spmfw_idx = __spm_get_spmfw_idx();

	if (dyna_load_pcm[spmfw_idx].ready) {
		pcmdesc = &(dyna_load_pcm[spmfw_idx].desc);
	} else {
		INFO("firmware is not ready!!!\n");
		return;
	}

	pwrctrl = __spm_suspend.pwrctrl;

	if (!pwrctrl->wdt_disable) {
		__spm_set_pcm_wdt(0);
		mmio_write_32(SUSPEND_FOOTPRINT, mmio_read_32(SUSPEND_FOOTPRINT) | SPM_WDT_DISABLE_FOOTPRINT);
	}

	__spm_get_wakeup_status(&spm_wakesta);
	__spm_clean_after_wakeup();
	spm_wake_reason = __spm_output_wake_reason(&spm_wakesta, pcmdesc, "suspend");

	INFO("spm_wake_reason=%d\n", spm_wake_reason);
}

void spm_pcm_wdt(uint64_t enable, uint64_t time)
{
#if SPM_PCMWDT_EN
	struct pwr_ctrl *pwrctrl;

	pwrctrl = __spm_suspend.pwrctrl;

	if (time)
		mmio_write_32(PCM_TIMER_VAL, time);
	if (!pwrctrl->wdt_disable)
		__spm_set_pcm_wdt(enable);
	if (enable)
		mmio_write_32(SUSPEND_FOOTPRINT, mmio_read_32(SUSPEND_FOOTPRINT) | SPM_WDT_ENABLE_FOOTPRINT);
	else
		mmio_write_32(SUSPEND_FOOTPRINT, mmio_read_32(SUSPEND_FOOTPRINT) | SPM_WDT_DISABLE_FOOTPRINT);
#endif
}

void spm_suspend(void)
{
	spm_lock_get();

#if SPM_FW_NO_RESUME
	go_to_sleep_before_wfi_no_resume();
#else
	go_to_sleep_before_wfi();
#endif

	spm_lock_release();
}

void spm_suspend_finish(void)
{
	spm_lock_get();
	go_to_sleep_after_wfi();
	spm_lock_release();
}
