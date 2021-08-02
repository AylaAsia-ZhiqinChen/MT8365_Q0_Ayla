/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef __MTK_WDT_H__
#define __MTK_WDT_H__

#define MTK_WDT_BASE		RGU_BASE

#define MTK_WDT_MODE		(MTK_WDT_BASE + 0x00)
#define MTK_WDT_LENGTH		(MTK_WDT_BASE + 0x04)
#define MTK_WDT_RESTART	(MTK_WDT_BASE + 0x08)
#define MTK_WDT_STATUS		(MTK_WDT_BASE + 0x0C)
#define MTK_WDT_INTERVAL	(MTK_WDT_BASE + 0x10)
#define MTK_WDT_SWRST		(MTK_WDT_BASE + 0x14)
#define MTK_WDT_SWSYSRST	(MTK_WDT_BASE + 0x18)
#define MTK_WDT_NONRST_REG	(MTK_WDT_BASE + 0x20)
#define MTK_WDT_NONRST_REG2	(MTK_WDT_BASE + 0x24)
#define MTK_WDT_REQ_MODE	(MTK_WDT_BASE + 0x30)
#define MTK_WDT_REQ_IRQ_EN	(MTK_WDT_BASE + 0x34)
#define MTK_WDT_DEBUG_CTL	(MTK_WDT_BASE + 0x40)

/*WDT_MODE*/
#define MTK_WDT_MODE_KEY  0x22000000

#define MTK_WDT_MODE_DDR_RESERVE  (0x1 << 7)
#define MTK_WDT_MODE_DUAL_MODE  (0x1 << 6)
#define MTK_WDT_MODE_AUTO_RESTART (0x1 << 4) /* sw reserved */
#define MTK_WDT_MODE_IRQ	(0x1 << 3)
#define MTK_WDT_MODE_EXTEN	(0x1 << 2)
#define MTK_WDT_MODE_EXT_POL	(0x1 << 1)
#define MTK_WDT_MODE_ENABLE	(0x1 << 0)

/*WDT_LENGTH*/
#define MTK_WDT_LENGTH_TIME_OUT         0xffe0
#define MTK_WDT_LENGTH_KEYMASK         0x001f
#define MTK_WDT_LENGTH_KEY		0x0008

/*WDT_RESTART*/
#define MTK_WDT_RESTART_KEY		0x1971

/*WDT_STATUS*/
#define MTK_WDT_STATUS_HWWDT_RST	(1 << 31)
#define MTK_WDT_STATUS_SWWDT_RST	(1 << 30)
#define MTK_WDT_STATUS_IRQWDT_RST	(1 << 29)
#define MTK_WDT_STATUS_SECURITY_RST	(1 << 28)
#define MTK_WDT_STATUS_DEBUG_RST	(1 << 19)
#define MTK_WDT_STATUS_THERMAL_CTL_RST	(1 << 18)
#define MTK_WDT_STATUS_SPMWDT_RST	(1 << 1)
#define MTK_WDT_STATUS_SPM_THERMAL_RST	(1 << 0)

/*WDT_INTERVAL*/
#define MTK_WDT_INTERVAL_MASK		0xfff

/*WDT_SWRST*/
#define MTK_WDT_SWRST_KEY		0x1209

/*WDT_SWSYSRST*/
#define MTK_WDT_SWSYS_RST_KEY		0x88000000
#define MTK_WDT_SWSYS_RST_MD_RST       (0x1 << 7)

/* MTK_WDT_DEBUG_CTL */
#define MTK_DEBUG_CTL_KEY           0x59000000
#define MTK_RG_DDR_PROTECT_EN       (0x1 << 0)
#define MTK_RG_MCU_LATH_EN          (0x1 << 1)
#define MTK_RG_DRAMC_SREF           (0x1 << 8)
#define MTK_RG_DRAMC_ISO            (0x1 << 9)
#define MTK_RG_CONF_ISO             (0x1 << 10)
#define MTK_DDR_RESERVE_STA         (0x1 << 16)
#define MTK_DDR_SREF_STA            (0x1 << 17)

/* Reboot reason */
#define RE_BOOT_REASON_UNKNOW           (0x00)
#define RE_BOOT_BY_WDT_HW               (0x01)
#define RE_BOOT_BY_WDT_SW               (0x02)
#define RE_BOOT_WITH_INTTERUPT          (0x04)
#define RE_BOOT_BY_SPM_THERMAL          (0x08)
#define RE_BOOT_BY_SPM              	(0x10)
#define RE_BOOT_BY_THERMAL_DIRECT       (0x20)
#define RE_BOOT_BY_DEBUG        			(0x40)
#define RE_BOOT_BY_SECURITY        		(0x80)

#define RE_BOOT_ABNORMAL                (0xF0)

/* Reboot from which stage */
#define RE_BOOT_FROM_UNKNOW             (0x00)
#define RE_BOOT_FROM_PRE_LOADER         (0x01)
#define RE_BOOT_FROM_U_BOOT             (0x02)
#define RE_BOOT_FROM_KERNEL             (0x03)
#define RE_BOOT_FROM_POWER_ON           (0x04)

#define WDT_NORMAL_REBOOT               (0x100)
#define WDT_BY_PASS_PWK_REBOOT          (0x200)
#define WDT_NOT_WDT_REBOOT              (0x400)

typedef enum wd_swsys_reset_type {
	WD_MD_RST,
}WD_SYS_RST_TYPE;

extern void mtk_wdt_init(void);
extern BOOL mtk_is_rgu_trigger_reset(void);
extern void mtk_arch_reset(char mode);
extern int mtk_wdt_boot_check(void);
int rgu_dram_reserved(int enable);
int rgu_is_reserve_ddr_enabled(void);
int rgu_is_dram_slf(void);
void rgu_release_rg_dramc_conf_iso(void);
void rgu_release_rg_dram_setting(void);
void rgu_release_rg_dramc_iso(void);
void rgu_release_rg_dramc_sref(void);
int rgu_is_reserve_ddr_mode_success(void);
void rgu_swsys_reset(WD_SYS_RST_TYPE reset_type);

extern unsigned g_rgu_status;

#endif   /*__MTK_WDT_H__*/
