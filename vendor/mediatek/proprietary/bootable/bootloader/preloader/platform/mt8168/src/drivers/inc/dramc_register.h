#include "mt8168.h"
#include "ddrphy_nao_reg.h"
#include "ddrphy_pll_reg.h"
#include "ddrphy_wo_pll_reg.h"
#include "dramc_ch0_nao_reg.h"
#include "dramc_ch0_reg.h"

/* SW Virtual base address */
#define Channel_A_DRAMC_NAO_BASE_VIRTUAL    0x40000
#define Channel_B_DRAMC_NAO_BASE_VIRTUAL    0x50000
#define Channel_A_DRAMC_AO_BASE_VIRTUAL	    0x60000
#define Channel_B_DRAMC_AO_BASE_VIRTUAL     0x70000
#define Channel_A_PHY_NAO_BASE_VIRTUAL      0x80000
#define Channel_B_PHY_NAO_BASE_VIRTUAL      0x90000
#define Channel_A_PHY_AO_BASE_VIRTUAL       0xa0000
#define Channel_B_PHY_AO_BASE_VIRTUAL       0xb0000
#define MAX_BASE_VIRTUAL                    0xc0000

/* HW Physical base address */
#define Channel_A_DRAMC_NAO_BASE_ADDRESS    0x1022E000
#define Channel_B_DRAMC_NAO_BASE_ADDRESS    0x10238000
#define Channel_A_DRAMC_AO_BASE_ADDRESS     0x1022C000
#define Channel_B_DRAMC_AO_BASE_ADDRESS     0x10232000
#define Channel_A_PHY_NAO_BASE_ADDRESS      0x10230000
#define Channel_B_PHY_NAO_BASE_ADDRESS      0x1023A000
#define Channel_A_PHY_AO_BASE_ADDRESS       0x10228000
#define Channel_B_PHY_AO_BASE_ADDRESS       0x10234000

#define POS_BANK_NUM			(16)
#define SHIFT_TO_CHB_ADDR		((U32)CHANNEL_B << POS_BANK_NUM)

#define DRAMC_BROADCAST_ON		0x1
#define DRAMC_BROADCAST_OFF		0x0
#define DRAMC_WBR				0x100010B4 /* TBD */

#define SPM_LIB_USE
#define AP_SLEEP_BASE		0x10006000	/* SPM base address */

#ifdef SPM_LIB_USE
#define SPM_BASE_ADDRESS 0x10006000

#define SPM_POWERON_CONFIG_EN (SPM_BASE_ADDRESS + 0x0000)
	#define POWERON_CONFIG_EN_BCLK_CG_EN Fld(1, 0)//[0:0]
	#define POWERON_CONFIG_EN_PROJECT_CODE Fld(16, 16)
#define SPM_POWER_ON_VAL0 (SPM_BASE_ADDRESS + 0x0004)
	#define SPM_POWER_ON_VAL0_INI_PWRON_REG0_RF Fld(32, 0)
	#define SPM_POWER_ON_VAL0_SC_DR_SHU_LEVEL_PCM Fld(2, 30)
	#define SPM_POWER_ON_VAL0_SC_PHYPLL2_MODE_SW_PCM Fld(1, 29)//[29:29]
	#define SPM_POWER_ON_VAL0_SC_PHYPLL_MODE_SW_PCM Fld(1, 28)//[28:28]
	#define SPM_POWER_ON_VAL0_SC_PHYPLL2_SHU_EN_PCM Fld(1, 27)//[27:27]
	#define SPM_POWER_ON_VAL0_SC_PHYPLL_SHU_EN_PCM Fld(1, 26)//[26:26]
	#define SPM_POWER_ON_VAL0_SC_DPHY_RXDLY_TRACK_EN Fld(1, 25)//[25:25]
	#define SPM_POWER_ON_VAL0_SC_MPLL_S_OFF Fld(1, 24)//[24:24]
	#define SPM_POWER_ON_VAL0_SC_DPHY_PRECAL_UP Fld(1, 23)//[23:23]
	#define SPM_POWER_ON_VAL0_SC_DR_SHU_EN_PCM Fld(1, 22)//[22:22]
	#define SPM_POWER_ON_VAL0_SC_CKSQ1_OFF Fld(1, 21)//[21:21]
	#define SPM_POWER_ON_VAL0_SC_CKSQ0_OFF Fld(1, 20)//[20:20]
	#define SPM_POWER_ON_VAL0_SC_SHU_RESTORE Fld(1, 19)//[19:19]
	#define SPM_POWER_ON_VAL0_SC_MPLL_OFF Fld(1, 18)//[18:18] //BIANCO_TO_BE_PORTING
	#define SPM_POWER_ON_VAL0_SC_DPY_BCLK_ENABLE Fld(1, 17)//[17:17]
	#define SPM_POWER_ON_VAL0_SC_DDRPHY_FB_CK_EN_PCM Fld(1, 16)//[16:16]
	#define SPM_POWER_ON_VAL0_SC_PHYPLL_EN_PCM Fld(1, 15)//[15:15]
	#define SPM_POWER_ON_VAL0_SC_DPY_VREF_EN_PCM Fld(1, 14)//[14:14]
	#define SPM_POWER_ON_VAL0_SC_DPY_DLL_CK_EN_PCM Fld(1, 13)//[13:13]
	#define SPM_POWER_ON_VAL0_SC_DPY_DLL_EN_PCM Fld(1, 12)//[12:12]
	#define SPM_POWER_ON_VAL0_SC_TX_TRACKING_DIS Fld(1, 11)//[11:11]
	#define SPM_POWER_ON_VAL0_SC_MPLLOUT_OFF Fld(1, 10)//[10:10]
	//Empty [9:9]
	#define SPM_POWER_ON_VAL0_SC_DPY_2ND_DLL_EN_PCM Fld(1, 8)//[8:8]
	#define SPM_POWER_ON_VAL0_SC_DMSUS_OFF_PCM Fld(1, 7)//[7:7]
	#define SPM_POWER_ON_VAL0_SC_DPY_MODE_SW_PCM Fld(1, 6)//[6:6]
	#define SPM_POWER_ON_VAL0_SC_MD26M_CK_OFF Fld(1, 5)//[5:5]
	#define SPM_POWER_ON_VAL0_SC_DR_GATE_RETRY_EN_PCM Fld(1, 4)//[4:4]
	#define SPM_POWER_ON_VAL0_SC_AXI_CK_OFF Fld(1, 3)//[3:3]
	#define SPM_POWER_ON_VAL0_SC_MEM_CK_OFF Fld(1, 2)//[2:2]
	#define SPM_POWER_ON_VAL0_SC_TIMER_RST Fld(1, 1)//[1:1]
	#define SPM_POWER_ON_VAL0_SC_26M_CK_OFF Fld(1, 0)//[0:0]
#define SPM_POWER_ON_VAL1 (SPM_BASE_ADDRESS + 0x0008)
	#define SPM_POWER_ON_VAL1_SC_DR_SHORT_QUEUE_PCM Fld(1, 31)//[31:31]
	#define SPM_POWER_ON_VAL1_EMI_CLK_OFF_REQ_PCM Fld(1, 1)//[1:1]
#define SPM_CLK_CON (SPM_BASE_ADDRESS + 0x000C)
	#define SPM_CLK_CON_REG_SYSCLK0_EN_CTRL Fld(2, 0)
	#define SPM_CLK_CON_REG_SYSCLK1_EN_CTRL Fld(2, 2)
	#define SPM_CLK_CON_REG_SYSSETTLE_SEL Fld(1, 4)//[4:4]
	#define SPM_CLK_CON_REG_SPM_LOCK_INFRA_DCM Fld(1, 5)//[5:5]
	#define SPM_CLK_CON_REG_EXT_SRCCLKEN_MASK Fld(3, 6)
	#define SPM_CLK_CON_REG_CXO32K_REMOVE_EN_MD1 Fld(1, 9)//[9:9]
	#define SPM_CLK_CON_REG_CXO32K_REMOVE_EN_MD2 Fld(1, 10)//[10:10]
	#define SPM_CLK_CON_REG_CLKSQ0_SEL_CTRL Fld(1, 11)//[11:11]
	#define SPM_CLK_CON_REG_CLKSQ1_SEL_CTRL Fld(1, 12)//[12:12]
	#define SPM_CLK_CON_REG_SRCCLKEN0_EN Fld(1, 13)//[13:13]
	#define SPM_CLK_CON_REG_SRCCLKEN1_EN Fld(1, 14)//[14:14]
	#define SPM_CLK_CON_REG_SCP_DCM_EN Fld(1, 15)//[15:15]
	#define SPM_CLK_CON_REG_SYSCLK0_SRC_MASK_B Fld(7, 16)
	#define SPM_CLK_CON_REG_SYSCLK1_SRC_MASK_B Fld(7, 23)
#define SPM_PCM_PWR_IO_EN (SPM_BASE_ADDRESS + 0x002C)
	#define PCM_PWR_IO_EN_RG_PCM_PWR_IO_EN Fld(8, 0)
	#define PCM_PWR_IO_EN_RG_RF_SYNC_EN Fld(8, 16)

#define SPM_DRAMC_DPY_CLK_SW_CON_SEL (SPM_BASE_ADDRESS + 0x0460)
	#define DRAMC_DPY_CLK_SW_CON_SEL_FULL Fld(32, 0)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DR_GATE_RETRY_EN_SEL Fld(2, 0)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_EMI_CLK_OFF_SEL Fld(2, 2)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DPY_MODE_SW_SEL Fld(2, 4)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DMSUS_OFF_SEL Fld(2, 6)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_MEM_CK_OFF_SEL Fld(2, 8)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DPY_2ND_DLL_EN_SEL Fld(2, 10)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DPY_DLL_EN_SEL Fld(2, 12)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DPY_DLL_CK_EN_SEL Fld(2, 14)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DPY_VREF_EN_SEL Fld(2, 16)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_PHYPLL_EN_SEL Fld(2, 18)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DDRPHY_FB_CK_EN_SEL Fld(2, 20)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SEPERATE_PHY_PWR_SEL Fld(1, 23)//[23:23]
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_SW_DMDRAMCSHU_ACK_SEL Fld(2, 24)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_EMI_CLK_OFF_ACK_SEL Fld(2, 26)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DR_SHORT_QUEUE_ACK_SEL Fld(2, 28)
	#define DRAMC_DPY_CLK_SW_CON_SEL_SW_DRAMC_DFS_STA_SEL Fld(2, 30)
#define SPM_DRAMC_DPY_CLK_SW_CON (SPM_BASE_ADDRESS + 0x0464)
	#define DRAMC_DPY_CLK_SW_CON_SW_DR_GATE_RETRY_EN Fld(2, 0)
	#define DRAMC_DPY_CLK_SW_CON_SW_EMI_CLK_OFF Fld(2, 2)
	#define DRAMC_DPY_CLK_SW_CON_SW_DPY_MODE_SW Fld(2, 4)
	#define DRAMC_DPY_CLK_SW_CON_SW_DMSUS_OFF Fld(2, 6)
	#define DRAMC_DPY_CLK_SW_CON_SW_MEM_CK_OFF Fld(2, 8)
	#define DRAMC_DPY_CLK_SW_CON_SW_DPY_2ND_DLL_EN Fld(2, 10)
	#define DRAMC_DPY_CLK_SW_CON_SW_DPY_DLL_EN Fld(2, 12)
	#define DRAMC_DPY_CLK_SW_CON_SW_DPY_DLL_CK_EN Fld(2, 14)
	#define DRAMC_DPY_CLK_SW_CON_SW_DPY_VREF_EN Fld(2, 16)
	#define DRAMC_DPY_CLK_SW_CON_SW_PHYPLL_EN Fld(2, 18)
	#define DRAMC_DPY_CLK_SW_CON_SW_DDRPHY_FB_CK_EN Fld(2, 20)
	#define DRAMC_DPY_CLK_SW_CON_SC_DMDRAMCSHU_ACK Fld(2, 24)
	#define DRAMC_DPY_CLK_SW_CON_SC_EMI_CLK_OFF_ACK Fld(2, 26)
	#define DRAMC_DPY_CLK_SW_CON_SC_DR_SHORT_QUEUE_ACK Fld(2, 28)
	#define DRAMC_DPY_CLK_SW_CON_SC_DRAMC_DFS_STA Fld(2, 30)
#define SPM_S1_MODE_CH (SPM_BASE_ADDRESS + 0x0468)
	#define S1_MODE_CH_SPM_S1_MODE_CH Fld(2, 0)
	#define S1_MODE_CH_S1_EMI_CK_SWITCH Fld(2, 8)
#define SPM_DRAMC_DPY_CLK_SW_CON_SEL2 (SPM_BASE_ADDRESS + 0x0470)
	#define DRAMC_DPY_CLK_SW_CON_SEL2_FULL Fld(32, 0)
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_PHYPLL_SHU_EN_SEL Fld(1, 0)//[0:0]
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_PHYPLL2_SHU_EN_SEL Fld(1, 1)//[1:1]
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_PHYPLL_MODE_SW_SEL Fld(1, 2)//[2:2]
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_PHYPLL2_MODE_SW_SEL Fld(1, 3)//[3:3]
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_DR_SHORT_QUEUE_SEL Fld(1, 4)//[4:4]
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_DR_SHU_EN_SEL Fld(1, 5)//[5:5]
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_DR_SHU_LEVEL_SEL Fld(1, 6)//[6:6]
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_DPY_BCLK_ENABLE_SEL Fld(2, 8)
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_SHU_RESTORE_SEL Fld(2, 10)
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_DPHY_PRECAL_UP_SEL Fld(2, 12)
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_DPHY_RXDLY_TRACK_EN_SEL Fld(2, 14)
	#define DRAMC_DPY_CLK_SW_CON_SEL2_SW_TX_TRACKING_DIS_SEL Fld(2, 16)
#define SPM_DRAMC_DPY_CLK_SW_CON2 (SPM_BASE_ADDRESS + 0x0474)
	#define DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL_SHU_EN Fld(1, 0)//[0:0]
	#define DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL2_SHU_EN Fld(1, 1)//[1:1]
	#define DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL_MODE_SW Fld(1, 2)//[2:2]
	#define DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL2_MODE_SW Fld(1, 3)//[3:3]
	#define DRAMC_DPY_CLK_SW_CON2_SW_DR_SHORT_QUEUE Fld(1, 4)//[4:4]
	#define DRAMC_DPY_CLK_SW_CON2_SW_DR_SHU_EN Fld(1, 5)//[5:5]
	#define DRAMC_DPY_CLK_SW_CON2_SW_DR_SHU_LEVEL Fld(2, 6)
	#define DRAMC_DPY_CLK_SW_CON2_SW_DPY_BCLK_ENABLE Fld(2, 8)
	#define DRAMC_DPY_CLK_SW_CON2_SW_SHU_RESTORE Fld(2, 10)
	#define DRAMC_DPY_CLK_SW_CON2_SW_DPHY_PRECAL_UP Fld(2, 12)
	#define DRAMC_DPY_CLK_SW_CON2_SW_DPHY_RXDLY_TRACK_EN Fld(2, 14)
	#define DRAMC_DPY_CLK_SW_CON2_SW_TX_TRACKING_DIS Fld(2, 16)
#define SPM_DRAMC_DMYRD_CON (SPM_BASE_ADDRESS + 0x0478)
	#define SPM_DRAMC_DMYRD_EN_MOD_SEL_CH1 Fld(1, 10)//[10:10]
	#define SPM_DRAMC_DMYRD_INTV_SEL_CH1 Fld(1, 9)//[9:9]
	#define SPM_DRAMC_DMYRD_EN_CH1 Fld(1, 8)//[8:8]
	#define SPM_DRAMC_DMYRD_EN_MOD_SEL_CH0 Fld(1, 2)//[2:2]
	#define SPM_DRAMC_DMYRD_INTV_SEL_CH0 Fld(1, 1)//[1:1]
	#define SPM_DRAMC_DMYRD_EN_CH0 Fld(1, 0)//[0:0]
#endif

/*========== Page Breaker ==================*/
#if 1
#define EMI_APB_BASE    (IO_PHYS + 0x219000)
#define CHN0_EMI_BASE   (IO_PHYS + 0x22f000)
#define CHN1_EMI_BASE   (IO_PHYS + 0x239000)
#define EMI_MPU_BASE	(IO_PHYS + 0x226000)
#else
#define EMI_APB_BASE    0x10219000
#define CHN0_EMI_BASE   0x1022f000
#define CHN1_EMI_BASE   0x10239000
#endif


#define EMI_CONA        (EMI_APB_BASE + 0x000)
#define EMI_CONB        (EMI_APB_BASE + 0x008)
#define EMI_CONC        (EMI_APB_BASE + 0x010)
#define EMI_COND        (EMI_APB_BASE + 0x018)
#define EMI_CONE        (EMI_APB_BASE + 0x020)
#define EMI_CONF        (EMI_APB_BASE + 0x028)
#define EMI_CONG        (EMI_APB_BASE + 0x030)
#define EMI_CONH        (EMI_APB_BASE + 0x038)
#define EMI_CONH_2ND    (EMI_APB_BASE + 0x03c)
#define EMI_CONI        (EMI_APB_BASE + 0x040)
#define EMI_CONJ        (EMI_APB_BASE + 0x048)
#define EMI_CONM        (EMI_APB_BASE + 0x060)
#define EMI_CONN        (EMI_APB_BASE + 0x068)
#define EMI_CONO        (EMI_APB_BASE + 0x070)
#define EMI_MDCT        (EMI_APB_BASE + 0x078)
#define EMI_MDCT_2ND        (EMI_APB_BASE + 0x07c)
#define EMI_IOCL        (EMI_APB_BASE + 0x0d0)
#define EMI_IOCL_2ND        (EMI_APB_BASE + 0x0d4)
#define EMI_IOCM        (EMI_APB_BASE + 0x0d8)
#define EMI_IOCM_2ND        (EMI_APB_BASE + 0x0dc)
#define EMI_TESTB       (EMI_APB_BASE + 0x0e8)
#define EMI_TESTC       (EMI_APB_BASE + 0x0f0)
#define EMI_TESTD       (EMI_APB_BASE + 0x0f8)
#define EMI_SLVA        (EMI_APB_BASE + 0x800)
#define EMI_ARBA        (EMI_APB_BASE + 0x100)
#define EMI_ARBB        (EMI_APB_BASE + 0x108)
#define EMI_ARBC        (EMI_APB_BASE + 0x110)
#define EMI_ARBD        (EMI_APB_BASE + 0x118)
#define EMI_ARBE        (EMI_APB_BASE + 0x120)
#define EMI_ARBE_2ND    (EMI_APB_BASE + 0x124)
#define EMI_ARBF        (EMI_APB_BASE + 0x128)
#define EMI_ARBG        (EMI_APB_BASE + 0x130)
#define EMI_ARBH        (EMI_APB_BASE + 0x138)
#define EMI_ARBI        (EMI_APB_BASE + 0x140)
#define EMI_ARBI_2ND    (EMI_APB_BASE + 0x144)
#define EMI_ARBJ        (EMI_APB_BASE + 0x148)
#define EMI_ARBJ_2ND    (EMI_APB_BASE + 0x14c)
#define EMI_ARBK        (EMI_APB_BASE + 0x150)
#define EMI_ARBK_2ND    (EMI_APB_BASE + 0x154)
#define EMI_SLCT        (EMI_APB_BASE + 0x158)
#define EMI_BMEN        (EMI_APB_BASE + 0x400)
#define EMI_SHF0        (EMI_APB_BASE + 0x710)
#define EMI_SHF1        (EMI_APB_BASE + 0x718)
#define EMI_CLUA        (EMI_APB_BASE + 0x71c)
#define EMI_BWCT0       (EMI_APB_BASE + 0x5B0)
#define EMI_BWCT1       (EMI_APB_BASE + 0x5B4)
#define EMI_BWCT2       (EMI_APB_BASE + 0x5B8)
#define EMI_BWCT3       (EMI_APB_BASE + 0x5BC)
#define EMI_BWCT4       (EMI_APB_BASE + 0x5C0)
#define EMI_BWST0       (EMI_APB_BASE + 0x5C4)
#define EMI_BWCT0_3RD   (EMI_APB_BASE + 0x770)
#define EMI_BWCT0_4TH   (EMI_APB_BASE + 0x780)
#define EMI_BWCT0_5TH   (EMI_APB_BASE + 0x7B0)

#define CHN_EMI_CONA(base)  (base + 0x000)
#define CHN_EMI_CONB(base)  (base + 0x008)
#define CHN_EMI_CONC(base)  (base + 0x010)
#define CHN_EMI_MDCT(base)  (base + 0x018)
#define CHN_EMI_COND(base)  (base + 0x020)
#define CHN_EMI_TESTB(base) (base + 0x048)
#define CHN_EMI_TESTC(base) (base + 0x050)
#define CHN_EMI_TESTD(base) (base + 0x058)
#define CHN_EMI_MD_PRE_MASK(base) (base + 0x080)
#define CHN_EMI_MD_PRE_MASK_SHF(base) (base + 0x088)
#define CHN_EMI_AP_ERALY_CKE(base) (base + 0x090)
#define CHN_EMI_DQFR(base) (base + 0x098)
#define CHN_EMI_ARBI(base)  (base + 0x140)
#define CHN_EMI_ARBI_2ND(base)  (base + 0x144)
#define CHN_EMI_ARBJ(base)  (base + 0x148)
#define CHN_EMI_ARBJ_2ND(base)  (base + 0x14c)
#define CHN_EMI_ARBK(base)  (base + 0x150)
#define CHN_EMI_ARBK_2ND(base)  (base + 0x154)
#define CHN_EMI_SLCT(base)  (base + 0x158)
#define CHN_EMI_ARB_REF(base)   (base + 0x15c)
#define CHN_EMI_DRS_MON0(base)   (base + 0x16c)
#define CHN_EMI_DRS_MON1(base)   (base + 0x170)
#define CHN_EMI_RKARB0(base)   (base + 0x1b0)
#define CHN_EMI_RKARB1(base)   (base + 0x1b4)
#define CHN_EMI_RKARB2(base)   (base + 0x1b8)
#define CHN_EMI_SHF0(base)   (base + 0x710)
#define CHN_EMI_BISTC(base) (base + 0xa08)

#define EMI_MPU_CTRL                 (EMI_MPU_BASE+0x000)
#define EMI_MPU_DBG                  (EMI_MPU_BASE+0x004)
#define EMI_MPU_SA0                  (EMI_MPU_BASE+0x100)
#define EMI_MPU_EA0                  (EMI_MPU_BASE+0x200)
#define EMI_MPU_SA(region)           (EMI_MPU_SA0 + (region*4))
#define EMI_MPU_EA(region)           (EMI_MPU_EA0 + (region*4))
#define EMI_MPU_APC0                 (EMI_MPU_BASE+0x300)
#define EMI_MPU_APC(region, domain)  (EMI_MPU_APC0 + (region*4) + ((domain/8)*0x100))

