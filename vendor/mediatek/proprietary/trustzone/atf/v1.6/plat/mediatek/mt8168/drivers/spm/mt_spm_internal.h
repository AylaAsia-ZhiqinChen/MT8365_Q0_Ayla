#ifndef __MT_SPM_INTERNAL__
#define __MT_SPM_INTERNAL__

/**************************************
 * Config and Parameter
 **************************************/
/* SPM_POWER_ON_VAL0 */
#define POWER_ON_VAL0_DEF	0x1000f100
/* SPM_POWER_ON_VAL1 */
#define POWER_ON_VAL1_DEF	0x00815828
/* PCM_FSM_STA */
#define PCM_FSM_STA_DEF		0x00048490
/* SPM_WAKEUP_EVENT_MASK */
#define SPM_WAKEUP_EVENT_MASK_DEF	0xC0000000

/* PCM_WDT_VAL */
#define PCM_WDT_TIMEOUT		(20 * 32768)	/* 20s */
/* PCM_TIMER_VAL */
#define PCM_TIMER_MAX		(0xffffffff - PCM_WDT_TIMEOUT)

/**************************************
 * Define and Declare
 **************************************/
/* PCM_PWR_IO_EN */
#define PCM_PWRIO_EN_R0		(1U << 0)
#define PCM_PWRIO_EN_R7		(1U << 7)
#define PCM_RF_SYNC_R0		(1U << 16)
#define PCM_RF_SYNC_R6		(1U << 22)
#define PCM_RF_SYNC_R7		(1U << 23)

/* SPM_SWINT */
#define PCM_SW_INT0		(1U << 0)
#define PCM_SW_INT1		(1U << 1)
#define PCM_SW_INT2		(1U << 2)
#define PCM_SW_INT3		(1U << 3)
#define PCM_SW_INT4		(1U << 4)
#define PCM_SW_INT5		(1U << 5)
#define PCM_SW_INT6		(1U << 6)
#define PCM_SW_INT7		(1U << 7)
#define PCM_SW_INT8		(1U << 8)
#define PCM_SW_INT9		(1U << 9)
#define PCM_SW_INT_ALL		(PCM_SW_INT9 | PCM_SW_INT8 | PCM_SW_INT7 | \
				 PCM_SW_INT6 | PCM_SW_INT5 | PCM_SW_INT4 | \
				 PCM_SW_INT3 | PCM_SW_INT2 | PCM_SW_INT1 | \
				 PCM_SW_INT0)

/* SPM_AP_STANDBY_CON */
#define WFI_OP_AND		1
#define WFI_OP_OR		0

/* SPM_IRQ_MASK */
#define ISRM_TWAM		(1U << 2)
#define ISRM_PCM_RETURN		(1U << 3)
#define ISRM_RET_IRQ0		(1U << 8)
#define ISRM_RET_IRQ1		(1U << 9)
#define ISRM_RET_IRQ2		(1U << 10)
#define ISRM_RET_IRQ3		(1U << 11)
#define ISRM_RET_IRQ4		(1U << 12)
#define ISRM_RET_IRQ5		(1U << 13)
#define ISRM_RET_IRQ6		(1U << 14)
#define ISRM_RET_IRQ7		(1U << 15)
#define ISRM_RET_IRQ8		(1U << 16)
#define ISRM_RET_IRQ9		(1U << 17)
#define ISRM_RET_IRQ_AUX	((ISRM_RET_IRQ9) | (ISRM_RET_IRQ8) | \
				 (ISRM_RET_IRQ7) | (ISRM_RET_IRQ6) | \
				 (ISRM_RET_IRQ5) | (ISRM_RET_IRQ4) | \
				 (ISRM_RET_IRQ3) | (ISRM_RET_IRQ2) | \
				 (ISRM_RET_IRQ1))
#define ISRM_ALL_EXC_TWAM	(ISRM_RET_IRQ_AUX /*| ISRM_RET_IRQ0 | ISRM_PCM_RETURN*/)
#define ISRM_ALL		(ISRM_ALL_EXC_TWAM | ISRM_TWAM)

/* SPM_IRQ_STA */
#define ISRS_TWAM		(1U << 2)
#define ISRS_PCM_RETURN		(1U << 3)
#define ISRS_SW_INT0		(1U << 4)
#define ISRC_TWAM		ISRS_TWAM
#define ISRC_ALL_EXC_TWAM	ISRS_PCM_RETURN
#define ISRC_ALL		(ISRC_ALL_EXC_TWAM | ISRC_TWAM)

/* SPM_WAKEUP_MISC */
#define WAKE_MISC_TWAM		(1U << 18)
#define WAKE_MISC_PCM_TIMER	(1U << 19)
#define WAKE_MISC_CPU_WAKE	(1U << 20)

/* SPM_ACK_WAIT_CYCLE setting */
#define SPM2CONN_ACK_WAIT_CYCLE_TIME_COUNT	30 /*30T*/
#define SPM2CONN_ACK_CYCLE_WAIT		(SPM2CONN_ACK_WAIT_CYCLE_TIME_COUNT & 0xff)
#define SPM2CONN_ACK_WAIT_SHIFT_APSRC		0
#define SPM2CONN_ACK_WAIT_SHIFT_DDREN		8
#define SPM2CONN_ACK_WAIT_SHIFT_DDREN2		16

#define SPM2CONN_ACK_WAIT_MASKBIT_APSRC		24
#define SPM2CONN_ACK_WAIT_MASKBIT_DDREN		25
#define SPM2CONN_ACK_WAIT_MASKBIT_DDREN2	26

#define SPM2CONN_ACK_WAIT_MASK_VALUE (\
	(1U << SPM2CONN_ACK_WAIT_MASKBIT_APSRC) \
	| (1U << SPM2CONN_ACK_WAIT_MASKBIT_DDREN) \
	| (1U << SPM2CONN_ACK_WAIT_MASKBIT_DDREN2))

#define SPM2CONN_ACK_WAIT_CYCLE_VALUE ( \
	(SPM2CONN_ACK_CYCLE_WAIT << SPM2CONN_ACK_WAIT_SHIFT_APSRC) \
	| (SPM2CONN_ACK_CYCLE_WAIT << SPM2CONN_ACK_WAIT_SHIFT_DDREN) \
	| (SPM2CONN_ACK_CYCLE_WAIT << SPM2CONN_ACK_WAIT_SHIFT_DDREN2))



struct pcm_desc {
	const char *version;	/* PCM code version */
	const uint32_t *base;	/* binary array base */
	const uint32_t base_dma;	/* dma addr of base */
	const uint16_t size;		/* binary array size */
	const uint8_t sess;		/* session number */
	const uint8_t replace;	/* replace mode */
	const uint16_t addr_2nd;	/* 2nd binary array size */
	const uint16_t reserved;	/* for 32bit alignment */

	uint32_t vec0;		/* event vector 0 config */
	uint32_t vec1;		/* event vector 1 config */
	uint32_t vec2;		/* event vector 2 config */
	uint32_t vec3;		/* event vector 3 config */
	uint32_t vec4;		/* event vector 4 config */
	uint32_t vec5;		/* event vector 5 config */
	uint32_t vec6;		/* event vector 6 config */
	uint32_t vec7;		/* event vector 7 config */
	uint32_t vec8;		/* event vector 8 config */
	uint32_t vec9;		/* event vector 9 config */
	uint32_t vec10;		/* event vector 10 config */
	uint32_t vec11;		/* event vector 11 config */
	uint32_t vec12;		/* event vector 12 config */
	uint32_t vec13;		/* event vector 13 config */
	uint32_t vec14;		/* event vector 14 config */
	uint32_t vec15;		/* event vector 15 config */
};

struct pwr_ctrl {
	/* for SPM */
	uint32_t pcm_flags;
	uint32_t pcm_flags_cust;	/* can override pcm_flags */
	uint32_t pcm_flags_cust_set;	/* set bit of pcm_flags, after pcm_flags_cust */
	uint32_t pcm_flags_cust_clr;	/* clr bit of pcm_flags, after pcm_flags_cust */
	uint32_t pcm_flags1;
	uint32_t pcm_flags1_cust;	/* can override pcm_flags1 */
	uint32_t pcm_flags1_cust_set;	/* set bit of pcm_flags1, after pcm_flags1_cust */
	uint32_t pcm_flags1_cust_clr;	/* clr bit of pcm_flags1, after pcm_flags1_cust */
	uint32_t timer_val;		/* @ 1T 32K */
	uint32_t timer_val_cust;	/* @ 1T 32K, can override timer_val */
	uint32_t timer_val_ramp_en;		/* stress for dpidle */
	uint32_t timer_val_ramp_en_sec;	/* stress for suspend */
	uint32_t wake_src;
	uint32_t wake_src_cust;	/* can override wake_src */
	uint32_t wakelock_timer_val;
	uint8_t wdt_disable;		/* disable wdt in suspend */

	/* Auto-gen Start */

	/* SPM_AP_STANDBY_CON */
	uint8_t wfi_op;
	uint8_t mp0_cputop_idle_mask;
	uint8_t mp1_cputop_idle_mask;
	uint8_t mcusys_idle_mask;
	uint8_t mm_mask_b;
	uint8_t md_ddr_en_0_dbc_en;
	uint8_t md_ddr_en_1_dbc_en;
	uint8_t md_mask_b;
	uint8_t sspm_mask_b;
	uint8_t scp_mask_b;
	uint8_t srcclkeni_mask_b;
	uint8_t md_apsrc_1_sel;
	uint8_t md_apsrc_0_sel;
	uint8_t conn_ddr_en_dbc_en;
	uint8_t conn_mask_b;
	uint8_t conn_apsrc_sel;
	uint8_t conn_srcclkena_sel_mask;

	/* SPM_SRC_REQ */
	uint8_t spm_apsrc_req;
	uint8_t spm_f26m_req;
	uint8_t spm_infra_req;
	uint8_t spm_vrf18_req;
	uint8_t spm_ddren_req;
	uint8_t spm_rsv_src_req;
	uint8_t spm_ddren_2_req;
	uint8_t cpu_md_dvfs_sop_force_on;

	/* SPM_SRC_MASK */
	uint8_t csyspwreq_mask;
	uint8_t ccif0_md_event_mask_b;
	uint8_t ccif0_ap_event_mask_b;
	uint8_t ccif1_md_event_mask_b;
	uint8_t ccif1_ap_event_mask_b;
	uint8_t ccif2_md_event_mask_b;
	uint8_t ccif2_ap_event_mask_b;
	uint8_t ccif3_md_event_mask_b;
	uint8_t ccif3_ap_event_mask_b;
	uint8_t md_srcclkena_0_infra_mask_b;
	uint8_t md_srcclkena_1_infra_mask_b;
	uint8_t conn_srcclkena_infra_mask_b;
	uint8_t ufs_infra_req_mask_b;
	uint8_t srcclkeni_infra_mask_b;
	uint8_t md_apsrc_req_0_infra_mask_b;
	uint8_t md_apsrc_req_1_infra_mask_b;
	uint8_t conn_apsrcreq_infra_mask_b;
	uint8_t ufs_srcclkena_mask_b;
	uint8_t md_vrf18_req_0_mask_b;
	uint8_t md_vrf18_req_1_mask_b;
	uint8_t ufs_vrf18_req_mask_b;
	uint8_t gce_vrf18_req_mask_b;
	uint8_t conn_infra_req_mask_b;
	uint8_t gce_apsrc_req_mask_b;
	uint8_t disp0_apsrc_req_mask_b;
	uint8_t disp1_apsrc_req_mask_b;
	uint8_t mfg_req_mask_b;
	uint8_t vdec_req_mask_b;
	uint8_t mcu_apsrcreq_infra_mask_b;

	/* SPM_SRC2_MASK */
	uint8_t md_ddr_en_0_mask_b;
	uint8_t md_ddr_en_1_mask_b;
	uint8_t conn_ddr_en_mask_b;
	uint8_t ddren_md32_apsrc_req_mask_b;
	uint8_t ddren_scp_apsrc_req_mask_b;
	uint8_t disp0_ddren_mask_b;
	uint8_t disp1_ddren_mask_b;
	uint8_t gce_ddren_mask_b;
	uint8_t ddren_emi_self_refresh_ch0_mask_b;
	uint8_t ddren_emi_self_refresh_ch1_mask_b;
	uint8_t mcu_apsrc_req_mask_b;
	uint8_t mcu_ddren_mask_b;

	/* SPM_WAKEUP_EVENT_MASK */
	uint32_t spm_wakeup_event_mask;

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	uint32_t spm_wakeup_event_ext_mask;

	/* SPM_SRC3_MASK */
	uint8_t md_ddr_en_2_0_mask_b;
	uint8_t md_ddr_en_2_1_mask_b;
	uint8_t conn_ddr_en_2_mask_b;
	uint8_t ddren2_md32_apsrc_req_mask_b;
	uint8_t ddren2_scp_apsrc_req_mask_b;
	uint8_t disp0_ddren2_mask_b;
	uint8_t disp1_ddren2_mask_b;
	uint8_t gce_ddren2_mask_b;
	uint8_t ddren2_emi_self_refresh_ch0_mask_b;
	uint8_t ddren2_emi_self_refresh_ch1_mask_b;
	uint8_t mcu_ddren_2_mask_b;

	/* SPARE_SRC_REQ_MASK */
	uint8_t spare1_ddren_mask_b;
	uint8_t spare1_apsrc_req_mask_b;
	uint8_t spare1_vrf18_req_mask_b;
	uint8_t spare1_infra_req_mask_b;
	uint8_t spare1_srcclkena_mask_b;
	uint8_t spare1_ddren2_mask_b;
	uint8_t spare2_ddren_mask_b;
	uint8_t spare2_apsrc_req_mask_b;
	uint8_t spare2_vrf18_req_mask_b;
	uint8_t spare2_infra_req_mask_b;
	uint8_t spare2_srcclkena_mask_b;
	uint8_t spare2_ddren2_mask_b;

	/* MP0_CPU0_WFI_EN */
	uint8_t mp0_cpu0_wfi_en;

	/* MP0_CPU1_WFI_EN */
	uint8_t mp0_cpu1_wfi_en;

	/* MP0_CPU2_WFI_EN */
	uint8_t mp0_cpu2_wfi_en;

	/* MP0_CPU3_WFI_EN */
	uint8_t mp0_cpu3_wfi_en;

	/* MP1_CPU0_WFI_EN */
	uint8_t mp1_cpu0_wfi_en;

	/* MP1_CPU1_WFI_EN */
	uint8_t mp1_cpu1_wfi_en;

	/* MP1_CPU2_WFI_EN */
	uint8_t mp1_cpu2_wfi_en;

	/* MP1_CPU3_WFI_EN */
	uint8_t mp1_cpu3_wfi_en;

	/* idle control */
	uint32_t idle_switch;
	uint32_t by_swt;		/* block reason */
	uint32_t by_boot;		/* block reason */
	uint32_t by_cpu;		/* block reason */
	uint32_t by_clk;		/* block reason */
	uint32_t by_gpt;		/* block reason */
	uint32_t cpu_cnt[4];
	uint32_t infra1_idle_mask;
	uint32_t infra0_idle_mask;
	uint32_t infra2_idle_mask;
	uint32_t infra3_idle_mask;
	uint32_t infra4_idle_mask;
	uint32_t mmsys0_idle_mask;
	uint32_t mmsys1_idle_mask;
	uint32_t cam_idle_mask;		/* IMAGE */
	uint32_t mfg_idle_mask;
	uint32_t venc_idle_mask;
	uint32_t vdec_idle_mask;
	uint32_t apu_idle_mask;
	uint32_t log_en;

	/* Auto-gen End */
};

/* code gen by spm_pwr_ctrl_atf.pl, need struct pwr_ctrl */
enum pwr_ctrl_enum {
	PWR_PCM_FLAGS,
	PWR_PCM_FLAGS_CUST,
	PWR_PCM_FLAGS_CUST_SET,
	PWR_PCM_FLAGS_CUST_CLR,
	PWR_PCM_FLAGS1,
	PWR_PCM_FLAGS1_CUST,
	PWR_PCM_FLAGS1_CUST_SET,
	PWR_PCM_FLAGS1_CUST_CLR,
	PWR_TIMER_VAL,
	PWR_TIMER_VAL_CUST,
	PWR_TIMER_VAL_RAMP_EN,
	PWR_TIMER_VAL_RAMP_EN_SEC,
	PWR_WAKE_SRC,
	PWR_WAKE_SRC_CUST,
	PWR_WAKELOCK_TIMER_VAL,
	PWR_WDT_DISABLE,
	/* SPM_AP_STANDBY_CON */
	PWR_WFI_OP,
	PWR_MP0_CPUTOP_IDLE_MASK,
	PWR_MP1_CPUTOP_IDLE_MASK,
	PWR_MCUSYS_IDLE_MASK,
	PWR_MM_MASK_B,
	PWR_MD_DDR_EN_0_DBC_EN,
	PWR_MD_DDR_EN_1_DBC_EN,
	PWR_MD_MASK_B,
	PWR_SSPM_MASK_B,
	PWR_SCP_MASK_B,
	PWR_SRCCLKENI_MASK_B,
	PWR_MD_APSRC_1_SEL,
	PWR_MD_APSRC_0_SEL,
	PWR_CONN_DDR_EN_DBC_EN,
	PWR_CONN_MASK_B,
	PWR_CONN_APSRC_SEL,
	PWR_CONN_SRCCLKENA_SEL_MASK,
	/* SPM_SRC_REQ */
	PWR_SPM_APSRC_REQ,
	PWR_SPM_F26M_REQ,
	PWR_SPM_INFRA_REQ,
	PWR_SPM_VRF18_REQ,
	PWR_SPM_DDREN_REQ,
	PWR_SPM_RSV_SRC_REQ,
	PWR_SPM_DDREN_2_REQ,
	PWR_CPU_MD_DVFS_SOP_FORCE_ON,
	/* SPM_SRC_MASK */
	PWR_CSYSPWREQ_MASK,
	PWR_CCIF0_MD_EVENT_MASK_B,
	PWR_CCIF0_AP_EVENT_MASK_B,
	PWR_CCIF1_MD_EVENT_MASK_B,
	PWR_CCIF1_AP_EVENT_MASK_B,
	PWR_CCIF2_MD_EVENT_MASK_B,
	PWR_CCIF2_AP_EVENT_MASK_B,
	PWR_CCIF3_MD_EVENT_MASK_B,
	PWR_CCIF3_AP_EVENT_MASK_B,
	PWR_MD_SRCCLKENA_0_INFRA_MASK_B,
	PWR_MD_SRCCLKENA_1_INFRA_MASK_B,
	PWR_CONN_SRCCLKENA_INFRA_MASK_B,
	PWR_UFS_INFRA_REQ_MASK_B,
	PWR_SRCCLKENI_INFRA_MASK_B,
	PWR_MD_APSRC_REQ_0_INFRA_MASK_B,
	PWR_MD_APSRC_REQ_1_INFRA_MASK_B,
	PWR_CONN_APSRCREQ_INFRA_MASK_B,
	PWR_UFS_SRCCLKENA_MASK_B,
	PWR_MD_VRF18_REQ_0_MASK_B,
	PWR_MD_VRF18_REQ_1_MASK_B,
	PWR_UFS_VRF18_REQ_MASK_B,
	PWR_GCE_VRF18_REQ_MASK_B,
	PWR_CONN_INFRA_REQ_MASK_B,
	PWR_GCE_APSRC_REQ_MASK_B,
	PWR_DISP0_APSRC_REQ_MASK_B,
	PWR_DISP1_APSRC_REQ_MASK_B,
	PWR_MFG_REQ_MASK_B,
	PWR_VDEC_REQ_MASK_B,
	PWR_MCU_APSRCREQ_INFRA_MASK_B,
	/* SPM_SRC2_MASK */
	PWR_MD_DDR_EN_0_MASK_B,
	PWR_MD_DDR_EN_1_MASK_B,
	PWR_CONN_DDR_EN_MASK_B,
	PWR_DDREN_MD32_APSRC_REQ_MASK_B,
	PWR_DDREN_SCP_APSRC_REQ_MASK_B,
	PWR_DISP0_DDREN_MASK_B,
	PWR_DISP1_DDREN_MASK_B,
	PWR_GCE_DDREN_MASK_B,
	PWR_DDREN_EMI_SELF_REFRESH_CH0_MASK_B,
	PWR_DDREN_EMI_SELF_REFRESH_CH1_MASK_B,
	PWR_MCU_APSRC_REQ_MASK_B,
	PWR_MCU_DDREN_MASK_B,
	/* SPM_WAKEUP_EVENT_MASK */
	PWR_SPM_WAKEUP_EVENT_MASK,
	/* SPM_WAKEUP_EVENT_EXT_MASK */
	PWR_SPM_WAKEUP_EVENT_EXT_MASK,
	/* SPM_SRC3_MASK */
	PWR_MD_DDR_EN_2_0_MASK_B,
	PWR_MD_DDR_EN_2_1_MASK_B,
	PWR_CONN_DDR_EN_2_MASK_B,
	PWR_DDREN2_MD32_APSRC_REQ_MASK_B,
	PWR_DDREN2_SCP_APSRC_REQ_MASK_B,
	PWR_DISP0_DDREN2_MASK_B,
	PWR_DISP1_DDREN2_MASK_B,
	PWR_GCE_DDREN2_MASK_B,
	PWR_DDREN2_EMI_SELF_REFRESH_CH0_MASK_B,
	PWR_DDREN2_EMI_SELF_REFRESH_CH1_MASK_B,
	PWR_MCU_DDREN_2_MASK_B,
	/* SPARE_SRC_REQ_MASK */
	PWR_SPARE1_DDREN_MASK_B,
	PWR_SPARE1_APSRC_REQ_MASK_B,
	PWR_SPARE1_VRF18_REQ_MASK_B,
	PWR_SPARE1_INFRA_REQ_MASK_B,
	PWR_SPARE1_SRCCLKENA_MASK_B,
	PWR_SPARE1_DDREN2_MASK_B,
	PWR_SPARE2_DDREN_MASK_B,
	PWR_SPARE2_APSRC_REQ_MASK_B,
	PWR_SPARE2_VRF18_REQ_MASK_B,
	PWR_SPARE2_INFRA_REQ_MASK_B,
	PWR_SPARE2_SRCCLKENA_MASK_B,
	PWR_SPARE2_DDREN2_MASK_B,
	/* MP0_CPU0_WFI_EN */
	PWR_MP0_CPU0_WFI_EN,
	/* MP0_CPU1_WFI_EN */
	PWR_MP0_CPU1_WFI_EN,
	/* MP0_CPU2_WFI_EN */
	PWR_MP0_CPU2_WFI_EN,
	/* MP0_CPU3_WFI_EN */
	PWR_MP0_CPU3_WFI_EN,
	/* MP1_CPU0_WFI_EN */
	PWR_MP1_CPU0_WFI_EN,
	/* MP1_CPU1_WFI_EN */
	PWR_MP1_CPU1_WFI_EN,
	/* MP1_CPU2_WFI_EN */
	PWR_MP1_CPU2_WFI_EN,
	/* MP1_CPU3_WFI_EN */
	PWR_MP1_CPU3_WFI_EN,
	/* IDLE CONTROL */
	PWR_IDLE_SWITCH,
	PWR_BY_SWT,
	PWR_BY_BOOT,
	PWR_BY_CPU,
	PWR_BY_CLK,
	PWR_BY_GPT,
	PWR_CPU0_CNT,
	PWR_CPU1_CNT,
	PWR_CPU2_CNT,
	PWR_CPU3_CNT,
	PWR_INFRA1_IDLE_MASK,
	PWR_INFRA0_IDLE_MASK,
	PWR_INFRA2_IDLE_MASK,
	PWR_INFRA3_IDLE_MASK,
	PWR_INFRA4_IDLE_MASK,
	PWR_MMSYS0_IDLE_MASK,
	PWR_MMSYS1_IDLE_MASK,
	PWR_CAM_IDLE_MASK,
	PWR_MFG_IDLE_MASK,
	PWR_VENC_IDLE_MASK,
	PWR_VDEC_IDLE_MASK,
	PWR_APU_IDLE_MASK,
	PWR_INFRA1_IDLE_RG,
	PWR_INFRA0_IDLE_RG,
	PWR_INFRA2_IDLE_RG,
	PWR_INFRA3_IDLE_RG,
	PWR_INFRA4_IDLE_RG,
	PWR_MMSYS0_IDLE_RG,
	PWR_MMSYS1_IDLE_RG,
	PWR_CAM_IDLE_RG,
	PWR_MFG_IDLE_RG,
	PWR_VENC_IDLE_RG,
	PWR_VDEC_IDLE_RG,
	PWR_APU_IDLE_RG,
	PWR_LOG_EN,
	/* 26M MODE SELECT */
	PWR_PCM_ADD_FAKE_26M_FLAG,
	PWR_PCM_REMOVE_FAKE_26M_FLAG,
	PWR_PCM_ADD_FLAG1_DIS_TOP_26M_CLK_OFF,
	PWR_PCM_REMOVE_FLAG1_DIS_TOP_26M_CLK_OFF,
	PWR_PCM_ADD_FLAG1_SWITCH_TO_ULPLL,
	PWR_PCM_REMOVE_FLAG1_SWITCH_TO_ULPLL,

	PWR_MAX_COUNT,
};

enum {
	SPM_SUSPEND,
	SPM_RESUME,
	SPM_DPIDLE_ENTER,
	SPM_DPIDLE_LEAVE,
	SPM_ENTER_SODI,
	SPM_LEAVE_SODI,
	SPM_ENTER_SODI3,
	SPM_LEAVE_SODI3,
	SPM_SUSPEND_PREPARE,
	SPM_POST_SUSPEND,
	SPM_DPIDLE_PREPARE,
	SPM_POST_DPIDLE,
	SPM_SODI_PREPARE,
	SPM_POST_SODI,
	SPM_SODI3_PREPARE,
	SPM_POST_SODI3,
	SPM_VCORE_PWARP_CMD,
	SPM_PWR_CTRL_SUSPEND,
	SPM_PWR_CTRL_DPIDLE,
	SPM_PWR_CTRL_SODI,
	SPM_PWR_CTRL_SODI3,
	SPM_PWR_CTRL_VCOREFS,
};

#define DYNA_LOAD_PCM_PATH_SIZE 128
#define PCM_FIRMWARE_VERSION_SIZE 128

enum dyna_load_pcm_index {
	DYNA_LOAD_PCM_SUSPEND_LP4_3733 = 0,
	DYNA_LOAD_PCM_SUSPEND_LP4_3200,
	DYNA_LOAD_PCM_SUSPEND_LP3_1866,
	DYNA_LOAD_PCM_MAX,
};

struct dyna_load_pcm_t {
	char path[DYNA_LOAD_PCM_PATH_SIZE];
	char version[PCM_FIRMWARE_VERSION_SIZE];
	char *buf;
	struct pcm_desc desc;
	int ready;
};

extern struct dyna_load_pcm_t dyna_load_pcm[DYNA_LOAD_PCM_MAX];

struct wake_status {
	uint32_t assert_pc;          /* PCM_REG_DATA_INI */
	uint32_t r12;                /* PCM_REG12_DATA */
	uint32_t r12_ext;            /* PCM_REG12_EXT_DATA */
	uint32_t raw_sta;            /* SPM_WAKEUP_STA */
	uint32_t raw_ext_sta;        /* SPM_WAKEUP_EXT_STA */
	uint32_t wake_misc;          /* SPM_WAKEUP_MISC */
	uint32_t wake_event_mask;    /* SPM_WAKEUP_EVENT_MASK */
	uint32_t timer_out;          /* PCM_TIMER_OUT */
	uint32_t r13;                /* PCM_REG13_DATA */
	uint32_t idle_sta;           /* SUBSYS_IDLE_STA */
	uint32_t req_sta;            /* SRC_REQ_STA */
	uint32_t ddren_sta;          /* SRC_DDREN_STA */
	uint32_t debug_flag;         /* SPM_SW_DEBUG */
	uint32_t debug_flag1;        /* WDT_LATCH_SPARE0_FIX */
	uint32_t event_reg;          /* PCM_EVENT_REG_STA */
	uint32_t isr;                /* SPM_IRQ_STA */
	uint32_t rsv_6;              /* SPM_SW_RSV_6 */
	uint32_t log_index;
};

struct spm_lp_scen {
	struct pcm_desc *pcmdesc;
	struct pwr_ctrl *pwrctrl;
};

extern struct spm_lp_scen __spm_suspend;
extern struct spm_lp_scen __spm_vcorefs;
extern struct spm_lp_scen __spm_dpidle;
extern struct spm_lp_scen __spm_sodi;

extern uint32_t __spm_set_sysclk_settle(void);
extern int __spm_get_spmfw_idx(void);
extern int __spm_gpt_countdown_time(void);
extern int __spm_is_last_online_cpu(uint32_t cpu);
extern int __spm_is_idle_blocked_by_clk(struct pwr_ctrl *pwrctrl);
extern int __spm_does_system_boot_120s(void);
extern void __spm_clean_idle_block_cnt(struct pwr_ctrl *pwrctrl);
extern void __spm_set_cpu_status(int cpu);
extern void __spm_reset_and_init_pcm(const struct pcm_desc *pcmdesc);
extern void __spm_kick_im_to_fetch(const struct pcm_desc *pcmdesc);

extern void __spm_init_pcm_register(void);	/* init r0 and r7 */
extern void __spm_init_event_vector(const struct pcm_desc *pcmdesc);
extern void __spm_src_req_update(const struct pwr_ctrl *pwrctrl, unsigned int resource_usage);
extern void __spm_set_power_control(const struct pwr_ctrl *pwrctrl);
extern void __spm_set_wakeup_event(const struct pwr_ctrl *pwrctrl);
extern void __spm_kick_pcm_to_run(void);
extern void __spm_set_pcm_flags(struct pwr_ctrl *pwrctrl);
extern void __spm_send_cpu_wakeup_event(void);

extern void __spm_get_wakeup_status(struct wake_status *wakesta);
extern void __spm_clean_after_wakeup(void);
extern enum WAKE_REASON __spm_output_wake_reason(const struct wake_status *wakesta,
		const struct pcm_desc *pcmdesc);

extern void __spm_sync_vcore_dvfs_power_control(struct pwr_ctrl *dest_pwr_ctrl, const struct pwr_ctrl *src_pwr_ctrl);
extern void __spm_sync_mc_dsr_power_control(struct pwr_ctrl *dest_pwr_ctrl, const struct pwr_ctrl *src_pwr_ctrl);

extern void __spm_set_pcm_wdt(int en);
extern uint32_t _spm_get_wake_period(int pwake_time, enum WAKE_REASON last_wr);


static inline void set_pwrctrl_pcm_flags(struct pwr_ctrl *pwrctrl, uint32_t flags)
{
	if (pwrctrl->pcm_flags_cust == 0)
		pwrctrl->pcm_flags = flags;
	else
		pwrctrl->pcm_flags = pwrctrl->pcm_flags_cust;
}

static inline void set_pwrctrl_pcm_flags1(struct pwr_ctrl *pwrctrl, uint32_t flags)
{
	if (pwrctrl->pcm_flags1_cust == 0)
		pwrctrl->pcm_flags1 = flags;
	else
		pwrctrl->pcm_flags1 = pwrctrl->pcm_flags1_cust;
}

#endif /* __MT_SPM_INTERNAL__ */
