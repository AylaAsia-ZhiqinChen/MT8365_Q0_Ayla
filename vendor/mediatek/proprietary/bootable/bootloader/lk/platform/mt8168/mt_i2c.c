/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/**
 * @file mtk_i2c.c
 * This i2c driver is used to control MediaTek I2C controller.\n
 * It provides the interfaces which will be used in LK.
 */
/**
 * @defgroup IP_group_i2c I2C
 *
 *   @{
 *       @defgroup IP_group_i2c_external EXTERNAL
 *         The external API document for I2C. \n
 *
 *         @{
 *            @defgroup IP_group_i2c_external_function 1.function
 *              External function in i2c driver.
 *            @defgroup IP_group_i2c_external_struct 2.structure
 *              none.
 *            @defgroup IP_group_i2c_external_typedef 3.typedef
 *              none.
 *            @defgroup IP_group_i2c_external_enum 4.enumeration
 *              none.
 *            @defgroup IP_group_i2c_external_def 5.define
 *              none.
 *         @}
 *
 *       @defgroup IP_group_i2c_internal INTERNAL
 *         The internal API document for I2C. \n
 *
 *         @{
 *            @defgroup IP_group_i2c_internal_function 1.function
 *              Internal function in i2c driver.
 *            @defgroup IP_group_i2c_internal_struct 2.structure
 *              Internal structure in i2c driver.
 *            @defgroup IP_group_i2c_internal_typedef 3.typedef
 *              none.
 *            @defgroup IP_group_i2c_internal_enum 4.enumeration
 *              Internal enumeration in i2c driver.
 *            @defgroup IP_group_i2c_internal_def 5.define
 *              Internal define in i2c driver.
 *         @}
 *   @}
 */
#include <platform/mt_i2c.h>
#include <platform/mt_pmic_wrap_init.h>
#include <platform/mt_gpio.h>
struct mtk_i2c *i2c_global;
/**
 * @brief i2c source clock.
 */
static uint32_t g_i2c_source_clock = MTK_I2C_SOURCE_CLK;
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Write data to i2c controller register.
 * @param[in]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains register base\n
 *     address.
 * @param[in]
 *     offset: register relative base offset value.
 * @param[in]
 *     value: The value set to register.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static void i2c_writel(struct mtk_i2c *i2c, uint32_t offset,
		 uint32_t value)
{
	mt_reg_sync_writel(value, (i2c->base) + (offset));
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Read data from i2c controller register.
 * @param[in]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains register base\n
 *     address.
 * @param[in]
 *     offset: register relative base offset value.
 * @return
 *     i2c controller register value.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static uint32_t i2c_readl(struct mtk_i2c *i2c, uint32_t offset)
{
	return DRV_Reg32((i2c->base) + (offset));
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Write data to DMA controller register.
 * @param[in]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains DMA register\n
 *     base address.
 * @param[in]
 *     offset: register relative base offset value.
 * @param[in]
 *     value: The value set to register.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static inline void i2c_dma_writel(struct mtk_i2c *i2c, uint32_t offset,
				  uint32_t value)
{
	writel(value, (i2c->dmabase + offset));
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Read data from DMA controller register.
 * @param[in]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains DMA register\n
 *     base address.
 * @param[in]
 *     offset: register relative base offset value.
 * @return
 *     DMA controller register value.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static inline uint32_t i2c_dma_readl(struct mtk_i2c *i2c, uint32_t offset)
{
	return readl(i2c->dmabase + offset);
}

/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Enable i2c clock.
 * @param[in]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains i2c bus number.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static void i2c_clock_enable(struct mtk_i2c *i2c)
{
#ifndef CONFIG_MTK_FPGA
	switch (i2c->id) {
	case 0:
		writel(MTK_I2C0_CLK_OFFSET, MTK_I2C_CLK_CLR);
		break;
	case 1:
		writel(MTK_I2C1_CLK_OFFSET, MTK_I2C_CLK_CLR);
		break;
	case 2:
		writel(MTK_I2C2_CLK_OFFSET, MTK_I2C_CLK_CLR);
		break;
	case 3:
		writel(MTK_I2C3_CLK_OFFSET, MTK_I2C_CLK_CLR);
		break;
	default:
		I2CERR("i2c clk enable, invalid para: i2c->id=%d\n",i2c->id);
	}
	writel(MTK_APDMA_CLK_OFFSET, MTK_APDMA_CLK_CLR);
#endif
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Disable i2c clock.
 * @param[in]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains i2c bus number.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static void i2c_clock_disable(struct mtk_i2c *i2c)
{
#ifndef CONFIG_MTK_FPGA
	switch (i2c->id) {
	case 0:
		writel(MTK_I2C0_CLK_OFFSET, MTK_I2C_CLK_SET);
		break;
	case 1:
		writel(MTK_I2C1_CLK_OFFSET, MTK_I2C_CLK_SET);
		break;
	case 2:
		writel(MTK_I2C2_CLK_OFFSET, MTK_I2C_CLK_SET);
		break;
	case 3:
		writel(MTK_I2C3_CLK_OFFSET, MTK_I2C_CLK_SET);
		break;
	default:
		I2CERR("i2c clk disable, invalid para: i2c->id=%d\n",i2c->id);
	}
    writel(MTK_APDMA_CLK_OFFSET, MTK_APDMA_CLK_SET);
#endif
}
#ifdef CONFIG_MTK_IRQ
static static void mtk_i2c_irq(void)
{
	uint16_t restart_flag = 0;
	uint16_t intr_stat;
	if (i2c_global->auto_restart)
		restart_flag = I2C_RS_TRANSFER;
	intr_stat = i2c_readl(i2c_global, OFFSET_INTR_STAT);
	i2c_writel(i2c_global, OFFSET_INTR_STAT, intr_stat);
	/*
	 * when occurs ack error, i2c controller generate two interrupts
	 * first is the ack error interrupt, then the complete interrupt
	 * i2c->irq_stat need keep the two interrupt value.
	 */
	i2c_global->irq_stat |= intr_stat;
	if (i2c_global->irq_stat & (I2C_TRANSAC_COMP | restart_flag))
		i2c_global->msg_complete = true;
}
static void mtk_irq_init(struct mtk_i2c *i2c)
{
	register_int_handler(i2c->irqnr, mtk_i2c_irq, "i2c irq handler");
}
#endif
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Get i2c bus base address, DMA base address and source clock.
 * @param[out]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains register base\n
 *     address, DMA base address and bus number information.
 * @return
 *     0, set base address successfully.\n
 *     -EINVAL_I2C, invalid i2c bus id.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     1. Invalid i2c bus number, return -EINVAL_I2C.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static int mtk_i2c_init_base(struct mtk_i2c *i2c)
{
	switch (i2c->id) {
	case 0:
		i2c->base = MTK_I2C0_BASE;
		i2c->dmabase = MTK_I2C0_DMA;
		i2c->irqnr = MTK_I2C0_GIC_IRQ;
		break;
	case 1:
		i2c->base = MTK_I2C1_BASE;
		i2c->dmabase = MTK_I2C1_DMA;
		i2c->irqnr = MTK_I2C1_GIC_IRQ;
		break;
	case 2:
		i2c->base = MTK_I2C2_BASE;
		i2c->dmabase = MTK_I2C2_DMA;
		i2c->irqnr = MTK_I2C2_GIC_IRQ;
		break;
	case 3:
		i2c->base = MTK_I2C3_BASE;
		i2c->dmabase = MTK_I2C3_DMA;
		i2c->irqnr = MTK_I2C3_GIC_IRQ;
		break;
	default:
		I2CERR("invalid para: i2c->id=%d\n", i2c->id);
		return -EINVAL_I2C;
	}
	i2c->clk = g_i2c_source_clock;
	i2c->clk_src_div = MTK_I2C_CLK_DIV;
	if(!i2c->poll_en) {
#ifdef CONFIG_MTK_IRQ
		i2c_global = i2c;
		mtk_irq_init(i2c);
#endif
	}
	return 0;
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Initialize i2c hardware, soft reset i2c controller, then\n
 *     configure io mode and control registers.
 * @param[in]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains register base\n
 *     address, ioconfig and i2c hardware information.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static void mtk_i2c_init_hw(struct mtk_i2c *i2c)
{
	uint16_t control_reg;
	if (!(i2c->mode & I2C_FIFO_FORCE))
		i2c_writel(i2c, OFFSET_SOFTRESET, I2C_SOFT_RST);
	/* set ioconfig */
	if (i2c->pushpull)
		i2c_writel(i2c, OFFSET_IO_CONFIG, I2C_IO_CONFIG_PUSH_PULL);
	else
		i2c_writel(i2c, OFFSET_IO_CONFIG, I2C_IO_CONFIG_OPEN_DRAIN);
	control_reg = I2C_CONTROL_DEFAULT | I2C_CONTROL_ACKERR_DET_EN |
		      I2C_CONTROL_CLK_EXT_EN;
	i2c_writel(i2c, OFFSET_CONTROL, control_reg);
	if (i2c->mode & I2C_DCM_ENABLE)
		i2c_writel(i2c, OFFSET_DCM_EN, I2C_DCM_OPEN);
	if (i2c->mode & I2C_CONTI_TRANS)
		i2c_writel(i2c, OFFSET_DELAY_LEN, i2c->delay_len);
	else
		i2c_writel(i2c, OFFSET_DELAY_LEN, I2C_DELAY_LEN);
	i2c_dma_writel(i2c, OFFSET_DMA_RST, I2C_DMA_HARD_RST);
	i2c_dma_writel(i2c, OFFSET_DMA_RST, I2C_DMA_CLR_FLAG);
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Calculate i2c speed.\n
 *     Hardware design:\n
 *     i2c_bus_freq = source_clk / (2 * sample_cnt * step_cnt)\n
 *     The calculation want to pick the highest bus frequency that\n
 *     is still less than or equal to i2c->speed_hz. The\n
 *     calculation try to get sample_cnt and step_cnt.
 * @param[in]
 *     clk_src: i2c module source clock.
 * @param[in]
 *     target_speed: i2c target speed.
 * @param[out]
 *     timing_step_cnt: i2c step_cnt value.
 * @param[out]
 *     timing_sample_cnt: i2c sample_cnt value.
 * @return
 *     0, calculate speed successfully.\n
 *     -EINVAL_I2C, calculate speed fail.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     1. Target speed is too low, calculate speed fail, return\n
 *     -EINVAL_I2C.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static int mtk_i2c_calculate_speed(unsigned int clk_src,
				   unsigned int target_speed,
				   unsigned int *timing_step_cnt,
				   unsigned int *timing_sample_cnt)
{
	unsigned int step_cnt;
	unsigned int sample_cnt;
	unsigned int max_step_cnt;
	unsigned int base_sample_cnt = MAX_SAMPLE_CNT_DIV;
	unsigned int base_step_cnt;
	unsigned int opt_div;
	unsigned int best_mul;
	unsigned int cnt_mul;
	if (target_speed > MAX_FS_PLUS_SPEED)
		max_step_cnt = MAX_HS_STEP_CNT_DIV;
	else
		max_step_cnt = MAX_STEP_CNT_DIV;
	base_step_cnt = max_step_cnt;
	/* find the best combination */
	opt_div = DIV_ROUND_UP(clk_src >> 1, target_speed);
	best_mul = MAX_SAMPLE_CNT_DIV * max_step_cnt;
	/* Search for the best pair (sample_cnt, step_cnt) with
	* 0 < sample_cnt < MAX_SAMPLE_CNT_DIV
	* 0 < step_cnt < max_step_cnt
	* sample_cnt * step_cnt >= opt_div
	* optimizing for sample_cnt * step_cnt being minimal
	*/
	for (sample_cnt = 1; sample_cnt <= MAX_SAMPLE_CNT_DIV; sample_cnt++) {
		step_cnt = DIV_ROUND_UP(opt_div, sample_cnt);
		cnt_mul = step_cnt * sample_cnt;
		if (step_cnt > max_step_cnt)
			continue;
		if (cnt_mul < best_mul) {
			best_mul = cnt_mul;
			base_sample_cnt = sample_cnt;
			base_step_cnt = step_cnt;
			if (best_mul == opt_div)
				break;
		}
	}
	sample_cnt = base_sample_cnt;
	step_cnt = base_step_cnt;
	if ((clk_src / (2 * sample_cnt * step_cnt)) > target_speed) {
		I2CERR("Unsupported speed (%u KHz)\n", target_speed);
		return -EINVAL_I2C;
	}
	*timing_step_cnt = step_cnt - 1;
	*timing_sample_cnt = sample_cnt - 1;
	return 0;
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Calculate i2c speed and write sample_cnt, step_cnt to TIMING register.
 * @param[out]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains i2c source clock,
 *     clock divide and speed.
 * @return
 *     0, set speed successfully.\n
 *     error code from mtk_i2c_calculate_speed().
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     1. If mtk_i2c_calculate_speed() fails, return its error code.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static int mtk_i2c_set_speed(struct mtk_i2c *i2c)
{
	bool low_power_clk = false;
	unsigned int clk_src;
	unsigned int step_cnt;
	unsigned int sample_cnt;
	unsigned int target_speed;
	int ret;
	if ((i2c->clk == MTK_I2C_ULPOSC_DIV8) ||
	    (i2c->clk == MTK_I2C_ULPOSC_DIV16) ||
	    (i2c->clk == MTK_I2C_CLK_26M))
		low_power_clk = true;
	if (i2c->speed == 0)
		i2c->speed = I2C_DEFAULT_SPEED;
	if ((!low_power_clk) && (i2c->speed == I2C_DEFAULT_SPEED)) {
		i2c->clock_div_reg = I2C_CLK_DIV_100K;
		i2c->timing_reg = I2C_TIMING_100K;
		i2c->high_speed_reg = 0;
	} else if ((!low_power_clk) && (i2c->speed == MAX_FS_MODE_SPEED)) {
		i2c->clock_div_reg = I2C_CLK_DIV_400K;
		i2c->timing_reg = I2C_TIMING_400K;
		i2c->high_speed_reg = 0;
	} else if ((!low_power_clk) && (i2c->speed == MAX_FS_PLUS_SPEED)) {
		i2c->clock_div_reg = I2C_CLK_DIV_1000K;
		i2c->timing_reg = I2C_TIMING_1000K;
		i2c->high_speed_reg = 0;
	} else {
		i2c->clock_div_reg = I2C_DEFAULT_CLK_DIV;
		if (i2c->clk_src_div == 0)
			i2c->clk_src_div = MTK_I2C_CLK_DIV;
		i2c->clk_src_div *= i2c->clock_div_reg;
		clk_src = (i2c->clk) / (i2c->clk_src_div);
		target_speed = i2c->speed;
		if (target_speed > MAX_FS_PLUS_SPEED) {
			/* Set master code speed register */
			i2c->timing_reg = I2C_TIMING_400K;
			/* Set the high speed mode register */
			ret = mtk_i2c_calculate_speed(clk_src, target_speed,
						      &step_cnt, &sample_cnt);
			if (ret < 0)
				return ret;
			i2c->high_speed_reg = I2C_TIME_DEFAULT_VALUE |
					      (sample_cnt << 12) |
					      (step_cnt << 8);
		} else {
			ret = mtk_i2c_calculate_speed(clk_src, target_speed,
						      &step_cnt, &sample_cnt);
			if (ret < 0)
				return ret;
			i2c->timing_reg = (sample_cnt << 8) | step_cnt;
			/* Disable the high speed transaction */
			i2c->high_speed_reg = 0;
		}
	}
	i2c_writel(i2c, OFFSET_CLOCK_DIV, (i2c->clock_div_reg - 1));
	i2c_writel(i2c, OFFSET_TIMING, i2c->timing_reg);
	i2c_writel(i2c, OFFSET_HS, i2c->high_speed_reg);
	return 0;
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Dump i2c controller registers and DMA registers value.
 * @param[in]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains register base\n
 *     address and DMA base address.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static void i2c_dump_info(struct mtk_i2c *i2c)
{
	I2CERR("I2C structure:\n");
	I2CERR("id=%d,dma_en=%x,auto_restart=%x,poll_en=%x,op=%x\n",
	       i2c->id, i2c->dma_en, i2c->auto_restart, i2c->poll_en, i2c->op);
	I2CERR("irq_stat=%x,source_clk=%d,clk_div=%d,speed=%d\n",
	       i2c->irq_stat, i2c->clk, i2c->clk_src_div, i2c->speed);
	I2CERR("filter_msg=%x,msg_complete=%x,addr=%x\n",
		i2c->filter_msg, i2c->msg_complete, i2c->addr);
	I2CERR("mode=%x,irqnr=%x,timing_reg=%x,high_speed_reg=%x\n",
		i2c->mode, i2c->irqnr, i2c->timing_reg, i2c->high_speed_reg);
	I2CERR("con_num=%d,delay_len=%x,ext_time=%x,scl_ratio=%x\n",
		i2c->con_num, i2c->delay_len, i2c->ext_time, i2c->scl_ratio);
	I2CERR("hs_scl_ratio=%x,scl_mis_comp=%x,sta_stop_time=%x\n",
		i2c->hs_scl_ratio, i2c->scl_mis_comp, i2c->sta_stop_time);
	I2CERR("hs_sta_stop_time=%x,sda_time=%x\n",
		i2c->hs_sta_stop_time, i2c->sda_time);
	I2CERR("I2C base address 0x%llx\n", i2c->base);
	I2CERR("I2C register:\n");
	I2CERR("SLAVE_ADDR=0x%x,INTR_MASK=0x%x,INTR_STAT=0x%x\n",
		(i2c_readl(i2c, OFFSET_SLAVE_ADDR)),
		(i2c_readl(i2c, OFFSET_INTR_MASK)),
		(i2c_readl(i2c, OFFSET_INTR_STAT)));
	I2CERR("CONTROL=0x%x,TIMING=0x%x\n",
		(i2c_readl(i2c, OFFSET_CONTROL)),
		(i2c_readl(i2c, OFFSET_TIMING)));
	I2CERR("TRANSFER_LEN=0x%x,TRANSAC_LEN=0x%x,DELAY_LEN=0x%x\n",
		(i2c_readl(i2c, OFFSET_TRANSFER_LEN)),
		(i2c_readl(i2c, OFFSET_TRANSAC_LEN)),
		(i2c_readl(i2c, OFFSET_DELAY_LEN)));
	I2CERR("START=0x%x,EXT_CONF=0x%x,IO_CONFIG=0x%x\n",
		(i2c_readl(i2c, OFFSET_START)),
		(i2c_readl(i2c, OFFSET_EXT_CONF)),
		(i2c_readl(i2c, OFFSET_IO_CONFIG)));
	I2CERR("FIFO_STAT1=0x%x,FIFO_STAT=0x%x,FIFO_THRESH=0x%x\n",
		(i2c_readl(i2c, OFFSET_FIFO_STAT1)),
		(i2c_readl(i2c, OFFSET_FIFO_STAT)),
		(i2c_readl(i2c, OFFSET_FIFO_THRESH)));
	I2CERR("DEBUGSTAT=0x%x,TRANSFER_LEN_AUX=0x%x,CLOCK_DIV=0x%x\n",
		(i2c_readl(i2c, OFFSET_DEBUGSTAT)),
		(i2c_readl(i2c, OFFSET_TRANSFER_LEN_AUX)),
		(i2c_readl(i2c, OFFSET_CLOCK_DIV)));
	I2CERR("HS=0x%x,SCL_HL_RATIO=0x%x,HS_SCL_HL_RATIO=0x%x\n",
		(i2c_readl(i2c, OFFSET_HS)),
		(i2c_readl(i2c, OFFSET_SCL_HL_RATIO)),
		(i2c_readl(i2c, OFFSET_HS_SCL_HL_RATIO)));
	I2CERR("STA_STOP_AC_TIME=0x%x,HS_STA_STOP_AC_TIME=0x%x\n",
		(i2c_readl(i2c, OFFSET_STA_STOP_AC_TIME)),
		(i2c_readl(i2c, OFFSET_HS_STA_STOP_AC_TIME)));
	I2CERR("SCL_MIS_COMP_POINT=0x%x,SDA_TIME=0x%x,FIFO_PAUSE=0x%x\n",
		(i2c_readl(i2c, OFFSET_SCL_MIS_COMP_POINT)),
		(i2c_readl(i2c, OFFSET_SDA_TIME)),
		(i2c_readl(i2c, OFFSET_FIFO_PAUSE)));
	I2CERR("DMA base address 0x%llx\n", i2c->dmabase);
	I2CERR("I2C DMA register:\n");
	I2CERR("OFFSET_DMA_TX_MEM_ADDR=0x%x,OFFSET_DMA_RX_MEM_ADDR=0x%x\n",
	       (i2c_dma_readl(i2c, OFFSET_DMA_TX_MEM_ADDR)),
	       (i2c_dma_readl(i2c, OFFSET_DMA_RX_MEM_ADDR)));
	I2CERR("OFFSET_DMA_TX_LEN=0x%x,OFFSET_DMA_RX_LEN=0x%x\n",
	       (i2c_dma_readl(i2c, OFFSET_DMA_TX_LEN)),
	       (i2c_dma_readl(i2c, OFFSET_DMA_RX_LEN)));
	I2CERR("OFFSET_DMA_CON=0x%x,OFFSET_DMA_EN=0x%x\n",
	       (i2c_dma_readl(i2c, OFFSET_DMA_CON)),
	       (i2c_dma_readl(i2c, OFFSET_DMA_EN)));
	I2CERR("OFFSET_DMA_INT_EN=0x%x,OFFSET_DMA_INT_FLAG=0x%x\n",
	       (i2c_dma_readl(i2c, OFFSET_DMA_INT_EN)),
	       (i2c_dma_readl(i2c, OFFSET_DMA_INT_FLAG)));
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Configure i2c register and trigger transfer.
 * @param[out]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains register base\n
 *     address, operation mode, interrupt status and i2c driver data.
 * @param[out]
 *     msgs: i2c_msg pointer, struct i2c_msg contains slave\n
 *     address, operation mode, msg length and data buffer.
 * @param[in]
 *     num: i2c_msg number.
 * @param[in]
 *     left_num: left i2c_msg number.
 * @return
 *     0, i2c transfer successfully.\n
 *     -ETIMEDOUT_I2C, i2c transfer timeout.\n
 *     -EREMOTEIO_I2C, i2c receive data length does not equal to request data\n
 *     length.\n
 *     -ENXIO_I2C, i2c transfer ack error.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     1. i2c transfer timeout, return -ETIMEDOUT_I2C.\n
 *     2. i2c receive data length does not equal to request data\n
 *     length, return -EREMOTEIO_I2C.\n
 *     3. i2c transfer ack error, return -ENXIO_I2C.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static int mtk_i2c_do_transfer(struct mtk_i2c *i2c, struct i2c_msg *msgs,
			       int num, int left_num)
{
	bool tmo = false;
	bool trans_error = false;
	uint8_t *data_buf = msgs->buf;
	uint16_t data_len = msgs->len;
	uint16_t read_len;
	uint16_t addr_reg;
	uint16_t start_reg;
	uint16_t control_reg;
	uint16_t restart_flag = 0;
	uint32_t tmo_poll = I2C_POLL_VALUE;
	int ret;
	i2c->irq_stat = 0;
	if (i2c->auto_restart)
		restart_flag = I2C_RS_TRANSFER;
	control_reg = i2c_readl(i2c, OFFSET_CONTROL) &
		      ~(I2C_CONTROL_DIR_CHANGE | I2C_CONTROL_RS);
	if ((i2c->speed > MAX_FS_PLUS_SPEED) ||
	    ((num > 1) && !(i2c->mode & I2C_MULTI_STOP)))
		control_reg |= I2C_CONTROL_RS;
	if (i2c->op == I2C_MASTER_WRRD)
		control_reg |= I2C_CONTROL_DIR_CHANGE | I2C_CONTROL_RS;
	if (i2c->dma_en)
		control_reg |= I2C_CONTROL_AYNCS_MODE | I2C_CONTROL_DMA_EN;
	i2c_writel(i2c, OFFSET_CONTROL, control_reg);
	/* set start condition */
	if (i2c->mode & I2C_EXTEN_SET)
		i2c_writel(i2c, OFFSET_EXT_CONF, i2c->ext_time);
	else {
		if (i2c->speed <= I2C_DEFAULT_SPEED)
			i2c_writel(i2c, OFFSET_EXT_CONF, I2C_ST_START_CON);
		else
			i2c_writel(i2c, OFFSET_EXT_CONF, I2C_FS_START_CON);
	}
	/* set ac timing register */
	if (i2c->mode & I2C_ACTIME_SET) {
		i2c_writel(i2c, OFFSET_SCL_HL_RATIO, i2c->scl_ratio);
		i2c_writel(i2c, OFFSET_HS_SCL_HL_RATIO, i2c->hs_scl_ratio);
		i2c_writel(i2c, OFFSET_SCL_MIS_COMP_POINT, i2c->scl_mis_comp);
		i2c_writel(i2c, OFFSET_STA_STOP_AC_TIME, i2c->sta_stop_time);
		i2c_writel(i2c, OFFSET_HS_STA_STOP_AC_TIME,
			   i2c->hs_sta_stop_time);
		i2c_writel(i2c, OFFSET_SDA_TIME, i2c->sda_time);
	}
	addr_reg = msgs->addr << 1;
	if (i2c->op == I2C_MASTER_RD)
		addr_reg |= 0x1;
	i2c_writel(i2c, OFFSET_SLAVE_ADDR, addr_reg);
	/* clear interrupt status */
	i2c_writel(i2c, OFFSET_INTR_STAT, I2C_RS_TRANSFER | I2C_ACKERR |
		   I2C_TRANSAC_COMP);
	if (!(i2c->mode & I2C_FIFO_FORCE))
		i2c_writel(i2c, OFFSET_FIFO_ADDR_CLR, I2C_FIFO_ADDR_CLR);
	if (i2c->poll_en)
		i2c_writel(i2c, OFFSET_INTR_MASK, 0);
	else
		i2c_writel(i2c, OFFSET_INTR_MASK, restart_flag | I2C_ACKERR |
			   I2C_TRANSAC_COMP);
	/* set transfer and transaction len */
	if (i2c->op == I2C_MASTER_WRRD) {
		i2c_writel(i2c, OFFSET_TRANSFER_LEN, msgs->len);
		i2c_writel(i2c, OFFSET_TRANSFER_LEN_AUX, (msgs + 1)->len);
		i2c_writel(i2c, OFFSET_TRANSAC_LEN, I2C_WRRD_TRANAC_VALUE);
	} else {
		if (i2c->mode & I2C_CONTI_TRANS) {
			i2c_writel(i2c, OFFSET_TRANSFER_LEN, msgs->len);
			i2c_writel(i2c, OFFSET_TRANSAC_LEN, i2c->con_num);
			msgs->len *= i2c->con_num;
		} else {
			i2c_writel(i2c, OFFSET_TRANSFER_LEN, msgs->len);
			i2c_writel(i2c, OFFSET_TRANSAC_LEN, num);
		}
	}
	if (i2c->dma_en) {
		if (i2c->op == I2C_MASTER_WR) {
			i2c_dma_writel(i2c, OFFSET_DMA_INT_FLAG,
				       I2C_DMA_INT_FLAG_NONE);
			i2c_dma_writel(i2c, OFFSET_DMA_CON,
				       I2C_DMA_CON_TX);
			i2c_dma_writel(i2c, OFFSET_DMA_TX_MEM_ADDR,
				       (uint32_t)(uint64_t)(msgs->buf));
			i2c_dma_writel(i2c, OFFSET_DMA_TX_LEN,
				       (uint32_t)(msgs->len));
		} else if (i2c->op == I2C_MASTER_RD) {
			i2c_dma_writel(i2c, OFFSET_DMA_INT_FLAG,
				       I2C_DMA_INT_FLAG_NONE);
			i2c_dma_writel(i2c, OFFSET_DMA_CON,
				       I2C_DMA_CON_RX);
			i2c_dma_writel(i2c, OFFSET_DMA_RX_MEM_ADDR,
				       (uint32_t)(uint64_t)(msgs->buf));
			i2c_dma_writel(i2c, OFFSET_DMA_RX_LEN,
				       (uint32_t)(msgs->len));
		} else if (i2c->op == I2C_MASTER_WRRD) {
			i2c_dma_writel(i2c, OFFSET_DMA_INT_FLAG,
				       I2C_DMA_CLR_FLAG);
			i2c_dma_writel(i2c, OFFSET_DMA_CON,
				       I2C_DMA_CLR_FLAG);
			i2c_dma_writel(i2c, OFFSET_DMA_TX_MEM_ADDR,
				       (uint32_t)(uint64_t)(msgs->buf));
			i2c_dma_writel(i2c, OFFSET_DMA_RX_MEM_ADDR,
				       (uint32_t)(uint64_t)((msgs + 1)->buf));
			i2c_dma_writel(i2c, OFFSET_DMA_TX_LEN,
				       (uint32_t)(msgs->len));
			i2c_dma_writel(i2c, OFFSET_DMA_RX_LEN,
				       (uint32_t)((msgs + 1)->len));
		}
		i2c_dma_writel(i2c, OFFSET_DMA_EN, I2C_DMA_START_EN);
	} else {
		if (!(i2c->mode & I2C_FIFO_FORCE) &&
		    (i2c->op != I2C_MASTER_RD)) {
			data_buf = msgs->buf;
			data_len = msgs->len;
			while (data_len--)
				i2c_writel(i2c, OFFSET_DATA_PORT,
					   *(data_buf++));
		}
	}
	if (!i2c->auto_restart) {
		start_reg = I2C_TRANSAC_START;
	} else {
		start_reg = I2C_TRANSAC_START | I2C_RS_MUL_TRIG;
		if (left_num >= 1)
			start_reg |= I2C_RS_MUL_CNFG;
	}
	i2c_writel(i2c, OFFSET_START, start_reg);
	if (i2c->poll_en) {
		for (;;) {
			i2c->irq_stat = i2c_readl(i2c, OFFSET_INTR_STAT);
			if (i2c->irq_stat & (I2C_TRANSAC_COMP | restart_flag)) {
				tmo = false;
				if (i2c->irq_stat & I2C_ACKERR)
					trans_error = true;
				break;
			}
			tmo_poll--;
			if (tmo_poll == 0) {
				tmo = true;
				break;
			}
		}
	} else {
		for (;;) {
			if (i2c->msg_complete && (i2c->irq_stat &
			    (I2C_TRANSAC_COMP | restart_flag))) {
				tmo = false;
				if (i2c->irq_stat & I2C_ACKERR)
					trans_error = true;
				break;
			}
			tmo_poll--;
			if (tmo_poll == 0) {
				tmo = true;
				break;
			}
		}
	}
	/* clear interrupt mask */
	i2c_writel(i2c, OFFSET_INTR_MASK, ~(restart_flag | I2C_ACKERR |
					    I2C_TRANSAC_COMP));
	if ((!tmo) && (!trans_error)) {
		if (!i2c->dma_en && i2c->op != I2C_MASTER_WR &&
		    !(i2c->mode & I2C_FIFO_FORCE)) {
			data_buf = (i2c->op == I2C_MASTER_RD) ?
				   msgs->buf : (msgs + 1)->buf;
			data_len = (i2c->op == I2C_MASTER_RD) ?
				   msgs->len : (msgs + 1)->len;
				read_len = i2c_readl(i2c, OFFSET_FIFO_STAT1)
					   & 0x1f;
			if (read_len == data_len) {
				while (data_len--)
					*(data_buf++) = i2c_readl(i2c,
							OFFSET_DATA_PORT);
			} else {
				I2CERR("fifo read error!\n");
				I2CERR("data_len %x, read_len %x\n",
					data_len, read_len);
				if (i2c->filter_msg == false)
					i2c_dump_info(i2c);
				return -EREMOTEIO_I2C;
			}
		}
	} else {
		/* timeout or ACKERR */
		if (tmo)
			ret = -ETIMEDOUT_I2C;
		else
			ret = -ENXIO_I2C;
		if (i2c->filter_msg == false) {
			if (tmo) {
				I2CERR("id=%d, addr: %x, transfer timeout\n",
				       i2c->id, msgs->addr);
			} else {
				I2CERR("id=%d, addr: %x, I2C_ACKERR\n",
				       i2c->id, msgs->addr);
			}
			i2c_dump_info(i2c);
		}
		return ret;
	}
	return 0;
}
/** @ingroup IP_group_i2c_internal_function
 * @par Description
 *     Common i2c transfer API. Set i2c transfer mode according to i2c_msg\n
 *     information, then call mtk_i2c_do_transfer() to configure i2c register\n
 *     and trigger transfer.
 * @param[out]
 *     i2c: mtk_i2c pointer, struct mtk_i2c contains register base\n
 *     address, operation mode, interrupt status and i2c driver data.
 * @param[out]
 *     msgs: i2c_msg pointer, struct i2c_msg contains slave\n
 *     address, operation mode, msg length and data buffer.
 * @param[in]
 *     num: i2c_msg number.
 * @return
 *     i2c_msg number, i2c transfer successfully.\n
 *     -EINVAL_I2C, msg length is 0 or more than 16, msg data buffer is NULL,\n
 *     use DMA MODE or slave address more than 0x7f.\n
 *     error code from mtk_i2c_init_base().\n
 *     error code from mtk_i2c_set_speed().\n
 *     error code from mtk_i2c_do_transfer().
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     1. If msg length is 0 or more than 16, msg data buffer is NULL,\n
 *     use DMA MODE or slave address more than 0x7f, return -EINVAL_I2C.
 *     2. If mtk_i2c_init_base() fails, return its error code.\n
 *     3. If mtk_i2c_set_speed() fails, return its error code.\n
 *     4. If mtk_i2c_do_transfer() fails, return its error code.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
static int mtk_i2c_transfer(struct mtk_i2c *i2c, struct i2c_msg *msgs, int num)
{
	uint8_t num_cnt;
	int left_num = num;
	int ret;
	ret = mtk_i2c_init_base(i2c);
	if (ret) {
		I2CERR("Failed to init i2c base.\n");
		return ret;
	}

	if (!(i2c->mode & I2C_FIFO_FORCE)) {
		i2c_clock_enable(i2c);
	}
	mtk_i2c_init_hw(i2c);
	ret = mtk_i2c_set_speed(i2c);
	if (ret) {
		I2CERR("Failed to set the speed.\n");
		goto err_exit;
	}
	for (num_cnt = 0; num_cnt < num; num_cnt++) {
		if (((msgs + num_cnt)->addr) > 0x7f) {
			I2CERR("i2c addr: msgs[%d]->addr(%x) > 0x7f, error!\n",
			       num_cnt, ((msgs + num_cnt)->addr));
			ret = -EINVAL_I2C;
			goto err_exit;
		}
		if (!(msgs + num_cnt)->buf) {
			I2CERR("msgs[%d]->buf is NULL.\n", num_cnt);
			ret = -EINVAL_I2C;
			goto err_exit;
		}
		if ((msgs + num_cnt)->len == 0) {
			I2CERR("msgs[%d]->len == 0, error!\n", num_cnt);
			ret = -EINVAL_I2C;
			goto err_exit;
		}
		if (!(i2c->mode & I2C_FIFO_FORCE) &&
		    (msgs + num_cnt)->len > I2C_FIFO_SIZE)
			i2c->dma_en = true;
	}
	if ((num == 1) || ((!i2c->dma_en) && (num == 2) &&
	    (!(msgs[0].flags & I2C_M_RD) && (msgs[1].flags & I2C_M_RD) &&
	    (msgs[0].addr == msgs[1].addr)) && !(i2c->mode & I2C_MULTI_TRANS)))
		i2c->auto_restart = false;
	else
		i2c->auto_restart = true;
	while (left_num--) {
		if (msgs->flags & I2C_M_RD)
			i2c->op = I2C_MASTER_RD;
		else
			i2c->op = I2C_MASTER_WR;
		if (!i2c->auto_restart) {
			if (num == 2) {
				/* combined two messages into one transaction */
				i2c->op = I2C_MASTER_WRRD;
				left_num--;
			}
		}
		ret = mtk_i2c_do_transfer(i2c, msgs, num, left_num);
		if (ret < 0)
			goto err_exit;
		msgs++;
	}
	ret = I2C_OK;
err_exit:
	if (!(i2c->mode & I2C_FIFO_FORCE))
		i2c_clock_disable(i2c);
	return ret;
}
/** @ingroup IP_group_i2c_external_function
 * @par Description
 *     Initialize struct mtk_i2c and i2c_msg, then read data from\n
 *     slave device.
 * @param[in]
 *     bus_num: i2c bus number.
 * @param[in]
 *     device_addr: slave device 7bits address.
 * @param[in]
 *     speed_khz: i2c transfer speed.
 * @param[out]
 *     buffer: read data buffer pointer.
 * @param[in]
 *     len: read data length.
 * @return
 *     0, i2c transfer successfully.\n
 *     error code from mtk_i2c_transfer().
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     If mtk_i2c_transfer() fails, return its error code.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
int mtk_i2c_read(uint8_t bus_num, uint8_t device_addr, uint32_t speed_khz,
		 uint8_t *buffer, uint16_t len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	struct mtk_i2c i2c_mtk;
	struct mtk_i2c *i2c = &i2c_mtk;
	memset(i2c, 0, sizeof(struct mtk_i2c));
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	i2c->id = bus_num;
	i2c->addr = device_addr;
	i2c->speed = speed_khz;
	i2c->mode = 0;
	msgs.addr = i2c->addr;
	msgs.flags = 1;
	msgs.buf = buffer;
	msgs.len = len;
	ret = mtk_i2c_transfer(i2c, &msgs, 1);
	if ((i2c->filter_msg == false) && (ret != I2C_OK))
		I2CERR("mtk_i2c_read fails(%d).\n", ret);
	return ret;
}
/** @ingroup IP_group_i2c_external_function
 * @par Description
 *     Initialize struct mtk_i2c and i2c_msg, then write data to\n
 *     slave device.
 * @param[in]
 *     bus_num: i2c bus number.
 * @param[in]
 *     device_addr: slave device 7bits address.
 * @param[in]
 *     speed_khz: i2c transfer speed.
 * @param[in]
 *     buffer: write data buffer pointer.
 * @param[in]
 *     len: write data length.
 * @return
 *     0, i2c transfer successfully.\n
 *     error code from mtk_i2c_transfer().
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     If mtk_i2c_transfer() fails, return its error code.\n
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
int mtk_i2c_write(uint8_t bus_num, uint8_t device_addr, uint32_t speed_khz,
		  uint8_t *buffer, uint16_t len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	struct mtk_i2c i2c_mtk;
	struct mtk_i2c *i2c = &i2c_mtk;
	memset(i2c, 0, sizeof(struct mtk_i2c));
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	i2c->id = bus_num;
	i2c->addr = device_addr;
	i2c->speed = speed_khz;
	i2c->mode = 0;
	msgs.addr = i2c->addr;
	msgs.flags = 0;
	msgs.buf = buffer;
	msgs.len = len;
	ret = mtk_i2c_transfer(i2c, &msgs, 1);
	if ((i2c->filter_msg == false) && (ret != I2C_OK))
		I2CERR("mtk_i2c_write fails(%d).\n", ret);
	return ret;
}
/** @ingroup IP_group_i2c_external_function
 * @par Description
 *     Initialize struct mtk_i2c and i2c_msg, first write data to\n
 *     slave device then read data from slave device.
 * @param[in]
 *     bus_num: i2c bus number.
 * @param[in]
 *     device_addr: slave device 7bits address.
 * @param[in]
 *     speed_khz: i2c transfer speed.
 * @param[in]
 *     write_buffer: write data buffer pointer.
 * @param[out]
 *     read_buffer: read data buffer pointer.
 * @param[in]
 *     write_len: write data length.
 * @param[in]
 *     read_len: read data length.
 * @return
 *     0, i2c transfer successfully.\n
 *     error code from mtk_i2c_transfer().
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     If mtk_i2c_transfer() fails, return its error code.\n
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
int mtk_i2c_write_read(uint8_t bus_num, uint8_t device_addr, uint32_t speed_khz,
		       uint8_t *write_buffer, uint8_t *read_buffer,
		       uint16_t write_len, uint16_t read_len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs[2];
	struct mtk_i2c i2c_mtk;
	struct mtk_i2c *i2c = &i2c_mtk;
	memset(i2c, 0, sizeof(struct mtk_i2c));
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	i2c->id = bus_num;
	i2c->addr = device_addr;
	i2c->speed = speed_khz;
	i2c->mode = 0;
	msgs[0].addr = i2c->addr;
	msgs[0].flags = 0;
	msgs[0].buf = write_buffer;
	msgs[0].len = write_len;
	msgs[1].addr = i2c->addr;
	msgs[1].flags = 1;
	msgs[1].buf = read_buffer;
	msgs[1].len = read_len;
	ret = mtk_i2c_transfer(i2c, msgs, 2);
	if ((i2c->filter_msg == false) && (ret != I2C_OK))
		I2CERR("mtk_i2c_write_read fails(%d).\n", ret);
	return ret;
}

int i2c_read(mt_i2c *i2c,uint8_t *buffer, uint32_t len)
{
  return 0;
}

int i2c_write(mt_i2c *i2c,uint8_t  *buffer, uint32_t len)
{
  return 0;
}

int i2c_write_read(mt_i2c *i2c,uint8_t *buffer, uint32_t write_len, uint32_t read_len)
{
  return 0;
}

int i2c_hw_init(void)
{
	return 0;
}