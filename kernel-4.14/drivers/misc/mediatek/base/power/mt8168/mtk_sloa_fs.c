/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/syscore_ops.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <mt-plat/mtk_secure_api.h>
#include <mt-plat/upmu_common.h>
#include <mtk_clkbuf_ctl.h>
#include "mtk_sloa_fs.h"
#include <pmic_api.h>

static void __iomem *scpsys_base;	/* 0x10006000 */

/* SMC call's marco */
#define SMC_CALL(_name, _arg0, _arg1, _arg2) \
		 mt_secure_call(MTK_SIP_KERNEL_SPM_##_name, \
				_arg0, _arg1, _arg2, 0)

#define BUF_SIZE	(PAGE_SIZE / 4)
#define UN_INIT		(100)

/* scpsys */
#define SPM_BASE			(scpsys_base)
#define PCM_REG_DATA_INI		(SPM_BASE + 0x028)
#define SPM_SWINT_CLR			(SPM_BASE + 0x094)
#define SPM_CPU_WAKEUP_EVENT		(SPM_BASE + 0x0B0)
#define SPM_IRQ_MASK			(SPM_BASE + 0x0B4)
#define SPM_WAKEUP_EVENT_MASK		(SPM_BASE + 0x0C4)
#define PCM_REG13_DATA			(SPM_BASE + 0x134)
#define PCM_REG12_EXT_DATA		(SPM_BASE + 0x144)
#define SPM_IRQ_STA			(SPM_BASE + 0x158)
#define SPM_WAKEUP_STA			(SPM_BASE + 0x15C)
#define SPM_WAKEUP_EXT_STA		(SPM_BASE + 0x160)
#define SUBSYS_IDLE_STA			(SPM_BASE + 0x170)
#define SRC_REQ_STA			(SPM_BASE + 0x17C)
#define PWR_STATUS			(SPM_BASE + 0x180)
#define PWR_STATUS_2ND			(SPM_BASE + 0x184)
#define SRC_DDREN_STA			(SPM_BASE + 0x1E0)
#define SPM_BSI_D0_SR			(SPM_BASE + 0x41C)
#define SPM_BSI_D1_SR			(SPM_BASE + 0x420)
#define SPM_BSI_D2_SR			(SPM_BASE + 0x424)
#define SPM_SW_DEBUG			(SPM_BASE + 0x604)
#define SPM_SW_RSV_0			(SPM_BASE + 0x608)
#define SPM_SW_RSV_6			(SPM_BASE + 0x64C)
#define SPM_SW_RSV_10			(SPM_BASE + 0x65C)
#define WDT_LATCH_SPARE0_FIX		(SPM_BASE + 0x780)

/* SPM Flag */
#define SPM_FLAG_DIS_INFRA_PDN		BIT(1)
#define SPM_FLAG_DIS_BUS_CLOCK_OFF	BIT(6)

/* PWR_STATUS_2ND */
#define PWR_STA_DISP			BIT(3)

/* SPM_SWINT */
#define PCM_SW_INT0		BIT(0)
#define PCM_SW_INT1		BIT(1)
#define PCM_SW_INT2		BIT(2)
#define PCM_SW_INT3		BIT(3)
#define PCM_SW_INT4		BIT(4)
#define PCM_SW_INT5		BIT(5)
#define PCM_SW_INT6		BIT(6)
#define PCM_SW_INT7		BIT(7)
#define PCM_SW_INT8		BIT(8)
#define PCM_SW_INT9		BIT(9)
#define PCM_SW_INT_ALL		(PCM_SW_INT9 | PCM_SW_INT8 | PCM_SW_INT7 | \
				 PCM_SW_INT6 | PCM_SW_INT5 | PCM_SW_INT4 | \
				 PCM_SW_INT3 | PCM_SW_INT2 | PCM_SW_INT1 | \
				 PCM_SW_INT0)
/* SPM_IRQ_MASK */
#define ISRM_PCM_RETURN		BIT(3)
#define ISRM_RET_IRQ0		BIT(8)
#define ISRM_RET_IRQ1		BIT(9)
#define ISRM_RET_IRQ2		BIT(10)
#define ISRM_RET_IRQ3		BIT(11)
#define ISRM_RET_IRQ4		BIT(12)
#define ISRM_RET_IRQ5		BIT(13)
#define ISRM_RET_IRQ6		BIT(14)
#define ISRM_RET_IRQ7		BIT(15)
#define ISRM_RET_IRQ8		BIT(16)
#define ISRM_RET_IRQ9		BIT(17)
#define ISRM_RET_IRQ_AUX	((ISRM_RET_IRQ9) | (ISRM_RET_IRQ8) | \
				 (ISRM_RET_IRQ7) | (ISRM_RET_IRQ6) | \
				 (ISRM_RET_IRQ5) | (ISRM_RET_IRQ4) | \
				 (ISRM_RET_IRQ3) | (ISRM_RET_IRQ2) | \
				 (ISRM_RET_IRQ1))
#define ISRM_ALL_EXC_TWAM	(ISRM_RET_IRQ_AUX)

/* SPM_IRQ_STA */
#define ISRS_PCM_RETURN		BIT(3)
#define ISRC_ALL_EXC_TWAM	ISRS_PCM_RETURN

/**************************************
 * Macro and Inline
 **************************************/
#define DEFINE_ATTR_RO(_name)			\
	static struct kobj_attribute _name##_attr = {	\
		.attr	= {				\
			.name = #_name,			\
			.mode = 0444,			\
		},					\
		.show	= _name##_show,			\
	}

#define DEFINE_ATTR_RW(_name)			\
	static struct kobj_attribute _name##_attr = {	\
		.attr	= {				\
			.name = #_name,			\
			.mode = 0644,			\
		},					\
		.show	= _name##_show,			\
		.store	= _name##_store,		\
	}

#define __ATTR_OF(_name)	(&_name##_attr.attr)

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

const char *cg_name[32] = {
	"INFRA1",
	"INFRA0",
	"INFRA2",
	"INFRA3",
	"INFRA4",
	"MMSYS0",
	"MMSYS1",
	"PWR_CAM",
	"PWR_MFG",
	"PWR_VENC",
	"PWR_VDEC",
	"PWR_APU",
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

enum pwr_ctrl_enum {
	PW_PCM_FLAGS,
	PW_PCM_FLAGS_CUST,
	PW_PCM_FLAGS_CUST_SET,
	PW_PCM_FLAGS_CUST_CLR,
	PW_PCM_FLAGS1,
	PW_PCM_FLAGS1_CUST,
	PW_PCM_FLAGS1_CUST_SET,
	PW_PCM_FLAGS1_CUST_CLR,
	PW_TIMER_VAL,
	PW_TIMER_VAL_CUST,
	PW_TIMER_VAL_RAMP_EN,
	PW_TIMER_VAL_RAMP_EN_SEC,
	PW_WAKE_SRC,
	PW_WAKE_SRC_CUST,
	PW_WAKELOCK_TIMER_VAL,
	PW_WDT_DISABLE,

	/* SPM_AP_STANDBY_CON */
	PW_WFI_OP,
	PW_MP0_CPUTOP_IDLE_MASK,
	PW_MP1_CPUTOP_IDLE_MASK,
	PW_MCUSYS_IDLE_MASK,
	PW_MM_MASK_B,
	PW_MD_DDR_EN_0_DBC_EN,
	PW_MD_DDR_EN_1_DBC_EN,
	PW_MD_MASK_B,
	PW_SSPM_MASK_B,
	PW_SCP_MASK_B,
	PW_SRCCLKENI_MASK_B,
	PW_MD_APSRC_1_SEL,
	PW_MD_APSRC_0_SEL,
	PW_CONN_DDR_EN_DBC_EN,
	PW_CONN_MASK_B,
	PW_CONN_APSRC_SEL,
	PW_CONN_SRCCLKENA_SEL_MASK,

	/* SPM_SRC_REQ */
	PW_SPM_APSRC_REQ,
	PW_SPM_F26M_REQ,
	PW_SPM_INFRA_REQ,
	PW_SPM_VRF18_REQ,
	PW_SPM_DDREN_REQ,
	PW_SPM_RSV_SRC_REQ,
	PW_SPM_DDREN_2_REQ,
	PW_CPU_MD_DVFS_SOP_FORCE_ON,

	/* SPM_SRC_MASK */
	PW_CSYSPWREQ_MASK,
	PW_CCIF0_MD_EVENT_MASK_B,
	PW_CCIF0_AP_EVENT_MASK_B,
	PW_CCIF1_MD_EVENT_MASK_B,
	PW_CCIF1_AP_EVENT_MASK_B,
	PW_CCIF2_MD_EVENT_MASK_B,
	PW_CCIF2_AP_EVENT_MASK_B,
	PW_CCIF3_MD_EVENT_MASK_B,
	PW_CCIF3_AP_EVENT_MASK_B,
	PW_MD_SRCCLKENA_0_INFRA_MASK_B,
	PW_MD_SRCCLKENA_1_INFRA_MASK_B,
	PW_CONN_SRCCLKENA_INFRA_MASK_B,
	PW_UFS_INFRA_REQ_MASK_B,
	PW_SRCCLKENI_INFRA_MASK_B,
	PW_MD_APSRC_REQ_0_INFRA_MASK_B,
	PW_MD_APSRC_REQ_1_INFRA_MASK_B,
	PW_CONN_APSRCREQ_INFRA_MASK_B,
	PW_UFS_SRCCLKENA_MASK_B,
	PW_MD_VRF18_REQ_0_MASK_B,
	PW_MD_VRF18_REQ_1_MASK_B,
	PW_UFS_VRF18_REQ_MASK_B,
	PW_GCE_VRF18_REQ_MASK_B,
	PW_CONN_INFRA_REQ_MASK_B,
	PW_GCE_APSRC_REQ_MASK_B,
	PW_DISP0_APSRC_REQ_MASK_B,
	PW_DISP1_APSRC_REQ_MASK_B,
	PW_MFG_REQ_MASK_B,
	PW_VDEC_REQ_MASK_B,
	PW_MCU_APSRCREQ_INFRA_MASK_B,

	/* SPM_SRC2_MASK */
	PW_MD_DDR_EN_0_MASK_B,
	PW_MD_DDR_EN_1_MASK_B,
	PW_CONN_DDR_EN_MASK_B,
	PW_DDREN_SSPM_APSRC_REQ_MASK_B,
	PW_DDREN_SCP_APSRC_REQ_MASK_B,
	PW_DISP0_DDREN_MASK_B,
	PW_DISP1_DDREN_MASK_B,
	PW_GCE_DDREN_MASK_B,
	PW_DDREN_EMI_SELF_REFRESH_CH0_MASK_B,
	PW_DDREN_EMI_SELF_REFRESH_CH1_MASK_B,
	PW_MCU_APSRC_REQ_MASK_B,
	PW_MCU_DDREN_MASK_B,

	/* SPM_WAKEUP_EVENT_MASK */
	PW_SPM_WAKEUP_EVENT_MASK,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	PW_SPM_WAKEUP_EVENT_EXT_MASK,

	/* SPM_SRC3_MASK */
	PW_MD_DDR_EN_2_0_MASK_B,
	PW_MD_DDR_EN_2_1_MASK_B,
	PW_CONN_DDR_EN_2_MASK_B,
	PW_DDREN2_SSPM_APSRC_REQ_MASK_B,
	PW_DDREN2_SCP_APSRC_REQ_MASK_B,
	PW_DISP0_DDREN2_MASK_B,
	PW_DISP1_DDREN2_MASK_B,
	PW_GCE_DDREN2_MASK_B,
	PW_DDREN2_EMI_SELF_REFRESH_CH0_MASK_B,
	PW_DDREN2_EMI_SELF_REFRESH_CH1_MASK_B,
	PW_MCU_DDREN_2_MASK_B,

	/* SPARE_SRC_REQ_MASK */
	PW_SPARE1_DDREN_MASK_B,
	PW_SPARE1_APSRC_REQ_MASK_B,
	PW_SPARE1_VRF18_REQ_MASK_B,
	PW_SPARE1_INFRA_REQ_MASK_B,
	PW_SPARE1_SRCCLKENA_MASK_B,
	PW_SPARE1_DDREN2_MASK_B,
	PW_SPARE2_DDREN_MASK_B,
	PW_SPARE2_APSRC_REQ_MASK_B,
	PW_SPARE2_VRF18_REQ_MASK_B,
	PW_SPARE2_INFRA_REQ_MASK_B,
	PW_SPARE2_SRCCLKENA_MASK_B,
	PW_SPARE2_DDREN2_MASK_B,

	/* MP0_CPU0_WFI_EN */
	PW_MP0_CPU0_WFI_EN,

	/* MP0_CPU1_WFI_EN */
	PW_MP0_CPU1_WFI_EN,

	/* MP0_CPU2_WFI_EN */
	PW_MP0_CPU2_WFI_EN,

	/* MP0_CPU3_WFI_EN */
	PW_MP0_CPU3_WFI_EN,

	/* MP1_CPU0_WFI_EN */
	PW_MP1_CPU0_WFI_EN,

	/* MP1_CPU1_WFI_EN */
	PW_MP1_CPU1_WFI_EN,

	/* MP1_CPU2_WFI_EN */
	PW_MP1_CPU2_WFI_EN,

	/* MP1_CPU3_WFI_EN */
	PW_MP1_CPU3_WFI_EN,

	/* IDLE CONTROL */
	PW_IDLE_SWITCH,
	PW_BY_SWT,
	PW_BY_BOOT,
	PW_BY_CPU,
	PW_BY_CLK,
	PW_BY_GPT,
	PW_CPU0_CNT,
	PW_CPU1_CNT,
	PW_CPU2_CNT,
	PW_CPU3_CNT,
	PW_INFRA1_IDLE_MASK,
	PW_INFRA0_IDLE_MASK,
	PW_INFRA2_IDLE_MASK,
	PW_INFRA3_IDLE_MASK,
	PW_INFRA4_IDLE_MASK,
	PW_MMSYS0_IDLE_MASK,
	PW_MMSYS1_IDLE_MASK,
	PW_CAM_IDLE_MASK,
	PW_MFG_IDLE_MASK,
	PW_VENC_IDLE_MASK,
	PW_VDEC_IDLE_MASK,
	PW_APU_IDLE_MASK,
	PW_INFRA1_IDLE_RG,
	PW_INFRA0_IDLE_RG,
	PW_INFRA2_IDLE_RG,
	PW_INFRA3_IDLE_RG,
	PW_INFRA4_IDLE_RG,
	PW_MMSYS0_IDLE_RG,
	PW_MMSYS1_IDLE_RG,
	PW_CAM_IDLE_RG,
	PW_MFG_IDLE_RG,
	PW_VENC_IDLE_RG,
	PW_VDEC_IDLE_RG,
	PW_APU_IDLE_RG,
	PW_LOG_EN,

	/* 26M MODE SELECT */
	PW_PCM_ADD_FAKE_26M_FLAG,
	PW_PCM_REMOVE_FAKE_26M_FLAG,
	PW_PCM_ADD_FLAG1_DIS_TOP_26M_CLK_OFF,
	PW_PCM_REMOVE_FLAG1_DIS_TOP_26M_CLK_OFF,
	PW_PCM_ADD_FLAG1_SWITCH_TO_ULPLL,
	PW_PCM_REMOVE_FLAG1_SWITCH_TO_ULPLL,

	PW_MAX_COUNT,
};

struct wake_status {
	u32 assert_pc;
	u32 r12;
	u32 r12_ext;
	u32 raw_sta;
	u32 raw_ext_sta;
	u32 wake_misc;
	u32 wake_event_mask;
	u32 timer_out;
	u32 r13;
	u32 idle_sta;
	u32 req_sta;
	u32 ddren_sta;
	u32 debug_flag;
	u32 debug_flag1;
	u32 event_reg;
	u32 isr;
	u32 rsv_6;
	u32 log_index;
};

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
	[23] = "R12_MSDC2_WAKEUP",
	[24] = "R12_CSYSPWREQ_B",
	[25] = "R12_DSPWDT_IRQ_B",
	[27] = "R12_SEJ",
};

struct pwr_ctrl {
	u32 pcm_flags;
	u32 pcm_flags_cust;
	u32 pcm_flags_cust_set;
	u32 pcm_flags_cust_clr;
	u32 pcm_flags1;
	u32 pcm_flags1_cust;
	u32 pcm_flags1_cust_set;
	u32 pcm_flags1_cust_clr;
	u32 timer_val;
	u32 timer_val_cust;
	u32 timer_val_ramp_en;
	u32 timer_val_ramp_en_sec;
	u32 wake_src;
	u32 wake_src_cust;
	u32 wakelock_timer_val;
	u8 wdt_disable;
	/* Auto-gen Start */

	/* SPM_AP_STANDBY_CON */
	u8 wfi_op;
	u8 mp0_cputop_idle_mask;
	u8 mp1_cputop_idle_mask;
	u8 mcusys_idle_mask;
	u8 mm_mask_b;
	u8 md_ddr_en_0_dbc_en;
	u8 md_ddr_en_1_dbc_en;
	u8 md_mask_b;
	u8 sspm_mask_b;
	u8 scp_mask_b;
	u8 srcclkeni_mask_b;
	u8 md_apsrc_1_sel;
	u8 md_apsrc_0_sel;
	u8 conn_ddr_en_dbc_en;
	u8 conn_mask_b;
	u8 conn_apsrc_sel;
	u8 conn_srcclkena_sel_mask;

	/* SPM_SRC_REQ */
	u8 spm_apsrc_req;
	u8 spm_f26m_req;
	u8 spm_infra_req;
	u8 spm_vrf18_req;
	u8 spm_ddren_req;
	u8 spm_rsv_src_req;
	u8 spm_ddren_2_req;
	u8 cpu_md_dvfs_sop_force_on;

	/* SPM_SRC_MASK */
	u8 csyspwreq_mask;
	u8 ccif0_md_event_mask_b;
	u8 ccif0_ap_event_mask_b;
	u8 ccif1_md_event_mask_b;
	u8 ccif1_ap_event_mask_b;
	u8 ccif2_md_event_mask_b;
	u8 ccif2_ap_event_mask_b;
	u8 ccif3_md_event_mask_b;
	u8 ccif3_ap_event_mask_b;
	u8 md_srcclkena_0_infra_mask_b;
	u8 md_srcclkena_1_infra_mask_b;
	u8 conn_srcclkena_infra_mask_b;
	u8 ufs_infra_req_mask_b;
	u8 srcclkeni_infra_mask_b;
	u8 md_apsrc_req_0_infra_mask_b;
	u8 md_apsrc_req_1_infra_mask_b;
	u8 conn_apsrcreq_infra_mask_b;
	u8 ufs_srcclkena_mask_b;
	u8 md_vrf18_req_0_mask_b;
	u8 md_vrf18_req_1_mask_b;
	u8 ufs_vrf18_req_mask_b;
	u8 gce_vrf18_req_mask_b;
	u8 conn_infra_req_mask_b;
	u8 gce_apsrc_req_mask_b;
	u8 disp0_apsrc_req_mask_b;
	u8 disp1_apsrc_req_mask_b;
	u8 mfg_req_mask_b;
	u8 vdec_req_mask_b;
	u8 mcu_apsrcreq_infra_mask_b;

	/* SPM_SRC2_MASK */
	u8 md_ddr_en_0_mask_b;
	u8 md_ddr_en_1_mask_b;
	u8 conn_ddr_en_mask_b;
	u8 ddren_sspm_apsrc_req_mask_b;
	u8 ddren_scp_apsrc_req_mask_b;
	u8 disp0_ddren_mask_b;
	u8 disp1_ddren_mask_b;
	u8 gce_ddren_mask_b;
	u8 ddren_emi_self_refresh_ch0_mask_b;
	u8 ddren_emi_self_refresh_ch1_mask_b;
	u8 mcu_apsrc_req_mask_b;
	u8 mcu_ddren_mask_b;

	/* SPM_WAKEUP_EVENT_MASK */
	u32 spm_wakeup_event_mask;

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	u32 spm_wakeup_event_ext_mask;

	/* SPM_SRC3_MASK */
	u8 md_ddr_en_2_0_mask_b;
	u8 md_ddr_en_2_1_mask_b;
	u8 conn_ddr_en_2_mask_b;
	u8 ddren2_sspm_apsrc_req_mask_b;
	u8 ddren2_scp_apsrc_req_mask_b;
	u8 disp0_ddren2_mask_b;
	u8 disp1_ddren2_mask_b;
	u8 gce_ddren2_mask_b;
	u8 ddren2_emi_self_refresh_ch0_mask_b;
	u8 ddren2_emi_self_refresh_ch1_mask_b;
	u8 mcu_ddren_2_mask_b;

	/* SPARE_SRC_REQ_MASK */
	u8 spare1_ddren_mask_b;
	u8 spare1_apsrc_req_mask_b;
	u8 spare1_vrf18_req_mask_b;
	u8 spare1_infra_req_mask_b;
	u8 spare1_srcclkena_mask_b;
	u8 spare1_ddren2_mask_b;
	u8 spare2_ddren_mask_b;
	u8 spare2_apsrc_req_mask_b;
	u8 spare2_vrf18_req_mask_b;
	u8 spare2_infra_req_mask_b;
	u8 spare2_srcclkena_mask_b;
	u8 spare2_ddren2_mask_b;

	/* MP0_CPU0_WFI_EN */
	u8 mp0_cpu0_wfi_en;

	/* MP0_CPU1_WFI_EN */
	u8 mp0_cpu1_wfi_en;

	/* MP0_CPU2_WFI_EN */
	u8 mp0_cpu2_wfi_en;

	/* MP0_CPU3_WFI_EN */
	u8 mp0_cpu3_wfi_en;

	/* MP1_CPU0_WFI_EN */
	u8 mp1_cpu0_wfi_en;

	/* MP1_CPU1_WFI_EN */
	u8 mp1_cpu1_wfi_en;

	/* MP1_CPU2_WFI_EN */
	u8 mp1_cpu2_wfi_en;

	/* MP1_CPU3_WFI_EN */
	u8 mp1_cpu3_wfi_en;

	/* Auto-gen End */
};

static char *pwr_ctrl_str[PW_MAX_COUNT] = {
	[PW_PCM_FLAGS] = "pcm_flags",
	[PW_PCM_FLAGS_CUST] = "pcm_flags_cust",
	[PW_PCM_FLAGS_CUST_SET] = "pcm_flags_cust_set",
	[PW_PCM_FLAGS_CUST_CLR] = "pcm_flags_cust_clr",
	[PW_PCM_FLAGS1] = "pcm_flags1",
	[PW_PCM_FLAGS1_CUST] = "pcm_flags1_cust",
	[PW_PCM_FLAGS1_CUST_SET] = "pcm_flags1_cust_set",
	[PW_PCM_FLAGS1_CUST_CLR] = "pcm_flags1_cust_clr",
	[PW_TIMER_VAL] = "timer_val",
	[PW_TIMER_VAL_CUST] = "timer_val_cust",
	[PW_TIMER_VAL_RAMP_EN] = "timer_val_ramp_en",
	[PW_TIMER_VAL_RAMP_EN_SEC] = "timer_val_ramp_en_sec",
	[PW_WAKE_SRC] = "wake_src",
	[PW_WAKE_SRC_CUST] = "wake_src_cust",
	[PW_WAKELOCK_TIMER_VAL] = "wakelock_timer_val",
	[PW_WDT_DISABLE] = "wdt_disable",

	/* SPM_AP_STANDBY_CON */
	[PW_WFI_OP] = "wfi_op",
	[PW_MP0_CPUTOP_IDLE_MASK] = "mp0_cputop_idle_mask",
	[PW_MP1_CPUTOP_IDLE_MASK] = "mp1_cputop_idle_mask",
	[PW_MCUSYS_IDLE_MASK] = "mcusys_idle_mask",
	[PW_MM_MASK_B] = "mm_mask_b",
	[PW_MD_DDR_EN_0_DBC_EN] = "md_ddr_en_0_dbc_en",
	[PW_MD_DDR_EN_1_DBC_EN] = "md_ddr_en_1_dbc_en",
	[PW_MD_MASK_B] = "md_mask_b",
	[PW_SSPM_MASK_B] = "sspm_mask_b",
	[PW_SCP_MASK_B] = "scp_mask_b",
	[PW_SRCCLKENI_MASK_B] = "srcclkeni_mask_b",
	[PW_MD_APSRC_1_SEL] = "md_apsrc_1_sel",
	[PW_MD_APSRC_0_SEL] = "md_apsrc_0_sel",
	[PW_CONN_DDR_EN_DBC_EN] = "conn_ddr_en_dbc_en",
	[PW_CONN_MASK_B] = "conn_mask_b",
	[PW_CONN_APSRC_SEL] = "conn_apsrc_sel",
	[PW_CONN_SRCCLKENA_SEL_MASK] = "conn_srcclkena_sel_mask",

	/* SPM_SRC_REQ */
	[PW_SPM_APSRC_REQ] = "spm_apsrc_req",
	[PW_SPM_F26M_REQ] = "spm_f26m_req",
	[PW_SPM_INFRA_REQ] = "spm_infra_req",
	[PW_SPM_VRF18_REQ] = "spm_vrf18_req",
	[PW_SPM_DDREN_REQ] = "spm_ddren_req",
	[PW_SPM_RSV_SRC_REQ] = "spm_rsv_src_req",
	[PW_SPM_DDREN_2_REQ] = "spm_ddren_2_req",
	[PW_CPU_MD_DVFS_SOP_FORCE_ON] = "cpu_md_dvfs_sop_force_on",

	/* SPM_SRC_MASK */
	[PW_CSYSPWREQ_MASK] = "csyspwreq_mask",
	[PW_CCIF0_MD_EVENT_MASK_B] = "ccif0_md_event_mask_b",
	[PW_CCIF0_AP_EVENT_MASK_B] = "ccif0_ap_event_mask_b",
	[PW_CCIF1_MD_EVENT_MASK_B] = "ccif1_md_event_mask_b",
	[PW_CCIF1_AP_EVENT_MASK_B] = "ccif1_ap_event_mask_b",
	[PW_CCIF2_MD_EVENT_MASK_B] = "ccif2_md_event_mask_b",
	[PW_CCIF2_AP_EVENT_MASK_B] = "ccif2_ap_event_mask_b",
	[PW_CCIF3_MD_EVENT_MASK_B] = "ccif3_md_event_mask_b",
	[PW_CCIF3_AP_EVENT_MASK_B] = "ccif3_ap_event_mask_b",
	[PW_MD_SRCCLKENA_0_INFRA_MASK_B] = "md_srcclkena_0_infra_mask_b",
	[PW_MD_SRCCLKENA_1_INFRA_MASK_B] = "md_srcclkena_1_infra_mask_b",
	[PW_CONN_SRCCLKENA_INFRA_MASK_B] = "conn_srcclkena_infra_mask_b",
	[PW_UFS_INFRA_REQ_MASK_B] = "ufs_infra_req_mask_b",
	[PW_SRCCLKENI_INFRA_MASK_B] = "srcclkeni_infra_mask_b",
	[PW_MD_APSRC_REQ_0_INFRA_MASK_B] = "md_apsrc_req_0_infra_mask_b",
	[PW_MD_APSRC_REQ_1_INFRA_MASK_B] = "md_apsrc_req_1_infra_mask_b",
	[PW_CONN_APSRCREQ_INFRA_MASK_B] = "conn_apsrcreq_infra_mask_b",
	[PW_UFS_SRCCLKENA_MASK_B] = "ufs_srcclkena_mask_b",
	[PW_MD_VRF18_REQ_0_MASK_B] = "md_vrf18_req_0_mask_b",
	[PW_MD_VRF18_REQ_1_MASK_B] = "md_vrf18_req_1_mask_b",
	[PW_UFS_VRF18_REQ_MASK_B] = "ufs_vrf18_req_mask_b",
	[PW_GCE_VRF18_REQ_MASK_B] = "gce_vrf18_req_mask_b",
	[PW_CONN_INFRA_REQ_MASK_B] = "conn_infra_req_mask_b",
	[PW_GCE_APSRC_REQ_MASK_B] = "gce_apsrc_req_mask_b",
	[PW_DISP0_APSRC_REQ_MASK_B] = "disp0_apsrc_req_mask_b",
	[PW_DISP1_APSRC_REQ_MASK_B] = "disp1_apsrc_req_mask_b",
	[PW_MFG_REQ_MASK_B] = "mfg_req_mask_b",
	[PW_VDEC_REQ_MASK_B] = "vdec_req_mask_b",
	[PW_MCU_APSRCREQ_INFRA_MASK_B] = "mcu_apsrcreq_infra_mask_b",

	/* SPM_SRC2_MASK */
	[PW_MD_DDR_EN_0_MASK_B] = "md_ddr_en_0_mask_b",
	[PW_MD_DDR_EN_1_MASK_B] = "md_ddr_en_1_mask_b",
	[PW_CONN_DDR_EN_MASK_B] = "conn_ddr_en_mask_b",
	[PW_DDREN_SSPM_APSRC_REQ_MASK_B] = "ddren_sspm_apsrc_req_mask_b",
	[PW_DDREN_SCP_APSRC_REQ_MASK_B] = "ddren_scp_apsrc_req_mask_b",
	[PW_DISP0_DDREN_MASK_B] = "disp0_ddren_mask_b",
	[PW_DISP1_DDREN_MASK_B] = "disp1_ddren_mask_b",
	[PW_GCE_DDREN_MASK_B] = "gce_ddren_mask_b",
	[PW_DDREN_EMI_SELF_REFRESH_CH0_MASK_B] =
		"ddren_emi_self_refresh_ch0_mask_b",
	[PW_DDREN_EMI_SELF_REFRESH_CH1_MASK_B] =
		"ddren_emi_self_refresh_ch1_mask_b",
	[PW_MCU_APSRC_REQ_MASK_B] = "mcu_apsrc_req_mask_b",
	[PW_MCU_DDREN_MASK_B] = "mcu_ddren_mask_b",

	/* SPM_WAKEUP_EVENT_MASK */
	[PW_SPM_WAKEUP_EVENT_MASK] = "spm_wakeup_event_mask",

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	[PW_SPM_WAKEUP_EVENT_EXT_MASK] = "spm_wakeup_event_ext_mask",

	/* SPM_SRC3_MASK */
	[PW_MD_DDR_EN_2_0_MASK_B] = "md_ddr_en_2_0_mask_b",
	[PW_MD_DDR_EN_2_1_MASK_B] = "md_ddr_en_2_1_mask_b",
	[PW_CONN_DDR_EN_2_MASK_B] = "conn_ddr_en_2_mask_b",
	[PW_DDREN2_SSPM_APSRC_REQ_MASK_B] = "ddren2_sspm_apsrc_req_mask_b",
	[PW_DDREN2_SCP_APSRC_REQ_MASK_B] = "ddren2_scp_apsrc_req_mask_b",
	[PW_DISP0_DDREN2_MASK_B] = "disp0_ddren2_mask_b",
	[PW_DISP1_DDREN2_MASK_B] = "disp1_ddren2_mask_b",
	[PW_GCE_DDREN2_MASK_B] = "gce_ddren2_mask_b",
	[PW_DDREN2_EMI_SELF_REFRESH_CH0_MASK_B] =
		"ddren2_emi_self_refresh_ch0_mask_b",
	[PW_DDREN2_EMI_SELF_REFRESH_CH1_MASK_B] =
		"ddren2_emi_self_refresh_ch1_mask_b",
	[PW_MCU_DDREN_2_MASK_B] = "mcu_ddren_2_mask_b",

	/* SPARE_SRC_REQ_MASK */
	[PW_SPARE1_DDREN_MASK_B] = "spare1_ddren_mask_b",
	[PW_SPARE1_APSRC_REQ_MASK_B] = "spare1_apsrc_req_mask_b",
	[PW_SPARE1_VRF18_REQ_MASK_B] = "spare1_vrf18_req_mask_b",
	[PW_SPARE1_INFRA_REQ_MASK_B] = "spare1_infra_req_mask_b",
	[PW_SPARE1_SRCCLKENA_MASK_B] = "spare1_srcclkena_mask_b",
	[PW_SPARE1_DDREN2_MASK_B] = "spare1_ddren2_mask_b",
	[PW_SPARE2_DDREN_MASK_B] = "spare2_ddren_mask_b",
	[PW_SPARE2_APSRC_REQ_MASK_B] = "spare2_apsrc_req_mask_b",
	[PW_SPARE2_VRF18_REQ_MASK_B] = "spare2_vrf18_req_mask_b",
	[PW_SPARE2_INFRA_REQ_MASK_B] = "spare2_infra_req_mask_b",
	[PW_SPARE2_SRCCLKENA_MASK_B] = "spare2_srcclkena_mask_b",
	[PW_SPARE2_DDREN2_MASK_B] = "spare2_ddren2_mask_b",

	/* MP0_CPU0_WFI_EN */
	[PW_MP0_CPU0_WFI_EN] = "mp0_cpu0_wfi_en",

	/* MP0_CPU1_WFI_EN */
	[PW_MP0_CPU1_WFI_EN] = "mp0_cpu1_wfi_en",

	/* MP0_CPU2_WFI_EN */
	[PW_MP0_CPU2_WFI_EN] = "mp0_cpu2_wfi_en",

	/* MP0_CPU3_WFI_EN */
	[PW_MP0_CPU3_WFI_EN] = "mp0_cpu3_wfi_en",

	/* MP1_CPU0_WFI_EN */
	[PW_MP1_CPU0_WFI_EN] = "mp1_cpu0_wfi_en",

	/* MP1_CPU1_WFI_EN */
	[PW_MP1_CPU1_WFI_EN] = "mp1_cpu1_wfi_en",

	/* MP1_CPU2_WFI_EN */
	[PW_MP1_CPU2_WFI_EN] = "mp1_cpu2_wfi_en",

	/* MP1_CPU3_WFI_EN */
	[PW_MP1_CPU3_WFI_EN] = "mp1_cpu3_wfi_en",
};

struct pwr_ctrl pwrctrl_sleep;
struct pwr_ctrl pwrctrl_dp;
struct pwr_ctrl pwrctrl_so3;
struct pwr_ctrl pwrctrl_so;
struct pwr_ctrl pwrctrl_dvfs;

/**************************************
 * xxx_ctrl_show Function
 **************************************/
/* code gen by spm_pwr_ctrl_atf.pl, need struct pwr_ctrl */
static ssize_t show_pwr_ctrl(int id, const struct pwr_ctrl *pwrctrl, char *buf)
{
	char *p = buf;

	p += sprintf(p, "pcm_flags = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_PCM_FLAGS, 0));
	p += sprintf(p, "pcm_flags_cust = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_PCM_FLAGS_CUST, 0));
	p += sprintf(p, "pcm_flags_cust_set = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id,
			      PW_PCM_FLAGS_CUST_SET, 0));
	p += sprintf(p, "pcm_flags_cust_clr = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_PCM_FLAGS_CUST_CLR, 0));
	p += sprintf(p, "pcm_flags1 = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_PCM_FLAGS1, 0));
	p += sprintf(p, "pcm_flags1_cust = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_PCM_FLAGS1_CUST, 0));
	p += sprintf(p, "pcm_flags1_cust_set = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_PCM_FLAGS1_CUST_SET, 0));
	p += sprintf(p, "pcm_flags1_cust_clr = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_PCM_FLAGS1_CUST_CLR, 0));
	p += sprintf(p, "timer_val = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_TIMER_VAL, 0));
	p += sprintf(p, "timer_val_cust = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_TIMER_VAL_CUST, 0));
	p += sprintf(p, "timer_val_ramp_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_TIMER_VAL_RAMP_EN, 0));
	p += sprintf(p, "timer_val_ramp_en_sec = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_TIMER_VAL_RAMP_EN_SEC, 0));
	p += sprintf(p, "wake_src = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_WAKE_SRC, 0));
	p += sprintf(p, "wake_src_cust = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_WAKE_SRC_CUST, 0));
	p += sprintf(p, "wakelock_timer_val = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_WAKELOCK_TIMER_VAL, 0));
	p += sprintf(p, "wdt_disable = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_WDT_DISABLE, 0));
	/* SPM_AP_STANDBY_CON */
	p += sprintf(p, "wfi_op = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_WFI_OP, 0));
	p += sprintf(p, "mp0_cputop_idle_mask = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MP0_CPUTOP_IDLE_MASK, 0));
	p += sprintf(p, "mp1_cputop_idle_mask = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MP1_CPUTOP_IDLE_MASK, 0));
	p += sprintf(p, "mcusys_idle_mask = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MCUSYS_IDLE_MASK, 0));
	p += sprintf(p, "mm_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MM_MASK_B, 0));
	p += sprintf(p, "md_ddr_en_0_dbc_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_DDR_EN_0_DBC_EN, 0));
	p += sprintf(p, "md_ddr_en_1_dbc_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_DDR_EN_1_DBC_EN, 0));
	p += sprintf(p, "md_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MD_MASK_B, 0));
	p += sprintf(p, "sspm_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SSPM_MASK_B, 0));
	p += sprintf(p, "scp_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SCP_MASK_B, 0));
	p += sprintf(p, "srcclkeni_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SRCCLKENI_MASK_B, 0));
	p += sprintf(p, "md_apsrc_1_sel = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MD_APSRC_1_SEL, 0));
	p += sprintf(p, "md_apsrc_0_sel = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MD_APSRC_0_SEL, 0));
	p += sprintf(p, "conn_ddr_en_dbc_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CONN_DDR_EN_DBC_EN, 0));
	p += sprintf(p, "conn_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_CONN_MASK_B, 0));
	p += sprintf(p, "conn_apsrc_sel = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_CONN_APSRC_SEL, 0));
	p += sprintf(p, "conn_srcclkena_sel_mask = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CONN_SRCCLKENA_SEL_MASK, 0));
	/* SPM_SRC_REQ */
	p += sprintf(p, "spm_apsrc_req = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SPM_APSRC_REQ, 0));
	p += sprintf(p, "spm_f26m_req = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SPM_F26M_REQ, 0));
	p += sprintf(p, "spm_infra_req = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SPM_INFRA_REQ, 0));
	p += sprintf(p, "spm_vrf18_req = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SPM_VRF18_REQ, 0));
	p += sprintf(p, "spm_ddren_req = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SPM_DDREN_REQ, 0));
	p += sprintf(p, "spm_rsv_src_req= 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SPM_RSV_SRC_REQ, 0));
	p += sprintf(p, "spm_ddren_2_req= 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_SPM_DDREN_2_REQ, 0));
	p += sprintf(p, "cpu_md_dvfs_sop_force_on= 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CPU_MD_DVFS_SOP_FORCE_ON, 0));
	/* SPM_SRC_MASK */
	p += sprintf(p, "csyspwreq_mask = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_CSYSPWREQ_MASK, 0));
	p += sprintf(p, "ccif0_md_event_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CCIF0_MD_EVENT_MASK_B, 0));
	p += sprintf(p, "ccif0_ap_event_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CCIF0_AP_EVENT_MASK_B, 0));
	p += sprintf(p, "ccif1_md_event_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CCIF1_MD_EVENT_MASK_B, 0));
	p += sprintf(p, "ccif1_ap_event_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CCIF1_AP_EVENT_MASK_B, 0));
	p += sprintf(p, "ccif2_md_event_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CCIF2_MD_EVENT_MASK_B, 0));
	p += sprintf(p, "ccif2_ap_event_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CCIF2_AP_EVENT_MASK_B, 0));
	p += sprintf(p, "ccif3_md_event_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CCIF3_MD_EVENT_MASK_B, 0));
	p += sprintf(p, "ccif3_ap_event_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CCIF3_AP_EVENT_MASK_B, 0));
	p += sprintf(p, "md_srcclkena_0_infra_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_SRCCLKENA_0_INFRA_MASK_B, 0));
	p += sprintf(p, "md_srcclkena_1_infra_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_SRCCLKENA_1_INFRA_MASK_B, 0));
	p += sprintf(p, "conn_srcclkena_infra_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CONN_SRCCLKENA_INFRA_MASK_B, 0));
	p += sprintf(p, "ufs_infra_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_UFS_INFRA_REQ_MASK_B, 0));
	p += sprintf(p, "srcclkeni_infra_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SRCCLKENI_INFRA_MASK_B, 0));
	p += sprintf(p, "md_apsrc_req_0_infra_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_APSRC_REQ_0_INFRA_MASK_B, 0));
	p += sprintf(p, "md_apsrc_req_1_infra_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_APSRC_REQ_1_INFRA_MASK_B, 0));
	p += sprintf(p, "conn_apsrcreq_infra_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CONN_APSRCREQ_INFRA_MASK_B, 0));
	p += sprintf(p, "ufs_srcclkena_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_UFS_SRCCLKENA_MASK_B, 0));
	p += sprintf(p, "md_vrf18_req_0_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_VRF18_REQ_0_MASK_B, 0));
	p += sprintf(p, "md_vrf18_req_1_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_VRF18_REQ_1_MASK_B, 0));
	p += sprintf(p, "ufs_vrf18_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_UFS_VRF18_REQ_MASK_B, 0));
	p += sprintf(p, "gce_vrf18_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_GCE_VRF18_REQ_MASK_B, 0));
	p += sprintf(p, "conn_infra_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CONN_INFRA_REQ_MASK_B, 0));
	p += sprintf(p, "gce_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_GCE_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "disp0_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DISP0_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "disp1_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DISP1_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "mfg_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MFG_REQ_MASK_B, 0));
	p += sprintf(p, "vdec_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_VDEC_REQ_MASK_B, 0));
	p += sprintf(p, "mcu_apsrcreq_infra_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MCU_APSRCREQ_INFRA_MASK_B, 0));
	/* SPM_SRC2_MASK */
	p += sprintf(p, "md_ddr_en_0_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_DDR_EN_0_MASK_B, 0));
	p += sprintf(p, "md_ddr_en_1_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_DDR_EN_1_MASK_B, 0));
	p += sprintf(p, "conn_ddr_en_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CONN_DDR_EN_MASK_B, 0));
	p += sprintf(p, "ddren_sspm_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DDREN_SSPM_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "ddren_scp_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DDREN_SCP_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "disp0_ddren_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DISP0_DDREN_MASK_B, 0));
	p += sprintf(p, "disp1_ddren_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DISP1_DDREN_MASK_B, 0));
	p += sprintf(p, "gce_ddren_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_GCE_DDREN_MASK_B, 0));
	p += sprintf(p, "ddren_emi_self_refresh_ch0_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DDREN_EMI_SELF_REFRESH_CH0_MASK_B, 0));
	p += sprintf(p, "ddren_emi_self_refresh_ch1_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DDREN_EMI_SELF_REFRESH_CH1_MASK_B, 0));
	p += sprintf(p, "mcu_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MCU_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "mcu_ddren_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MCU_DDREN_MASK_B, 0));
	/* SPM_WAKEUP_EVENT_MASK */
	p += sprintf(p, "spm_wakeup_event_mask = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPM_WAKEUP_EVENT_MASK, 0));
	/* SPM_WAKEUP_EVENT_EXT_MASK */
	p += sprintf(p, "spm_wakeup_event_ext_mask = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPM_WAKEUP_EVENT_EXT_MASK, 0));
	/* SPM_SRC3_MASK */
	p += sprintf(p, "md_ddr_en_2_0_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_DDR_EN_2_0_MASK_B, 0));
	p += sprintf(p, "md_ddr_en_2_1_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MD_DDR_EN_2_1_MASK_B, 0));
	p += sprintf(p, "conn_ddr_en_2_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_CONN_DDR_EN_2_MASK_B, 0));
	p += sprintf(p, "ddren2_sspm_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DDREN2_SSPM_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "ddren2_scp_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DDREN2_SCP_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "disp0_ddren2_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DISP0_DDREN2_MASK_B, 0));
	p += sprintf(p, "disp1_ddren2_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DISP1_DDREN2_MASK_B, 0));
	p += sprintf(p, "gce_ddren2_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_GCE_DDREN2_MASK_B, 0));
	p += sprintf(p, "ddren2_emi_self_refresh_ch0_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DDREN2_EMI_SELF_REFRESH_CH0_MASK_B, 0));
	p += sprintf(p, "ddren2_emi_self_refresh_ch1_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_DDREN2_EMI_SELF_REFRESH_CH1_MASK_B, 0));
	p += sprintf(p, "mcu_ddren_2_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_MCU_DDREN_2_MASK_B, 0));
	/* SPARE_SRC_REQ_MASK */
	p += sprintf(p, "spare1_ddren_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE1_DDREN_MASK_B, 0));
	p += sprintf(p, "spare1_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE1_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "spare1_vrf18_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE1_VRF18_REQ_MASK_B, 0));
	p += sprintf(p, "spare1_infra_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE1_INFRA_REQ_MASK_B, 0));
	p += sprintf(p, "spare1_srcclkena_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE1_SRCCLKENA_MASK_B, 0));
	p += sprintf(p, "spare1_ddren2_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE1_DDREN2_MASK_B, 0));
	p += sprintf(p, "spare2_ddren_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE2_DDREN_MASK_B, 0));
	p += sprintf(p, "spare2_apsrc_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE2_APSRC_REQ_MASK_B, 0));
	p += sprintf(p, "spare2_vrf18_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE2_VRF18_REQ_MASK_B, 0));
	p += sprintf(p, "spare2_infra_req_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE2_INFRA_REQ_MASK_B, 0));
	p += sprintf(p, "spare2_srcclkena_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE2_SRCCLKENA_MASK_B, 0));
	p += sprintf(p, "spare2_ddren2_mask_b = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS,
			      id, PW_SPARE2_DDREN2_MASK_B, 0));
	/* MP0_CPU0_WFI_EN */
	p += sprintf(p, "mp0_cpu0_wfi_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MP0_CPU1_WFI_EN, 0));
	/* MP0_CPU1_WFI_EN */
	p += sprintf(p, "mp0_cpu1_wfi_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MP0_CPU1_WFI_EN, 0));
	/* MP0_CPU2_WFI_EN */
	p += sprintf(p, "mp0_cpu2_wfi_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MP0_CPU2_WFI_EN, 0));
	/* MP0_CPU3_WFI_EN */
	p += sprintf(p, "mp0_cpu3_wfi_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MP0_CPU3_WFI_EN, 0));
	/* MP1_CPU0_WFI_EN */
	p += sprintf(p, "mp1_cpu0_wfi_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MP1_CPU0_WFI_EN, 0));
	/* MP1_CPU1_WFI_EN */
	p += sprintf(p, "mp1_cpu1_wfi_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MP1_CPU1_WFI_EN, 0));
	/* MP1_CPU2_WFI_EN */
	p += sprintf(p, "mp1_cpu2_wfi_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MP1_CPU2_WFI_EN, 0));
	/* MP0_CPU3_WFI_EN */
	p += sprintf(p, "mp1_cpu3_wfi_en = 0x%zx\n",
		     SMC_CALL(GET_PWR_CTRL_ARGS, id, PW_MP1_CPU3_WFI_EN, 0));

	WARN_ON(p - buf >= PAGE_SIZE);

	return p - buf;
}

static ssize_t suspend_ctrl_show(struct kobject *kobj,
				 struct kobj_attribute *attr, char *buf)
{
	return show_pwr_ctrl(SPM_PWR_CTRL_SUSPEND, &pwrctrl_sleep, buf);
}

static ssize_t dpidle_ctrl_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return show_pwr_ctrl(SPM_PWR_CTRL_DPIDLE, &pwrctrl_dp, buf);
}

static ssize_t sodi_ctrl_show(struct kobject *kobj,
			      struct kobj_attribute *attr, char *buf)
{
	return show_pwr_ctrl(SPM_PWR_CTRL_SODI, &pwrctrl_so, buf);
}

static ssize_t vcore_dvfs_ctrl_show(struct kobject *kobj,
				    struct kobj_attribute *attr, char *buf)
{
	return show_pwr_ctrl(SPM_PWR_CTRL_VCOREFS, &pwrctrl_dvfs, buf);
}

/**************************************
 * xxx_ctrl_store Function
 **************************************/
/* code gen by spm_pwr_ctrl_atf.pl, need struct pwr_ctrl */
static ssize_t store_pwr_ctrl(int id, struct pwr_ctrl *pwrctrl,
			      const char *buf, size_t count)
{
	u32 val;
	char cmd[64];

	if (sscanf(buf, "%63s %x", cmd, &val) != 2)
		return -EPERM;

	pr_info("[SPM] pwr_ctrl: cmd = %s, val = 0x%x\n", cmd, val);

	if (!strcmp(cmd, pwr_ctrl_str[PW_PCM_FLAGS])) {
		pwrctrl->pcm_flags = val;
		SMC_CALL(PWR_CTRL_ARGS, id, PW_PCM_FLAGS, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_PCM_FLAGS_CUST])) {
		pwrctrl->pcm_flags_cust = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_PCM_FLAGS_CUST, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_PCM_FLAGS_CUST_SET])) {
		pwrctrl->pcm_flags_cust_set = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_PCM_FLAGS_CUST_SET, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_PCM_FLAGS_CUST_CLR])) {
		pwrctrl->pcm_flags_cust_clr = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_PCM_FLAGS_CUST_CLR, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_PCM_FLAGS1])) {
		pwrctrl->pcm_flags1 = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_PCM_FLAGS1, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_PCM_FLAGS1_CUST])) {
		pwrctrl->pcm_flags1_cust = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_PCM_FLAGS1_CUST, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_PCM_FLAGS1_CUST_SET])) {
		pwrctrl->pcm_flags1_cust_set = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_PCM_FLAGS1_CUST_SET, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_PCM_FLAGS1_CUST_CLR])) {
		pwrctrl->pcm_flags1_cust_clr = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_PCM_FLAGS1_CUST_CLR, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_TIMER_VAL])) {
		pwrctrl->timer_val = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_TIMER_VAL, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_TIMER_VAL_CUST])) {
		pwrctrl->timer_val_cust = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_TIMER_VAL_CUST, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_TIMER_VAL_RAMP_EN])) {
		pwrctrl->timer_val_ramp_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_TIMER_VAL_RAMP_EN, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_TIMER_VAL_RAMP_EN_SEC])) {
		pwrctrl->timer_val_ramp_en_sec = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_TIMER_VAL_RAMP_EN_SEC, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_WAKE_SRC])) {
		pwrctrl->wake_src = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_WAKE_SRC, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_WAKE_SRC_CUST])) {
		pwrctrl->wake_src_cust = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_WAKE_SRC_CUST, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_WAKELOCK_TIMER_VAL])) {
		pwrctrl->wakelock_timer_val = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_WAKELOCK_TIMER_VAL, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_WDT_DISABLE])) {
		pwrctrl->wdt_disable = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_WDT_DISABLE, val);
	 /* SPM_AP_STANDBY_CON */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_WFI_OP])) {
		pwrctrl->wfi_op = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_WFI_OP, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP0_CPUTOP_IDLE_MASK])) {
		pwrctrl->mp0_cputop_idle_mask = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP0_CPUTOP_IDLE_MASK, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP1_CPUTOP_IDLE_MASK])) {
		pwrctrl->mp1_cputop_idle_mask = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP1_CPUTOP_IDLE_MASK, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MCUSYS_IDLE_MASK])) {
		pwrctrl->mcusys_idle_mask = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MCUSYS_IDLE_MASK, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MM_MASK_B])) {
		pwrctrl->mm_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MM_MASK_B, val);
	}  else if (!strcmp(cmd, pwr_ctrl_str[PW_MD_DDR_EN_0_DBC_EN])) {
		pwrctrl->md_ddr_en_0_dbc_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_DDR_EN_0_DBC_EN, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_DDR_EN_1_DBC_EN])) {
		pwrctrl->md_ddr_en_1_dbc_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_DDR_EN_1_DBC_EN, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_MASK_B])) {
		pwrctrl->md_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SSPM_MASK_B])) {
		pwrctrl->sspm_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SSPM_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SCP_MASK_B])) {
		pwrctrl->scp_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SCP_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SRCCLKENI_MASK_B])) {
		pwrctrl->srcclkeni_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SRCCLKENI_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_APSRC_1_SEL])) {
		pwrctrl->md_apsrc_1_sel = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_APSRC_1_SEL, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_APSRC_0_SEL])) {
		pwrctrl->md_apsrc_0_sel = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_APSRC_0_SEL, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CONN_DDR_EN_DBC_EN])) {
		pwrctrl->conn_ddr_en_dbc_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CONN_DDR_EN_DBC_EN, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CONN_MASK_B])) {
		pwrctrl->conn_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CONN_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CONN_APSRC_SEL])) {
		pwrctrl->conn_apsrc_sel = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CONN_APSRC_SEL, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CONN_SRCCLKENA_SEL_MASK])) {
		pwrctrl->conn_apsrc_sel = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CONN_SRCCLKENA_SEL_MASK, val);
	/* SPM_SRC_REQ */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_APSRC_REQ])) {
		pwrctrl->spm_apsrc_req = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_APSRC_REQ, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_F26M_REQ])) {
		pwrctrl->spm_f26m_req = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_F26M_REQ, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_INFRA_REQ])) {
		pwrctrl->spm_infra_req = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_INFRA_REQ, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_VRF18_REQ])) {
		pwrctrl->spm_vrf18_req = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_VRF18_REQ, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_DDREN_REQ])) {
		pwrctrl->spm_ddren_req = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_DDREN_REQ, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_RSV_SRC_REQ])) {
		pwrctrl->spm_rsv_src_req = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_RSV_SRC_REQ, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_DDREN_2_REQ])) {
		pwrctrl->spm_ddren_2_req = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_DDREN_2_REQ, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CPU_MD_DVFS_SOP_FORCE_ON])) {
		pwrctrl->cpu_md_dvfs_sop_force_on = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CPU_MD_DVFS_SOP_FORCE_ON, val);
	/* SPM_SRC_MASK */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CSYSPWREQ_MASK])) {
		pwrctrl->csyspwreq_mask = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CSYSPWREQ_MASK, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CCIF0_MD_EVENT_MASK_B])) {
		pwrctrl->ccif0_md_event_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CCIF0_MD_EVENT_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CCIF0_AP_EVENT_MASK_B])) {
		pwrctrl->ccif0_ap_event_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CCIF0_AP_EVENT_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CCIF1_MD_EVENT_MASK_B])) {
		pwrctrl->ccif1_md_event_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CCIF1_MD_EVENT_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CCIF1_AP_EVENT_MASK_B])) {
		pwrctrl->ccif1_ap_event_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CCIF1_AP_EVENT_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CCIF2_MD_EVENT_MASK_B])) {
		pwrctrl->ccif2_md_event_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CCIF2_MD_EVENT_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CCIF2_AP_EVENT_MASK_B])) {
		pwrctrl->ccif2_ap_event_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CCIF2_AP_EVENT_MASK_B, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_CCIF3_MD_EVENT_MASK_B])) {
		pwrctrl->ccif3_md_event_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CCIF3_MD_EVENT_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CCIF3_AP_EVENT_MASK_B])) {
		pwrctrl->ccif3_ap_event_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CCIF3_AP_EVENT_MASK_B, val);
	} else if (!strcmp(cmd,
			   pwr_ctrl_str[PW_MD_SRCCLKENA_0_INFRA_MASK_B])) {
		pwrctrl->md_srcclkena_0_infra_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id,
			 PW_MD_SRCCLKENA_0_INFRA_MASK_B, val);
	} else if (!strcmp(cmd,
			   pwr_ctrl_str[PW_MD_SRCCLKENA_1_INFRA_MASK_B])) {
		pwrctrl->md_srcclkena_1_infra_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_MD_SRCCLKENA_1_INFRA_MASK_B, val);
	} else if (!strcmp(cmd,
			   pwr_ctrl_str[PW_CONN_SRCCLKENA_INFRA_MASK_B])) {
		pwrctrl->conn_srcclkena_infra_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_CONN_SRCCLKENA_INFRA_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_UFS_INFRA_REQ_MASK_B])) {
		pwrctrl->ufs_infra_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_UFS_INFRA_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SRCCLKENI_INFRA_MASK_B])) {
		pwrctrl->srcclkeni_infra_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SRCCLKENI_INFRA_MASK_B, val);
	} else if (!strcmp(cmd,
			   pwr_ctrl_str[PW_MD_APSRC_REQ_0_INFRA_MASK_B])) {
		pwrctrl->md_apsrc_req_0_infra_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_MD_APSRC_REQ_0_INFRA_MASK_B, val);
	} else if (!strcmp(cmd,
			   pwr_ctrl_str[PW_MD_APSRC_REQ_1_INFRA_MASK_B])) {
		pwrctrl->md_apsrc_req_1_infra_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_MD_APSRC_REQ_1_INFRA_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CONN_APSRCREQ_INFRA_MASK_B])) {
		pwrctrl->conn_apsrcreq_infra_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_CONN_APSRCREQ_INFRA_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_UFS_SRCCLKENA_MASK_B])) {
		pwrctrl->ufs_srcclkena_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_UFS_SRCCLKENA_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_VRF18_REQ_0_MASK_B])) {
		pwrctrl->md_vrf18_req_0_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_VRF18_REQ_0_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_VRF18_REQ_1_MASK_B])) {
		pwrctrl->md_vrf18_req_1_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_VRF18_REQ_1_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_UFS_VRF18_REQ_MASK_B])) {
		pwrctrl->ufs_vrf18_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_UFS_VRF18_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_GCE_VRF18_REQ_MASK_B])) {
		pwrctrl->gce_vrf18_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_GCE_VRF18_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CONN_INFRA_REQ_MASK_B])) {
		pwrctrl->conn_infra_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CONN_INFRA_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_GCE_APSRC_REQ_MASK_B])) {
		pwrctrl->gce_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_GCE_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_DISP0_APSRC_REQ_MASK_B])) {
		pwrctrl->disp0_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_DISP0_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_DISP1_APSRC_REQ_MASK_B])) {
		pwrctrl->disp1_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_DISP1_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_MFG_REQ_MASK_B])) {
		pwrctrl->mfg_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MFG_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_VDEC_REQ_MASK_B])) {
		pwrctrl->vdec_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_VDEC_REQ_MASK_B, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_MCU_APSRCREQ_INFRA_MASK_B])) {
		pwrctrl->vdec_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MCU_APSRCREQ_INFRA_MASK_B, val);
	/* SPM_SRC2_MASK */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_DDR_EN_0_MASK_B])) {
		pwrctrl->md_ddr_en_0_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_DDR_EN_0_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_DDR_EN_1_MASK_B])) {
		pwrctrl->md_ddr_en_1_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_DDR_EN_1_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CONN_DDR_EN_MASK_B])) {
		pwrctrl->conn_ddr_en_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CONN_DDR_EN_MASK_B, val);
	} else if (!strcmp(cmd,
			   pwr_ctrl_str[PW_DDREN_SSPM_APSRC_REQ_MASK_B])) {
		pwrctrl->ddren_sspm_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_DDREN_SSPM_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_DDREN_SCP_APSRC_REQ_MASK_B])) {
		pwrctrl->ddren_scp_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_DDREN_SCP_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_DISP0_DDREN_MASK_B])) {
		pwrctrl->disp0_ddren_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_DISP0_DDREN_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_DISP1_DDREN_MASK_B])) {
		pwrctrl->disp1_ddren_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_DISP1_DDREN_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_GCE_DDREN_MASK_B])) {
		pwrctrl->gce_ddren_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_GCE_DDREN_MASK_B, val);
	} else if (!strcmp(cmd,
			pwr_ctrl_str[PW_DDREN_EMI_SELF_REFRESH_CH0_MASK_B])) {
		pwrctrl->ddren_emi_self_refresh_ch0_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_DDREN_EMI_SELF_REFRESH_CH0_MASK_B, val);
	} else if (!strcmp(cmd,
			pwr_ctrl_str[PW_DDREN_EMI_SELF_REFRESH_CH1_MASK_B])) {
		pwrctrl->ddren_emi_self_refresh_ch1_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_DDREN_EMI_SELF_REFRESH_CH1_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MCU_APSRC_REQ_MASK_B])) {
		pwrctrl->ddren_emi_self_refresh_ch0_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MCU_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MCU_DDREN_MASK_B])) {
		pwrctrl->ddren_emi_self_refresh_ch1_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MCU_DDREN_MASK_B, val);
	/* SPM_WAKEUP_EVENT_MASK */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_WAKEUP_EVENT_MASK])) {
		pwrctrl->spm_wakeup_event_mask = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_WAKEUP_EVENT_MASK, val);
	/* SPM_WAKEUP_EVENT_EXT_MASK */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPM_WAKEUP_EVENT_EXT_MASK])) {
		pwrctrl->spm_wakeup_event_ext_mask = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPM_WAKEUP_EVENT_EXT_MASK, val);
	/* SPM_SRC3_MASK */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_DDR_EN_2_0_MASK_B])) {
		pwrctrl->md_ddr_en_2_0_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_DDR_EN_2_0_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MD_DDR_EN_2_1_MASK_B])) {
		pwrctrl->md_ddr_en_2_1_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MD_DDR_EN_2_1_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_CONN_DDR_EN_2_MASK_B])) {
		pwrctrl->conn_ddr_en_2_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_CONN_DDR_EN_2_MASK_B, val);
	} else if (!strcmp(cmd,
			   pwr_ctrl_str[PW_DDREN2_SSPM_APSRC_REQ_MASK_B])) {
		pwrctrl->ddren2_sspm_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_DDREN2_SSPM_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd,
			   pwr_ctrl_str[PW_DDREN2_SCP_APSRC_REQ_MASK_B])) {
		pwrctrl->ddren2_scp_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_DDREN2_SCP_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_DISP0_DDREN2_MASK_B])) {
		pwrctrl->disp0_ddren2_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS, id, PW_DISP0_DDREN2_MASK_B, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_DISP1_DDREN2_MASK_B])) {
		pwrctrl->disp1_ddren2_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_DISP1_DDREN2_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_GCE_DDREN2_MASK_B])) {
		pwrctrl->gce_ddren2_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_GCE_DDREN2_MASK_B, val);
	} else if (!strcmp(cmd,
			pwr_ctrl_str[PW_DDREN2_EMI_SELF_REFRESH_CH0_MASK_B])) {
		pwrctrl->ddren2_emi_self_refresh_ch0_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_DDREN2_EMI_SELF_REFRESH_CH0_MASK_B, val);
	} else if (!strcmp(cmd,
			pwr_ctrl_str[PW_DDREN2_EMI_SELF_REFRESH_CH1_MASK_B])) {
		pwrctrl->ddren2_emi_self_refresh_ch1_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,
			 id, PW_DDREN2_EMI_SELF_REFRESH_CH1_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MCU_DDREN_2_MASK_B])) {
		pwrctrl->ddren2_emi_self_refresh_ch1_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MCU_DDREN_2_MASK_B, val);
	/* SPARE_SRC_REQ_MASK */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE1_DDREN_MASK_B])) {
		pwrctrl->spare1_ddren_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE1_DDREN_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE1_APSRC_REQ_MASK_B])) {
		pwrctrl->spare1_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE1_APSRC_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE1_VRF18_REQ_MASK_B])) {
		pwrctrl->spare1_vrf18_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE1_VRF18_REQ_MASK_B,	val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE1_INFRA_REQ_MASK_B])) {
		pwrctrl->spare1_infra_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE1_INFRA_REQ_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE1_SRCCLKENA_MASK_B])) {
		pwrctrl->spare1_srcclkena_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE1_SRCCLKENA_MASK_B, val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE1_DDREN2_MASK_B])) {
		pwrctrl->spare1_ddren2_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE1_DDREN2_MASK_B, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_SPARE2_DDREN_MASK_B])) {
		pwrctrl->spare2_ddren_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE2_DDREN_MASK_B, val);
	} else if (!strcmp(cmd, pwr_ctrl_str[PW_SPARE2_APSRC_REQ_MASK_B])) {
		pwrctrl->spare2_apsrc_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE2_APSRC_REQ_MASK_B,	val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE2_VRF18_REQ_MASK_B])) {
		pwrctrl->spare2_vrf18_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE2_VRF18_REQ_MASK_B,	val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE2_INFRA_REQ_MASK_B])) {
		pwrctrl->spare2_infra_req_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE2_INFRA_REQ_MASK_B,	val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE2_SRCCLKENA_MASK_B])) {
		pwrctrl->spare2_srcclkena_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE2_SRCCLKENA_MASK_B,	val);
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_SPARE2_DDREN2_MASK_B])) {
		pwrctrl->spare2_ddren2_mask_b = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_SPARE2_DDREN2_MASK_B, val);
	/* MP0_CPU0_WFI_EN */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP0_CPU0_WFI_EN])) {
		pwrctrl->mp0_cpu0_wfi_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP0_CPU0_WFI_EN, val);
	/* MP0_CPU1_WFI_EN */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP0_CPU1_WFI_EN])) {
		pwrctrl->mp0_cpu1_wfi_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP0_CPU1_WFI_EN, val);
	/* MP0_CPU2_WFI_EN */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP0_CPU2_WFI_EN])) {
		pwrctrl->mp0_cpu2_wfi_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP0_CPU2_WFI_EN, val);
	/* MP0_CPU3_WFI_EN */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP0_CPU3_WFI_EN])) {
		pwrctrl->mp0_cpu3_wfi_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP0_CPU3_WFI_EN, val);
	/* MP1_CPU0_WFI_EN */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP1_CPU0_WFI_EN])) {
		pwrctrl->mp1_cpu0_wfi_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP1_CPU0_WFI_EN, val);
	/* MP1_CPU1_WFI_EN */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP1_CPU1_WFI_EN])) {
		pwrctrl->mp1_cpu1_wfi_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP1_CPU1_WFI_EN, val);
	/* MP1_CPU2_WFI_EN */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP1_CPU2_WFI_EN])) {
		pwrctrl->mp1_cpu2_wfi_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP1_CPU2_WFI_EN, val);
	/* MP1_CPU3_WFI_EN */
	} else if (!strcmp(cmd,	pwr_ctrl_str[PW_MP1_CPU3_WFI_EN])) {
		pwrctrl->mp1_cpu3_wfi_en = val;
		SMC_CALL(PWR_CTRL_ARGS,	id, PW_MP1_CPU3_WFI_EN, val);
	}

	return count;
}

static ssize_t suspend_ctrl_store(struct kobject *kobj,
				  struct kobj_attribute *attr,
				  const char *buf, size_t count)
{
	return store_pwr_ctrl(SPM_PWR_CTRL_SUSPEND, &pwrctrl_sleep,
		buf, count);
}

static ssize_t dpidle_ctrl_store(struct kobject *kobj,
				 struct kobj_attribute *attr,
				 const char *buf, size_t count)
{
	return store_pwr_ctrl(SPM_PWR_CTRL_DPIDLE, &pwrctrl_dp, buf, count);
}

static ssize_t sodi_ctrl_store(struct kobject *kobj,
			       struct kobj_attribute *attr,
			       const char *buf, size_t count)
{
	return store_pwr_ctrl(SPM_PWR_CTRL_SODI, &pwrctrl_so, buf, count);
}

static ssize_t vcore_dvfs_ctrl_store(struct kobject *kobj,
				     struct kobj_attribute *attr,
				     const char *buf, size_t count)
{
	return store_pwr_ctrl(SPM_PWR_CTRL_VCOREFS,
		&pwrctrl_dvfs, buf, count);
}

/**************************************
 * Init Function
 **************************************/
DEFINE_ATTR_RW(suspend_ctrl);
DEFINE_ATTR_RW(dpidle_ctrl);
DEFINE_ATTR_RW(sodi_ctrl);
DEFINE_ATTR_RW(vcore_dvfs_ctrl);

static struct attribute *spm_attrs[] = {
	/* for spm_lp_scen.pwrctrl */
	__ATTR_OF(suspend_ctrl),
	__ATTR_OF(dpidle_ctrl),
	__ATTR_OF(sodi_ctrl),
	__ATTR_OF(vcore_dvfs_ctrl),

	/* must */
	NULL,
};

static struct attribute_group spm_attr_group = {
	.name = "spm",
	.attrs = spm_attrs,
};

void sloa_suspend_infra_power(bool on)
{
	u32 pcm_flags;

	if (on) {
		pr_notice("suspend infra power on\n");
		pcm_flags = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
				     PW_PCM_FLAGS, 0) | SPM_FLAG_DIS_INFRA_PDN;
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_FLAGS, pcm_flags);
	} else {
		pr_notice("suspend infra power off\n");
		pcm_flags = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
				     PW_PCM_FLAGS, 0) & ~SPM_FLAG_DIS_INFRA_PDN;
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_FLAGS, pcm_flags);
	}
}
EXPORT_SYMBOL(sloa_suspend_infra_power);

int sloa_suspend_26m_mode(enum clk_26m mode)
{
	enum PMU_FLAGS_LIST vcore_index, vcore_sram_index;
	static u32 vcore = UN_INIT, vcore_sram = UN_INIT, pcm_flags;
	unsigned int pmic_id;

	vcore_index = PMIC_RG_VCORE_SLEEP_VOLTAGE;
	vcore_sram_index = PMIC_RG_VSRAM_PROC_SLEEP_VOLTAGE;

	if (vcore == UN_INIT && vcore_sram == UN_INIT) {
		vcore = pmic_get_register_value(vcore_index);
		vcore_sram = pmic_get_register_value(vcore_sram_index);
	}

	/* set Vcore sleep voltage by pmic id */
	pmic_id = pmic_get_register_value(PMIC_SWCID);
	if (pmic_id == PMIC_MT6390_CHIP_ID)
		pmic_set_register_value(vcore_index, 0x3);	/* 600mV */
	else
		pmic_set_register_value(vcore_index, 0x2);	/* 625mV */

	pr_debug("pmic id = 0x%x\n", pmic_id);

	if (mode == FAKE_DCXO_26M) {
		clk_buf_mode_set(CLK_BUF_BB_MD, 1);		/* SW mode */
		pmic_set_register_value(vcore_sram_index, 0x6);	/* 800mV */
		pcm_flags = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
				     PW_PCM_FLAGS, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_FLAGS, pcm_flags | SPM_FLAG_DIS_BUS_CLOCK_OFF);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_ADD_FAKE_26M_FLAG, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_REMOVE_FLAG1_DIS_TOP_26M_CLK_OFF, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_REMOVE_FLAG1_SWITCH_TO_ULPLL, 0);
	} else if (mode == ULPLL_26M) {
		clk_buf_mode_set(CLK_BUF_BB_MD, 0);		/* HW mode */
		pmic_set_register_value(vcore_sram_index, 0x6);	/* 800mV */
		pcm_flags = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
				     PW_PCM_FLAGS, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_FLAGS, pcm_flags | SPM_FLAG_DIS_BUS_CLOCK_OFF);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_REMOVE_FAKE_26M_FLAG, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_ADD_FLAG1_DIS_TOP_26M_CLK_OFF, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_ADD_FLAG1_SWITCH_TO_ULPLL, 0);
	} else {
		clk_buf_mode_set(CLK_BUF_BB_MD, 0);		/* HW mode */
		pmic_set_register_value(vcore_index, vcore);	/* 550mV */
		pmic_set_register_value(vcore_sram_index,	/* 600mV */
					vcore_sram);
		pcm_flags = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
				     PW_PCM_FLAGS, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_FLAGS, pcm_flags &
			 ~SPM_FLAG_DIS_BUS_CLOCK_OFF);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_REMOVE_FAKE_26M_FLAG, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_REMOVE_FLAG1_DIS_TOP_26M_CLK_OFF, 0);
		SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_PCM_REMOVE_FLAG1_SWITCH_TO_ULPLL, 0);
	}

	return 0;
}
EXPORT_SYMBOL(sloa_suspend_26m_mode);

static int sloa_syscore_suspend(void)
{
	u32 sec, wakesrc, pcm_flags, pcm_flags1, req = 0;

	sec = (SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			PW_TIMER_VAL, 0)) / 32768;
	wakesrc = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			   PW_WAKE_SRC, 0);
	pcm_flags = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			     PW_PCM_FLAGS, 0);
	pcm_flags1 = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			      PW_PCM_FLAGS1, 0);
	req |= (SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_SPM_APSRC_REQ, 0)) << 0;
	req |= (SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_SPM_F26M_REQ, 0)) << 1;
	req |= (SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_SPM_INFRA_REQ, 0)) << 3;
	req |= (SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_SPM_VRF18_REQ, 0)) << 4;
	req |= (SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SUSPEND,
			 PW_SPM_DDREN_REQ, 0)) << 7;
	pr_notice("sec = %u, wakesrc = 0x%x, sw = 0x%x 0x%x, req = 0x%x\n",
		  sec, wakesrc, pcm_flags, pcm_flags1, req);
	pr_notice("pwr = 0x%x, pwr_2nd = 0x%x, sw_rsv_10 = 0x%x\n",
		  readl(PWR_STATUS), readl(PWR_STATUS_2ND),
		  readl(SPM_SW_RSV_10));

	return 0;
}

static void spm_output_wake_reason(const struct wake_status *wakesta)
{
	int i;

	/* assert_pc shows 0 when normal */
	for (i = 0; i <= 31; i++) {
		if (wakesta->r12 & (1U << i)) {
			pr_notice("wake up by %s, assert %u, timeout = %u\n",
				  wakeup_src_str[i], wakesta->assert_pc,
				  wakesta->timer_out);
			break;
		} else if (wakesta->r12 == 0) {
			pr_notice("r12 = 0?, assert %u, timeout = %u\n",
				  wakesta->assert_pc, wakesta->timer_out);
			break;
		}
	}

	pr_notice("debug = 0x%x 0x%x, r12 = 0x%x 0x%x, raw_sta = 0x%x 0x%x\n",
		  wakesta->debug_flag, wakesta->debug_flag1,
		  wakesta->r12, wakesta->r12_ext,
		  wakesta->raw_sta, wakesta->raw_ext_sta);
	pr_notice("req_sta = 0x%x 0x%x 0x%x 0x%x, wake_misc = 0x%x 0x%x\n",
		  wakesta->req_sta, wakesta->idle_sta,
		  wakesta->event_reg, wakesta->isr,
		  wakesta->wake_misc, wakesta->wake_event_mask);
	pr_notice("ddren_sta = 0x%x, r13 = 0x%x, rsv_6 = 0x%x\n",
		  wakesta->ddren_sta, wakesta->r13, wakesta->rsv_6);
}

static void spm_get_wakeup_status(struct wake_status *wakesta)
{
	wakesta->assert_pc = readl(PCM_REG_DATA_INI);
	wakesta->r12 = readl(SPM_SW_RSV_0);
	wakesta->r12_ext = readl(PCM_REG12_EXT_DATA);
	wakesta->raw_sta = readl(SPM_WAKEUP_STA);
	wakesta->raw_ext_sta = readl(SPM_WAKEUP_EXT_STA);
	wakesta->wake_misc = readl(SPM_BSI_D0_SR);
	wakesta->wake_event_mask = readl(SPM_WAKEUP_EVENT_MASK);
	wakesta->timer_out = readl(SPM_BSI_D1_SR);
	wakesta->r13 = readl(PCM_REG13_DATA);
	wakesta->idle_sta = readl(SUBSYS_IDLE_STA);
	wakesta->req_sta = readl(SRC_REQ_STA);
	wakesta->debug_flag = readl(SPM_SW_DEBUG);
	wakesta->debug_flag1 = readl(WDT_LATCH_SPARE0_FIX);
	wakesta->event_reg = readl(SPM_BSI_D2_SR);
	wakesta->isr = readl(SPM_IRQ_STA);
	wakesta->ddren_sta = readl(SRC_DDREN_STA);
	wakesta->rsv_6 = readl(SPM_SW_RSV_6);
}

static void spm_clean_after_wakeup(void)
{
	writel(readl(SPM_WAKEUP_STA) | readl(SPM_SW_RSV_0), SPM_SW_RSV_0);
	writel(0, SPM_CPU_WAKEUP_EVENT);
	writel(~0, SPM_WAKEUP_EVENT_MASK);
	writel(readl(SPM_IRQ_MASK) | ISRM_ALL_EXC_TWAM, SPM_IRQ_MASK);
	writel(ISRC_ALL_EXC_TWAM, SPM_IRQ_STA);
	writel(PCM_SW_INT_ALL, SPM_SWINT_CLR);
}

static void sloa_syscore_resume(void)
{
	struct wake_status spm_wakesta;

	spm_get_wakeup_status(&spm_wakesta);
	spm_clean_after_wakeup();
	spm_output_wake_reason(&spm_wakesta);
}

static struct syscore_ops sloa_syscore_ops = {
	.suspend = sloa_syscore_suspend,
	.resume = sloa_syscore_resume,
};

/*
 * idle_state
 */
static int _idle_state_open(struct seq_file *s, void *data)
{
	return 0;
}

static int idle_state_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, _idle_state_open, inode->i_private);
}

static ssize_t idle_state_read(struct file *filp, char __user *userbuf,
			       size_t count, loff_t *f_pos)
{
	static const char *d = "/sys/kernel/debug/cpuidle";
	char buf[BUF_SIZE] = { 0 };
	int cpu, len = 0;

	len += snprintf(buf + len, BUF_SIZE - len,
		       "******* idle state dump *******\n");
	for (cpu = 0; cpu <= 3; cpu++)
		len += snprintf(buf + len, BUF_SIZE - len,
				"dpidle_cnt[%d] = %zu, soidle_cnt[%d] = %zu\n",
				cpu, SMC_CALL(GET_PWR_CTRL_ARGS,
					      SPM_PWR_CTRL_DPIDLE,
					      PW_CPU0_CNT + cpu, 0),
				cpu, SMC_CALL(GET_PWR_CTRL_ARGS,
					      SPM_PWR_CTRL_SODI,
					      PW_CPU0_CNT + cpu, 0));

	len += snprintf(buf + len, BUF_SIZE - len,
			"\n******* variables dump *******\n");
	len += snprintf(buf + len, BUF_SIZE - len,
			"dpidle_switch = %zu, soidle_switch = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				 PW_IDLE_SWITCH, 0),
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				 PW_IDLE_SWITCH, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"\n******* idle command help *******\n");
	len += snprintf(buf + len, BUF_SIZE - len,
			"status: cat %s/idle_state\n", d);
	len += snprintf(buf + len, BUF_SIZE - len,
			"dpidle: cat %s/dpidle_state\n", d);
	len += snprintf(buf + len, BUF_SIZE - len,
			"soidle: cat %s/soidle_state\n", d);

	return simple_read_from_buffer(userbuf, count, f_pos, buf, len);
}

static const struct file_operations idle_state_fops = {
	.owner = THIS_MODULE,
	.open = idle_state_open,
	.read = idle_state_read,
	.llseek = seq_lseek,
	.release = single_release,
};

/*
 * dpidle_state
 */
static int _dpidle_state_open(struct seq_file *s, void *data)
{
	return 0;
}

static int dpidle_state_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, _dpidle_state_open, inode->i_private);
}

static ssize_t dpidle_state_read(struct file *filp, char __user *userbuf,
				 size_t count, loff_t *f_pos)
{
	static const char *d = "/sys/kernel/debug/cpuidle";
	char buf[BUF_SIZE] = { 0 };
	u32 i, clks[NR_GRPS] = { 0 }, idle_masks[NR_GRPS];
	int len = 0;

	/* idle masks */
	idle_masks[CG_INFRA1] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					 PW_INFRA1_IDLE_MASK, 0);
	idle_masks[CG_INFRA0] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					 PW_INFRA0_IDLE_MASK, 0);
	idle_masks[CG_INFRA2] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					 PW_INFRA2_IDLE_MASK, 0);
	idle_masks[CG_INFRA3] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					 PW_INFRA3_IDLE_MASK, 0);
	idle_masks[CG_INFRA4] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					 PW_INFRA4_IDLE_MASK, 0);
	idle_masks[CG_MMSYS0] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					 PW_MMSYS0_IDLE_MASK, 0);
	idle_masks[CG_MMSYS1] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					 PW_MMSYS1_IDLE_MASK, 0);
	idle_masks[PWR_CAM] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				       PW_CAM_IDLE_MASK, 0);
	idle_masks[PWR_MFG] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				       PW_MFG_IDLE_MASK, 0);
	idle_masks[PWR_VENC] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					PW_VENC_IDLE_MASK, 0);
	idle_masks[PWR_VDEC] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
					PW_VDEC_IDLE_MASK, 0);
	idle_masks[PWR_APU] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				       PW_APU_IDLE_MASK, 0);

	/* clks */
	clks[CG_INFRA1] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				    PW_INFRA1_IDLE_RG, 0);
	clks[CG_INFRA0] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				    PW_INFRA0_IDLE_RG, 0);
	clks[CG_INFRA2] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				    PW_INFRA2_IDLE_RG, 0);
	clks[CG_INFRA3] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				    PW_INFRA3_IDLE_RG, 0);
	clks[CG_INFRA4] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				    PW_INFRA4_IDLE_RG, 0);

	if (readl(PWR_STATUS_2ND) & PWR_STA_DISP) {
		clks[CG_MMSYS0] = ~SMC_CALL(GET_PWR_CTRL_ARGS,
					    SPM_PWR_CTRL_DPIDLE,
					    PW_MMSYS0_IDLE_RG, 0);
		clks[CG_MMSYS1] = ~SMC_CALL(GET_PWR_CTRL_ARGS,
					    SPM_PWR_CTRL_DPIDLE,
					    PW_MMSYS1_IDLE_RG, 0);
	}

	clks[PWR_CAM] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				  PW_CAM_IDLE_RG, 0);
	clks[PWR_MFG] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				  PW_MFG_IDLE_RG, 0);
	clks[PWR_VENC] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				   PW_VENC_IDLE_RG, 0);
	clks[PWR_VDEC] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				   PW_VDEC_IDLE_RG, 0);
	clks[PWR_APU] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				  PW_APU_IDLE_RG, 0);

	len += snprintf(buf + len, BUF_SIZE - len,
			"*********** deep idle state ************\n");
	len += snprintf(buf + len, BUF_SIZE - len,
			"dpidle_block_cnt[by_swt ] = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				 PW_BY_SWT, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"dpidle_block_cnt[by_boot] = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				 PW_BY_BOOT, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"dpidle_block_cnt[by_cpu ] = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				 PW_BY_CPU, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"dpidle_block_cnt[by_clk ] = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				 PW_BY_CLK, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"dpidle_block_cnt[by_gpt ] = %zu\n\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				 PW_BY_GPT, 0));

	for (i = 0; i < NR_GRPS; i++)
		len += snprintf(buf + len, BUF_SIZE - len,
				"[%-8s] = 0x%08x, block = 0x%08x\n",
				cg_name[i], idle_masks[i],
				(clks[i] & idle_masks[i]));

	len += snprintf(buf + len, BUF_SIZE - len,
			"\n*********** dpidle command help  ************\n");
	len += snprintf(buf + len, BUF_SIZE - len,
			"switch on/off: echo dpidle 1/0 > %s\n", d);
	len += snprintf(buf + len, BUF_SIZE - len,
			"log on/off: echo log_en 1/0 > %s\n", d);

	return simple_read_from_buffer(userbuf, count, f_pos, buf, len);
}

static ssize_t dpidle_state_write(struct file *filp, const char __user *userbuf,
				  size_t count, loff_t *f_pos)
{
	char cmd[32], buf[BUF_SIZE] = { 0 };
	int param;

	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = '\0';

	if (sscanf(buf, "%31s %d", cmd, &param) == 2) {
		if (!strcmp(cmd, "dpidle"))
			SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				 PW_IDLE_SWITCH, param);
		else if (!strcmp(cmd, "log_en"))
			SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_DPIDLE,
				 PW_LOG_EN, param);

		return count;
	}

	return -EINVAL;
}

static const struct file_operations dpidle_state_fops = {
	.owner = THIS_MODULE,
	.open = dpidle_state_open,
	.read = dpidle_state_read,
	.write = dpidle_state_write,
	.llseek = seq_lseek,
	.release = single_release,
};

/*
 * soidle_state
 */
static int _soidle_state_open(struct seq_file *s, void *data)
{
	return 0;
}

static int soidle_state_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, _soidle_state_open, inode->i_private);
}

static ssize_t soidle_state_read(struct file *filp, char __user *userbuf,
				 size_t count, loff_t *f_pos)
{
	static const char *d = "/sys/kernel/debug/cpuidle";
	char buf[BUF_SIZE] = { 0 };
	u32 i, clks[NR_GRPS] = { 0 }, idle_masks[NR_GRPS];
	int len = 0;

	/* idle masks */
	idle_masks[CG_INFRA1] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					 PW_INFRA1_IDLE_MASK, 0);
	idle_masks[CG_INFRA0] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					 PW_INFRA0_IDLE_MASK, 0);
	idle_masks[CG_INFRA2] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					 PW_INFRA2_IDLE_MASK, 0);
	idle_masks[CG_INFRA3] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					 PW_INFRA3_IDLE_MASK, 0);
	idle_masks[CG_INFRA4] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					 PW_INFRA4_IDLE_MASK, 0);
	idle_masks[CG_MMSYS0] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					 PW_MMSYS0_IDLE_MASK, 0);
	idle_masks[CG_MMSYS1] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					 PW_MMSYS1_IDLE_MASK, 0);
	idle_masks[PWR_CAM] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				       PW_CAM_IDLE_MASK, 0);
	idle_masks[PWR_MFG] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				       PW_MFG_IDLE_MASK, 0);
	idle_masks[PWR_VENC] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					PW_VENC_IDLE_MASK, 0);
	idle_masks[PWR_VDEC] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
					PW_VDEC_IDLE_MASK, 0);
	idle_masks[PWR_APU] = SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				       PW_APU_IDLE_MASK, 0);

	/* clks */
	clks[CG_INFRA1] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				    PW_INFRA1_IDLE_RG, 0);
	clks[CG_INFRA0] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				    PW_INFRA0_IDLE_RG, 0);
	clks[CG_INFRA2] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				    PW_INFRA2_IDLE_RG, 0);
	clks[CG_INFRA3] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				    PW_INFRA3_IDLE_RG, 0);
	clks[CG_INFRA4] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				    PW_INFRA4_IDLE_RG, 0);

	if (readl(PWR_STATUS_2ND) & PWR_STA_DISP) {
		clks[CG_MMSYS0] = ~SMC_CALL(GET_PWR_CTRL_ARGS,
					    SPM_PWR_CTRL_SODI,
					    PW_MMSYS0_IDLE_RG, 0);
		clks[CG_MMSYS1] = ~SMC_CALL(GET_PWR_CTRL_ARGS,
					    SPM_PWR_CTRL_SODI,
					    PW_MMSYS1_IDLE_RG, 0);
	}

	clks[PWR_CAM] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				  PW_CAM_IDLE_RG, 0);
	clks[PWR_MFG] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				  PW_MFG_IDLE_RG, 0);
	clks[PWR_VENC] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				   PW_VENC_IDLE_RG, 0);
	clks[PWR_VDEC] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				   PW_VDEC_IDLE_RG, 0);
	clks[PWR_APU] = ~SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				  PW_APU_IDLE_RG, 0);

	len += snprintf(buf + len, BUF_SIZE - len,
			"*********** soidle state ************\n");
	len += snprintf(buf + len, BUF_SIZE - len,
			"soidle_block_cnt[by_swt ] = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				 PW_BY_SWT, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"soidle_block_cnt[by_boot] = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				 PW_BY_BOOT, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"soidle_block_cnt[by_cpu ] = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				 PW_BY_CPU, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"soidle_block_cnt[by_clk ] = %zu\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				 PW_BY_CLK, 0));
	len += snprintf(buf + len, BUF_SIZE - len,
			"soidle_block_cnt[by_gpt ] = %zu\n\n",
			SMC_CALL(GET_PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				 PW_BY_GPT, 0));

	for (i = 0; i < NR_GRPS; i++)
		len += snprintf(buf + len, BUF_SIZE - len,
				"[%-8s] = 0x%08x, block = 0x%08x\n",
				cg_name[i], idle_masks[i],
				(clks[i] & idle_masks[i]));

	len += snprintf(buf + len, BUF_SIZE - len,
			"\n*********** sodi command help  ************\n");
	len += snprintf(buf + len, BUF_SIZE - len,
			"switch on/off: echo soidle 1/0 > %s\n", d);
	len += snprintf(buf + len, BUF_SIZE - len,
			"log on/off: echo log_en 1/0 > %s\n", d);


	return simple_read_from_buffer(userbuf, count, f_pos, buf, len);
}

static ssize_t soidle_state_write(struct file *filp, const char __user *userbuf,
				  size_t count, loff_t *f_pos)
{
	char cmd[32], buf[BUF_SIZE] = { 0 };
	int param;

	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = '\0';

	if (sscanf(buf, "%31s %d", cmd, &param) == 2) {
		if (!strcmp(cmd, "soidle"))
			SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				 PW_IDLE_SWITCH, param);
		else if (!strcmp(cmd, "log_en"))
			SMC_CALL(PWR_CTRL_ARGS, SPM_PWR_CTRL_SODI,
				 PW_LOG_EN, param);

		return count;
	}

	return -EINVAL;
}

static const struct file_operations soidle_state_fops = {
	.owner = THIS_MODULE,
	.open = soidle_state_open,
	.read = soidle_state_read,
	.write = soidle_state_write,
	.llseek = seq_lseek,
	.release = single_release,
};

int sloa_fs_init(void)
{
	struct dentry *root_entry, *file_entry;
	struct device_node *node;
	const u8 *compatible_node = "mediatek,mt8168-scpsys";
	static const char *d = "/sys/kernel/debug/cpuidle";
	int ret;

	node = of_find_compatible_node(NULL, NULL, compatible_node);
	if (!node) {
		pr_err("cannot find \"%s\"\n", compatible_node);
		return -ENODEV;
	}

	scpsys_base = of_iomap(node, 0);
	if (!scpsys_base) {
		pr_err("cannot iomap \"%s\"\n", compatible_node);
		return -ENODEV;
	}

	ret = sysfs_create_group(power_kobj, &spm_attr_group);
	if (ret)
		pr_notice("failed to create /sys/power/spm (%d)\n", ret);

	root_entry = debugfs_create_dir("cpuidle", NULL);
	if (IS_ERR(root_entry))
		pr_notice("Can not create %s: %ld\n", d, PTR_ERR(root_entry));

	file_entry = debugfs_create_file("idle_state", 0444,
					 root_entry, NULL, &idle_state_fops);
	if (IS_ERR(file_entry))
		pr_notice("Can not create %s/idle_state: %ld\n",
			  d, PTR_ERR(file_entry));

	file_entry = debugfs_create_file("dpidle_state", 0644,
					 root_entry, NULL, &dpidle_state_fops);
	if (IS_ERR(file_entry))
		pr_notice("Can not create %s/dpidle_state: %ld\n",
			  d, PTR_ERR(file_entry));

	file_entry  = debugfs_create_file("soidle_state", 0644,
					  root_entry, NULL, &soidle_state_fops);
	if (IS_ERR(file_entry))
		pr_notice("Can not create %s/soidle_state: %ld\n",
			  d, PTR_ERR(file_entry));

	register_syscore_ops(&sloa_syscore_ops);

	return ret;
}

late_initcall_sync(sloa_fs_init);

MODULE_DESCRIPTION("SLOA-FS Driver v0.1");
