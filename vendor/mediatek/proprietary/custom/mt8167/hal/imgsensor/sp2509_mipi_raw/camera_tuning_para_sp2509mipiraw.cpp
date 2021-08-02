#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_sp2509mipiraw.h"
#include "camera_info_sp2509mipiraw.h"
#include "camera_custom_AEPlinetable.h"
#include "camera_custom_tsf_tbl.h"
const NVRAM_CAMERA_ISP_PARAM_STRUCT CAMERA_ISP_DEFAULT_VALUE =
{{
    //Version
    Version: NVRAM_CAMERA_PARA_FILE_VERSION,
    //SensorId
    SensorId: SENSOR_ID,
    ISPComm:{
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        }
    },
    ISPPca: {
#include INCLUDE_FILENAME_ISP_PCA_PARAM
    },
    ISPRegs:{
    #include INCLUDE_FILENAME_ISP_REGS_PARAM
    },
    ISPMfbMixer:{{
        {//00: MFB mixer for ISO 100
            0x00000000, 0x00000000
        },
        {//01: MFB mixer for ISO 200
            0x00000000, 0x00000000
        },
        {//02: MFB mixer for ISO 400
            0x00000000, 0x00000000
        },
        {//03: MFB mixer for ISO 800
            0x00000000, 0x00000000
        },
        {//04: MFB mixer for ISO 1600
            0x00000000, 0x00000000
        },
        {//05: MFB mixer for ISO 2400
            0x00000000, 0x00000000
        },
        {//06: MFB mixer for ISO 3200
            0x00000000, 0x00000000
        }
    }},
    ISPMulitCCM:{
      Poly22:{
        75170, // i4R_AVG
        13190, // i4R_STD
        79140, // i4B_AVG
        26270, // i4B_STD
        4095,      // i4R_MAX
        512,      // i4R_MIN
        4095,      // i4G_MAX
        512,      // i4G_MIN
        4095,      // i4B_MAX
        512,      // i4B_MIN
        {  // i4P00[9]
                8897296,-2989626,-787686,-1208954,6829026,-500072,170190,-2770908,7720566
        },
        {  // i4P10[9]
                1867396,-1257886,-609516,-495040,-44440,539480,-147722,392332,-245110
        },
        {  // i4P01[9]
                1628734,-988046,-640704,-716820,-361112,1077932,-114812,-380908,495378
        },
        {  // i4P20[9]
                788014,-983900,196062,-43050,119624,-76574,281758,-1043902,762090
                },
                { // i4P11[9]
                -71500,-689612,761476,243148,119000,-362148,286776,-619070,332618
                },
                { // i4P02[9]
                -631502,130466,501236,302926,68298,-371224,43616,-17274,-25994
                }

      },
      AWBGain:{
        // Strobe
        {
          810,    // i4R
          512,    // i4G
          677    // i4B
        },
        // A
        {
            519,    // i4R
            512,    // i4G
            1450    // i4B
        },
        // TL84
        {
            605,    // i4R
            512,    // i4G
            1172    // i4B
        },
        // CWF
        {
            771,    // i4R
            512,    // i4G
            1293    // i4B
        },
        // D65
        {
            810,    // i4R
            512,    // i4G
            677    // i4B
        },
        // Reserved 1
        {
            512,    // i4R
            512,    // i4G
            512    // i4B
        },
        // Reserved 2
        {
            512,    // i4R
            512,    // i4G
            512    // i4B
        },
        // Reserved 3
        {
            512,    // i4R
            512,    // i4G
            512    // i4B
        }
      },
      Weight:{
        1, // Strobe
        1, // A
        1, // TL84
        1, // CWF
        1, // D65
        1, // Reserved 1
        1, // Reserved 2
        1  // Reserved 3
                }
    },
    //bInvokeSmoothCCM
    bInvokeSmoothCCM: MFALSE

}};

const NVRAM_CAMERA_3A_STRUCT CAMERA_3A_NVRAM_DEFAULT_VALUE =
{
    NVRAM_CAMERA_3A_FILE_VERSION, // u4Version
    SENSOR_ID, // SensorId

    // AE NVRAM
    {
        // rDevicesInfo
        {
            1136,    // u4MinGain, 1024 base = 1x
            6144,    // u4MaxGain, 16x
            61,    // u4MiniISOGain, ISOxx  
            128,    // u4GainStepUnit, 1x/8 
            39458,    // u4PreExpUnit 
            20,    // u4PreMaxFrameRate
            39458,    // u4VideoExpUnit  
            20,    // u4VideoMaxFrameRate 
            1024,    // u4Video2PreRatio, 1024 base = 1x 
            39458,    // u4CapExpUnit 
            20,    // u4CapMaxFrameRate
            1024,    // u4Cap2PreRatio, 1024 base = 1x
            39458,    // u4Video1ExpUnit
            20,    // u4Video1MaxFrameRate
            1024,    // u4Video12PreRatio, 1024 base = 1x
            39458,    // u4Video2ExpUnit
            20,    // u4Video2MaxFrameRate
            1024,    // u4Video22PreRatio, 1024 base = 1x
            39458,    // u4Custom1ExpUnit
            20,    // u4Custom1MaxFrameRate
            1024,    // u4Custom12PreRatio, 1024 base = 1x
            39458,    // u4Custom2ExpUnit
            20,    // u4Custom2MaxFrameRate
            1024,    // u4Custom22PreRatio, 1024 base = 1x
            39458,    // u4Custom3ExpUnit
            20,    // u4Custom3MaxFrameRate
            1024,    // u4Custom32PreRatio, 1024 base = 1x
            39458,    // u4Custom4ExpUnit
            20,    // u4Custom4MaxFrameRate
            1024,    // u4Custom42PreRatio, 1024 base = 1x
            39458,    // u4Custom5ExpUnit
            20,    // u4Custom5MaxFrameRate
            1024,    // u4Custom52PreRatio, 1024 base = 1x
            28,    // u4LensFno, Fno = 2.8
            350    // u4FocusLength_100x
        },
        // rHistConfig
        {
            4,    // u4HistHighThres
            40,    // u4HistLowThres
            2,    // u4MostBrightRatio
            1,    // u4MostDarkRatio
            160,    // u4CentralHighBound
            20,    // u4CentralLowBound
            {240, 230, 220, 210, 200},    // u4OverExpThres[AE_CCT_STRENGTH_NUM] 
            {62, 70, 82, 108, 141},    // u4HistStretchThres[AE_CCT_STRENGTH_NUM] 
            {18, 22, 26, 30, 34}    // u4BlackLightThres[AE_CCT_STRENGTH_NUM] 
        },
        // rCCTConfig
        {
            TRUE,    // bEnableBlackLight
            TRUE,    // bEnableHistStretch
            TRUE,    // bEnableAntiOverExposure
            TRUE,    // bEnableTimeLPF
            TRUE,    // bEnableCaptureThres
            TRUE,    // bEnableVideoThres
            TRUE,    // bEnableVideo1Thres
            TRUE,    // bEnableVideo2Thres
            TRUE,    // bEnableCustom1Thres
            TRUE,    // bEnableCustom2Thres
            TRUE,    // bEnableCustom3Thres
            TRUE,    // bEnableCustom4Thres
            TRUE,    // bEnableCustom5Thres
            TRUE,    // bEnableStrobeThres
            55,    // u4AETarget
            47,    // u4StrobeAETarget
            50,    // u4InitIndex
            4,    // u4BackLightWeight
            32,    // u4HistStretchWeight
            4,    // u4AntiOverExpWeight
            2,    // u4BlackLightStrengthIndex
            2,    // u4HistStretchStrengthIndex
            2,    // u4AntiOverExpStrengthIndex
            2,    // u4TimeLPFStrengthIndex
            {1, 3, 5, 7, 8},    // u4LPFConvergeTable[AE_CCT_STRENGTH_NUM] 
            90,    // u4InDoorEV = 9.0, 10 base 
            1,    // i4BVOffset delta BV = value/10 
            64,    // u4PreviewFlareOffset
            64,    // u4CaptureFlareOffset
            3,    // u4CaptureFlareThres
            64,    // u4VideoFlareOffset
            3,    // u4VideoFlareThres
            64,    // u4CustomFlareOffset
            3,    // u4CustomFlareThres
            64,    // u4StrobeFlareOffset
            3,    // u4StrobeFlareThres
            160,    // u4PrvMaxFlareThres
            0,    // u4PrvMinFlareThres
            160,    // u4VideoMaxFlareThres
            0,    // u4VideoMinFlareThres
            18,    // u4FlatnessThres    // 10 base for flatness condition.
            75,    // u4FlatnessStrength
            //rMeteringSpec
            {
                //rHS_Spec
                {
                    TRUE,//bEnableHistStretch           // enable histogram stretch
                    1024,//u4HistStretchWeight          // Histogram weighting value
                    40,//u4Pcent                      // 
                    172,//u4Thd                        // 0~255
                    75, //74,//u4FlatThd                    // 0~255
                    120,//u4FlatBrightPcent
                    120,//u4FlatDarkPcent
                    //sFlatRatio
                    {
                        1000,  //i4X1
                        1024,  //i4Y1
                        2400,  //i4X2
                        0  //i4Y2
                    },
                    TRUE, //bEnableGreyTextEnhance
                    1800, //u4GreyTextFlatStart, > sFlatRatio.i4X1, < sFlatRatio.i4X2
                    {
                        10,  //i4X1
                        1024,  //i4Y1
                        80,  //i4X2
                        0  //i4Y2
                    }
                },
                //rAOE_Spec
                {
                    TRUE, //bEnableAntiOverExposure
                    1024, //u4AntiOverExpWeight
                    10, //u4Pcent
                    200, //u4Thd
                    TRUE, //bEnableCOEP
                    1, //u4COEPcent
                    106, //u4COEThd
                    0, //u4BVCompRatio
                    //sCOEYRatio;     // the outer y ratio
                    {
                        23,  //i4X1
                        1024,  //i4Y1
                        47,  //i4X2
                        0  //i4Y2
                    },
                    //sCOEDiffRatio;  // inner/outer y difference ratio
                    {
                        1500,  //i4X1
                        0,  //i4Y1
                        2100,  //i4X2
                        1024  //i4Y2
                    }
                },
                //rABL_Spec
                {
                    TRUE,//bEnableBlackLight
                    1024,//u4BackLightWeight
                    400,//u4Pcent
                    28,//u4Thd
                    255, // center luminance
                    256, // final target limitation, 256/128 = 2x
                    //sFgBgEVRatio
                    {
                        2200,  //i4X1
                        0,  //i4Y1
                        4000,  //i4X2
                        1024  //i4Y2
                    },
                    //sBVRatio
                    {
                        3800,  //i4X1
                        0,  //i4Y1
                        5000,  //i4X2
                        1024  //i4Y2
                    }
                },
                //rNS_Spec
                {
                    TRUE, // bEnableNightScene
                    5,//u4Pcent
                    160,//u4Thd
                    72,//u4FlatThd
                    200,//u4BrightTonePcent
                    86,//u4BrightToneThd
                    500,//u4LowBndPcent
                    5,//u4LowBndThd
                    23,//u4LowBndThdLimit
                    50,//u4FlatBrightPcent
                    300,//u4FlatDarkPcent
                    //sFlatRatio
                    {
                        1200,  //i4X1
                        1024,  //i4Y1
                        2400,  //i4X2
                        0  //i4Y2
                    },
                    //sBVRatio
                    {
                        -500,  //i4X1
                        1024,  //i4Y1
                        3000,  //i4X2
                        0  //i4Y2
                    },
                    TRUE, // bEnableNightSkySuppresion
                    //sSkyBVRatio
                    {
                        -4000,  //i4X1
                        1024,  //i4Y1
                        -2000,  //i4X2
                        0  //i4Y2
                    }
                },
                // rTOUCHFD_Spec
                {
                    40, //uMeteringYLowBound;
                    50, //uMeteringYHighBound;
                    40, //uFaceYLowBound;
                    50, //uFaceYHighBound;
                    3, //uFaceCentralWeight;
                    120, //u4MeteringStableMax;
                    80, //u4MeteringStableMin;
                }
            }, //End rMeteringSpec
            // rFlareSpec
            {
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //uPrvFlareWeightArr[16];
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //uVideoFlareWeightArr[16];
                96,                                               //u4FlareStdThrHigh;
                48,                                               //u4FlareStdThrLow;
                0,                                               //u4PrvCapFlareDiff;
                4,                                               //u4FlareMaxStepGap_Fast;
                0,                                               //u4FlareMaxStepGap_Slow;
                1800,                                               //u4FlarMaxStepGapLimitBV;
                0,                                               //u4FlareAEStableCount;
            },
            //rAEMoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                190, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                70, //u4B2TStart
                60, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAEVideoMoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAEVideo1MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAEVideo2MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom1MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom2MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom3MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom4MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom5MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAEFaceMoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                190, //u4Bright2TargetEnd
                10, //u4Dark2TargetStart
                80, //u4B2TEnd
                30, //u4B2TStart
                20, //u4D2TEnd
                60, //u4D2TStart
            },
            //rAETrackingMoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                190, //u4Bright2TargetEnd
                10, //u4Dark2TargetStart
                80, //u4B2TEnd
                30, //u4B2TStart
                20, //u4D2TEnd
                60, //u4D2TStart
            },
            //rAEAOENVRAMParam =
            {
                1,      // i4AOEStrengthIdx: 0 / 1 / 2
                130,      // u4BVCompRatio
                {//rAEAOEAlgParam
                    {//rAEAOEAlgParam[0]
                        47,  //u4Y_Target
                        25,  //u4AOE_OE_percent
                        210,  //u4AOE_OEBound
                        10,  //u4AOE_DarkBound
                        950,  //u4AOE_LowlightPrecent
                        1,  //u4AOE_LowlightBound
                        145,  //u4AOESceneLV_L
                        180,  //u4AOESceneLV_H
                        40,  //u4AOE_SWHdrLE_Bound
                    },
                    {//rAEAOEAlgParam[1]
                        47,  //u4Y_Target
                        25,  //u4AOE_OE_percent
                        210,  //u4AOE_OEBound
                        15,  //u4AOE_DarkBound
                        950,  //u4AOE_LowlightPrecent
                        3,  //u4AOE_LowlightBound
                        145,  //u4AOESceneLV_L
                        180,  //u4AOESceneLV_H
                        40,  //u4AOE_SWHdrLE_Bound
                    },
                    {//rAEAOEAlgParam[2]
                        47,  //u4Y_Target
                        25,  //u4AOE_OE_percent
                        210,  //u4AOE_OEBound
                        25,  //u4AOE_DarkBound
                        950,  //u4AOE_LowlightPrecent
                        8,  //u4AOE_LowlightBound
                        145,  //u4AOESceneLV_L
                        180,  //u4AOESceneLV_H
                        40,  //u4AOE_SWHdrLE_Bound
                    }
                }
            }
        }
    },
    // AWB NVRAM
    {
        {
          {
            // AWB calibration data
            {
                // rUnitGain (unit gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rGoldenGain (golden sample gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rD65Gain (D65 WB gain: 1.0 = 512)
                {
                    841,    // D65Gain_R
                    512,    // D65Gain_G
                    615    // D65Gain_B
                }
            },
            // Original XY coordinate of AWB light source
            {
                // Strobe
                {
                    0,    // i4X
                    0    // i4Y
                },
                // Horizon
                {
                    -340,    // OriX_Hor
                    -176    // OriY_Hor
                },
                // A
                {
                    -263,    // OriX_A
                    -213    // OriY_A
                },
                // TL84
                {
                    -130,    // OriX_TL84
                    -279    // OriY_TL84
                },
                // CWF
                {
                    -83,    // OriX_CWF
                    -333    // OriY_CWF
                },
                // DNP
                {
                    -34,    // OriX_DNP
                    -262    // OriY_DNP
                },
                // D65
                {
                    115,    // OriX_D65
                    -251    // OriY_D65
                },
                // DF
                {
                    0,    // OriX_DF
                    0    // OriY_DF
                }
            },
            // Rotated XY coordinate of AWB light source
            {
                // Strobe
                {
                    0,    // i4X
                    0    // i4Y
                },
                // Horizon
                {
                    -320,    // RotX_Hor
                    -211    // RotY_Hor
                },
                // A
                {
                    -240,    // RotX_A
                    -240    // RotY_A
                },
                // TL84
                {
                    -100,    // RotX_TL84
                    -292    // RotY_TL84
                },
                // CWF
                {
                    -48,    // RotX_CWF
                    -340    // RotY_CWF
                },
                // DNP
                {
                    -6,    // RotX_DNP
                    -265    // RotY_DNP
                },
                // D65
                {
                    141,    // RotX_D65
                    -238    // RotY_D65
                },
                // DF
                {
                    121,    // RotX_DF
                    -328    // RotY_DF
                }
            },
            // AWB gain of AWB light source
            {
                // Strobe 
                {
                    512,    // i4R
                    512,    // i4G
                    512    // i4B
                },
                // Horizon 
                {
                    512,    // AWBGAIN_HOR_R
                    639,    // AWBGAIN_HOR_G
                    1285    // AWBGAIN_HOR_B
                },
                // A 
                {
                    512,    // AWBGAIN_A_R
                    548,    // AWBGAIN_A_G
                    1044    // AWBGAIN_A_B
                },
                // TL84 
                {
                    627,    // AWBGAIN_TL84_R
                    512,    // AWBGAIN_TL84_G
                    890    // AWBGAIN_TL84_B
                },
                // CWF 
                {
                    718,    // AWBGAIN_CWF_R
                    512,    // AWBGAIN_CWF_G
                    900    // AWBGAIN_CWF_B
                },
                // DNP 
                {
                    696,    // AWBGAIN_DNP_R
                    512,    // AWBGAIN_DNP_G
                    764    // AWBGAIN_DNP_B
                },
                // D65 
                {
                    841,    // AWBGAIN_D65_R
                    512,    // AWBGAIN_D65_G
                    615    // AWBGAIN_D65_B
                },
                // DF 
                {
                    512,    // AWBGAIN_DF_R
                    512,    // AWBGAIN_DF_G
                    512    // AWBGAIN_DF_B
                }
            },
            // Rotation matrix parameter
            {
                -6,    // RotationAngle
                255,    // Cos
                -27    // Sin
            },
            // Daylight locus parameter
            {
                -103,    // i4SlopeNumerator
                128    // i4SlopeDenominator
            },
            // Predictor gain
            {
                101, // i4PrefRatio100
                // DaylightLocus_L
                {
                    811,    // i4R
                    530,    // i4G
                    635    // i4B
                },
                // DaylightLocus_H
                {
                    655,    // i4R
                    512,    // i4G
                    753    // i4B
                },
                // Temporal General
                {
                    841,    // i4R
                    512,    // i4B
                    615    // i4B
                },
                // AWB LSC Gain
                {
                    655,        // i4R
                    512,        // i4G
                    753        // i4B
                }
            },
            // AWB light area
            {
                // Strobe:FIXME
                {
                    0,    // i4RightBound
                    0,    // i4LeftBound
                    0,    // i4UpperBound
                    0    // i4LowerBound
                },
                // Tungsten
                {
                    -195,    // TungRightBound
                    -720,    // TungLeftBound
                    -140,    // TungUpperBound
                    -192    // TungLowerBound
                },
                // Warm fluorescent
                {
                    -195,    // WFluoRightBound
                    -720,    // WFluoLeftBound
                    -192,    // WFluoUpperBound
                    -450    // WFluoLowerBound
                },
                // Fluorescent
                {
                    -20,    // FluoRightBound
                    -195,    // FluoLeftBound
                    -178,    // FluoUpperBound
                    -330    // FluoLowerBound
                },
                // CWF
                {
                -20,    // CWFRightBound
                -195,    // CWFLeftBound
                -330,    // CWFUpperBound
                -420    // CWFLowerBound
                },
                // Daylight
                {
                    200,    // DayRightBound
                    -20,    // DayLeftBound
                    -178,    // DayUpperBound
                    -330    // DayLowerBound
                },
                // Shade
                {
                    501,    // ShadeRightBound
                    200,    // ShadeLeftBound
                    -178,    // ShadeUpperBound
                    -279    // ShadeLowerBound
                },
                // Daylight Fluorescent
                {
                    171,    // DFRightBound
                    -20,    // DFLeftBound
                    -330,    // DFUpperBound
                    -420    // DFLowerBound
                }
            },
            // PWB light area
            {
                // Reference area
                {
                    501,    // PRefRightBound
                    -720,    // PRefLeftBound
                    0,    // PRefUpperBound
                    -450    // PRefLowerBound
                },
                // Daylight
                {
                    225,    // PDayRightBound
                    -20,    // PDayLeftBound
                    -178,    // PDayUpperBound
                    -330    // PDayLowerBound
                },
                // Cloudy daylight
                {
                    325,    // PCloudyRightBound
                    150,    // PCloudyLeftBound
                    -178,    // PCloudyUpperBound
                    -330    // PCloudyLowerBound
                },
                // Shade
                {
                    425,    // PShadeRightBound
                    150,    // PShadeLeftBound
                    -178,    // PShadeUpperBound
                    -330    // PShadeLowerBound
                },
                // Twilight
                {
                    -20,    // PTwiRightBound
                    -180,    // PTwiLeftBound
                    -178,    // PTwiUpperBound
                    -330    // PTwiLowerBound
                },
                // Fluorescent
                {
                    191,    // PFluoRightBound
                    -200,    // PFluoLeftBound
                    -188,    // PFluoUpperBound
                    -390    // PFluoLowerBound
                },
                // Warm fluorescent
                {
                    -140,    // PWFluoRightBound
                    -340,    // PWFluoLeftBound
                    -188,    // PWFluoUpperBound
                    -390    // PWFluoLowerBound
                },
                // Incandescent
                {
                    -140,    // PIncaRightBound
                    -340,    // PIncaLeftBound
                    -178,    // PIncaUpperBound
                    -330    // PIncaLowerBound
                },
                // Gray World
                {
                    5000,    // PGWRightBound
                    -5000,    // PGWLeftBound
                    5000,    // PGWUpperBound
                    -5000    // PGWLowerBound
                }
            },
            // PWB default gain	
            {
                // Daylight
                {
                    809,    // PWB_Day_R
                    512,    // PWB_Day_G
                    660    // PWB_Day_B
                },
                // Cloudy daylight
                {
                    989,    // PWB_Cloudy_R
                    512,    // PWB_Cloudy_G
                    561    // PWB_Cloudy_B
                },
                // Shade
                {
                    1065,    // PWB_Shade_R
                    512,    // PWB_Shade_G
                    529    // PWB_Shade_B
                },
                // Twilight
                {
                    599,    // PWB_Twi_R
                    512,    // PWB_Twi_G
                    842    // PWB_Twi_B
                },
                // Fluorescent
                {
                    720,    // PWB_Fluo_R
                    512,    // PWB_Fluo_G
                    791    // PWB_Fluo_B
                },
                // Warm fluorescent
                {
                    507,    // PWB_WFluo_R
                    512,    // PWB_WFluo_G
                    1050    // PWB_WFluo_B
                },
                // Incandescent
                {
                    486,    // PWB_Inca_R
                    512,    // PWB_Inca_G
                    997    // PWB_Inca_B
                },
                // Gray World
                {
                    512,    // PWB_GW_R
                    512,    // PWB_GW_G
                    512    // PWB_GW_B
                }
            },
            // AWB preference color	
            {
                // Tungsten
                {
                    0,    // TUNG_SLIDER
                    5628    // TUNG_OFFS
                },
                // Warm fluorescent	
                {
                    0,    // WFluo_SLIDER
                    5601    // WFluo_OFFS
                },
                // Shade
                {
                    0,    // Shade_SLIDER
                    1808    // Shade_OFFS
                },
                // Sunset Area
                {
                    -19,   // i4Sunset_BoundXr_Thr
                    -265    // i4Sunset_BoundYr_Thr
                },
                // Sunset Vertex
                {
                    18,   // i4Sunset_BoundXr_Thr
                    -265    // i4Sunset_BoundYr_Thr
                },
                // Shade F Area
                {
                    -195,   // i4BoundXrThr
                    -310    // i4BoundYrThr
                },
                // Shade F Vertex
                {
                    -156,   // i4BoundXrThr
                    -306    // i4BoundYrThr
                },
                // Shade CWF Area
                {
                    -195,   // i4BoundXrThr
                    -407    // i4BoundYrThr
                },
                // Shade CWF Vertex
                {
                    -156,   // i4BoundXrThr
                    -370    // i4BoundYrThr
                },
                // Low CCT Area
                {
                    -340,   // i4BoundXrThr
                    343    // i4BoundYrThr
                },
                // Low CCT Vertex
                {
                    -340,   // i4BoundXrThr
                    343    // i4BoundYrThr
                }
            },
            // CCT estimation
            {
                // CCT
                {
                    2300,    // i4CCT[0]
                    2850,    // i4CCT[1]
                    3750,    // i4CCT[2]
                    5100,    // i4CCT[3]
                    6500     // i4CCT[4]
                },
                // Rotated X coordinate
                {
                -461,    // i4RotatedXCoordinate[0]
                -381,    // i4RotatedXCoordinate[1]
                -241,    // i4RotatedXCoordinate[2]
                -147,    // i4RotatedXCoordinate[3]
                0    // i4RotatedXCoordinate[4]
                }
            }
        },
        // Algorithm Tuning Paramter
        {
            // AWB Backup Enable
            0,

            // Daylight locus offset LUTs for tungsten
            {
                21, // i4Size: LUT dimension
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                {0, 350, 800, 1222, 1444, 1667, 1889, 2111, 2333, 2556, 2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778, 5000} // i4LUTOut
            },
            // Daylight locus offset LUTs for WF
            {
                21, // i4Size: LUT dimension
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                {0, 350, 700, 1000, 1444, 1667, 1889, 2111, 2333, 2556, 2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778, 5000} // i4LUTOut
            },
            // Daylight locus offset LUTs for Shade
            {
                21, // i4Size: LUT dimension
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000} // i4LUTOut
            },
            // Preference gain for each light source
            {
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // STROBE
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // TUNGSTEN
                {
                    {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, 
                    {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}, {500, 512, 500}
                }, // WARM F
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // F
                {
                    {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, 
                    {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}, {512, 512, 522}
                }, // CWF
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // DAYLIGHT
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // SHADE
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                } // DAYLIGHT F
            },
                // Parent block weight parameter
                {
                    1,      // bEnable
                    6           // i4ScalingFactor: [6] 1~12, [7] 1~6, [8] 1~3, [9] 1~2, [>=10]: 1
                },
                // AWB LV threshold for predictor
                {
                    115,    // i4InitLVThr_L
                    155,    // i4InitLVThr_H
                    100      // i4EnqueueLVThr
                },
                // AWB number threshold for temporal predictor
                {
                    65,     // i4Neutral_ParentBlk_Thr
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   2,   2,   2,   2,   2,   2,   2}  // (%) i4CWFDF_LUTThr
                },
                // AWB light neutral noise reduction for outdoor
                {
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    // Non neutral
                    {   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // Flurescent
                    {   0,   0,   0,   0,   0,   3,   5,   5,   5,   5,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // CWF
                    {   0,   0,   0,   0,   0,   3,   5,   5,   5,   5,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // Daylight
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   2,   2,   2,   2,   2,   2,   2},  // (%)
                    // DF
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                },
                // AWB feature detection
                {
                // Sunset Prop
                    {
                        1,          // i4Enable
                        120,        // i4LVThr_L
                        130,        // i4LVThr_H
                        10,         // i4SunsetCountThr
                        0,          // i4SunsetCountRatio_L
                        171         // i4SunsetCountRatio_H
                    },
                    // Shade F Detection
                    {
                        1,          // i4Enable
                        50,        // i4LVThr_L
                        70,        // i4LVThr_H
                        128         // i4DaylightProb
                    },
                    // Shade CWF Detection
                    {
                        1,          // i4Enable
                        50,        // i4LVThr_L
                        70,        // i4LVThr_H
                        192         // i4DaylightProb
                    },
                    // Low CCT
                    {
                        0,          // i4Enable
                        256        // i4SpeedRatio
                    }
                },
                // AWB non-neutral probability for spatial and temporal weighting look-up table (Max: 100; Min: 0)
                {
                    //LV0   1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18
                    {   0,  33,  66, 100, 100, 100, 100, 100, 100, 100, 100,  70,  30,  20,  10,   0,   0,   0,   0}
                },
                // AWB daylight locus probability look-up table (Max: 100; Min: 0)
                {   //LV0    1     2     3      4     5     6     7     8      9      10     11    12   13     14    15   16    17    18
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   0,   0,   0,   0}, // Strobe
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  25,   0,   0,   0}, // Tungsten
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  25,  25,  25,   0,   0,   0}, // Warm fluorescent
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  95,  75,  50,  25,  25,  25,   0,   0,   0}, // Fluorescent
                    {  90,  90,  90,  90,  90,  90,  90,  90,  90,  90,  80,  55,  30,  30,  30,  30,   0,   0,   0}, // CWF
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  40,  30,  20}, // Daylight
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  25,   0,   0,   0,   0}, // Shade
                    {  90,  90,  90,  90,  90,  90,  90,  90,  90,  90,  80,  55,  30,  30,  30,  30,   0,   0,   0} // Daylight fluorescent
                }
            }
            },
        {
          {
            // AWB calibration data
            {
                // rUnitGain (unit gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rGoldenGain (golden sample gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rD65Gain (D65 WB gain: 1.0 = 512)
                {
                    792,    // D65Gain_R
                    530,    // D65Gain_G
                    512    // D65Gain_B
                }
            },
            // Original XY coordinate of AWB light source
            {
                // Strobe
                {
                    0,    // i4X
                    0    // i4Y
                },
                // Horizon
                {
                    -221,    // OriX_Hor
                    -68    // OriY_Hor
                },
                // A
                {
                    -159,    // OriX_A
                    -103    // OriY_A
                },
                // TL84
                {
                    -57,    // OriX_TL84
                    -165    // OriY_TL84
                },
                // CWF
                {
                    -31,    // OriX_CWF
                    -193    // OriY_CWF
                },
                // DNP
                {
                    62,    // OriX_DNP
                    -161    // OriY_DNP
                },
                // D65
                {
                    161,    // OriX_D65
                    -136    // OriY_D65
                },
                // DF
                {
                    133,    // OriX_DF
                    -195    // OriY_DF
                }
            },
            // Rotated XY coordinate of AWB light source
            {
                // Strobe
                {
                    0,    // i4X
                    0    // i4Y
                },
                // Horizon
                {
                    -213,    // RotX_Hor
                    -91    // RotY_Hor
                },
                // A
                {
                    -148,    // RotX_A
                    -119    // RotY_A
                },
                // TL84
                {
                    -39,    // RotX_TL84
                    -170    // RotY_TL84
                },
                // CWF
                {
                    -11,    // RotX_CWF
                    -196    // RotY_CWF
                },
                // DNP
                {
                    79,    // RotX_DNP
                    -154    // RotY_DNP
                },
                // D65
                {
                    175,    // RotX_D65
                    -118    // RotY_D65
                },
                // DF
                {
                    153,    // RotX_DF
                    -180    // RotY_DF
                }
            },
            // AWB gain of AWB light source
            {
                // Strobe 
                {
                    512,    // i4R
                    512,    // i4G
                    512    // i4B
                },
                // Horizon 
                {
                    512,    // AWBGAIN_HOR_R
                    630,    // AWBGAIN_HOR_G
                    932    // AWBGAIN_HOR_B
                },
                // A 
                {
                    512,    // AWBGAIN_A_R
                    552,    // AWBGAIN_A_G
                    788    // AWBGAIN_A_B
                },
                // TL84 
                {
                    593,    // AWBGAIN_TL84_R
                    512,    // AWBGAIN_TL84_G
                    692    // AWBGAIN_TL84_B
                },
                // CWF 
                {
                    637,    // AWBGAIN_CWF_R
                    512,    // AWBGAIN_CWF_G
                    693    // AWBGAIN_CWF_B
                },
                // DNP 
                {
                    692,    // AWBGAIN_DNP_R
                    512,    // AWBGAIN_DNP_G
                    585    // AWBGAIN_DNP_B
                },
                // D65 
                {
                    792,    // AWBGAIN_D65_R
                    530,    // AWBGAIN_D65_G
                    512    // AWBGAIN_D65_B
                },
                // DF 
                {
                    798,    // AWBGAIN_DF_R
                    512,    // AWBGAIN_DF_G
                    557    // AWBGAIN_DF_B
                }
            },
            // Rotation matrix parameter
            {
                -6,    // RotationAngle
                255,    // Cos
                -27    // Sin
            },
            // Daylight locus parameter
            {
                -102,    // i4SlopeNumerator
                128    // i4SlopeDenominator
            },
            // Predictor gain
            {
                100, // i4PrefRatio100
                // DaylightLocus_L
                {
                    792,    // i4R
                    530,    // i4G
                    512    // i4B
                },
                // DaylightLocus_H
                {
                    677,    // i4R
                    512,    // i4G
                    545    // i4B
                },
                // Temporal General
                {
                    792,    // i4R
                    530,    // i4G
                    512    // i4B
                },
                // AWB LSC Gain
                {
                    650,        // i4R
                    512,        // i4G
                    564        // i4B
                }
            },
            // AWB light area
            {
                // Strobe:FIXME
                {
                    0,    // i4RightBound
                    0,    // i4LeftBound
                    0,    // i4UpperBound
                    0    // i4LowerBound
                },
                // Tungsten
                {
                    -89,    // TungRightBound
                    -739,    // TungLeftBound
                    -55,    // TungUpperBound
                    -155    // TungLowerBound
                },
                // Warm fluorescent
                {
                    -89,    // WFluoRightBound
                    -739,    // WFluoLeftBound
                    -155,    // WFluoUpperBound
                    -275    // WFluoLowerBound
                },
                // Fluorescent
                {
                    29,    // FluoRightBound
                    -89,    // FluoLeftBound
                    -46,    // FluoUpperBound
                    -183    // FluoLowerBound
                },
                // CWF
                {
                29,    // CWFRightBound
                -89,    // CWFLeftBound
                -183,    // CWFUpperBound
                -246    // CWFLowerBound
                },
                // Daylight
                {
                    200,    // DayRightBound
                    29,    // DayLeftBound
                    -38,    // DayUpperBound
                    -198    // DayLowerBound
                },
                // Shade
                {
                    560,    // ShadeRightBound
                    200,    // ShadeLeftBound
                    -38,    // ShadeUpperBound
                    -198    // ShadeLowerBound
                },
                // Daylight Fluorescent
                {
                    205,    // DFRightBound
                    55,    // DFLeftBound
                    -186,    // DFUpperBound
                    -241    // DFLowerBound
                }
            },
            // PWB light area
            {
                // Reference area
                {
                    560,    // PRefRightBound
                    -739,    // PRefLeftBound
                    0,    // PRefUpperBound
                    -275    // PRefLowerBound
                },
                // Daylight
                {
                    225,    // PDayRightBound
                    29,    // PDayLeftBound
                    -38,    // PDayUpperBound
                    -198    // PDayLowerBound
                },
                // Cloudy daylight
                {
                    325,    // PCloudyRightBound
                    150,    // PCloudyLeftBound
                    -38,    // PCloudyUpperBound
                    -198    // PCloudyLowerBound
                },
                // Shade
                {
                    425,    // PShadeRightBound
                    150,    // PShadeLeftBound
                    -38,    // PShadeUpperBound
                    -198    // PShadeLowerBound
                },
                // Twilight
                {
                    29,    // PTwiRightBound
                    -131,    // PTwiLeftBound
                    -38,    // PTwiUpperBound
                    -198    // PTwiLowerBound
                },
                // Fluorescent
                {
                    225,    // PFluoRightBound
                    -139,    // PFluoLeftBound
                    -68,    // PFluoUpperBound
                    -246    // PFluoLowerBound
                },
                // Warm fluorescent
                {
                    -48,    // PWFluoRightBound
                    -248,    // PWFluoLeftBound
                    -68,    // PWFluoUpperBound
                    -246    // PWFluoLowerBound
                },
                // Incandescent
                {
                    -48,    // PIncaRightBound
                    -248,    // PIncaLeftBound
                    -38,    // PIncaUpperBound
                    -198    // PIncaLowerBound
                },
                // Gray World
                {
                    5000,    // PGWRightBound
                    -5000,    // PGWLeftBound
                    5000,    // PGWUpperBound
                    -5000    // PGWLowerBound
                }
            },
            // PWB default gain	
            {
                // Daylight
                {
                    713,    // PWB_Day_R
                    512,    // PWB_Day_G
                    524    // PWB_Day_B
                },
                // Cloudy daylight
                {
                    840,    // PWB_Cloudy_R
                    512,    // PWB_Cloudy_G
                    459    // PWB_Cloudy_B
                },
                // Shade
                {
                    905,    // PWB_Shade_R
                    512,    // PWB_Shade_G
                    432    // PWB_Shade_B
                },
                // Twilight
                {
                    547,    // PWB_Twi_R
                    512,    // PWB_Twi_G
                    649    // PWB_Twi_B
                },
                // Fluorescent
                {
                    659,    // PWB_Fluo_R
                    512,    // PWB_Fluo_G
                    614    // PWB_Fluo_B
                },
                // Warm fluorescent
                {
                    496,    // PWB_WFluo_R
                    512,    // PWB_WFluo_G
                    772    // PWB_WFluo_B
                },
                // Incandescent
                {
                    473,    // PWB_Inca_R
                    512,    // PWB_Inca_G
                    729    // PWB_Inca_B
                },
                // Gray World
                {
                    512,    // PWB_GW_R
                    512,    // PWB_GW_G
                    512    // PWB_GW_B
                }
            },
            // AWB preference color	
            {
                // Tungsten
                {
                    0,    // TUNG_SLIDER
                    4962    // TUNG_OFFS
                },
                // Warm fluorescent	
                {
                    0,    // WFluo_SLIDER
                    4615    // WFluo_OFFS
                },
                // Shade
                {
                    0,    // Shade_SLIDER
                    1342    // Shade_OFFS
                },
                // Sunset Area
                {
                    108,   // i4Sunset_BoundXr_Thr
                    -154    // i4Sunset_BoundYr_Thr
                },
                // Sunset Vertex
                {
                    108,   // i4Sunset_BoundXr_Thr
                    -154    // i4Sunset_BoundYr_Thr
                },
                // Shade F Area
                {
                    -9,   // i4BoundXrThr
                    -118    // i4BoundYrThr
                },
                // Shade F Vertex
                {
                    -9,   // i4BoundXrThr
                    -118    // i4BoundYrThr
                },
                // Shade CWF Area
                {
                    -11,   // i4BoundXrThr
                    -196    // i4BoundYrThr
                },
                // Shade CWF Vertex
                {
                    -11,   // i4BoundXrThr
                    -196    // i4BoundYrThr
                },
                // Low CCT Area
                {
                    -233,   // i4BoundXrThr
                    341    // i4BoundYrThr
                },
                // Low CCT Vertex
                {
                    -233,   // i4BoundXrThr
                    341    // i4BoundYrThr
                }
            },
            // CCT estimation
            {
                // CCT
                {
                    2300,    // i4CCT[0]
                    2850,    // i4CCT[1]
                    3750,    // i4CCT[2]
                    5100,    // i4CCT[3]
                    6500     // i4CCT[4]
                },
                // Rotated X coordinate
                {
                -388,    // i4RotatedXCoordinate[0]
                -323,    // i4RotatedXCoordinate[1]
                -214,    // i4RotatedXCoordinate[2]
                -96,    // i4RotatedXCoordinate[3]
                0    // i4RotatedXCoordinate[4]
                }
            }
        },
            // Algorithm Tuning Paramter
            {
                // AWB Backup Enable
                0,

            // Daylight locus offset LUTs for tungsten
            {
                21, // i4Size: LUT dimension
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000} // i4LUTOut
            },
            // Daylight locus offset LUTs for WF
            {
                21, // i4Size: LUT dimension
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000} // i4LUTOut
            },
            // Daylight locus offset LUTs for Shade
            {
                21, // i4Size: LUT dimension
                {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000}, // i4LUTIn
                {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000} // i4LUTOut
            },
            // Preference gain for each light source
            {
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 515, 512}
                }, // STROBE
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // TUNGSTEN
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // WARM F
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // F
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // CWF
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {502, 512, 522}, {502, 512, 522}, {502, 512, 522}
                }, // DAYLIGHT
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // SHADE
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                } // DAYLIGHT F
            },
                // Parent block weight parameter
                {
                    1,      // bEnable
                    6           // i4ScalingFactor: [6] 1~12, [7] 1~6, [8] 1~3, [9] 1~2, [>=10]: 1
                },
                // AWB LV threshold for predictor
                {
                    115,    // i4InitLVThr_L
                    155,    // i4InitLVThr_H
                    100      // i4EnqueueLVThr
                },
                // AWB number threshold for temporal predictor
                {
                    65,     // i4Neutral_ParentBlk_Thr
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   2,   2,   2,   2,   2,   2,   2,   2}  // (%) i4CWFDF_LUTThr
                },
                // AWB light neutral noise reduction for outdoor
                {
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    // Non neutral
                    {   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // Flurescent
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // CWF
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // Daylight
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   2,   2,   2,   2,   2,   2,   2},  // (%)
                    // DF
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                },
                // AWB feature detection
                {
                // Sunset Prop
                    {
                        1,          // i4Enable
                        120,        // i4LVThr_L
                        140,        // i4LVThr_H
                        10,         // i4SunsetCountThr
                        0,          // i4SunsetCountRatio_L
                        171         // i4SunsetCountRatio_H
                    },
                    // Shade F Detection
                    {
                        1,          // i4Enable
                        95,        // i4LVThr_L
                        115,        // i4LVThr_H
                        128         // i4DaylightProb
                    },
                    // Shade CWF Detection
                    {
                        1,          // i4Enable
                        90,        // i4LVThr_L
                        110,        // i4LVThr_H
                        128         // i4DaylightProb
                    },
                    // Low CCT
                    {
                        1,          // i4Enable
                        256        // i4SpeedRatio
                    }
                },
                // AWB non-neutral probability for spatial and temporal weighting look-up table (Max: 100; Min: 0)
                {
                    //LV0   1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  70,  30,  20,  10,   0,   0,   0,   0}
                },
                // AWB daylight locus probability look-up table (Max: 100; Min: 0)
                {   //LV0    1     2     3      4     5     6     7     8      9      10     11    12   13     14    15   16    17    18
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   0,   0,   0,   0}, // Strobe
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   0,   0,   0}, // Tungsten
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   0,   0,   0}, // Warm fluorescent
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  70,  40,  20,   0,   0,   0}, // Fluorescent
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,  12,   0,   0,   0,   0}, // CWF
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  50,  30,  20}, // Daylight
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   0,   0,   0,   0}, // Shade
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,  12,   0,   0,   0,   0} // Daylight fluorescent
                }
            }
            }
    },
    // Flash AWB NVRAM
    {
#include INCLUDE_FILENAME_FLASH_AWB_PARA
    },
    {0}
};

#include INCLUDE_FILENAME_ISP_LSC_PARAM
//};  //  namespace

const CAMERA_TSF_TBL_STRUCT CAMERA_TSF_DEFAULT_VALUE =
{
    {
                0,  // isTsfEn
        2, // tsfCtIdx
        {20, 2000, -110, -110, 512, 512, 512, 0}    // rAWBInput[8]
    },
#include INCLUDE_FILENAME_TSF_PARA
#include INCLUDE_FILENAME_TSF_DATA
};




typedef NSFeature::RAWSensorInfo<SENSOR_ID> SensorInfoSingleton_T;


namespace NSFeature {
template <>
UINT32
SensorInfoSingleton_T::
impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, VOID*const pDataBuf, UINT32 const size) const
{
    UINT32 dataSize[CAMERA_DATA_TYPE_NUM] = {sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT),
                                             sizeof(NVRAM_CAMERA_3A_STRUCT),
                                             sizeof(NVRAM_CAMERA_SHADING_STRUCT),
                                             sizeof(NVRAM_LENS_PARA_STRUCT),
                                             sizeof(AE_PLINETABLE_T),
                                             0,
                                             sizeof(CAMERA_TSF_TBL_STRUCT)};

    if (CameraDataType > CAMERA_DATA_TSF_TABLE || NULL == pDataBuf || (size < dataSize[CameraDataType]))
    {
        return 1;
    }

    switch(CameraDataType)
    {
        case CAMERA_NVRAM_DATA_ISP:
            memcpy(pDataBuf,&CAMERA_ISP_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_3A:
            memcpy(pDataBuf,&CAMERA_3A_NVRAM_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_3A_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_SHADING:
            memcpy(pDataBuf,&CAMERA_SHADING_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_SHADING_STRUCT));
            break;
        case CAMERA_DATA_AE_PLINETABLE:
            memcpy(pDataBuf,&g_PlineTableMapping,sizeof(AE_PLINETABLE_T));
            break;
        case CAMERA_DATA_TSF_TABLE:
            memcpy(pDataBuf,&CAMERA_TSF_DEFAULT_VALUE,sizeof(CAMERA_TSF_TBL_STRUCT));
            break;
        default:
            return 1;
    }
    return 0;
}}; // NSFeature


