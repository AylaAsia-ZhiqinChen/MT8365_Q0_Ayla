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

#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include <stdbool.h>
#include <semphr.h>
#include <dma.h>
#include <debug.h>

/* Enable this Macro to print SPI debug INFO */
//#define SPI_PRINT_DEBUG

#define SPITAG                  "[SPI-SSPM] "
#ifdef SPI_PRINT_DEBUG
#define SPI_MSG(fmt, arg...)    PRINTF_I(SPITAG fmt, ##arg)
#define SPI_ERR(fmt, arg...)    PRINTF_E(SPITAG "%d: "fmt, __LINE__, ##arg)
#else
#define SPI_MSG(fmt, arg...)
#define SPI_ERR(fmt, arg...)
#endif



#define EINVAL      201 /* Invalid argument */
#define ELEN        202 /* LEN more than 32byte */
#define EMODE       203 /* SPI more ERROR */
#define ESTATUS     204 /* Pause Status ERROR */

enum spi_error_code {
    SPI_NO_ERROR             = 0,
    SPI_GPIO_ERROR           = 1,
    SPI_CLK_ENABLE_ERROR     = 2,
    SPI_CLK_DISABLE_ERROR    = 3,
};

enum spi_sample_sel {
    POSEDGE,
    NEGEDGE
};
enum spi_cs_pol {
    ACTIVE_LOW,
    ACTIVE_HIGH
};

enum spi_cpol {
    SPI_CPOL_0,
    SPI_CPOL_1
};

enum spi_cpha {
    SPI_CPHA_0,
    SPI_CPHA_1
};

enum spi_mlsb {
    SPI_LSB,
    SPI_MSB
};

enum spi_endian {
    SPI_LENDIAN,
    SPI_BENDIAN
};

enum spi_transfer_mode {
    FIFO_TRANSFER,
    DMA_TRANSFER,
    OTHER1,
    OTHER2,
};

enum spi_pause_mode {
    PAUSE_MODE_DISABLE,
    PAUSE_MODE_ENABLE
};
enum spi_finish_intr {
    FINISH_INTR_DIS,
    FINISH_INTR_EN,
};

enum spi_deassert_mode {
    DEASSERT_DISABLE,
    DEASSERT_ENABLE
};

enum spi_ulthigh {
    ULTRA_HIGH_DISABLE,
    ULTRA_HIGH_ENABLE
};

enum spi_tckdly {
    TICK_DLY0,
    TICK_DLY1,
    TICK_DLY2,
    TICK_DLY3
};
enum spi_irq_flag {
    IRQ_IDLE,
    IRQ_BUSY
};

struct mt_chip_conf {
    uint32_t setuptime;
    uint32_t holdtime;
    uint32_t high_time;
    uint32_t low_time;
    uint32_t cs_idletime;
    uint32_t ulthgh_thrsh;
    enum spi_sample_sel sample_sel;
    enum spi_cs_pol cs_pol;
    enum spi_cpol cpol;
    enum spi_cpha cpha;
    enum spi_mlsb tx_mlsb;
    enum spi_mlsb rx_mlsb;
    enum spi_endian tx_endian;
    enum spi_endian rx_endian;
    enum spi_transfer_mode com_mod;
    enum spi_pause_mode pause;
    enum spi_finish_intr finish_intr;
    enum spi_deassert_mode deassert;
    enum spi_ulthigh ulthigh;
    enum spi_tckdly tckdly;
};

/*
 * secure dirver map region (via drApiMapPhys)
 * 1. t-driver will own virtual address
 * 2. SPI used virtual address range is 0x80000 - 0xFEFFF
 */
#define SPI_REG_CFG0        (0x00)
#define SPI_REG_CFG1        (0x04)
#define SPI_REG_TX_SRC      (0x08)
#define SPI_REG_RX_DST      (0x0C)
#define SPI_REG_TX_DATA     (0x10)
#define SPI_REG_RX_DATA     (0x14)
#define SPI_REG_CMD         (0x18)
#define SPI_REG_STATUS0     (0x1C)
#define SPI_REG_STATUS1     (0x20)
#define SPI_REG_PAD_SEL     (0x24)
#define SPI_REG_CFG2        (0x28)

/************************************************************************/

#define SPI_CFG0_CS_HOLD_OFFSET             0
#define SPI_CFG0_CS_SETUP_OFFSET            16

#define SPI_CFG0_CS_HOLD_MASK               0xffff
#define SPI_CFG0_CS_SETUP_MASK              0xffff0000

#define SPI_CFG1_CS_IDLE_OFFSET             0
#define SPI_CFG1_PACKET_LOOP_OFFSET         8
#define SPI_CFG1_PACKET_LENGTH_OFFSET       16
#define SPI_CFG1_GET_TICK_DLY_OFFSET        29

#define SPI_CFG1_CS_IDLE_MASK               0xff
#define SPI_CFG1_PACKET_LOOP_MASK           0xff00
#define SPI_CFG1_PACKET_LENGTH_MASK         0x3ff0000
#define SPI_CFG1_GET_TICK_DLY_MASK          0xe0000000

#define SPI_CFG2_SCK_HIGH_OFFSET            0
#define SPI_CFG2_SCK_LOW_OFFSET             16

#define SPI_CFG2_SCK_HIGH_MASK              0xffff
#define SPI_CFG2_SCK_LOW_MASK               0xffff0000


#define SPI_CMD_ACT_OFFSET                  0
#define SPI_CMD_RESUME_OFFSET               1
#define SPI_CMD_RST_OFFSET                  2
#define SPI_CMD_PAUSE_EN_OFFSET             4
#define SPI_CMD_DEASSERT_OFFSET             5
#define SPI_CMD_SAMPLE_SEL_OFFSET           6
#define SPI_CMD_CS_POL_OFFSET               7
#define SPI_CMD_CPHA_OFFSET                 8
#define SPI_CMD_CPOL_OFFSET                 9
#define SPI_CMD_RX_DMA_OFFSET               10
#define SPI_CMD_TX_DMA_OFFSET               11
#define SPI_CMD_TXMSBF_OFFSET               12
#define SPI_CMD_RXMSBF_OFFSET               13
#define SPI_CMD_RX_ENDIAN_OFFSET            14
#define SPI_CMD_TX_ENDIAN_OFFSET            15
#define SPI_CMD_FINISH_IE_OFFSET            16
#define SPI_CMD_PAUSE_IE_OFFSET             17

#define SPI_CMD_ACT_MASK                    0x1
#define SPI_CMD_RESUME_MASK                 0x2
#define SPI_CMD_RST_MASK                    0x4
#define SPI_CMD_PAUSE_EN_MASK               0x10
#define SPI_CMD_DEASSERT_MASK               0x20
#define SPI_CMD_SAMPLE_SEL_MASK             0x40
#define SPI_CMD_CS_POL_MASK                 0x80
#define SPI_CMD_CPHA_MASK                   0x100
#define SPI_CMD_CPOL_MASK                   0x200
#define SPI_CMD_RX_DMA_MASK                 0x400
#define SPI_CMD_TX_DMA_MASK                 0x800
#define SPI_CMD_TXMSBF_MASK                 0x1000
#define SPI_CMD_RXMSBF_MASK                 0x2000
#define SPI_CMD_RX_ENDIAN_MASK              0x4000
#define SPI_CMD_TX_ENDIAN_MASK              0x8000
#define SPI_CMD_FINISH_IE_MASK              0x10000
#define SPI_CMD_PAUSE_IE_MASK               0x20000

#define SPI_ULTRA_HIGH_EN_OFFSET            0
#define SPI_ULTRA_HIGH_THRESH_OFFSET        16

#define SPI_ULTRA_HIGH_EN_MASK              0x1
#define SPI_ULTRA_HIGH_THRESH_MASK          0xffff0000


struct spi_transfer {
    uint32_t    id;
    uint32_t    base;
    uint32_t    running;

    void        *tx_buf;
    void        *rx_buf;
    uint32_t    len;
    uint32_t    is_dma_used;
    uint32_t    is_transfer_end; /*for clear pause bit*/

    uint8_t     is_last_xfer;

    uint32_t    tx_dma; //dma_addr_t
    uint32_t    rx_dma; //dma_addr_t

    struct mt_chip_conf *chip_config;

};

#define SPI_REG_GET32(addr)     (*(volatile uint32_t*)(addr))
#define SPI_REG_SET32(addr, val)    (*(volatile uint32_t*)(addr) = val)

#define SPI_READ(base, offset)      SPI_REG_GET32(base + offset)
#define SPI_WRITE(base, offset, val)        SPI_REG_SET32(base + offset, val)

#define SPI_SET_BITS(REG, BS)       OUTREG32(REG, INREG32(REG) | (unsigned int)(BS))


extern int32_t SSPMSpiPollingHandle(uint32_t spi_id, uint32_t spi_base);

extern int32_t SpiSend(struct spi_transfer* spiData);
extern int32_t SpiTest(void);

#endif
