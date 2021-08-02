/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_gc02m1mipi_raw_cxt_rear.h"
#include "camera_info_gc02m1mipi_raw_cxt_rear.h"
#include "camera_custom_AEPlinetable.h"
#include "camera_custom_tsf_tbl.h"
#include "AE_Tuning_Para/camera_ae_tuning_para_gc02m1mipi_raw_cxt_rear.h"
#define NVRAM_TUNING_PARAM_NUM 5341001

extern "C" fptrDefault getDefaultDataFunc(MUINT32 sensorType)
{
    if(sensorType==0)
        return NSFeature::RAWSensorInfo<SENSOR_ID>::getDefaultData;
    else
        return NSFeature::YUVSensorInfo<SENSOR_ID>::getDefaultData;
}

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
      0x01FF0001, // MIX3_CTRL_0
      0x00FF0000, // MIX3_CTRL_1
      0xFFFF0000  // MIX3_SPARE
    }},
    .ISPMulitCCM={
        .CCM_Method= 0,  //0:dynamic CCM, 1:SCCM(v1.0), 2:SCCM(v2.0)

        .dynamic_CCM=
        {
            {.set={//CT_D65
                0x1E390384, 0x00000043, 0x02FF1F8D, 0x00001F74, 0x1E661FDA, 0x000003C0
            }},
            {.set={//CT_TL84
                0x1E830365, 0x00000018, 0x02DF1F4B, 0x00001FD6, 0x1E291F91, 0x00000446
            }},
            {.set={//CT_CWF
                0x1EAA0348, 0x0000000E, 0x02DF1F5F, 0x00001FC2, 0x1E121F94, 0x0000045A
            }},
            {.set={//CT_A
                0x1ECD0336, 0x00001FFD, 0x02F41F1D, 0x00001FEF, 0x1C681EE6, 0x000006B2
            }}
        },

        .dynamic_CCM_AWBGain=
        {
            {//CT_D65
                900,  // i4R
                512,  // i4G
                793  // i4B
            },
            {//CT_TL84
                650,  // i4R
                512,  // i4G
                1004  // i4B
            },
            {//CT_CWF
                697,  // i4R
                512,  // i4G
                1003  // i4B
            },
            {//CT_A
                501,  // i4R
                512,  // i4G
                1234  // i4B
            }
        },
        .PQ_Type=0,
        .CCM_LV_valid_NUM=1,
        .CCM_FLASH_valid_NUM=1,

        .CCM_CT_valid_NUM = 5,

        .CCM=
        {
            {//LV_00
                {.set={//CT_00
                    0x1F64024C, 0x00000050, 0x03401F1A, 0x00001FA6, 0x1C391EC6, 0x00000701
                }},
                {.set={//CT_01
                    0x1EF90300, 0x00000007, 0x02C61F30, 0x0000000A, 0x1CC51F12, 0x00000629
                }},
                {.set={//CT_02
                    0x1EE6032E, 0x00001FEC, 0x02FC1F45, 0x00001FBF, 0x1E171F71, 0x00000478
                }},
                {.set={//CT_03
                    0x1EE2034B, 0x00001FD3, 0x02FA1F40, 0x00001FC6, 0x1E001F73, 0x0000048D
                }},
                {.set={//CT_04
                    0x1E3303A1, 0x0000002C, 0x02E51F9E, 0x00001F7D, 0x1EBB1FE3, 0x00000362
                }},
                {.set={//CT_05
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_06
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_07
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }}
            },
            {//LV_01
                {.set={//CT_00
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_01
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_02
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_03
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_04
             		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_05
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_06
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_07
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }}
            },
            {//LV_02
                {.set={//CT_00
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_01
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_02
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_03
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_04
             		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_05
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_06
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_07
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }}
            },
            {//LV_03
                {.set={//CT_00
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_01
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_02
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_03
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_04
             		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_05
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_06
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_07
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }}
            },
            {//LV_04
                {.set={//CT_00
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_01
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_02
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_03
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_04
             		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_05
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_06
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_07
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }}
            },
            {//LV_05
                {.set={//CT_00
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_01
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_02
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_03
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_04
             		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_05
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_06
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_07
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }}
            },
            {//FLASH_00
                {.set={//CT_00
                    0x1F6602D7, 0x00001FC3, 0x02071F98, 0x00000061, 0x1DE41FB8, 0x00000464
                }},
                {.set={//CT_01
                    0x1F7B02B3, 0x00001FD2, 0x022B1FB2, 0x00000023, 0x1E891FFB, 0x0000037C
                }},
                {.set={//CT_02
                    0x1F040306, 0x00001FF6, 0x02421FB3, 0x0000000B, 0x1F430006, 0x000002B7
                }},
                {.set={//CT_03
                    0x1E2603B6, 0x00000024, 0x021E1FAD, 0x00000035, 0x1F07001C, 0x000002DD
                }},
                {.set={//CT_04
                    0x1F0C02F4, 0x00000000, 0x02491FE0, 0x00001FD7, 0x1F26001B, 0x000002BF
                }},
                {.set={//CT_05
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_06
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_07
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }}
            },
            {//FLASH_01
                {.set={//CT_00
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_01
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_02
    				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_03
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_04
             		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_05
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_06
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }},
                {.set={//CT_07
                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
                }}
            }
        },

        .AWBGain=
        {
            {//LV_00
                {//CT_00
                    501,  // i4R
                    512,  // i4G
                    1234  // i4B
                },
                {//CT_01
                    697,  // i4R
                    512,  // i4G
                    1003  // i4B
                },
                {//CT_02
                    650,  // i4R
                    512,  // i4G
                    1004  // i4B
                },
                {//CT_03
                    900,  // i4R
                    512,  // i4G
                    793  // i4B
                },
                {//CT_04
                    919,  // i4R
                    512,  // i4G
                    747  // i4B
                },
                {//CT_05
                    512,  // i4R
                    512,  // i4G
                  512     // i4B
                },
                {//CT_06
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_07
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                }
            },
            {//LV_01
                {//CT_00
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_01
                  638,    // i4R
                  512,    // i4G
                  1457    // i4B
                },
                {//CT_02
                  930,    // i4R
                  512,    // i4G
                  1138    // i4B
                },
                {//CT_03
                  1052,   // i4R
                  512,    // i4G
                  1319    // i4B
                },
                {//CT_04
                  1233,   // i4R
                  512,    // i4G
                  787     // i4B
                },
                {//CT_05
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_06
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_07
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                }
            },
            {//LV_02
                {//CT_00
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_01
                  638,    // i4R
                  512,    // i4G
                  1457    // i4B
                },
                {//CT_02
                  930,    // i4R
                  512,    // i4G
                  1138    // i4B
                },
                {//CT_03
                  1052,   // i4R
                  512,    // i4G
                  1319    // i4B
                },
                {//CT_04
                  1233,   // i4R
                  512,    // i4G
                  787     // i4B
                },
                {//CT_05
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_06
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_07
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                }
            },
            {//LV_03
                {//CT_00
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_01
                  638,    // i4R
                  512,    // i4G
                  1457    // i4B
                },
                {//CT_02
                  930,    // i4R
                  512,    // i4G
                  1138    // i4B
                },
                {//CT_03
                  1052,   // i4R
                  512,    // i4G
                  1319    // i4B
                },
                {//CT_04
                  1233,   // i4R
                  512,    // i4G
                  787     // i4B
                },
                {//CT_05
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_06
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_07
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                }
            },
            {//LV_04
                {//CT_00
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_01
                  638,    // i4R
                  512,    // i4G
                  1457    // i4B
                },
                {//CT_02
                  930,    // i4R
                  512,    // i4G
                  1138    // i4B
                },
                {//CT_03
                  1052,   // i4R
                  512,    // i4G
                  1319    // i4B
                },
                {//CT_04
                  1233,   // i4R
                  512,    // i4G
                  787     // i4B
                },
                {//CT_05
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_06
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_07
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                }
            },
            {//LV_05
                {//CT_00
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_01
                  638,    // i4R
                  512,    // i4G
                  1457    // i4B
                },
                {//CT_02
                  930,    // i4R
                  512,    // i4G
                  1138    // i4B
                },
                {//CT_03
                  1052,   // i4R
                  512,    // i4G
                  1319    // i4B
                },
                {//CT_04
                  1233,   // i4R
                  512,    // i4G
                  787     // i4B
                },
                {//CT_05
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_06
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_07
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                }
            },
            {//FLASH_00
                {//CT_00
                  539,    // i4R
                  512,    // i4G
                 1741     // i4B
                },
                {//CT_01
                  671,    // i4R
                  512,    // i4G
                  1410    // i4B
                },
                {//CT_02
                  925,    // i4R
                  512,    // i4G
                 1082     // i4B
                },
                {//CT_03
                 1077,    // i4R
                  512,    // i4G
                 1242     // i4B
                },
                {//CT_04
                 1257,   // i4R
                  512,    // i4G
                  707     // i4B
                },
                {//CT_05
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_06
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_07
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                }
            },
            {//FLASH_01
                {//CT_00
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_01
                  638,    // i4R
                  512,    // i4G
                  1457    // i4B
                },
                {//CT_02
                  930,    // i4R
                  512,    // i4G
                  1138    // i4B
                },
                {//CT_03
                  1052,   // i4R
                  512,    // i4G
                  1319    // i4B
                },
                {//CT_04
                  1233,   // i4R
                  512,    // i4G
                  787     // i4B
                },
                {//CT_05
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_06
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                },
                {//CT_07
                  512,    // i4R
                  512,    // i4G
                  512     // i4B
                }
            }
        },

       .Weight=
       {
            {//LV_00
                1, 1, 1, 1, 1, 1, 1, 1
            },
            {//LV_01
                1, 1, 1, 1, 1, 1, 1, 1
            },
            {//LV_02
                1, 1, 1, 1, 1, 1, 1, 1
            },
            {//LV_03
                1, 1, 1, 1, 1, 1, 1, 1
            },
            {//LV_04
                1, 1, 1, 1, 1, 1, 1, 1
            },
            {//LV_05
                1, 1, 1, 1, 1, 1, 1, 1
            },
            {//FLASH_00
                1, 1, 1, 1, 1, 1, 1, 1
            },
            {//FLASH_01
                1, 1, 1, 1, 1, 1, 1, 1
            }
        },
        .Poly22=
        {
            {//LV_00
                68700,  // i4R_AVG
                16476,  // i4R_STD
                100850,  // i4B_AVG
                18013,  // i4B_STD
                900,  // i4R_MAX
                821,  // i4R_MIN
                766,  // i4G_MAX
                734,  // i4G_MIN
                1714,  // i4B_MAX
                959,  // i4B_MIN
                // i4P00
                {
                    8570000, -3705000, 250000, -1707500, 7475000, -650000, -1342500, -5730000, 12200000
                },
                // i4P10
                {
                    -248432, 46277, 228357, 352830, 683966, -1049897, -1106419, -4698074, 5791392
                },
                // i4P01
                {
                    -565591, 641354, -57920, -109592, 638172, -537502, -2099320, -6740041, 8830439
                },
                // i4P20
                {
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                // i4P11
                {
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                // i4P02
                {
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                }
            },
            {//LV_01
                 96325, // i4R_AVG
                 25002, // i4R_STD
                117525, // i4B_AVG
                 28993, // i4B_STD
    			  1257, // i4R_MAX
    			   851, // i4R_MIN
    			   703, // i4G_MAX
    			   652, // i4G_MIN
    			  1145, // i4B_MAX
    			   783, // i4B_MIN
                { // i4P00[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P10[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P01[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P20[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P11[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P02[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                }

             },
             {//LV_02
                 96325, // i4R_AVG
                 25002, // i4R_STD
                117525, // i4B_AVG
                 28993, // i4B_STD
    			  1257, // i4R_MAX
    			   851, // i4R_MIN
    			   703, // i4G_MAX
    			   652, // i4G_MIN
    			  1145, // i4B_MAX
    			   783, // i4B_MIN
                { // i4P00[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P10[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P01[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P20[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P11[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P02[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                }

              },
              {//LV_03
                 96325, // i4R_AVG
                 25002, // i4R_STD
                117525, // i4B_AVG
                 28993, // i4B_STD
    			  1257, // i4R_MAX
    			   851, // i4R_MIN
    			   703, // i4G_MAX
    			   652, // i4G_MIN
    			  1145, // i4B_MAX
    			   783, // i4B_MIN
                { // i4P00[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P10[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P01[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P20[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P11[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P02[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                }

              },
              {//LV_04
                 96325, // i4R_AVG
                 25002, // i4R_STD
                117525, // i4B_AVG
                 28993, // i4B_STD
    			  1257, // i4R_MAX
    			   851, // i4R_MIN
    			   703, // i4G_MAX
    			   652, // i4G_MIN
    			  1145, // i4B_MAX
    			   783, // i4B_MIN
                { // i4P00[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P10[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P01[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P20[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P11[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P02[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                }

              },
              {//LV05
                 96325, // i4R_AVG
                 25002, // i4R_STD
                117525, // i4B_AVG
                 28993, // i4B_STD
    			  1257, // i4R_MAX
    			   851, // i4R_MIN
    			   703, // i4G_MAX
    			   652, // i4G_MIN
    			  1145, // i4B_MAX
    			   783, // i4B_MIN
                { // i4P00[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P10[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P01[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P20[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P11[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P02[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                }

              },
              {//FLASH_00
                 96325, // i4R_AVG
                 25002, // i4R_STD
                117525, // i4B_AVG
                 28993, // i4B_STD
    			  1257, // i4R_MAX
    			   851, // i4R_MIN
    			   703, // i4G_MAX
    			   652, // i4G_MIN
    			  1145, // i4B_MAX
    			   783, // i4B_MIN
                { // i4P00[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P10[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P01[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P20[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P11[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P02[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                }
              },
              {//FLASH_01
                 96325, // i4R_AVG
                 25002, // i4R_STD
                117525, // i4B_AVG
                 28993, // i4B_STD
    			  1257, // i4R_MAX
    			   851, // i4R_MIN
    			   703, // i4G_MAX
    			   652, // i4G_MIN
    			  1145, // i4B_MAX
    			   783, // i4B_MIN
                { // i4P00[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P10[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P01[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P20[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P11[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                },
                { // i4P02[9]
                    0, 0, 0, 0, 0, 0, 0, 0, 0
                }

              }
          }
    },
    DngMetadata:
    {
        0,  //i4RefereceIlluminant1
        3,  //i4RefereceIlluminant2
        {
            // rNoiseProfile[4]
            {
                {3.530980e-6, 3.357493e-4},
                {8.098436e-8, -2.336529e-4},
            },
            {
                {3.530980e-6, 3.357493e-4},
                {8.098436e-8, -2.336529e-4},
            },
            {
                {3.530980e-6, 3.357493e-4},
                {8.098436e-8, -2.336529e-4},
            },
            {
                {3.530980e-6, 3.357493e-4},
                {8.098436e-8, -2.336529e-4},
            },
        },
    },
    ANR_TBL:
    {
      //CT_00
      {
        //ISO_00
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_01
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_02
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_03
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_04
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_05
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }}
        },
        //CT_01
      {
        //ISO_00
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_01
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_02
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_03
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_04
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }},
            //ISO_05
        {.set={
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04490447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445,
          0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444,
          0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444, 0x04440444, 0x04440444,
          0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04450445, 0x04440444,
          0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446, 0x04440444, 0x04440444,
          0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04450445, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444, 0x044e0449, 0x04440446,
          0x04440444, 0x04440444, 0x04490446, 0x04440445, 0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04450444, 0x04440444, 0x04440444, 0x04440444, 0x044a0447, 0x04440445, 0x04440444, 0x04440444,
          0x044d0448, 0x04440446, 0x04440444, 0x04440444, 0x04480446, 0x04440445, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04490446, 0x04440445,
          0x04440444, 0x04440444, 0x044c0447, 0x04440445, 0x04440444, 0x04440444, 0x04470445, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04440444,
          0x04440444, 0x04440444, 0x04440444, 0x04440444, 0x04430443, 0x04430443, 0x04440443, 0x04430443, 0x04430443, 0x04430443,
          0x04470445, 0x04430444, 0x04430443, 0x04430443, 0x04490446, 0x04430444, 0x04430443, 0x04430443, 0x04460444, 0x04430444,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443,
          0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443, 0x04430443
        }}
        }
    }
}};

const NVRAM_CAMERA_3A_STRUCT CAMERA_3A_NVRAM_DEFAULT_VALUE =
{
    NVRAM_CAMERA_3A_FILE_VERSION, // u4Version
    SENSOR_ID, // SensorId

    // AE NVRAM
    {
        getAENVRAM_gc02m1mipi_raw_cxt_rear<CAM_SCENARIO_PREVIEW>(),
        getAENVRAM_gc02m1mipi_raw_cxt_rear<CAM_SCENARIO_VIDEO>(),
        getAENVRAM_gc02m1mipi_raw_cxt_rear<CAM_SCENARIO_CAPTURE>(),
        getAENVRAM_gc02m1mipi_raw_cxt_rear<CAM_SCENARIO_CUSTOM1>(),
        getAENVRAM_gc02m1mipi_raw_cxt_rear<CAM_SCENARIO_CUSTOM2>(),
        getAENVRAM_gc02m1mipi_raw_cxt_rear<CAM_SCENARIO_CUSTOM3>(),
        getAENVRAM_gc02m1mipi_raw_cxt_rear<CAM_SCENARIO_CUSTOM4>()
    },

    // AWB NVRAM
    AWB_NVRAM_START
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
                    // rUnitGain TL84 (unit gain: 1.0 = 512)
                    {
                            0,    // i4R
                            0,    // i4G
                            0    // i4B
                    },
                    // rGoldenGain TL84 (golden sample gain: 1.0 = 512)
                    {
                            0,    // i4R
                            0,    // i4G
                            0    // i4B
                    },
                     // rUnitGain Alight (unit gain: 1.0 = 512)
                    {
                            0,    // i4R
                            0,    // i4G
                            0    // i4B
                    },
                    // rGoldenGain Alight (golden sample gain: 1.0 = 512)
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
                // rD65Gain
                {
                    855,  // i4R
                    512,  // i4G
                    843  // i4B
                }
            },
            // rOriginalXY
            {
                // rStrobe
                {
                    0,  // i4X
                    0  // i4Y
                },
                // rHorizon
                {
                    -410,  // i4X
                    -328  // i4Y
                },
                // rA
                {
                    -295,  // i4X
                    -359  // i4Y
                },
                // rTL84
                {
                    -183,  // i4X
                    -388  // i4Y
                },
                // rCWF
                {
                    -124,  // i4X
                    -408  // i4Y
                },
                // rDNP
                {
                    -48,  // i4X
                    -399  // i4Y
                },
                // rD65
                {
                    5,  // i4X
                    -374  // i4Y
                },
                // rDF
                {
                    0,  // i4X
                    0  // i4Y
                }
            },
            // rRotatedXY
            {
                // rStrobe
                {
                    0,  // i4X
                    0  // i4Y
                },
                // rHorizon
                {
                    -385,  // i4X
                    -356  // i4Y
                },
                // rA
                {
                    -269,  // i4X
                    -378  // i4Y
                },
                // rTL84
                {
                    -155,  // i4X
                    -399  // i4Y
                },
                // rCWF
                {
                    -95,  // i4X
                    -415  // i4Y
                },
                // rDNP
                {
                    -20,  // i4X
                    -401  // i4Y
                },
                // rD65
                {
                    31,  // i4X
                    -372  // i4Y
                },
                // rDF
                {
                    22,  // i4X
                    -410  // i4Y
                }
            },
            // rLightAWBGain
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
                    573,  // i4G
                    1556  // i4B
                },
                // rA
                {
                    558,  // i4R
                    512,  // i4G
                    1241  // i4B
                },
                // rTL84
                {
                    676,  // i4R
                    512,  // i4G
                    1110  // i4B
                },
                // rCWF
                {
                    753,  // i4R
                    512,  // i4G
                    1052  // i4B
                },
                // rDNP
                {
                    823,  // i4R
                    512,  // i4G
                    938  // i4B
                },
                // rD65
                {
                    855,  // i4R
                    512,  // i4G
                    843  // i4B
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
                -4,  // i4RotationAngle
                255,  // i4Cos
                -18  // i4Sin
            },
            // rDaylightLocus
            {
                -111,  // i4SlopeNumerator
                128  // i4SlopeDenominator
            },
            // rPredictorGain
            {
                101,  // i4PrefRatio100
                // rSpatial_DaylightLocus_L
                {
                    965,  // i4R
                    530,  // i4G
                    780  // i4B
                },
                // rSpatial_DaylightLocus_H
                {
                    802,  // i4R
                    512,  // i4G
                    845  // i4B
                },
                // rTemporal_General
                {
                    955,  // i4R
                    512,  // i4G
                    750  // i4B
                },
                // rAWBGain_LSC
                {
                    786,  // i4R
                    512,  // i4G
                    908  // i4B
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
                    -201,  // i4RightBound
                    -785,  // i4LeftBound
                    -265,  // i4UpperBound
                    -385  // i4LowerBound
                },
                // rWarmFluorescent
                {
                    -201,  // i4RightBound
                    -785,  // i4LeftBound
                    -385,  // i4UpperBound
                    -505  // i4LowerBound
                },
                // rFluorescent
                {
                    -64,  // i4RightBound
                    -201,  // i4LeftBound
                    -302,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rCWF
                {
                    -64,  // i4RightBound
                    -201,  // i4LeftBound
                    -407,  // i4UpperBound
                    -545  // i4LowerBound
                },
                // rDaylight
                {
                    110,  // i4RightBound
                    -64,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rShade
                {
                    391,  // i4RightBound
                    110,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rDaylightFluorescent
                {
                    101,  // i4RightBound
                    -64,  // i4LeftBound
                    -407,  // i4UpperBound
                    -470  // i4LowerBound
                }
            },
            // rPWBLightArea
            {
                // rReferenceArea
                {
                    391,  // i4RightBound
                    -785,  // i4LeftBound
                    0,  // i4UpperBound
                    -505  // i4LowerBound
                },
                // rDaylight
                {
                    86,  // i4RightBound
                    -64,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rCloudyDaylight
                {
                    186,  // i4RightBound
                    11,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rShade
                {
                    286,  // i4RightBound
                    11,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rTwilight
                {
                    -64,  // i4RightBound
                    -224,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rFluorescent
                {
                    81,  // i4RightBound
                    -255,  // i4LeftBound
                    -322,  // i4UpperBound
                    -465  // i4LowerBound
                },
                // rWarmFluorescent
                {
                    -169,  // i4RightBound
                    -369,  // i4LeftBound
                    -322,  // i4UpperBound
                    -465  // i4LowerBound
                },
                // rIncandescent
                {
                    -169,  // i4RightBound
                    -369,  // i4LeftBound
                    -312,  // i4UpperBound
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
                    817,  // i4R
                    512,  // i4G
                    850  // i4B
                },
                // rCloudyDaylight
                {
                    928,  // i4R
                    512,  // i4G
                    761  // i4B
                },
                // rShade
                {
                    997,  // i4R
                    512,  // i4G
                    715  // i4B
                },
                // rTwilight
                {
                    653,  // i4R
                    512,  // i4G
                    1033  // i4B
                },
                // rFluorescent
                {
                    740,  // i4R
                    512,  // i4G
                    1010  // i4B
                },
                // rWarmFluorescent
                {
                    569,  // i4R
                    512,  // i4G
                    1269  // i4B
                },
                // rIncandescent
                {
                    545,  // i4R
                    512,  // i4G
                    1208  // i4B
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
                    40,  // i4SliderValue
                   4800  // i4OffsetThr
                },
                // rWarmFluorescent
                {
                    40,  // i4SliderValue
                    4780  // i4OffsetThr
                },
                // rShade
                {
                    50,  // i4SliderValue
                    410  // i4OffsetThr
                },
                // rSunsetArea
                {
                    -54,  // i4BoundXrThr
                    -401  // i4BoundYrThr
                },
                // rShadeFArea
                {
                    -201,  // i4BoundXrThr
                    -403  // i4BoundYrThr
                },
                // rShadeFVerTex
                {
                    -133,  // i4BoundXrThr
                    -405  // i4BoundYrThr
                },
                // rShadeCWFArea
                {
                    -201,  // i4BoundXrThr
                    -460  // i4BoundYrThr
                },
                // rShadeCWFVerTex
                {
                    -123,  // i4BoundXrThr
                    -445  // i4BoundYrThr
                }
            },
            // rCCTEstimation
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
		                -525,	// i4RotatedXCoordinate[0]
		                -398,	// i4RotatedXCoordinate[1]
		                -285,	// i4RotatedXCoordinate[2]
		                -169,	// i4RotatedXCoordinate[3]
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
                        {480, 512, 480}, {480, 512, 480}, {480, 512, 500}, {480, 512, 485}, {508, 512, 526}, {508, 512, 512}, {508, 512, 513}, {508, 512, 512}, {510, 512, 512}, {515, 512, 495},
           	            {515, 512, 495}, {525, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
        	        }, // TUNGSTEN
        	        {
                        {512, 512, 512}, {512, 512, 520}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {515, 512, 510}, {512, 512, 510}, {520, 512, 512}, {508, 512, 515},
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
        	        }, // WARM F
        	        {
                        {512, 512, 512}, {512, 512, 512}, {525, 512, 512}, {515, 512, 512}, {515, 512, 518}, {515, 512, 516}, {510, 512, 518}, {505, 512, 520}, {508, 512, 520}, {512, 512, 520},
                        {512, 512, 520}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // F
                    {
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {500, 512, 500}, {500, 512, 500}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 490}, {512, 512, 495}, {512, 512, 490}, {512, 512, 470}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // CWF
                    {
                        {512, 512, 512}, {512, 512, 512}, {520, 512, 500}, {505, 512, 502}, {505, 512, 502}, {510, 512, 512}, {510, 512, 520}, {506, 512, 515}, {506, 512, 520}, {512, 512, 520},
                        {512, 512, 518}, {506, 512, 505}, {500, 512, 505}, {508, 512, 500}, {500, 512, 510}, {508, 512, 512}, {510, 512, 505}, {512, 512, 505}, {512, 512, 512}
                    }, // DAYLIGHT
                    {
                        {512, 512, 512}, {512, 512, 512}, {520, 512, 508}, {520, 512, 508}, {512, 512, 530}, {512, 512, 530}, {515, 512, 512}, {514, 512, 518}, {515, 512, 512}, {512, 512, 512},
                        {525, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 500}, {508, 512, 512}, {500, 512, 500}, {490, 512, 480}, {490, 512, 512}, {512, 512, 512}
                    }, // SHADE
                    {
                        {515, 512, 512}, {515, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 506}, {505, 512, 515}, {520, 512, 525}, {512, 512, 520}, {512, 512, 520},
                        {512, 512, 512}, {515, 512, 505}, {500, 512, 490}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
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
	                { 0,   0,   0,   0,   0,   3,   5,   5,    5,   5,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
	                // CWF
	                { 0,   0,   0,   0,   0,   3,   5,   5,    5,   5,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
	                // Daylight
	                { 0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   2,   2,   2,   2,   2,   2,   2,   2},  // (%)
	                // DF
	                { 0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
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
                    1,  // i4Enable
                        50,  // i4LVThr_L
                        90,  // i4LVThr_H
                        128  // i4DaylightProb
                    },

                    // Shade CWF Detection
                    {
                        1,          // i4Enable
                        50,  // i4LVThr_L
                        90,  // i4LVThr_H
                        192  // i4DaylightProb
                    },
                },

                // AWB non-neutral probability for spatial and temporal weighting look-up table (Max: 100; Min: 0)
                {
                    //LV0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    {  90, 90,  90,  90,  90,  90,  90,  90,  90,  90,   65,  50,  30,  20,  10,   0,   0,   0,   0}
                },

                // AWB daylight locus probability look-up table (Max: 100; Min: 0)1
                {
                    //LV0  1    2    3    4    5    6    7    8    9    10    11    12   13   14   15   16   17   18
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  50,  25,   0,  0,   0,   0}, // Strobe
                    {92,  92,   94, 100, 100, 100, 100, 100, 100, 100,  100,   90, 100,  75,  50,  25,  0,   0,   0}, // Tungsten
                    {100, 100, 100, 100, 100, 100,  90,  90, 100, 100,  100,   85,  50,  25,  25,  25,  0,   0,   0}, // Warm fluorescent
                    {100, 100, 100, 100, 100, 100, 100,  95,  95,  95,   85,   85,  85,  70,  70,  25,  0,   0,   0}, // Fluorescent
                    {100, 100, 100, 100,  90,  90, 100,  90, 100, 100,   95,   65,  50,  40,  30,  25,  0,   0,   0}, // CWF
                    { 80,  90, 100, 100, 100,  90,  80, 100, 100,  70,   80,   90,  90,  80,  100,  80,  25, 10,  0}, // Daylight
                    { 80,  80,  80,  80,  50,  50, 100,  50, 100,  80,   80,   75,  75,  90,  50,  50,  70,  20,   0}, // Shade
                    { 80,  80,  90,  90,  90,  90, 100,  90,  80,  80,   90,   70,  70,  75,  30,  30, 0,   0,   0}  // Daylight fluorescent
                },

                // AWB tuning information
                {
                    0,       // project code
                    0,       // model
                    0,   // date
                    0,          // reserved 0
                    0,          // reserved 1
                    0,          // reserved 2
                    0,          // reserved 3
                    0,          // reserved 4
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
                    // rUnitGain TL84 (unit gain: 1.0 = 512)
                    {
                            0,    // i4R
                            0,    // i4G
                            0    // i4B
                    },
                    // rGoldenGain TL84 (golden sample gain: 1.0 = 512)
                    {
                            0,    // i4R
                            0,    // i4G
                            0    // i4B
                    },
                     // rUnitGain Alight (unit gain: 1.0 = 512)
                    {
                            0,    // i4R
                            0,    // i4G
                            0    // i4B
                    },
                    // rGoldenGain Alight (golden sample gain: 1.0 = 512)
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
                // rD65Gain
                {
                    855,  // i4R
                    512,  // i4G
                    843  // i4B
                }
            },
            // rOriginalXY
            {
                // rStrobe
                {
                    0,  // i4X
                    0  // i4Y
                },
                // rHorizon
                {
                    -410,  // i4X
                    -328  // i4Y
                },
                // rA
                {
                    -295,  // i4X
                    -359  // i4Y
                },
                // rTL84
                {
                    -183,  // i4X
                    -388  // i4Y
                },
                // rCWF
                {
                    -124,  // i4X
                    -408  // i4Y
                },
                // rDNP
                {
                    -48,  // i4X
                    -399  // i4Y
                },
                // rD65
                {
                    5,  // i4X
                    -374  // i4Y
                },
                // rDF
                {
                    0,  // i4X
                    0  // i4Y
                }
            },
            // rRotatedXY
            {
                // rStrobe
                {
                    0,  // i4X
                    0  // i4Y
                },
                // rHorizon
                {
                    -385,  // i4X
                    -356  // i4Y
                },
                // rA
                {
                    -269,  // i4X
                    -378  // i4Y
                },
                // rTL84
                {
                    -155,  // i4X
                    -399  // i4Y
                },
                // rCWF
                {
                    -95,  // i4X
                    -415  // i4Y
                },
                // rDNP
                {
                    -20,  // i4X
                    -401  // i4Y
                },
                // rD65
                {
                    31,  // i4X
                    -372  // i4Y
                },
                // rDF
                {
                    22,  // i4X
                    -410  // i4Y
                }
            },
            // rLightAWBGain
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
                    573,  // i4G
                    1556  // i4B
                },
                // rA
                {
                    558,  // i4R
                    512,  // i4G
                    1241  // i4B
                },
                // rTL84
                {
                    676,  // i4R
                    512,  // i4G
                    1110  // i4B
                },
                // rCWF
                {
                    753,  // i4R
                    512,  // i4G
                    1052  // i4B
                },
                // rDNP
                {
                    823,  // i4R
                    512,  // i4G
                    938  // i4B
                },
                // rD65
                {
                    855,  // i4R
                    512,  // i4G
                    843  // i4B
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
                -4,  // i4RotationAngle
                255,  // i4Cos
                -18  // i4Sin
            },
            // rDaylightLocus
            {
                -111,  // i4SlopeNumerator
                128  // i4SlopeDenominator
            },
            // rPredictorGain
            {
                101,  // i4PrefRatio100
                // rSpatial_DaylightLocus_L
                {
                    965,  // i4R
                    530,  // i4G
                    780  // i4B
                },
                // rSpatial_DaylightLocus_H
                {
                    802,  // i4R
                    512,  // i4G
                    845  // i4B
                },
                // rTemporal_General
                {
                    955,  // i4R
                    512,  // i4G
                    750  // i4B
                },
                // rAWBGain_LSC
                {
                    786,  // i4R
                    512,  // i4G
                    908  // i4B
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
                    -201,  // i4RightBound
                    -785,  // i4LeftBound
                    -265,  // i4UpperBound
                    -385  // i4LowerBound
                },
                // rWarmFluorescent
                {
                    -201,  // i4RightBound
                    -785,  // i4LeftBound
                    -385,  // i4UpperBound
                    -505  // i4LowerBound
                },
                // rFluorescent
                {
                    -64,  // i4RightBound
                    -201,  // i4LeftBound
                    -302,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rCWF
                {
                    -64,  // i4RightBound
                    -201,  // i4LeftBound
                    -407,  // i4UpperBound
                    -545  // i4LowerBound
                },
                // rDaylight
                {
                    110,  // i4RightBound
                    -64,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rShade
                {
                    391,  // i4RightBound
                    110,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rDaylightFluorescent
                {
                    101,  // i4RightBound
                    -64,  // i4LeftBound
                    -407,  // i4UpperBound
                    -470  // i4LowerBound
                }
            },
            // rPWBLightArea
            {
                // rReferenceArea
                {
                    391,  // i4RightBound
                    -785,  // i4LeftBound
                    0,  // i4UpperBound
                    -505  // i4LowerBound
                },
                // rDaylight
                {
                    86,  // i4RightBound
                    -64,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rCloudyDaylight
                {
                    186,  // i4RightBound
                    11,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rShade
                {
                    286,  // i4RightBound
                    11,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rTwilight
                {
                    -64,  // i4RightBound
                    -224,  // i4LeftBound
                    -312,  // i4UpperBound
                    -407  // i4LowerBound
                },
                // rFluorescent
                {
                    81,  // i4RightBound
                    -255,  // i4LeftBound
                    -322,  // i4UpperBound
                    -465  // i4LowerBound
                },
                // rWarmFluorescent
                {
                    -169,  // i4RightBound
                    -369,  // i4LeftBound
                    -322,  // i4UpperBound
                    -465  // i4LowerBound
                },
                // rIncandescent
                {
                    -169,  // i4RightBound
                    -369,  // i4LeftBound
                    -312,  // i4UpperBound
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
                    817,  // i4R
                    512,  // i4G
                    850  // i4B
                },
                // rCloudyDaylight
                {
                    928,  // i4R
                    512,  // i4G
                    761  // i4B
                },
                // rShade
                {
                    997,  // i4R
                    512,  // i4G
                    715  // i4B
                },
                // rTwilight
                {
                    653,  // i4R
                    512,  // i4G
                    1033  // i4B
                },
                // rFluorescent
                {
                    740,  // i4R
                    512,  // i4G
                    1010  // i4B
                },
                // rWarmFluorescent
                {
                    569,  // i4R
                    512,  // i4G
                    1269  // i4B
                },
                // rIncandescent
                {
                    545,  // i4R
                    512,  // i4G
                    1208  // i4B
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
                    40,  // i4SliderValue
                    4800  // i4OffsetThr
                },
                // rWarmFluorescent
                {
                    40,  // i4SliderValue
                    4780  // i4OffsetThr
                },
                // rShade
                {
                    50,  // i4SliderValue
                    410  // i4OffsetThr
                },
                // rSunsetArea
                {
                    -54,  // i4BoundXrThr
                    -401  // i4BoundYrThr
                },
                // rShadeFArea
                {
                    -201,  // i4BoundXrThr
                    -403  // i4BoundYrThr
                },
                // rShadeFVerTex
                {
                    -133,  // i4BoundXrThr
                    -405  // i4BoundYrThr
                },
                // rShadeCWFArea
                {
                    -201,  // i4BoundXrThr
                    -460  // i4BoundYrThr
                },
                // rShadeCWFVerTex
                {
                    -123,  // i4BoundXrThr
                    -445  // i4BoundYrThr
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
		                -525,	// i4RotatedXCoordinate[0]
		                -398,	// i4RotatedXCoordinate[1]
		                -285,	// i4RotatedXCoordinate[2]
		                -169,	// i4RotatedXCoordinate[3]
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
                        {480, 512, 480}, {480, 512, 480}, {480, 512, 500}, {480, 512, 485}, {508, 512, 526}, {508, 512, 512}, {508, 512, 513}, {508, 512, 512}, {510, 512, 512}, {515, 512, 495},
           	            {515, 512, 495}, {525, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
        	        }, // TUNGSTEN
        	        {
                        {512, 512, 512}, {512, 512, 520}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {515, 512, 510}, {512, 512, 510}, {520, 512, 512}, {508, 512, 515},
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
        	        }, // WARM F
        	        {
                        {512, 512, 512}, {512, 512, 512}, {525, 512, 512}, {515, 512, 512}, {515, 512, 518}, {515, 512, 516}, {510, 512, 518}, {505, 512, 520}, {508, 512, 520}, {512, 512, 520},
                        {512, 512, 520}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // F
                    {
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {502, 512, 502}, {500, 512, 500}, {510, 512, 510}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512},
                        {512, 512, 512}, {512, 512, 512}, {512, 512, 490}, {512, 512, 495}, {512, 512, 490}, {512, 512, 470}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                    }, // CWF
                    {
                        {512, 512, 512}, {512, 512, 512}, {520, 512, 500}, {505, 512, 502}, {505, 512, 502}, {510, 512, 512}, {510, 512, 520}, {506, 512, 515}, {506, 512, 520}, {512, 512, 520},
                        {512, 512, 518}, {506, 512, 505}, {500, 512, 505}, {508, 512, 500}, {500, 512, 510}, {508, 512, 512}, {510, 512, 505}, {512, 512, 505}, {512, 512, 512}
                    }, // DAYLIGHT
                    {
                        {512, 512, 512}, {512, 512, 512}, {520, 512, 508}, {520, 512, 508}, {512, 512, 530}, {512, 512, 530}, {515, 512, 512}, {514, 512, 518}, {515, 512, 512}, {512, 512, 512},
                        {525, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 500}, {508, 512, 512}, {500, 512, 500}, {490, 512, 480}, {490, 512, 512}, {512, 512, 512}
                    }, // SHADE
                    {
                        {515, 512, 512}, {515, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 506}, {505, 512, 515}, {520, 512, 525}, {512, 512, 520}, {512, 512, 520},
                        {512, 512, 512}, {515, 512, 505}, {500, 512, 490}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
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
	                { 0,   0,   0,   0,   0,   3,   5,   5,    5,   5,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
	                // CWF
	                { 0,   0,   0,   0,   0,   3,   5,   5,    5,   5,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
	                // Daylight
	                { 0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   2,   2,   2,   2,   2,   2,   2,   2},  // (%)
	                // DF
	                { 0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
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
                    50,  // i4LVThr_L
                    90,  // i4LVThr_H
                    128  // i4DaylightProb
                    },

                    // Shade CWF Detection
                    {
                        1,          // i4Enable
                    50,  // i4LVThr_L
                    90,  // i4LVThr_H
                    192  // i4DaylightProb
                }
                },

                // AWB non-neutral probability for spatial and temporal weighting look-up table (Max: 100; Min: 0)
                {
                    //LV0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    {  90, 90,  90,  90,  90,  90,  90,  90,  90,  90,   65,  50,  30,  20,  10,   0,   0,   0,   0}
                },

                // AWB daylight locus probability look-up table (Max: 100; Min: 0)
               {
                    //LV0  1    2    3    4    5    6    7    8    9    10    11    12   13   14   15   16   17   18
                    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100, 100,  50,  25,   0,  0,   0,   0}, // Strobe
                    {92,  92,   94, 100, 100, 100, 100, 100, 100, 100,  100,   90, 100,  75,  50,  25,  0,   0,   0}, // Tungsten
                    {100, 100, 100, 100, 100, 100,  90,  90, 100, 100,  100,   85,  50,  25,  25,  25,  0,   0,   0}, // Warm fluorescent
                    {100, 100, 100, 100, 100, 100, 100,  95,  95,  95,   85,   85,  85,  70,  70,  25,  0,   0,   0}, // Fluorescent
                    {100, 100, 100, 100,  90,  90, 100,  90, 100, 100,   95,   65,  50,  40,  30,  25,  0,   0,   0}, // CWF
                    { 80,  90, 100, 100, 100,  90,  80, 100, 100,  70,   80,   90,  90,  80,  100,  80,  25,  10,  0}, // Daylight
                    { 80,  80,  80,  80,  50,  50, 100,  50, 100,  80,   80,   75,  75,  90,  50,  50,  70,  20,   0}, // Shade
                    { 80,  80,  90,  90,  90,  90, 100,  90,  80,  80,   90,   70,  70,  75,  30,  30, 0,   0,   0}  // Daylight fluorescent
                },

                // AWB tuning information
                {
                    6739,       // project code
                    5588,       // model
                    20150624,   // date
                    0,          // reserved 0
                    1,          // reserved 1
                    2,          // reserved 2
                    3,          // reserved 3
                    4,          // reserved 4
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
        1,  // isTsfEn
        2,  // tsfCtIdx
        {20, 2000, -110, -110, 512, 512, 512, 0}    // rAWBInput[8]
    },

#include INCLUDE_FILENAME_TSF_PARA
#include INCLUDE_FILENAME_TSF_DATA
};

const NVRAM_CAMERA_FEATURE_STRUCT CAMERA_FEATURE_DEFAULT_VALUE =
{
#include INCLUDE_FILENAME_FEATURE_PARA
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
        sizeof(CAMERA_TSF_TBL_STRUCT),
        0,
        0,
        sizeof(NVRAM_CAMERA_FEATURE_STRUCT)
    };

    if (CameraDataType > CAMERA_NVRAM_DATA_FEATURE || NULL == pDataBuf || (size != dataSize[CameraDataType]))
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
      case CAMERA_NVRAM_DATA_FEATURE:
        memcpy(pDataBuf,&CAMERA_FEATURE_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_FEATURE_STRUCT));
        break;
      default:
        break;
    }
    return 0;
  }};  //  NSFeature

