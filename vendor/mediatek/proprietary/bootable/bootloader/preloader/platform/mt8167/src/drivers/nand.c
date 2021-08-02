/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "typedefs.h"
#include "platform.h"
#include "blkdev.h"
#include "cust_nand.h"
#include "nand.h"
#include "nand_core.h"
#include "bmt.h"
#include "partition.h"
/* #include "partition_define.h" */
#include "dram_buffer.h"
#include "gpio.h"

#if defined(MTK_COMBO_NAND_SUPPORT)
	/* BMT_POOL_SIZE is not used anymore */
#else
	#ifndef PART_SIZE_BMTPOOL
	#define BMT_POOL_SIZE (80)
	#else
	#define BMT_POOL_SIZE (PART_SIZE_BMTPOOL)
	#endif
#endif
#define CFG_RANDOMIZER	(1) /* for randomizer code */
#define CFG_2CS_NAND	(1) /* for 2CS nand */
#define CFG_COMBO_NAND	(1) /* for Combo nand */

#define NFI_TRICKY_CS	(1)  /* must be 1 or > 1? */
#define PMT_POOL_SIZE	(2)
/******************************************************************************
*
* Macro definition
*
*******************************************************************************/

#define NFI_SET_REG32(reg, value)   (DRV_WriteReg32(reg, DRV_Reg32(reg) | (value)))
#define NFI_SET_REG16(reg, value)   (DRV_WriteReg16(reg, DRV_Reg16(reg) | (value)))
#define NFI_CLN_REG32(reg, value)   (DRV_WriteReg32(reg, DRV_Reg32(reg) & (~(value))))
#define NFI_CLN_REG16(reg, value)   (DRV_WriteReg16(reg, DRV_Reg16(reg) & (~(value))))
#define RAND_TYPE_SAMSUNG 0
#define RAND_TYPE_TOSHIBA 1
#define RAND_TYPE_NONE 2

#define FIFO_PIO_READY(x)  (0x1 & x)
#define WAIT_NFI_PIO_READY(timeout) \
	do {\
		while((!FIFO_PIO_READY(DRV_Reg(NFI_PIO_DIRDY_REG16))) && (--timeout));\
		if (timeout == 0){\
			MSG(ERR, "Error: FIFO_PIO_READY timeout at line=%d, file =%s\n",\
				__LINE__, __FILE__);\
		}\
	} while(0);

#define TIMEOUT_1   0x1fff
#define TIMEOUT_2   0x8ff
#define TIMEOUT_3   0xffff
#define TIMEOUT_4   5000        //PIO

#define STATUS_READY			(0x40)
#define STATUS_FAIL				(0x01)
#define STATUS_WR_ALLOW			(0x80)

#if defined(MTK_TLC_NAND_SUPPORT)
bool tlc_lg_left_plane = TRUE; /* logical left plane of tlc nand.  used to do page program */
NFI_TLC_PG_CYCLE tlc_program_cycle;
bool tlc_not_keep_erase_lvl = FALSE;/* not keep erase level */
u32 slc_ratio = 6; /* slc mode block ration in FS partition. means slc_ration % */
u32 sys_slc_ratio;
u32 usr_slc_ratio;
bool tlc_cache_program = FALSE; /* whether use cache program */
bool tlc_snd_phyplane = FALSE; /* second plane operation */
#endif

u32 PAGE_SIZE;
u32 BLOCK_SIZE;
static u32 PAGES_PER_BLOCK = 255;

#if CFG_2CS_NAND
static bool g_bTricky_CS = FALSE;
static bool g_b2Die_CS = FALSE;
static u32 g_nanddie_pages = 0;
#endif

enum flashdev_vendor gVendor;
extern u64 part_get_startaddress(u64 byte_address);
#define STORAGE_BUFFER_SIZE 32768
/* extern u8 storage_buffer[STORAGE_BUFFER_SIZE]; */
#define storage_buffer g_dram_buf->storage_buffer
/* u8 __DRAM__ nand_nfi_buf[NAND_NFI_BUFFER_SIZE]; */
#define nand_nfi_buf g_dram_buf->nand_nfi_buf
#define nand_bad_block_tbl_buf g_dram_buf->nand_bad_block_tbl_buf

#define MLC_MICRON_SLC_MODE	(0)

#define ERR_RTN_SUCCESS   1
#define ERR_RTN_FAIL      0
#define ERR_RTN_BCH_FAIL -1
u32 MICRON_TRANSFER(u32 pageNo);
u32 SANDISK_TRANSFER(u32 pageNo);
u32 HYNIX_TRANSFER(u32 pageNo);

typedef u32 (*GetLowPageNumber)(u32 pageNo);

GetLowPageNumber functArray[]=
{
	MICRON_TRANSFER,
	HYNIX_TRANSFER,
	SANDISK_TRANSFER,
};

u32 SANDISK_TRANSFER(u32 pageNo)
{
	if (0 == pageNo)
		return pageNo;
	else
		return pageNo+pageNo-1;
}

u32 MICRON_TRANSFER(u32 pageNo)
{
	u32 temp;

	if(pageNo < 4)
		return pageNo;
	temp = (pageNo - 4) & 0xFFFFFFFE;
	if(pageNo<=130)
		return (pageNo+temp);
	else
		return (pageNo+temp-2);
}

u32 HYNIX_TRANSFER(u32 pageNo)
{
	u32 temp;

	if(pageNo < 4)
		return pageNo;
	temp = pageNo+(pageNo&0xFFFFFFFE)-2;
	return temp;
}

/**************************************************************************
*  Randomizer
**************************************************************************/
#define SS_SEED_NUM 128
#define EFUSE_RANDOM_CFG	((volatile u32 *)(0x10009020))
#define EFUSE_RANDOM_ENABLE 0x00001000
static bool use_randomizer = FALSE;
static bool pre_randomizer = FALSE;

static U16 SS_RANDOM_SEED[SS_SEED_NUM] =
{
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


/**************************************************************************
*  MACRO LIKE FUNCTION
**************************************************************************/

static inline u32 PAGE_NUM(u64 logical_size)
{
	return ((unsigned long)(logical_size) / PAGE_SIZE);
}

inline u64 LOGICAL_ADDR(u32 page_addr)
{
	return ((unsigned long long)(page_addr) * PAGE_SIZE);
}

inline u64 BLOCK_ALIGN(u64 logical_addr)
{
	return (((u64) (logical_addr / BLOCK_SIZE)) * BLOCK_SIZE);
}

#ifndef REDUCE_NAND_PL_SIZE
typedef U32(*STORGE_READ) (u8 * buf, u32 start, u32 img_size);

typedef struct
{
	u32 page_size;
	u32 pktsz;
} device_info_t;

device_info_t gdevice_info;
boot_dev_t g_dev_vfunc;
#endif
static blkdev_t g_nand_bdev;
__attribute__((aligned(4))) unsigned char g_nand_spare[256];

unsigned int nand_maf_id;
unsigned int nand_dev_id;
uint8 ext_id1, ext_id2, ext_id3;

#ifndef REDUCE_NAND_PL_SIZE
static u32 g_i4ErrNum;
#endif
static BOOL g_bInitDone;
BOOL g_bHwEcc = TRUE;
u8 *Bad_Block_Table;

struct nand_chip g_nand_chip;
struct nand_ecclayout *nand_oob = NULL;

#ifndef REDUCE_NAND_PL_SIZE /* xiaolei not used */
static struct nand_ecclayout nand_oob_16 = {
	.eccbytes = 8,
	.eccpos = {8, 9, 10, 11, 12, 13, 14, 15},
	.oobfree = {{1, 6}, {0, 0}}
};

struct nand_ecclayout nand_oob_64 = {
	.eccbytes = 32,
	.eccpos = {32, 33, 34, 35, 36, 37, 38, 39,
	       40, 41, 42, 43, 44, 45, 46, 47,
	       48, 49, 50, 51, 52, 53, 54, 55,
	       56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 6}, {0, 0}}
};
#endif

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
	       120, 121, 122, 123, 124, 125, 126, 127},
	.oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 7}, {33, 7}, {41, 7}, {49, 7}, {57, 6}}
};
#ifndef REDUCE_NAND_PL_SIZE /* not used.marked by xiaolei */
struct NAND_CMD
{
	u32 u4ColAddr;
	u32 u4RowAddr;
	u32 u4OOBRowAddr;
	u8 au1OOB[64];
	u8 *pDataBuf;
};

static struct NAND_CMD g_kCMD;
#endif
flashdev_info devinfo;
static char *nfi_buf;
static bool mtk_nand_read_status(void);

bool get_device_info(u8*id, flashdev_info *devinfo);

#ifndef REDUCE_NAND_PL_SIZE
struct nand_manufacturers nand_manuf_ids[] = {
	{NAND_MANFR_TOSHIBA, "Toshiba"},
	{NAND_MANFR_SAMSUNG, "Samsung"},
	{NAND_MANFR_FUJITSU, "Fujitsu"},
	{NAND_MANFR_NATIONAL, "National"},
	{NAND_MANFR_RENESAS, "Renesas"},
	{NAND_MANFR_STMICRO, "ST Micro"},
	{NAND_MANFR_HYNIX, "Hynix"},
	{NAND_MANFR_MICRON, "Micron"},
	{NAND_MANFR_AMD, "AMD"},
	{0x0, "Unknown"}
};

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

#define NAND_SECTOR_SIZE 512

u32 mtk_nand_page_transform(u64 logical_address, u32* blk, u32* map_blk)
{
	u64 start_address;
	u32 block;
	u32 page_in_block;
	u32 mapped_block;

	if (VEND_NONE != gVendor) {
		start_address = part_get_startaddress(logical_address);
		if (start_address == 0xFFFFFFFF) {

			while(1);
		}
		block = (u32)(start_address/BLOCK_SIZE) + \
			(u32)((logical_address-start_address) / g_nand_chip.erasesize);
		page_in_block = PAGE_NUM(logical_address-start_address) %\
			(1 << (g_nand_chip.phys_erase_shift-g_nand_chip.page_shift));
		page_in_block = PAGE_NUM(logical_address-start_address) %\
			(g_nand_chip.erasesize / g_nand_chip.page_size);

	#if defined(MTK_TLC_NAND_SUPPORT)
		if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			&& devinfo.tlcControl.normaltlc) {
			page_in_block *= 3;
		} else
	#endif
		{
			if (devinfo.vendor != VEND_NONE) {
				/* page_in_block = devinfo.feature_set.PairPage[page_in_block]; */
				page_in_block = functArray[devinfo.feature_set.ptbl_idx](page_in_block);
			}
		}
	    mapped_block = get_mapping_block_index(block);
	} else {
		block = (u32)(logical_address/BLOCK_SIZE);
		mapped_block = get_mapping_block_index(block);
		page_in_block = (u32)(PAGE_NUM(logical_address) %\
			(BLOCK_SIZE >> g_nand_chip.page_shift));
	}
	*blk = block;
	*map_blk = mapped_block;
	return mapped_block*(BLOCK_SIZE/PAGE_SIZE)+page_in_block;
}


#if defined(MTK_TLC_NAND_SUPPORT)
void NFI_TLC_GetMappedWL(u32 pageidx, NFI_TLC_WL_INFO* WL_Info)
{
	/* this function is just for normal tlc */
	WL_Info->word_line_idx = pageidx / 3;
	WL_Info->wl_pre = (NFI_TLC_WL_PRE)(pageidx % 3);
}

u32 NFI_TLC_GetRowAddr(u32 rowaddr)
{
	u32 real_row;
	u32 temp = 0xFF;
	int page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;

	if(devinfo.tlcControl.normaltlc)
		temp = page_per_block / 3;
	else
		temp = page_per_block;

	real_row = ((rowaddr / temp) << devinfo.tlcControl.block_bit) | (rowaddr % temp);

    return real_row;
}

/* this function is just for normal tlc */
u32 NFI_TLC_SetpPlaneAddr(u32 rowaddr, bool left_plane)
{
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

/* rowaddr is the real address, the return value is the page */
/* increased by degree (pageidx = block no * page per block) */
u32 NFI_TLC_GetMappedPgAddr(u32 rowaddr)
{
	u32 page_idx;
	u32 page_shift = 0;
	u32 real_row;
	int page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;

	real_row = rowaddr;

	if (devinfo.tlcControl.normaltlc) {
		page_shift = devinfo.tlcControl.block_bit;
		if(devinfo.tlcControl.pPlaneEn)
		    real_row &= (~(1 << devinfo.tlcControl.pPlane_bit));
		/* always get wl's low page */
		page_idx = ((real_row >> page_shift) * page_per_block) + \
			(((real_row << (32-page_shift)) >> (32-page_shift)) * 3);
	} else {
		/* micron tlc */
		page_shift = devinfo.tlcControl.block_bit;
		page_idx = ((real_row >> page_shift) * page_per_block) + \
			((real_row << (32-page_shift)) >> (32-page_shift));
	}

	return page_idx;
}
#endif

#if CFG_RANDOMIZER
static int mtk_nand_turn_on_randomizer(u32 page, int type, int fgPage)
{
	u32 u4NFI_CFG = 0;
	u32 u4NFI_RAN_CFG = 0;
	/* to fit flash which page per block is not 32 aligned */
	u32 u4PgNum = page % PAGES_PER_BLOCK;

	u4NFI_CFG = DRV_Reg32(NFI_CNFG_REG16);
	/* empty threshold 40 */
	DRV_WriteReg32(NFI_ENMPTY_THRESH_REG32, 40);

#ifndef REDUCE_NAND_PL_SIZE
	if (type) {
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
#endif

	u4NFI_CFG |= CNFG_RAN_SEL;
	if (PAGES_PER_BLOCK <= SS_SEED_NUM) {
		if (type)
			u4NFI_RAN_CFG |= RAN_CNFG_ENCODE_SEED(SS_RANDOM_SEED[u4PgNum % PAGES_PER_BLOCK])\
		    		| RAN_CNFG_ENCODE_EN;
		else
			u4NFI_RAN_CFG |= RAN_CNFG_DECODE_SEED(SS_RANDOM_SEED[u4PgNum % PAGES_PER_BLOCK])\
				| RAN_CNFG_DECODE_EN;
	} else {
		if (type)

			u4NFI_RAN_CFG |= RAN_CNFG_ENCODE_SEED(SS_RANDOM_SEED[u4PgNum & (SS_SEED_NUM-1)])\
				| RAN_CNFG_ENCODE_EN;
		else
			u4NFI_RAN_CFG |= RAN_CNFG_DECODE_SEED(SS_RANDOM_SEED[u4PgNum & (SS_SEED_NUM-1)])\
				| RAN_CNFG_DECODE_EN;
	}


	if(fgPage) /* reload seed for each page */
		u4NFI_CFG &= ~CNFG_RAN_SEC;
	else /* reload seed for each sector */
		u4NFI_CFG |= CNFG_RAN_SEC;

	DRV_WriteReg32(NFI_CNFG_REG16, u4NFI_CFG);
	DRV_WriteReg32(NFI_RANDOM_CNFG_REG32, u4NFI_RAN_CFG);
	/* MSG(INIT, "[PL]ran turn on type:%d 0x%x 0x%x\n", type, DRV_Reg32(NFI_RANDOM_CNFG_REG32), page); */

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
	/* MSG(INIT, "[PL]ran turn off\n"); */
}
#else
#define mtk_nand_israndomizeron() (FALSE)
#define mtk_nand_turn_on_randomizer(page, type, fgPage)
#define mtk_nand_turn_off_randomizer()
#endif


/**************************************************************************
*  reset descriptor
**************************************************************************/
void mtk_nand_reset_descriptor(void)
{

	g_nand_chip.page_shift = 0;
	g_nand_chip.page_size = 0;
	g_nand_chip.ChipID = 0;     /* Type of DiskOnChip */
	g_nand_chip.chips_name = 0;
	g_nand_chip.chipsize = 0;
	g_nand_chip.erasesize = 0;
	g_nand_chip.mfr = 0;        /* Flash IDs - only one type of flash per device */
	g_nand_chip.id = 0;
	g_nand_chip.name = 0;
	g_nand_chip.numchips = 0;
	g_nand_chip.oobblock = 0;   /* Size of OOB blocks (e.g. 512) */
	g_nand_chip.oobsize = 0;    /* Amount of OOB data per block (e.g. 16) */
	g_nand_chip.eccsize = 0;
	g_nand_chip.bus16 = 0;
	g_nand_chip.nand_ecc_mode = 0;
	g_nand_chip.nand_fdm_size = 0;

}

bool get_device_info(u8*id, flashdev_info *devinfo)
{
	u32 i,m,n,mismatch;
	int target=-1,target_id_len=-1;
	unsigned int flash_number = sizeof(gen_FlashTable) / sizeof(gen_FlashTable[0]);

	for (i = 0; i<flash_number; i++) {
		mismatch=0;
		for (m=0; m<gen_FlashTable[i].id_length; m++) {
			if(id[m]!=gen_FlashTable[i].id[m]) {
				mismatch=1;
				break;
			}
		}
		if(mismatch == 0 && gen_FlashTable[i].id_length > target_id_len) {
			target=i;
			target_id_len=gen_FlashTable[i].id_length;
		}
	}

    	if(target != -1){
		MSG(INIT, "Recognize NAND: ID [");
		for(n=0;n<gen_FlashTable[target].id_length;n++){
			devinfo->id[n] = gen_FlashTable[target].id[n];
			MSG(INIT, "%x ",devinfo->id[n]);
		}
#if defined(MTK_TLC_NAND_SUPPORT)
		MSG(INIT, "], Device Name [%s], Page Size [%d]B Spare Size [%d]B Total Size [%d]KB\n",
			gen_FlashTable[target].devciename,gen_FlashTable[target].pagesize,
			gen_FlashTable[target].sparesize, gen_FlashTable[target].totalsize);
#else
		MSG(INIT, "], Device Name [%s], Page Size [%d]B Spare Size [%d]B Total Size [%d]MB\n",
			gen_FlashTable[target].devciename,gen_FlashTable[target].pagesize,
			gen_FlashTable[target].sparesize, gen_FlashTable[target].totalsize);
#endif
		devinfo->id_length=gen_FlashTable[target].id_length;
		devinfo->blocksize = gen_FlashTable[target].blocksize;
		devinfo->addr_cycle = gen_FlashTable[target].addr_cycle;
		devinfo->iowidth = gen_FlashTable[target].iowidth;
		devinfo->timmingsetting = gen_FlashTable[target].timmingsetting;
		devinfo->advancedmode = gen_FlashTable[target].advancedmode;
		devinfo->pagesize = gen_FlashTable[target].pagesize;
		devinfo->sparesize = gen_FlashTable[target].sparesize;
		devinfo->totalsize = gen_FlashTable[target].totalsize;
		devinfo->sectorsize = gen_FlashTable[target].sectorsize;
		devinfo->s_acccon= gen_FlashTable[target].s_acccon;
		devinfo->s_acccon1= gen_FlashTable[target].s_acccon1;
		devinfo->freq= gen_FlashTable[target].freq;
		devinfo->vendor = gen_FlashTable[target].vendor;
		//devinfo->ttarget = gen_FlashTable[target].ttarget;
		gVendor = gen_FlashTable[target].vendor;
		memcpy((u8*)&devinfo->feature_set, (u8*)&gen_FlashTable[target].feature_set,
			sizeof(struct MLC_feature_set));
		memcpy(devinfo->devciename, gen_FlashTable[target].devciename, sizeof(devinfo->devciename));
#if defined(MTK_TLC_NAND_SUPPORT)
		devinfo->NAND_FLASH_TYPE = gen_FlashTable[target].NAND_FLASH_TYPE;
		memcpy((u8*)&devinfo->tlcControl, (u8*)&gen_FlashTable[target].tlcControl, sizeof(NFI_TLC_CTRL));
		devinfo->two_phyplane = gen_FlashTable[target].two_phyplane;
#endif
    		return true;
	} else {
	    	MSG(INIT, "Not Found NAND: ID [");
		for(n=0;n<NAND_MAX_ID;n++) {
			MSG(INIT, "%x ",id[n]);
		}
		MSG(INIT, "]\n");
        	return false;
	}
}

static bool mtk_nand_check_RW_count(u16 u2WriteSize)
{
	u32 timeout = 0xFFFF;
	u16 u2SecNum = u2WriteSize >> g_nand_chip.sector_shift;

	while (ADDRCNTR_CNTR(DRV_Reg32(NFI_ADDRCNTR_REG16)) < u2SecNum) {
		timeout--;
		if (0 == timeout)
			return FALSE;
	}
	return TRUE;
}

static bool mtk_nand_status_ready(u32 u4Status)
{
	u32 timeout = 0xFFFF;

	while ((DRV_Reg32(NFI_STA_REG32) & u4Status) != 0) {
		timeout--;
		if (0 == timeout)
			return FALSE;
	}
	return TRUE;
}

static void mtk_nand_set_mode(u16 u2OpMode)
{
	u16 u2Mode = DRV_Reg16(NFI_CNFG_REG16);

	u2Mode &= ~CNFG_OP_MODE_MASK;
	u2Mode |= u2OpMode;
	DRV_WriteReg16(NFI_CNFG_REG16, u2Mode);
}

static bool mtk_nand_set_command(u16 command)
{
	/* Write command to device */
	DRV_WriteReg16(NFI_CMD_REG16, command);
	return mtk_nand_status_ready(STA_CMD_STATE);
}

static bool mtk_nand_set_address(u32 u4ColAddr, u32 u4RowAddr, u16 u2ColNOB, u16 u2RowNOB)
{
	/* fill cycle addr */
	DRV_WriteReg32(NFI_COLADDR_REG32, u4ColAddr);
	DRV_WriteReg32(NFI_ROWADDR_REG32, u4RowAddr);
	DRV_WriteReg16(NFI_ADDRNOB_REG16, u2ColNOB | (u2RowNOB << ADDR_ROW_NOB_SHIFT));
	return mtk_nand_status_ready(STA_ADDR_STATE);
}

static void ECC_Decode_Start(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE)) ;
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_EN);
}

static void ECC_Decode_End(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE)) ;
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
}

static void ECC_Encode_Start(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg32(ECC_ENCIDLE_REG32) & ENC_IDLE)) ;
	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_EN);
}

static void ECC_Encode_End(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg32(ECC_ENCIDLE_REG32) & ENC_IDLE)) ;
	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
}

static void ECC_Config(u32 ecc_bit)
{
	u32 u4ENCODESize;
	u32 u4DECODESize;

	u32 ecc_bit_cfg = ECC_CNFG_ECC4;

	u4DECODESize = ((g_nand_chip.sector_size + g_nand_chip.nand_fdm_size) << 3) \
		+ ecc_bit * ECC_PARITY_BIT;

	switch (ecc_bit)
	{
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
	do{
		;
	} while (!DRV_Reg16(ECC_DECIDLE_REG16));

	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
	do{
		;
	} while (!DRV_Reg32(ECC_ENCIDLE_REG32));

	/* setup FDM register base */
	/* DRV_WriteReg32(ECC_FDMADDR_REG32, NFI_FDM0L_REG32); */

	u4ENCODESize = (g_nand_chip.sector_size + g_nand_chip.nand_fdm_size) << 3;

	/* configure ECC decoder && encoder */
	DRV_WriteReg32(ECC_DECCNFG_REG32, \
		ecc_bit_cfg | DEC_CNFG_NFI | DEC_CNFG_EMPTY_EN | (u4DECODESize << DEC_CNFG_CODE_SHIFT));

	DRV_WriteReg32(ECC_ENCCNFG_REG32, \
		ecc_bit_cfg | ENC_CNFG_NFI | (u4ENCODESize << ENC_CNFG_MSG_SHIFT));

	#ifndef MANUAL_CORRECT
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_CORRECT);
	#else
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_EL);
	#endif

}

/******************************************************************************
* mtk_nand_check_bch_error
*
* DESCRIPTION:
*   Check BCH error or not !
*
* PARAMETERS:
*   struct mtd_info *mtd
*    u8* pDataBuf
*    u32 u4SecIndex
*    u32 u4PageAddr
*
* RETURNS:
*   None
*
* NOTES:
*   None
*
******************************************************************************/
static bool mtk_nand_check_bch_error(u8 * pDataBuf, u32 u4SecIndex, u32 u4PageAddr)
{
	bool bRet = TRUE;
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
			MSG(INIT, "[xl] timeout 0x%x 0x%x\n",
				u2SectorDoneMask, DRV_Reg16(ECC_DECDONE_REG16));
			return FALSE;
		}
	}
#ifndef MANUAL_CORRECT
	if(0 == (DRV_Reg32(NFI_STA_REG32) & STA_READ_EMPTY)) {
		u4ErrorNumDebug0 = DRV_Reg32(ECC_DECENUM0_REG32);
		u4ErrorNumDebug1 = DRV_Reg32(ECC_DECENUM1_REG32);
		if (0 != (u4ErrorNumDebug0 & 0xFFFFFFFF) || 0 != (u4ErrorNumDebug1 & 0xFFFFFFFF)) {
			for (i = 0; i <= u4SecIndex; ++i) {
#if 1
				u4ErrNum = (DRV_Reg32((ECC_DECENUM0_REG32+(i/4)))>>((i%4)*8))& ERR_NUM0;
#else
				if (i < 4)
					u4ErrNum = DRV_Reg32(ECC_DECENUM0_REG32) >> (i * 8);
				else
					u4ErrNum = DRV_Reg32(ECC_DECENUM1_REG32) >> ((i - 4) * 8);
				u4ErrNum &= ERR_NUM0;
#endif

				if (ERR_NUM0 == u4ErrNum) {
					MSG(ERR, "PL UnCorrectable at Page=%d, Sector=%d\n", u4PageAddr, i);
					bRet = false;
				}
#ifndef REDUCE_NAND_PL_SIZE
				else {
				if (u4ErrNum)
					MSG(ERR, "PL Correct %d at Page=%d, Sector=%d\n", u4ErrNum, u4PageAddr, i);
				}
#endif
			}
			if (false == bRet) {
				if (0 != (DRV_Reg32(NFI_STA_REG32) & STA_READ_EMPTY)) {
					MSG(INIT, "[xl] empty\n");
					MSG(ERR, "Empty pg 0x%x\n", u4PageAddr);
					bRet = true;
				}
			}
		}
	}
#else
/* We will manually correct the error bits in the last sector, not all the sectors of the page! */
	/* memset(au4ErrBitLoc, 0x0, sizeof(au4ErrBitLoc)); */
	u4ErrorNumDebug = DRV_Reg32(ECC_DECENUM_REG32);
	u4ErrNum = (DRV_Reg32((ECC_DECENUM_REG32+(u4SecIndex/4)))>>((u4SecIndex%4)*8))& ERR_NUM0;

	if (u4ErrNum) {
		if (ERR_NUM0 == u4ErrNum) {
			/* mtd->ecc_stats.failed++; */
			bRet = FALSE;
		} else {
			for (i = 0; i < ((u4ErrNum + 1) >> 1); ++i) {
				au4ErrBitLoc[i] = DRV_Reg32(ECC_DECEL0_REG32 + i);
				u4ErrBitLoc1th = au4ErrBitLoc[i] & 0x3FFF;
				if (u4ErrBitLoc1th < 0x1000) {
					u4ErrByteLoc = u4ErrBitLoc1th / 8;
					u4BitOffset = u4ErrBitLoc1th % 8;
					pDataBuf[u4ErrByteLoc] = pDataBuf[u4ErrByteLoc] ^ (1 << u4BitOffset);
					/* mtd->ecc_stats.corrected++; */
				} else {
					/* mtd->ecc_stats.failed++; */
					MSG(INIT, "UnCorrectable ErrLoc=%d\n", au4ErrBitLoc[i]);
				}
				u4ErrBitLoc2nd = (au4ErrBitLoc[i] >> 16) & 0x3FFF;
				if (0 != u4ErrBitLoc2nd) {
					if (u4ErrBitLoc2nd < 0x1000) {
						u4ErrByteLoc = u4ErrBitLoc2nd / 8;
						u4BitOffset = u4ErrBitLoc2nd % 8;
						pDataBuf[u4ErrByteLoc] = pDataBuf[u4ErrByteLoc] ^ (1 << u4BitOffset);
						/* mtd->ecc_stats.corrected++; */
					} else {
						/* mtd->ecc_stats.failed++; */
						MSG(INIT, "UnCorrectable High ErrLoc=%d\n", au4ErrBitLoc[i]);
					}
				}
			}
		}
		if (0 == (DRV_Reg16(ECC_DECFER_REG16) & (1 << u4SecIndex)))
			bRet = FALSE;
	}
#endif

	return bRet;
}

static bool mtk_nand_RFIFOValidSize(u16 u2Size)
{
	u32 timeout = 0xFFFF;

	while (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) < u2Size) {
		timeout--;
		if (0 == timeout)
			return FALSE;
	}
	if (u2Size == 0) {
		while (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16))) {
			timeout--;
			if (0 == timeout)
				return FALSE;
		}
	}
	return TRUE;
}

static bool mtk_nand_WFIFOValidSize(u16 u2Size)
{
	u32 timeout = 0xFFFF;

	while (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) > u2Size) {
		timeout--;
		if (0 == timeout)
			return FALSE;
	}
	if (u2Size == 0) {
		while (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16))) {
			timeout--;
			if (0 == timeout)
				return FALSE;
		}
	}
	return TRUE;
}

bool mtk_nand_reset(void)
{
	int timeout = 0xFFFF;

	if (DRV_Reg16(NFI_MASTERSTA_REG16) & 0xFFF) { /* master is busy */

		DRV_WriteReg32(NFI_CON_REG16, CON_FIFO_FLUSH | CON_NFI_RST);
		while (DRV_Reg16(NFI_MASTERSTA_REG16) & 0xFFF)
		{
			timeout--;
			if (!timeout)
				MSG(INIT, "MASTERSTA timeout\n");
		}
	}
	/* issue reset operation */
	DRV_WriteReg32(NFI_CON_REG16, CON_FIFO_FLUSH | CON_NFI_RST);

	return mtk_nand_status_ready(STA_NFI_FSM_MASK | STA_NAND_BUSY)\
		&& mtk_nand_RFIFOValidSize(0) && mtk_nand_WFIFOValidSize(0);
}

bool mtk_nand_device_reset(void)
{
	u32 timeout = 0xFFFF;

	mtk_nand_reset();

	DRV_WriteReg(NFI_CNFG_REG16, CNFG_OP_RESET);

	mtk_nand_set_command(NAND_CMD_RESET);

	while(!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));

	if(!timeout)
		return FALSE;
	else
		return TRUE;
}
//---------------------------------------------------------------------------
bool mtk_nand_SetFeature(u16 cmd, u32 addr, u8 *value,  u8 bytes)
{
	kal_uint16 reg_val = 0;
	kal_uint8 write_count = 0;
	kal_uint32 timeout = TIMEOUT_3;

	mtk_nand_reset();

	reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	mtk_nand_set_command(cmd);
	mtk_nand_set_address(addr, 0, 1, 0);

	DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);
	while ( (write_count < bytes) && timeout ) {
	    	WAIT_NFI_PIO_READY(timeout)
	        if(timeout == 0)
	        	break;
	        DRV_WriteReg32(NFI_DATAW_REG32, *value++);
	        write_count++;
	        timeout = TIMEOUT_3;
    	}
	while ( (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY) && (timeout) ){timeout--;}
	mtk_nand_read_status();

	if(timeout != 0)
		return TRUE;
	else
		return FALSE;
}

bool mtk_nand_GetFeature(u16 cmd, u32 addr, u8 *value,  u8 bytes)
{
	kal_uint16 reg_val = 0;
	kal_uint8 read_count = 0;
	kal_uint32 timeout = TIMEOUT_3;

	mtk_nand_reset();

	reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW | CNFG_READ_EN);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	mtk_nand_set_command(cmd);
	mtk_nand_set_address(addr, 0, 1, 0);

	DRV_WriteReg32(NFI_CON_REG16, 0 << CON_NFI_SEC_SHIFT);
	reg_val = DRV_Reg32(NFI_CON_REG16);
	reg_val &= ~CON_NOB_MASK;
	reg_val |= ((4 << CON_NOB_SHIFT)|CON_NFI_SRD);
	DRV_WriteReg32(NFI_CON_REG16, reg_val);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);
	while ((read_count < bytes) && timeout ) {
	    	WAIT_NFI_PIO_READY(timeout)
	        if(timeout == 0)
	            break;
		*value++ = DRV_Reg32(NFI_DATAR_REG32);
	        read_count++;
	        timeout = TIMEOUT_3;
    	}
	mtk_nand_read_status();
	if(timeout != 0)
		return TRUE;
	else
		return FALSE;

}

#if 1
static void mtk_nand_modeentry_rrtry(void)
{
	mtk_nand_reset();

	mtk_nand_set_mode(CNFG_OP_CUST);

	mtk_nand_set_command(0x5C);
	mtk_nand_set_command(0xC5);

	mtk_nand_status_ready(STA_NFI_OP_MASK);
}

static void mtk_nand_rren_rrtry(bool needB3)
{
	mtk_nand_reset();

	mtk_nand_set_mode(CNFG_OP_CUST);

	if(needB3)
		mtk_nand_set_command(0xB3);
	mtk_nand_set_command(0x26);
	mtk_nand_set_command(0x5D);

	mtk_nand_status_ready(STA_NFI_OP_MASK);
}

static void mtk_nand_rren_15nm_rrtry(bool flag)
{
	mtk_nand_reset();

	mtk_nand_set_mode(CNFG_OP_CUST);

	if(flag)
		mtk_nand_set_command(0x26);
	else
		mtk_nand_set_command(0xCD);

	mtk_nand_set_command(0x5D);

	mtk_nand_status_ready(STA_NFI_OP_MASK);
}
static void mtk_nand_sprmset_rrtry(u32 addr, u32 data) /* single parameter setting */
{
	u16           reg_val     	 = 0;
	u8            write_count     = 0;
	u32           reg = 0;
	u32           timeout=TIMEOUT_3;

	mtk_nand_reset();

	reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	mtk_nand_set_command(0x55);
	mtk_nand_set_address(addr, 0, 1, 0);

	mtk_nand_status_ready(STA_NFI_OP_MASK);

	DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);


	WAIT_NFI_PIO_READY(timeout);

    	DRV_WriteReg8(NFI_DATAW_REG32, data);

	while(!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));

}

static void mtk_nand_toshiba_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	u32 acccon;
	const u8 data_tbl[8][6] = {
		{0x04, 0x04, 0x7C, 0x7E, 0x00},
		{0x00, 0x7C, 0x78, 0x78, 0x00},
		{0x7C, 0x76, 0x74, 0x72, 0x00},
		{0x08, 0x08, 0x00, 0x00, 0x00},
		{0x0B, 0x7E, 0x76, 0x74, 0x00},
		{0x10, 0x76, 0x72, 0x70, 0x00},
		{0x02, 0x7C, 0x7E, 0x70, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00}
	};
	u8 add_reg[6] = {0x04, 0x05, 0x06, 0x07, 0x0D};
	u8 cnt = 0;

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669); /* to fit read retry timing */

	if(0 == retryCount)
		mtk_nand_modeentry_rrtry();

	for(cnt = 0; cnt < 5; cnt ++)
		mtk_nand_sprmset_rrtry(add_reg[cnt], data_tbl[retryCount][cnt]);

	if(3 == retryCount)
		mtk_nand_rren_rrtry(TRUE);
	else if(6 > retryCount)
		mtk_nand_rren_rrtry(FALSE);

	if(7 == retryCount) /* to exit */
		mtk_nand_device_reset();

	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}

static void mtk_nand_toshiba_15nm_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	u32 acccon;
	const u8 data_tbl[11][6] = {
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
	u8 add_reg[6] = {0x04, 0x05, 0x06, 0x07, 0x0D};
	u8 cnt = 0;

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669); /* to fit read retry timing */

	if(0 == retryCount)
		mtk_nand_modeentry_rrtry();

	for(cnt = 0; cnt < 5; cnt ++)
		mtk_nand_sprmset_rrtry(add_reg[cnt], data_tbl[retryCount][cnt]);

	if(defValue) /* to exit */
		mtk_nand_device_reset();
	else {
		if(0 == retryCount)
			mtk_nand_rren_15nm_rrtry(TRUE);
		else
			mtk_nand_rren_15nm_rrtry(FALSE);
	}
	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}
#endif

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

	mtk_nand_reset();

	mtk_nand_set_mode(CNFG_OP_CUST);

	mtk_nand_set_command(0x5C);
	mtk_nand_set_command(0xC5);
	mtk_nand_set_command(0x55);
	mtk_nand_set_address(0x00, 0, 1, 0);

	mtk_nand_status_ready(STA_NFI_OP_MASK);

	DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);


	WAIT_NFI_PIO_READY(timeout);
	timeout=TIMEOUT_3;
	DRV_WriteReg8(NFI_DATAW_REG32, 0x01);

	while(!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));
}

static void mtk_nand_rren_rrtry_tlc_a19nm(bool needB3)
{
	mtk_nand_reset();

	mtk_nand_set_mode(CNFG_OP_CUST);

	if(needB3)
		mtk_nand_set_command(0xB3);
	mtk_nand_set_command(0x5D);

	mtk_nand_status_ready(STA_NFI_OP_MASK);
}

static void mtk_nand_sprmset_rrtry_tlc_a19nm(u32 addr, u32 data) /* single parameter setting */
{
	u16           reg_val     	 = 0;
	u8            write_count     = 0;
	u32           reg = 0;
	u32           timeout=TIMEOUT_3;

	mtk_nand_reset();

	reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	mtk_nand_set_command(0x55);
	mtk_nand_set_address(addr, 0, 1, 0);

	mtk_nand_status_ready(STA_NFI_OP_MASK);

	DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);


	WAIT_NFI_PIO_READY(timeout);
	timeout=TIMEOUT_3;
	DRV_WriteReg8(NFI_DATAW_REG32, data);

	while(!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));
}

static void mtk_nand_toshiba_rrtry_tlc_a19nm(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	u32 acccon;
    u8 cnt = 0;
    u8 add_reg_tlc[7] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
	u8 add_reg_slc = 0x0B;

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669); /* to fit read retry timing */

	if(0 == retryCount)
		mtk_nand_modeentry_rrtry_tlc_a19nm();

	if(devinfo.tlcControl.slcopmodeEn)
		mtk_nand_sprmset_rrtry_tlc_a19nm(add_reg_slc, data_tbl_slc_a19nm[cnt]);
	else {
		for(cnt = 0; cnt < 7; cnt ++)
			mtk_nand_sprmset_rrtry_tlc_a19nm(add_reg_tlc[cnt], data_tbl_tlc_a19nm[retryCount][cnt]);
	}

	if(defValue) {
		mtk_nand_device_reset();
		mtk_nand_reset();
		/* should do NAND DEVICE interface change under sync mode */
	} else {
		if((!devinfo.tlcControl.slcopmodeEn)
			&& (30 == retryCount))
			mtk_nand_rren_rrtry_tlc_a19nm(TRUE);
		else
			mtk_nand_rren_rrtry_tlc_a19nm(FALSE);
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
	if(FALSE == defValue)
		mtk_nand_reset();
	else
		mtk_nand_device_reset();
	mtk_nand_SetFeature(deviceinfo.feature_set.FeatureSet.sfeatureCmd,\
				deviceinfo.feature_set.FeatureSet.readRetryAddress,\
				(u8 *)&feature,4);
	if(FALSE == defValue)
		mtk_nand_set_command(deviceinfo.feature_set.FeatureSet.readRetryPreCmd);
}

#ifndef REDUCE_NAND_PL_SIZE
static void sandisk_19nm_rr_init(void)
{
	u32 reg_val = 0;
	u32 count = 0;
	u32 timeout = 0xffff;
	u32 acccon;

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669); /* to fit read retry timing */

	mtk_nand_reset();

	reg_val = (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);
	mtk_nand_set_command(0x3B);
	mtk_nand_set_command(0xB9);

	for(count = 0; count < 9; count++) {
		mtk_nand_set_command(0x53);
		mtk_nand_set_address((0x04 + count), 0, 1, 0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		timeout = 0xffff;
		WAIT_NFI_PIO_READY(timeout);
		DRV_WriteReg32(NFI_DATAW_REG32, 0x00);
		mtk_nand_reset();
	}

	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}

static void sandisk_19nm_rr_loading(u32 retryCount, bool defValue)
{
	u32 reg_val = 0;
	u32 timeout = 0xffff;
	u32 acccon;

	u8 count;
	u8 cmd_reg[4] = {0x4, 0x5, 0x7};
	u16 sandisk_19nm_rr_table[18] = {
		0x0000,
		0xFF0F, 0xEEFE, 0xDDFD, 0x11EE,
		0x22ED, 0x33DF, 0xCDDE, 0x01DD,
		0x0211, 0x1222, 0xBD21, 0xAD32,
		0x9DF0, 0xBCEF, 0xACDC, 0x9CFF,
		0x0000 /* align */
	};

	acccon = DRV_Reg32(NFI_ACCCON_REG32);
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x31C08669); /* to fit read retry timing */

	mtk_nand_reset();

	reg_val = (CNFG_OP_CUST | CNFG_BYTE_RW);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	if((0 != retryCount) || defValue)
	 	mtk_nand_set_command(0xD6);
	mtk_nand_set_command(0x3B);
	mtk_nand_set_command(0xB9);

	for(count = 0; count < 3; count++) {
		mtk_nand_set_command(0x53);
		mtk_nand_set_address(cmd_reg[count], 0, 1, 0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		timeout = 0xffff;
		WAIT_NFI_PIO_READY(timeout);
		if(count == 0)
			DRV_WriteReg32(NFI_DATAW_REG32,
				(((sandisk_19nm_rr_table[retryCount] & 0xF000) >> 8) \
				| ((sandisk_19nm_rr_table[retryCount] & 0x00F0) >> 4)));
		else if(count == 1)
			DRV_WriteReg32(NFI_DATAW_REG32, \
				((sandisk_19nm_rr_table[retryCount] & 0x000F) << 4));
		else if(count == 2)
			DRV_WriteReg32(NFI_DATAW_REG32, \
				((sandisk_19nm_rr_table[retryCount] & 0x0F00) >> 4));

		mtk_nand_reset();
	}

	if(!defValue)
		mtk_nand_set_command(0xB6);

	DRV_WriteReg32(NFI_ACCCON_REG32, acccon);
}

static void mtk_nand_sandisk_19nm_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	if((retryCount == 0) && (!defValue))
		sandisk_19nm_rr_init();
	sandisk_19nm_rr_loading(retryCount, defValue);
}
#else
static void mtk_nand_sandisk_19nm_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	/* nop */
}
#endif

#define HYNIX_RR_TABLE_SIZE  (1026)  /* hynix read retry table size */
#define SINGLE_RR_TABLE_SIZE (64)

#define READ_RETRY_STEP devinfo.feature_set.FeatureSet.readRetryCnt
#define HYNIX_16NM_RR_TABLE_SIZE  ((READ_RETRY_STEP == 12)?(784):(528))  /* hynix read retry table size */
#define SINGLE_RR_TABLE_16NM_SIZE  ((READ_RETRY_STEP == 12)?(48):(32))

#define NAND_HYX_RR_TBL_BUF g_dram_buf->nand_hynix_rr_table

static u8 real_hynix_rr_table_idx = 0;
static u32 g_hynix_retry_count = 0;
static u8 real_hynix_rr_tabl[48];

static bool hynix_rr_table_select(u8 table_index, flashdev_info *deviceinfo)
{
	u32 i,j,k;
	u32 zero_num = 0;
	u32 one_num = 0;
	u32 table_size = (deviceinfo->feature_set.FeatureSet.rtype == \
		RTYPE_HYNIX)? SINGLE_RR_TABLE_SIZE : SINGLE_RR_TABLE_16NM_SIZE;
	u8 *temp_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF+table_size*table_index*2+2;
	u8 *temp_inversed_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF+table_size*table_index*2+table_size+2;

	if(deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX) { /* 20nm */
		for(i = 0; i < table_size; i++){
				if(0xFF != (temp_rr_table[i] ^ temp_inversed_rr_table[i]))
					return FALSE; /* error table */
		}
	} else { /* 16nm & F-Die */
		temp_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF + 16;
		temp_inversed_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF + table_size + 16;
		for(j = 0; j < table_size; j++) { /* byte of each set */
			real_hynix_rr_tabl[j] = 0; /* initial */

			for(k = 0; k < 8; k++) { /* bit of each byte */
				zero_num = 0;
				one_num = 0;

				for(i = 0; i < 8; i++) {
					if((temp_rr_table[(table_size * i * 2) + j] >> k) & 0x1)
						one_num ++;
					else
						zero_num ++;
				}
				if(one_num == zero_num) {
					zero_num = 0;
					one_num = 0;
					for(i = 0; i < 8; i++) {
						if((temp_inversed_rr_table[(table_size * i * 2) + j] >> k) & 0x1)
							one_num ++;
						else
							zero_num ++;
					}
					if(one_num == zero_num) /* no valuable table existed */
						return FALSE;
					else /* repeated time > 4 */
						real_hynix_rr_tabl[j] |= (((zero_num > one_num) ? 1 : 0) << k);
				} else /* repeated time > 4 */
					real_hynix_rr_tabl[j] |= (((zero_num > one_num) ? 0 : 1) << k);
			}
		}
	}

	return TRUE; /* correct table */
}

static void HYNIX_RR_TABLE_READ(flashdev_info *deviceinfo)
{
	u32 reg_val = 0;
	u32 read_count = 0, max_count = HYNIX_RR_TABLE_SIZE;
	u32 timeout = 0xffff;
	u8*  rr_table = (u8*)(NAND_HYX_RR_TBL_BUF);
	u8 table_index = 0;
	u8 add_reg1[3] = {0xFF, 0xCC};
	u8 data_reg1[3] = {0x40, 0x4D};
	u8 cmd_reg[6] = {0x16, 0x17, 0x04, 0x19, 0x00};
	u8 add_reg2[6] = {0x00, 0x00, 0x00, 0x02, 0x00};
	bool RR_TABLE_EXIST = TRUE;
	u32 table_size = (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX)? \
		SINGLE_RR_TABLE_SIZE : SINGLE_RR_TABLE_16NM_SIZE;
	u8 *hynix_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF+table_size*real_hynix_rr_table_idx*2+2;

	if((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
		|| (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
        	read_count = 1;
		if(deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
        		add_reg1[1]= 0x38;
		else /* FDIE */
			add_reg1[1]= 0x0E;
	        data_reg1[1] = 0x52;
	        max_count = HYNIX_16NM_RR_TABLE_SIZE;
	        if(READ_RETRY_STEP == 12)
	            add_reg2[2] = 0x1F;
	}
	mtk_nand_device_reset();
	/* take care under sync mode. need change nand device inferface xiaolei */

	mtk_nand_reset();

	DRV_WriteReg(NFI_CNFG_REG16, (CNFG_OP_CUST | CNFG_BYTE_RW));

	mtk_nand_set_command(0x36);

	for(; read_count < 2; read_count++) {
		mtk_nand_set_address(add_reg1[read_count],0,1,0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		timeout = 0xffff;
		WAIT_NFI_PIO_READY(timeout);
		DRV_WriteReg32(NFI_DATAW_REG32, data_reg1[read_count]);
		mtk_nand_reset();
	}

	for(read_count = 0; read_count < 5; read_count++)
		mtk_nand_set_command(cmd_reg[read_count]);
	for(read_count = 0; read_count < 5; read_count++)
        	mtk_nand_set_address(add_reg2[read_count],0,1,0);
	mtk_nand_set_command(0x30);
	DRV_WriteReg(NFI_CNRNB_REG16, 0xF1);
	timeout = 0xffff;
	while(!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));

	reg_val = (CNFG_OP_CUST | CNFG_BYTE_RW | CNFG_READ_EN);
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);
	DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BRD | (2<< CON_NFI_SEC_SHIFT)));
	DRV_WriteReg(NFI_STRDATA_REG16, 0x1);
	timeout = 0xffff;
	read_count = 0;
	while ((read_count < max_count) && timeout ) {
		WAIT_NFI_PIO_READY(timeout);
		*rr_table++ = (U8)DRV_Reg32(NFI_DATAR_REG32);
		read_count++;
		timeout = 0xFFFF;
	}

	mtk_nand_device_reset();
	/* take care under sync mode. need change nand device inferface xiaolei */

	reg_val = (CNFG_OP_CUST | CNFG_BYTE_RW);
	if((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
		|| (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
		DRV_WriteReg(NFI_CNFG_REG16, reg_val);
		mtk_nand_set_command(0x36);
		mtk_nand_set_address(add_reg1[1],0,1,0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		WAIT_NFI_PIO_READY(timeout);
		DRV_WriteReg32(NFI_DATAW_REG32, 0x00);
		mtk_nand_reset();
		mtk_nand_set_command(0x16);
		mtk_nand_reset();
		mtk_nand_set_mode(CNFG_OP_READ);
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
		mtk_nand_set_command(0x00);
		mtk_nand_set_address(0,0xC800,2,3);//dummy read, add don't care
		mtk_nand_set_command(0x30);
	} else {
		DRV_WriteReg(NFI_CNFG_REG16, reg_val);
		mtk_nand_set_command(0x38);
	}
	timeout = 0xffff;
	while(!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));
	mtk_nand_reset();

	rr_table = (u8*)(NAND_HYX_RR_TBL_BUF);

	MSG(INIT, "RR Table in OTP: \n");
	read_count = 0;
	while (read_count < max_count) {
		MSG(INIT, "0x%x ", rr_table[read_count]);
		read_count++;
		if((read_count % 8) == 0)
			MSG(INIT, "\n");
    	}
	MSG(INIT, "\n");

	if(deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX) {
	        if((rr_table[0] != 8) || (rr_table[1] != 8)) {
	            RR_TABLE_EXIST = FALSE;
	    	    ASSERT(0);
	        }
	} else if((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
		     || (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
	    	for(read_count=0;read_count<8;read_count++) {
	        	if((rr_table[read_count] != READ_RETRY_STEP) || (rr_table[read_count+8] != 4)) {
	        		RR_TABLE_EXIST = FALSE;
	    			break;
	        	}
	    	}
	}

	if(RR_TABLE_EXIST) {
		for(table_index = 0 ;table_index < 8; table_index++) {
			if(hynix_rr_table_select(table_index, deviceinfo)) {
				real_hynix_rr_table_idx = table_index;
				MSG(INIT, "Hynix rr_tbl_id %d\n",real_hynix_rr_table_idx);
				break;
			} else {
				if((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
				    || (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
					MSG(INIT, "RR Table not found\n");
					ASSERT(0);
				}
			}
		}
		if(table_index == 8) {
			MSG(INIT, "RR Table not found\n");
			ASSERT(0);
		}

		if(deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX)
			hynix_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF+table_size*real_hynix_rr_table_idx*2+2;
		else
			hynix_rr_table = (u8 *)real_hynix_rr_tabl;

		MSG(INIT, "RR Table In DRAM: \n");
		read_count = 0;
		while (read_count < table_size) {
			MSG(INIT, "0x%x ", hynix_rr_table[read_count]);
			read_count++;
			if((read_count % 8) == 0)
				MSG(INIT, "\n");
 		}
		MSG(INIT, "\n");
	} else
        	MSG(INIT, "Hynix RR table index error!\n");
}

static void HYNIX_Set_RR_Para(u32 rr_index, flashdev_info *deviceinfo)
{
	u32 reg_val = 0;
	u32 timeout=0xffff;
	u8 count, max_count = 8;
	u8 add_reg[9] = {0xCC, 0xBF, 0xAA, 0xAB, 0xCD, 0xAD, 0xAE, 0xAF};
	u32 table_size = (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX)? \
		SINGLE_RR_TABLE_SIZE : SINGLE_RR_TABLE_16NM_SIZE;
	u8 *hynix_rr_table = (u8 *)NAND_HYX_RR_TBL_BUF+table_size*real_hynix_rr_table_idx*2+2;

	if((deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
		|| (deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE)) {
		if(deviceinfo->feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
			add_reg[0] = 0x38; /* 0x38, 0x39, 0x3A, 0x3B */
		else /* FDIE */
			add_reg[0] = 0x0E; /* 0x0E, 0x0F, 0x10, 0x11 */
		for(count =1; count < 4; count++)
			add_reg[count] = add_reg[0] + count;
		#if 1
		hynix_rr_table = (u8 *)real_hynix_rr_tabl;
		#else
        	hynix_rr_table += 14;
		#endif
		max_count = 4;
	}
	mtk_nand_reset();

	DRV_WriteReg(NFI_CNFG_REG16, (CNFG_OP_CUST | CNFG_BYTE_RW));

	mtk_nand_set_command(0x36);

	for(count = 0; count < max_count; count++) {
		/* mtk_nand_set_command(0x36); */
		udelay(1);
		mtk_nand_set_address(add_reg[count], 0, 1, 0);
		DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
		DRV_WriteReg(NFI_STRDATA_REG16, 1);
		timeout = 0xffff;
		WAIT_NFI_PIO_READY(timeout);
		DRV_WriteReg32(NFI_DATAW_REG32, hynix_rr_table[rr_index*max_count + count]);
		/* mtk_nand_reset(); */
	}
	mtk_nand_set_command(0x16);
}

static void mtk_nand_hynix_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	if(defValue == FALSE) {
		if(g_hynix_retry_count == READ_RETRY_STEP)
			g_hynix_retry_count = 0;
		HYNIX_Set_RR_Para(g_hynix_retry_count, &deviceinfo);
		/* HYNIX_Get_RR_Para(g_hynix_retry_count, &deviceinfo); */
		g_hynix_retry_count ++;
	}
	/* HYNIX_Set_RR_Para(retryCount, &deviceinfo); */
}

static void mtk_nand_hynix_16nm_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	if(defValue == FALSE) {
		if(g_hynix_retry_count == READ_RETRY_STEP)
			g_hynix_retry_count = 0;
		HYNIX_Set_RR_Para(g_hynix_retry_count, &deviceinfo);
		/* mb(); */
		/* HYNIX_Get_RR_Para(g_hynix_retry_count, &deviceinfo); */
	g_hynix_retry_count ++;
	}
	/* HYNIX_Set_RR_Para(retryCount, &deviceinfo); */
}

static void mtk_nand_hynix_fdie_rrtry(flashdev_info deviceinfo, u32 retryCount, bool defValue)
{
	if(defValue == FALSE) {
		if(g_hynix_retry_count == READ_RETRY_STEP)
			g_hynix_retry_count = 0;
		HYNIX_Set_RR_Para(g_hynix_retry_count, &deviceinfo);
		/* mb(); */
		/* HYNIX_Get_RR_Para(g_hynix_retry_count, &deviceinfo); */
	g_hynix_retry_count ++;
	}
	/* HYNIX_Set_RR_Para(retryCount, &deviceinfo); */
}

int mtk_nand_read_page_hwecc(u64 logical_addr, char *buf);

void compare(u8 * testbuff, u8 * sourcebuff, u32 length, char *s)
{
	u32 errnum = 0;
	u32 ii = 0;
	u32 index;

	printf("%s", s);
	for (index = 0; index < length; index++) {
		if (testbuff[index] != sourcebuff[index]) {
			u8 t = sourcebuff[index] ^ testbuff[index];
			for (ii = 0; ii < 8; ii++) {
				if ((t >> ii) & 0x1 == 1)
					errnum++;
			}
		}

	}
	if (errnum > 0)
		printf(": page have %d mismatch bits\n", errnum);
	else
		printf(" :the two buffers are same!\n");
}

static int HYNIX_LSBFF_Verify(flashdev_info deviceinfo, u64 offset, u8 * buf)
{
	u32 timeout;
	int rtn;

	mtk_nand_device_reset();
	mtk_nand_reset();
	DRV_WriteReg(NFI_CNFG_REG16, (CNFG_OP_CUST | CNFG_BYTE_RW));
	mtk_nand_set_command(0x36);
	/* 0x38 addr --> 20nm */
	/* 0x0E addr --> 16nm */
	if(deviceinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX)
		mtk_nand_set_address(0x38, 0, 1, 0);
	else
		mtk_nand_set_address(0x0E, 0, 1, 0);
	DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
	DRV_WriteReg(NFI_STRDATA_REG16, 1);
	timeout = 0xffff;
	WAIT_NFI_PIO_READY(timeout);
	DRV_WriteReg32(NFI_DATAW_REG32, 0x6A);
	mtk_nand_set_command(0x16);

	/* read data below */
	/* row_addr should be the real address */
	/* rtn = mtk_nand_read_page_hw(row_addr, testbuff, g_nand_spare); */
	rtn = mtk_nand_read_page_hwecc(offset, buf);
	if(!rtn)
		printf("HYNIX_LSBFF_Verify fail at offset 0x%llx\n", offset);

	mtk_nand_device_reset();
	mtk_nand_reset();
	DRV_WriteReg(NFI_CNFG_REG16, (CNFG_OP_CUST | CNFG_BYTE_RW));
	mtk_nand_set_command(0x36);
	/* 0x38 addr --> 20nm */
	/* 0x0E addr --> 16nm */
	if(deviceinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX)
		mtk_nand_set_address(0x38, 0, 1, 0);
	else
		mtk_nand_set_address(0x0E, 0, 1, 0);
    	DRV_WriteReg(NFI_CON_REG16, (CON_NFI_BWR | (1 << CON_NFI_SEC_SHIFT)));
	DRV_WriteReg(NFI_STRDATA_REG16, 1);
	timeout = 0xffff;
	WAIT_NFI_PIO_READY(timeout);
    	DRV_WriteReg32(NFI_DATAW_REG32, 0x00);
	mtk_nand_set_command(0x16);
	mtk_nand_reset();
	mtk_nand_set_mode(CNFG_OP_READ);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
	mtk_nand_set_command(0x00);
	mtk_nand_set_address(0,0xC800,2,3);/* 0xC800 = 200 BLOCK, dummy read, add don't care */
	mtk_nand_set_command(0x30);
	timeout = 0xffff;
	while(!(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY_RETURN) && (timeout--));
	mtk_nand_reset();

	return rtn;
}

void mtk_nand_hynix_lsbff_vry(flashdev_info deviceinfo, u64 offset)
{
	int rtn;
	u32 block, mapped_block, page_no;
	u8 *testbuff = (u8*)nand_nfi_buf;
	u8 *sourcebuff = (u8*)storage_buffer;

	rtn = mtk_nand_read_page_hwecc(offset, sourcebuff);
	if(!rtn)
		printf("[lsbff] read 0x%x fail\n", offset);

	g_bHwEcc = FALSE;

	rtn = HYNIX_LSBFF_Verify(deviceinfo, offset, testbuff);
	if(!rtn)
		printf("[lsbff] verify read 0x%x fail\n", offset);

	compare(testbuff, sourcebuff, devinfo.pagesize, "lsbff verify check ");

	use_randomizer = FALSE;
	pre_randomizer = FALSE;
	page_no = mtk_nand_page_transform(offset,&block,&mapped_block);
	if(((page_no % 256) == 0) || ((page_no % 256) == 253)) {
		page_no += 2;
		rtn = mtk_nand_read_page_hw(page_no, testbuff, g_nand_spare);
		if(!rtn)
			printf("[lsbff] msb read %d fail\n", page_no);
	} else {
		page_no += 3;
		rtn = mtk_nand_read_page_hw(page_no, testbuff, g_nand_spare);
		if(!rtn)
			printf("[lsbff] msb read %d fail\n", page_no);
	}
	/* memset(sourcebuff, 0xFF, devinfo.pagesize); */
	/* compare(testbuff, sourcebuff, devinfo.pagesize, "msbff verify check "); */
	use_randomizer = TRUE;
	pre_randomizer = TRUE;
	g_bHwEcc = TRUE;
}

/* sandisk 1y nm */
u32 special_rrtry_setting[37]= {
	0x00000000,0x7C00007C,0x787C0004,0x74780078,
	0x7C007C08,0x787C7C00,0x74787C7C,0x70747C00,
	0x7C007800,0x787C7800,0x74787800,0x70747800,
	0x6C707800,0x00040400,0x7C000400,0x787C040C,
	0x7478040C,0x7C000810,0x00040810,0x04040C0C,
	0x00040C10,0x00081014,0x000C1418,0x7C040C0C,
	0x74787478,0x70747478,0x6C707478,0x686C7478,
	0x74787078,0x70747078,0x686C7078,0x6C707078,
	0x6C706C78,0x686C6C78,0x64686C78,0x686C6874,
	0x64686874,
};

/* sandisk 1z nm */
u32 sandisk_1znm_rrtry_setting[33]= {
	0x00000000,0x00000404,0x00007C7C,0x7C7C0404,
	0x00000808,0x7C7C0000,0x7C7C0808,0x04080404,
	0x04040000,0x7C007C7C,0x04080808,0x787C040C,
	0x78780000,0x00000C0C,0x00007878,0x04007C7C,
	0x7C747878,0x78787C7C,0x08040404,0x04080C08,
	0x08080808,0x78787878,0x007C7C7C,0x00040408,
	0x00080000,0x00780804,0x7C780C08,0x7874087C,
	0x74787C7C,0x74740000,0x08000000,0x747C7878,
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
	u32	timeout = TIMEOUT_3;
	u32 value1, value2, value3;

	if((feature > 1) || defValue) { /* add exit rr cmd sequence */
		/* set 0x55h cmd + 0x00h address + 0x00 data */
		mtk_nand_reset();
		reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
		DRV_WriteReg(NFI_CNFG_REG16, reg_val);

		mtk_nand_set_command(0x55);
		mtk_nand_set_address(0, 0, 1, 0);
		DRV_WriteReg32(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT);
		NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
		DRV_WriteReg(NFI_STRDATA_REG16, 0x1);
		WAIT_NFI_PIO_READY(timeout)
		if(timeout == 0)
			MSG(INIT, "mtk_nand_sandisk_tlc_1ynm_rrtry: timeout\n");
		DRV_WriteReg32(NFI_DATAW_REG32, 0);

		/* set device reset */
		mtk_nand_device_reset();
	}

	if(devinfo.tlcControl.slcopmodeEn) {
		if (process == 1)
			value3 = sandisk_tlc_rrtbl_14h[feature];
		else if (process == 2)
			value3 = sandisk_1z_tlc_rrtbl_14h[feature];
		mtk_nand_SetFeature(deviceinfo.feature_set.FeatureSet.sfeatureCmd, 0x14, (u8 *)&value3, 4);
	}
	else {
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

	if(FALSE == defValue) {
		mtk_nand_reset();
		reg_val |= (CNFG_OP_CUST | CNFG_BYTE_RW);
		DRV_WriteReg(NFI_CNFG_REG16, reg_val);

		mtk_nand_set_command(0x5D);

		mtk_nand_reset();
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
		} else if(retryStart != 0xFFFFFFFF) {
			value = retryStart+loopNo;
		} else {
			value = special_rrtry_setting[loopNo];
		}
	}

	return value;
}

typedef u32 (*rrtryFunctionType)(flashdev_info deviceinfo, u32 feature, bool defValue);

static rrtryFunctionType rtyFuncArray[]=
{
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
	rtyFuncArray[deviceinfo.feature_set.FeatureSet.rtype](deviceinfo, feature,defValue);
}

static bool mtk_nand_read_status(void)
{
	int status, i;
	unsigned int timeout;

	mtk_nand_reset();

	/* Disable HW ECC */
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

	/* Disable 16-bit I/O */
	NFI_CLN_REG32(NFI_PAGEFMT_REG32, PAGEFMT_DBYTE_EN);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_OP_SRD | CNFG_READ_EN | CNFG_BYTE_RW);

	DRV_WriteReg32(NFI_CON_REG16, CON_NFI_SRD | (1 << CON_NOB_SHIFT));

	DRV_WriteReg32(NFI_CON_REG16, 0x3);
	mtk_nand_set_mode(CNFG_OP_SRD);
	DRV_WriteReg16(NFI_CNFG_REG16, 0x2042);
	mtk_nand_set_command(NAND_CMD_STATUS);
	DRV_WriteReg32(NFI_CON_REG16, 0x90);

	timeout = TIMEOUT_4;
	WAIT_NFI_PIO_READY(timeout);

	if (timeout)
	status = (DRV_Reg16(NFI_DATAR_REG32));

	/* ~  clear NOB */
	DRV_WriteReg32(NFI_CON_REG16, 0);

	if (g_nand_chip.bus16 == NAND_BUS_WIDTH_16) {
		NFI_SET_REG32(NFI_PAGEFMT_REG32, PAGEFMT_DBYTE_EN);
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	}
	/* check READY/BUSY status first */
	if (!(STATUS_READY & status))
		MSG(ERR, "status is not ready\n");
	/* flash is ready now, check status code */
#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	&& (devinfo.tlcControl.slcopmodeEn)) { /*hynix tlc need doule check */

		if (SLC_MODE_OP_FALI & status) {
			if (!(STATUS_WR_ALLOW & status)) {
				MSG(INIT, "status locked\n");
				return FALSE;
			} else {
				MSG(INIT, "status unknown\n");
				return FALSE;
			}
		} else
			return TRUE;
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
		} else
			return TRUE;
	}
}

static void mtk_nand_configure_fdm(u16 u2FDMSize)
{
	NFI_CLN_REG32(NFI_PAGEFMT_REG32, PAGEFMT_FDM_MASK | PAGEFMT_FDM_ECC_MASK);
	NFI_SET_REG32(NFI_PAGEFMT_REG32, u2FDMSize << PAGEFMT_FDM_SHIFT);
	NFI_SET_REG32(NFI_PAGEFMT_REG32, u2FDMSize << PAGEFMT_FDM_ECC_SHIFT);
}

static void mtk_nand_set_autoformat(bool bEnable)
{
	if (bEnable)
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	else
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
}

static void mtk_nand_command_bp(unsigned command)
{
	u32 timeout;

	switch (command)
	{
	case NAND_CMD_READID:
		/* Issue NAND chip reset command */
		mtk_nand_device_reset();

		mtk_nand_reset();

		/* Disable HW ECC */
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

		/* Disable 16-bit I/O */
		NFI_CLN_REG32(NFI_PAGEFMT_REG32, PAGEFMT_DBYTE_EN);
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN | CNFG_BYTE_RW);
		mtk_nand_reset();
		mtk_nand_set_mode(CNFG_OP_SRD);
		mtk_nand_set_command(NAND_CMD_READID);
		mtk_nand_set_address(0, 0, 1, 0);
		DRV_WriteReg32(NFI_CON_REG16, CON_NFI_SRD);
		while (DRV_Reg32(NFI_STA_REG32) & STA_DATAR_STATE) ;
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
static u8 mtk_nand_read_byte(void)
{
	/* Check the PIO bit is ready or not */
	u32 timeout = TIMEOUT_4;
	WAIT_NFI_PIO_READY(timeout);
	return DRV_Reg8(NFI_DATAR_REG32);
}

bool getflashid(u8 * nand_id, int longest_id_number)
{
	u8 maf_id = 0;
	u8 dev_id = 0;
	int i = 0;
	u8 *id = nand_id;
	u32 regVal;

	DRV_WriteReg32(NFI_ACCCON_REG32, NFI_DEFAULT_ACCESS_TIMING);

	DRV_WriteReg16(NFI_CNFG_REG16, 0);
	regVal = DRV_Reg32(NFI_PAGEFMT_REG32) & 0x4;
	DRV_WriteReg32(NFI_PAGEFMT_REG32, regVal);

	mtk_nand_command_bp(NAND_CMD_READID);

	maf_id = mtk_nand_read_byte();
	dev_id = mtk_nand_read_byte();

	if (maf_id == 0 || dev_id == 0)
		return FALSE;

	id[0] = maf_id;
	id[1] = dev_id;

	for (i = 2; i < longest_id_number; i++)
		id[i] = mtk_nand_read_byte();

	return TRUE;
}
/*******************************************************************************
 * GPIO(PinMux) register definition
 *******************************************************************************/
//For NFI GPIO setting */
#define NFI_GPIO_BASE 0x10005000
/* NCLE */
#define NFI_GPIO_MODE3				(NFI_GPIO_BASE + 0x320)
/* NCEB1/NCEB0/NREB */
#define NFI_GPIO_MODE4				(NFI_GPIO_BASE + 0x330)
/* NRNB/NREB_C/NDQS_C */
#define NFI_GPIO_MODE5				(NFI_GPIO_BASE + 0x340)
/* NLD7/NLD6/NLD4/NLD3/NLD0 */
#define NFI_GPIO_MODE17				(NFI_GPIO_BASE + 0x460)
/* NALE/NWEB/NLD1/NLD5/NLD8  NLD8--->NDQS */
#define NFI_GPIO_MODE18				(NFI_GPIO_BASE + 0x470)
/* NLD2 */
#define NFI_GPIO_MODE19				(NFI_GPIO_BASE + 0x480)

/* PD, NCEB0/NCEB1/NRNB */
#define NFI_GPIO_PULLUP				(NFI_GPIO_BASE + 0xE60)

/* Drving */
#define NFI_GPIO_DRV_MODE0		(NFI_GPIO_BASE + 0xD00)
#define NFI_GPIO_DRV_MODE6		(NFI_GPIO_BASE + 0xD60)
#define NFI_GPIO_DRV_MODE7		(NFI_GPIO_BASE + 0xD70)

/* TDSEL, no need */

/* RDSEL, no need for 1.8V */
#define NFI_GPIO_RDSEL1_EN		(NFI_GPIO_BASE + 0xC10)
#define NFI_GPIO_RDSEL6_EN		(NFI_GPIO_BASE + 0xC60)
#define NFI_GPIO_RDSEL7_EN		(NFI_GPIO_BASE + 0xC70)

void nand_gpio_cfg_bit32(u32 addr, u32 field , u32 val)
{
	u32 tv = (unsigned int)(*(volatile u32*)(addr));

	tv &= ~(field);
	tv |= val;
	(*(volatile u32*)(addr) = (u32)(tv));
}

#define NFI_GPIO_CFG_BIT32(reg,field,val) nand_gpio_cfg_bit32(reg, field, val)

#define NFI_EFUSE_M_SW_RES	((volatile u32 *)(0x10009000 + 0x0120))
#define NFI_EFUSE_IO_33V	0x00000100
/* 0 : 3.3v (MT8130 default), 1 : 1.8v */
#define NFI_EFUSE_Is_IO_33V()	(((*NFI_EFUSE_M_SW_RES)&NFI_EFUSE_IO_33V)?FALSE:TRUE)

static void mtk_nand_gpio_init(void)
{
	/* Enable Pinmux Function setting */
	/* NCLE*/
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE3, (0x7 << 12), (0x6 << 12));
	/* NCEB1/NCEB0/NREB */
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE4, (0x1FF << 0), (0x1B6 << 0));
	/* NRNB/NREB_C/NDQS_C */
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE5, (0x1FF << 3), (0x1B1 << 3));
	/* NLD7/NLD6/NLD4/NLD3/NLD0 */
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE17, (0x7FFF << 0), (0x4924 << 0));
	/* NALE/NWEB/NLD1/NLD5/NLD8, NLD8 for NDQS */
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE18, (0x7FFF << 0), (0x4924 << 0));
	/* NLD2 */
	NFI_GPIO_CFG_BIT32(NFI_GPIO_MODE19, (0x7 << 0), (0x5 << 0));

	/* PULL UP setting */
	/* PD, NCEB0, NCEB1, NRNB */
	NFI_GPIO_CFG_BIT32(NFI_GPIO_PULLUP, (0xF0FF << 0), (0x1011 << 0));

	/* Driving setting */
	if(NFI_EFUSE_Is_IO_33V()) {
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE0, (0xF << 12), (0x2 << 12));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE6, (0xFF << 8), (0x22 << 8));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE7, (0xFF << 0), (0x22 << 0));
	} else {
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE0, (0xF << 12), (0x4 << 12));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE6, (0xFF << 8), (0x44 << 8));
		NFI_GPIO_CFG_BIT32(NFI_GPIO_DRV_MODE7, (0xFF << 0), (0x44 << 0));
	}

	/* TDSEL, No need */

	/* RDSEL, only need for 3.3V */
	if(NFI_EFUSE_Is_IO_33V()) {
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

	DRV_WriteReg16(NFI_CSEL_REG16, cs);
	mtk_nand_command_bp(NAND_CMD_READID);
	for(i=0;i<NAND_MAX_ID;i++) {
		ids[i]=mtk_nand_read_byte();
		if(ids[i] != id[i]) {
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
		return (page - g_nanddie_pages);
	}
	DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
	return page;
}

#else

#define mtk_nand_cs_check(id, cs)  (1)
#define mtk_nand_cs_on(cs, page)   (page)
#endif

/* #define NAND_PL_UT */
#ifdef NAND_PL_UT
void nand_pl_ut(void)
{
	u8 *testbuff = (u8*)nand_nfi_buf;
	u8 *sourcebuff = (u8*)storage_buffer;
	u64 offset;

	MSG(INIT, "nand pl ut start\n");
	memset(testbuff, 0x5A, 32768);
	memset(sourcebuff, 0xA5, 32768);

	offset = 100 * (devinfo.blocksize * 1024); //block 100
	mtk_nand_erase(offset);
	mtk_nand_write_page_hwecc(offset, sourcebuff);
	mtk_nand_read_page_hwecc(offset, testbuff);
	compare(testbuff, sourcebuff, 16384, "source and test buff check ");
	MSG(INIT, "nand pl ut end\n");
}
#endif
void dump_nfi(void)
{
	MSG(INIT, "~~~~Dump NFI Register in PL~~~~\n");
	MSG(INIT, "NFI_CNFG_REG16: 0x%x\n", DRV_Reg16(NFI_CNFG_REG16));
	MSG(INIT, "NFI_PAGEFMT_REG32: 0x%x\n", DRV_Reg32(NFI_PAGEFMT_REG32));
	MSG(INIT, "NFI_CON_REG16: 0x%x\n", DRV_Reg16(NFI_CON_REG16));
	MSG(INIT, "NFI_ACCCON_REG32: 0x%x\n", DRV_Reg32(NFI_ACCCON_REG32));
	MSG(INIT, "NFI_INTR_EN_REG16: 0x%x\n", DRV_Reg16(NFI_INTR_EN_REG16));
	MSG(INIT, "NFI_INTR_REG16: 0x%x\n", DRV_Reg16(NFI_INTR_REG16));
	MSG(INIT, "NFI_CMD_REG16: 0x%x\n", DRV_Reg16(NFI_CMD_REG16));
	MSG(INIT, "NFI_ADDRNOB_REG16: 0x%x\n", DRV_Reg16(NFI_ADDRNOB_REG16));
	MSG(INIT, "NFI_COLADDR_REG32: 0x%x\n", DRV_Reg32(NFI_COLADDR_REG32));
	MSG(INIT, "NFI_ROWADDR_REG32: 0x%x\n", DRV_Reg32(NFI_ROWADDR_REG32));
	MSG(INIT, "NFI_STRDATA_REG16: 0x%x\n", DRV_Reg16(NFI_STRDATA_REG16));
	MSG(INIT, "NFI_DATAW_REG32: 0x%x\n", DRV_Reg32(NFI_DATAW_REG32));
	MSG(INIT, "NFI_DATAR_REG32: 0x%x\n", DRV_Reg32(NFI_DATAR_REG32));
	MSG(INIT, "NFI_PIO_DIRDY_REG16: 0x%x\n", DRV_Reg16(NFI_PIO_DIRDY_REG16));
	MSG(INIT, "NFI_STA_REG32: 0x%x\n", DRV_Reg32(NFI_STA_REG32));
	MSG(INIT, "NFI_FIFOSTA_REG16: 0x%x\n", DRV_Reg16(NFI_FIFOSTA_REG16));
	/* MSG(INIT, "NFI_LOCKSTA_REG16: 0x%x\n", DRV_Reg16(NFI_LOCKSTA_REG16)); */
	MSG(INIT, "NFI_ADDRCNTR_REG16: 0x%x\n", DRV_Reg16(NFI_ADDRCNTR_REG16));
	MSG(INIT, "NFI_STRADDR_REG32: 0x%x\n", DRV_Reg32(NFI_STRADDR_REG32));
	MSG(INIT, "NFI_BYTELEN_REG16: 0x%x\n", DRV_Reg16(NFI_BYTELEN_REG16));
	MSG(INIT, "NFI_CSEL_REG16: 0x%x\n", DRV_Reg16(NFI_CSEL_REG16));
	MSG(INIT, "NFI_IOCON_REG16: 0x%x\n", DRV_Reg16(NFI_IOCON_REG16));
	MSG(INIT, "NFI_FDM0L_REG32: 0x%x\n", DRV_Reg32(NFI_FDM0L_REG32));
	MSG(INIT, "NFI_FDM0M_REG32: 0x%x\n", DRV_Reg32(NFI_FDM0M_REG32));
	MSG(INIT, "NFI_LOCK_REG16: 0x%x\n", DRV_Reg16(NFI_LOCK_REG16));
	MSG(INIT, "NFI_LOCKCON_REG32: 0x%x\n", DRV_Reg32(NFI_LOCKCON_REG32));
	MSG(INIT, "NFI_LOCKANOB_REG16: 0x%x\n", DRV_Reg16(NFI_LOCKANOB_REG16));
	MSG(INIT, "NFI_FIFODATA0_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA0_REG32));
	MSG(INIT, "NFI_FIFODATA1_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA1_REG32));
	MSG(INIT, "NFI_FIFODATA2_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA2_REG32));
	MSG(INIT, "NFI_FIFODATA3_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA3_REG32));
	MSG(INIT, "NFI_MASTERSTA_REG16: 0x%x\n", DRV_Reg16(NFI_MASTERSTA_REG16));
	MSG(INIT, "ECC_ENCCON_REG16	  :%x\n",*ECC_ENCCON_REG16	    );
	MSG(INIT, "ECC_ENCCNFG_REG32  	:%x\n",*ECC_ENCCNFG_REG32  	);
	MSG(INIT, "ECC_ENCDIADDR_REG32	:%x\n",*ECC_ENCDIADDR_REG32	);
	MSG(INIT, "ECC_ENCIDLE_REG32  	:%x\n",*ECC_ENCIDLE_REG32  	);
	MSG(INIT, "ECC_ENCPAR0_REG32   :%x\n",*ECC_ENCPAR0_REG32    );
	MSG(INIT, "ECC_ENCPAR1_REG32   :%x\n",*ECC_ENCPAR1_REG32    );
	MSG(INIT, "ECC_ENCPAR2_REG32   :%x\n",*ECC_ENCPAR2_REG32    );
	MSG(INIT, "ECC_ENCPAR3_REG32   :%x\n",*ECC_ENCPAR3_REG32    );
	MSG(INIT, "ECC_ENCPAR4_REG32   :%x\n",*ECC_ENCPAR4_REG32    );
	MSG(INIT, "ECC_ENCPAR5_REG32   :%x\n",*ECC_ENCPAR5_REG32    );
	MSG(INIT, "ECC_ENCPAR6_REG32   :%x\n",*ECC_ENCPAR6_REG32    );
	MSG(INIT, "ECC_ENCSTA_REG32    :%x\n",*ECC_ENCSTA_REG32     );
	MSG(INIT, "ECC_ENCIRQEN_REG16  :%x\n",*ECC_ENCIRQEN_REG16   );
	MSG(INIT, "ECC_ENCIRQSTA_REG16 :%x\n",*ECC_ENCIRQSTA_REG16  );
	MSG(INIT, "ECC_DECCON_REG16    :%x\n",*ECC_DECCON_REG16     );
	MSG(INIT, "ECC_DECCNFG_REG32   :%x\n",*ECC_DECCNFG_REG32    );
	MSG(INIT, "ECC_DECDIADDR_REG32 :%x\n",*ECC_DECDIADDR_REG32  );
	MSG(INIT, "ECC_DECIDLE_REG16   :%x\n",*ECC_DECIDLE_REG16    );
	MSG(INIT, "ECC_DECFER_REG16    :%x\n",*ECC_DECFER_REG16     );
	MSG(INIT, "ECC_DECENUM0_REG32  :%x\n",*ECC_DECENUM0_REG32   );
	MSG(INIT, "ECC_DECENUM1_REG32  :%x\n",*ECC_DECENUM1_REG32   );
	MSG(INIT, "ECC_DECDONE_REG16   :%x\n",*ECC_DECDONE_REG16    );
	MSG(INIT, "ECC_DECEL0_REG32    :%x\n",*ECC_DECEL0_REG32     );
	MSG(INIT, "ECC_DECEL1_REG32    :%x\n",*ECC_DECEL1_REG32     );
	MSG(INIT, "ECC_DECEL2_REG32    :%x\n",*ECC_DECEL2_REG32     );
	MSG(INIT, "ECC_DECEL3_REG32    :%x\n",*ECC_DECEL3_REG32     );
	MSG(INIT, "ECC_DECEL4_REG32    :%x\n",*ECC_DECEL4_REG32     );
	MSG(INIT, "ECC_DECEL5_REG32    :%x\n",*ECC_DECEL5_REG32     );
	MSG(INIT, "ECC_DECEL6_REG32    :%x\n",*ECC_DECEL6_REG32     );
	MSG(INIT, "ECC_DECEL7_REG32    :%x\n",*ECC_DECEL7_REG32     );
	MSG(INIT, "ECC_DECIRQEN_REG16  :%x\n",*ECC_DECIRQEN_REG16   );
	MSG(INIT, "ECC_DECIRQSTA_REG16 :%x\n",*ECC_DECIRQSTA_REG16  );
	MSG(INIT, "ECC_DECFSM_REG32    :%x\n",*ECC_DECFSM_REG32     );
	MSG(INIT, "ECC_BYPASS_REG32    :%x\n",*ECC_BYPASS_REG32     );
}

int mtk_nand_init(void)
{
	int i, j, busw;
	u8 id[NAND_MAX_ID];
	u16 spare_bit = 0;

	u16 spare_per_sector = 16;
	u32 ecc_bit = 4;
#if CFG_COMBO_NAND
	int bmt_sz = 0;
#endif
	u32 pagesize = g_nand_chip.oobblock;

	/* Config pin mux for NAND device */
	mtk_nand_gpio_init();

	nfi_buf = (unsigned char *)NAND_NFI_BUFFER;

	memset(&devinfo, 0, sizeof(devinfo));

	/* Dynamic Control */
	g_bInitDone = FALSE;
#ifndef REDUCE_NAND_PL_SIZE
	g_kCMD.u4OOBRowAddr = (u32) - 1;
#endif
#if CFG_FPGA_PLATFORM		/* FPGA NAND is placed at CS1 */
	DRV_WriteReg16(NFI_CSEL_REG16, 0);
#else
	DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
#endif


	/* Set default NFI access timing control */
	DRV_WriteReg32(NFI_ACCCON_REG32, NFI_DEFAULT_ACCESS_TIMING);

	DRV_WriteReg16(NFI_CNFG_REG16, 0);
	DRV_WriteReg32(NFI_PAGEFMT_REG32, 4);

	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	mtk_nand_reset();

	/* Read the first 4 byte to identify the NAND device */
	g_nand_chip.page_shift = NAND_LARGE_PAGE;
	g_nand_chip.page_size = 1 << g_nand_chip.page_shift;
	g_nand_chip.oobblock = NAND_PAGE_SIZE;
	g_nand_chip.oobsize = NAND_BLOCK_BLKS;
	g_nand_chip.nand_fdm_size = 8;

	g_nand_chip.nand_ecc_mode = NAND_ECC_HW;

	mtk_nand_command_bp(NAND_CMD_READID);

	for(i=0;i<NAND_MAX_ID;i++)
		id[i]=mtk_nand_read_byte ();
	nand_maf_id = id[0];
	nand_dev_id = id[1];
	memset(&devinfo, 0, sizeof(devinfo));

	if (!get_device_info(id, &devinfo)) {
		/* MSG(INIT, "NAND unsupport\n"); */
		ASSERT(0);
	}
#if CFG_2CS_NAND
	if (mtk_nand_cs_check(id, NFI_TRICKY_CS)) {
		MSG(INIT, "Twins Nand ");
		g_b2Die_CS = TRUE;
		g_bTricky_CS = TRUE;
	}
#endif
	g_nand_chip.name = devinfo.devciename;
#if defined(MTK_TLC_NAND_SUPPORT)
	g_nand_chip.chipsize = (u64)devinfo.totalsize << 10;
#endif
	/* g_nand_chip.chipsize = ((u64)devinfo.totalsize) << 20; */

#if CFG_2CS_NAND
	g_nanddie_pages = (u32)(g_nand_chip.chipsize / devinfo.pagesize);

	if(g_b2Die_CS) {
		g_nand_chip.chipsize <<= 1;
		MSG(INIT, "%dMB\n", (u32)(g_nand_chip.chipsize/1024/1024));
	}
	/* MSG(INIT, "[Bean]g_nanddie_pages %x\n", g_nanddie_pages); */
#endif
	g_nand_chip.page_size = devinfo.pagesize;
	g_nand_chip.page_shift = uffs(g_nand_chip.page_size) - 1;
	g_nand_chip.oobblock = g_nand_chip.page_size;
	if(devinfo.sectorsize == 512)
		g_nand_chip.erasesize = devinfo.blocksize << 10;
	else
		g_nand_chip.erasesize = (devinfo.blocksize << 10)/2;
#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& (devinfo.tlcControl.normaltlc))
		g_nand_chip.erasesize = (devinfo.blocksize << 10)/3;
#endif

	PAGE_SIZE = (u32) g_nand_chip.page_size;
	BLOCK_SIZE = (u32) devinfo.blocksize << 10;
	PAGES_PER_BLOCK = BLOCK_SIZE / PAGE_SIZE;
	g_nand_chip.phys_erase_shift = uffs(g_nand_chip.erasesize) - 1;
	g_nand_chip.sector_size = NAND_SECTOR_SIZE;
	g_nand_chip.sector_shift= 9;
	if(devinfo.sectorsize == 1024) {
		g_nand_chip.sector_size = 1024;
		g_nand_chip.sector_shift= 10;
		NFI_CLN_REG32(NFI_PAGEFMT_REG32, PAGEFMT_SECTOR_SEL);
	}

	g_nand_chip.bus16 = devinfo.iowidth;
#if 1
	if(devinfo.vendor == VEND_MICRON) {
		if(devinfo.feature_set.FeatureSet.Async_timing.feature != 0xFF) {
			struct gFeatureSet *feature_set = &(devinfo.feature_set.FeatureSet);
			mtk_nand_SetFeature((u16) feature_set->sfeatureCmd, \
			feature_set->Async_timing.address, (u8 *)&feature_set->Async_timing.feature,\
			sizeof(feature_set->Async_timing.feature));
#if CFG_2CS_NAND
			if (g_bTricky_CS) {
				DRV_WriteReg16(NFI_CSEL_REG16, NFI_TRICKY_CS);
				mtk_nand_SetFeature((u16) feature_set->sfeatureCmd, \
				feature_set->Async_timing.address, (u8 *)&feature_set->Async_timing.feature,\
				sizeof(feature_set->Async_timing.feature));
				DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
			}
#endif
		}
	}
#endif
	DRV_WriteReg32(NFI_ACCCON_REG32, devinfo.timmingsetting);
	/* MSG(INIT, "[NAND]Timing: 0x%X\n", DRV_Reg32(NFI_ACCCON_REG32)); */
	if (!devinfo.sparesize)
		g_nand_chip.oobsize = (8 << ((ext_id2 >> 2) & 0x01))\
			* (g_nand_chip.oobblock / g_nand_chip.sector_size);
	else
		g_nand_chip.oobsize = devinfo.sparesize;
	spare_per_sector = g_nand_chip.oobsize / (g_nand_chip.page_size / g_nand_chip.sector_size);

	switch(spare_per_sector)
	{
	#ifndef MTK_COMBO_NAND_SUPPORT
	case 16:
		spare_bit = PAGEFMT_SPARE_16;
		ecc_bit = 4;
		spare_per_sector = 16;
		break;
	case 26:
	case 27:
	case 28:
		spare_bit = PAGEFMT_SPARE_26;
		ecc_bit = 10;
		spare_per_sector = 26;
		break;
	case 32:
		ecc_bit = 12;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_32_1KS;
		else
			spare_bit = PAGEFMT_SPARE_32;
		spare_per_sector = 32;
		break;
	case 40:
		ecc_bit = 18;
		spare_bit = PAGEFMT_SPARE_40;
		spare_per_sector = 40;
		break;
	case 44:
		ecc_bit = 20;
		spare_bit = PAGEFMT_SPARE_44;
		spare_per_sector = 44;
		break;
	case 48:
	case 49:
	ecc_bit = 22;
		spare_bit = PAGEFMT_SPARE_48;
		spare_per_sector = 48;
		break;
	case 50:
	case 51:
		ecc_bit = 24;
		spare_bit = PAGEFMT_SPARE_50;
		spare_per_sector = 50;
		break;
	case 52:
	case 54:
	case 56:
		ecc_bit = 24;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_52_1KS;
		else
			spare_bit = PAGEFMT_SPARE_52;
		spare_per_sector = 32;
		break;
	#endif
	case 62:
	case 63:
		ecc_bit = 28;
		spare_bit = PAGEFMT_SPARE_62;
		spare_per_sector = 62;
		break;
	case 64:
		ecc_bit = 32;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_64_1KS;
		else
			spare_bit = PAGEFMT_SPARE_64;
		spare_per_sector = 64;
		break;
	case 72:
		ecc_bit = 36;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_72_1KS;
		spare_per_sector = 72;
		break;
	case 80:
		ecc_bit = 40;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_80_1KS;
		spare_per_sector = 80;
		break;
	case 88:
		ecc_bit = 44;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_88_1KS;
		spare_per_sector = 88;
		break;
	case 96:
	case 98:
		ecc_bit = 48;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_96_1KS;
		spare_per_sector = 96;
		break;
	case 100:
	case 102:
	case 104:
		ecc_bit = 52;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_100_1KS;
		spare_per_sector = 100;
		break;
	case 122:
	case 124:
	case 126:
		#if defined(MTK_TLC_NAND_SUPPORT)
		if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			&& devinfo.tlcControl.ecc_recalculate_en) {
		    if(60 < devinfo.tlcControl.ecc_required) { /* 68,72,80 */
			g_nand_chip.nand_fdm_size = 3; /* 122 - 119 */
			ecc_bit = 68;
		    }
		    else
		        ecc_bit = 60;
		}
		else
	#endif
			ecc_bit = 60;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_122_1KS;
		spare_per_sector = 122;
		break;
	case 128:
	#if defined(MTK_TLC_NAND_SUPPORT)
		if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			&& devinfo.tlcControl.ecc_recalculate_en) {
			if(68 < devinfo.tlcControl.ecc_required) { /* 72,80 */
				g_nand_chip.nand_fdm_size = 2; /* 128 - 126; 126 = 72*14/8 */
			        ecc_bit = 72;
			} else
			        ecc_bit = 68;
		}
		else
	#endif
			ecc_bit = 68;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_128_1KS;
		spare_per_sector = 128;
		break;
	#if defined(MTK_TLC_NAND_SUPPORT)
	case 134:
		ecc_bit = 72;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_134_1KS;
		spare_per_sector = 134;
		break;
	case 148:
		ecc_bit = 80;
		if(devinfo.sectorsize == 1024)
			spare_bit = PAGEFMT_SPARE_148_1KS;
		spare_per_sector = 148;
		break;
	#endif
	default:
		MSG(INIT, "[NAND]: NFI not support oobsize: %x\n", spare_per_sector);
		ASSERT(0);
	}

	g_nand_chip.oobsize = spare_per_sector * (g_nand_chip.page_size / g_nand_chip.sector_size);
	/* MSG(INIT, "[NAND]: oobsize: %x\n", g_nand_chip.oobsize); */

	#ifdef MNTL_SUPPORT
	bmt_sz = (int)(g_nand_chip.chipsize/BLOCK_SIZE/100);
	#else
	bmt_sz = (int)(g_nand_chip.chipsize/BLOCK_SIZE/100*6);
	#endif

	MSG(INIT, "[NAND]: g_nand_chip.chipsize:%llx, BLOCK_SIZE:%x, bmt_sz:%d\n",
		g_nand_chip.chipsize, BLOCK_SIZE, bmt_sz);

	g_nand_chip.chipsize -= (bmt_sz*BLOCK_SIZE);

	if (g_nand_chip.bus16 == NAND_BUS_WIDTH_16) {
#ifdef  DBG_PRELOADER
		MSG(INIT, "USE 16 IO\n");
#endif
		NFI_SET_REG32(NFI_PAGEFMT_REG32, PAGEFMT_DBYTE_EN);
	}

	pagesize = g_nand_chip.oobblock;
	#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	&& (devinfo.tlcControl.normaltlc)
	&& (devinfo.two_phyplane))
		pagesize >>= 1;
	else
	#endif
		if(devinfo.two_phyplane)
			pagesize >>= 1;

	if (16384 == pagesize) {
		NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_16K_1KS);
		nand_oob = &nand_oob_128;
	} else if (8192 == pagesize) {
		NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_8K_1KS);
		nand_oob = &nand_oob_128;
	}
	#ifndef REDUCE_NAND_PL_SIZE
	else if (4096 == pagesize) {
		if(devinfo.sectorsize == 512)
			NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_4K);
		else
			NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_4K_1KS);
		nand_oob = &nand_oob_128;
	} else if (2048 == pagesize) {
		if(devinfo.sectorsize == 512)
			NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_2K);
		else
			NFI_SET_REG32(NFI_PAGEFMT_REG32, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_2K_1KS);
		nand_oob = &nand_oob_64;
	}
	#endif

	if (g_nand_chip.nand_ecc_mode == NAND_ECC_HW) {
		/* MSG (INIT, "Use HW ECC\n"); */
		NFI_SET_REG32(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		ECC_Config(ecc_bit);
		mtk_nand_configure_fdm(g_nand_chip.nand_fdm_size);
	}

	/* Initilize interrupt. Clear interrupt, read clear. */
	DRV_Reg16(NFI_INTR_REG16);
	#if CFG_RANDOMIZER
	if(devinfo.vendor != VEND_NONE)	{
	#if 0
		if ((devinfo.feature_set.randConfig.type == RAND_TYPE_SAMSUNG) ||
		(devinfo.feature_set.randConfig.type == RAND_TYPE_TOSHIBA)) {
			MSG(INIT, "[NAND]USE Randomizer\n");
			use_randomizer = TRUE;
		} else {
			MSG(INIT, "[NAND]OFF Randomizer\n");
			use_randomizer = FALSE;
		}
	#endif
		if((*EFUSE_RANDOM_CFG)&EFUSE_RANDOM_ENABLE) {
			MSG(INIT, "[NFI]RANDOM ON\n");
			use_randomizer = TRUE;
			pre_randomizer = TRUE;
		} else {
			MSG(INIT, "[NFI]RANDOM OFF\n");
			use_randomizer = FALSE;
			pre_randomizer = FALSE;
		}

	#if defined(MTK_TLC_NAND_SUPPORT)
		if(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
			MSG(INIT, "[NFI]tlc force RANDOM ON\n");
			use_randomizer = TRUE;
			pre_randomizer = TRUE;
		}
	#endif
	}
	#endif

	if((devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
	|| (devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX)
	|| (devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE))
		HYNIX_RR_TABLE_READ(&devinfo);

	/* Interrupt arise when read data or program data to/from AHB is done. */
	DRV_WriteReg16(NFI_INTR_EN_REG16, 0);

	#ifdef NAND_PL_UT
	nand_pl_ut();
	#endif

	if (!(init_bmt(&g_nand_chip, bmt_sz))) {
		MSG(INIT, "Error: init bmt failed: bmt_sz:%d\n", bmt_sz);
		ASSERT(0);
		return 0;
	}
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		g_nand_chip.chipsize -= (devinfo.blocksize * 1024) * (PMT_POOL_SIZE);
	else
		g_nand_chip.chipsize -=
			(g_nand_chip.sector_size == 512?\
			g_nand_chip.erasesize : g_nand_chip.erasesize*2) * (PMT_POOL_SIZE);
	return 0;
}

//-----------------------------------------------------------------------------
static void mtk_nand_stop_read(void)
{
	NFI_CLN_REG32(NFI_CON_REG16, CON_NFI_BRD);
	if (g_bHwEcc)
		ECC_Decode_End();
}

//-----------------------------------------------------------------------------
static void mtk_nand_stop_write(void)
{
	NFI_CLN_REG32(NFI_CON_REG16, CON_NFI_BWR);
	if (g_bHwEcc)
		ECC_Encode_End();
}

static bool mtk_nand_check_dececc_done(u32 u4SecNum)
{
	u32 timeout, dec_mask;
	timeout = 0xffffff;
	dec_mask = (1 << (u4SecNum - 1));
	while (dec_mask != (DRV_Reg(ECC_DECDONE_REG16) & dec_mask)) {
		if (timeout == 0) {
			MSG(INIT, "ECC_DECDONE_REG16 timeout 0x%x %d\n",DRV_Reg(ECC_DECDONE_REG16),u4SecNum);
			return false;
		}
		timeout--;
	}
	timeout = 0xffffff;
	while ((DRV_Reg32(ECC_DECFSM_REG32) & 0x3F3FFF0F)!= ECC_DECFSM_IDLE) {
		if (timeout == 0) {
			MSG(INIT, "ECC_DECFSM_REG32 timeout 0x%x 0x%x %d\n",
				DRV_Reg32(ECC_DECFSM_REG32),DRV_Reg(ECC_DECDONE_REG16),u4SecNum);
			return false;
		}
		timeout--;
	}
	return true;
}

static bool mtk_nand_read_page_data(u32 * buf)
{
	u32 timeout = 0xFFFF;
	u32 u4Size = g_nand_chip.oobblock;
	u32 i;
	u32 *pBuf32;

	#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& (devinfo.tlcControl.normaltlc)) {
		if(devinfo.tlcControl.pPlaneEn)
			u4Size /= 2;
		if(devinfo.two_phyplane)
			u4Size /= 2;
	} else
	#endif
	if(devinfo.two_phyplane)
		u4Size /= 2;
#if (USE_AHB_MODE)
	pBuf32 = (u32 *) buf;
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	DRV_Reg16(NFI_INTR_REG16);
	DRV_WriteReg16(NFI_INTR_EN_REG16, INTR_AHB_DONE_EN);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BRD);

	#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	   &&(devinfo.tlcControl.needchangecolumn || devinfo.two_phyplane))
		DRV_WriteReg(NFI_TLC_RD_WHR2_REG16, (TLC_RD_WHR2_EN | 0x055)); /* trigger data sample */
	#endif

	while (!(DRV_Reg16(NFI_INTR_REG16) & INTR_AHB_DONE)) {
		timeout--;
		if (0 == timeout)
			return FALSE;
	}

	timeout = 0xFFFF;
	while ((u4Size >> g_nand_chip.sector_shift) > ((DRV_Reg32(NFI_BYTELEN_REG16) & 0x1f000) >> 12)) {
		timeout--;
		if (0 == timeout)
			return FALSE;
	}

#else
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BRD);

	#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	   &&(devinfo.tlcControl.needchangecolumn || devinfo.two_phyplane))
		DRV_WriteReg(NFI_TLC_RD_WHR2_REG16, (TLC_RD_WHR2_EN | 0x055)); /* trigger data sample */
	#endif
	pBuf32 = (u32 *) buf;

	for (i = 0; (i < (u4Size >> 2)) && (timeout > 0);) {
		if (DRV_Reg16(NFI_PIO_DIRDY_REG16) & 1)	{
			*pBuf32++ = DRV_Reg32(NFI_DATAR_REG32);
			i++;
		} else
			timeout--;
		if (0 == timeout)
			return FALSE;
	}
#endif
	return TRUE;
}

static bool mtk_nand_write_page_data(u32 * buf)
{
	u32 timeout = 0xFFFF;
	u32 u4Size = g_nand_chip.oobblock;

	#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& (devinfo.tlcControl.normaltlc)) {
		if(devinfo.tlcControl.pPlaneEn)
			u4Size /= 2;
		if(devinfo.two_phyplane)
			u4Size /= 2;
	} else
	#endif
		if(devinfo.two_phyplane)
			u4Size /= 2;
#if (USE_AHB_MODE)
	u32 *pBuf32;
	pBuf32 = (u32 *) buf;

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	DRV_Reg16(NFI_INTR_REG16);
	DRV_WriteReg16(NFI_INTR_EN_REG16, INTR_AHB_DONE_EN);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);
	while (!(DRV_Reg16(NFI_INTR_REG16) & INTR_AHB_DONE)) {
		timeout--;
		if (0 == timeout)
			return FALSE;
	}

#else
	u32 i;
	u32 *pBuf32;
	pBuf32 = (u32 *) buf;

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	NFI_SET_REG32(NFI_CON_REG16, CON_NFI_BWR);

	for (i = 0; (i < (u4Size >> 2)) && (timeout > 0);) {
		if (DRV_Reg16(NFI_PIO_DIRDY_REG16) & 1) {
			DRV_WriteReg32(NFI_DATAW_REG32, *pBuf32++);
			i++;
		} else
			timeout--;

		if (0 == timeout)
			return FALSE;
	}
#endif
    return TRUE;
}

static void mtk_nand_read_fdm_data(u32 u4SecNum, u8 * spare_buf)
{
	u32 *pBuf32 = (u32 *) spare_buf;
	u32 fdm_temp[2];
	u32 i, j;
	u8 *byte_ptr;

	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		byte_ptr = (u8*)fdm_temp;

		if(spare_buf) {
			for (i = 0; i < u4SecNum; ++i) {
				fdm_temp[0] = DRV_Reg32(NFI_FDM0L_REG32 + (i << 3));
				fdm_temp[1] = DRV_Reg32(NFI_FDM0M_REG32 + (i << 3));
				for(j = 0; j < g_nand_chip.nand_fdm_size; j++) {
					*(spare_buf + (i * g_nand_chip.nand_fdm_size) + j) = *(byte_ptr + j);
				}
			}
		}
	} else {
		for (i = 0; i < u4SecNum; ++i) {
			*pBuf32++ = DRV_Reg32(NFI_FDM0L_REG32 + (i << 3));
			*pBuf32++ = DRV_Reg32(NFI_FDM0M_REG32 + (i << 3));
		}
	}

}

//-----------------------------------------------------------------------------
static void mtk_nand_write_fdm_data(u32 u4SecNum, u8 * oob)
{
	u32 *pBuf32 = (u32 *) oob;
	u8 *pBuf;
	u8* byte_ptr;
	u32 fdm_data[2];
	u32 i, j;

	pBuf = (u8*)fdm_data;
	byte_ptr = (u8*)oob;
	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		for (i = 0; i < u4SecNum; ++i) {
			fdm_data[0] = 0xFFFFFFFF;
			fdm_data[1] = 0xFFFFFFFF;

			for (j = 0; j < g_nand_chip.nand_fdm_size; j++) {
				*(pBuf + j) = *(byte_ptr + j + (i * g_nand_chip.nand_fdm_size));
			}

			DRV_WriteReg32(NFI_FDM0L_REG32 + (i << 3), fdm_data[0]);
			DRV_WriteReg32(NFI_FDM0M_REG32 + (i << 3), fdm_data[1]);
		}
	} else {
		for (i = 0; i < u4SecNum; ++i) {
			DRV_WriteReg32(NFI_FDM0L_REG32 + (i << 3), *pBuf32++);
			DRV_WriteReg32(NFI_FDM0M_REG32 + (i << 3), *pBuf32++);
		}
	}
}

//---------------------------------------------------------------------------
static bool mtk_nand_ready_for_read(u32 page_addr, u32 sec_num, u8 * buf)
{
	u32 u4RowAddr = page_addr;
	u32 colnob = 2;
	u32 rownob = devinfo.addr_cycle - colnob;
	bool bRet = FALSE;
	u16 read_cmd;

	if (!mtk_nand_reset())
		goto cleanup;

	/* Enable HW ECC */
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

	mtk_nand_set_mode(CNFG_OP_READ);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
		DRV_WriteReg32(NFI_CON_REG16, sec_num << CON_NFI_SEC_SHIFT);

#if USE_AHB_MODE
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
#else
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
#endif
	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC) {
		DRV_WriteReg32(NFI_STRADDR_REG32, buf);
		if (g_bHwEcc)
			NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		else
			NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	}

	mtk_nand_set_autoformat(TRUE);
	if (g_bHwEcc)
		ECC_Decode_Start();

	if (!mtk_nand_set_command(NAND_CMD_READ0))
		goto cleanup;

	if (!mtk_nand_set_address(0, u4RowAddr, colnob, rownob))
		goto cleanup;

	read_cmd = NAND_CMD_READSTART;
#ifdef MTK_TLC_NAND_SUPPORT
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& (devinfo.tlcControl.normaltlc)
		&& devinfo.two_phyplane
		&& (!tlc_snd_phyplane))
		read_cmd = MULTI_PLANE_READ_CMD;
#endif

	if (!mtk_nand_set_command(read_cmd))
		goto cleanup;

	if (!mtk_nand_status_ready(STA_NAND_BUSY))
		goto cleanup;

	bRet = TRUE;

cleanup:
	return bRet;
}

//-----------------------------------------------------------------------------
static bool mtk_nand_ready_for_write(u32 page_addr, u32 sec_num, u8 * buf)
{
	bool bRet = FALSE;
	u32 u4RowAddr = page_addr;
	u32 colnob = 2;
	u32 rownob = devinfo.addr_cycle - colnob;
	u16 prg_cmd;

	if (!mtk_nand_reset())
		return FALSE;

	mtk_nand_set_mode(CNFG_OP_PRGM);

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_READ_EN);

	DRV_WriteReg32(NFI_CON_REG16, sec_num << CON_NFI_SEC_SHIFT);

#if USE_AHB_MODE
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	DRV_WriteReg32(NFI_STRADDR_REG32, buf);
#else
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
#endif

	if (g_bHwEcc)
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	else
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

	mtk_nand_set_autoformat(TRUE);
	if (g_bHwEcc)
		ECC_Encode_Start();

	prg_cmd = NAND_CMD_SEQIN;
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER)\
		&& devinfo.two_phyplane && tlc_snd_phyplane)
		prg_cmd = PLANE_INPUT_DATA_CMD;
	if (!mtk_nand_set_command(prg_cmd))
		goto cleanup;

	if (!mtk_nand_set_address(0, u4RowAddr, colnob, rownob))
		goto cleanup;

	if (!mtk_nand_status_ready(STA_NAND_BUSY))
		goto cleanup;

	bRet = TRUE;
cleanup:

	return bRet;
}

/*#############################################################################
# NAND Driver : Page Read
#
# NAND Page Format (Large Page 2KB)
#  |------ Page:2048 Bytes ----->>||---- Spare:64 Bytes -->>|
#
# Parameter Description:
#     page_addr               : specify the starting page in NAND flash
#
#############################################################################*/
int mtk_nand_read_page_hwecc(u64 logical_addr, char *buf)
{
	int i, start, len, offset = 0;
	u32 page_no;
	u32 block, mapped_block;
	int rtn = ERR_RTN_SUCCESS;
	u8 *oob = buf + g_nand_chip.page_size;

	page_no = mtk_nand_page_transform(logical_addr,&block,&mapped_block);

	#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& (devinfo.tlcControl.normaltlc)
		&& (devinfo.two_phyplane)) {
		mapped_block <<= 1;
		page_no = mapped_block * (BLOCK_SIZE/PAGE_SIZE) + (page_no % (BLOCK_SIZE/PAGE_SIZE));
	} else
	#endif
		if(devinfo.two_phyplane) {
			mapped_block <<= 1;
			page_no = mapped_block * (BLOCK_SIZE/PAGE_SIZE) + (page_no % (BLOCK_SIZE/PAGE_SIZE));
		}

	rtn = mtk_nand_read_page_hw(page_no, buf, g_nand_spare);
	if (rtn != ERR_RTN_SUCCESS)
		return FALSE;

	for (i = 0; i < MTD_MAX_OOBFREE_ENTRIES && nand_oob->oobfree[i].length; i++) {
		/* Set the reserved bytes to 0xff */
		start = nand_oob->oobfree[i].offset;
		len = nand_oob->oobfree[i].length;
		memcpy(oob + offset, g_nand_spare + start, len);
		offset += len;
	}

	return true;
}

int mtk_nand_read_page_hw(u32 page, u8 * dat, u8 * oob)
{
	int bRet = ERR_RTN_SUCCESS;
	u8 *pPageBuf;
	u32 u4SecNum = g_nand_chip.oobblock >> g_nand_chip.sector_shift;
	pPageBuf = (u8 *) dat;
	bool readRetry = FALSE;
	int retryCount = 0;
	u32 reg_val = 0;
#if defined(MTK_TLC_NAND_SUPPORT)
	NFI_TLC_WL_INFO  tlc_wl_info;
	bool   tlc_left_plane = TRUE;
	int spare_per_sector = g_nand_chip.oobsize/u4SecNum;
	u32 snd_real_row_addr = 0;
	NFI_TLC_WL_INFO  snd_tlc_wl_info;
#endif
	u32 block_addr = 0;
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	u32 real_row_addr = 0;
	u32 retrytotalcnt = devinfo.feature_set.FeatureSet.readRetryCnt;
	u32   logical_plane_num = 1;
	u32   data_sector_num = 0;
	u8    *temp_byte_ptr = NULL;
	u8    *spare_ptr = NULL;
	bool ready_to_read = TRUE;
	u32 page_in_block = 0;

#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& devinfo.tlcControl.normaltlc)
	    devinfo.tlcControl.slcopmodeEn = TRUE;
#endif
#if MLC_MICRON_SLC_MODE
	u8 feature[4];
#endif

#if CFG_2CS_NAND
	if (g_bTricky_CS)
		page = mtk_nand_cs_on(NFI_TRICKY_CS, page);
#endif
	do {
		data_sector_num = u4SecNum;
	    	temp_byte_ptr = dat;
		spare_ptr = oob;
		logical_plane_num = 1;

#if defined(MTK_TLC_NAND_SUPPORT)
		if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
			if(devinfo.tlcControl.normaltlc) {
				NFI_TLC_GetMappedWL(page, &tlc_wl_info);
				real_row_addr = NFI_TLC_GetRowAddr(tlc_wl_info.word_line_idx);

				if(devinfo.tlcControl.pPlaneEn) {
					tlc_left_plane = TRUE; /*  begin at left logical plane */
					logical_plane_num = 2;
					data_sector_num /= 2;
					real_row_addr = NFI_TLC_SetpPlaneAddr(real_row_addr, tlc_left_plane);
				}

				if(devinfo.two_phyplane) {
					NFI_TLC_GetMappedWL(page + page_per_block, &snd_tlc_wl_info);
					snd_real_row_addr = NFI_TLC_GetRowAddr(snd_tlc_wl_info.word_line_idx);

					logical_plane_num *= 2;
					data_sector_num /= 2;

					if(devinfo.tlcControl.pPlaneEn) {
						tlc_left_plane = TRUE; /* begin at left logical plane */
						snd_real_row_addr = NFI_TLC_SetpPlaneAddr(snd_real_row_addr, tlc_left_plane);
					}
				}
			} else
				real_row_addr = NFI_TLC_GetRowAddr(page);

			if( devinfo.tlcControl.slcopmodeEn) {
				if (0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);

					mtk_nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);

					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
				}
			} else {
				if(devinfo.tlcControl.normaltlc) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);

					if(tlc_wl_info.wl_pre == WL_LOW_PAGE)
						mtk_nand_set_command(LOW_PG_SELECT_CMD);
					else if(tlc_wl_info.wl_pre == WL_MID_PAGE)
						mtk_nand_set_command(MID_PG_SELECT_CMD);
					else if(tlc_wl_info.wl_pre == WL_HIGH_PAGE)
						mtk_nand_set_command(HIGH_PG_SELECT_CMD);

					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
				}
			}
			reg_val = 0;
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
				} else if(0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);
					mtk_nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);
					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
					if (devinfo.vendor == VEND_SANDISK) {
						block_addr = real_row_addr/page_per_block;
						page_in_block = real_row_addr % page_per_block;
						page_in_block <<= 1;
						real_row_addr = page_in_block + block_addr * page_per_block;
						/*printk("mtk_nand_exec_read_page SLC Mode real_row_addr:%d, u4RowAddr:%d\n",
							real_row_addr, u4RowAddr);*/
					}
				}
			}
			real_row_addr = page;
		}

		if(use_randomizer && page >= RAND_START_ADDR)
			mtk_nand_turn_on_randomizer(page, 0, 0);
		else if(pre_randomizer && page < RAND_START_ADDR)
			mtk_nand_turn_on_randomizer(page, 0, 0);


#if defined(MTK_TLC_NAND_SUPPORT)
		if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			tlc_snd_phyplane = FALSE;
#endif

		ready_to_read = mtk_nand_ready_for_read(real_row_addr, data_sector_num, pPageBuf);
#if defined(MTK_TLC_NAND_SUPPORT)
		if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			&& (devinfo.tlcControl.normaltlc)
			&& devinfo.two_phyplane) {
			if(!devinfo.tlcControl.slcopmodeEn) {
				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				if(tlc_wl_info.wl_pre == WL_LOW_PAGE)
					mtk_nand_set_command(LOW_PG_SELECT_CMD);
				else if(tlc_wl_info.wl_pre == WL_MID_PAGE)
					mtk_nand_set_command(MID_PG_SELECT_CMD);
				else if(tlc_wl_info.wl_pre == WL_HIGH_PAGE)
					mtk_nand_set_command(HIGH_PG_SELECT_CMD);

				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);
			}

			tlc_snd_phyplane = TRUE;
			ready_to_read = mtk_nand_ready_for_read(snd_real_row_addr, data_sector_num, pPageBuf);
		}
		#endif

		if (ready_to_read) {
#if defined(MTK_TLC_NAND_SUPPORT)
			if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
				tlc_snd_phyplane = FALSE;
#endif
			while(logical_plane_num) {
#if defined(MTK_TLC_NAND_SUPPORT)
				if(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
					if(devinfo.tlcControl.needchangecolumn || devinfo.two_phyplane) {
				    		/* change colunm address */
						if(devinfo.tlcControl.pPlaneEn) {
							if(tlc_snd_phyplane)
								snd_real_row_addr = NFI_TLC_SetpPlaneAddr(snd_real_row_addr, tlc_left_plane);
							else
								real_row_addr = NFI_TLC_SetpPlaneAddr(real_row_addr, tlc_left_plane);
						}
						#if 1 /* reset here to flush fifo status left by the left plane read */
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

						mtk_nand_set_command(CHANGE_COLUNM_ADDR_1ST_CMD);
						if(tlc_snd_phyplane)
							mtk_nand_set_address(0, snd_real_row_addr, 2, devinfo.addr_cycle - 2);
						else
							mtk_nand_set_address(0, real_row_addr, 2, devinfo.addr_cycle - 2);
						mtk_nand_set_command(CHANGE_COLUNM_ADDR_2ND_CMD);

						reg_val = DRV_Reg(NFI_CNFG_REG16);
						reg_val |= CNFG_READ_EN;
						reg_val &= ~CNFG_OP_MODE_MASK;
						reg_val |= CNFG_OP_READ;
						DRV_WriteReg(NFI_CNFG_REG16, reg_val);
					}

					DRV_WriteReg32(NFI_STRADDR_REG32, temp_byte_ptr);
					DRV_WriteReg32(NFI_CON_REG16, data_sector_num << CON_NFI_SEC_SHIFT);

					if (g_bHwEcc)
						ECC_Decode_Start();
				}



				#endif
				if (!mtk_nand_read_page_data((u32 *) temp_byte_ptr))
					bRet = ERR_RTN_FAIL;

				if (!mtk_nand_status_ready(STA_NAND_BUSY))
					bRet = ERR_RTN_FAIL;

				if (g_bHwEcc) {
					if (!mtk_nand_check_dececc_done(data_sector_num)) {
						dump_nfi();
						bRet = ERR_RTN_FAIL;
					}
				}
				mtk_nand_read_fdm_data(data_sector_num, spare_ptr);
				if (g_bHwEcc) {
					if (!mtk_nand_check_bch_error(temp_byte_ptr, data_sector_num - 1, page)) {
						MSG(ERASE, "check bch error !\n");
						if(devinfo.vendor != VEND_NONE)
							readRetry = TRUE;
						bRet = ERR_RTN_BCH_FAIL;
					}
				}
				if(0 != (DRV_Reg32(NFI_STA_REG32) & STA_READ_EMPTY)) {
					if (retryCount != 0) {
						MSG(INIT, "RR empty page return uncorrectable!\n");
						bRet = ERR_RTN_BCH_FAIL;
					}else {
						memset(pPageBuf, 0xFF,g_nand_chip.page_size);
						memset(oob, 0xFF,g_nand_chip.nand_fdm_size * u4SecNum);
						readRetry = FALSE;
						bRet = ERR_RTN_SUCCESS;
					}
				}
				mtk_nand_stop_read();
			#if defined(MTK_TLC_NAND_SUPPORT)
				if(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
					if(devinfo.tlcControl.needchangecolumn || devinfo.two_phyplane)
						DRV_WriteReg(NFI_TLC_RD_WHR2_REG16, 0x055); /* disable */

					if(devinfo.two_phyplane) {
						if(4 == logical_plane_num)
							tlc_left_plane = FALSE;

						else if(3 == logical_plane_num) {
							tlc_left_plane = TRUE;
							tlc_snd_phyplane = TRUE;
						}
					}
					if(2 == logical_plane_num) {
						tlc_left_plane = FALSE;
						if(!devinfo.tlcControl.pPlaneEn) {
							tlc_left_plane = TRUE;
							tlc_snd_phyplane = TRUE;
						}
			 		}

					spare_ptr += (g_nand_chip.nand_fdm_size * data_sector_num);
					temp_byte_ptr += (data_sector_num *(1 << g_nand_chip.sector_shift));
				}
			#endif
				logical_plane_num --;

				if(bRet == ERR_RTN_BCH_FAIL)
				break;
			}
		}

	    	if(use_randomizer && page >= RAND_START_ADDR)
			mtk_nand_turn_off_randomizer();
		else if(pre_randomizer && page < RAND_START_ADDR)
			mtk_nand_turn_off_randomizer();

		#if defined(MTK_TLC_NAND_SUPPORT)
		if(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
			if((devinfo.tlcControl.slcopmodeEn)
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

				mtk_nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
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

					mtk_nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
				}
			}
		}
		if (bRet == ERR_RTN_BCH_FAIL) {
			u32 feature = mtk_nand_rrtry_setting(devinfo, devinfo.feature_set.FeatureSet.rtype,devinfo.feature_set.FeatureSet.readRetryStart,retryCount);
			#if defined(MTK_TLC_NAND_SUPPORT)
			if((devinfo.feature_set.FeatureSet.rtype == RTYPE_SANDISK_TLC_1YNM)
				&& (devinfo.tlcControl.slcopmodeEn))
				retrytotalcnt = 10;
			else if((devinfo.feature_set.FeatureSet.rtype == RTYPE_SANDISK_TLC_1ZNM)
                                && (devinfo.tlcControl.slcopmodeEn))
                                retrytotalcnt = 24;
			else if((devinfo.feature_set.FeatureSet.rtype == RTYPE_TOSHIBA_TLC_A19NM)
				&& (devinfo.tlcControl.slcopmodeEn))
				retrytotalcnt = 5;
			#endif
			/* for sandisk 1znm slc mode read retry */
			if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC) {
				if ((devinfo.feature_set.FeatureSet.rtype == RTYPE_SANDISK)
					&& (devinfo.tlcControl.slcopmodeEn))
					retrytotalcnt = 25;
			}
			if(retryCount < retrytotalcnt) {
				mtk_nand_rrtry_func(devinfo,feature,FALSE);
				retryCount++;
			} else {
				feature = devinfo.feature_set.FeatureSet.readRetryDefault;
				#if defined(MTK_TLC_NAND_SUPPORT)
				if((devinfo.feature_set.FeatureSet.rtype == RTYPE_TOSHIBA_TLC_A19NM)
					&& (devinfo.tlcControl.slcopmodeEn))
					feature = 5;
				#endif
				if((devinfo.feature_set.FeatureSet.rtype == RTYPE_SANDISK) && (g_sandisk_retry_case < 2)) {
					g_sandisk_retry_case++;
					mtk_nand_rrtry_func(devinfo,feature,FALSE);
					retryCount = 0;
				} else {
					mtk_nand_rrtry_func(devinfo,feature,TRUE);
					readRetry = FALSE;
					g_sandisk_retry_case = 0;
				}
			}
			if(g_sandisk_retry_case == 1)
		                mtk_nand_set_command(0x26);

		} else {
			if(retryCount != 0) {
				u32 feature = devinfo.feature_set.FeatureSet.readRetryDefault;
				#if defined(MTK_TLC_NAND_SUPPORT)
				if((devinfo.feature_set.FeatureSet.rtype == RTYPE_TOSHIBA_TLC_A19NM)
					&& (devinfo.tlcControl.slcopmodeEn))
					feature = 5;
				#endif
				mtk_nand_rrtry_func(devinfo,feature,TRUE);
			}
			readRetry = FALSE;
			g_sandisk_retry_case = 0;
		}
		if(TRUE == readRetry)
			bRet = ERR_RTN_SUCCESS;
	}while(readRetry);

	if(retryCount != 0) {
		u32 feature = devinfo.feature_set.FeatureSet.readRetryDefault;
		if(bRet == ERR_RTN_SUCCESS) {
			MSG(INIT, "u4RowAddr:0x%x read retry pass, retrycnt:%d ENUM0:%x,ENUM1:%x \n",\
				page,retryCount,DRV_Reg32(ECC_DECENUM1_REG32),DRV_Reg32(ECC_DECENUM0_REG32));
			if((devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX_16NM)
				|| (devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX)
				|| (devinfo.feature_set.FeatureSet.rtype == RTYPE_HYNIX_FDIE))
				g_hynix_retry_count--;
		} else
			MSG(INIT, "u4RowAddr:0x%x read retry fail\n",page);

		if(devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
			mtk_nand_rrtry_func(devinfo,feature,TRUE);
		g_sandisk_retry_case = 0;
	}

	return bRet;
}


/* #############################################################################*/
/* # NAND Driver : Page Write							*/
/* #										*/
/* # NAND Page Format (Large Page 2KB)						*/
/* #  |------ Page:2048 Bytes ----->>||---- Spare:64 Bytes -->>|		*/
/* #										*/
/* # Parameter Description:							*/
/* #     page_addr               : specify the starting page in NAND flash	*/
/* #										*/
/* #############################################################################*/

int mtk_nand_write_page_hwecc(u64 logical_addr, char *buf)
{
	u32 block,mapped_block;
	u32 page_no;
	u8 *oob = buf + g_nand_chip.oobblock;
	int i;
	int start, len, offset;
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;

	page_no = mtk_nand_page_transform(logical_addr ,&block, &mapped_block);

	for (i = 0; i < sizeof(g_nand_spare); i++)
		*(g_nand_spare + i) = 0xFF;

	offset = 0;
	for (i = 0; i < MTD_MAX_OOBFREE_ENTRIES && nand_oob->oobfree[i].length; i++) {
		/* Set the reserved bytes to 0xff */
		start = nand_oob->oobfree[i].offset;
		len = nand_oob->oobfree[i].length;
		memcpy((g_nand_spare + start), (oob + offset), len);
		offset += len;
	}

	// write bad index into oob
	if (mapped_block != block)
		set_bad_index_to_oob(g_nand_spare, block);
	else
		set_bad_index_to_oob(g_nand_spare, FAKE_INDEX);

#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	&& (devinfo.tlcControl.normaltlc)
	&& (devinfo.two_phyplane))
		page_no = ((mapped_block << 1) * page_per_block) + (page_no % page_per_block);
#endif

	if (!mtk_nand_write_page_hw(page_no, buf, g_nand_spare)) {
#if defined(MTK_TLC_NAND_SUPPORT)
		if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			&& (devinfo.tlcControl.normaltlc)
			&& (devinfo.two_phyplane))
			page_no = (mapped_block * page_per_block) + (page_no % page_per_block);

#endif
		MSG(INIT, "write fail @ block 0x%x, page 0x%x\n", mapped_block, page_no);
		return update_bmt((u64)page_no * g_nand_chip.oobblock, UPDATE_WRITE_FAIL, buf, g_nand_spare);
	}

	return TRUE;
}

int mtk_nand_write_page(u32 page, u8 * dat, u8 * oob)
{
	bool bRet = TRUE;
	u32 pagesz = g_nand_chip.oobblock;
	u32 timeout, u4SecNum = pagesz >> g_nand_chip.sector_shift;

	int i, j, start, len;
	bool empty = TRUE;
	u8 oob_checksum = 0;
	NFI_TLC_WL_INFO  tlc_wl_info;
	u32 reg_val;
	u32 real_row_addr = 0;
	u32 block_addr = 0;
	u32 page_in_block = 0;
	u32 page_per_block = 0;
	u32 temp_sec_num = u4SecNum;
#if MLC_MICRON_SLC_MODE
	u8 feature[4];
#endif

#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& devinfo.tlcControl.normaltlc)
	    devinfo.tlcControl.slcopmodeEn = TRUE;

#endif
#if CFG_2CS_NAND
	if (g_bTricky_CS)
		page = mtk_nand_cs_on(NFI_TRICKY_CS, page);
#endif
	page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;

	for (i = 0; i < MTD_MAX_OOBFREE_ENTRIES && nand_oob->oobfree[i].length; i++) {
		/* Set the reserved bytes to 0xff */
		start = nand_oob->oobfree[i].offset;
		len = nand_oob->oobfree[i].length;
		for (j = 0; j < len; j++) {
			oob_checksum ^= oob[start + j];
			if (oob[start + j] != 0xFF)
				empty = FALSE;
		}
	}

	if (!empty)
		oob[nand_oob->oobfree[i - 1].offset + nand_oob->oobfree[i - 1].length] = oob_checksum;

	if (use_randomizer && page >= RAND_START_ADDR)
		mtk_nand_turn_on_randomizer(page, 1,0);
	else if(pre_randomizer && page < RAND_START_ADDR)
		mtk_nand_turn_on_randomizer(page, 1,0);

	while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY) ;

#if defined(MTK_TLC_NAND_SUPPORT)
	mtk_nand_reset();

	if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		if (devinfo.tlcControl.normaltlc)  {
			NFI_TLC_GetMappedWL(page, &tlc_wl_info);
			real_row_addr = NFI_TLC_GetRowAddr(tlc_wl_info.word_line_idx);
			if(devinfo.tlcControl.pPlaneEn)	{
				real_row_addr = NFI_TLC_SetpPlaneAddr(real_row_addr, tlc_lg_left_plane);
				temp_sec_num /= 2;
			}

			if(devinfo.two_phyplane)
				temp_sec_num /= 2;

		} else
			real_row_addr = NFI_TLC_GetRowAddr(page);

		if(devinfo.tlcControl.slcopmodeEn) {
			if(((!devinfo.tlcControl.pPlaneEn) || tlc_lg_left_plane)
				 && (!tlc_snd_phyplane)) {
				if(0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
					reg_val = DRV_Reg(NFI_CNFG_REG16);
					reg_val &= ~CNFG_READ_EN;
					reg_val &= ~CNFG_OP_MODE_MASK;
					reg_val |= CNFG_OP_CUST;
					DRV_WriteReg(NFI_CNFG_REG16, reg_val);

					mtk_nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);

					reg_val = DRV_Reg32(NFI_CON_REG16);
					reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
					/* issue reset operation */
					DRV_WriteReg32(NFI_CON_REG16, reg_val);
				}
			}
		} else {
			if(devinfo.tlcControl.normaltlc) {
				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				if(PROGRAM_1ST_CYCLE == tlc_program_cycle)
					mtk_nand_set_command(PROGRAM_1ST_CYCLE_CMD);
				else if(PROGRAM_2ND_CYCLE == tlc_program_cycle)
					mtk_nand_set_command(PROGRAM_2ND_CYCLE_CMD);

				if(tlc_wl_info.wl_pre == WL_LOW_PAGE)
					mtk_nand_set_command(LOW_PG_SELECT_CMD);
				else if(tlc_wl_info.wl_pre == WL_MID_PAGE)
					mtk_nand_set_command(MID_PG_SELECT_CMD);
				else if(tlc_wl_info.wl_pre == WL_HIGH_PAGE)
					mtk_nand_set_command(HIGH_PG_SELECT_CMD);

				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);
			}
		}
	} else
	#endif
	{
		real_row_addr = page;
		if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC) ||
				(devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER)) {
			if(devinfo.two_phyplane)
				temp_sec_num /= 2;

			if(devinfo.tlcControl.slcopmodeEn) {
				if (devinfo.vendor == VEND_MICRON) {
					if (!tlc_snd_phyplane) {
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
			            	}
				} else if(0xFF != devinfo.tlcControl.en_slc_mode_cmd) {
					if (!tlc_snd_phyplane) {
						reg_val = DRV_Reg(NFI_CNFG_REG16);
						reg_val &= ~CNFG_READ_EN;
						reg_val &= ~CNFG_OP_MODE_MASK;
						reg_val |= CNFG_OP_CUST;
						DRV_WriteReg(NFI_CNFG_REG16, reg_val);

						mtk_nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);
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
						/*printk("mtk_nand_exec_read_page SLC Mode real_row_addr:%d, u4RowAddr:%d\n",
						real_row_addr, u4RowAddr);*/
					}
				}
			}

		    	while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY) ;
		}
	}

	if (mtk_nand_ready_for_write(real_row_addr, temp_sec_num, dat)) {
		mtk_nand_write_fdm_data(temp_sec_num, oob);

		if (!mtk_nand_write_page_data((u32 *) dat))
			bRet = false;

		if (!mtk_nand_check_RW_count(temp_sec_num << g_nand_chip.sector_shift))
			bRet = false;

		mtk_nand_stop_write();
		#if defined(MTK_TLC_NAND_SUPPORT)
		if(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
			if(devinfo.tlcControl.normaltlc) {
				if((devinfo.tlcControl.pPlaneEn) && tlc_lg_left_plane) {
			  		mtk_nand_set_command(PROGRAM_LEFT_PLANE_CMD);
					MSG(INIT, "[xl] program 1\n");
				} else {
					if(devinfo.tlcControl.slcopmodeEn) {
						if(devinfo.two_phyplane && (!tlc_snd_phyplane)) {
							mtk_nand_set_command(PROGRAM_LEFT_PLANE_CMD);
							MSG(INIT, "[xl] program 2\n");
						} else {
							mtk_nand_set_command(PROG_DATA_CMD);
							MSG(INIT, "[xl] program 3\n");
						}
					} else if(tlc_wl_info.wl_pre == WL_HIGH_PAGE) {
						if(devinfo.two_phyplane && (!tlc_snd_phyplane)) {
							mtk_nand_set_command(PROGRAM_LEFT_PLANE_CMD);
							MSG(INIT, "[xl] program 4\n");
						} else {
							if(tlc_cache_program) {
							mtk_nand_set_command(NAND_CMD_CACHEDPROG);
								MSG(INIT, "[xl] program 5\n");
							} else {
								mtk_nand_set_command(PROG_DATA_CMD);
								MSG(INIT, "[xl] program 6\n");
							}
						}
					} else {
						if(devinfo.two_phyplane && (!tlc_snd_phyplane))	{
							mtk_nand_set_command(PROGRAM_LEFT_PLANE_CMD);
							MSG(INIT, "[xl] program 7\n");
						} else {
							mtk_nand_set_command(PROGRAM_RIGHT_PLANE_CMD);
							MSG(INIT, "[xl] program 8\n");
						}
					}
			    }
			} else /* micron tlc */
		        	mtk_nand_set_command(PROG_DATA_CMD);
		} else
		#endif
		{
			if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER) {
				if ((devinfo.two_phyplane) && (!tlc_snd_phyplane))
					mtk_nand_set_command(PLANE_PROG_DATA_CMD);
				else if (tlc_cache_program)
					mtk_nand_set_command(NAND_CMD_CACHEDPROG);
				else
				mtk_nand_set_command(NAND_CMD_PAGEPROG);
			} else
		        	mtk_nand_set_command(NAND_CMD_PAGEPROG);
		}

		mtk_nand_status_ready(STA_NAND_BUSY);
		if (use_randomizer && page >= RAND_START_ADDR)
			mtk_nand_turn_off_randomizer();
		else if (pre_randomizer && page < RAND_START_ADDR)
			mtk_nand_turn_off_randomizer();

		bRet = mtk_nand_read_status();

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

				mtk_nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
			}
		}
#endif

		if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_MLC_HYBER) &&
			(devinfo.tlcControl.slcopmodeEn)) {
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

				mtk_nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
			}
		}
	} else {
		if(use_randomizer && page >= RAND_START_ADDR)
			mtk_nand_turn_off_randomizer();
		else if(pre_randomizer && page < RAND_START_ADDR)
			mtk_nand_turn_off_randomizer();

		return FALSE;
	}

	return bRet;
}

int mtk_nand_write_page_hw(u32 page, u8 * dat, u8 * oob)
{
	bool bRet = TRUE;
	u8 *temp_page_buf = NULL;
	u8 *temp_fdm_buf = NULL;
	u32 u4SecNum = g_nand_chip.page_size >> g_nand_chip.sector_shift;
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	u32 page_size = g_nand_chip.page_size;

	MSG(INIT, "mtk_nand_write_page_hw: page %d\n",page);
#if defined(MTK_TLC_NAND_SUPPORT)
	if(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)	 {
		if((devinfo.tlcControl.normaltlc) && (devinfo.tlcControl.pPlaneEn)) {
			if(devinfo.two_phyplane) {
				page_size /= 4;
				u4SecNum /= 4;
			} else {
				page_size /= 2;
				u4SecNum /= 2;
			}

			tlc_snd_phyplane = FALSE;
		    	tlc_lg_left_plane = TRUE; /* program left plane */
		    	temp_page_buf = dat;
			temp_fdm_buf = oob;

			bRet = mtk_nand_write_page(page, temp_page_buf, temp_fdm_buf);
			if(!bRet) /* operation fail */
				return bRet;

			tlc_lg_left_plane = FALSE; /* program right plane */
		    	temp_page_buf += page_size;
			temp_fdm_buf += (u4SecNum * g_nand_chip.nand_fdm_size);

		    	bRet = mtk_nand_write_page(page, temp_page_buf, temp_fdm_buf);

			if(devinfo.two_phyplane) {
				tlc_snd_phyplane = TRUE;
			    	tlc_lg_left_plane = TRUE; /* program left plane */
			    	temp_page_buf += page_size;
				temp_fdm_buf += (u4SecNum * g_nand_chip.nand_fdm_size);

			    	bRet = mtk_nand_write_page(page + page_per_block, temp_page_buf, temp_fdm_buf);
			    	if(!bRet) /* operation fail */
			        	return bRet;

				tlc_lg_left_plane = FALSE; /* program right plane */
			    	temp_page_buf += page_size;
				temp_fdm_buf += (u4SecNum * g_nand_chip.nand_fdm_size);

			    	bRet = mtk_nand_write_page(page + page_per_block, temp_page_buf, temp_fdm_buf);

				tlc_snd_phyplane = FALSE;
			}

		} else {
			if((devinfo.tlcControl.normaltlc) && (devinfo.two_phyplane)) {
				page_size /= 2;
				u4SecNum /= 2;
			}

			tlc_snd_phyplane = FALSE;
			temp_page_buf = dat;
			temp_fdm_buf = oob;

		    	bRet = mtk_nand_write_page(page, temp_page_buf, temp_fdm_buf);

			if(!bRet) /* operation fail */
				return bRet;

			if((devinfo.tlcControl.normaltlc) && (devinfo.two_phyplane)) {
				tlc_snd_phyplane = TRUE;
				temp_page_buf += page_size;
				temp_fdm_buf += (u4SecNum * g_nand_chip.nand_fdm_size);
			    	bRet = mtk_nand_write_page(page + page_per_block, temp_page_buf, temp_fdm_buf);
				tlc_snd_phyplane = FALSE;
			}
		}
	} else
	#endif
	{
		if(devinfo.two_phyplane) {
			page_size /= 2;
			u4SecNum /= 2;
		}
		tlc_snd_phyplane = FALSE;
		temp_page_buf = dat;
		temp_fdm_buf = oob;

		bRet = mtk_nand_write_page(page, temp_page_buf, temp_fdm_buf);
		if(!bRet) /* operation fail */
			return bRet;

		if(devinfo.two_phyplane) {
			tlc_snd_phyplane = TRUE;
			temp_page_buf += page_size;
			temp_fdm_buf += (u4SecNum * 8);
			bRet = mtk_nand_write_page(page + page_per_block, temp_page_buf, temp_fdm_buf);
			if(!bRet) /* operation fail */
				return bRet;
			tlc_snd_phyplane = FALSE;
		}
	}

	return bRet;
}

#if defined(MTK_TLC_NAND_SUPPORT)
bool mtk_nand_slc_write_wodata(u32 page)
{
	bool bRet = FALSE; /* FALSE --> Pass  TRUE-->Fail */
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

	reg_val = DRV_Reg(NFI_CNFG_REG16);
	reg_val &= ~CNFG_READ_EN;
	reg_val &= ~CNFG_OP_MODE_MASK;
	reg_val |= CNFG_OP_CUST;
	DRV_WriteReg(NFI_CNFG_REG16, reg_val);

	mtk_nand_set_command(0xA2);

	reg_val = DRV_Reg32(NFI_CON_REG16);
	reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
	/* issue reset operation */
	DRV_WriteReg32(NFI_CON_REG16, reg_val);

	mtk_nand_set_mode(CNFG_OP_PRGM);
	mtk_nand_set_command(NAND_CMD_SEQIN);

    	mtk_nand_set_address(0, real_row_addr, 2, 3);

	mtk_nand_set_command(NAND_CMD_PAGEPROG);

	slc_en = devinfo.tlcControl.slcopmodeEn;
	devinfo.tlcControl.slcopmodeEn = TRUE;
	bRet = !mtk_nand_read_status();
	devinfo.tlcControl.slcopmodeEn = slc_en;

	return bRet;
}
#endif
unsigned int nand_block_bad(u64 logical_addr)
{
	int block = (int)(logical_addr/BLOCK_SIZE);
	int mapped_block;
	mtk_nand_page_transform(logical_addr,&block,&mapped_block);
	if (nand_block_bad_hw((u64)mapped_block * BLOCK_SIZE)) {
		if (update_bmt((u64)mapped_block * BLOCK_SIZE, UPDATE_UNMAPPED_BLOCK, NULL, NULL))
			return logical_addr;
		return logical_addr + BLOCK_SIZE;
	}

	return logical_addr;
}

bool nand_block_bad_hw(u64 logical_addr)
{
	bool bRet = FALSE;
	u32 page = (u32)(logical_addr / g_nand_chip.oobblock);
	int i, page_num = (BLOCK_SIZE / g_nand_chip.oobblock);
	char *tmp = (char *)nfi_buf;
	u32 u4SecNum = g_nand_chip.oobblock >> g_nand_chip.sector_shift;

	memset(tmp, 0x0, g_nand_chip.oobblock + g_nand_chip.oobsize);
	memset(g_nand_spare, 0x0, 128);

	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
		page &= ~(page_num - 1);
#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	    && (devinfo.tlcControl.normaltlc)
	    && (devinfo.two_phyplane))
		page = ((page / page_num) << 1) * page_num;
	else
#endif
	{
		if(devinfo.two_phyplane)
			page = ((page / page_num) << 1) * page_num;
	}

#if defined(MTK_TLC_NAND_SUPPORT)
	#if 1
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& (devinfo.vendor == VEND_SANDISK)) {
		bRet = mtk_nand_slc_write_wodata(page);

		if((devinfo.two_phyplane) && (!bRet))
			bRet = mtk_nand_slc_write_wodata(page + page_num);
	} else
	#endif
	{
		mtk_nand_read_page_hw(page, tmp, g_nand_spare);
		if (g_nand_spare[0] != 0xFF) {
			bRet = TRUE;
			MSG(INIT, "bad block 0x%x\n", page);
		}

	}
#else
	#if CFG_2CS_NAND
	if (g_bTricky_CS)
		page = mtk_nand_cs_on(NFI_TRICKY_CS, page);
	#endif

	if(use_randomizer && page >= RAND_START_ADDR)
		mtk_nand_turn_on_randomizer(page, 0,0);
	else if(pre_randomizer && page < RAND_START_ADDR)
	    mtk_nand_turn_on_randomizer(page, 0,0);

	if (mtk_nand_ready_for_read(page, u4SecNum, tmp)) {
		if (!mtk_nand_read_page_data((u32 *) tmp))
			bRet = FALSE;

		if (!mtk_nand_status_ready(STA_NAND_BUSY))
			bRet = FALSE;

		if (!mtk_nand_check_dececc_done(u4SecNum))
			bRet = FALSE;

		mtk_nand_read_fdm_data(u4SecNum, g_nand_spare);

		if (!mtk_nand_check_bch_error(tmp, u4SecNum - 1, page))	{
			MSG(ERASE, "check bch error !\n");
			bRet = FALSE;
		}

		if(0 != (DRV_Reg32(NFI_STA_REG32) & STA_READ_EMPTY)) {
			memset(nfi_buf, 0xFF,g_nand_chip.page_size);
			memset(g_nand_spare, 0xFF,g_nand_chip.nand_fdm_size * u4SecNum);
			bRet = FALSE;
		}
		mtk_nand_stop_read();
	}

	if(use_randomizer && page >= RAND_START_ADDR)
		mtk_nand_turn_off_randomizer();
	else if(pre_randomizer && page < RAND_START_ADDR)
		mtk_nand_turn_off_randomizer();

	if (g_nand_spare[0] != 0xFF || g_nand_spare[8] != 0xFF \
		|| g_nand_spare[16] != 0xFF || g_nand_spare[24] != 0xFF)	{
		bRet = TRUE;
	}
#endif

	return bRet;
}

bool mark_block_bad(u64 logical_addr)
{
	u32 block;
	u32 mapped_block;

	mtk_nand_page_transform(logical_addr,&block,&mapped_block);
	return mark_block_bad_hw((u64)mapped_block * BLOCK_SIZE);
}

bool mark_block_bad_hw(u64 offset)
{
	bool bRet = FALSE;
	u32 index;
	u32 page_addr = (u32)(offset / g_nand_chip.oobblock);
	u32 u4SecNum = g_nand_chip.oobblock >> g_nand_chip.sector_shift;
	unsigned char *pspare;
	int i, page_num = (BLOCK_SIZE/ g_nand_chip.oobblock);
	unsigned char* buf = nand_nfi_buf;
	memset(buf, 0x00, STORAGE_BUFFER_SIZE);

	for (index = 0; index < 64; index++)
		*(g_nand_spare + index) = 0xFF;

	pspare = g_nand_spare;

	for (index = 8, i = 0; i < 4; i++)
		pspare[i * index] = 0x0;

	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
		page_addr &= ~(page_num - 1);

#if CFG_2CS_NAND
	if (g_bTricky_CS)
		page_addr = mtk_nand_cs_on(NFI_TRICKY_CS, page_addr);
#endif

	MSG(BAD, "Mark bad block 0x%x\n", page_addr);
	if(use_randomizer && page_addr >= RAND_START_ADDR)
		mtk_nand_turn_on_randomizer(page_addr, 1,0);
	else if(pre_randomizer && page_addr < RAND_START_ADDR)
		mtk_nand_turn_on_randomizer(page_addr, 1,0);

	while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY) ;

	if (mtk_nand_ready_for_write(page_addr, u4SecNum, buf)) {
		mtk_nand_write_fdm_data(u4SecNum, pspare);
		if (!mtk_nand_write_page_data((u32 *) & buf))
			bRet = FALSE;
		if (!mtk_nand_check_RW_count(g_nand_chip.oobblock))
			bRet = FALSE;
		mtk_nand_stop_write();
		mtk_nand_set_command(NAND_CMD_PAGEPROG);
		mtk_nand_status_ready(STA_NAND_BUSY);
	} else
		return FALSE;

	if(use_randomizer && page_addr >= RAND_START_ADDR)
		mtk_nand_turn_off_randomizer();
	else if(pre_randomizer && page_addr < RAND_START_ADDR)
		mtk_nand_turn_off_randomizer();

	for (index = 0; index < 64; index++)
		*(pspare + index) = 0xFF;
}

/*#############################################################################*/
/*# NAND Driver : Page Write
/*#
/*# NAND Page Format (Large Page 2KB)
/*#  |------ Page:2048 Bytes ----->>||---- Spare:64 Bytes -->>|
/*#
/*# Parameter Description:
/*#     page_addr               : specify the starting page in NAND flash
/*#
/*#############################################################################*/
bool mtk_nand_erase_hw(u64 offset)
{
	bool bRet = TRUE;
	u32 timeout, u4SecNum = g_nand_chip.oobblock >> g_nand_chip.sector_shift;
	u32 rownob = devinfo.addr_cycle - 2;
	u32 page_addr = (u32)(offset / g_nand_chip.oobblock);
#if defined(MTK_TLC_NAND_SUPPORT)
	NFI_TLC_WL_INFO  tlc_wl_info;
	u32 reg_val = 0;
	u32 snd_real_row_addr = 0;
	u32 page_per_block = devinfo.blocksize * 1024 / devinfo.pagesize;
	NFI_TLC_WL_INFO  snd_tlc_wl_info;
#endif
	u32   real_row_addr = 0;

	MSG(INIT, "mtk_nand_erase_hw: page %d\n", page_addr);
#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& devinfo.tlcControl.normaltlc)
		devinfo.tlcControl.slcopmodeEn = TRUE;
#endif

	if (nand_block_bad_hw(offset))
		return FALSE;

#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& (devinfo.tlcControl.normaltlc)
		&& (devinfo.two_phyplane))
		page_addr = ((page_addr / page_per_block) << 1) * page_per_block;
	else
#endif
		if(devinfo.two_phyplane)
			page_addr = ((page_addr / page_per_block) << 1) * page_per_block;

#if CFG_2CS_NAND
	if (g_bTricky_CS)
	page_addr = mtk_nand_cs_on(NFI_TRICKY_CS, page_addr);
#endif

#if defined(MTK_TLC_NAND_SUPPORT)
	if(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		if(devinfo.tlcControl.normaltlc) {
		    NFI_TLC_GetMappedWL(page_addr, &tlc_wl_info);
		        real_row_addr = NFI_TLC_GetRowAddr(tlc_wl_info.word_line_idx);
				if(devinfo.two_phyplane) {
					NFI_TLC_GetMappedWL((page_addr + page_per_block), &snd_tlc_wl_info);
					snd_real_row_addr = NFI_TLC_GetRowAddr(snd_tlc_wl_info.word_line_idx);
				}
		}
		else
		        real_row_addr = NFI_TLC_GetRowAddr(page_addr);
	} else
#endif
	{
		real_row_addr = page_addr;
	}

	mtk_nand_reset();

#if defined(MTK_TLC_NAND_SUPPORT)
	if(devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
		if((devinfo.tlcControl.slcopmodeEn)
			&& (0xFF != devinfo.tlcControl.en_slc_mode_cmd)) {
			reg_val = DRV_Reg(NFI_CNFG_REG16);
			reg_val &= ~CNFG_READ_EN;
			reg_val &= ~CNFG_OP_MODE_MASK;
			reg_val |= CNFG_OP_CUST;
			DRV_WriteReg(NFI_CNFG_REG16, reg_val);

			mtk_nand_set_command(devinfo.tlcControl.en_slc_mode_cmd);

			reg_val = DRV_Reg32(NFI_CON_REG16);
			reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
			/* issue reset operation */
			DRV_WriteReg32(NFI_CON_REG16, reg_val);
		} else {
			if(tlc_not_keep_erase_lvl) {
				reg_val = DRV_Reg(NFI_CNFG_REG16);
				reg_val &= ~CNFG_READ_EN;
				reg_val &= ~CNFG_OP_MODE_MASK;
				reg_val |= CNFG_OP_CUST;
				DRV_WriteReg(NFI_CNFG_REG16, reg_val);

				mtk_nand_set_command(NOT_KEEP_ERASE_LVL_A19NM_CMD);

				reg_val = DRV_Reg32(NFI_CON_REG16);
				reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
				/* issue reset operation */
				DRV_WriteReg32(NFI_CON_REG16, reg_val);
			}
		}
	}
#endif

#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& devinfo.tlcControl.normaltlc
		&& devinfo.two_phyplane) {
		mtk_nand_set_mode(CNFG_OP_CUST);
		mtk_nand_set_command(NAND_CMD_ERASE1);
		mtk_nand_set_address(0, real_row_addr, 0, rownob);
		mtk_nand_set_command(NAND_CMD_ERASE1);
		mtk_nand_set_address(0, snd_real_row_addr, 0, rownob);
		mtk_nand_set_command(NAND_CMD_ERASE2);
	} else
#endif
	{
		mtk_nand_set_mode(CNFG_OP_ERASE);
		mtk_nand_set_command(NAND_CMD_ERASE1);
		mtk_nand_set_address(0, real_row_addr, 0, rownob);

		mtk_nand_set_command(NAND_CMD_ERASE2);
	}

	if (!mtk_nand_status_ready(STA_NAND_BUSY))
		return FALSE;

	if (!mtk_nand_read_status())
		bRet = FALSE;

#if defined(MTK_TLC_NAND_SUPPORT)
	if((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
		&& (devinfo.tlcControl.slcopmodeEn)) { /* hynix tlc need doule check */

		if(0xFF != devinfo.tlcControl.dis_slc_mode_cmd) {
			reg_val = DRV_Reg32(NFI_CON_REG16);
			reg_val |= CON_FIFO_FLUSH|CON_NFI_RST;
			/* issue reset operation */
			DRV_WriteReg32(NFI_CON_REG16, reg_val);

			reg_val = DRV_Reg(NFI_CNFG_REG16);
			reg_val &= ~CNFG_READ_EN;
			reg_val &= ~CNFG_OP_MODE_MASK;
			reg_val |= CNFG_OP_CUST;
			DRV_WriteReg(NFI_CNFG_REG16, reg_val);

			mtk_nand_set_command(devinfo.tlcControl.dis_slc_mode_cmd);
		}
	}
#endif
	return bRet;
}

int mtk_nand_erase(u64 logical_addr)
{
	u32 block;
	u32 mapped_block;

	mtk_nand_page_transform((u64)logical_addr,&block,&mapped_block);
	if (!mtk_nand_erase_hw((u64)mapped_block * BLOCK_SIZE))	{
		MSG(INIT, "erase block 0x%x failed\n", mapped_block);
		return update_bmt((u64)mapped_block * BLOCK_SIZE, UPDATE_ERASE_FAIL, NULL, NULL);
	}

	return TRUE;
}

bool mtk_nand_wait_for_finish(void)
{
	while (DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY) ;
	return TRUE;
}

/**************************************************************************
*  MACRO LIKE FUNCTION
**************************************************************************/
static int nand_bread(blkdev_t * bdev, u32 blknr, u32 blks, u8 * buf, u32 part_id)
{
	u32 i;
	u64 offset = (u64)blknr * bdev->blksz;

	for (i = 0; i < blks; i++) {
		offset = nand_read_data(buf, offset);
		offset += bdev->blksz;
		buf += bdev->blksz;
	}
	return 0;
}

static int nand_bwrite(blkdev_t * bdev, u32 blknr, u32 blks, u8 * buf, u32 part_id)
{
	u32 i;
	u64 offset = (u64)blknr * bdev->blksz;

	for (i = 0; i < blks; i++) {
		offset = nand_write_data(buf, offset);
		offset += bdev->blksz;
		buf += bdev->blksz;
	}
	return 0;
}

// ==========================================================
// NAND Common Interface - Init
// ==========================================================
u32 nand_init_device(void)
{
	if (!blkdev_get(BOOTDEV_NAND)) {
		mtk_nand_reset_descriptor();
		mtk_nand_init();

		if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC) {
			PAGE_SIZE = (u32) g_nand_chip.page_size;
			PAGES_PER_BLOCK = BLOCK_SIZE / PAGE_SIZE;
			if(VEND_NONE != gVendor)
		  		BLOCK_SIZE = (u32) g_nand_chip.erasesize*2;
		else
			BLOCK_SIZE = (u32) g_nand_chip.erasesize;
		}

		memset(&g_nand_bdev, 0, sizeof(blkdev_t));
		g_nand_bdev.blksz = g_nand_chip.page_size;
		if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
			g_nand_bdev.erasesz = (devinfo.blocksize * 1024);
		else {
			if(VEND_NONE != gVendor)
				g_nand_bdev.erasesz = g_nand_chip.erasesize*2;
			else
				g_nand_bdev.erasesz = g_nand_chip.erasesize;
		}
		g_nand_bdev.blks = g_nand_chip.chipsize;
		g_nand_bdev.bread = nand_bread;
		g_nand_bdev.bwrite = nand_bwrite;
		g_nand_bdev.blkbuf = (u8 *) storage_buffer;
		g_nand_bdev.type = BOOTDEV_NAND;
		blkdev_register(&g_nand_bdev);
	}

	return 0;
}

#ifndef REDUCE_NAND_PL_SIZE /* not used. marked by xiaolei */
void Invert_Bits(u8 * buff_ptr, u32 bit_pos)
{
	u32 byte_pos = 0;
	u8 byte_val = 0;
	u8 temp_val = 0;
	u32 invert_bit = 0;

	byte_pos = bit_pos >> 3;
	invert_bit = bit_pos & ((1 << 3) - 1);
	byte_val = buff_ptr[byte_pos];
	temp_val = byte_val & (1 << invert_bit);

	if (temp_val > 0)
		byte_val &= ~temp_val;
	else
		byte_val |= (1 << invert_bit);
	buff_ptr[byte_pos] = byte_val;
}

void compare_page(u8 * testbuff, u8 * sourcebuff, u32 length, char *s)
{
	u32 errnum = 0;
	u32 ii = 0;
	u32 index;

	printf("%s", s);
	for (index = 0; index < length; index++) {
		if (testbuff[index] != sourcebuff[index]) {
			u8 t = sourcebuff[index] ^ testbuff[index];
			for (ii = 0; ii < 8; ii++) {
				if ((t >> ii) & 0x1 == 1)
					errnum++;
			}
			printf(" ([%d]=%x) != ([%d]=%x )",
				index, sourcebuff[index], index, testbuff[index]);
		}
	}
	if (errnum > 0)	{
		printf(": page have %d mismatch bits\n", errnum);
	} else {
		printf(" :the two buffers are same!\n");
	}
}

u8 empty_page(u8 * sourcebuff, u32 length)
{
	u32 index = 0;

	for (index = 0; index < length; index++) {
		if (sourcebuff[index] != 0xFF) {
			return 0;
		}
	}
	return 1;
}

u32 __nand_ecc_test(u32 offset, u32 max_ecc_capable)
{

	int ecc_level = max_ecc_capable;
	int sec_num = g_nand_chip.page_size >> g_nand_chip.sector_shift;
	u32 sec_size = g_nand_chip.page_size / sec_num;
	u32 NAND_MAX_PAGE_LENGTH = g_nand_chip.page_size + 8 * sec_num;
	u32 chk_bit_len = 64 * 4;
	u32 page_per_blk = BLOCK_SIZE / g_nand_chip.page_size;
	u32 sec_index, curr_error_bit, err_bits_per_sec, page_idx, errbits, err;

	u8 *testbuff = (u8*)malloc(NAND_MAX_PAGE_LENGTH);
	u8 *sourcebuff = (u8*)malloc(NAND_MAX_PAGE_LENGTH);
	u8 empty;

	for (err_bits_per_sec = 1; err_bits_per_sec <= ecc_level; err_bits_per_sec++) {
		printf("~~~start test ecc correct in ");
#if USE_AHB_MODE
		printf(" AHB mode");
#else
		printf(" MCU mode");
#endif
		printf(", every sector have %d bit error~~~\n", err_bits_per_sec);
		for (curr_error_bit = 0; curr_error_bit < chk_bit_len && offset < g_nand_chip.chipsize;
			offset += g_nand_chip.page_size) {
			memset(testbuff, 0x0a, NAND_MAX_PAGE_LENGTH);
			memset(sourcebuff, 0x0b, NAND_MAX_PAGE_LENGTH);
			g_bHwEcc = TRUE;
			nand_read_data(sourcebuff, offset);
			empty = empty_page(sourcebuff, g_nand_chip.page_size);
			if (empty) {
				printf("page %d is empty\n", offset / g_nand_chip.page_size);
				memset(sourcebuff, 0x0c, NAND_MAX_PAGE_LENGTH);
				nand_write_data(sourcebuff, offset);
				nand_read_data(sourcebuff, offset);
			}
			if (0 != (DRV_Reg32(ECC_DECENUM0_REG32) & 0xFFFFF)
				||0 != (DRV_Reg32(ECC_DECENUM1_REG32) & 0xFFFFF) ) {
				printf("skip the page %d, because it is empty ( %d )or already have error bits (%x)!\n",
					offset / g_nand_chip.page_size, empty, err);
			} else {
				printf("~~~start test ecc correct in Page 0x%x ~~~\n", offset / g_nand_chip.page_size);
				memcpy(testbuff, sourcebuff, NAND_MAX_PAGE_LENGTH);
				for (sec_index = 0; sec_index < sec_num; sec_index++) {
					//printf("insert err bit @ page %d:sector %d : bit ",page_idx+offset/g_nand_chip.page_size,sec_index);
					for (errbits = 0; errbits < err_bits_per_sec; errbits++) {
						Invert_Bits(((u8 *) testbuff) + sec_index * sec_size, curr_error_bit);
						//printf("%d, ",curr_error_bit);
						curr_error_bit++;
					}
					//printf("\n");
				}
				g_bHwEcc = FALSE;
				nand_write_data(testbuff, offset);
				compare_page(testbuff, sourcebuff, NAND_MAX_PAGE_LENGTH, "source and test buff check ");
				g_bHwEcc = TRUE;
				nand_read_data(testbuff, offset);
				compare_page(testbuff, sourcebuff, NAND_MAX_PAGE_LENGTH, "read back check ");
			}
		}
	}

	free(testbuff);
	free(sourcebuff);

}

u32 nand_ecc_test(void)
{
	part_t *part = part_get(PART_UBOOT);
	u32 offset = (part->startblk) * g_nand_chip.page_size;
	__nand_ecc_test(offset, 4);

	part_t *part2 = part_get(PART_BOOTIMG);
	offset = (part2->startblk) * g_nand_chip.page_size;
	__nand_ecc_test(offset, 4);
	return 0;
}
#endif

u32 nand_get_device_id(u8 * id, u32 len)
{
	u8 buf[16];

	// Config pin mux for NAND device, since EMI init will be called before NAND init done/
	mtk_nand_gpio_init();

	if (TRUE != getflashid(buf, len))
		return -1;

	len = len > 16 ? 16 : len;

	memcpy(id, buf, len);

	return 0;
}

/* LEGACY - TO BE REMOVED { */
// ==========================================================
// NAND Common Interface - Correct R/W Address
// ==========================================================
u64 nand_find_safe_block(u64 offset)
{

	u64 original_offset = offset;
	u64 new_offset = 0;
	unsigned int blk_index = 0;
	static BOOL Bad_Block_Table_init = FALSE;

	if (Bad_Block_Table_init == FALSE) {
		Bad_Block_Table_init = TRUE;
		Bad_Block_Table = nand_bad_block_tbl_buf;
		memset(Bad_Block_Table, 0, sizeof(Bad_Block_Table));
		print("Bad_Block_Table init, sizeof(Bad_Block_Table)= %d \n",
			sizeof(Bad_Block_Table));
	}

	blk_index = (u32)(BLOCK_ALIGN(offset) / BLOCK_SIZE);
	if (Bad_Block_Table[blk_index] == 1)
		return offset;

	// new_offset is block alignment
	new_offset = nand_block_bad(BLOCK_ALIGN(offset));

	// find next block until the block is good
	while (new_offset != BLOCK_ALIGN(offset)) {
		offset = new_offset;
		new_offset = nand_block_bad(BLOCK_ALIGN(offset));
	}

	if (original_offset != offset) {
		Bad_Block_Table[(original_offset / BLOCK_SIZE)] = 2;
		print("offset (0x%llx) is bad block. next safe block is (0x%lx)\n",
			original_offset, offset);
	}

	Bad_Block_Table[(BLOCK_ALIGN(offset) / BLOCK_SIZE)] = 1;

	return offset;
}

/* LEGACY - TO BE REMOVED } */

// ==========================================================
// NAND Common Interface - Read Function
// ==========================================================
u64 nand_read_data(u8 * buf, u64 offset)
{

	// make sure the block is safe to flash
	if (devinfo.NAND_FLASH_TYPE != NAND_FLASH_TLC)
		offset = nand_find_safe_block(offset);

	if (mtk_nand_read_page_hwecc(offset, buf) == FALSE) {
		print("nand_read_data fail\n");
		return -1;
	}

	return offset;
}

// ==========================================================
// NAND Common Interface - Write Function
// ==========================================================
u64 nand_write_data(u8 * buf, u64 offset)
{
	// make sure the block is safe to flash
	offset = nand_find_safe_block(offset);

	if (mtk_nand_write_page_hwecc(offset, buf) == FALSE) {
		print("nand_write_data fail\n");
		ASSERT(0);
	}

	return offset;
}

// ==========================================================
// NAND Common Interface - Erase Function
// ==========================================================
bool nand_erase_data(u64 offset, u64 offset_limit, u64 size)
{

	u64 img_size = size;
	u32 tpgsz;
	u32 tblksz;
	u64 cur_offset;
	u32 i = 0;

	// do block alignment check
	if (offset % BLOCK_SIZE != 0) {
		print("offset must be block alignment (0x%x)\n", BLOCK_SIZE);
		ASSERT(0);
	}
	// calculate block number of this image
	if ((img_size % BLOCK_SIZE) == 0) {
		tblksz = img_size / BLOCK_SIZE;
	} else {
		tblksz = (img_size / BLOCK_SIZE) + 1;
	}

	print("[ERASE] image size = 0x%llx\n", img_size);
	print("[ERASE] the number of nand block of this image = %d\n", tblksz);

	// erase nand block
	cur_offset = offset;
	while (tblksz != 0) {
		if (mtk_nand_erase(cur_offset) == FALSE) {
			print("[ERASE] erase fail\n");
			mark_block_bad(cur_offset);
		}
		cur_offset += BLOCK_SIZE;

		tblksz--;

		if (tblksz != 0 && cur_offset >= offset_limit) {
			print("[ERASE] cur offset (0x%llx) exceeds erase limit address (0x%lx)\n",
				cur_offset, offset_limit);
			return TRUE;
		}
	}

	return TRUE;
}

