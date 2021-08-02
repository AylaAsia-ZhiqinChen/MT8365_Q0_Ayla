#ifndef __JPEG_REG_H__
#define __JPEG_REG_H__

#include "mmsys_reg_base.h"





#define REG_ADDR_JPEG_ENC_RSTB                           (JPEG_ENC_BASE + 0x100)
#define REG_ADDR_JPEG_ENC_CTRL                           (JPEG_ENC_BASE + 0x104)
#define REG_ADDR_JPEG_ENC_QUALITY                        (JPEG_ENC_BASE + 0x108)
#define REG_ADDR_JPEG_ENC_BLK_NUM                        (JPEG_ENC_BASE + 0x10C)
#define REG_ADDR_JPEG_ENC_BLK_CNT                        (JPEG_ENC_BASE + 0x110)
#define REG_ADDR_JPEG_ENC_INTERRUPT_STATUS               (JPEG_ENC_BASE + 0x11C)
#define REG_ADDR_JPEG_ENC_DST_ADDR0                      (JPEG_ENC_BASE + 0x120)
#define REG_ADDR_JPEG_ENC_DMA_ADDR0                      (JPEG_ENC_BASE + 0x124)
#define REG_ADDR_JPEG_ENC_STALL_ADDR0                    (JPEG_ENC_BASE + 0x128)
#define REG_ADDR_JPEG_ENC_OFFSET_ADDR                    (JPEG_ENC_BASE + 0x138)
#define REG_ADDR_JPEG_ENC_CURR_DMA_ADDR                  (JPEG_ENC_BASE + 0x13C)
#define REG_ADDR_JPEG_ENC_RST_MCU_NUM                    (JPEG_ENC_BASE + 0x150)
#define REG_ADDR_JPEG_ENC_IMG_SIZE                       (JPEG_ENC_BASE + 0x154)
//#define REG_ADDR_JPEG_ENC_GULTRA_TRESH                   (JPEG_ENC_BASE + 0x158)
#define REG_ADDR_JPEG_ENC_DEBUG_INFO0                    (JPEG_ENC_BASE + 0x160)
#define REG_ADDR_JPEG_ENC_DEBUG_INFO1                    (JPEG_ENC_BASE + 0x164)
#define REG_ADDR_JPEG_ENC_TOTAL_CYCLE                    (JPEG_ENC_BASE + 0x168)
#define REG_ADDR_JPEG_ENC_BYTE_OFFSET_MASK               (JPEG_ENC_BASE + 0x16C)
#define REG_ADDR_JPEG_ENC_SRC_LUMA_ADDR                  (JPEG_ENC_BASE + 0x170)
#define REG_ADDR_JPEG_ENC_SRC_CHROMA_ADDR                (JPEG_ENC_BASE + 0x174)
#define REG_ADDR_JPEG_ENC_STRIDE                         (JPEG_ENC_BASE + 0x178)
#define REG_ADDR_JPEG_ENC_IMG_STRIDE                     (JPEG_ENC_BASE + 0x17C)
//#define REG_ADDR_JPEG_ENC_MEM_CYCLE                      (JPEG_ENC_BASE + 0x208)




#define REG_ADDR_JPEG_ENC_DCM_CTRL                       (JPEG_ENC_BASE + 0x300)

#define REG_ADDR_JPEG_ENC_SMI_DEBUG0                     (JPEG_ENC_BASE + 0x304)
#define REG_ADDR_JPEG_ENC_SMI_DEBUG1                     (JPEG_ENC_BASE + 0x308)
#define REG_ADDR_JPEG_ENC_SMI_DEBUG2                     (JPEG_ENC_BASE + 0x30C)
#define REG_ADDR_JPEG_ENC_SMI_DEBUG3                     (JPEG_ENC_BASE + 0x310)
#define REG_ADDR_JPEG_ENC_CODEC_SEL                      (JPEG_ENC_BASE + 0x314)

#define REG_ADDR_JPEG_ENC_ULTRA_THRES                    (JPEG_ENC_BASE + 0x318)

#define REG_ADDR_JPEG_ENC_PASS1_DL_CTRL                  (JPEG_ENC_BASE + 0x400)
#define REG_ADDR_JPEG_ENC_PASS1_WIDTH_PIXEL              (JPEG_ENC_BASE + 0x404)
#define REG_ADDR_JPEG_ENC_PASS1_HEIGHT_PIXEL             (JPEG_ENC_BASE + 0x408)
#define REG_ADDR_JPEG_ENC_PASS1_WIDTH_MCU                (JPEG_ENC_BASE + 0x40C)
#define REG_ADDR_JPEG_ENC_PASS1_HEIGHT_MCU               (JPEG_ENC_BASE + 0x410)
#define REG_ADDR_JPEG_ENC_PASS1_TILE_NUM_IN_ROW          (JPEG_ENC_BASE + 0x414)
#define REG_ADDR_JPEG_ENC_PASS1_TILE_ROW_NUM             (JPEG_ENC_BASE + 0x418)


#define REG_ADDR_JPEG_ENC_PASS1_BS_BUF_BANK1             (JPEG_ENC_BASE + 0x41C)
#define REG_ADDR_JPEG_ENC_PASS1_BS_BUF_BANK2             (JPEG_ENC_BASE + 0x420)
#define REG_ADDR_JPEG_ENC_PASS1_BS_BUF_STRIDE            (JPEG_ENC_BASE + 0x424)

#define REG_ADDR_JPEG_ENC_PASS1_SINFO_BUF_BANK1          (JPEG_ENC_BASE + 0x428)
#define REG_ADDR_JPEG_ENC_PASS1_SINFO_BUF_BANK2          (JPEG_ENC_BASE + 0x42C)
#define REG_ADDR_JPEG_ENC_PASS1_SINFO_BUF_STRIDE         (JPEG_ENC_BASE + 0x430)

#define REG_ADDR_JPEG_ENC_PASS1_TILE_DL_DEBUG            (JPEG_ENC_BASE + 0x434)


#define REG_ADDR_JPEG_ENC_PASS2_RSTB                     (JPEG_ENC_BASE + 0x800)
#define REG_ADDR_JPEG_ENC_PASS2_CTRL                     (JPEG_ENC_BASE + 0x804)
#define REG_ADDR_JPEG_ENC_PASS2_TILE_BS_SEG_NUM          (JPEG_ENC_BASE + 0x808)
#define REG_ADDR_JPEG_ENC_PASS2_BS_BUF_BANK1             (JPEG_ENC_BASE + 0x80C)
#define REG_ADDR_JPEG_ENC_PASS2_BS_BUF_BANK2             (JPEG_ENC_BASE + 0x810)
#define REG_ADDR_JPEG_ENC_PASS2_BS_BUF_STRIDE            (JPEG_ENC_BASE + 0x814)

#define REG_ADDR_JPEG_ENC_PASS2_SINFO_BUF_BANK1          (JPEG_ENC_BASE + 0x818)
#define REG_ADDR_JPEG_ENC_PASS2_SINFO_BUF_BANK2          (JPEG_ENC_BASE + 0x81C)
#define REG_ADDR_JPEG_ENC_PASS2_SINFO_BUF_STRIDE         (JPEG_ENC_BASE + 0x820)

#define REG_ADDR_JPEG_ENC_PASS2_TILE_NUM_IN_ROW          (JPEG_ENC_BASE + 0x824)
#define REG_ADDR_JPEG_ENC_PASS2_TILE_ROW_NUM             (JPEG_ENC_BASE + 0x828)
#define REG_ADDR_JPEG_ENC_PASS2_TILE_HEIGHT1             (JPEG_ENC_BASE + 0x82C)
#define REG_ADDR_JPEG_ENC_PASS2_TILE_HEIGHT2             (JPEG_ENC_BASE + 0x830)



#define REG_ADDR_JPEG_ENC_PASS2_DST_BASE                 (JPEG_ENC_BASE + 0x834)
#define REG_ADDR_JPEG_ENC_PASS2_DST_OFFSET               (JPEG_ENC_BASE + 0x838)
#define REG_ADDR_JPEG_ENC_PASS2_STATE                    (JPEG_ENC_BASE + 0x83C)
#define REG_ADDR_JPEG_ENC_PASS2_INT_STS                  (JPEG_ENC_BASE + 0x840)
#define REG_ADDR_JPEG_ENC_PASS2_DCM_CTRL                 (JPEG_ENC_BASE + 0x844)

#define REG_ADDR_JPEG_ENC_PASS2_DMA_ADDR                 (JPEG_ENC_BASE + 0x854)
#define REG_ADDR_JPEG_ENC_PASS2_STALL_ADDR               (JPEG_ENC_BASE + 0x868)


/********************************************************************/
//                        JPEG ENCODE CONFIG
/********************************************************************/

#define JPEG_P1_CHECKSUM_EN (0x20000000)
#define JPEG_P2_CHECKSUM_EN (0x00008000)
//#define JPEG_P1_CHECKSUM_EN (0x00000000)
//#define JPEG_P2_CHECKSUM_EN (0x00000000)


#define CONFIG_JPEG_ENC_PASS1_DL_CTRL                  ( (JPEG_P1_CHECKSUM_EN) | 0x18)          //default set SI_WRAPPER + FAST_MODE on
#define CONFIG_JPEG_ENC_PASS2_CTRL                     ( (JPEG_P2_CHECKSUM_EN) | 0x10000)       //default set wrapper on
//#define CONFIG_JPEG_ENC_PASS1_DL_CTRL                  (0x0)          //default set SI_WRAPPER + FAST_MODE off
//#define CONFIG_JPEG_ENC_PASS2_CTRL                     (0x00000)       //default set wrapper off


/********************************************************************/
/********************************************************************/



#define REG_ADDR_JPGDEC_DEST_ADDR0_Y                 ( JPEG_DEC_BASE + 0x0140 )
#define REG_ADDR_JPGDEC_DEST_ADDR0_U                 ( JPEG_DEC_BASE + 0x0144 )
#define REG_ADDR_JPGDEC_DEST_ADDR0_V                 ( JPEG_DEC_BASE + 0x0148 )
#define REG_ADDR_JPGDEC_PAUSE_MCU_NUM                ( JPEG_DEC_BASE + 0x0170 )
#define REG_ADDR_JPGDEC_OPERATION_MODE               ( JPEG_DEC_BASE + 0x017C )
#define REG_ADDR_JPGDEC_TRIG                         ( JPEG_DEC_BASE + 0x0240 )
#define REG_ADDR_JPGDEC_INTERRUPT_STATUS             ( JPEG_DEC_BASE + 0x0274 )


#endif
