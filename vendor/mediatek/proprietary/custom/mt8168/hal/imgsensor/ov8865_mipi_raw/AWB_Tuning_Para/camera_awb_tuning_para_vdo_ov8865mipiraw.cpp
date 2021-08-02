#include "camera_custom_nvram.h"
#include "camera_awb_tuning_para_ov8865mipiraw.h"

#define NVRAM_TUNING_PARAM_NUM  6511001

template <>
AWB_NVRAM_T const&
getAWBNVRAM_ov8865mipiraw<CAM_SCENARIO_VIDEO>() // VIDEO
{
    static AWB_NVRAM_T  g_rAWBNVRAM =
    AWB_NVRAM_START
        {
            {
                // AWB calibration data
                {
                    // rUnitGain (unit gain: 1.0 = 512)
                    {
                        0, // UnitGain_R
                        0, // UnitGain_G
                        0  // UnitGain_B
                    },
                    // rGoldenGain (golden sample gain: 1.0 = 512)
                    {
                        0, // GoldenGain_R
                        0, // GoldenGain_G
                        0  // GoldenGain_B
                    },
                    // rUnitGain for TL84(unit gain: 1.0 = 512)
                    {
                        0, // UnitGain_R
                        0, // UnitGain_G
                        0  // UnitGain_B
                    },
                    // rGoldenGain for TL84(golden sample gain: 1.0 = 512)
                    {
                        0, // GoldenGain_R
                        0, // GoldenGain_G
                        0  // GoldenGain_B
                    },
                    // rUnitGain for ALight(unit gain: 1.0 = 512)
                    {
                        0, // UnitGain_R
                        0, // UnitGain_G
                        0  // UnitGain_B
                    },
                    // rGoldenGain for ALight(golden sample gain: 1.0 = 512)
                    {
                        0, // GoldenGain_R
                        0, // GoldenGain_G
                        0  // GoldenGain_B
                    },
                    // rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
                    {
                        0, // TuningUnitGain_R
                        0, // TuningUnitGain_G
                        0 // TuningUnitGain_B
                    },
                    // rD65Gain (D65 WB gain: 1.0 = 512)
                    {
                       837,  // i4R
                       512,  // i4G
                       597  // i4B
                    }
                },
                // Original XY coordinate of AWB light source
                {
                    // Strobe
                    {
                        0, // OriX_Strobe
                        0 // OriY_Strobe
                    },
                    // rHorizon
                    {
                        -382,  // i4X
                        -198  // i4Y
                    },
                    // rA
                    {
                        -277,  // i4X
                        -228  // i4Y
                    },
                    // rTL84
                    {
                        -167,  // i4X
                        -246  // i4Y
                    },
                    // rCWF
                    {
                        -118,  // i4X
                        -297  // i4Y
                    },
                    // rDNP
                    {
                        -28,  // i4X
                        -259  // i4Y
                    },
                    // rD65
                    {
                        125,  // i4X
                        -238  // i4Y
                    },
                    // rDF
                    {
                        0,  // i4X
                        0  // i4Y
                    }
                },
                // Rotated XY coordinate of AWB light source
                {
                    // Strobe
                    {
                        0, // RotX_Strobe
                        0 // RotY_Strobe
                    },
                                    // rHorizon
                {
                    -375,  // i4X
                    -211  // i4Y
                },
                // rA
                {
                    -269,  // i4X
                    -238  // i4Y
                },
                // rTL84
                {
                    -158,  // i4X
                    -252  // i4Y
                },
                // rCWF
                {
                    -108,  // i4X
                    -301  // i4Y
                },
                // rDNP
                {
                    -19,  // i4X
                    -260  // i4Y
                    },
                // rD65
                {
                    133,  // i4X
                    -234  // i4Y
                },
                // rDF
                {
                    111,  // i4X
                    -293  // i4Y
                }
                },
                // AWB gain of AWB light source
            {
                // rStrobe
                {
                    512,  // i4R
                    512,  // i4G
                    512  // i4B
                },
                // rHorizon
                {
                    512,  // i4R
                    657,  // i4G
                    1440  // i4B
                },
                // rA
                {
                    512,  // i4R
                    548,  // i4G
                    1085  // i4B
                },
                // rTL84
                {
                    570,  // i4R
                    512,  // i4G
                    895  // i4B
                },
                // rCWF
                {
                    653,  // i4R
                    512,  // i4G
                    899  // i4B
                },
                // rDNP
                {
                    700,  // i4R
                    512,  // i4G
                    756  // i4B
                },
                // rD65
                {
                    837,  // i4R
                    512,  // i4G
                    597  // i4B
                },
                // rDF
                {
                    512,  // i4R
                    512,  // i4G
                    512  // i4B
                }
            },
            // rRotationMatrix
            {
                -2,  // i4RotationAngle
                256,  // i4Cos
                -9  // i4Sin
            },
           // rDaylightLocus
            {
                -117,  // i4SlopeNumerator
                128  // i4SlopeDenominator
            },
	            // Predictor gain
            {
                101,  // i4PrefRatio100
                 // rSpatial_DaylightLocus_L
                {
                    807,  // i4R
                    530,  // i4G
                    617  // i4B
                },
                // rSpatial_DaylightLocus_H
                {
                    672,  // i4R
                    512,  // i4G
                    733  // i4B
                },
                // rTemporal_General
                {
                    837,  // i4R
                    512,  // i4G
                    597  // i4B
                },
                // rAWBGain_LSC
                {
                    656,  // i4R
                    512,  // i4G
                    749  // i4B
                }
            },
                // AWB light area
            {
                // rStrobe
                {
                    0,  // i4RightBound
                    0,  // i4LeftBound
                    0,  // i4UpperBound
                    0  // i4LowerBound
                },
                // rTungsten
                {
                    -202,  // i4RightBound
                    -775,  // i4LeftBound
                    -156,  // i4UpperBound
                    -245  // i4LowerBound
                },
                // rWarmFluorescent
                {
                    -202,  // i4RightBound
                    -775,  // i4LeftBound
                    -245,  // i4UpperBound
                    -391  // i4LowerBound
                },
                // rFluorescent
                {
                    -64,  // i4RightBound
                    -202,  // i4LeftBound
                    -174,  // i4UpperBound
                    -277  // i4LowerBound
                },
                // rCWF
                {
                    -12,  // i4RightBound
                    -202,  // i4LeftBound
                    -277,  // i4UpperBound
                    -356  // i4LowerBound
                },
                // rDaylight
                {
                    163,  // i4RightBound
                    -64,  // i4LeftBound
                    -174,  // i4UpperBound
                    -277  // i4LowerBound
                },
                // rShade
                {
                    493,  // i4RightBound
                    163,  // i4LeftBound
                    -174,  // i4UpperBound
                    -261  // i4LowerBound
                },
                // rDaylightFluorescent
                {
                    163,  // i4RightBound
                    -12,  // i4LeftBound
                    -277,  // i4UpperBound
                    -356  // i4LowerBound
                }
            },
                // PWB light area
                {
                    // Reference area
                    {
                        529, // PRefRightBound
                        -782, // PRefLeftBound
                        -305, // PRefUpperBound
                        -530 // PRefLowerBound
                    },
                    // Daylight
                    {
                        224, // PDayRightBound
                        28, // PDayLeftBound
                        -348, // PDayUpperBound
                        -464 // PDayLowerBound
                    },
                    // Cloudy daylight
                    {
                        324, // PCloudyRightBound
                        149, // PCloudyLeftBound
                        -348, // PCloudyUpperBound
                        -464 // PCloudyLowerBound
                    },
                    // Shade
                    {
                        424, // PShadeRightBound
                        149, // PShadeLeftBound
                        -348, // PShadeUpperBound
                        -464 // PShadeLowerBound
                    },
                    // Twilight
                    {
                        28, // PTwiRightBound
                        -132, // PTwiLeftBound
                        -348, // PTwiUpperBound
                        -464 // PTwiLowerBound
                    },
                    // Fluorescent
                    {
                        219, // PFluoRightBound
                        -169, // PFluoLeftBound
                        -338, // PFluoUpperBound
                        -535 // PFluoLowerBound
                    },
                    // Warm fluorescent
                    {
                        -158, // PWFluoRightBound
                        -358, // PWFluoLeftBound
                        -338, // PWFluoUpperBound
                        -535 // PWFluoLowerBound
                    },
                    // Incandescent
                    {
                        -158, // PIncaRightBound
                        -358, // PIncaLeftBound
                        -348, // PIncaUpperBound
                        -464 // PIncaLowerBound
                    },
                    // Gray World
                    {
                        5000, // PGWRightBound
                        -5000, // PGWLeftBound
                        5000, // PGWUpperBound
                        -5000 // PGWLowerBound
                    }
                },
                // PWB default gain
                {
                    // Daylight
                    {
                        1052, // PWB_Day_R
                        512, // PWB_Day_G
                        748 // PWB_Day_B
                    },
                    // Cloudy daylight
                    {
                        1222, // PWB_Cloudy_R
                        512, // PWB_Cloudy_G
                        644 // PWB_Cloudy_B
                    },
                    // Shade
                    {
                        1307, // PWB_Shade_R
                        512, // PWB_Shade_G
                        602 // PWB_Shade_B
                    },
                    // Twilight
                    {
                        827, // PWB_Twi_R
                        512, // PWB_Twi_G
                        952 // PWB_Twi_B
                    },
                    // Fluorescent
                    {
                        956, // PWB_Fluo_R
                        512, // PWB_Fluo_G
                        894 // PWB_Fluo_B
                    },
                    // Warm fluorescent
                    {
                        652, // PWB_WFluo_R
                        512, // PWB_WFluo_G
                        1311 // PWB_WFluo_B
                    },
                    // Incandescent
                    {
                        626, // PWB_Inca_R
                        512, // PWB_Inca_G
                        1258 // PWB_Inca_B
                    },
                    // Gray World
                    {
                        512, // PWB_GW_R
                        512, // PWB_GW_G
                        512 // PWB_GW_B
                    }
                },
                // AWB preference color
            {
                // rTungsten
                {
                    40,  // i4SliderValue
                    8500  // i4OffsetThr 4700
                },
                // rWarmFluorescent
                {
                    40,  // i4SliderValue
                    8500  // i4OffsetThr
                },
                // rShade
                {
                    50,  // i4SliderValue
                    909  // i4OffsetThr
                },
                // rSunsetArea
                {
                    16,  // i4BoundXrThr
                    -260  // i4BoundYrThr
                },
                // rShadeFArea
                {
                    -202,  // i4BoundXrThr
                    -256  // i4BoundYrThr
                },
                // rShadeFVerTex
                {
                    -133,  // i4BoundXrThr
                    -267  // i4BoundYrThr
                },
                // rShadeCWFArea
                {
                    -202,  // i4BoundXrThr
                    -305  // i4BoundYrThr
                },
                // rShadeCWFVerTex
                {
                    -107,  // i4BoundXrThr
                    -331  // i4BoundYrThr
                }
            },
                // CCT estimation
                {
	                // CCT
	                {
		                2300,	// i4CCT[0]
		                2850,	// i4CCT[1]
		                3750,	// i4CCT[2]
		                5100,	// i4CCT[3]
		                6500 	// i4CCT[4]
	                },
	                // Rotated X coordinate
	                {
		                -508,	// i4RotatedXCoordinate[0]
		                -402,	// i4RotatedXCoordinate[1]
		                -291,	// i4RotatedXCoordinate[2]
		                -152,	// i4RotatedXCoordinate[3]
		                0 	    // i4RotatedXCoordinate[4]
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
                    {0, 250,  500, 1333, 1667, 2313, 2625, 2938, 3250, 3563, 3875, 4188, 4500, 4813, 5125, 5438, 5750, 6063, 6375, 6688,  7000} // i4LUTOut
                },

                // Daylight locus offset LUTs for WF
                {
                    21, // i4Size: LUT dimension
                    {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                    {0, 350,  850, 1250, 1500, 1767, 1950, 2111, 2333, 2556, 2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778,  5000} // i4LUTOut
                },

                // Daylight locus offset LUTs for shade
                {
                    21, // i4Size: LUT dimension
                    {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                    {0, 500, 1000, 1500, 2500, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8000, 8000, 8500, 9000, 9500, 10000}  // i4LUTOut
                },
                // Preference gain for each light source
                {
                    //        LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                    {
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                    //        LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
          	            {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
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
                    115, //100,    // i4InitLVThr_L
                    155, //140,    // i4InitLVThr_H
                    100 //80      // i4EnqueueLVThr
                },
                // AWB number threshold for temporal predictor
                {
                        65,     // i4Neutral_ParentBlk_Thr
                    //LV0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   2,   2,   2,   2,   2,   2,   2}  // (%) i4CWFDF_LUTThr
                },
                // AWB light neutral noise reduction for outdoor
                {
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    // Non neutral
	                { 3,   3,   3,   3,   3,   3,   3,   3,    3,   3,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
	                // Flurescent
	                { 0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
	                // CWF
	                { 0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
	                // Daylight
	                { 0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   2,   2,   2,   2,   2,   2,   2,   2},  // (%)
	                // DF
	                { 0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                },
                // AWB feature detection
                {
                    // Sunset Prop
                    {
                        0,          // i4Enable
                        120,        // i4LVThr_L
                        130,        // i4LVThr_H
                        10,         // i4SunsetCountThr
                        0,          // i4SunsetCountRatio_L
                        171         // i4SunsetCountRatio_H
                    },

                    // Shade F Detection
                    {
                        0,          // i4Enable
                        50,//105,        // i4LVThr
                        90,//105,        // i4LVThr
                        96 //10         // i4DaylightProb
                    },

                    // Shade CWF Detection
                    {
                        0,          // i4Enable
                        50,//95,         // i4LVThr
                        90,//95,         // i4LVThr
                        192 //224 //128 //35         // i4DaylightProb
                    },
                },

                // AWB non-neutral probability for spatial and temporal weighting look-up table (Max: 100; Min: 0)
                {
                    //LV0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    {  90, 90,  90,  90,  90,  90,  90,  90,  90,  90,   90,  50,  30,  20,  10,   0,   0,   0,   0}
                },

                // AWB daylight locus probability look-up table (Max: 100; Min: 0)
                {
                    //LV0  1    2    3    4    5    6    7    8    9     10    11   12   13   14   15  16   17   18
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  50,  25,   0,  0,   0,   0}, // Strobe
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  25,  25,  25,  0,   0,   0}, // Tungsten
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  25,  25,  25,  0,   0,   0}, // Warm fluorescent
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  25,  25,  25,  0,   0,   0}, // Fluorescent
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  30,  30,  30,  0,   0,   0}, // CWF
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100, 100,  75,  50, 40,  30,  20}, // Daylight
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  50,  25,   0,  0,   0,   0}, // Shade
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  90,  75,  50, 30,   0,   0}  // Daylight fluorescent
                },
                // AWB tuning information
                {
                     0, // i4ProjCode
                     0, // i4Model
                     0, // i4Date
                     0, // i4Res0
                     0, // i4Res1
                     0, // i4Res2
                     0, // i4Res3
                     0, // i4Res4
       },
    }
};
    return g_rAWBNVRAM;
};
