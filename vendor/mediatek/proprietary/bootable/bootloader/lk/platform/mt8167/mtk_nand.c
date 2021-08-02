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

#include <string.h>
#include <config.h>
#include <malloc.h>
#include <printf.h>
#include <platform/mt_typedefs.h>
#include <platform/mtk_nand.h>
#include <mt_partition.h>
#include <platform/bmt.h>
#include "partition_define.h"
#include "cust_nand.h"
#include <arch/ops.h>
#include <nand_device_list.h>
#include <kernel/event.h>
#include <platform/mt_irq.h>
#include <platform/mt_gpio.h>

/* #define NAND_LK_TEST */
#ifdef NAND_LK_TEST
#include "mt_partition.h"
#endif
#if defined(MTK_COMBO_NAND_SUPPORT)
/* BMT_POOL_SIZE is not used anymore */
#else
#ifndef PART_SIZE_BMTPOOL
#define BMT_POOL_SIZE (80)
#else
#define BMT_POOL_SIZE (PART_SIZE_BMTPOOL)
#endif
#endif
#define CFG_RANDOMIZER    (1)   /* for randomizer code */
#define CFG_2CS_NAND    (1) /* for 2CS nand */
#define CFG_COMBO_NAND  (1) /* for Combo nand */

#define NFI_TRICKY_CS  (1)  /* must be 1 or > 1? */

#define PMT_POOL_SIZE   (2)

#define STATUS_READY            (0x40)
#define STATUS_FAIL             (0x01)
#define STATUS_WR_ALLOW         (0x80)

#define MLC_MICRON_SLC_MODE (0)

#ifdef CONFIG_CMD_NAND
extern int mt_part_register_device(part_dev_t *dev);

struct nand_ecclayout nand_oob_16 = {
	.eccbytes = 8,
	.eccpos = {8, 9, 10, 11, 12, 13, 14, 15},
	.oobfree = {{1, 6}, {0, 0} }
};

struct nand_ecclayout nand_oob_64 = {
	.eccbytes = 32,
	.eccpos = {
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63
	},
	.oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 6}, {0, 0} }
};

struct nand_ecclayout nand_oob_128 = {
	.eccbytes = 64,
	.eccpos = {
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 86,
		88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127
	},
	.oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 7}, {33, 7}, {41, 7}, {49, 7},
		{57, 6}
	}
};

static bmt_struct *g_bmt = NULL;
static struct nand_chip g_nand_chip;
static int en_interrupt = 0;
static event_t nand_int_event;
#if defined(MTK_TLC_NAND_SUPPORT)
bool tlc_lg_left_plane = TRUE; //logical left plane of tlc nand.  used to do page program
NFI_TLC_PG_CYCLE tlc_program_cycle;
bool tlc_not_keep_erase_lvl = FALSE;//not keep erase level
#endif
/* flashdev_info gen_FlashTable_p[MAX_FLASH]; */

#if CFG_2CS_NAND
static bool g_bTricky_CS = FALSE;   /* for nandbase.c */
static bool g_b2Die_CS = FALSE;
static u32 g_nanddie_pages;
#endif

bool tlc_cache_program = FALSE; //whether use cache program
bool tlc_snd_phyplane = FALSE; // second plane operation

u32 slc_ratio = 6; //slc mode block ration in FS partition. means slc_ration %
u32 sys_slc_ratio = 6;
u32 usr_slc_ratio = 6;
extern void mtk_pmt_reset(void);
extern void mtk_slc_blk_addr(u64 addr, u32* blk_num, u32* page_in_block);
extern bool mtk_block_istlc(u64 addr);
extern bool mtk_nand_IsBMTPOOL(u64 logical_address);

#define ERR_RTN_SUCCESS   1
#define ERR_RTN_FAIL      0
#define ERR_RTN_BCH_FAIL -1
u32 BLOCK_SIZE;
static u32 PAGES_PER_BLOCK = 255;

#define NFI_ISSUE_COMMAND(cmd, col_addr, row_addr, col_num, row_num) \
   do { \
      DRV_WriteReg(NFI_CMD_REG16, cmd);\
      while (DRV_Reg32(NFI_STA_REG32) & STA_CMD_STATE);\
      DRV_WriteReg32(NFI_COLADDR_REG32, col_addr);\
      DRV_WriteReg32(NFI_ROWADDR_REG32, row_addr);\
      DRV_WriteReg(NFI_ADDRNOB_REG16, col_num | (row_num<<ADDR_ROW_NOB_SHIFT));\
      while (DRV_Reg32(NFI_STA_REG32) & STA_ADDR_STATE);\
   } while (0);

flashdev_info devinfo;

#define CHIPVER_ECO_1 (0x8a00)
#define CHIPVER_ECO_2 (0x8a01)
#define RAND_TYPE_SAMSUNG 0
#define RAND_TYPE_TOSHIBA 1
#define RAND_TYPE_NONE 2
extern u64 part_get_startaddress(u64 byte_address, u32 *idx);
extern bool raw_partition(u32 index);
bool __nand_erase(u64 logical_addr);
bool mark_block_bad(u64 logical_addr);
bool nand_erase_hw(u64 offset);
int check_data_empty(void *data, unsigned size);

struct NAND_CMD g_kCMD;
static u32 g_i4ErrNum;
static bool g_bInitDone;
u64 total_size;
u64 g_nand_size = 0;

#define LPAGE 32768
#define LSPARE 4096

__attribute__ ((aligned(64)))
static unsigned char g_data_buf[LPAGE + LSPARE];
__attribute__ ((aligned(64)))
static struct nand_buffers nBuf;
__attribute__ ((aligned(64)))
static unsigned char data_buf_temp[LPAGE];
__attribute__ ((aligned(64)))
static unsigned char oob_buf_temp[LSPARE];
#if defined(MTK_TLC_NAND_SUPPORT)
__attribute__ ((aligned(64))) static unsigned char local_tlc_wl_buffer[LPAGE];
#endif
enum flashdev_vendor gVendor;
unsigned char g_spare_buf[LSPARE];
static u64 download_size = 0;

u32 MICRON_TRANSFER(u32 pageNo);
u32 SANDISK_TRANSFER(u32 pageNo);
u32 HYNIX_TRANSFER(u32 pageNo);

typedef u32(*GetLowPageNumber) (u32 pageNo);

GetLowPageNumber functArray[] = {
	MICRON_TRANSFER,
	HYNIX_TRANSFER,
	SANDISK_TRANSFER,
};

u32 SANDISK_TRANSFER(u32 pageNo)
{
	if (0 == pageNo) {
		return pageNo;
	} else {
		return pageNo + pageNo - 1;
	}
}

u32 HYNIX_TRANSFER(u32 pageNo)
{
	u32 temp;
	if (pageNo < 4)
		return pageNo;
	temp = pageNo + (pageNo & 0xFFFFFFFE) - 2;
	return temp;
}

u32 MICRON_TRANSFER(u32 pageNo)
{
	u32 temp;
	if (pageNo < 4)
		return pageNo;
	temp = (pageNo - 4) & 0xFFFFFFFE;
	if (pageNo <= 130)
		return (pageNo + temp);
	else
		return (pageNo + temp - 2);
}

/**************************************************************************
*  Randomizer
**************************************************************************/
#define SS_SEED_NUM 128
#define EFUSE_RANDOM_CFG    ((volatile u32 *)(0x10009020))
#define EFUSE_RANDOM_ENABLE 0x00001000
static bool use_randomizer = FALSE;
static bool pre_randomizer = FALSE;

static U16 SS_RANDOM_SEED[SS_SEED_NUM] = {
	/* for page 0~127 */
	0x576A, 0x05E8, 0x629D, 0x45A3, 0x649C, 0x4BF0, 0x2342, 0x272E,
	0x7358, 0x4FF3, 0x73EC, 0x5F70, 0x7A60, 0x1AD8, 0x3472, 0x3612,
	0x224F, 0x0454, 0x030E, 0x70A5, 0x7809, 0x2521, 0x484F, 0x5A2D,
	0x492A, 0x043D, 0x7F61, 0x3969, 0x517A, 0x3B42, 0x769D, 0x0647,
	0x7E2A, 0x1383, 0x49D9, 0x07B8, 0x2578, 0x4EEC, 0x4423, 0x352F,
	0x5B22, 0x72B9, 0x367B, 0x24B6, 0x7E8E, 0x2318, 0x6BD0, 0x5519,
	0x1783, 0x18A7, 0x7B6E, 0x7602, 0x4B7F, 0x3648, 0x2C53, 0x6B99,
	0x0C23, 0x67CF, 0x7E0E, 0x4D8C, 0x5079, 0x209D, 0x244A, 0x747B,
	0x350B, 0x0E4D, 0x7004, 0x6AC3, 0x7F3E, 0x21F5, 0x7A15, 0x2379,
	0x1517, 0x1ABA, 0x4E77, 0x15A1, 0x04FA, 0x2D61, 0x253A, 0x1302,
	0x1F63, 0x5AB3, 0x049A, 0x5AE8, 0x1CD7, 0x4A00, 0x30C8, 0x3247,
	0x729C, 0x5034, 0x2B0E, 0x57F2, 0x00E4, 0x575B, 0x6192, 0x38F8,
	0x2F6A, 0x0C14, 0x45FC, 0x41DF, 0x38DA, 0x7AE1, 0x7322, 0x62DF,
	0x5E39, 0x0E64, 0x6D85, 0x5951, 0x5937, 0x6281, 0x33A1, 0x6A32,
	0x3A5A, 0x2BAC, 0x743A, 0x5E74, 0x3B2E, 0x7EC7, 0x4FD2, 0x5D28,
	0x751F, 0x3EF8, 0x39B1, 0x4E49, 0x746B, 0x6EF6, 0x44BE, 0x6DB7
};

static bool mtk_nand_read_status(void);
#if CFG_2CS_NAND
static int mtk_nand_cs_check(u8 *id, u16 cs);
static u32 mtk_nand_cs_on(u16 cs, u32 page);
#endif
static inline unsigned int uffs(unsigned int x)
{
	unsigned int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}

void dump_nfi(void)
{
	printf("~~~~Dump NFI Register in LK~~~~\n");
	printf("NFI_CNFG_REG16: 0x%x\n", DRV_Reg16(NFI_CNFG_REG16));
	printf("NFI_PAGEFMT_REG32: 0x%x\n", DRV_Reg32(NFI_PAGEFMT_REG32));
	printf("NFI_CON_REG16: 0x%x\n", DRV_Reg16(NFI_CON_REG16));
	printf("NFI_ACCCON_REG32: 0x%x\n", DRV_Reg32(NFI_ACCCON_REG32));
	printf("NFI_INTR_EN_REG16: 0x%x\n", DRV_Reg16(NFI_INTR_EN_REG16));
	printf("NFI_INTR_REG16: 0x%x\n", DRV_Reg16(NFI_INTR_REG16));
	printf("NFI_CMD_REG16: 0x%x\n", DRV_Reg16(NFI_CMD_REG16));
	printf("NFI_ADDRNOB_REG16: 0x%x\n", DRV_Reg16(NFI_ADDRNOB_REG16));
	printf("NFI_COLADDR_REG32: 0x%x\n", DRV_Reg32(NFI_COLADDR_REG32));
	printf("NFI_ROWADDR_REG32: 0x%x\n", DRV_Reg32(NFI_ROWADDR_REG32));
	printf("NFI_STRDATA_REG16: 0x%x\n", DRV_Reg16(NFI_STRDATA_REG16));
	printf("NFI_DATAW_REG32: 0x%x\n", DRV_Reg32(NFI_DATAW_REG32));
	printf("NFI_DATAR_REG32: 0x%x\n", DRV_Reg32(NFI_DATAR_REG32));
	printf("NFI_PIO_DIRDY_REG16: 0x%x\n", DRV_Reg16(NFI_PIO_DIRDY_REG16));
	printf("NFI_STA_REG32: 0x%x\n", DRV_Reg32(NFI_STA_REG32));
	printf("NFI_FIFOSTA_REG16: 0x%x\n", DRV_Reg16(NFI_FIFOSTA_REG16));
	/* printf("NFI_LOCKSTA_REG16: 0x%x\n", DRV_Reg16(NFI_LOCKSTA_REG16)); */
	printf("NFI_ADDRCNTR_REG16: 0x%x\n", DRV_Reg16(NFI_ADDRCNTR_REG16));
	printf("NFI_STRADDR_REG32: 0x%x\n", DRV_Reg32(NFI_STRADDR_REG32));
	printf("NFI_BYTELEN_REG16: 0x%x\n", DRV_Reg16(NFI_BYTELEN_REG16));
	printf("NFI_CSEL_REG16: 0x%x\n", DRV_Reg16(NFI_CSEL_REG16));
	printf("NFI_IOCON_REG16: 0x%x\n", DRV_Reg16(NFI_IOCON_REG16));
	printf("NFI_FDM0L_REG32: 0x%x\n", DRV_Reg32(NFI_FDM0L_REG32));
	printf("NFI_FDM0M_REG32: 0x%x\n", DRV_Reg32(NFI_FDM0M_REG32));
	printf("NFI_LOCK_REG16: 0x%x\n", DRV_Reg16(NFI_LOCK_REG16));
	printf("NFI_LOCKCON_REG32: 0x%x\n", DRV_Reg32(NFI_LOCKCON_REG32));
	printf("NFI_LOCKANOB_REG16: 0x%x\n", DRV_Reg16(NFI_LOCKANOB_REG16));
	printf("NFI_FIFODATA0_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA0_REG32));
	printf("NFI_FIFODATA1_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA1_REG32));
	printf("NFI_FIFODATA2_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA2_REG32));
	printf("NFI_FIFODATA3_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA3_REG32));
	printf("NFI_MASTERSTA_REG16: 0x%x\n", DRV_Reg16(NFI_MASTERSTA_REG16));
	/* printf("NFI clock register: 0x%x: %s\n",(PERI_CON_BASE+0x18), (DRV_Reg32((volatile u32 *)(PERI_CON_BASE+0x18)) & (0x1)) ? "Clock Disabled" : "Clock Enabled"); */
	/* printf("NFI clock SEL (MT8127):0x%x: %s\n",(PERI_CON_BASE+0x5C), (DRV_Reg32((volatile u32 *)(PERI_CON_BASE+0x5C)) & (0x1)) ? "Half clock" : "Quarter clock"); */
}

u32 mtk_nand_page_transform(u64 logical_address, u32 *blk, u32 *map_blk)
{
	u64 start_address;
	u32 index = 0;
	u32 block;
	u32 page_in_block;
	u32 mapped_block;

	devinfo.tlcControl.slcopmodeEn = FALSE;

	/* if(g_nand_chip.sector_size == 1024) */
	if (VEND_NONE != gVendor) {
		start_address = part_get_startaddress((u64) logical_address, &index);

		/* MSG(ERR, "start_address(0x%x), logical_address(0x%x) index(%d)\n",start_address,logical_address,index); */
		if ((0xFFFFFFFF != index) && (raw_partition(index))) {
			/* if(start_address == 0xFFFFFFFF) */
			/* while(1); */
			/* MSG(ERR, "raw_partition(%d)\n",index); */
#if defined(MTK_TLC_NAND_SUPPORT)
			if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
				if (devinfo.tlcControl.normaltlc) {
					block = (u32)((u32)(start_address / BLOCK_SIZE) + (u32)((logical_address-start_address) / (BLOCK_SIZE / 3)));
					page_in_block = ((u32)((logical_address-start_address) >> g_nand_chip.page_shift) % ((BLOCK_SIZE/g_nand_chip.page_size)/3));
					page_in_block *= 3;
					devinfo.tlcControl.slcopmodeEn = TRUE;
				} else {
					block = (u32)((u32)(start_address / BLOCK_SIZE) + (u32)((logical_address-start_address)/ (BLOCK_SIZE / 3)));
					page_in_block = ((u32)((logical_address-start_address) >> g_nand_chip.page_shift) % ((BLOCK_SIZE/g_nand_chip.page_size)/3));
					//MSG(INIT , "[LOW]0x%x, 0x%x\n",block,page_in_block);

					if (devinfo.vendor != VEND_NONE) {
						//page_in_block = devinfo.feature_set.PairPage[page_in_block];
						page_in_block = functArray[devinfo.feature_set.ptbl_idx](page_in_block);
					}
				}
			} else
#endif
			{
				block = (u32) ((start_address / BLOCK_SIZE) + (logical_address - start_address) / g_nand_chip.erasesize);
				page_in_block =
				    (u32) (((logical_address -
				             start_address) / g_nand_chip.page_size) % ((1 << (g_nand_chip.phys_erase_shift - g_nand_chip.page_shift))));

				if (devinfo.vendor != VEND_NONE) {
					/* page_in_block = devinfo.feature_set.PairPage[page_in_block]; */
					page_in_block = functArray[devinfo.feature_set.ptbl_idx] (page_in_block);
				}
			}
			mapped_block = get_mapping_block_index(block);
			/* MSG(ERR, "transform_address(0x%x)\n",mapped_block*(BLOCK_SIZE/(g_nand_chip.page_size))+page_in_block); */
		} else {
			if (((devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER)
			        ||(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC))
			        && (!mtk_block_istlc(logical_address))) {
				mtk_slc_blk_addr(logical_address, &block, &page_in_block);
				devinfo.tlcControl.slcopmodeEn = TRUE;
				printf(INFO,"devinfo.tlcControl.slcopmodeEn = TRUE\n");
			} else {
				block = (u32)(logical_address/BLOCK_SIZE);
				page_in_block = (u32)((logical_address/g_nand_chip.page_size) % (BLOCK_SIZE >> g_nand_chip.page_shift));
			}
			mapped_block = get_mapping_block_index(block);
		}
	} else {
		block = (u32) (logical_address / BLOCK_SIZE);
		mapped_block = get_mapping_block_index(block);
		page_in_block = (u32) ((logical_address / g_nand_chip.page_size) % (BLOCK_SIZE >> g_nand_chip.page_shift));
	}
	*blk = block;
	*map_blk = mapped_block;
	return mapped_block * (BLOCK_SIZE / (g_nand_chip.page_size)) + page_in_block;
}

bool get_device_info(u8 *id, flashdev_info *devinfo)
{
	u32 i, m, n, mismatch;
	int target = -1;
	u8 target_id_len = 0;
	/* printf("[xiaolei] [lk nand] flash number %d\n", flash_number); */
	for (i = 0; i < flash_number; i++) {
		mismatch = 0;
		for (m = 0; m < gen_FlashTable_p[i].id_length; m++) {
			if (id[m] != gen_FlashTable_p[i].id[m]) {
				mismatch = 1;
				/* printf("[xiaolei] [lk nand] ID %d %d\n",id[m], gen_FlashTable_p[i].id[m]); */
				break;
			}
		}
		if (mismatch == 0 && gen_FlashTable_p[i].id_length > target_id_len) {
			target = i;
			target_id_len = gen_FlashTable_p[i].id_length;
		}
	}

	if (target != -1) {
		MSG(INIT, "Recognize NAND: ID [");
		for (n = 0; n < gen_FlashTable_p[target].id_length; n++) {
			devinfo->id[n] = gen_FlashTable_p[target].id[n];
			MSG(INIT, "%x ", devinfo->id[n]);
		}
		if (devinfo->NAND_FLASH_TYPE == NAND_FLASH_TLC)
			printf( "], Device Name [%s], Page Size [%d]B Spare Size [%d]B Total Size [%d]KB\n",
			        gen_FlashTable_p[target].devciename, gen_FlashTable_p[target].pagesize,
			        gen_FlashTable_p[target].sparesize, gen_FlashTable_p[target].totalsize);
		else
			printf("], Device Name [%s], Page Size [%d]B Spare Size [%d]B Total Size [%d]MB\n",
			       gen_FlashTable_p[target].devciename, gen_FlashTable_p[target].pagesize,
			       gen_FlashTable_p[target].sparesize, gen_FlashTable_p[target].totalsize);
		devinfo->id_length = gen_FlashTable_p[target].id_length;
		devinfo->blocksize = gen_FlashTable_p[target].blocksize;
		devinfo->addr_cycle = gen_FlashTable_p[target].addr_cycle;
		devinfo->iowidth = gen_FlashTable_p[target].iowidth;
		devinfo->timmingsetting = gen_FlashTable_p[target].timmingsetting;
		devinfo->advancedmode = gen_FlashTable_p[target].advancedmode;
		devinfo->pagesize = gen_FlashTable_p[target].pagesize;
		devinfo->sparesize = gen_FlashTable_p[target].sparesize;
		devinfo->totalsize = gen_FlashTable_p[target].totalsize;
		devinfo->sectorsize = gen_FlashTable_p[target].sectorsize;
		devinfo->s_acccon = gen_FlashTable_p[target].s_acccon;
		devinfo->s_acccon1 = gen_FlashTable_p[target].s_acccon1;
		devinfo->freq = gen_FlashTable_p[target].freq;
		devinfo->vendor = gen_FlashTable_p[target].vendor;

		/* devinfo->ttarget = gen_FlashTable_p[target].ttarget; */
		gVendor = gen_FlashTable_p[target].vendor;
		memcpy((u8 *) &devinfo->feature_set, (u8 *) &gen_FlashTable_p[target].feature_set, sizeof(struct MLC_feature_set));
		memcpy(devinfo->devciename, gen_FlashTable_p[target].devciename, sizeof(devinfo->devciename));

		devinfo->NAND_FLASH_TYPE = gen_FlashTable_p[target].NAND_FLASH_TYPE;
		memcpy((u8*)&devinfo->tlcControl, (u8*)&gen_FlashTable_p[target].tlcControl, sizeof(NFI_TLC_CTRL));
		devinfo->two_phyplane = gen_FlashTable_p[target].two_phyplane;
		if (devinfo->two_phyplane)
			MSG(INIT, "this is 2p NAND\n");
		return true;
	} else {
		MSG(INIT, "Not Found NAND: ID [");
		for (n = 0; n < NAND_MAX_ID; n++) {
			MSG(INIT, "%x ", id[n]);
		}
		MSG(INIT, "]\n");
		return false;
	}
}

#if defined(MTK_TLC_NAND_SUPPORT)
bool mtk_is_normal_tlc_nand(void)
{
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc))
		return TRUE;
	else
		return FALSE;
}

//tlc releated functions
void NFI_TLC_GetMappedWL(u32 pageidx, NFI_TLC_WL_INFO* WL_Info)
{
	//this function is just for normal tlc
	WL_Info->word_line_idx = pageidx / 3;
	WL_Info->wl_pre = (NFI_TLC_WL_PRE)(pageidx % 3);
}

u32 NFI_TLC_GetRowAddr(u32 rowaddr)
{
	u32 real_row;
	u32 temp = 0xFF;
	int page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;

	if (devinfo.tlcControl.normaltlc) { // normal tlc
		temp = page_per_block / 3;
	} else {
		temp = page_per_block;
	}

	real_row = ((rowaddr / temp) << devinfo.tlcControl.block_bit) | (rowaddr % temp);

	return real_row;
}

u32 NFI_TLC_SetpPlaneAddr(u32 rowaddr, bool left_plane)
{
	//this function is just for normal tlc
	u32 real_row;

	if (devinfo.tlcControl.pPlaneEn) {
		if (left_plane)
			real_row = (rowaddr & (~(1 << devinfo.tlcControl.pPlane_bit)));
		else
			real_row = (rowaddr | (1 << devinfo.tlcControl.pPlane_bit));
	} else
		real_row = rowaddr;

	return real_row;
}

u32 NFI_TLC_GetMappedPgAddr(u32 rowaddr) //rowaddr is the real address, the return value is the page increased by degree (pageidx = block no * page per block)
{
	u32 page_idx;
	u32 page_shift = 0;
	u32 real_row;
	int page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;

	real_row = rowaddr;

	if (devinfo.tlcControl.normaltlc) { // normal tlc
		page_shift = devinfo.tlcControl.block_bit;
		if (devinfo.tlcControl.pPlaneEn) {
			real_row &= (~(1 << devinfo.tlcControl.pPlane_bit));
		}
		page_idx = ((real_row >> page_shift) * page_per_block) + (((real_row << (32-page_shift)) >> (32-page_shift)) * 3); //always get wl's low page
	} else { //micron tlc
		page_shift = devinfo.tlcControl.block_bit;

		page_idx = ((real_row >> page_shift) * page_per_block) + ((real_row << (32-page_shift)) >> (32-page_shift));
	}

	return page_idx;
}
#endif
#if CFG_RANDOMIZER
static int mtk_nand_turn_on_randomizer(u32 page, int type, int fgPage)
{
	u32 u4NFI_CFG = 0;
	u32 u4NFI_RAN_CFG = 0;
	u32 u4PgNum = page % PAGES_PER_BLOCK; // to fit flash which page per block is not 32 aligned
	u4NFI_CFG = DRV_Reg32(NFI_CNFG_REG16);

	DRV_WriteReg32(NFI_ENMPTY_THRESH_REG32, 40);    /* empty threshold 40 */

	if (type) {     /* encode */
		DRV_WriteReg32(NFI_RANDOM_ENSEED01_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_ENSEED02_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_ENSEED03_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_ENSEED04_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_ENSEED05_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_ENSEED06_TS_REG32, 0);
	} else {
		DRV_WriteReg32(NFI_RANDOM_DESEED01_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_DESEED02_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_DESEED03_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_DESEED04_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_DESEED05_TS_REG32, 0);
		DRV_WriteReg32(NFI_RANDOM_DESEED06_TS_REG32, 0);
	}
	u4NFI_CFG |= CNFG_RAN_SEL;
	if (PAGES_PER_BLOCK <= SS_SEED_NUM) {
		if (type) {
			u4NFI_RAN_CFG |= RAN_CNFG_ENCODE_SEED(SS_RANDOM_SEED[u4PgNum % PAGES_PER_BLOCK]) | RAN_CNFG_ENCODE_EN;
		} else {
			u4NFI_RAN_CFG |= RAN_CNFG_DECODE_SEED(SS_RANDOM_SEED[u4PgNum % PAGES_PER_BLOCK]) | RAN_CNFG_DECODE_EN;
		}
	} else {
		if (type) {
			u4NFI_RAN_CFG |= RAN_CNFG_ENCODE_SEED(SS_RANDOM_SEED[u4PgNum & (SS_SEED_NUM-1)]) | RAN_CNFG_ENCODE_EN;
		} else {
			u4NFI_RAN_CFG |= RAN_CNFG_DECODE_SEED(SS_RANDOM_SEED[u4PgNum & (SS_SEED_NUM-1)]) | RAN_CNFG_DECODE_EN;
		}
	}

	if (fgPage)     /* reload seed for each page */
		u4NFI_CFG &= ~CNFG_RAN_SEC;
	else            /* reload seed for each sector */
		u4NFI_CFG |= CNFG_RAN_SEC;

	DRV_WriteReg32(NFI_CNFG_REG16, u4NFI_CFG);
	DRV_WriteReg32(NFI_RANDOM_CNFG_REG32, u4NFI_RAN_CFG);
	/* printf("[LK]ran turn on type:%d 0x%x 0x%x\n", type, DRV_Reg32(NFI_RANDOM_CNFG_REG32), page); */
	return 0;
}

static bool mtk_nand_israndomizeron(void)
{
	u32 nfi_ran_cnfg = 0;
	nfi_ran_cnfg = DRV_Reg32(NFI_RANDOM_CNFG_REG32);
	if (nfi_ran_cnfg & (RAN_CNFG_ENCODE_EN | RAN_CNFG_DECODE_EN))
		return TRUE;

	return FALSE;
}

static void mtk_nand_turn_off_randomizer(void)
{
	u32 u4NFI_CFG = DRV_Reg32(NFI_CNFG_REG16);
	u4NFI_CFG &= ~CNFG_RAN_SEL;
	u4NFI_CFG &= ~CNFG_RAN_SEC;
	DRV_WriteReg32(NFI_RANDOM_CNFG_REG32, 0);
	DRV_WriteReg32(NFI_CNFG_REG16, u4NFI_CFG);
	/* printf("[LK]ran turn off\n"); */
}
#else
#define mtk_nand_israndomizeron() (FALSE)
#define mtk_nand_turn_on_randomizer(page, type, fgPage)
#define mtk_nand_turn_off_randomizer()
#endif

static void ECC_Config(u32 ecc_level)
{
	u32 u4ENCODESize;
	u32 u4DECODESize;
	u32 ecc_bit_cfg = 0;
	u32 sector_size = NAND_SECTOR_SIZE;
	if (devinfo.sectorsize == 1024)
		sector_size = 1024;
	u4DECODESize = ((sector_size + g_nand_chip.nand_fdm_size) << 3) + ecc_level * ECC_PARITY_BIT;
	switch (ecc_level) {
#ifndef MTK_COMBO_NAND_SUPPORT
		case 4:
			ecc_bit_cfg = ECC_CNFG_ECC4;
			break;
		case 8:
			ecc_bit_cfg = ECC_CNFG_ECC8;
			break;
		case 10:
			ecc_bit_cfg = ECC_CNFG_ECC10;
			break;
		case 12:
			ecc_bit_cfg = ECC_CNFG_ECC12;
			break;
		case 14:
			ecc_bit_cfg = ECC_CNFG_ECC14;
			break;
		case 16:
			ecc_bit_cfg = ECC_CNFG_ECC16;
			break;
		case 18:
			ecc_bit_cfg = ECC_CNFG_ECC18;
			break;
		case 20:
			ecc_bit_cfg = ECC_CNFG_ECC20;
			break;
		case 22:
			ecc_bit_cfg = ECC_CNFG_ECC22;
			break;
		case 24:
			ecc_bit_cfg = ECC_CNFG_ECC24;
			break;
#endif
		case 28:
			ecc_bit_cfg = ECC_CNFG_ECC28;
			break;
		case 32:
			ecc_bit_cfg = ECC_CNFG_ECC32;
			break;
		case 36:
			ecc_bit_cfg = ECC_CNFG_ECC36;
			break;
		case 40:
			ecc_bit_cfg = ECC_CNFG_ECC40;
			break;
		case 44:
			ecc_bit_cfg = ECC_CNFG_ECC44;
			break;
		case 48:
			ecc_bit_cfg = ECC_CNFG_ECC48;
			break;
		case 52:
			ecc_bit_cfg = ECC_CNFG_ECC52;
			break;
		case 56:
			ecc_bit_cfg = ECC_CNFG_ECC56;
			break;
		case 60:
			ecc_bit_cfg = ECC_CNFG_ECC60;
			break;
#if defined(MTK_TLC_NAND_SUPPORT)
		case 68:
			ecc_bit_cfg = ECC_CNFG_ECC68;
			u4DECODESize -= 7;
			break;
		case 72:
			ecc_bit_cfg = ECC_CNFG_ECC72;
			u4DECODESize -= 7;
			break;
		case 80:
			ecc_bit_cfg = ECC_CNFG_ECC80;
			u4DECODESize -= 7;
			break;
#endif
		default:
			break;

	}
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
	do {
		;
	} while (!DRV_Reg16(ECC_DECIDLE_REG16));

	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
	do {
		;
	} while (!DRV_Reg32(ECC_ENCIDLE_REG32));

	/* setup FDM register base */
	/* DRV_WriteReg32(ECC_FDMADDR_REG32, NFI_FDM0L_REG32); */

	u4ENCODESize = (sector_size + g_nand_chip.nand_fdm_size) << 3;

	/* configure ECC decoder && encoder */
	DRV_WriteReg32(ECC_DECCNFG_REG32, ecc_bit_cfg | DEC_CNFG_NFI | DEC_CNFG_EMPTY_EN | (u4DECODESize << DEC_CNFG_CODE_SHIFT));
	DRV_WriteReg32(ECC_ENCCNFG_REG32, ecc_bit_cfg | ENC_CNFG_NFI | (u4ENCODESize << ENC_CNFG_MSG_SHIFT));
#ifndef MANUAL_CORRECT
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_CORRECT);
#else
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_EL);
#endif
}

static void ECC_Decode_Start(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE));
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_EN);
}

static void ECC_Decode_End(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE));
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
}

/* ------------------------------------------------------------------------------- */
static void ECC_Encode_Start(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg32(ECC_ENCIDLE_REG32) & ENC_IDLE));
	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_EN);
}

/* ------------------------------------------------------------------------------- */
static void ECC_Encode_End(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg32(ECC_ENCIDLE_REG32) & ENC_IDLE));
	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
}

/* ------------------------------------------------------------------------------- */
static bool nand_check_bch_error(u8 *pDataBuf, u32 u4SecIndex, u32 u4PageAddr)
{
	bool bRet = true;
	u16 u2SectorDoneMask = 1 << u4SecIndex;
	u32 u4ErrorNumDebug0, u4ErrorNumDebug1, i, u4ErrNum;
	u32 timeout = 0xFFFF;

#ifdef MANUAL_CORRECT
	u32 au4ErrBitLoc[6];
	u32 u4ErrByteLoc, u4BitOffset;
	u32 u4ErrBitLoc1th, u4ErrBitLoc2nd;
#endif

	while (0 == (u2SectorDoneMask & DRV_Reg16(ECC_DECDONE_REG16))) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}

#ifndef MANUAL_CORRECT
	if (0 == (DRV_Reg32(NFI_STA_REG32) & STA_READ_EMPTY)) {
		u4ErrorNumDebug0 = DRV_Reg32(ECC_DECENUM0_REG32);
		u4ErrorNumDebug1 = DRV_Reg32(ECC_DECENUM1_REG32);
		if (0 != (u4ErrorNumDebug0 & 0xFFFFFFFF) || 0 != (u4ErrorNumDebug1 & 0xFFFFFFFF)) {
			for (i = 0; i <= u4SecIndex; ++i) {
#if 1
				u4ErrNum = (DRV_Reg32((ECC_DECENUM0_REG32 + (i / 4))) >> ((i % 4) * 8)) & ERR_NUM0;
#else
				if (i < 4) {
					u4ErrNum = DRV_Reg32(ECC_DECENUM0_REG32) >> (i * 8);
				} else {
					u4ErrNum = DRV_Reg32(ECC_DECENUM1_REG32) >> ((i - 4) * 8);
				}
				u4ErrNum &= ERR_NUM0;
#endif

				if (ERR_NUM0 == u4ErrNum) {
					MSG(ERR, "In LittleKernel UnCorrectable at PageAddr=%d, Sector=%d\n", u4PageAddr, i);
					bRet = false;
				} else {
					//if (u4ErrNum) {
					//  MSG(ERR, " In LittleKernel Correct %d at PageAddr=%d, Sector=%d\n", u4ErrNum, u4PageAddr, i);
					//}
				}
			}
			if (bRet == false) {
				if (0 != (DRV_Reg32(NFI_STA_REG32) & STA_READ_EMPTY)) {
					MSG(ERR, "Empty data at 0x%x\n", u4PageAddr);
					bRet = true;
				}
			}
		}
	}
#else
	memset(au4ErrBitLoc, 0x0, sizeof(au4ErrBitLoc));
	u4ErrorNumDebug0 = DRV_Reg32(ECC_DECENUM_REG32);
	u4ErrNum = (DRV_Reg32((ECC_DECENUM_REG32 + (u4SecIndex / 4))) >> ((u4SecIndex % 4) * 8)) & ERR_NUM0;

	if (u4ErrNum) {
		if (ERR_NUM0 == u4ErrNum) {
			MSG(ERR, "UnCorrectable at PageAddr=%d\n", u4PageAddr);
			bRet = false;
		} else {
			for (i = 0; i < ((u4ErrNum + 1) >> 1); ++i) {
				au4ErrBitLoc[i] = DRV_Reg32(ECC_DECEL0_REG32 + i);
				u4ErrBitLoc1th = au4ErrBitLoc[i] & 0x3FFF;
				if (u4ErrBitLoc1th < 0x1000) {
					u4ErrByteLoc = u4ErrBitLoc1th / 8;
					u4BitOffset = u4ErrBitLoc1th % 8;
					pDataBuf[u4ErrByteLoc] = pDataBuf[u4ErrByteLoc] ^ (1 << u4BitOffset);
				} else {
					MSG(ERR, "UnCorrectable ErrLoc=%d\n", au4ErrBitLoc[i]);
				}

				u4ErrBitLoc2nd = (au4ErrBitLoc[i] >> 16) & 0x3FFF;
				if (0 != u4ErrBitLoc2nd) {
					if (u4ErrBitLoc2nd < 0x1000) {
						u4ErrByteLoc = u4ErrBitLoc2nd / 8;
						u4BitOffset = u4ErrBitLoc2nd % 8;
						pDataBuf[u4ErrByteLoc] = pDataBuf[u4ErrByteLoc] ^ (1 << u4BitOffset);
					} else {
						MSG(ERR, "UnCorrectable High ErrLoc=%d\n", au4ErrBitLoc[i]);
					}
				}
			}
			bRet = true;
		}

		if (0 == (DRV_Reg16(ECC_DECFER_REG16) & (1 << u4SecIndex))) {
			bRet = false;
		}
	}
#endif

	return bRet;
}

#if 1
static bool nand_RFIFOValidSize(u16 u2Size)
{
	u32 timeout = 0xFFFF;
	while (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) < u2Size) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	if (u2Size == 0) {
		while (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16))) {
			timeout--;
			if (0 == timeout) {
				printf("nand_RFIFOValidSize failed: 0x%x\n", u2Size);
				return false;
			}
		}
	}

	return true;
}

/* ------------------------------------------------------------------------------- */
static bool nand_WFIFOValidSize(u16 u2Size)
{
	u32 timeout = 0xFFFF;
	while (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) > u2Size) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	if (u2Size == 0) {
		while (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16))) {
			timeout--;
			if (0 == timeout) {
				printf("nand_RFIFOValidSize failed: 0x%x\n", u2Size);
				return false;
			}
		}
	}

	return true;
}
#endif

static bool nand_status_ready(u32 u4Status)
{
	u32 timeout = 0xFFFF;
	while ((DRV_Reg32(NFI_STA_REG32) & u4Status) != 0) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	return true;
}

static bool nand_reset(void)
{
	int timeout = 0xFFFF;
	bool ret;
	if (DRV_Reg16(NFI_MASTERSTA_REG16) & 0xFFF) { /* master is busy */
		DRV_WriteReg32(NFI_CON_REG16, CON_FIFO_FLUSH | CON_NFI_RST);
		while (DRV_Reg16(NFI_MASTERSTA_REG16) & 0xFFF) {
			timeout--;
			if (!timeout) {
				MSG(FUC, "Wait for NFI_MASTERSTA timeout\n");
			}
		}
	}
	/* issue reset operation */
	DRV_WriteReg32(NFI_CON_REG16, CON_FIFO_FLUSH | CON_NFI_RST);

	ret = nand_status_ready(STA_NFI_FSM_MASK | STA_NAND_BUSY) && nand_RFIFOValidSize(0) && nand_WFIFOValidSize(0);
	return ret;
}

/* ------------------------------------------------------------------------------- */
static void nand_set_mode(u16 u2OpMode)
{
	u16 u2Mode = DRV_Reg16(NFI_CNFG_REG16);
	u2Mode &= ~CNFG_OP_MODE_MASK;
	u2Mode |= u2OpMode;
	DRV_WriteReg16(NFI_CNFG_REG16, u2Mode);
}

/* ------------------------------------------------------------------------------- */
static void nand_set_autoformat(bool bEnable)
{
	if (bEnable) {
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	} else {
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	}
}

/* ------------------------------------------------------------------------------- */
static void nand_configure_fdm(u16 u2FDMSize)
{
	NFI_CLN_REG32(NFI_PAGEFMT_REG32, PAGEFMT_FDM_MASK | PAGEFMT_FDM_ECC_MASK);
	NFI_SET_REG32(NFI_PAGEFMT_REG32, u2FDMSize << PAGEFMT_FDM_SHIFT);
	NFI_SET_REG32(NFI_PAGEFMT_REG32, u2FDMSize << PAGEFMT_FDM_ECC_SHIFT);
}

/* ------------------------------------------------------------------------------- */
static bool nand_set_command(u16 command)
{
	/* Write command to device */
	DRV_WriteReg16(NFI_CMD_REG16, command);
	return nand_status_ready(STA_CMD_STATE);
}

/* ------------------------------------------------------------------------------- */
static bool nand_set_address(u32 u4ColAddr, u32 u4RowAddr, u16 u2ColNOB, u16 u2RowNOB)
{
	/* fill cycle addr */
	DRV_WriteReg32(NFI_COLADDR_REG32, u4ColAddr);
	DRV_WriteReg32(NFI_ROWADDR_REG32, u4RowAddr);
	DRV_WriteReg16(NFI_ADDRNOB_REG16, u2ColNOB | (u2RowNOB << ADDR_ROW_NOB_SHIFT));
	return nand_status_ready(STA_ADDR_STATE);
}

/* ------------------------------------------------------------------------------- */
static bool nand_device_reset(void)
{
	u32 timeout = 0xFFFF;

	nand_reset();

	DRV_WriteReg(NFI_CNFG_REG16, CNFG_OP_RESET);

	nand_set_command(NAND_CMD_RESET);

	while (!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));

	if (!timeout)
		return FALSE;
	else
		return TRUE;
}

/* ------------------------------------------------------------------------------- */

static bool nand_check_RW_count(struct nand_chip *nand, u16 u2WriteSize)
{
	u32 timeout = 0xFFFF;
	u16 u2SecNum = u2WriteSize >> nand->sector_shift;
	while (ADDRCNTR_CNTR(DRV_Reg32(NFI_ADDRCNTR_REG16)) < u2SecNum) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	return true;
}

/* ------------------------------------------------------------------------------- */
static bool nand_ready_for_read(struct nand_chip *nand, u32 u4RowAddr, u32 u4ColAddr, bool bFull, u8 *buf)
{
	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	bool bRet = false;
	u16 sec_num = 1 << (nand->page_shift - nand->sector_shift);
	u32 col_addr = u4ColAddr;
	u16 read_cmd;
	if (nand->options & NAND_BUSWIDTH_16)
		col_addr >>= 1;
	u32 colnob = 2, rownob = devinfo.addr_cycle - 2;

	if (!nand_reset()) {
		goto cleanup;
	}

	if (devinfo.two_phyplane)
		sec_num >>= 1;
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	nand_set_mode(CNFG_OP_READ);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
		DRV_WriteReg32(NFI_CON_REG16, sec_num << CON_NFI_SEC_SHIFT);

	if (bFull) {
#if USE_AHB_MODE
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
#else
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
#endif
		if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
			DRV_WriteReg32(NFI_STRADDR_REG32, buf);
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	} else {
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	}

	nand_set_autoformat(bFull);
	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC) {
		if (bFull)
			ECC_Decode_Start();
	}
	if (!nand_set_command(NAND_CMD_READ_0)) {
		goto cleanup;
	}
	if (!nand_set_address(col_addr, u4RowAddr, colnob, rownob)) {
		goto cleanup;
	}

	read_cmd = NAND_CMD_READ_START;
#ifdef MTK_TLC_NAND_SUPPORT
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)
	        && devinfo.two_phyplane
	        && (!tlc_snd_phyplane))
		read_cmd = MULTI_PLANE_READ_CMD;
#endif

	if (!nand_set_command(read_cmd)) {
		goto cleanup;
	}

	if (!nand_status_ready(STA_NAND_BUSY)) {
		goto cleanup;
	}

	bRet = true;

cleanup:
	return bRet;
}

/* ----------------------------------------------------------------------------- */
static bool nand_ready_for_write(struct nand_chip *nand, u32 u4RowAddr, u8 *buf)
{
	bool bRet = false;
	u16 sec_num = 1 << (nand->page_shift - nand->sector_shift);
	u32 colnob = 2, rownob = devinfo.addr_cycle - 2;
	u16 prg_cmd;
	u16 temp_sec_num;

	temp_sec_num = sec_num;

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && devinfo.tlcControl.normaltlc) {
		if (devinfo.tlcControl.pPlaneEn)
			temp_sec_num = sec_num / 2;

		if (devinfo.two_phyplane)
			temp_sec_num /= 2;
	}
#endif
	if (!nand_reset()) {
		return false;
	}

	if (devinfo.two_phyplane)
		temp_sec_num /= 2;
	nand_set_mode(CNFG_OP_PRGM);
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
	DRV_WriteReg32(NFI_CON_REG16, temp_sec_num << CON_NFI_SEC_SHIFT);
#if USE_AHB_MODE
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	DRV_WriteReg32(NFI_STRADDR_REG32, buf);
#else
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
#endif

	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	nand_set_autoformat(true);
	ECC_Encode_Start();
	prg_cmd = NAND_CMD_SEQIN;
	if (((devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER)
	        ||(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC))
	        && devinfo.two_phyplane && tlc_snd_phyplane)
		prg_cmd = PLANE_INPUT_DATA_CMD;
	if (!nand_set_command(prg_cmd)) {
		goto cleanup;
	}
	if (!nand_set_address(0, u4RowAddr, colnob, rownob)) {
		goto cleanup;
	}

	if (!nand_status_ready(STA_NAND_BUSY)) {
		goto cleanup;
	}

	bRet = true;
cleanup:

	return bRet;
}

/* ----------------------------------------------------------------------------- */
static bool nand_dma_read_data(u8 *pDataBuf, u32 u4Size)
{
	u32 timeout = 0xFFFF;

	arch_clean_invalidate_cache_range((addr_t) pDataBuf, (size_t) u4Size);

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	DRV_Reg16(NFI_INTR_REG16);
	DRV_WriteReg16(NFI_INTR_EN_REG16, INTR_AHB_DONE_EN);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BRD);
#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        &&(devinfo.tlcControl.needchangecolumn || devinfo.two_phyplane))
		DRV_WriteReg(NFI_TLC_RD_WHR2_REG16, (TLC_RD_WHR2_EN | 0x055)); //trigger data sample
#endif

	if (en_interrupt) {
		if (event_wait_timeout(&nand_int_event, 100)) {
			printf("[nand_dma_read_data]wait for AHB done timeout\n");
			dump_nfi();
			return false;
		}

		timeout = 0xFFFF;
		while ((u4Size >> g_nand_chip.sector_shift) > ((DRV_Reg32(NFI_BYTELEN_REG16) & 0x1f000) >> 12)) {
			timeout--;
			if (0 == timeout) {
				return false;   /* 4 */
			}
		}
	} else {
		while (!(DRV_Reg16(NFI_INTR_REG16) & INTR_AHB_DONE)) {
			timeout--;
			if (0 == timeout) {
				return false;
			}
		}

		timeout = 0xFFFF;
		while ((u4Size >> g_nand_chip.sector_shift) > ((DRV_Reg32(NFI_BYTELEN_REG16) & 0x1f000) >> 12)) {
			timeout--;
			if (0 == timeout) {
				return false;   /* 4 */
			}
		}
	}
	return true;
}

static bool nand_mcu_read_data(u8 *pDataBuf, u32 length)
{
	u32 timeout = 0xFFFF;
	u32 i;
	u32 *pBuf32;
	if (length % 4) {
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	} else {
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	}

	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BRD);
	pBuf32 = (u32 *) pDataBuf;
	if (length % 4) {
		for (i = 0; (i < length) && (timeout > 0);) {
			WAIT_NFI_PIO_READY(timeout);
			*pDataBuf++ = DRV_Reg8(NFI_DATAR_REG32);
			i++;

		}
	} else {
		WAIT_NFI_PIO_READY(timeout);
		for (i = 0; (i < (length >> 2)) && (timeout > 0);) {
			WAIT_NFI_PIO_READY(timeout);
			*pBuf32++ = DRV_Reg32(NFI_DATAR_REG32);
			i++;
		}
	}
	return true;
}

static bool nand_read_page_data(u8 *buf, u32 length)
{
#if USE_AHB_MODE
	return nand_dma_read_data(buf, length);
#else
	return nand_mcu_read_data(buf, length);
#endif
}

static bool nand_dma_write_data(u8 *buf, u32 length)
{
	u32 timeout = 0xFFFF;
#if defined(MTK_TLC_NAND_SUPPORT)
	u32 reg_val;
#endif
	arch_clean_invalidate_cache_range((addr_t) buf, (size_t) length);

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	DRV_Reg16(NFI_INTR_REG16);
	DRV_WriteReg16(NFI_INTR_EN_REG16, INTR_AHB_DONE_EN);

	if ((unsigned int)buf % 16) {
		/* printf("Un-16-aligned address\n"); */
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	} else {
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	}

	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
#if defined(MTK_TLC_NAND_SUPPORT)
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		reg_val = DRV_Reg(NFI_DEBUG_CON1_REG16);
		reg_val |= 0x4000; //enable ce hold
		DRV_WriteReg(NFI_DEBUG_CON1_REG16, reg_val);
	}
#endif

	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	if (en_interrupt) {
		if (event_wait_timeout(&nand_int_event, 100)) {
			printf("[nand_dma_write_data]wait for AHB done timeout\n");
			dump_nfi();
			return false;
		}
	} else {
		while (!(DRV_Reg16(NFI_INTR_REG16) & INTR_AHB_DONE)) {
			timeout--;
			if (0 == timeout) {
				printf("wait write AHB done timeout\n");
				dump_nfi();
				return FALSE;
			}
		}
	}
	return true;
}

static bool nand_mcu_write_data(const u8 *buf, u32 length)
{
	u32 timeout = 0xFFFF;
	u32 i;
	u32 *pBuf32 = (u32 *) buf;
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);

	if ((u32) buf % 4 || length % 4)
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	else
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	if ((u32) buf % 4 || length % 4) {
		for (i = 0; (i < (length)) && (timeout > 0);) {
			if (DRV_Reg16(NFI_PIO_DIRDY_REG16) & 1) {
				DRV_WriteReg32(NFI_DATAW_REG32, *buf++);
				i++;
			} else {
				timeout--;
			}
			if (0 == timeout) {
				printf("[%s] nand mcu write timeout\n", __func__);
				dump_nfi();
				return false;
			}
		}
	} else {
		for (i = 0; (i < (length >> 2)) && (timeout > 0);) {
			if (DRV_Reg16(NFI_PIO_DIRDY_REG16) & 1) {
				DRV_WriteReg32(NFI_DATAW_REG32, *pBuf32++);
				i++;
			} else {
				timeout--;
			}
			if (0 == timeout) {
				printf("[%s] nand mcu write timeout\n", __func__);
				dump_nfi();
				return false;
			}
		}
	}

	return true;
}

/* ----------------------------------------------------------------------------- */
static bool nand_write_page_data(u8 *buf, u32 length)
{
#if USE_AHB_MODE
	return nand_dma_write_data(buf, length);
#else
	return nand_mcu_write_data(buf, length);
#endif
}

static void nand_read_fdm_data(u8 *pDataBuf, u32 u4SecNum)
{
	u32 *pBuf32 = (u32 *) pDataBuf;
	u32 fdm_temp[2];
	u32 i, j;
	u8 *byte_ptr;

	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		byte_ptr = (u8*)fdm_temp;

		if (pDataBuf) {
			for (i = 0; i < u4SecNum; ++i) {
				fdm_temp[0] = DRV_Reg32(NFI_FDM0L_REG32 + (i << 1));
				fdm_temp[1] = DRV_Reg32(NFI_FDM0M_REG32 + (i << 1));
				for (j = 0; j < g_nand_chip.nand_fdm_size; j++) {
					*(pDataBuf + (i * g_nand_chip.nand_fdm_size) + j) = *(byte_ptr + j);
				}
			}
		}
	} else {
		for (i = 0; i < u4SecNum; ++i) {
			*pBuf32++ = DRV_Reg32(NFI_FDM0L_REG32 + (i << 1));
			*pBuf32++ = DRV_Reg32(NFI_FDM0M_REG32 + (i << 1));
		}
	}
}

static void nand_write_fdm_data(u8 *pDataBuf, u32 u4SecNum)
{
	u32 *pBuf32 = (u32 *) pDataBuf;
	u8 *pBuf;
	u8* byte_ptr;
	u32 fdm_data[2];
	u32 i, j;

	pBuf = (u8*)fdm_data;
	byte_ptr = (u8*)pDataBuf;
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		for (i = 0; i < u4SecNum; ++i) {
			fdm_data[0] = 0xFFFFFFFF;
			fdm_data[1] = 0xFFFFFFFF;

			for (j = 0; j < g_nand_chip.nand_fdm_size; j++) {
				*(pBuf + j) = *(byte_ptr + j + (i * g_nand_chip.nand_fdm_size));
			}

			DRV_WriteReg32(NFI_FDM0L_REG32 + (i << 1), fdm_data[0]);
			DRV_WriteReg32(NFI_FDM0M_REG32 + (i << 1), fdm_data[1]);
		}
	} else {
		for (i = 0; i < u4SecNum; ++i) {
			DRV_WriteReg32(NFI_FDM0L_REG32 + (i << 1), *pBuf32++);
			DRV_WriteReg32(NFI_FDM0M_REG32 + (i << 1), *pBuf32++);
		}
	}
}

static void nand_stop_read(void)
{
	NFI_CLN_REG32(NFI_CON_REG16, CON_NFI_BRD);
	ECC_Decode_End();
}

static void nand_stop_write(void)
{
#if defined(MTK_TLC_NAND_SUPPORT)
	u32 reg_val;
#endif
	NFI_CLN_REG32(NFI_CON_REG16, CON_NFI_BWR);
	ECC_Encode_End();
#if defined(MTK_TLC_NAND_SUPPORT)
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		reg_val = DRV_Reg(NFI_DEBUG_CON1_REG16);
		reg_val &= (~0x4000); //disable ce hold
		DRV_WriteReg(NFI_DEBUG_CON1_REG16, reg_val);
	}
#endif
}

static bool nand_check_dececc_done(u32 u4SecNum)
{
	u32 timeout, dec_mask;
	timeout = 0xffff;
	/* dec_mask = (1 << u4SecNum) - 1; */
	dec_mask = (1 << (u4SecNum - 1));
	/* while ((dec_mask != DRV_Reg(ECC_DECDONE_REG16)) && timeout > 0) */
	while (dec_mask != (DRV_Reg(ECC_DECDONE_REG16) & dec_mask)) {
		if (timeout == 0) {
			/* MSG(ERR, "ECC_DECDONE: timeout\n"); */
			MSG(ERR, "ECC_DECDONE: timeout 0x%x %d\n", DRV_Reg(ECC_DECDONE_REG16), u4SecNum);
			dump_nfi();
			return false;
		}
		timeout--;
	}
	timeout = 0xffff;
	while ((DRV_Reg32(ECC_DECFSM_REG32) & 0x3F3FFF0F) != ECC_DECFSM_IDLE) {
		if (timeout == 0) {
			/* MSG(ERR, "ECC_DECDONE: timeout\n"); */
			MSG(ERR, "ECC_DECDONE: timeout 0x%x 0x%x %d\n", DRV_Reg32(ECC_DECFSM_REG32), DRV_Reg(ECC_DECDONE_REG16), u4SecNum);
			dump_nfi();
			return false;
		}
		timeout--;
	}
	return true;
}

/* --------------------------------------------------------------------------- */
static bool mtk_nand_read_status(void)
{
	int status = 0;     /* , i; */
	unsigned int timeout;

	nand_reset();

	/* Disable HW ECC */
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

	/* Disable 16-bit I/O */
	NFI_CLN_REG32(NFI_PAGEFMT_REG32, PAGEFMT_DBYTE_EN);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_OP_SRD | CNFG_READ_EN | CNFG_BYTE_RW);

	DRV_WriteReg32(NFI_CON_REG16, CON_NFI_SRD | (1 << CON_NFI_NOB_SHIFT));

	DRV_WriteReg32(NFI_CON_REG16, 0x3);
	nand_set_mode(CNFG_OP_SRD);
	DRV_WriteReg16(NFI_CNFG_REG16, 0x2042);
	nand_set_command(NAND_CMD_STATUS);
	DRV_WriteReg32(NFI_CON_REG16, 0x90);

	timeout = TIMEOUT_4;
	WAIT_NFI_PIO_READY(timeout);

	if (timeout) {
		status = (DRV_Reg16(NFI_DATAR_REG32));
	}
	/* ~  clear NOB */
	DRV_WriteReg32(NFI_CON_REG16, 0);

	if (g_nand_chip.bus16 == IO_WIDTH_16) {
		NFI_SET_REG32(NFI_PAGEFMT_REG32, PAGEFMT_DBYTE_EN);
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	}
	/* check READY/BUSY status first */
	if (!(STATUS_READY & status)) {
		MSG(ERR, "status is not ready\n");
	}
	/* flash is ready now, check status code */
#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.slcopmodeEn)) { //hynix tlc need doule check
		if (SLC_MODE_OP_FALI & status) {
			if (!(STATUS_WR_ALLOW & status)) {
				MSG(INIT, "status locked\n");
				return FALSE;
			} else {
				MSG(INIT, "status unknown\n");
				return FALSE;
			}
		} else {
			return TRUE;
		}
	} else
#endif
	{
		if (STATUS_FAIL & status) {
			if (!(STATUS_WR_ALLOW & status)) {
				MSG(INIT, "status locked\n");
				return FALSE;
			} else {
				MSG(INIT, "status unknown\n");
				return FALSE;
			}
		} else {
			return TRUE;
		}
	}
}

bool mtk_nand_SetFeature(u16 cmd, u32 addr, u8 *value, u8 bytes)
{
	kal_uint16 reg_val = 0;
	kal_uint8 write_count = 0;
	kal_uint32 timeout = TIMEOUT_3; /* 0xffff; */

	nand_reset();

	reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	nand_set_command(cmd);
	nand_set_address(addr, 0, 1, 0);
	/* NFI_ISSUE_COMMAND(cmd, addr, 0, 1, 0) */

	/* SAL_NFI_Config_Sector_Number(1); */
	DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);
	/* SAL_NFI_Start_Data_Transfer(KAL_FALSE, KAL_TRUE); */
	while ((write_count < bytes) && timeout) {
		WAIT_NFI_PIO_READY(timeout)
		if (timeout == 0) {
			break;
		}
		DRV_WriteReg32(NFI_DATAW_REG32, *value++);
		write_count++;
		timeout = TIMEOUT_3;
	}
	while ((*NFI_STA_REG32 & STA_NAND_BUSY) && (timeout)) {
		timeout--;
	}
	mtk_nand_read_status();
	if (timeout != 0)
		return TRUE;
	else
		return FALSE;
}

bool mtk_nand_GetFeature(u16 cmd, u32 addr, u8 *value, u8 bytes)
{
	kal_uint16 reg_val = 0;
	kal_uint8 read_count = 0;
	kal_uint32 timeout = TIMEOUT_3; /* 0xffff; */

	nand_reset();

	reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW | CNFG_READ_EN);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	nand_set_command(cmd);
	nand_set_address(addr, 0, 1, 0);

	/* SAL_NFI_Config_Sector_Number(0); */
	DRV_WriteReg32(NFI_CON_REG16, 0 << CON_NFI_SEC_SHIFT);
	reg_val = DRV_Reg32(NFI_CON_REG16);
	reg_val &= ~CON_NFI_NOB_MASK;
	reg_val |= ((4 << CON_NFI_NOB_SHIFT) | CON_NFI_SRD);
	DRV_WriteReg32(NFI_CON_REG16, reg_val);
	/* NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BWR); */
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);
	/* SAL_NFI_Start_Data_Transfer(KAL_TRUE, KAL_TRUE); */
	while ((read_count < bytes) && timeout) {
		WAIT_NFI_PIO_READY(timeout)
		if (timeout == 0) {
			break;
		}
		*value++ = DRV_Reg32(NFI_DATAR_REG32);
		read_count++;
		timeout = TIMEOUT_3;
	}
	mtk_nand_read_status();
	if (timeout != 0)
		return TRUE;
	else
		return FALSE;

}

#if 1
const u8 data_tbl[8][5] = {
	{0x04, 0x04, 0x7C, 0x7E, 0x00},
	{0x00, 0x7C, 0x78, 0x78, 0x00},
	{0x7C, 0x76, 0x74, 0x72, 0x00},
	{0x08, 0x08, 0x00, 0x00, 0x00},
	{0x0B, 0x7E, 0x76, 0x74, 0x00},
	{0x10, 0x76, 0x72, 0x70, 0x00},
	{0x02, 0x7C, 0x7E, 0x70, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00}
};

const U8 data_tbl_15nm[11][5] = {
	{0x00, 0x00, 0x00, 0x00, 0x00},
	{0x02, 0x04, 0x02, 0x00, 0x00},
	{0x7C, 0x00, 0x7C, 0x7C, 0x00},
	{0x7A, 0x00, 0x7A, 0x7A, 0x00},
	{0x78, 0x02, 0x78, 0x7A, 0x00},
	{0x7E, 0x04, 0x7E, 0x7A, 0x00},
	{0x76, 0x04, 0x76, 0x78, 0x00},
	{0x04, 0x04, 0x04, 0x76, 0x00},
	{0x06, 0x0A, 0x06, 0x02, 0x00},
	{0x74, 0x7C, 0x74, 0x76, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00}
};

static void mtk_nand_modeentry_rrtry(void)
{
	nand_reset();

	nand_set_mode(CNFG_OP_CUST);

	nand_set_command(0x5C);
	nand_set_command(0xC5);

	nand_status_ready(STA_NFI_OP_MASK);
}

static void mtk_nand_rren_rrtry(bool needB3)
{
	nand_reset();

	nand_set_mode(CNFG_OP_CUST);

	if (needB3)
		nand_set_command(0xB3);
	nand_set_command(0x26);
	nand_set_command(0x5D);

	nand_status_ready(STA_NFI_OP_MASK);
}


static void mtk_nand_rren_15nm_rrtry(bool flag)
{
	nand_reset();

	nand_set_mode(CNFG_OP_CUST);

	if (flag)
		nand_set_command(0x26);
	else
		nand_set_command(0xCD);

	nand_set_command(0x5D);

	nand_status_ready(STA_NFI_OP_MASK);
}

static void mtk_nand_sprmset_rrtry(u32 addr, u32 data)  /* single parameter setting */
{
	u16 reg_val = 0;
	u8 write_count = 0;
	u32 reg = 0;
	u32 timeout = TIMEOUT_3;    /* 0xffff; */

	nand_reset();

	reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	nand_set_command(0x55);
	nand_set_address(addr, 0, 1, 0);

	nand_status_ready(STA_NFI_OP_MASK);

	DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);

	WAIT_NFI_PIO_READY(timeout);
	timeout = TIMEOUT_3;
	DRV_WriteReg8(NFI_DATAW_REG32, data);

	while (!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));

}

static void mtk_nand_toshiba_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	u32 acccon;
	u8 add_reg[6] = { 0x04, 0x05, 0x06, 0x07, 0x0D };
	u8 cnt = 0;

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669);   /* to fit read retry timing */

	if (0 == retryCount)
		mtk_nand_modeentry_rrtry();

	for (cnt = 0; cnt < 5; cnt++) {
		mtk_nand_sprmset_rrtry(add_reg[cnt], data_tbl[retryCount][cnt]);
	}

	if (3 == retryCount)
		mtk_nand_rren_rrtry(TRUE);
	else if (6 > retryCount)
		mtk_nand_rren_rrtry(FALSE);

	if (7 == retryCount) {  /* to exit */
		nand_set_mode(CNFG_OP_RESET);
		NFI_ISSUE_COMMAND(NAND_CMD_RESET, 0, 0, 0, 0);
		nand_reset();
	}

	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}

#if defined(MTK_TLC_NAND_SUPPORT)
const u8 data_tbl_tlc_a19nm[32][7] = {
	{0xF7, 0xFD, 0xFD, 0xFC, 0xFD, 0xFA, 0xF8},
	{0xF7, 0xFD, 0xFD, 0xFB, 0xFC, 0xF8, 0xF6},
	{0xF7, 0xFD, 0xFC, 0xFA, 0xFA, 0xF6, 0xF3},
	{0xF7, 0xFD, 0xFC, 0xF9, 0xF9, 0xF4, 0xF1},
	{0xF7, 0xFC, 0xFB, 0xF8, 0xF7, 0xF2, 0xEE},
	{0xF8, 0xFC, 0xFB, 0xF7, 0xF6, 0xF0, 0xEC},
	{0xF8, 0xFC, 0xFA, 0xF6, 0xF4, 0xEE, 0xE9},
	{0xF4, 0xF8, 0xF8, 0xF9, 0xFA, 0xF8, 0xF8},
	{0xF5, 0xF8, 0xF7, 0xF7, 0xF7, 0xF4, 0xF3},
	{0xF5, 0xF9, 0xF6, 0xF5, 0xF5, 0xF0, 0xEE},
	{0xF6, 0xF9, 0xF5, 0xF4, 0xF2, 0xEC, 0xE9},
	{0xF4, 0xFA, 0xFB, 0xFA, 0xFB, 0xFA, 0xFA},
	{0xF4, 0xFA, 0xFA, 0xF9, 0xFA, 0xF8, 0xF8},
	{0xF4, 0xFA, 0xFA, 0xF8, 0xF9, 0xF6, 0xF5},
	{0xF5, 0xFA, 0xF9, 0xF7, 0xF7, 0xF4, 0xF3},
	{0xF5, 0xFB, 0xF9, 0xF6, 0xF6, 0xF2, 0xF0},
	{0xF5, 0xFB, 0xF8, 0xF5, 0xF5, 0xF0, 0xEE},
	{0xF5, 0xFB, 0xF8, 0xF5, 0xF4, 0xEE, 0xEB},
	{0xF6, 0xFB, 0xF7, 0xF4, 0xF2, 0xEC, 0xE9},
	{0xFA, 0xFE, 0xFE, 0xF9, 0xFA, 0xF8, 0xF8},
	{0xFB, 0xFE, 0xFD, 0xF7, 0xF7, 0xF4, 0xF3},
	{0xFB, 0xFF, 0xFC, 0xF5, 0xF5, 0xF0, 0xEE},
	{0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x08, 0x04, 0x04, 0x02, 0x02, 0x00, 0x00},
	{0x0C, 0x08, 0x06, 0x04, 0x04, 0x02, 0x00},
	{0x10, 0x0C, 0x08, 0x06, 0x06, 0x04, 0x00},
	{0x04, 0xFD, 0xFD, 0xFC, 0xFD, 0xFA, 0xF8},
	{0x04, 0xFD, 0xFC, 0xFA, 0xFA, 0xF6, 0xF3},
	{0xFC, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08},
	{0xF8, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const u8 data_tbl_slc_a19nm[6] = {
	0x10, 0x20, 0x40, 0xF0, 0xE0, 0x00
};

static void mtk_nand_modeentry_rrtry_tlc_a19nm(void)
{
	u32 timeout=TIMEOUT_3;

	nand_reset();

	nand_set_mode(CNFG_OP_CUST);

	nand_set_command(0x5C);
	nand_set_command(0xC5);
	nand_set_command(0x55);
	nand_set_address(0x00, 0, 1, 0);

	nand_status_ready(STA_NFI_OP_MASK);

	DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);


	WAIT_NFI_PIO_READY(timeout);
	timeout=TIMEOUT_3;
	DRV_WriteReg8(NFI_DATAW_REG32, 0x01);

	while (!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));
}

static void mtk_nand_rren_rrtry_tlc_a19nm(bool needB3)
{
	nand_reset();

	nand_set_mode(CNFG_OP_CUST);

	if (needB3)
		nand_set_command(0xB3);
	nand_set_command(0x5D);

	nand_status_ready(STA_NFI_OP_MASK);
}

static void mtk_nand_sprmset_rrtry_tlc_a19nm(u32 addr, u32 data) //single parameter setting
{
	u16           reg_val        = 0;
	u8            write_count     = 0;
	u32           reg = 0;
	u32           timeout=TIMEOUT_3;//0xffff;

	nand_reset();

	reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	nand_set_command(0x55);
	nand_set_address(addr, 0, 1, 0);

	nand_status_ready(STA_NFI_OP_MASK);

	DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);


	WAIT_NFI_PIO_READY(timeout);
	timeout=TIMEOUT_3;
	DRV_WriteReg8(NFI_DATAW_REG32, data);

	while (!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));
}

static void mtk_nand_toshiba_rrtry_tlc_a19nm(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	u32 acccon;
	u8 cnt = 0;
	u8 add_reg_tlc[7] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
	u8 add_reg_slc = 0x0B;

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669); //to fit read retry timing

	if (0 == retryCount)
		mtk_nand_modeentry_rrtry_tlc_a19nm();

	if (devinfo.tlcControl.slcopmodeEn) { //slc block
		mtk_nand_sprmset_rrtry_tlc_a19nm(add_reg_slc, data_tbl_slc_a19nm[cnt]);
	} else { //tlc block
		for (cnt = 0; cnt < 7; cnt ++) {
			mtk_nand_sprmset_rrtry_tlc_a19nm(add_reg_tlc[cnt], data_tbl_tlc_a19nm[retryCount][cnt]);
		}
	}

	if (defValue) { // to exit
		nand_device_reset();
		nand_reset();
		//should do NAND DEVICE interface change under sync mode
	} else {
		if ((!devinfo.tlcControl.slcopmodeEn)
		        && (30 == retryCount))
			mtk_nand_rren_rrtry_tlc_a19nm(TRUE);
		else
			mtk_nand_rren_rrtry_tlc_a19nm(FALSE);
	}

	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}

#endif

static void mtk_nand_toshiba_15nm_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	u32 acccon;
	u8 add_reg[6] = {0x04, 0x05, 0x06, 0x07, 0x0D};
	u8 cnt = 0;

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669); /* to fit read retry timing */

	if (0 == retryCount)
		mtk_nand_modeentry_rrtry();

	for (cnt = 0; cnt < 5; cnt++) {
		mtk_nand_sprmset_rrtry(add_reg[cnt], data_tbl_15nm[retryCount][cnt]);
	}

	if (10 == retryCount) { /* to exit */
		nand_set_mode(CNFG_OP_RESET);
		NFI_ISSUE_COMMAND (NAND_CMD_RESET, 0, 0, 0, 0);
		nand_reset();
	}   else {
		if (0 == retryCount)
			mtk_nand_rren_15nm_rrtry(TRUE);
		else
			mtk_nand_rren_15nm_rrtry(FALSE);
	}

	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}
#endif
static void mtk_nand_micron_rrtry(flashdev_info deviceinfo, u32 feature, bool defValue)
{
	/* u32 feature = deviceinfo.feature_set.FeatureSet.readRetryStart+retryCount; */
	mtk_nand_SetFeature(deviceinfo.feature_set.FeatureSet.sfeatureCmd,\
	                    deviceinfo.feature_set.FeatureSet.readRetryAddress,\
	                    (u8 *)&feature,4);
}

static int g_sandisk_retry_case = 0;
static void mtk_nand_sandisk_rrtry(flashdev_info deviceinfo, u32 feature, bool defValue)
{
	/* u32 feature = deviceinfo.feature_set.FeatureSet.readRetryStart+retryCount; */
	if (FALSE == defValue)
		nand_reset();
	else {
		nand_device_reset();
		nand_reset();
	}
	mtk_nand_SetFeature(deviceinfo.feature_set.FeatureSet.sfeatureCmd,\
	                    deviceinfo.feature_set.FeatureSet.readRetryAddress,\
	                    (u8 *)&feature,4);
	if (FALSE == defValue)
		nand_set_command(deviceinfo.feature_set.FeatureSet.readRetryPreCmd);
}

#if 1               /* sandisk 19nm read retry */
u16 sandisk_19nm_rr_table[18] = {
	0x0000,
	0xFF0F, 0xEEFE, 0xDDFD, 0x11EE, /* 04h[7:4] | 07h[7:4] | 04h[3:0] | 05h[7:4] */
	0x22ED, 0x33DF, 0xCDDE, 0x01DD,
	0x0211, 0x1222, 0xBD21, 0xAD32,
	0x9DF0, 0xBCEF, 0xACDC, 0x9CFF,
	0x0000
};

static void sandisk_19nm_rr_init(void)
{
	u32 reg_val = 0;
	u32 count = 0;
	u32 timeout = 0xffff;
	u32 acccon;

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669);   /* to fit read retry timing */

	nand_reset();

	reg_val = (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	nand_set_command(0x3B);
	nand_set_command(0xB9);

	for (count = 0; count < 9; count++) {
		nand_set_command(0x53);
		nand_set_address((0x04 + count), 0, 1, 0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		timeout = 0xffff;
		WAIT_NFI_PIO_READY(timeout);
		DRV_WriteReg32(NFI_DATAW_REG32, 0x00);
		nand_reset();
	}

	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}

static void sandisk_19nm_rr_loading(u32 retryCount, bool defValue)
{
	u32 reg_val = 0;
	u32 timeout = 0xffff;
	u32 acccon;
	u8 count;
	u8 cmd_reg[4] = { 0x4, 0x5, 0x7 };

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669);   /* to fit read retry timing */

	nand_reset();

	reg_val = (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	if ((0 != retryCount) || defValue) {
		nand_set_command(0xD6);
	}

	nand_set_command(0x3B);
	nand_set_command(0xB9);

	for (count = 0; count < 3; count++) {
		nand_set_command(0x53);
		nand_set_address(cmd_reg[count], 0, 1, 0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		timeout = 0xffff;
		WAIT_NFI_PIO_READY(timeout);
		if (count == 0)
			DRV_WriteReg32(NFI_DATAW_REG32,
			               (((sandisk_19nm_rr_table[retryCount] & 0xF000) >> 8) | ((sandisk_19nm_rr_table[retryCount] & 0x00F0) >> 4)));
		else if (count == 1)
			DRV_WriteReg32(NFI_DATAW_REG32, ((sandisk_19nm_rr_table[retryCount] & 0x000F) << 4));
		else if (count == 2)
			DRV_WriteReg32(NFI_DATAW_REG32, ((sandisk_19nm_rr_table[retryCount] & 0x0F00) >> 4));

		nand_reset();
	}

	if (!defValue) {
		nand_set_command(0xB6);
	}

	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}

static void mtk_nand_sandisk_19nm_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	if ((retryCount == 0) && (!defValue))
		sandisk_19nm_rr_init();
	sandisk_19nm_rr_loading(retryCount, defValue);
}
#endif

#define HYNIX_RR_TABLE_SIZE  (1026) /* hynix read retry table size */
#define SINGLE_RR_TABLE_SIZE (64)

#define READ_RETRY_STEP (devinfo.feature_set.FeatureSet.readRetryCnt + devinfo.feature_set.FeatureSet.readRetryStart)   /* 8 step or 12 step to fix read retry table */
#define HYNIX_16NM_RR_TABLE_SIZE  ((READ_RETRY_STEP == 12)?(784):(528)) /* hynix read retry table size */
#define SINGLE_RR_TABLE_16NM_SIZE  ((READ_RETRY_STEP == 12)?(48):(32))

u8 nand_hynix_rr_table[(HYNIX_RR_TABLE_SIZE + 16) / 16 * 16];   /* align as 16 byte */

#define NAND_HYX_RR_TBL_BUF nand_hynix_rr_table

static u8 real_hynix_rr_table_idx = 0;
static u32 g_hynix_retry_count = 0;
static u8 real_hynix_rr_tabl[48]; //take max size 48

static bool hynix_rr_table_select(u8 table_index, flashdev_info *deviceinfo)
{
	u32 i,j,k;
	u32 zero_num = 0;
	u32 one_num = 0;
	u32 table_size = (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX)? SINGLE_RR_TABLE_SIZE : SINGLE_RR_TABLE_16NM_SIZE;
	u8 *temp_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF+table_size*table_index*2+2;
	u8 *temp_inversed_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF+table_size*table_index*2+table_size+2;

	if (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX) { //20nm
		for (i = 0; i < table_size; i++) {
			if (0xFF != (temp_rr_table[i] ^ temp_inversed_rr_table[i]))
				return FALSE; // error table
		}
	} else { // 16nm & F-Die
		temp_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF + 16;
		temp_inversed_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF + table_size + 16;
		for (j = 0; j < table_size; j++) { //byte of each set
			real_hynix_rr_tabl[j] = 0; //initial

			for (k = 0; k < 8; k++) { // bit of each byte
				zero_num = 0;
				one_num = 0;

				for (i = 0; i < 8; i++) { // set number
					if ((temp_rr_table[(table_size * i * 2) + j] >> k) & 0x1)
						one_num ++;
					else
						zero_num ++;
				}
				if (one_num == zero_num) {
					zero_num = 0;
					one_num = 0;
					for (i = 0; i < 8; i++) { // set number
						if ((temp_inversed_rr_table[(table_size * i * 2) + j] >> k) & 0x1)
							one_num ++;
						else
							zero_num ++;
					}
					if (one_num == zero_num) //no valuable table existed
						return FALSE;
					else { // repeated time > 4
						real_hynix_rr_tabl[j] |= (((zero_num > one_num) ? 1 : 0) << k);
					}
				} else { // repeated time > 4
					real_hynix_rr_tabl[j] |= (((zero_num > one_num) ? 0 : 1) << k);
				}
			}
		}
	}

	return TRUE;        /* correct table */
}

static void HYNIX_RR_TABLE_READ(flashdev_info *deviceinfo)
{
	u32 reg_val = 0;
	u32 read_count = 0, max_count = HYNIX_RR_TABLE_SIZE;
	u32 timeout = 0xffff;
	u8 *rr_table = (u8 *) (NAND_HYX_RR_TBL_BUF);
	u8 table_index = 0;
	u8 add_reg1[3] = { 0xFF, 0xCC };
	u8 data_reg1[3] = { 0x40, 0x4D };
	u8 cmd_reg[6] = { 0x16, 0x17, 0x04, 0x19, 0x00 };
	u8 add_reg2[6] = { 0x00, 0x00, 0x00, 0x02, 0x00 };
	bool RR_TABLE_EXIST = TRUE;
	if ((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
	        || (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
		read_count = 1;
		if (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
			add_reg1[1]= 0x38;
		else //FDIE
			add_reg1[1]= 0x0E;
		data_reg1[1] = 0x52;
		max_count = HYNIX_16NM_RR_TABLE_SIZE;
		if (READ_RETRY_STEP == 12) {
			add_reg2[2] = 0x1F;
		}
	}
	nand_device_reset();
	/* take care under sync mode. need change nand device inferface xiaolei */

	nand_reset();

	DRV_WriteReg(NFI_CNFG_REG16, (CNFG_OP_CUST | CNFG_BYTE_RW));

	nand_set_command(0x36);

	for (; read_count < 2; read_count++) {
		nand_set_address(add_reg1[read_count], 0, 1, 0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		timeout = 0xffff;
		WAIT_NFI_PIO_READY(timeout);
		DRV_WriteReg32(NFI_DATAW_REG32, data_reg1[read_count]);
		nand_reset();
	}

	for (read_count = 0; read_count < 5; read_count++) {
		nand_set_command(cmd_reg[read_count]);
	}
	for (read_count = 0; read_count < 5; read_count++) {
		nand_set_address(add_reg2[read_count], 0, 1, 0);
	}
	nand_set_command(0x30);
	DRV_WriteReg(NFI_CNRNB_REG16, 0xF1);
	timeout = 0xffff;
	while (!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));

	reg_val = (CNFG_OP_CUST | CNFG_BYTE_RW | CNFG_READ_EN);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);
	DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BRD | (2 << CON_NFI_SEC_SHIFT)));
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);
	timeout = 0xffff;
	read_count = 0;     /* how???? */
	while ((read_count < max_count) && timeout) {
		WAIT_NFI_PIO_READY(timeout);
		*rr_table++ = (U8) DRV_Reg32(NFI_DATAR_REG32);
		read_count++;
		timeout = 0xFFFF;
	}

	nand_device_reset();
	/* take care under sync mode. need change nand device inferface xiaolei */

	reg_val = (CNFG_OP_CUST | CNFG_BYTE_RW);
	if ((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
	        || (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
		DRV_WriteReg(NFI_CNFG_REG16, reg_val);
		nand_set_command(0x36);
		nand_set_address(add_reg1[1],0,1,0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		WAIT_NFI_PIO_READY(timeout);
		DRV_WriteReg32(NFI_DATAW_REG32, 0x00);
		nand_reset();
		nand_set_command(0x16);
		nand_reset();
		nand_set_mode(CNFG_OP_READ);
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
		nand_set_command(0x00);
		nand_set_address(0,0xC800,2,3);//dummy read, add don't care
		nand_set_command(0x30);
	} else {
		DRV_WriteReg(NFI_CNFG_REG16, reg_val);
		nand_set_command(0x38);
	}
	timeout = 0xffff;
	while (!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));
	nand_reset();
	rr_table = (u8 *) (NAND_HYX_RR_TBL_BUF);
	if (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX) {
		if ((rr_table[0] != 8) || (rr_table[1] != 8)) {
			RR_TABLE_EXIST = FALSE;
			ASSERT(0);
		}
	} else if ((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
	           || (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
		for (read_count=0; read_count<8; read_count++) {
			if ((rr_table[read_count] != READ_RETRY_STEP) || (rr_table[read_count+8] != 4)) {
				RR_TABLE_EXIST = FALSE;
				break;
			}
		}
	}
	if (RR_TABLE_EXIST) {
		for (table_index = 0; table_index < 8; table_index++) {
			if (hynix_rr_table_select(table_index, deviceinfo)) {
				real_hynix_rr_table_idx = table_index;
				MSG(INIT, "Hynix rr_tbl_id %d\n", real_hynix_rr_table_idx);
				break;
			} else {
				if ((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
				        || (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
					MSG(INIT, "RR Table not found\n");
					ASSERT(0);
				}
			}
		}
		if (table_index == 8) {
			MSG(INIT, "RR Table not found\n");
			ASSERT(0);
		}
	} else {
		MSG(INIT, "Hynix RR table index error!\n");
	}
}

static void HYNIX_Set_RR_Para(u32 rr_index, flashdev_info *deviceinfo)
{
	u32 reg_val = 0;
	u32 timeout = 0xffff;
	u8 count, max_count = 8;
	u8 add_reg[9] = {0xCC, 0xBF, 0xAA, 0xAB, 0xCD, 0xAD, 0xAE, 0xAF};
	u32 table_size = (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX)?SINGLE_RR_TABLE_SIZE : SINGLE_RR_TABLE_16NM_SIZE;
	u8 *hynix_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF+table_size*real_hynix_rr_table_idx*2+2;
	if ((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
	        || (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
		if (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
			add_reg[0] = 0x38; //0x38, 0x39, 0x3A, 0x3B
		else //FDIE
			add_reg[0] = 0x0E; //0x0E, 0x0F, 0x10, 0x11
		for (count =1; count < 4; count++) {
			add_reg[count] = add_reg[0] + count;
		}
#if 1
		hynix_rr_table = (u8 *)real_hynix_rr_tabl;
#else
		hynix_rr_table += 14;
#endif
		max_count = 4;
	}
	nand_reset();

	DRV_WriteReg(NFI_CNFG_REG16, (CNFG_OP_CUST | CNFG_BYTE_RW));
	nand_set_command(0x36);

	for (count = 0; count < max_count; count++) {
		//nand_set_command(0x36);
		udelay(1);
		nand_set_address(add_reg[count], 0, 1, 0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		timeout = 0xffff;
		WAIT_NFI_PIO_READY(timeout);
		DRV_WriteReg32(NFI_DATAW_REG32, hynix_rr_table[rr_index*max_count + count]);
		//nand_reset();
	}
	nand_set_command(0x16);
}

static void mtk_nand_hynix_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	if (defValue == FALSE) {
		if (g_hynix_retry_count == READ_RETRY_STEP) {
			g_hynix_retry_count = 0;
		}
		HYNIX_Set_RR_Para(g_hynix_retry_count, &deviceinfo);
		/* HYNIX_Get_RR_Para(g_hynix_retry_count, &deviceinfo); */
		g_hynix_retry_count++;
	}
	/* HYNIX_Set_RR_Para(retryCount, &deviceinfo); */
}

static void mtk_nand_hynix_16nm_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	if (defValue == FALSE) {
		if (g_hynix_retry_count == READ_RETRY_STEP) {
			g_hynix_retry_count = 0;
		}
		HYNIX_Set_RR_Para(g_hynix_retry_count, &deviceinfo);
		/* mb(); */
		/* HYNIX_Get_RR_Para(g_hynix_retry_count, &deviceinfo); */
		g_hynix_retry_count++;

	}
	/* HYNIX_Set_RR_Para(retryCount, &deviceinfo); */
}

static void mtk_nand_hynix_fdie_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	if (defValue == FALSE) {
		if (g_hynix_retry_count == READ_RETRY_STEP) {
			g_hynix_retry_count = 0;
		}
		HYNIX_Set_RR_Para(g_hynix_retry_count, &deviceinfo);
		//mb();
		//HYNIX_Get_RR_Para(g_hynix_retry_count, &deviceinfo);
		g_hynix_retry_count ++;

	}
	//HYNIX_Set_RR_Para(retryCount, &deviceinfo);
}

u32 special_rrtry_setting[37] = {
	0x00000000, 0x7C00007C, 0x787C0004, 0x74780078,
	0x7C007C08, 0x787C7C00, 0x74787C7C, 0x70747C00,
	0x7C007800, 0x787C7800, 0x74787800, 0x70747800,
	0x6C707800, 0x00040400, 0x7C000400, 0x787C040C,
	0x7478040C, 0x7C000810, 0x00040810, 0x04040C0C,
	0x00040C10, 0x00081014, 0x000C1418, 0x7C040C0C,
	0x74787478, 0x70747478, 0x6C707478, 0x686C7478,
	0x74787078, 0x70747078, 0x686C7078, 0x6C707078,
	0x6C706C78, 0x686C6C78, 0x64686C78, 0x686C6874,
	0x64686874,
};

#if defined(MTK_TLC_NAND_SUPPORT)
u32 sandisk_tlc_rrtbl_12h[40]= {
	0x00000000, 0x08000004, 0x00000404, 0x04040408,
	0x08040408, 0x0004080C, 0x04040810, 0x0C0C0C00,
	0x0E0E0E00, 0x10101000, 0x12121200, 0x080808FC,
	0xFC08FCF8, 0x0000FBF6, 0x0408FBF4, 0xFEFCF8FA,
	0xFCF8F4EC, 0xF8F8F8EC, 0x0002FCE4, 0xFCFEFEFE,
	0xFFFC00FD, 0xFEFB00FC, 0xFEFAFEFA, 0xFDF9FDFA,
	0xFBF8FBFA, 0xF9F7FAF8, 0xF8F6F9F4, 0xF5F4F8F2,
	0xF4F2F6EE, 0xF0F0F4E8, 0xECECF0E6, 0x020400FA,
	0x00FEFFF8, 0xFEFEFDF6, 0xFDFDFCF4, 0xFBFCFCF2,
	0xF9FBFBF0, 0xF8F9F9EE, 0xF6F8F8ED, 0xF4F7F6EA,
};

u32 sandisk_tlc_rrtbl_13h[40]= {
	0x00000000, 0x00040800, 0x00080004, 0x00020404,
	0x00040800, 0x00080000, 0x00FC0000, 0x000C0C0C,
	0x000E0E0E, 0x00101010, 0x00141414, 0x000008FC,
	0x0004FCF8, 0x00FC00F6, 0x00FC0404, 0x00FCFE08,
	0x00FCFC00, 0x00F8F8FA, 0x000000F4, 0x00FAFC02,
	0x00F8FF00, 0x00F6FDFE, 0x00F4FBFC, 0x00F2F9FA,
	0x00F0F7F8, 0x00EEF5F6, 0x00ECF3F4, 0x00EAF1F2,
	0x00E8ECEE, 0x00E0E4E8, 0x00DAE0E2, 0x00000000,
	0x00FEFEFE, 0x00FBFCFC, 0x00F9FAFA, 0x00F7F8F8,
	0x00F5F6F6, 0x00F3F4F4, 0x00F1F2F2, 0x00EFF0EF,
};

u32 sandisk_tlc_rrtbl_14h[11]= {
	0x00000000, 0x00000010, 0x00000020, 0x00000030,
	0x00000040, 0x00000050, 0x00000060, 0x000000F0,
	0x000000E0, 0x000000D0, 0x000000C0,
};

u32 sandisk_1z_tlc_rrtbl_12h[47] = {
	0x00000000, 0x081000F8, 0xF8F0F8F8, 0x000000E8,
	0x08F80800, 0xF8F8F8F0, 0xF80808F0, 0x08080808,
	0xF800F000, 0x00000800, 0xF80000F8, 0x08F800F8,
	0x000808F8, 0x00F8F808, 0x0000F8F0, 0x00080008,
	0x00100010, 0xF8F000E8, 0xF80800F0, 0x00101008,
	0x0018F810, 0x08180810, 0x00101018, 0x00040000,
	0x00100808, 0x00081000, 0xF80000F8, 0xF8F8F8F0,
	0x00F004E8, 0x08F008E8, 0x08E810E0, 0x100008E0,
	0xF80004E0, 0x080004E0, 0x000004E0, 0x100004E8,
	0x0800FCE8, 0x0000FC00, 0x0000FC00, 0xF800FC00,
	0xF000FC00, 0xF000F000, 0xF000F800, 0xE800F000,
	0xE800E800, 0xF000E800, 0xE000E800,
};

u32 sandisk_1z_tlc_rrtbl_13h[47] = {
	0x00000000, 0x00F80008, 0x00F8F8F8, 0x00E0F0F8,
	0x00000008, 0x00E8F000, 0x0000F0F0, 0x00E8F8F8,
	0x00F8F8F0, 0x00E800F8, 0x0008F8E8, 0x00F8F8F0,
	0x00F8F800, 0x00F0F800, 0x00F000F8, 0x00F0F8F0,
	0x000008F8, 0x00F0F0F0, 0x000800E8, 0x0008F808,
	0x0000FC08, 0x00F8FC00, 0x00F00000, 0x00FCFCFC,
	0x0010FC10, 0x0010FC10, 0x0010FC10, 0x0008FC08,
	0x00000008, 0x00E80800, 0x00E808F8, 0x00001000,
	0x0000F8F0, 0x000008E8, 0x0000FCE0, 0x000010E8,
	0x000008E0, 0x00000000, 0x0000FC00, 0x0000F800,
	0x0000F000, 0x0000F000, 0x0000F000, 0x0000E800,
	0x0000E800, 0x0000F000, 0x0000E000,
};

u32 sandisk_1z_tlc_rrtbl_14h[25] = {
	0x00000000, 0x00000008, 0x000000F8, 0x00000010,
	0x000000F0, 0x00000018, 0x000000E8, 0x00000020,
	0x000000E0, 0x00000028, 0x000000D8, 0x00000030,
	0x000000D0, 0x00000038, 0x000000C8, 0x00000040,
	0x000000C0, 0x00000048, 0x000000B8, 0x00000050,
	0x000000B0, 0x00000058, 0x00000060, 0x00000068,
	0x00000070,
};

static void mtk_nand_sandisk_tlc_rrtry(flashdev_info deviceinfo, u32 feature, bool defValue, int process)
{
	u16 reg_val = 0;
	u32 timeout = TIMEOUT_3;
	u32 value1, value2, value3;

	if ((feature > 1) || defValue) { //add exit rr cmd sequence
		//set 0x55h cmd + 0x00h address + 0x00 data
		nand_reset();
		reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
		DRV_WriteReg(NFI_CNFG_REG16, reg_val);

		nand_set_command(0x55);
		nand_set_address(0, 0, 1, 0);
		DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
		NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
		DRV_WriteReg(NFI_STRDATA_REG16, 0x1);
		WAIT_NFI_PIO_READY(timeout)
		if (timeout == 0) {
			MSG(INIT, "mtk_nand_sandisk_tlc_1ynm_rrtry: timeout\n");
		}
		DRV_WriteReg32(NFI_DATAW_REG32, 0);

		//set device reset
		nand_device_reset();
	}

	if (devinfo.tlcControl.slcopmodeEn) { //slc block
		if (process == 1)
			value3 = sandisk_tlc_rrtbl_14h[feature];
		else if (process == 2)
			value3 = sandisk_1z_tlc_rrtbl_14h[feature];
		mtk_nand_SetFeature(deviceinfo.feature_set.FeatureSet.sfeatureCmd, 0x14, (u8 *)&value3, 4);
	} else { //tlc block
		if (process == 1) {
			value1 = sandisk_tlc_rrtbl_12h[feature];
			value2 = sandisk_tlc_rrtbl_13h[feature];
		} else if (process == 2) {
			value1 = sandisk_1z_tlc_rrtbl_12h[feature];
			value2 = sandisk_1z_tlc_rrtbl_13h[feature];
		}
		mtk_nand_SetFeature(deviceinfo.feature_set.FeatureSet.sfeatureCmd, 0x12, (u8 *)&value1, 4);
		mtk_nand_SetFeature(deviceinfo.feature_set.FeatureSet.sfeatureCmd, 0x13, (u8 *)&value2, 4);
	}

	if (FALSE == defValue) {
		//set 0x5D cmd
		nand_reset();
		reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
		DRV_WriteReg(NFI_CNFG_REG16, reg_val);

		nand_set_command(0x5D);

		nand_reset();
	}
}

static void mtk_nand_sandisk_tlc_1ynm_rrtry(flashdev_info deviceinfo, u32 feature, bool defValue)
{
	mtk_nand_sandisk_tlc_rrtry(deviceinfo, feature, defValue, 1);
}

static void mtk_nand_sandisk_tlc_1znm_rrtry(flashdev_info deviceinfo, u32 feature, bool defValue)
{
	mtk_nand_sandisk_tlc_rrtry(deviceinfo, feature, defValue, 2);
}

#endif
/* sandisk 1z nm */
u32 sandisk_1znm_rrtry_setting[33] = {
	0x00000000, 0x00000404, 0x00007C7C, 0x7C7C0404,
	0x00000808, 0x7C7C0000, 0x7C7C0808, 0x04080404,
	0x04040000, 0x7C007C7C, 0x04080808, 0x787C040C,
	0x78780000, 0x00000C0C, 0x00007878, 0x04007C7C,
	0x7C747878, 0x78787C7C, 0x08040404, 0x04080C08,
	0x08080808, 0x78787878, 0x007C7C7C, 0x00040408,
	0x00080000, 0x00780804, 0x7C780C08, 0x7874087C,
	0x74787C7C, 0x74740000, 0x08000000, 0x747C7878,
	0x74700478,
};

u32 sandisk_1znm_rrtry_slc[25] = {
	0x00000000, 0x00000004, 0x0000007C, 0x00000008,
	0x00000078, 0x0000000C, 0x00000074, 0x00000010,
	0x00000070, 0x00000014, 0x0000006C, 0x00000018,
	0x00000068, 0x0000001C, 0x00000064, 0x00000020,
	0x00000060, 0x00000024, 0x0000005C, 0x00000028,
	0x00000058, 0x0000002C, 0x00000030, 0x00000034,
	0x00000038,
};

u32 sandisk_1znm_8GB_rrtry_setting[32] = {
	0x00000000, 0x78780404, 0x747C0404, 0x78040000,
	0x7C7C0404, 0x7C000000, 0x74000000, 0x78040808,
	0x78047C7C, 0x7C007C7C, 0x707C0404, 0x74747C7C,
	0x70780000, 0x78080C0C, 0x7C7C7878, 0x04080404,
	0x78087878, 0x70787C7C, 0x6C707878, 0x6C740000,
	0x74000808, 0x6C787C7C, 0x04040000, 0x6C747474,
	0x707C7878, 0x74000C0C, 0x080C0404, 0x747C7878,
	0x68707878, 0x70000808, 0x780C1010, 0x080C0000,
};

u32 sandisk_1znm_8GB_rrtry_slc[25] = {
	0x00000000, 0x00000004, 0x0000007C, 0x00000008,
	0x00000078, 0x0000000C, 0x00000074, 0x00000010,
	0x00000070, 0x00000014, 0x0000006C, 0x00000018,
	0x00000068, 0x0000001C, 0x00000064, 0x00000020,
	0x00000060, 0x00000024, 0x0000005C, 0x00000028,
	0x00000058, 0x0000002C, 0x00000030, 0x00000034,
	0x00000038,
};

static u32 mtk_nand_rrtry_setting(flashdev_info deviceinfo, enum readRetryType type, u32 retryStart, u32 loopNo)
{
	u32 value;
	/* if(RTYPE_MICRON == type || RTYPE_SANDISK== type || RTYPE_TOSHIBA== type || RTYPE_HYNIX== type) */
	{
		if (retryStart == 0xFFFFFFFE) {
			/* sandisk 1znm 16GB MLC */
			if (devinfo.tlcControl.slcopmodeEn)
				value = sandisk_1znm_rrtry_slc[loopNo];
			else
				value = sandisk_1znm_rrtry_setting[loopNo];
		} else if (retryStart == 0xFFFFFFFD) {
			/* sandisk 1znm 8GB MLC */
			if (devinfo.tlcControl.slcopmodeEn)
				value = sandisk_1znm_8GB_rrtry_slc[loopNo];
			else
				value = sandisk_1znm_8GB_rrtry_setting[loopNo];
		} else if (retryStart != 0xFFFFFFFF) {
			value = retryStart + loopNo;
		} else {
			value = special_rrtry_setting[loopNo];
		}
	}

	return value;
}

typedef u32(*rrtryFunctionType) (flashdev_info deviceinfo, u32 feature, bool defValue);

static rrtryFunctionType rtyFuncArray[] = {
	mtk_nand_micron_rrtry,
	mtk_nand_sandisk_rrtry,
	mtk_nand_sandisk_19nm_rrtry,
	mtk_nand_toshiba_rrtry,
	mtk_nand_toshiba_15nm_rrtry,
	mtk_nand_hynix_rrtry,
	mtk_nand_hynix_16nm_rrtry,
#if defined(MTK_TLC_NAND_SUPPORT)
	mtk_nand_sandisk_tlc_1ynm_rrtry,
#endif
	mtk_nand_hynix_fdie_rrtry,
#if defined(MTK_TLC_NAND_SUPPORT)
	mtk_nand_toshiba_rrtry_tlc_a19nm,
	mtk_nand_sandisk_tlc_1znm_rrtry
#endif
};

static void mtk_nand_rrtry_func(flashdev_info deviceinfo, u32 feature, bool defValue)
{
	rtyFuncArray[deviceinfo.feature_set.FeatureSet.rtype] (deviceinfo, feature, defValue);
}

int nand_exec_read_page_hw(struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8 * pPageBuf, u8 * pFDMBuf)
{
	int bRet;
	u32 u4SecNum = u4PageSize >> nand->sector_shift;
	/* bool retry = FALSE; */
	bool readRetry = FALSE;
	int retryCount = 0;
	u32 reg_val = 0;
	u32 real_row_addr = 0;
	u32 block_addr = 0;
	u32 page_in_block = 0;
	u32 page_per_block = 0;
	u32 retrytotalcnt = devinfo.feature_set.FeatureSet.readRetryCnt;
#if defined(MTK_TLC_NAND_SUPPORT)
	NFI_TLC_WL_INFO  tlc_wl_info;
	bool   tlc_left_plane = TRUE;
	int spare_per_sector = nand->oobsize/u4SecNum;
	u32   snd_real_row_addr = 0;
	NFI_TLC_WL_INFO  snd_tlc_wl_info;
	u32   logical_plane_num = 1;
	u32   data_sector_num = 0;
	u8    *temp_byte_ptr = NULL;
	u8    *spare_ptr = NULL;
	bool ready_to_read = TRUE;
#endif
#if MLC_MICRON_SLC_MODE
	u8 feature[4];
#endif

#if CFG_2CS_NAND
	if (g_bTricky_CS) {
		u4RowAddr = mtk_nand_cs_on(NFI_TRICKY_CS, u4RowAddr);
	}
#endif
	/* MSG(INFO, "nand_exec_read_page_hw_single 0x%x\n", u4RowAddr); */
	page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	do {
		data_sector_num = u4SecNum;
		temp_byte_ptr = pPageBuf;
		spare_ptr = pFDMBuf;
		logical_plane_num = 1;

#if defined(MTK_TLC_NAND_SUPPORT)
		if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
			if (devinfo.tlcControl.normaltlc) { // normal tlc
				NFI_TLC_GetMappedWL(u4RowAddr, &tlc_wl_info);
				real_row_addr = NFI_TLC_GetRowAddr(tlc_wl_info.word_line_idx);

				if (devinfo.tlcControl.pPlaneEn) {
					tlc_left_plane = TRUE; // begin at left logical plane
					logical_plane_num = 2;
					data_sector_num /= 2;
					real_row_addr = NFI_TLC_SetpPlaneAddr(real_row_addr, tlc_left_plane);
				}

				if (devinfo.two_phyplane) {
					NFI_TLC_GetMappedWL(u4RowAddr + page_per_block, &snd_tlc_wl_info);
					snd_real_row_addr = NFI_TLC_GetRowAddr(snd_tlc_wl_info.word_line_idx);

					logical_plane_num *= 2;
					data_sector_num /= 2;

					if (devinfo.tlcControl.pPlaneEn) {
						tlc_left_plane = TRUE; // begin at left logical plane
						snd_real_row_addr = NFI_TLC_SetpPlaneAddr(snd_real_row_addr, tlc_left_plane);
					}
				}
			} else {
				real_row_addr = NFI_TLC_GetRowAddr(u4RowAddr);
			}

			if (devinfo.tlcControl.slcopmodeEn) { // slc mode
				if (0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);

					nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);

					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
				}
			} else { //tlc mode
				if (devinfo.tlcControl.normaltlc) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);

					if (tlc_wl_info.wl_pre == WL_LOW_PAGE)
						nand_set_command(LOW_PG_SELECT_CMD);
					else if (tlc_wl_info.wl_pre == WL_MID_PAGE)
						nand_set_command(MID_PG_SELECT_CMD);
					else if (tlc_wl_info.wl_pre == WL_HIGH_PAGE)
						nand_set_command(HIGH_PG_SELECT_CMD);

					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
				}
			}
			reg_val = 0; // reset reg_val
		} else
#endif
		{
			real_row_addr = u4RowAddr;
			if (devinfo.tlcControl.slcopmodeEn) { /*  slc mode */

				if (devinfo.vendor == VEND_MICRON) {
#if MLC_MICRON_SLC_MODE
					feature[0] = 0x00;
					feature[1] = 0x01;
					feature[2] = 0x00;
					feature[3] = 0x00;
					mtk_nand_SetFeature((u16) devinfo.feature_set.FeatureSet.sfeatureCmd,
					                    0x91, (u8 *) &feature, 4);
#else
					block_addr = real_row_addr/page_per_block;
					page_in_block = real_row_addr % page_per_block;
					page_in_block = functArray[devinfo.feature_set.ptbl_idx](page_in_block);
					real_row_addr = page_in_block + block_addr * page_per_block;
					/*printk("mtk_nand_exec_read_page SLC Mode real_row_addr:%d, u4RowAddr:%d\n",
					    real_row_addr, u4RowAddr);*/
#endif
				} else if (0xFF != devinfo.tlcControl.en_slc_mode_cmd) {

					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);

					nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);

					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);

					if (devinfo.vendor == VEND_SANDISK) {
						block_addr = real_row_addr/page_per_block;
						page_in_block = real_row_addr % page_per_block;
						page_in_block <<= 1;
						real_row_addr = page_in_block + block_addr * page_per_block;
						/*printk("mtk_nand_exec_read_sector SLC Mode real_row_addr:%d, u4RowAddr:%d\n",
						real_row_addr, u4RowAddr);*/
					}
				}
			}

		}

		if (use_randomizer && u4RowAddr >= RAND_START_ADDR /*&& retry == FALSE */) {
			mtk_nand_turn_on_randomizer(u4RowAddr, 0, 0);
		} else if (pre_randomizer && u4RowAddr < RAND_START_ADDR /*&& retry == FALSE */) {
			mtk_nand_turn_on_randomizer(u4RowAddr, 0, 0);
		}

		bRet = ERR_RTN_SUCCESS;
#if defined(MTK_TLC_NAND_SUPPORT)
		if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			tlc_snd_phyplane = FALSE;
#endif

		ready_to_read = nand_ready_for_read(nand, real_row_addr, 0, true, pPageBuf);

#if defined(MTK_TLC_NAND_SUPPORT)
		if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		        && (devinfo.tlcControl.normaltlc)
		        && devinfo.two_phyplane) {
			if (!devinfo.tlcControl.slcopmodeEn) {
				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				if (tlc_wl_info.wl_pre == WL_LOW_PAGE)
					nand_set_command(LOW_PG_SELECT_CMD);
				else if (tlc_wl_info.wl_pre == WL_MID_PAGE)
					nand_set_command(MID_PG_SELECT_CMD);
				else if (tlc_wl_info.wl_pre == WL_HIGH_PAGE)
					nand_set_command(HIGH_PG_SELECT_CMD);

				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);
			}

			tlc_snd_phyplane = TRUE;
			ready_to_read = nand_ready_for_read(nand, snd_real_row_addr, 0, true, pPageBuf);
		}
#endif
		if (ready_to_read) {
#if defined(MTK_TLC_NAND_SUPPORT)
			tlc_snd_phyplane = FALSE;
#endif
			while (logical_plane_num) {
#if defined(MTK_TLC_NAND_SUPPORT)
				if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
					if (devinfo.tlcControl.needchangecolumn || devinfo.two_phyplane) {
						////////////change colunm address///////////
						if (devinfo.tlcControl.pPlaneEn) {
							if (tlc_snd_phyplane)
								snd_real_row_addr = NFI_TLC_SetpPlaneAddr(snd_real_row_addr, tlc_left_plane);
							else
								real_row_addr = NFI_TLC_SetpPlaneAddr(real_row_addr, tlc_left_plane);
						}
#if 1 //reset here to flush fifo status left by the left plane read
						reg_val = DRV_Reg32(NFI_CON_REG16);
						reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
						/* issue reset operation */
						DRV_WriteReg32(NFI_CON_REG16, reg_val); //not use SAL_NFI_Reset() here
#endif

						reg_val = DRV_Reg(NFI_CNFG_REG16);
						reg_val &= ~CNFG_READ_EN;
						reg_val &= ~CNFG_OP_MODE_MASK;
						reg_val |= CNFG_OP_CUST;
						DRV_WriteReg(NFI_CNFG_REG16, reg_val);

						nand_set_command(CHANGE_COLUNM_ADDR_1ST_CMD);
						if (tlc_snd_phyplane)
							nand_set_address(0, snd_real_row_addr, 2, devinfo.addr_cycle - 2);
						else
							nand_set_address(0, real_row_addr, 2, devinfo.addr_cycle - 2);
						nand_set_command(CHANGE_COLUNM_ADDR_2ND_CMD);

						reg_val = DRV_Reg(NFI_CNFG_REG16);
						reg_val |= CNFG_READ_EN;
						reg_val &= ~CNFG_OP_MODE_MASK;
						reg_val |= CNFG_OP_READ;
						DRV_WriteReg(NFI_CNFG_REG16, reg_val);
					}
				}

				if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
					DRV_WriteReg32(NFI_STRADDR_REG32, temp_byte_ptr);

					DRV_WriteReg32(NFI_CON_REG16, data_sector_num << CON_NFI_SEC_SHIFT);

					ECC_Decode_Start();
				}
#endif

				if (!nand_read_page_data(temp_byte_ptr, data_sector_num * (1 << nand->sector_shift))) {
					MSG(INIT, "nand_exec_read_page_hw: fail 1\n");
					//dump_nfi();
					bRet = ERR_RTN_FAIL;
				}
				if (!nand_status_ready(STA_NAND_BUSY)) {
					MSG(INIT, "nand_exec_read_page_hw: fail 2\n");
					//dump_nfi();
					bRet = ERR_RTN_FAIL;
				}

				if (!nand_check_dececc_done(data_sector_num)) {
					MSG(INIT, "nand_exec_read_page_hw: fail 3\n");
					bRet = ERR_RTN_FAIL;
				}

				nand_read_fdm_data(spare_ptr, data_sector_num);

				if (!nand_check_bch_error(temp_byte_ptr, data_sector_num - 1, u4RowAddr)) {
					bRet = ERR_RTN_BCH_FAIL;
					if (devinfo.vendor != VEND_NONE) {
						readRetry = TRUE;
					}
					g_i4ErrNum++;
				}
				if (0 != (DRV_Reg32(NFI_STA_REG32) & STA_READ_EMPTY)) {
					if (retryCount != 0) {
						MSG(INFO, "NFI read retry read empty page, return as uncorrectable!\n");
						bRet = ERR_RTN_BCH_FAIL;
					} else {
						memset(pPageBuf, 0xFF, u4PageSize);
						memset(pFDMBuf, 0xFF, nand->nand_fdm_size*u4SecNum);
						readRetry = FALSE;
						bRet = ERR_RTN_SUCCESS;
					}
				}
				nand_stop_read();

#if defined(MTK_TLC_NAND_SUPPORT)
				if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
					if (devinfo.tlcControl.needchangecolumn || devinfo.two_phyplane)
						DRV_WriteReg(NFI_TLC_RD_WHR2_REG16, 0x055); //disable

					if (devinfo.two_phyplane) {
						if (4 == logical_plane_num) {
							tlc_left_plane = FALSE;
						} else if (3 == logical_plane_num) {
							tlc_left_plane = TRUE;
							tlc_snd_phyplane = TRUE;
						}
					}
					if (2 == logical_plane_num) {
						tlc_left_plane = FALSE;
						if (!devinfo.tlcControl.pPlaneEn) {
							tlc_left_plane = TRUE;
							tlc_snd_phyplane = TRUE;
						}
					}

					spare_ptr += (nand->nand_fdm_size * data_sector_num);
					temp_byte_ptr += (data_sector_num *(1 << nand->sector_shift));
				}
#endif
				logical_plane_num --;

				if (bRet == ERR_RTN_BCH_FAIL)
					break;
			}
		}

		if (use_randomizer && u4RowAddr >= RAND_START_ADDR) {
			mtk_nand_turn_off_randomizer();
		} else if (pre_randomizer && u4RowAddr < RAND_START_ADDR) {
			mtk_nand_turn_off_randomizer();
		}

#if defined(MTK_TLC_NAND_SUPPORT)
		if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		        || (devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER)) {
			if ((devinfo.tlcControl.slcopmodeEn)
			        &&(0xFF != devinfo.tlcControl.dis_slc_mode_cmd)) {
				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);

				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
			}
		}
#endif
		if (bRet == ERR_RTN_BCH_FAIL) {
			u32 feature =
			    mtk_nand_rrtry_setting(devinfo, devinfo.feature_set.FeatureSet.rtype, devinfo.feature_set.FeatureSet.readRetryStart, retryCount);
			/* for sandisk 1znm slc mode read retry */
			if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_SANDISK)
			        && (devinfo.tlcControl.slcopmodeEn))
				retrytotalcnt = 25;
#if defined(MTK_TLC_NAND_SUPPORT)
			if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_SANDISK_TLC_1YNM)
			        && (devinfo.tlcControl.slcopmodeEn))
				retrytotalcnt = 10;
			else if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_SANDISK_TLC_1ZNM)
			         && (devinfo.tlcControl.slcopmodeEn))
				retrytotalcnt = 24;
			else if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_TOSHIBA_TLC_A19NM)
			         && (devinfo.tlcControl.slcopmodeEn))
				retrytotalcnt = 5;
#endif
			if (retryCount < retrytotalcnt) {
				mtk_nand_rrtry_func(devinfo, feature, FALSE);
				retryCount++;
			} else {
				feature = devinfo.feature_set.FeatureSet.readRetryDefault;
#if defined(MTK_TLC_NAND_SUPPORT)
				if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_TOSHIBA_TLC_A19NM)
				        && (devinfo.tlcControl.slcopmodeEn))
					feature = 5;
#endif
				if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_SANDISK) && (g_sandisk_retry_case < 2)) {
					g_sandisk_retry_case++;
					mtk_nand_rrtry_func(devinfo, feature, FALSE);
					retryCount = 0;
				} else {
					mtk_nand_rrtry_func(devinfo, feature, TRUE);
					readRetry = FALSE;
					g_sandisk_retry_case = 0;
				}
			}
			if (g_sandisk_retry_case == 1) {
				nand_set_command(0x26);
			}
		} else {
			if (retryCount != 0) {
				u32 feature = devinfo.feature_set.FeatureSet.readRetryDefault;
#if defined(MTK_TLC_NAND_SUPPORT)
				if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_TOSHIBA_TLC_A19NM)
				        && (devinfo.tlcControl.slcopmodeEn))
					feature = 5;
#endif
				mtk_nand_rrtry_func(devinfo, feature, TRUE);
			}
			readRetry = FALSE;
			g_sandisk_retry_case = 0;
		}
		if (TRUE == readRetry)
			bRet = ERR_RTN_SUCCESS;
	} while (readRetry);
	if (retryCount != 0) {
		u32 feature = devinfo.feature_set.FeatureSet.readRetryDefault;
		if (bRet == ERR_RTN_SUCCESS) {
			printf("u4RowAddr:0x%x read retry pass, retrycnt:%d ENUM0:%x,ENUM1:%x \n",u4RowAddr,retryCount,DRV_Reg32(ECC_DECENUM1_REG32),DRV_Reg32(ECC_DECENUM0_REG32));
			if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
			        || (devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX)
			        || (devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
				g_hynix_retry_count--;
			}
		} else {
			printf("u4RowAddr:0x%x read retry fail\n",u4RowAddr);
		}
		mtk_nand_rrtry_func(devinfo, feature, TRUE);
		g_sandisk_retry_case = 0;
	}
	/* if(use_randomizer && u4RowAddr >= RAND_START_ADDR) */
	/* {             mtk_nand_turn_off_randomizer();} */
	/* else if(pre_randomizer && u4RowAddr < RAND_START_ADDR) */
	/* {             mtk_nand_turn_off_randomizer();} */

	return bRet;
}

static bool nand_exec_read_page(struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8 *pPageBuf, u8 *pFDMBuf)
{
	int bRet = ERR_RTN_SUCCESS;
	u32 page_per_block = (devinfo.blocksize * 1024) / devinfo.pagesize;
	u32 block;
	/* int page_in_block = u4RowAddr % page_per_block; */
	u32 page_addr;
	u32 mapped_block;
	int i, start, len, offset;
	struct nand_oobfree *free;
	u8 oob[256];

	/* mapped_block = get_mapping_block_index(block); */
	page_addr = mtk_nand_page_transform((u64) u4RowAddr << g_nand_chip.page_shift, &block, &mapped_block);

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)
	        && (devinfo.two_phyplane)) {
		page_addr = (mapped_block << 1) * page_per_block + (page_addr % page_per_block);
	}
#endif
	if (devinfo.two_phyplane) {
		page_addr = (mapped_block << 1) * page_per_block + (page_addr % page_per_block);
	}

	/* bRet = nand_exec_read_page_hw(nand, (mapped_block * page_per_block + page_in_block), u4PageSize, pPageBuf, oob); */
	bRet = nand_exec_read_page_hw(nand, page_addr, u4PageSize, pPageBuf, oob);
	if (bRet == ERR_RTN_FAIL)
		return false;

	offset = 0;
	free = nand->ecclayout->oobfree;
	for (i = 0; i < MTD_MAX_OOBFREE_ENTRIES && free[i].length; i++) {
		start = free[i].offset;
		len = free[i].length;
		memcpy(pFDMBuf + offset, oob + start, len);
		offset += len;
	}

	return bRet;
}

static bool nand_exec_write_page_raw(struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8 * pPageBuf, u8 * pFDMBuf);

static bool nand_exec_write_page(struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8 *pPageBuf, u8 *pFDMBuf)
{
	bool bRet = true;
	/* u32 page_per_block = 1 << (nand->phys_erase_shift - nand->page_shift); */
	u32 block;
	/* int page_in_block = u4RowAddr % page_per_block; */
	u32 u4SecNum = u4PageSize >> nand->sector_shift;
	u32 page_addr;
	u32 mapped_block;
	u32 reg_val;
	u32 real_row_addr = 0;
	u32 block_addr = 0;
	u32 page_in_block = 0;
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
#if MLC_MICRON_SLC_MODE
	u8 feature[4];
#endif

	page_addr = mtk_nand_page_transform((u64) u4RowAddr << g_nand_chip.page_shift, &block, &mapped_block);
	if (devinfo.two_phyplane) {
		page_addr = (mapped_block << 1) * page_per_block + (page_addr % page_per_block);
		bRet = nand_exec_write_page_raw(nand, page_addr, u4PageSize, pPageBuf, pFDMBuf);
	} else {
#if CFG_2CS_NAND
		if (g_bTricky_CS) {
			page_addr = mtk_nand_cs_on(NFI_TRICKY_CS, page_addr);
		}
#endif
		if (use_randomizer && page_addr >= RAND_START_ADDR) {
			mtk_nand_turn_on_randomizer(page_addr, 1, 0);
		} else if (pre_randomizer && page_addr < RAND_START_ADDR) {
			mtk_nand_turn_on_randomizer(page_addr, 1, 0);
		}

		page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
		real_row_addr = page_addr;
		if (devinfo.tlcControl.slcopmodeEn) { // slc mode
			if (devinfo.vendor == VEND_MICRON) {
#if MLC_MICRON_SLC_MODE
				feature[0] = 0x00;
				feature[1] = 0x01;
				feature[2] = 0x00;
				feature[3] = 0x00;
				mtk_nand_SetFeature((u16) devinfo.feature_set.FeatureSet.sfeatureCmd,
				                    0x91, (u8 *) &feature, 4);
#else
				block_addr = real_row_addr/page_per_block;
				page_in_block = real_row_addr % page_per_block;
				page_in_block = functArray[devinfo.feature_set.ptbl_idx](page_in_block);
				real_row_addr = page_in_block + block_addr * page_per_block;
				/*printk("mtk_nand_exec_read_page SLC Mode real_row_addr:%d, u4RowAddr:%d\n",
				    real_row_addr, u4RowAddr);*/
#endif
			} else if (0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);
				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);
				if (devinfo.vendor == VEND_SANDISK) {
					block_addr = real_row_addr/page_per_block;
					page_in_block = real_row_addr % page_per_block;
					page_in_block <<= 1;
					real_row_addr = page_in_block + block_addr * page_per_block;
					/*printk("mtk_nand_exec_read_sector SLC Mode real_row_addr:%d, u4RowAddr:%d\n",
					real_row_addr, u4RowAddr);*/
				}
			}
		}

		if (nand_ready_for_write(nand, real_row_addr, pPageBuf)) {
			nand_write_fdm_data(pFDMBuf, u4SecNum);
			if (!nand_write_page_data(pPageBuf, u4PageSize)) {
				bRet = false;
			}
			if (!nand_check_RW_count(nand, u4PageSize)) {
				bRet = false;
			}
			nand_stop_write();
			if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER) {
				if ((devinfo.two_phyplane) && (!tlc_snd_phyplane))
					nand_set_command(PLANE_PROG_DATA_CMD);
				else if (tlc_cache_program)
					nand_set_command(NAND_CMD_CACHEDPROG);
				else
					nand_set_command(NAND_CMD_PAGE_PROG);
			} else
				nand_set_command(NAND_CMD_PAGE_PROG);
			while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);
		}

		if (devinfo.tlcControl.slcopmodeEn) {
#if MLC_MICRON_SLC_MODE
			if (devinfo.vendor == VEND_MICRON) {
				feature[0] = 0x02;
				feature[1] = 0x01;
				feature[2] = 0x00;
				feature[3] = 0x00;
				mtk_nand_SetFeature((u16) devinfo.feature_set.FeatureSet.sfeatureCmd,
				                    0x91, (u8 *) &feature, 4);
			} else
#endif
				if (0xFF != devinfo.tlcControl.dis_slc_mode_cmd) {
					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);
					nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
				}
		}

		if (use_randomizer && page_addr >= RAND_START_ADDR) {
			mtk_nand_turn_off_randomizer();
		} else if (pre_randomizer && page_addr < RAND_START_ADDR) {
			mtk_nand_turn_off_randomizer();
		}
	}
	return bRet;
}

static bool nand_exec_write_page_hw(struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8 *pPageBuf, u8 *pFDMBuf)
{
	bool bRet = true;
	/* u32 page_per_block = 1 << (nand->phys_erase_shift - nand->page_shift); */
	/* int block = u4RowAddr / page_per_block; */
	/* int page_in_block = u4RowAddr % page_per_block; */
	u32 u4SecNum = u4PageSize >> nand->sector_shift;
#if defined(MTK_TLC_NAND_SUPPORT)
	NFI_TLC_WL_INFO  tlc_wl_info;
#endif
	u32 reg_val;
	u32 real_row_addr = 0;
	u32 block_addr = 0;
	u32 page_in_block = 0;
	u32 page_per_block = 0;
#if MLC_MICRON_SLC_MODE
	u8 feature[4];
#endif

#if CFG_2CS_NAND
	if (g_bTricky_CS) {
		u4RowAddr = mtk_nand_cs_on(NFI_TRICKY_CS, u4RowAddr);
	}
#endif
	MSG(INFO, "write page %d\n", u4RowAddr);
	if (use_randomizer && u4RowAddr >= RAND_START_ADDR) {
		mtk_nand_turn_on_randomizer(u4RowAddr, 1, 0);
	} else if (pre_randomizer && u4RowAddr < RAND_START_ADDR) {
		mtk_nand_turn_on_randomizer(u4RowAddr, 1, 0);
	}
	page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
#if defined(MTK_TLC_NAND_SUPPORT)
	nand_reset();

	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)    {
		if (devinfo.tlcControl.normaltlc) { //normal tlc
			NFI_TLC_GetMappedWL(u4RowAddr, &tlc_wl_info);
			real_row_addr = NFI_TLC_GetRowAddr(tlc_wl_info.word_line_idx);
			if (devinfo.tlcControl.pPlaneEn) {
				real_row_addr = NFI_TLC_SetpPlaneAddr(real_row_addr, tlc_lg_left_plane);
			}
		} else {
			real_row_addr = NFI_TLC_GetRowAddr(u4RowAddr);
		}

		if (devinfo.tlcControl.slcopmodeEn) { // slc mode
			if (((!devinfo.tlcControl.pPlaneEn) || tlc_lg_left_plane)
			        && (!tlc_snd_phyplane)) {
				if (0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);

					nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);

					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
				}
			}
		} else { //tlc mode
			if (devinfo.tlcControl.normaltlc) {
				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				if (PROGRAM_1ST_CYCLE == tlc_program_cycle) {
					nand_set_command(PROGRAM_1ST_CYCLE_CMD);
				} else if (PROGRAM_2ND_CYCLE == tlc_program_cycle) {
					nand_set_command(PROGRAM_2ND_CYCLE_CMD);
				}

				if (tlc_wl_info.wl_pre == WL_LOW_PAGE)
					nand_set_command(LOW_PG_SELECT_CMD);
				else if (tlc_wl_info.wl_pre == WL_MID_PAGE)
					nand_set_command(MID_PG_SELECT_CMD);
				else if (tlc_wl_info.wl_pre == WL_HIGH_PAGE)
					nand_set_command(HIGH_PG_SELECT_CMD);

				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);
			}
		}
	} else
#endif
	{
		real_row_addr = u4RowAddr;
		if (devinfo.tlcControl.slcopmodeEn) { // slc mode
			dprintf(INFO,"SLC mode %x\n",u4RowAddr);
			if (devinfo.vendor == VEND_MICRON) {
#if MLC_MICRON_SLC_MODE
				feature[0] = 0x00;
				feature[1] = 0x01;
				feature[2] = 0x00;
				feature[3] = 0x00;
				mtk_nand_SetFeature((u16) devinfo.feature_set.FeatureSet.sfeatureCmd,
				                    0x91, (u8 *) &feature, 4);
#else
				block_addr = real_row_addr/page_per_block;
				page_in_block = real_row_addr % page_per_block;
				page_in_block = functArray[devinfo.feature_set.ptbl_idx](page_in_block);
				real_row_addr = page_in_block + block_addr * page_per_block;
				/*printk("mtk_nand_exec_read_page SLC Mode real_row_addr:%d, u4RowAddr:%d\n",
				    real_row_addr, u4RowAddr);*/
#endif
			} else if (0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
				if (!tlc_snd_phyplane) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);

					nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);
					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
				}
				if (devinfo.vendor == VEND_SANDISK) {
					block_addr = real_row_addr/page_per_block;
					page_in_block = real_row_addr % page_per_block;
					page_in_block <<= 1;
					real_row_addr = page_in_block + block_addr * page_per_block;
					MSG(INFO, "slc mode real_row_addr:%d, u4RowAddr:%d\n",
					    real_row_addr, u4RowAddr);
				}
			}
		}
	}

	if (nand_ready_for_write(nand, real_row_addr, pPageBuf)) {
		nand_write_fdm_data(pFDMBuf, u4SecNum);
		if (!nand_write_page_data(pPageBuf, u4PageSize)) {
			bRet = false;
		}
		if (!nand_check_RW_count(nand, u4PageSize)) {
			bRet = false;
		}
		nand_stop_write();
#if defined(MTK_TLC_NAND_SUPPORT)
		if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
			if (devinfo.tlcControl.normaltlc) { //normal tlc
				if ((devinfo.tlcControl.pPlaneEn) && tlc_lg_left_plane) {
					nand_set_command(PROGRAM_LEFT_PLANE_CMD);
					printf("[xl] program 1\n");
				} else {
					if (devinfo.tlcControl.slcopmodeEn) {
						if (devinfo.two_phyplane && (!tlc_snd_phyplane)) {
							nand_set_command(PROGRAM_LEFT_PLANE_CMD);
							printf("[xl] program 2\n");
						} else {
							nand_set_command(NAND_CMD_PAGE_PROG);
							printf("[xl] program 3\n");
						}
					} else if (tlc_wl_info.wl_pre == WL_HIGH_PAGE) {
						if (devinfo.two_phyplane && (!tlc_snd_phyplane)) {
							nand_set_command(PROGRAM_LEFT_PLANE_CMD);
							printf("[xl] program 4\n");
						} else {
							if (tlc_cache_program) {
								nand_set_command(NAND_CMD_CACHEDPROG);
								printf("[xl] program 5\n");
							} else {
								nand_set_command(NAND_CMD_PAGE_PROG);
								printf("[xl] program 6\n");
							}
						}
					} else {
						if (devinfo.two_phyplane && (!tlc_snd_phyplane)) {
							nand_set_command(PROGRAM_LEFT_PLANE_CMD);
							printf("[xl] program 7\n");
						} else {
							nand_set_command(PROGRAM_RIGHT_PLANE_CMD);
							printf("[xl] program 8\n");
						}
					}
				}
			} else //micron tlc
				nand_set_command(NAND_CMD_PAGE_PROG);
		} else
#endif
		{
			if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER) {
				if ((devinfo.two_phyplane) && (!tlc_snd_phyplane))
					nand_set_command(PLANE_PROG_DATA_CMD);
				else if (tlc_cache_program)
					nand_set_command(NAND_CMD_CACHEDPROG);
				else
					nand_set_command(NAND_CMD_PAGE_PROG);
			} else
				nand_set_command(NAND_CMD_PAGE_PROG);

		}
		while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);
	} else {
		bRet = FALSE;
		printf("nand_ready_for_write failed\n");
	}

#if defined(MTK_TLC_NAND_SUPPORT)
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		if ((devinfo.tlcControl.slcopmodeEn)
		        &&(0xFF != devinfo.tlcControl.dis_slc_mode_cmd)) {
			reg_val = DRV_Reg32(NFI_CON_REG16);
			reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
			/* issue reset operation */
			DRV_WriteReg32(NFI_CON_REG16, reg_val);

			reg_val = DRV_Reg(NFI_CNFG_REG16);
			reg_val &= ~CNFG_READ_EN;
			reg_val &= ~CNFG_OP_MODE_MASK;
			reg_val |= CNFG_OP_CUST;
			DRV_WriteReg(NFI_CNFG_REG16, reg_val);

			nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
		}
	} else
#endif
	{
		if (devinfo.tlcControl.slcopmodeEn) {
			if (devinfo.vendor == VEND_MICRON) {
#if MLC_MICRON_SLC_MODE
				feature[0] = 0x00;
				feature[1] = 0x01;
				feature[2] = 0x00;
				feature[3] = 0x00;
				mtk_nand_SetFeature((u16) devinfo.feature_set.FeatureSet.sfeatureCmd,
				                    0x91, (u8 *) &feature, 4);
#else
				block_addr = real_row_addr/page_per_block;
				page_in_block = real_row_addr % page_per_block;
				page_in_block = functArray[devinfo.feature_set.ptbl_idx](page_in_block);
				real_row_addr = page_in_block + block_addr * page_per_block;
				/*printk("mtk_nand_exec_read_page SLC Mode real_row_addr:%d, u4RowAddr:%d\n",
				    real_row_addr, u4RowAddr);*/
#endif
			} else if (0xFF != devinfo.tlcControl.dis_slc_mode_cmd) {
				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);

				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
			}
		}
	}

	if (use_randomizer && u4RowAddr >= RAND_START_ADDR) {
		mtk_nand_turn_off_randomizer();
	} else if (pre_randomizer && u4RowAddr < RAND_START_ADDR) {
		mtk_nand_turn_off_randomizer();
	}
	return bRet;
}

static bool nand_exec_write_page_raw(struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8 * pPageBuf, u8 * pFDMBuf)
{
	bool bRet = true;
#if defined(MTK_TLC_NAND_SUPPORT)
	u8 *temp_page_buf = NULL;
	u8 *temp_fdm_buf = NULL;
	u32 u4SecNum = u4PageSize >> nand->sector_shift;
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	u32 page_size = u4PageSize;

	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		if ((devinfo.tlcControl.normaltlc) && (devinfo.tlcControl.pPlaneEn)) { //normal tlc && pplane enable
			if (devinfo.two_phyplane) {
				page_size /= 4;
				u4SecNum /= 4;
			} else {
				page_size /= 2;
				u4SecNum /= 2;
			}

			tlc_snd_phyplane = FALSE;
			tlc_lg_left_plane = TRUE; //program left plane
			temp_page_buf = pPageBuf;
			temp_fdm_buf = pFDMBuf;

			bRet = nand_exec_write_page_hw(nand, u4RowAddr, page_size, temp_page_buf, temp_fdm_buf); //u4PageSize must be mtd->writesize
			if (!bRet) { //operation fail
				return bRet;
			}

			tlc_lg_left_plane = FALSE; //program right plane
			temp_page_buf += page_size;
			temp_fdm_buf += (u4SecNum * nand->nand_fdm_size);

			bRet = nand_exec_write_page_hw(nand, u4RowAddr, page_size, temp_page_buf, temp_fdm_buf); //u4PageSize must be mtd->writesize

			if (devinfo.two_phyplane) {
				tlc_snd_phyplane = TRUE;
				tlc_lg_left_plane = TRUE; //program left plane
				temp_page_buf += page_size;
				temp_fdm_buf += (u4SecNum * nand->nand_fdm_size);

				bRet = nand_exec_write_page_hw(nand, u4RowAddr + page_per_block, page_size, temp_page_buf, temp_fdm_buf); //u4PageSize must be mtd->writesize
				if (!bRet) { //operation fail
					return bRet;
				}

				tlc_lg_left_plane = FALSE; //program right plane
				temp_page_buf += page_size;
				temp_fdm_buf += (u4SecNum * nand->nand_fdm_size);

				bRet = nand_exec_write_page_hw(nand, u4RowAddr + page_per_block, page_size, temp_page_buf, temp_fdm_buf); //u4PageSize must be mtd->writesize

				tlc_snd_phyplane = FALSE;
			}
		} else {
			if ((devinfo.tlcControl.normaltlc) && (devinfo.two_phyplane)) {
				page_size /= 2;
				u4SecNum /= 2;
			}

			tlc_snd_phyplane = FALSE;
			temp_page_buf = pPageBuf;
			temp_fdm_buf = pFDMBuf;
			bRet = nand_exec_write_page_hw(nand, u4RowAddr, page_size, temp_page_buf, temp_fdm_buf);

			if (!bRet) { //operation fail
				return bRet;
			}

			if ((devinfo.tlcControl.normaltlc) && (devinfo.two_phyplane)) {
				tlc_snd_phyplane = TRUE;
				temp_page_buf += page_size;
				temp_fdm_buf += (u4SecNum * nand->nand_fdm_size);
				bRet = nand_exec_write_page_hw(nand, u4RowAddr + page_per_block, page_size, temp_page_buf, temp_fdm_buf);
				tlc_snd_phyplane = FALSE;
			}
		}
	} else
#endif
	{
		if (devinfo.two_phyplane) {
			page_size /= 2;
			u4SecNum /= 2;
		}

		tlc_snd_phyplane = FALSE;
		temp_page_buf = pPageBuf;
		temp_fdm_buf = pFDMBuf;

		bRet = nand_exec_write_page_hw(nand, u4RowAddr, page_size, temp_page_buf, temp_fdm_buf); //u4PageSize must be mtd->writesize
		if (!bRet) { //operation fail
			return bRet;
		}

		if (devinfo.two_phyplane) {
			tlc_snd_phyplane = TRUE;
			temp_page_buf += page_size;
			temp_fdm_buf += (u4SecNum * 8);

			bRet = nand_exec_write_page_hw(nand, u4RowAddr + page_per_block, page_size, temp_page_buf, temp_fdm_buf); //u4PageSize must be mtd->writesize
			if (!bRet) { //operation fail
				return bRet;
			}

			tlc_snd_phyplane = FALSE;
		}
	}
	return bRet;
}

static bool nand_read_oob_raw(struct nand_chip *chip, u32 page_addr, u32 length, u8 *buf)
{
	u32 sector = 0;
	u32 col_addr = 0;
	u32 spare_per_sec = devinfo.sparesize >> (chip->page_shift - chip->sector_shift);

	if (length > 32 || length % OOB_AVAIL_PER_SECTOR || !buf) {
		printf("[%s] invalid parameter, length: %d, buf: %p\n", __func__, length, buf);
		return false;
	}

	while (length > 0) {
		col_addr = chip->sector_size + sector * (chip->sector_size + spare_per_sec);
		if (!nand_ready_for_read(chip, page_addr, col_addr, false, NULL))
			return false;
		if (!nand_mcu_read_data(buf, length))
			return false;
		NFI_CLN_REG32(NFI_CON_REG16, CON_NFI_BRD);
		sector++;
		length -= OOB_AVAIL_PER_SECTOR;
	}

	return true;
}

#if defined(MTK_TLC_NAND_SUPPORT)
bool mtk_nand_slc_write_wodata(u32 page)
{
	bool bRet = FALSE; //FALSE --> Pass  TRUE-->Fail
	bool slc_en;
	u32 real_row_addr;
	u32 reg_val;
	NFI_TLC_WL_INFO tlc_wl_info;

#if CFG_2CS_NAND
	if (g_bTricky_CS) {
		page = mtk_nand_cs_on(NFI_TRICKY_CS, page);
	}
#endif

	NFI_TLC_GetMappedWL(page, &tlc_wl_info);
	real_row_addr = NFI_TLC_GetRowAddr(tlc_wl_info.word_line_idx);

	//set 0xA2 cmd
	reg_val = DRV_Reg(NFI_CNFG_REG16);
	reg_val &= ~CNFG_READ_EN;
	reg_val &= ~CNFG_OP_MODE_MASK;
	reg_val |= CNFG_OP_CUST;
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	nand_set_command(0xA2);

	reg_val = DRV_Reg32(NFI_CON_REG16);
	reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
	/* issue reset operation */
	DRV_WriteReg32(NFI_CON_REG16, reg_val);

	//set 0x80 cmd
	nand_set_mode(CNFG_OP_PRGM);
	nand_set_command(NAND_CMD_SEQIN);

	//set address
	nand_set_address(0, real_row_addr, 2, 3);

	//set 0x10 cmd
	nand_set_command(NAND_CMD_PAGE_PROG);

	//read status
	slc_en = devinfo.tlcControl.slcopmodeEn;
	devinfo.tlcControl.slcopmodeEn = TRUE;//get i/O 2
	bRet = !mtk_nand_read_status();
	devinfo.tlcControl.slcopmodeEn = slc_en;

	printf("%s check page:%d bRet:%d\n", __func__, page, bRet);
	return bRet;
}
#endif

bool nand_block_bad_hw(struct nand_chip *nand, u64 offset)
{
	u32 page_per_block = BLOCK_SIZE / nand->page_size;
	u32 page_addr;
	u32 block;
	u32 mapped_block;
	bool bRet = FALSE;

	/* mapped_block = get_mapping_block_index(block); */
	page_addr = mtk_nand_page_transform(offset, &block, &mapped_block);

	memset(oob_buf_temp, 0, LSPARE);

	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
		page_addr &= ~(page_per_block - 1);

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)
	        && (devinfo.two_phyplane)) {
		page_addr = (mapped_block << 1) * page_per_block + (page_addr % page_per_block);
	} else
#endif
		if (devinfo.two_phyplane) {
			page_addr = (mapped_block << 1) * page_per_block + (page_addr % page_per_block);
		}

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.vendor == VEND_SANDISK)
	        && (mtk_nand_IsBMTPOOL(offset))) {
		bRet = mtk_nand_slc_write_wodata(page_addr);

		if ((devinfo.two_phyplane) && (!bRet)) {
			bRet = mtk_nand_slc_write_wodata(page_addr + page_per_block);
		}

		return bRet;
	}
#endif

	if (FALSE == nand_exec_read_page_hw(nand, page_addr, nand->page_size, data_buf_temp, oob_buf_temp)) {
		printf("nand_exec_read_page_hw return fail\n");
	}

	if (oob_buf_temp[0] != 0xff) {
		printf("Bad block detect at block 0x%x, oob_buf[0] is %x\n", page_addr / page_per_block, oob_buf_temp[0]);
		return true;
	}

	return false;
}

static bool nand_block_bad(struct nand_chip *nand, u32 page_addr)
{
	/* u32 page_per_block = 1 << (nand->phys_erase_shift - nand->page_shift); */
	/* int block = page_addr / page_per_block; */
	/* int mapped_block = get_mapping_block_index(block); */

	return nand_block_bad_hw(nand, (((u64) page_addr) << nand->page_shift));
}

/* not support un-block-aligned write */
static int nand_part_write(part_dev_t *dev, uchar * src, u64 dst, u32 size, unsigned int part_id)
{
	_dprintf("%s\n", __func__);
	struct nand_chip *nand = (struct nand_chip *)(dev->blkdev->priv);
	u8 res = 0;
	u32 u4PageSize = 1 << nand->page_shift;
	u32 u4PageNumPerBlock = BLOCK_SIZE / nand->page_size / 2;
	u32 u4BlkEnd = (u32) (nand->chipsize / BLOCK_SIZE);
	u32 u4BlkAddr = (u32) (dst / BLOCK_SIZE);
	u32 u4ColAddr = dst & (u4PageSize - 1);
	u32 u4RowAddr = dst / nand->page_size;
	u32 u4EraseAddr;
	u32 u4RowEnd;
	u32 u4WriteLen = 0;
	u32 i4Len;
	bool ret;
	/* u32 mapped; */
	u32 k = 0;
	/* mtk_nand_page_transform((u64)dst,&u4BlkAddr,&mapped); */
	for (k = 0; k < sizeof(g_kCMD.au1OOB); k++)
		*(g_kCMD.au1OOB + k) = 0xFF;
	MSG(ERR, "dst 0x%llx\n", dst);

#if defined(MTK_TLC_NAND_SUPPORT)
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		u4PageNumPerBlock = BLOCK_SIZE/nand->page_size/3;
#endif
	while (((u32) size > u4WriteLen) && (u4BlkAddr < u4BlkEnd)) {
#if 1
		if (!u4ColAddr) {
			MSG(ERR, "Erase the block of 0x%08x\n", u4BlkAddr);
#if 1
			__nand_erase((u64)u4BlkAddr * BLOCK_SIZE);
#else
#if CFG_2CS_NAND
			if (g_bTricky_CS) {
				u4RowAddr = mtk_nand_cs_on(NFI_TRICKY_CS, u4RowAddr);
				printf("nand_part_write Erase check CE here\n");
			}
#endif
			u4EraseAddr = u4BlkAddr * u4PageNumPerBlock * 2;
			nand_reset();
			nand_set_mode(CNFG_OP_ERASE);
			nand_set_command(NAND_CMD_ERASE_1);
			nand_set_address(0, u4EraseAddr, 0, 3);
			nand_set_command(NAND_CMD_ERASE_2);
			while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);
			mtk_nand_read_status();
#endif
		}
#else
		if (__nand_erase(dst) == FALSE) {
			MSG(ERR, "erase fail");
			mark_block_bad((u64) dst);
		}
#endif
		/* res = nand_block_bad(nand, ((u4BlkAddr >> 1) * u4PageNumPerBlock)); */

		if (!res) {
			if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
				u4RowEnd = ((u4RowAddr + u4PageNumPerBlock) / u4PageNumPerBlock) * u4PageNumPerBlock;
			else
				u4RowEnd = (u4RowAddr + u4PageNumPerBlock) & (~u4PageNumPerBlock + 1);

			for (; u4RowAddr < u4RowEnd; u4RowAddr++) {
				i4Len = min(size - u4WriteLen, u4PageSize - u4ColAddr);
				if (0 >= i4Len) {
					break;
				}
				if ((u4ColAddr == 0) && (i4Len == u4PageSize)) {
					memcpy(data_buf_temp, (src + u4WriteLen), u4PageSize);
					nand_exec_write_page(nand, u4RowAddr, u4PageSize, data_buf_temp, g_kCMD.au1OOB);
				} else {
					nand_exec_read_page(nand, u4RowAddr, u4PageSize, nand->buffers->databuf, g_kCMD.au1OOB);
					memcpy(nand->buffers->databuf + u4ColAddr, src + u4WriteLen, i4Len);
					nand_exec_write_page(nand, u4RowAddr, u4PageSize, nand->buffers->databuf, g_kCMD.au1OOB);
				}
				u4WriteLen += i4Len;
				u4ColAddr = (u4ColAddr + i4Len) & (u4PageSize - 1);
			}
		} else {
			printf("Detect bad block at block 0x%x\n", u4BlkAddr);
			u4RowAddr += u4PageNumPerBlock;
		}
		u4BlkAddr++;
	}

	return (int)u4WriteLen;

}

static int nand_part_read(part_dev_t *dev, u64 source, uchar *dst, u32 size, unsigned int part_id)
{
	struct nand_chip *nand = (struct nand_chip *)(dev->blkdev->priv);
	uint8_t res = 0;
	u32 u4PageSize = 1 << nand->page_shift;
	u32 u4PageNumPerBlock = BLOCK_SIZE / nand->page_size;
	u32 u4BlkEnd = (u32) (nand->chipsize / BLOCK_SIZE);
	u32 u4BlkAddr = (u32) (source / BLOCK_SIZE);
	u32 u4ColAddr = (u32) (source & (u4PageSize - 1));
	u32 u4RowAddr = (u32) (source / nand->page_size);
	u32 u4RowEnd;
	/* u32 mapped; */
	u32 u4ReadLen = 0;
	u32 i4Len;
	/* mtk_nand_page_transform((u64)source,&u4BlkAddr,&mapped); */
#if defined(MTK_TLC_NAND_SUPPORT)
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		u4PageNumPerBlock = BLOCK_SIZE/nand->page_size/3;
#endif
	while ((size > u4ReadLen) && (u4BlkAddr < u4BlkEnd)) {
		if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
			res = nand_block_bad(nand, (u4BlkAddr * u4PageNumPerBlock));

		if (!res) {
			if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
				u4RowEnd = ((u4RowAddr + u4PageNumPerBlock) / u4PageNumPerBlock) * u4PageNumPerBlock;
			else
				u4RowEnd = (u4RowAddr + u4PageNumPerBlock) & (~u4PageNumPerBlock + 1);
			for (; u4RowAddr < u4RowEnd; u4RowAddr++) {
				i4Len = min(size - u4ReadLen, u4PageSize - u4ColAddr);
				if (0 >= i4Len) {
					break;
				}
				if ((u4ColAddr == 0) && (i4Len == u4PageSize)) {
					nand_exec_read_page(nand, u4RowAddr, u4PageSize, dst + u4ReadLen, g_kCMD.au1OOB);
				} else {
					nand_exec_read_page(nand, u4RowAddr, u4PageSize, nand->buffers->databuf, g_kCMD.au1OOB);
					memcpy(dst + u4ReadLen, nand->buffers->databuf + u4ColAddr, i4Len);
				}
				u4ReadLen += i4Len;
				u4ColAddr = (u4ColAddr + i4Len) & (u4PageSize - 1);
			}
		} else {
			printf("Detect bad block at block 0x%x\n", u4BlkAddr);
			u4RowAddr += u4PageNumPerBlock;
		}
		u4BlkAddr++;
	}
	return (int)u4ReadLen;
}

static void nand_command_bp(struct nand_chip *nand_chip, unsigned command, int column, int page_addr)
{
	struct nand_chip *nand = nand_chip;
	u32 timeout;
	switch (command) {
		case NAND_CMD_SEQIN:
			if (g_kCMD.u4RowAddr != (u32) page_addr) {
				memset(g_kCMD.au1OOB, 0xFF, sizeof(g_kCMD.au1OOB));
				g_kCMD.pDataBuf = NULL;
			}
			g_kCMD.u4RowAddr = page_addr;
			g_kCMD.u4ColAddr = column;
			break;
		case NAND_CMD_PAGE_PROG:
			if (g_kCMD.pDataBuf || (0xFF != g_kCMD.au1OOB[0])) {
				u8 *pDataBuf = g_kCMD.pDataBuf ? g_kCMD.pDataBuf : nand->buffers->databuf;
				nand_exec_write_page(nand, g_kCMD.u4RowAddr, nand->writesize, pDataBuf, g_kCMD.au1OOB);
				g_kCMD.u4RowAddr = (u32) -1;
				g_kCMD.u4OOBRowAddr = (u32) -1;
			}
			break;

		case NAND_CMD_READ_OOB:
			g_kCMD.u4RowAddr = page_addr;
			g_kCMD.u4ColAddr = column + nand->writesize;
			g_i4ErrNum = 0;
			break;

		case NAND_CMD_READ_0:
			g_kCMD.u4RowAddr = page_addr;
			g_kCMD.u4ColAddr = column;
			g_i4ErrNum = 0;
			break;

		case NAND_CMD_ERASE_1:
			nand_reset();
			nand_set_mode(CNFG_OP_ERASE);
			nand_set_command(NAND_CMD_ERASE_1);
			nand_set_address(0, page_addr, 0, devinfo.addr_cycle - 2);
			break;

		case NAND_CMD_ERASE_2:
			nand_set_command(NAND_CMD_ERASE_2);
			while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);
			break;

		case NAND_CMD_STATUS:
			NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
			nand_reset();
			nand_set_mode(CNFG_OP_SRD);
			nand_set_command(NAND_CMD_STATUS);
			NFI_CLN_REG32(NFI_CON_REG16, CON_NFI_NOB_MASK);
			DRV_WriteReg32(NFI_CON_REG16, CON_NFI_SRD | (1 << CON_NFI_NOB_SHIFT));
			break;

		case NAND_CMD_RESET:
			nand_reset();
			break;
		case NAND_CMD_READ_ID:
			NFI_ISSUE_COMMAND(NAND_CMD_RESET, 0, 0, 0, 0);
			timeout = TIMEOUT_4;
			while (timeout) {
				timeout--;
			}
			nand_reset();
			/* Disable HW ECC */
			NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
			NFI_CLN_REG32(NFI_PAGEFMT_REG32, PAGEFMT_DBYTE_EN);
			NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN | CNFG_BYTE_RW);
			nand_reset();
			nand_set_mode(CNFG_OP_SRD);
			nand_set_command(NAND_CMD_READ_ID);
			nand_set_address(0, 0, 1, 0);
			DRV_WriteReg32(NFI_CON_REG16, CON_NFI_SRD);
			while (DRV_Reg32(NFI_STA_REG32) & STA_DATAR_STATE);
			break;
		default:
			printf("[ERR] nand_command_bp : unknow command %d\n", command);
			break;
	}
}

static u8 nand_read_byte(void)
{
	/* Check the PIO bit is ready or not */
	unsigned int timeout = TIMEOUT_4;
	WAIT_NFI_PIO_READY(timeout);
	return DRV_Reg8(NFI_DATAR_REG32);
}

#if 0
static void nand_read_buf(struct nand_chip *nand, u_char *buf, int len)
{
	struct nand_chip *nand = nand;
	struct CMD *pkCMD = &g_kCMD;
	u32 u4ColAddr = pkCMD->u4ColAddr;
	u32 u4PageSize = nand->writesize;

	if (u4ColAddr < u4PageSize) {
		if ((u4ColAddr == 0) && (len >= u4PageSize)) {
			nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize, buf, pkCMD->au1OOB);
			if (len > u4PageSize) {
				u32 u4Size = min(len - u4PageSize,
				                 sizeof(pkCMD->au1OOB));
				memcpy(buf + u4PageSize, pkCMD->au1OOB, u4Size);
			}
		} else {
			nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize, nand->buffers->databuf, pkCMD->au1OOB);
			memcpy(buf, nand->buffers->databuf + u4ColAddr, len);
		}
		pkCMD->u4OOBRowAddr = pkCMD->u4RowAddr;
	} else {
		u32 u4Offset = u4ColAddr - u4PageSize;
		u32 u4Size = min(len - u4PageSize - u4Offset, sizeof(pkCMD->au1OOB));
		if (pkCMD->u4OOBRowAddr != pkCMD->u4RowAddr) {
			nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize, nand->buffers->databuf, pkCMD->au1OOB);
			pkCMD->u4OOBRowAddr = pkCMD->u4RowAddr;
		}
		memcpy(buf, pkCMD->au1OOB + u4Offset, u4Size);
	}
	pkCMD->u4ColAddr += len;
}

static void nand_write_buf(struct nand_chip nand, const u_char *buf, int len)
{
	struct CMD *pkCMD = &g_kCMD;
	u32 u4ColAddr = pkCMD->u4ColAddr;
	u32 u4PageSize = nand->writesize;
	u32 i;

	if (u4ColAddr >= u4PageSize) {
		u8 *pOOB = pkCMD->au1OOB;
		u32 u4Size = min(len, sizeof(pkCMD->au1OOB));
		for (i = 0; i < u4Size; i++) {
			pOOB[i] &= buf[i];
		}
	} else {
		pkCMD->pDataBuf = (u8 *) buf;
	}
	pkCMD->u4ColAddr += len;
}
#endif
void lk_nand_irq_handler(unsigned int irq)
{
	u32 inte, sts;

	mt_irq_ack(irq);
	inte = DRV_Reg16(NFI_INTR_EN_REG16);
	sts = DRV_Reg16(NFI_INTR_REG16);
	/* MSG(INT, "[lk_nand_irq_handler]irq %x enable:%x %x\n",irq,inte,sts); */
	if (sts & inte) {
		/* printf("[lk_nand_irq_handler]send event,\n"); */
		DRV_WriteReg16(NFI_INTR_EN_REG16, 0);
		DRV_WriteReg16(NFI_INTR_REG16, sts);
		event_signal(&nand_int_event, 0);
	}
	return;
}

/*******************************************************************************
 * GPIO(PinMux) register definition
 *******************************************************************************/
//For NFI GPIO setting
#define NFI_GPIO_BASE 0x10005000
//NCLE
#define NFI_GPIO_MODE3              (NFI_GPIO_BASE + 0x320)
//NCEB1/NCEB0/NREB
#define NFI_GPIO_MODE4              (NFI_GPIO_BASE + 0x330)
//NRNB/NREB_C/NDQS_C
#define NFI_GPIO_MODE5              (NFI_GPIO_BASE + 0x340)
//NLD7/NLD6/NLD4/NLD3/NLD0
#define NFI_GPIO_MODE17             (NFI_GPIO_BASE + 0x460)
//NALE/NWEB/NLD1/NLD5/NLD8    //NLD8--->NDQS
#define NFI_GPIO_MODE18             (NFI_GPIO_BASE + 0x470)
//NLD2
#define NFI_GPIO_MODE19             (NFI_GPIO_BASE + 0x480)

//PD, NCEB0/NCEB1/NRNB
#define NFI_GPIO_PULLUP             (NFI_GPIO_BASE + 0xE60)

//Drving
#define NFI_GPIO_DRV_MODE0      (NFI_GPIO_BASE + 0xD00)
#define NFI_GPIO_DRV_MODE6      (NFI_GPIO_BASE + 0xD60)
#define NFI_GPIO_DRV_MODE7      (NFI_GPIO_BASE + 0xD70)

//TDSEL, no need

//RDSEL, no need for 1.8V
#define NFI_GPIO_RDSEL1_EN      (NFI_GPIO_BASE + 0xC10)
#define NFI_GPIO_RDSEL6_EN      (NFI_GPIO_BASE + 0xC60)
#define NFI_GPIO_RDSEL7_EN      (NFI_GPIO_BASE + 0xC70)

void nand_gpio_cfg_bit32(u32 addr, u32 field , u32 val)
{
	u32 tv = (unsigned int)(*(volatile u32*)(addr));
	tv &= ~(field);
	tv |= val;
	(*(volatile u32*)(addr) = (u32)(tv));
}
#define NFI_GPIO_CFG_BIT32(reg,field,val) nand_gpio_cfg_bit32(reg, field, val)

#define NFI_EFUSE_M_SW_RES  ((volatile u32 *)(0x10009000 + 0x0120))
#define NFI_EFUSE_IO_33V    0x00000100
#define NFI_EFUSE_Is_IO_33V()   (((*NFI_EFUSE_M_SW_RES)&NFI_EFUSE_IO_33V)?FALSE:TRUE) // 0 : 3.3v (MT8130 default), 1 : 1.8v

static void mtk_nand_gpio_init(void)
{
	//Enable Pinmux Function setting
	//NCLE
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE3, (0x7 << 12), (0x6 << 12));
	//NCEB1/NCEB0/NREB
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE4, (0x1FF << 0), (0x1B6 << 0));
	//NRNB/NREB_C/NDQS_C
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE5, (0x1FF << 3), (0x1B1 << 3));
	//NLD7/NLD6/NLD4/NLD3/NLD0
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE17, (0x7FFF << 0), (0x4924 << 0));
	//NALE/NWEB/NLD1/NLD5/NLD8, NLD8 for NDQS
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE18, (0x7FFF << 0), (0x4924 << 0));
	//NLD2
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE19, (0x7 << 0), (0x5 << 0));

	//PULL UP setting
	//PD, NCEB0, NCEB1, NRNB
	NFI_GPIO_CFG_BIT32(NFI_GPIO_PULLUP, (0xF0FF << 0), (0x1011 << 0));

	//Driving setting
	if (NFI_EFUSE_Is_IO_33V()) {
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE0, (0xF << 12), (0x2 << 12));  //need check
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE6, (0xFF << 8), (0x22 << 8));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE7, (0xFF << 0), (0x22 << 0));
	} else {
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE0, (0xF << 12), (0x4 << 12));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE6, (0xFF << 8), (0x44 << 8));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE7, (0xFF << 0), (0x44 << 0));
	}

	//TDSEL, No need

	//RDSEL, only need for 3.3V
	if (NFI_EFUSE_Is_IO_33V()) {
		NFI_GPIO_CFG_BIT32(NFI_GPIO_RDSEL1_EN, (0x3F << 6), (0xC << 6));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_RDSEL6_EN, (0xFFF << 0), (0x30C << 0));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_RDSEL7_EN, (0xFFF << 0), (0x30C << 0));
	}
}

#if CFG_2CS_NAND
/* #define CHIP_ADDRESS (0x100000) */
static int mtk_nand_cs_check(u8 *id, u16 cs)
{
	u8 ids[NAND_MAX_ID];
	int i = 0;
	/* if(devinfo.ttarget == TTYPE_2DIE) */
	/* { */
	/* MSG(INIT,"2 Die Flash\n"); */
	/* g_bTricky_CS = TRUE; */
	/* return 0; */
	/* } */
	DRV_WriteReg16(NFI_CSEL_REG16, cs);
	nand_command_bp(&g_nand_chip, NAND_CMD_READ_ID, 0, 0);
	for (i = 0; i < NAND_MAX_ID; i++) {
		ids[i] = nand_read_byte();
		if (ids[i] != id[i]) {
			MSG(INIT, "Nand cs[%d] not support(%d,%x)\n", cs, i, ids[i]);
			DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
			return 0;
		}
	}
	DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
	return 1;
}

static u32 mtk_nand_cs_on(u16 cs, u32 page)
{
	u32 cs_page = page / g_nanddie_pages;
	if (cs_page) {
		DRV_WriteReg16(NFI_CSEL_REG16, cs);
		/* if(devinfo.ttarget == TTYPE_2DIE) */
		/* return page;//return (page | CHIP_ADDRESS); */
		return (page - g_nanddie_pages);
	}
	DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
	return page;
}

#else

#define mtk_nand_cs_check(id, cs)  (1)
#define mtk_nand_cs_on(cs, page)   (page)
#endif

//#define TLC_2P_LK_UT
#ifdef TLC_2P_LK_UT
__attribute__((aligned(64))) static u8 temp_buffer_tlc[LPAGE + LSPARE];
__attribute__((aligned(64))) static u8 temp_buffer_tlc_rd[LPAGE + LSPARE];

int mtk_tlc_2p_unit_test(struct nand_chip *nand)
{
	printf("Begin to lk tlc 2p unit test ... \n");
	int err = 0;
	int patternbuff[128] = {
		0x0103D901, 0xFF1802DF, 0x01200400, 0x00000021, 0x02040122, 0x02010122, 0x03020407, 0x1A050103,
		0x00020F1B, 0x08C0C0A1, 0x01550800, 0x201B0AC1, 0x41990155, 0x64F0FFFF, 0x201B0C82, 0x4118EA61,
		0xF00107F6, 0x0301EE1B, 0x0C834118, 0xEA617001, 0x07760301, 0xEE151405, 0x00202020, 0x20202020,
		0x00202020, 0x2000302E, 0x3000FF14, 0x00FF0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x01D90301, 0xDF0218FF, 0x00042001, 0x21000000, 0x22010402, 0x22010102, 0x07040203, 0x0301051A,
		0x1B0F0200, 0xA1C0C008, 0x00085501, 0xC10A1B20, 0x55019941, 0xFFFFF064, 0x820C1B20, 0x61EA1841,
		0xF60701F0, 0x1BEE0103, 0x1841830C, 0x017061EA, 0x01037607, 0x051415EE, 0x20202000, 0x20202020,
		0x20202000, 0x2E300020, 0x14FF0030, 0x0000FF00, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
	};
	u32 j, k, p = (devinfo.blocksize << 10)/devinfo.pagesize, m;
	u32 test_page;
	u8* buf = (u8*) malloc(devinfo.blocksize << 11);
	u32 count;

	printf("[P] %d\n", p);

	for (m=0; m<64; m++)
		memcpy(temp_buffer_tlc+(512*m),(u8*)patternbuff, 512);

	memset(temp_buffer_tlc + 32768, 0xFF, LSPARE);
	memset(temp_buffer_tlc_rd + 32768, 0xFF, LSPARE);

#if 0
	//slc mode test: 2nd block of misc part
	printf("***************SLC MODE TEST***********************\n");
	test_page = 666*p;//84 * p + (p / 3);
	devinfo.tlcControl.slcopmodeEn = true;
	if (!nand_erase_hw((u64)test_page * devinfo.pagesize)) {
		printf("erase block failed\n");
	}
	for (k = 0; k < (p/3); k++) {
		printf("***************w p %d***********************\n",test_page + k);
		if (!nand_exec_write_page_raw(nand, test_page + k, nand->page_size, temp_buffer_tlc, (temp_buffer_tlc + 32768)))
			printf("Write page 0x%x fail!\n", test_page + k);

		printf("***************r p %d***********************\n",test_page + k);
		memset(temp_buffer_tlc_rd, 0x00, nand->page_size);
		if (!nand_exec_read_page_hw(nand, test_page + k, nand->page_size, temp_buffer_tlc_rd, (temp_buffer_tlc_rd + 32768)))
			printf("Read page 0x%x fail!\n", test_page + k);

		if (memcmp(temp_buffer_tlc, temp_buffer_tlc_rd, nand->page_size)) {
			printf("compare fail!\n");
			err = 1;
			break;
		} else {
			printf("compare OK!\n");
		}
	}
#endif

	//tlc mode test: block 888
	printf("***************TLC MODE TEST***********************\n");
	test_page = 666 * p;
	devinfo.tlcControl.slcopmodeEn = false;
	if (!nand_erase_hw((u64)test_page * devinfo.pagesize)) {
		printf("erase block failed\n");
	}

	memset(buf, 0x00, (devinfo.blocksize << 11));
	for (k = 0; k < p; k++) {
		memcpy(buf + (nand->page_size * k), temp_buffer_tlc, nand->page_size);
	}
	printf("***************w b %d***********************\n", test_page);
	mtk_nand_write_tlc_block(nand, buf, test_page/2);
	devinfo.tlcControl.slcopmodeEn = false;
	for (k = 0; k < p; k++) {
		printf("***************r p %d***********************\n",test_page + k);
		memset(temp_buffer_tlc_rd, 0x00, nand->page_size);
		if (!nand_exec_read_page_hw(nand, test_page + k, nand->page_size, temp_buffer_tlc_rd, (temp_buffer_tlc_rd + 32768)))
			printf("Read page 0x%x fail!\n", test_page + k);

		if (memcmp(temp_buffer_tlc, temp_buffer_tlc_rd, nand->page_size)) {
			printf("compare fail!\n");
			err = 2;
			break;
		} else {
			printf("compare OK!\n");
		}
	}

	free(buf);
	return err;
}
#endif

int nand_init_device(struct nand_chip *nand)
{
	int index;      /* j, busw,; */
	u8 id[NAND_MAX_ID];
	u32 spare_bit;
	u32 spare_per_sec;
	u32 ecc_bit;
#if CFG_COMBO_NAND
	int bmt_sz = 0;
#endif
	u32 pagesize = nand->oobblock;
	memset(&devinfo, 0, sizeof(devinfo));
	g_bInitDone = FALSE;
	g_kCMD.u4OOBRowAddr = (u32) -1;
	mtk_nand_gpio_init();
#ifdef MACH_FPGA        /* FPGA NAND is placed at CS1 */
	DRV_WriteReg16(NFI_CSEL_REG16, 0);
#else
	DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
#endif
	DRV_WriteReg32(NFI_ACCCON_REG32, NFI_DEFAULT_ACCESS_TIMING);
	DRV_WriteReg16(NFI_CNFG_REG16, 0);
	DRV_WriteReg32(NFI_PAGEFMT_REG32, 4);
	nand_reset();

	nand->nand_ecc_mode = NAND_ECC_HW;
	nand_command_bp(&g_nand_chip, NAND_CMD_READ_ID, 0, 0);
	MSG(INFO, "NAND ID: ");
	for (index = 0; index < NAND_MAX_ID; index++) {
		id[index] = nand_read_byte();
		MSG(INFO, " %x", id[index]);
	}
	MSG(INFO, "\n ");
	if (!get_device_info(id, &devinfo)) {
		MSG(ERR, "NAND unsupport\n");
		dump_nfi();
		return -1;
	}
#if CFG_2CS_NAND
	if (mtk_nand_cs_check(id, NFI_TRICKY_CS)) {
		MSG(INIT, "Twins Nand ");
		g_bTricky_CS = TRUE;
		g_b2Die_CS = TRUE;  /* only for 2die flash , temp solution */
	}
#endif
	nand->name = devinfo.devciename;
#if defined(MTK_TLC_NAND_SUPPORT)
	nand->chipsize = (u64)devinfo.totalsize << 10;
#else
	/* nand->chipsize = ((u64) devinfo.totalsize) << 20; */
#endif
#if CFG_2CS_NAND
	g_nanddie_pages = (u32) (nand->chipsize / devinfo.pagesize);
	if (devinfo.two_phyplane)
		g_nanddie_pages <<= 1;
	/* if(devinfo.ttarget == TTYPE_2DIE) */
	/* { */
	/* g_nanddie_pages = g_nanddie_pages / 2; */
	/* MSG(INFO, "[Bean]Nand pages 0x%x\n", g_nanddie_pages); */
	/* } */
	if (g_b2Die_CS) {
		nand->chipsize <<= 1;
		MSG(INFO, "%dMB\n", (u32) (nand->chipsize / 1024 / 1024));
	}
	/* MSG(INFO, "[Bean]g_nanddie_pages %x\n", g_nanddie_pages); */
#endif
	if (devinfo.sectorsize == 512)
		nand->erasesize = devinfo.blocksize << 10;
	else
		nand->erasesize = (devinfo.blocksize << 10) / 2;
#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)) {
		nand->erasesize = (devinfo.blocksize << 10)/3;
	}
#endif
	BLOCK_SIZE = devinfo.blocksize << 10;
	PAGES_PER_BLOCK = BLOCK_SIZE / devinfo.pagesize;
	nand->phys_erase_shift = uffs(nand->erasesize) - 1;
	nand->page_size = devinfo.pagesize;
	nand->writesize = devinfo.pagesize;
	nand->page_shift = uffs(nand->page_size) - 1;
	nand->oobblock = nand->page_size;
	nand->bus16 = devinfo.iowidth;
	nand->id_length = devinfo.id_length;
	nand->sector_size = NAND_SECTOR_SIZE;
	nand->sector_shift = 9;
	nand->nand_fdm_size = 8;
	if (devinfo.sectorsize == 1024) {
		nand->sector_size = 1024;
		nand->sector_shift = 10;
		NFI_CLN_REG32(NFI_PAGEFMT_REG32, PAGEFMT_SECTOR_SEL);
	}
	for (index = 0; index < devinfo.id_length; index++) {
		nand->id[index] = id[index];
	}
#if 1
	if (devinfo.vendor == VEND_MICRON) {
		if (devinfo.feature_set.FeatureSet.Async_timing.feature != 0xFF) {
			struct gFeatureSet *feature_set = &(devinfo.feature_set.FeatureSet);
			mtk_nand_SetFeature((u16) feature_set->sfeatureCmd,
			                    feature_set->Async_timing.address, (u8 *) &feature_set->Async_timing.feature,
			                    sizeof(feature_set->Async_timing.feature));
		}
#if CFG_2CS_NAND
		if (g_bTricky_CS) {
			DRV_WriteReg16(NFI_CSEL_REG16, NFI_TRICKY_CS);
			if (devinfo.feature_set.FeatureSet.Async_timing.feature != 0xFF) {
				struct gFeatureSet *feature_set = &(devinfo.feature_set.FeatureSet);
				mtk_nand_SetFeature((u16) feature_set->sfeatureCmd,
				                    feature_set->Async_timing.address, (u8 *) &feature_set->Async_timing.feature,
				                    sizeof(feature_set->Async_timing.feature));
			}
			DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
		}
#endif
	}
#endif
	/* MSG(INFO, "AHB Clock(Orig:0x%x ",DRV_Reg32(PERI_CON_BASE+0x5C)); */

	/* DRV_WriteReg32(PERI_CON_BASE+0x5C, 0x1); // 133MHZ */

	/* MSG(INFO, "Set:0x%x)\n",DRV_Reg32(PERI_CON_BASE+0x5C)); */
	DRV_WriteReg32(NFI_ACCCON_REG32, devinfo.timmingsetting);
	/* MSG(INFO, "[Nand]timmingsetting(0x%x)\n", devinfo.timmingsetting); */

	spare_per_sec = devinfo.sparesize >> (nand->page_shift - nand->sector_shift);
	/* spare_per_sec = devinfo.sparesize/(nand->page_size/nand->sector_size); */
	printf("[NAND]: NFI oobsize: %x, %x,%x\n", spare_per_sec, nand->page_shift, nand->sector_shift);

	switch (spare_per_sec) {
#ifndef MTK_COMBO_NAND_SUPPORT
		case 16:
			spare_bit = PAGEFMT_SPARE_16;
			ecc_bit = 4;
			spare_per_sec = 16;
			break;
		case 26:
		case 27:
		case 28:
			spare_bit = PAGEFMT_SPARE_26;
			ecc_bit = 10;
			spare_per_sec = 26;
			break;
		case 32:
			ecc_bit = 12;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_32_1KS;
			else
				spare_bit = PAGEFMT_SPARE_32;
			spare_per_sec = 32;
			break;
		case 40:
			ecc_bit = 18;
			spare_bit = PAGEFMT_SPARE_40;
			spare_per_sec = 40;
			break;
		case 44:
			ecc_bit = 20;
			spare_bit = PAGEFMT_SPARE_44;
			spare_per_sec = 44;
			break;
		case 48:
		case 49:
			ecc_bit = 22;
			spare_bit = PAGEFMT_SPARE_48;
			spare_per_sec = 48;
			break;
		case 50:
		case 51:
			ecc_bit = 24;
			spare_bit = PAGEFMT_SPARE_50;
			spare_per_sec = 50;
			break;
		case 52:
		case 54:
		case 56:
			ecc_bit = 24;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_52_1KS;
			else
				spare_bit = PAGEFMT_SPARE_52;
			spare_per_sec = 32;
			break;
#endif
		case 62:
		case 63:
			ecc_bit = 28;
			spare_bit = PAGEFMT_SPARE_62;
			spare_per_sec = 62;
			break;
		case 64:
			ecc_bit = 32;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_64_1KS;
			else
				spare_bit = PAGEFMT_SPARE_64;
			spare_per_sec = 64;
			break;
		case 72:
			ecc_bit = 36;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_72_1KS;
			spare_per_sec = 72;
			break;
		case 80:
			ecc_bit = 40;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_80_1KS;
			spare_per_sec = 80;
			break;
		case 88:
			ecc_bit = 44;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_88_1KS;
			spare_per_sec = 88;
			break;
		case 96:
		case 98:
			ecc_bit = 48;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_96_1KS;
			spare_per_sec = 96;
			break;
		case 100:
		case 102:
		case 104:
			ecc_bit = 52;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_100_1KS;
			spare_per_sec = 100;
			break;
		case 122:
		case 124:
		case 126:
#if defined(MTK_TLC_NAND_SUPPORT)
			if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			        && devinfo.tlcControl.ecc_recalculate_en) {
				if (60 < devinfo.tlcControl.ecc_required) { //68,72,80
					g_nand_chip.nand_fdm_size = 3;//122 - 119;// 119 = 68*14/8
					ecc_bit = 68;
				} else {
					ecc_bit = 60;
				}
			} else
#endif
				ecc_bit = 60;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_122_1KS;
			spare_per_sec = 122;
			break;
		case 128:
#if defined(MTK_TLC_NAND_SUPPORT)
			if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			        && devinfo.tlcControl.ecc_recalculate_en) {
				if (68 < devinfo.tlcControl.ecc_required) { //72,80
					g_nand_chip.nand_fdm_size = 2;//128 - 126;// 126 = 72*14/8
					ecc_bit = 72;
				} else {
					ecc_bit = 68;
				}
			} else
#endif
				ecc_bit = 68;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_128_1KS;
			spare_per_sec = 128;
			break;
#if defined(MTK_TLC_NAND_SUPPORT)
		case 134:
			ecc_bit = 72;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_134_1KS;
			spare_per_sec = 134;
			break;
		case 148:
			ecc_bit = 80;
			if (devinfo.sectorsize == 1024)
				spare_bit = PAGEFMT_SPARE_148_1KS;
			spare_per_sec = 148;
			break;
#endif
		default:
			printf("[NAND]: NFI not support oobsize: %x\n", spare_per_sec);
			while (1);
			return -1;
	}

	devinfo.sparesize = spare_per_sec << (nand->page_shift - nand->sector_shift);
	/* MSG(INFO, "[NAND]nand eccbit %d , sparesize %d\n",ecc_bit,devinfo.sparesize); */
	if (!devinfo.sparesize) {
		nand->oobsize = (8 << ((id[3] >> 2) & 0x01)) * (nand->oobblock / nand->sector_size);    /* FIX ME ,kai */
	} else {
		nand->oobsize = devinfo.sparesize;
	}
	nand->buffers = &nBuf;  /* malloc(sizeof(struct nand_buffers)); */
	if (nand->bus16 == IO_WIDTH_16) {
		NFI_SET_REG32(NFI_PAGEFMT_REG32, PAGEFMT_DBYTE_EN);
		nand->options |= NAND_BUSWIDTH_16;
	}

	pagesize = nand->oobblock;

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)
	        && (devinfo.two_phyplane)) {
		pagesize >>= 1;
	} else
#endif
		if (devinfo.two_phyplane) {
			pagesize >>= 1;
		}

	if (16384 == pagesize) {
		NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_16K_1KS);
		nand->ecclayout = &nand_oob_128;
	} else if (8192 == pagesize) {
		NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_8K_1KS);
		nand->ecclayout = &nand_oob_128;
	} else if (4096 == pagesize) {
		if (devinfo.sectorsize == 512)
			NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_4K);
		else
			NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_4K_1KS);
		nand->ecclayout = &nand_oob_128;
	} else if (2048 == pagesize) {
		if (devinfo.sectorsize == 512)
			NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_2K);
		else
			NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_2K_1KS);
		nand->ecclayout = &nand_oob_64;
	}

	if (nand->nand_ecc_mode == NAND_ECC_HW) {
		NFI_SET_REG32(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		ECC_Config(ecc_bit);
		nand_configure_fdm(g_nand_chip.nand_fdm_size);
	}
	DRV_Reg16(NFI_INTR_REG16);
	DRV_WriteReg16(NFI_INTR_EN_REG16, 0);
	if (en_interrupt) {
		event_init(&nand_int_event, false, EVENT_FLAG_AUTOUNSIGNAL);
		mt_irq_set_sens(MT_NFI_IRQ_ID, MT65xx_EDGE_SENSITIVE);
		mt_irq_set_polarity(MT_NFI_IRQ_ID, MT65xx_POLARITY_LOW);
		mt_irq_unmask(MT_NFI_IRQ_ID);
	}
#if CFG_RANDOMIZER
	if (devinfo.vendor != VEND_NONE) {
		/* mtk_nand_randomizer_config(&devinfo.feature_set.randConfig); */
#if 0
		if ((devinfo.feature_set.randConfig.type == RAND_TYPE_SAMSUNG) ||
		        (devinfo.feature_set.randConfig.type == RAND_TYPE_TOSHIBA)) {
			MSG(INFO, "[NAND]USE Randomizer\n");
			use_randomizer = TRUE;
		} else {
			MSG(INFO, "[NAND]OFF Randomizer\n");
			use_randomizer = FALSE;
		}
#endif
		if ((*EFUSE_RANDOM_CFG) & EFUSE_RANDOM_ENABLE) {
			MSG(INFO, "[NAND]EFUSE RANDOM CFG is ON\n");
			use_randomizer = TRUE;
			pre_randomizer = TRUE;
		} else {
			MSG(INFO, "[NAND]EFUSE RANDOM CFG is OFF\n");
			use_randomizer = FALSE;
			pre_randomizer = FALSE;
		}

#if defined(MTK_TLC_NAND_SUPPORT)
		if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
			MSG(INFO, "[NFI]tlc force RANDOM ON\n");
			use_randomizer = TRUE;
			pre_randomizer = TRUE;
		}
#endif
		/* DRV_WriteReg32(NFI_DLYCTRL_REG32,0x8001); */
		/* DRV_WriteReg32(NFI_DQS_DELAY_CTRL,0x000F0000); //temp */
		/* DRV_WriteReg32(NFI_DQS_DELAY_MUX,0x3); //temp */
	}
#endif

	if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
	        || (devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX)
	        || (devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE))
		HYNIX_RR_TABLE_READ(&devinfo);

	g_nand_size = nand->chipsize;
#if CFG_COMBO_NAND
	if (PART_SIZE_BMTPOOL) {
		bmt_sz = (PART_SIZE_BMTPOOL) / BLOCK_SIZE;
	} else {
#ifdef MNTL_SUPPORT
		bmt_sz = (int)(g_nand_chip.chipsize/BLOCK_SIZE/100);
#else
		bmt_sz = (int)(g_nand_chip.chipsize/BLOCK_SIZE/100*6);
#endif
	}
	/* if (id[0] == 0x45) */
	/* { */
	/* bmt_sz = bmt_sz * 2; */
	/* } */
#endif
#if defined(MTK_COMBO_NAND_SUPPORT)
#if CFG_COMBO_NAND
	nand->chipsize -= (bmt_sz * BLOCK_SIZE);
#else
	nand->chipsize -= (PART_SIZE_BMTPOOL);
#endif
	/* #if CFG_2CS_NAND */
	/* if(g_b2Die_CS) */
	/* { */
	/* nand->chipsize -= (PART_SIZE_BMTPOOL);  // if 2CS nand need cut down again */
	/* } */
	/* #endif */
#else
	nand->chipsize -= BLOCK_SIZE * (BMT_POOL_SIZE);
#endif
	/* nand->chipsize -= nand->erasesize * (PMT_POOL_SIZE); */

	g_bInitDone = true;
#if defined(MTK_TLC_NAND_SUPPORT)
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		mtk_pmt_reset();
#endif

#ifdef TLC_2P_LK_UT
	mtk_tlc_2p_unit_test(nand);
#endif

	if (!g_bmt) {
#if defined(MTK_COMBO_NAND_SUPPORT)
#if CFG_COMBO_NAND
		if (!(g_bmt = init_bmt(nand, bmt_sz)))
#else
		if (!(g_bmt = init_bmt(nand, (PART_SIZE_BMTPOOL) / BLOCK_SIZE)))
#endif
#else
		if (!(g_bmt = init_bmt(nand, BMT_POOL_SIZE)))
#endif
		{
			printf("Error: init bmt failed, chipsize=%llx,,%llx\n", nand->chipsize, total_size);
			return -1;
		}
	}
	return 0;

}

void nand_init(void)
{

	static part_dev_t dev;
	static block_dev_desc_t blkdev;
	if (!nand_init_device(&g_nand_chip)) {
		struct nand_chip *t_nand = &g_nand_chip;
		/* printf("NAND init done in LK\n"); */
		total_size = t_nand->chipsize - BLOCK_SIZE * (PMT_POOL_SIZE);
		dev.id = 0;
		dev.init = 1;
		dev.read = nand_part_read;
		dev.write = nand_part_write;
		/*  for blkdev here */
		dev.blkdev = &blkdev;
		dev.blkdev->blksz = 512;
		dev.blkdev->blk_bits = 9;
		dev.blkdev->priv = t_nand;
		mt_part_register_device(&dev);
		/* printf("NAND register done in LK\n"); */
		return;
	} else {
		printf("NAND init fail in LK\n");
	}

}

//#define TLC_LK_UT
#ifdef TLC_LK_UT
__attribute__((aligned(64))) static u8 temp_buffer_tlc[LPAGE + LSPARE];
__attribute__((aligned(64))) static u8 temp_buffer_tlc_rd[LPAGE + LSPARE];

int mtk_tlc_unit_test(struct nand_chip *nand)
{
	printf("Begin to lk tlc unit test ... \n");
	int err = 0;
	int patternbuff[128] = {
		0x0103D901, 0xFF1802DF, 0x01200400, 0x00000021, 0x02040122, 0x02010122, 0x03020407, 0x1A050103,
		0x00020F1B, 0x08C0C0A1, 0x01550800, 0x201B0AC1, 0x41990155, 0x64F0FFFF, 0x201B0C82, 0x4118EA61,
		0xF00107F6, 0x0301EE1B, 0x0C834118, 0xEA617001, 0x07760301, 0xEE151405, 0x00202020, 0x20202020,
		0x00202020, 0x2000302E, 0x3000FF14, 0x00FF0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x01D90301, 0xDF0218FF, 0x00042001, 0x21000000, 0x22010402, 0x22010102, 0x07040203, 0x0301051A,
		0x1B0F0200, 0xA1C0C008, 0x00085501, 0xC10A1B20, 0x55019941, 0xFFFFF064, 0x820C1B20, 0x61EA1841,
		0xF60701F0, 0x1BEE0103, 0x1841830C, 0x017061EA, 0x01037607, 0x051415EE, 0x20202000, 0x20202020,
		0x20202000, 0x2E300020, 0x14FF0030, 0x0000FF00, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
	};
	u32 j, k, p = (devinfo.blocksize << 10)/devinfo.pagesize, m;
	u32 test_page;
	u8* buf = (u8*) malloc(devinfo.blocksize << 10);
	u32 count;

	printf("[P] %d\n", p);

	for (m=0; m<32; m++)
		memcpy(temp_buffer_tlc+(512*m),(u8*)patternbuff, 512);

	memset(temp_buffer_tlc + 16384, 0xFF, LSPARE);
	memset(temp_buffer_tlc_rd + 16384, 0xFF, LSPARE);

	printf("***************read pl***********************\n");
	memset(temp_buffer_tlc_rd, 0xA5, 16384);
	if (!nand_exec_read_page(nand, 1 * p / 3, devinfo.pagesize, temp_buffer_tlc_rd, (temp_buffer_tlc_rd + 16384)))
		printf("Read page 0x%x fail!\n", 1 * p / 3);
	for (m=0; m<32; m++)
		printf("[5]0x%x %x %x %x\n", *((int *)temp_buffer_tlc_rd+m*4), *((int *)temp_buffer_tlc_rd+1+m*4), *((int *)temp_buffer_tlc_rd+2+m*4), *((int *)temp_buffer_tlc_rd+3+m*4));

	//slc mode test: 2nd block of misc part
	printf("***************SLC MODE TEST***********************\n");
	test_page = 84 * p + (p / 3);
	__nand_erase((u64)test_page * devinfo.pagesize);
	for (k = 0; k < (p/3); k++) {
		printf("***************w p %d***********************\n",test_page + k);
		if (!nand_exec_write_page(nand, test_page + k, devinfo.pagesize, temp_buffer_tlc, (temp_buffer_tlc + 16384)))
			printf("Write page 0x%x fail!\n", test_page + k);

		printf("***************r p %d***********************\n",test_page + k);
		memset(temp_buffer_tlc_rd, 0x00, devinfo.pagesize);
		if (!nand_exec_read_page(nand, test_page + k, devinfo.pagesize, temp_buffer_tlc_rd, (temp_buffer_tlc_rd + 16384)))
			printf("Read page 0x%x fail!\n", test_page + k);

		if (memcmp(temp_buffer_tlc, temp_buffer_tlc_rd, devinfo.pagesize)) {
			printf("compare fail!\n");
			err = 1;
			break;
		} else {
			printf("compare OK!\n");
		}
	}

	//tlc mode test: block 888
	printf("***************TLC MODE TEST***********************\n");
	test_page = 888 * p;
	__nand_erase((u64)test_page * devinfo.pagesize);

	memset(buf, 0x00, (devinfo.blocksize << 10));
	for (k = 0; k < p; k++) {
		memcpy(buf + (devinfo.pagesize * k), temp_buffer_tlc, devinfo.pagesize);
	}
	printf("***************w b %d***********************\n", test_page);
	mtk_nand_write_tlc_block(nand, buf, test_page);

	for (k = 0; k < p; k++) {
		printf("***************r p %d***********************\n",test_page + k);
		memset(temp_buffer_tlc_rd, 0x00, devinfo.pagesize);
		if (!nand_exec_read_page(nand, test_page + k, devinfo.pagesize, temp_buffer_tlc_rd, (temp_buffer_tlc_rd + 16384)))
			printf("Read page 0x%x fail!\n", test_page + k);

		if (memcmp(temp_buffer_tlc, temp_buffer_tlc_rd, devinfo.pagesize)) {
			printf("compare fail!\n");
			err = 2;
			break;
		} else {
			printf("compare OK!\n");
		}
	}

	//data retention test.
	//slc test read cycle: 100K
	//tlc test read cycle: 30K
#if 0
	test_page = 84 * p + (p / 3);
	for (count = 0; count < 100000; count++) {
		for (k = 0; k < (p/3); k++) {
			printf("***************[SLC READ]p %d cnt %d***********************\n",test_page + k, count);
			memset(temp_buffer_tlc_rd, 0x00, devinfo.pagesize);
			if (!nand_exec_read_page(nand, test_page + k, devinfo.pagesize, temp_buffer_tlc_rd, (temp_buffer_tlc_rd + 16384)))
				printf("Read page 0x%x fail!\n", test_page + k);

			if (memcmp(temp_buffer_tlc, temp_buffer_tlc_rd, devinfo.pagesize)) {
				printf("compare fail!\n");
				err = 1;
				break;
			} else {
				printf("compare OK!\n");
			}
		}
	}

	test_page = 888 * p;
	for (count = 0; count < 30000; count++) {
		for (k = 0; k < p; k++) {
			printf("***************[TLC READ]p %d cnt %d***********************\n",test_page + k, count);
			memset(temp_buffer_tlc_rd, 0x00, devinfo.pagesize);
			if (!nand_exec_read_page(nand, test_page + k, devinfo.pagesize, temp_buffer_tlc_rd, (temp_buffer_tlc_rd + 16384)))
				printf("Read page 0x%x fail!\n", test_page + k);

			if (memcmp(temp_buffer_tlc, temp_buffer_tlc_rd, devinfo.pagesize)) {
				printf("compare fail!\n");
				err = 2;
				break;
			} else {
				printf("compare OK!\n");
			}
		}
	}
#endif
	free(buf);
	return err;
}
#endif
void nand_driver_test(void)
{
#ifdef NAND_LK_TEST
	u32 test_len = 2048 * 1024;
	long len;
	int fail = 0;
	u32 index = 0;
	part_dev_t *dev = mt_part_get_device();
	part_t *part = mt_part_get_partition(PART_EXPDB);
	unsigned long start_addr = part->startblk * BLK_SIZE;
	u8 *original = malloc(test_len);
	u8 *source = malloc(test_len);
	u8 *readback = malloc(test_len);

	for (index = 0; index < test_len; index++) {
		source[index] = index % 16;
	}
	memset(original, 0x0a, test_len);
	memset(readback, 0x0b, test_len);
	MSG(ERR, "~~~~~~~~~nand driver test in lk~~~~~~~~~~~~~~\n");
	/* len = dev->read(dev, start_addr, (uchar *) original, test_len, 0); */
	/* if (len != test_len) */
	/* { */
	/* MSG(ERR,"read original fail %d\n", len); */
	/* } */
	/* MSG(ERR,"oringinal data:"); */
	/* for (index = 0; index < 300; index++) */
	/* { */
	/* MSG(ERR," %x", original[index]); */
	/* } */
	/* MSG(ERR,"\n"); */
	len = dev->write(dev, (uchar *) source, start_addr, test_len, 0);
	if (len != test_len) {
		MSG(ERR, "write source fail %d\n", len);
	}
	len = dev->read(dev, start_addr, (uchar *) readback, test_len, 0);
	if (len != test_len) {
		MSG(ERR, "read back fail %d\n", len);
	}
	MSG(ERR, "readback data:");
	for (index = 0; index < 300; index++) {
		MSG(ERR, " %x", readback[index]);
	}
	MSG(ERR, "\n");
	for (index = 0; index < test_len; index++) {
		if (source[index] != readback[index]) {
			MSG(ERR, "compare fail %d\n", index);
			fail = 1;
			break;
		}
	}
	if (fail == 0) {
		MSG(ERR, "compare success!\n");
	}
	len = dev->write(dev, (uchar *) original, start_addr, test_len, 0);
	if (len != test_len) {
		MSG(ERR, "write back fail %d\n", len);
	} else {
		MSG(ERR, "recovery success\n");
	}
	memset(original, 0xd, test_len);
	len = dev->read(dev, start_addr, (uchar *) original, test_len, 0);
	if (len != test_len) {
		MSG(ERR, "read original fail %d\n", len);
	}
	dprintf(INFO, "read back oringinal data:");
	for (index = 0; index < 300; index++) {
		MSG(ERR, " %x", original[index]);
	}
	MSG(ERR, "\n");
	MSG(ERR, "~~~~~~~~~nand driver test in lk~~~~~~~~~~~~~~\n");
	free(original);
	free(source);
	free(readback);
#endif
}

/******************** ***/
/*    support for fast boot    */
/***********************/
int nand_erase(u64 offset, u64 size)
{
	u64 img_size = (u64) size;
	/* u32 tpgsz; */
	u32 tblksz;
	u64 cur_offset;
	/* u32 i = 0; */
	u32 index;
	u32 block_size;

	/* do block alignment check */
	/* if ((u32)(offset % block_size) != 0) */
	/* { */
	/* printf("offset must be block alignment (0x%x)\n", block_size); */
	/* return -1; */
	/* } */
	/* /printf ("[ERASE] offset = 0x%x\n", (u32)offset); */
	part_get_startaddress(offset, &index);
	/* printf ("[ERASE] index = %d\n", index); */
	if (raw_partition(index)) {
		printf("[ERASE] raw TRUE\n");
		block_size = BLOCK_SIZE / 2;
#if defined(MTK_TLC_NAND_SUPPORT)
		if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			block_size = BLOCK_SIZE/3;
#endif
	} else {
		block_size = BLOCK_SIZE;
	}
	if ((u32) (offset % block_size) != 0) {
		printf("offset must be block alignment (0x%x)\n", block_size);
		return -1;
	}
	/* calculate block number of this image */
	if ((img_size % block_size) == 0) {
		tblksz = img_size / block_size;
	} else {
		tblksz = (img_size / block_size) + 1;
	}

	printf("[ERASE] image size = 0x%llx\n", img_size);
	printf("[ERASE] the number of nand block of this image = %d\n", tblksz);

	/* erase nand block */
	cur_offset = offset;
	while (tblksz != 0) {
		/* printf ("[ERASE] cur_offset = 0x%llx\n", cur_offset); */
		if (__nand_erase(cur_offset) == FALSE) {
			printf("[ERASE] erase 0x%x fail\n", cur_offset);
			mark_block_bad(cur_offset);

		}
		if ((!raw_partition(index))
		        &&  ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER)
		             ||(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC))
		        && (!mtk_block_istlc(cur_offset))) {
			if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER)
				cur_offset += block_size / 2;
			else if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
				cur_offset += block_size / 3;
			if ((cur_offset % block_size) == 0)
				tblksz--;
		} else {
			cur_offset += block_size;
			tblksz--;
		}

		if (tblksz != 0 && cur_offset >= total_size) {
			printf("[ERASE] cur offset (0x%x) exceeds erase limit address (0x%x)\n", cur_offset, total_size);
			return 0;
		}
	}

	return 0;

}

bool __nand_erase(u64 logical_addr)
{
	u32 block;
	u32 mapped_block;
	u64 addr;

	mtk_nand_page_transform(logical_addr, &block, &mapped_block);
	addr = (u64)(mapped_block);
	addr = addr * BLOCK_SIZE;
	if (!nand_erase_hw(addr)) {
		printf("erase block 0x%x failed\n", mapped_block);
		if (update_bmt((u64) mapped_block * BLOCK_SIZE, UPDATE_ERASE_FAIL, NULL, NULL)) {
			printf("erase block fail and update bmt sucess\n");
			return TRUE;
		} else {
			printf("erase block 0x%x failed but update bmt fail\n", mapped_block);
			return FALSE;
		}
	}

	return TRUE;
}

static int erase_fail_test = 0;
bool nand_erase_hw(u64 offset)
{
	bool bRet = TRUE;
	/* u32 timeout, u4SecNum = g_nand_chip.oobblock >> g_nand_chip.sector_shift; */
	u32 rownob = devinfo.addr_cycle - 2;
	u32 page_addr = (u32) (offset / g_nand_chip.oobblock);
#if defined(MTK_TLC_NAND_SUPPORT)
	NFI_TLC_WL_INFO  tlc_wl_info;
	NFI_TLC_WL_INFO  snd_tlc_wl_info;
#endif
	u32 reg_val = 0;
	u32 snd_real_row_addr = 0;
	u32   real_row_addr = 0;
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
#if MLC_MICRON_SLC_MODE
	u8 feature[4];
#endif
	if ((devinfo.NAND_FLASH_TYPE != NAND_FLASH_MLC_HYBER)
	        && (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)) {
		if (nand_block_bad_hw(&g_nand_chip, offset)) {
			return FALSE;
		}
	}
	if (erase_fail_test) {
		erase_fail_test = 0;
		return FALSE;
	}
#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)
	        && (devinfo.two_phyplane)) {
		page_addr = (((page_addr / page_per_block) << 1) * page_per_block) + (page_addr % page_per_block);
	} else
#endif
		if (devinfo.two_phyplane) {
			page_addr = (((page_addr / page_per_block) << 1) * page_per_block) + (page_addr % page_per_block);
		}
	printf("nand_erase_hw page_addr: %d\n", page_addr);
#if CFG_2CS_NAND
	if (g_bTricky_CS)
		page_addr = mtk_nand_cs_on(NFI_TRICKY_CS, page_addr);
#endif
#if defined(MTK_TLC_NAND_SUPPORT)
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		if (devinfo.tlcControl.normaltlc) { //normal tlc
			NFI_TLC_GetMappedWL(page_addr, &tlc_wl_info);
			real_row_addr = NFI_TLC_GetRowAddr(tlc_wl_info.word_line_idx);

			if (devinfo.two_phyplane) {
				NFI_TLC_GetMappedWL((page_addr + page_per_block), &snd_tlc_wl_info);
				snd_real_row_addr = NFI_TLC_GetRowAddr(snd_tlc_wl_info.word_line_idx);
			}
		} else {
			real_row_addr = NFI_TLC_GetRowAddr(page_addr);
		}
	} else
#endif
	{
		real_row_addr = page_addr;
		if (devinfo.two_phyplane) {
			snd_real_row_addr = page_addr + page_per_block;
		}
	}
	nand_reset();

#if defined(MTK_TLC_NAND_SUPPORT)
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		if ((devinfo.tlcControl.slcopmodeEn)
		        && (0xFF != devinfo.tlcControl.en_slc_mode_cmd)) {
			reg_val = DRV_Reg(NFI_CNFG_REG16);
			reg_val &= ~CNFG_READ_EN;
			reg_val &= ~CNFG_OP_MODE_MASK;
			reg_val |= CNFG_OP_CUST;
			DRV_WriteReg(NFI_CNFG_REG16, reg_val);

			nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);

			reg_val = DRV_Reg32(NFI_CON_REG16);
			reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
			/* issue reset operation */
			DRV_WriteReg32(NFI_CON_REG16, reg_val);
		} else {
			if (tlc_not_keep_erase_lvl) {
				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				nand_set_command(NOT_KEEP_ERASE_LVL_A19NM_CMD);

				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);
			}
		}
	} else
#endif
	{
		if (devinfo.tlcControl.slcopmodeEn) { // slc mode
#if MLC_MICRON_SLC_MODE
			if (devinfo.vendor == VEND_MICRON) {
				feature[0] = 0x00;
				feature[1] = 0x01;
				feature[2] = 0x00;
				feature[3] = 0x00;
				mtk_nand_SetFeature((u16) devinfo.feature_set.FeatureSet.sfeatureCmd,
				                    0x91, (u8 *) &feature, 4);
			} else
#endif
				if (0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);
					nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);

					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);

				}
		}
	}

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && devinfo.tlcControl.normaltlc
	        && devinfo.two_phyplane) {
		nand_set_mode(CNFG_OP_CUST);
		nand_set_command(NAND_CMD_ERASE_1);
		nand_set_address(0, real_row_addr, 0, rownob);
		nand_set_command(NAND_CMD_ERASE_1);
		nand_set_address(0, snd_real_row_addr, 0, rownob);
		nand_set_command(NAND_CMD_ERASE_2);
	} else
#endif
	{
		if (devinfo.two_phyplane) {
			nand_set_mode(CNFG_OP_CUST);
			nand_set_command(NAND_CMD_ERASE_1);
			nand_set_address(0, real_row_addr, 0, rownob);
			nand_set_command(NAND_CMD_ERASE_1);
			nand_set_address(0, snd_real_row_addr, 0, rownob);
			nand_set_command(NAND_CMD_ERASE_2);
		} else {
			nand_set_mode(CNFG_OP_ERASE);
			nand_set_command(NAND_CMD_ERASE_1);
			nand_set_address(0, real_row_addr, 0, rownob);
			nand_set_command(NAND_CMD_ERASE_2);
		}
	}

	if (!nand_status_ready(STA_NAND_BUSY)) {
		bRet = FALSE;
	}
	bRet = mtk_nand_read_status();

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.slcopmodeEn)) { //hynix tlc need doule check
		if (0xFF != devinfo.tlcControl.dis_slc_mode_cmd) {
			reg_val = DRV_Reg32(NFI_CON_REG16);
			reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
			/* issue reset operation */
			DRV_WriteReg32(NFI_CON_REG16, reg_val);

			reg_val = DRV_Reg(NFI_CNFG_REG16);
			reg_val &= ~CNFG_READ_EN;
			reg_val &= ~CNFG_OP_MODE_MASK;
			reg_val |= CNFG_OP_CUST;
			DRV_WriteReg(NFI_CNFG_REG16, reg_val);

			nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
		}
	} else
#endif
	{
		if (devinfo.tlcControl.slcopmodeEn) {
#if MLC_MICRON_SLC_MODE
			if (devinfo.vendor == VEND_MICRON) {
				feature[0] = 0x02;
				feature[1] = 0x01;
				feature[2] = 0x00;
				feature[3] = 0x00;
				mtk_nand_SetFeature((u16) devinfo.feature_set.FeatureSet.sfeatureCmd,
				                    0x91, (u8 *) &feature, 4);
			} else
#endif
				if (0xFF != devinfo.tlcControl.dis_slc_mode_cmd) {
					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);

					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);
					nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
				}
		}
	}

	return bRet;
}

bool mark_block_bad_hw(u64 offset)
{
	u32 index;
	/* unsigned char buf[4096]; */
	unsigned char *buf = g_data_buf;
	/* unsigned char spare_buf[64]; */
	unsigned char *spare_buf = g_spare_buf;
	u32 page_addr = (u32) (offset / g_nand_chip.oobblock);
	u32 u4SecNum = g_nand_chip.oobblock >> g_nand_chip.sector_shift;
	u32 i, page_num = (BLOCK_SIZE / g_nand_chip.oobblock);
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;

	memset(buf, 0x00, LPAGE);

	for (index = 0; index < 64; index++)
		*(spare_buf + index) = 0xFF;

	for (index = 8, i = 0; i < u4SecNum; i++)
		spare_buf[i * index] = 0x0;

	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
		page_addr &= ~(page_num - 1);

	MSG(INIT, "Mark bad block at 0x%x\n", page_addr);
	while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)
	        && (devinfo.two_phyplane)) {
		page_addr = (((page_addr / page_per_block) << 1) * page_per_block) + (page_addr % page_per_block);
	} else
#endif
		if (devinfo.two_phyplane) {
			page_addr = (((page_addr / page_per_block) << 1) * page_per_block) + (page_addr % page_per_block);
		}

	return nand_exec_write_page_raw(&g_nand_chip, page_addr, g_nand_chip.oobblock, (u8 *) buf, (u8 *) spare_buf);
}

bool mark_block_bad(u64 logical_addr)
{
	/* u32 block; */
	/* u32 mapped_block; */
	/* mtk_nand_page_transform(logical_addr,&block,&mapped_block); */
	return mark_block_bad_hw((u64) logical_addr);
}

int nand_write_page_hw(u32 page, u8 *dat, u8 *oob)
{
	/* u32 pagesz = g_nand_chip.oobblock; */
	/* u32 u4SecNum = pagesz >> g_nand_chip.sector_shift; */

	int i, j, start, len;
	bool empty = TRUE;
	u8 oob_checksum = 0;
	for (i = 0; i < MTD_MAX_OOBFREE_ENTRIES && g_nand_chip.ecclayout->oobfree[i].length; i++) {
		/* Set the reserved bytes to 0xff */
		start = g_nand_chip.ecclayout->oobfree[i].offset;
		len = g_nand_chip.ecclayout->oobfree[i].length;
		for (j = 0; j < len; j++) {
			oob_checksum ^= oob[start + j];
			if (oob[start + j] != 0xFF)
				empty = FALSE;
		}
	}

	if (!empty) {
		oob[g_nand_chip.ecclayout->oobfree[i - 1].offset + g_nand_chip.ecclayout->oobfree[i - 1].length] = oob_checksum;
	}

	while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);
	return nand_exec_write_page_raw(&g_nand_chip, page, g_nand_chip.oobblock, (u8 *) dat, (u8 *) oob);

}

int nand_write_page_hwecc(u64 logical_addr, char *buf, char *oob_buf)
{
	/* u32 page_size = g_nand_chip.oobblock; */
	/* u32 block_size = BLOCK_SIZE; */
	u32 block;
	u32 mapped_block;
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	u32 page_no;
	/* u32 page_in_block = (logical_addr/page_size)%pages_per_blk; */
	u32 i;
	int start, len, offset;
	page_no = mtk_nand_page_transform(logical_addr, &block, &mapped_block);
	for (i = 0; i < sizeof(g_spare_buf); i++)
		*(g_spare_buf + i) = 0xFF;

	offset = 0;

	if (oob_buf != NULL) {
		for (i = 0; i < MTD_MAX_OOBFREE_ENTRIES && g_nand_chip.ecclayout->oobfree[i].length; i++) {
			/* Set the reserved bytes to 0xff */
			start = g_nand_chip.ecclayout->oobfree[i].offset;
			len = g_nand_chip.ecclayout->oobfree[i].length;
			memcpy((g_spare_buf + start), (oob_buf + offset), len);
			offset += len;
		}
	}
	/* write bad index into oob */
	if (mapped_block != block) {
		/* MSG(INIT, "page: 0x%x\n", page_in_block); */
		set_bad_index_to_oob(g_spare_buf, block);
	} else {
		set_bad_index_to_oob(g_spare_buf, FAKE_INDEX);
	}

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)
	        && (devinfo.two_phyplane)) {
		page_no = ((mapped_block << 1) * page_per_block) + (page_no % page_per_block);
	} else
#endif
		if (devinfo.two_phyplane) {
			page_no = ((mapped_block << 1) * page_per_block) + (page_no % page_per_block);
		}

	if (!nand_write_page_hw(page_no, (u8 *) buf, g_spare_buf)) {
#if defined(MTK_TLC_NAND_SUPPORT)
		if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		        && (devinfo.tlcControl.normaltlc)
		        && (devinfo.two_phyplane)) {
			page_no = (mapped_block * page_per_block) + (page_no % page_per_block);
		} else
#endif
			if (devinfo.two_phyplane) {
				page_no = (mapped_block * page_per_block) + (page_no % page_per_block);
			}
		MSG(INIT, "write fail happened @ block 0x%x, page 0x%x\n", mapped_block, page_no);
		return update_bmt((u64) page_no * g_nand_chip.oobblock, UPDATE_WRITE_FAIL, (u8 *) buf, g_spare_buf);
	}

	return TRUE;
}

#if defined(MTK_TLC_NAND_SUPPORT)
bool mtk_nand_write_tlc_wl(struct nand_chip *chip,
                           uint8_t *buf, u32 wl, NFI_TLC_PG_CYCLE program_cycle)
{
	int page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	u32 block;
	u32 page_in_block;
	u32 mapped_block;
	u32 page;
	uint8_t *temp_buf = NULL;

	devinfo.tlcControl.slcopmodeEn = FALSE;//tlc mode program

	tlc_program_cycle = program_cycle;
	page = wl * 3;

	//buf may be virtual address
	temp_buf = buf;
	memcpy(local_tlc_wl_buffer, temp_buf, devinfo.pagesize);
	if (!(nand_exec_write_page_raw(chip, page, devinfo.pagesize, local_tlc_wl_buffer, g_spare_buf))) {
		printf("write fail at wl: 0x%x, page: 0x%x\n", wl, page);

		return FALSE;
	}

	temp_buf += devinfo.pagesize;
	memcpy(local_tlc_wl_buffer, temp_buf, devinfo.pagesize);
	if (!(nand_exec_write_page_raw(chip, page + 1, devinfo.pagesize, local_tlc_wl_buffer, g_spare_buf))) {
		printf("write fail at wl: 0x%x, page: 0x%x\n", wl, page);

		return FALSE;
	}

	temp_buf += devinfo.pagesize;
	memcpy(local_tlc_wl_buffer, temp_buf, devinfo.pagesize);
	if (!(nand_exec_write_page_raw(chip, page + 2, devinfo.pagesize, local_tlc_wl_buffer, g_spare_buf))) {
		printf("write fail at wl: 0x%x, page: 0x%x\n", wl, page);

		return FALSE;
	}
	return TRUE;
}

bool mtk_nand_write_tlc_block_hw(struct nand_chip *chip,
                                 uint8_t *buf, u32 mapped_block)
{
	int page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	u32 index;
	bool bRet = TRUE;
	u32 base_wl_index;
	u8 *temp_buf = NULL;

	base_wl_index = mapped_block * page_per_block / 3;

	tlc_cache_program = TRUE;

	for (index = 0; index < (page_per_block / 3); index++) {
		if (index == 0) {
			temp_buf = buf + (index * 3 * devinfo.pagesize);
			bRet = mtk_nand_write_tlc_wl(chip, temp_buf, base_wl_index + index, PROGRAM_1ST_CYCLE);
			if (!bRet)
				break;

			temp_buf = buf + ((index + 1) * 3 * devinfo.pagesize);
			bRet = mtk_nand_write_tlc_wl(chip, temp_buf, base_wl_index + index + 1, PROGRAM_1ST_CYCLE);
			if (!bRet)
				break;

			temp_buf = buf + (index * 3 * devinfo.pagesize);
			bRet = mtk_nand_write_tlc_wl(chip, temp_buf, base_wl_index + index, PROGRAM_2ND_CYCLE);
			if (!bRet)
				break;
		}

		if ((index + 2) < (page_per_block / 3)) {
			temp_buf = buf + ((index + 2) * 3 * devinfo.pagesize);
			bRet = mtk_nand_write_tlc_wl(chip, temp_buf, base_wl_index + index + 2, PROGRAM_1ST_CYCLE);
			if (!bRet)
				break;
		}

		if ((index + 1) < (page_per_block / 3)) {
			temp_buf = buf + ((index + 1) * 3 * devinfo.pagesize);
			bRet = mtk_nand_write_tlc_wl(chip, temp_buf, base_wl_index + index + 1, PROGRAM_2ND_CYCLE);
			if (!bRet)
				break;
		}

		if (index == ((page_per_block / 3)-1))
			tlc_cache_program = FALSE;

		temp_buf = buf + (index * 3 * devinfo.pagesize);
		bRet = mtk_nand_write_tlc_wl(chip, temp_buf, base_wl_index + index, PROGRAM_3RD_CYCLE);
		if (!bRet)
			break;

	}
	//printk("[xiaolei] mtk_nand_write_page 0x%x\n", (u32)buf);
	tlc_cache_program = FALSE;
	return bRet;
}


bool mtk_nand_write_tlc_block(struct nand_chip *chip,
                              uint8_t *buf,u32 page)
{
	int page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	u32 block;
	u32 page_in_block;
	u32 mapped_block;
	u32 index;
	bool bRet = TRUE;
	u32 base_wl_index;
	u8 *temp_buf = NULL;

	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC) {
		printf("error : not tlc nand\n");
		return FALSE;
	}
	if (!devinfo.tlcControl.normaltlc) {
		printf("error : not normal tlc nand\n");
		return FALSE;
	}

	page_in_block = mtk_nand_page_transform((u64)page * devinfo.pagesize,&block,&mapped_block);

	if ((page_in_block % page_per_block) != 0) {
		printf("error : normal tlc block program is not block aligned\n");
		return FALSE;
	}

	//MSG(INIT,"[WRITE] %d, %d, %d %d\n",mapped_block, block, page_in_block, page_per_block);
	memset(g_spare_buf, 0xff, sizeof(g_spare_buf));
	// write bad index into oob
	if (mapped_block != block) {
		set_bad_index_to_oob(g_spare_buf, block);
	} else {
		set_bad_index_to_oob(g_spare_buf, FAKE_INDEX);
	}

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc)
	        && (devinfo.two_phyplane)) {
		mapped_block <<= 1;
	}
#endif

	bRet = mtk_nand_write_tlc_block_hw(chip, buf, mapped_block);

	//printk("[xiaolei] mtk_nand_write_page 0x%x\n", (u32)buf);
	if (!bRet) {
		MSG(INIT, "write fail at block: 0x%x, page: 0x%x\n", mapped_block, page_in_block);
		if (update_bmt((u64)((u64)page_in_block) << chip->page_shift, UPDATE_WRITE_FAIL, (u8 *) buf, g_spare_buf)) {
			MSG(INIT, "Update BMT success\n");
			return TRUE;
		} else {
			MSG(INIT, "Update BMT fail\n");
			return FALSE;
		}
	}
	return TRUE;
}

#endif

int nand_get_alignment(void)
{
	return BLOCK_SIZE;
}

int nand_img_read(u64 source, uchar *dst, int size)
{

	uint8_t res;
	u32 u4PageSize = 1 << g_nand_chip.page_shift;
	u32 u4PageNumPerBlock = BLOCK_SIZE / g_nand_chip.page_size;
	u32 u4BlkEnd = (u32) (g_nand_chip.chipsize / BLOCK_SIZE);
	u32 u4BlkAddr = (u32) (source / BLOCK_SIZE);
	u32 u4ColAddr = (u32) (source & (u4PageSize - 1));
	u32 u4RowAddr = (u32) (source / g_nand_chip.page_size);
	u32 u4RowEnd;
	/* u32 mapped; */
	u32 u4ReadLen = 0;
	u32 i4Len;
	/* mtk_nand_page_transform((u64)source,&u4BlkAddr,&mapped); */
	while (((u32) size > u4ReadLen) && (u4BlkAddr < u4BlkEnd)) {
		res = nand_block_bad(&g_nand_chip, (u4BlkAddr * u4PageNumPerBlock));

		if (!res) {
			u4RowEnd = (u4RowAddr + u4PageNumPerBlock) & (~u4PageNumPerBlock + 1);
			for (; u4RowAddr < u4RowEnd; u4RowAddr++) {
				i4Len = min(size - u4ReadLen, u4PageSize - u4ColAddr);
				if (0 >= i4Len) {
					break;
				}
				if ((u4ColAddr == 0) && (i4Len == u4PageSize)) {
					nand_exec_read_page(&g_nand_chip, u4RowAddr, u4PageSize, dst + u4ReadLen, g_kCMD.au1OOB);
				} else {
					nand_exec_read_page(&g_nand_chip, u4RowAddr, u4PageSize, g_nand_chip.buffers->databuf, g_kCMD.au1OOB);
					memcpy(dst + u4ReadLen, g_nand_chip.buffers->databuf + u4ColAddr, i4Len);
				}
				u4ReadLen += i4Len;
				u4ColAddr = (u4ColAddr + i4Len) & (u4PageSize - 1);
			}
		} else {
			dprintf(INFO, "Detect bad block at block 0x%x\n", u4BlkAddr);
			u4RowAddr += u4PageNumPerBlock;
		}
		u4BlkAddr++;
	}
	return (int)u4ReadLen;
}

#if defined(MTK_MLC_NAND_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT)
int nand_write_img(u64 addr, void *data, u32 img_sz, u64 partition_size, int img_type)
#else
int nand_write_img(u32 addr, void *data, u32 img_sz, u32 partition_size, int img_type)
#endif
{
	unsigned int page_size = g_nand_chip.oobblock;
	unsigned int img_spare_size = 64;
	unsigned int write_size;
	unsigned int block_size = BLOCK_SIZE;
	u64 partition_start;
	u64 partition_end = (u64) addr + partition_size;
	bool ret = true;
	u32 index;
	bool bIsRaw = FALSE;
	unsigned int b_lastpage = 0;
	bool tlc_block = FALSE;
	printf("[nand_wite_img]write to img size, %x addr %llx img_type %d\n",img_sz,addr, img_type);

	partition_start = part_get_startaddress((u64)addr, &index);
	partition_end = partition_size + partition_start;

	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		tlc_block = mtk_block_istlc(addr);
		if (tlc_block) {
			block_size = BLOCK_SIZE;
		} else {
			block_size = BLOCK_SIZE/3;
		}
	} else {
		bIsRaw = raw_partition(index);
		if (!bIsRaw) {
			if (mtk_block_istlc(addr))
				block_size = BLOCK_SIZE;
			else
				block_size = BLOCK_SIZE/2;
		} else {
			block_size = BLOCK_SIZE/2;
		}
	}

	if (addr % block_size || partition_size % block_size) {
		printf("[nand_write_img]partition address or partition size is not block size alignment\n");
		return -1;
	}
	if (img_sz > partition_size) {
		printf("[nand_write_img]img size %x exceed partition size\n", img_sz);
		return -1;
	}
	if (page_size == 16384) {
		img_spare_size = 512;
	} else if (page_size == 8192) {
		img_spare_size = 256;
	} else if (page_size == 4096) {
		img_spare_size = 128;
	} else if (page_size == 2048) {
		img_spare_size = 64;
	}
	/* printf("[nand_wite_img]page_size:0x%lx, block_size:0x%x, partition_size:0x%llx, partition_end:0x%llx\n",\ */
	/* page_size,block_size, partition_size, partition_end); */
	if (tlc_block)
		write_size = block_size;
	else
		write_size = page_size;
	printf ("[nand_write_img] index = %d write_size 0x%x block_size 0x%x\n", index, write_size,block_size);

	while (img_sz > 0) {

		if ((addr + img_sz) > partition_end) {
			printf("[nand_wite_img]write to addr %llx,img size %x exceed parition size,may be so many bad blocks\n", addr, img_sz);
			return -1;
		}

		/*1. need to erase before write */
		if ((addr % block_size) == 0) {
			if (__nand_erase((u64) addr) == FALSE) {
				printf("[ERASE] erase 0x%llx fail\n", addr);
				mark_block_bad((u64) addr);
				addr += block_size;
				continue;   /* erase fail, skip this block */
			}
		}
		/*2. write page */
		if ((img_sz < write_size)) {
			if (!tlc_block) {
				b_lastpage = 1;
				memset(g_data_buf,0xff,write_size);
				memcpy(g_data_buf,data,img_sz);
				if ((img_type == UBIFS_IMG)&& (check_data_empty((void *)g_data_buf,page_size))) {
					printf("[nand_write_img]skip empty page\n");
					ret = true;
				} else {
					ret = nand_write_page_hwecc((u64)addr,(char*)g_data_buf,NULL);
				}
			}
		} else {
			if ((img_type == UBIFS_IMG)&& (check_data_empty((void *)data,page_size))) {
				printf("[nand_write_img]skip empty page\n");
				ret = true;
			} else {
				if (tlc_block)
					ret = mtk_nand_write_tlc_block(&g_nand_chip,data,addr>>g_nand_chip.page_shift);
				else
					ret = nand_write_page_hwecc((u64)addr,data,NULL);
			}
		}
		if (ret == FALSE) {
			printf("[nand_write_img]write fail at 0x%llx\n", addr);
			if (__nand_erase((u64) addr) == FALSE) {
				printf("[ERASE] erase 0x%llx fail\n", addr);
				mark_block_bad((u64) addr);
			}
			data -= ((addr % block_size) / page_size) * write_size;
			img_sz += ((addr % block_size) / page_size) * write_size;
			addr += block_size;
			continue;   /* write fail, try  to write the next block */
		}
		if (b_lastpage) {
			data += img_sz;
			img_sz = 0;
			addr += page_size;
		} else {
			data += write_size;
			img_sz -= write_size;
			addr += page_size;
		}
	}
	/*3. erase any block remained in partition */
	addr = ((addr + block_size - 1) / block_size) * block_size;
	/* printf("[nand_write_img]start erase the left size at 0x%llx\n",addr); */

	/* nand_erase((u64) addr, (u64) (partition_end - addr)); */
	/* printf("[nand_write_img]Done\n"); */

	return 0;
}

#if defined(MTK_MLC_NAND_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT)
int nand_write_img_ex(u64 addr, void *data, u32 length, u64 total_size, u32 *next_offset, u64 partition_start, u64 partition_size, int img_type)
#else
int nand_write_img_ex(u32 addr, void *data, u32 length, u32 total_size, u32 *next_offset, u32 partition_start, u32 partition_size, int img_type)
#endif
{
	unsigned int page_size = g_nand_chip.oobblock;
	unsigned int img_spare_size = 64;
	unsigned int write_size;
	unsigned int block_size = BLOCK_SIZE;
	u64 partition_end = partition_start + partition_size;
	/* unsigned int first_chunk = 0; */
	unsigned int last_chunk = 0;
	unsigned int left_size = 0;
	bool ret;
	bool tlc_block = FALSE;
	u32 index;
	u64 last_addr = (u64) addr;
	u32 dst_block = 0;
	part_get_startaddress((u64) addr, &index);
	printf("[nand_write_img_ex]write to addr %llx,img size %x, img_type %d\n", addr, length, img_type);
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		tlc_block = mtk_block_istlc(addr);
		if (tlc_block) {
			block_size = BLOCK_SIZE;
		} else {
			block_size = BLOCK_SIZE/3;
		}
	} else {
		if (raw_partition(index)) {
			block_size = BLOCK_SIZE / 2;
		} else {
			block_size = BLOCK_SIZE;
		}
	}
	if (partition_start % block_size || partition_size % block_size) {
		printf("[nand_write_img_ex]partition address or partition size is not block size alignment\n");
		return -1;
	}
	if (length > partition_size) {
		printf("[nand_write_img_ex]img size %x exceed partition size\n", length);
		return -1;
	}

	if (page_size == 16384) {
		img_spare_size = 512;
	} else if (page_size == 8192) {
		img_spare_size = 256;
	} else if (page_size == 4096) {
		img_spare_size = 128;
	} else if (page_size == 2048) {
		img_spare_size = 64;
	}

	if (last_addr % page_size) {
		printf("[nand_write_img_ex]write addr is not page_size %d alignment\n", page_size);
		return -1;
	}
	if (img_type == YFFS2_IMG) {
		write_size = page_size + img_spare_size;
		if (total_size % write_size) {
			printf("[nand_write_img_ex]total image size %lld is not w_size %d alignment\n", total_size, write_size);
			return -1;
		}
	}
	if (tlc_block)
		write_size = block_size;
	else
		write_size = page_size;

	if (addr == partition_start) {
		printf("[nand_write_img_ex]first chunk\n");
		/* first_chunk = 1; */
		download_size = 0;
		memset(g_data_buf, 0xff, write_size);
	}
	if ((length + download_size) >= total_size) {
		printf("[nand_write_img_ex]last chunk\n");
		last_chunk = 1;
	}

	left_size = (download_size % write_size);

	while (length > 0) {

		if ((addr + length) > partition_end) {
			printf("[nand_write_img_ex]write to addr %llx,img size %llx exceed parition size,may be so many bad blocks\n", addr, length);
			return -1;
		}

		/*1. need to erase before write */
		if ((addr % block_size) == 0) {
			if (__nand_erase((u64) addr) == FALSE) {
				printf("[ERASE] erase 0x%llx fail\n", addr);
				mark_block_bad((u64) addr);
				addr += block_size;
				continue;   /* erase fail, skip this block */
			}
		}
		if ((length < write_size) && (!left_size)) {
			memset(g_data_buf, 0xff, write_size);
			memcpy(g_data_buf, data, length);

			if (!last_chunk) {
				download_size += length;
				break;
			}
		} else if (left_size) {
			memcpy(&g_data_buf[left_size], data, write_size - left_size);

		} else {
			memcpy(g_data_buf, data, write_size);
		}

		/*2. write page */

		if (img_type == YFFS2_IMG) {
			ret = nand_write_page_hwecc((u64) addr, (char *)g_data_buf, (char *)g_data_buf + page_size);
		} else {
			if ((img_type == UBIFS_IMG) && (check_data_empty((void *)g_data_buf, page_size))) {
				printf("[nand_write_img_ex]skip empty page\n");
				ret = true;
			} else {
				if (tlc_block)
					ret = mtk_nand_write_tlc_block(&g_nand_chip,data,addr>>g_nand_chip.page_shift);
				else
					ret = nand_write_page_hwecc((u64)addr,data,NULL);
			}
		}
		/*need to check? */
		if (ret == FALSE) {
			printf("[nand_write_img_ex]write fail at 0x%llx\n", addr);
			while (1) {
				dst_block = find_next_good_block((u64) addr / block_size);
				if (dst_block == 0) {
					printf("[nand_write_img_ex]find next good block fail\n");
					return -1;
				}
				ret = block_replace((u64) addr / block_size, dst_block, (u64) addr / page_size);
				if (ret == FALSE) {
					printf("[nand_write_img_ex]block replace fail,continue\n");
					continue;
				} else {
					printf("[nand_write_img_ex]block replace sucess %x--> %x\n", (u32) (addr / block_size), dst_block);
					break;
				}

			}
			addr = (addr % block_size) + (dst_block * block_size);
			/*      if (__nand_erase(addr) == FALSE)
			   {
			   printf("[ERASE] erase 0x%x fail\n",addr);
			   mark_block_bad (addr);
			   }
			   data -= ((addr%block_size)/page_size)*write_size;
			   length += ((addr%block_size)/page_size)*write_size;
			   addr += block_size; */
			continue;   /* write fail, try  to write the next block */
		}
		if (left_size) {
			data += (write_size - left_size);
			length -= (write_size - left_size);
			addr += page_size;
			download_size += (write_size - left_size);
			left_size = 0;
		} else {
			data += write_size;
			length -= write_size;
			addr += page_size;
			download_size += write_size;
		}
	}
	*next_offset = addr - last_addr;
	if (last_chunk) {
		/*3. erase any block remained in partition */
		addr = ((addr + block_size - 1) / block_size) * block_size;

		nand_erase((u64) addr, (u64) (partition_end - addr));
	}
	return 0;
}

int check_data_empty(void *data, unsigned size)
{
	unsigned i;
	u32 *tp = (u32 *) data;

	for (i = 0; i < size / 4; i++) {
		if (*(tp + i) != 0xffffffff) {
			return 0;
		}
	}
	return 1;
}

static u32 find_next_good_block(u32 start_block)
{
	u32 i;
	u32 dst_block = 0;
	for (i = start_block; i < (total_size / BLOCK_SIZE); i++) {
		if (!nand_block_bad(&g_nand_chip, i * (BLOCK_SIZE / g_nand_chip.page_size))) {
			dst_block = i;
			break;
		}
	}
	return dst_block;
}

static bool block_replace(u32 src_block, u32 dst_block, u32 error_page)
{
	bool ret;
	u32 block_size = BLOCK_SIZE;
	u32 page_size = g_nand_chip.page_size;
	u32 i;
	u8 *data_buf;
	u8 *spare_buf;
	ret = __nand_erase((u64) dst_block * block_size);
	if (ret == FALSE) {
		printf("[block_replace]%x-->%x erase fail\n", src_block, dst_block);
		mark_block_bad((u64) src_block * block_size);
		return ret;
	}
	data_buf = (u8 *) malloc(LPAGE);
	spare_buf = (u8 *) malloc(LSPARE);
	if (!data_buf || !spare_buf) {
		printf("[block_replace]malloc mem fail\n");
		return -1;
	}

	memset(data_buf, 0xff, LPAGE);
	memset(spare_buf, 0xff, LSPARE);
	for (i = 0; i < error_page; i++) {
		nand_exec_read_page(&g_nand_chip, src_block * (block_size / page_size) + i, page_size, data_buf, spare_buf);
		ret = nand_write_page_hwecc((u64) dst_block * block_size + i * page_size, (char *)data_buf, (char *)spare_buf);
		if (ret == FALSE)
			mark_block_bad((u64) dst_block * block_size);
	}

	mark_block_bad((u64) src_block * block_size);
	free(data_buf);
	free(spare_buf);
	return ret;

}

/* Add for Get DL information */
#define PRE_SCAN_BLOCK_NUM 20
/*Support to check format/download status, 2013/01/19 {*/
/* Max Number of Load Sections */
#define MAX_LOAD_SECTIONS       40
#define DL_MAGIC "DOWNLOAD INFORMATION!!"
#define DL_INFO_VER_V1  "V1.0"

#define DL_MAGIC_NUM_COUNT 32
#define DL_MAGIC_OFFSET 24
#define DL_IMG_NAME_LENGTH 16
#define DL_CUSTOM_INFO_SIZE (128)

/*download status v1 and old version for emmc*/
#define FORMAT_START    "FORMAT_START"
#define FORMAT_DONE      "FORMAT_DONE"
#define BL_START             "BL_START"
#define BL_DONE              "BL_DONE"
#define DL_START            "DL_START"
#define DL_DONE             "DL_DONE"
#define DL_ERROR           "DL_ERROR"
#define DL_CK_DONE       "DL_CK_DONE"
#define DL_CK_ERROR      "DL_CK_ERROR"

/*v1 and old version for emmc*/
#define CHECKSUM_PASS "PASS"
#define CHECKSUM_FAIL "FAIL"

typedef enum {
	DL_INFO_VERSION_V0 = 0,
	DL_INFO_VERSION_V1 = 1,
	DL_INFO_VERSION_UNKOWN = 0xFF,
} DLInfoVersion;

/*version v1.0 {*/
typedef struct {
	char image_name[DL_IMG_NAME_LENGTH];
} IMG_DL_INFO;

typedef struct {
	unsigned int image_index;
	unsigned int pc_checksum;
	unsigned int da_checksum;
	char checksum_status[8];
} CHECKSUM_INFO_V1;

typedef struct {
	char magic_num[DL_MAGIC_OFFSET];
	char version[DL_MAGIC_NUM_COUNT - DL_MAGIC_OFFSET];
	CHECKSUM_INFO_V1 part_info[MAX_LOAD_SECTIONS];
	char ram_checksum[16];
	char download_status[16];
	IMG_DL_INFO img_dl_info[MAX_LOAD_SECTIONS];
} DL_STATUS_V1;
/*version v1.0 }*/
/*Support to check format/download status, 2013/01/19 {*/

#define DL_NOT_FOUND 2
#define DL_PASS 0
#define DL_FAIL 1

int nand_get_dl_info(void)
{
	DL_STATUS_V1 download_info;
	u8 *data_buf;
	u8 *spare_buf;
	int ret;
	u32 block_size = BLOCK_SIZE;
	u32 page_size = g_nand_chip.page_size;
	u32 pages_per_block = block_size / page_size;
	u32 total_blocks = (u32) (g_nand_size / BLOCK_SIZE);
	u32 i, block_i, page_i;
	u32 block_addr;
	u32 dl_info_blkAddr = 0xFFFFFFFF;
	u32 page_index[4];

	data_buf = (u8 *) malloc(LPAGE);
	spare_buf = (u8 *) malloc(LSPARE);
	if (!data_buf || !spare_buf) {
		printf("[nand_get_dl_info]malloc mem fail\n");
		ret = -1;
		return ret;
	}
	/* DL information block should program to good block instead of always at last block. */
	page_index[0] = 0;
	page_index[1] = 1;
	page_index[2] = pages_per_block - 3;
	page_index[3] = pages_per_block - 1;

	block_i = 1;
	do {
		block_addr = pages_per_block * (total_blocks - block_i);
		for (page_i = 0; page_i < 4; page_i++) {
			nand_exec_read_page(&g_nand_chip, block_addr + page_index[page_i], page_size, data_buf, spare_buf);
			ret = memcmp((void *)data_buf, DL_MAGIC, sizeof(DL_MAGIC));
			if (!ret) {
				dl_info_blkAddr = block_addr;
				break;
			}
		}
		if (dl_info_blkAddr != 0xFFFFFFFF) {
			break;
		}
		block_i++;
	} while (block_i <= PRE_SCAN_BLOCK_NUM);
	if (dl_info_blkAddr == 0xFFFFFFFF) {
		printf("DL INFO NOT FOUND\n");
		ret = DL_NOT_FOUND;
	} else {
		printf("get dl info from 0x%x\n", dl_info_blkAddr);

		memcpy(&download_info, data_buf, sizeof(download_info));
		if (!memcmp(download_info.download_status, DL_DONE, sizeof(DL_DONE)) || !memcmp(download_info.download_status, DL_CK_DONE, sizeof(DL_CK_DONE))) {
			printf("dl done. status = %s\n", download_info.download_status);
			printf("dram checksum : %s\n", download_info.ram_checksum);
			for (i = 0; i < PART_MAX_COUNT; i++) {
				if (download_info.part_info[i].image_index != 0) {
					printf("image_index:%d, checksum: %s\n", download_info.part_info[i].image_index,
					       download_info.part_info[i].checksum_status);
				}
			}
			ret = DL_PASS;
		} else {
			printf("dl error. status = %s\n", download_info.download_status);
			printf("dram checksum : %s\n", download_info.ram_checksum);
			for (i = 0; i < PART_MAX_COUNT; i++) {
				if (download_info.part_info[i].image_index != 0) {
					printf("image_index:%d, checksum: %s\n", download_info.part_info[i].image_index,
					       download_info.part_info[i].checksum_status);
				}
			}
			ret = DL_FAIL;
		}
	}
	free(data_buf);
	free(spare_buf);

	return ret;
}

u32 mtk_nand_erasesize(void)
{
	return g_nand_chip.erasesize;
}

#endif
