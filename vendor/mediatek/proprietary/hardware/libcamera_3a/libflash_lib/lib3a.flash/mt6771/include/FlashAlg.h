#ifndef __FLASHALG_H__
#define __FLASHALG_H__

#define FLASHALG_H_REVISION    7521001

#ifdef SIM_MAIN
#include "camera_custom_types.h"
#include "camera_custom_3a_nvram.h"
#include "camera_custom_awb_nvram.h"
#include "camera_custom_flash_nvram.h"
#endif

//#include <dbg_aaa_param.h>
#include "camera_flash_algo_tuning.h"
#include "dbg_flash_algo_param.h"

namespace NS3A
{
struct FlashAlgStrobeProfile
{
public:
   float iso;
   int   exp;
   float pfEng;
   float distance;
   int   dutyNum;
   int   stepNum;
   int   dutyTickNum;
   int   stepTickNum;
   int   * dutyTick;
   int   * stepTick;
   float * engTab;

   //int dutyNumL;
   //int* dutyTickL;

   FLASH_AWB_ALGO_GAIN_N_T *pFlWbCalTbl;
};

struct FlashAlgStrobeLim
{
   int minStep;
   int maxStep;
   int minDuty;
   int maxDuty;
};

struct FlashAlgExpPara
{
   int exp;
   int iso;
   int isFlash;
   int step;
   int duty;

   //int dutyL;  // modify for dual-flash
};

struct FlashAlgStaData
{
   int    row;
   int    col;
   int    dig_row;
   int    dig_col;
   short  * data;
   double normalizeFactor;
   int    bit;
};

struct FlashAlgPLineNode
{
   int iso;
   int exp;
};

typedef struct      evSetting
{
   MUINT32 u4Eposuretime;        //!<: Exposure time in ms
   MUINT32 u4AfeGain;            //!<: raw gain
   MUINT32 u4IspGain;            //!<: sensor gain
   MUINT8  uIris;                //!<: Iris
   MUINT8  uSensorMode;          //!<: sensor mode
   MUINT8  uFlag;                //!<: flag to indicate hysteresis ...
                                 //    MUINT8  uLV;                        //!<: LV avlue , in ISO 100 condition  LV=TV+AV
}evSetting;


typedef struct      PLine
{
   MUINT32   u4TotalIndex;                 //preview table Tatal index
   MINT32    i4StrobeTrigerBV;             // Strobe triger point in strobe auto mode
   MINT32    i4MaxBV;
   MINT32    i4MinBV;
   evSetting *pCurrentTable;              //point to current table
}PLine;


enum
{
   FlashAlg_Err_Ok             = 0,
   FlashAlg_Err_NoMem          = -100,
   FlashAlg_Err_Div0           = -101,
   FlashAlg_Err_Para           = -102,
   FlashAlg_Err_Other          = -103,
   FlashAlg_Err_CaliDataNotSet = -104,
   FlashAlg_Stage_Af           = 1,
   FlashAlg_Stage_Capture      = 0,
};

typedef enum
{
   FLASH_CAMERA_REAR_SENSOR,
   FLASH_CAMERA_FRONT_SENSOR,
   FLASH_CAMERA_DEFAULT_SENSOR,
} FLASH_CAMERA_TYPE;

typedef struct
{
   AWB_GAIN_T            NonPreferencePrvAWBGain;
   AWB_GAIN_T            PreferencePrvAWBGain;
   MINT32                i4AWBCCT;
   FLASH_AWB_ROTATION_MATRIX_T AwbRotationMatrix;
   MINT32                i4RotationMatrixUnit; // Rotation matrix unit
}FlashAlgInputInfoStruct;

typedef struct
{
   double LumaY;         // outpur for flash AE on NVRAM
   double R;
   double G;
   double B;
   int    Rgain;
   int    Ggain;
   int    Bgain;
   int    duty;     // X
   int    dutyLT;   // Y, low color temporature
}CalData;

typedef struct
{
   int duty;
   int dutyLT;      // Y, low color temporature
}ChooseResult;

// CFG_Mask_4_cfgFlashPolicy
enum
{
   CFG_FL_PF_CONV  = 0, // 0:default, 1:smooth
   CFG_FL_PF_CYC   = 2, // 0:default, 1:smooth
   CFG_FL_EstDecEv = 4, // 0:default, 1:yDecTarget
   CFG_FL_HYB      = 6, // 0:default, 1:Force, 2:Auto
};

typedef struct
{
   int x1;
   int y1;
   int x2;
   int y2;
}Position;

typedef struct
{
   Position facePosition;
   float    weight;
   double   num;
   double   sum;
   double   mean;
   int      inValid;
}BlockInfo;

typedef struct
{
   BlockInfo block[9];
   int       faceFlashSt;
   int       cnt;
   double    sumWeight;
   double    yMainFace;
   double    yAvgFace;
   int       inValid;
}FlashAlgFacePos;

enum
{
   FACE_FLASH     = 0,  // 0: disable, 1:enable
   MAIN_FACE_Y    = 1,  // decided by Main Face (Big Face)
   AVERAGE_FACE_Y = 2,  // decide by Multi-Face
};

typedef enum
{
   FLASH_SINGLE = 1,
   FLASH_DUAL,
}FLASH_TYPE;
};
#endif // __FLASHALG_H__
