#include "emi.h"
#include "dramc_common.h"
#include "dramc_api.h"
#include "x_hal_io.h"

#ifdef DDR_RESERVE_MODE
#define LP3_CKE_FIX_ON_TO_LEAVE_SR_WITH_LVDRAM 0

extern DRAMC_CTX_T *ps_curr_dram_ctx;
extern DRAMC_CTX_T *dramc_ctx_lp4;
extern DRAMC_CTX_T *dramc_ctx_lp3;
extern DRAMC_CTX_T *dramc_ctx_ddr4;
extern DRAMC_CTX_T *dramc_ctx_ddr3;

extern u32 g_ddr_reserve_enable;
extern u32 g_ddr_reserve_success;
extern u32 g_ddr_reserve_ready;
u32 g_ddr_reserve_ta_err;

static u32 reserve_reg_backup_address[] =
{
    (DRAMC_REG_SPCMDCTRL),
    (DRAMC_REG_SPCMDCTRL + SHIFT_TO_CHB_ADDR),
    (DRAMC_REG_SHU_SCINTV),
    (DRAMC_REG_SHU_SCINTV + SHIFT_TO_CHB_ADDR),
    (DRAMC_REG_DQSOSCR),
    (DRAMC_REG_DQSOSCR + SHIFT_TO_CHB_ADDR),
    (DRAMC_REG_DUMMY_RD),
    (DRAMC_REG_DUMMY_RD + SHIFT_TO_CHB_ADDR),
    (DRAMC_REG_DRAMC_PD_CTRL),
    (DRAMC_REG_DRAMC_PD_CTRL + SHIFT_TO_CHB_ADDR)
};

static u32 dram_type = 0;

#if LP3_CKE_FIX_ON_TO_LEAVE_SR_WITH_LVDRAM
#if ENABLE_LP3_SW
static u32 u4Cha_cke_backup = 0;
static u32 u4Chb_cke_backup = 0;
#endif
#endif

static int reserve_sync_writel(u32 addr, unsigned int val)
{
	(*(volatile unsigned int *)(addr)) = val;
	dsb();
	return 0;
}

static unsigned int reserve_reg_readl(u32 addr)
{
	return (*(volatile unsigned int *)(addr));
}

static void set_spm_poweron_config_en(DRAMC_CTX_T * p)
{
    u32 value = 0;

	spm_pinmux_setting(p);

	/*
	 * ! set  sc_dpy_2nd_dll_en, sc_dpy_dll_en, sc_dpy_dll_ck_en ,
	 * sc_dpy_vref_en , sc_phypll_en = 1
	 */
	io_32_write_fld_multi(SPM_POWER_ON_VAL0,
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_DPY_2ND_DLL_EN_PCM) |
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_DPY_DLL_EN_PCM) |
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_DPY_DLL_CK_EN_PCM) |
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_DPY_VREF_EN_PCM) |
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_PHYPLL_EN_PCM));

    return;
}

static void dump_SR(DRAMC_CTX_T * p, U32 dram_type)
{
    U32 u4value = 0;

    u4value = io_32_read_fld_align(DRAMC_REG_MISC_STATUSA, MISC_STATUSA_SREF_STATE);
    show_err2("!CHA SREF_STATE[0x%X]\n", u4value);
    if((dram_type == TYPE_LPDDR4) || (dram_type == TYPE_LPDDR4X) || (dram_type == TYPE_PCDDR4))
    {
        u4value = io_32_read_fld_align(DRAMC_REG_MISC_STATUSA + SHIFT_TO_CHB_ADDR, MISC_STATUSA_SREF_STATE);
        show_err2("!CHB SREF_STATE[0x%X]\n", u4value);
    }
    return;
}

static void dump_gating_error_rg(DRAMC_CTX_T * p, u32 dram_type)
{
    u32 value = io32_read_4b(DDRPHY_MISC_STBERR_RK0_R);

    show_err2("###DDR reserve CHA Gating STBERR[0x%X]\n", value);
    if((dram_type == TYPE_LPDDR4) || (dram_type == TYPE_LPDDR4X) || (dram_type == TYPE_PCDDR4))
    {
        value = io32_read_4b(DDRPHY_MISC_STBERR_RK0_R + SHIFT_TO_CHB_ADDR);
        show_err2("###DDR reserve CHB Gating STBERR[0x%X]\n", value);
    }
    return;
}

#define RESERVE_PDEF_SPM_PLL_CON (0x1000644C)
static void dramc_ddr_reserved_mode_setting(void)
{
	u32 wbr_backup = 0;
	u32 u4value = 0, timeout;
	u32 backup_broadcast = get_dramc_broadcast();
	DRAMC_CTX_T * p;

	show_msg((INFO, "Enter %s \n", __func__));
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	dram_type = io_32_read_fld_align(DRAMC_REG_ARBCTL, ARBCTL_RSV_DRAM_TYPE);
	show_msg((INFO, "%s dram type =%x \n", __func__, dram_type));

	switch (dram_type) {
		case TYPE_LPDDR4X:
		case TYPE_LPDDR4P:
		case TYPE_LPDDR4:
			ps_curr_dram_ctx = dramc_ctx_lp4;
			break;
		case TYPE_LPDDR3:
			ps_curr_dram_ctx = dramc_ctx_lp3;
			break;
		case TYPE_PCDDR4:
			ps_curr_dram_ctx = dramc_ctx_ddr4;
			break;
		case TYPE_PCDDR3:
			ps_curr_dram_ctx = dramc_ctx_ddr3;
			break;
		default:
			show_err("[Error] Unrecognized type\n");
			break;
	}

	p = ps_curr_dram_ctx;
	set_spm_poweron_config_en(p);
	u4value = io_32_read_fld_align(DRAMC_REG_SHUSTATUS, SHUSTATUS_SHUFFLE_LEVEL);
	show_err2("### shuffle level[%d]\n", u4value);
#if 0//ENABLE_DVFS_CDC_SYNCHRONIZER_OPTION
	if(io_32_read_fld_align(DRAMC_REG_SHUCTRL2, SHUCTRL2_R_DVFS_CDC_OPTION))
	{
		DDR_Reserved_Mode_Cdc_Option_Patch(p);
	}
#endif
#ifdef HW_GATING
	dramc_hw_gating_on_off(p, 0);//Disable HW Gating tracking for gating tracking fifo mode
#endif

	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_SPM_DVFS_CONTROL_SEL);//change DVFS to RG mode

	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL_SHU_EN);//toggle shu_en to sync shu_level, need MISC_SPM_CTRL1_SPM_DVFS_CONTROL_SEL = 1
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL2_SHU_EN);//toggle shu_en to sync shu_level
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL_SHU_EN);//toggle shu_en to sync shu_level
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL2_SHU_EN);//toggle shu_en to sync shu_level
	//io_32_write_fld_multi(SPM_POWER_ON_VAL0, p_fld(1, SPM_POWER_ON_VAL0_SC_PHYPLL_SHU_EN_PCM)
	//			  | p_fld(1, SPM_POWER_ON_VAL0_SC_PHYPLL2_SHU_EN_PCM));//Set spm shuffle enable to 1
	//udelay(1);
	//io_32_write_fld_multi(SPM_POWER_ON_VAL0, p_fld(0, SPM_POWER_ON_VAL0_SC_PHYPLL_SHU_EN_PCM)
	//			  | p_fld(0, SPM_POWER_ON_VAL0_SC_PHYPLL2_SHU_EN_PCM));//Set spm shuffle enable to 0

	udelay(1);
	//! toggle hsu restore
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, 1, SHUCTRL2_R_SHU_RESTORE);
	udelay(1);
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, 0, SHUCTRL2_R_SHU_RESTORE);
	udelay(1);

	if(dram_type == TYPE_LPDDR3 || dram_type == TYPE_PCDDR3)//LPDDR3
	{
		io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0 + SHIFT_TO_CHB_ADDR, 0, MISC_CG_CTRL0_RG_CG_DRAMC_CHB_CK_OFF);//Open CHB controller clk
	}
	//Backup regs
	dramc_backup_registers(p, reserve_reg_backup_address, sizeof(reserve_reg_backup_address)/sizeof(u32));
	//Disable MR4
	io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL, 1, SPCMDCTRL_REFRDIS);
	//Disable DQSOSC en
	io_32_write_fld_align_all(DRAMC_REG_SHU_SCINTV, 1, SHU_SCINTV_DQSOSCENDIS);
	//Disable DQSOSC rd
	io_32_write_fld_align_all(DRAMC_REG_DQSOSCR, 1, DQSOSCR_DQSOSCRDIS);
	//Disable Dummy Read
	io_32_write_fld_multi_all(DRAMC_REG_DUMMY_RD, p_fld(0x0, DUMMY_RD_DQSG_DMYWR_EN)
			| p_fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
			| p_fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
			| p_fld(0x0, DUMMY_RD_DUMMY_RD_EN)
			| p_fld(0x0, DUMMY_RD_DMY_RD_DBG)
			| p_fld(0x0, DUMMY_RD_DMY_WR_DBG));
	//Disable DDRPHY dynamic clock gating
	io_32_write_fld_align_all(DRAMC_REG_DRAMC_PD_CTRL, 0, DRAMC_PD_CTRL_PHYCLKDYNGEN);//disable DDRPHY dynamic clock gating

	//To 26MHz
	if((dram_type == TYPE_LPDDR4) || (dram_type == TYPE_LPDDR4X))
	{
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL0, p_fld(0, MISC_CG_CTRL0_W_CHG_MEM)
					| p_fld(0, MISC_CG_CTRL0_CLK_MEM_SEL));//[5:4] mem_ck mux: 2'b00: 26MHz, [0]: change memory clock
		io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, 1, MISC_CG_CTRL0_W_CHG_MEM);//change clock freq
		udelay(1);
		io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_W_CHG_MEM);//disable memory clock change
	}
	else//LP3
	{
#if LP3_CKE_FIX_ON_TO_LEAVE_SR_WITH_LVDRAM
#if ENABLE_LP3_SW
		//Dram clock free run
		io_32_write_fld_align_all(DRAMC_REG_DRAMC_PD_CTRL, 1, DRAMC_PD_CTRL_MIOCKCTRLOFF);//disable DDRPHY dynamic clock gating

		//CKE control
		u4Cha_cke_backup = io32_read_4b(DRAMC_REG_CKECTRL);
		u4Chb_cke_backup = io32_read_4b(DRAMC_REG_CKECTRL + SHIFT_TO_CHB_ADDR);
		//CKE fix on
		io_32_write_fld_multi_all(DRAMC_REG_CKECTRL, p_fld(0x0, CKECTRL_CKE1FIXOFF) |
					p_fld(0x0, CKECTRL_CKEFIXOFF) |
					p_fld(0x1, CKECTRL_CKE1FIXON) |
					p_fld(0x1, CKECTRL_CKEFIXON));
#endif
#endif
		io_32_write_fld_multi(DDRPHY_MISC_CG_CTRL0, p_fld(0, MISC_CG_CTRL0_W_CHG_MEM)
					| p_fld(0, MISC_CG_CTRL0_CLK_MEM_SEL));//[5:4] mem_ck mux: 2'b00: 26MHz, [0]: change memory clock
		io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, 1, MISC_CG_CTRL0_W_CHG_MEM);//change clock freq
		udelay(1);
		io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_W_CHG_MEM);//disable memory clock change
	}

	//RG_*PHDET_EN=0 (DLL)
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI2, 0x0, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI2, 0x0, B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI2, 0x0, B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI2, 0x0, B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2);

	//*PI_CG=1, RG_*MPDIV_CG=1
	io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI2, p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0)
				| p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0)
				| p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0)
				| p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0)
				| p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0)
				| p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0)
				| p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0)
				| p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0)
				| p_fld(0x1, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI2, p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1)
				| p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1)
				| p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1)
				| p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1)
				| p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1)
				| p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1)
				| p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1)
				| p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1)
				| p_fld(0x1, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI2, p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_CG_MCK_B2)
				| p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B2)
				| p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_CG_MCTL_B2)
				| p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_CG_FB_B2)
				| p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_CG_DQS_B2)
				| p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_CG_DQM_B2)
				| p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_CG_DQ_B2)
				| p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2)
				| p_fld(0x1, B2_DLL_ARPI2_RG_ARPI_MPDIV_CG_B2));
	io_32_write_fld_multi_all(DDRPHY_CA_DLL_ARPI2, p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)
				| p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
				| p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
				| p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
				| p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CS)
				| p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CLK)
				| p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CMD)
				| p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)
				| p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));

	//RG_*BIAS_EN=0
	//io_32_write_fld_align_all(DDRPHY_B0_DQ6, 0x0, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0);
	//io_32_write_fld_align_all(DDRPHY_B1_DQ6, 0x0, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1);
	//io_32_write_fld_align_all(DDRPHY_CA_CMD6, 0x0, CA_CMD6_RG_RX_ARCMD_BIAS_EN);

	//RG_*VREF_EN=0
	io_32_write_fld_align_all(DDRPHY_B0_DQ5, 0x0, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ5, 0x0, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ5, 0x0, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD5, 0x0, CA_CMD5_RG_RX_ARCMD_VREF_EN);

	///TODO: only CHA?
	//RG_*MCK8X_EN=0
	io_32_write_fld_multi(DDRPHY_PLL4, p_fld(0, PLL4_RG_RPHYPLL_ADA_MCK8X_EN)
					| p_fld(0, PLL4_RG_RPHYPLL_AD_MCK8X_EN));//RG_*MCK8X_EN=0; Since there is only 1 PLL, only to control CHA
	//RG_*MIDPI_EN=0 RG_*MIDPI_CKDIV4_EN=0
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ6, p_fld(0, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0) | p_fld(0, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ6, p_fld(0, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1) | p_fld(0, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ6, p_fld(0, SHU_B2_DQ6_RG_ARPI_MIDPI_EN_B2) | p_fld(0, SHU_B2_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B2));
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD6, p_fld(0, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA) | p_fld(0, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA));
	//io_32_write_fld_align_all(DDRPHY_SHU1_B0_DQ6, 0x0, SHU1_B0_DQ6_RG_ARPI_MIDPI_EN_B0);
	//io_32_write_fld_align_all(DDRPHY_SHU1_B1_DQ6, 0x0, SHU1_B1_DQ6_RG_ARPI_MIDPI_EN_B1);
	//io_32_write_fld_align_all(DDRPHY_SHU1_CA_CMD6, 0x0, SHU1_CA_CMD6_RG_ARPI_MIDPI_EN_CA);

	io_32_write_fld_align(DDRPHY_PLL1, 0x0, PLL1_RG_RPHYPLL_EN); //RG_*PLL_EN=0; Since there is only 1 PLL, only control CHA
	io_32_write_fld_align(DDRPHY_PLL2, 0x0, PLL2_RG_RCLRPLL_EN); //RG_*PLL_EN=0; Since there is only 1 PLL, only control CHA

	//!set sc_mpll to SPM register //Chen-Hsiang modify @20170316
	//u4value = reserve_reg_readl(RESERVE_PDEF_SPM_PLL_CON);
	//reserve_sync_writel(RESERVE_PDEF_SPM_PLL_CON, u4value | (0x1 << 8) | (0x1 << 4));//set sc_mpll_off=1 , sc_mpll_s_off=1
	//TBD
	//*((UINT32P)(0x1000631c )) |= (0x1 << 1); //ddrphy_pwr_iso=1 //Lewis@20160621: Fix LP3 Hang from S0 suspend into reserve mode
	//*((UINT32P)(0x100063b8 )) |= (0x1 << 1);

	//RG_*RESETB=0
	io_32_write_fld_align_all(DDRPHY_B0_DQ3, 0x0, B0_DQ3_RG_ARDQ_RESETB_B0);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI0, 0x0, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ3, 0x0, B1_DQ3_RG_ARDQ_RESETB_B1);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI0, 0x0, B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ3, 0x0, B2_DQ3_RG_ARDQ_RESETB_B2);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI0, 0x0, B2_DLL_ARPI0_RG_ARPI_RESETB_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD3, 0x0, CA_CMD3_RG_ARCMD_RESETB);
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI0, 0x0, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
	io_32_write_fld_align(DDRPHY_PLL4, 0x0, PLL4_RG_RPHYPLL_RESETB);//Since there is only 1 PLL, only control CHA
	//Lewis@20160628: Fix LP3 enter S0 then into reserve mode fail due to CHB PHY not reset(LP3 PHY use 2 channels)

	mpll_init();

	//marked by kaihsin on May 7th, not necessary to PDN DDRPHY
	//*((UINT32P)(0x1000631c )) &= ~(0x1 << 2); //ddrphy_pwr_on=0
	//*((UINT32P)(0x100063b8 )) &= ~(0x1 << 2);

	// wait 1us
	udelay(1);

	//marked by kaihsin on May 7th, not necessary to PDN DDRPHY
	//*((UINT32P)(0x1000631c )) |= (0x1 << 2); //ddrphy_pwr_on=1
	//*((UINT32P)(0x100063b8 )) |= (0x1 << 2);
	// wait 1us
	//udelay(1);

	//RG_*RESETB=1
	io_32_write_fld_align_all(DDRPHY_B0_DQ3, 0x1, B0_DQ3_RG_ARDQ_RESETB_B0);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI0, 0x1, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ3, 0x1, B1_DQ3_RG_ARDQ_RESETB_B1);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI0, 0x1, B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ3, 0x1, B2_DQ3_RG_ARDQ_RESETB_B2);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI0, 0x1, B2_DLL_ARPI0_RG_ARPI_RESETB_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD3, 0x1, CA_CMD3_RG_ARCMD_RESETB);
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI0, 0x1, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
	io_32_write_fld_align(DDRPHY_PLL4, 0x1, PLL4_RG_RPHYPLL_RESETB);//Since there is only 1 PLL, only control CHA

	//TBD
	//*((UINT32P)(0x1000631c )) &= ~(0x1 << 1); //ddrphy_pwr_iso=0 //Lewis@20160621: Fix LP3 Hang from S0 suspend into reserve mode
	//*((UINT32P)(0x100063b8 )) &= ~(0x1 << 1);

	u4value = reserve_reg_readl(RESERVE_PDEF_SPM_PLL_CON);
	reserve_sync_writel(RESERVE_PDEF_SPM_PLL_CON, u4value & ~((0x1 << 8) | (0x1 << 4)));//set sc_mpll_off=0 , sc_mpll_s_off=0
	udelay(20);

	//RG_*PLL_EN=1
	io_32_write_fld_align(DDRPHY_PLL1, 0x1, PLL1_RG_RPHYPLL_EN); //RG_*PLL_EN=1; Since there is only 1 PLL, only control CHA
	io_32_write_fld_align(DDRPHY_PLL2, 0x1, PLL2_RG_RCLRPLL_EN); //RG_*PLL_EN=1; Since there is only 1 PLL, only control CHA

	 //Wait 20us for MEMPLL
	 udelay(20);

	//RG_*VREF_EN=1 (Vref is only used in read, DQ(B0, B1)is use for RX)
	if((dram_type == TYPE_LPDDR4) || (dram_type == TYPE_LPDDR4X) || (dram_type == TYPE_PCDDR4))
	{
		io_32_write_fld_align_all(DDRPHY_B0_DQ5, 0x1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
		io_32_write_fld_align_all(DDRPHY_B1_DQ5, 0x1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
		io_32_write_fld_align_all(DDRPHY_B2_DQ5, 0x1, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2);
	}
	else
	{
		io_32_write_fld_align_all(DDRPHY_B0_DQ5, 0x1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
		io_32_write_fld_align_all(DDRPHY_B1_DQ5, 0x1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
		io_32_write_fld_align_all(DDRPHY_B2_DQ5, 0x1, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2);
		io_32_write_fld_align_all(DDRPHY_CA_CMD5, 0, CA_CMD5_RG_RX_ARCMD_VREF_EN);
	}
	//RG_*MCK8X_EN=1
	io_32_write_fld_align(DDRPHY_PLL4, 1, PLL4_RG_RPHYPLL_ADA_MCK8X_EN);//RG_*MCK8X_EN=1; Since there is only 1 PLL, only to control CHA

	//wait 1us
	udelay(1);

	if((dram_type == TYPE_LPDDR4) || (dram_type == TYPE_LPDDR4X) || (dram_type == TYPE_PCDDR4))
	{

#if DUAL_FREQ_K
				io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6, 0x1, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0);
				io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6, 0x1, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1);
				io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6, 0x1, SHU_B2_DQ6_RG_ARPI_MIDPI_EN_B2);
				io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD6, 0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA);

#else
		show_msg((INFO, "p->frequency = %d\n", p->frequency));

		if (p->frequency > DDR1866_FREQ)
		{
				io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6, 0x1, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0);
				io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6, 0x1, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1);
				io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6, 0x1, SHU_B2_DQ6_RG_ARPI_MIDPI_EN_B2);
				io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD6, 0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA);

		}
		else
		{
				io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6, 0x1, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0);
				io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6, 0x1, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1);
				io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6, 0x1, SHU_B2_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B2);
				io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD6, 0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA);
		}

#endif
		//wait 1us
		udelay(1);

		//*PI_CG=0, RG_*MPDIV_CG=0
		io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI2, p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
		io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI2, p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
		io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI2, p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCK_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCTL_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_FB_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQS_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQM_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQ_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_MPDIV_CG_B2));
		io_32_write_fld_multi_all(DDRPHY_CA_DLL_ARPI2, p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)//not include 11th bit(CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CS)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLK)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CMD)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	}
	else
	{
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6, 0x1, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6, 0x1, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6, 0x1, SHU_B2_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B2);
		io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD6, 0x1, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA);

		//wait 1us
		udelay(1);

		//*PI_CG=0, RG_*MPDIV_CG=0
		io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI2, p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0)
					| p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
		io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI2, p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1)
					| p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
		io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI2, p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCK_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCTL_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_FB_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQS_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQM_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQ_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2)
					| p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_MPDIV_CG_B2));

		io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI2, p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)//not include 11th bit(CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CS)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLK)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CMD)
				   // | p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)//11th
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
		io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI2 + SHIFT_TO_CHB_ADDR, p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA)//not include 11th bit(CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CS)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLK)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CMD)
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN)//11th
					| p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	}

	//RG_*BIAS_EN=1
	//io_32_write_fld_align_all(DDRPHY_B0_DQ6, 0x0, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0);//Chen-Hsiang modify @20170316
	//io_32_write_fld_align_all(DDRPHY_B1_DQ6, 0x0, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1);//Chen-Hsiang modify @20170316

	// wait 1us
	udelay(1);

	if((dram_type == TYPE_LPDDR4) || (dram_type == TYPE_LPDDR4X) || (dram_type == TYPE_PCDDR4))
	{
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL0, p_fld(0, MISC_CG_CTRL0_W_CHG_MEM)
					| p_fld(1, MISC_CG_CTRL0_CLK_MEM_SEL));//[5:4] mem_ck mux: 2'b01: memory clock, [0]: change memory clock
		io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, 1, MISC_CG_CTRL0_W_CHG_MEM);//change clock freq
		udelay(1);
		io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_W_CHG_MEM);//disable memory clock change
	}
	else
	{
		io_32_write_fld_multi(DDRPHY_MISC_CG_CTRL0, p_fld(0, MISC_CG_CTRL0_W_CHG_MEM)
					| p_fld(1, MISC_CG_CTRL0_CLK_MEM_SEL));//[5:4] mem_ck mux: 2'b01: memory clock, [0]: change memory clock
		io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, 1, MISC_CG_CTRL0_W_CHG_MEM);//change clock freq
		udelay(1);
		io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_W_CHG_MEM);//disable memory clock change
	}

	//force top feedback MCK not divide
	//rg_ddrphy_fb_ck_force_en = 1 -- TBD
	//*CLK_MEM_DFS_CFG |= (0x1 << 8); //rg_ddrphy_fb_ck_force_en = 1
	//u4value = reserve_reg_readl(0x10060464);
	//reserve_sync_writel(0x10060464, u4value | (0x3 << 20)); //set sc_ddrphy_fb_ck_ch*_en = 1
	//u4value = reserve_reg_readl(0x10060004);
	//reserve_sync_writel(0x10060004, u4value | (0x1 << 16)); //set sc_ddrphy_fb_ck_ch*_en = 1
	//*DRAMC_WBR = 0x3;
	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1, p_fld(1, MISC_SPM_CTRL1_RG_DDRPHY_DB_CK_CH1_EN)
		| p_fld(1, MISC_SPM_CTRL1_RG_DDRPHY_DB_CK_CH0_EN));//clock freerun

	//1st DLL enable
	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI2, 0x1, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);//Since CHA CA is DLL master
	// wait 1us
	udelay(1);
	//2nd DLL enable
	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI2 + SHIFT_TO_CHB_ADDR, 0x1, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI2, 0x1, B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);//Chen-Hsiang modify @20170316
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI2, 0x1, B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);//Chen-Hsiang modify @20170316
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI2, 0x1, B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2);//Chen-Hsiang modify @20170316
	// wait 1us
	udelay(1);

	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1, p_fld(0, MISC_SPM_CTRL1_RG_DDRPHY_DB_CK_CH1_EN)
		| p_fld(0, MISC_SPM_CTRL1_RG_DDRPHY_DB_CK_CH0_EN));//disable clock freerun
	//top feedback MCK to divided frequency -- TBD
	//*CLK_MEM_DFS_CFG &= ~(0x1 << 8); //rg_ddrphy_fb_ck_force_en = 0
	//u4value = reserve_reg_readl(0x10060464);
	//reserve_sync_writel(0x10060464, u4value & ~(0x3 << 20)); //set sc_ddrphy_fb_ck_ch*_en = 0
	//u4value = reserve_reg_readl(0x10060004);
	//reserve_sync_writel(0x10060004, u4value & ~(0x1 << 16)); //set sc_ddrphy_fb_ck_ch*_en = 0

	io_32_write_fld_align_all(DRAMC_REG_DDRCONF0, 1, DDRCONF0_RDATRST);//R_DMRDATRST = 1
	io_32_write_fld_align_all(DRAMC_REG_DDRCONF0, 0, DDRCONF0_RDATRST);//R_DMRDATRST = 0

	//! set SPM to control PLL enable and disable PLL enable from ddrphy conf
	/*TINFO ="Switching PHYPLL enable path from DDRPHY to SPM control by setting SPM SW" */
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_SPM_DVFS_CONTROL_SEL);//change DVFS to SPM mode^M
	udelay(1);

	io_32_write_fld_align(SPM_POWER_ON_VAL0, 1, SPM_POWER_ON_VAL0_SC_PHYPLL_MODE_SW_PCM);
	u4value = reserve_reg_readl(SPM_DRAMC_DPY_CLK_SW_CON2);
	reserve_sync_writel(SPM_DRAMC_DPY_CLK_SW_CON2, u4value | (0x1 << 2));
	udelay(1);
	//RG_*PLL_EN=1
	io_32_write_fld_align(DDRPHY_PLL1, 0x0, PLL1_RG_RPHYPLL_EN); //disable DDRPHY PHYPLL enable, RG_*PLL_EN=0; Since there is only 1 PLL, only control CHA
	io_32_write_fld_align(DDRPHY_PLL2, 0x0, PLL2_RG_RCLRPLL_EN); //disable DDRPHY CLRPLL enable, RG_*PLL_EN=0; Since there is only 1 PLL, only control CHA
	/*TINFO ="Setting RX input delay tracking enable from SPM side(un-paused)" */
	//! speed >= 3200 need enable RX input delay tracking
	if((dram_type == TYPE_LPDDR4) || (dram_type == TYPE_LPDDR4X))
	{
		//u4value = reserve_reg_readl(SPM_DRAMC_DPY_CLK_SW_CON2);
		//reserve_sync_writel(SPM_DRAMC_DPY_CLK_SW_CON2, u4value & ~(0x3 << 16));
		io_32_write_fld_align(SPM_POWER_ON_VAL0, 1, SPM_POWER_ON_VAL0_SC_DPHY_RXDLY_TRACK_EN);
	}
	dramc_broadcast_on_off(backup_broadcast);
	//DDRPhyFreqMeter();
	//dump_gating_error_rg(p, dram_type);
	//dump_SR(p, dram_type);
}

#ifdef USE_TA2_IN_DDR_RESERVE_MODE
static void ta2_test_run_time_hw(DRAMC_CTX_T * p)//notice: Rank number and channel number should handle
{
    DRAM_CHANNEL_T channel_bak = p->channel;
    DRAM_RANK_T rank_bak = p->rank;
    u8 channel_idx = 0;
    u8 rank_idx = 0;
    u32 error_value = 0;
    static u32 err_count = 0;
    static u32 pass_count = 0;

    io_32_write_fld_align_all(DRAMC_REG_TEST2_4, 4, TEST2_4_TESTAGENTRKSEL);//Rank select is controlled By HW
    //io_32_write_fld_align_all(DRAMC_REG_TEST2_2, 0x400, TEST2_2_TEST2_OFF); //Not change, suppose this area is preserved for memeory test
    for(channel_idx = CHANNEL_A; channel_idx < p->support_channel_num; channel_idx++)
    {
        p->channel = channel_idx;
        dramc_engine2_set_pat(p, TEST_XTALK_PATTERN, p->support_rank_num - 1, 0);//TEST2_3_TESTCNT 1: HW switch R0 and R1 automatically 0: only R0
        io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), 1, TEST2_3_TEST2W);//Trigger W
        mdelay(1);
        error_value = dramc_engine2_compare(p, TE_OP_WRITE_READ_CHECK);
        for(rank_idx =0 ; rank_idx < p->support_rank_num; rank_idx++)
        {
            if(error_value & (1<<rank_idx))
            {
                err_count++;
                show_msg((INFO, "HW channel(%d) Rank(%d), TA2 failed, pass_cnt:%d, err_cnt:%d, error_value 0x%x\n", channel_idx, rank_idx, pass_count, err_count, error_value));
            }
            else
            {
                pass_count++;
                show_msg((INFO, "HW channel(%d) Rank(%d), TA2 pass, pass_cnt:%d, err_cnt:%d\n", channel_idx, rank_idx, pass_count, err_count));
            }
        }
        io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), p_fld(0, TEST2_3_TEST2W) | p_fld(0, TEST2_3_TEST2R) | p_fld(0, TEST2_3_TEST1));
    }
    p->channel = channel_bak;
    p->rank = rank_bak;
    return;
}
#endif

static void dramc_ddr_reserved_mode_after_sr(void)
{
    u32 wbr_backup = 0;
    DRAMC_CTX_T * p = ps_curr_dram_ctx; //done in ddr reserve mode setting
    u32 lp3_mr1_wr = 0;
    u32 lp3_mr2_rlwl = 0;
    u32 lp4_mr13_fsp = 0;
    u32 lp4_mr2_rlwl = 0;
    u32 backup_broadcast = get_dramc_broadcast();
    u8 channel = CHANNEL_A;
    bool support_2rank = (get_dram_rank_nr() == 2) ? TRUE : FALSE;

#if MRW_CHECK_ONLY
    show_err2("\n==[MR Dump] %s==\n", __func__);
#endif

    dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);	
    show_err3("%s dram type =%x \n", __func__, dram_type);
    if(dram_type == TYPE_LPDDR3)
    {   //LPDDR3
        io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0 + SHIFT_TO_CHB_ADDR, 1, MISC_CG_CTRL0_RG_CG_DRAMC_CHB_CK_OFF);//Close CHB controller clk

        lp3_mr1_wr = io_32_read_fld_align(DRAMC_REG_SHU_HWSET_MR2, SHU_HWSET_MR2_HWSET_MR2_OP);
        //Since we do MR2 with (RG of Mr13) first while DVFS, we use (RG of MR13) to apply OP of MR2
        lp3_mr2_rlwl = io_32_read_fld_align(DRAMC_REG_SHU_HWSET_MR13, SHU_HWSET_MR13_HWSET_MR13_OP);
        p->channel = CHANNEL_A;
        dramc_mode_reg_write_by_rank(p, 0, 1, lp3_mr1_wr);
        dramc_mode_reg_write_by_rank(p, 0, 2, lp3_mr2_rlwl);
        dramc_mode_reg_write_by_rank(p, 0, 17, 0);//R1 lost data WA, set R0 R1 both for safe{On charging mode, system may issue MR17=0xFF in R1}
        if(support_2rank == TRUE)//DRAM is dual rank
        {
            dramc_mode_reg_write_by_rank(p, 1, 1, lp3_mr1_wr);
            dramc_mode_reg_write_by_rank(p, 1, 2, lp3_mr2_rlwl);
            dramc_mode_reg_write_by_rank(p, 1, 17, 0);//R1 lost data WA, set R0 R1 both for safe{On charging mode, system may issue MR17=0xFF in R1}
        }
    }
    else if((dram_type == TYPE_LPDDR4) || (dram_type == TYPE_LPDDR4X))
    {   //LPDDR4
        lp4_mr13_fsp = io_32_read_fld_align(DRAMC_REG_SHU_HWSET_MR13, SHU_HWSET_MR13_HWSET_MR13_OP);
        lp4_mr2_rlwl = io_32_read_fld_align(DRAMC_REG_SHU_HWSET_MR2, SHU_HWSET_MR2_HWSET_MR2_OP);
        for(channel = CHANNEL_A; channel < p->support_channel_num; channel++)
        {
            p->channel = channel;
            dramc_mode_reg_write_by_rank(p, 0, 13, lp4_mr13_fsp);
            dramc_mode_reg_write_by_rank(p, 0, 2, lp4_mr2_rlwl);
            dramc_mode_reg_write_by_rank(p, 0, 17, 0);//R1 lost data WA, set R0 R1 both for safe{On charging mode, system may issue MR17=0xFF in R1}
            if(support_2rank == TRUE)//DRAM is dual rank
            {
                dramc_mode_reg_write_by_rank(p, 1, 13, lp4_mr13_fsp);
                dramc_mode_reg_write_by_rank(p, 1, 2, lp4_mr2_rlwl);
                dramc_mode_reg_write_by_rank(p, 1, 17, 0);//R1 lost data WA, set R0 R1 both for safe{On charging mode, system may issue MR17=0xFF in R1}
            }
        }
    }
#ifdef USE_TA2_IN_DDR_RESERVE_MODE
    {
        int i = 0;
        for(i=0; i<10; i++)
        {
            ta2_test_run_time_hw(p);
        }
    }
#endif
    //Restore regs
    dramc_restore_registers(p, reserve_reg_backup_address, sizeof(reserve_reg_backup_address)/sizeof(u32));
#if LP3_CKE_FIX_ON_TO_LEAVE_SR_WITH_LVDRAM
#if ENABLE_LP3_SW
    if(dram_type == TYPE_LPDDR3)
    {
        io32_write_4b(DRAMC_REG_CKECTRL, u4Cha_cke_backup);
        io32_write_4b(DRAMC_REG_CKECTRL + SHIFT_TO_CHB_ADDR, u4Chb_cke_backup);
    }
#endif
#endif

#ifdef HW_GATING
    dramc_hw_gating_on_off(p,1);//Enable HW Gating tracking for gating tracking fifo mode
#if GATING_ONLY_FOR_DEBUG
    dramc_hw_gating_debug_on_off(p, 1);
#endif
#endif
    dramc_broadcast_on_off(backup_broadcast);
    dump_gating_error_rg(p, dram_type);
    //DramcRegDump(p);
    return;
}

static void switch_26MHz_disable_dummyread_refresh_allbank(DRAMC_CTX_T *p)
{
    io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_CLK_MEM_SEL);//Switch clk to 26MHz
    io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, 1, MISC_CG_CTRL0_W_CHG_MEM);
    io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_W_CHG_MEM);

    io_32_write_fld_align_all(DRAMC_REG_REFCTRL0, 0, REFCTRL0_PBREFEN);//Switch to all bank refresh

    io_32_write_fld_multi_all(DRAMC_REG_DUMMY_RD, p_fld(0x0, DUMMY_RD_DQSG_DMYWR_EN)//Disable Dummy Read
                | p_fld(0x0, DUMMY_RD_DQSG_DMYRD_EN) | p_fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                | p_fld(0x0, DUMMY_RD_DUMMY_RD_EN) | p_fld(0x0, DUMMY_RD_DMY_RD_DBG)
                | p_fld(0x0, DUMMY_RD_DMY_WR_DBG));
    return;
}

void before_init_dram_while_reserve_mode_fail(DRAM_DRAM_TYPE_T dram_type)
{
    DRAMC_CTX_T * p;
    show_msg((INFO, "\n\tReserve mode fail\tBefore_Init_DRAM_While_Reserve_Mode_fail\n"));

	switch (dram_type) {
		case TYPE_LPDDR4X:
		case TYPE_LPDDR4P:
		case TYPE_LPDDR4:
			ps_curr_dram_ctx = dramc_ctx_lp4;
			break;
		case TYPE_LPDDR3:
			ps_curr_dram_ctx = dramc_ctx_lp3;
			break;
		case TYPE_PCDDR4:
			ps_curr_dram_ctx = dramc_ctx_ddr4;
			break;
		case TYPE_PCDDR3:
			ps_curr_dram_ctx = dramc_ctx_ddr3;
			break;
		default:
			show_err("[Error] Unrecognized type\n");
			break;
	}

    p = ps_curr_dram_ctx;

#if (SW_CHANGE_FOR_SIMULATION==0)
	enable_dramc_phy_dcm(p, 0);
#endif
    switch_26MHz_disable_dummyread_refresh_allbank(p);

	*((volatile unsigned int *)SPM_DRAMC_DPY_CLK_SW_CON2) &= ~(0x1 << 2);//20170210: confirmed by chen-Hsiang
 	*((volatile unsigned int *)SPM_POWER_ON_VAL0) &= ~(0x1 << 28);//20170210: confirmed by chen-Hsiang
}

#define	CHAN_DRAMC_NAO_MISC_STATUSA(base)	(base + 0x80)
#define SREF_STATE				(1 << 16)

static unsigned int is_dramc_exit_slf(void)
{	
	u32 dram_type = (*((volatile unsigned int *)(Channel_A_DRAMC_AO_BASE_ADDRESS + 0x10)) >> 10 & 0x7);
	unsigned int ret = *(volatile unsigned *)CHAN_DRAMC_NAO_MISC_STATUSA(Channel_A_DRAMC_NAO_BASE_ADDRESS);

	if ((ret & SREF_STATE) != 0) {
		show_err2("DRAM CHAN-A is in self-refresh (MISC_STATUSA = 0x%x)\n", ret);
		return 0;
	}

	if(dram_type >= TYPE_LPDDR4) //LP3:1, LP4:2, LP4X:3
	{
		ret = *(volatile unsigned *)CHAN_DRAMC_NAO_MISC_STATUSA(Channel_B_DRAMC_NAO_BASE_ADDRESS);
		if ((ret & SREF_STATE) != 0) {
			show_err2("DRAM CHAN-B is in self-refresh (MISC_STATUSA = 0x%x)\n", ret);
			return 0;
		}
	}
	show_err("ALL DRAM CHAN is not in self-refresh\n");
	return 1;
}

static void release_dram(void)
{
	int i;
	int counter = 3;

	// scy: restore pmic setting (VCORE, VDRAM, VSRAM, VDDQ)
	//////restore_pmic_setting();
	rgu_release_rg_dramc_conf_iso();//Release DRAMC/PHY conf ISO
	dramc_ddr_reserved_mode_setting();	
	rgu_release_rg_dramc_iso();//Release PHY IO ISO
	rgu_release_rg_dramc_sref();//Let DRAM Leave SR

	// setup for EMI: touch center EMI and channel EMI to enable CLK
	show_msg((INFO, "[DDR reserve] EMI CONA: %x\n", *(volatile unsigned int*)EMI_CONA));
	show_msg((INFO, "[DDR reserve] EMI CHA CONA: %x\n", *(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE)));
	show_msg((INFO, "[DDR reserve] EMI CHB CONA: %x\n", *(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE)));
	for (i=0;i<10;i++);

	while(counter)
	{
		if(is_dramc_exit_slf() == 1) /* expect to exit dram-self-refresh */
			break;
		counter--;
	}

	if(counter == 0)
	{
		if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==1)
		{
			show_msg((INFO, "[DDR Reserve] release dram from self-refresh FAIL!\n"));
			g_ddr_reserve_success = 0;
		}
	}
	else
	{
		 show_msg((INFO, "[DDR Reserve] release dram from self-refresh PASS!\n"));
	}
	dramc_ddr_reserved_mode_after_sr();
	//Expect to Use LPDDR3200 and PHYPLL as output, so no need to handle
	//shuffle status since the status will be reset by system reset
	//There is an PLLL_SHU_GP in SPM which will reset by system reset
	return;
}
#endif

void check_ddr_reserve_status(void)
{
	/* get status of DCS and DVFSRC */
#ifdef DDR_RESERVE_MODE
    int dcs_success = rgu_is_emi_dcs_success(), dvfsrc_success = rgu_is_dvfsrc_success();
    int dcs_en = rgu_is_emi_dcs_enable(), dvfsrc_en = rgu_is_dvfsrc_enable();
#else
	int dcs_success /*= rgu_is_emi_dcs_success()*/, dvfsrc_success /*= rgu_is_dvfsrc_success()*/;
	int dcs_en /*= rgu_is_emi_dcs_enable()*/, dvfsrc_en /*= rgu_is_dvfsrc_enable()*/;
#endif

	*(volatile unsigned int *) (CHN0_EMI_BASE + 0x3FC) &= ~0x1;
	*(volatile unsigned int *) (CHN1_EMI_BASE + 0x3FC) &= ~0x1;

#ifdef DDR_RESERVE_MODE
	int counter = 3;
	if(rgu_is_reserve_ddr_enabled())
	{
	  g_ddr_reserve_enable = 1;
#ifdef LAST_DRAMC
	  dram_fatal_set_ddr_rsv_mode_flow();
#endif

	  if(rgu_is_reserve_ddr_mode_success())
	  {
		while(counter)
		{
		  if(rgu_is_dram_slf())
		  {
			g_ddr_reserve_success = 1;
			break;
		  }
		  counter--;
		}
		if(counter == 0)
		{
		  show_err("[DDR Reserve] ddr reserve mode success but DRAM not in self-refresh!\n");
		  g_ddr_reserve_success = 0;
#ifdef LAST_DRAMC
	  dram_fatal_set_ddr_rsv_mode_err();
#endif
		}
	  }
	  else
	  {
		show_err("[DDR Reserve] ddr reserve mode FAIL!\n");
		g_ddr_reserve_success = 0;
#ifdef LAST_DRAMC
	  dram_fatal_set_ddr_rsv_mode_err();
#endif
	  }
	/* Disable DDR-reserve mode in pre-loader stage then enable it again in kernel stage */
	//rgu_dram_reserved(0);

#if 1
	/* overwrite g_ddr_reserve_success if some of dcs/dvfsrc/drs failed */
	/* TODO: check DRS status */
	if ((dcs_en == 1 && dcs_success == 0) || (dvfsrc_en == 1 && dvfsrc_success == 0)) {
		show_err("[DDR Reserve] DRAM content might be corrupted -> clear g_ddr_reserve_success\n");

		if (dvfsrc_en == 1 && dvfsrc_success == 0) {
			show_err("[DDR Reserve] DVFSRC fail!\n");
#ifdef LAST_DRAMC
			dram_fatal_set_dvfsrc_err();
#endif
		}

		if (dcs_en == 1 && dcs_success == 0) {
		g_ddr_reserve_success = 0;
			show_err("[DDR Reserve] DCS fail!\n");
#ifdef LAST_DRAMC
			dram_fatal_set_emi_dcs_err();
#endif
		}
	} else {
		show_err3("[DDR Reserve] DCS/DVFSRC success! (dcs_en=%d, dvfsrc_en=%d)\n", dcs_en, dvfsrc_en);
	}
#endif

	/* release dram, no matter success or failed */
	release_dram();
	}
	else
	{
	  show_err("[DDR Reserve] ddr reserve mode not be enabled yet\n");
	  g_ddr_reserve_enable = 0;
	}
#endif
}

