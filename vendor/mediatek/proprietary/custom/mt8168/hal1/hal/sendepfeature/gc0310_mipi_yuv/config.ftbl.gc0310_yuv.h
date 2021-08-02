/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef _MTK_CUSTOM_PROJECT_HAL_IMGSENSOR_GC0310_YUV_CONFIGFTBL__H_
#define _MTK_CUSTOM_PROJECT_HAL_IMGSENSOR_GC0310_YUV_CONFIGFTBL__H_
//#if 1
//

/*******************************************************************************
 *
 ******************************************************************************/
FTABLE_DEFINITION(SENSOR_DRVNAME_GC0310_MIPI_YUV)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FTABLE_SCENE_INDEP()
    //==========================================================================
#if 1
    //  Scene Mode
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_SCENE_MODE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::SCENE_MODE_AUTO), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::SCENE_MODE_AUTO,           
                MtkCameraParameters::SCENE_MODE_NIGHT,          
                //MtkCameraParameters::SCENE_MODE_HDR,
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Effect
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_EFFECT), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::EFFECT_NONE), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::EFFECT_NONE,       
                MtkCameraParameters::EFFECT_MONO,   
                MtkCameraParameters::EFFECT_NEGATIVE,
                MtkCameraParameters::EFFECT_SEPIA,      
             //   MtkCameraParameters::EFFECT_SEPIA_BLUE, 
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Picture Size
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_PICTURE_SIZE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_("640x480"), 
            ITEM_AS_VALUES_(
				"176x144",   "320x240",   "400x240",  "640x384", "640x480",

            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Preview Size
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_PREVIEW_SIZE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_("640x480"), 
            ITEM_AS_VALUES_(
                "176x144",      "320x240",      "352x288",    "560x336",  "480x320",    "480x368", 
                "640x384",      "640x480",
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Video Size
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_VIDEO_SIZE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_("640x480"), 
            ITEM_AS_VALUES_(
                "176x144",      "320x240",		"352x288", "480x320",      "640x480", 
								"720x480",		"864x480",      "1280x720",     "1920x1080", "1920x1088", 
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Preview Frame Rate Range
    FTABLE_CONFIG_AS_TYPE_OF_USER(
        KEY_AS_(MtkCameraParameters::KEY_PREVIEW_FPS_RANGE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_("5000,30000"), 
            ITEM_AS_USER_LIST_(
                "(5000,30000)",
#if 0
                "(60000,60000)",
                "(120000,120000)",
#endif
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Exposure Compensation
    FTABLE_CONFIG_AS_TYPE_OF_USER(
        KEY_AS_(MtkCameraParameters::KEY_EXPOSURE_COMPENSATION), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_("0"), 
            ITEM_AS_USER_LIST_(
                "-1",       //min exposure compensation index
                "1",        //max exposure compensation index
                "1.0",      //exposure compensation step; EV = step x index
            )
        ), 
        //......................................................................
        #if 1   //  SCENE HDR
        SCENE_AS_(MtkCameraParameters::SCENE_MODE_HDR, 
            ITEM_AS_DEFAULT_("0"), 
            ITEM_AS_USER_LIST_(
                "0",        //min exposure compensation index
                "0",        //max exposure compensation index
                "1.0",      //exposure compensation step; EV = step x index
            )
        )
        #endif
    )
#endif
    //==========================================================================
#if 1
    //  Anti-banding (Flicker)
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_ANTIBANDING), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::ANTIBANDING_OFF), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::ANTIBANDING_OFF, 
                MtkCameraParameters::ANTIBANDING_50HZ, 
                MtkCameraParameters::ANTIBANDING_60HZ, 
                MtkCameraParameters::ANTIBANDING_AUTO, 
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Video Snapshot
#if (1 == VSS_SUPPORTED)
    FTABLE_CONFIG_AS_TYPE_OF_USER(
        KEY_AS_(MtkCameraParameters::KEY_VIDEO_SNAPSHOT_SUPPORTED), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::TRUE), 
        ), 
    )
#else
    FTABLE_CONFIG_AS_TYPE_OF_USER(
        KEY_AS_(MtkCameraParameters::KEY_VIDEO_SNAPSHOT_SUPPORTED), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::FALSE), 
        ), 
    )
#endif
#endif

    //==========================================================================
#if 1
    //  Video Stabilization (EIS)
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_SUPPORTED(
        KEY_AS_(MtkCameraParameters::KEY_VIDEO_STABILIZATION), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::FALSE), 
            ITEM_AS_SUPPORTED_(
            #if 0
                MtkCameraParameters::FALSE
            #else
                MtkCameraParameters::TRUE
            #endif
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Zoom
    FTABLE_CONFIG_AS_TYPE_OF_USER(
        KEY_AS_(MtkCameraParameters::KEY_ZOOM), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_("0"),  //Zoom Index
            ITEM_AS_USER_LIST_(
                //Zoom Ratio
                "100", "114", "132", "151", "174", 
                "200", "229", "263", "303", "348", 
                "400", 
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  Zsd
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_ZSD_MODE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::OFF), 
            ITEM_AS_VALUES_(
                #if ZSD_MODE_SUPPORT
                MtkCameraParameters::ON,
                #endif
                MtkCameraParameters::OFF, 
            )
        ), 
    )
#endif
    //==========================================================================
#if 1
    //  (Shot) Capture Mode
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_CAPTURE_MODE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::CAPTURE_MODE_NORMAL), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::CAPTURE_MODE_NORMAL, 
                MtkCameraParameters::CAPTURE_MODE_FACE_BEAUTY, 
                //MtkCameraParameters::CAPTURE_MODE_CONTINUOUS_SHOT, 
                //MtkCameraParameters::CAPTURE_MODE_AUTO_PANORAMA_SHOT,                
            )
        ), 
    )
#endif
    //==========================================================================
END_FTABLE_SCENE_INDEP()
//------------------------------------------------------------------------------
/*******************************************************************************
 *
 ******************************************************************************/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FTABLE_SCENE_DEP()
    //==========================================================================
#if 1
    //  Focus Mode
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_FOCUS_MODE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::FOCUS_MODE_FIXED), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::FOCUS_MODE_FIXED,   
            )
        ), 
        //......................................................................
    )
#endif
    //==========================================================================
#if 1
    //  ISO
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_ISO_SPEED), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_("auto"), 
            ITEM_AS_VALUES_(
                "auto", 
            )
        ), 
        //......................................................................
    #if 1	//	SCENE AUTO
    	SCENE_AS_(MtkCameraParameters::SCENE_MODE_AUTO, 
    		ITEM_AS_DEFAULT_("auto"), 
    		ITEM_AS_VALUES_(
    			"auto", "100", "200", "400", 
    		)
    	)
    #endif
    	//......................................................................
    #if 1	//	SCENE NORMAL
    	SCENE_AS_(MtkCameraParameters::SCENE_MODE_NORMAL, 
    		ITEM_AS_DEFAULT_("auto"), 
    		ITEM_AS_VALUES_(
    			"auto", "100", "200", "400", 
    		)
    	)
    #endif
    )
#endif
    //==========================================================================
#if 1
    //  White Balance.
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_WHITE_BALANCE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::WHITE_BALANCE_AUTO), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::WHITE_BALANCE_AUTO,            MtkCameraParameters::WHITE_BALANCE_INCANDESCENT, 
                MtkCameraParameters::WHITE_BALANCE_DAYLIGHT,        MtkCameraParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT, 
                MtkCameraParameters::WHITE_BALANCE_FLUORESCENT,     MtkCameraParameters::WHITE_BALANCE_TUNGSTEN, 
            )
        ), 
        //......................................................................
    )
#endif
    //==========================================================================
#if 1
    //  ISP Edge
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_EDGE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::MIDDLE), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::LOW, MtkCameraParameters::MIDDLE, MtkCameraParameters::HIGH, 
            )
        ), 
        //......................................................................
        //......................................................................
    )
#endif
    //==========================================================================
#if 0
    //  ISP Hue
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_HUE), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::MIDDLE), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::LOW, MtkCameraParameters::MIDDLE, MtkCameraParameters::HIGH, 
            )
        ), 
        //......................................................................
        //......................................................................
    )
#endif
    //==========================================================================
#if 1
    //  ISP Saturation
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_SATURATION), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::MIDDLE), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::LOW, MtkCameraParameters::MIDDLE, MtkCameraParameters::HIGH, 
            )
        ), 
        //......................................................................
        //......................................................................
    )
#endif
    //==========================================================================
#if 1
    //  ISP Brightness
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_BRIGHTNESS), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::MIDDLE), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::LOW, MtkCameraParameters::MIDDLE, MtkCameraParameters::HIGH, 
            )
        ), 
        //......................................................................
        //......................................................................
    )
#endif
    //==========================================================================
#if 1
    //  ISP Contrast
    FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_VALUES(
        KEY_AS_(MtkCameraParameters::KEY_CONTRAST), 
        SCENE_AS_DEFAULT_SCENE(
            ITEM_AS_DEFAULT_(MtkCameraParameters::MIDDLE), 
            ITEM_AS_VALUES_(
                MtkCameraParameters::LOW, MtkCameraParameters::MIDDLE, MtkCameraParameters::HIGH, 
            )
        ), 
        //......................................................................
        //......................................................................
    )
#endif
    //==========================================================================
END_FTABLE_SCENE_DEP()
//------------------------------------------------------------------------------
END_FTABLE_DEFINITION()


/*******************************************************************************
 *
 ******************************************************************************/
#endif
//#endif //_MTK_CUSTOM_PROJECT_HAL_IMGSENSOR_SRC_CONFIGFTBL__H_

