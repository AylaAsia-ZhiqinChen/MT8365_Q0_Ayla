
#ifndef __NAND_DEVICE_LIST_H__
#define __NAND_DEVICE_LIST_H__

#define NAND_MAX_ID		6
#define CHIP_CNT		20
#define P_SIZE		16384
#define P_PER_BLK		256
#define C_SIZE		8192
#define RAMDOM_READ		(1<<0)
#define CACHE_READ		(1<<1)
#define RAND_TYPE_SAMSUNG 0
#define RAND_TYPE_TOSHIBA 1
#define RAND_TYPE_NONE 2

#define READ_RETRY_MAX 10
struct gFeature
{
	u32 address;
	u32 feature;
};

enum readRetryType
{
	RTYPE_MICRON,
	RTYPE_SANDISK,
	RTYPE_SANDISK_19NM,
	RTYPE_TOSHIBA,
	RTYPE_HYNIX,
	RTYPE_HYNIX_16NM,
	#if defined(MTK_TLC_NAND_SUPPORT)
	RTYPE_SANDISK_TLC_1YNM,
	#endif
	RTYPE_HYNIX_FDIE,
	#if defined(MTK_TLC_NAND_SUPPORT)
	RTYPE_TOSHIBA_TLC_A19NM,
	RTYPE_SANDISK_TLC_1ZNM
	#endif
};

struct gFeatureSet
{
	u8 sfeatureCmd;
	u8 gfeatureCmd;
	u8 readRetryPreCmd;
	u8 readRetryCnt;
	u32 readRetryAddress;
	u32 readRetryDefault;
	u32 readRetryStart;
	enum readRetryType rtype;
	struct gFeature Interface;
	struct gFeature Async_timing;
};

struct gRandConfig
{
	u8 type;
	u32 seed[6];
};

enum pptbl
{
	MICRON_8K,
	HYNIX_8K,
	SANDISK_16K,
	PPTBL_NONE,
};

struct MLC_feature_set
{
	enum pptbl ptbl_idx;
	struct gFeatureSet 	 FeatureSet;
	struct gRandConfig   randConfig;
};

enum flashdev_vendor
{
	VEND_SAMSUNG,
	VEND_MICRON,
	VEND_TOSHIBA,
	VEND_HYNIX,
	VEND_SANDISK,
	VEND_BIWIN,
	VEND_NONE,
};

enum flashdev_IOWidth
{
	IO_8BIT = 8,
	IO_16BIT = 16,
	IO_TOGGLEDDR = 9,
	IO_TOGGLESDR = 10,
	IO_ONFI = 12,
};

#define NAND_FLASH_SLC   (0x0000)
#define NAND_FLASH_MLC   (0x0001)
#define NAND_FLASH_TLC   (0x0002)
#define NAND_FLASH_MLC_HYBER	(0x0003)
#define NAND_FLASH_MASK   (0x00FF)

typedef struct {
	bool		slcopmodeEn;				/*TRUE: slc mode	FALSE: tlc mode*/
	bool		pPlaneEn;					/*this chip has pseudo plane*/
	bool		needchangecolumn;			/*read page with change column address command*/
	bool		normaltlc;					/*whether need 09/0d 01/02/03*/
	u16		en_slc_mode_cmd;			/*enable slc mode cmd*/
	u16		dis_slc_mode_cmd;			/*disable slc mode cmd: 0xff is invalid*/
	bool		ecc_recalculate_en;		/*for nfi config*/
	u8		ecc_required;				/*required ecc bit*/
	u8		block_bit;					/*block address start bit;*/
	u8		pPlane_bit;				/*pesudo plane bit;*/
}NFI_TLC_CTRL;

typedef enum {
	PROGRAM_1ST_CYCLE = 1,
	PROGRAM_2ND_CYCLE = 2,
	PROGRAM_3RD_CYCLE = 3
}NFI_TLC_PG_CYCLE;

typedef enum {
	WL_LOW_PAGE = 0,
	WL_MID_PAGE = 1,
	WL_HIGH_PAGE = 2,
}NFI_TLC_WL_PRE;

typedef struct {
	u32 word_line_idx;
	NFI_TLC_WL_PRE wl_pre;
}NFI_TLC_WL_INFO;

typedef struct
{
	u8 id[NAND_MAX_ID];
	u8 id_length;
	u8 addr_cycle;
	u32 iowidth;
	#if defined(MTK_TLC_NAND_SUPPORT)
	u32 totalsize;
	#else
	u16 totalsize;
	#endif
	u16 blocksize;
	u16 pagesize;
	u16 sparesize;
	u32 timmingsetting;
	u32 s_acccon;
	u32 s_acccon1;
	u32 freq;
	#if defined(MTK_TLC_NAND_SUPPORT)
	enum flashdev_vendor vendor;
	#else
	u16 vendor;
	#endif
	u16 sectorsize;
	u8 devciename[30];
	u32 advancedmode;
	struct MLC_feature_set feature_set;
	u16 NAND_FLASH_TYPE;
	NFI_TLC_CTRL tlcControl;
	bool two_phyplane; //whether have two physical plane.
}flashdev_info,*pflashdev_info;

#if defined(MTK_TLC_NAND_SUPPORT)
static const flashdev_info gen_FlashTable[]={
	{{0x45,0x4C,0x98,0xA3,0x76,0x00}, 5,5,IO_8BIT,0x10F2000,6144,16384,1952,0x10401011, 0xC03222,0x101,80,VEND_SANDISK,1024, "SDTNSIAMA016G",0 ,
                	{PPTBL_NONE, {0xEF,0xEE,0x5D,46,0x11,0,0,RTYPE_SANDISK_TLC_1ZNM,{0x80, 0x00},{0x80, 0x01}},
                	{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
                	NAND_FLASH_TLC, {FALSE, FALSE, TRUE, TRUE, 0xA2, 0xFF, TRUE, 68, 8, 0}, false},
	{{0x45,0xDE,0x98,0x92,0x72,0x00}, 5,5,IO_8BIT,0x83C580,4128,16384,1952,0x10401011, 0xC03222,0x101,80,VEND_SANDISK,1024, "SDTNRCAMAOO8G",0 ,
		{PPTBL_NONE, {0xEF,0xEE,0x5D,39,0x11,0,0,RTYPE_SANDISK_TLC_1YNM,{0x80, 0x00},{0x80, 0x01}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_TLC, {FALSE, TRUE, TRUE, TRUE, 0xA2, 0xFF, TRUE, 68, 9, 8}, false},
	{{0x45,0xDE,0xA8,0x92,0x76,0x00}, 5,5,IO_8BIT,0x884E80,8256,32768,3904,0x10401011, 0xC03222,0x101,80,VEND_SANDISK,1024, "SDTNRIAMAOO8G",0 ,
		{PPTBL_NONE, {0xEF,0xEE,0x5D,39,0x11,0,0,RTYPE_SANDISK_TLC_1YNM,{0x80, 0x00},{0x80, 0x01}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_TLC, {false, true, true, true, 0xA2, 0xFF, true, 68, 9, 8}, false},
	{{0x98,0xDE,0x98,0x92,0x72,0x00}, 5,5,IO_8BIT,0x83C580,4128,16384,1952,0x10401011, 0xC03222,0x101,80,VEND_TOSHIBA,1024, "TC58TEG6TCKTA00",0 ,
		{PPTBL_NONE, {0xEF,0xEE,0x5D,31,0x11,31,0,RTYPE_TOSHIBA_TLC_A19NM,{0x80, 0x00},{0x80, 0x01}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_TLC, {false, true, true, true, 0xA2, 0xFF, true, 68, 9, 8}, false},
	{{0x98,0x3A,0x98,0xA3,0x76,0x00}, 5,5,IO_8BIT,0x10F2000,6144,16384,1952,0x10401011, 0xC03222,0x101,80,VEND_SANDISK,1024, "TC58TEG7THLBA09",0 ,
                	{PPTBL_NONE, {0xEF,0xEE,0x5D,46,0x11,0,0,RTYPE_SANDISK_TLC_1ZNM,{0x80, 0x00},{0x80, 0x01}},
               	 	{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
                	NAND_FLASH_TLC, {FALSE, FALSE, TRUE, TRUE, 0xA2, 0xFF, TRUE, 68, 8, 0}, false},		
	{{0x45,0xDE,0x94,0x93,0x76,0x51}, 6,5,IO_8BIT,8192 << 10,4096,16384,1280,0x10401011, 0xC03222,0x101,80,VEND_SANDISK,1024, "SDTNSGAMA008GM ",0 ,
	                {SANDISK_16K, {0xEF,0xEE,0x5D,32,0x11,0,0xFFFFFFFD,RTYPE_SANDISK,{0x80, 0x00},{0x80, 0x01}},
	                {RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
	                NAND_FLASH_MLC_HYBER, {FALSE, FALSE, FALSE, FALSE, 0xA2, 0xFF, FALSE, 0xFF, 8, 0xFF}, false },
	{{0x45,0xDE,0x94,0x93,0x76,0x00}, 5,5,IO_8BIT,8192 << 10,4096,16384,1280,0x10804222, 0xC03222,0x101,80,VEND_SANDISK,1024, "SDTNRGAMA008GK ",0 ,
		{SANDISK_16K, {0xEF,0xEE,0x5D,32,0x11,0,0xFFFFFFFF,RTYPE_SANDISK,{0x80, 0x00},{0x80, 0x01}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_MLC, {FALSE, FALSE, FALSE, FALSE, 0xFF, 0xFF, FALSE, 0xFF, 8, 0xFF}, false },
	{{0xAD,0xDE,0x14,0xA7,0x42,0x00}, 5,5,IO_8BIT,8192 << 10,4096,16384,1280,0x10804222, 0xC03222,0x101,80,VEND_HYNIX,1024, "H27UCG8T2ETR",0 ,
		{SANDISK_16K, {0xFF,0xFF,0xFF,7,0xFF,0,1,RTYPE_HYNIX_16NM,{0xFF, 0xFF},{0xFF, 0xFF}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_MLC, {FALSE, FALSE, FALSE, FALSE, 0xFF, 0xFF, FALSE, 0xFF, 8, 0xFF}, false },
	{{0xAD,0xDE,0x94,0xA7,0x42,0x00}, 5,5,IO_8BIT,8192 << 10,4096,16384,1280,0x10804222, 0xC03222,0x101,80,VEND_BIWIN,1024, "BW27UCG8T2ETR",0 ,
		{SANDISK_16K, {0xFF,0xFF,0xFF,7,0xFF,0,1,RTYPE_HYNIX_16NM,{0xFF, 0xFF},{0xFF, 0xFF}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_MLC, {FALSE, FALSE, FALSE, FALSE, 0xFF, 0xFF, FALSE, 0xFF, 8, 0xFF}, false },
	{{0x45,0xD7,0x84,0x93,0x72,0x00}, 5,5,IO_8BIT,4096 << 10,4096,16384,1280,0x10804222, 0xC03222,0x101,80,VEND_SANDISK,1024, "SDTNRGAMA004GK ",0 ,
		{SANDISK_16K, {0xEF,0xEE,0x5D,32,0x11,0,0xFFFFFFFF,RTYPE_SANDISK,{0x80, 0x00},{0x80, 0x01}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_MLC, {FALSE, FALSE, FALSE, FALSE, 0xFF, 0xFF, FALSE, 0xFF, 8, 0xFF}, false },
	{{0xAD,0xD7,0x94,0x91,0x60,0x00}, 5,5,IO_8BIT,4096 << 10,2048,8192,640,0x10804222, 0xC03222,0x101,80,VEND_HYNIX,1024, "H27UBG8T2CTR",0 ,
		{HYNIX_8K, {0xFF,0xFF,0xFF,7,0xFF,0,1,RTYPE_HYNIX,{0xFF, 0xFF},{0xFF, 0xFF}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_MLC, {FALSE, FALSE, FALSE, FALSE, 0xFF, 0xFF, FALSE, 0xFF, 8, 0xFF}, false },
	{{0x45,0x3A,0x94,0x93,0x76,0x51}, 5,5,IO_8BIT,16384 << 10,8192,32768,2560,0x10804222, 0xC03222,0x101,80,VEND_SANDISK,1024, "SDTNSGAMA016G",0 ,
		{SANDISK_16K, {0xEF,0xEE,0x5D,33,0x11,0,0xFFFFFFFE,RTYPE_SANDISK,{0x80, 0x00},{0x80, 0x01}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_MLC_HYBER, {FALSE, FALSE, FALSE, FALSE, 0xA2, 0xFF, FALSE, 0xFF, 8, 0xFF}, true },
	{{0x2C,0x64,0x44,0x4B,0xA9,0x00}, 5,5,IO_8BIT,8192 << 10,2048,8192,640,0x10804222, 0xC03222,0x101,80,VEND_MICRON,1024, "MT29F64G08CBABA",0 ,
		{MICRON_8K, {0xEF,0xEE,0xFF,7,0x89,0,1,RTYPE_MICRON,{0x1, 0x14},{0x1, 0x5}},
		{RAND_TYPE_SAMSUNG,{0x2D2D,1,1,1,1,1}}},
		NAND_FLASH_MLC_HYBER, {FALSE, FALSE, FALSE, FALSE, 0xFF, 0xFF, FALSE, 0xFF, 8, 0xFF}, false },                		
};
#endif
#endif
