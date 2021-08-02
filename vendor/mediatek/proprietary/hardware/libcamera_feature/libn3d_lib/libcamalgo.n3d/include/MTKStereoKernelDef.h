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
#ifndef __MTK_STEREO_KERNEL_DEF_H__
#define __MTK_STEREO_KERNEL_DEF_H__

#include "MTKUtilType.h"
#include "MTKStereoKernelErrCode.h"

#define MAX_GEO_LEVEL   (3)
#define MAX_GRID_WD     (640)
#define MAX_GRID_HT     (480)
#define MAX_REF_PT_NUM  (192)

#define __RELAX_LEARNING_CRITERIA__
#define __R_T__   //(RX+T)

#define FLOAT_ERR_TOL   (1e-5)

#ifndef __STEREO_KERNEL_DEF_INPUT_FORMAT_ENUM__
#define __STEREO_KERNEL_DEF_INPUT_FORMAT_ENUM__
typedef enum
{
    STEREO_KERNEL_INPUT_FORMAT_YV12 = 0,
    STEREO_KERNEL_INPUT_FORMAT_RGBA,
    STEREO_KERNEL_INPUT_FORMAT_RGB,
    STEREO_KERNEL_INPUT_FORMAT_YUYV,
    STEREO_KERNEL_INPUT_FORMAT_Y,
    STEREO_KERNEL_INPUT_FORMAT_YUV422,
}
STEREO_KERNEL_INPUT_FORMAT_ENUM ;
#endif

typedef enum
{
    STEREO_KERNEL_SCENARIO_IMAGE_PREVIEW,
    STEREO_KERNEL_SCENARIO_IMAGE_CAPTURE,
    STEREO_KERNEL_SCENARIO_VIDEO_RECORD,

    STEREO_KERNEL_SCENARIO_IMAGE_PLAYBACK,
    STEREO_KERNEL_SCENARIO_VIDEO_PLAYBACK,
}
STEREO_KERNEL_SCENARIO_ENUM ;

typedef enum
{
    STEREO_KERNEL_OUTPUT_FEFM = 0,
    STEREO_KERNEL_OUTPUT_RECT,
    STEREO_KERNEL_OUTPUT_RECT_CAP,
    STEREO_KERNEL_OUTPUT_DEPTH,
    STEREO_KERNEL_OUTPUT_3A,
    STEREO_KERNEL_OUTPUT_SCENE_INFO,
}
STEREO_KERNEL_OUTPUT_ENUM ;

typedef struct
{
	void* mGraphicBuffer;
	void* mEGLImage;
}
STEREO_KERNEL_GRAPHIC_BUFFER_STRUCT ; //For GPU graphic buffer

typedef struct
{
    MUINT32 pixel_array_width ;
    MUINT32 pixel_array_height ;
    MUINT32 sensor_offset_x0 ;
    MUINT32 sensor_offset_y0 ;
    MUINT32 sensor_size_w0 ;
    MUINT32 sensor_size_h0 ;
    MUINT32 sensor_scale_w ;
    MUINT32 sensor_scale_h ;
    MUINT32 sensor_offset_x1 ;
    MUINT32 sensor_offset_y1 ;
    MUINT32 sensor_size_w1 ;
    MUINT32 sensor_size_h1 ;
    MUINT32 tg_offset_x ;
    MUINT32 tg_offset_y ;
    MUINT32 tg_size_w ;
    MUINT32 tg_size_h ;
    MUINT32 rrz_offset_x ;
    MUINT32 rrz_offset_y ;
    MUINT32 rrz_usage_width ;
    MUINT32 rrz_usage_height ;
    MUINT32 rrz_out_width ;
    MUINT32 rrz_out_height ;
    MUINT32 mdp_rotate ;
    MUINT32 mdp_offset_x ;
    MUINT32 mdp_offset_y ;
    MUINT32 mdp_usage_width ;
    MUINT32 mdp_usage_height ;
    //MUINT32 mdp_out_width ; // algo in width
    //MUINT32 mdp_out_height ; // alog in height
}
STEREO_KERNEL_FLOW_INFO_STRUCT ;

typedef struct
{
    MUINT32 width ;
    MUINT32 height ;
    MUINT32 depth ;
    MUINT32 stride ;
    MUINT32 format ;

    MUINT32 act_width ;
    MUINT32 act_height ;

    MUINT32 offset_x ;
    MUINT32 offset_y ;
}
STEREO_KERNEL_IMG_INFO_STRUCT ;

typedef struct
{
    MUINT32 inp_w ;
    MUINT32 inp_h ;
    MUINT32 out_w ;
    MUINT32 out_h ;
    MUINT32 src_w ;
    MUINT32 src_h ;
    MUINT32 wpe_w ;
    MUINT32 wpe_h ;
}
STEREO_KERNEL_IMG_IO_STRUCT ;

typedef struct
{
    MUINT32 dac_i ;
    MUINT32 dac_v ;
    MFLOAT  dac_c ;
    MUINT32 dac_w[4] ;
}
STEREO_KERNEL_AF_INFO_STRUCT ; // RUN-TIME

typedef struct
{
    MUINT32 dac_mcr ;
    MUINT32 dac_inf ;
    MUINT32 dac_str ;
    MUINT32 dist_mcr ;  //mm
    MUINT32 dist_inf ;  //mm
}
STEREO_KERNEL_AF_INIT_STRUCT ;

typedef struct
{
    MUINT32 p_tune ;
    MUINT32 l_tune ;	// Remove s_tune and and l_tune. 
						// [0~7] thr for drop aware mechanism, default : 7
						// [8~15] scale compensation var tolerance, default : 7
						// [16~23] scale compensation shift tolerance, default : 3
}
STEREO_KERNEL_TUNING_PARA_STRUCT ;

typedef struct
{
    STEREO_KERNEL_IMG_INFO_STRUCT img_main ;
    STEREO_KERNEL_IMG_INFO_STRUCT img_auxi ;
    MUINT32 size ;
}
STEREO_KERNEL_GEO_INFO_STRUCT ;

typedef struct STEREO_KERNEL_FIXATION_POINT_STURCT
{
    MFLOAT main[MAX_REF_PT_NUM][2];
    MFLOAT auxi[MAX_REF_PT_NUM][2];
    MUINT32 dist = 0; //mm
    MUINT32 dac = 0;
    MUINT32 npt = 0;
}
STEREO_KERNEL_FIXATION_POINT_STRUCT ;

typedef struct
{
    STEREO_KERNEL_SCENARIO_ENUM scenario ; 

    STEREO_KERNEL_IMG_IO_STRUCT iio_main ;
    STEREO_KERNEL_IMG_IO_STRUCT iio_auxi ;

    MUINT32 geo_info ;
    STEREO_KERNEL_GEO_INFO_STRUCT geo_img[MAX_GEO_LEVEL] ;
    STEREO_KERNEL_IMG_INFO_STRUCT pho_img ;
    //STEREO_KERNEL_IMG_INFO_STRUCT pho_img_auxi ;

    MFLOAT fov_main[2] ;
    MFLOAT fov_auxi[2] ;
    MFLOAT baseline ;

    MUINT32 system_cfg ;

    STEREO_KERNEL_AF_INIT_STRUCT af_init_main ;
    STEREO_KERNEL_AF_INIT_STRUCT af_init_auxi ;

    STEREO_KERNEL_FLOW_INFO_STRUCT flow_main ;
    STEREO_KERNEL_FLOW_INFO_STRUCT flow_auxi ;

    MUINT32 working_buffer_size ;
    //STEREO_KERNEL_TUNING_PARA_STRUCT tuning_para;

    STEREO_KERNEL_FIXATION_POINT_STRUCT fix_pt ;
    
    MINT32 dpe_convergence_offset;	// Additional convergence offset requested by Depth engine
}
STEREO_KERNEL_SET_ENV_INFO_STRUCT ;

typedef struct
{
    MUINT32 ext_mem_size;
    MUINT8* ext_mem_start_addr; //working buffer start address
}
STEREO_KERNEL_SET_WORK_BUF_INFO_STRUCT ;

typedef enum STEREO_KERNEL_EGLIMAGE_COLOR_SPACE
{
    STEREO_KERNEL_EGLIMAGE_COLOR_SPACE_YUV_NOT_SET=1,/* set to BT601_REDUCE as default */
    STEREO_KERNEL_EGLIMAGE_COLOR_SPACE_YUV_BT601_NARROW,
    STEREO_KERNEL_EGLIMAGE_COLOR_SPACE_YUV_BT601_FULL,
    STEREO_KERNEL_EGLIMAGE_COLOR_SPACE_YUV_BT709_NARROW
}
STEREO_KERNEL_EGLIMAGE_COLOR_SPACE;

typedef struct
{
    MUINT8* addr_ms ;
    MUINT8* addr_md ;
    MUINT8* addr_mm ;

    MUINT8* addr_as ;
    MUINT8* addr_ad ;
    MUINT8* addr_am ;

    MUINT8* addr_mp ;
    MUINT8* addr_ap ;

    MINT32* addr_mw[3] ;
    MINT32* addr_aw[3] ;

    STEREO_KERNEL_GRAPHIC_BUFFER_STRUCT src_gb;
    STEREO_KERNEL_GRAPHIC_BUFFER_STRUCT dst_gb;

    MUINT16 *addr_me[MAX_GEO_LEVEL] ;
    MUINT16 *addr_ae[MAX_GEO_LEVEL] ;
    MUINT16 *addr_rl[MAX_GEO_LEVEL] ;
    MUINT16 *addr_lr[MAX_GEO_LEVEL] ;

    STEREO_KERNEL_AF_INFO_STRUCT af_main ;
    STEREO_KERNEL_AF_INFO_STRUCT af_auxi ;
    MINT32 eis[4] ;
    MINT32 accInfo[3];	// g sensor information used by AF

    //STEREO_KERNEL_IMG_IO_STRUCT iio_cap ; // B+M capture size

    MUINT32 runtime_cfg ;

    MINT32 buffer_number ;  //register GPU buffer number

    void* InputGB;          //buffer queue or single buffer
    void* OutputGB;
    void* OutputGB_Mask;
    void* OutputGB_AuxiWarpMapX;
    void* OutputGB_AuxiWarpMapY;
    void* OutputGB_MainWarpMapX;
    void* OutputGB_MainWarpMapY;
    bool  GPU_FMT_IS_YUV;   //1:yuv data, 0:rgb data
    bool  GPU_BUF_IS_SRC;   //1:source image, 0:dst image
    STEREO_KERNEL_EGLIMAGE_COLOR_SPACE source_color_domain;
    bool  SPLIT_MASK;       //1: GPU output mask in outputGB_Mask, 0:GPU output mask in OutputGB
}
STEREO_KERNEL_SET_PROC_INFO_STRUCT ;

typedef struct
{
    MUINT32 out_n[7] ;
    void*   out_p[7] ;

    void*  out_debug ; 
}
STEREO_KERNEL_RESULT_STRUCT ;

typedef struct
{
	// debug control settings for simulation
    MUINT32 opt_dac_given;      // input
    MUINT32 opt_dac_index;      // fixed dac
    MFLOAT opt_actShift[2];     // additional shift compensation
}
STEREO_KERNEL_SET_DEBUG_CONTROL_STRUCT;

#endif
