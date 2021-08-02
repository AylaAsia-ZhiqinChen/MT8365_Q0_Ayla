#include "camera_custom_nvram.h"
#include "camera_awb_tuning_para_gc2375mipiraw.h"

#define NVRAM_TUNING_PARAM_NUM  6511001

template <>
AWB_NVRAM_T const&
getAWBNVRAM_gc2375mipiraw<CAM_SCENARIO_PREVIEW>() // PREVIEW
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
                        810,  // i4R
                        512,  // i4G
                        717  // i4B
                    }
                },
                // Original XY coordinate of AWB light source
                {
                    // Strobe
                    {
                        0, // OriX_Strobe
                        0 // OriY_Strobe
                    },
                    // Horizon
                    {
                    -454,  // i4X
                    -184  // i4Y
                    },
                     // rA
                    {
                    -362,  // i4X
                    -238  // i4Y
                    },
                    // rTL84
                    {
                    -178,  // i4X
                    -372  // i4Y
                    },
                    // rCWF
                    {
                    -129,  // i4X
                    -411  // i4Y
                    },
                    // rDNP
                    {
                    -111,  // i4X
                    -328  // i4Y
                    },
                    // rD65
                    {
                    45,  // i4X
                    -294  // i4Y
                    },
                    // DF
                    {
                        0, // OriX_DF
                        0 // OriY_DF
                    }
                },
                // Rotated XY coordinate of AWB light source
                {
                    // Strobe
                    {
                        0, // RotX_Strobe
                        0 // RotY_Strobe
                    },
                    // Horizon
                    {
                    -428,  // i4X
                    -238  // i4Y
                    },
                    // rA
                    {
                        -330,  // i4X
                        -280  // i4Y
                    },
                    // rTL84
                    {
                        -132,  // i4X
                        -391  // i4Y
                    },
                    // rCWF
                    {
                        -78,  // i4X
                        -423  // i4Y
                    },
                    // rDNP
                    {
                        -70,  // i4X
                        -339  // i4Y
                    },
                    // rD65
                    {
                        80,  // i4X
                        -286  // i4Y
                    },
                    // rDF
                    {
                        54,  // i4X
                        -407  // i4Y
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
                        512,  // i4RR                             
                        737,  // i4GG                             
                        1748  // i4BB
                    },              
                    // rA           
                    {               
                        512,  // i4R                              
                        605,  // i4G                              
                        1363  // i4B
                    },              
                    // rTL84        
                    {               
                        666,  // i4R_R
                        512,  // i4G_G
                        1078  // i4B_B
                    },              
                    // rCWF         
                    {               
                        750,  // i4RR
                        512,  // i4GG
                        1064  // i4BB
                    },              
                    // rDNP         
                    {               
                        686,  // i4R
                        512,  // i4G
                        927  // i4B 
                    },              
                    // rD65         
                    {               
                        810,  // i4R
                        512,  // i4G
                        717  // i4B 
                    },              
                    // rDF          
                    {               
                        512,  // i4R
                        512,  // i4G
                        512  // i4B 
                    }               
                },
                // Rotation matrix parameter
                {                              
                    -7,  // i4RotationAngle    
                    254,  // i4Cos             
                    -31  // i4Sin              
                },                             
                // rDaylightLocus              
                {                              
                    -94,  // i4SlopeNumerator  
                    128  // i4SlopeDenominator 
                },                             
	            //// rPredictorGain              
                {                              
                    101,  // i4PrefRatio100    
                    // rSpatial_DaylightLocus_L
                    {                          
                        780,  // i4R           
                        530,  // i4G           
                        737  // i4B            
                    },                         
                    // rSpatial_DaylightLocus_H
                    {                          
                        692,  // i4R           
                        512,  // i4G           
                        810  // i4B            
                    },                         
                    // rTemporal_General       
                    {                          
                        810,  // i4R           
                        512,  // i4G           
                        717  // i4B            
                    },                         
                    // rAWBGain_LSC            
                    {                          
                        625,  // i4R           
                        512,  // i4G           
                        878  // i4B            
                    }                          
                },                             
                // rAWBLightArea               
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
                        -211,  // i4RightBound 
                        -828,  // i4LeftBound  
                        -183,  // i4UpperBound 
                        -336  // i4LowerBound  
                    },                         
                    // rWarmFluorescent        
                    {                          
                        -211,  // i4RightBound 
                        -828,  // i4LeftBound  
                        -336,  // i4UpperBound 
                        -513  // i4LowerBound  
                    },                         
                    // rFluorescent            
                    {                          
                        -90,  // i4RightBound  
                        -211,  // i4LeftBound  
                        -226,  // i4UpperBound 
                        -407  // i4LowerBound  
                    },                         
                    // rCWF                    
                    {                          
                        -20,  // i4RightBound  
                        -211,  // i4LeftBound  
                        -407,  // i4UpperBound 
                        -478  // i4LowerBound  
                    },                         
                    // rDaylight               
                    {                          
                        110,  // i4RightBound  
                        -90,  // i4LeftBound   
                        -226,  // i4UpperBound 
                        -407  // i4LowerBound  
                    },                         
                    // rShade                  
                    {                          
                        440,  // i4RightBound  
                        110,  // i4LeftBound   
                        -226,  // i4UpperBound 
                        -341  // i4LowerBound  
                    },                         
                    // rDaylightFluorescent    
                    {                          
                        110,  // i4RightBound  
                        -20,  // i4LeftBound   
                        -407,  // i4UpperBound 
                        -478  // i4LowerBound  
                    }                          
                },                             
                // PWB light area
                {
                    // Reference area
                 {                                 
                     440,  // i4RightBound         
                     -828,  // i4LeftBound         
                     -158,  // i4UpperBound        
                     -513  // i4LowerBound         
                 },                                
                 // rDaylight                      
                 {                                 
                     135,  // i4RightBound         
                     -90,  // i4LeftBound          
                     -226,  // i4UpperBound        
                     -407  // i4LowerBound         
                 },                                
                 // rCloudyDaylight                
                 {                                 
                     235,  // i4RightBound         
                     60,  // i4LeftBound           
                     -226,  // i4UpperBound        
                     -407  // i4LowerBound         
                 },                                
                 // rShade                         
                 {                                 
                     335,  // i4RightBound         
                     60,  // i4LeftBound           
                     -226,  // i4UpperBound        
                     -407  // i4LowerBound         
                 },                                
                 // rTwilight                      
                 {                                 
                     -90,  // i4RightBound         
                     -250,  // i4LeftBound         
                     -226,  // i4UpperBound        
                     -407  // i4LowerBound         
                 },                                
                 // rFluorescent                   
                 {                                 
                     130,  // i4RightBound         
                     -232,  // i4LeftBound         
                     -236,  // i4UpperBound        
                     -473  // i4LowerBound         
                 },                                
                 // rWarmFluorescent               
                 {                                 
                     -230,  // i4RightBound        
                     -430,  // i4LeftBound         
                     -236,  // i4UpperBound        
                     -473  // i4LowerBound         
                 },                                
                 // rIncandescent                  
                 {                                 
                     -230,  // i4RightBound        
                     -430,  // i4LeftBound         
                     -226,  // i4UpperBound        
                     -407  // i4LowerBound         
                 },                                
                 // rGrayWorld                     
                 {                                 
                     5000,  // i4RightBound        
                     -5000,  // i4LeftBound        
                     5000,  // i4UpperBound        
                     -5000  // i4LowerBound        
                 }                                 
             },                                    
             // rPWBDefaultGain                    
             {                                     
                 // rDaylight                      
                 {                                 
                     770,  // i4R                  
                     512,  // i4G                  
                     804  // i4B                   
                 },                                
                 // rCloudyDaylight                
                 {                                 
                     929,  // i4R                  
                     512,  // i4G                  
                     693  // i4B                   
                 },                                
                 // rShade                         
                 {                                 
                     1002,  // i4R                 
                     512,  // i4G                  
                     654  // i4B                   
                 },                                
                 // rTwilight                      
                 {                                 
                     576,  // i4R                  
                     512,  // i4G                  
                     1009  // i4B                  
                 },                                
                 // rFluorescent                   
                 {                                 
                     720,  // i4R                  
                     512,  // i4G                  
                     928  // i4B                   
                 },                                
                 // rWarmFluorescent               
                 {                                 
                     473,  // i4R                  
                     512,  // i4G                  
                     1290  // i4B                  
                 },                                
                 // rIncandescent                  
                 {                                 
                     452,  // i4R                  
                     512,  // i4G                  
                     1218  // i4B                  
                 },                                
                 // rGrayWorld                     
                 {                                 
                     512,  // i4R                  
                     512,  // i4G                  
                     512  // i4B                   
                 }                                 
             },                                    
             // rPreferenceColor                   
             {                                     
                 // rTungsten                      
                 {                                 
                     40,  // i4SliderValue 40        
                     6800,//4198  // i4OffsetThr          
                 },                                
                 // rWarmFluorescent               
                 {                                 
                     40,  // i4SliderValue 40        
                     6800,//4198  // i4OffsetThr          // WFluo_OFFS
                 },                                
                 // rShade                         
                 {                                 
                     50,  // i4SliderValue         
                     912  // i4OffsetThr           
                 },                                
                 // rSunsetArea                    
                 {                                 
                     -54,  // i4BoundXrThr         
                     -339  // i4BoundYrThr         
                 },                                
                 // rShadeFArea                    
                 {                                 
                     -211,  // i4BoundXrThr        
                     -395  // i4BoundYrThr         
                 },                                
                 // rShadeFVerTex                  
                 {                                 
                     -151,  // i4BoundXrThr        
                     -401  // i4BoundYrThr         
                 },                                
                 // rShadeCWFArea                  
                 {                                 
                     -211,  // i4BoundXrThr        
                     -427  // i4BoundYrThr         
                 },                                
                 // rShadeCWFVerTex                
                 {                                 
                     -116,  // i4BoundXrThr        
                     -453  // i4BoundYrThr         
 
                    }
                },
                // CCT estimation
                {
                    // CCT
                    {
                        2300,  // CCT0
                        2850,  // CCT1
                        3750,  // CCT2
                        5100,  // CCT3
                        6500  // CCT4
                    },
                    // Rotated X coordinate
                    {
                        -508,  // RotatedXCoordinate0
                        -410,  // RotatedXCoordinate1
                        -212,  // RotatedXCoordinate2
                        -150,  // RotatedXCoordinate3
                        0  // RotatedXCoordinate4
                    }
                }
            },
            // Algorithm Tuning Paramter
            {
                // AWB Backup Enable
                0,
                // Daylight locus offset LUTs for tungsten
                {
                    21,// i4Size: LUT dimension
                    {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                    {0, 350, 800, 1222, 1444, 1667, 1889, 2111, 2333, 2556, 2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778, 5000}   // i4LUTOut
                },
                // Daylight locus offset LUTs for WF
                {
                    21,// i4Size: LUT dimension
                    {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                    {0, 350, 700, 1000, 1444, 1667, 1889, 2111, 2333, 2556, 2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778, 5000}   // i4LUTOut
                },
                // Daylight locus offset LUTs for Shade
                {
                    21,// i4Size: LUT dimension
                    {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                    {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}   // i4LUTOut
                },
                // Preference gain for each light source
                {
                    {
                        //    LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        //    LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // STROBE
                    {
                        //    LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        //    LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // TUNGSTEN
                    {
                        //    LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        //    LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // WARM F
                    {
                        //    LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        //    LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // F
                    {
                        //    LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        //    LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // CWF
                    {
                        //    LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        //    LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // DAYLIGHT
                    {
                        //    LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        //    LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // SHADE
                    {
                        //    LV0              LV1              LV2              LV3              LV4              LV5              LV6              LV7              LV8              LV9
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        //    LV10             LV11             LV12             LV13             LV14             LV15             LV16             LV17             LV18
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    } // DAYLIGHT F
                },
                // Parent block weight parameter
                {
                    1,  // bEnable
                    6  // i4ScalingFactor: [6] 1~12, [7] 1~6, [8] 1~3, [9] 1~2, [>=10]: 1
                },
                // AWB LV threshold for predictor
                {
                    90,  // i4InitLVThr_L
                    130,  // i4InitLVThr_H
                    100  // i4EnqueueLVThr
                },
                // AWB number threshold for temporal predictor
                {
                    65,  // i4Neutral_ParentBlk_Thr
                    //LV0    1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18
                    { 100,  100,  100,  100,  100,  100,  100,  100,  100,  100,   50,   25,    2,    2,    2,    2,    2,    2,    2}  // i4CWFDF_LUTThr
                },
                // AWB light neutral noise reduction for outdoor
                {
                    //LV0  1   2   3   4   5   6   7   8   9   10   11   12   13   14   15   16   17   18
                    // Non neutral
                    {  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  10,  10,  10,  10,  10,  10,  10,  10},  //
                    // Flurescent
                    {  0,  0,  0,  0,  0,  3,  5,  5,  5,  5,  5,  10,  10,  10,  10,  10,  10,  10,  10},  //
                    // CWF
                    {  0,  0,  0,  0,  0,  3,  5,  5,  5,  5,  5,  10,  10,  10,  10,  10,  10,  10,  10},  //
                    // Daylight
                    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   2,   2,   2,   2,   2,   2,   2,   2},  //
                    // DF
                    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  10,  10,  10,  10,  10,  10,  10,  10}  //
                },
                // AWB feature detection
                {
                     // Sunset Prop
                     {
                         1, // i4Enable
                         120, // i4LVThr_L
                         130, // i4LVThr_H
                         10, // i4SunsetCountThr
                         0, // i4SunsetCountRatio_L
                         171 // i4SunsetCountRatio_H
                     },
                     // Shade F Detection
                     {
                         0, // i4Enable
                         90, // i4LVThr_L
                         115, // i4LVThr_H
                         128 // i4DaylightProb
                     },
                     // Shade CWF Detection
                     {
                         0, // i4Enable
                         90, // i4LVThr_L
                         115, // i4LVThr_H
                         208 // i4DaylightProb
                     }
                 },
                 // AWB non-neutral probability for spatial and temporal weighting look-up table (Max: 100; Min: 0)
                 {
                     //LV0   1    2    3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18
                     {  0,  33,  66,  100,  100,  100,  100,  100,  100,  100,  100,   70,   30,   20,   10,    0,    0,    0,    0}
                },
                // AWB daylight locus probability look-up table (Max: 100; Min: 0)
                {
                    //LV0    1     2     3     4     5     6     7     8     9     10    11    12    13    14   15   16   17   18
                    { 100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,   50,   25,   0,   0,   0,   0}, // Strobe
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,   95,  75,  50,  25,  25,  0,   0,   0}, // Tungsten
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,   95,  75,  50,  25,  25,  0,   0,   0}, // Warm fluorescent
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,   95,  75,  50,  25,  25,  0,   0,   0}, // Fluorescent
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,   95,  75,  50,  25,  25,  0,   0,   0}, // CWF
                    { 100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,   75,  50,  40,  30,  20}, // Daylight
                    { 100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,   75,   50,   25,   0,   0,   0,   0}, // Shade
                    {  90,   90,   90,   90,   90,   90,   90,   90,   90,   90,   80,   55,   30,   30,   30,  30,   0,   0,   0}  // Daylight fluorescent
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
