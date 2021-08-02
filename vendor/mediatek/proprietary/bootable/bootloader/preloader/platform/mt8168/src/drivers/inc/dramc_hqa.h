#ifndef _DRAMC_HQA_H
#define _DRAMC_HQA_H

//================================================
//=============pmic related api for ETT HQA test ==============
//================================================
//#define DRAM_ETT
//#define DRAM_HQA

#ifdef DRAM_HQA
#define NVCORE_NVDRAM
//#define HVCORE_HVDRAM
//#define LVCORE_LVDRAM
//#define HVCORE_LVDRAM
//#define LVCORE_HVDRAM
//#define NVCORE_LVDRAM
//#define NVCORE_HVDRAM
#endif //end #ifdef DRAM_HQA

#if defined(DRAM_HQA) || defined(DRAM_ETT)
#undef CALIBRATION_LOG
#define CALIBRATION_LOG			1
#endif

#ifdef DRAM_ETT
#define CONFIG_EYESCAN_LOG	1
#else
#define CONFIG_EYESCAN_LOG	0
#endif

#if defined(DRAM_HQA)
#define CONFIG_FOR_HQA_REPORT_USED		1
#define CONFIG_FOR_HQA_TEST_USED		1
#else
#define CONFIG_FOR_HQA_REPORT_USED		0
#define CONFIG_FOR_HQA_TEST_USED		0
#endif

typedef enum {
	VOLT_PREFIX_NV = 1,
	VOLT_PREFIX_HV,
	VOLT_PREFIX_LV
}VOLT_PREFIX_T;

typedef enum {
	DRAM_VOL_VMEM = 0,
	DRAM_VOL_VDD, //1.5V@PC3, 1.35V@PC3L, 1.2V@PC4
	DRAM_VOL_VDD1, //1.8V@LP3, 1.8V@LP4
	DRAM_VOL_VDD2, //1.2V@LP3, 1.1V@LP4
	DRAM_VOL_VDDQ, //1.2V@LP3, 1.1V@LP4, 0.6V@LP4x, 1.5V@PC3, 1.35V@PC3L, 1.2V@PC4
	DRAM_VOL_VPP,  //2.5V@PC4
} DRAM_VOL_T;

#if defined(DRAM_HQA) || defined(DRAM_ETT)
enum
{
	ETT_HVCORE_HVDRAM = 1,
	ETT_NVCORE_NVDRAM,
	ETT_LVCORE_LVDRAM,
	ETT_HVCORE_LVDRAM,
	ETT_LVCORE_HVDRAM,
	ETT_NVCORE_LVDRAM,
	ETT_NVCORE_HVDRAM,
	ETT_VCORE_INC,
	ETT_VCORE_DEC,
	ETT_VDRAM_INC,
	ETT_VDRAM_DEC,
	ETT_VCORE_VDRAM_MAX
};

typedef struct _VOLTAGE_VALUE_T
{
	U8 HV;
	U8 NV;
	U8 LV;
}VOLTAGE_VALUE_T;

typedef struct _VOLTAGE_LIST_T
{
	VOLTAGE_VALUE_T vcore;
	VOLTAGE_VALUE_T vdram;
}VOLTAGE_LIST_T;

typedef struct _CRITERIA_SIGNAL_T
{
    U32 CA;
    U32 RX;
    U32 TX;
}CRITERIA_SIGNAL_T;

typedef struct _CRITERIA_LIST_T
{
    CRITERIA_SIGNAL_T HTLV;
    CRITERIA_SIGNAL_T NTNV;
    CRITERIA_SIGNAL_T LTHV;
    CRITERIA_SIGNAL_T BUFF;
}CRITERIA_LIST_T;

typedef enum
{
    ETT_FAIL = 0,
    ETT_WARN,
    ETT_PASS,
    ETT_RET_MAX
}ETT_RESULT_T;
#endif //defined(DRAM_HQA) || defined(DRAM_ETT)

/*------------------- Add for HQA used ------------------------*/
#define LOGI print
#define LOGV print

#define	EYESCAN_LOG_DELAY_MS	0
#define mcSHOW_DBG_MSG(_x_)	do {LOGI _x_ ; delay_ms(EYESCAN_LOG_DELAY_MS);} while (0)
#define mcSHOW_EYESCAN_MSG(_x_) do {LOGI _x_ ;} while (0)
#define mcSHOW_ERR_MSG(_x_)	do {LOGI _x_ ; delay_ms(EYESCAN_LOG_DELAY_MS);} while (0)
#define mcSHOW_DBG_MSG5(_x_)

enum hqa_report_format {
    HQA_REPORT_FORMAT0 = 0,
    HQA_REPORT_FORMAT0_1,
    HQA_REPORT_FORMAT0_2,
    HQA_REPORT_FORMAT1,
    HQA_REPORT_FORMAT2,
    HQA_REPORT_FORMAT3,
    HQA_REPORT_FORMAT4,
    HQA_REPORT_FORMAT5,
    HQA_REPORT_FORMAT6
};

#define VREF_VOLTAGE_TABLE_NUM 51
#define VREF_VOLTAGE_TABLE_TOTAL_NUM 81
#define VENDER_JV_LOG	0
#define VREF_TOTAL_NUM_WITH_RANGE 51+30 //range0 0~50 + range1 21~50
#define EYESCAN_BROKEN_NUM 3
#define EYESCAN_DATA_INVALID 0x7fff

#if (CONFIG_FOR_HQA_TEST_USED == 1)
extern unsigned short gFinalCBTVrefCA[CHANNEL_NUM][RANK_MAX];
extern unsigned short gFinalCBTCA[CHANNEL_NUM][RANK_MAX][10];
extern unsigned short gFinalRXPerbitWin[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH];
extern unsigned short gFinalTXPerbitWin[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH];
extern unsigned short gFinalTXPerbitWin_min_max[CHANNEL_NUM][RANK_MAX];
extern unsigned short gFinalTXPerbitWin_min_margin[CHANNEL_NUM][RANK_MAX];
extern unsigned short gFinalTXPerbitWin_min_margin_bit[CHANNEL_NUM][RANK_MAX];
extern signed char gFinalClkDuty[CHANNEL_NUM];
extern unsigned int gFinalClkDutyMinMax[CHANNEL_NUM][2];
extern signed char gFinalDQSDuty[CHANNEL_NUM][DQS_NUMBER];
extern unsigned int gFinalDQSDutyMinMax[CHANNEL_NUM][DQS_NUMBER][2];
#endif
extern unsigned char gFinalCBTVrefDQ[CHANNEL_NUM][RANK_MAX];
extern unsigned char gFinalRXVrefDQ[CHANNEL_NUM][RANK_MAX];
extern unsigned char gFinalTXVrefDQ[CHANNEL_NUM][RANK_MAX];
extern unsigned char gFinalTXVrefDQRange[CHANNEL_NUM][RANK_MAX];


#if (CONFIG_EYESCAN_LOG == 1)
extern S16  gEyeScan_Min[VREF_TOTAL_NUM_WITH_RANGE][DQ_DATA_WIDTH_LP4][EYESCAN_BROKEN_NUM];
extern S16  gEyeScan_Max[VREF_TOTAL_NUM_WITH_RANGE][DQ_DATA_WIDTH_LP4][EYESCAN_BROKEN_NUM];
extern U16  gEyeScan_CaliDelay[DQS_NUMBER];
extern U8  gEyeScan_WinSize[VREF_TOTAL_NUM_WITH_RANGE][DQ_DATA_WIDTH];
extern S8  gEyeScan_DelayCellPI[DQ_DATA_WIDTH];
extern U8  gEyeScan_ContinueVrefHeight[DQ_DATA_WIDTH];
extern U16  gEyeScan_TotalPassCount[DQ_DATA_WIDTH];
extern U8 gEyeScan_index[DQ_DATA_WIDTH];
extern U8 gu1pass_in_this_vref_flag[DQ_DATA_WIDTH];
#endif
extern unsigned char final_rx_vref_dq[CHANNEL_NUM][RANK_MAX];


extern const U16 gRXVref_Voltage_Table_LP4[RX_VREF_RANGE_END+1];

extern const U16 gVref_Voltage_Table_LP4X[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM];

extern const U16 gVref_Voltage_Table_LP4[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM];
extern const U16 gVref_Voltage_Table_DDR4[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM];

void hqa_log_message_for_report(DRAMC_CTX_T *p,
								U32 channel,
								U32 rank,
								enum hqa_report_format format,
								char *main_str,
								U32 byte_bit_idx,
								S32 value,
								char *ans_str);
void HQA_measure_message_reset_all_data(DRAMC_CTX_T *p);
void Dramc_K_TX_EyeScan_Log(DRAMC_CTX_T *p);
void print_EYESCAN_LOG_message(DRAMC_CTX_T *p, U8 print_type);
void print_HQA_measure_message(DRAMC_CTX_T *p);

void ta2_test_run_time_hw_presetting(DRAMC_CTX_T *p,
						U32 len,
						TA2_RKSEL_TYPE_T rksel_mode);
void ta2_test_run_time_pat_setting(DRAMC_CTX_T *p, unsigned char PatSwitch);
void ta2_test_run_time_hw_write(DRAMC_CTX_T *p, U32 enable);
DRAM_STATUS_T ta2_test_run_time_hw_status(DRAMC_CTX_T *p);

/*----------------- End of HQA used ---------------------------*/

/*----------------- start of ETT used ---------------------------*/
#ifdef DRAM_ETT
/////////////////////////////////// SAVE TO SRAM FORMAT ////////////////////////////////////////////
typedef struct _SAVE_TO_SRAM_FORMAT_PASS_WIN_DATA_T
{
    S16 first_pass;
    S16 last_pass;
    S16 win_center;
    U16 win_size;
    U16 left_margin;
    U16 right_margin;
} SAVE_TO_SRAM_FORMAT_PASS_WIN_DATA_T;

typedef struct _SAVE_TO_SRAM_FORMAT_SwImpedanceCal_T
{
    U8  DRVP;
    U8  DRVN;
    U8  ODTN;
    U8  Reserved_1; //sagy
    U32 Reserved_2; //sagy
} SAVE_TO_SRAM_FORMAT_SwImpedanceCal_T;

typedef struct _SAVE_TO_SRAM_FORMAT_CaTraining_T
{
    U8  CA_delay;
    U8  Clk_delay;
    U8  CS_delay;
    U8  Reserved[4];
    SAVE_TO_SRAM_FORMAT_PASS_WIN_DATA_T WinPerBit[10];
}SAVE_TO_SRAM_FORMAT_CaTraining_T;

typedef struct _SAVE_TO_SRAM_FORMAT_Write_Leveling_T
{
    U8  CA_delay;
    U8  Clk_delay;
    U8  Final_Clk_delay;
    U8  DQS0_delay;
    U8  DQS1_delay;
    U8  DQS2_delay;
    U8  DQS3_delay;
    U8  Reserved[1];
}SAVE_TO_SRAM_FORMAT_Write_Leveling_T;

typedef struct _SAVE_TO_SRAM_FORMAT_GatingWindow_T
{
    U8  DQS0_2T;
    U8  DQS0_05T;
    U8  DQS0_PI;
    U8  DQS1_2T;
    U8  DQS1_05T;
    U8  DQS1_PI;
    U8  Reserved0[2];

    U8  DQS2_2T;
    U8  DQS2_05T;
    U8  DQS2_PI;
    U8  DQS3_2T;
    U8  DQS3_05T;
    U8  DQS3_PI;
    U8  Reserved1[2];

    U8  DQS0_RODT_2T;
    U8  DQS0_RODT_05T;
    U8  DQS1_RODT_2T;
    U8  DQS1_RODT_05T;
    U8  DQS2_RODT_2T;
    U8  DQS2_RODT_05T;
    U8  DQS3_RODT_2T;
    U8  DQS3_RODT_05T;
    u8  Gating_Win[4];
}SAVE_TO_SRAM_FORMAT_GatingWindow_T;

typedef struct _SAVE_TO_SRAM_FORMAT_RXWinDowPerbitCal_T
{
    U8  DQS0_delay;
    U8  DQS1_delay;
    U8  DQS2_delay;
    U8  DQS3_delay;
    U8  DQM0_delay;
    U8  DQM1_delay;
    U8  DQM2_delay;
    U8  DQM3_delay;

    U8  DQ_delay[32];
    SAVE_TO_SRAM_FORMAT_PASS_WIN_DATA_T WinPerBit[32];

}SAVE_TO_SRAM_FORMAT_RXWinDowPerbitCal_T;

typedef struct _SAVE_TO_SRAM_FORMAT_TxWindowPerbitCal_T
{
    U8  PI_DQ_delay[4];
    U8  Large_UI[4];
    U8  Small_UI[4];
    U8  PI[4];
    SAVE_TO_SRAM_FORMAT_PASS_WIN_DATA_T WinPerBit[32];
}SAVE_TO_SRAM_FORMAT_TxWindowPerbitCal_T;

typedef struct _SAVE_TO_SRAM_FORMAT_DATLAT_T
{
    U8  best_step;
    U8  Reserved[7];
}SAVE_TO_SRAM_FORMAT_DATLAT_T;

typedef struct _SAVE_TO_SRAM_FORMAT_RANK_T
{
    SAVE_TO_SRAM_FORMAT_SwImpedanceCal_T    SwImpedanceCal;

    SAVE_TO_SRAM_FORMAT_CaTraining_T    CaTraining;

    SAVE_TO_SRAM_FORMAT_Write_Leveling_T    Write_Leveling;

    SAVE_TO_SRAM_FORMAT_GatingWindow_T  GatingWindow;

    SAVE_TO_SRAM_FORMAT_TxWindowPerbitCal_T TxWindowPerbitCal;

    SAVE_TO_SRAM_FORMAT_DATLAT_T    DATLAT;

    SAVE_TO_SRAM_FORMAT_RXWinDowPerbitCal_T RxWindowPerbitCal;
} SAVE_TO_SRAM_FORMAT_RANK_T;

typedef struct _SAVE_TO_SRAM_FORMAT_RxdqsGatingPostProcess_T
{
    U8  s1ChangeDQSINCTL;
    U8  reg_TX_dly_DQSgated_min;
    U8  u1TXDLY_Cal_min;
    U8  TX_dly_DQSgated_check_min;
    U8  TX_dly_DQSgated_check_max;
    U8  DQSINCTL;
    U8  RANKINCTL;
    U8  u4XRTR2R;
}SAVE_TO_SRAM_FORMAT_RxdqsGatingPostProcess_T;

typedef struct _SAVE_TO_SRAM_FORMAT_DualRankRxdatlatCal_T
{
    U8  Rank0_Datlat;
    U8  Rank1_Datlat;
    U8  Final_Datlat;
    U8  Reserved[5];
}SAVE_TO_SRAM_FORMAT_DualRankRxdatlatCal_T;

typedef struct _SAVE_TO_SRAM_FORMAT_CHANNEL_T
{
    SAVE_TO_SRAM_FORMAT_RANK_T                      RANK[RANK_MAX];
    SAVE_TO_SRAM_FORMAT_RxdqsGatingPostProcess_T    RxdqsGatingPostProcess;
    SAVE_TO_SRAM_FORMAT_DualRankRxdatlatCal_T       DualRankRxdatlatCal;
} SAVE_TO_SRAM_FORMAT_CHANNEL_T;

typedef enum _EYE_SCAN_TYPE_T
{
    EYE_SCAN_TX_VREF = 0,
    EYE_SCAN_CA_VREF,
    EYE_SCAN_RX_VREF,
    EYE_SCAN_CS_VREF,
    EYE_SCAN_TX_DRV,
    EYE_SCAN_CA_DRV,
    EYE_SCAN_RX_DRV,
    EYE_SCAN_TYPE_MAX
}EYE_SCAN_TYPE_T;

typedef enum _CALI_TYPE_T
{
    CALI_TX = 0,
    CALI_CA,
    CALI_RX,
    CALI_JM,
    CALI_MAP,
    CALI_MAX
}CALI_TYPE_T;

typedef enum _ETT_TYPE_T
{
    ETT_DDR3X16X2 = 0,
    ETT_LPDDR3,
    ETT_DDR3X16,
    ETT_EVB,
    ETT_LPDDR3_168,
    ETT_DDR4X16X2,
    ETT_DDR3X8X4,
    ETT_MAX
}ETT_TYPE_T;

typedef struct {
	char *key;
	char *tip;
	void (*cmd)(void);
}ETT_CMD_T;

/*----------------- end of ETT used ---------------------------*/
#endif /*DRAM_ETT*/

#endif /*_DRAMC_HQA_H*/

