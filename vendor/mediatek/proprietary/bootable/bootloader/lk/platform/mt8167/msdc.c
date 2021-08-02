/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2010
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#define MSDC_DEBUG_KICKOFF

#include "msdc.h"
#define MSDC_USE_PATCH_BIT2_TURNING_WITH_ASYNC	1

#if defined(MMC_MSDC_DRV_CTP)
#include <common.h>
#include "api.h"        //For invocation cache_clean_invalidate()
#include "cache_api.h"  //For invocation cache_clean_invalidate()
#endif

#if defined(MMC_MSDC_DRV_LK)
#include <kernel/event.h>
#include <platform/mt_irq.h>
#endif

#if defined(MMC_MSDC_DRV_CTP)
#include "gpio.h"

#if defined(MSDC_USE_DCM)
#include "dcm.h"
#endif

#ifndef FPGA_PLATFORM
#include "pmic.h"
#include "clock_manager.h"
#endif
#endif

static int msdc_rsp[] = {
	0,  /* RESP_NONE */
	1,  /* RESP_R1 */
	2,  /* RESP_R2 */
	3,  /* RESP_R3 */
	4,  /* RESP_R4 */
	1,  /* RESP_R5 */
	1,  /* RESP_R6 */
	1,  /* RESP_R7 */
	7,  /* RESP_R1b */
};

static msdc_priv_t msdc_priv[MSDC_MAX_NUM];
static u32 hclks_msdc50[] = { 26000000, 400000000, 200000000, 156000000, 182000000,
                              156000000, 100000000, 624000000, 312000000
                            };
static u32 hclks_msdc30[] = { 26000000, 208000000, 188000000, 156000000,
                              94000000, 126000000, 126000000, 190000000
                            };


void msdc_dump_card_status(u32 card_status)
{
	static char *state[] = {
		"Idle",            /* 0 */
		"Ready",           /* 1 */
		"Ident",           /* 2 */
		"Stby",            /* 3 */
		"Tran",            /* 4 */
		"Data",            /* 5 */
		"Rcv",             /* 6 */
		"Prg",             /* 7 */
		"Dis",             /* 8 */
		"Ina",             /* 9 */
		"Sleep",           /* 10 */
		"Reserved",        /* 11 */
		"Reserved",        /* 12 */
		"Reserved",        /* 13 */
		"Reserved",        /* 14 */
		"I/O mode",        /* 15 */
	};
	if (card_status & R1_OUT_OF_RANGE)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Out of Range\n"));

	if (card_status & R1_ADDRESS_ERROR)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Address Error\n"));

	if (card_status & R1_BLOCK_LEN_ERROR)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Block Len Error\n"));

	if (card_status & R1_ERASE_SEQ_ERROR)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Erase Seq Error\n"));

	if (card_status & R1_ERASE_PARAM)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Erase Param\n"));

	if (card_status & R1_WP_VIOLATION)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] WP Violation\n"));

	if (card_status & R1_CARD_IS_LOCKED)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Card is Locked\n"));

	if (card_status & R1_LOCK_UNLOCK_FAILED)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Lock/Unlock Failed\n"));

	if (card_status & R1_COM_CRC_ERROR)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Command CRC Error\n"));

	if (card_status & R1_ILLEGAL_COMMAND)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Illegal Command\n"));

	if (card_status & R1_CARD_ECC_FAILED)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Card ECC Failed\n"));

	if (card_status & R1_CC_ERROR)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] CC Error\n"));

	if (card_status & R1_ERROR)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Error\n"));

	if (card_status & R1_UNDERRUN)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Underrun\n"));

	if (card_status & R1_OVERRUN)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Overrun\n"));

	if (card_status & R1_CID_CSD_OVERWRITE)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] CID/CSD Overwrite\n"));

	if (card_status & R1_WP_ERASE_SKIP)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] WP Eraser Skip\n"));

	if (card_status & R1_CARD_ECC_DISABLED)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Card ECC Disabled\n"));

	if (card_status & R1_ERASE_RESET)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Erase Reset\n"));

	if (card_status & R1_READY_FOR_DATA)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Ready for Data\n"));

	if (card_status & R1_SWITCH_ERROR)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Switch error\n"));

	if (card_status & R1_URGENT_BKOPS)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] Urgent background operations\n"));

	if (card_status & R1_APP_CMD)
		MSDC_TRC_PRINT(MSDC_DSTATUS,("\t[CARD_STATUS] App Command\n"));

	MSDC_TRC_PRINT(MSDC_INFO,("\t[CARD_STATUS] '%s' State\n",
	                          state[R1_CURRENT_STATE(card_status)]));
}

void msdc_dump_ocr_reg(u32 resp)
{
	if (resp & (1 << 7))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] Low Voltage Range\n"));
	if (resp & (1 << 15))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 2.7-2.8 volt\n"));
	if (resp & (1 << 16))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 2.8-2.9 volt\n"));
	if (resp & (1 << 17))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 2.9-3.0 volt\n"));
	if (resp & (1 << 18))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 3.0-3.1 volt\n"));
	if (resp & (1 << 19))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 3.1-3.2 volt\n"));
	if (resp & (1 << 20))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 3.2-3.3 volt\n"));
	if (resp & (1 << 21))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 3.3-3.4 volt\n"));
	if (resp & (1 << 22))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 3.4-3.5 volt\n"));
	if (resp & (1 << 23))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] 3.5-3.6 volt\n"));
	if (resp & (1 << 24)) {
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] Switching to 1.8V Accepted (S18A)\n"));
		if (MSDC_DREG)
			MSDC_TRC_PRINT(MSDC_INFO,("\t[OCR] Switching to 1.8V Accepted (S18A)\n"));
	}

	if (resp & (1 << 30))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] Card Capacity Status (CCS)\n"));
	if (resp & (1UL << 31))
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] Card Power Up Status (Idle)\n"));
	else
		MSDC_TRC_PRINT(MSDC_DREG,("\t[OCR] Card Power Up Status (Busy)\n"));
}

void msdc_dump_io_resp(u32 resp)
{
	u32 flags = (resp >> 8) & 0xFF;
	char *state[] = {"DIS", "CMD", "TRN", "RFU"};

	if (flags & (1 << 7))
		MSDC_TRC_PRINT(MSDC_STACK,("\t[IO] COM_CRC_ERR\n"));
	if (flags & (1 << 6))
		MSDC_TRC_PRINT(MSDC_STACK,("\t[IO] Illgal command\n"));
	if (flags & (1 << 3))
		MSDC_TRC_PRINT(MSDC_STACK,("\t[IO] Error\n"));
	if (flags & (1 << 2))
		MSDC_TRC_PRINT(MSDC_STACK,("\t[IO] RFU\n"));
	if (flags & (1 << 1))
		MSDC_TRC_PRINT(MSDC_STACK,("\t[IO] Function number error\n"));
	if (flags & (1 << 0))
		MSDC_TRC_PRINT(MSDC_STACK,("\t[IO] Out of range\n"));

	MSDC_TRC_PRINT(MSDC_STACK,("[IO] State: %s, Data:0x%x\n", state[(resp >> 12) & 0x3], resp & 0xFF));
}

void msdc_dump_rca_resp(u32 resp)
{
	u32 card_status = (((resp >> 15) & 0x1) << 23) |
	                  (((resp >> 14) & 0x1) << 22) |
	                  (((resp >> 13) & 0x1) << 19) |
	                  (resp & 0x1fff);

	MSDC_TRC_PRINT(MSDC_STACK,("\t[RCA] 0x%x\n", resp >> 16));
	msdc_dump_card_status(card_status);
}

static void msdc_dump_dbg_register(struct mmc_host *host)
{
	u32 base = host->base;
	u32 i;

	for (i = 0; i < 26; i++) {
		MSDC_WRITE32(MSDC_DBG_SEL, i);
		MSDC_TRC_PRINT(MSDC_HREG,("[SD%d]SW_DBG_SEL: write reg[%x] to 0x%x\n", host->id, OFFSET_MSDC_DBG_SEL, i));
		MSDC_TRC_PRINT(MSDC_HREG,("[SD%d]SW_DBG_OUT: read  reg[%x] to 0x%x\n",
		                          host->id, OFFSET_MSDC_DBG_OUT, MSDC_READ32(MSDC_DBG_OUT)));
	}

	MSDC_WRITE32(MSDC_DBG_SEL, 0);
}

void msdc_dump_register(struct mmc_host *host)
{
	u32 base = host->base;

	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_CFG       = 0x%x\n", host->id, OFFSET_MSDC_CFG,       MSDC_READ32(MSDC_CFG)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_IOCON     = 0x%x\n", host->id, OFFSET_MSDC_IOCON,     MSDC_READ32(MSDC_IOCON)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_PS        = 0x%x\n", host->id, OFFSET_MSDC_PS,        MSDC_READ32(MSDC_PS)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_INT       = 0x%x\n", host->id, OFFSET_MSDC_INT,       MSDC_READ32(MSDC_INT)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_INTEN     = 0x%x\n", host->id, OFFSET_MSDC_INTEN,     MSDC_READ32(MSDC_INTEN)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_FIFOCS    = 0x%x\n", host->id, OFFSET_MSDC_FIFOCS,    MSDC_READ32(MSDC_FIFOCS)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_TXDATA    = not read\n", host->id, OFFSET_MSDC_TXDATA));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_RXDATA    = not read\n", host->id, OFFSET_MSDC_RXDATA));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_CFG        = 0x%x\n", host->id, OFFSET_SDC_CFG,        MSDC_READ32(SDC_CFG)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_CMD        = 0x%x\n", host->id, OFFSET_SDC_CMD,        MSDC_READ32(SDC_CMD)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_ARG        = 0x%x\n", host->id, OFFSET_SDC_ARG,        MSDC_READ32(SDC_ARG)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_STS        = 0x%x\n", host->id, OFFSET_SDC_STS,        MSDC_READ32(SDC_STS)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_RESP0      = 0x%x\n", host->id, OFFSET_SDC_RESP0,      MSDC_READ32(SDC_RESP0)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_RESP1      = 0x%x\n", host->id, OFFSET_SDC_RESP1,      MSDC_READ32(SDC_RESP1)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_RESP2      = 0x%x\n", host->id, OFFSET_SDC_RESP2,      MSDC_READ32(SDC_RESP2)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_RESP3      = 0x%x\n", host->id, OFFSET_SDC_RESP3,      MSDC_READ32(SDC_RESP3)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_BLK_NUM    = 0x%x\n", host->id, OFFSET_SDC_BLK_NUM,    MSDC_READ32(SDC_BLK_NUM)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_VOL_CHG    = 0x%x\n", host->id, OFFSET_SDC_VOL_CHG,    MSDC_READ32(SDC_VOL_CHG)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_CSTS       = 0x%x\n", host->id, OFFSET_SDC_CSTS,       MSDC_READ32(SDC_CSTS)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_CSTS_EN    = 0x%x\n", host->id, OFFSET_SDC_CSTS_EN,    MSDC_READ32(SDC_CSTS_EN)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_DATCRC_STS = 0x%x\n", host->id, OFFSET_SDC_DCRC_STS,   MSDC_READ32(SDC_DCRC_STS)));

	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] EMMC_CFG0      = 0x%x\n", host->id, OFFSET_EMMC_CFG0,      MSDC_READ32(EMMC_CFG0)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] EMMC_CFG1      = 0x%x\n", host->id, OFFSET_EMMC_CFG1,      MSDC_READ32(EMMC_CFG1)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] EMMC_STS       = 0x%x\n", host->id, OFFSET_EMMC_STS,       MSDC_READ32(EMMC_STS)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] EMMC_IOCON     = 0x%x\n", host->id, OFFSET_EMMC_IOCON,     MSDC_READ32(EMMC_IOCON)));

	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_ACMD_RESP  = 0x%x\n", host->id, OFFSET_SDC_ACMD_RESP,  MSDC_READ32(SDC_ACMD_RESP)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_ACMD19_TRG = 0x%x\n", host->id, OFFSET_SDC_ACMD19_TRG, MSDC_READ32(SDC_ACMD19_TRG)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_ACMD19_STS = 0x%x\n", host->id, OFFSET_SDC_ACMD19_STS, MSDC_READ32(SDC_ACMD19_STS)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_DMA_HIGH4BIT = 0x%x\n", host->id, OFFSET_MSDC_DMA_HIGH4BIT, MSDC_READ32(MSDC_DMA_HIGH4BIT)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] DMA_SA         = 0x%x\n", host->id, OFFSET_MSDC_DMA_SA,    MSDC_READ32(MSDC_DMA_SA)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] DMA_CA         = 0x%x\n", host->id, OFFSET_MSDC_DMA_CA,    MSDC_READ32(MSDC_DMA_CA)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] DMA_CTRL       = 0x%x\n", host->id, OFFSET_MSDC_DMA_CTRL,  MSDC_READ32(MSDC_DMA_CTRL)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] DMA_CFG        = 0x%x\n", host->id, OFFSET_MSDC_DMA_CFG,   MSDC_READ32(MSDC_DMA_CFG)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SW_DBG_SEL     = 0x%x\n", host->id, OFFSET_MSDC_DBG_SEL,   MSDC_READ32(MSDC_DBG_SEL)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SW_DBG_OUT     = 0x%x\n", host->id, OFFSET_MSDC_DBG_OUT,   MSDC_READ32(MSDC_DBG_OUT)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_DMA_LEN   = 0x%x\n", host->id, OFFSET_MSDC_DMA_LEN,   MSDC_READ32(MSDC_DMA_LEN)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] PATCH_BIT0     = 0x%x\n", host->id, OFFSET_MSDC_PATCH_BIT0,MSDC_READ32(MSDC_PATCH_BIT0)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] PATCH_BIT1     = 0x%x\n", host->id, OFFSET_MSDC_PATCH_BIT1,MSDC_READ32(MSDC_PATCH_BIT1)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_PATCH_BIT2= 0x%x\n", host->id, OFFSET_MSDC_PATCH_BIT2,MSDC_READ32(MSDC_PATCH_BIT2)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] PAD_TUNE(0)    = 0x%x\n", host->id, OFFSET_MSDC_PAD_TUNE,  MSDC_READ32(MSDC_PAD_TUNE)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] PAD_TUNE1      = 0x%x\n", host->id, OFFSET_MSDC_PAD_TUNE1, MSDC_READ32(MSDC_PAD_TUNE1)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] DAT_RD_DLY0    = 0x%x\n", host->id, OFFSET_MSDC_DAT_RDDLY0,MSDC_READ32(MSDC_DAT_RDDLY0)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] DAT_RD_DLY1    = 0x%x\n", host->id, OFFSET_MSDC_DAT_RDDLY1,MSDC_READ32(MSDC_DAT_RDDLY1)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] DAT_RD_DLY2    = 0x%x\n", host->id, OFFSET_MSDC_DAT_RDDLY2,MSDC_READ32(MSDC_DAT_RDDLY2)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] DAT_RD_DLY3    = 0x%x\n", host->id, OFFSET_MSDC_DAT_RDDLY3,MSDC_READ32(MSDC_DAT_RDDLY3)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] HW_DBG_SEL     = 0x%x\n", host->id, OFFSET_MSDC_HW_DBG,    MSDC_READ32(MSDC_HW_DBG)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MAIN_VER       = 0x%x\n", host->id, OFFSET_MSDC_VERSION,   MSDC_READ32(MSDC_VERSION)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] MSDC_ECO_VER   = 0x%x\n", host->id, OFFSET_MSDC_ECO_VER,   MSDC_READ32(MSDC_ECO_VER)));
	MSDC_TRC_PRINT(MSDC_HREG,("[SD%d] Reg[%x] SDC_FIFO_CFG   = 0x%x\n", host->id, OFFSET_SDC_FIFO_CFG,   MSDC_READ32(SDC_FIFO_CFG)));

	msdc_dump_dbg_register(host);

}

#ifndef FPGA_PLATFORM
static void msdc_dump_clock_sts(struct mmc_host *host)
{
#ifdef MTK_MSDC_BRINGUP_DEBUG
	MSDC_DBG_PRINT(MSDC_PLL,(" msdcpll en[0x1000C250][bit0 should be 0x1]=0x%x\n", MSDC_READ32(0x1000C250)));
	MSDC_DBG_PRINT(MSDC_PLL,(" msdcpll on[0x1000C25C][bit0~1 should be 0x1]=0x%x\n", MSDC_READ32(0x1000C25C)));

	MSDC_DBG_PRINT(MSDC_PLL,(" pdn, mux[0x10000070][bit23, 16~19 for msdc0, bit31, 16~19 for msdc1]=0x%x\n", MSDC_READ32(0x10000070)));
#endif
}

static void msdc_dump_ldo_sts(struct mmc_host *host)
{
#ifdef MTK_MSDC_BRINGUP_DEBUG
	u32 ldo_en = 0, ldo_vol = 0;

	pwrap_read( 0x0A18, &ldo_en );  // MT6325_LDO_CON12, MT6325_PMIC_RG_VEMC33_EN_SHIFT
	pwrap_read( 0x0A4A, &ldo_vol ); // MT6325_LDO_VCON6, MT6325_PMIC_RG_VEMC_3V3_VOSEL_SHIFT
	MSDC_TRC_PRINT(MSDC_PMIC,(" VEMC_EN[0x0A18][bit1 should be 1]=0x%d, VEMC_VOL[0x0A4A][bit9 should be 1]=0x%x\n", ldo_en, ldo_vol));
#endif
}
#endif

#if defined(MMC_MSDC_DRV_CTP)
/* need reset some register while switch to hs400 mode with emmc50
 * do stress test need change mode from hs400 to others, so need backup if switched */
int msdc_register_partial_backup_and_reset(struct mmc_host* host)
{
	/*mt8167 eMMC don't support HS400 mode*/
	return 0;
}

int msdc_register_partial_restore(struct mmc_host* host)
{
	/*mt8167 eMMC don't support HS400 mode*/
	return 0;
}
#endif

static void msdc_dump_info(struct mmc_host *host)
{
	// 1: dump msdc hw register
	msdc_dump_register(host);

	// 2: For designer
	msdc_dump_dbg_register(host);

#ifndef FPGA_PLATFORM
	// 3: check msdc clock gate and clock source
	msdc_dump_clock_sts(host);

	// 4: check msdc pmic ldo
	msdc_dump_ldo_sts(host);
#endif
}

#ifndef FPGA_PLATFORM
void msdc_set_smt(struct mmc_host *host, int smt)
{
	switch (host->id) {
		case 0:
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_CLK_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_CMD_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_DAT0_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_DAT1_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_DAT2_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_DAT3_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_DAT4_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_DAT5_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_DAT6_MASK, smt);
			MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_DAT7_MASK, smt);
			break;
		case 1:
			MSDC_SET_FIELD(MSDC1_GPIO_SMT0_ADDR, MSDC1_SMT_CLK_MASK, smt);
			MSDC_SET_FIELD(MSDC1_GPIO_SMT0_ADDR, MSDC1_SMT_CMD_MASK, smt);
			MSDC_SET_FIELD(MSDC1_GPIO_SMT0_ADDR, MSDC1_SMT_DAT3_MASK, smt);
			MSDC_SET_FIELD(MSDC1_GPIO_SMT0_ADDR, MSDC1_SMT_DAT2_MASK, smt);
			MSDC_SET_FIELD(MSDC1_GPIO_SMT0_ADDR, MSDC1_SMT_DAT1_MASK, smt);
			MSDC_SET_FIELD(MSDC1_GPIO_SMT1_ADDR, MSDC1_SMT_DAT0_MASK, smt);
			break;
		case 2:
			MSDC_SET_FIELD(MSDC2_GPIO_SMT_ADDR, MSDC2_SMT_CLK_MASK, smt);
			MSDC_SET_FIELD(MSDC2_GPIO_SMT_ADDR, MSDC2_SMT_CMD_MASK, smt);
			MSDC_SET_FIELD(MSDC2_GPIO_SMT_ADDR, MSDC2_SMT_DAT3_MASK, smt);
			MSDC_SET_FIELD(MSDC2_GPIO_SMT_ADDR, MSDC2_SMT_DAT2_MASK, smt);
			MSDC_SET_FIELD(MSDC2_GPIO_SMT_ADDR, MSDC2_SMT_DAT1_MASK, smt);
			MSDC_SET_FIELD(MSDC2_GPIO_SMT_ADDR, MSDC2_SMT_DAT0_MASK, smt);
			break;
		default:
			break;
	}

}

void msdc_set_rdtdsel(struct mmc_host *host, bool sd_18)
{
	switch (host->id) {
		case 0: //always 1.8V
			MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL_CLK_MASK, 0xA);
			MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL_CMD_MASK, 0xA);
			MSDC_SET_FIELD(MSDC0_GPIO_TDSEL1_ADDR, MSDC0_TDSEL_DAT_MASK, 0xA);
			MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL_CLK_MASK, 0);
			MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL_CMD_MASK, 0);
			MSDC_SET_FIELD(MSDC0_GPIO_RDSEL1_ADDR, MSDC0_RDSEL_DAT_MASK, 0);
			break;
		case 1:
			MSDC_SET_FIELD(MSDC1_GPIO_TDSEL1_ADDR, MSDC1_TDSEL_CLK_MASK, 0xA);
			MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL_CMD_MASK, 0xA);
			MSDC_SET_FIELD(MSDC1_GPIO_TDSEL1_ADDR, MSDC1_TDSEL_DAT_MASK, 0xA);
			if (sd_18) {
				MSDC_SET_FIELD(MSDC1_GPIO_RDSEL1_ADDR, MSDC1_RDSEL_CLK_MASK, 0);
				MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL_CMD_MASK, 0);
				MSDC_SET_FIELD(MSDC1_GPIO_RDSEL1_ADDR, MSDC1_RDSEL_DAT_MASK, 0);
			} else {
				MSDC_SET_FIELD(MSDC1_GPIO_RDSEL1_ADDR, MSDC1_RDSEL_CLK_MASK, 0xC);
				MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL_CMD_MASK, 0xC);
				MSDC_SET_FIELD(MSDC1_GPIO_RDSEL1_ADDR, MSDC1_RDSEL_DAT_MASK, 0xC);
			}
			break;
		case 2:
			MSDC_SET_FIELD(MSDC2_GPIO_TDSEL1_ADDR, MSDC2_TDSEL_CLK_MASK, 0xA);
			MSDC_SET_FIELD(MSDC2_GPIO_TDSEL0_ADDR, MSDC2_TDSEL_CMD_MASK, 0xA);
			MSDC_SET_FIELD(MSDC2_GPIO_TDSEL1_ADDR, MSDC2_TDSEL_DAT_MASK, 0xA);
			if (sd_18) {
				MSDC_SET_FIELD(MSDC2_GPIO_RDSEL0_ADDR, MSDC2_RDSEL_CLK_MASK, 0);
				MSDC_SET_FIELD(MSDC2_GPIO_RDSEL0_ADDR, MSDC2_RDSEL_CMD_MASK, 0);
				MSDC_SET_FIELD(MSDC2_GPIO_RDSEL1_ADDR, MSDC2_RDSEL_DAT_MASK, 0);
			} else {
				MSDC_SET_FIELD(MSDC2_GPIO_RDSEL0_ADDR, MSDC2_RDSEL_CLK_MASK, 0xC);
				MSDC_SET_FIELD(MSDC2_GPIO_RDSEL0_ADDR, MSDC2_RDSEL_CMD_MASK, 0xC);
				MSDC_SET_FIELD(MSDC2_GPIO_RDSEL1_ADDR, MSDC2_RDSEL_DAT_MASK, 0xC);
			}
			break;
		default:
			break;
	}
}

/* pull up means that host driver the line to HIGH
 * pull down means that host driver the line to LOW */
static void msdc_pin_set(struct mmc_host *host, MSDC_PIN_STATE mode)
{
	/* driver CLK/DAT pin */
	MSDC_ASSERT(host);
	MSDC_ASSERT(mode < MSDC_PST_MAX);

	switch (host->id) {
		case 0:
			/*MSDC_SET_FIELD(MSDC0_GPIO_PUPD2_ADDR, MSDC0_PUPD_CLK_MASK, MSDC_PST_PD_50KOHM);*/
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD2_ADDR, MSDC0_PUPD_CMD_MASK, mode);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD_DAT0_MASK, mode);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD_DAT1_MASK, mode);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD_DAT2_MASK, mode);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD_DAT3_MASK, mode);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD1_ADDR, MSDC0_PUPD_DAT4_MASK, mode);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD1_ADDR, MSDC0_PUPD_DAT5_MASK, mode);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD1_ADDR, MSDC0_PUPD_DAT6_MASK, mode);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD1_ADDR, MSDC0_PUPD_DAT7_MASK, mode);
			break;
		case 1:
			/*MSDC_SET_FIELD(MSDC1_GPIO_PUPD1_ADDR, MSDC1_PUPD_CLK_MASK, MSDC_PST_PD_50KOHM);*/
			MSDC_SET_FIELD(MSDC1_GPIO_PUPD2_ADDR, MSDC1_PUPD_CMD_MASK, mode);
			MSDC_SET_FIELD(MSDC1_GPIO_PUPD0_ADDR, MSDC1_PUPD_DAT0_MASK, mode);
			MSDC_SET_FIELD(MSDC1_GPIO_PUPD1_ADDR, MSDC1_PUPD_DAT1_MASK, mode);
			MSDC_SET_FIELD(MSDC1_GPIO_PUPD1_ADDR, MSDC1_PUPD_DAT2_MASK, mode);
			MSDC_SET_FIELD(MSDC1_GPIO_PUPD1_ADDR, MSDC1_PUPD_DAT3_MASK, mode);
			break;
		case 2:
			/*MSDC_SET_FIELD(MSDC2_GPIO_PUPD1_ADDR, MSDC2_PUPD_CLK_MASK, MSDC_PST_PD_50KOHM);*/
			MSDC_SET_FIELD(MSDC2_GPIO_PUPD1_ADDR, MSDC2_PUPD_CMD_MASK, mode);
			MSDC_SET_FIELD(MSDC2_GPIO_PUPD0_ADDR, MSDC2_PUPD_DAT0_MASK, mode);
			MSDC_SET_FIELD(MSDC2_GPIO_PUPD0_ADDR, MSDC2_PUPD_DAT1_MASK, mode);
			MSDC_SET_FIELD(MSDC2_GPIO_PUPD0_ADDR, MSDC2_PUPD_DAT2_MASK, mode);
			MSDC_SET_FIELD(MSDC2_GPIO_PUPD1_ADDR, MSDC2_PUPD_DAT3_MASK, mode);
			break;
		default:
			break;
	}
}

/* host can modify from 0-7 */
void msdc_set_driving(struct mmc_host *host, struct msdc_cust *msdc_cap, bool sd_18)
{
	u32 clkdrv, cmddrv, datdrv;

	MSDC_ASSERT(host);
	MSDC_ASSERT(msdc_cap);
	MSDC_ASSERT(host->id < MSDC_MAX_NUM);

	if (host && msdc_cap) {
		clkdrv = (sd_18) ? msdc_cap->clk_18v_drv : msdc_cap->clk_drv;
		cmddrv = (sd_18) ? msdc_cap->cmd_18v_drv : msdc_cap->cmd_drv;
		datdrv = (sd_18) ? msdc_cap->dat_18v_drv : msdc_cap->dat_drv;

		switch (host->id) {
			case 0:
				MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV_CLK_MASK, clkdrv);
				MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV_CMD_MASK, cmddrv);
				MSDC_SET_FIELD(MSDC0_GPIO_DRV1_ADDR, MSDC0_DRV_DAT_MASK, datdrv);
				break;
			case 1:
				MSDC_SET_FIELD(MSDC1_GPIO_DRV1_ADDR, MSDC1_DRV_CLK_MASK, clkdrv);
				MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV_CMD_MASK, cmddrv);
				MSDC_SET_FIELD(MSDC1_GPIO_DRV1_ADDR, MSDC1_DRV_DAT_MASK, datdrv);
				break;
			case 2:
				MSDC_SET_FIELD(MSDC2_GPIO_DRV1_ADDR, MSDC2_DRV_CLK_MASK, clkdrv);
				MSDC_SET_FIELD(MSDC2_GPIO_DRV0_ADDR, MSDC2_DRV_CMD_MASK, cmddrv);
				MSDC_SET_FIELD(MSDC2_GPIO_DRV1_ADDR, MSDC2_DRV_DAT_MASK, datdrv);
				break;
			default:
				break;
		}
	}
}

#if defined(MMC_MSDC_DRV_CTP)
void msdc_get_driving(struct mmc_host *host,struct msdc_cust *msdc_cap, bool sd_18)
{
	u32 clkdrv=0, cmddrv=0, datdrv=0;

	MSDC_ASSERT(host);
	MSDC_ASSERT(msdc_cap);
	MSDC_ASSERT(host->id < MSDC_MAX_NUM);

	if (host && msdc_cap) {
		switch (host->id) {
			case 0:
				MSDC_GET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV_CLK_MASK, clkdrv);
				MSDC_GET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV_CMD_MASK, cmddrv);
				MSDC_GET_FIELD(MSDC0_GPIO_DRV1_ADDR, MSDC0_DRV_DAT_MASK, datdrv);
				break;
			case 1:
				MSDC_GET_FIELD(MSDC1_GPIO_DRV1_ADDR, MSDC1_DRV_CLK_MASK, clkdrv);
				MSDC_GET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV_CMD_MASK, cmddrv);
				MSDC_GET_FIELD(MSDC1_GPIO_DRV1_ADDR, MSDC1_DRV_DAT_MASK, datdrv);
				break;
			case 2:
				MSDC_GET_FIELD(MSDC2_GPIO_DRV1_ADDR, MSDC2_DRV_CLK_MASK, clkdrv);
				MSDC_GET_FIELD(MSDC2_GPIO_DRV0_ADDR, MSDC2_DRV_CMD_MASK, cmddrv);
				MSDC_GET_FIELD(MSDC2_GPIO_DRV1_ADDR, MSDC2_DRV_DAT_MASK, datdrv);
				break;
			default:
				break;
		}

		if (sd_18) {
			msdc_cap->clk_18v_drv = clkdrv;
			msdc_cap->cmd_18v_drv = cmddrv;
			msdc_cap->dat_18v_drv = datdrv;
		} else {
			msdc_cap->clk_drv = clkdrv;
			msdc_cap->cmd_drv = cmddrv;
			msdc_cap->dat_drv = datdrv;
		}
	}

}
#endif
#endif

#if defined(MMC_MSDC_DRV_CTP)
#ifndef FPGA_PLATFORM
void msdc_pmic_VEMC_3V3_sel(MT65XX_POWER_VOLTAGE volt)
{
	if (volt == VOL_3000)    { mt6325_upmu_set_rg_vemc_3v3_vosel(0);}
	else if (volt == VOL_3300)    { mt6325_upmu_set_rg_vemc_3v3_vosel(1);}
	else { MSDC_ERR_PRINT(MSDC_ERROR,("Not support to Set VEMC_3V3 power to %d\n", volt));}
}


void msdc_pmic_VMC_sel(MT65XX_POWER_VOLTAGE volt)
{
	if (volt == VOL_3300) { mt6325_upmu_set_rg_vmc_vosel(1);}
	else if (volt == VOL_1800) { mt6325_upmu_set_rg_vmc_vosel(0);}
	else { MSDC_ERR_PRINT(MSDC_ERROR,("Not support to Set VMC1 power to %d\n", volt));}
}

void msdc_pmic_VMCH_sel(MT65XX_POWER_VOLTAGE volt)
{
	if (volt == VOL_3000)  { mt6325_upmu_set_rg_vmch_vosel(0);}
	else if (volt == VOL_3300)  { mt6325_upmu_set_rg_vmch_vosel(1);}
	else { MSDC_ERR_PRINT(MSDC_ERROR,("Not support to Set VMCH1 power to %d\n", volt));}
}

u32 hwPowerOn(MT65XX_POWER powerId, MT65XX_POWER_VOLTAGE powerVolt)
{
	switch (powerId) {
		case MT6325_POWER_LDO_VEMC33:
			msdc_pmic_VEMC_3V3_sel(powerVolt);
			mt6325_upmu_set_rg_vemc33_en(1);
			break;
		case MT6325_POWER_LDO_VMC:
			msdc_pmic_VMC_sel(powerVolt);
			mt6325_upmu_set_rg_vmc_en(1);
			break;
		case MT6325_POWER_LDO_VMCH:
			msdc_pmic_VMCH_sel(powerVolt);
			mt6325_upmu_set_rg_vmch_en(1);
			break;
		default:
			MSDC_ERR_PRINT(MSDC_ERROR,("Not support to Set %d power on\n", powerId));
			break;
	}

	mdelay(100); /* requires before voltage stable */

	return 0;
}

u32 hwPowerDown(MT65XX_POWER powerId)
{
	switch (powerId) {
		case MT6325_POWER_LDO_VEMC33:
			mt6325_upmu_set_rg_vemc33_en(0);
			break;
		case MT6325_POWER_LDO_VMC:
			mt6325_upmu_set_rg_vmc_en(0);
			break;
		case MT6325_POWER_LDO_VMCH:
			mt6325_upmu_set_rg_vmch_en(0);
			break;
		default:
			MSDC_ERR_PRINT(MSDC_ERROR,("Not support to Set %d power down\n", powerId));
			break;
	}
	return 0;
}

static u32 msdc_ldo_power(u32 on, MT65XX_POWER powerId, MT65XX_POWER_VOLTAGE powerVolt, u32 *status)
{
	if (on) { // want to power on
		if (*status == 0) {  // can power on
			MSDC_TRC_PRINT(MSDC_PMIC,("msdc LDO<%d> power on<%d>\n", powerId, powerVolt));
			hwPowerOn(powerId, powerVolt);
			*status = powerVolt;
		} else if (*status == powerVolt) {
			MSDC_TRC_PRINT(MSDC_PMIC,("msdc LDO<%d><%d> power on again!\n", powerId, powerVolt));
		} else { // for sd3.0 later
			MSDC_TRC_PRINT(MSDC_PMIC,("msdc LDO<%d> change<%d> to <%d>\n", powerId, *status, powerVolt));
			hwPowerDown(powerId);
			hwPowerOn(powerId, powerVolt);
			*status = powerVolt;
		}
	} else {  // want to power off
		if (*status != 0) {  // has been powerred on
			MSDC_TRC_PRINT(MSDC_PMIC,("msdc LDO<%d> power off\n", powerId));
			hwPowerDown(powerId);
			*status = 0;
		} else {
			MSDC_ERR_PRINT(MSDC_ERROR,("LDO<%d> not power on\n", powerId));
		}
	}

	return 0;
}
#endif /* end of FPGA_PLATFORM */
#endif /* end of MMC_MSDC_DRV_CTP */

void msdc_clock(struct mmc_host *host, int on)
{
	/* emmc/scard clock is on default */
	MSDC_TRC_PRINT(MSDC_INIT,("[SD%d] Turn %s %s clock \n", host->id, on ? "on" : "off", "host"));
}

void msdc_clr_fifo(struct mmc_host *host)
{
	u32 base = host->base;
	MSDC_CLR_FIFO();
}

void msdc_reset(struct mmc_host *host)
{
	u32 base = host->base;
	MSDC_RESET();
}

void msdc_abort(struct mmc_host *host)
{
	u32 base = host->base;

	MSDC_TRC_PRINT(MSDC_WARN,("[SD%d] Abort: MSDC_FIFOCS=%xh MSDC_PS=%xh SDC_STS=%xh\n",
	                          host->id, MSDC_READ32(MSDC_FIFOCS), MSDC_READ32(MSDC_PS), MSDC_READ32(SDC_STS)));

	/* reset controller */
	msdc_reset(host);

	/* clear fifo */
	msdc_clr_fifo(host);

	/* make sure txfifo and rxfifo are empty */
	if (MSDC_TXFIFOCNT() != 0 || MSDC_RXFIFOCNT() != 0) {
		MSDC_TRC_PRINT(MSDC_WARN,("[SD%d] Abort: TXFIFO(%d), RXFIFO(%d) != 0\n",
		                          host->id, MSDC_TXFIFOCNT(), MSDC_RXFIFOCNT()));
	}

	/* clear all interrupts */
	MSDC_WRITE32(MSDC_INT, MSDC_READ32(MSDC_INT));
}

#ifdef FPGA_PLATFORM
#define PWR_GPIO            (0x10001E84)
#define PWR_GPIO_EO         (0x10001E88)

#define PWR_MASK_EN         (0x1 << 8)
#define PWR_MASK_VOL_18     (0x1 << 9)
#define PWR_MASK_VOL_33     (0x1 << 10)
#define PWR_MASK_L4         (0x1 << 11)
#define PWR_MSDC_DIR        (PWR_MASK_EN | PWR_MASK_VOL_18 | PWR_MASK_VOL_33 | PWR_MASK_L4)

#define MSDC0_PWR_MASK_EN         (0x1 << 12)
#define MSDC0_PWR_MASK_VOL_18     (0x1 << 13)
#define MSDC0_PWR_MASK_VOL_33     (0x1 << 14)
#define MSDC0_PWR_MASK_L4         (0x1 << 15)
#define MSDC0_PWR_MSDC_DIR        (MSDC0_PWR_MASK_EN | MSDC0_PWR_MASK_VOL_18 | MSDC0_PWR_MASK_VOL_33 | MSDC0_PWR_MASK_L4)

static void msdc_clr_gpio(u32 bits)
{
	u32 l_val = 0;

	switch (bits) {
		case MSDC0_PWR_MASK_EN:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("clr card pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (MSDC0_PWR_MASK_EN),0);
			MSDC_SET_FIELD(PWR_GPIO_EO, (MSDC0_PWR_MASK_EN),0);
			break;
		case MSDC0_PWR_MASK_VOL_18:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("clr card 1.8v pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (MSDC0_PWR_MASK_VOL_18|MSDC0_PWR_MASK_VOL_33), 0);
			MSDC_SET_FIELD(PWR_GPIO_EO, (MSDC0_PWR_MASK_VOL_18|MSDC0_PWR_MASK_VOL_33), 0);
			break;
		case MSDC0_PWR_MASK_VOL_33:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("clr card 3.3v pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (MSDC0_PWR_MASK_VOL_18|MSDC0_PWR_MASK_VOL_33), 0);
			MSDC_SET_FIELD(PWR_GPIO_EO, (MSDC0_PWR_MASK_VOL_18|MSDC0_PWR_MASK_VOL_33), 0);
			break;
		case MSDC0_PWR_MASK_L4:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set l4 dir:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    MSDC0_PWR_MASK_L4, 0);
			MSDC_SET_FIELD(PWR_GPIO_EO, MSDC0_PWR_MASK_L4, 0);
			break;
		case PWR_MASK_EN:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("clr card pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    PWR_MASK_EN,0);
			MSDC_SET_FIELD(PWR_GPIO_EO, PWR_MASK_EN,0);
			break;
		case PWR_MASK_VOL_18:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("clr card 1.8v pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (PWR_MASK_VOL_18|PWR_MASK_VOL_33), 0);
			MSDC_SET_FIELD(PWR_GPIO_EO, (PWR_MASK_VOL_18|PWR_MASK_VOL_33), 0);
			break;
		case PWR_MASK_VOL_33:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("clr card 3.3v pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (PWR_MASK_VOL_18|PWR_MASK_VOL_33), 0);
			MSDC_SET_FIELD(PWR_GPIO_EO, (PWR_MASK_VOL_18|PWR_MASK_VOL_33), 0);
			break;
		case PWR_MASK_L4:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set l4 dir:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    PWR_MASK_L4, 0);
			MSDC_SET_FIELD(PWR_GPIO_EO, PWR_MASK_L4, 0);
			break;
		default:
			MSDC_ERR_PRINT(MSDC_ERROR,("[%s:%d]invalid value: 0x%x\n", __FILE__, __func__, bits));
			break;
	}

#ifdef ___MSDC_DEBUG___
	MSDC_TRC_PRINT(MSDC_PMIC,("[clr]PWR_GPIO[8-16]:0x%x\n", MSDC_READ32(PWR_GPIO)));
	MSDC_TRC_PRINT(MSDC_PMIC,("[clr]GPIO_DIR[8-16]:0x%x\n", MSDC_READ32(PWR_GPIO_EO)));
#endif
}

static void msdc_set_gpio(u32 bits)
{
	u32 l_val = 0;

	switch (bits) {
		case MSDC0_PWR_MASK_EN:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set card pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    MSDC0_PWR_MASK_EN,1);
			MSDC_SET_FIELD(PWR_GPIO_EO, MSDC0_PWR_MASK_EN,1);
			break;
		case MSDC0_PWR_MASK_VOL_18:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set card 1.8v pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (MSDC0_PWR_MASK_VOL_18|MSDC0_PWR_MASK_VOL_33), 1);
			MSDC_SET_FIELD(PWR_GPIO_EO, (MSDC0_PWR_MASK_VOL_18|MSDC0_PWR_MASK_VOL_33), 1);
			break;
		case MSDC0_PWR_MASK_VOL_33:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set card 3.3v pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (MSDC0_PWR_MASK_VOL_18|MSDC0_PWR_MASK_VOL_33), 2);
			MSDC_SET_FIELD(PWR_GPIO_EO, (MSDC0_PWR_MASK_VOL_18|MSDC0_PWR_MASK_VOL_33), 2);
			break;
		case MSDC0_PWR_MASK_L4:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set l4 dir:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    MSDC0_PWR_MASK_L4, 1);
			MSDC_SET_FIELD(PWR_GPIO_EO, MSDC0_PWR_MASK_L4, 1);
			break;
		case PWR_MASK_EN:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set card pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    PWR_MASK_EN,1);
			MSDC_SET_FIELD(PWR_GPIO_EO, PWR_MASK_EN,1);
			break;
		case PWR_MASK_VOL_18:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set card 1.8v pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (PWR_MASK_VOL_18|PWR_MASK_VOL_33), 1);
			MSDC_SET_FIELD(PWR_GPIO_EO, (PWR_MASK_VOL_18|PWR_MASK_VOL_33), 1);
			break;
		case PWR_MASK_VOL_33:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set card 3.3v pwr:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    (PWR_MASK_VOL_18|PWR_MASK_VOL_33), 2);
			MSDC_SET_FIELD(PWR_GPIO_EO, (PWR_MASK_VOL_18|PWR_MASK_VOL_33), 2);
			break;
		case PWR_MASK_L4:
			/* check for set before */
			MSDC_TRC_PRINT(MSDC_PMIC,("set l4 dir:\n"));
			MSDC_SET_FIELD(PWR_GPIO,    PWR_MASK_L4, 1);
			MSDC_SET_FIELD(PWR_GPIO_EO, PWR_MASK_L4, 1);
			break;
		default:
			MSDC_ERR_PRINT(MSDC_ERROR,("[%s:%s]invalid value: 0x%x\n", __FILE__, __func__, bits));
			break;
	}

#ifdef ___MSDC_DEBUG___
	MSDC_TRC_PRINT(MSDC_PMIC,("[clr]PWR_GPIO[8-16]:0x%x\n", MSDC_READ32(PWR_GPIO)));
	MSDC_TRC_PRINT(MSDC_PMIC,("[clr]GPIO_DIR[8-16]:0x%x\n", MSDC_READ32(PWR_GPIO_EO)));
#endif

}

void msdc_set_card_pwr(struct mmc_host *host, int on)
{

	if (on) {
		switch (host->id) {
			case 0:
				msdc_set_gpio(MSDC0_PWR_MASK_VOL_18);
				msdc_set_gpio(MSDC0_PWR_MASK_L4);
				msdc_set_gpio(MSDC0_PWR_MASK_EN);
				break;
			case 1:
			case 2:
				msdc_set_gpio(PWR_MASK_VOL_33);
				msdc_set_gpio(PWR_MASK_L4);
				msdc_set_gpio(PWR_MASK_EN);
				break;
			default:
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s:%s] power on, invalid host id: %d\n", __FILE__, __func__, host->id));
				break;
		}
	} else {
		switch (host->id) {
			case 0:
				msdc_clr_gpio(MSDC0_PWR_MASK_EN);
				msdc_clr_gpio(MSDC0_PWR_MASK_VOL_18);
				msdc_clr_gpio(MSDC0_PWR_MASK_L4);
				break;
			case 1:
			case 2:
				msdc_clr_gpio(PWR_MASK_EN);
				msdc_clr_gpio(PWR_MASK_VOL_33);
				msdc_clr_gpio(PWR_MASK_L4);
				break;
			default:
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s:%s] power off, invalid host id: %d\n", __FILE__, __func__, host->id));
				break;
		}
	}
	mdelay(10);
}
#endif

void msdc_config_pin(struct mmc_host *host, int mode)
{
	MSDC_TRC_PRINT(MSDC_GPIO,("[SD%d] Pins mode(%d), none(0), down(1), up(2), keep(3)\n",
	                          host->id, mode));

	MSDC_TRC_PRINT(MSDC_GPIO,("[SD%d] Pins mode(%d), none(0), down(1), up(2), keep(3)\n",
	                          host->id, mode));

#ifndef FPGA_PLATFORM
	switch (mode) {
		case MSDC_PIN_PULL_UP:
			msdc_pin_set(host, MSDC_PST_PU_10KOHM);
			break;

		case MSDC_PIN_PULL_DOWN:
			msdc_pin_set(host, MSDC_PST_PD_50KOHM);
			break;

		case MSDC_PIN_PULL_NONE:
		default:
			msdc_pin_set(host, MSDC_PST_PD_50KOHM);
			break;
	}
#endif
}

void msdc_set_axi_burst_len(struct mmc_host *host, u8 len)
{
}

void msdc_set_axi_outstanding(struct mmc_host *host, u8 rw, u8 num)
{
}

void msdc_set_startbit(struct mmc_host *host, u8 start_bit)
{
	u32 base = host->base;
	u32 l_start_bit;
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;

	if (host->id != 3) {
		return;
	}

	/* set start bit */
	MSDC_SET_FIELD(MSDC_CFG, MSDC_CFG_START_BIT, start_bit);
	priv->start_bit = start_bit;

	MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_START_BIT, l_start_bit);
#if 1
	switch (l_start_bit) {
		case 0:
			MSDC_DBG_PRINT(MSDC_DEBUG,("[info][%s %d] read data start bit at rising edge\n", __func__, __LINE__));
			break;
		case 1:
			MSDC_DBG_PRINT(MSDC_DEBUG,("[info][%s %d] read data start bit at falling edge\n", __func__, __LINE__));
			break;
		case 2:
			MSDC_DBG_PRINT(MSDC_DEBUG,("[info][%s %d] read data start bit at rising & falling edge\n", __func__, __LINE__));
			break;
		case 3:
			MSDC_DBG_PRINT(MSDC_DEBUG,("[info][%s %d] read data start bit at rising | falling edge\n", __func__, __LINE__));
			break;
		default:
			break;
	}
#endif
}

void msdc_set_smpl(struct mmc_host *host, u8 HS400, u8 mode, u8 type)
{
	u32 base = host->base;
	int i=0;
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;

	static u8 read_data_edge[8] = {MSDC_SMPL_RISING, MSDC_SMPL_RISING, MSDC_SMPL_RISING, MSDC_SMPL_RISING,
	                               MSDC_SMPL_RISING, MSDC_SMPL_RISING, MSDC_SMPL_RISING, MSDC_SMPL_RISING
	                              };
	static u8 write_data_edge[4] = {MSDC_SMPL_RISING, MSDC_SMPL_RISING, MSDC_SMPL_RISING, MSDC_SMPL_RISING};

	switch (type) {
		case TYPE_CMD_RESP_EDGE:
			if (mode == MSDC_SMPL_RISING || mode == MSDC_SMPL_FALLING) {
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, mode);
				priv->rsmpl = mode;
			} else {
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: SD%d invalid resp parameter: HS400=%d, type=%d, mode=%d\n", __func__, host->id, HS400, type, mode));
			}
			break;

		case TYPE_WRITE_CRC_EDGE:
			if (mode == MSDC_SMPL_RISING || mode == MSDC_SMPL_FALLING) {
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL_SEL, 0);
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL, mode);
				priv->wdsmpl = mode;
			} else if (mode == MSDC_SMPL_SEPERATE && !HS400) {
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D0SPL, write_data_edge[0]); //only dat0 is for write crc status.
				priv->wdsmpl = mode;
			} else {
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: SD%d invalid crc parameter: HS400=%d, type=%d, mode=%d\n", __func__, host->id, HS400, type, mode));
			}
			break;

		case TYPE_READ_DATA_EDGE:
			if (HS400) {
				msdc_set_startbit(host, START_AT_RISING_AND_FALLING); //for HS400, start bit is output both on rising and falling edge
				priv->start_bit = START_AT_RISING_AND_FALLING;
			} else {
				msdc_set_startbit(host, START_AT_RISING); //for the other mode, start bit is only output on rising edge. but DDR50 can try falling edge if error casued by pad delay
				priv->start_bit = START_AT_RISING;
			}
			if (mode == MSDC_SMPL_RISING || mode == MSDC_SMPL_FALLING) {
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL_SEL, 0);
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, mode);
				priv->rdsmpl = mode;
			} else if (mode == MSDC_SMPL_SEPERATE) {
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL_SEL, 1);
				for (i=0; i<8; i++);
				{
					MSDC_SET_FIELD(MSDC_IOCON, (MSDC_IOCON_R_D0SPL << i), read_data_edge[i]);
				}
				priv->rdsmpl = mode;
			} else {
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: SD%d invalid read parameter: HS400=%d, type=%d, mode=%d\n", __func__, host->id, HS400, type, mode));
			}
			break;

		case TYPE_WRITE_DATA_EDGE:
			if (mode == MSDC_SMPL_RISING|| mode == MSDC_SMPL_FALLING) {
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL_SEL, 0);
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL, mode);
				priv->wdsmpl = mode;
			} else if (mode == MSDC_SMPL_SEPERATE) {
				MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL_SEL, 1);
				for (i=0; i<4; i++);
				{
					MSDC_SET_FIELD(MSDC_IOCON, (MSDC_IOCON_W_D0SPL << i), write_data_edge[i]);//dat0~4 is for SDIO card.
				}
				priv->wdsmpl = mode;
			} else {
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: SD%d invalid write parameter: HS400=%d, type=%d, mode=%d\n", __func__, host->id, HS400, type, mode));
			}
			break;

		default:
			MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: SD%d invalid parameter: HS400=%d, type=%d, mode=%d\n", __func__, host->id, HS400, type, mode));
			break;
	}
}

static u32 msdc_cal_timeout(struct mmc_host *host, u64 ns, u32 clks, u32 clkunit)
{
	u32 timeout, clk_ns;

	clk_ns  = 1000000000UL / host->cur_bus_clk;
	timeout = ns / clk_ns + clks;
	timeout = timeout / clkunit;
	return timeout;
}

void msdc_set_timeout(struct mmc_host *host, u64 ns, u32 clks)
{
	u32 base = host->base;
	u32 timeout, clk_ns;
	u32 mode = 0;

	if (host->cur_bus_clk == 0) {
		timeout = 0;
	} else {
		clk_ns  = 1000000000UL / host->cur_bus_clk;
		timeout = (ns + clk_ns - 1) / clk_ns + clks;
		timeout = (timeout + (1 << 20) - 1) >> 20; /* in 1048576 sclk cycle unit */
		MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, mode);
		timeout = mode >= 2 ? timeout * 2 : timeout; //DDR mode will double the clk cycles for data timeout
		timeout = timeout > 1 ? timeout - 1 : 0;
		timeout = timeout > 255 ? 255 : timeout;
	}
	MSDC_SET_FIELD(SDC_CFG, SDC_CFG_DTOC, timeout);

	MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] Set read data timeout: %dns %dclks -> %d x 1048576 cycles, mode:%d, clk_freq=%dKHz\n",
	                           host->id, ns, clks, timeout + 1, mode, (host->cur_bus_clk / 1000)));
}

void msdc_set_blklen(struct mmc_host *host, u32 blklen)
{
	//u32 base = host->base;
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;

	host->blklen     = blklen;
	priv->cfg.blklen = blklen;
}

void msdc_set_blknum(struct mmc_host *host, u32 blknum)
{
	u32 base = host->base;
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;

	/* autocmd23 with packed cmd, this feature is conflict with data tag, reliable write, and force flush cache */
#if defined(MMC_MSDC_DRV_CTP)
	if (priv->autocmd & MSDC_AUTOCMD23) {
#if MSDC_USE_DATA_TAG
		blknum |= (1 << 29);
		blknum &= ~(1 << 30);
#endif
#if MSDC_USE_RELIABLE_WRITE
		blknum |= (1 << 31);
		blknum &= ~(1 << 30);
#endif
#if MSDC_USE_FORCE_FLUSH
		blknum |= (1 << 24);
		blknum &= ~(1 << 30);
#endif
#if MSDC_USE_PACKED_CMD
		blknum &= ~0xffff;
		blknum |= (1 << 30);
#endif
	}
#endif
	if (priv->cmd23_flags & MSDC_RELIABLE_WRITE) {
		blknum |= (1 << 31);
		blknum &= ~(1 << 30);
	}

	MSDC_WRITE32(SDC_BLK_NUM, blknum);
}

void msdc_set_dmode(struct mmc_host *host, int mode)
{
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;
	//u32 base = host->base;
#if defined(MSDC_ENABLE_DMA_MODE)
	priv->cfg.mode = mode;
#endif

	if (mode == MSDC_MODE_PIO) {
		host->blk_read  = msdc_pio_bread;
		host->blk_write = msdc_pio_bwrite;
#if defined(MSDC_ENABLE_DMA_MODE)
	} else {
		host->blk_read  = msdc_dma_bread;
		host->blk_write = msdc_dma_bwrite;
#endif
	}
	MSDC_TRC_PRINT(MSDC_INFO, ("Transfer method: %s \n", (mode == MSDC_MODE_PIO)?"PIO" : "DMA Relations"));
}

void msdc_set_pio_bits(struct mmc_host *host, int bits)
{
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;

	priv->pio_bits = bits;
}

void msdc_set_autocmd(struct mmc_host *host, int cmd, int on)
{
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;

	if (on) {
		priv->autocmd |= cmd;
	} else {
		priv->autocmd &= ~cmd;
	}
}

void msdc_set_reliable_write(struct mmc_host *host, int on)
{
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;

	if (on) {
		priv->cmd23_flags |= MSDC_RELIABLE_WRITE;
	} else {
		priv->cmd23_flags &= ~MSDC_RELIABLE_WRITE;
	}
}

void msdc_set_autocmd23_feature(struct mmc_host *host, int on)
{
	u32 base = host->base;
	if (on) {
		MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_BLKNUM_SEL, 0);
	} else {
		MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_BLKNUM_SEL, 1);
	}
}

int msdc_send_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;
	u32 base   = host->base;
	u32 opcode = cmd->opcode;
	u32 rsptyp = cmd->rsptyp;
	u32 rawcmd;
	u32 timeout = cmd->timeout;
	u32 error = MMC_ERR_NONE;

	/* rawcmd :
	 * vol_swt << 30 | auto_cmd << 28 | blklen << 16 | go_irq << 15 |
	 * stop << 14 | rw << 13 | dtype << 11 | rsptyp << 7 | brk << 6 | opcode
	 */
	rawcmd = (opcode & ~(SD_CMD_BIT | SD_CMD_APP_BIT)) |
	         msdc_rsp[rsptyp] << 7 | host->blklen << 16;

	if (opcode == MMC_CMD_WRITE_MULTIPLE_BLOCK) {
		rawcmd |= ((2 << 11) | (1 << 13));
		if (priv->autocmd & MSDC_AUTOCMD12) {
			rawcmd |= (1 << 28);
		} else if (priv->autocmd & MSDC_AUTOCMD23) {
			rawcmd |= (2 << 28);
		}
	} else if (opcode == MMC_CMD_WRITE_BLOCK || opcode == MMC_CMD50) {
		rawcmd |= ((1 << 11) | (1 << 13));
	} else if (opcode == MMC_CMD_READ_MULTIPLE_BLOCK) {
		rawcmd |= (2 << 11);
		if (priv->autocmd & MSDC_AUTOCMD12) {
			rawcmd |= (1 << 28);
		} else if (priv->autocmd & MSDC_AUTOCMD23) {
			rawcmd |= (2 << 28);
		}
	} else if (opcode == MMC_CMD_READ_SINGLE_BLOCK ||
	           opcode == SD_ACMD_SEND_SCR ||
	           opcode == SD_CMD_SWITCH ||
	           opcode == MMC_CMD_SEND_EXT_CSD ||
	           opcode == MMC_CMD_SEND_WRITE_PROT ||
	           opcode == MMC_CMD_SEND_WRITE_PROT_TYPE ||
	           opcode == MMC_CMD21) {
		rawcmd |= (1 << 11);
	} else if (opcode == MMC_CMD_STOP_TRANSMISSION) {
		rawcmd |= (1 << 14);
		rawcmd &= ~(0x0FFF << 16);
	} else if (opcode == SD_IO_RW_EXTENDED) {
		if (cmd->arg & 0x80000000)  /* R/W flag */
			rawcmd |= (1 << 13);
		if ((cmd->arg & 0x08000000) && ((cmd->arg & 0x1FF) > 1))
			rawcmd |= (2 << 11); /* multiple block mode */
		else
			rawcmd |= (1 << 11);
	} else if (opcode == SD_IO_RW_DIRECT) {
		if ((cmd->arg & 0x80000000) && ((cmd->arg >> 9) & 0x1FFFF))/* I/O abt */
			rawcmd |= (1 << 14);
	} else if (opcode == SD_CMD_VOL_SWITCH) {
		rawcmd |= (1 << 30);
	} else if (opcode == SD_CMD_SEND_TUNING_BLOCK) {
		rawcmd |= (1 << 11); /* CHECKME */
		if (priv->autocmd & MSDC_AUTOCMD19)
			rawcmd |= (3 << 28);
	} else if (opcode == MMC_CMD_GO_IRQ_STATE) {
		rawcmd |= (1 << 15);
	} else if (opcode == MMC_CMD_WRITE_DAT_UNTIL_STOP) {
		rawcmd |= ((1<< 13) | (3 << 11));
	} else if (opcode == MMC_CMD_READ_DAT_UNTIL_STOP) {
		rawcmd |= (3 << 11);
	}

	MSDC_TRC_PRINT(MSDC_STACK, ("+[SD%d] CMD(%d): ARG(0x%x), RAW(0x%x), BLK_NUM(0x%x) RSP(%d)\n",
	                            host->id, (opcode & ~(SD_CMD_BIT | SD_CMD_APP_BIT)), cmd->arg, rawcmd, MSDC_READ32(SDC_BLK_NUM), rsptyp));

	if (opcode == MMC_CMD_SEND_STATUS) {
		if (SDC_IS_CMD_BUSY()) {
			WAIT_COND(!SDC_IS_CMD_BUSY(), cmd->timeout, timeout);
			if (timeout == 0) {
				error = MMC_ERR_TIMEOUT;
				MSDC_ERR_PRINT(MSDC_ERROR,("-[SD%d] CMD(%d): SDC_IS_CMD_BUSY timeout\n",
				                           host->id, (opcode & ~(SD_CMD_BIT | SD_CMD_APP_BIT))));
				goto end;
			}
		}
	} else {
		if (SDC_IS_BUSY()) {
			WAIT_COND(!SDC_IS_BUSY(), 1000, timeout);
			if (timeout == 0) {
				error = MMC_ERR_TIMEOUT;
				MSDC_ERR_PRINT(MSDC_ERROR,("-[SD%d] CMD(%d): SDC_IS_BUSY timeout\n",
				                           host->id, (opcode & ~(SD_CMD_BIT | SD_CMD_APP_BIT))));
				goto end;
			}
		}
	}

	SDC_SEND_CMD(rawcmd, cmd->arg);

end:
	cmd->error = error;

	return error;
}

int msdc_wait_rsp(struct mmc_host *host, struct mmc_command *cmd)
{
	u32 base   = host->base;
	u32 rsptyp = cmd->rsptyp;
	u32 status;
	u32 opcode = (cmd->opcode & ~(SD_CMD_BIT | SD_CMD_APP_BIT));
	u32 error = MMC_ERR_NONE;
	u32 wints = MSDC_INT_CMDTMO | MSDC_INT_CMDRDY | MSDC_INT_RSPCRCERR |
	            MSDC_INT_ACMDRDY | MSDC_INT_ACMDCRCERR | MSDC_INT_ACMDTMO |
	            MSDC_INT_ACMD19_DONE;
	u32 *resp = &cmd->resp[0];

	if (cmd->opcode == MMC_CMD_GO_IRQ_STATE)
		wints |= MSDC_INT_MMCIRQ;

	status = msdc_intr_wait(host, wints);
#if defined(FEATURE_MMC_SDIO)
	if (status & MSDC_INT_SDIOIRQ) {
		if (mmc_card_sdio(host->card)) {
			struct sdio_func *func = host->card->io_func[0];
			if (func->irq_handler)
				func->irq_handler(func);
		}
	}
#endif
	if (status == 0) {
		error = MMC_ERR_TIMEOUT;
		goto end;
	}

	if (   (status & MSDC_INT_CMDRDY)
	        || (status & MSDC_INT_ACMDRDY) ||
	        (status & MSDC_INT_ACMD19_DONE)) {

		switch (rsptyp) {
			case RESP_NONE:
				MSDC_TRC_PRINT(MSDC_STACK, ("-[SD%d] CMD(%d): RSP(%d)\n", host->id, opcode, rsptyp));
				break;

			case RESP_R2:
				*resp++ = MSDC_READ32(SDC_RESP3);
				*resp++ = MSDC_READ32(SDC_RESP2);
				*resp++ = MSDC_READ32(SDC_RESP1);
				*resp++ = MSDC_READ32(SDC_RESP0);
				MSDC_TRC_PRINT(MSDC_STACK, ("-[SD%d] CMD(%d): RSP(%d) = 0x%x 0x%x 0x%x 0x%x\n",
				                            host->id, opcode, cmd->rsptyp, cmd->resp[0], cmd->resp[1], cmd->resp[2], cmd->resp[3]));
				break;

			default: /* Response types 1, 3, 4, 5, 6, 7(1b) */
				if (   (status & MSDC_INT_ACMDRDY)
				        || (status & MSDC_INT_ACMD19_DONE)) {
					cmd->resp[0] = MSDC_READ32(SDC_ACMD_RESP);
				} else {
					cmd->resp[0] = MSDC_READ32(SDC_RESP0);
				}
				MSDC_TRC_PRINT(MSDC_STACK, ("-[SD%d] CMD(%d): RSP(%d) = 0x%x AUTO(%d)\n", host->id, opcode,
				                            cmd->rsptyp, cmd->resp[0],
				                            ((status & MSDC_INT_ACMDRDY) || (status & MSDC_INT_ACMD19_DONE)) ? 1 : 0));
				break;
		}
	} else if ((status & MSDC_INT_RSPCRCERR) || (status & MSDC_INT_ACMDCRCERR)) {
		error = MMC_ERR_BADCRC;
		MSDC_TRC_PRINT(MSDC_STACK, ("-[SD%d] CMD(%d): RSP(%d) ERR(BADCRC)\n",
		                            host->id, opcode, cmd->rsptyp));
	} else if ((status & MSDC_INT_CMDTMO) || (status & MSDC_INT_ACMDTMO)) {
		error = MMC_ERR_TIMEOUT;
		MSDC_TRC_PRINT(MSDC_STACK, ("-[SD%d] CMD(%d): RSP(%d) ERR(CMDTO) AUTO(%d)\n",
		                            host->id, opcode, cmd->rsptyp, status & MSDC_INT_ACMDTMO ? 1: 0));
	} else {
		error = MMC_ERR_INVALID;
		MSDC_ERR_PRINT(MSDC_ERROR, ("-[SD%d] CMD(%d): RSP(%d) ERR(INVALID), Status:%x\n",
		                            host->id, opcode, cmd->rsptyp, status));
	}
end:

	if (rsptyp == RESP_R1B) {
		while ((MSDC_READ32(MSDC_PS) & MSDC_PS_DAT0) != MSDC_PS_DAT0);
	}

	if (error == MMC_ERR_NONE) {
		switch (cmd->rsptyp) {
			case RESP_R1:
			case RESP_R1B:
				msdc_dump_card_status(cmd->resp[0]);
				break;
			case RESP_R3:
				msdc_dump_ocr_reg(cmd->resp[0]);
				break;
			case RESP_R5:
				msdc_dump_io_resp(cmd->resp[0]);
				break;
			case RESP_R6:
				msdc_dump_rca_resp(cmd->resp[0]);
				break;
		}
	}

	cmd->error = error;
	if (cmd->opcode == MMC_CMD_APP_CMD && error == MMC_ERR_NONE) {
		host->app_cmd = 1;
		host->app_cmd_arg = cmd->arg;
	} else {
		host->app_cmd = 0;
	}

	return error;
}


int msdc_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
	int err;

	err = msdc_send_cmd(host, cmd);
	if (err != MMC_ERR_NONE)
		return err;

	err = msdc_wait_rsp(host, cmd);

	if (err == MMC_ERR_BADCRC && cmd->opcode != MMC_CMD21) {
		u32 base = host->base;
		u32 tmp = MSDC_READ32(SDC_CMD);

		/* check if data is used by the command or not */
		if (tmp & SDC_CMD_DTYP) {
			msdc_abort_handler(host, 1);
		}

#if defined(FEATURE_MMC_CM_TUNING)
		//Light: For CMD17/18/24/25, tuning may have been done by
		//       msdc_abort_handler()->msdc_get_card_status()->msdc_cmd() for CMD13->msdc_tune_cmdrsp().
		//       This means that 2nd invocation of msdc_tune_cmdrsp() occurs here!
		//--> To Do: consider if 2nd invocation can be avoid
		if ( host->app_cmd!=2 ) { //Light 20121225, to prevent recursive call path: msdc_tune_cmdrsp->msdc_app_cmd->msdc_cmd->msdc_tune_cmdrsp
			err = msdc_tune_cmdrsp(host, cmd);
			if (err != MMC_ERR_NONE) {
				MSDC_ERR_PRINT(MSDC_ERROR,("[Err handle][%s:%d]tune cmd fail\n", __func__, __LINE__));
			}
		}
#endif
	}
	return err;
}

int msdc_cmd_stop(struct mmc_host *host, struct mmc_command *cmd)
{
	struct mmc_command stop;

	if (mmc_card_mmc(host->card) && (cmd) && (cmd->opcode == 18))
		stop.rsptyp  = RESP_R1;
	else
		stop.rsptyp  = RESP_R1B;
	stop.opcode  = MMC_CMD_STOP_TRANSMISSION;
	stop.arg     = 0;
	stop.retries = CMD_RETRIES;
	stop.timeout = CMD_TIMEOUT;

	return msdc_cmd(host, &stop);
}

static int msdc_get_card_status(struct mmc_host *host, u32 *status)
{
	int err;
	struct mmc_command cmd;

	cmd.opcode  = MMC_CMD_SEND_STATUS;
	cmd.arg     = host->card->rca << 16;
	cmd.rsptyp  = RESP_R1;
	cmd.retries = CMD_RETRIES;
	cmd.timeout = CMD_TIMEOUT;

	err = msdc_cmd(host, &cmd);

	if (err == MMC_ERR_NONE) {
		*status = cmd.resp[0];
	}
	return err;
}
void msdc_abort_handler(struct mmc_host *host, int abort_card)
{
	//Copy from BROM-Light version
	//u32 base = host->base;
	u32 status = 0;
	u32 state = 0;
	u32 err;
	//u32 count=0;

	while (state != 4) { // until status to "tran"; //20130125 Comment out by Light
		//while ( abort_card ) { //20130125 Light
		msdc_abort(host);
		err=msdc_get_card_status(host, &status);
		//To do: move the following 2 if clause into msdc_get_card_status() or write as a function
#if 0 //Light: turn if off before I verify it
		//#if defined(MMC_MSDC_DRV_CTP)
		if (err == MMC_ERR_BADCRC) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[Err handle][%s:%d]cmd13 crc error\n", __func__, __LINE__));
			msdc_tune_update_cmdrsp(host, count++);

			if (count >= 512)
				count = 0;
		}

		if (err == MMC_ERR_TIMEOUT) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[Err handle][%s:%d]cmd13 timeout\n", __func__, __LINE__));
			msdc_tune_update_cmdrsp(host, count++);

			if (count >= 512)
				count = 0;
		}
#else
		if (err != MMC_ERR_NONE) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[Err handle][%s:%d]cmd13 fail\n", __func__, __LINE__));
			goto out;
		}
#endif

		state = R1_CURRENT_STATE(status);
		mmc_dump_card_status(status);

		MSDC_DBG_PRINT(MSDC_DEBUG,("check card state<%d>\n", state));
		if (state == 5 || state == 6) {
			if (abort_card) {
				MSDC_DBG_PRINT(MSDC_DEBUG,("state<%d> need cmd12 to stop\n", state));

				err=msdc_cmd_stop(host, NULL);

				//To do: move the following 2 if clause into msdc_cmd_stop() or write as a function
#if 0 //Light: turn if off before I verify it
				//#if defined(MMC_MSDC_DRV_CTP)
				if (err == MMC_ERR_BADCRC) {
					MSDC_ERR_PRINT(MSDC_ERROR,("[Err handle][%s:%d]cmd12 crc error\n", __func__, __LINE__));
					msdc_tune_update_cmdrsp(host, count++);

					if (count >= 512)
						count = 0;

					continue;
				}

				if (err == MMC_ERR_TIMEOUT) {
					MSDC_ERR_PRINT(MSDC_ERROR,("[Err handle][%s:%d]cmd12 timeout\n", __func__, __LINE__));
					msdc_tune_update_cmdrsp(host, count++);

					if (count >= 512)
						count = 0;

					continue;
				}
#else
				if (err != MMC_ERR_NONE) {
					MSDC_ERR_PRINT(MSDC_ERROR,("[Err handle][%s:%d]cmd12 fail\n", __func__, __LINE__));
					goto out;
				}
#endif
			}
			//break;  //20130125 Light
		} else if (state == 7) {  // busy in programing
			MSDC_DBG_PRINT(MSDC_DEBUG,("state<%d> card is busy\n", state));
			mdelay(100);
		} else if (state != 4) {
			MSDC_DBG_PRINT(MSDC_DEBUG,("state<%d> ??? \n", state));
			goto out;
		}
	}

	msdc_abort(host);

	return;

out:
	MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] data abort failed\n",host->id));
}

void msdc_intr_unmask(struct mmc_host *host, u32 bits)
{
	u32 base = host->base;
	u32 val;

	val  = MSDC_READ32(MSDC_INTEN);
	val |= bits;
	MSDC_WRITE32(MSDC_INTEN, val);
}

void msdc_intr_mask(struct mmc_host *host, u32 bits)
{
	u32 base = host->base;
	u32 val;

	val  = MSDC_READ32(MSDC_INTEN);
	val &= ~bits;
	MSDC_WRITE32(MSDC_INTEN, val);
}

static int msdc_app_cmd(struct mmc_host *host)
{
	struct mmc_command appcmd;
	int err = MMC_ERR_NONE;
	int retries = 10;
	appcmd.opcode = MMC_CMD_APP_CMD;
	appcmd.arg = host->app_cmd_arg;
	appcmd.rsptyp  = RESP_R1;
	appcmd.retries = CMD_RETRIES;
	appcmd.timeout = CMD_TIMEOUT;

	do {
		err = msdc_cmd(host, &appcmd);
		if (err == MMC_ERR_NONE)
			break;
	} while (retries--);
	return err;
}

#if defined(MSDC_ENABLE_DMA_MODE)
int msdc_dma_send_sandisk_fwid(struct mmc_host *host, uchar *buf,u32 opcode, ulong nblks)
{
	//int multi;
	struct mmc_command cmd;
	struct mmc_data data;

	MSDC_ASSERT(nblks <= host->max_phys_segs);

	//MSG(OPS, "[SD%d] Read data %d blks from 0x%x\n", host->id, nblks, src);

	//multi = nblks > 1 ? 1 : 0;

	/* send read command */
	cmd.opcode  = opcode;
	cmd.rsptyp  = RESP_R1;
	cmd.arg     = 0;  //src;
	cmd.retries = 0;
	cmd.timeout = CMD_TIMEOUT;

	data.blks    = nblks;
	data.buf     = (u8*)buf;
	data.timeout = 100; /* 100ms */

	return msdc_dma_transfer(host, &cmd, &data);
}
#endif

void msdc_brk_cmd(struct mmc_host *host)
{
	u32 base = host->base;
	u32 tmo =0;

	WAIT_COND(SDC_IS_BUSY() == 0, tmo, tmo);

	if (tmo == 0) {
		MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: SDC BUSY timeout happend, before send break cmd\n", __func__));
	}

	SDC_SEND_CMD(0x000000e8, 0);
}

int msdc_pio_read(struct mmc_host *host, u32 *ptr, u32 size)
{
	int err = MMC_ERR_NONE;
#if defined(MMC_MSDC_DRV_CTP)
	u8 *ptr8;
	u16 *ptr16;
#endif
	msdc_priv_t *priv = host->priv;
	u32 base = host->base;
	u32 ints = MSDC_INT_DATCRCERR | MSDC_INT_DATTMO | MSDC_INT_XFER_COMPL;
	//u32 timeout = 100000;
	u32 status;
	u32 totalsz = size;
	u8  done = 0;
	u32 size_per_round;
	u32 dcrc;
	u8* u8ptr;

#if defined(MMC_MSDC_DRV_CTP)
	if (priv->pio_bits == 16)
		ptr16 = (u16 *) ptr;
	else if (priv->pio_bits == 8)
		ptr8 = (u8 *) ptr;
#endif

	while (1) {
#if defined(MSDC_USE_IRQ)
		//For CTP only
		DisableIRQ();
		status = msdc_irq_sts[host->id];
		msdc_irq_sts[host->id] &= ~ints;
		EnableIRQ();
#else
		status = MSDC_READ32(MSDC_INT);
		MSDC_WRITE32(MSDC_INT, status);
#if defined(FEATURE_MMC_SDIO)
		if (status & MSDC_INT_SDIOIRQ) {
			MSDC_DBG_PRINT(MSDC_DEBUG,("(%s)INT status:0x%x\n", __func__, status));
			if (host->id == 2) {
				mmc_sdio_proc_pending_irqs(host->card);
				//sdio_read_pending_irq(host->card->io_func[0]);
			}
		}
#endif
#endif

		if (status & ~ints) {
			MSDC_DBG_PRINT(MSDC_WARN,("[SD%d]<CHECKME> Unexpected INT(0x%x)\n",
			                          host->id, status));
		}
		if (status & MSDC_INT_DATCRCERR) {
			MSDC_GET_FIELD(SDC_DCRC_STS, SDC_DCRC_STS_POS|SDC_DCRC_STS_NEG, dcrc);
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] DAT CRC error (0x%x), Left:%d/%d bytes, RXFIFO:%d,dcrc:0x%x\n",
			                           host->id, status, size, totalsz, MSDC_RXFIFOCNT(),dcrc));
			err = MMC_ERR_BADCRC;
			break;
		} else if (status & MSDC_INT_DATTMO) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] DAT TMO error (0x%x), Left: %d/%d bytes, RXFIFO:%d\n",
			                           host->id, status, size, totalsz, MSDC_RXFIFOCNT()));
			err = MMC_ERR_TIMEOUT;
			break;
		} else if (status & MSDC_INT_ACMDCRCERR) {
			MSDC_GET_FIELD(SDC_DCRC_STS, SDC_DCRC_STS_POS|SDC_DCRC_STS_NEG, dcrc);
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] AUTOCMD CRC error (0x%x), Left:%d/%d bytes, RXFIFO:%d,dcrc:0x%x\n",
			                           host->id, status, size, totalsz, MSDC_RXFIFOCNT(),dcrc));
			err = MMC_ERR_ACMD_RSPCRC;
			break;
		} else if (status & MSDC_INT_XFER_COMPL) {
			done = 1;
		}

		if (size == 0 && done)
			break;

		/* Note. RXFIFO count would be aligned to 4-bytes alignment size */
		//if ((size >=  MSDC_FIFO_THD) && (MSDC_RXFIFOCNT() >= MSDC_FIFO_THD))
		if (size > 0) {
			int left;
			if ( (size >= MSDC_FIFO_THD) && (MSDC_RXFIFOCNT() >= MSDC_FIFO_THD) )
				left = MSDC_FIFO_THD;
			else if ( (size < MSDC_FIFO_THD) && (MSDC_RXFIFOCNT() >= size) )
				left = size;
			else
				continue;

			size_per_round = left;

#if defined(MMC_MSDC_DRV_CTP)
			if (priv->pio_bits == 8) {
				do {
					*ptr8++ = MSDC_FIFO_READ8();;
					left--;
				} while (left);

			} else if (priv->pio_bits == 16) {
				do {
					if (left> 1) {
						*ptr16++ = MSDC_FIFO_READ16();
						left-=2;
					} else {
						u8ptr = (u8*)ptr;
						while (left--) {
							*u8ptr++ = MSDC_FIFO_READ8();
						}
					}
				} while (left);

			} else
#endif
			{
				//if (priv->pio_bits==32 )
				do {
					if (left> 3) {
						*ptr++ = MSDC_FIFO_READ32();
						left-=4;
					} else {
						u8ptr = (u8*)ptr;
						while (left--) {
							*u8ptr++ = MSDC_FIFO_READ8();
						}
					}
				} while (left);
			}
			size -= size_per_round;

			MSDC_DBG_PRINT(MSDC_PIO,("[SD%d] Read %d bytes, RXFIFOCNT: %d,  Left: %d/%d\n",
			                         host->id, size_per_round, MSDC_RXFIFOCNT(), size, totalsz));
		}
	}

	if (err != MMC_ERR_NONE) {
		msdc_abort(host); /* reset internal fifo and state machine */
		MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] %d-bit PIO Read Error (%d)\n", host->id,
		                           priv->pio_bits, err));
	}

	return err;
}

int msdc_pio_write(struct mmc_host *host, u32 *ptr, u32 size)
{
	int err = MMC_ERR_NONE;
	u8 *ptr8=(u8 *)ptr;
	u32 base = host->base;
	u32 ints = MSDC_INT_DATCRCERR | MSDC_INT_DATTMO | MSDC_INT_XFER_COMPL;
	//u32 timeout = 250000;
	u32 status;
#if defined(MMC_MSDC_DRV_CTP)
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;
#endif
	u32 size_per_round;

	while (1) {
#if defined(MSDC_USE_IRQ)
		//For CTP only
		DisableIRQ();
		status = msdc_irq_sts[host->id];
		msdc_irq_sts[host->id] &= ~ints;
		EnableIRQ();
#else
		status = MSDC_READ32(MSDC_INT);
		MSDC_WRITE32(MSDC_INT, status);
#if defined(FEATURE_MMC_SDIO)
		if (status & MSDC_INT_SDIOIRQ) {
			MSDC_DBG_PRINT(MSDC_DEBUG,("(%s)INT status:0x%x\n", __func__, status));
			if ( (host->id == 2) || (host->id == 1) ) {
				mmc_sdio_proc_pending_irqs(host->card);
				//sdio_read_pending_irq(host->card->io_func[0]);
			}
		}
#endif
#endif
		if (status & ~ints) {
			MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d]<CHECKME> Unexpected INT(0x%x)\n",
			                           host->id, status));
		}
		if (status & MSDC_INT_DATCRCERR) {
			MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] DAT CRC error (0x%x), Left DAT: %d bytes\n",
			                           host->id, status, size));
			err = MMC_ERR_BADCRC;
			break;
		} else if (status & MSDC_INT_DATTMO) {
			MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] DAT TMO error (0x%x), Left DAT: %d bytes, MSDC_FIFOCS=%xh\n",
			                           host->id, status, size, MSDC_READ32(MSDC_FIFOCS)));
			err = MMC_ERR_TIMEOUT;
			break;
		} else if (status & MSDC_INT_ACMDCRCERR) {
			MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] AUTO CMD CRC error (0x%x), Left DAT: %d bytes\n",
			                           host->id, status, size));
			err = MMC_ERR_ACMD_RSPCRC;
			break;
		} else if (status & MSDC_INT_XFER_COMPL) {
			if (size == 0) {
				MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] all data flushed to card\n", host->id));
				break;
			} else {
				MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d]<CHECKME> XFER_COMPL before all data written\n",
				                           host->id));
			}
		}

		if (size == 0)
			continue;


		if (MSDC_TXFIFOCNT() == 0) {

			int left;
#if defined(MMC_MSDC_DRV_CTP)
			if ( priv->pio_bits==32 ) {
				if ( size >= MSDC_FIFO_THD )
					left = MSDC_FIFO_THD;
				else
					left = size;
			} else
#endif
			{
				if ( size >= MSDC_FIFO_SZ )
					left = MSDC_FIFO_SZ;
				else
					left = size;
			}

			size_per_round = left;


#if defined(MMC_MSDC_DRV_CTP)
			if (priv->pio_bits == 8) {
				do {
					MSDC_FIFO_WRITE8(*ptr8);
					ptr8++;
					left--;
				} while (left);
			} else if (priv->pio_bits == 16) {
				do {
					if (left > 1) {
						MSDC_FIFO_WRITE16(*(u16*)ptr8);
						ptr8+=2;
						left-=2;
					} else {
						while (left--) {
							MSDC_FIFO_WRITE8(*ptr8);
							ptr8++;
						}
					}
				} while (left);
			} else
#endif
			{
				//if ( write_unit==4 )
				do {
					if (left > 3) {
						MSDC_FIFO_WRITE32(*(u32*)ptr8);
						ptr8+=4;
						left-=4;
					} else {
						while (left--) {
							MSDC_FIFO_WRITE8(*ptr8);
							ptr8++;
						}
					}
				} while (left);
			}

			size -= size_per_round;
		}
	}

	if (err != MMC_ERR_NONE) {
		msdc_abort(host); /* reset internal fifo and state machine */
		MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] PIO Write Error (%d)\n", host->id, err));
	}

	return err;
}

int msdc_pio_get_sandisk_fwid(struct mmc_host *host, uchar *dst)
{
	//msdc_priv_t *priv = (msdc_priv_t*)host->priv;
	//u32 base = host->base;
	u32 blksz = host->blklen;
	int err = MMC_ERR_NONE, derr = MMC_ERR_NONE;
	//int multi;
	struct mmc_command cmd;

	ulong *ptr = (ulong *)dst;

	//MSG(OPS, "[SD%d] Read data %d bytes from 0x%x\n", host->id, nblks * blksz, src);

	msdc_set_blknum(host, 1);
	msdc_set_blklen(host, blksz);
	msdc_set_timeout(host, 100000000, 0);

	/* send read command */
	cmd.opcode  = MMC_CMD21;
	cmd.rsptyp  = RESP_R1;
	cmd.arg     = 0;
	cmd.retries = 0;
	cmd.timeout = CMD_TIMEOUT;
	err = msdc_cmd(host, &cmd);

	if (err != MMC_ERR_NONE)
		goto done;

	err = derr = msdc_pio_read(host, (u32*)ptr, 1 * blksz);


done:
	if (err != MMC_ERR_NONE) {
		if (derr != MMC_ERR_NONE) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] Read data error (%d)\n", host->id, derr));
			msdc_abort_handler(host, 1);
		} else {
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] Read error (%d)\n", host->id, err));
		}
	}
	return (derr == MMC_ERR_NONE) ? err : derr;
}
int msdc_pio_send_sandisk_fwid(struct mmc_host *host,uchar *src)
{
	//msdc_priv_t *priv = (msdc_priv_t*)host->priv;
	//u32 base = host->base;
	int err = MMC_ERR_NONE, derr = MMC_ERR_NONE;
	//int multi;
	u32 blksz = host->blklen;
	struct mmc_command cmd;

	ulong *ptr = (ulong *)src;

	//MSG(OPS, "[SD%d] Write data %d bytes to 0x%x\n", host->id, nblks * blksz, dst);



	msdc_set_blknum(host, 1);
	msdc_set_blklen(host, blksz);

	/* No need since MSDC always waits 8 cycles for write data timeout */

	/* send write command */
	cmd.opcode  = MMC_CMD50;
	cmd.rsptyp  = RESP_R1;
	cmd.arg     = 0;
	cmd.retries = 0;
	cmd.timeout = CMD_TIMEOUT;
	err = msdc_cmd(host, &cmd);

	if (err != MMC_ERR_NONE)
		goto done;

	err = derr = msdc_pio_write(host, (u32*)ptr, 1 * blksz);

done:
	if (err != MMC_ERR_NONE) {
		if (derr != MMC_ERR_NONE) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] Write data error (%d)\n", host->id, derr));
			msdc_abort_handler(host, 1);
		} else {
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] Write error (%d)\n", host->id, err));
		}
	}
	return (derr == MMC_ERR_NONE) ? err : derr;
}

int msdc_pio_bread(struct mmc_host *host, uchar *dst, ulong src, ulong nblks)
{
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;
	//u32 base = host->base;
	u32 blksz = host->blklen;
	int err = MMC_ERR_NONE, derr = MMC_ERR_NONE, cmd_err = MMC_ERR_NONE;
	int multi;
	struct mmc_command cmd;
	ulong *ptr = (ulong *)dst;

	MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] Read data %d bytes from 0x%x\n", host->id, nblks * blksz, src));

	multi = nblks > 1 ? 1 : 0;

	msdc_set_blknum(host, nblks);
	msdc_set_blklen(host, blksz);
	msdc_set_timeout(host, 100000000, 0);

	/* send read command */
	cmd.opcode  = multi ? MMC_CMD_READ_MULTIPLE_BLOCK : MMC_CMD_READ_SINGLE_BLOCK;

	/* CMD23 with length only 1 */
	if (priv->autocmd & MSDC_AUTOCMD23)
		cmd.opcode = MMC_CMD_READ_MULTIPLE_BLOCK;

	cmd.rsptyp  = RESP_R1;
	cmd.arg     = src;
	cmd.retries = 0;
	cmd.timeout = CMD_TIMEOUT;

	host->cmd = &cmd;
	err = msdc_cmd(host, &cmd);
	if (err != MMC_ERR_NONE)
		goto done;

	derr = msdc_pio_read(host, (u32*)ptr, nblks * blksz);
	if (derr != MMC_ERR_NONE)
		goto done;

	if (multi && (priv->autocmd == 0)) {
		cmd_err = msdc_cmd_stop(host, &cmd);
	}

done:
	if (err != MMC_ERR_NONE) {
		/* msdc_cmd will do cmd tuning flow, so if enter here, cmd maybe timeout.
		 * need reset host */
		//Light: msdc_abort_handler() combined from preloader/LK and CTP can not meet this purpose,
		//       so call msdc_abort() directly
		//msdc_abort_handler(host, 0);
		msdc_abort(host);
		return err;                    // high level will retry
	}

	if (derr != MMC_ERR_NONE) {
		/* crc error find in data transfer. need reset host & send cmd12 */
		/* if autocmd crc occur, will enter here too */
		msdc_abort_handler(host, 1);

		return derr;
	}

	if (cmd_err != MMC_ERR_NONE) {
		/* msdc_cmd will do cmd tuning flow, so if enter here, cmd maybe timeout
		 * need reset host */
		//Light: msdc_abort_handler() combined from preloader/LK and CTP can not meet this purpose,
		//       so call msdc_abort() directly
		//msdc_abort_handler(host, 0);
		msdc_abort(host);
	}
	return MMC_ERR_NONE;
}

int msdc_pio_bwrite(struct mmc_host *host, ulong dst, uchar *src, ulong nblks)
{
	msdc_priv_t *priv = (msdc_priv_t*)host->priv;
	//u32 base = host->base;
	int err = MMC_ERR_NONE, derr = MMC_ERR_NONE, cmd_err = MMC_ERR_NONE;
	int multi;
	u32 blksz = host->blklen;
	struct mmc_command cmd;
	//struct mmc_command stop;
	ulong *ptr = (ulong *)src;

	MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] Write data %d bytes to 0x%x\n", host->id, nblks * blksz, dst));

	multi = nblks > 1 ? 1 : 0;

	msdc_set_blknum(host, nblks);
	msdc_set_blklen(host, blksz);

	/* send write command */
	cmd.opcode  = multi ? MMC_CMD_WRITE_MULTIPLE_BLOCK : MMC_CMD_WRITE_BLOCK;

	/* CMD23 with length only 1 */
	if (priv->autocmd & MSDC_AUTOCMD23)
		cmd.opcode = MMC_CMD_WRITE_MULTIPLE_BLOCK;

	cmd.rsptyp  = RESP_R1;
	cmd.arg     = dst;
	cmd.retries = 0;
	cmd.timeout = CMD_TIMEOUT;
	err = msdc_cmd(host, &cmd);

	if (err != MMC_ERR_NONE)
		goto done;

	host->cmd = &cmd;
	derr = msdc_pio_write(host, (u32*)ptr, nblks * blksz);
	if (multi && (priv->autocmd == 0)) {
		cmd_err = msdc_cmd_stop(host, &cmd);
	}

done:
	if (err != MMC_ERR_NONE) {
		/* msdc_cmd will do cmd tuning flow, so if enter here, cmd maybe timeout.
		 * need reset host */
		//Light: msdc_abort_handler() combined from preloader/LK and CTP can not meet this purpose,
		//       so call msdc_abort() directly
		//msdc_abort_handler(host, 0);
		msdc_abort(host);
		return err;                    // high level will retry
	}

	if (derr != MMC_ERR_NONE) {
		/* crc error find in data transfer. need reset host & send cmd12 */
		/* if autocmd crc occur, will enter here too */
		msdc_abort_handler(host, 1);

		return derr;
	}

	if (cmd_err != MMC_ERR_NONE) {
		/* msdc_cmd will do cmd tuning flow, so if enter here, cmd maybe timeout
		 * need reset host */
		//Light: msdc_abort_handler() combined from preloader/LK and CTP can not meet this purpose,
		//       so call msdc_abort() directly
		//msdc_abort_handler(host, 0);
		msdc_abort(host);
		return MMC_ERR_FAILED;  // high level will retry
	}

	return MMC_ERR_NONE;
}

/* perloader will pre-set msdc pll and the mux channel of msdc pll */
/* note: pll will not changed */
void msdc_config_clksrc(struct mmc_host *host, u32 clksrc, u32 hclksrc)
{
	// modify the clock
	MSDC_ASSERT(host);

	/* Perloader and LK use 200 is ok, no need change source */
	host->pll_mux_clk  = clksrc;
	host->pll_mux_hclk = hclksrc;
	host->src_clk      = hclks_msdc30[clksrc];

	MSDC_TRC_PRINT(MSDC_INIT,("[info][%s] pll_clk %u (%uMHz), pll_hclk %u\n", __func__, host->pll_mux_clk, host->src_clk/1000000, host->pll_mux_hclk));
}


void msdc_config_clock(struct mmc_host *host, int ddr, u32 hz, u32 hs_timing)
{
	msdc_priv_t *priv = host->priv;
	u32 base = host->base;
	u32 mode, hs400_src = 0;
	u32 div;
	u32 sclk;
	u32 u4buswidth=0;

	if (hz >= host->f_max) {
		hz = host->f_max;
	} else if (hz < host->f_min) {
		hz = host->f_min;
	}

	msdc_config_clksrc(host, host->pll_mux_clk, host->pll_mux_hclk);

	if (hs_timing & EXT_CSD_HS_TIMEING_HS400) {
		mode = 0x3; /* HS400 mode */
		sclk = host->src_clk >> 1; // use 400Mhz source
		div  = 0;
	} else if (ddr) {
		mode = 0x2; /* ddr mode and use divisor */
		if (hz >= (host->src_clk >> 2)) {
			div  = 0;               /* mean div = 1/2 */
			sclk = host->src_clk >> 2; /* sclk = clk/div/2. 2: internal divisor */
		} else {
			div  = (host->src_clk + ((hz << 2) - 1)) / (hz << 2);
			sclk = (host->src_clk >> 2) / div;
			div  = (div >> 1);     /* since there is 1/2 internal divisor */
		}
	} else if (hz >= host->src_clk) {
		mode = 0x1; /* no divisor and divisor is ignored */
		div  = 0;
		sclk = host->src_clk;
	} else {
		mode = 0x0; /* use divisor */
		if (hz >= (host->src_clk >> 1)) {
			div  = 0;               /* mean div = 1/2 */
			sclk = host->src_clk >> 1; /* sclk = clk / 2 */
		} else {
			div  = (host->src_clk + ((hz << 2) - 1)) / (hz << 2);
			sclk = (host->src_clk >> 2) / div;
		}
	}
	host->cur_bus_clk = sclk;

	/* set clock mode and divisor */
	MSDC_SET_FIELD(MSDC_CFG, (MSDC_CFG_CKMOD_HS400 | MSDC_CFG_CKMOD |MSDC_CFG_CKDIV),\
	               (hs400_src << 14) | (mode << 12) | div);

	/* wait clock stable */
	while (!(MSDC_READ32(MSDC_CFG) & MSDC_CFG_CKSTB));

	if (hs_timing & EXT_CSD_HS_TIMEING_HS400) {
		msdc_set_smpl(host, 1, priv->rsmpl, TYPE_CMD_RESP_EDGE);
		msdc_set_smpl(host, 1, priv->rdsmpl, TYPE_READ_DATA_EDGE);
		msdc_set_smpl(host, 1, priv->wdsmpl, TYPE_WRITE_CRC_EDGE);
		MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS,0);
	} else {
		msdc_set_smpl(host, 0, priv->rsmpl, TYPE_CMD_RESP_EDGE);
		msdc_set_smpl(host, 0, priv->rdsmpl, TYPE_READ_DATA_EDGE);
		msdc_set_smpl(host, 0, priv->wdsmpl, TYPE_WRITE_CRC_EDGE);
#ifdef MSDC_USE_PATCH_BIT2_TURNING_WITH_ASYNC
		MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS,1);
#else
		MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS,0);
#endif
	}
	MSDC_GET_FIELD(SDC_CFG,SDC_CFG_BUSWIDTH,u4buswidth);

	MSDC_TRC_PRINT(MSDC_INIT,("[SD%d] SET_CLK(%dkHz): SCLK(%dkHz) MODE(%d) DDR(%d) DIV(%d) DS(%d) RS(%d) buswidth(%s)\n",
	                          host->id, hz/1000, sclk/1000, mode, ddr > 0 ? 1 : 0, div,\
	                          msdc_cap[host->id].data_edge, msdc_cap[host->id].cmd_edge,\
	                          (u4buswidth == 0) ? "1-bit" : (u4buswidth == 1)? "4-bits" : (u4buswidth == 2)? "8-bits" : "undefined"));
}

void msdc_config_bus(struct mmc_host *host, u32 width)
{
	u32 val,mode, div;
	u32 base = host->base;

	val = (width == HOST_BUS_WIDTH_8) ? 2 :
	      (width == HOST_BUS_WIDTH_4) ? 1 : 0;

	MSDC_SET_FIELD(SDC_CFG, SDC_CFG_BUSWIDTH, val);

	MSDC_GET_FIELD(MSDC_CFG,MSDC_CFG_CKMOD,mode);
	MSDC_GET_FIELD(MSDC_CFG,MSDC_CFG_CKDIV,div);
	MSDC_TRC_PRINT(MSDC_INFO,("[MSDC%d] CLK (%uMHz), SCLK(%dkHz) MODE(%d) DIV(%d) buswidth(%u-bits)\n",
	                          host->id, host->src_clk/1000000, host->cur_bus_clk/1000, mode, div, width));

}

////////////////////////////////////////////////////////////////////////////////
//
// Power Control -- Common for ASIC and FPGA
//
////////////////////////////////////////////////////////////////////////////////
#ifdef FPGA_PLATFORM
void msdc_set_host_level_pwr(struct mmc_host *host, u32 on, u32 level)
{
	//Parameter host is currently not used. Reserve it for future usage

	// GPO[3:2] = {LVL_PWR33, LVL_PWR18};
	msdc_clr_gpio(PWR_MASK_VOL_18);
	msdc_clr_gpio(PWR_MASK_VOL_33);

	if ( on ) {
		if (level)
			msdc_set_gpio(PWR_MASK_VOL_18);
		else
			msdc_set_gpio(PWR_MASK_VOL_33);
	}

	//add for fpga debug
	msdc_set_gpio(PWR_MASK_L4);
}
#else
#if defined(MMC_MSDC_DRV_CTP)
void msdc_set_host_level_pwr(struct mmc_host *host, u32 on, u32 level)
{
	switch (host->id) {

		case 0:
			//no need change;
			break;
		case 1:
			host->cur_pwr = VOL_1800;
			msdc_set_rdtdsel(host, (host->cur_pwr == VOL_1800));
			msdc_set_driving(host, &msdc_cap[host->id], (host->cur_pwr == VOL_1800));

			msdc_ldo_power(on, MT6325_POWER_LDO_VMC, VOL_1800, &g_msdc1_io);
			break;
		case 2:
			host->cur_pwr = VOL_1800;
			msdc_set_rdtdsel(host, (host->cur_pwr == VOL_1800));
			msdc_set_driving(host, &msdc_cap[host->id], (host->cur_pwr == VOL_1800));

			msdc_ldo_power(on, MT6325_POWER_LDO_VMC, VOL_1800, &g_msdc2_io);
			break;
		default:

			break;
	}
}
#endif
#endif


void msdc_set_host_pwr(struct mmc_host *host, int on)
{
#ifndef FPGA_PLATFORM
	msdc_set_rdtdsel(host, (host->cur_pwr == VOL_1800));
	msdc_set_driving(host, &msdc_cap[host->id], (host->cur_pwr == VOL_1800));

#if defined(MMC_MSDC_DRV_CTP)
	switch (host->id) {
		case 0:
			//do nothing since it is always on
			host->cur_pwr = VOL_3300;
			break;
		case 1:
			msdc_ldo_power(on, MT6325_POWER_LDO_VMC, VOL_3300, &g_msdc1_io);
			msdc_ldo_power(on, MT6325_POWER_LDO_VMCH, VOL_3300, &g_msdc1_flash);
			host->cur_pwr = VOL_3300;
			break;
		case 2:
			/* for sd */
			msdc_ldo_power(on, MT6325_POWER_LDO_VMC, VOL_3300, &g_msdc2_io);
			msdc_ldo_power(on, MT6325_POWER_LDO_VMCH, VOL_3300, &g_msdc2_flash);
			host->cur_pwr = VOL_3300;
			break;
		default:
			break;
	}
#endif
	// How to invoke PMIC API?
	//MSDC_ASSERT(0);
#else
	msdc_set_host_level_pwr(host, on, 0);
#endif
}

void msdc_host_power(struct mmc_host *host, int on)
{
	MSDC_TRC_PRINT(MSDC_PMIC,("[SD%d] Turn %s %s power \n", host->id, on ? "on" : "off", "host"));

	if (on) {
		msdc_config_pin(host, MSDC_PIN_PULL_UP);
		msdc_set_host_pwr(host, 1);
		msdc_clock(host, 1);
	} else {
		msdc_clock(host, 0);
		msdc_set_host_pwr(host, 0);
		msdc_config_pin(host, MSDC_PIN_PULL_DOWN);
	}
}

void msdc_card_power(struct mmc_host *host, int on)
{
	MSDC_TRC_PRINT(MSDC_PMIC,("[SD%d] Turn %s %s power \n", host->id, on ? "on" : "off", "card"));

#ifdef FPGA_PLATFORM
	msdc_set_card_pwr(host,on);

	mdelay(10);

#else
#if defined(MMC_MSDC_DRV_CTP)
	switch (host->id) {
		case 0:
			//Do nothing since it is always on
			break;
		case 1:
			msdc_ldo_power(on, MT6325_POWER_LDO_VMCH, VOL_3300, &g_msdc1_flash);
			mdelay(10);
			break;
		default:
			break;
	}
#endif
	// How to invoke PMIC API?
	//MSDC_ASSERT(0);
#endif
}

void msdc_power(struct mmc_host *host, u8 mode)
{
	if (mode == MMC_POWER_ON || mode == MMC_POWER_UP) {
		msdc_host_power(host, 1);
		msdc_card_power(host, 1);
	} else {
		msdc_card_power(host, 0);
		msdc_host_power(host, 0);
	}
}

#if defined(FEATURE_MMC_UHS1)
int msdc_switch_volt(struct mmc_host *host, int volt)
{
	u32 base = host->base;
	int err = MMC_ERR_FAILED;
	u32 timeout = 1000;
	u32 status;
	u32 bus_clk = host->cur_bus_clk;
	u32 u4swvolt = 0;

	/* make sure SDC is not busy (TBC) */
	WAIT_COND(!SDC_IS_BUSY(), timeout, timeout);
	if (timeout == 0) {
		err = MMC_ERR_TIMEOUT;
		goto out;
	}

	/* check if CMD/DATA lines both 0 */
	if ((MSDC_READ32(MSDC_PS) & ((1 << 24) | (0xF << 16))) == 0) {

		/* pull up disabled in CMD and DAT[3:0] */
		msdc_config_pin(host, MSDC_PIN_PULL_NONE);

		/* change signal from 3.3v to 1.8v */
		msdc_set_host_level_pwr(host, 1, 1);

		/* wait at least 5ms for 1.8v signal switching in card */
		mdelay(10);

		if (bus_clk > 65000000) {
			/* config clock to 260KHz~65MHz mode for volt switch detection by host. */
			msdc_config_clock(host, 0, MSDC_MIN_SCLK,0);
		}

		u4swvolt = host->cur_bus_clk/1000;
		MSDC_SET_FIELD(SDC_VOL_CHG,SDC_VOL_CHG_VCHGCNT, u4swvolt);
		MSDC_DBG_PRINT(MSDC_DEBUG, ("[%s] sd%d sclk %u SDC_VOL_CHG=0x%x\n", __func__, host->id, host->cur_bus_clk, u4swvolt));


		/* pull up enabled in CMD and DAT[3:0] */
		msdc_config_pin(host, MSDC_PIN_PULL_UP);
		mdelay(5);

		/* start to detect volt change by providing 1.8v signal to card */
		MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_BV18SDT);

		/* wait at max. 1ms */
		mdelay(1);

		while ((status = MSDC_READ32(MSDC_CFG)) & MSDC_CFG_BV18SDT);

		if (status & MSDC_CFG_BV18PSS)
			err = MMC_ERR_NONE;
		else
			MSDC_ERR_PRINT(MSDC_ERROR,("[%s] sd%d v18 switch failed, MSDC_CFG=0x%x\n", __func__, host->id, status));

		/* config clock back to init clk freq. */
		if (bus_clk > 65000000) {
			/* config clock back to init clk freq. */
			msdc_config_clock(host, 0, bus_clk, 0);
		}
	}

out:

	return err;
}
#endif

void msdc_reset_tune_counter(struct mmc_host *host)
{
	host->time_read = 0;
}

#if defined(FEATURE_MMC_CM_TUNING)
int msdc_tune_cmdrsp(struct mmc_host *host, struct mmc_command *cmd)
{
	u32 base = host->base;
	u32 sel = 0;
	u32 rsmpl,cur_rsmpl, orig_rsmpl;
	u32 rrdly,cur_rrdly, orig_rrdly;
	u32 cntr,cur_cntr,orig_cmdrtc;
	u32 dl_cksel, cur_dl_cksel, orig_dl_cksel;
	u32 times = 0;
	int result = MMC_ERR_CMDTUNEFAIL;
	u8 hs400 = 0, orig_clkmode;
	u32 t_rrdly, t_rsmpl, t_dl_cksel,t_cmdrtc;

	if (host->cur_bus_clk > 100000000) {
		sel = 1;
	}

	MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, orig_rsmpl);
	MSDC_GET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, orig_rrdly);
	MSDC_GET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_CMD_RSP_TA_CNTR, orig_cmdrtc);
	MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, orig_dl_cksel);
	MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, orig_clkmode);

	hs400 = (orig_clkmode == 3) ? 1 : 0;

	dl_cksel = 0;
	do {
		cntr = 0;
		do {
			rrdly = 0;
			do {
				for (rsmpl = 0; rsmpl < 2; rsmpl++) {
					cur_rsmpl = (orig_rsmpl + rsmpl) % 2;
					msdc_set_smpl(host, hs400, cur_rsmpl, TYPE_CMD_RESP_EDGE);
					if (host->cur_bus_clk <= 400000) {
						MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, 0);
					}
					if (cmd->opcode != MMC_CMD_STOP_TRANSMISSION) {
						if (host->app_cmd) {
							host->app_cmd = false;
							result = msdc_app_cmd(host);
							host->app_cmd = true;
							if (result != MMC_ERR_NONE)
								return MMC_ERR_CMDTUNEFAIL;
						}
						result = msdc_send_cmd(host, cmd);
						if (result == MMC_ERR_TIMEOUT)
							rsmpl--;
						if (result != MMC_ERR_NONE && cmd->opcode != MMC_CMD_STOP_TRANSMISSION) {
							if (cmd->opcode == MMC_CMD_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_CMD_WRITE_MULTIPLE_BLOCK || cmd->opcode == MMC_CMD_READ_SINGLE_BLOCK ||cmd->opcode == MMC_CMD_WRITE_BLOCK)
								msdc_abort_handler(host,1);
							continue;
						}
						result = msdc_wait_rsp(host, cmd);
					} else if (cmd->opcode == MMC_CMD_STOP_TRANSMISSION) {
						result = MMC_ERR_NONE;
						goto done;
					} else
						result = MMC_ERR_BADCRC;

					/* for debugging */
					times++;
#ifdef ___MSDC_DEBUG___
					MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, t_rsmpl);
					MSDC_GET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, t_rrdly);
					//MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_CKGEN_RX_SDCLKO_SEL, t_cksel);
					MSDC_GET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_CMD_RSP_TA_CNTR, t_cmdrtc);
					MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, t_dl_cksel);

					MSDC_DBG_PRINT(MSDC_TUNING,("[SD%d] <TUNE_CMD%d><%d><%s> CMDRRDLY=%d, RSPL=%dh\n",
					                            host->id, (cmd->opcode & (~(SD_CMD_BIT | SD_CMD_APP_BIT))), times, (result == MMC_ERR_NONE) ?
					                            "PASS" : "FAIL", t_rrdly, t_rsmpl));
					MSDC_DBG_PRINT(MSDC_TUNING,("[SD%d] <TUNE_CMD><%d><%s> CMD_RSP_TA_CNTR=%xh\n",
					                            host->id, times, (result == MMC_ERR_NONE) ?
					                            "PASS" : "FAIL", t_cmdrtc));
					if (host->cur_bus_clk > 100000000) {
						MSDC_DBG_PRINT(MSDC_TUNING,("[SD%d] <TUNE_CMD%d><%d><%s> CMD_RSP_TA_CNTR=%xh, INT_DAT_LATCH_CK_SEL=%xh\n",
						                            host->id, (cmd->opcode & (~(SD_CMD_BIT | SD_CMD_APP_BIT))), times, (result == MMC_ERR_NONE) ?
						                            "PASS" : "FAIL", t_cmdrtc, t_dl_cksel));
					}
#endif

					if (result == MMC_ERR_NONE) {
						host->app_cmd = false;
						goto done;
					}

					if (cmd->opcode == MMC_CMD_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_CMD_WRITE_MULTIPLE_BLOCK || cmd->opcode == MMC_CMD_READ_SINGLE_BLOCK ||cmd->opcode == MMC_CMD_WRITE_BLOCK)
						msdc_abort_handler(host,1);
				}
				cur_rrdly = (orig_rrdly + rrdly + 1) % 32;
				MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, cur_rrdly);
			} while (++rrdly < 32);
			if (!sel)
				break;
			cur_cntr = (orig_cmdrtc + cntr + 1) % 8;
			MSDC_SET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_CMD_RSP_TA_CNTR, cur_cntr);
		} while (++cntr < 8);
		/* no need to update data ck sel */
		if (!sel)
			break;
		cur_dl_cksel = (orig_dl_cksel +dl_cksel+1) % 8;
		MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, cur_dl_cksel);
		dl_cksel++;
	} while (dl_cksel < 8);

	/* no need to update ck sel */
	if (result != MMC_ERR_NONE)
		result = MMC_ERR_CMDTUNEFAIL;
done:
	MSDC_TRC_PRINT(MSDC_TUNING,("[SD%d] <TUNE_CMD%d><%d><%s>@msdc_tune_cmdrsp\n",
	                            host->id, (cmd->opcode & (~(SD_CMD_BIT | SD_CMD_APP_BIT))), times,\
	                            (result == MMC_ERR_NONE) ? "PASS" : "FAIL"));
	return result;
}
#endif

#if defined(MMC_MSDC_DRV_CTP)
void msdc_tune_update_cmdrsp(struct mmc_host *host, u32 count)
{
	u32 base = host->base;
	u32 sel = 0;
	u32 rsmpl,cur_rsmpl, orig_rsmpl;
	u32 rrdly,cur_rrdly, orig_rrdly;
	u32 cntr,cur_cntr,orig_cmdrtc;
	u32 dl_cksel, cur_dl_cksel, orig_dl_cksel;
	u32 times = 0;
	u8 hs400 = 0, orig_clkmode;

	MSDC_DBG_PRINT(MSDC_DEBUG,("cur_bus_clk = %d\n", host->cur_bus_clk));
	if (host->cur_bus_clk > 100000000) {
		sel = 1;
	}

	MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, orig_rsmpl);
	MSDC_GET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, orig_rrdly);
	MSDC_GET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_CMD_RSP_TA_CNTR, orig_cmdrtc);
	MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, orig_dl_cksel);
	MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, orig_clkmode);

	hs400 = (orig_clkmode == 3) ? 1 : 0;
	dl_cksel = 0;
	cntr = 0;
	rrdly = 0;
	if (sel == 1) {
		if (count >= 8 * 64 && count < 8 * 8 * 64) {
			dl_cksel = count % 8;
			cur_dl_cksel = (orig_dl_cksel + dl_cksel + 1) % 8;
			MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, cur_dl_cksel);

			count = count % (8 * 64);
		}

		if (count >= 64 && count < 8 * 64) {
			cntr = count % 8;
			cur_cntr = (orig_cmdrtc + cntr + 1) % 8;
			MSDC_SET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_CMD_RSP_TA_CNTR, cur_cntr);

			count = count % 64;
		}
	}

	if (count >= 2 && count < 64) {
		rrdly = count % 32;
		cur_rrdly = (orig_rrdly + rrdly + 1) % 32;
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, cur_rrdly);

		count = (count > 32 ? 1 : 0);
	}

	if (count >= 0 && count < 2) {
		cur_rsmpl = (orig_rsmpl + count) % 2;
		msdc_set_smpl(host, hs400, cur_rsmpl, TYPE_CMD_RESP_EDGE);
	}
}
#endif

#if defined(FEATURE_MMC_RD_TUNING)
int msdc_tune_bread(struct mmc_host *host, uchar *dst, ulong src, ulong nblks)
{
	u32 base = host->base;
	u32 dcrc, ddr = 0, sel = 0;
	u32 cur_rxdly0, cur_rxdly1;
	u32 rdsmpl, cur_rdsmpl, orig_rdsmpl;
	u32 dsel,cur_dsel,orig_dsel;
	u32 dl_cksel,cur_dl_cksel,orig_dl_cksel;
	u32 rxdly;
	u32 cur_dat0, cur_dat1, cur_dat2, cur_dat3, cur_dat4, cur_dat5,
	    cur_dat6, cur_dat7;
	u32 orig_dat0, orig_dat1, orig_dat2, orig_dat3, orig_dat4, orig_dat5,
	    orig_dat6, orig_dat7;
	u32 orig_clkmode;
	u32 times = 0;
	int result = MMC_ERR_READTUNEFAIL;
	u8 hs400 = 0;
	u32 t_dspl, t_ckgen_dsel, t_int_cksel;

	if (host->cur_bus_clk > 100000000)
		sel = 1;

	MSDC_GET_FIELD(MSDC_CFG, MSDC_CFG_CKMOD, orig_clkmode);
	ddr = (orig_clkmode == 2) ? 1 : 0;
	hs400 = (orig_clkmode == 3) ? 1 : 0;

	MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_CKGEN_MSDC_DLY_SEL, orig_dsel);
	MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, orig_dl_cksel);
	MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, orig_rdsmpl);

	/* Tune Method 2. delay each data line */
	MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_DDLSEL, 1);

	dl_cksel = 0;
	do {
		dsel = 0;
		do {
			rxdly = 0;
			do {
				for (rdsmpl = 0; rdsmpl < 2; rdsmpl++) {
					cur_rdsmpl = (orig_rdsmpl + rdsmpl) % 2;
					msdc_set_smpl(host, hs400, cur_rdsmpl, TYPE_READ_DATA_EDGE);

					result = host->blk_read(host, dst, src, nblks);
					if (result == MMC_ERR_CMDTUNEFAIL || result == MMC_ERR_CMD_RSPCRC || result == MMC_ERR_ACMD_RSPCRC)
						goto done;

					MSDC_GET_FIELD(SDC_DCRC_STS, SDC_DCRC_STS_POS|SDC_DCRC_STS_NEG, dcrc);

					if (!ddr) dcrc &= ~SDC_DCRC_STS_NEG;

					/* for debugging */
					times++;
#ifdef ___MSDC_DEBUG___

					MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, t_dspl);
					MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_CKGEN_MSDC_DLY_SEL, t_ckgen_dsel);
					MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, t_int_cksel);

					MSDC_DBG_PRINT(MSDC_TUNING,("[SD%d] <TUNE_BREAD_%d><%s><cmd%d:0x%lx> DCRC=%xh, ret=%d\n",
					                            host->id, times, (result == MMC_ERR_NONE && dcrc == 0) ?
					                            "PASS" : "FAIL", (nblks == 1 ? 17 : 18), src, dcrc, result));
					MSDC_DBG_PRINT(MSDC_TUNING,("[SD%d] <TUNE_BREAD_%d><%s><cmd%d:0x%lx> DATRDDLY0=%xh, DATRDDLY1=%xh, DSMPL=%xh\n",
					                            host->id, times, (result == MMC_ERR_NONE && dcrc == 0) ?
					                            "PASS" : "FAIL", (nblks == 1 ? 17 : 18), src, MSDC_READ32(MSDC_DAT_RDDLY0), MSDC_READ32(MSDC_DAT_RDDLY1), t_dspl));

					if (host->cur_bus_clk >= 100000000) {
						MSDC_DBG_PRINT(MSDC_TUNING,("[SD%d] <TUNE_BREAD_%d><%s><cmd%d:0x%lx> CKGEN_MSDC_DLY_SEL=%xh, INT_DAT_LATCH_CK_SEL=%xh\n",
						                            host->id, times, (result == MMC_ERR_NONE && dcrc == 0) ?
						                            "PASS" : "FAIL", (nblks == 1 ? 17 : 18), src, t_ckgen_dsel, t_int_cksel));
					}
#endif
					/* no crc error in this data line */
					if (result == MMC_ERR_NONE && dcrc == 0) {
						goto done;
					} else {
						result = MMC_ERR_BADCRC;
					}
				}
				cur_rxdly0 = MSDC_READ32(MSDC_DAT_RDDLY0);
				cur_rxdly1 = MSDC_READ32(MSDC_DAT_RDDLY1);

				orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
				orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
				orig_dat2 = (cur_rxdly0 >> 8)  & 0x1F;
				orig_dat3 = (cur_rxdly0 >> 0)  & 0x1F;
				orig_dat4 = (cur_rxdly1 >> 24) & 0x1F;
				orig_dat5 = (cur_rxdly1 >> 16) & 0x1F;
				orig_dat6 = (cur_rxdly1 >> 8)  & 0x1F;
				orig_dat7 = (cur_rxdly1 >> 0)  & 0x1F;

				// Bits8~15 of dcrc have been masked for non-ddr case,
				// so we can process ddr and non-ddr cases with the same code
				cur_dat0 = (dcrc & ((1 << 0) || (1 << 8)) ) ?  (orig_dat0 + 1) : orig_dat0;
				cur_dat1 = (dcrc & ((1 << 1) || (1 << 9)) ) ?  (orig_dat1 + 1) : orig_dat1;
				cur_dat2 = (dcrc & ((1 << 2) || (1 << 10)) ) ? (orig_dat2 + 1) : orig_dat2;
				cur_dat3 = (dcrc & ((1 << 3) || (1 << 11)) ) ? (orig_dat3 + 1) : orig_dat3;
				cur_dat4 = (dcrc & ((1 << 4) || (1 << 12)) ) ? (orig_dat4 + 1) : orig_dat4;
				cur_dat5 = (dcrc & ((1 << 5) || (1 << 13)) ) ? (orig_dat5 + 1) : orig_dat5;
				cur_dat6 = (dcrc & ((1 << 6) || (1 << 14)) ) ? (orig_dat6 + 1) : orig_dat6;
				cur_dat7 = (dcrc & ((1 << 7) || (1 << 15)) ) ? (orig_dat7 + 1) : orig_dat7;

				cur_rxdly0 = ((cur_dat0 & 0x1F) << 24) | ((cur_dat1 & 0x1F) << 16) |
				             ((cur_dat2 & 0x1F)<< 8) | ((cur_dat3 & 0x1F) << 0);
				cur_rxdly1 = ((cur_dat4 & 0x1F) << 24) | ((cur_dat5 & 0x1F) << 16) |
				             ((cur_dat6 & 0x1F) << 8) | ((cur_dat7 & 0x1F) << 0);

				MSDC_WRITE32(MSDC_DAT_RDDLY0, cur_rxdly0);
				MSDC_WRITE32(MSDC_DAT_RDDLY1, cur_rxdly1);
			} while (++rxdly < 32);
			if (!sel)
				break;
			cur_dsel = (orig_dsel + dsel + 1) % 32;

			MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_CKGEN_MSDC_DLY_SEL, cur_dsel);
		} while (++dsel < 32);
		/* no need to update data ck sel */
		if (orig_clkmode != 1)
			break;

		cur_dl_cksel = (orig_dl_cksel + dl_cksel + 1) % 8;
		MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, cur_dl_cksel);
		dl_cksel++;
	} while (dl_cksel < 8);
done:
	MSDC_TRC_PRINT(MSDC_TUNING,("[SD%d] <msdc_tune_bread<%d><%s><cmd%d>@msdc_tune_bread\n",
	                            host->id, times, (result == MMC_ERR_NONE && dcrc == 0) ?"PASS" : "FAIL", (nblks == 1 ? 17 : 18)));


	return result;
}

#define READ_TUNING_MAX_HS (2 * 32)
#define READ_TUNING_MAX_UHS (2 * 32 * 32)
#define READ_TUNING_MAX_UHS_CLKMOD1 (2 * 32 * 32 *8)

int msdc_tune_read(struct mmc_host *host)
{
	u32 base = host->base;
	u32 dcrc, ddr = 0, sel = 0;
	u32 cur_rxdly0 = 0 , cur_rxdly1 = 0;
	u32 cur_dsmpl = 0, orig_dsmpl;
	u32 cur_dsel = 0,orig_dsel;
	u32 cur_dl_cksel = 0,orig_dl_cksel;
	u32 cur_dat0 = 0, cur_dat1 = 0, cur_dat2 = 0, cur_dat3 = 0, cur_dat4 = 0, cur_dat5 = 0,
	    cur_dat6 = 0, cur_dat7 = 0;
	u32 orig_dat0, orig_dat1, orig_dat2, orig_dat3, orig_dat4, orig_dat5,
	    orig_dat6, orig_dat7;
	u32 orig_clkmode;
	//u32 times = 0;
	int result = MMC_ERR_NONE;
	u8 hs400 = 0;

	if (host->cur_bus_clk > 100000000)
		sel = 1;

	if (host->card) {
		ddr = mmc_card_ddr(host->card);
	}
	MSDC_GET_FIELD(MSDC_CFG,MSDC_CFG_CKMOD,orig_clkmode);
	hs400 = (orig_clkmode == 3) ? 1 : 0;
	//if(orig_clkmode == 1)
	//MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_CKGEN_RX_SDCLKO_SEL, 0);

	MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_CKGEN_MSDC_DLY_SEL, orig_dsel);
	MSDC_GET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, orig_dl_cksel);
	MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, orig_dsmpl);

	/* Tune Method 2. delay each data line */
	MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_DDLSEL, 1);

	cur_dsmpl = (orig_dsmpl + 1) ;
	msdc_set_smpl(host, hs400, (cur_dsmpl % 2), TYPE_READ_DATA_EDGE);
	if (cur_dsmpl >= 2) {
		MSDC_GET_FIELD(SDC_DCRC_STS, SDC_DCRC_STS_POS|SDC_DCRC_STS_NEG, dcrc);
		if (!ddr) dcrc &= ~SDC_DCRC_STS_NEG;

		cur_rxdly0 = MSDC_READ32(MSDC_DAT_RDDLY0);
		cur_rxdly1 = MSDC_READ32(MSDC_DAT_RDDLY1);

		orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
		orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
		orig_dat2 = (cur_rxdly0 >> 8) & 0x1F;
		orig_dat3 = (cur_rxdly0 >> 0) & 0x1F;
		orig_dat4 = (cur_rxdly1 >> 24) & 0x1F;
		orig_dat5 = (cur_rxdly1 >> 16) & 0x1F;
		orig_dat6 = (cur_rxdly1 >> 8) & 0x1F;
		orig_dat7 = (cur_rxdly1 >> 0) & 0x1F;

		// Bits8~15 of dcrc have been masked for non-ddr case,
		// so we can process ddr and non-ddr cases with the same code
		cur_dat0 = (dcrc & ((1 << 0) || (1 << 8)) ) ?  (orig_dat0 + 1) : orig_dat0;
		cur_dat1 = (dcrc & ((1 << 1) || (1 << 9)) ) ?  (orig_dat1 + 1) : orig_dat1;
		cur_dat2 = (dcrc & ((1 << 2) || (1 << 10)) ) ? (orig_dat2 + 1) : orig_dat2;
		cur_dat3 = (dcrc & ((1 << 3) || (1 << 11)) ) ? (orig_dat3 + 1) : orig_dat3;
		cur_dat4 = (dcrc & ((1 << 4) || (1 << 12)) ) ? (orig_dat4 + 1) : orig_dat4;
		cur_dat5 = (dcrc & ((1 << 5) || (1 << 13)) ) ? (orig_dat5 + 1) : orig_dat5;
		cur_dat6 = (dcrc & ((1 << 6) || (1 << 14)) ) ? (orig_dat6 + 1) : orig_dat6;
		cur_dat7 = (dcrc & ((1 << 7) || (1 << 15)) ) ? (orig_dat7 + 1) : orig_dat7;

		cur_rxdly0 = ((cur_dat0 & 0x1F) << 24) | ((cur_dat1 & 0x1F) << 16) |
		             ((cur_dat2 & 0x1F) << 8) | ((cur_dat3 & 0x1F) << 0);
		cur_rxdly1 = ((cur_dat4 & 0x1F) << 24) | ((cur_dat5 & 0x1F)<< 16) |
		             ((cur_dat6 & 0x1F) << 8) | ((cur_dat7 & 0x1F) << 0);

		MSDC_WRITE32(MSDC_DAT_RDDLY0, cur_rxdly0);
		MSDC_WRITE32(MSDC_DAT_RDDLY1, cur_rxdly1);

	}
	if (cur_dat0 >= 32 || cur_dat1 >= 32 || cur_dat2 >= 32 || cur_dat3 >= 32 ||
	        cur_dat4 >= 32 || cur_dat5 >= 32 || cur_dat6 >= 32 || cur_dat7 >= 32) {
		if (sel) {
			cur_dsel = (orig_dsel + 1);
			MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_CKGEN_MSDC_DLY_SEL, cur_dsel % 32);
		}
	}
	if (cur_dsel >= 32) {
		if (orig_clkmode == 1 && sel) {
			cur_dl_cksel = (orig_dl_cksel + 1);
			MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_INT_DAT_LATCH_CK_SEL, cur_dl_cksel % 8);
		}
	}
	++(host->time_read);
	if ((sel == 1 && orig_clkmode == 1 && host->time_read == READ_TUNING_MAX_UHS_CLKMOD1)||
	        (sel == 1 && orig_clkmode != 1 && host->time_read == READ_TUNING_MAX_UHS)||
	        (sel == 0 && orig_clkmode != 1 && host->time_read == READ_TUNING_MAX_HS)) {
		result = MMC_ERR_READTUNEFAIL;
	}

	MSDC_TRC_PRINT(MSDC_TUNING,("[SD%d] <msdc_tune_read <%s> @msdc_tune_bread\n",
	                            host->id, (result == MMC_ERR_NONE && dcrc == 0) ?"PASS" : "FAIL"));
	return result;
}
#endif  /* end of FEATURE_MMC_RD_TUNING */

int msdc_tune_rw_hs400(struct mmc_host *host, uchar *dst, ulong src, ulong nblks, unsigned int rw)
{
	return 0;
}

#if defined(FEATURE_MMC_WR_TUNING)
int msdc_tune_bwrite(struct mmc_host *host, ulong dst, uchar *src, ulong nblks)
{
	u32 base = host->base;
	u32 orig_clkmode;
	u32 sel = 0;
	//u32 ddrckdly = 0;
	u32 wrrdly, cur_wrrdly, orig_wrrdly;
	u32 wdsmpl, cur_wdsmpl, orig_wdsmpl;
	u32 d_cntr,orig_d_cntr,cur_d_cntr;
	u32 rxdly, cur_rxdly0;
	u32 orig_dat0, orig_dat1, orig_dat2, orig_dat3;
	u32 cur_dat0, cur_dat1, cur_dat2, cur_dat3;
	u32 times = 0;
	//u32 status;
	int result = MMC_ERR_WRITETUNEFAIL;
	u8 hs400 = 0;
	u32 t_dspl, t_wrrdly, t_d_cntr;// t_dl_cksel, t_ddrdly, t_cksel;

	if (host->cur_bus_clk > 100000000)
		sel = 1;

	//if (mmc_card_ddr(host->card))
	//    ddrckdly = 1;

	MSDC_GET_FIELD(MSDC_CFG,MSDC_CFG_CKMOD,orig_clkmode);

	hs400 = (orig_clkmode == 2) ? 1 : 0;

	MSDC_GET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, orig_wrrdly);
	MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL, orig_wdsmpl);
	MSDC_GET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_WRDAT_CRCS_TA_CNTR, orig_d_cntr);

	/* Tune Method 2. delay data0 line */
	MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_DDLSEL, 1);

	cur_rxdly0 = MSDC_READ32(MSDC_DAT_RDDLY0);

	orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
	orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
	orig_dat2 = (cur_rxdly0 >> 8) & 0x1F;
	orig_dat3 = (cur_rxdly0 >> 0) & 0x1F;

	d_cntr = 0;
	do {
		rxdly = 0;
		do {
			wrrdly = 0;
			do {
				for (wdsmpl = 0; wdsmpl < 2; wdsmpl++) {
					cur_wdsmpl = (orig_wdsmpl + wdsmpl) % 2;
					msdc_set_smpl(host, hs400, cur_wdsmpl, TYPE_WRITE_CRC_EDGE);
					result = host->blk_write(host, dst, src, nblks);
					if (   result == MMC_ERR_CMDTUNEFAIL
					        || result == MMC_ERR_CMD_RSPCRC
					        || result == MMC_ERR_ACMD_RSPCRC) {
						goto done;
					}

					/* for debugging */
					times++;
#ifdef ___MSDC_DEBUG___

					MSDC_GET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, t_wrrdly);
					MSDC_GET_FIELD(MSDC_IOCON, MSDC_IOCON_W_D_SMPL, t_dspl);
					MSDC_GET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_WRDAT_CRCS_TA_CNTR, t_d_cntr);

					MSDC_DBG_PRINT(MSDC_TUNING,("[SD%d] <TUNE_BWRITE_%d><%s><cmd%d:0x%x> ret=%d, DSPL=%d, WRRDLY=%d, MSDC_DAT_RDDLY0=%xh\n",
					                            host->id, times, result == MMC_ERR_NONE ? "PASS" : "FAIL", (nblks == 1 ? 24 : 25), (unsigned int)dst,
					                            result, t_dspl, t_wrrdly, MSDC_READ32(MSDC_DAT_RDDLY0)));

					if (host->cur_bus_clk >= 100000000) {
						MSDC_DBG_PRINT(MSDC_TUNING,("[SD%d] <TUNE_BWRITE_%d><%s><cmd%d:0x%x> MSDC_PB1_WRDAT_CRCS_TA_CNTR=%xh\n",
						                            host->id, times, (result == MMC_ERR_NONE) ? "PASS" : "FAIL", (nblks == 1 ? 24 : 25), (unsigned int)dst,
						                            t_d_cntr));
					}
#endif
					if (result == MMC_ERR_NONE) {
						goto done;
					}
				}
				cur_wrrdly = ++orig_wrrdly % 32;
				MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, cur_wrrdly);
			} while (++wrrdly < 32);

			cur_dat0 = ++orig_dat0 % 32; /* only adjust bit-1 for crc */
			cur_dat1 = orig_dat1;
			cur_dat2 = orig_dat2;
			cur_dat3 = orig_dat3;

			cur_rxdly0 = (cur_dat0 << 24) | (cur_dat1 << 16) | (cur_dat2 << 8) | (cur_dat3 << 0);

			MSDC_WRITE32(MSDC_DAT_RDDLY0, cur_rxdly0);
		} while (++rxdly < 32);

		/* no need to update data ck sel */
		if (!sel)
			break;

		cur_d_cntr= (orig_d_cntr + d_cntr +1 )% 8;
		MSDC_SET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_WRDAT_CRCS_TA_CNTR, cur_d_cntr);
		d_cntr++;
	} while (d_cntr < 8);
done:
	MSDC_TRC_PRINT(MSDC_TUNING,("[SD%d] <TUNE_BWRITE_%d><%s><cmd%d> ret=%d@msdc_tune_bwrite\n",
	                            host->id, times, result == MMC_ERR_NONE ? "PASS" : "FAIL",
	                            (nblks == 1 ? 24 : 25), result));

	return result;
}
#endif /* end of FEATURE_MMC_WR_TUNING */

int msdc_tune_uhs1(struct mmc_host *host, struct mmc_card *card)
{
	u32 base = host->base;
	u32 status;
	int i;
	int err = MMC_ERR_FAILED;
	struct mmc_command cmd;

	cmd.opcode  = SD_CMD_SEND_TUNING_BLOCK;
	cmd.arg     = 0;
	cmd.rsptyp  = RESP_R1;
	cmd.retries = CMD_RETRIES;
	cmd.timeout = 0xFFFFFFFF;

	msdc_set_timeout(host, 100000000, 0);
	msdc_set_autocmd(host, MSDC_AUTOCMD19, 1);

	for (i = 0; i < 13; i++) {
		/* Note. select a pad to be tuned. msdc only tries 32 times to tune the
		 * pad since there is only 32 tuning steps for a pad.
		 */
		MSDC_SET_FIELD(SDC_ACMD19_TRG, SDC_ACMD19_TRG_TUNESEL, i);

		/* Note. autocmd19 will only trigger done interrupt and won't trigger
		 * autocmd timeout and crc error interrupt. (autocmd19 is a special command
		 * and is different from autocmd12 and autocmd23.
		 */
		err = msdc_cmd(host, &cmd);
		if (err != MMC_ERR_NONE)
			goto out;

		/* read and check acmd19 sts. bit-1: success, bit-0: fail */
		status = MSDC_READ32(SDC_ACMD19_STS);

		if (!status) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] ACMD19_TRG(%d), STS(0x%x) Failed\n", host->id, i,status));
			err = MMC_ERR_FAILED;
			goto out;
		}
	}
	err = MMC_ERR_NONE;

out:
	msdc_set_autocmd(host, MSDC_AUTOCMD19, 0);
	return err;
}

static u32 test_delay_bit(u32 delay, u32 bit)
{
	bit %= PAD_DELAY_MAX;
	return delay & (1 << bit);
}

static int get_delay_len(u32 delay, u32 start_bit)
{
	u32 i;

	for (i = 0; i < (PAD_DELAY_MAX - start_bit); i++) {
		if (test_delay_bit(delay, start_bit + i) == 0)
			return i;
	}
	return PAD_DELAY_MAX - start_bit;
}

static struct msdc_delay_phase get_best_delay(u32 delay)
{
	int start = 0, len = 0;
	int start_final = 0, len_final = 0;
	u8 final_phase = 0xff;
	struct msdc_delay_phase delay_phase = { 0, };

	if (delay == 0) {
		dprintf(CRITICAL, "phase error: [map:%x]\n", delay);
		delay_phase.final_phase = final_phase;
		return delay_phase;
	}

	while (start < PAD_DELAY_MAX) {
		len = get_delay_len(delay, start);
		if (len_final < len) {
			start_final = start;
			len_final = len;
		}
		start += len ? len : 1;
		if (len >= 12 && start_final < 4)
			break;
	}

	/* The rule is that to find the smallest delay cell */
	if (start_final == 0)
		final_phase = (start_final + len_final / 3) % PAD_DELAY_MAX;
	else
		final_phase = (start_final + len_final / 2) % PAD_DELAY_MAX;
	dprintf(ALWAYS, "phase: [map:%x] [maxlen:%d] [final:%d]\n",
		 delay, len_final, final_phase);

	delay_phase.maxlen = len_final;
	delay_phase.start = start_final;
	delay_phase.final_phase = final_phase;
	return delay_phase;
}

int msdc_tune_cmd_data(struct mmc_host *mmc)
{
	addr_t base = mmc->base;
	u32 rise_delay = 0, fall_delay = 0;
	struct msdc_delay_phase final_rise_delay, final_fall_delay = { 0, };
	u8 final_delay, final_maxlen;
	int cmd_err;
	int i, ret;

	MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_RSPL);
	MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_R_D_SMPL);
	MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_W_D_SMPL);
	for (i = 0 ; i < PAD_DELAY_MAX; i++) {
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRDLY, i);
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATRRDLY, i);
		ret = mmc_send_tuning(mmc, &cmd_err);
		if (!ret)
			rise_delay |= (1 << i);
	}
	final_rise_delay = get_best_delay(rise_delay);
	if (final_rise_delay.maxlen >= 12 ||
	    (final_rise_delay.start == 0 && final_rise_delay.maxlen >= 4))
		goto skip_fall;

	MSDC_SET_BIT32(MSDC_IOCON, MSDC_IOCON_RSPL);
	MSDC_SET_BIT32(MSDC_IOCON, MSDC_IOCON_R_D_SMPL);
	MSDC_SET_BIT32(MSDC_IOCON, MSDC_IOCON_W_D_SMPL);
	for (i = 0; i < PAD_DELAY_MAX; i++) {
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRDLY, i);
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATRRDLY, i);
		ret = mmc_send_tuning(mmc, &cmd_err);
		if (!ret)
			fall_delay |= (1 << i);
	}
	final_fall_delay = get_best_delay(fall_delay);

skip_fall:
	final_maxlen = MAX(final_rise_delay.maxlen, final_fall_delay.maxlen);
	if (final_maxlen == final_rise_delay.maxlen) {
		MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_RSPL);
		MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_R_D_SMPL);
		MSDC_CLR_BIT32(MSDC_IOCON, MSDC_IOCON_W_D_SMPL);
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRDLY,
			      final_rise_delay.final_phase);
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATRRDLY,
			      final_rise_delay.final_phase);
		final_delay = final_rise_delay.final_phase;
	} else {
		MSDC_SET_BIT32(MSDC_IOCON, MSDC_IOCON_RSPL);
		MSDC_SET_BIT32(MSDC_IOCON, MSDC_IOCON_R_D_SMPL);
		MSDC_SET_BIT32(MSDC_IOCON, MSDC_IOCON_W_D_SMPL);
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRDLY,
			      final_fall_delay.final_phase);
		MSDC_SET_FIELD(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATRRDLY,
			      final_fall_delay.final_phase);
		final_delay = final_fall_delay.final_phase;
	}

	dprintf(ALWAYS, "Final cmd/data pad delay: %x\n", final_delay);
	return final_delay == 0xff ? -EIO : 0;
}

int msdc_tune_hs200(struct mmc_host *host, struct mmc_card *card)
{
	int ret = 0;

	ret = msdc_tune_cmd_data(host);
	if (ret == -EIO)
		dprintf(ALWAYS, "HS200 tuning fail\n");

	/*CMD21 will change host->blklen setting, so need switch host->blklen as 512*/
        msdc_set_blklen(host, MMC_BLOCK_SIZE);

	return ret;
}

int msdc_tune_hs400(struct mmc_host *host, struct mmc_card *card)
{
	return 0;
}

//#if defined(FEATURE_MMC_CARD_DETECT)
void msdc_card_detect(struct mmc_host *host, int on)
{
	u32 base = host->base;

	if ((msdc_cap[host->id].flags & MSDC_CD_PIN_EN) == 0) {
		MSDC_CARD_DETECTION_OFF();
		return;
	}

	if (on) {
		MSDC_SET_FIELD(MSDC_PS, MSDC_PS_CDDEBOUNCE, DEFAULT_DEBOUNCE);
		MSDC_CARD_DETECTION_ON();
	} else {
		MSDC_CARD_DETECTION_OFF();
		MSDC_SET_FIELD(MSDC_PS, MSDC_PS_CDDEBOUNCE, 0);
	}
}

int msdc_card_avail(struct mmc_host *host)
{
	u32 base = host->base;
	u32 sts, avail = 0;

	if ((msdc_cap[host->id].flags & MSDC_REMOVABLE) == 0)
		return 1;

	if (msdc_cap[host->id].flags & MSDC_CD_PIN_EN) {
		MSDC_GET_FIELD(MSDC_PS, MSDC_PS_CDSTS, sts);
		avail = sts == 0 ? 1 : 0;
	}

	return avail;
}
//#endif

#if defined(MMC_MSDC_DRV_CTP)
int msdc_card_protected(struct mmc_host *host)
{
	u32 base = host->base;
	u32 prot;

	if (msdc_cap[host->id].flags & MSDC_WP_PIN_EN) {
		MSDC_GET_FIELD(MSDC_PS, MSDC_PS_WP, prot);
	} else {
		prot = 0;
	}

	return prot;
}
#endif

#if defined(MMC_MSDC_DRV_CTP) || defined(MMC_MSDC_DRV_LK)
void msdc_hard_reset(struct mmc_host *host)
{
	msdc_card_power(host, 0);
	mdelay(10);

	msdc_card_power(host, 1);
	mdelay(10);
}

void msdc_soft_reset(struct mmc_host *host)
{
	u32 base = host->base;
	u32 tmo = 0x0000ffff;

	MSDC_RESET();
	MSDC_SET_FIELD(MSDC_DMA_CTRL, MSDC_DMA_CTRL_STOP, 1);
	WAIT_COND((MSDC_READ32(MSDC_DMA_CFG) & MSDC_DMA_CFG_STS) == 0, 0xFFFF, tmo);

	if (tmo == 0) {
		MSDC_DBG_PRINT( MSDC_DMA,("[SD%d] MSDC_DMA_CFG_STS != inactive\n", host->id));
	}
	MSDC_CLR_FIFO();
}
#endif

#if defined(MMC_MSDC_DRV_CTP)
void msdc_emmc_hard_reset(struct mmc_host *host)
{
	u32 base = host->base;

	MSDC_SET_BIT32(EMMC_IOCON, EMMC_IOCON_BOOTRST);
	//mt_set_gpio_out(PAD_MSDC0_RSTB,GPIO_OUT_ZERO);
	mdelay(10);
	MSDC_CLR_BIT32(EMMC_IOCON, EMMC_IOCON_BOOTRST);
	//mt_set_gpio_out(PAD_MSDC0_RSTB,GPIO_OUT_ONE);
}
#endif

#ifdef FEATURE_MMC_BOOT_MODE
int msdc_emmc_boot_start(struct mmc_host *host, u32 hz, int ddr, int mode, int ackdis, u8 hostbuswidth, u64 size)
{
	int err = MMC_ERR_NONE;
	u32 sts;
	u32 base = host->base;
	u32 tmo = 0xFFFFFFFF;
	u32 acktmo, dattmo;
	u64 acktime,dattime;
	u32 test_timer1;
	u32 test_timer2;

	MSDC_RESET();
	MSDC_CLR_FIFO();
	msdc_set_blklen(host, 512);
	msdc_set_blknum(host, size/512);
	msdc_config_bus(host, hostbuswidth);
	msdc_config_clock(host, (ddr ? MMC_STATE_DDR : 0), hz, 0);
	//MSDC_SET_FIELD(MSDC_DMA_CFG,3 << 12,0x2);
	//MSDC_SET_FIELD(MSDC_DMA_CFG,3 <<  8,0x1);
	/* requires 74 clocks/1ms before CMD0 */
	MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_CKPDN);
	mdelay(2);
	MSDC_CLR_BIT32(MSDC_CFG, MSDC_CFG_CKPDN);

	/* configure boot timeout value */
	WAIT_COND(SDC_IS_BUSY() == 0, tmo, tmo);
	acktime = 50 * 1000 * 1000ULL;
	dattime = 1000 * 1000 * 1000ULL;
	acktmo = msdc_cal_timeout(host, acktime, 0, 1<<EMMC_BOOT_TMO_IN_CLK_2POWER);   /* 50ms  MT6583 MSDC IP eMMC boot timeout unit change to 2^16*/
	dattmo = msdc_cal_timeout(host, dattime, 0, 1<<EMMC_BOOT_TMO_IN_CLK_2POWER);   /* 1sec */

	if (acktmo == 0) acktmo = 1;
	if (dattmo == 0) dattmo = 1;
	acktmo = acktmo > 0xFFE ? 0xFFE : acktmo;
	dattmo = dattmo > 0xFFFFE ? 0xFFFFE : dattmo;

	MSDC_TRC_PRINT(MSDC_INIT,("[SD%d] EMMC BOOT ACK timeout: %d ms (clkcnt: %d)(host->cur_bus_clk = %d)\n", host->id,
	                          (acktmo * 65536) / (host->cur_bus_clk / 1000), acktmo, host->cur_bus_clk));
	MSDC_TRC_PRINT(MSDC_INIT,("[SD%d] EMMC BOOT DAT timeout: %d ms (clkcnt: %d)\n", host->id,
	                          (dattmo * 65536) / (host->cur_bus_clk / 1000), dattmo));

	MSDC_SET_BIT32(EMMC_CFG0, EMMC_CFG0_BOOTSUPP);
	MSDC_SET_FIELD(EMMC_CFG0, EMMC_CFG0_BOOTACKDIS, ackdis);
	MSDC_SET_FIELD(EMMC_CFG0, EMMC_CFG0_BOOTMODE, mode);
	MSDC_SET_FIELD(EMMC_CFG1, EMMC_CFG1_BOOTACKTMC, acktmo);
	MSDC_SET_FIELD(EMMC_CFG1, EMMC_CFG1_BOOTDATTMC, dattmo);

	if (mode == EMMC_BOOT_RST_CMD_MODE) {
		MSDC_WRITE32(SDC_ARG, 0xFFFFFFFA);
	} else {
		MSDC_WRITE32(SDC_ARG, 0);
	}
	MSDC_WRITE32(SDC_CMD, 0x02001000); /* bit[12]: 1 multiple block read, 0: single block read */
#if 0 //init timer to test MT6583 ACK/DAT timeour modification test case
	MSDC_WRITE32(0x10008040,0x31);
	MSDC_WRITE32(0x10008044,0x0);
	test_timer1 = MSDC_READ32(0x10008048);//init timer to test MT6583 ACK/DAT timeour modification test case
#endif
	MSDC_SET_BIT32(EMMC_CFG0, EMMC_CFG0_BOOTSTART);

	WAIT_COND((MSDC_READ32(EMMC_STS) & EMMC_STS_BOOTUPSTATE) == EMMC_STS_BOOTUPSTATE, tmo, tmo);
	if (!ackdis) {
		do {
			sts = MSDC_READ32(EMMC_STS);
			if (sts == 0)
				continue;
			MSDC_WRITE32(EMMC_STS, sts);    /* write 1 to clear */

			/* if ack is error, hw will first set bootackrcv bit, then set bootackerr bit
			 * so the best way is check EMMC_STS_BOOTACKERR  bit after EMMC_STS_BOOTACKRCV bit set*/
			if (sts & EMMC_STS_BOOTACKERR) {
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(0x%x): boot up ack error\n", __func__, host->id, sts));
				err = MMC_ERR_BADCRC;
				goto out;
			} else if (sts & EMMC_STS_BOOTACKRCV) {
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(0x%x): boot ack received\n",  __func__,host->id, sts));
				break;
			} else if (sts & EMMC_STS_BOOTACKTMO) {
#if 0
				test_timer2 = MSDC_READ32(0x10008048);
				test_timer1 = (test_timer2 - test_timer1) /6000;
				MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] EMMC_STS(%x): boot up ack timeout(%d ms)\n", host->id, sts,test_timer1));
				//test MT6583 ACK/DAT timeour modification test case
#endif
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(0x%x): boot up ack timeout\n",  __func__,host->id, sts));
				err = MMC_ERR_TIMEOUT;
				goto out;
			} else if (sts & EMMC_STS_BOOTUPSTATE) {
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(%x): boot up mode state\n",  __func__, host->id, sts));
			} else {
				MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(0x%x): boot up unexpected\n",  __func__,host->id, sts));
			}
		} while (1);
	}
	//printf("ackdis(%d) err(%d)\n",ackdis,err);

	/* check if data received */
	do {
		sts = MSDC_READ32(EMMC_STS);
		if (sts == 0)
			continue;
		if (sts & EMMC_STS_BOOTDATRCV) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(0x%x): boot dat received\n",  __func__,host->id, sts));
			break;
		}
		if (sts & EMMC_STS_BOOTCRCERR) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(0x%x): boot up data crc error\n",  __func__,host->id, sts));
			err = MMC_ERR_BADCRC;
			goto out;
		} else if (sts & EMMC_STS_BOOTDATTMO) {
#if 0
			test_timer2 = MSDC_READ32(0x10008048);
			test_timer1 = (test_timer2 - test_timer1) /6000;
			MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(%x): boot up data timeout(%d s)\n",  __func__,host->id, sts,test_timer1));
			//test MT6583 ACK/DAT timeour modification test case
#endif
			MSDC_ERR_PRINT(MSDC_ERROR,("[%s]: [SD%d] EMMC_STS(0x%x): boot up data timeout\n",  __func__,host->id, sts));
			err = MMC_ERR_TIMEOUT;
			goto out;
		}
	} while (1);
out:
	return err;
}

void msdc_emmc_boot_stop(struct mmc_host *host)
{
	u32 base = host->base;
	u32 tmo = 0xFFFFFFFF;

	/* Step5. stop the boot mode */
	MSDC_WRITE32(SDC_ARG, 0x00000000);
	MSDC_WRITE32(SDC_CMD, 0x00001000);

	MSDC_SET_FIELD(EMMC_CFG0, EMMC_CFG0_BOOTWDLY, 2);
	MSDC_SET_BIT32(EMMC_CFG0, EMMC_CFG0_BOOTSTOP);
	WAIT_COND((MSDC_READ32(EMMC_STS) & EMMC_STS_BOOTUPSTATE) == 0, tmo, tmo);

	/* Step6. */
	MSDC_CLR_BIT32(EMMC_CFG0, EMMC_CFG0_BOOTSUPP);

	/* Step7. clear EMMC_STS bits */
	MSDC_WRITE32(EMMC_STS, MSDC_READ32(EMMC_STS));
}

int msdc_emmc_boot_read(struct mmc_host *host, u64 size, u32 *to, int read_mode)
{
	int err = MMC_ERR_NONE;
	int derr = MMC_ERR_NONE;
	u32 sts;
	u64 totalsz = size;
	u32 base = host->base;
	u64 left_sz, xfer_sz;

	msdc_priv_t *priv = (msdc_priv_t*)host->priv;
	struct dma_config *cfg = &priv->cfg;
	MSDC_ASSERT((MSDC_MODE_PIO <= read_mode) && (read_mode <= MSDC_MODE_DMA_DESC));

	if (read_mode == MSDC_MODE_PIO) {
		MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_PIO);
		while (size) {
			sts = MSDC_READ32(EMMC_STS);
			if (sts & EMMC_STS_BOOTCRCERR) {
				MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] EMMC_STS(0x%x): boot up data crc error\n", host->id, sts));
				err = MMC_ERR_BADCRC;
				goto out;
			} else if (sts & EMMC_STS_BOOTDATTMO) {
				MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] EMMC_STS(0x%x): boot up data timeout error\n", host->id, sts));
				err = MMC_ERR_TIMEOUT;
				goto out;
			}
			/* Note. RXFIFO count would be aligned to 4-bytes alignment size */
			if ((size >=  MSDC_FIFO_THD) && (MSDC_RXFIFOCNT() >= MSDC_FIFO_THD)) {
				int left = MSDC_FIFO_THD >> 2;
				do {
					*to++ = MSDC_FIFO_READ32();
				} while (--left);
				size -= MSDC_FIFO_THD;
				MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] Read %d bytes, RXFIFOCNT: %d,  Left: %d/%d\n",
				                           host->id, MSDC_FIFO_THD, MSDC_RXFIFOCNT(), size, totalsz));
			} else if ((size < MSDC_FIFO_THD) && MSDC_RXFIFOCNT() >= size) {
				while (size) {
					if (size > 3) {
						*to++ = MSDC_FIFO_READ32();
						size -= 4;
					} else {
						u32 val = MSDC_FIFO_READ32();
						memcpy(to, &val, size);
						size = 0;
					}
				}
				MSDC_DBG_PRINT(MSDC_DEBUG,("[SD%d] Read left bytes, RXFIFOCNT: %d, Left: %d/%d\n",
				                           host->id, MSDC_RXFIFOCNT(), size, totalsz));
			}
		}

out:
		if (err) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] EMMC_BOOT: read boot code fail(%d), FIFOCNT=%d\n",
			                           host->id, err, MSDC_RXFIFOCNT()));
		}
	} else {
		//MSDC_CLR_BIT32(MSDC_CFG, MSDC_CFG_PIO);
		cfg->mode = read_mode;
		left_sz = size;
		if (read_mode == MSDC_MODE_DMA_BASIC) {
			cfg->inboot = 1;
			xfer_sz = left_sz > MAX_DMA_CNT ? MAX_DMA_CNT : left_sz;
			//msdc_set_blknum(host, xfer_sz/512);
		} else {
			xfer_sz = left_sz;
		}

		while (left_sz) {

			u32 base = host->base;
			cfg->xfersz = xfer_sz;
			//printf("to (0x%x) xfer_sz(0x%x)\n",to,xfer_sz);
			if (cfg->mode == MSDC_MODE_DMA_BASIC) {
				cfg->sglen = 1;
				cfg->sg[0].addr = (u32)to;
				cfg->sg[0].len = xfer_sz;
				msdc_flush_membuf(to, xfer_sz);
			} else {

				cfg->sglen = msdc_sg_init(cfg->sg, to, xfer_sz);
				cfg->flags |= DMA_FLAG_EN_CHKSUM;
			}

			MSDC_DMA_ON();
			//printf("nblks(%d),xfer_sz(%d),left_sz(%d)\n",nblks,xfer_sz,left_sz);
			msdc_dma_config(host, cfg);
			if (left_sz - xfer_sz != 0)
				MSDC_SET_FIELD(MSDC_DMA_CTRL, MSDC_DMA_CTRL_LASTBUF, 0);
			msdc_dma_start(host);
			err = derr = msdc_dma_wait_done(host, 0xFFFFFFFF);

			msdc_dma_stop(host);
			msdc_flush_membuf(to, xfer_sz);
			if (err != MMC_ERR_NONE)
				goto done;
			to     =(u8*)to + xfer_sz;
			left_sz -= xfer_sz;

			/* left_sz > 0 only when in basic dma mode */
			if (left_sz) {
				xfer_sz  = (xfer_sz > left_sz) ? left_sz : xfer_sz;
			}
		}

done:
		if (derr != MMC_ERR_NONE) {
			MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] EMMC boot read error(%d)\n", host->id,derr));
			msdc_abort_handler(host, 1);
		}
	}

	return err;
}

void msdc_emmc_boot_reset(struct mmc_host *host, int reset)
{
	u32 base = host->base;
	u32 wints = MSDC_INT_CMDRDY | MSDC_INT_CMDTMO;
	u32 l_arg, l_cmd, status;
	u32 tmo=0xffffffff;

	switch (reset) {
		case EMMC_BOOT_PWR_RESET:
			msdc_hard_reset(host);
			break;
		case EMMC_BOOT_RST_N_SIG:
			if (msdc_cap[host->id].flags & MSDC_RST_PIN_EN) {
				/* set n_reset pin to low */
				MSDC_SET_BIT32(EMMC_IOCON, EMMC_IOCON_BOOTRST);

				/* tRSTW (RST_n pulse width) at least 1us */
				mdelay(1);

				/* set n_reset pin to high, mark this line if do boot ACK & boot DAT timeout test */
				MSDC_CLR_BIT32(EMMC_IOCON, EMMC_IOCON_BOOTRST);

				/* tRSCA (RST_n to command time) at least 200us,
				   tRSTH (RST_n high period) at least 1us */
				MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_CKPDN);
				mdelay(1);
				MSDC_CLR_BIT32(MSDC_CFG, MSDC_CFG_CKPDN);
			}
			break;
		case EMMC_BOOT_PRE_IDLE_CMD:
			/* bring emmc to pre-idle mode by software reset command. (MMCv4.41)*/
			SDC_SEND_CMD(0x0, 0xF0F0F0F0);

			/* read SDC_ARG & SDC_CMD for avoid buffered register */
			l_arg = MSDC_READ32(SDC_ARG);
			l_cmd = MSDC_READ32(SDC_CMD);

			/* check cmd0 is send */
			status = msdc_intr_wait(host, wints);
			if (status & MSDC_INT_CMDTMO) {
				MSDC_ERR_PRINT(MSDC_ERROR,("[SD%d] CMD0:ERR(CMDTO)\n", host->id));
			}

			mdelay(1); //need delay to make sure pre-idle
			break;
	}
}
#endif

#define msdc_set_pad_init(...)

int msdc_init(int id, struct mmc_host *host, int clksrc, int mode)
{
	u32 baddr[] = {MSDC0_BASE, MSDC1_BASE, MSDC2_BASE};
	u32 base = baddr[id];
	u32 hclksrc;

	msdc_priv_t *priv;
	struct dma_config *cfg;

	MSDC_ASSERT(id < MSDC_MAX_NUM);

	MSDC_TRC_PRINT(MSDC_INIT,("[%s]: msdc%d Host controller intialization start with %u mode \n", __func__, id,mode));
	clksrc = (clksrc == -1) ? msdc_cap[id].clk_src : clksrc;
	hclksrc = msdc_cap[id].hclk_src;
	MSDC_ASSERT((id < 3) && (clksrc < MSDC30_CLKSRC_MAX));
	MSDC_ASSERT(hclksrc < MSDC50_CLKSRC4HCLK_MAX);

	priv = &msdc_priv[id];
	cfg  = &priv->cfg;

	memset(priv, 0, sizeof(msdc_priv_t));

	host->id     = id;
	host->base   = base;
	host->pll_mux_clk  = clksrc;
	host->pll_mux_hclk = hclksrc;

#ifndef FPGA_PLATFORM
	host->f_max  = hclks_msdc30[clksrc];
#else
	host->f_max  = MSDC_MAX_SCLK;
#endif

	host->f_min  = MSDC_MIN_SCLK;
	host->blkbits= MMC_BLOCK_BITS;
	host->blklen = 0;
	host->priv   = (void*)priv;

	host->caps   = MMC_CAP_MULTIWRITE;

	if (msdc_cap[id].flags & MSDC_HIGHSPEED)
		host->caps |= (MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED);
#if defined(FEATURE_MMC_UHS1)
	if (msdc_cap[id].flags & MSDC_UHS1)
		host->caps |= MMC_CAP_SD_UHS1;
#endif
	if (msdc_cap[id].flags & MSDC_DDR)
		host->caps |= MMC_CAP_DDR;
	if (msdc_cap[id].data_pins == 4)
		host->caps |= MMC_CAP_4_BIT_DATA;
	if (msdc_cap[id].data_pins == 8)
		host->caps |= MMC_CAP_8_BIT_DATA | MMC_CAP_4_BIT_DATA;
	if (msdc_cap[id].flags & MSDC_HS200)
		host->caps |= MMC_CAP_EMMC_HS200;
	if (msdc_cap[id].flags & MSDC_HS400)
		host->caps |= MMC_CAP_EMMC_HS400;

	host->ocr_avail = MMC_VDD_27_36;
	/* msdc0 only support 1.8 IO */
	if (host->caps & (MMC_CAP_EMMC_HS200 | MMC_CAP_EMMC_HS400))
		host->ocr_avail |= MMC_VDD_165_195;


	host->max_hw_segs   = MAX_DMA_TRAN_SIZE/512;
	host->max_phys_segs = MAX_DMA_TRAN_SIZE/512;
	host->max_seg_size  = MAX_DMA_TRAN_SIZE;
	host->max_blk_size  = 2048;
	host->max_blk_count = 65535;
	host->app_cmd = 0;
	host->app_cmd_arg = 0;

	priv->rdsmpl        = msdc_cap[id].data_edge;
	priv->wdsmpl        = msdc_cap[id].data_edge;
	priv->rsmpl         = msdc_cap[id].cmd_edge;

#if defined(MSDC_ENABLE_DMA_MODE)
	cfg->sg      = &priv->sg[0];
	cfg->burstsz = MSDC_BRUST_64B;
	cfg->flags   = DMA_FLAG_NONE;
	cfg->mode    = mode;
	cfg->inboot  = 0;

	msdc_init_gpd_bd(host);
	priv->alloc_bd    = 0;
	priv->alloc_gpd   = 0;
	priv->active_head = NULL;
	priv->active_tail = NULL;
#endif

	// set current power level: VOL_1800 or VOL_3300
	host->cur_pwr = VOL_3300;

	msdc_clock(host, 1);
#ifndef FEATURE_EMMC_NOT_REINIT
	msdc_power(host, MMC_POWER_OFF);
	msdc_power(host, MMC_POWER_ON);
#endif

	/* set to SD/MMC mode */
	MSDC_SET_FIELD(MSDC_CFG, MSDC_CFG_MODE, MSDC_SDMMC);
	MSDC_SET_BIT32(MSDC_CFG, MSDC_CFG_PIO);

	MSDC_RESET();
	MSDC_CLR_FIFO();
	MSDC_CLR_INT();

	/* reset tuning parameter */
	MSDC_WRITE32(MSDC_PAD_TUNE, 0x0000000);
	MSDC_WRITE32(MSDC_DAT_RDDLY0, 0x00000000);
	MSDC_WRITE32(MSDC_DAT_RDDLY1, 0x00000000);
	MSDC_WRITE32(MSDC_IOCON, 0x00000000);
	MSDC_WRITE32(MSDC_PATCH_BIT0, 0x403C0046);

	/* High 16 bit = 0 mean Power KPI is on, open KPI exclude MSDC_CK_SD_CKGN[designer asked]
	 * bit6-7 ECO switch, enable it for SLT load test */
	//MSDC_WRITE32(MSDC_PATCH_BIT1, 0x100000C9);
	//MSDC_WRITE32(MSDC_PATCH_BIT1, 0xFFFE00C9); /* 2013-1-6 close KPI for e2 eco verify */
	//MSDC_PATCH_BIT1:WRDAT_CRCS_TA_CNTR need fix to 3'001 by default,(<50MHz) (>=50MHz set 3'001 as initial value is OK for tunning)
	//YD:CMD_RSP_TA_CNTR need fix to 3'001 by default(<50MHz)(>=50MHz set 3'001as initial value is OK for tunning)

	/* enable SDIO mode. it's must otherwise sdio command failed */
	MSDC_SET_BIT32(SDC_CFG, SDC_CFG_SDIO);

	/* disable detect SDIO device interupt function */
	MSDC_CLR_BIT32(SDC_CFG, SDC_CFG_SDIOIDE);

	/* enable wake up events */
#if defined(MMC_MSDC_DRV_CTP)
	MSDC_SET_BIT32(SDC_CFG, SDC_CFG_INSWKUP);
#endif

#ifdef MSDC_USE_PATCH_BIT2_TURNING_WITH_ASYNC
	MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_RESPWAIT,3);
	MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CRCSTSENSEL,2);
	MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_RESPSTSENSEL,2);
	MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS,1);
	MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGRESP,0);
#else
	MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGCRCSTS,0);
	MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_CFGRESP,1);
#endif

	/* support data tune */
	MSDC_CLR_BIT32(MSDC_PAD_TUNE, MSDC_PAD_TUNE_RXDLYSEL);
	MSDC_SET_BIT32(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRDLYSEL);
	MSDC_SET_BIT32(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATRRDLYSEL);

	/* support stop clk */
	MSDC_CLR_BIT32(SDC_FIFO_CFG, SDC_FIFO_CFG_WR_VALID_SEL);
	MSDC_CLR_BIT32(SDC_FIFO_CFG, SDC_FIFO_CFG_RD_VALID_SEL);
	MSDC_SET_FIELD(MSDC_PATCH_BIT1, MSDC_PB1_STOP_DLY_SEL, 6);
	MSDC_SET_FIELD(MSDC_PATCH_BIT2, MSDC_PB2_POPENCNT, 0);

	/* disable busy check */
	MSDC_CLR_BIT32(MSDC_PATCH_BIT1, MSDC_PB1_BUSY_CHECK_SEL);
	MSDC_SET_BIT32(MSDC_PATCH_BIT1, MSDC_PB1_EN_SINGLE_BURST);

#ifndef FPGA_PLATFORM
	/* set clk, cmd, dat pad driving */
	msdc_set_driving(host, &msdc_cap[host->id], (host->cur_pwr == VOL_1800));
	msdc_set_rdtdsel(host,0);
	msdc_set_pad_init(host);
	msdc_set_smt(host, 1);
#endif
	/* disable boot function, else eMMC intialization may be failed after BROM ops. */
	MSDC_CLR_BIT32(EMMC_CFG0, EMMC_CFG0_BOOTSUPP);

	/* set sampling edge */
	MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_RSPL, msdc_cap[host->id].cmd_edge);
	MSDC_SET_FIELD(MSDC_IOCON, MSDC_IOCON_R_D_SMPL, msdc_cap[host->id].data_edge);

#if 0
	/* write crc timeout detection */
	MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_DETWR_CRCTMO, 1);

#if defined(MMC_MSDC_DRV_CTP)
#if (MSDC_USE_FORCE_FLUSH || MSDC_USE_RELIABLE_WRITE || MSDC_USE_DATA_TAG || MSDC_USE_PACKED_CMD)
	MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_BLKNUM_SEL, 0);
#else
	MSDC_SET_FIELD(MSDC_PATCH_BIT0, MSDC_PB0_BLKNUM_SEL, 1);
#endif
#endif
#endif
	msdc_set_startbit(host, START_AT_RISING);
#ifndef FEATURE_EMMC_NOT_REINIT
	msdc_config_bus(host, HOST_BUS_WIDTH_1);
	msdc_config_clock(host, 0, MSDC_MIN_SCLK, 0);
#endif
	msdc_set_dmode(host, mode);
	msdc_set_pio_bits(host, 32);

	/* disable sdio interrupt by default. sdio interrupt enable upon request */
	msdc_intr_unmask(host, MSDC_INTEN_DFT);

	msdc_irq_init(host);

	msdc_set_timeout(host, 100000000, 0);
#if defined(FEATURE_MMC_CARD_DETECT)
	msdc_card_detect(host, 1);
#endif
#if defined(MSDC_USE_DCM)
	dcm_disable(ALL_DCM);
	dcm_enable(MSDC_DCM);
#endif

	if ((host->id == 0) || (host->id == 1)) {
		/* disable SDIO func */
		MSDC_SET_FIELD(SDC_CFG, SDC_CFG_SDIO, 0);
		MSDC_SET_FIELD(SDC_CFG, SDC_CFG_SDIOIDE, 0);
		MSDC_SET_FIELD(SDC_CFG, SDC_CFG_INSWKUP, 0);
	}

	MSDC_TRC_PRINT(MSDC_INIT,("[%s]: msdc%d Host controller intialization done\n", __func__, id));
	return 0;
}

#if defined(MSDC_WITH_DEINIT)
int msdc_deinit(struct mmc_host *host)
{
	u32 base = host->base;

#if defined(FEATURE_MMC_CARD_DETECT)
	msdc_card_detect(host, 0);
#endif

	msdc_intr_mask(host, 0x0001FFFB);
	msdc_irq_deinit(host);

	MSDC_RESET();
	MSDC_CLR_FIFO();
	MSDC_CLR_INT();
	msdc_power(host, MMC_POWER_OFF);

	return 0;
}
#endif

int msdc_polling_CD_interrupt(struct mmc_host *host)
{
	u32 base = host->base;
	u32 intsts;
	intsts = MSDC_READ32(MSDC_INT);
	MSDC_WRITE32(MSDC_INT, intsts);
	//printf("SDIO INT(0x%x)\n",intsts);
	if (intsts & MSDC_INT_CDSC)
		return 1;
	else
		return 0;
}
