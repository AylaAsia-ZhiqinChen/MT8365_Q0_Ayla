#ifndef _MTK_ADC_HW_H
#define _MTK_ADC_HW_H

#ifdef AUXADC_BASE
#undef AUXADC_BASE
#endif
#define AUXADC_BASE			0x11001000
#define AUXADC_CLK_STA			0x10001094
#define AUXADC_CLK_CLR			0x1000108c
#define AUXADC_CLK_SET			0x10001088

#define AUXADC1_SW_CG			(1 << 5)
#define AUXADC2_SW_CG			(1 << 15)
#define AUX_SW_CG_ADC			(1 << 14)	/*main clk*/
#define AUX_SW_CG_TP			(1 << 31)

#define AUXADC_CON0                     (AUXADC_BASE + 0x000)
#define AUXADC_CON1                     (AUXADC_BASE + 0x004)
#define AUXADC_CON1_SET                 (AUXADC_BASE + 0x008)
#define AUXADC_CON1_CLR                 (AUXADC_BASE + 0x00C)
#define AUXADC_CON2                     (AUXADC_BASE + 0x010)
#define AUXADC_DAT0                     (AUXADC_BASE + 0x014)

#define AUXADC_TP_CMD			(AUXADC_BASE + 0x005c)
#define AUXADC_TP_ADDR			(AUXADC_BASE + 0x0060)
#define AUXADC_TP_CON0			(AUXADC_BASE + 0x0064)
#define AUXADC_TP_DATA0			(AUXADC_BASE + 0x0074)

#define PAD_AUX_XP			13
#define TP_CMD_ADDR_X			0x0005

#define AUXADC_CON_RTP			(10209000 + 0x0404)

#define AUXADC_DET_VOLT                 (AUXADC_BASE + 0x084)
#define AUXADC_DET_SEL                  (AUXADC_BASE + 0x088)
#define AUXADC_DET_PERIOD               (AUXADC_BASE + 0x08C)
#define AUXADC_DET_DEBT                 (AUXADC_BASE + 0x090)
#define AUXADC_MISC                     (AUXADC_BASE + 0x094)
#define AUXADC_ECC                      (AUXADC_BASE + 0x098)
#define AUXADC_SAMPLE_LIST              (AUXADC_BASE + 0x09c)
#define AUXADC_ABIST_PERIOD             (AUXADC_BASE + 0x0A0)
#endif   /*_MTK_ADC_HW_H*/
