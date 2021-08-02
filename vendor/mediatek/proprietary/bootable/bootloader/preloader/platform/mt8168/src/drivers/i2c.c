/*
 * Copyright (c) 2016 MediaTek Inc.
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
#include <i2c.h>
#include <gpio.h>
#include <sync_write.h>
#include <pmic.h>
mt_i2c *i2c_g;
static inline void i2c_writel(mt_i2c *i2c, u8 offset, u32 value)
{
	mt_reg_sync_writel(value, (i2c->base) + (offset));
}
static inline u32 i2c_readl(mt_i2c *i2c, u8 offset)
{
    return DRV_Reg32(i2c->base + offset);
}
static int mtk_i2c_clock_enable(mt_i2c *i2c)
{
	switch (i2c->id) {
	case 0:
		mt_reg_sync_writel(MTK_I2C0_CLK_OFFSET, MTK_I2C_CLK_CLR);
		break;
	case 1:
		mt_reg_sync_writel(MTK_I2C1_CLK_OFFSET, MTK_I2C_CLK_CLR);
		break;
	case 2:
		mt_reg_sync_writel(MTK_I2C2_CLK_OFFSET, MTK_I2C_CLK_CLR);
		break;
	case 3:
		mt_reg_sync_writel(MTK_I2C3_CLK_OFFSET, MTK_I2C_CLK_CLR);
		break;
	default:
		dprintf(I2CTAG "i2c clk enable, invalid para: i2c->id=%d\n",i2c->id);
		return -EINVAL_I2C;
    }
    mt_reg_sync_writel(MTK_APDMA_CLK_OFFSET, MTK_I2C_CLK_CLR);
	return 0;
}
static int mtk_i2c_clock_disable(mt_i2c *i2c)
{
	switch (i2c->id) {
	case 0:
		mt_reg_sync_writel(MTK_I2C0_CLK_OFFSET, MTK_I2C_CLK_SET);
		break;
	case 1:
		mt_reg_sync_writel(MTK_I2C1_CLK_OFFSET, MTK_I2C_CLK_SET);
		break;
	case 2:
		mt_reg_sync_writel(MTK_I2C2_CLK_OFFSET, MTK_I2C_CLK_SET);
		break;
	case 3:
		mt_reg_sync_writel(MTK_I2C3_CLK_OFFSET, MTK_I2C_CLK_SET);
		break;
	default:
		dprintf(I2CTAG "i2c clk disable, invalid para: i2c->id=%d\n",i2c->id);
		return -EINVAL_I2C;
	}
	mt_reg_sync_writel(MTK_APDMA_CLK_OFFSET, MTK_I2C_CLK_SET);
	return 0;
}

void mtk_i2c_irq(void)
{
	u16 restart_flag = 0;
	u16 intr_stat;
	if (i2c_g->auto_restart)
		restart_flag = I2C_RS_TRANSFER;
	intr_stat = i2c_readl(i2c_g, OFFSET_INTR_STAT);
	i2c_writel(i2c_g, OFFSET_INTR_STAT, intr_stat);
	/*
	 * when occurs ack error, i2c controller generate two interrupts
	 * first is the ack error interrupt, then the complete interrupt
	 * i2c->irq_stat need keep the two interrupt value.
	 */
	i2c_g->irq_stat |= intr_stat;
	if (i2c_g->irq_stat & (I2C_TRANSAC_COMP | restart_flag))
		i2c_g->msg_complete = true;
}
static int mtk_i2c_init_base(mt_i2c *i2c)
{
	switch (i2c->id) {
	case 0:
		i2c->base = MTK_I2C0_BASE;
		i2c->pdmabase = MTK_I2C0_DMA;
		i2c->irqnr = MTK_I2C0_GIC_IRQ;
		break;
	case 1:
		i2c->base = MTK_I2C1_BASE;
		i2c->pdmabase = MTK_I2C1_DMA;
		i2c->irqnr = MTK_I2C1_GIC_IRQ;
		break;
	case 2:
		i2c->base = MTK_I2C2_BASE;
		i2c->pdmabase = MTK_I2C2_DMA;
		i2c->irqnr = MTK_I2C2_GIC_IRQ;
		break;
	case 3:
		i2c->base = MTK_I2C3_BASE;
		i2c->pdmabase = MTK_I2C3_DMA;
		i2c->irqnr = MTK_I2C3_GIC_IRQ;
		break;
	default:
		dprintf(I2CTAG "invalid para: i2c->id=%d\n",i2c->id);
		return -EINVAL_I2C;
	}

	i2c->clk = MTK_I2C_SOURCE_CLK;
	i2c->clk_src_div = MTK_I2C_CLK_DIV;
	i2c->aux_len_reg = true;
	return 0;
}
static void mtk_i2c_init_hw(mt_i2c *i2c)
{
	u16 control_reg;
	i2c_writel(i2c, OFFSET_SOFTRESET, I2C_SOFT_RST);
	/* set ioconfig */
	if (i2c->pushpull)
		i2c_writel(i2c, OFFSET_IO_CONFIG, I2C_IO_CONFIG_PUSH_PULL);
	else
		i2c_writel(i2c, OFFSET_IO_CONFIG, I2C_IO_CONFIG_OPEN_DRAIN);
	i2c_writel(i2c, OFFSET_DCM_EN, I2C_DCM_DISABLE);
	control_reg = I2C_CONTROL_ACKERR_DET_EN | I2C_CONTROL_CLK_EXT_EN;
	i2c_writel(i2c, OFFSET_CONTROL, control_reg);
	i2c_writel(i2c, OFFSET_DELAY_LEN, I2C_DELAY_LEN);
	i2c_writel(i2c, OFFSET_CLOCK_DIV, (I2C_DEFAULT_CLK_DIV - 1));
	mt_reg_sync_writel(I2C_DMA_HARD_RST, i2c->pdmabase + OFFSET_RST);
	mt_reg_sync_writel(I2C_DMA_CLR_FLAG, i2c->pdmabase + OFFSET_RST);
}
/*
 * Calculate i2c port speed
 *
 * Hardware design:
 * i2c_bus_freq = parent_clk / (clock_div * 2 * sample_cnt * step_cnt)
 * clock_div: fixed in hardware, but may be various in different SoCs
 *
 * The calculation want to pick the highest bus frequency that is still
 * less than or equal to i2c->speed_hz. The calculation try to get
 * sample_cnt and step_cn
 */
static int mtk_i2c_calculate_speed(unsigned int clk_src,
                                   unsigned int target_speed,
                                   unsigned int *timing_step_cnt,
                                   unsigned int *timing_sample_cnt)
{
	unsigned int step_cnt;
	unsigned int sample_cnt;
	unsigned int max_step_cnt;
	unsigned int base_sample_cnt = MAX_SAMPLE_CNT_DIV;;
	unsigned int base_step_cnt;
	unsigned int opt_div;
	unsigned int best_mul;
	unsigned int cnt_mul;
	if (target_speed > MAX_HS_MODE_SPEED)
		target_speed = MAX_HS_MODE_SPEED;
	if (target_speed > MAX_FS_MODE_SPEED)
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
		/* In this case, hardware can't support such
		 * low i2c_bus_freq
		 */
		dprintf(I2CTAG "Unsupported speed (%u khz)\n", target_speed);
		return -EINVAL_I2C;
	}
	*timing_step_cnt = step_cnt - 1;
	*timing_sample_cnt = sample_cnt - 1;
	return 0;
}
static int mtk_i2c_set_speed(mt_i2c *i2c)
{
	unsigned int clk_src;
	unsigned int step_cnt;
	unsigned int sample_cnt;
	unsigned int target_speed;
	int ret;
	if (i2c->speed == 0)
		i2c->speed = MAX_ST_MODE_SPEED;
	if (i2c->clk_src_div == 0)
		i2c->clk_src_div = MTK_I2C_CLK_DIV;
	i2c->clk_src_div *= I2C_DEFAULT_CLK_DIV;
	clk_src = (i2c->clk) / (i2c->clk_src_div);
	target_speed = i2c->speed;
	if (target_speed > MAX_FS_MODE_SPEED) {
		/* set master code speed register */
		ret = mtk_i2c_calculate_speed(clk_src, MAX_FS_MODE_SPEED,
					&step_cnt, &sample_cnt);
		if (ret < 0)
			return ret;
		i2c->timing_reg = (sample_cnt << 8) | step_cnt;
		/* set the high speed mode register */
		ret = mtk_i2c_calculate_speed(clk_src, target_speed,
					&step_cnt, &sample_cnt);
		if (ret < 0)
			return ret;
		i2c->high_speed_reg = I2C_TIME_DEFAULT_VALUE |
				(sample_cnt << 12) | (step_cnt << 8);
	} else {
		ret = mtk_i2c_calculate_speed(clk_src, target_speed,
					&step_cnt, &sample_cnt);
		if (ret < 0)
			return ret;
		i2c->timing_reg = (sample_cnt << 8) | step_cnt;
		/* disable the high speed transaction */
		i2c->high_speed_reg = I2C_TIME_CLR_VALUE;
	}
	i2c_writel(i2c, OFFSET_TIMING, i2c->timing_reg);
	i2c_writel(i2c, OFFSET_HS, i2c->high_speed_reg);
	return 0;
}
static void i2c_dump_info(mt_i2c *i2c)
{
	dprintf(I2CTAG "I2C structure:\n"
			I2CTAG"Id=%d,Dma_en=%x,Auto_restart=%x,Poll_en=%x,Op=%x\n"
			I2CTAG"Irq_stat=%x,source_clk=%d,clk_div=%d,speed=%d\n",
			i2c->id,i2c->dma_en,i2c->auto_restart,i2c->poll_en,i2c->op,
			i2c->irq_stat,i2c->clk,i2c->clk_src_div,i2c->speed);
	dprintf(I2CTAG "base address 0x%x\n",i2c->base);
	dprintf(I2CTAG "I2C register:\n"
			I2CTAG"SLAVE_ADDR=%x,INTR_MASK=%x,INTR_STAT=%x,CONTROL=%x\n"
			I2CTAG"TRANSFER_LEN=%x,TRANSAC_LEN=%x,DELAY_LEN=%x\n"
			I2CTAG"TIMING=%x,START=%x,FIFO_STAT=%x,IO_CONFIG=%x,HS=%x\n"
			I2CTAG"DCM_EN=%x,DEBUGSTAT=%x,EXT_CONF=%x\n"
			I2CTAG"TRANSFER_LEN_AUX=%x,FIFO_THRESH=%x,RSV_DEBUG=%x\n"
			I2CTAG"DEBUGCTRL=%x,CLOCK_DIV=%x,SCL_HL_RATIO=%x\n"
			I2CTAG"SCL_HS_HL_RATIO=%x,SCL_MIS_COMP_POINT=%x\n"
			I2CTAG"STA_STOP_AC_TIME=%x,HS_STA_STOP_AC_TIME=%x\n"
			I2CTAG"DATA_TIME=%x,TIME_OUT=%x\n",
			(i2c_readl(i2c, OFFSET_SLAVE_ADDR)),
			(i2c_readl(i2c, OFFSET_INTR_MASK)),
			(i2c_readl(i2c, OFFSET_INTR_STAT)),
			(i2c_readl(i2c, OFFSET_CONTROL)),
			(i2c_readl(i2c, OFFSET_TRANSFER_LEN)),
			(i2c_readl(i2c, OFFSET_TRANSAC_LEN)),
			(i2c_readl(i2c, OFFSET_DELAY_LEN)),
			(i2c_readl(i2c, OFFSET_TIMING)),
			(i2c_readl(i2c, OFFSET_START)),
			(i2c_readl(i2c, OFFSET_FIFO_STAT)),
			(i2c_readl(i2c, OFFSET_IO_CONFIG)),
			(i2c_readl(i2c, OFFSET_HS)),
			(i2c_readl(i2c, OFFSET_DCM_EN)),
			(i2c_readl(i2c, OFFSET_DEBUGSTAT)),
			(i2c_readl(i2c, OFFSET_EXT_CONF)),
			(i2c_readl(i2c, OFFSET_TRANSFER_LEN_AUX)),
			(i2c_readl(i2c, OFFSET_FIFO_THRESH)),
			(i2c_readl(i2c, OFFSET_RSV_DEBUG)),
			(i2c_readl(i2c, OFFSET_DEBUGCTRL)),
			(i2c_readl(i2c, OFFSET_CLOCK_DIV)),
			(i2c_readl(i2c, OFFSET_SCL_HL_RATIO)),
			(i2c_readl(i2c, OFFSET_SCL_HS_HL_RATIO)),
			(i2c_readl(i2c, OFFSET_SCL_MIS_COMP_POINT)),
			(i2c_readl(i2c, OFFSET_STA_STOP_AC_TIME)),
			(i2c_readl(i2c, OFFSET_HS_STA_STOP_AC_TIME)),
			(i2c_readl(i2c, OFFSET_DATA_TIME)),
			(i2c_readl(i2c, OFFSET_TIME_OUT)));
}
static int mtk_i2c_do_transfer(mt_i2c *i2c, struct i2c_msg *msgs,
                               int num, int left_num)
{
	int ret;
	u16 addr_reg;
	u16 start_reg;
	u16 control_reg;
	u16 restart_flag = 0;
	u8 trans_error = 0;
	u8 tmo = 1;
	u8 *ptr = msgs->buf;
	u16 data_size = msgs->len;
	u32 tmo_poll = 0xfffff;
	i2c->irq_stat = 0;
	i2c->msg_complete = false;
	if (i2c->auto_restart)
		restart_flag = I2C_RS_TRANSFER;
	control_reg = i2c_readl(i2c, OFFSET_CONTROL) &
				  ~(I2C_CONTROL_DIR_CHANGE | I2C_CONTROL_RS);
	if ((i2c->speed > MAX_FS_MODE_SPEED) || (num > 1))
		control_reg |= I2C_CONTROL_RS;
	if (i2c->op == I2C_MASTER_WRRD)
		control_reg |= I2C_CONTROL_DIR_CHANGE | I2C_CONTROL_RS;
	if (i2c->dma_en)
		control_reg |= I2C_CONTROL_DMA_EN;
	i2c_writel(i2c, OFFSET_CONTROL, control_reg);
	/* set start condition */
	if (i2c->speed <= MAX_ST_MODE_SPEED)
		i2c_writel(i2c, OFFSET_EXT_CONF, I2C_ST_START_CON);
	else
		i2c_writel(i2c, OFFSET_EXT_CONF, I2C_FS_START_CON);
	addr_reg = msgs->addr << 1;
	if (i2c->op == I2C_MASTER_RD)
		addr_reg |= 0x1;
	i2c_writel(i2c, OFFSET_SLAVE_ADDR, addr_reg);
	/* clear interrupt status */
	i2c_writel(i2c, OFFSET_INTR_STAT, restart_flag | I2C_HS_NACKERR |
			   I2C_ACKERR | I2C_TRANSAC_COMP);
	i2c_writel(i2c, OFFSET_FIFO_ADDR_CLR, I2C_FIFO_ADDR_CLR);
	/* enable interrupt */
	i2c_writel(i2c, OFFSET_INTR_MASK, restart_flag | I2C_HS_NACKERR |
			   I2C_ACKERR | I2C_TRANSAC_COMP);
	/* set transfer and transaction len */
	if (i2c->op == I2C_MASTER_WRRD) {
		if (i2c->aux_len_reg) {
			i2c_writel(i2c, OFFSET_TRANSFER_LEN, msgs->len);
			i2c_writel(i2c, OFFSET_TRANSFER_LEN_AUX, (msgs + 1)->len);
		} else {
			i2c_writel(i2c, OFFSET_TRANSFER_LEN,
					   (msgs->len | (((msgs + 1)->len) << 8)));
		}
		i2c_writel(i2c, OFFSET_TRANSAC_LEN, I2C_WRRD_TRANAC_VALUE);
	} else {
		i2c_writel(i2c, OFFSET_TRANSFER_LEN, msgs->len);
		i2c_writel(i2c, OFFSET_TRANSAC_LEN, num);
	}
	if (i2c->dma_en) {
		/* prepare buffer data to start transfer */
		if (i2c->op == I2C_MASTER_RD) {
			mt_reg_sync_writel(I2C_DMA_INT_FLAG_NONE, i2c->pdmabase + OFFSET_INT_FLAG);
			mt_reg_sync_writel(I2C_DMA_CON_RX, i2c->pdmabase + OFFSET_CON);
			mt_reg_sync_writel((u32)((addr_t)i2c->rx_buff), i2c->pdmabase + OFFSET_RX_MEM_ADDR);
			mt_reg_sync_writel(msgs->len, i2c->pdmabase + OFFSET_RX_LEN);
		} else if (i2c->op == I2C_MASTER_WR) {
			mt_reg_sync_writel(I2C_DMA_INT_FLAG_NONE, i2c->pdmabase + OFFSET_INT_FLAG);
			mt_reg_sync_writel(I2C_DMA_CON_TX, i2c->pdmabase + OFFSET_CON);
			memcpy(i2c->tx_buff, msgs->buf, msgs->len);
			mt_reg_sync_writel((u32)((addr_t)i2c->tx_buff), i2c->pdmabase + OFFSET_TX_MEM_ADDR);
			mt_reg_sync_writel(msgs->len, i2c->pdmabase + OFFSET_TX_LEN);
		} else {
			mt_reg_sync_writel(I2C_DMA_CLR_FLAG, i2c->pdmabase + OFFSET_INT_FLAG);
			mt_reg_sync_writel(I2C_DMA_CLR_FLAG, i2c->pdmabase + OFFSET_CON);
			memcpy(i2c->tx_buff, msgs->buf, msgs->len);
			mt_reg_sync_writel((u32)((addr_t)i2c->tx_buff), i2c->pdmabase + OFFSET_TX_MEM_ADDR);
			mt_reg_sync_writel((u32)((addr_t)i2c->rx_buff), i2c->pdmabase + OFFSET_RX_MEM_ADDR);
			mt_reg_sync_writel(msgs->len, i2c->pdmabase + OFFSET_TX_LEN);
			mt_reg_sync_writel((msgs + 1)->len, i2c->pdmabase + OFFSET_RX_LEN);
		}
		mt_reg_sync_writel(I2C_DMA_START_EN, i2c->pdmabase + OFFSET_EN);
	} else {
		if (I2C_MASTER_RD != i2c->op) {
			while (data_size--) {
				i2c_writel(i2c, OFFSET_DATA_PORT, *ptr);
				ptr++;
			}
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
				tmo = 1;
				if (i2c->irq_stat & (I2C_HS_NACKERR | I2C_ACKERR ))
					trans_error = 1;
				break;
			}
			tmo_poll--;
			if (tmo_poll == 0) {
				tmo = 0;
				break;
			}
		}
	} else {
		for (;;) {
			if (i2c->msg_complete &&
				(i2c->irq_stat & (I2C_TRANSAC_COMP | restart_flag))) {
				tmo = 1;
				if (i2c->irq_stat & (I2C_HS_NACKERR | I2C_ACKERR ))
					trans_error = 1;
				break;
			}
			tmo_poll--;
			if (tmo_poll == 0) {
				tmo = 0;
				break;
			}
		}
	}
	/* clear interrupt mask */
	i2c_writel(i2c, OFFSET_INTR_MASK,
			~(restart_flag | I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
	if ((tmo != 0 ) && (trans_error == 0)) {
		/* transfer success ,we need to get data from fifo */
		if ((i2c->op == I2C_MASTER_RD) || (i2c->op == I2C_MASTER_WRRD)) {
			ptr = (i2c->op == I2C_MASTER_RD) ?
				  msgs->buf : (msgs + 1)->buf;
			if (!i2c->dma_en) {
				data_size = i2c_readl(i2c, OFFSET_FIFO_STAT1) & 0x001F;
				while (data_size--) {
					*ptr = i2c_readl(i2c, OFFSET_DATA_PORT);
					ptr++;
				}
			} else {
				data_size = (i2c->op == I2C_MASTER_RD) ?
					msgs->len : (msgs + 1)->len;
				memcpy(ptr, i2c->rx_buff, data_size);
			}
		}
	} else {
		/* timeout or ACKERR */
		if ( tmo == 0 ) {
			ret = -ETIMEDOUT_I2C;
		} else {
			ret = -EREMOTEIO_I2C;
		}
		if (i2c->filter_msg == false) {
			if ( tmo == 0 ) {
				dprintf(I2CTAG "id=%d,addr: %x, transfer timeout\n",
					   i2c->id, msgs->addr);
			} else {
				dprintf(I2CTAG "id=%d,addr: %x, transfer error\n",
					   i2c->id, msgs->addr);
			}
			if (i2c->irq_stat & I2C_HS_NACKERR)
				dprintf(I2CTAG "I2C_HS_NACKERR\n");
			if (i2c->irq_stat & I2C_ACKERR)
				dprintf(I2CTAG "I2C_ACKERR\n");
			i2c_dump_info(i2c);
		}
		mtk_i2c_init_hw(i2c);
		if (mtk_i2c_set_speed(i2c)) {
			dprintf(I2CTAG "Failed to set the speed.\n");
		}
		return ret;
	}
	return 0;
}
int mtk_i2c_transfer(mt_i2c *i2c,
                     struct i2c_msg msgs[], int num)
{
	int ret;
	int left_num = num;
	u8 num_cnt;
	ret = mtk_i2c_init_base(i2c);
	if (ret) {
		dprintf(I2CTAG "Failed to init i2c base.\n");
		return ret;
	}
	mtk_i2c_clock_enable(i2c);
	mtk_i2c_init_hw(i2c);
	ret = mtk_i2c_set_speed(i2c);
	if (ret) {
		dprintf(I2CTAG "Failed to set the speed.\n");
		return ret;
	}
	i2c_g = i2c;
	for (num_cnt = 0; num_cnt < num; num_cnt++) {
		if (((msgs+num_cnt)->addr) > 0x7f) {
			dprintf(I2CTAG "i2c addr: msgs[%d]->addr(%x) > 0x7f, error! \n",
					num_cnt, ((msgs+num_cnt)->addr));
			return -EINVAL_I2C;
		}
		if (((msgs+num_cnt)->len) > 8) {
			dprintf(I2CTAG "FIFO MODE: msgs[%d]->len(%d) > 8, error! \n",
					num_cnt, ((msgs+num_cnt)->len));
			return -EINVAL_I2C;
		}
		if ((i2c_g->dma_en) && (((msgs+num_cnt)->len) > 255)) {
			dprintf(I2CTAG "DMA MODE: msgs[%d]->len(%d) > 255, error! \n",
					num_cnt, ((msgs+num_cnt)->len));
			return -EINVAL_I2C;
		}
	}
	if (i2c_g->dma_en) {
		i2c_g->tx_buff = malloc(256);
		i2c_g->rx_buff = malloc(256);
	}
	while (left_num--) {
		if (!msgs->buf) {
			dprintf(I2CTAG "data buffer is NULL.\n");
			ret = -EINVAL_I2C;
			goto err_exit;
		}
		if (msgs->flags & I2C_M_RD)
			i2c_g->op = I2C_MASTER_RD;
		else
			i2c_g->op = I2C_MASTER_WR;
		if (!i2c_g->auto_restart) {
			if (num > 1) {
				/* combined two messages into one transaction */
				i2c_g->op = I2C_MASTER_WRRD;
				left_num--;
			}
		}
		ret = mtk_i2c_do_transfer(i2c_g, msgs, num, left_num);
		if (ret < 0)
			goto err_exit;
		msgs++;
	}
	ret = I2C_OK;
	err_exit:
	if (i2c_g->dma_en) {
		free(i2c_g->tx_buff);
		free(i2c_g->rx_buff);
	}
	return ret;
}
/*
 * New read interface: Read bytes
 *   mt_i2c:    I2C chip config, see mt_i2c.
 *   buffer:  Where to read/write the data.
 *   len:     How many bytes to read/write
 *   Returns: ERROR_CODE
 */
int i2c_read(mt_i2c *i2c, u8 *buffer, u32 len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	msgs.addr = i2c->addr;
	msgs.flags = 1;
	msgs.buf = buffer;
	msgs.len = len;
	ret = mtk_i2c_transfer(i2c, &msgs, 1);
	if((i2c->filter_msg == false) && (I2C_OK != ret))
		dprintf(I2CTAG "i2c_read fails(%d).\n",ret);
	return ret;
}
int mtk_i2c_read(u16 bus_num, u8 device_addr, u32 speed_khz,
                 u8 *buffer, u32 len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	mt_i2c i2c_mtk;
	mt_i2c *i2c = &i2c_mtk;
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	i2c->id = bus_num;
	i2c->addr = device_addr;
	i2c->speed = speed_khz;
	msgs.addr = i2c->addr;
	msgs.flags = 1;
	msgs.buf = buffer;
	msgs.len = len;
	ret = mtk_i2c_transfer(i2c, &msgs, 1);
	if((i2c->filter_msg == false) && (I2C_OK != ret))
		dprintf(I2CTAG "mtk_i2c_read fails(%d).\n",ret);
	return ret;
}
/*
 * New write interface: Write bytes
 *   i2c:    I2C chip config, see mt_i2c.
 *   buffer:  Where to read/write the data.
 *   len:     How many bytes to read/write
 *   Returns: ERROR_CODE
 */
int i2c_write(mt_i2c *i2c, u8 *buffer, u32 len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	msgs.addr = i2c->addr;
	msgs.flags = 0;
	msgs.buf = buffer;
	msgs.len = len;
	ret = mtk_i2c_transfer(i2c, &msgs, 1);
	if((i2c->filter_msg == false) && (I2C_OK != ret))
		dprintf(I2CTAG "i2c_write fails(%d).\n",ret);
	return ret;
}
int mtk_i2c_write(u16 bus_num, u8 device_addr, u32 speed_khz,
                  u8 *buffer, u32 len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	mt_i2c i2c_mtk;
	mt_i2c *i2c = &i2c_mtk;
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	i2c->id = bus_num;
	i2c->addr = device_addr;
	i2c->speed = speed_khz;
	msgs.addr = i2c->addr;
	msgs.flags = 0;
	msgs.buf = buffer;
	msgs.len = len;
	ret = mtk_i2c_transfer(i2c, &msgs, 1);
	if((i2c->filter_msg == false) && (I2C_OK != ret))
		dprintf(I2CTAG "mtk_i2c_write fails(%d).\n",ret);
	return ret;
}
/*
 * New write then read back interface: Write bytes then read bytes
 *   i2c:    I2C chip config, see mt_i2c.
 *   buffer:  Where to read/write the data.
 *   write_len:     How many bytes to write
 *   read_len:     How many bytes to read
 *   Returns: ERROR_CODE
 */
int i2c_write_read(mt_i2c *i2c, u8 *buffer, u32 write_len, u32 read_len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs[2];
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	msgs[0].addr = i2c->addr;
	msgs[0].flags = 0;
	msgs[0].buf = buffer;
	msgs[0].len = write_len;
	msgs[1].addr = i2c->addr;
	msgs[1].flags = 1;
	msgs[1].buf = buffer;
	msgs[1].len = read_len;
	ret = mtk_i2c_transfer(i2c, msgs, 2);
	if((i2c->filter_msg == false) && (I2C_OK != ret))
		dprintf(I2CTAG "i2c_write_read fails(%d).\n",ret);
	return ret;
}
int mtk_i2c_write_read(u16 bus_num, u8 device_addr, u32 speed_khz,
                       u8 *write_buffer, u8 *read_buffer,
                       u32 write_len, u32 read_len)
{
	int ret = I2C_OK;
	struct i2c_msg msgs[2];
	mt_i2c i2c_mtk;
	mt_i2c *i2c = &i2c_mtk;
	i2c->poll_en = true;
	i2c->dma_en = false;
	i2c->auto_restart = false;
	i2c->pushpull = false;
	i2c->filter_msg = false;
	i2c->id = bus_num;
	i2c->addr = device_addr;
	i2c->speed = speed_khz;
	msgs[0].addr = i2c->addr;
	msgs[0].flags = 0;
	msgs[0].buf = write_buffer;
	msgs[0].len = write_len;
	msgs[1].addr = i2c->addr;
	msgs[1].flags = 1;
	msgs[1].buf = read_buffer;
	msgs[1].len = read_len;
	ret = mtk_i2c_transfer(i2c, msgs, 2);
	if((i2c->filter_msg == false) && (I2C_OK != ret))
		dprintf(I2CTAG "mtk_i2c_write_read fails(%d).\n",ret);
	return ret;
}

int i2c_hw_init(void)
{
	u32 gpio_reg;
	/* I2C0_SDA */
	gpio_reg = (DRV_Reg32(MTK_GPIO_I2C_BASE0) &
		   (~(0x7 << MTK_GPIO_SDA0))) | (0x1 << MTK_GPIO_SDA0);
	DRV_WriteReg32(MTK_GPIO_I2C_BASE0, gpio_reg);

	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_ENABLE_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SDA0)),
			MTK_GPIO_I2C_PULL_ENABLE_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_SEL_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SDA0)),
			MTK_GPIO_I2C_PULL_SEL_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_RESL_BASE) |
			(0x3 << MTK_GPIO_RESL_SDA0)),
			MTK_GPIO_I2C_RESL_BASE);

	/* I2C0_SCL */
	gpio_reg = (DRV_Reg32(MTK_GPIO_I2C_BASE0) &
		   (~(0x7 << MTK_GPIO_SCL0))) | (0x1 << MTK_GPIO_SCL0);
	DRV_WriteReg32(MTK_GPIO_I2C_BASE0, gpio_reg);

	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_ENABLE_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SCL0)),
			MTK_GPIO_I2C_PULL_ENABLE_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_SEL_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SCL0)),
			MTK_GPIO_I2C_PULL_SEL_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_RESL_BASE) |
			(0x3 << MTK_GPIO_RESL_SCL0)),
			MTK_GPIO_I2C_RESL_BASE);

	/* I2C1_SDA */
	gpio_reg = (DRV_Reg32(MTK_GPIO_I2C_BASE0) &
		   (~(0x7 << MTK_GPIO_SDA1))) | (0x1 << MTK_GPIO_SDA1);
	DRV_WriteReg32(MTK_GPIO_I2C_BASE0, gpio_reg);

	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_ENABLE_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SDA1)),
			MTK_GPIO_I2C_PULL_ENABLE_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_SEL_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SDA1)),
			MTK_GPIO_I2C_PULL_SEL_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_RESL_BASE) |
			(0x3 << MTK_GPIO_RESL_SDA1)),
			MTK_GPIO_I2C_RESL_BASE);

	/* I2C1_SCL */
	gpio_reg = (DRV_Reg32(MTK_GPIO_I2C_BASE1) &
		   (~(0x7 << MTK_GPIO_SCL1))) | (0x1 << MTK_GPIO_SCL1);
	DRV_WriteReg32(MTK_GPIO_I2C_BASE1, gpio_reg);

	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_ENABLE_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SCL1)),
			MTK_GPIO_I2C_PULL_ENABLE_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_SEL_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SCL1)),
			MTK_GPIO_I2C_PULL_SEL_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_RESL_BASE) |
			(0x3 << MTK_GPIO_RESL_SCL1)),
			MTK_GPIO_I2C_RESL_BASE);

	/* I2C2_SDA */
	gpio_reg = (DRV_Reg32(MTK_GPIO_I2C_BASE1) &
		   (~(0x7 << MTK_GPIO_SDA2))) | (0x1 << MTK_GPIO_SDA2);
	DRV_WriteReg32(MTK_GPIO_I2C_BASE1, gpio_reg);

	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_ENABLE_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SDA2)),
			MTK_GPIO_I2C_PULL_ENABLE_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_SEL_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SDA2)),
			MTK_GPIO_I2C_PULL_SEL_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_RESL_BASE) |
			(0x3 << MTK_GPIO_RESL_SDA2)),
			MTK_GPIO_I2C_RESL_BASE);

	/* I2C2_SCL */
	gpio_reg = (DRV_Reg32(MTK_GPIO_I2C_BASE1) &
		   (~(0x7 << MTK_GPIO_SCL2))) | (0x1 << MTK_GPIO_SCL2);
	DRV_WriteReg32(MTK_GPIO_I2C_BASE1, gpio_reg);

	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_ENABLE_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SCL2)),
			MTK_GPIO_I2C_PULL_ENABLE_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_SEL_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SCL2)),
			MTK_GPIO_I2C_PULL_SEL_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_RESL_BASE) |
			(0x3 << MTK_GPIO_RESL_SCL2)),
			MTK_GPIO_I2C_RESL_BASE);

	/* I2C3_SDA */
	gpio_reg = (DRV_Reg32(MTK_GPIO_I2C_BASE1) &
		   (~(0x7 << MTK_GPIO_SDA3))) | (0x1 << MTK_GPIO_SDA3);
	DRV_WriteReg32(MTK_GPIO_I2C_BASE1, gpio_reg);

	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_ENABLE_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SDA3)),
			MTK_GPIO_I2C_PULL_ENABLE_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_SEL_BASE0) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SDA3)),
			MTK_GPIO_I2C_PULL_SEL_BASE0);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_RESL_BASE) |
			(0x3 << MTK_GPIO_RESL_SDA3)),
			MTK_GPIO_I2C_RESL_BASE);

	/* I2C3_SCL */
	gpio_reg = (DRV_Reg32(MTK_GPIO_I2C_BASE1) &
		   (~(0x7 << MTK_GPIO_SCL3))) | (0x1 << MTK_GPIO_SCL3);
	DRV_WriteReg32(MTK_GPIO_I2C_BASE1, gpio_reg);

	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_ENABLE_BASE1) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SCL3)),
			MTK_GPIO_I2C_PULL_ENABLE_BASE1);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_PULL_SEL_BASE1) |
			(0x1 << MTK_GPIO_PULL_ENABLE_SCL3)),
			MTK_GPIO_I2C_PULL_SEL_BASE1);
	mt_reg_sync_writel((DRV_Reg32(MTK_GPIO_I2C_RESL_BASE) |
			(0x3 << MTK_GPIO_RESL_SCL3)),
			MTK_GPIO_I2C_RESL_BASE);

	return 0;
}