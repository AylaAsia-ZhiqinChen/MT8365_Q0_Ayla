/*--------------------------------------------------------------*/
/*		DRAMC_COMMON.H				*/
/*--------------------------------------------------------------*/

#ifndef _DRAMC_COMMON_H_
#define _DRAMC_COMMON_H_

#include "typedefs.h"
#include "dramc_api.h"
#include "dramc_hqa.h"
#include "dramc_register.h"
#if (FOR_DV_SIMULATION_USED == 0)
#include "emi.h"
#endif

/*------------------------------------------------------------*/
/*                  macros, defines, typedefs, enums          */
/*------------------------------------------------------------*/
//#define DIAG
//#define DIAG_REDUCE_LOG
//#define MRW_DUMP
#if defined(TARGET_BUILD_VARIANT_ENG) && !defined(DRAM_SLT)
#define ENABLE_FULL_CALIB_LOG
#endif

/************************** Common Macro *********************/
#if (FOR_DV_SIMULATION_USED == 1) /* TBA used API */
#define dsb()
#define dramc_printf(level, fmt, args...) \
	do { \
		if ((level) > 0) \
			printf(fmt, ## args); \
	} while (0)

#define show_msg_with_timestamp(_x_) \
	do { \
		dramc_printf _x_; \
		show_current_time(); \
	} while (0)

#define INFO			1
#define CRITICAL		2

#define delay_ms(x)		delay_us(1)
#else /* REAL chip API */
#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")

#define delay_us(x)       udelay(x)
#define delay_ms(x)       mdelay(x)

#ifdef DRAM_SLT
#define dramc_printf(level, fmt, args...) \
do { \
	if ((level) >= 0) {\
		print(fmt, ## args);\
		delay_us(100);\
	}\
} while (0)
#else
#define dramc_printf(level, fmt, args...) \
do { \
        if ((level) >= 0) {\
                print(fmt, ## args);\
        }\
} while (0)
#endif
#define show_msg_with_timestamp(_x_)  print _x_

#define INFO			0
#define CRITICAL		0
#endif /* end of FOR_DV_SIMULATION_USED */

/**********************************************/
/* Priority of debug log                      */
/*--------------------------------------------*/
/* show_msg: High                       */
/* show_msg2: Medium High               */
/* show_msg3: Medium Low                */
/**********************************************/
#define show_log(_x_)   dramc_printf(INFO, _x_)
#define show_msg0(_x_)  dramc_printf _x_ /* Show the summary logs, very important, always enabled! */

#ifdef ENABLE_FULL_CALIB_LOG
#define show_diag(_x_)  dramc_printf _x_ /* Diag preloader logs */
#define show_mrw(_x_)   dramc_printf _x_ /* Dump the MR write ops */
#define show_msg(_x_)   dramc_printf _x_ /* Calibration Result */
#define show_msg2(_x_)  dramc_printf _x_ /* Calibration Sequence */
#define show_msg3(_x_)  dramc_printf _x_ /* Calibration Title or Info. */
#else
  #if defined (DIAG) || defined (DIAG_PRELOADER)
    #ifdef DIAG_REDUCE_LOG
#define show_diag(_x_)  if (p->frequency >= 1600) dramc_printf _x_ /* Diag preloader logs */
    #else
#define show_diag(_x_)  dramc_printf _x_ /* Diag preloader logs */
	#endif
  #else
  	#ifdef DRAM_SLT
#define show_diag(_x_)  dramc_printf _x_
	#else
#define show_diag(_x_)
	#endif
  #endif
  #ifdef MRW_DUMP
#define show_mrw(_x_)   dramc_printf _x_
  #else
#define show_mrw(_x_)
  #endif
#define show_msg(_x_)
#define show_msg2(_x_)
#define show_msg3(_x_)
#endif

#define show_err(_x_)   dramc_printf(CRITICAL, _x_)
#define show_err2(_x_, _y_)   dramc_printf(CRITICAL, _x_, _y_)
#define show_err3(_x_, _y_, _z_)   dramc_printf(CRITICAL, _x_, _y_, _z_)

// new log style
#define info(fmt, ...)  dramc_printf(INFO, fmt, __VA_ARGS__)
#define error(fmt, ...) dramc_printf(CRITICAL, fmt, __VA_ARGS__)

#define USE_PMIC_CHIP_MT6357	1

// Macro func for Vref window detection in DIAG SW
#ifdef DIAG
// Noted, the start/end here stands for the offset start/end on both direction
// As MTK suggested,
//  - CA should cover the 155mV window, i.e. 77.5mV on both side, so it is 77.5/4.4 = 18 steps
//  - TX should cover the 140mV window, i.e. 70mV on both side, so it is 70/4.4 = 16 steps
//  - RX should cover 50mV on each side, so it is 50/12.7 = 4 steps
//
// We will scan from -4 to +4 steps for CA/TX, and scan from -2 to +2 steps for RX
#define CA_VREF_CHECK_WIN_PERM_START 56 // 18-4=14 steps * 0.4% = 56
#define CA_VREF_CHECK_WIN_PERM_END   88 // 18+4=22 steps * 0.4% = 88
#define TX_VREF_CHECK_WIN_PERM_START 48 // 16-4=12 steps * 0.4% = 48
#define TX_VREF_CHECK_WIN_PERM_END   80 // 16+4=20 steps * 0.4% = 80

// the CA/TX (MR12/MR14) set level is also 0~50, and there are 2 ranges, range[0] is 10% to 30%, range[1] is 22% to 42%
// MR12: bit6 stands for range 0/1, bit0-5 stand for level 0~50
// 0.4% per step, so it is 4 per-mil
#define MR_VREF_RANGE0_MIN_PERM      100 // 10%
#define MR_VREF_RANGE0_MAX_PERM      300 // 30%
#define MR_VREF_RANGE1_MIN_PERM      220 // 22%
#define MR_VREF_RANGE1_MAX_PERM      420 // 42%
#define MR_VREF_STEP_PERM            4   // 0.4%

#define CONVERT_VREF_PERM_TO_CONF(perm) (((perm)<MR_VREF_RANGE1_MIN_PERM)?(((perm)-MR_VREF_RANGE0_MIN_PERM)/MR_VREF_STEP_PERM):((((perm)-MR_VREF_RANGE1_MIN_PERM)/MR_VREF_STEP_PERM)|0x40))
#define CONVERT_VREF_CONF_TO_PERM(conf) ((((conf)&0x40)!=0)?(MR_VREF_RANGE1_MIN_PERM+((conf)&0x3F)*MR_VREF_STEP_PERM):(MR_VREF_RANGE0_MIN_PERM+((conf)&0x3F)*MR_VREF_STEP_PERM))

#define RX_VREF_CHECK_WIN_CONF_START 2 // 4-2=2 steps
#define RX_VREF_CHECK_WIN_CONF_END   6 // 4+2=6 steps

#define CA_VREF_MIN_CONF 0
#define CA_VREF_MAX_CONF 50
#define TX_VREF_MIN_CONF 0
#define TX_VREF_MAX_CONF 50
#define RX_VREF_MIN_CONF 0
#define RX_VREF_MAX_CONF 31

// audio + xtalk pattern for CA/TX
#define DRAMC_ENGINE_DO_TX_TEST(p, err) do {                               \
    dramc_engine2_init(p, p->test2_1, p->test2_2, p->test_pattern, 0);       \
    dramc_engine2_set_pat(p, TEST_AUDIO_PATTERN, 0, 0);                       \
    err = dramc_engine2_run(p, TE_OP_WRITE_READ_CHECK, TEST_AUDIO_PATTERN);  \
    dramc_engine2_set_pat(p, TEST_XTALK_PATTERN, 0, 0);                       \
    err |= dramc_engine2_run(p, TE_OP_WRITE_READ_CHECK, TEST_XTALK_PATTERN); \
    dramc_engine2_end(p);                                                    \
} while(0)

// xtalk pattern for RX
#define DRAMC_ENGINE_DO_RX_TEST(p, err) do {                               \
    dramc_engine2_init(p, p->test2_1, p->test2_2, p->test_pattern, 0);       \
    dramc_engine2_set_pat(p, TEST_XTALK_PATTERN, 0, 0);                       \
    err = dramc_engine2_run(p, TE_OP_WRITE_READ_CHECK, TEST_XTALK_PATTERN);  \
    dramc_engine2_end(p);                                                    \
} while(0)
#endif

#endif
