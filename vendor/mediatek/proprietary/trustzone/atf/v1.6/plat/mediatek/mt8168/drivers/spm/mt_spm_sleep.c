#include <arch_helpers.h>
#include <debug.h>
#include <console.h>
#include <mmio.h>
#include <mt_spm.h>
#include <mt_spm_internal.h>
#include <mt_spm_reg.h>
#include <mt_spm_vcorefs.h>
#include <mtk_mcdi.h>
#include <plat_pm.h>
#include <platform.h>
#include <platform_def.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <uart.h>

/* for internal debug */
static struct wake_status spm_wakesta; /* record last wakesta */
static unsigned int resource_usage;

/**************************************
 * SW code for suspend
 **************************************/
#define WAKE_SRC_FOR_SUSPEND \
	(WAKE_SRC_R12_PCM_TIMER | \
	 WAKE_SRC_R12_KP_F32K_WAKEUP_EVENT_2_1 | \
	 WAKE_SRC_R12_WDT_32K_EVENT_B | \
	 WAKE_SRC_R12_CONN2AP_WAKEUP_B | \
	 WAKE_SRC_R12_EINT_EVENT_B | \
	 WAKE_SRC_R12_CONN_WDT_IRQ | \
	 WAKE_SRC_R12_IRRX_WAKEUP | \
	 WAKE_SRC_R12_LOW_BATTERY_IRQ_B | \
	 WAKE_SRC_R12_DSP_WAKEUP_B | \
	 WAKE_SRC_R12_WDT_WAKEUP_EVENT_B | \
	 WAKE_SRC_R12_USB_CONNECT | \
	 WAKE_SRC_R12_USB_POWERDWN_B | \
	 WAKE_SRC_R12_SYS_TIMER_EVENT_B | \
	 WAKE_SRC_R12_EINT_SECURE | \
	 WAKE_SRC_R12_NIC_IRQ | \
	 WAKE_SRC_R12_UART0_IRQ_B | \
	 WAKE_SRC_R12_AFE_IRQ_MCU_B | \
	 WAKE_SRC_R12_THERM_CTRL_EVENT_B | \
	 WAKE_SRC_R12_SYS_CIRQ_B | \
	 WAKE_SRC_R12_MSDC2_WAKEUP | \
	 WAKE_SRC_R12_DSPWDT_IRQ_B | \
	 WAKE_SRC_R12_SEJ)

#define SLP_PCM_FLAGS \
	(SPM_FLAG_SUSPEND_OPTION)

static struct pwr_ctrl suspend_ctrl = {
	.wake_src = WAKE_SRC_FOR_SUSPEND,
	.pcm_flags = SLP_PCM_FLAGS,
	.timer_val = 0x9600000, /* 4800s */

	/* Auto-gen Start */

	/* SPM_AP_STANDBY_CON */
	.wfi_op = 0x1,
	.mp0_cputop_idle_mask = 0,
	.mp1_cputop_idle_mask = 0,
	.mcusys_idle_mask = 0,
	.mm_mask_b = 0,
	.md_ddr_en_0_dbc_en = 0,
	.md_ddr_en_1_dbc_en = 0,
	.md_mask_b = 0,
	.sspm_mask_b = 0,
	.scp_mask_b = 0,
	.srcclkeni_mask_b = 0,
	.md_apsrc_1_sel = 0,
	.md_apsrc_0_sel = 0,
	.conn_ddr_en_dbc_en = 0,
	.conn_mask_b = 0x1,
	.conn_apsrc_sel = 0,
	.conn_srcclkena_sel_mask = 0x1,

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
	.csyspwreq_mask = 0,
	.ccif0_md_event_mask_b = 0,
	.ccif0_ap_event_mask_b = 0,
	.ccif1_md_event_mask_b = 0,
	.ccif1_ap_event_mask_b = 0,
	.ccif2_md_event_mask_b = 0,
	.ccif2_ap_event_mask_b = 0,
	.ccif3_md_event_mask_b = 0,
	.ccif3_ap_event_mask_b = 0,
	.md_srcclkena_0_infra_mask_b = 0,
	.md_srcclkena_1_infra_mask_b = 0,
	.conn_srcclkena_infra_mask_b = 0x1,
	.ufs_infra_req_mask_b = 0,
	.srcclkeni_infra_mask_b = 0,
	.md_apsrc_req_0_infra_mask_b = 0,
	.md_apsrc_req_1_infra_mask_b = 0,
	.conn_apsrcreq_infra_mask_b = 0x1,
	.ufs_srcclkena_mask_b = 0,
	.md_vrf18_req_0_mask_b = 0,
	.md_vrf18_req_1_mask_b = 0,
	.ufs_vrf18_req_mask_b = 0,
	.gce_vrf18_req_mask_b = 0x1,
	.conn_infra_req_mask_b = 0x1,
	.gce_apsrc_req_mask_b = 0x1,
	.disp0_apsrc_req_mask_b = 0x1,
	.disp1_apsrc_req_mask_b = 0x1,
	.mfg_req_mask_b = 0,
	.vdec_req_mask_b = 0,
	.mcu_apsrcreq_infra_mask_b = 0x1,

	/* SPM_SRC2_MASK */
	.md_ddr_en_0_mask_b = 0,
	.md_ddr_en_1_mask_b = 0,
	.conn_ddr_en_mask_b = 0x1,
	.ddren_md32_apsrc_req_mask_b = 0,
	.ddren_scp_apsrc_req_mask_b = 0,
	.disp0_ddren_mask_b = 0x1,
	.disp1_ddren_mask_b = 0x1,
	.gce_ddren_mask_b = 0x1,
	.ddren_emi_self_refresh_ch0_mask_b = 0,
	.ddren_emi_self_refresh_ch1_mask_b = 0,
	.mcu_apsrc_req_mask_b = 0,
	.mcu_ddren_mask_b = 0,

	/* SPM_SRC3_MASK */
	.md_ddr_en_2_0_mask_b = 0,
	.md_ddr_en_2_1_mask_b = 0,
	.conn_ddr_en_2_mask_b = 0x1,
	.ddren2_md32_apsrc_req_mask_b = 0,
	.ddren2_scp_apsrc_req_mask_b = 0,
	.disp0_ddren2_mask_b = 0x1,
	.disp1_ddren2_mask_b = 0x1,
	.gce_ddren2_mask_b = 0x1,
	.ddren2_emi_self_refresh_ch0_mask_b = 0x1,
	.ddren2_emi_self_refresh_ch1_mask_b = 0x1,
	.mcu_ddren_2_mask_b = 0x1,

	/* SPARE_SRC_REQ_MASK */
	.spare1_ddren_mask_b = 0x1,
	.spare1_apsrc_req_mask_b = 0x1,
	.spare1_vrf18_req_mask_b = 0x1,
	.spare1_infra_req_mask_b = 0x1,
	.spare1_srcclkena_mask_b = 0x1,
	.spare1_ddren2_mask_b = 0x1,
	.spare2_ddren_mask_b = 0x1,
	.spare2_apsrc_req_mask_b = 0,
	.spare2_vrf18_req_mask_b = 0,
	.spare2_infra_req_mask_b = 0,
	.spare2_srcclkena_mask_b = 0,
	.spare2_ddren2_mask_b = 0x1,

	/* SPM_WAKEUP_EVENT_MASK */
	.spm_wakeup_event_mask = 0xC0000000,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	.spm_wakeup_event_ext_mask = 0xFFFFFFFF,


	/* MP0_CPU0_WFI_EN */
	.mp0_cpu0_wfi_en = 1,

	/* MP0_CPU1_WFI_EN */
	.mp0_cpu1_wfi_en = 1,

	/* MP0_CPU2_WFI_EN */
	.mp0_cpu2_wfi_en = 1,

	/* MP0_CPU3_WFI_EN */
	.mp0_cpu3_wfi_en = 1,

	/* MP1_CPU0_WFI_EN */
	.mp1_cpu0_wfi_en = 0,

	/* MP1_CPU1_WFI_EN */
	.mp1_cpu1_wfi_en = 0,

	/* MP1_CPU2_WFI_EN */
	.mp1_cpu2_wfi_en = 0,

	/* MP1_CPU3_WFI_EN */
	.mp1_cpu3_wfi_en = 0,

	/* Auto-gen End */
};

struct spm_lp_scen __spm_suspend = {
	.pwrctrl = &suspend_ctrl,
};

void spm_suspend_args(uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4)
{
	struct pwr_ctrl *pwrctrl;

	pwrctrl = __spm_suspend.pwrctrl;
	pwrctrl->pcm_flags = x1;
	pwrctrl->pcm_flags1 = x2;
	pwrctrl->timer_val = x3;

	/* get spm resource request from kernel */
	resource_usage = x4;
}

void go_to_sleep_before_wfi_no_resume(void)
{
	struct pwr_ctrl *pwrctrl;
	uint64_t mpidr = read_mpidr();
	uint32_t cpu = platform_get_core_pos(mpidr), settle;

	pwrctrl = __spm_suspend.pwrctrl;

	settle = __spm_set_sysclk_settle();
	__spm_set_cpu_status(cpu);
	__spm_set_power_control(pwrctrl);
	__spm_set_wakeup_event(pwrctrl);
	__spm_sync_vcore_dvfs_power_control(pwrctrl, __spm_vcorefs.pwrctrl);
	__spm_set_pcm_flags(pwrctrl);
	if (!pwrctrl->wdt_disable)
		__spm_set_pcm_wdt(1);
	__spm_send_cpu_wakeup_event();

	if (is_infra_pdn(pwrctrl->pcm_flags))
		mtk_uart_save();

	INFO("cpu%d: \"%s\", wakesrc = 0x%x, pcm_con1 = 0x%x\n",
	     cpu, spm_get_firmware_version(), pwrctrl->wake_src,
	     mmio_read_32(PCM_CON1));
	INFO("settle = %u, sec = %u, sw_flag = 0x%x 0x%x, src_req = 0x%x\n",
	     settle, mmio_read_32(PCM_TIMER_VAL) / 32768,
	     pwrctrl->pcm_flags, pwrctrl->pcm_flags1,
	     mmio_read_32(SPM_SRC_REQ));
}

static void go_to_sleep_after_wfi(void)
{
	struct pcm_desc *pcmdesc = NULL;
	struct pwr_ctrl *pwrctrl;

	pwrctrl = __spm_suspend.pwrctrl;

	if (is_infra_pdn(pwrctrl->pcm_flags))
		mtk_uart_restore();

	if (!pwrctrl->wdt_disable)
		__spm_set_pcm_wdt(0);

	__spm_get_wakeup_status(&spm_wakesta);
	/* __spm_clean_after_wakeup(); */
	__spm_output_wake_reason(&spm_wakesta, pcmdesc);
}

void spm_suspend(void)
{
	spm_lock_get();
	mcupm_hp_idle();
	mcupm_hold_req();
	go_to_sleep_before_wfi_no_resume();
	spm_lock_release();
}

void spm_suspend_finish(void)
{
	spm_lock_get();
	go_to_sleep_after_wfi();
	mcupm_release_req();
	spm_lock_release();
}
