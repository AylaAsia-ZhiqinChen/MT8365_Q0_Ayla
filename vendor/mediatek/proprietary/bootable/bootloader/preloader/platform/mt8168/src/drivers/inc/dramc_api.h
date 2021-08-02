#ifndef _PI_API_H
#define _PI_API_H

#if (FOR_DV_SIMULATION_USED == 0)
#include "custom_emi.h"
#endif
#include "typedefs.h"

#define DRAMK_VERSION	20190818

#define FOR_DV_SIMULATION_USED		0
#define UNDER_PORTING			1

//#define DRAM_SLT
#define TX_WIN_CRITERIA		16  /*16PI*/
#define RX_WIN_CRITERIA		65	/*0.65UI*/

/* To control code size if needed */
#define SUPPORT_TYPE_LPDDR4		COMBO_LPDDR4
#define LPDDR4_DIV4_MODE_EN		0
#define SUPPORT_TYPE_LPDDR3		COMBO_LPDDR3
#define SUPPORT_TYPE_PCDDR4		COMBO_PCDDR4
#define PCDDR4_DIV4_MODE_EN		0
#define SUPPORT_TYPE_PCDDR3		COMBO_PCDDR3
#define SUPPORT_PCDDR3_32BIT	1

/***********************************************************************/
/*                  Public Types                                       */
/***********************************************************************/
#define FALSE   0
#define TRUE    1

/***********************************************************************/
/*              Multi Thread Multi Core                                      */
/***********************************************************************/
//#define PARALLEL_CH_CAL
#ifdef PARALLEL_CH_CAL
#define DRAMC_INIT_MULTI_CORE
#define CHANNEL_A_ENABLE 1
#define CHANNEL_B_ENABLE 1
#define CHANNEL_C_ENABLE 1
#define CHANNEL_D_ENABLE 1
#endif

/***********************************************************************/
/*              Constant Define                                        */
/***********************************************************************/
#define chip_name	"[MT8168]"
#define PL_version		"Version 0.1"

#define NEED_REVIEW			1
#define NON_EXIST_RG		0

#define HW_BROADCAST_ON		1

#define DUAL_FREQ_K 		1

#define ENABLE_RANK_NUMBER_AUTO_DETECTION 1
/* Impedance calculation used Resister values
 * Caculation Equation:
 * OCDP_final = (fcR_PU * OCDP) / fcR_OCD
 * OCDN_final = (fcR_PD * OCDN) / fcR_OCD
 * ODTP_final = (fcR_EXT * OCDP) / fcR_ODT
 * ODTN_final = (fcR_EXT * OCDN) / fcR_ODT
 */

#if SUPPORT_TYPE_LPDDR4
#define OCDP_DEFAULT	0x1f
#define OCDN_DEFALUT	0x1f
#else
#define OCDP_DEFAULT	0x8
#define OCDN_DEFALUT	0x9
#endif

#define CALIBRATION_LOG 0
/* #define LK_LEAVE_FREQ_LP4_DDR4266 */

#define FAST_CAL		0

/* Bring Up Selection : Do Not open it when normal operation */
#if CFG_MTK_DRAM_FIRST_BRINGUP
#define FIRST_BRING_UP 1
#else
#define FIRST_BRING_UP 0
#endif

#if (FOR_DV_SIMULATION_USED == 1)
#define CHANNEL_MAX		2 /* 2 Virtual channel, 1 physical channel */

#define CHANNEL_NUM		2
#else
#define CHANNEL_MAX		2

#define CHANNEL_NUM		2
#endif

/*Feature option*/
#define ENABLE_DUTY_SCAN_V2   1

/* SW option */
#define CBT_K_RANK1_USE_METHOD  1
#if (FOR_DV_SIMULATION_USED == 1)
#define ENABLE_WRITE_DBI 0
#else
#define ENABLE_WRITE_DBI (SUPPORT_TYPE_LPDDR4)
#endif
#define ENABLE_READ_DBI 0

#define DDR4266_FREQ 2133
#define DDR3733_FREQ 1866
#define DDR3200_FREQ 1600
#define DDR2666_FREQ 1333
#define DDR2400_FREQ 1200
#define DDR2280_FREQ 1140
#define DDR1866_FREQ 933
#define DDR1600_FREQ 800
#define DDR1333_FREQ 667
#define DDR1200_FREQ 600
#define DDR1066_FREQ 533
#define DDR933_FREQ 467
#define DDR800_FREQ 400

/* SI simulation indicate that per-bit de-skew is neccessary, especially
 * for DDR3X16 configuration.
 * Here define it as the lowest supported frequence, that is
 * to enable per-bit de-skew for all freq, all types.
 */
#define PERBIT_THRESHOLD_FREQ	(DDR1600_FREQ)
#define LP4_LOW_FREQSEL	(DDR_DDR1600)
#define LP4_MIDDLE_FREQ	(DDR2666_FREQ)
#define LP4_MIDDLE_FREQSEL	(DDR_DDR2666)
#define LP4_HIGHEST_FREQ	(DDR3200_FREQ)
#define LP4_HIGHEST_FREQSEL	(DDR_DDR3200)

#define LP3_HIGHEST_FREQ	(DDR1866_FREQ)
#define DDR4_HIGHEST_FREQ	(DDR3200_FREQ)
#define DDR3_HIGHEST_FREQ	(DDR1866_FREQ)

#define APPLY_LP4_POWER_INIT_SEQUENCE	1

/* Definitions indicating DRAMC, DDRPHY register shuffle offset */
#define SHU_GRP_DRAMC_OFFSET      0x600
#define SHU_GRP_DDRPHY_OFFSET     0x500

/* init for test engine */
/* pattern0 and base address for test engine when we do calibration */
#define DEFAULT_TEST2_1_CAL 0x55000000
/* pattern1 and offset address for test engine when we  do calibraion */
#if (FOR_DV_SIMULATION_USED == 0)
#define DEFAULT_TEST2_2_CAL 0xaa000400
#else
/* cc notes: Reduce length to save simulation time */
#define DEFAULT_TEST2_2_CAL 0xaa000080
#endif
/* timeout for TE2: (CMP_CPT_POLLING_PERIOD X MAX_CMP_CPT_WAIT_LOOP) */
#define CMP_CPT_POLLING_PERIOD 1
/* max loop */
#define MAX_CMP_CPT_WAIT_LOOP 10000

#define ENABLE_BLOCK_APHY_CLOCK_DFS_OPTION 1
#define ENABLE_TMRRI_NEW_MODE		1
#define DLL_ASYNC_MODE				0
#define ENABLE_DLL_ALL_SLAVE_MODE	1
#define CBT_MOVE_CA_INSTEAD_OF_CLK  1
#define DramcHWDQSGatingTracking_DVT_FIFO_MODE 1 /* LP4 Gating tracing mode */
#define DramcHWDQSGatingTracking_DVT_LP3_FIFO_MODE 0
#define DramcHWDQSGatingTracking_DVT_JADE_TRACKING_MODE 1
#define ENABLE_DVFS_CDC_SYNCHRONIZER_OPTION	1
#define LP3_CATRAING_SHIFT_CLK_PI 8

/* common */
#define CATRAINING_NUM_LP4		6
#define CATRAINING_NUM_LP3		10
/* Note: If add new dram tye, remember to update  CATRAINING_NUM_MAX! */
#define CATRAINING_NUM_MAX		(CATRAINING_NUM_LP3 > CATRAINING_NUM_LP4	\
					? CATRAINING_NUM_LP3 : CATRAINING_NUM_LP4)

#define CA_GOLDEN_PATTERN		0x55555555
#define DQS_NUMBER			4
#define DQS_BIT_NUM			8
/* define max support bus width in the system (to allocate array size) */
#define DQ_DATA_WIDTH			32
#define DQ_DATA_WIDTH_LP4		16
/* 100us */
#define TIME_OUT_CNT			1000
#define HW_REG_SHUFFLE_MAX		3

/* Gating window */
/* 8 for testing 1 */
#define DQS_GW_COARSE_STEP		1
#define DQS_GW_FINE_START		0
#define DQS_GW_FINE_END			32
#define DQS_GW_FINE_STEP		4
#define GATING_ADJUST_TXDLY_FOR_TRACKING	1

/* DATLAT */
#define DATLAT_TAP_NUMBER	32 /* DATLAT[3:0] = {0x80[20:4]} */

/* TX DQ/DQS */
#define MAX_TX_DQDLY_TAPS	31 /* max DQ TAP number */
#define MAX_TX_DQSDLY_TAPS	31 /* max DQS TAP number */
#define TX_DQ_OE_SHIFT_LP4	3
#define TX_K_DQM_WITH_WDBI	(SUPPORT_TYPE_LPDDR4 || SUPPORT_TYPE_PCDDR4)

/* Run time config */
/* DQS strobe calibration enable */
#define HW_GATING
#define DUMMY_READ_FOR_TRACKING
#if SUPPORT_TYPE_LPDDR4
#define ENABLE_SW_RUN_TIME_ZQ_WA	0
#else
#define ENABLE_SW_RUN_TIME_ZQ_WA	1
#endif
#if !ENABLE_SW_RUN_TIME_ZQ_WA
#define ZQCS_ENABLE_LP4
#endif
#define ZQCS_ENABLE_LP3
/* After enable rumtime MR4 read, Get DramC SPCMDRESP_REFRESH_RATE. */
#define TEMP_SENSOR_ENABLE
/* Low pwer settings */
#if SUPPORT_TYPE_LPDDR3
#define APPLY_LOWPOWER_GOLDEN_SETTINGS	0
#else
#define APPLY_LOWPOWER_GOLDEN_SETTINGS	1
#endif
#define SPM_LIB_USE
#ifdef SPM_LIB_USE
#define SPM_CONTROL_AFTERK
#endif
#define IMPEDANCE_TRACKING_ENABLE
#define IMPEDANCE_HW_SAVING

#define MR01	1
#define MR02	2
#define MR03	3
#define MR05	5
#define MR06	6
#define MR08	8
#define MR11	11
#define MR12	12
#define MR13	13
#define MR14	14
#define MR18	18
#define MR19	19
#define MR22	22
#define MR23	23
#define MR37	37

#define MR01_FSP0_INIT	0x26
#define MR01_FSP1_INIT	0x56
#define MR02_INIT	0x1a
#define MR02_4266	0x3f
#define MR02_3733	0x36
#define MR02_3200	0x2d
#define MR02_2666	0x24
#define MR02_1600	0x12
#define MR03_INIT	0x31
#define MR11_ODT_DIS	0x00
#define MR11_ODT_60	0x04 /* CA/DQ ODT set to 60ohm */
#define MR11_ODT_80	0x03
#define MR12_INIT	0x4d /* LP4: 27.2%, Unterm CA */
#define MR13_RRO 1
#define MR13_FSP0_INIT	0 | (MR13_RRO << 4) | (1 << 3)
#define MR13_FSP1_INIT	0xc0 | (MR13_RRO << 4) | (1 << 3)
#define MR13_FSP1_SET	0x40 | (MR13_RRO << 4) | (1 << 3)
#define MR14_FSP0_INIT	0x4d /* LP4: 27.2%, Unterm DQ */
#define MR14_FSP1_INIT	0x10 /* LP4: termed DQ */
#define MR22_20		0x20
#define MR22_38		0x38
#define MR22_24		0x24
#define MR22_3C		0x3C
#define MR23_INIT	0x3f

#define PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER 1

#define TX_OE_EXTEND		0
#define TX_PICG_NEW_MODE	1
#define RX_PICG_NEW_MODE	1

#define ENABLE_TX_TRACKING	1
#if ENABLE_TX_TRACKING
#define ENABLE_SW_TX_TRACKING	0
#endif
#define ENABLE_RX_TRACKING_LP4	1
#define DVT_8PHASE_UNCERTAINTY_EN	0

#define ENABLE_RODT_TRACKING	1

#define CPU_RW_TEST_AFTER_K	1
#define TA2_RW_TEST_AFTER_K	1

/* pre-emphasis shoulde be adjusted by different frequency and IMP	*/
#define TX_DQ_PRE_EMPHASIS 0

#define PRINT_CALIBRATION_SUMMARY	1

#define XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY

#define DRAMC_MODEREG_CHECK	0

#define ENABLE_MIOCK_JMETER

//#define ENABLE_DUTY_SCAN	//[QW] TBD
#define ENABLE_LP3_SW     1

//#define DRAM_FULLSCAN

#define SET_FLD			0x1
#define CLEAR_FLD		0x0

#define TERM			1
#define UNTERM			0

#define PATTERN1	0x55000000
#define PATTERN2	0xaa000000
#define PATTERN3	0xaaaa5555

#define NIBBLE_MAX	0xfU
#define BYTE_MAX	0xffU
#define WORD_MAX	0xffffU
#define LWORD_MAX	0xffffffffU
#define UNCHAR_MAX	0x7fU
#define NUM_4096	4096

#define EDGE_NUMBER	2
/************* FIRST_BRING_UP Init Definition **************/
#if FIRST_BRING_UP

//#define FIX_SINGLE_CHANNEL

#undef ENABLE_WRITE_DBI
#define ENABLE_WRITE_DBI 0

#undef ENABLE_DUTY_SCAN_V2
#define ENABLE_DUTY_SCAN_V2		0

#undef APPLY_LOWPOWER_GOLDEN_SETTINGS
#define APPLY_LOWPOWER_GOLDEN_SETTINGS	0

#undef TX_K_DQM_WITH_WDBI
#define TX_K_DQM_WITH_WDBI		0

#undef PARALLEL_CH_CAL

#undef PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER
#define PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER	0

#undef ENABLE_TX_TRACKING
#undef ENABLE_SW_TX_TRACKING
#define ENABLE_TX_TRACKING		0
#define ENABLE_SW_TX_TRACKING		0

#undef CPU_RW_TEST_AFTER_K
#undef TA2_RW_TEST_AFTER_K
#define CPU_RW_TEST_AFTER_K		1
#define TA2_RW_TEST_AFTER_K		1

#undef ENABLE_RX_TRACKING_LP4
#define ENABLE_RX_TRACKING_LP4		0

#undef HW_GATING
#undef DUMMY_READ_FOR_TRACKING
/* #undef ZQCS_ENABLE_LP4 */
/* #undef SPM_CONTROL_AFTERK */
#undef TEMP_SENSOR_ENABLE
#undef IMPEDANCE_TRACKING_ENABLE
#undef ENABLE_SW_RUN_TIME_ZQ_WA
#define ENABLE_SW_RUN_TIME_ZQ_WA	0

#undef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY

//#undef ENABLE_RODT_TRACKING
//#define ENABLE_RODT_TRACKING	0

#undef CALIBRATION_LOG
#define CALIBRATION_LOG                        1

/* LP4 uses single channel to bringup. Undef BROADCAST */
#ifdef FIX_SINGLE_CHANNEL
#undef HW_BROADCAST_ON
#define HW_BROADCAST_ON		0
#endif

#undef GATING_ADJUST_TXDLY_FOR_TRACKING
#define GATING_ADJUST_TXDLY_FOR_TRACKING  0
#undef ENABLE_DUTY_SCAN

#endif /* FIRST_BRING_UP */

/*
 *****************************************************************************
 * for D Sim sumulation used
 *****************************************************************************
 */
#define SIMULATION_LP4_ZQ 1
#define SIMULATION_CBT 1
#define SIMULATION_WRITE_LEVELING  1
#define SIMULATION_GATING 1
#define SIMUILATION_LP4_RDDQC 1
#define SIMULATION_DATLAT 1
#define SIMULATION_SW_IMPED 1
#define SIMULATION_RX_PERBIT    1
#define SIMULATION_TX_PERBIT 1 /*  Please enable with write leveling */
#define SIMULATION_RX_OFFSET	0

#if (!PRINT_CALIBRATION_SUMMARY)
#define set_calibration_result(x, y, z)
#endif

/***********************************************************************/
/*               Defines                                                */
/***********************************************************************/
#define ENABLE  1
#define DISABLE 0

#define CBT_LOW_FREQ   0
#define CBT_HIGH_FREQ   1

#define PASS_RANGE_NA	0x7fff
#define MAX_CLK_PI_DELAY	31
#define MAX_CS_PI_DELAY		63
#define MAX_RX_DQSDLY_TAPS	127
#define MAX_RX_DQDLY_TAPS	63
/* RX_VREF_DEFAULT */
#define RX_VREF_DEFAULT_ODT	0xe
#define RX_VREF_DEFAULT		0x16
#define RX_VREF_DEFAULT_X_ODT	0xb
#define RX_VREF_DEFAULT_X	0x16
#define RX_VREF_DEFAULT_P	0x10

#if FAST_CAL
/* CBT */
#define CBT_VREF_BEGIN		(13 - 5)
#define CBT_VREF_END		(13 + 5)
#define CBT_VREF_BEGIN_X	(27 - 3)
#define CBT_VREF_END_X		(27 + 7)
#define CBT_VREF_RANGE1_BEGIN	21
#define CBT_VREF_MAX		50
#define MAX_CA_PI_DELAY	63
/* Write Leveling */
#define WL_OFFSET	12
#define WL_STEP		1
#define WL_RANGE	64
/* RxdqsGating */
#define RX_DQS_BEGIN_4266	20 /*2 4 0*/
#define RX_DQS_BEGIN_3733	21 /*2 5 0*/
#define RX_DQS_BEGIN_3200	27 /*3 3 0*/
#define RX_DQS_BEGIN_1600	18 /*2 2 0*/
#define RX_DQS_RANGE	16
/* RX DQ/DQS */
#define RX_VREF_RANGE_BEGIN	6
#define RX_VREF_RANGE_END	31
#define RX_VREF_RANGE_STEP	1
#define RX_VREF_RANGE_BEGIN_ODTOFF	18
#define MAX_RX_DQSDLY_TAPS_2666 10
#define MAX_RX_DQSDLY_TAPS_1600 36
/* TX DQ/DQS */
#define TX_VREF_RANGE_BEGIN	16
#define TX_VREF_RANGE_END	50
#define TX_VREF_RANGE_STEP	2
#define TX_VREF_RANGE_BEGIN1	(13 - 5) /* 300/1100(VDDQ) = 27.2% */
#define TX_VREF_RANGE_END1	(13 + 5)
#define TX_VREF_RANGE_BEGIN2	(30 - 5) /* 290/600(VDDQ)=48.3% */
#define TX_VREF_RANGE_END2	(30 + 5)
#else
/* CBT */
#define CBT_VREF_BEGIN		(13 - 5)
#define CBT_VREF_END		(13 + 5)
#define CBT_VREF_BEGIN_X	(27 - 3)
#define CBT_VREF_END_X		(27 + 7)
#define CBT_VREF_RANGE1_BEGIN	21
#define CBT_VREF_MAX		50
#define MAX_CA_PI_DELAY	63
/* Write Leveling */
#define WL_OFFSET 0
#define WL_STEP		1
#define WL_RANGE	64
/* RxdqsGating */
#define RX_DQS_BEGIN_4266	19
#define RX_DQS_BEGIN_3733	18
#define RX_DQS_BEGIN_2400	33
#define RX_DQS_BEGIN_3200	26
#define RX_DQS_BEGIN_1600	18
#define RX_DQS_RANGE	16
/* RX DQ/DQS */
#define RX_VREF_RANGE_BEGIN	0
#define RX_VREF_RANGE_END	31
#define RX_VREF_RANGE_STEP	1
#define RX_VREF_RANGE_BEGIN_ODTOFF	18
#define MAX_RX_DQSDLY_TAPS_2666 32
#define MAX_RX_DQSDLY_TAPS_1600 48
/* TX DQ/DQS */
#define TX_VREF_RANGE_BEGIN	0
#define TX_VREF_RANGE_END	50
#define TX_VREF_RANGE_STEP	2
#define TX_VREF_RANGE_BEGIN1	(13 - 5) /* 300/1100(VDDQ) = 27.2% */
#define TX_VREF_RANGE_END1	(13 + 5)
#define TX_VREF_RANGE_BEGIN2	(27 - 5) /* 290/600(VDDQ)=48.3% */
#define TX_VREF_RANGE_END2	(27 + 5)
#endif

typedef enum {
	DRAM_OK = 0,
	DRAM_FAIL,
} DRAM_STATUS_T;     /*  DRAM status type */

typedef enum
{
	DIV8_MODE = 0,
	DIV4_MODE,
	UNKNOWN_MODE,
} DIV_MODE_T;
typedef enum {
	CKE_FIXOFF = 0,
	CKE_FIXON,
	/*
	 * After CKE FIX on/off,
	 * CKE should be returned to dynamic (control by HW)
	 */
	CKE_DYNAMIC
} CKE_FIX_OPTION;

typedef enum {
	/* just need to write CKE FIX register to current channel */
	CKE_WRITE_TO_ONE_CHANNEL = 0,
	/* need to write CKE FIX register to all channel */
	CKE_WRITE_TO_ALL_CHANNEL
} CKE_FIX_CHANNEL;

typedef enum {
	AD_MPLL_208M_CK = 0,
	DA_MPLL_52M_DIV_CK,
	FMEM_CK_BFE_DCM_CH0,
} CLOCK_SRC_T;

typedef enum {
	DLL_MASTER = 0,
	DLL_SLAVE,
} DRAM_DLL_MODE_T;

/* Do not change */
typedef enum {
	DDR_DDR1066 = 0,
	DDR_DDR1200,
	DDR_DDR1333,
	DDR_DDR1600,
	DDR_DDR1866,
	DDR_DDR2400,
	DDR_DDR2666,
	DDR_DDR2800,
	DDR_DDR3200,
	DDR_DDR3733,
	DDR_FREQ_MAX,
} DRAM_PLL_FREQ_SEL_T; /*  DRAM DFS type */

typedef enum {
	DRAM_DFS_SHUFFLE_1 = 0,
	DRAM_DFS_SHUFFLE_2,
	DRAM_DFS_SHUFFLE_3,
	DRAM_DFS_SHUFFLE_MAX
} DRAM_DFS_SHUFFLE_TYPE_T;	/*  DRAM SHUFFLE RG type */

typedef struct _DRAM_DFS_FREQUENCY_TABLE_T {
	DRAM_PLL_FREQ_SEL_T freq_sel;
	DIV_MODE_T divmode;
	unsigned short frequency;
	DRAM_DFS_SHUFFLE_TYPE_T shuffleIdx;
} DRAM_DFS_FREQUENCY_TABLE_T;

typedef enum {
	DRAM_CALIBRATION_ZQ = 0,
	DRAM_CALIBRATION_SW_IMPEDANCE,
	DRAM_CALIBRATION_CA_TRAIN,
	DRAM_CALIBRATION_WRITE_LEVEL,
	DRAM_CALIBRATION_GATING,
	DRAM_CALIBRATION_DATLAT,
	DRAM_CALIBRATION_RX_RDDQC,
	DRAM_CALIBRATION_RX_PERBIT,
	DRAM_CALIBRATION_TX_PERBIT,
	DRAM_CALIBRATION_MAX
} DRAM_CALIBRATION_STATUS_T;

typedef enum {
	DDRPHY_CONF_A = 0,
	DDRPHY_CONF_B,
	DDRPHY_CONF_C,
	DDRPHY_CONF_D,
	DDRPHY_CONF_MAX
} DDRPHY_CONF_T;

/* cc notes: Virtual!! */
typedef enum {
	CHANNEL_A = 0,
	CHANNEL_B,
	CHANNEL_C,
	CHANNEL_D,
} DRAM_CHANNEL_T;

typedef enum {
	CHANNEL_SINGLE = 1,
	CHANNEL_DUAL,
	CHANNEL_THIRD,
	CHANNEL_FOURTH
} DRAM_CHANNEL_NUMBER_T;

typedef enum {
	RANK_0 = 0,
	RANK_1,
	RANK_MAX
} DRAM_RANK_T;

#define CKE_WRITE_TO_ALL_RANK		(RANK_MAX)
typedef enum {
	RANK_SINGLE = 1,
	RANK_DUAL
} DRAM_RANK_NUMBER_T;

/* Do not change */
typedef enum {
	TYPE_PCDDR3 = 0,
	TYPE_PCDDR4,
	TYPE_LPDDR3,
	TYPE_LPDDR4,
	TYPE_LPDDR4X,
	TYPE_LPDDR4P,
	TYPE_MAX,
} DRAM_DRAM_TYPE_T;

typedef enum {
	PINMUX_TYPE_LP4_EMCP = 0,
	PINMUX_TYPE_LP4_DSC,
	PINMUX_TYPE_LP4_DSC_X16,
	PINMUX_TYPE_LP3_EMCP,
	PINMUX_TYPE_LP3_DSC,
	PINMUX_TYPE_PC4_X16,
	PINMUX_TYPE_PC4_X16X2,
	PINMUX_TYPE_PC3_X16,
	PINMUX_TYPE_PC3_X16X2,
	PINMUX_TYPE_PC3_X8X4,
} DRAM_PINMUX_TYPE_T;

/*  For faster switching between term and un-term operation
 * FSP_0: For un-terminated freq.
 * FSP_1: For terminated freq.
 */
typedef enum {
	FSP_0 = 0,
	FSP_1,
	FSP_MAX
} DRAM_FAST_SWITH_POINT_T;

/*
 * Internal CBT mode enum
 * 1. Calibration flow uses get_dram_cbt_mode to
 *    differentiate between mixed vs non-mixed LP4
 * 2. Declared as dram_cbt_mode[RANK_MAX] internally to
 *    store each rank's CBT mode type
 */
typedef enum {
	CBT_NORMAL_MODE = 0,
	CBT_BYTE_MODE1
} DRAM_CBT_MODE_T;

/*
 * External CBT mode enum
 * Due to MDL structure compatibility (single field for dram CBT mode),
 * the below enum is used in preloader to differentiate between dram cbt modes
 */
typedef enum {
	CBT_R0_R1_NORMAL = 0,		/*  Normal mode */
	CBT_R0_R1_BYTE,		/*  Byte mode */
	CBT_R0_NORMAL_R1_BYTE,	/*  Mixed mode R0: Normal R1: Byte */
	CBT_R0_BYTE_R1_NORMAL	/*  Mixed mode R0: Byte R1: Normal */
} DRAM_CBT_MODE_EXTERN_T;

typedef enum {
	ODT_OFF = 0,
	ODT_ON
} DRAM_ODT_MODE_T;

typedef enum {
	DBI_OFF = 0,
	DBI_ON
} DRAM_DBI_MODE_T;

typedef enum {
	DATA_WIDTH_16BIT = 16,
	DATA_WIDTH_32BIT = 32
} DRAM_DATA_WIDTH_T;

typedef enum {
	GET_MDL_USED = 0,
	NORMAL_USED
} DRAM_INIT_USED_T;

typedef enum {
	MODE_1X = 0,
	MODE_2X
} DRAM_DRAM_MODE_T;

typedef enum {
	PACKAGE_SBS = 0,
	PACKAGE_POP
} DRAM_PACKAGE_T;

typedef enum {
	TE_OP_WRITE_READ_CHECK = 0,
	TE_OP_READ_CHECK
} DRAM_TE_OP_T;

typedef enum {
	TEST_ISI_PATTERN = 0,   /* don't change */
	TEST_AUDIO_PATTERN = 1, /* don't change */
	TEST_XTALK_PATTERN = 2, /* don't change */
} DRAM_TEST_PATTERN_T;

typedef enum {
	BL_TYPE_4 = 0,
	BL_TYPE_8
} DRAM_BL_TYPE_T;

typedef enum {
	fcDATLAT_USE_DEFAULT = 0,
	fcDATLAT_USE_RX_SCAN,
} DRAM_DATLAT_CALIBRATION_TYTE_T;

typedef enum {
	TX_DQ_DQS_MOVE_DQ_ONLY = 0,
	TX_DQ_DQS_MOVE_DQM_ONLY,
	TX_DQ_DQS_MOVE_DQ_DQM
} DRAM_TX_PER_BIT_CALIBRATION_TYTE_T;

typedef enum {
	TX_DQM_WINDOW_SPEC_IN = 0xfe,
	TX_DQM_WINDOW_SPEC_OUT = 0xff
} DRAM_TX_PER_BIT_DQM_WINDOW_RESULT_TYPE_T;

typedef enum {
	VREF_RANGE_0 = 0,
	VREF_RANGE_1,
	VREF_RANGE_MAX
} DRAM_VREF_RANGE_T;

typedef struct _REG_TRANSFER {
	unsigned int addr;
	unsigned int fld;
} REG_TRANSFER_T;

/*
 * enum for CKE toggle mode
 * (toggle both ranks
 * 1. at the same time (CKE_RANK_DEPENDENT)
 * 2. individually (CKE_RANK_INDEPENDENT))
 */
typedef enum {
	CKE_RANK_INDEPENDENT = 0,
	CKE_RANK_DEPENDENT
} CKE_CTRL_MODE_T;

typedef enum {
	channel_density_2Gb = 0,
	channel_density_3Gb,
	channel_density_4Gb,
	channel_density_6Gb,
	channel_density_8Gb,
	channel_density_12Gb,
	channel_density_16Gb,
	channel_density_number,
} DRAM_SUPPORT_DENSITY;

typedef enum {
    DRAM_ADC_SIZE_LEVEL_0 = 0,
    DRAM_ADC_SIZE_LEVEL_1,
    DRAM_ADC_SIZE_LEVEL_2,
    DRAM_ADC_SIZE_LEVEL_3,
    DRAM_ADC_SIZE_LEVEL_4,
    DRAM_ADC_SIZE_MAX
}DRAM_ADC_SIZE_T;
typedef enum
{ //p->pinmux ID from MDL table
    PIN_MUX_TYPE_DDR3X16X2 = 0,
    PIN_MUX_TYPE_DDR4X16X2,
    PIN_MUX_TYPE_LPDDR3_178BALL,
    PIN_MUX_TYPE_LPDDR3_168BALL,
    PIN_MUX_TYPE_DDR3X8,
    PIN_MUX_TYPE_LPDDR3_211BALL,
    PIN_MUX_TYPE_LPDDR3_eMCP_1,
    PIN_MUX_TYPE_LPDDR3_eMCP_MMD,
    PIN_MUX_TYPE_DDR3X4,
    PIN_MUX_TYPE_MAX
} PIN_MUX_TYPE_T;

typedef enum
{
    DRAM_COMSETTING_DEFAULT = 0,
    DRAM_COMSETTING_DDR3_X4,
    DRAM_COMSETTING_DDR3_X8,
    DRAM_COMSETTING_DDR3_X16,
    DRAM_COMSETTING_DDR3_X32,
    DRAM_COMSETTING_DDR4_X16,
    DRAM_COMSETTING_DDR4_X32,
    DRAM_COMSETTING_LP3_178BALL,
    DRAM_COMSETTING_LP3_168BALL,
    DRAM_COMSETTING_DDR3_ASYM,
    DRAM_COMSETTING_DDR4_ASYM,
    DRAM_COMSETTING_MAX
} DRAM_COMMONSETTING_T;

typedef enum
{
    DutyScan_Calibration_K_CLK= 0,
    DutyScan_Calibration_K_DQS,
    DutyScan_Calibration_K_DQ,
    DutyScan_Calibration_K_DQM
}DUTYSCAN_CALIBRATION_FLOW_K_T;

/*
 * Definitions to enable specific freq's
 * ACTiming support (To save code size)
 */

#if (LPDDR4_DIV4_MODE_EN == 0)
#define SUPPORT_LP4_DDR3200_ACTIM 1
#define SUPPORT_LP4_DDR2666_ACTIM 1
#define SUPPORT_LP4_DDR2400_ACTIM 0
#define SUPPORT_LP4_DDR1600_ACTIM 1
#else
#define SUPPORT_LP4_DDR1600_ACTIM 1
#define SUPPORT_LP4_DDR1333_ACTIM 1
#define SUPPORT_LP4_DDR1200_ACTIM 1
#endif

#define SUPPORT_LP3_DDR1866_ACTIM 0
#define SUPPORT_LP3_DDR1600_ACTIM 1
#define SUPPORT_LP3_DDR1333_ACTIM 1
#define SUPPORT_LP3_DDR1200_ACTIM 1

#if (PCDDR4_DIV4_MODE_EN == 0)
#define SUPPORT_PC4_DDR3200_ACTIM 1
#define SUPPORT_PC4_DDR2667_ACTIM 1
#else
#define SUPPORT_PC4_DDR1600_ACTIM 1
#define SUPPORT_PC4_DDR1333_ACTIM 1
#endif

#define SUPPORT_PC3_DDR1866_ACTIM 0
#define SUPPORT_PC3_DDR1600_ACTIM 1
#define SUPPORT_PC3_DDR1333_ACTIM 1

/*
 *  Used to keep track the total number of LP4 ACTimings
 *  Since READ_DBI is enable/disabled using preprocessor C define
 * -> Save code size by excluding unneeded ACTimingTable entries
 * Note 1: READ_DBI on/off is for (LP4 data rate >= DDR2667 (FSP1))
 * Must make sure DDR3733 is the 1st entry (DMCATRAIN_INTV is used)
 */
typedef enum {
	AC_TIME_NON_USED = 0,
#if SUPPORT_TYPE_LPDDR4
#if (LPDDR4_DIV4_MODE_EN == 0) /* DIV8 */
#if SUPPORT_LP4_DDR3200_ACTIM
#if ENABLE_READ_DBI
	AC_TIME_LP4_DIV8_BYTE_DDR3200_RDBI_ON,
	AC_TIME_LP4_DIV8_NORM_DDR3200_RDBI_ON,
#else
	AC_TIME_LP4_DIV8_BYTE_DDR3200_RDBI_OFF,
	AC_TIME_LP4_DIV8_NORM_DDR3200_RDBI_OFF,
#endif
#endif
#if SUPPORT_LP4_DDR2666_ACTIM
#if ENABLE_READ_DBI
	AC_TIME_LP4_DIV8_BYTE_DDR2666_RDBI_ON,
	AC_TIME_LP4_DIV8_NORM_DDR2666_RDBI_ON,
#else
	AC_TIME_LP4_DIV8_BYTE_DDR2666_RDBI_OFF,
	AC_TIME_LP4_DIV8_NORM_DDR2666_RDBI_OFF,
#endif
#endif
#if SUPPORT_LP4_DDR2400_ACTIM
	AC_TIME_LP4_DIV8_BYTE_DDR2400_RDBI_OFF,
	AC_TIME_LP4_DIV8_NORM_DDR2400_RDBI_OFF,
#endif

#if SUPPORT_LP4_DDR1600_ACTIM
	AC_TIME_LP4_DIV8_BYTE_DDR1600_RDBI_OFF,
	AC_TIME_LP4_DIV8_NORM_DDR1600_RDBI_OFF,
#endif
#else /* DIV4 */
#if SUPPORT_LP4_DDR1600_ACTIM
	AC_TIME_LP4_DIV4_BYTE_DDR1600_RDBI_OFF,
	AC_TIME_LP4_DIV4_NORM_DDR1600_RDBI_OFF,
#endif
#if SUPPORT_LP4_DDR1333_ACTIM
	AC_TIME_LP4_DIV4_BYTE_DDR1333_RDBI_OFF,
	AC_TIME_LP4_DIV4_NORM_DDR1333_RDBI_OFF,
#endif
#if SUPPORT_LP4_DDR1200_ACTIM
	AC_TIME_LP4_DIV4_BYTE_DDR1200_RDBI_OFF,
	AC_TIME_LP4_DIV4_NORM_DDR1200_RDBI_OFF,
#endif
#endif /* LPDDR4_DIV4_MODE_EN */
#endif /* SUPPORT_TYPE_LPDDR4 */
/* ----------- PCDDR4 ------------*/
#if SUPPORT_TYPE_PCDDR4
#if SUPPORT_PC4_DDR3200_ACTIM
	AC_TIME_PC4_NORM_DDR3200,
#endif
#if SUPPORT_PC4_DDR2667_ACTIM
	AC_TIME_PC4_NORM_DDR2667,
#endif
#if SUPPORT_PC4_DDR1600_ACTIM
	AC_TIME_PC4_NORM_DDR1600,
#endif
#if SUPPORT_PC4_DDR1333_ACTIM
	AC_TIME_PC4_NORM_DDR1333,
#endif
#endif /* SUPPORT_TYPE_PCDDR4 */
/* ----------- LPDDR3 ------------*/
#if SUPPORT_TYPE_LPDDR3
#if SUPPORT_LP3_DDR1866_ACTIM
	AC_TIME_LP3_NORM_DDR1866,
#endif
#if SUPPORT_LP3_DDR1600_ACTIM
	AC_TIME_LP3_NORM_DDR1600,
#endif
#if SUPPORT_LP3_DDR1333_ACTIM
	AC_TIME_LP3_NORM_DDR1333,
#endif
#if SUPPORT_LP3_DDR1200_ACTIM
	AC_TIME_LP3_NORM_DDR1200,
#endif
#endif /* SUPPORT_TYPE_LPDDR3 */
/* ----------- PCDDR3 ------------*/
#if SUPPORT_TYPE_PCDDR3
#if SUPPORT_PC3_DDR1866_ACTIM
	AC_TIME_PC3_NORM_DDR1866,
#endif
#if SUPPORT_PC3_DDR1600_ACTIM
	AC_TIME_PC3_NORM_DDR1600,
#endif
#if SUPPORT_PC3_DDR1333_ACTIM
	AC_TIME_PC3_NORM_DDR1333,
#endif
#endif /* SUPPORT_TYPE_PCDDR3 */
	AC_TIMING_NUMBER
} AC_TIMING_COUNT_TYPE_T;

typedef enum {
	TA2_RKSEL_XRT = 3,
	TA2_RKSEL_HW = 4,
} TA2_RKSEL_TYPE_T;

typedef enum {
	TA2_PAT_SWITCH_OFF = 0,
	TA2_PAT_SWITCH_ON,
} TA2_PAT_SWITCH_TYPE_T;

typedef enum {
	GW_MODE_NORMAL = 0,
	GW_MODE_7UI,
	GW_MODE_8UI,
} GW_MODE_TYPE_T;

enum {
	TERM_TYPE_DRVP = 0,
	TERM_TYPE_DRVN,
	TERM_TYPE_ODTP,
	TERM_TYPE_ODTN,
	TERM_TYPE_NUM,
};

#define TERM_OPTION_NUM	2

#define TOTAL_AC_TIMING_NUMBER		(AC_TIMING_NUMBER - 1)

#if SUPPORT_TYPE_PCDDR4
#define CFG_DRAM_CALIB_OPTIMIZATION		1
#endif

#if !DRAM_ETT
// Preloader: using config CFG_DRAM_CALIB_OPTIMIZATION to identify
#if (FOR_DV_SIMULATION_USED==0)
// Preloader: using config CFG_DRAM_CALIB_OPTIMIZATION to identify
#ifdef DRAM_SLT
#define CFG_DRAM_CALIB_OPTIMIZATION		0 /*slt bypass fast k*/
#else
#define CFG_DRAM_CALIB_OPTIMIZATION		(SUPPORT_TYPE_LPDDR4 | SUPPORT_TYPE_PCDDR4)
#endif
#define SUPPORT_SAVE_TIME_FOR_CALIBRATION		CFG_DRAM_CALIB_OPTIMIZATION
#else
// DV simulation, use full calibration flow
#define SUPPORT_SAVE_TIME_FOR_CALIBRATION		0
#endif
#define EMMC_READY CFG_DRAM_CALIB_OPTIMIZATION
#if SUPPORT_TYPE_PCDDR4
#define FEACTURE_EN 0
#define BYPASS_VREF_CAL		(SUPPORT_SAVE_TIME_FOR_CALIBRATION)
#else
#define FEACTURE_EN CFG_DRAM_CALIB_OPTIMIZATION
#define BYPASS_VREF_CAL		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#endif
#define BYPASS_CBT		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_JME		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_IMP		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_DUTY		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_DATLAT		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_WRITELEVELING	(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_RDDQC		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_RXWINDOW		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_TXWINDOW		(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_GatingCal	(SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_CA_PER_BIT_DELAY_CELL (SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#define BYPASS_TX_PER_BIT_DELAY_CELL (SUPPORT_SAVE_TIME_FOR_CALIBRATION & FEACTURE_EN)
#else
// ETT
#define SUPPORT_SAVE_TIME_FOR_CALIBRATION 0
#define EMMC_READY 0
#define BYPASS_VREF_CAL 1
#define BYPASS_CBT 1
#define BYPASS_DATLAT 1
#define BYPASS_WRITELEVELING 1
#define BYPASS_RDDQC 1
#define BYPASS_RXWINDOW 1
#define BYPASS_TXWINDOW 1
#define BYPASS_GatingCal 1
#define BYPASS_CA_PER_BIT_DELAY_CELL CA_PER_BIT_DELAY_CELL
//#define BYPASS_TX_PER_BIT_DELAY_CELL 0
//for DRAM calibration data stored in emmc trial-run: disable LP3 to reduce code size for msdc driver
//#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
//#undef ENABLE_LP3_SW
//#define ENABLE_LP3_SW 0
//#endif
#endif

#define DQS_NUMBER_LP4   2
#define DQS_BIT_NUMBER  8
#define RUNTIME_SHMOO_RELEATED_FUNCTION     CFG_DRAM_SAVE_FOR_RUNTIME_SHMOO
#define RUNTIME_SHMOO_RG_BACKUP_NUM         (200)
#define CA_PER_BIT_DELAY_CELL 1 //LP4

/* Samsung's 1xnm LP4(X) uses nWR & RL settings to determine the freq range it operates at
 * Frequency info determined by nWR & RL is used to adjust write-related internal timing(Write Pulse Width = WCSL) other than auto-precharge
 * For all samsung LP4 dram 1. Fix nWR to 30    2. tWTR@800Mhz: "original tWTR" + 2tCK
 */
#define DDR_VENDOR_SAMSUNG 1
#define SAMSUNG_LP4_NWR_WORKAROUND 1

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
#if RUNTIME_SHMOO_RELEATED_FUNCTION
typedef struct _RUNTIME_SHMOO_SAVE_PARAMETER_T
{
    U8 flag;
    U16 TX_PI_delay;
    U16 TX_Original_PI_delay;
    U16 TX_DQM_PI_delay;
    U16 TX_Original_DQM_PI_delay;
    U8 TX_Vref_Range;
    U8 TX_Vref_Value;
    U8 TX_Channel;
    U8 TX_Rank;
    U8 TX_Byte;
} RUNTIME_SHMOO_SAVE_PARAMETER_T;
#endif

typedef struct _SAVE_TIME_FOR_CALIBRATION_T
{
    //U8 femmc_Ready;

    // Calibration or not
    //U8 Bypass_TXWINDOW;
    //U8 Bypass_RXWINDOW;
    //U8 Bypass_RDDQC;

    // delay cell time
    //U8 ucg_num_dlycell_perT_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];
    //U16 u2gdelay_cell_ps_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];
    U8 u1RankNum;
    U8 ucnum_dlycell_perT;
    U16 u2DelayCellTimex100;

    // CLK & DQS duty
    S8 s1ClockDuty_clk_delay_cell[CHANNEL_NUM][RANK_MAX];
    U8 u1clk_use_rev_bit;
    S8 s1DQSDuty_clk_delay_cell[CHANNEL_NUM][DQS_NUMBER_LP4];
    U8 u1dqs_use_rev_bit;

    // CBT
    U8 u1CBTVref_Save[CHANNEL_NUM][RANK_MAX];
    U8 u1CBTClkDelay_Save[CHANNEL_NUM][RANK_MAX];
    U8 u1CBTCmdDelay_Save[CHANNEL_NUM][RANK_MAX];
    U8 u1CBTCsDelay_Save[CHANNEL_NUM][RANK_MAX];
    #if CA_PER_BIT_DELAY_CELL
    U8 u1CBTCA_PerBit_DelayLine_Save[CHANNEL_NUM][RANK_MAX][DQS_BIT_NUMBER];
    #endif

    // Write leveling
    U8 u1WriteLeveling_bypass_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];  //for bypass writeleveling

    // Gating
    U8 u1Gating2T_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gating05T_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gatingfine_tune_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gating2T_Save_P1[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gating05T_Save_P1[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gatingfine_tune_Save_P1[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1Gatingucpass_count_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];

    // TX perbit
    U8 u1TxWindowPerbitVref_Save[CHANNEL_NUM][RANK_MAX];
    U16 u1TxCenter_min_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U16 u1TxCenter_max_Save[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U16 u1Txwin_center_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];
    U16 u1Txfirst_pass_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];
    U16 u1Txlast_pass_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];
    U8 u1TX_PerBit_DelayLine_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];

    // Datlat
    U8 u1RxDatlat_Save[CHANNEL_NUM][RANK_MAX];

    // RX perbit
    U8 u1RxWinPerbitVref_Save[CHANNEL_NUM];
    U8 u1RxWinPerbit_DQS[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1RxWinPerbit_DQM[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1RxWinPerbit_DQ[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];
    S16 u1RxWinPerbitDQ_firsbypass_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];  //for bypass rxwindow
    S16 u1RxWinPerbitDQ_lastbypass_Save[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH_LP4];  //for bypass rxwindow

    //TX OE
    U8 u1TX_OE_DQ_MCK[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];
    U8 u1TX_OE_DQ_UI[CHANNEL_NUM][RANK_MAX][DQS_NUMBER_LP4];

#if RUNTIME_SHMOO_RELEATED_FUNCTION
    U8 u1SwImpedanceResule[2][4];
    U32 u4TXRG_Backup[CHANNEL_NUM][RUNTIME_SHMOO_RG_BACKUP_NUM];

    RUNTIME_SHMOO_SAVE_PARAMETER_T Runtime_Shmoo_para;
#endif
} SAVE_TIME_FOR_CALIBRATION_T;

extern int write_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData);
extern int read_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData);
//3733, 3200, 2667, 2400, 2280, 1600, 800
extern U8 _MappingFreqArray[];
#endif


typedef struct _DRAMC_CTX_T {
	DRAM_CHANNEL_NUMBER_T support_channel_num;
	DRAM_CHANNEL_T channel;
	DRAM_RANK_NUMBER_T support_rank_num;
	DRAM_RANK_T rank;
	DRAM_PLL_FREQ_SEL_T freq_sel;
	DRAM_DFS_SHUFFLE_TYPE_T shu_type;
	DRAM_DFS_FREQUENCY_TABLE_T *dfs_table;
	DRAM_DRAM_TYPE_T dram_type;
	DRAM_PINMUX_TYPE_T pinmux_type;
	DRAM_FAST_SWITH_POINT_T dram_fsp;
	DRAM_ODT_MODE_T odt_onoff;
	DRAM_CBT_MODE_T dram_cbt_mode[RANK_MAX];
	DRAM_DBI_MODE_T dbi_r_onoff[FSP_MAX];
	DRAM_DBI_MODE_T dbi_w_onoff[FSP_MAX];
	DRAM_DATA_WIDTH_T data_width;
	unsigned int test2_1;
	unsigned int test2_2;
	DRAM_TEST_PATTERN_T test_pattern;
	unsigned short frequency;
	unsigned short freqGroup;
	unsigned short shuffle_frequency[DRAM_DFS_SHUFFLE_MAX];
	unsigned short vendor_id;
	unsigned short revision_id;
	unsigned short density;
	unsigned long long ranksize[RANK_MAX];
	unsigned char ucnum_dlycell_perT;
	unsigned short delay_cell_timex100;
	unsigned char enable_cbt_scan_vref;
	unsigned char enable_rx_scan_vref;
	unsigned char enable_tx_scan_vref;
	unsigned char ssc_en;
	unsigned char en_4bit_mux;
#if PRINT_CALIBRATION_SUMMARY
	unsigned int cal_result_flag[CHANNEL_MAX][RANK_MAX];
	unsigned int cal_execute_flag[CHANNEL_MAX][RANK_MAX];
#endif
	unsigned char arfg_write_leveling_init_shif[CHANNEL_MAX][RANK_MAX];
	unsigned char fg_tx_perbif_init[CHANNEL_MAX][RANK_MAX];
	#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
    U8 femmc_Ready;
    // Calibration or not
    U8 Bypass_TXWINDOW;
    U8 Bypass_RXWINDOW;
    U8 Bypass_RDDQC;
    SAVE_TIME_FOR_CALIBRATION_T *pSavetimeData;
    #endif
	unsigned char is_emcp;
} DRAMC_CTX_T;

typedef struct _PASS_WIN_DATA_T {
	signed short first_pass;
	signed short last_pass;
	signed short win_center;
	unsigned short win_size;
	unsigned short best_dqdly;
} PASS_WIN_DATA_T;

typedef struct _RX_DELAY_SET_PERBIT_T {
	signed int dqs_dly_perbyte[DQS_NUMBER];
	signed int dqm_dly_perbyte[DQS_NUMBER];
} RX_DELAY_SET_PERBIT_T;


typedef struct _FINAL_WIN_DATA_T {
	unsigned char final_vref;
	signed int final_ca_clk;
	unsigned char final_range;
	unsigned int ca_win_sum_max;
	unsigned short ca_perbit_win_min_max;
	unsigned short ca_perbit_win_min_max_idx;
} FINAL_WIN_DATA_T;

typedef struct _SCAN_WIN_DATA_T {
	unsigned int finish_count;
	unsigned int ca_win_sum;
	signed int ca_center_sum;
	signed int ca_center[CATRAINING_NUM_MAX];
	signed int first_ca_pass[CATRAINING_NUM_MAX];
	signed int last_ca_pass[CATRAINING_NUM_MAX];
} SCAN_WIN_DATA_T;

typedef struct _TXDQS_PERBIT_DLY_T {
	signed char first_dqdly_pass;
	signed char last_dqdly_pass;
	signed char first_dqsdly_pass;
	signed char last_dqsdly_pass;
	unsigned char best_dqdly;
	unsigned char best_dqsdly;
} TXDQS_PERBIT_DLY_T;

typedef struct _RXDQS_GATING_TRANS_T {
	unsigned char dqs_lead[DQS_NUMBER];
	unsigned char dqs_lag[DQS_NUMBER];
	unsigned char dqs_high[DQS_NUMBER];
	unsigned char dqs_transition[DQS_NUMBER];
	unsigned char dly_coarse_large_leadLag[DQS_NUMBER];
	unsigned char dly_coarse_0p5t_leadLag[DQS_NUMBER];
	unsigned char dly_fine_tune_leadLag[DQS_NUMBER];
} RXDQS_GATING_TRANS_T;

typedef struct _RXDQS_GATING_WIN_T {
	unsigned char pass_begin[DQS_NUMBER];
	unsigned char pass_count[DQS_NUMBER];
	unsigned char min_coarse_tune2t[DQS_NUMBER];
	unsigned char min_coarse_tune0p5t[DQS_NUMBER];
	unsigned char min_fine_tune[DQS_NUMBER];
	unsigned char pass_count_1[DQS_NUMBER];
	unsigned char pass_count_2[DQS_NUMBER];
	unsigned char min_coarse_tune2t_1[DQS_NUMBER];
	unsigned char min_coarse_tune0p5t_1[DQS_NUMBER];
	unsigned char min_fine_tune_1[DQS_NUMBER];
} RXDQS_GATING_WIN_T;

typedef struct _RXDQS_GATING_BEST_WIN_T {
	unsigned char best_fine_tune[DQS_NUMBER];
	unsigned char best_coarse_tune0p5t[DQS_NUMBER];
	unsigned char best_coarse_tune2t[DQS_NUMBER];
	unsigned char best_fine_tune_p1[DQS_NUMBER];
	unsigned char best_coarse_tune0p5t_p1[DQS_NUMBER];
	unsigned char best_coarse_tune2t_p1[DQS_NUMBER];
} RXDQS_GATING_BEST_WIN_T;

typedef struct _RXDQS_GATING_CAL_T {
	unsigned char dly_coarse_0p5t;
	unsigned char dly_coarse_large;
	unsigned char dly_fine_xt;
	unsigned char dqs_gw_fine_step;
	unsigned char dly_coarse_large_p1;
	unsigned char dly_coarse_0p5t_p1;
} RXDQS_GATING_CAL_T;

typedef struct _TX_DLY_T {
	unsigned char dq_final_pi[DQS_NUMBER];
	unsigned char dq_final_ui_large[DQS_NUMBER];
	unsigned char dq_final_ui_small[DQS_NUMBER];
	unsigned char dq_final_oen_ui_large[DQS_NUMBER];
	unsigned char dq_final_oen_ui_small[DQS_NUMBER];
} TX_DLY_T;

typedef struct _TX_FINAL_DLY_T {
	unsigned char dq_final_dqm_pi[DQS_NUMBER];
	unsigned char dq_final_dqm_ui_large[DQS_NUMBER];
	unsigned char dq_final_dqm_ui_small[DQS_NUMBER];
	unsigned char dq_final_dqm_oen_ui_large[DQS_NUMBER];
	unsigned char dq_final_dqm_oen_ui_small[DQS_NUMBER];
} TX_FINAL_DLY_T;

typedef struct _DRAM_INFO_BY_MRR_T {
	unsigned short mr5_vendor_id;
	unsigned short mr6_vevision_id;
	unsigned long long mr8_density[CHANNEL_NUM][RANK_MAX];
} DRAM_INFO_BY_MRR_T;

typedef struct _VCORE_DELAYCELL_T {
	unsigned short u2Vcore;
	unsigned short u2DelayCell;
} VCORE_DELAYCELL_T;

/* cc notes: Initially defined for LP4.
 * Reused in this project for all types
 */
typedef struct _MR_SET_VALUE_T {
	unsigned short mr00_value[FSP_MAX]; /* cc add for PCDDR3/4 use */
	unsigned short mr01_value[FSP_MAX];
	unsigned short mr02_value[FSP_MAX];
	unsigned short mr03_value[FSP_MAX];
	unsigned short mr04_value[FSP_MAX]; /* cc add for PCDDR4 */
	unsigned short mr05_value[FSP_MAX]; /* cc add for PCDDR4 */
	unsigned short mr06_value[FSP_MAX]; /* cc add for PCDDR4 */
	unsigned short mr10_value[FSP_MAX]; /* cc add for LP3 */
	unsigned short mr12_value[CHANNEL_NUM][RANK_MAX][FSP_MAX];
	unsigned short mr13_value[FSP_MAX];
	unsigned short mr14_value[CHANNEL_NUM][RANK_MAX][FSP_MAX];
	unsigned short mr18_value[CHANNEL_NUM][RANK_MAX];
	unsigned short mr19_value[CHANNEL_NUM][RANK_MAX];
	unsigned short mr23_value[CHANNEL_NUM][RANK_MAX];
	unsigned short mr63_value[FSP_MAX]; /* cc add for LP3 */
} MR_SET_VALUE_T;

/* For new register access */
#define DRAMC_REG_ADDR(offset)    ((p->channel << POS_BANK_NUM)+(offset))
#define SYS_REG_ADDR(offset)    (offset)

/***********************************************************************/
/*               Public Functions                                       */
/***********************************************************************/
/*  basic function */
unsigned char is_lp4_family(DRAMC_CTX_T *p);
unsigned char is_byte_swapped(DRAMC_CTX_T *p);
/*  Used to support freq's not in ACTimingTable */
void set_freq_group(DRAMC_CTX_T *p);
void dramc_init_pre_settings(DRAMC_CTX_T *p);

DRAM_STATUS_T dramc_init(DRAMC_CTX_T *p);

void dramc_setting_init(DRAMC_CTX_T *p);
void auto_refresh_cke_off(DRAMC_CTX_T *p);
void dvfs_settings(DRAMC_CTX_T *p);
void set_mr13_vrcg_to_normal_operation(DRAMC_CTX_T *p);
DRAM_STATUS_T ddr_update_ac_timing(DRAMC_CTX_T *p);

void dramc_set_rank_engine2(DRAMC_CTX_T *p, unsigned char u1RankSel);
DRAM_STATUS_T dramc_engine2_init(DRAMC_CTX_T *p, unsigned int test2_1,
	unsigned int test2_2, unsigned char testaudpat,
	unsigned char log2loopcount);
unsigned int dramc_engine2_run(DRAMC_CTX_T *p, DRAM_TE_OP_T wr,
	unsigned char testaudpat);
void dramc_engine2_end(DRAMC_CTX_T *p);
void dramc_run_time_config(DRAMC_CTX_T *p);

void transfer_to_spm_control(DRAMC_CTX_T *p);
void transfer_pll_to_spm_control(DRAMC_CTX_T *p);
void spm_pinmux_setting(DRAMC_CTX_T *p);

void enable_dramc_phy_dcm(DRAMC_CTX_T *p, unsigned char en);

void set_dram_mr_write_leveling_on_off(DRAMC_CTX_T *p, unsigned char on_off);
void move_dramc_tx_dqs(DRAMC_CTX_T *p, unsigned char byte_idx,
	signed char shift_ui);
void cke_fix_on_off(DRAMC_CTX_T *p, unsigned char rank, CKE_FIX_OPTION option,
	CKE_FIX_CHANNEL write_channel_num);
/*
 * Control CKE toggle mode
 * (toggle both ranks
 * 1. at the same time (CKE_RANK_DEPENDENT)
 * 2. individually (CKE_RANK_INDEPENDENT))
 */
void cke_rank_ctrl(DRAMC_CTX_T *p, CKE_CTRL_MODE_T cke_ctrl_mode);
/*
 * rx_dqs_isi_pulse_cg() -
 * API for "RX DQS ISI pulse CG function" 0: disable, 1: enable
 */
void rx_dqs_isi_pulse_cg(DRAMC_CTX_T *p, unsigned char on_off);
DRAM_STATUS_T DramcRegDump(DRAMC_CTX_T *p);
void dramc_mrr_by_rank(DRAMC_CTX_T *p, unsigned char rank,
	unsigned char mr_idx, unsigned short *value_p);
void dramc_mode_reg_read(DRAMC_CTX_T *p, unsigned char mr_idx,
	unsigned short *value);
void dramc_mode_reg_write(DRAMC_CTX_T *p, unsigned char mr_idx,
	unsigned short value);
void dram_phy_reset(DRAMC_CTX_T *p);
unsigned char get_mr4_refresh_rate(DRAMC_CTX_T *p, DRAM_CHANNEL_T channel);
/*  mandatory calibration function */
DRAM_STATUS_T dramc_dqsosc_auto(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_start_dqsosc(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcStopDQSOSC(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_zq_calibration(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_sw_impedance_cal(DRAMC_CTX_T *p,
	unsigned char term_option);

void apply_config_before_calibration(DRAMC_CTX_T *p);
void apply_config_after_calibration(DRAMC_CTX_T *p);
void dramc_rxdqs_gating_pre_process(DRAMC_CTX_T *p);
void dramc_rxdqs_gating_post_process(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_rx_window_perbit_cal(DRAMC_CTX_T *p,
	unsigned char use_test_engine);
DRAM_STATUS_T dramc_rx_input_buffer_offset_cal(DRAMC_CTX_T *p);
void dramc_rxdatlat_cal(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_tx_window_perbit_cal(DRAMC_CTX_T *p,
	DRAM_TX_PER_BIT_CALIBRATION_TYTE_T cal_type,
	unsigned char vref_scan_enable);

#if TX_PICG_NEW_MODE
void tx_picg_setting(DRAMC_CTX_T * p);
#endif

unsigned short dfs_get_highest_freq(DRAMC_CTX_T *p);

#if ENABLE_RX_TRACKING_LP4
void dramc_rx_input_delay_tracking_init_by_freq(DRAMC_CTX_T *p);
void dramc_rx_input_delay_tracking_init_common(DRAMC_CTX_T *p);
void dramc_rx_input_delay_tracking_hw(DRAMC_CTX_T *p);
#endif

void dramc_hw_gating_init(DRAMC_CTX_T *p);
void dramc_hw_gating_on_off(DRAMC_CTX_T *p, unsigned char on_off);
void dramc_print_hw_gating_status(DRAMC_CTX_T *p, unsigned char channel);

/*  reference function */
DRAM_STATUS_T dramc_rank_swap(DRAMC_CTX_T *p, unsigned char rank);

/*  dump all reg for debug */
DRAM_STATUS_T cmd_bus_training(DRAMC_CTX_T *p);

void mpll_init(void);

void set_channel_number(DRAMC_CTX_T *p);
void set_rank_number(DRAMC_CTX_T *p);
void set_phy_2_channel_mapping(DRAMC_CTX_T *p, unsigned char u1Channel);
unsigned char get_phy_2_channel_mapping(DRAMC_CTX_T *p);
void set_rank(DRAMC_CTX_T *p, unsigned char ucRank);
unsigned char get_rank(DRAMC_CTX_T *p);

void dramc_hw_dqsosc(DRAMC_CTX_T *p);

void dramc_write_dbi_on_off(DRAMC_CTX_T *p, unsigned char onoff);
void dramc_read_dbi_on_off(DRAMC_CTX_T *p, unsigned char onoff);
void dramc_drs(DRAMC_CTX_T *p, unsigned char enable);

/*
 * Macro that implements ceil
 * function for unsigned integers (Test before using!)
 */
#define CEILING(n, v) (((n)%(v)) ? (((n)/(v)) + 1) : ((n)/(v)))

/* For Fix Compiler Error*/
void global_option_init(DRAMC_CTX_T *p);
int init_dram(DRAM_DRAM_TYPE_T dram_type,
	DRAM_CBT_MODE_EXTERN_T dram_cbt_mode_extern,
	DRAM_INFO_BY_MRR_T *DramInfo, unsigned char get_mdl_used);
void ddr_phy_freq_sel(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T sel);
void set_freq_sel_by_table(DRAMC_CTX_T *p, DRAM_DFS_FREQUENCY_TABLE_T *table);
DIV_MODE_T get_div_mode(DRAMC_CTX_T *p);
void dramc_update_impedance_term_2un_term(DRAMC_CTX_T *p);
void cbt_switch_freq(DRAMC_CTX_T *p, unsigned char freq);
void dramc_sw_impedance_save_register(DRAMC_CTX_T *p,
	unsigned char ca_term_option, unsigned char dq_term_option,
	unsigned char save_to_where);
DRAM_STATUS_T dramc_dual_rank_rxdatlat_cal(DRAMC_CTX_T *p);
void auto_refresh_switch(DRAMC_CTX_T *p, unsigned char option);
void dramc_mode_reg_write_by_rank(DRAMC_CTX_T *p, unsigned char rank,
	unsigned char mr_idx, unsigned short value);
void set_mrr_pinmux_mapping(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_write_leveling(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_rx_dqs_gating_cal(DRAMC_CTX_T *p);
void dramc_ac_timing_optimize(DRAMC_CTX_T *p);
void dramc_dump_debug_info(DRAMC_CTX_T *p);
void set_vcore_by_freq(DRAMC_CTX_T *p);
void set_dram_voltage(DRAMC_CTX_T *p);
void switch_dramc_voltage_to_auto_mode(DRAMC_CTX_T *p);
unsigned int get_dramc_broadcast(void);
void dramc_broadcast_on_off(unsigned int on_off);
void dramc_backup_registers(DRAMC_CTX_T *p, unsigned int *backup_addr,
	unsigned int backup_num);
void dramc_restore_registers(DRAMC_CTX_T *p, unsigned int *restore_addr,
	unsigned int restore_num);
void init_global_variables_by_condition(DRAMC_CTX_T *p);
#if (ENABLE_WRITE_DBI == 1)
void set_dram_mode_reg_for_write_dbi_on_off(DRAMC_CTX_T *p,
	unsigned char onoff);
void dramc_write_minus_1mck_for_write_dbi(DRAMC_CTX_T *p,
	signed char shift_ui);
#endif

#if (ENABLE_READ_DBI == 1)
void enable_dram_mode_reg_for_read_dbi_after_cal(DRAMC_CTX_T *p);
#endif
void print_calibration_result(DRAMC_CTX_T *p);
void apply_write_dbi_power_improve(DRAMC_CTX_T *p, unsigned char onoff);
void dramc_cmd_bus_training_post_process(DRAMC_CTX_T *p);
unsigned char get_pre_miock_jmeter_hqa_used_flag(void);
void dramc_miock_jmeter_hqa(DRAMC_CTX_T *p);
DRAM_CBT_MODE_T get_dram_cbt_mode(DRAMC_CTX_T *p);
void dramc_gating_mode(DRAMC_CTX_T *p, GW_MODE_TYPE_T mode);
void move_dramc_tx_dq_oen(DRAMC_CTX_T *p, unsigned char byte_dx,
	signed char shift_ui);
//void *memset(void *dst, int c, unsigned int n);
void no_parking_on_clrpll(DRAMC_CTX_T *p);
void dramc_tx_set_vref(DRAMC_CTX_T *p, unsigned char vref_range,
	unsigned char vref_value);
DRAM_STATUS_T dramc_dqsosc_mr23(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_dqsosc_set_mr18_mr19(DRAMC_CTX_T *p);
DRAM_STATUS_T dramc_dqsosc_shu_settings(DRAMC_CTX_T *p);
void dramc_save_to_shuffle_reg(DRAMC_CTX_T *p, DRAM_DFS_SHUFFLE_TYPE_T src_rg,
			   DRAM_DFS_SHUFFLE_TYPE_T dst_rg);
void dfs_init_for_calibration(DRAMC_CTX_T *p);
void cbt_dramc_dfs_direct_jump(DRAMC_CTX_T *p, unsigned char shu_level);
DRAM_STATUS_T dramc_mr_init_lp4(DRAMC_CTX_T *p);
void dramc_new_duty_calibration(DRAMC_CTX_T *p);
void dramc_engine2_set_pat(DRAMC_CTX_T *p, unsigned char testaudpat,
	unsigned char log2loopcount,
			unsigned char Use_Len1_Flag);
void dramc_tx_oe_calibration(DRAMC_CTX_T *p);
DRAM_STATUS_T ta2_test_run_time_hw(DRAMC_CTX_T *p);
unsigned char *dramc_get_4bitmux_byte_mapping(DRAMC_CTX_T *p);
unsigned char *dramc_get_phy2dramc_dq_mapping(DRAMC_CTX_T *p);
unsigned char mapping_phy_byte_to_dramc(DRAMC_CTX_T *p,
	unsigned char phy_byte);

void o1_path_on_off(DRAMC_CTX_T *p, unsigned char on_off);
void print_calibration_basic_info(DRAMC_CTX_T *p);
void set_calibration_result(DRAMC_CTX_T *p, unsigned char cal_type,
	unsigned char result);
DRAM_STATUS_T execute_move_dramc_delay(DRAMC_CTX_T *p,
	REG_TRANSFER_T regs[], signed char shift_ui);

/*  External references */
extern void emi_init(DRAMC_CTX_T *p);
extern void emi_init2(void);

extern MR_SET_VALUE_T dram_mr;
extern DRAM_DFS_FREQUENCY_TABLE_T freq_tbl_lp4[DRAM_DFS_SHUFFLE_MAX];
extern DRAM_DFS_FREQUENCY_TABLE_T freq_tbl_lp3[DRAM_DFS_SHUFFLE_MAX];
extern unsigned char print_mode_reg_write;

extern unsigned int dramc_imp_result[TERM_OPTION_NUM][TERM_TYPE_NUM];
extern signed int ca_train_clk_delay[CHANNEL_NUM][RANK_MAX];
extern unsigned int ca_train_cs_delay[CHANNEL_NUM][RANK_MAX];
extern signed int ca_train_cmd_delay[CHANNEL_NUM][RANK_MAX];
/*  LPDDR DQ -> PHY DQ mapping */
#if SUPPORT_TYPE_LPDDR4
extern const unsigned char lpddr4_phy2dramc_byte_mapping_dsc[CHANNEL_NUM][2];
extern const unsigned char lpddr4_phy2dramc_byte_mapping_emcp[CHANNEL_NUM][2];
extern const unsigned char lpddr4_phy2dramc_dq_mapping_dsc[CHANNEL_NUM][16];
extern const unsigned char lpddr4_phy2dramc_dq_mapping_emcp[CHANNEL_NUM][16];
#endif

#if SUPPORT_TYPE_LPDDR3
extern const unsigned char lpddr3_phy_mapping_pop[CHANNEL_NUM][32];
extern const unsigned char lpddr3_4bitmux_byte_mapping[DATA_WIDTH_32BIT];

extern const unsigned char lpddr3_phy2dramc_byte_mapping_dsc[4];
extern const unsigned char lpddr3_phy2dramc_byte_mapping_emcp[4];
extern const unsigned char lpddr3_phy2dramc_dq_mapping_dsc[32];
extern const unsigned char lpddr3_phy2dramc_dq_mapping_emcp[32];
#endif

#if SUPPORT_TYPE_PCDDR4
extern const unsigned char pcddr4_phy2dramc_byte_mapping[CHANNEL_NUM][2];
extern const unsigned char pcddr4_phy2dramc_dq_mapping[CHANNEL_NUM][16];
#endif

#if SUPPORT_TYPE_PCDDR3
extern const unsigned char pcddr3_phy2dramc_byte_mapping[DQS_NUMBER];
extern const unsigned char pcddr3_phy2dramc_dq_mapping_x16[DATA_WIDTH_32BIT];
#endif

extern signed int wl_final_delay[CHANNEL_NUM][DQS_NUMBER];
extern short rx_dqs_duty_offset[CHANNEL_NUM][DQS_NUMBER][EDGE_NUMBER];
#if SIMULATION_WRITE_LEVELING
extern unsigned char wrlevel_done[CHANNEL_NUM];
#endif
#if GATING_ADJUST_TXDLY_FOR_TRACKING
extern unsigned char tx_dly_cal_min[CHANNEL_NUM];
extern unsigned char tx_dly_cal_max[CHANNEL_NUM];
#endif
#if DRAMC_MODEREG_CHECK
void dramc_mode_reg_check(DRAMC_CTX_T *p);
#endif

extern void ddrphy_lpbk_test_ctrl();
extern DRAM_DRAM_TYPE_T get_ddr_type(void);

#endif /* _PI_API_H */
