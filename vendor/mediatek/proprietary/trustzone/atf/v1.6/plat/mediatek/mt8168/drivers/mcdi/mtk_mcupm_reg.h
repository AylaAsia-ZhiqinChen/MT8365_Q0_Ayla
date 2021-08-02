#ifndef __MTK_MCUPM_REG_H__
#define __MTK_MCUPM_REG_H__


/**************************************
 * Define and Declare
 **************************************/
#define MCUPM_CFGREG_SW_RSTN        (MCUPM_BASE + 0x000)
#define MCUPM_CFGREG_WFI_EN_SET     (MCUPM_BASE + 0x058)
#define MCUPM_CFGREG_WFI_EN_CLR     (MCUPM_BASE + 0x05C)
#define MCUPM_CFGREG_WFI_EN_STA     (MCUPM_BASE + 0x060)
#define MCUPM_CFGREG_SW_INT_SET     (MCUPM_BASE + 0x070)
#define MCUPM_CFGREG_SW_INT_STA     (MCUPM_BASE + 0x078)
#define MCUPM_CFGREG_SW_OPT         (MCUPM_BASE + 0x07C)
#define MCUPM_MCDI_CPU_PDN_STA      (MCUPM_BASE + 0x2D20)

#define MCUPM_DMA0_SRC              (MCUPM_BASE + 0x300)
#define MCUPM_DMA0_DST              (MCUPM_BASE + 0x304)
#define MCUPM_DMA0_WPPT             (MCUPM_BASE + 0x308)
#define MCUPM_DMA0_WPTO             (MCUPM_BASE + 0x30C)
#define MCUPM_DMA0_COUNT            (MCUPM_BASE + 0x310)
#define MCUPM_DMA0_CON              (MCUPM_BASE + 0x314)
#define MCUPM_DMA0_START            (MCUPM_BASE + 0x318)
#define MCUPM_DMA0_INTSTA           (MCUPM_BASE + 0x31C)
#define MCUPM_DMA0_ACKINT           (MCUPM_BASE + 0x320)
#define MCUPM_DMA0_RLCT             (MCUPM_BASE + 0x324)
#define MCUPM_DMA0_LIMITER          (MCUPM_BASE + 0x328)

/* base of mcupm internal sram (0x10217000) */
#define MCUPM_SRAM_BASE             (MCUPM_BASE + 0x1000)
/* base of mcupm debug sram (0x10218800) */
#define MCUPM_DEBUG_SRAM_BASE       (MCUPM_SRAM_BASE + 0x1800)
#define MCUPM_DEBUG_SRAM_SIZE       (0x600)
#define MCUPM_DEBUG_SRAM_NF_WORD    (MCUPM_DEBUG_SRAM_SIZE / 4)

/* base of mcupm mbox sram (0x10218D00) */
#define MCUPM_MBOX_SRAM_BASE        (MCUPM_SRAM_BASE + 0x1D00)
#define MCUPM_MCDI_AVAIL_CPU_MASK   (MCUPM_MBOX_SRAM_BASE + 0x08)

#endif
