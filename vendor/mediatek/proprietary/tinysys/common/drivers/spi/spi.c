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
#include <driver_api.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <spi.h>
#include <ispeed.h>
#include <spi-plat.h>
#include <irq.h>

#define SPI_TRANSFER_POLLING

/*#define SPI_AUTO_SELECT_MODE*/
#ifdef SPI_AUTO_SELECT_MODE
#define SPI_DATA_SIZE 32
#endif
#define IDLE 0
#define INPROGRESS 1
#define PAUSED 2
#define SPI_FIFO_SIZE 32

#define PACKET_SIZE 0x400



//#define SPI_DMA
#if SPI_DMA
static struct mt_chip_conf mt_chip_conf_dma_test = {
    .setuptime = 50,
    .holdtime = 50,
    .high_time = 5,
    .low_time = 5,
    .cs_idletime = 10,
    .ulthgh_thrsh = 0,
    .cpol = SPI_CPOL_0,
    .cpha = SPI_CPHA_0,
    .rx_mlsb = SPI_MSB,
    .tx_mlsb = SPI_MSB,
    .tx_endian = SPI_LENDIAN,
    .rx_endian = SPI_LENDIAN,
    .com_mod = DMA_TRANSFER,
    .pause = PAUSE_MODE_DISABLE,
    .finish_intr = FINISH_INTR_EN,
    .deassert = DEASSERT_DISABLE,
    .ulthigh = ULTRA_HIGH_DISABLE,
    .tckdly = TICK_DLY0
};
#endif

static struct mt_chip_conf mt_chip_conf_fifo_test = {
    .setuptime = 50,
    .holdtime = 50,
    .high_time = 5,
    .low_time = 5,
    .cs_idletime = 10,
    .ulthgh_thrsh = 0,
    .cpol = SPI_CPOL_0,
    .cpha = SPI_CPHA_0,
    .rx_mlsb = SPI_MSB,
    .tx_mlsb = SPI_MSB,
    .tx_endian = SPI_LENDIAN,
    .rx_endian = SPI_LENDIAN,
    .com_mod = FIFO_TRANSFER,
    .pause = PAUSE_MODE_DISABLE,
    .finish_intr = FINISH_INTR_EN,
    .deassert = DEASSERT_DISABLE,
    .ulthigh = ULTRA_HIGH_DISABLE,
    .tckdly = TICK_DLY0
};

void dump_chip_config(struct mt_chip_conf *chip_config)
{
    if (chip_config != NULL) {
        SPI_MSG("setuptime=%d\n", chip_config->setuptime);
        SPI_MSG("holdtime=%d\n", chip_config->holdtime);
        SPI_MSG("high_time=%d\n", chip_config->high_time);
        SPI_MSG("low_time=%d\n", chip_config->low_time);
        SPI_MSG("cs_idletime=%d\n", chip_config->cs_idletime);
        SPI_MSG("ulthgh_thrsh=%d\n", chip_config->ulthgh_thrsh);
        SPI_MSG("cpol=%d\n", chip_config->cpol);
        SPI_MSG("cpha=%d\n", chip_config->cpha);
        SPI_MSG("tx_mlsb=%d\n", chip_config->tx_mlsb);
        SPI_MSG("rx_mlsb=%d\n", chip_config->rx_mlsb);
        SPI_MSG("tx_endian=%d\n", chip_config->tx_endian);
        SPI_MSG("rx_endian=%d\n", chip_config->rx_endian);
        SPI_MSG("com_mod=%d\n", chip_config->com_mod);
        SPI_MSG("pause=%d\n", chip_config->pause);
        SPI_MSG("finish_intr=%d\n", chip_config->finish_intr);
        SPI_MSG("deassert=%d\n", chip_config->deassert);
        SPI_MSG("ulthigh=%d\n", chip_config->ulthigh);
        SPI_MSG("tckdly=%d\n", chip_config->tckdly);
    } else {
        SPI_ERR("dump chip_config is NULL !!\n");
    }
}

void dump_reg_config(uint32_t base)
{
    SPI_MSG("SPI_REG_CFG0=0x%x\n", ispeed_single_read(base + SPI_REG_CFG0));
    SPI_MSG("SPI_REG_CFG1=0x%x\n", ispeed_single_read(base + SPI_REG_CFG1));
    SPI_MSG("SPI_REG_CFG2=0x%x\n", ispeed_single_read(base + SPI_REG_CFG2));
    SPI_MSG("SPI_REG_TX_SRC=0x%x\n", ispeed_single_read(base + SPI_REG_TX_SRC));
    SPI_MSG("SPI_REG_RX_DST=0x%x\n", ispeed_single_read(base + SPI_REG_RX_DST));
    SPI_MSG("SPI_REG_CMD=0x%x\n", ispeed_single_read(base + SPI_REG_CMD));
    SPI_MSG("SPI_REG_PAD_SEL=0x%x\n", ispeed_single_read(base + SPI_REG_PAD_SEL));
    SPI_MSG("SPI_REG_STATUS1=0x%x\n", ispeed_single_read(base + SPI_REG_STATUS1));
}

static uint32_t IsInterruptEnable(uint32_t base)
{
    uint32_t cmd;
    cmd = ispeed_single_read(base + SPI_REG_CMD);
    return (cmd >> SPI_CMD_FINISH_IE_OFFSET) & 1;
}

static void inline clear_pause_bit(uint32_t base)
{
    uint32_t reg_val;

    reg_val = ispeed_single_read(base + SPI_REG_CMD);
    reg_val &= ~SPI_CMD_PAUSE_EN_MASK;
    ispeed_single_write(base + SPI_REG_CMD, reg_val);
}

static void inline SetPauseBit(uint32_t base)
{
    uint32_t reg_val;

    reg_val = ispeed_single_read(base + SPI_REG_CMD);
    reg_val |= 1 << SPI_CMD_PAUSE_EN_OFFSET;
    ispeed_single_write(base + SPI_REG_CMD, reg_val);
}

static void inline ClearResumeBit(uint32_t base)
{
    uint32_t reg_val;

    reg_val = ispeed_single_read(base + SPI_REG_CMD);
    reg_val &= ~SPI_CMD_RESUME_MASK;
    ispeed_single_write(base + SPI_REG_CMD, reg_val);
}

/*
*  SpiSetupPacket: used to define per data length and loop count
* @ ptr : data structure from User
*/
static uint32_t inline SpiSetupPacket(struct spi_transfer *xfer)
{
    uint32_t packet_size = 0;
    uint32_t packet_loop = 0;
    uint32_t cfg1 = 0;
    uint32_t base = xfer->base;

    /*set transfer packet and loop*/
    if (xfer->len < PACKET_SIZE)
        packet_size = xfer->len;
    else
        packet_size = PACKET_SIZE;

    if (xfer->len % packet_size) {
        ///packet_loop = xfer->len/packet_size + 1;
        /*parameter not correct, there will be more data transfer,notice user to change*/
        SPI_ERR("The lens are not a multiple of %d, your len %u\n\n", PACKET_SIZE, xfer->len);
    }
    packet_loop = (xfer->len) / packet_size;

    cfg1 = ispeed_single_read(base + SPI_REG_CFG1);
    cfg1 &= ~(SPI_CFG1_PACKET_LENGTH_MASK + SPI_CFG1_PACKET_LOOP_MASK);
    cfg1 |= (packet_size - 1) << SPI_CFG1_PACKET_LENGTH_OFFSET;
    cfg1 |= (packet_loop - 1) << SPI_CFG1_PACKET_LOOP_OFFSET;
    ispeed_single_write(base + SPI_REG_CFG1, cfg1);

    return 0;
}

static void inline SpiStartTransfer(uint32_t base)
{
    uint32_t reg_val;
    reg_val = ispeed_single_read(base + SPI_REG_CMD);
    reg_val |= 1 << SPI_CMD_ACT_OFFSET;

    /*All register must be prepared before setting the start bit [SMP]*/
    ispeed_single_write(base + SPI_REG_CMD, reg_val);

    return;
}

/*
*  SpiChipConfig: used to define per data length and loop count
* @ ptr : HW config setting from User
*/
void SpiChipConfig(uint32_t id, struct mt_chip_conf *ptr)
{
    struct mt_chip_conf *ChipConfig = SpiGetDefaultChipConfig(id);

    if (NULL == ptr) { //default
        ChipConfig->setuptime = 3;
        ChipConfig->holdtime = 3;
        ChipConfig->high_time = 10;
        ChipConfig->low_time = 10;
        ChipConfig->cs_idletime = 2;
        ChipConfig->ulthgh_thrsh = 0;

        ChipConfig->cpol = SPI_CPOL_0;
        ChipConfig->cpha = SPI_CPHA_0;
        ChipConfig->cs_pol = ACTIVE_LOW;
        ChipConfig->sample_sel = POSEDGE;

        ChipConfig->rx_mlsb = SPI_MSB;
        ChipConfig->tx_mlsb = SPI_MSB;

        ChipConfig->tx_endian = SPI_LENDIAN;
        ChipConfig->rx_endian = SPI_LENDIAN;

        ChipConfig->com_mod = FIFO_TRANSFER;
        ChipConfig->pause = PAUSE_MODE_DISABLE;
        ChipConfig->finish_intr = FINISH_INTR_EN;
        ChipConfig->deassert = DEASSERT_DISABLE;
        ChipConfig->ulthigh = ULTRA_HIGH_DISABLE;
        ChipConfig->tckdly = TICK_DLY0;
    } else {
        ChipConfig->setuptime = ptr->setuptime;
        ChipConfig->holdtime = ptr->holdtime;
        ChipConfig->high_time = ptr->high_time;
        ChipConfig->low_time = ptr->low_time;
        ChipConfig->cs_idletime = ptr->cs_idletime;
        ChipConfig->ulthgh_thrsh = ptr->ulthgh_thrsh;

        ChipConfig->cpol = ptr->cpol;
        ChipConfig->cpha = ptr->cpha;

        ChipConfig->rx_mlsb = ptr->rx_mlsb;
        ChipConfig->tx_mlsb = ptr->tx_mlsb ;

        ChipConfig->tx_endian = ptr->tx_endian;
        ChipConfig->rx_endian = ptr->rx_endian;

        ChipConfig->com_mod = ptr->com_mod;
        ChipConfig->pause = ptr->pause;
        ChipConfig->finish_intr = ptr->finish_intr;
        ChipConfig->deassert = ptr->deassert;
        ChipConfig->ulthigh = ptr->ulthigh;
        ChipConfig->tckdly = ptr->tckdly ;

    }

    //dump_chip_config(ChipConfig);
    SetChipConfig(id, ChipConfig);
}

static void inline SpiDisableDma(uint32_t base)
{
    uint32_t  cmd;

    cmd = ispeed_single_read(base + SPI_REG_CMD);
    cmd &= ~SPI_CMD_TX_DMA_MASK;
    cmd &= ~SPI_CMD_RX_DMA_MASK;
    ispeed_single_write(base + SPI_REG_CMD, cmd);
}

#define INVALID_DMA_ADDRESS 0xffffffff

static inline void SpiEnableDma(struct spi_transfer *xfer, uint32_t mode)
{
    uint32_t cmd;
    uint32_t base = xfer->base;

    cmd = ispeed_single_read(base + SPI_REG_CMD);
#define SPI_4B_ALIGN 0x4
    /* set up the DMA bus address */
    if ((mode == DMA_TRANSFER) || (mode == OTHER1)) {
        if ((xfer->tx_buf != NULL) || ((xfer->tx_dma != INVALID_DMA_ADDRESS) && (xfer->tx_dma != 0))) {
            if (xfer->tx_dma & (SPI_4B_ALIGN - 1)) {
                SPI_ERR("Warning!Tx_DMA address should be 4Byte alignment,buf:%p,dma:%x\n",
                        xfer->tx_buf, xfer->tx_dma);
            }
            ispeed_single_write(base + SPI_REG_TX_SRC, xfer->tx_dma);
            cmd |= 1 << SPI_CMD_TX_DMA_OFFSET;
        }
    }
    if ((mode == DMA_TRANSFER) || (mode == OTHER2)) {
        if ((xfer->rx_buf != NULL) || ((xfer->rx_dma != INVALID_DMA_ADDRESS) && (xfer->rx_dma != 0))) {
            if (xfer->rx_dma & (SPI_4B_ALIGN - 1)) {
                SPI_ERR("Warning!Rx_DMA address should be 4Byte alignment,buf:%p,dma:%x\n",
                        xfer->rx_buf, xfer->rx_dma);
            }
            ispeed_single_write(base + SPI_REG_RX_DST, xfer->rx_dma);
            cmd |= 1 << SPI_CMD_RX_DMA_OFFSET;
        }
    }
    ispeed_single_write(base + SPI_REG_CMD, cmd);
}

static void  inline SpiResumeTransfer(uint32_t base)
{
    uint32_t reg_val;

    reg_val = ispeed_single_read(base + SPI_REG_CMD);
    reg_val &= ~SPI_CMD_RESUME_MASK;
    reg_val |= 1 << SPI_CMD_RESUME_OFFSET;
    /*All register must be prepared before setting the start bit [SMP]*/
    ispeed_single_write(base + SPI_REG_CMD, reg_val);

    return;
}

void ResetSpi(uint32_t base)
{
    uint32_t reg_val;

    /*set the software reset bit in SPI_REG_CMD.*/
    reg_val = ispeed_single_read(base + SPI_REG_CMD);
    reg_val &= ~SPI_CMD_RST_MASK;
    reg_val |= 1 << SPI_CMD_RST_OFFSET;
    ispeed_single_write(base + SPI_REG_CMD, reg_val);

    reg_val = ispeed_single_read(base + SPI_REG_CMD);
    reg_val &= ~SPI_CMD_RST_MASK;
    ispeed_single_write(base + SPI_REG_CMD, reg_val);

    SPI_MSG("ResetSpi!\n");

    return;
}

/*
* Write chip configuration to HW register
*/
int32_t SpiSetup(struct spi_transfer *xfer)
{
    uint32_t reg_val, base;
    struct mt_chip_conf *chip_config = xfer->chip_config;

    base = xfer->base;

    if (chip_config == NULL) {
        SPI_MSG("SpiSetup chip_config is NULL !!\n");
    } else {
        /*set the timing*/
        reg_val = ispeed_single_read(base + SPI_REG_CFG2);
        reg_val &= ~(SPI_CFG2_SCK_HIGH_MASK | SPI_CFG2_SCK_LOW_MASK);
        reg_val |= ((chip_config->high_time - 1) << SPI_CFG2_SCK_HIGH_OFFSET);
        reg_val |= ((chip_config->low_time - 1) << SPI_CFG2_SCK_LOW_OFFSET);
        ispeed_single_write(base + SPI_REG_CFG2, reg_val);

        reg_val = ispeed_single_read(base + SPI_REG_CFG0);
        reg_val &= ~(SPI_CFG0_CS_HOLD_MASK | SPI_CFG0_CS_SETUP_MASK);
        reg_val |= ((chip_config->holdtime - 1) << SPI_CFG0_CS_HOLD_OFFSET);
        reg_val |= ((chip_config->setuptime - 1) << SPI_CFG0_CS_SETUP_OFFSET);
        ispeed_single_write(base + SPI_REG_CFG0, reg_val);

        reg_val = ispeed_single_read(base + SPI_REG_CFG1);
        reg_val &= ~(SPI_CFG1_CS_IDLE_MASK);
        reg_val |= ((chip_config->cs_idletime - 1) << SPI_CFG1_CS_IDLE_OFFSET);

        reg_val &= ~(SPI_CFG1_GET_TICK_DLY_MASK);
        reg_val |= ((chip_config->tckdly) << SPI_CFG1_GET_TICK_DLY_OFFSET);
        ispeed_single_write(base + SPI_REG_CFG1, reg_val);

        /*config CFG1 bit[28:26] is 0*/
        reg_val = ispeed_single_read(base + SPI_REG_CFG1);
        reg_val &= ~(0x7 << 26);
        ispeed_single_write(base + SPI_REG_CFG1, reg_val);

        ispeed_single_write(base + SPI_REG_PAD_SEL, 0x0);

        /*set the mlsbx and mlsbtx*/
        reg_val = ispeed_single_read(base + SPI_REG_CMD);
        reg_val &= ~(SPI_CMD_TX_ENDIAN_MASK | SPI_CMD_RX_ENDIAN_MASK);
        reg_val &= ~(SPI_CMD_TXMSBF_MASK | SPI_CMD_RXMSBF_MASK);
        reg_val &= ~(SPI_CMD_CPHA_MASK | SPI_CMD_CPOL_MASK);
        reg_val &= ~(SPI_CMD_SAMPLE_SEL_MASK | SPI_CMD_CS_POL_MASK);
        reg_val |= (chip_config->tx_mlsb << SPI_CMD_TXMSBF_OFFSET);
        reg_val |= (chip_config->rx_mlsb << SPI_CMD_RXMSBF_OFFSET);
        reg_val |= (chip_config->tx_endian << SPI_CMD_TX_ENDIAN_OFFSET);
        reg_val |= (chip_config->rx_endian << SPI_CMD_RX_ENDIAN_OFFSET);
        reg_val |= (chip_config->cpha << SPI_CMD_CPHA_OFFSET);
        reg_val |= (chip_config->cpol << SPI_CMD_CPOL_OFFSET);
        reg_val |= (chip_config->sample_sel << SPI_CMD_SAMPLE_SEL_OFFSET);
        reg_val |= (chip_config->cs_pol << SPI_CMD_CS_POL_OFFSET);
        ispeed_single_write(base + SPI_REG_CMD, reg_val);

        /*set pause mode*/
#ifdef SPI_TRANSFER_POLLING
        reg_val = ispeed_single_read(base + SPI_REG_CMD);
        reg_val &= ~SPI_CMD_PAUSE_EN_MASK;
        reg_val &= ~ SPI_CMD_PAUSE_IE_MASK; /*disable pause IE in polling mode*/

        reg_val |= (chip_config->pause << SPI_CMD_PAUSE_EN_OFFSET);
        ispeed_single_write(base + SPI_REG_CMD, reg_val);
#else
        reg_val = ispeed_single_read(base + SPI_REG_CMD);
        reg_val &= ~SPI_CMD_PAUSE_EN_MASK;
        reg_val &= ~ SPI_CMD_PAUSE_IE_MASK;
        reg_val |= (chip_config->pause << SPI_CMD_PAUSE_EN_OFFSET);
        reg_val |= (chip_config->pause << SPI_CMD_PAUSE_IE_OFFSET);
        ispeed_single_write(base + SPI_REG_CMD, reg_val);
#endif

#ifdef SPI_TRANSFER_POLLING
        reg_val = ispeed_single_read(base + SPI_REG_CMD);
        reg_val &= ~ SPI_CMD_FINISH_IE_MASK;/*disable finish IE in polling mode*/
        ispeed_single_write(base + SPI_REG_CMD, reg_val);
#else
        /*set finish interrupt always enable*/
        reg_val = ispeed_single_read(base + SPI_REG_CMD);
        reg_val &= ~ SPI_CMD_FINISH_IE_MASK;
        reg_val |= (1 << SPI_CMD_FINISH_IE_OFFSET);
        ispeed_single_write(base + SPI_REG_CMD, reg_val);
#endif

        /*set the communication of mode*/
        reg_val = ispeed_single_read(base + SPI_REG_CMD);
        reg_val &= ~ SPI_CMD_TX_DMA_MASK;
        reg_val &= ~ SPI_CMD_RX_DMA_MASK;
        ispeed_single_write(base + SPI_REG_CMD, reg_val);

        /*set deassert mode*/
        reg_val = ispeed_single_read(base + SPI_REG_CMD);
        reg_val &= ~SPI_CMD_DEASSERT_MASK;
        reg_val |= (chip_config->deassert << SPI_CMD_DEASSERT_OFFSET);
        ispeed_single_write(base + SPI_REG_CMD, reg_val);
    }
    return 0;
}

#ifdef SPI_TRANSFER_POLLING
static void SpiRecvCheck(struct spi_transfer *xfer)
{
    uint32_t cnt, i;

    cnt = (xfer->len % 4) ? (xfer->len / 4 + 1) : (xfer->len / 4);
    for (i = 0; i < cnt; i++) {
        if (*((uint32_t *) xfer->rx_buf + i) != *((uint32_t *) xfer->tx_buf + i)) {
            SPI_ERR("Error!! tx xfer %d is:%x\n", i,  *((uint32_t *) xfer->tx_buf + i));
            SPI_ERR("Error!! rx xfer %d is:%x\n", i,  *((uint32_t *) xfer->rx_buf + i));
        }
    }
}

int32_t SSPMSpiPollingHandle(uint32_t spi_id, uint32_t spi_base)
{

    struct spi_transfer xfer_intr00;
    struct mt_chip_conf chip_config_intr00;
    struct spi_transfer *xfer_intr = &xfer_intr00;
    struct mt_chip_conf *chip_config_intr = &chip_config_intr00;
    uint32_t reg_val, cnt;
    uint32_t i;
    uint32_t base = spi_base;
    uint32_t id = spi_id;
    uint32_t ret = SPI_NO_ERROR;

    SPI_MSG(" SpiHandlePolling id:0x%x\n", id);

    xfer_intr = GetSpiTransfer(id);
    chip_config_intr = GetChipConfig(id);

    if (NULL == chip_config_intr) {
        SPI_ERR("chip_config is NULL!!\n");
        return -EINVAL;
    }

    /*pause mode*/
    if (chip_config_intr->pause) {
        if (GetPauseStatus(id) == INPROGRESS) {
            SetPauseStatus(id, PAUSED);
            SPI_MSG("IRQ set PAUSED state\n");
        } else {
            SPI_ERR("Wrong spi status.\n");
            return -EINVAL;
        }
    } else {
        SetPauseStatus(id, IDLE);
        SPI_MSG("IRQ set IDLE state\n");
    }

    SPI_MSG("start to read data !!\n");
    /*FIFO*/
    if ((chip_config_intr->com_mod == FIFO_TRANSFER) && xfer_intr->rx_buf) {
        SPI_MSG("xfer->len:%d\n", xfer_intr->len);
        cnt = (xfer_intr->len % 4) ? (xfer_intr->len / 4 + 1) : (xfer_intr->len / 4);
        for (i = 0; i < cnt; i++) {
            reg_val = ispeed_single_read(base + SPI_REG_RX_DATA); /*get the data from rx*/
            SPI_MSG("SPI_RX_DATA_REG:0x%x\n", reg_val);
            *((uint32_t *)xfer_intr->rx_buf + i) = reg_val;
        }
    }

    if (chip_config_intr->com_mod == DMA_TRANSFER)
        SpiRecvCheck(xfer_intr);

    if (xfer_intr->is_last_xfer == 1 && xfer_intr->is_transfer_end == 1) {
        SetPauseStatus(id, IDLE);
        ResetSpi(base);
        SPI_MSG("Pause set IDLE state & disable clk\n");
    }

    /*set irq flag to ask SpiNextMessage continue to run*/

    SetIrqFlag(id, IRQ_IDLE);

    SPI_MSG("SSPMSpiPollingHandle-----------xfer end-------\n");
    return ret;
}
#endif

int32_t SpiNextXfer(struct spi_transfer *xfer)
{
    uint32_t  mode, cnt, i;
    int ret = SPI_NO_ERROR;

    struct mt_chip_conf *chip_config = GetChipConfig(xfer->id);

    if (chip_config == NULL) {
        SPI_ERR("SpiNextXfer get chip_config is NULL\n");
        return -EINVAL;
    }

    if ((!IsInterruptEnable(xfer->base))) {
        SPI_MSG("interrupt is disable\n");
    }

    mode = chip_config->com_mod;

    if ((mode == FIFO_TRANSFER)) {
        if (xfer->len > SPI_FIFO_SIZE) {
            ret = -ELEN;
            SPI_ERR("xfer len is invalid over fifo size\n");
            goto fail;
        }
    }

    /*
       * cannot 1K align & FIFO->DMA need used pause mode
       * this is to clear pause bit (CS turn to idle after data transfer done)
    */
    if (mode == DMA_TRANSFER) {
        if ((xfer->is_last_xfer == 1) && (xfer->is_transfer_end == 1))
            clear_pause_bit(xfer->base);
    } else if (mode == FIFO_TRANSFER) {
        if (xfer->is_transfer_end == 1)
            clear_pause_bit(xfer->base);
    } else {
        SPI_ERR("xfer mode is invalid !\n");
        ret = -EMODE;
        goto fail;
    }

    //SetPauseStatus(IDLE); //runing status, nothing about pause mode
    //disable DMA
    SpiDisableDma(xfer->base);

    /*need setting transfer data length & loop count*/
    ret = SpiSetupPacket(xfer);

    /*Using FIFO to send data*/
    if ((mode == FIFO_TRANSFER)) {
        cnt = (xfer->len % 4) ? (xfer->len / 4 + 1) : (xfer->len / 4);
        for (i = 0; i < cnt; i++) {
            ispeed_single_write(xfer->base + SPI_REG_TX_DATA, *((uint32_t *) xfer->tx_buf + i));
            SPI_MSG("tx_buf data is:%x\n", *((uint32_t *) xfer->tx_buf + i));
            SPI_MSG("tx_buf addr is:%p\n", (uint32_t *)xfer->tx_buf + i);
        }
    }
    /*Using DMA to send data*/
    if ((mode == DMA_TRANSFER)) {
        SpiEnableDma(xfer, mode);
    }
    SetSpiTransfer(xfer);

    SPI_MSG("xfer->id = %d, running=%d.\n", xfer->id, GetPauseStatus(xfer->id));

    if (GetPauseStatus(xfer->id) == PAUSED) { //running
        SPI_MSG("pause status to resume.\n");
        SetPauseStatus(xfer->id, INPROGRESS);   //set running status
        SpiResumeTransfer(xfer->base);
    } else if (GetPauseStatus(xfer->id) == IDLE) {
        SPI_MSG("The xfer start\n");
        /*if there is only one transfer, pause bit should not be set.*/
        if ((chip_config->pause)) { // need to think whether is last  msg <&& !is_last_xfer(msg,xfer)>
            SPI_MSG("set pause mode.\n");
            SetPauseBit(xfer->base);
        }
        /*All register must be prepared before setting the start bit [SMP]*/

        SetPauseStatus(xfer->id, INPROGRESS);   //set running status
        SpiStartTransfer(xfer->base);
        SPI_MSG("SpiStartTransfer\n");
    } else {
        SPI_ERR("Wrong status\n");
        ret = -ESTATUS;
        goto fail;
    }
    dump_reg_config(xfer->base);

#ifdef SPI_TRANSFER_POLLING
    ret = SpiPollingHandler(xfer);
#endif

    /*exit pause mode*/
    if (GetPauseStatus(xfer->id) == PAUSED && xfer->is_last_xfer == 1) {
        ClearResumeBit(xfer->base);
    }
    return 0;
fail:
    SetPauseStatus(xfer->id, IDLE); //set running status
    ResetSpi(xfer->base);
    SetIrqFlag(xfer->id, IRQ_IDLE);
    return ret;
}

static int32_t SpiNextMessage(struct spi_transfer *xfer)
{
    int32_t ret = SPI_NO_ERROR;
    struct mt_chip_conf *chip_config;

    chip_config = GetChipConfig(xfer->id);
    if (chip_config == NULL) {
        SPI_ERR("SpiNextMessage get chip_config is NULL\n");
        return -EMODE;
    }
    SpiSetup(xfer);
    ret = SpiNextXfer(xfer);
    return ret;
}

int32_t SpiTransfer(struct spi_transfer *xfer)
{
    int32_t ret = SPI_NO_ERROR;
    //struct mt_chip_conf *chip_config;

    /*wait intrrupt had been clear*/
    while (IRQ_BUSY == GetIrqFlag(xfer->id)) {
        /*need a pause instruction to avoid unknow exception*/
        SPI_MSG("wait IRQ handle finish\n");
    }

    /*set flag to block next transfer*/
    SetIrqFlag(xfer->id, IRQ_BUSY);

    /*Through xfer->len to select DMA Or FIFO mode*/
#ifdef SPI_AUTO_SELECT_MODE
    if (xfer) {
        /*if transfer len > 32, used DMA mode*/
        if (xfer->len > SPI_DATA_SIZE) {
            xfer->chip_config = GetChipConfig(xfer->id);
            xfer->chip_config->com_mod = DMA_TRANSFER;
            SetChipConfig(xfer.id, xfer.chip_config);
            SPI_MSG("data size > 32, select DMA mode !\n");
        }
    } else {
        SPI_ERR("xfer is NULL pointer. \n");
    }
#endif
    if ((!xfer)) {
        SPI_ERR("xfer is NULL pointer. \n");
        goto out;
    }
    if ((NULL == xfer)) {
        SPI_ERR("the xfer is NULL.\n");
        goto out;
    }

    if (!((xfer->tx_buf || xfer->rx_buf) && xfer->len)) {
        SPI_ERR("missing tx %p or rx %p buf, len%d\n", xfer->tx_buf, xfer->rx_buf, xfer->len);
        goto out;
    }

    if (xfer) {
        ret = SpiNextMessage(xfer);
    }
    return ret;
out:
    return -EINVAL;

}

int32_t spi_handle(struct spi_transfer* spiData)
{
    int32_t ret = SPI_NO_ERROR;
    struct spi_transfer     xfer;
    struct spi_transfer    *xfer_p = &xfer;

    uint32_t packet_loop, rest_size;

    config_spi_base(spiData);

    xfer_p->tx_buf = spiData->tx_buf;
    xfer_p->rx_buf = spiData->rx_buf;

    xfer.is_last_xfer = 1;

    xfer.id = spiData->id;
    xfer.base = spiData->base;

    xfer.rx_buf = xfer_p->rx_buf;
    xfer.tx_buf = xfer_p->tx_buf;
    xfer.tx_dma = spiData->tx_dma;
    xfer.rx_dma = spiData->rx_dma;
    xfer.len = spiData->len;
    xfer.is_dma_used = spiData->is_dma_used;
    xfer.is_transfer_end = spiData->is_transfer_end;

    ret = Spi_enable_clk(xfer.id);
    if (ret != SPI_NO_ERROR)
        return ret;
    //ret = spi_gpio_set(xfer.id);
    //if (ret != SPI_NO_ERROR)
    //    return ret;

    SPI_MSG("tx_dma=0x%x,rx_dma=0x%x\n", xfer.tx_dma , xfer.rx_dma);
    SPI_MSG("xfer.len=%d, xfer.is_dma_used=%d\n", xfer.len, xfer.is_dma_used);
    SPI_MSG("IRQ status=%d\n", GetIrqFlag(xfer.id));
    while (IRQ_BUSY == GetIrqFlag(xfer.id)) {
        /*need a pause instruction to avoid unknow exception*/
        SPI_MSG("IPC wait IRQ handle finish\n");
    }

    xfer.chip_config = spiData->chip_config;

    SpiChipConfig(xfer.id, xfer.chip_config);


#ifndef NO_DMA_LENGTH_LIMIT
    packet_loop = spiData->len / 1024;
    rest_size = spiData->len % 1024;
    SPI_MSG("packet_loop=%d,rest_size=%d\n", packet_loop, rest_size);

    if (xfer.len <= 1024 || rest_size == 0) {
        SpiTransfer(&xfer);
    } else {
        xfer.chip_config->pause = PAUSE_MODE_ENABLE;
        SetChipConfig(xfer.id, xfer.chip_config);
        xfer.is_last_xfer = 0;
        xfer.len = 1024 * packet_loop;
        SpiTransfer(&xfer);

        xfer.chip_config->pause = PAUSE_MODE_DISABLE;
        SetChipConfig(xfer.id, xfer.chip_config);
        xfer.is_last_xfer = 1;
        xfer.len = rest_size;
        xfer.rx_buf = (uint32_t *)(xfer_p->rx_buf) + 1024 * packet_loop ;
        xfer.tx_buf = (uint32_t *)(xfer_p->tx_buf) + 1024 * packet_loop ;

        SpiTransfer(&xfer);
        xfer.rx_buf = xfer_p->rx_buf;
    }
#else
    SpiTransfer(&xfer);
#endif
    ret = Spi_disable_clk(xfer.id);
    if (ret != SPI_NO_ERROR)
        return ret;

    return ret;
}

int32_t SpiSend(struct spi_transfer* spiData)
{
    int32_t ret;
    if(is_in_isr()) {
        ret = spi_handle(spiData);
    }
    else {
        taskENTER_CRITICAL();
        ret = spi_handle(spiData);
        taskEXIT_CRITICAL();
    }
    return ret;
}

#if SPI_DMA
#define SPI_DATA_LENGTH     16642
static uint32_t spi_tx_buf[SPI_DATA_LENGTH];
static uint32_t spi_rx_buf[SPI_DATA_LENGTH];
#endif

int32_t SpiTest(void)
{
    uint32_t ret = SPI_NO_ERROR;
    struct spi_transfer xfer;

    uint32_t send_data = 0x12341234;
    uint32_t recv_data = 0;

    SPI_MSG("SpiTest Start\n");

    xfer.tx_buf = &send_data;
    xfer.rx_buf = &recv_data;
    xfer.len = 4;
    xfer.id = 8;
    xfer.is_dma_used = 0;
    xfer.is_transfer_end = 1;
    xfer.chip_config = &mt_chip_conf_fifo_test;
    ret = SpiSend(&xfer);
    if (ret != SPI_NO_ERROR)
        return ret;

    xfer.tx_buf = &send_data;
    xfer.rx_buf = &recv_data;
    xfer.len = 4;
    xfer.id = 9;
    xfer.is_dma_used = 0;
    xfer.is_transfer_end = 1;
    xfer.chip_config = &mt_chip_conf_fifo_test;
    ret = SpiSend(&xfer);
    SPI_MSG("SpiTest End!! ret = %d\n", ret);

    return ret;
}


