#ifndef SOC_MEDIATEK_MT8167_PMIC_WRAP_H
#define SOC_MEDIATEK_MT8167_PMIC_WRAP_H

#include <sync_write.h>
#include <upmu_hw.h>
#include <pll.h>
#include <mt8167.h>

/* external API */
S32 pwrap_read(U32 adr, U32 *rdata);
S32 pwrap_write(U32 adr, U32 wdata);
S32 pwrap_wacs2(U32 write, U32 adr, U32 wdata, U32 *rdata);
S32 pwrap_init_preloader(void);

#define read32(addr)		__raw_readl((void *)addr)
#define write32(addr,val)	mt_reg_sync_writel((val), ((void *)addr))

#define MUDI

#define MODULE_SW_CG_3_SET	(TOPCKGEN_BASE+0x02c)
#define MODULE_SW_CG_1_SET	(TOPCKGEN_BASE+0x054)
#define MODULE_SW_CG_1_CLR	(TOPCKGEN_BASE+0x084)

#define PWRAP_CG_AP		(1 << 20)
#define PWRAP_CG_MD		(1 << 27)
#define PWRAP_CG_CONN	(1 << 28)
#define PWRAP_CG_26M	(1 << 29)

#define PMIC_B_DCM_EN	(1 << 1)
#define PMIC_SPI_DCM_EN	(1 << 2)

#define CLK_CFG_5_CLR		(CKSYS_BASE+0x098)
#define CLK_SPI_CK_26M		0x1

#define PMIC_WRAP_BASE		(PWRAP_BASE)//0x1000F000
static struct MT8167_pwrap_regs * const mt8167_pwrap = (void *)PMIC_WRAP_BASE;

enum {
	WACS2 = 1 << 3
};

/* timeout setting */
enum {
	TIMEOUT_READ_US        = 255,
	TIMEOUT_WAIT_IDLE_US   = 255
};

/* PMIC_WRAP registers */
struct MT8167_pwrap_regs {
	U32 mux_sel;
	U32 wrap_en;
	U32 dio_en;
	U32 sidly;
	U32 rddmy;
	U32 si_ck_con;
	U32 cshext_write;
	U32 cshext_read;
	U32 cslext_start;
	U32 cslext_end;
	U32 staupd_prd;
	U32 staupd_grpen;
	U32 reserved[4];
	U32 staupd_man_trig;
	U32 staupd_sta;
	U32 wrap_sta;
	U32 harb_init;
	U32 harb_hprio;
	U32 hiprio_arb_en;
	U32 harb_sta0;
	U32 harb_sta1;
	U32 man_en;
	U32 man_cmd;
	U32 man_rdata;
	U32 man_vldclr;
	U32 wacs0_en;
	U32 init_done0;
	U32 wacs0_cmd;
	U32 wacs0_rdata;
	U32 wacs0_vldclr;
	U32 wacs1_en;
	U32 init_done1;
	U32 wacs1_cmd;
	U32 wacs1_rdata;
	U32 wacs1_vldclr;
	U32 wacs2_en;
	U32 init_done2;
	U32 wacs2_cmd;
	U32 wacs2_rdata;
	U32 wacs2_vldclr;
	U32 int_en;
	U32 int_flg_raw;
	U32 int_flg;
	U32 int_clr;
	U32 sig_adr;
	U32 sig_mode;
	U32 sig_value;
	U32 sig_errval;
	U32 crc_en;
	U32 timer_en;
	U32 timer_sta;
	U32 wdt_unit;
	U32 wdt_src_en;
	U32 wdt_flg;
	U32 debug_int_sel;
	U32 dvfs_adr0;
	U32 dvfs_wdata0;
	U32 dvfs_adr1;
	U32 dvfs_wdata1;
	U32 dvfs_adr2;
	U32 dvfs_wdata2;
	U32 dvfs_adr3;
	U32 dvfs_wdata3;
	U32 dvfs_adr4;
	U32 dvfs_wdata4;
	U32 dvfs_adr5;
	U32 dvfs_wdata5;
	U32 dvfs_adr6;
	U32 dvfs_wdata6;
	U32 dvfs_adr7;
	U32 dvfs_wdata7;
	U32 spminf_sta;
	U32 cipher_key_sel;
	U32 cipher_iv_sel;
	U32 cipher_en;
	U32 cipher_rdy;
	U32 cipher_mode;
	U32 cipher_swrst;
	U32 dcm_en;
	U32 dcm_dbc_prd;
	U32 ext_ck;
	U32 adc_cmd_addr;
	U32 adc_cmd;
	U32 adc_rdy_addr;
	U32 adc_rdata_addr1;
	U32 adc_rdata_addr2;
	U32 gps_sta;
	U32 swrst;
	U32 op_type;
	U32 msb_first;
};

enum {
	RDATA_WACS_RDATA_SHIFT = 0,
	RDATA_WACS_FSM_SHIFT   = 16,
	RDATA_WACS_REQ_SHIFT   = 19,
	RDATA_SYNC_IDLE_SHIFT,
	RDATA_INIT_DONE_SHIFT,
	RDATA_SYS_IDLE_SHIFT,
};

enum {
	RDATA_WACS_RDATA_MASK = 0xffff,
	RDATA_WACS_FSM_MASK   = 0x7,
	RDATA_WACS_REQ_MASK   = 0x1,
	RDATA_SYNC_IDLE_MASK  = 0x1,
	RDATA_INIT_DONE_MASK  = 0x1,
	RDATA_SYS_IDLE_MASK   = 0x1,
};

/* WACS_FSM */
enum {
	WACS_FSM_IDLE     = 0x00,
	WACS_FSM_REQ      = 0x02,
	WACS_FSM_WFDLE    = 0x04, /* wait for dle, wait for read data done */
	WACS_FSM_WFVLDCLR = 0x06, /* finish read data, wait for valid flag
				   * clearing */
	WACS_INIT_DONE    = 0x01,
	WACS_SYNC_IDLE    = 0x01,
	WACS_SYNC_BUSY    = 0x00
};

/* dewrapper defaule value */
enum {
	DEFAULT_VALUE_READ_TEST  = 0x5aa5,
	WRITE_TEST_VALUE         = 0xa55a
};

enum pmic_regck {
	REG_CLOCK_18MHZ,
	REG_CLOCK_26MHZ,
	REG_CLOCK_SAFE_MODE
};

/* manual commnd */
enum {
	OP_WR    = 0x1,
	OP_CSH   = 0x0,
	OP_CSL   = 0x1,
	OP_OUTS  = 0x8,
	OP_OUTD  = 0x9,
	OP_INS   = 0xC,
	OP_IND   = 0xE
};

/* error information flag */
enum {
	E_PWR_INVALID_ARG             = 1,
	E_PWR_INVALID_RW              = 2,
	E_PWR_INVALID_ADDR            = 3,
	E_PWR_INVALID_WDAT            = 4,
	E_PWR_INVALID_OP_MANUAL       = 5,
	E_PWR_NOT_IDLE_STATE          = 6,
	E_PWR_NOT_INIT_DONE           = 7,
	E_PWR_NOT_INIT_DONE_READ      = 8,
	E_PWR_WAIT_IDLE_TIMEOUT       = 9,
	E_PWR_WAIT_IDLE_TIMEOUT_READ  = 10,
	E_PWR_INIT_SIDLY_FAIL         = 11,
	E_PWR_RESET_TIMEOUT           = 12,
	E_PWR_TIMEOUT                 = 13,
	E_PWR_INIT_RESET_SPI          = 20,
	E_PWR_INIT_SIDLY              = 21,
	E_PWR_INIT_REG_CLOCK          = 22,
	E_PWR_INIT_ENABLE_PMIC        = 23,
	E_PWR_INIT_DIO                = 24,
	E_PWR_INIT_CIPHER             = 25,
	E_PWR_INIT_WRITE_TEST         = 26,
	E_PWR_INIT_ENABLE_CRC         = 27,
	E_PWR_INIT_ENABLE_DEWRAP      = 28,
	E_PWR_INIT_ENABLE_EVENT       = 29,
	E_PWR_READ_TEST_FAIL          = 30,
	E_PWR_WRITE_TEST_FAIL         = 31,
	E_PWR_SWITCH_DIO              = 32,
	E_PWR_INVALID_DATA            = 33
};

#endif /* SOC_MEDIATEK_MT8167_PMIC_WRAP_H */
