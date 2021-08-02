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
#include "irq.h"


SemaphoreHandle_t i2c_mutex[I2C_NR];/* For semaphore handling */
SemaphoreHandle_t i2c_irq_semaphore[I2C_NR];/* For interrupt handling */
static uint32_t irq_stat[I2C_NR];
static uint32_t read_irq_status(struct mt_i2c_t *i2c, uint8_t addr);

#define mt_reg_sync_writel(v, a)  *(volatile unsigned int *)(a) = (v)
#define I2C_PMIC_WR(addr, data)   pwrap_write((uint32_t)addr, data)
#define I2C_PMIC_RD(addr)         ({ \
    uint32_t ext_data; \
    (pwrap_read((uint32_t)addr, &ext_data) != 0) ?  -1:ext_data; })

static inline void i2c_writel(mt_i2c *i2c, uint8_t offset, uint16_t value) {
    mt_reg_sync_writel(value, (i2c->base) + (offset));
}

static inline uint32_t i2c_readl(mt_i2c *i2c, uint8_t offset) {
    return DRV_Reg32((i2c->base) + (offset));
}

/* *********************************i2c debug****************************************************** */
#define I2C_DEBUG_FS
#ifdef I2C_DEBUG_FS
#define PORT_COUNT 7
#define MESSAGE_COUNT 16
#define I2C_T_DMA 1
#define I2C_T_TRANSFERFLOW 2
#define I2C_T_SPEED 3
/*7 ports,16 types of message */
uint8_t i2c_port[PORT_COUNT][MESSAGE_COUNT];

#define I2CINFO(type, format, arg...) do { \
    if (type < MESSAGE_COUNT && type >= 0) { \
        if (i2c_port[i2c->id][0] != 0 && (i2c_port[i2c->id][type] != 0 || \
                i2c_port[i2c->id][MESSAGE_COUNT - 1] != 0)) { \
            I2CLOG(format, ## arg); \
        } \
    } \
} while (0)

#else
#define I2CINFO(type, format, arg...)
#endif

/* *********************************common API****************************************************** */
/*Set i2c port speed*/
int32_t i2c_set_speed_fix(mt_i2c *i2c) {
    int ret = 0;
    uint32_t tmp;
    uint32_t sample_cnt;
    uint32_t step_cnt;

    if (i2c->speed == 100) {
        sample_cnt = 1;
        step_cnt = 13;
    } else if (i2c->speed == 400) {
        sample_cnt = 0;
        step_cnt = 6;
    } else if (i2c->speed == 3400) {
        sample_cnt = 0;
        step_cnt = 0;
    } else {
        I2CERR("Don't support HS speed in SPM\n");
        ret = -1;
        return ret;
    }

    tmp = i2c_readl(i2c, SPM_OFFSET_TIMING) & ~((0x7 << 8) | (0x3f << 0));
    tmp = (sample_cnt & 0x7) << 8 | (step_cnt & 0x3f) << 0 | tmp;
    i2c->timing_reg = tmp;

    return ret;
}


void _i2c_dump_info(mt_i2c *i2c) {
    int ret;
    uint32_t val;

    I2CERR("I2C structure:\n\r"
           I2CTAG "Clk=%ld,Id=%d,Mode=%x,St_rs=%x,Dma_en=%x,Op=%x,Poll_en=%x,Irq_stat=%x\n\r"
           I2CTAG "Trans_len=%x,Trans_num=%x,Trans_auxlen=%x,Data_size=%x,speed=%ld\n\r",
           /* ,Trans_stop=%u,Trans_comp=%u,Trans_error=%u\n" */
           i2c->clk, i2c->id, i2c->mode, i2c->st_rs, i2c->dma_en, i2c->op, i2c->poll_en,
           i2c->irq_stat, i2c->trans_data.trans_len, i2c->trans_data.trans_num,
           i2c->trans_data.trans_auxlen, i2c->trans_data.data_size, i2c->speed);
    /* atomic_read(&i2c->trans_stop),atomic_read(&i2c->trans_comp),atomic_read(&i2c->trans_err), */

    I2CERR("base address 0x%lx\n\r", i2c->base);
    I2CERR("I2C register:\n\r"
           I2CTAG "SLAVE_ADDR=%lx,INTR_MASK=%lx,INTR_STAT=%lx,CONTROL=%lx,TRANSFER_LEN=%lx\n\r"
           I2CTAG "TRANSAC_LEN=%lx,DELAY_LEN=%lx,TIMING=%lx,START=%lx,FIFO_STAT=%lx\n\r"
           I2CTAG
           "HS=%lx,DCM_EN=%lx,TRANSFER_LEN_AUX=%lx\n\r",
           (i2c_readl(i2c, SPM_OFFSET_SLAVE_ADDR)), (i2c_readl(i2c, SPM_OFFSET_INTR_MASK)),
           (i2c_readl(i2c, SPM_OFFSET_INTR_STAT)), (i2c_readl(i2c, SPM_OFFSET_CONTROL)),
           (i2c_readl(i2c, SPM_OFFSET_TRANSFER_LEN) & 0xff), (i2c_readl(i2c, SPM_OFFSET_TRANSAC_LEN)),
           (i2c_readl(i2c, SPM_OFFSET_DELAY_LEN)), (i2c_readl(i2c, SPM_OFFSET_TIMING)),
           (i2c_readl(i2c, SPM_OFFSET_START)), (i2c_readl(i2c, SPM_OFFSET_FIFO_STAT)),
           (i2c_readl(i2c, SPM_OFFSET_HS)),
           (i2c_readl(i2c, SPM_OFFSET_DCM_EN)),
           ((i2c_readl(i2c, SPM_OFFSET_TRANSFER_LEN) >> 8) & 0x1f));

    ret = ispeed_sysreg_single_read((AP_I2C6_BASE + OFFSET_DEBUGSTAT), &val, 10);
    if (ret < 0)
        I2CERR("ispeed_sysreg_single_read OFFSET_DEBUGSTAT fail\n");
    I2CERR("DEBUGSTAT=%lx\n", val);

    return;
}

static int32_t _i2c_deal_result(mt_i2c *i2c) {
    int32_t ret = i2c->msg_len;
    uint16_t data_size = 0;
    uint8_t *ptr = i2c->msg_buf;
    bool transfer_error = false;
    long timeout_poll = 0xffffff;

#ifdef I2C_EARLY_PORTING
    I2CFUC();
#endif
    if (i2c->poll_en) { /* Poll mode */
        while (timeout_poll > 0) {
            i2c->irq_stat = read_irq_status(i2c, OFFSET_INTR_STAT);
            if (i2c->irq_stat) {
                //I2CLOG("i2c->irq_stat=0x%x\n", i2c->irq_stat);
                /*Clear interrupt status */
                i2c_writel(i2c, SPM_OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
                break;
            }
            timeout_poll--;
        }
    } else { /* Interrupt mode, wait for ISR to wake up */
        timeout_poll = xSemaphoreTake(i2c_irq_semaphore[i2c->id], 3072);
        if (timeout_poll == pdTRUE) {
            i2c->irq_stat = irq_stat[i2c->id];
            irq_stat[i2c->id] = 0; /* clear the irq_stat */
        }
    }
    if (i2c->irq_stat & (SPM_I2C_ACKERR))
        transfer_error = true;

    /*Check the transfer status */
    if (timeout_poll != 0 && transfer_error == false) {
        /*Transfer success ,we need to get data from fifo,
            only read mode or write_read mode need to get data */
        if ((!i2c->dma_en) && (i2c->op == I2C_MASTER_RD || i2c->op == I2C_MASTER_WRRD)) {
            data_size = (i2c_readl(i2c, SPM_OFFSET_FIFO_STAT) >> 4) & 0x000F;
#ifdef I2C_TEST
            I2CLOG("data_size = %d\n", data_size);
            if (data_size == 0)
                _i2c_dump_info(i2c);
#endif
            while (data_size--) {
                *ptr = i2c_readl(i2c, SPM_OFFSET_DATA_PORT);
#ifdef I2C_TEST
                I2CLOG("addr %x read byte = 0x%x\n", i2c->addr, *ptr);
#endif
                ptr++;
            }
        }
    } else {
        /*Timeout or ACKERR */
        if (timeout_poll == 0) {
            I2CERR("id=%d,addr: %x, transfer timeout\n\r", i2c->id, i2c->addr);
            _i2c_dump_info(i2c);
            ret = -ETIMEDOUT_I2C;
        } else {
            I2CERR("id=%d,addr: %x, transfer error\n\r", i2c->id, i2c->addr);
            ret = -EREMOTEIO_I2C;
        }
        if (i2c->irq_stat & SPM_I2C_ACKERR)
            I2CERR("I2C_ACKERR\n\r");
        if (i2c->filter_msg == false) { /* TEST */
            _i2c_dump_info(i2c);
        }
        /* spin_lock(&i2c->lock); */
        /*Reset i2c port */
        i2c_writel(i2c, SPM_OFFSET_SOFTRESET, 0x0001);
        /*Set slave address */
        i2c_writel(i2c, SPM_OFFSET_SLAVE_ADDR, 0x0000);
        /*Clear interrupt status */
        i2c_writel(i2c, SPM_OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
        /*Clear fifo address */
        i2c_writel(i2c, SPM_OFFSET_FIFO_ADDR_CLR, 0x0001);

        /* spin_unlock(&i2c->lock); */
    }

    return ret;
}

void _i2c_write_reg(mt_i2c *i2c) {
    uint8_t *ptr = i2c->msg_buf;
    uint32_t data_size = i2c->trans_data.data_size;
    uint32_t addr_reg = 0;
#ifdef I2C_EARLY_PORTING
    I2CFUC();
#endif

    i2c_writel(i2c, SPM_OFFSET_CONTROL, i2c->control_reg);

    /* set timing reg */
    i2c_writel(i2c, SPM_OFFSET_TIMING, i2c->timing_reg);
    i2c_writel(i2c, SPM_OFFSET_HS, i2c->high_speed_reg);

    if (0 == i2c->delay_len)
        i2c->delay_len = 2;
    if (~i2c->control_reg & I2C_CONTROL_RS) {   /* bit is set to 1, i.e.,use repeated stop */
        i2c_writel(i2c, SPM_OFFSET_DELAY_LEN, i2c->delay_len);
    }

    /*Set slave address */
    addr_reg = i2c->read_flag ? ((i2c->addr << 1) | 0x1) : ((i2c->addr << 1) & ~0x1);
    i2c_writel(i2c, SPM_OFFSET_SLAVE_ADDR, addr_reg);
    /*Clear interrupt status */
    i2c_writel(i2c, SPM_OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    /*Clear fifo address */
    i2c_writel(i2c, SPM_OFFSET_FIFO_ADDR_CLR, 0x0001);

    /*Setup the interrupt mask flag */
    if (i2c->poll_en)
        i2c_writel(i2c, SPM_OFFSET_INTR_MASK, i2c_readl(i2c,
                   SPM_OFFSET_INTR_MASK) & ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP)); /*Disable interrupt */
    else
        i2c_writel(i2c, SPM_OFFSET_INTR_MASK, i2c_readl(i2c,
                   SPM_OFFSET_INTR_MASK) | (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP)); /*Enable interrupt */

    /*Set transfer len */
    i2c_writel(i2c, SPM_OFFSET_TRANSFER_LEN, ((i2c->trans_data.trans_auxlen << 8) |
                   i2c->trans_data.trans_len) & 0xFFFF);
    /*Set transaction len */
    i2c_writel(i2c, SPM_OFFSET_TRANSAC_LEN, i2c->trans_data.trans_num & 0xFF);

    /*Prepare buffer data to start transfer */
    if (i2c->dma_en) {
    } else {
        /*Set fifo mode data */
        if (I2C_MASTER_RD != i2c->op) { /*both write && write_read mode */
            while (data_size--) {
                i2c_writel(i2c, SPM_OFFSET_DATA_PORT, *ptr);
                ptr++;
            }
        }
    }
    /*Set trans_data */
    i2c->trans_data.data_size = data_size;
}

int32_t _i2c_get_transfer_len(mt_i2c *i2c) {
    int32_t ret = I2C_OK;
    uint16_t trans_num = 0;
    uint16_t data_size = 0;
    uint16_t trans_len = 0;
    uint16_t trans_auxlen = 0;
    /* I2CFUC(); */
    /*Get Transfer len and transaux len */
    if (false == i2c->dma_en) { /*non-DMA mode */
        if (I2C_MASTER_WRRD != i2c->op) {
            trans_len = (i2c->msg_len) & 0xFFFF; /*write or read*/
            trans_num = (i2c->msg_len >> 16) & 0xFF;
            if (0 == trans_num)
                trans_num = 1;
            trans_auxlen = 0;
            data_size = trans_len * trans_num;

            if (!trans_len || !trans_num || trans_len * trans_num > I2C_FIFO_SIZE) {
                I2CERR
                (" non-WRRD transfer length is not right. trans_len=%x, tans_num=%x, trans_auxlen=%x\n\r",
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
                (" WRRD transfer length is not right. trans_len=%x, tans_num=%x, trans_auxlen=%x\n\r",
                 trans_len, trans_num, trans_auxlen);
                I2C_BUG_ON(!trans_len || !trans_auxlen || trans_len > I2C_FIFO_SIZE
                           || trans_auxlen > I2C_FIFO_SIZE);
                ret = -EINVAL_I2C;
            }
        }
    }

    i2c->trans_data.trans_num = trans_num;
    i2c->trans_data.trans_len = trans_len;
    i2c->trans_data.data_size = data_size;
    i2c->trans_data.trans_auxlen = trans_auxlen;

    return ret;
}

int32_t _i2c_transfer_interface(mt_i2c *i2c) {
    int32_t return_value = 0;
    int32_t ret = 0;
    uint8_t *ptr = i2c->msg_buf;

    /* I2CFUC(); */
    i2c_clock_enable();
    if (i2c->dma_en) {
        I2CINFO(I2C_T_DMA, "DMA Transfer mode!\n");
        if (i2c->pdmabase == 0) {
            I2CERR(" I2C%d doesnot support DMA mode!\n\r", i2c->id);
            I2C_BUG_ON(i2c->pdmabase == NULL);
            ret = -EINVAL_I2C;
            goto err;
        }
        if ((uint32_t) ptr > DMA_ADDRESS_HIGH) {
            I2CERR(" DMA mode should use physical buffer address!\n\r");
            I2C_BUG_ON((uint32_t) ptr > DMA_ADDRESS_HIGH);
            ret = -EINVAL_I2C;
            goto err;
        }
    }
    i2c->irq_stat = 0;

    return_value = _i2c_get_transfer_len(i2c);
    if (return_value < 0) {
        I2CERR("_i2c_get_transfer_len fail,return_value=%ld\n\r", return_value);
        ret = -EINVAL_I2C;
        goto err;
    }

    return_value = i2c_set_speed_fix(i2c);
    if (return_value < 0) {
        I2CERR("i2c_set_speed_fix fail,return_value=%ld\n\r", return_value);
        ret = -EINVAL_I2C;
        goto err;
    }

    /*Set Control Register */
#ifdef CONFIG_MT_I2C_FPGA_ENABLE
    i2c->control_reg = I2C_CONTROL_ACKERR_DET_EN;
#else
    i2c->control_reg = I2C_CONTROL_ACKERR_DET_EN | I2C_CONTROL_CLK_EXT_EN;
#endif
    if (i2c->dma_en)
        i2c->control_reg |= I2C_CONTROL_DMA_EN;

    if (I2C_MASTER_WRRD == i2c->op)
        i2c->control_reg |= I2C_CONTROL_DIR_CHANGE;

    if (HS_MODE == i2c->mode
            || (i2c->trans_data.trans_num > 1 && I2C_TRANS_REPEATED_START == i2c->st_rs)) {
        i2c->control_reg |= I2C_CONTROL_RS;
    }

    _i2c_write_reg(i2c);

    /*All register must be prepared before setting the start bit [SMP] */
    /*I2C_MB();*/
    /* I2CLOG(I2C_T_TRANSFERFLOW, "Before start .....\n"); */
    /*Start the transfer */
    i2c_writel(i2c, SPM_OFFSET_START, 0x0001);
    /* spin_unlock(&i2c->lock); */
    ret = _i2c_deal_result(i2c);
    /* I2CINFO(I2C_T_TRANSFERFLOW, "After i2c transfer .....\n");*/
err:
    i2c_clock_disable();
    return ret;
}

int32_t _i2c_check_para(mt_i2c *i2c) {
    int32_t ret = 0;

    /* I2CFUC(); */
    if (i2c->addr == 0) {
        I2CERR(" addr is invalid.\n\r");
        I2C_BUG_ON(i2c->addr == NULL);
        ret = -EINVAL_I2C;
        goto err;
    }

    if (i2c->msg_buf == NULL) {
        I2CERR(" data buffer is NULL.\n\r");
        I2C_BUG_ON(i2c->msg_buf == NULL);
        ret = -EINVAL_I2C;
        goto err;
    }

    if (i2c->id >= I2C_NR) {
        I2CERR("invalid para: i2c->id=%d\n\r", i2c->id);
        ret = -EINVAL_I2C;
    }

err:
    return ret;
}

void _config_mt_i2c(mt_i2c *i2c) {

    i2c->id = 0;
    i2c->base = SSPM_I2C6_BASE;

    if (i2c->st_rs == I2C_TRANS_REPEATED_START)
        i2c->st_rs = I2C_TRANS_REPEATED_START;
    else
        i2c->st_rs = I2C_TRANS_STOP;

    i2c->dma_en = false;
    /* Make sure interrupt is enabled, when poll_en false */
    i2c->poll_en = true;

    i2c->filter_msg = false;

    /* Set device speed,set it before set_control register */
    if (100 == i2c->speed) {
        i2c->mode = ST_MODE;
    } else if (400 == i2c->speed) {
        i2c->mode = FS_MODE;
    } else if (3400 == i2c->speed) {
        i2c->mode = HS_MODE;
        i2c->pushpull = true;
    }

    /*Set ioconfig */
    if (i2c->pushpull == true)
        i2c->pushpull = true;
    else
        i2c->pushpull = false;
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
#ifdef I2C_EARLY_PORTING
    I2CFUC();
#endif
    /* read */
    i2c->read_flag |= I2C_M_RD;
    i2c->op = I2C_MASTER_RD;
    i2c->msg_buf = buffer;
    i2c->msg_len = len;

    ret = _i2c_check_para(i2c);
    if (ret < 0) {
        I2CERR(" _i2c_check_para fail\n\r");
        goto err;
    }

    /* Use mutex for I2C controller protection */
    if (xSemaphoreTake(i2c_mutex[i2c->id], portMAX_DELAY) != pdTRUE) {
        I2CERR("get mutex fail\n\r");
        goto mutexError;
    }

    _config_mt_i2c(i2c);
    /* get the addr */
    ret = _i2c_transfer_interface(i2c);
    if ((int)i2c->msg_len != ret) {
        I2CERR("read %ld bytes fails,ret=%ld.\n\r", i2c->msg_len, ret);
        ret = -1;
    } else {
        ret = I2C_OK;
        /* I2CLOG("read %d bytes pass,ret=%d.\n",i2c->msg_len,ret); */
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
#ifdef I2C_EARLY_PORTING
    I2CFUC();
#endif
    /* write */
    i2c->read_flag = !I2C_M_RD;
    i2c->op = I2C_MASTER_WR;
    i2c->msg_buf = buffer;
    i2c->msg_len = len;

    ret = _i2c_check_para(i2c);
    if (ret < 0) {
        I2CERR(" _i2c_check_para fail\n\r");
        goto err;
    }

    /* Use mutex for I2C controller protection */
    if (xSemaphoreTake(i2c_mutex[i2c->id], portMAX_DELAY) != pdTRUE) {
        I2CERR("get mutex fail\n\r");
        goto mutexError;
    }

    _config_mt_i2c(i2c);
    /* get the addr */
    ret = _i2c_transfer_interface(i2c);

    if ((int)i2c->msg_len != ret) {
        I2CERR("Write %ld bytes fails,ret=%ld.\n\r", i2c->msg_len, ret);
        ret = -1;
    } else {
        ret = I2C_OK;
        /* I2CLOG("Write %d bytes pass,ret=%d.\n",i2c->msg_len,ret); */
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
    /* I2CFUC(); */
    /* write and read */
    i2c->op = I2C_MASTER_WRRD;
    i2c->read_flag = !I2C_M_RD;
    i2c->msg_buf = buffer;
    i2c->msg_len = ((read_len & 0xFFFF) << 16) | (write_len & 0xFFFF);

    ret = _i2c_check_para(i2c);
    if (ret < 0) {
        I2CERR(" _i2c_check_para fail\n\r");
        goto err;
    }

    /* Use mutex for I2C controller protection */
    if (xSemaphoreTake(i2c_mutex[i2c->id], portMAX_DELAY) != pdTRUE) {
        I2CERR("get mutex fail\n\r");
        goto mutexError;
    }
    _config_mt_i2c(i2c);
    /* get the addr */
    ret = _i2c_transfer_interface(i2c);
    if ((int)i2c->msg_len != ret) {
        I2CERR("write_read 0x%lx bytes fails,ret=%ld.\n\r", i2c->msg_len, ret);
        ret = -1;
    } else {
        ret = I2C_OK;
        /* I2CLOG("write_read 0x%x bytes pass,ret=%d.\n",i2c->msg_len,ret); */
    }

    xSemaphoreGive(i2c_mutex[i2c->id]);
mutexError:
err:
    return ret;
}

static uint32_t read_irq_status(struct mt_i2c_t *i2c, uint8_t addr) {
    /* For IIC hw reason, we can't directly read INTR_STAT*/
    int retries = 10;
    int ret;
    uint32_t val;

    ret = ispeed_sysreg_single_read((AP_I2C6_BASE + addr), &val, retries);
    if (ret < 0)
        I2CERR("read_irq_status OFFSET_INTR_STAT bit[4]:CMP,bit[5]:ACKERR fail\n");

    return val;
}

static unsigned int i2c0_irq_handler(void *data) {
    struct mt_i2c_t i2c_t = {0};
    struct mt_i2c_t *i2c = &i2c_t;
    uint32_t val;
    int ret = 0;
    portBASE_TYPE taskWoken = pdFALSE;

    I2CLOG_ISR("I2C0, interrupt is coming!!\n");

    i2c_t.id = 0;
    i2c_t.base = SSPM_I2C6_BASE;
    i2c_writel(i2c, SPM_OFFSET_INTR_MASK, i2c_readl(i2c, SPM_OFFSET_INTR_MASK) &
                  ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));

    ret = ispeed_sysreg_single_read_from_ISR((AP_I2C6_BASE + OFFSET_INTR_STAT), &val, 10);
    if (ret < 0)
        I2CERR_ISR("read_irq_status fail\n");

    I2CLOG_ISR("intr 0x%x\n", val);

    irq_stat[0] = val;

    i2c_writel(i2c, SPM_OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    xSemaphoreGiveFromISR(i2c_irq_semaphore[i2c->id], &taskWoken);

    return 0;
}

/* Test I2C */
#ifdef I2C_TEST
#define SLAVE_ADDRESS 0x50
#define DATA_ADDRESS 0x00
int32_t mt_i2c_spm_test(void) {
    int ret;
    uint32_t wr_num, rd_num;
    uint8_t buf[8] = {0}, obuf[8] = {0};
    struct mt_i2c_t i2c = {0};
    uint32_t val, i, j;
    volatile uint32_t k;

    i2c.id = 0;
    i2c.addr = SLAVE_ADDRESS;
    i2c.mode = FS_MODE;
    i2c.speed = 400;
    buf[0] = DATA_ADDRESS;
    wr_num = 1;
    rd_num = 5;

    I2CLOG("test i2c_write_read\n");

    ret = i2c_write_read(&i2c, buf, wr_num, rd_num);
    if (ret != 0)
        I2CERR("mt_i2c_test: read 0x%x fail\n", DATA_ADDRESS);

    I2CLOG("test i2c_write/i2c_read\n");

    for (j = 0; j < 16; j++) {
        /*write*/
        for (i = 0; i < 8; i++) {
            buf[i] =  i * 0x10 + j;
            obuf[i] = buf[i];
        }
        buf[0] = DATA_ADDRESS + j * 0x10;
        ret = i2c_write(&i2c, buf, 7);
        if (ret < 0)
            I2CERR("i2c_write fail\n");

        k = 0xffff; /*delay at least 5ms*/
        while(k--){
        };

        /*read back*/
        buf[0] = DATA_ADDRESS + j * 0x10;
        ret = i2c_write(&i2c, buf, 1);
        if (ret < 0)
            I2CERR("i2c_write fail\n");

        ret = i2c_read(&i2c, buf + 1, 6);
        if (ret < 0)
            I2CERR("i2c_read fail\n");

        for (i = 1; i < 8; i++) {
            if (obuf[i] != buf[i]) {
                I2CERR("i2c data compare error: buf[%d] = 0x%x, should be 0x%x\n", i, buf[i], obuf[i]);
            }
        }
        I2CLOG("i2c data compare pass\n");
    }

    return 0;
}
#endif

int32_t i2c_hw_init(void) {
    int i = 0;
    int ret;

    for (i = 0; i < I2C_NR; i++) {
        i2c_mutex[i] = xSemaphoreCreateMutex();
        i2c_irq_semaphore[i] = xSemaphoreCreateBinary();
    }

    ret = intc_irq_request(INTC_IRQ_I2C, INTC_GRP_4, INTC_POL_LOW, i2c0_irq_handler, NULL);
    if (ret != 0)
        I2CERR("intc_irq_request fail\n");

    I2CLOG("fwq i2c init\n");

#ifdef I2C_TEST
    /* Do not test interrupt mode, before vTaskStartScheduler */
    for (i = 0; i < 5; i++) {
        I2CLOG("test %d times\n", i);
        mt_i2c_spm_test();
    }
#endif
    return 0;
}

