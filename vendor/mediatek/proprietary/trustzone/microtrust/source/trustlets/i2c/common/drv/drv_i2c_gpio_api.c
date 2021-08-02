/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (C) 2018 MICROTRUST Incorporated
 * All rights reserved
 *
 * This file and software is confidential and proprietary to MICROTRUST Inc.
 * Unauthorized copying of this file and software is strictly prohibited.
 * You MUST NOT disclose this file and software unless you get a license
 * agreement from MICROTRUST Incorporated.
 */

// clang-format off
#define HERE_IS_DRV 1
#include <drv_i2c_gpio_api_internal.h>
#include <i2c_gpio_util.h>
// clang-format on

#define ASSERT_PARAM_TYPE(pt)                                                                      \
    do {                                                                                           \
        if ((pt) != param_types)                                                                   \
            return DRV_ERROR_BAD_PARAMETER_TYPE;                                                   \
    } while (0)

#define ASSERT_PARAM_VALUE(pt)                                                                     \
    do {                                                                                           \
        if ((pt) == 0)                                                                             \
            return DRV_ERROR_NULL_PARAMETERS;                                                      \
    } while (0)

static unsigned long i2c_devapc_read(unsigned long regoff)
{
    return *(volatile unsigned long*)(devapc_virt_base + regoff);
}

static void i2c_devapc_write(unsigned long val, unsigned long regoff)
{
    *(volatile unsigned long*)(devapc_virt_base + regoff) = val;
}

static void i2c_set_module_apc(unsigned int module, E_MASK_DOM domain_num,
                               APC_ATTR permission_control)
{
    volatile unsigned int* base = NULL;

    unsigned int clr_bit = 0xFFFFFFFF ^ (0x3 << ((module % MOD_NO_IN_1_DEVAPC) * 2));
    unsigned int set_bit = permission_control << ((module % MOD_NO_IN_1_DEVAPC) * 2);

    if (domain_num == E_DOMAIN_0) {
        base = (volatile unsigned int*)((unsigned int)DEVAPC_D0_APC_0 +
                                        (module / MOD_NO_IN_1_DEVAPC) * 4);
    } else if (domain_num == E_DOMAIN_1) {
        base = (volatile unsigned int*)((unsigned int)DEVAPC_D1_APC_0 +
                                        (module / MOD_NO_IN_1_DEVAPC) * 4);
    } else if (domain_num == E_DOMAIN_2) {
        base = (volatile unsigned int*)((unsigned int)DEVAPC_D2_APC_0 +
                                        (module / MOD_NO_IN_1_DEVAPC) * 4);
    } else if (domain_num == E_DOMAIN_3) {
        base = (volatile unsigned int*)((unsigned int)DEVAPC_D3_APC_0 +
                                        (module / MOD_NO_IN_1_DEVAPC) * 4);
    } else if (domain_num == E_DOMAIN_4) {
        base = (volatile unsigned int*)((unsigned int)DEVAPC_D4_APC_0 +
                                        (module / MOD_NO_IN_1_DEVAPC) * 4);
    } else if (domain_num == E_DOMAIN_5) {
        base = (volatile unsigned int*)((unsigned int)DEVAPC_D5_APC_0 +
                                        (module / MOD_NO_IN_1_DEVAPC) * 4);
    } else if (domain_num == E_DOMAIN_6) {
        base = (volatile unsigned int*)((unsigned int)DEVAPC_D6_APC_0 +
                                        (module / MOD_NO_IN_1_DEVAPC) * 4);
    } else if (domain_num == E_DOMAIN_7) {
        base = (volatile unsigned int*)((unsigned int)DEVAPC_D7_APC_0 +
                                        (module / MOD_NO_IN_1_DEVAPC) * 4);
    }

    if (base != NULL) {
        i2c_devapc_write(i2c_devapc_read((unsigned long)base) & clr_bit, (unsigned long)base);
        i2c_devapc_write(i2c_devapc_read((unsigned long)base) | set_bit, (unsigned long)base);
    }
}

static inline void i2c_secure_enable(uint16_t bus_id)
{
    switch (bus_id) {
    case 0:
        i2c_set_module_apc(I2C0_DAPC_ID, E_DOMAIN_0, E_L1);
        break;
    case 1:
        i2c_set_module_apc(I2C1_DAPC_ID, E_DOMAIN_0, E_L1);
        break;
    case 2:
        i2c_set_module_apc(I2C2_DAPC_ID, E_DOMAIN_0, E_L1);
        break;
    case 3:
        i2c_set_module_apc(I2C3_DAPC_ID, E_DOMAIN_0, E_L1);
        break;
    case 4:
        i2c_set_module_apc(I2C4_DAPC_ID, E_DOMAIN_0, E_L1);
        break;
    default:
        msee_printf_va("(%s)[%s][%d] invalid para: bus_id = %d\n", LOG_NAME, __func__, __LINE__,
                       bus_id);
    }
}

static inline void i2c_secure_disable(uint16_t bus_id)
{
    switch (bus_id) {
    case 0:
        i2c_set_module_apc(I2C0_DAPC_ID, E_DOMAIN_0, E_L0);
        break;
    case 1:
        i2c_set_module_apc(I2C1_DAPC_ID, E_DOMAIN_0, E_L0);
        break;
    case 2:
        i2c_set_module_apc(I2C2_DAPC_ID, E_DOMAIN_0, E_L0);
        break;
    case 3:
        i2c_set_module_apc(I2C3_DAPC_ID, E_DOMAIN_0, E_L0);
        break;
    case 4:
        i2c_set_module_apc(I2C4_DAPC_ID, E_DOMAIN_0, E_L0);
        break;
    default:
        msee_printf_va("(%s)[%s][%d] invalid para: bus_id = %d\n", LOG_NAME, __func__, __LINE__,
                       bus_id);
    }
}

static inline U32 i2c_dma_secure_enable(mt_i2c* i2c)
{
    REG_WRITE(dma_virt_base + OFFSET_AP_DMA_GLOBAL_GSEC_EN,
              (REG_READ(dma_virt_base + OFFSET_AP_DMA_GLOBAL_GSEC_EN) | 0x1));

    switch (i2c->id) {
    case 0:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C0_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C0_SEC_EN) | 0x1));
        break;
    case 1:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C8_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C8_SEC_EN) | 0x1));
        break;
    case 2:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C6_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C6_SEC_EN) | 0x1));
        break;
    case 3:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C7_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C7_SEC_EN) | 0x1));
        break;
    case 4:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C3_SEC_EN,
                    (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C3_SEC_EN) | 0x1));
        break;
    default:
        msee_printf_va("(%s)[%s][%d] invalid para: i2c->id = %d\n", LOG_NAME, __func__, __LINE__,
                       i2c->id);
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_GLOBAL_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_GLOBAL_SEC_EN) | 0x0));
    }
    return DRV_SUCCESS;
}

static inline U32 i2c_dma_secure_disable(mt_i2c* i2c)
{
    switch (i2c->id) {
    case 0:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C0_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C0_SEC_EN) & ~0x1));
        break;
    case 1:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C8_SEC_EN,
                  (REG_READ(dma_virt_base+ OFFSET_AP_DMA_I2C8_SEC_EN) & ~0x1));
        break;
    case 2:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C6_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C6_SEC_EN) & ~0x1));
        break;
    case 3:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C7_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C7_SEC_EN) & ~0x1));
        break;
    case 4:
        REG_WRITE(dma_virt_base + OFFSET_AP_DMA_I2C3_SEC_EN,
                  (REG_READ(dma_virt_base + OFFSET_AP_DMA_I2C3_SEC_EN) & ~0x1));
        break;
    default:
        msee_printf_va("(%s)[%s][%d] invalid para: i2c->id=%d\n", LOG_NAME, __func__, __LINE__,
                       i2c->id);
    }

    REG_WRITE(dma_virt_base + OFFSET_AP_DMA_GLOBAL_GSEC_EN,
              (REG_READ(dma_virt_base + OFFSET_AP_DMA_GLOBAL_GSEC_EN) & ~0x1));
    return DRV_SUCCESS;
}

static void i2c_dma_record_info(mt_i2c* i2c)
{
    g_dma_regs.base = (unsigned long)i2c->pdmabase;
    g_dma_regs.int_flag = REG_READ(i2c->pdmabase + OFFSET_INT_FLAG);
    g_dma_regs.int_en = REG_READ(i2c->pdmabase + OFFSET_INT_EN);
    g_dma_regs.en = REG_READ(i2c->pdmabase + OFFSET_EN);
    g_dma_regs.rst = REG_READ(i2c->pdmabase + OFFSET_RST);
    g_dma_regs.stop = REG_READ(i2c->pdmabase + OFFSET_STOP);
    g_dma_regs.flush = REG_READ(i2c->pdmabase + OFFSET_FLUSH);
    g_dma_regs.con = REG_READ(i2c->pdmabase + OFFSET_CON);
    g_dma_regs.tx_mem_addr = REG_READ(i2c->pdmabase + OFFSET_TX_MEM_ADDR);
    g_dma_regs.rx_mem_addr = REG_READ(i2c->pdmabase + OFFSET_RX_MEM_ADDR);
    g_dma_regs.tx_len = REG_READ(i2c->pdmabase + OFFSET_TX_LEN);
    g_dma_regs.rx_len = REG_READ(i2c->pdmabase + OFFSET_RX_LEN);
    g_dma_regs.int_buf_size = REG_READ(i2c->pdmabase + OFFSET_INT_BUF_SIZE);
    g_dma_regs.debug_sta = REG_READ(i2c->pdmabase + OFFSET_DEBUG_STA);
    g_dma_regs.tx_mem_addr2 = REG_READ(i2c->pdmabase + OFFSET_TX_MEM_ADDR2);
    g_dma_regs.rx_mem_addr2 = REG_READ(i2c->pdmabase + OFFSET_RX_MEM_ADDR2);
}

static void i2c_dump_info(mt_i2c* i2c)
{
    msee_printf_va("(%s)[%s][%d] ++++++++++++++++++++++++ i2c_reg_info ++++++++++++++++++++++++\n", LOG_NAME, __func__, __LINE__);
    msee_printf_va("(%s)[%s][%d] I2C structure:\n" I2CTAG
                   "Clk=%d,Id=%d,Mode=%x,Dma_en=%x,Op=%x,Poll_en=%x,Irq_stat=%x\n" I2CTAG
                   "total_len=%x,msg_len=%x,msg_aux_len=%x,speed=%d\n",
                   LOG_NAME, __func__, __LINE__, i2c->clk, i2c->id, i2c->mode, i2c->dma_en, i2c->op,
                   i2c->poll_en, i2c->irq_stat, i2c->total_len, i2c->msg_len, i2c->msg_aux_len,
                   i2c->speed);

    msee_printf_va("(%s)[%s][%d] base address 0x%x\n", LOG_NAME, __func__, __LINE__, i2c->base);
    msee_printf_va("(%s)[%s][%d] I2C register:\n" I2CTAG
                   "SLAVE_ADDR=%x,INTR_MASK=%x,INTR_STAT=%x,CONTROL=%x,TRANSFER_LEN=%x\n" I2CTAG
                   "TRANSAC_LEN=%x,DELAY_LEN=%x,TIMING=%x,LTIMING=%x, START=%x,FIFO_STAT=%x, OFFSET_CLOCK_DIV=%x\n",
                   LOG_NAME, __func__, __LINE__, (REG_READ(i2c->base + OFFSET_SLAVE_ADDR)),
                   (REG_READ(i2c->base + OFFSET_INTR_MASK)),
                   (REG_READ(i2c->base + OFFSET_INTR_STAT)), (REG_READ(i2c->base + OFFSET_CONTROL)),
                   (REG_READ(i2c->base + OFFSET_TRANSFER_LEN)),
                   (REG_READ(i2c->base + OFFSET_TRANSAC_LEN)),
                   (REG_READ(i2c->base + OFFSET_DELAY_LEN)), (REG_READ(i2c->base + OFFSET_TIMING)),
                   (REG_READ(i2c->base + OFFSET_LTIMING)),
                   (REG_READ(i2c->base + OFFSET_START)), (REG_READ(i2c->base + OFFSET_FIFO_STAT)),
                   (REG_READ(i2c->base + OFFSET_CLOCK_DIV)));
    msee_printf_va(I2CTAG "IO_CONFIG=%x,HS=%x,DEBUGSTAT=%x,EXT_CONF=%x,TRANSFER_LEN_AUX=%x, OFFSET_DMA_INFO = %x\n",
                   (REG_READ(i2c->base + OFFSET_IO_CONFIG)), (REG_READ(i2c->base + OFFSET_HS)),
                   (REG_READ(i2c->base + OFFSET_DEBUGSTAT)),
                   (REG_READ(i2c->base + OFFSET_EXT_CONF)),
                   (REG_READ(i2c->base + OFFSET_TRANSFER_LEN_AUX)),
                   (REG_READ(i2c->base + OFFSET_DMA_INFO)));

    if (i2c->dma_en) {
        msee_printf_va("(%s)[%s][%d] before enable DMA register(0x%x):\n" I2CTAG
                       "INT_FLAG=%x,INT_EN=%x,EN=%x,RST=%x,\n" I2CTAG
                       "STOP=%x,FLUSH=%x,CON=%x,TX_MEM_ADDR=%x, RX_MEM_ADDR=%x\n",
                       LOG_NAME, __func__, __LINE__, g_dma_regs.base, g_dma_regs.int_flag,
                       g_dma_regs.int_en, g_dma_regs.en, g_dma_regs.rst, g_dma_regs.stop,
                       g_dma_regs.flush, g_dma_regs.con, g_dma_regs.tx_mem_addr,
                       g_dma_regs.rx_mem_addr);
        msee_printf_va(I2CTAG "TX_LEN=%x,RX_LEN=%x,INT_BUF_SIZE=%x,DEBUG_STATUS=%x\n" I2CTAG
                              "TX_MEM_ADDR2=%x, RX_MEM_ADDR2=%x\n",
                       g_dma_regs.tx_len, g_dma_regs.rx_len, g_dma_regs.int_buf_size,
                       g_dma_regs.debug_sta, g_dma_regs.tx_mem_addr2, g_dma_regs.rx_mem_addr2);

        msee_printf_va(
            "(%s)[%s][%d] DMA register(0x%p):\n" I2CTAG
            "INT_FLAG=%x,INT_EN=%x,EN=%x,RST=%x,\n" I2CTAG
            "STOP=%x,FLUSH=%x,CON=%x,TX_MEM_ADDR=%x, RX_MEM_ADDR=%x\n",
            LOG_NAME, __func__, __LINE__, i2c->pdmabase,
            (REG_READ(i2c->pdmabase + OFFSET_INT_FLAG)), (REG_READ(i2c->pdmabase + OFFSET_INT_EN)),
            (REG_READ(i2c->pdmabase + OFFSET_EN)), (REG_READ(i2c->pdmabase + OFFSET_RST)),
            (REG_READ(i2c->pdmabase + OFFSET_STOP)), (REG_READ(i2c->pdmabase + OFFSET_FLUSH)),
            (REG_READ(i2c->pdmabase + OFFSET_CON)), (REG_READ(i2c->pdmabase + OFFSET_TX_MEM_ADDR)),
            (REG_READ(i2c->pdmabase + OFFSET_RX_MEM_ADDR)));
        msee_printf_va(I2CTAG "TX_LEN=%x,RX_LEN=%x,INT_BUF_SIZE=%x,DEBUG_STATUS=%x\n" I2CTAG
                              "TX_MEM_ADDR2=%x, RX_MEM_ADDR2=%x\n",
                       (REG_READ(i2c->pdmabase + OFFSET_TX_LEN)),
                       (REG_READ(i2c->pdmabase + OFFSET_RX_LEN)),
                       (REG_READ(i2c->pdmabase + OFFSET_INT_BUF_SIZE)),
                       (REG_READ(i2c->pdmabase + OFFSET_DEBUG_STA)),
                       (REG_READ(i2c->pdmabase + OFFSET_TX_MEM_ADDR2)),
                       (REG_READ(i2c->pdmabase + OFFSET_RX_MEM_ADDR2)));

        msee_printf_va("(%s)[%s][%d] ---------------------- i2c_reg_info ----------------------\n", LOG_NAME, __func__, __LINE__);
    }


}

static int i2c_dma_busy_wait_ready(struct mt_i2c_t* i2c)
{
    long dma_tmo_poll = 10;
    int res = DRV_SUCCESS;

    if (NULL == i2c) {
        msee_printf_va("(%s)[%s][%d] i2c_dma_busy_wait_ready NULL pointer err\n", LOG_NAME,
                       __func__, __LINE__);
        return DRV_ERROR_NULL_PARAMETERS;
    }
    while (1 == (REG_READ(i2c->pdmabase + OFFSET_EN))) {
        msee_printf_va("(%s)[%s][%d] wait dma transfer complet,dma_tmo_poll=%ld\n", LOG_NAME,
                       __func__, __LINE__, dma_tmo_poll);
        msee_msleep(5);
        dma_tmo_poll--;
        if (dma_tmo_poll == 0) {
            res = -ETIMEDOUT_I2C;
            break;
        }
    }
    return res;
}

static void i2c_init_hw(struct mt_i2c_t* i2c)
{
    /* clear interrupt status */
    REG_WRITE(i2c->base + OFFSET_INTR_MASK, REG_READ(i2c->base + OFFSET_INTR_MASK) &
                                                ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    REG_WRITE(i2c->base + OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    REG_WRITE(i2c->base + OFFSET_SOFTRESET, I2C_SOFT_RST);
    /* Set ioconfig */
    if (i2c->pushpull) {
        REG_WRITE(i2c->base + OFFSET_IO_CONFIG, I2C_IO_CONFIG_PUSH_PULL);
    } else {
        REG_WRITE(i2c->base + OFFSET_IO_CONFIG, I2C_IO_CONFIG_OPEN_DRAIN);
    }
    REG_WRITE(i2c->base + OFFSET_CLOCK_DIV, 0x404);

    REG_WRITE(i2c->base + OFFSET_TIMING, i2c->timing_reg);
    REG_WRITE(i2c->base + OFFSET_LTIMING, i2c->ltiming_reg);
    REG_WRITE(i2c->base + OFFSET_HS, i2c->high_speed_reg);

    if (i2c->dma_en) {
        /* DMA warm reset, and waits for EN to become 0 */
        REG_WRITE(i2c->pdmabase + OFFSET_RST, I2C_DMA_WARM_RST);
        msee_msleep(1);
        if (REG_READ(i2c->pdmabase + OFFSET_EN) != 0) {
            msee_printf_va("(%s)[%s][%d] DMA bus hang\n", LOG_NAME, __func__, __LINE__);
        }
    }
}

static int i2c_bus2_irq_handler(int irq_status, void* data)
{
    (void)irq_status;
    (void)data;
    U32 i2c_base = i2c2_virt_base;

    /* Clear interrupt mask */
    REG_WRITE(i2c_base + OFFSET_INTR_MASK, REG_READ(i2c_base + OFFSET_INTR_MASK) &
                                               ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    /* Save interrupt status */
    g_i2c2_irq_stat = REG_READ(i2c_base + OFFSET_INTR_STAT);
    /* Clear interrupt status, write 1 clear */
    REG_WRITE(i2c_base + OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));

    return DRV_SUCCESS;
}

static int i2c_bus3_irq_handler(int irq_status, void* data)
{
    (void)irq_status;
    (void)data;
    U32 i2c_base = i2c3_virt_base;

    /* Clear interrupt mask */
    REG_WRITE(i2c_base + OFFSET_INTR_MASK, REG_READ(i2c_base + OFFSET_INTR_MASK) &
                                               ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    /* Save interrupt status */
    g_i2c3_irq_stat = REG_READ(i2c_base + OFFSET_INTR_STAT);
    /* Clear interrupt status, write 1 clear */
    REG_WRITE(i2c_base + OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));

    return DRV_SUCCESS;
}

static int i2c_bus4_irq_handler(int irq_status, void* data)
{
    (void)irq_status;
    (void)data;
    U32 i2c_base = i2c4_virt_base;

    msee_printf_va("enter i2c_bus4_irq_handler\n");

    /* Clear interrupt mask */
    REG_WRITE(i2c_base + OFFSET_INTR_MASK, REG_READ(i2c_base + OFFSET_INTR_MASK) &
                                               ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    /* Save interrupt status */
    g_i2c4_irq_stat = REG_READ(i2c_base + OFFSET_INTR_STAT);
    /* Clear interrupt status, write 1 clear */
    REG_WRITE(i2c_base + OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));

    return DRV_SUCCESS;
}

static S32 i2c_set_speed(mt_i2c* i2c)
{
    int mode;
    unsigned int khz;
    unsigned int step_cnt;
    unsigned int sample_cnt;
    unsigned int sclk;
    unsigned int hclk;
    unsigned int max_step_cnt;
    unsigned int sample_div = MAX_SAMPLE_CNT_DIV;
    unsigned int step_div;
    unsigned int min_div;
    unsigned int best_mul;
    unsigned int cnt_mul;
    unsigned int speed_hz;

    speed_hz = i2c->speed;

    if (speed_hz > MAX_HS_MODE_SPEED) {
        return DRV_ERROR_BAD_PARAMETERS;
    } else if (speed_hz > MAX_FS_MODE_SPEED) {
        mode = HS_MODE;
        max_step_cnt = MAX_HS_STEP_CNT_DIV;
    } else {
        mode = FS_MODE;
        max_step_cnt = MAX_STEP_CNT_DIV;
    }
    i2c->mode = mode;
    step_div = max_step_cnt;

    /* Find the best combination */
    khz = speed_hz / 1000;
    hclk = i2c->clk / 1000;
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
        msee_printf_va("(%s)[%s][%d] %s mode: unsupported speed (%ldkhz)\n", LOG_NAME, __func__,
                       __LINE__, (mode == HS_MODE) ? "HS" : "ST/FT", (long int)khz);
        I2C_BUG_ON(sclk > khz);
        return -ENOTSUPP_I2C;
    }

    step_cnt--;
    sample_cnt--;

    if (mode == HS_MODE) {
        /* Set the hign speed mode register */
        i2c->timing_reg = 0x1303;
        i2c->high_speed_reg = I2C_TIME_DEFAULT_VALUE |
                              (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 12 |
                              (step_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 8;

        i2c->ltiming_reg = (0x3 << 6) | (0x3 << 0) |
                           (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 12 |
                           (step_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 9;
    } else {
        i2c->timing_reg = (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 8 |
                          (step_cnt & I2C_TIMING_STEP_DIV_MASK) << 0;
        /* Disable the high speed transaction */
        i2c->high_speed_reg = I2C_TIME_CLR_VALUE;

        i2c->ltiming_reg = (sample_cnt & I2C_TIMING_SAMPLE_COUNT_MASK) << 6 |
                           (step_cnt & I2C_TIMING_STEP_DIV_MASK) << 0;
    }

    return 0;
}

static void i2c_clock_enable(U16 i2c_id)
{
    REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_1_CLR,
              REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_1_CLR) | AP_DMA_CG_OFFSET);
    switch (i2c_id) {
    case 0:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_CLR,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_CLR) | I2C0_CG_OFFSET);
        break;
    case 1:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_CLR,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_CLR) | I2C1_CG_OFFSET);
        break;
    case 2:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_CLR,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_CLR) | I2C2_CG_OFFSET);
        break;
    case 3:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_CLR,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_CLR) | I2C3_CG_OFFSET);
        break;
    case 4:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_2_CLR,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_2_CLR) | I2C4_CG_OFFSET);
        break;
    default:
        msee_printf_va("(%s)[%s][%d] I2C bus %d not support\n", LOG_NAME, __func__, __LINE__,
                       i2c_id);
        break;
    }
}

static void i2c_clock_disable(U16 i2c_id)
{
    switch (i2c_id) {
    case 0:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_SET,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_SET) | I2C0_CG_OFFSET);
        break;
    case 1:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_SET,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_SET) | I2C1_CG_OFFSET);
        break;
    case 2:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_SET,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_SET) | I2C2_CG_OFFSET);
        break;
    case 3:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_SET,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_0_SET) | I2C3_CG_OFFSET);
        break;
    case 4:
        REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_2_SET,
                  REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_2_SET) | I2C4_CG_OFFSET);
        break;
    default:
        msee_printf_va("(%s)[%s][%d] I2C bus %d not support\n", LOG_NAME, __func__, __LINE__,
                       i2c_id);
        break;
    }
    REG_WRITE(infracfg_virt_base + OFFSET_MODULE_SW_CG_1_SET,
              REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_1_SET) | AP_DMA_CG_OFFSET);
}

static void i2c_write_reg(mt_i2c* i2c)
{
    U32 data_size;
    U32 i;
    U32 addr_reg = 0;
    U8* ptr = (U8*)dma_buf.mem_vir;

    REG_WRITE(i2c->base + OFFSET_CLOCK_DIV, 0x404);
    REG_WRITE(i2c->base + OFFSET_CONTROL, i2c->control_reg);

    /* set start condition */
    if (i2c->speed <= I2C_DEFAULT_SPEED) {
        REG_WRITE(i2c->base + OFFSET_EXT_CONF, I2C_ST_START_CON);
    } else {
        REG_WRITE(i2c->base + OFFSET_EXT_CONF, I2C_FS_START_CON);
    }

    if (~i2c->control_reg & I2C_CONTROL_RS) { /* bit is set to 1, i.e., use repeated stop */
        REG_WRITE(i2c->base + OFFSET_DELAY_LEN, I2C_DELAY_LEN);
    }
    REG_WRITE(i2c->base + OFFSET_DELAY_LEN, I2C_DELAY_LEN);

    /* Set ioconfig */
    if (i2c->pushpull) {
        REG_WRITE(i2c->base + OFFSET_IO_CONFIG, I2C_IO_CONFIG_PUSH_PULL);
    } else {
        REG_WRITE(i2c->base + OFFSET_IO_CONFIG, I2C_IO_CONFIG_OPEN_DRAIN);
    }

    /* set timing reg */
    REG_WRITE(i2c->base + OFFSET_TIMING, i2c->timing_reg);
    REG_WRITE(i2c->base + OFFSET_LTIMING, i2c->ltiming_reg);
    REG_WRITE(i2c->base + OFFSET_HS, i2c->high_speed_reg);

    /* Set slave address */
    addr_reg = i2c->addr << 1;
    if (i2c->op == I2C_MASTER_RD) {
        addr_reg |= 0x1;
    }
    REG_WRITE(i2c->base + OFFSET_SLAVE_ADDR, addr_reg);
    /* Clear interrupt status */
    REG_WRITE(i2c->base + OFFSET_INTR_STAT, (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
    /* Clear fifo address */
    REG_WRITE(i2c->base + OFFSET_FIFO_ADDR_CLR, I2C_FIFO_ADDR_CLR);

    /* Setup the interrupt mask flag */
    if (i2c->poll_en) {
        REG_WRITE(i2c->base + OFFSET_INTR_MASK,
                  REG_READ(i2c->base + OFFSET_INTR_MASK) &
                      ~(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP)); /* Disable interrupt */
    } else {
        REG_WRITE(i2c->base + OFFSET_INTR_MASK,
                  REG_READ(i2c->base + OFFSET_INTR_MASK) |
                      (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP)); /* Enable interrupt */
    }

    /* Set transfer and transaction len */
    if (i2c->op == I2C_MASTER_WRRD) {
        REG_WRITE(i2c->base + OFFSET_TRANSFER_LEN, i2c->msg_len);
        REG_WRITE(i2c->base + OFFSET_TRANSFER_LEN_AUX, i2c->msg_aux_len);
        REG_WRITE(i2c->base + OFFSET_TRANSAC_LEN, 0x02);
    } else if (i2c->op == I2C_MASTER_MULTI_WR) {
        REG_WRITE(i2c->base + OFFSET_TRANSFER_LEN, i2c->msg_len);
        REG_WRITE(i2c->base + OFFSET_TRANSAC_LEN, i2c->total_len / i2c->msg_len);
    } else {
        REG_WRITE(i2c->base + OFFSET_TRANSFER_LEN, i2c->msg_len);
        REG_WRITE(i2c->base + OFFSET_TRANSAC_LEN, 0x01);
    }

    /* Prepare buffer data to start transfer */
    if (i2c->dma_en) {
        if (i2c->op == I2C_MASTER_RD) {
            REG_WRITE(i2c->pdmabase + OFFSET_INT_FLAG, I2C_DMA_INT_FLAG_NONE);
            REG_WRITE(i2c->pdmabase + OFFSET_CON, I2C_DMA_CON_RX);
            REG_WRITE(i2c->pdmabase + OFFSET_RX_MEM_ADDR, (U32)dma_buf.mem_phy);
            REG_WRITE(i2c->pdmabase + OFFSET_RX_LEN, i2c->msg_len);
        } else if (i2c->op == I2C_MASTER_WR || i2c->op == I2C_MASTER_MULTI_WR) {
            REG_WRITE(i2c->pdmabase + OFFSET_INT_FLAG, I2C_DMA_INT_FLAG_NONE);
            REG_WRITE(i2c->pdmabase + OFFSET_CON, I2C_DMA_CON_TX);
            REG_WRITE(i2c->pdmabase + OFFSET_TX_MEM_ADDR, (U32)dma_buf.mem_phy);
            REG_WRITE(i2c->pdmabase + OFFSET_TX_LEN, i2c->total_len);
        } else {
            REG_WRITE(i2c->pdmabase + OFFSET_INT_FLAG, I2C_DMA_INT_FLAG_NONE);
            REG_WRITE(i2c->pdmabase + OFFSET_CON, I2C_DMA_CON_TX);
            REG_WRITE(i2c->pdmabase + OFFSET_TX_MEM_ADDR, (U32)dma_buf.mem_phy);
            REG_WRITE(i2c->pdmabase + OFFSET_RX_MEM_ADDR, (U32)dma_buf.mem_phy);
            REG_WRITE(i2c->pdmabase + OFFSET_TX_LEN, i2c->msg_len);
            REG_WRITE(i2c->pdmabase + OFFSET_RX_LEN, i2c->msg_aux_len);
        }

        i2c_dma_record_info(i2c);
        I2C_MB();

        REG_WRITE(i2c->base + OFFSET_CONTROL, i2c->control_reg);
        REG_WRITE(i2c->pdmabase + OFFSET_EN, I2C_DMA_START_EN);

        msee_printf_va("(%s)[%s][%d] I2C_T_DMA addr %x dma\n", LOG_NAME, __func__, __LINE__,
                       i2c->addr);
        msee_printf_va("(%s)[%s][%d] DMA Register:INT_FLAG:0x%x, CON:0x%x\n", LOG_NAME, __func__,
                       __LINE__, REG_READ(i2c->pdmabase + OFFSET_INT_FLAG),
                       REG_READ(i2c->pdmabase + OFFSET_CON));
        msee_printf_va("(%s)[%s][%d] DMA Register:TX_MEM_ADDR:0x%x, RX_MEM_ADDR:0x%x\n", LOG_NAME,
                       __func__, __LINE__, REG_READ(i2c->pdmabase + OFFSET_TX_MEM_ADDR),
                       REG_READ(i2c->pdmabase + OFFSET_RX_MEM_ADDR));
        msee_printf_va("(%s)[%s][%d] DMA Register:TX_LEN:0x%x, RX_LEN:0x%x, EN:0x%x\n", LOG_NAME,
                       __func__, __LINE__, REG_READ(i2c->pdmabase + OFFSET_TX_LEN),
                       REG_READ(i2c->pdmabase + OFFSET_RX_LEN),
                       REG_READ(i2c->pdmabase + OFFSET_EN));
    } else {
        /* Set fifo mode data */
        if (i2c->op != I2C_MASTER_RD) {
            data_size = i2c->total_len;
            msee_printf_va("data_size = %d\n", data_size);
            while (data_size--) {
                REG_WRITE(i2c->base + OFFSET_DATA_PORT, *ptr);
                ptr++;
            }
        }
    }
}

static S32 i2c_deal_result(mt_i2c* i2c)
{
    MSEE_Result result;
    long tmo = 1;
    long tmo_poll = 0xffff;
    S32 ret = i2c->msg_len;
    int dma_err = 0;
    U16 data_size = 0;
    U8* ptr = (U8*)dma_buf.mem_vir;
    bool TRANSFER_ERROR = FALSE;

    if (i2c->poll_en) { /* master read && poll mode */
        for (;;) {      /* check the interrupt status register */
            i2c->irq_stat = REG_READ(i2c->base + OFFSET_INTR_STAT);
            if (i2c->irq_stat & (I2C_HS_NACKERR | I2C_ACKERR)) {
                /* transfer error */
                TRANSFER_ERROR = TRUE;
                /* Clear interrupt status, write 1 clear */
                REG_WRITE(i2c->base + OFFSET_INTR_STAT,
                          (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
                break;
            } else if (i2c->irq_stat & I2C_TRANSAC_COMP) {
                /* transfer complete */
                /* Clear interrupt status, write 1 clear */
                REG_WRITE(i2c->base + OFFSET_INTR_STAT,
                          (I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP));
                break;
            }
            tmo_poll--;
            if (tmo_poll == 0) {
                tmo = 0;
                break;
            }
        }
    } else { /* Interrupt mode, wait for interrupt wake up */
        result = msee_wait_for_irq_complete();
        if (result) {
            msee_printf_va("(%s)[%s][%d] msee_wait_for_irq_complete fail, result = 0x%x\n",
                           LOG_NAME, __func__, __LINE__, result);
            tmo = 0;
        } else {
            msee_printf_va("(%s)[%s][%d] msee_wait_for_irq_complete success, result = 0x%x\n",
                           LOG_NAME, __func__, __LINE__, result);
            if (i2c->id == I2C2_BUS_NO) {
                i2c->irq_stat = g_i2c2_irq_stat;
            } else if (i2c->id == I2C3_BUS_NO) {
                i2c->irq_stat = g_i2c3_irq_stat;
            } else if (i2c->id == I2C4_BUS_NO) {
                i2c->irq_stat = g_i2c4_irq_stat;
            }
            if (i2c->irq_stat & (I2C_HS_NACKERR | I2C_ACKERR)) {
                /* transfer error */
                TRANSFER_ERROR = TRUE;
            }
        }
    }

    /* Check the transfer status */
    if (!(tmo == 0) && TRANSFER_ERROR == FALSE) {
        /* Transfer success, we need to get data from fifo */
        msee_printf_va("(%s)[%s][%d] Transfer success\n", LOG_NAME, __func__, __LINE__);
        /* only read mode or write_read mode and fifo mode need to get data */
        if ((!i2c->dma_en) && (i2c->op == I2C_MASTER_RD || i2c->op == I2C_MASTER_WRRD)) {
            // data_size = (REG_READ(i2c->base + OFFSET_FIFO_STAT) >> 4) & 0x000F;
            if (i2c->op == I2C_MASTER_WRRD)
                data_size = i2c->msg_aux_len;
            else
                data_size = i2c->msg_len;
            msee_printf_va("(%s)[%s][%d] data_size = %x\n", LOG_NAME, __func__, __LINE__,
                           data_size);
            while (data_size--) {
                *ptr = REG_READ(i2c->base + OFFSET_DATA_PORT);
                msee_printf_va("(%s)[%s][%d] addr %x read byte = 0x%x\n", LOG_NAME, __func__,
                               __LINE__, i2c->addr, *ptr);
                ptr++;
            }
        }

        if (i2c->dma_en) {
          msee_printf_va("i2c_dma_busy_wait_ready\n");
            dma_err = i2c_dma_busy_wait_ready(i2c);
            if (dma_err) {
                msee_printf_va("(%s)[%s][%d] i2c wait dma ready err\n", LOG_NAME, __func__,
                               __LINE__);
                i2c_dump_info(i2c);
                i2c_init_hw(i2c);
            }
        }
    } else {
        /* Timeout or ACKERR */
        if (tmo == 0) {
            msee_printf_va("(%s)[%s][%d] id=%d,addr: %x, transfer timeout\n", LOG_NAME, __func__,
                           __LINE__, i2c->id, i2c->addr);
            ret = -ETIMEDOUT_I2C;
        }
        if (TRANSFER_ERROR == TRUE) {
            msee_printf_va("(%s)[%s][%d] id=%d,addr: %x, transfer error\n", LOG_NAME, __func__,
                           __LINE__, i2c->id, i2c->addr);
            ret = -EREMOTEIO_I2C;
        }
        if (i2c->isFilterMsg == FALSE) {
            i2c_dump_info(i2c);
        }
        i2c_init_hw(i2c);
    }
    return ret;
}

/*
 * In MTK platform the STOP will be issued after each
 * message was transferred which is not flow the clarify
 * for i2c_transfer(), several I2C devices tolerate the STOP,
 * but some device need Repeat-Start and do not compatible with STOP
 * MTK platform has WRRD mode which can write then read with
 * Repeat-Start between two message, so we combined two
 * messages into one transaction.
 * The max read length is 4096
 */
static bool i2c_should_combine(struct mt_i2c_msg* msg)
{
    struct mt_i2c_msg* next_msg = msg + 1;

    if ((next_msg->len < 4096) && msg->addr == next_msg->addr && !(msg->flags & I2C_M_RD) &&
        (next_msg->flags & I2C_M_RD) == I2C_M_RD) {
        return TRUE;
    }
    return FALSE;
}

static bool i2c_should_batch(struct mt_i2c_msg* prev, struct mt_i2c_msg* next)
{
    if ((prev->flags & I2C_M_RD) || (next->flags & I2C_M_RD))
        return FALSE;
    if (prev->len == next->len && prev->addr == next->addr)
        return TRUE;
    return FALSE;
}

static inline void i2c_copy_to_dma(mt_i2c* i2c, struct mt_i2c_msg* msg)
{
    /* if the operate is write, write-read, multi-write, need to copy the data
             to DMA memory */
    if (!(msg->flags & I2C_M_RD))
        msee_memcpy((void*)dma_buf.mem_vir + i2c->total_len - msg->len, (void*)msg->buf, msg->len);
}

static inline void i2c_copy_from_dma(mt_i2c* i2c, struct mt_i2c_msg* msg)
{
    (void)i2c;
    /* if the operate is read, need to copy the data from DMA memory */
    if (msg->flags & I2C_M_RD)
        msee_memcpy((void*)msg->buf, (void*)dma_buf.mem_vir, msg->len);
}

static void i2c_config(mt_i2c* i2c, struct mt_i2c_msg* msg)
{
    switch (i2c->id) {
    case 0:
        msee_printf_va("(%s)[%s][%d] I2C bus %d not support\n", LOG_NAME, __func__, __LINE__,
                       i2c->id);
        return;
    case 1:
        msee_printf_va("(%s)[%s][%d] I2C bus %d not support\n", LOG_NAME, __func__, __LINE__,
                       i2c->id);
        return;
    case 2:
        i2c->base = i2c2_virt_base;
        break;
    case 3:
        i2c->base = i2c3_virt_base;
        break;
    case 4:
        i2c->base = i2c4_virt_base;
        break;
    default:
        msee_printf_va("(%s)[%s][%d] invalid para: i2c->id = %d\n", LOG_NAME, __func__, __LINE__,
                       i2c->id);
        return;
    }

    if (i2c->total_len > 8 || i2c->msg_aux_len > 8) {
        i2c->dma_en = TRUE;
    } else {
        i2c->dma_en = FALSE;
    }

    if (msg->ext_flag & I2C_A_FILTER_MSG) {
        i2c->isFilterMsg = TRUE;
    } else {
        i2c->isFilterMsg = FALSE;
    }

    if (msg->ext_flag & I2C_POLLING_FLAG) {
        i2c->poll_en = TRUE;
    } else {
        i2c->poll_en = FALSE;
    }

    /* Set device speed, set it before set_control register */
    i2c->speed = msg->timing;

    /* Set ioconfig */
    if (msg->ext_flag & I2C_PUSHPULL_FLAG) {
        i2c->pushpull = TRUE;
    } else {
        i2c->pushpull = FALSE;
    }
}

static S32 i2c_transfer_interface(mt_i2c* i2c)
{
    S32 ret = DRV_SUCCESS;

#ifdef SECURE_LOCK
    // i2c_secure_enable(i2c->id);
    i2c_dma_secure_enable(i2c);
#endif

    #if 1

    if (i2c->poll_en == FALSE) {
        if (i2c->id == I2C2_BUS_NO) {
            ret = msee_request_irq(I2C2_IRQ_NUM, i2c_bus2_irq_handler, MSEE_INTR_MODE_LOW_LEVEL, 0,
                                   NULL);
            if (ret) {
                msee_printf_va("(%s)[%s][%d] msee_request_irq(i2c2) failed, ret = 0x%x\n", LOG_NAME,
                               __func__, __LINE__, ret);
                goto err;
            }
        }
        if (i2c->id == I2C3_BUS_NO) {
            ret = msee_request_irq(I2C3_IRQ_NUM, i2c_bus3_irq_handler, MSEE_INTR_MODE_LOW_LEVEL, 0,
                                   NULL);
            if (ret) {
                msee_printf_va("(%s)[%s][%d] msee_request_irq(i2c3) failed, ret = 0x%x\n", LOG_NAME,
                               __func__, __LINE__, ret);
                goto err;
            }
        }
        if (i2c->id == I2C4_BUS_NO) {
            ret = msee_request_irq(I2C4_IRQ_NUM, i2c_bus4_irq_handler, MSEE_INTR_MODE_LOW_LEVEL, 0,
                                   NULL);
            if (ret) {
                msee_printf_va("(%s)[%s][%d] msee_request_irq(i2c4) failed, ret = 0x%x\n", LOG_NAME,
                               __func__, __LINE__, ret);
                goto err;
            }
            msee_printf_va("msee_request_irq(i2c4) success\n");
        }
    }

    if (i2c->dma_en) {
        if (i2c->pdmabase == 0) {
            msee_printf_va("(%s)[%s][%d] I2C%d doesnot support DMA mode\n", LOG_NAME, __func__,
                           __LINE__, i2c->id);
            I2C_BUG_ON(i2c->pdmabase == 0);
            ret = DRV_ERROR_BAD_PARAMETERS;
            goto err;
        }
    }

    i2c->irq_stat = 0;

    /* get clock */
    i2c->clk = I2C_CLK_RATE;

    ret = i2c_set_speed(i2c);
    if (ret < 0) {
        msee_printf_va("(%s)[%s][%d] i2c_set_speed fail, ret = %d\n", LOG_NAME, __func__, __LINE__,
                       ret);
        ret = DRV_ERROR_BAD_PARAMETERS;
        goto err;
    }
    /* Set Control Register */

    i2c->control_reg = I2C_CONTROL_ACKERR_DET_EN | I2C_CONTROL_CLK_EXT_EN;
    if (i2c->dma_en) {
        i2c->control_reg |= I2C_CONTROL_DMA_EN | I2C_CONTROL_DMAACK_EN | I2C_CONTROL_ASYNC_MODE;
    }

    if (i2c->speed > MAX_FS_MODE_SPEED) {
        i2c->control_reg |= I2C_CONTROL_RS;
    }
    if (I2C_MASTER_WRRD == i2c->op) {
        i2c->control_reg |= I2C_CONTROL_DIR_CHANGE | I2C_CONTROL_RS;
    }

    i2c_write_reg(i2c);

    /* All register must be prepared before setting the start bit [SMP] */
    I2C_MB();

    i2c_dump_info(i2c);

    /* Start the transfer */
    REG_WRITE(i2c->base + OFFSET_START, I2C_TRANSAC_START);

    msee_printf_va("(%s)[%s][%d] OFFSET_START=%x\n", LOG_NAME, __func__, __LINE__,
                   REG_READ(i2c->base + OFFSET_START));

    ret = i2c_deal_result(i2c);

    if (i2c->poll_en == FALSE) {
        if (i2c->id == I2C2_BUS_NO) {
            msee_free_irq(I2C2_IRQ_NUM);
        }
        if (i2c->id == I2C3_BUS_NO) {
            msee_free_irq(I2C3_IRQ_NUM);
        }
        if (i2c->id == I2C4_BUS_NO) {
            msee_free_irq(I2C4_IRQ_NUM);
        }
    }

err:
#ifdef SECURE_LOCK
    i2c_dma_secure_disable(i2c);
    // i2c_secure_disable(i2c->id);
#endif

#endif

    return ret;
}

static S32 i2c_transfer(mt_i2c* i2c, struct mt_i2c_msg msgs[], S32 num)
{
    S32 ret = I2C_OK;
    S32 left_num = num;

    while (left_num--) {
        /* In MTK platform the max transfer number is 4096 */
        if (msgs->len > MAX_DMA_TRANS_SIZE) {
            msee_printf_va("(%s)[%s][%d] message data length is more than 4096\n", LOG_NAME,
                           __func__, __LINE__);
            ret = DRV_ERROR_BAD_PARAMETERS;
            goto err_exit;
        }
        if (msgs->addr == 0) {
            msee_printf_va("(%s)[%s][%d] addr is invalid.\n", LOG_NAME, __func__, __LINE__);
            ret = DRV_ERROR_BAD_PARAMETERS;
            goto err_exit;
        }
        if (msgs->buf == NULL) {
            msee_printf_va("(%s)[%s][%d] data buffer is NULL.\n", LOG_NAME, __func__, __LINE__);
            ret = DRV_ERROR_NULL_PARAMETERS;
            goto err_exit;
        }

        i2c->id = msgs->id;
        i2c->addr = msgs->addr;
        i2c->msg_len = msgs->len;
        i2c->msg_aux_len = 0;
        // i2c->pdmabase = dma_virt_base + 0x300 + (0x80 * (i2c->id));
        if (i2c->id == I2C2_BUS_NO) {
            i2c->pdmabase = dma_virt_base + I2C2_DMA_PHY_OFFSET;
        } else if (i2c->id == I2C3_BUS_NO) {
            i2c->pdmabase = dma_virt_base + I2C3_DMA_PHY_OFFSET;
        } else if (i2c->id == I2C4_BUS_NO) {
            i2c->pdmabase = dma_virt_base + I2C4_DMA_PHY_OFFSET;
        }

        if ((left_num + 1 == num) || !i2c_should_batch(msgs - 1, msgs)) {
            i2c->total_len = msgs->len;
            if (msgs->flags & I2C_M_RD)
                i2c->op = I2C_MASTER_RD;
            else
                i2c->op = I2C_MASTER_WR;
        } else {
            i2c->total_len += msgs->len;
        }

        /*
         * always use DMA mode.
         * 1st when write need copy the data of message to dma memory
         * 2nd when read need copy the DMA data to the message buffer.
         * The length should be less than 255.
         */
        i2c_copy_to_dma(i2c, msgs);

        if (left_num >= 1) {
            if (i2c_should_batch(msgs, msgs + 1)) {
                i2c->op = I2C_MASTER_MULTI_WR;
                msgs++;
                continue;
            }
            if (i2c_should_combine(msgs)) {
                i2c->msg_aux_len = (msgs + 1)->len;
                i2c->op = I2C_MASTER_WRRD;
                left_num--;
            }
        }
        msee_printf_va("(%s)[%s][%d] i2c->op = %d\n", LOG_NAME, __func__, __LINE__, i2c->op);
        msee_printf_va("(%s)[%s][%d] i2c->total_len = %d\n", LOG_NAME, __func__, __LINE__,
                       i2c->total_len);
        msee_printf_va("(%s)[%s][%d] i2c->msg_len = %d\n", LOG_NAME, __func__, __LINE__,
                       i2c->msg_len);
        msee_printf_va("(%s)[%s][%d] i2c->msg_aux_len = %d\n", LOG_NAME, __func__, __LINE__,
                       i2c->msg_aux_len);

        i2c_config(i2c, msgs);
        ret = i2c_transfer_interface(i2c);

        if (ret < 0)
            goto err_exit;
        if (i2c->op == I2C_MASTER_WRRD)
            i2c_copy_from_dma(i2c, msgs + 1);
        else
            i2c_copy_from_dma(i2c, msgs);

        msgs++;
        msee_printf_va("(%s)[%s][%d] left_num = %d, i2c->op 0x%x\n", LOG_NAME, __func__, __LINE__,
                       left_num, i2c->op);
        /* after combined two messages so we need ignore one */
        if (left_num > 0 && i2c->op == I2C_MASTER_WRRD)
            msgs++;
    }
    /* the return value is number of executed messages */
    ret = num;
err_exit:
    return ret;
}

static U32 gpio_get_pin_addr(unsigned long pin, PIN_addr pin_addr[])
{
    return gpio_virt_base + pin_addr[pin].offset;
}

static int gpio_set_dir(unsigned long pin, unsigned long dir)
{
    unsigned long bit;
#ifdef GPIO_BRINGUP
    unsigned long reg;
#endif
    U32 addr;

    if (pin >= MAX_GPIO_PIN)
        return DRV_ERROR_BAD_PARAMETERS;

    if (dir >= GPIO_DIR_MAX)
        return DRV_ERROR_BAD_PARAMETERS;

    addr = gpio_get_pin_addr(pin, DIR_addr);
    bit = DIR_offset[pin].offset;

#ifdef GPIO_BRINGUP
    reg = REG_READ(addr);
    if (dir == GPIO_DIR_IN)
        reg &= (~(1 << bit));
    else
        reg |= (1 << bit);

    REG_WRITE(addr, reg);
#else
    if (dir == GPIO_DIR_IN)
        REG_WRITE((1L << bit), addr + 8);
    else
        REG_WRITE((1L << bit), addr + 4);
#endif

    msee_printf_va("(%s)[%s][%d] pin:%ld, dir:%ld\n", LOG_NAME, __func__, __LINE__, pin, dir);

    return DRV_SUCCESS;
}

static int gpio_get_dir(unsigned long pin)
{
    unsigned long bit;
    unsigned long reg;
    U32 addr;

    if (pin >= MAX_GPIO_PIN)
        return DRV_ERROR_BAD_PARAMETERS;

    addr = gpio_get_pin_addr(pin, DIR_addr);
    bit = DIR_offset[pin].offset;

    reg = REG_READ(addr);
    return ((reg & (1L << bit)) != 0) ? 1 : 0;
}

static int gpio_set_out(unsigned long pin, unsigned long output)
{
    unsigned long bit;
#ifdef GPIO_BRINGUP
    unsigned long reg;
#endif
    U32 addr;

    if (pin >= MAX_GPIO_PIN)
        return DRV_ERROR_BAD_PARAMETERS;

    if (output >= GPIO_OUT_MAX)
        return DRV_ERROR_BAD_PARAMETERS;

    bit = DATAOUT_offset[pin].offset;
    addr = gpio_get_pin_addr(pin, DATAOUT_addr);

#ifdef GPIO_BRINGUP
    reg = REG_READ(addr);
    if (output == GPIO_OUT_ZERO)
        reg &= (~(1 << bit));
    else
        reg |= (1 << bit);

    REG_WRITE(addr, reg);
#else
    if (output == GPIO_OUT_ZERO)
        REG_WRITE((1L << bit), addr + 8);
    else
        REG_WRITE((1L << bit), addr + 4);
#endif

    return DRV_SUCCESS;
}

static int gpio_get_out(unsigned long pin)
{
    unsigned long bit;
    unsigned long reg;
    U32 addr;

    if (pin >= MAX_GPIO_PIN)
        return DRV_ERROR_BAD_PARAMETERS;

    addr = gpio_get_pin_addr(pin, DATAOUT_addr);
    bit = DATAOUT_offset[pin].offset;
    reg = REG_READ(addr);

    return ((reg & (1L << bit)) != 0) ? 1 : 0;
}

static int gpio_get_in(unsigned long pin)
{

    unsigned long bit;
    unsigned long reg;
    U32 addr;

    if (pin >= MAX_GPIO_PIN)
        return DRV_ERROR_BAD_PARAMETERS;

    addr = gpio_get_pin_addr(pin, DATAIN_addr);
    bit = DATAIN_offset[pin].offset;
    reg = REG_READ(addr);

    return ((reg & (1L << bit)) != 0) ? 1 : 0;
}

static int gpio_set_mode(unsigned long pin, unsigned long mode)
{
    unsigned long bit;
    unsigned long reg;
    unsigned long mask = (1L << GPIO_MODE_BITS) - 1;
    U32 addr;

    if (pin >= MAX_GPIO_PIN)
        return DRV_ERROR_BAD_PARAMETERS;

    if (mode >= GPIO_MODE_MAX)
        return DRV_ERROR_BAD_PARAMETERS;

    bit = MODE_offset[pin].offset;
    addr = gpio_get_pin_addr(pin, MODE_addr);

    mode = mode & mask;
    reg = REG_READ(addr);
    reg &= (~(mask << bit));
    reg |= (mode << bit);
    REG_WRITE(addr, reg);

    return DRV_SUCCESS;
}

static int gpio_get_mode(unsigned long pin)
{
    unsigned long bit;
    unsigned long reg;
    unsigned long mask = (1L << GPIO_MODE_BITS) - 1;
    U32 addr;

    if (pin >= MAX_GPIO_PIN)
        return DRV_ERROR_BAD_PARAMETERS;

    addr = gpio_get_pin_addr(pin, MODE_addr);
    bit = MODE_offset[pin].offset;
    reg = REG_READ(addr);

    return (reg >> bit) & mask;
}

int drv_i2c_gpio_api_init(void)
{
    int ret = DRV_SUCCESS;
    void* i2c2_virt_base_p = NULL;
    void* i2c3_virt_base_p = NULL;
    void* i2c4_virt_base_p = NULL;
    void* devapc_virt_base_p = NULL;
    void* dma_virt_base_p = NULL;
    void* gpio_virt_base_p = NULL;
    void* infracfg_virt_base_p = NULL;

    msee_printf_va("(%s)[%s][%d] ++\n", LOG_NAME, __func__, __LINE__);

    ret = msee_continuous_mem_alloc(BUF_SIZE, &dma_buf.mem_vir, &dma_buf.mem_phy);
    if (ret) {
        msee_printf_va("(%s)[%s][%d]init driver: msee_continuous_mem_alloc(write) fail.\n",
                       LOG_NAME, __func__, __LINE__);
        goto exit;
    }
    msee_printf_va("(%s)[%s][%d]init driver: dma_buf.mem_vir = 0x%x, dma_buf.mem_phy = 0x%x\n",
                   LOG_NAME, __func__, __LINE__, dma_buf.mem_vir, dma_buf.mem_phy);

    ret =
        msee_mmap_region(I2C2_PHY_START_ADDR, &i2c2_virt_base_p, I2C2_PHY_SIZE, MSEE_MAP_HARDWARE);
    if (ret) {
        msee_printf_va("(%s)[%s][%d]msee_mmap_region i2c2 failed.\n", LOG_NAME, __func__, __LINE__);
        goto exit_continuous_mem_free;
    }
    i2c2_virt_base = (U32)i2c2_virt_base_p+0x300;
    msee_printf_va("(%s)[%s][%d]msee_mmap_region i2c2 success, i2c2_virt_base = 0x%x\n", LOG_NAME,
                   __func__, __LINE__, i2c2_virt_base);

    ret =
        msee_mmap_region(I2C3_PHY_START_ADDR, &i2c3_virt_base_p, I2C3_PHY_SIZE, MSEE_MAP_HARDWARE);
    if (ret) {
        msee_printf_va("(%s)[%s][%d]msee_mmap_region i2c3 failed.\n", LOG_NAME, __func__, __LINE__);
        goto exit_unmmap_region_i2c2;
    }
    i2c3_virt_base = (U32)i2c3_virt_base_p;
    msee_printf_va("(%s)[%s][%d]msee_mmap_region i2c3 success, i2c3_virt_base = 0x%x\n", LOG_NAME,
                   __func__, __LINE__, i2c3_virt_base);

    ret =
        msee_mmap_region(I2C4_PHY_START_ADDR, &i2c4_virt_base_p, I2C4_PHY_SIZE, MSEE_MAP_HARDWARE);
    if (ret) {
        msee_printf_va("(%s)[%s][%d]msee_mmap_region i2c4 failed.\n", LOG_NAME, __func__, __LINE__);
        goto exit_unmmap_region_i2c3;
    }
    i2c4_virt_base = (U32)i2c4_virt_base_p+0x300;
    msee_printf_va("(%s)[%s][%d]msee_mmap_region i2c4 success, i2c4_virt_base = 0x%x\n", LOG_NAME,
                   __func__, __LINE__, i2c4_virt_base);

    ret = msee_mmap_region(DEVAPC_PHY_START_ADDR, &devapc_virt_base_p, DEVAPC_PHY_SIZE,
                           MSEE_MAP_HARDWARE);
    if (ret) {
        msee_printf_va("(%s)[%s][%d]msee_mmap_region devapc failed.\n", LOG_NAME, __func__,
                       __LINE__);
        goto exit_unmmap_region_i2c4;
    }
    devapc_virt_base = (U32)devapc_virt_base_p;
    msee_printf_va("(%s)[%s][%d]msee_mmap_region devapc success, devapc_virt_base = 0x%x\n",
                   LOG_NAME, __func__, __LINE__, devapc_virt_base);

    ret =
        msee_mmap_region(APDMA_PHY_START_ADDR, &dma_virt_base_p, APDMA_PHY_SIZE, MSEE_MAP_HARDWARE);
    if (ret) {
        msee_printf_va("(%s)[%s][%d]msee_mmap_region apdma failed.\n", LOG_NAME, __func__,
                       __LINE__);
        goto exit_unmmap_region_devapc;
    }
    dma_virt_base = (U32)dma_virt_base_p;
    msee_printf_va("(%s)[%s][%d]msee_mmap_region apdma success, dma_virt_base = 0x%x\n", LOG_NAME,
                   __func__, __LINE__, dma_virt_base);

    ret =
        msee_mmap_region(GPIO_PHY_START_ADDR, &gpio_virt_base_p, GPIO_PHY_SIZE, MSEE_MAP_HARDWARE);
    if (ret) {
        msee_printf_va("(%s)[%s][%d]msee_mmap_region gpio failed.\n", LOG_NAME, __func__, __LINE__);
        goto exit_unmmap_region_dma;
    }
    gpio_virt_base = (U32)gpio_virt_base_p;
    msee_printf_va("(%s)[%s][%d]msee_mmap_region gpio success, gpio_virt_base = 0x%x\n", LOG_NAME,
                   __func__, __LINE__, gpio_virt_base);

    ret = msee_mmap_region(INFRACFG_PHY_START_ADDR, &infracfg_virt_base_p, INFRACFG_PHY_SIZE,
                           MSEE_MAP_HARDWARE);
    if (ret) {
        msee_printf_va("(%s)[%s][%d]msee_mmap_region infracfg failed.\n", LOG_NAME, __func__,
                       __LINE__);
        goto exit_unmmap_region_gpio;
    }
    infracfg_virt_base = (U32)infracfg_virt_base_p;
    msee_printf_va("(%s)[%s][%d]msee_mmap_region infracfg success, infracfg_virt_base = 0x%x\n",
                   LOG_NAME, __func__, __LINE__, infracfg_virt_base);

    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return DRV_SUCCESS;

exit_unmmap_region_gpio:
    msee_unmmap_region(gpio_virt_base_p, GPIO_PHY_SIZE);
exit_unmmap_region_dma:
    msee_unmmap_region(dma_virt_base_p, APDMA_PHY_SIZE);
exit_unmmap_region_devapc:
    msee_unmmap_region(devapc_virt_base_p, DEVAPC_PHY_SIZE);
exit_unmmap_region_i2c4:
    msee_unmmap_region(i2c4_virt_base_p, I2C4_PHY_SIZE);
exit_unmmap_region_i2c3:
    msee_unmmap_region(i2c3_virt_base_p, I2C3_PHY_SIZE);
exit_unmmap_region_i2c2:
    msee_unmmap_region(i2c2_virt_base_p, I2C2_PHY_SIZE);
exit_continuous_mem_free:
    msee_continuous_mem_free((void*)&dma_buf.mem_vir);
exit:
    return ret;
}

int drv_i2c_api_transfer(unsigned long args)
{
    struct mt_i2c_t i2c;
    struct i2c_msg_data* msg_data = NULL;
    struct mt_i2c_msg msg[MAX_I2C_CMD_LEN];
    int result = DRV_SUCCESS, ret = DRV_SUCCESS;
    int i, j;
    int cg_state;
    void* va_l1 = NULL;
    void* va_l2 = NULL;
    void* va_l2_data = NULL;
    U16 transfer_length = 0;

    ASSERT_PARAM_VALUE(args);

    result =
        msee_map_user(&va_l1, (void*)(args), sizeof(struct i2c_msg_data), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        ret = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    msg_data = (struct i2c_msg_data*)va_l1;
    msee_printf_va("(%s)[%s][%d] msg_data->trans_num = %d\n", LOG_NAME, __func__, __LINE__,
                   msg_data->trans_num);
    msee_printf_va("(%s)[%s][%d] msg_data->buffer_len = %d\n", LOG_NAME, __func__, __LINE__,
                   msg_data->buffer_len);
    msee_printf_va("(%s)[%s][%d] msg_data->buffer_data_len = %d\n", LOG_NAME, __func__, __LINE__,
                   msg_data->buffer_data_len);

    result = msee_map_user(&va_l2, msg_data->buffer, msg_data->buffer_len, MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l2) failed\n", LOG_NAME, __func__, __LINE__);
        ret = DRV_ERROR_MSEE_MAP_USER_LV2;
        goto exit_unmap_user_l1;
    }

    result = msee_map_user(&va_l2_data, msg_data->buffer_data, msg_data->buffer_data_len,
                           MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l2_data) failed\n", LOG_NAME, __func__,
                       __LINE__);
        ret = DRV_ERROR_MSEE_MAP_USER_LV2;
        goto exit_unmap_user_l2;
    }

    transfer_length = msg_data->buffer_data_len / msg_data->trans_num;
    msee_printf_va("(%s)[%s][%d] transfer_length = %d\n", LOG_NAME, __func__, __LINE__,
                   transfer_length);
    for (i = 0; i < msg_data->trans_num; i++) {
        msee_memcpy((void*)(msg + i), va_l2 + (i * sizeof(struct mt_i2c_msg)),
                    sizeof(struct mt_i2c_msg));
        msg[i].buf = (U8*)(va_l2_data + (i * transfer_length));
#if DATA_DUMP
        msee_printf_va("(%s)[%s][%d] msg[%d].id = 0x%x\n", LOG_NAME, __func__, __LINE__, i,
                       msg[i].id);
        msee_printf_va("(%s)[%s][%d] msg[%d].addr = 0x%x\n", LOG_NAME, __func__, __LINE__, i,
                       msg[i].addr);
        msee_printf_va("(%s)[%s][%d] msg[%d].flags = 0x%x\n", LOG_NAME, __func__, __LINE__, i,
                       msg[i].flags);
        msee_printf_va("(%s)[%s][%d] msg[%d].len = 0x%x\n", LOG_NAME, __func__, __LINE__, i,
                       msg[i].len);
        msee_printf_va("(%s)[%s][%d] msg[%d].timing = 0x%x\n", LOG_NAME, __func__, __LINE__, i,
                       msg[i].timing);
        msee_printf_va("(%s)[%s][%d] msg[%d].ext_flag = 0x%x\n", LOG_NAME, __func__, __LINE__, i,
                       msg[i].ext_flag);
        for (j = 0; j < (int)transfer_length; j++) {
            msee_printf_va("(%s)[%s][%d] msg[%d].buf[%d] = 0x%x\n", LOG_NAME, __func__, __LINE__, i,
                           j, msg[i].buf[j]);
        }
#endif
    }

    i2c_clock_enable(msg[0].id);
    // cg_state = REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_2_STA);
    // msee_printf_va("+++cg_state = 0x%x\n", cg_state);
    result = i2c_transfer(&i2c, msg, msg_data->trans_num);
    i2c_clock_disable(msg[0].id);
    // cg_state = REG_READ(infracfg_virt_base + OFFSET_MODULE_SW_CG_2_STA);
    // msee_printf_va("---cg_state = 0x%x\n", cg_state);
    if (result != msg_data->trans_num) {
        ret = -1;
        msee_printf_va("(%s)[%s][%d] i2c transfer failed\n", LOG_NAME, __func__, __LINE__);
    } else {
        if (i2c.op == I2C_MASTER_RD || i2c.op == I2C_MASTER_WRRD) {
            msee_printf_va("(%s)[%s][%d] After i2c transfer\n", LOG_NAME, __func__, __LINE__);
            for (i = 0; i < msg_data->trans_num; i++) {
                for (j = 0; j < (int)transfer_length; j++) {
                    msee_printf_va("(%s)[%s][%d] msg[%d].buf[%d] = 0x%x\n", LOG_NAME, __func__,
                                   __LINE__, i, j, msg[i].buf[j]);
                }
            }
        }
        ret = DRV_SUCCESS;
    }

    result = msee_unmap_user(va_l2_data);
    if (result) {
        msee_printf_va("(%s)[%s][%d]msee_unmap_user(va_l2_data) failed\n", LOG_NAME, __func__,
                       __LINE__);
        ret = DRV_ERROR_MSEE_UNMAP_USER_LV2;
    }

exit_unmap_user_l2:
    result = msee_unmap_user(va_l2);
    if (result) {
        msee_printf_va("(%s)[%s][%d]msee_unmap_user(va_l2) failed\n", LOG_NAME, __func__, __LINE__);
        ret = DRV_ERROR_MSEE_UNMAP_USER_LV2;
    }
exit_unmap_user_l1:
    result = msee_unmap_user(va_l1);
    if (result) {
        msee_printf_va("(%s)[%s][%d]msee_unmap_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        ret = DRV_ERROR_MSEE_UNMAP_USER_LV1;
    }
exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return ret;
}

int drv_i2c_api_secure(unsigned long args)
{
    struct secure_i2c_msg* secure_i2c = NULL;
    int result = DRV_SUCCESS;
    void* va_l1 = NULL;

    ASSERT_PARAM_VALUE(args);

    result =
        msee_map_user(&va_l1, (void*)(args), sizeof(struct secure_i2c_msg), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    secure_i2c = (struct secure_i2c_msg*)va_l1;
    msee_printf_va("(%s)[%s][%d] secure_i2c->bus_id = %d\n", LOG_NAME, __func__, __LINE__,
                   secure_i2c->bus_id);
    msee_printf_va("(%s)[%s][%d] secure_i2c->slave_addr = %x\n", LOG_NAME, __func__, __LINE__,
                   secure_i2c->slave_addr);
    msee_printf_va("(%s)[%s][%d] secure_i2c->lock = %d\n", LOG_NAME, __func__, __LINE__,
                   secure_i2c->lock);

    if (secure_i2c->lock) {
        i2c_secure_enable(secure_i2c->bus_id);
    } else {
        i2c_secure_disable(secure_i2c->bus_id);
    }

    // TODO: handle slave_addr here.

    result = msee_unmap_user(va_l1);
    if (result) {
        msee_printf_va("(%s)[%s][%d]msee_unmap_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_UNMAP_USER_LV1;
    }

exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return result;
}

int drv_gpio_api_set_dir(unsigned long args)
{
    struct gpio_msg* gpio = NULL;
    int result = DRV_SUCCESS;
    void* va_l1 = NULL;

    ASSERT_PARAM_VALUE(args);

    result = msee_map_user(&va_l1, (void*)(args), sizeof(struct gpio_msg), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    gpio = (struct gpio_msg*)va_l1;
    msee_printf_va("(%s)[%s][%d] gpio->pin = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin);
    msee_printf_va("(%s)[%s][%d] gpio->mode = %d\n", LOG_NAME, __func__, __LINE__, gpio->mode);
    msee_printf_va("(%s)[%s][%d] gpio->dir = %d\n", LOG_NAME, __func__, __LINE__, gpio->dir);
    msee_printf_va("(%s)[%s][%d] gpio->output = %d\n", LOG_NAME, __func__, __LINE__, gpio->output);

    result = gpio_set_dir(gpio->pin, gpio->dir);
    if (result) {
        msee_printf_va("(%s)[%s][%d] gpio_set_dir failed\n", LOG_NAME, __func__, __LINE__);
        goto exit_unmap_user_l1;
    }

exit_unmap_user_l1:
    result = msee_unmap_user(va_l1);
    if (result) {
        msee_printf_va("(%s)[%s][%d]msee_unmap_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_UNMAP_USER_LV1;
    }
exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return result;
}

int drv_gpio_api_get_dir(unsigned long args)
{
    struct gpio_msg* gpio = NULL;
    int result = DRV_SUCCESS;
    void* va_l1 = NULL;

    ASSERT_PARAM_VALUE(args);

    result = msee_map_user(&va_l1, (void*)(args), sizeof(struct gpio_msg), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    gpio = (struct gpio_msg*)va_l1;
    msee_printf_va("(%s)[%s][%d] gpio->pin = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin);
    msee_printf_va("(%s)[%s][%d] gpio->mode = %d\n", LOG_NAME, __func__, __LINE__, gpio->mode);
    msee_printf_va("(%s)[%s][%d] gpio->dir = %d\n", LOG_NAME, __func__, __LINE__, gpio->dir);
    msee_printf_va("(%s)[%s][%d] gpio->output = %d\n", LOG_NAME, __func__, __LINE__, gpio->output);

    result = gpio_get_dir(gpio->pin);
    gpio->dir = (unsigned long)result;
    msee_printf_va("(%s)[%s][%d] gpio(%d) dir = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin,
                   gpio->dir);
    msee_unmap_user(va_l1);
    result = DRV_SUCCESS;

exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return result;
}

int drv_gpio_api_set_out(unsigned long args)
{
    struct gpio_msg* gpio = NULL;
    int result = DRV_SUCCESS;
    void* va_l1 = NULL;

    ASSERT_PARAM_VALUE(args);

    result = msee_map_user(&va_l1, (void*)(args), sizeof(struct gpio_msg), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    gpio = (struct gpio_msg*)va_l1;
    msee_printf_va("(%s)[%s][%d] gpio->pin = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin);
    msee_printf_va("(%s)[%s][%d] gpio->mode = %d\n", LOG_NAME, __func__, __LINE__, gpio->mode);
    msee_printf_va("(%s)[%s][%d] gpio->dir = %d\n", LOG_NAME, __func__, __LINE__, gpio->dir);
    msee_printf_va("(%s)[%s][%d] gpio->output = %d\n", LOG_NAME, __func__, __LINE__, gpio->output);

    result = gpio_set_out(gpio->pin, gpio->output);
    if (result) {
        msee_printf_va("(%s)[%s][%d] gpio_set_out failed\n", LOG_NAME, __func__, __LINE__);
        goto exit_unmap_user_l1;
    }

exit_unmap_user_l1:
    result = msee_unmap_user(va_l1);
    if (result) {
        msee_printf_va("(%s)[%s][%d]msee_unmap_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_UNMAP_USER_LV1;
    }
exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return result;
}

int drv_gpio_api_get_out(unsigned long args)
{
    struct gpio_msg* gpio = NULL;
    int result = DRV_SUCCESS;
    void* va_l1 = NULL;

    ASSERT_PARAM_VALUE(args);

    result = msee_map_user(&va_l1, (void*)(args), sizeof(struct gpio_msg), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    gpio = (struct gpio_msg*)va_l1;
    msee_printf_va("(%s)[%s][%d] gpio->pin = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin);
    msee_printf_va("(%s)[%s][%d] gpio->mode = %d\n", LOG_NAME, __func__, __LINE__, gpio->mode);
    msee_printf_va("(%s)[%s][%d] gpio->dir = %d\n", LOG_NAME, __func__, __LINE__, gpio->dir);
    msee_printf_va("(%s)[%s][%d] gpio->output = %d\n", LOG_NAME, __func__, __LINE__, gpio->output);

    result = gpio_get_out(gpio->pin);
    gpio->output = (unsigned long)result;
    msee_printf_va("(%s)[%s][%d] gpio(%d) out = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin,
                   gpio->output);
    msee_unmap_user(va_l1);
    result = DRV_SUCCESS;

exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return result;
}

int drv_gpio_api_get_in(unsigned long args)
{
    struct gpio_msg* gpio = NULL;
    int result = DRV_SUCCESS;
    void* va_l1 = NULL;

    ASSERT_PARAM_VALUE(args);

    result = msee_map_user(&va_l1, (void*)(args), sizeof(struct gpio_msg), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    gpio = (struct gpio_msg*)va_l1;
    msee_printf_va("(%s)[%s][%d] gpio->pin = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin);
    msee_printf_va("(%s)[%s][%d] gpio->mode = %d\n", LOG_NAME, __func__, __LINE__, gpio->mode);
    msee_printf_va("(%s)[%s][%d] gpio->dir = %d\n", LOG_NAME, __func__, __LINE__, gpio->dir);

    result = gpio_get_in(gpio->pin);
    gpio->output = (unsigned long)result;
    msee_printf_va("(%s)[%s][%d] value of gpio(%d) = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin,
                   gpio->output);
    msee_unmap_user(va_l1);
    result = DRV_SUCCESS;

exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return result;
}

int drv_gpio_api_set_mode(unsigned long args)
{
    struct gpio_msg* gpio = NULL;
    int result = DRV_SUCCESS;
    void* va_l1 = NULL;

    ASSERT_PARAM_VALUE(args);

    result = msee_map_user(&va_l1, (void*)(args), sizeof(struct gpio_msg), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    gpio = (struct gpio_msg*)va_l1;
    msee_printf_va("(%s)[%s][%d] gpio->pin = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin);
    msee_printf_va("(%s)[%s][%d] gpio->mode = %d\n", LOG_NAME, __func__, __LINE__, gpio->mode);
    msee_printf_va("(%s)[%s][%d] gpio->dir = %d\n", LOG_NAME, __func__, __LINE__, gpio->dir);
    msee_printf_va("(%s)[%s][%d] gpio->output = %d\n", LOG_NAME, __func__, __LINE__, gpio->output);

    result = gpio_set_mode(gpio->pin, gpio->mode);
    if (result) {
        msee_printf_va("(%s)[%s][%d] gpio_set_out failed\n", LOG_NAME, __func__, __LINE__);
        goto exit_unmap_user_l1;
    }

exit_unmap_user_l1:
    result = msee_unmap_user(va_l1);
    if (result) {
        msee_printf_va("(%s)[%s][%d]msee_unmap_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_UNMAP_USER_LV1;
    }
exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return result;
}

int drv_gpio_api_get_mode(unsigned long args)
{
    struct gpio_msg* gpio = NULL;
    int result = DRV_SUCCESS;
    void* va_l1 = NULL;

    ASSERT_PARAM_VALUE(args);

    result = msee_map_user(&va_l1, (void*)(args), sizeof(struct gpio_msg), MSEE_MAP_USER_DEFAULT);
    if (result) {
        msee_printf_va("(%s)[%s][%d] msee_map_user(va_l1) failed\n", LOG_NAME, __func__, __LINE__);
        result = DRV_ERROR_MSEE_MAP_USER_LV1;
        goto exit;
    }
    gpio = (struct gpio_msg*)va_l1;
    msee_printf_va("(%s)[%s][%d] gpio->pin = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin);
    msee_printf_va("(%s)[%s][%d] gpio->mode = %d\n", LOG_NAME, __func__, __LINE__, gpio->mode);
    msee_printf_va("(%s)[%s][%d] gpio->dir = %d\n", LOG_NAME, __func__, __LINE__, gpio->dir);
    msee_printf_va("(%s)[%s][%d] gpio->output = %d\n", LOG_NAME, __func__, __LINE__, gpio->output);

    result = gpio_get_mode(gpio->pin);
    gpio->mode = (unsigned long)result;
    msee_printf_va("(%s)[%s][%d] gpio(%d) mode = %d\n", LOG_NAME, __func__, __LINE__, gpio->pin,
                   gpio->mode);
    msee_unmap_user(va_l1);
    result = DRV_SUCCESS;

exit:
    msee_printf_va("(%s)[%s][%d] --\n", LOG_NAME, __func__, __LINE__);

    return result;
}
