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

#define I2CTAG                   "[I2C-SSPM] "
#define I2CFUC()                 PRINTF_D(I2CTAG "%s\n", __func__)
#define I2CLOG(fmt, arg...)      PRINTF_D(I2CTAG fmt, ##arg)
#define I2CERR(fmt, arg...)      PRINTF_E(I2CTAG "%d: "fmt, __LINE__, ##arg)
#define I2CLOG_ISR(fmt, arg...)  PRINTF_D_ISR(I2CTAG fmt, ##arg)
#define I2CERR_ISR(fmt, arg...)  PRINTF_E_ISR(I2CTAG "%d: "fmt, __LINE__, ##arg)

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
#define I2C_WRITE_FAIL_HS_NACKERR           0xA013
#define I2C_WRITE_FAIL_ACKERR               0xA014
#define I2C_WRITE_FAIL_TIMEOUT              0xA015

/* #define I2C_CLK_WRAPPER_RATE    36000    kHz for wrapper I2C work frequency  */

/******************************************register operation***********************************/

extern SemaphoreHandle_t i2c_mutex[I2C_NR];/* For semaphore handling */

enum {
    I2C0 = 0,
    I2C1 = 1,
    I2C2 = 2,
    I2C3 = 3,
};

#ifdef CONFIG_MT_I2C_FPGA_ENABLE
#define FPGA_CLOCK        12000    /* FPGA crystal frequency (KHz) */
#define I2C_CLK_DIV       (5*2)    /* frequency divisor */
#define I2C_CLK_RATE      (FPGA_CLOCK / I2C_CLK_DIV)    /* I2C base clock (KHz) */
#else
#define I2C_CLK_RATE      66000 /* TODO: Calculate from bus clock */
#define I2C_CLK_DIV       5    /* frequency divisor */
#endif

/* ACCESS CH1 register in SPM , need add offset 0x80 */
enum SPM_I2C_REGS_OFFSET {
    SPM_OFFSET_DATA_PORT = 0x80,    /* 0x0 */
    SPM_OFFSET_SLAVE_ADDR = 0x84,    /* 0x04 */
    SPM_OFFSET_INTR_MASK = 0x88,    /* 0x08 */
    SPM_OFFSET_INTR_STAT = 0x8C,    /* 0x0C */
    SPM_OFFSET_CONTROL = 0x90,    /* 0X10 */
    SPM_OFFSET_TRANSFER_LEN = 0x94,    /* 0X14 */
    SPM_OFFSET_TRANSAC_LEN = 0x98,    /* 0X18 */
    SPM_OFFSET_DELAY_LEN = 0x9C,    /* 0X1C */
    SPM_OFFSET_TIMING = 0xA0,    /* 0X20 */
    SPM_OFFSET_START = 0xA4,    /* 0X24 */
    SPM_OFFSET_FIFO_STAT = 0xB0,    /* 0X30 */
    SPM_OFFSET_FIFO_ADDR_CLR = 0xB8,    /* 0X38 */
    SPM_OFFSET_RSV_DEBUG = 0xC4,    /* 0X44 */
    SPM_OFFSET_HS = 0xC8,    /* 0X48 */
    SPM_OFFSET_SOFTRESET = 0xC0,    /* 0X50 */
    SPM_OFFSET_DCM_EN = 0xD4,    /* 0X54 */
    SPM_OFFSET_DEBUGCTRL = 0xE8,    /* 0x68 */
};
#define OFFSET_INTR_STAT    0x0c
#define OFFSET_DEBUGSTAT    0x64

#define I2C_HS_NACKERR            (1 << 2)
#define I2C_ACKERR                (1 << 1)
#define I2C_TRANSAC_COMP          (1 << 0)
#define SPM_I2C_ACKERR                (1 << 5) /*SPM need check bit4*/
#define SPM_I2C_TRANSAC_COMP          (1 << 4)

#define I2C_FIFO_SIZE             7

#define MAX_ST_MODE_SPEED         100    /* khz */
#define MAX_FS_MODE_SPEED         400    /* khz */
#define MAX_HS_MODE_SPEED         3400    /* khz */

#define MAX_DMA_TRANS_SIZE        65532    /* Max(65535) aligned to 4 bytes = 65532 */
#define MAX_DMA_TRANS_NUM         256

#define MAX_SAMPLE_CNT_DIV        8
#define MAX_STEP_CNT_DIV          64
#define MAX_HS_STEP_CNT_DIV       8

enum DMA_REGS_OFFSET {
    OFFSET_INT_FLAG = 0x0,
    OFFSET_INT_EN = 0x04,
    OFFSET_EN = 0x08,
    OFFSET_RST = 0x0C,
    OFFSET_CON = 0x18,
    OFFSET_TX_MEM_ADDR = 0x1C,
    OFFSET_RX_MEM_ADDR = 0x20,
    OFFSET_TX_LEN = 0x24,
    OFFSET_RX_LEN = 0x28,
};

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
#define I2C_CONTROL_WRAPPER          (0x1 << 0)
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
#if defined(I2C_EARLY_PORTING) || defined(I2C_TEST)
extern int32_t mt_i2c_test(void);
#endif

#endif                /* __MT_I2C_H__ */
