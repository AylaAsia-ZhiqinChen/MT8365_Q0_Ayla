#include <driver_api.h>
#include "FreeRTOS.h"
#include "task.h"
#include "dsp_clk.h"
#include "systimer.h"

static unsigned long dspclk_mux;
static unsigned long dsppll_rate;
static int dsppll_enable;
#ifdef CFG_DSP_ULPLL_SUPPORT
static int ulpll_enable;
#endif

#define BIT(_bit_)		(uint32_t)(1U << (_bit_))
#define BITMASK(_bits_)		GENMASK(1? _bits_, 0? _bits_)
#define BITS(_bits_, _val_)	(BITMASK(_bits_) & (_val_ << (0? _bits_)))

#define clk_readl(addr)				DRV_Reg32(addr)
#define clk_writel(addr, val)			DRV_WriteReg32(addr, val)
#define clk_setl(addr, val)			clk_writel(addr, clk_readl(addr) | (val))
#define clk_clrl(addr, val)			clk_writel(addr, clk_readl(addr) & ~(val))
#define clk_writel_mask(addr, mask, val)	clk_writel(addr, (clk_readl(addr) & ~(mask)) | (val));

#ifdef CFG_DSP_ULPLL_SUPPORT
void clk_ulpll_enable(void)
{
	if (++ulpll_enable == 1) {
		clk_setl(ULPLL_CON1, BIT(19));
		clk_setl(ULPLL_CON0, PLL_BASE_EN);
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}

}

void clk_ulpll_disable(void)
{
	if (--ulpll_enable == 0) {
		clk_clrl(ULPLL_CON0, PLL_BASE_EN);
	}
}

int ulpll_get_enabled(void)
{
	return ulpll_enable;
}

int set_mux_aud26m_sel(enum mux_26m_t mux_id)
{
	if (mux_id == DCXO_26) {
		clk_writel_mask(CLK_SCP_CFG_1, (0x7 << 24), (0x1 << 24));
	} else if (mux_id == ULPLL_26M) {
		clk_writel_mask(CLK_SCP_CFG_1, (0x7 << 24), (0x2 << 24));
	} else {
		return -1;
	}

	return 0;
}
#endif

int set_mux_sel(enum mux_id_t mux_id, uint32_t value)
{

	clk_writel_mask(CLK_CFG_8, (0x7 << 24), (value << 24));
	clk_writel(CLK_CFG_UPDATE1, 0x8);
	dspclk_mux = value;

	return 0;
}

void enable_dsp_mux()
{
	clk_clrl(CLK_CFG_8, BIT(31));
}

void disable_dsp_mux()
{
	clk_setl(CLK_CFG_8, BIT(31));
}

void clk_dsppll_enable(void)
{
	if (++dsppll_enable == 1) {
		clk_setl(DSPPLL_CON3, PLL_PWR_ON);
		udelay(1);
		clk_clrl(DSPPLL_CON3, PLL_ISO_EN);
		udelay(1);

		clk_setl(DSPPLL_CON0, PLL_BASE_EN);
		udelay(20);
	}

}

void clk_dsppll_disable(void)
{
	if (--dsppll_enable == 0) {
		clk_clrl(DSPPLL_CON0, PLL_BASE_EN);
		clk_setl(DSPPLL_CON3, PLL_ISO_EN);
		clk_clrl(DSPPLL_CON3, PLL_PWR_ON);
	}
}

unsigned long dsppll_get_freq(void)
{
	return dsppll_rate;
}

int dsppll_get_enabled(void)
{
	return dsppll_enable;
}

int dsppll_set_freq(unsigned long rate)
{
	if (dsppll_rate == rate)
		return 0;

	if (rate == 312000000) {
		DRV_WriteReg32(DSPPLL_CON1, 0x83180000);
	} else if (rate == 400000000) {
		DRV_WriteReg32(DSPPLL_CON1, 0x820F6276);
	} else if (rate == 600000000) {
		DRV_WriteReg32(DSPPLL_CON1, 0x821713b1);
	}

	dsppll_rate = rate;

	return 0;
}
