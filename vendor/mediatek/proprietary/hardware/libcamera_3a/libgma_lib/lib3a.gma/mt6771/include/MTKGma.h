//MTK_SWIP_PROJECT_START
#ifndef __MTK_GMA_H__
#define __MTK_GMA_H__

/*
*   CHIP / AWB_VERSION_MAIN / AWB_VERSION_SUB / System Version
*/
#define GMA_ALGO_CHIP_VERSION       (6771)
#define GMA_ALGO_MAIN_VERSION       (1)
#define GMA_ALGO_SUB_VERSION        (1)
#define GMA_ALGO_SYSTEM_VERSION     (1)


#include "MTKGmaType.h"
#include "MTKGmaErrCode.h"


/*****************************************************************************
    Feature Control Enum and Structure
******************************************************************************/

typedef enum
{
	MTKGMA_FEATURE_BEGIN,				// minimum of feature id
	MTKGMA_FEATURE_SET_ENV_INFO,		// feature id to setup environment information
    MTKGMA_FEATURE_SET_PROC_INFO,		// feature id to setup processing information
    MTKGMA_FEATURE_GET_RESULT,			// feature id to get result
    MTKGMA_FEATURE_GET_EXIF,				// feature id to get EXIF
	MTKGMA_FEATURE_CONFIG_SMOOTH,
	MTKGMA_FEATURE_CONFIG_FLARE,
	MTKGMA_FEATURE_CONFIG_ENHANCE_FACTOR,
	MTKGMA_FEATURE_CONFIG_MODE,
	MTKGMA_FEATURE_GET_DECODEDATA,
	MTKGMA_FEATURE_GET_ENCODEDATA,
	MTKGMA_FEATURE_GET_DUALCAM_SYNC_INFO,
    MTKGMA_FEATURE_MAX					// maximum of feature id
}	MTKGMA_FEATURE_ENUM;

typedef enum
{
  MTKGMA_TUNING_SET_Preview = 0,
    MTKGMA_TUNING_SET_Video,
    MTKGMA_TUNING_SET_Capture,
    MTKGMA_TUNING_SET_HDR_Preview,
    MTKGMA_TUNING_SET_HDR_Video,
    MTKGMA_TUNING_SET_HDR_Capture,
    MTKGMA_TUNING_SET_AutoHDR_Preview,
    MTKGMA_TUNING_SET_AutoHDR_Video,
    MTKGMA_TUNING_SET_AutoHDR_Capture,
    MTKGMA_TUNING_SET_RSV0,
    MTKGMA_TUNING_SET_RSV1,
    MTKGMA_TUNING_SET_RSV2,
    MTKGMA_TUNING_SET_MAX         // maximum of set enum
} eMTKGMA_TUNING_SET_T;


typedef enum
{
    eGMASensorDev_None         = 0x00,
    eGMASensorDev_Main         = 0x01,
    eGMASensorDev_Sub          = 0x02,
    eGMASensorDev_MainSecond   = 0x04,
    eGMASensorDev_Main3D       = 0x05,
    eGMASensorDev_Sub2         = 0x08,
    eGMASensorDev_MainThird    = 0x10
}   eGMASensorDev_T;


#define CONTRAST_WEIGHTING_TBL_NUM      (11)
#define LV_WEIGHTING_TBL_NUM            (20)
#define GMA_TBL_SIZE                    (193)
#define GMA_TBL_NUM                     (4)
#define GMA_AE_HISTOGRAM_BIN            (128)
#define GMA_LV_MAX                      (200) // LV: 20



typedef struct {
    MINT32 i4HdrContrastWtTbl[CONTRAST_WEIGHTING_TBL_NUM];
    MINT32 i4HdrLVWtTbl[LV_WEIGHTING_TBL_NUM];
    MINT32 i4NightContrastWtTbl[CONTRAST_WEIGHTING_TBL_NUM];
    MINT32 i4NightLVWtTbl[LV_WEIGHTING_TBL_NUM];
} MTK_GMA_TUNING_LUT_T;

typedef struct {
    MINT32 i4Enable;
    MINT32 i4CenterPt;
    MINT32 i4LowPercent;
    MINT32 i4LowCurve100;
    MINT32 i4HighCurve100;
    MINT32 i4HighPercent;
    MINT32 i4SlopeH100;
    MINT32 i4SlopeL100;
} MTK_GMA_TUNING_CURVE_T;

typedef struct {
    MINT32 i4Enable;
    MINT32 i4WaitAEStable;
    MINT32 i4Speed;     // 0 ~ 10
} MTK_GMA_TUNING_SMOOTH_T;

typedef struct {
    MINT32 i4Enable;
} MTK_GMA_TUNING_FLARE_T;

typedef enum {
    eFIXED_GMA_MODE = 0,
    eDYNAMIC_GMA_MODE,
} GMA_MODE_T;

// GMA NVRAM structure
typedef struct
{
    MINT32 i4EncNVRAMGMA[GMA_TBL_NUM][GMA_TBL_SIZE];
} MTK_GMA_NVRAM_PARAM_T;

typedef struct {
    GMA_MODE_T eGMAMode;  // 0: Fixed Gamma  1: Dynamic Gamma 2: Adaptive Gamma
    MINT32 i4LowContrastThr;
    MINT32 i4LowContrastRatio;
    MINT32 i4LowContrastSeg;
    MTK_GMA_TUNING_LUT_T rGMALUTs;
    MTK_GMA_TUNING_SMOOTH_T rGMASmooth;
    MTK_GMA_TUNING_CURVE_T rGMACurve;
    MTK_GMA_TUNING_FLARE_T rGMAFlare;
} MTK_GMA_TUNING_PARAM_T;


typedef struct {
    MUINT32 u4AETarget;
    MUINT32 u4AECurrentTarget;
    MUINT32 u4Eposuretime;   //!<: Exposure time in ms
    MUINT32 u4AfeGain;           //!<: raw gain
    MUINT32 u4IspGain;           //!<: sensor gain
    MUINT32 u4RealISOValue;
    MINT32  i4LightValue_x10;
    MUINT32 u4AECondition;
    MINT16  i2FlareOffset;
    MINT32  i4GammaIdx;   // next gamma idx
    MINT32  i4LESE_Ratio;    // LE/SE ratio
    MUINT32 u4SWHDR_SE;      //for sw HDR SE ,  -x EV , compare with converge AE
    MUINT32 u4MaxISO;
    MUINT32 u4AEStableCnt;
    MUINT32 u4OrgExposuretime;   //!<: Exposure time in ms
    MUINT32 u4OrgRealISOValue;
    MUINT32 u4Histogrm[GMA_AE_HISTOGRAM_BIN];
    MBOOL bGammaEnable;

    MINT32 i4AEStable;
    MINT32 i4EVRatio;
    MINT32 i4DeltaIndex;

    //For face lce
    MUINT32 u4FDProb;
    MUINT32 u4FaceNum;

} GMA_AE_INFO_T;

typedef struct
{
    MTK_GMA_NVRAM_PARAM_T rNVRAMGmaParam;
    MTK_GMA_TUNING_PARAM_T rTuningGmaParam;
} MTK_GMA_ENV_INFO_STRUCT, *P_MTK_GMA_ENV_INFO_STRUCT;

typedef struct
{
    MINT32 i4GMAMode;
    MINT32 i4FaceGMAFlag;
    MINT32 i4GmaProfile;
    MINT32 i4ChipVersion;
    MINT32 i4MainVersion;
    MINT32 i4SubVersion;
    MINT32 i4SystemVersion;
    MINT32 i4EVRatio;
    MINT32 i4LowContrastThr;
    MINT32 i4LowContrastRatio;
    MINT32 i4LowContrastSeg;
    MINT32 i4Contrast;
    MINT32 i4Contrast_L;
    MINT32 i4Contrast_H;
    MINT32 i4HdrContrastWeight;
    MINT32 i4EVContrastY;
    MINT32 i4ContrastY_L;
    MINT32 i4ContrastY_H;
    MINT32 i4NightContrastWeight;
    MINT32 i4LV;
    MINT32 i4LV_L;
    MINT32 i4LV_H;
    MINT32 i4HdrLVWeight;
    MINT32 i4NightLVWeight;
    MINT32 i4SmoothEnable;
    MINT32 i4SmoothSpeed;
    MINT32 i4SmoothWaitAE;
    MINT32 i4GMACurveEnable;
    MINT32 i4CenterPt;
    MINT32 i4LowCurve;
    MINT32 i4SlopeL;
    MINT32 i4FlareEnable;
    MINT32 i4FlareOffset;
} MTK_GMA_EXIF_INFO_STRUCT, *P_MTK_GMA_EXIF_INFO_STRUCT;

#ifndef MTKGMADUALCAMSYNCINFO
#define MTKGMADUALCAMSYNCINFO

typedef struct {
    MBOOL bIsValid;   // is data valid flag

    //GGMD1 Result
    MBOOL bIsFaceGMA;
    MINT32 i4EncGMA[GMA_TBL_SIZE];
}MTK_GMA_DUALCAM_SYNC_INFO_T, *P_MTK_GMA_DUALCAM_SYNC_INFO_T;
#endif //MTKGMADUALCAMSYNCINFO


typedef struct
{
    MINT32 i4CurrEncGMA[GMA_TBL_SIZE];
	GMA_AE_INFO_T rGMAAEInfo;   // Get current AE related information

    MBOOL bIsDualcamSlave;  // Dualcam Master: False, Slave: True
    MTK_GMA_DUALCAM_SYNC_INFO_T rGMADualcamSyncInfo;
} MTK_GMA_PROC_INFO_STRUCT, *P_MTK_GMA_PROC_INFO_STRUCT;

typedef struct
{
	MINT32 i4EncGMA[GMA_TBL_SIZE];
} MTK_GMA_RESULT_INFO_STRUCT, *P_MTK_GMA_RESULT_INFO_STRUCT;

class MTKGma
{
public:
    static MTKGma* createInstance(eGMASensorDev_T const eSensorDev);
    virtual void   destroyInstance(MTKGma* obj) = 0;

    virtual ~MTKGma(){}
    // Process Control
    virtual MRESULT GmaInit(void *InitInData, void *InitOutData) = 0;
    virtual MRESULT GmaMain(void) = 0;
	virtual MRESULT GmaExit(void) = 0;
	virtual MRESULT GmaReset(void) = 0;                 // RESET for each image

	// Feature Control
	virtual MRESULT GmaFeatureCtrl(MUINT32 FeatureID, void* pParaIn, void* pParaOut) = 0;
private:

};


#endif  //__MTK_Gma_H__
//MTK_SWIP_PROJECT_END
