#ifndef _DSP_CLK_H_
#define _DSP_CLK_H_

#define REG_APMIXDSYS_BASE    0x1000C000
#define REG_TOPCKGEN_BASE     0x10000000

#define DSPPLL_CON0           (REG_APMIXDSYS_BASE + 0x390)
#define DSPPLL_CON1           (REG_APMIXDSYS_BASE + 0x394)
#define DSPPLL_CON2           (REG_APMIXDSYS_BASE + 0x398)
#define DSPPLL_CON3           (REG_APMIXDSYS_BASE + 0x39C)

#define ULPLL_CON0           (REG_APMIXDSYS_BASE + 0x3B0)
#define ULPLL_CON1           (REG_APMIXDSYS_BASE + 0x3B4)

#define PLL_BASE_EN	BIT(0)
#define PLL_PWR_ON	BIT(0)
#define PLL_ISO_EN	BIT(1)

#define DSPPLL_312MHZ          0
#define DSPPLL_400MHZ          1
#define DSPPLL_600MHZ          2

#define CLK_MODE               (REG_TOPCKGEN_BASE + 0x0)
#define CLK_CFG_UPDATE1        (REG_TOPCKGEN_BASE + 0x8)
#define CLK_CFG_8              (REG_TOPCKGEN_BASE + 0xC0)

#define CLK_SCP_CFG_1          (REG_TOPCKGEN_BASE + 0x204)

#define CLK_DSP_SEL_26M               0
#define CLK_DSP_SEL_26M_D_2           1
#define CLK_DSP_SEL_DSPPLL            2
#define CLK_DSP_SEL_DSPPLL_D_2        3
#define CLK_DSP_SEL_DSPPLL_D_4        4
#define CLK_DSP_SEL_DSPPLL_D_8        5

#define CLK_TOPCKGEN_SEL_PLLGP_26M    1
#define CLK_TOPCKGEN_SEL_ULPLL_26M    2
#define CLK_TOPCKGEN_SEL_GPIO_26M     4

enum mux_id_t {
    MUX_CLK_DSP_SEL = 0,
    MUX_CLK_TOPCKGEN_26M_SEL,
    HIFI4DSP_MUX_NUM,
};

enum mux_26m_t {
    DCXO_26 = 0,
    ULPLL_26M,
};

int set_mux_sel(enum mux_id_t mux_id, uint32_t value);
void enable_dsp_mux();
void disable_dsp_mux();

void clk_dsppll_enable(void);
void clk_dsppll_disable(void);
int dsppll_get_enabled(void);

#ifdef CFG_DSP_ULPLL_SUPPORT
void clk_ulpll_enable(void);
void clk_ulpll_disable(void);
int ulpll_get_enabled(void);
int set_mux_aud26m_sel(enum mux_26m_t mux_id);
#endif

unsigned long dsppll_get_freq(void);
int dsppll_set_freq(unsigned long rate);

#endif
