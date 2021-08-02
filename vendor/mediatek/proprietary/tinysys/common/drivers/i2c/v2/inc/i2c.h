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

#ifndef __MT_I2C_H__
#define __MT_I2C_H__

#include <stdint.h>
#include <stdbool.h>
#include <semphr.h>
#include <debug.h>
#include <hal_i2c.h> /*IP base header*/

/*#define I2C_DEBUG*/
#ifdef I2C_DEBUG
#define I2CTAG                   "[I2C-SSPM] "
#define I2CFUC()                 PRINTF_D(I2CTAG "%s %d\n", __func__, __LINE__)
#define I2CDBG(fmt, arg...)      PRINTF_D(I2CTAG fmt, ##arg)
#define I2CLOG(fmt, arg...)      PRINTF_D(I2CTAG fmt, ##arg)
#define I2CERR(fmt, arg...)      PRINTF_E(I2CTAG "%d: "fmt, __LINE__, ##arg)
#define I2CLOG_ISR(fmt, arg...)  PRINTF_D_ISR(I2CTAG fmt, ##arg)
#define I2CERR_ISR(fmt, arg...)  PRINTF_E_ISR(I2CTAG "%d: "fmt, __LINE__, ##arg)
#else
#define I2CTAG                   "I@"
#define I2CFUC()
#define I2CDBG(fmt, arg...)
#define I2CLOG(fmt, arg...)
#define I2CERR(fmt, arg...)      PRINTF_E(I2CTAG fmt, ##arg)
#define I2CLOG_ISR(fmt, arg...)
#define I2CERR_ISR(fmt, arg...)  PRINTF_E_ISR(I2CTAG fmt, ##arg)
#endif

#ifdef CFG_FPGA
#define CONFIG_MT_I2C_FPGA_ENABLE
#endif

#define I2C_BUG_ON(a)
#define I2C_M_RD       0x0001

/* #define I2C_TEST */

#define I2C_OK                              0x0000
#define EAGAIN_I2C                          11    /* Try again */
#define EINVAL_I2C                          22    /* Invalid argument */
#define EOPNOTSUPP_I2C                      95    /* Operation not supported on transport endpoint */
#define ETIMEDOUT_I2C                       110    /* Connection timed out */
#define EREMOTEIO_I2C                       121    /* Remote I/O error */
#define ENOTSUPP_I2C                        524    /* Remote I/O error */

/***********************************register operation******************************************/

extern SemaphoreHandle_t i2c_mutex[I2C_NR];/* For semaphore handling */

enum {
    I2C0 = 0,
    I2C1 = 1,
    I2C2 = 2,
    I2C3 = 3,
};

#ifdef CONFIG_MT_I2C_FPGA_ENABLE
#define FPGA_CLOCK        10000    /* FPGA crystal frequency (KHz) */
#define I2C_CLK_DIV       (5*2)    /* frequency divisor */
#define I2C_CLK_RATE      (FPGA_CLOCK / I2C_CLK_DIV)    /* I2C base clock (KHz) */
#else
#define I2C_SRC_CLOCK     62500
#define I2C_CLK_DIV       5    /* frequency divisor */
#define I2C_CLK_RATE      (I2C_SRC_CLOCK / I2C_CLK_DIV)    /* I2C base clock (KHz) */
#endif

enum I2C_REGS_OFFSET {
    OFFSET_DATA_PORT = 0x0,
    OFFSET_SLAVE_ADDR = 0x04,
    OFFSET_INTR_MASK = 0x08,
    OFFSET_INTR_STAT = 0x0c,
    OFFSET_CONTROL = 0x10,
    OFFSET_TRANSFER_LEN = 0x14,
    OFFSET_TRANSAC_LEN = 0x18,
    OFFSET_DELAY_LEN = 0x1c,
    OFFSET_TIMING = 0x20,
    OFFSET_START = 0x24,
    OFFSET_EXT_CONF = 0x28,
    OFFSET_LTIMING = 0x2c,
    OFFSET_FIFO_STAT = 0x30,
    OFFSET_FIFO_THRESH = 0x34,
    OFFSET_FIFO_ADDR_CLR = 0x38,
    OFFSET_IO_CONFIG = 0x40,
    OFFSET_RSV_DEBUG = 0x44,
    OFFSET_HS = 0x48,
    OFFSET_SOFTRESET = 0x50,
    OFFSET_DCM_EN = 0x54,
    OFFSET_PATH_DIR = 0x60,
    OFFSET_DEBUGSTAT = 0x64,
    OFFSET_DEBUGCTRL = 0x68,
    OFFSET_TRANSFER_LEN_AUX = 0x6c,
};

#define I2C_HS_NACKERR            (1 << 2)
#define I2C_ACKERR                (1 << 1)
#define I2C_TRANSAC_COMP          (1 << 0)

#define I2C_FIFO_SIZE             7

#define MAX_ST_MODE_SPEED         100    /* khz */
#define MAX_FS_MODE_SPEED         400    /* khz */
#define MAX_HS_MODE_SPEED         3400   /* khz */

#define MAX_DMA_TRANS_SIZE        65532    /* Max(65535) aligned to 4 bytes = 65532 */
#define MAX_DMA_TRANS_NUM         256

#define MAX_SAMPLE_CNT_DIV        8
#define MAX_STEP_CNT_DIV          64
#define MAX_HS_STEP_CNT_DIV       8

#define I2C_TIMING_STEP_DIV_MASK         (0x3f << 0)
#define I2C_TIMING_SAMPLE_COUNT_MASK     (0x7 << 0)

#define I2C_TIME_CLR_VALUE               0x0000
#define I2C_TIME_DEFAULT_VALUE           0x0003

enum i2c_trans_st_rs {
    I2C_TRANS_STOP = 0,
    I2C_TRANS_REPEATED_START,
};

typedef enum {
    ST_MODE,
    FS_MODE,
    HS_MODE,
} I2C_SPEED_MODE;

enum mt_trans_op {
    I2C_MASTER_NONE = 0,
    I2C_MASTER_WR = 1,
    I2C_MASTER_RD,
    I2C_MASTER_WRRD,
};

/* CONTROL */
#define I2C_CONTROL_RS          (0x1 << 1)
#define I2C_CONTROL_DMA_EN      (0x1 << 2)
#define I2C_CONTROL_CLK_EXT_EN      (0x1 << 3)
#define I2C_CONTROL_DIR_CHANGE      (0x1 << 4)
#define I2C_CONTROL_ACKERR_DET_EN   (0x1 << 5)
#define I2C_CONTROL_TRANSFER_LEN_CHANGE (0x1 << 6)
#define I2C_CONTROL_DMA_ACK_EN            (0x1 << 8)
#define I2C_CONTROL_DMA_ASYNC_MODE        (0x1 << 9)

/***********************************end of register operation****************************************/

/***********************************I2C Param********************************************************/
struct mt_trans_data {
    uint16_t trans_num;
    uint16_t data_size;
    uint16_t trans_len;
    uint16_t trans_auxlen;
};

typedef struct mt_i2c_t {
    /* ==========set in i2c probe============ */
    uint32_t base;        /* i2c base addr */
    uint16_t id;
    uint16_t irqnr;        /* i2c interrupt number */
    uint16_t irq_stat;    /* i2c interrupt status */
    uint32_t clk;        /* host clock speed in khz */
    uint32_t pdn;        /*clock number */
    /* ==========common data define============ */
    enum i2c_trans_st_rs st_rs;
    enum mt_trans_op op;
    uint32_t pdmabase;
    uint32_t speed;        /* The speed (khz) */
    uint16_t delay_len;    /* number of half pulse between transfers in a trasaction */
    uint32_t msg_len;    /* number of half pulse between transfers in a trasaction */
    uint8_t *msg_buf;    /* pointer to msg data      */
    uint8_t addr;        /* The address of the slave device, 7bit,the value include read/write bit. */
    uint8_t master_code;    /* master code in HS mode */
    uint8_t mode;        /* ST/FS/HS mode */
    /* ==========reserved funtion============ */
    uint8_t is_push_pull_enable;    /* IO push-pull or open-drain */
    uint8_t is_clk_ext_disable;    /* clk entend default enable */
    uint8_t is_dma_enabled;    /* Transaction via DMA instead of 8-byte FIFO */
    uint8_t read_flag;    /* read,write,read_write */
    bool dma_en;
    bool poll_en;
    bool pushpull;        /* open drain */
    bool filter_msg;    /* filter msg error log */
    bool i2c_3dcamera_flag;    /* flag for 3dcamera */

    /* ==========define reg============ */
    uint16_t timing_reg;
    uint16_t ltiming_reg;
    uint16_t high_speed_reg;
    uint16_t control_reg;
    struct mt_trans_data trans_data;
} mt_i2c;

/* ============================================================================== */
/* I2C Exported Function */
/* ============================================================================== */
extern int32_t i2c_read(mt_i2c *i2c, uint8_t *buffer, uint32_t len);
extern int32_t i2c_write(mt_i2c *i2c, uint8_t *buffer, uint32_t len);
extern int32_t i2c_write_read(mt_i2c *i2c, uint8_t *buffer, uint32_t write_len,
                              uint32_t read_len);
extern int32_t i2c_set_speed(mt_i2c *i2c);
extern int32_t i2c_hw_init(void);

/*#define I2C_EARLY_PORTING*/
/*#define I2C_TEST*/
#define SET_I2C_FIX_SPEED
#if defined(I2C_EARLY_PORTING) || defined(I2C_TEST)
extern int32_t mt_i2c_test(void);
#endif

#endif                /* __MT_I2C_H__ */
