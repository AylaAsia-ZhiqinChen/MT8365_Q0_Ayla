/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "driver_api.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <i2c.h> /*Common code header*/
#include <task.h>
//#include "irq.h"

SemaphoreHandle_t i2c_mutex[I2C_NR];/* For semaphore handling */
SemaphoreHandle_t i2c_irq_semaphore[I2C_NR];/* For interrupt handling */
static uint32_t irq_stat[I2C_NR];

/*dump reg to memory*/
#define DUMP_REG_BAK_MAGIC 0x12c0c0de
#define REG_BAK_CNT 16
static uint32_t i2c_reg_bak[REG_BAK_CNT+1];
static const uint8_t i2c_backup_reg_off[] = {
    OFFSET_SLAVE_ADDR,
    OFFSET_INTR_MASK,
    OFFSET_INTR_STAT,
    OFFSET_CONTROL,
    OFFSET_TRANSFER_LEN,
    OFFSET_TRANSAC_LEN,
    OFFSET_DELAY_LEN,
    OFFSET_TIMING,
    OFFSET_START,
    OFFSET_LTIMING,
    OFFSET_FIFO_STAT,
    OFFSET_IO_CONFIG,
    OFFSET_HS,
    OFFSET_DEBUGSTAT,
    OFFSET_TRANSFER_LEN_AUX,
};
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define save_reg(x, y)  ((y << 24) | (i2c_readl(x, y) & 0xffff))

#define mt_reg_sync_writel(v, a)  *(volatile unsigned int *)(a) = (v)

static inline void i2c_writel(mt_i2c *i2c, uint8_t offset, uint16_t value) {
    int32_t ret;
    int32_t tries = 1;
    uint32_t reg = i2c->base + offset;
    uint32_t val;

    //ret = ispeed_single_write(reg, value, tries);
    ret = ispeed_sysreg_single_write(reg, value, tries);
    if (ret < 0)
        I2CERR("W(0x%x) = 0x%x err\n", reg, value);

    /* mt_reg_sync_writel(value, (i2c->base) + (offset)); */
}

static inline uint32_t i2c_readl(mt_i2c *i2c, uint8_t offset) {
    int32_t ret;
    int32_t tries = 1;
    uint32_t reg = i2c->base + offset;
    uint32_t val;

    //ret = ispeed_single_read(reg, &val, tries);
    ret = ispeed_sysreg_single_read(reg, &val, tries);
    if (ret < 0)
        I2CERR("R(0x%x) err, 0x%x\n", reg, val);

    return val;

    /*return DRV_Reg32((i2c->base) + (offset));*/
}

/* *********************************common API****************************************************** */
/*Set i2c port speed*/
static int32_t i2c_set_speed(mt_i2c *i2c) {
#ifdef SET_I2C_FIX_SPEED
    int ret = I2C_OK;
    uint8_t sample_cnt;
    uint8_t step_cnt;

    /* Set device speed,set it before set_control register */
    if (i2c->speed >= MAX_HS_MODE_SPEED) {
        i2c->mode = HS_MODE;
        i2c->pushpull = true;
        sample_cnt = 0;
        step_cnt = 1;
    } else if (i2c->speed >= MAX_FS_MODE_SPEED) {
        i2c->mode = FS_MODE;
        sample_cnt = 0;
        step_cnt = 0xf;
    } else if (i2c->speed >= MAX_ST_MODE_SPEED) {
        i2c->mode = ST_MODE;
        sample_cnt = 0;
        step_cnt = 0x3f;
    } else {
        I2CERR("speed err\n");
        ret = -1;
        return ret;
    }

#ifdef OBSOLETE_CODE
    if (i2c->mode == HS_MODE) {
        /* Set the hign speed mode register */
        i2c->timing_reg = 0x1303;
        i2c->ltiming_reg= (0x3 << 6) | (0x3 << 0) |
            (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 12 |
            (step_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 9;
        i2c->high_speed_reg = I2C_TIME_DEFAULT_VALUE |
            (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 12 |
            (step_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 8;
    } else
#endif
    {
        i2c->timing_reg =
            (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 8 |
            (step_cnt & I2C_TIMING_STEP_DIV_MASK) << 0;
         i2c->ltiming_reg =
             (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 6 |
             (step_cnt & I2C_TIMING_STEP_DIV_MASK) << 0;
        /* Disable the high speed transaction */
        i2c->high_speed_reg = I2C_TIME_CLR_VALUE;
    }
#else
    int32_t mode;
    uint32_t khz;
    uint32_t step_cnt;
    uint32_t sample_cnt;
    uint32_t sclk;
    uint32_t hclk;
    uint32_t max_step_cnt;
    uint32_t sample_div = MAX_SAMPLE_CNT_DIV;
    uint32_t step_div;
    uint32_t min_div;
    uint32_t best_mul;
    uint32_t cnt_mul;

    khz = i2c->speed;
    hclk = I2C_CLK_RATE;

    if (khz > MAX_HS_MODE_SPEED) {
        return -EINVAL_I2C;
    } else if (khz > MAX_FS_MODE_SPEED) {
        mode = HS_MODE;
        max_step_cnt = MAX_HS_STEP_CNT_DIV;
    } else {
        mode = FS_MODE;
        max_step_cnt = MAX_STEP_CNT_DIV;
    }
    step_div = max_step_cnt;

    /* Find the best combination */
    min_div = ((hclk >> 1) + khz - 1) / khz;
    best_mul = MAX_SAMPLE_CNT_DIV * max_step_cnt;
    for (sample_cnt = 1; sample_cnt <= MAX_SAMPLE_CNT_DIV; sample_cnt++) {
        step_cnt = (min_div + sample_cnt - 1) / sample_cnt;
        cnt_mul = step_cnt * sample_cnt;
        if (step_cnt > max_step_cnt)
            continue;
        if (cnt_mul < best_mul) {
            best_mul = cnt_mul;
            sample_div = sample_cnt;
            step_div = step_cnt;
            if (best_mul == min_div)
                break;
        }
    }
    sample_cnt = sample_div;
    step_cnt = step_div;
    sclk = hclk / (2 * sample_cnt * step_cnt);
    if (sclk > khz) {
        I2CERR("%d:err speed(%ldkhz)\n",
            mode , (long int)khz);
        return -ENOTSUPP_I2C;
    }

    step_cnt--;
    sample_cnt--;

    if (mode == HS_MODE) {
        /* Set the hign speed mode register */
        i2c->timing_reg = 0x1303;
        i2c->ltiming_reg= (0x3 << 6) | (0x3 << 0) |
            (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 12 |
            (step_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 9;
        i2c->high_speed_reg = I2C_TIME_DEFAULT_VALUE |
            (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 12 |
            (step_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 8;
    } else {
        i2c->timing_reg =
            (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 8 |
            (step_cnt & I2C_TIMING_STEP_DIV_MASK) << 0;
         i2c->ltiming_reg =
             (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 6 |
             (step_cnt & I2C_TIMING_STEP_DIV_MASK) << 0;
        /* Disable the high speed transaction */
        i2c->high_speed_reg = I2C_TIME_CLR_VALUE;
    }
#endif
    return 0;
}

static void _i2c_dump_info(mt_i2c *i2c) {
#ifdef I2C_DEBUG
    I2CERR("I2C:\n"
           I2CTAG "Clk=%d,Id=%d,Mode=%x,St_rs=%x,Dma_en=%x,Op=%x,Poll_en=%x,Irq_stat=%x\n"
           I2CTAG "Trans len=%x,num=%x,auxlen=%x,Data_size=%x,speed=%d\n",
           i2c->clk, i2c->id, i2c->mode, i2c->st_rs, i2c->dma_en, i2c->op, i2c->poll_en,
           i2c->irq_stat, i2c->trans_data.trans_len, i2c->trans_data.trans_num,
           i2c->trans_data.trans_auxlen, i2c->trans_data.data_size, i2c->speed);
    I2CERR("base %x\n", i2c->base);
#endif /*endif #ifdef I2C_DEBUG*/
#ifdef ADV_I2C_DUMP
    I2CERR("I2C reg:\n"
           I2CTAG "SLAVE_ADDR=%x,INTR_MASK=%x,INTR_STAT=%x,CONTROL=%x,TRANS_LEN=%x\n"
           I2CTAG "TRANSAC_LEN=%x,DELAY_LEN=%x,TIMING=%x,LTIMING=%x,START=%x,FIFO_STAT=%x\n"
           I2CTAG "HS=%x,DCM_EN=%x,TRANS_LEN_AUX=%x,DEBUGSTAT=%x\n",
           (i2c_readl(i2c, OFFSET_SLAVE_ADDR)), (i2c_readl(i2c, OFFSET_INTR_MASK)),
           (i2c_readl(i2c, OFFSET_INTR_STAT)), (i2c_readl(i2c, OFFSET_CONTROL)),
           (i2c_readl(i2c, OFFSET_TRANSFER_LEN)), (i2c_readl(i2c, OFFSET_TRANSAC_LEN)),
           (i2c_readl(i2c, OFFSET_DELAY_LEN)), (i2c_readl(i2c, OFFSET_TIMING)),
           (i2c_readl(i2c, OFFSET_LTIMING)),
           (i2c_readl(i2c, OFFSET_START)), (i2c_readl(i2c, OFFSET_FIFO_STAT)),
           (i2c_readl(i2c, OFFSET_HS)),
           (i2c_readl(i2c, OFFSET_DCM_EN)),
           (i2c_readl(i2c, OFFSET_TRANSFER_LEN_AUX)),
           (i2c_readl(i2c, OFFSET_DEBUGSTAT)));
#else

#ifdef I2C_DUMP_PRINT
    I2CERR(I2CTAG "[%x:%x][%x:%x][%x:%x][%x:%x][%x:%x][%x:%x]\n"
           I2CTAG "[%x:%x][%x:%x][%x:%x][%x:%x][%x:%x][%x:%x]\n"
           I2CTAG "[%x:%x][%x:%x][%x:%x][%x:%x]\n",
           OFFSET_SLAVE_ADDR, (i2c_readl(i2c, OFFSET_SLAVE_ADDR)),
           OFFSET_INTR_MASK, (i2c_readl(i2c, OFFSET_INTR_MASK)),
           OFFSET_INTR_STAT, (i2c_readl(i2c, OFFSET_INTR_STAT)),
           OFFSET_CONTROL, (i2c_readl(i2c, OFFSET_CONTROL)),
           OFFSET_TRANSFER_LEN, (i2c_readl(i2c, OFFSET_TRANSFER_LEN)),
           OFFSET_TRANSAC_LEN, (i2c_readl(i2c, OFFSET_TRANSAC_LEN)),
           OFFSET_DELAY_LEN, (i2c_readl(i2c, OFFSET_DELAY_LEN)),
           OFFSET_TIMING, (i2c_readl(i2c, OFFSET_TIMING)),
           OFFSET_START, (i2c_readl(i2c, OFFSET_START)),
           OFFSET_LTIMING, (i2c_readl(i2c, OFFSET_LTIMING)),
           OFFSET_FIFO_STAT, (i2c_readl(i2c, OFFSET_FIFO_STAT)),
           OFFSET_IO_CONFIG, (i2c_readl(i2c, OFFSET_IO_CONFIG)),
           OFFSET_HS, (i2c_readl(i2c, OFFSET_HS)),
           OFFSET_DCM_EN, (i2c_readl(i2c, OFFSET_DCM_EN)),
           OFFSET_DEBUGSTAT, (i2c_readl(i2c, OFFSET_DEBUGSTAT)),
           OFFSET_TRANSFER_LEN_AUX, (i2c_readl(i2c, OFFSET_TRANSFER_LEN_AUX)));

#else
    uint32_t i;
    for (i = 0; (i < REG_BAK_CNT) && (i < NELEMS(i2c_backup_reg_off)); i++) {
        i2c_reg_bak[i+1] = save_reg(i2c, i2c_backup_reg_off[i]);
    }
#endif
#endif
    return;
}

static int32_t _i2c_deal_result(mt_i2c *i2c) {
    int32_t ret = i2c->msg_len;
    uint32_t retry;
    uint16_t data_size = 0;
    uint8_t *ptr = i2c->msg_buf;
    long timeout_poll = INT_TMO_CNT;

    I2CFUC();

    if (i2c->poll_en) { /* Poll mode */
        while (timeout_poll > 0) {
            i2c->irq_stat = i2c_readl(i2c, OFFSET_INTR_STAT);
            if (i2c->irq_stat) {
                I2CDBG("i2c->irq_stat=0x%x\n", i2c->irq_stat);
                /*Clear interrupt status */
                i2c_writel(i2c, OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
                break;
            }
            timeout_poll--;
        }
    } else { /* Interrupt mode, wait for ISR to wake up */
        retry = INT_SEMAPHORE_TMO_CNT;
        do {
            retry--;
            timeout_poll = xSemaphoreTake(i2c_irq_semaphore[i2c->id], 0);
        } while ((timeout_poll != pdTRUE) && retry);
        if (timeout_poll == pdTRUE) {
            i2c->irq_stat = irq_stat[i2c->id];
            irq_stat[i2c->id] = 0; /* clear the irq_stat */
        }
    }

    /*Check the transfer status */
    if (timeout_poll != 0) {
        /*Transfer success ,we need to get data from fifo,
            only read mode or write_read mode need to get data */
        if (
#ifdef SSPM_I2C_DMA_SUPPORT
            (!i2c->dma_en) &&
#endif
            (i2c->op == I2C_MASTER_RD || i2c->op == I2C_MASTER_WRRD)) {
            data_size = (i2c_readl(i2c, OFFSET_FIFO_STAT) >> 4) & 0x000F;
#ifdef I2C_TEST
            I2CDBG("data_size = %d\n", data_size);
            if (data_size == 0)
                _i2c_dump_info(i2c);
#endif
            while (data_size--) {
                *ptr = i2c_readl(i2c, OFFSET_DATA_PORT);
#ifdef I2C_TEST
                I2CDBG("addr %x R = 0x%x\n", i2c->addr, *ptr);
#endif
                ptr++;
            }
        }
        if (i2c->irq_stat & I2C_ACKERR)
            I2CERR("ACKERR\n");
    } else {
        /*Timeout*/
        I2CERR("addr%x,trans tmo\n", i2c->addr);
        _i2c_dump_info(i2c);
        ret = -ETIMEDOUT_I2C;

        /*Reset i2c port */
        i2c_writel(i2c, OFFSET_SOFTRESET, 0x0001);
        /*Set slave address */
        i2c_writel(i2c, OFFSET_SLAVE_ADDR, 0x0000);
        /*Clear interrupt status */
        i2c_writel(i2c, OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
        /*Clear fifo address */
        i2c_writel(i2c, OFFSET_FIFO_ADDR_CLR, 0x0001);
    }

    return ret;
}

static void _i2c_write_reg(mt_i2c *i2c) {
    uint8_t *ptr = i2c->msg_buf;
    uint32_t data_size = i2c->trans_data.data_size;
    uint32_t addr_reg = 0;

    I2CFUC();

    i2c_writel(i2c, OFFSET_CONTROL, i2c->control_reg);

    /* set timing reg */
    i2c_writel(i2c, OFFSET_TIMING, i2c->timing_reg);
    i2c_writel(i2c, OFFSET_LTIMING, i2c->ltiming_reg);
    i2c_writel(i2c, OFFSET_HS, i2c->high_speed_reg);

    if (0 == i2c->delay_len)
        i2c->delay_len = 2;

    if (~i2c->control_reg & I2C_CONTROL_RS) {   /* bit is set to 1, i.e.,use repeated stop */
        i2c_writel(i2c, OFFSET_DELAY_LEN, i2c->delay_len);
    }

    /*Set slave address */
    addr_reg = i2c->addr << 1;
    if (i2c->read_flag)
        addr_reg |= 0x1;

    i2c_writel(i2c, OFFSET_SLAVE_ADDR, addr_reg);
    /*Clear interrupt status */
    i2c_writel(i2c, OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    /*Clear fifo address */
    i2c_writel(i2c, OFFSET_FIFO_ADDR_CLR, 0x0001);

    /*Set io config*/
    if (i2c->pushpull)
        i2c_writel(i2c, OFFSET_IO_CONFIG, 0x0);
    else
        i2c_writel(i2c, OFFSET_IO_CONFIG, 0x1a3);

    /*Setup the interrupt mask flag */
    if (i2c->poll_en)
        i2c_writel(i2c, OFFSET_INTR_MASK, ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP)); /*Disable interrupt */
    else
        i2c_writel(i2c, OFFSET_INTR_MASK, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP)); /*Enable interrupt */

    /*Set transfer len */
#ifdef I2C_DUAL
    i2c_writel(i2c, OFFSET_TRANSFER_LEN, (i2c->trans_data.trans_len & 0xFF) |
                  ((i2c->trans_data.trans_auxlen<<8) & 0x1F00));
#else
    i2c_writel(i2c, OFFSET_TRANSFER_LEN, i2c->trans_data.trans_len); /*write*/
    i2c_writel(i2c, OFFSET_TRANSFER_LEN_AUX, i2c->trans_data.trans_auxlen); /*read*/
#endif

    /*Set transaction len */
    i2c_writel(i2c, OFFSET_TRANSAC_LEN, i2c->trans_data.trans_num & 0xFF);

    /*Prepare buffer data to start transfer */
#ifdef SSPM_I2C_DMA_SUPPORT
    if (i2c->dma_en) {
    } else
#endif
    {
        /*Set fifo mode data */
        if (I2C_MASTER_RD != i2c->op) { /*both write && write_read mode */
#ifdef I2C_TEST
            I2CDBG("data_size = %d\n", data_size);
#endif
            while (data_size--) {
                i2c_writel(i2c, OFFSET_DATA_PORT, *ptr);
#ifdef I2C_TEST
                I2CDBG("addr %x W = 0x%x\n", i2c->addr, *ptr);
#endif
                ptr++;
            }
        }
    }
}

static int32_t _i2c_get_transfer_len(mt_i2c *i2c) {
    int32_t ret = I2C_OK;
    uint16_t trans_num = 0;
    uint16_t data_size = 0;
    uint16_t trans_len = 0;
    uint16_t trans_auxlen = 0;

    I2CFUC();
    /*Get Transfer len and transaux len */
#ifdef SSPM_I2C_DMA_SUPPORT
    if (i2c->dma_en) {
    } else
#endif
    { /*non-DMA mode */
        if (I2C_MASTER_WRRD != i2c->op) {
            trans_len = (i2c->msg_len) & 0xFFFF; /*write or read*/
            trans_num = (i2c->msg_len >> 16) & 0xFF;
            if (0 == trans_num)
                trans_num = 1;
            trans_auxlen = 0;
            data_size = trans_len * trans_num;

            if (!trans_len || !trans_num || trans_len * trans_num > I2C_FIFO_SIZE) {
                I2CERR
                ("nWRRD err.len=%x,num=%x,aux=%x\n",
                 trans_len, trans_num, trans_auxlen);
                I2C_BUG_ON(!trans_len || !trans_num
                           || trans_len * trans_num > I2C_FIFO_SIZE);
                ret = -EINVAL_I2C;
            }
        } else {
            trans_len = (i2c->msg_len) & 0xFFFF; /*write*/
            trans_auxlen = (i2c->msg_len >> 16) & 0xFFFF; /*read*/
            trans_num = 2;
            data_size = trans_len;
            if (!trans_len || !trans_auxlen || trans_len > I2C_FIFO_SIZE
                    || trans_auxlen > I2C_FIFO_SIZE) {
                I2CERR
                ("WRRD err.len=%x,num=%x,aux=%x\n",
                 trans_len, trans_num, trans_auxlen);
                I2C_BUG_ON(!trans_len || !trans_auxlen || trans_len > I2C_FIFO_SIZE
                           || trans_auxlen > I2C_FIFO_SIZE);
                ret = -EINVAL_I2C;
            }
        }
    }

    i2c->trans_data.data_size = data_size;
    i2c->trans_data.trans_num = trans_num;
    i2c->trans_data.trans_len = trans_len;
    i2c->trans_data.trans_auxlen = trans_auxlen;

    return ret;
}

static int32_t _i2c_transfer_interface(mt_i2c *i2c) {
    int32_t retv = 0;
    int32_t ret = I2C_OK;
    uint8_t *ptr = i2c->msg_buf;

    I2CFUC();

    i2c_clock_enable();
#ifdef SSPM_I2C_DMA_SUPPORT
    if (i2c->dma_en) {
        I2CLOG("DMA Transfer mode\n");
        if (i2c->pdmabase == 0) {
            I2CERR("%d no DMA mode\n", i2c->id);
            I2C_BUG_ON(i2c->pdmabase == NULL);
            ret = -EINVAL_I2C;
            goto err;
        }
        if ((uint32_t) ptr > DMA_ADDRESS_HIGH) {
            /*DMA mode should use physical buffer address*/
            I2CERR("DMA mode err\n");
            I2C_BUG_ON((uint32_t) ptr > DMA_ADDRESS_HIGH);
            ret = -EINVAL_I2C;
            goto err;
        }
    }
#endif
    i2c->irq_stat = 0;

    retv = _i2c_get_transfer_len(i2c);
    if (retv < 0) {
        I2CERR("get trans len,%d\n", retv);
        ret = -EINVAL_I2C;
        goto err;
    }

    retv = i2c_set_speed(i2c);
    if (retv < 0) {
        I2CERR("set speed,%d\n", retv);
        ret = -EINVAL_I2C;
        goto err;
    }

    /*Set Control Register */
#ifdef CONFIG_MT_I2C_FPGA_ENABLE
    i2c->control_reg = I2C_CONTROL_ACKERR_DET_EN;
#else
    i2c->control_reg = I2C_CONTROL_ACKERR_DET_EN | I2C_CONTROL_CLK_EXT_EN | I2C_CONTROL_DMA_ACK_EN;
#endif

#ifdef SSPM_I2C_DMA_SUPPORT
    if (i2c->dma_en)
        i2c->control_reg |= I2C_CONTROL_DMA_EN | I2C_CONTROL_DMA_ACK_EN | I2C_CONTROL_DMA_ASYNC_MODE;
#endif

    if (I2C_MASTER_WRRD == i2c->op)
        i2c->control_reg |= I2C_CONTROL_DIR_CHANGE | I2C_CONTROL_RS;

#ifdef OBSOLETE_CODE
    if (HS_MODE == i2c->mode
            || (i2c->trans_data.trans_num > 1 && I2C_TRANS_REPEATED_START == i2c->st_rs)) {
        i2c->control_reg |= I2C_CONTROL_RS;
    }
#endif

    _i2c_write_reg(i2c);

    /*All register must be prepared before setting the start bit [SMP] */
    /*I2C_MB();*/

    I2CDBG("Before start .....\n");
    /*Start the transfer */
    i2c_writel(i2c, OFFSET_START, 0x0001);
    ret = _i2c_deal_result(i2c);
    I2CDBG("After i2c transfer .....\n");
err:
    i2c_clock_disable();
    return ret;
}

static int32_t _i2c_check_para(mt_i2c *i2c) {
    int32_t ret = I2C_OK;

    I2CFUC();
     /*only 1 channel, force id 0*/
    i2c->id = 0;

    if (i2c->addr == NULL) {
        I2CERR("addr err\n");
        I2C_BUG_ON(i2c->addr == NULL);
        ret = -EINVAL_I2C;
        goto err;
    }

    if (i2c->msg_buf == NULL) {
        I2CERR("buf err\n");
        I2C_BUG_ON(i2c->msg_buf == NULL);
        ret = -EINVAL_I2C;
        goto err;
    }

err:
    return ret;
}

static void _config_mt_i2c(mt_i2c *i2c) {
    i2c->id = 0;
    //i2c->base = SSPM_I2C6_BASE;
    i2c->base = AP_I2C_BASE;

    if (i2c->st_rs != I2C_TRANS_REPEATED_START)
        i2c->st_rs = I2C_TRANS_STOP;

#ifdef SSPM_I2C_DMA_SUPPORT
    i2c->dma_en = false;
#endif
    /* Make sure interrupt is enabled, when poll_en false */
    i2c->poll_en = true;
    i2c->filter_msg = false;
}

/*-----------------------------------------------------------------------
 * new read interface: Read bytes
 *   mt_i2c:    I2C chip config, see mt_i2c_t.
 *   buffer:  Where to read/write the data.
 *   len:     How many bytes to read/write
 *   Returns: ERROR_CODE
 */
int32_t i2c_read(mt_i2c *i2c, uint8_t *buffer, uint32_t len) {
    int32_t ret = I2C_OK;
    uint32_t retry;

    I2CFUC();

    /* read */
    i2c->read_flag |= I2C_M_RD;
    i2c->op = I2C_MASTER_RD;
    i2c->msg_buf = buffer;
    i2c->msg_len = len;

    ret = _i2c_check_para(i2c);
    if (ret < 0) {
        I2CERR("chk para err\n");
        goto err;
    }

    retry = SEMAPHORE_TMO_CNT;
    /* Use mutex for I2C controller protection */
    do {
        retry--;
    } while ((xSemaphoreTake(i2c_mutex[i2c->id], 0) != pdTRUE) && retry);

    if (retry == 0) {
        I2CERR("get mutex err\n");
        ret = -1;
        goto mutexError;
    }

    _config_mt_i2c(i2c);
    /* get the addr */
    ret = _i2c_transfer_interface(i2c);

    if ((int)i2c->msg_len != ret) {
        I2CERR("R %dB fail,%d\n", i2c->msg_len, ret);
        ret = -1;
    } else {
        I2CDBG("R %dB pass,%d\n", i2c->msg_len, ret);
        ret = I2C_OK;
    }

    xSemaphoreGive(i2c_mutex[i2c->id]);

mutexError:
err:
    return ret;
}

/*-----------------------------------------------------------------------
 * New write interface: Write bytes
 *   i2c:    I2C chip config, see mt_i2c_t.
 *   buffer:  Where to read/write the data.
 *   len:     How many bytes to read/write
 *   Returns: ERROR_CODE
 */
int32_t i2c_write(mt_i2c *i2c, uint8_t *buffer, uint32_t len) {
    int32_t ret = I2C_OK;
    uint32_t retry;

    I2CFUC();

    /* write */
    i2c->read_flag = !I2C_M_RD;
    i2c->op = I2C_MASTER_WR;
    i2c->msg_buf = buffer;
    i2c->msg_len = len;

    ret = _i2c_check_para(i2c);
    if (ret < 0) {
        I2CERR("chk para err\n");
        goto err;
    }

    retry = SEMAPHORE_TMO_CNT;
    /* Use mutex for I2C controller protection */
    do {
        retry--;
    } while ((xSemaphoreTake(i2c_mutex[i2c->id], 0) != pdTRUE) && retry);

    if (retry == 0) {
        I2CERR("get mutex err\n");
        ret = -1;
        goto mutexError;
    }

    _config_mt_i2c(i2c);
    /* get the addr */
    ret = _i2c_transfer_interface(i2c);

    if ((int)i2c->msg_len != ret) {
        I2CERR("W %dB fail,%d\n", i2c->msg_len, ret);
        ret = -1;
    } else {
        I2CDBG("W %dB pass,%d\n", i2c->msg_len, ret);
        ret = I2C_OK;
    }

    xSemaphoreGive(i2c_mutex[i2c->id]);

mutexError:
err:
    return ret;
}

/*-----------------------------------------------------------------------
 * New write then read back interface: Write bytes then read bytes
 *   i2c:    I2C chip config, see mt_i2c_t.
 *   buffer:  Where to read/write the data.
 *   write_len:     How many bytes to write
 *   read_len:     How many bytes to read
 *   Returns: ERROR_CODE
 */
int32_t i2c_write_read(mt_i2c *i2c, uint8_t *buffer, uint32_t write_len, uint32_t read_len) {
    int32_t ret = I2C_OK;
    uint32_t retry;

    I2CFUC();

    /* write and read */
    i2c->op = I2C_MASTER_WRRD;
    i2c->read_flag = !I2C_M_RD;
    i2c->msg_buf = buffer;
    i2c->msg_len = ((read_len & 0xFFFF) << 16) | (write_len & 0xFFFF);

    ret = _i2c_check_para(i2c);
    if (ret < 0) {
        I2CERR("chk para err\n");
        goto err;
    }

    retry = SEMAPHORE_TMO_CNT;
    /* Use mutex for I2C controller protection */
    do {
        retry--;
    } while ((xSemaphoreTake(i2c_mutex[i2c->id], 0) != pdTRUE) && retry);

    if (retry == 0) {
        I2CERR("get mutex err\n");
        ret = -1;
        goto mutexError;
    }

    _config_mt_i2c(i2c);
    /* get the addr */
    ret = _i2c_transfer_interface(i2c);

    if ((int)i2c->msg_len != ret) {
        I2CERR("WR %dB fail,%d\n", i2c->msg_len, ret);
        ret = -1;
    } else {
        I2CDBG("WR %dB pass,%d\n", i2c->msg_len, ret);
        ret = I2C_OK;
    }

    xSemaphoreGive(i2c_mutex[i2c->id]);
mutexError:
err:
    return ret;
}

/* Test I2C */
#ifdef I2C_TEST
#define SLAVE_ADDRESS 0x50
#define DATA_ADDRESS 0x00
int32_t mt_i2c_sspm_test(void) {
    int32_t ret;
    uint32_t wr_num, rd_num;
    uint8_t buf[8] = {0}, obuf[8] = {0};
    struct mt_i2c_t i2c = {0};
    uint32_t val, i, j;
    volatile uint32_t k;

    i2c.id = 0;
    i2c.addr = SLAVE_ADDRESS;
    i2c.mode = FS_MODE;
    i2c.speed = 100;
    buf[0] = DATA_ADDRESS;
    wr_num = 1;
    rd_num = 5;

    I2CLOG("test i2c_write_read\n");

    ret = i2c_write_read(&i2c, buf, wr_num, rd_num);
    if (ret != 0)
        I2CERR("mt_i2c_test: read 0x%x fail\n", DATA_ADDRESS);

    I2CLOG("test i2c_write/i2c_read\n");

    for (j = 0; j < 16; j++) {
        ret = 0;

        /*write*/
        for (i = 0; i < 8; i++) {
            buf[i] =  i * 0x10 + j;
            obuf[i] = buf[i];
        }

#ifdef I2C_TEST_INSERT_FAIL
        /*fail insert test*/
        if (j == 3 || j == 8) {
            I2CLOG("j = %d,skip write\n", j);
            goto skip;
        }
#endif

        buf[0] = DATA_ADDRESS + j * 0x10;
        ret = i2c_write(&i2c, buf, 7);
        if (ret < 0)
            I2CERR("i2c_write fail\n");
skip:

        k = 0xfffff; /*delay at least 5ms*/
        while (k--){
        };

        /*clear buffer*/
        for (i = 0; i < 8; i++) {
            buf[i] =  0x0;
        }

        /*read back*/
        buf[0] = DATA_ADDRESS + j * 0x10;
        ret = i2c_write(&i2c, buf, 1);
        if (ret < 0)
            I2CERR("i2c_write fail\n");

        ret = i2c_read(&i2c, buf + 1, 6);
        if (ret < 0)
            I2CERR("i2c_read fail\n");

        for (i = 1; i < 7; i++) {
            if (obuf[i] != buf[i]) {
                I2CERR("i2c data compare error: buf[%d] = 0x%x, should be 0x%x\n", i, buf[i], obuf[i]);
                ret = -1;
            }
        }
        if (ret)
            I2CLOG("i2c data compare (%d) fail\n", j);
        else
            I2CLOG("i2c data compare (%d) pass\n", j);
    }

    return 0;
}
#endif

int32_t i2c_hw_init(void) {
    int ret = I2C_OK;
    int i = 0;

    for (i = 0; i < I2C_NR; i++) {
        i2c_mutex[i] = xSemaphoreCreateMutex();
        i2c_irq_semaphore[i] = xSemaphoreCreateBinary();
    }

    /*dump reg magic number*/
    i2c_reg_bak[0] = DUMP_REG_BAK_MAGIC;

    I2CLOG("i2c init\n");

#ifdef I2C_TEST
    I2CLOG("test start\n");
    /* Do not test interrupt mode, before vTaskStartScheduler */
    for (i = 0; i < 20; i++) {
        I2CLOG("== test %d times ==\n", i);
        mt_i2c_sspm_test();
    }
    I2CLOG("test end\n");
#endif
    return ret;
}
