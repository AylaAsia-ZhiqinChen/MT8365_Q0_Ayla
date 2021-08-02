#ifndef _MT_SPM_
#define _MT_SPM_

#include "typedefs.h"
#include "mt8168.h"

#define SPM_REG(offset)				(SPM_BASE + offset)
#define SPM_PROJECT_CODE			0xB16
#define POWERON_CONFIG_EN			SPM_REG(0x0000)
#define PWR_STATUS				SPM_REG(0x0180)
#define PWR_STATUS_2ND				SPM_REG(0x0184)

#define DIS_PWR_CON				SPM_REG(0x030C)
#define VEN_PWR_CON				SPM_REG(0x0304)
#define ISP_PWR_CON				SPM_REG(0x0308)
#define AUDIO_PWR_CON				SPM_REG(0x0314)
#define VPU_TOP_PWR_CON				SPM_REG(0x0324)
#define CONN_PWR_CON				SPM_REG(0x032C)
#define MFG_PWR_CON				SPM_REG(0x0338)
#define CAM_PWR_CON				SPM_REG(0x0344)
#define VDE_PWR_CON				SPM_REG(0x0370)
#define APU_PWR_CON				SPM_REG(0x0378)
#define DSP_PWR_CON				SPM_REG(0x037C)

#define INFRACFG_REG(offset)			(INFRACFG_AO_BASE + offset)
#define INFRA_TOPAXI_PROTECTEN			INFRACFG_REG(0x0220)
#define INFRA_TOPAXI_PROTECTEN_STA1		INFRACFG_REG(0x0228)
#define INFRA_TOPAXI_PROTECTEN_1		INFRACFG_REG(0x0250)
#define INFRA_TOPAXI_PROTECTEN_STA1_1		INFRACFG_REG(0x0258)
#define INFRA_TOPAXI_PROTECTEN_SET		INFRACFG_REG(0x02A0)
#define INFRA_TOPAXI_PROTECTEN_CLR		INFRACFG_REG(0x02A4)
#define INFRA_TOPAXI_PROTECTEN_1_SET		INFRACFG_REG(0x02A8)
#define INFRA_TOPAXI_PROTECTEN_1_CLR		INFRACFG_REG(0x02AC)
#define AUDIO_BUS_AUD_SI0			INFRACFG_REG(0x0800)
#define AUDIO_BUS_INFRA_SI0			INFRACFG_REG(0x0808)

/**************************************
 * Macro and Inline
 **************************************/
#define spm_read(addr)				DRV_Reg32(addr)
#define spm_write(addr, val)			DRV_WriteReg32(addr, val)

/* Define MTCMOS power control */
#define PWR_RST_B                        (0x1 << 0)
#define PWR_ISO                          (0x1 << 1)
#define PWR_ON                           (0x1 << 2)
#define PWR_ON_2ND                       (0x1 << 3)
#define PWR_CLK_DIS                      (0x1 << 4)
#define SRAM_CKISO                       (0x1 << 5)
#define SRAM_ISOINT_B                    (0x1 << 6)
#define SLPB_CLAMP                       (0x1 << 7)
#define APU_PDN_BIT0                     (0x1 << 8)
#define APU_PDN_BIT1                     (0x1 << 9)
#define APU_PDN_BIT2                     (0x1 << 10)
#define pwrdnreqn_mp0_adb                (0x1 << 10)
#define APU_PDN_BIT3                     (0x1 << 11)
#define pwrdnreqn_mp1_adb                (0x1 << 11)
#define pwrdnreqn_icc_mp0_adb            (0x1 << 12)
#define APU_PDN_BIT4                     (0x1 << 12)
#define APU_PDN_BIT5                     (0x1 << 13)
#define pwrdnreqn_icd_mp0_adb            (0x1 << 13)
#define APU_PDN_BIT6                     (0x1 << 14)
#define pwrdnreqn_icc_mp1_adb            (0x1 << 14)
#define pwrdnreqn_icd_mp1_adb            (0x1 << 15)
#define DSP_SLPB_BIT0                    (0x1 << 16)
#define DSP_SLPB_BIT1                    (0x1 << 17)
#define DSP_SLPB_BIT2                    (0x1 << 18)
#define DSP_SLPB_BIT3                    (0x1 << 19)
#define DSP_SRAM_SLEEP_B_ACK             (0x1 << 20)
#define APU_SLPB_BIT0                    (0x1 << 22)
#define APU_SLPB_BIT1                    (0x1 << 23)
#define APU_SLPB_BIT2                    (0x1 << 24)
#define APU_SLPB_BIT3                    (0x1 << 25)
#define pwrdnreqn_l2c_mp0_adb            (0x1 << 26)
#define APU_SLPB_BIT4                    (0x1 << 26)
#define pwrdnreqn_l2c_mp1_adb            (0x1 << 27)

/* Define MTCMOS Bus Protect Mask */
#define MD1_PROT_STEP1_0_MASK            ((0x1 << 7))
#define MD1_PROT_STEP1_0_ACK_MASK        ((0x1 << 7))
#define MD1_PROT_STEP2_0_MASK            ((0x1 << 3) \
					  |(0x1 << 4))
#define MD1_PROT_STEP2_0_ACK_MASK        ((0x1 << 3) \
					  |(0x1 << 4))
#define MD1_PROT_STEP2_1_MASK            ((0x1 << 6))
#define MD1_PROT_STEP2_1_ACK_MASK        ((0x1 << 6))
#define CONN_PROT_STEP1_0_MASK           ((0x1 << 13))
#define CONN_PROT_STEP1_0_ACK_MASK       ((0x1 << 13))
#define CONN_PROT_STEP1_1_MASK           ((0x1 << 18))
#define CONN_PROT_STEP1_1_ACK_MASK       ((0x1 << 18))
#define CONN_PROT_STEP2_0_MASK           ((0x1 << 14))
#define CONN_PROT_STEP2_0_ACK_MASK       ((0x1 << 14))
#define DPY_PROT_STEP1_0_MASK            ((0x1 << 0) \
					  |(0x1 << 23) \
					  |(0x1 << 26))
#define DPY_PROT_STEP1_0_ACK_MASK        ((0x1 << 0) \
					  |(0x1 << 23) \
					  |(0x1 << 26))
#define DPY_PROT_STEP1_1_MASK            ((0x1 << 10) \
					  |(0x1 << 11) \
					  |(0x1 << 12) \
					  |(0x1 << 13) \
					  |(0x1 << 14) \
					  |(0x1 << 15) \
					  |(0x1 << 16) \
					  |(0x1 << 17))
#define DPY_PROT_STEP1_1_ACK_MASK        ((0x1 << 10) \
					  |(0x1 << 11) \
					  |(0x1 << 12) \
					  |(0x1 << 13) \
					  |(0x1 << 14) \
					  |(0x1 << 15) \
					  |(0x1 << 16) \
					  |(0x1 << 17))
#define DPY_PROT_STEP2_0_MASK            ((0x1 << 1) \
					  |(0x1 << 2) \
					  |(0x1 << 3) \
					  |(0x1 << 4) \
					  |(0x1 << 10) \
					  |(0x1 << 11) \
					  |(0x1 << 21) \
					  |(0x1 << 22))
#define DPY_PROT_STEP2_0_ACK_MASK        ((0x1 << 1) \
					  |(0x1 << 2) \
					  |(0x1 << 3) \
					  |(0x1 << 4) \
					  |(0x1 << 10) \
					  |(0x1 << 11) \
					  |(0x1 << 21) \
					  |(0x1 << 22))
#define DIS_PROT_STEP1_0_MASK            ((0x1 << 16) \
					  |(0x1 << 17))
#define DIS_PROT_STEP1_0_ACK_MASK        ((0x1 << 16) \
					  |(0x1 << 17))
#define DIS_PROT_STEP2_0_MASK            ((0x1 << 1) \
					  |(0x1 << 2) \
					  |(0x1 << 10) \
					  |(0x1 << 11))
#define DIS_PROT_STEP2_0_ACK_MASK        ((0x1 << 1) \
					  |(0x1 << 2) \
					  |(0x1 << 10) \
					  |(0x1 << 11))
#define MFG_PROT_STEP1_0_MASK            ((0x1 << 25))
#define MFG_PROT_STEP1_0_ACK_MASK        ((0x1 << 25))
#define MFG_PROT_STEP2_0_MASK            ((0x1 << 21) \
					  |(0x1 << 22))
#define MFG_PROT_STEP2_0_ACK_MASK        ((0x1 << 21) \
					  |(0x1 << 22))
#define MCU_PROT_STEP1_0_MASK            ((0x1 << 0) \
					  |(0x1 << 9))
#define MCU_PROT_STEP1_0_ACK_MASK        ((0x1 << 0) \
					  |(0x1 << 9))
#define MCU_PROT_STEP2_0_MASK            ((0x1 << 12))
#define MCU_PROT_STEP2_0_ACK_MASK        ((0x1 << 12))
#define APU_PROT_STEP1_0_MASK            ((0x1 << 2))
#define APU_PROT_STEP1_0_ACK_MASK        ((0x1 << 2))
#define APU_PROT_STEP2_0_MASK            ((0x1 << 20))
#define APU_PROT_STEP2_0_ACK_MASK        ((0x1 << 20))
#define CAM_PROT_STEP1_0_MASK            ((0x1 << 19))
#define CAM_PROT_STEP1_0_ACK_MASK        ((0x1 << 19))
#define CAM_PROT_STEP2_0_MASK            ((0x1 << 20))
#define CAM_PROT_STEP2_0_ACK_MASK        ((0x1 << 20))

/* Define MTCMOS Power Status Mask */

#define MD1_PWR_STA_MASK                 (0x1 << 0)
#define CONN_PWR_STA_MASK                (0x1 << 1)
#define DPY_PWR_STA_MASK                 (0x1 << 2)
#define DIS_PWR_STA_MASK                 (0x1 << 3)
#define MFG_PWR_STA_MASK                 (0x1 << 4)
#define IFR_PWR_STA_MASK                 (0x1 << 6)
#define MFG_CORE0_PWR_STA_MASK           (0x1 << 7)
#define MP0_CPUTOP_PWR_STA_MASK          (0x1 << 8)
#define MP0_CPU0_PWR_STA_MASK            (0x1 << 9)
#define MP0_CPU1_PWR_STA_MASK            (0x1 << 10)
#define MP0_CPU2_PWR_STA_MASK            (0x1 << 11)
#define MP0_CPU3_PWR_STA_MASK            (0x1 << 12)
#define MCU_PWR_STA_MASK                 (0x1 << 14)
#define MM_PWR_STA_MASK                  (0x1 << 15)
#define APU_PWR_STA_MASK                 (0x1 << 16)
#define DSP_PWR_STA_MASK                 (0x1 << 17)
#define VEN_PWR_STA_MASK                 (0x1 << 21)
#define MFG_ASYNC_PWR_STA_MASK           (0x1 << 23)
#define AUDIO_PWR_STA_MASK               (0x1 << 24)
#define CAM_PWR_STA_MASK                 (0x1 << 25)
#define VCODEC_PWR_STA_MASK              (0x1 << 26)
#define VDE_PWR_STA_MASK                 (0x1 << 31)

/* Define Non-CPU SRAM Mask */
#define MD1_SRAM_PDN                     (0x1 << 8)
#define MD1_SRAM_PDN_ACK                 (0x0 << 12)
#define MD1_SRAM_PDN_ACK_BIT0            (0x1 << 12)
#define DPY_SRAM_PDN                     (0xF << 8)
#define DPY_SRAM_PDN_ACK                 (0xF << 12)
#define DPY_SRAM_PDN_ACK_BIT0            (0x1 << 12)
#define DPY_SRAM_PDN_ACK_BIT1            (0x1 << 13)
#define DPY_SRAM_PDN_ACK_BIT2            (0x1 << 14)
#define DPY_SRAM_PDN_ACK_BIT3            (0x1 << 15)
#define DIS_SRAM_PDN                     (0x1 << 8)
#define DIS_SRAM_PDN_ACK                 (0x1 << 12)
#define DIS_SRAM_PDN_ACK_BIT0            (0x1 << 12)
#define MFG_SRAM_PDN                     (0x3 << 8)
#define MFG_SRAM_PDN_ACK                 (0x3 << 12)
#define MFG_SRAM_PDN_ACK_BIT0            (0x1 << 12)
#define MFG_SRAM_PDN_ACK_BIT1            (0x1 << 13)
#define IFR_SRAM_PDN                     (0x1 << 8)
#define IFR_SRAM_PDN_ACK                 (0x1 << 12)
#define IFR_SRAM_PDN_ACK_BIT0            (0x1 << 12)
#define MFG_CORE0_SRAM_PDN               (0x1 << 8)
#define MFG_CORE0_SRAM_PDN_ACK           (0x1 << 12)
#define MFG_CORE0_SRAM_PDN_ACK_BIT0      (0x1 << 12)
#define MM_SRAM_PDN                      (0xF << 8)
#define MM_SRAM_PDN_ACK                  (0xF << 12)
#define MM_SRAM_PDN_ACK_BIT0             (0x1 << 12)
#define MM_SRAM_PDN_ACK_BIT1             (0x1 << 13)
#define MM_SRAM_PDN_ACK_BIT2             (0x1 << 14)
#define MM_SRAM_PDN_ACK_BIT3             (0x1 << 15)
#define APU_SRAM_PDN                     (0x3 << 8)
#define APU_SRAM_PDN_ACK                 (0x3 << 15)
#define APU_SRAM_PDN_ACK_BIT0            (0x1 << 15)
#define APU_SRAM_PDN_ACK_BIT1            (0x1 << 16)
#define DSP_SRAM_PDN                     (0xF << 8)
#define DSP_SRAM_PDN_ACK                 (0xF << 12)
#define DSP_SRAM_PDN_ACK_BIT0            (0x1 << 12)
#define DSP_SRAM_PDN_ACK_BIT1            (0x1 << 13)
#define DSP_SRAM_PDN_ACK_BIT2            (0x1 << 14)
#define DSP_SRAM_PDN_ACK_BIT3            (0x1 << 15)
#define VEN_SRAM_PDN                     (0x1 << 8)
#define VEN_SRAM_PDN_ACK                 (0x1 << 12)
#define VEN_SRAM_PDN_ACK_BIT0            (0x1 << 12)
#define AUDIO_SRAM_PDN                   (0x1F << 8)
#define AUDIO_SRAM_PDN_ACK               (0x1F << 13)
#define AUDIO_SRAM_PDN_ACK_BIT0          (0x1 << 13)
#define AUDIO_SRAM_PDN_ACK_BIT1          (0x1 << 14)
#define AUDIO_SRAM_PDN_ACK_BIT2          (0x1 << 15)
#define AUDIO_SRAM_PDN_ACK_BIT3          (0x1 << 16)
#define AUDIO_SRAM_PDN_ACK_BIT4          (0x1 << 17)
#define CAM_SRAM_PDN                     (0x3 << 8)
#define CAM_SRAM_PDN_ACK                 (0x3 << 12)
#define CAM_SRAM_PDN_ACK_BIT0            (0x1 << 12)
#define CAM_SRAM_PDN_ACK_BIT1            (0x1 << 13)
#define VCODEC_SRAM_PDN                  (0x1 << 8)
#define VCODEC_SRAM_PDN_ACK              (0x1 << 12)
#define VCODEC_SRAM_PDN_ACK_BIT0         (0x1 << 12)
#define VDE_SRAM_PDN                     (0x1 << 8)
#define VDE_SRAM_PDN_ACK                 (0x1 << 12)
#define VDE_SRAM_PDN_ACK_BIT0            (0x1 << 12)

#define STA_POWER_DOWN			0
#define STA_POWER_ON			1

extern int spm_mtcmos_ctrl_dis(int state);
extern int spm_mtcmos_ctrl_conn(int state);
extern int spm_mtcmos_ctrl_mfg(int state);
extern int spm_mtcmos_ctrl_apu_shut_down(int state);
extern int spm_mtcmos_ctrl_dsp_shut_down(int state);
extern int spm_mtcmos_ctrl_ven(int state);
extern int spm_mtcmos_ctrl_audio(int state);
extern int spm_mtcmos_ctrl_cam(int state);
extern int spm_mtcmos_ctrl_vde(int state);

#endif
