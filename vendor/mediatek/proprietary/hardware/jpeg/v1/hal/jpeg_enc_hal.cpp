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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#define MTK_LOG_ENABLE 1
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <string.h>

#include "jpeg_enc_hal.h"
#include "jpeg_drv.h"

#define JPEG_DRV_NAME "/dev/mtk_jpeg"

#define xlog(...) \
        do { \
            ALOGW(__VA_ARGS__); \
        } while (0)

#undef LOG_TAG
#define LOG_TAG "JpgEncHal"

//#ifdef MTK_M4U_SUPPORT
//#if 1
//#define USE_M4U
//#endif

      
 
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

// Use the ID to lock encoder driver
//static int drvLockID;
static int currentID;


#ifdef USE_M4U

#include "m4u_lib.h"

static MTKM4UDrv *pM4uDrv = NULL;
static unsigned int dst_va, dst_size, dst_mva;

#endif

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
JPEG_ENC_STATUS_ENUM jpegEncLockEncoder(int* drvID)
{
    int tempID;
    
    *drvID = open(JPEG_DRV_NAME, O_RDONLY, 0);
    if( *drvID == -1)
    {
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;
    }
    
    if(ioctl(*drvID, JPEG_ENC_IOCTL_INIT))
    {
        close(*drvID);
        *drvID = -1;
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;
    }
    
    currentID = *drvID;
    
    return JPEG_ENC_STATUS_OK;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
JPEG_ENC_STATUS_ENUM jpegEncUnlockEncoder(int drvID)
{
#ifdef USE_M4U
    if (pM4uDrv != NULL)
    {
        pM4uDrv->m4u_invalid_tlb_range(M4U_CLNTMOD_JPEG_ENC, dst_mva, dst_mva + dst_size - 1);
        pM4uDrv->m4u_dealloc_mva(M4U_CLNTMOD_JPEG_ENC, dst_va, dst_size, dst_mva);
        delete pM4uDrv;
        pM4uDrv = NULL;
    }
#endif
    if(ioctl(drvID, JPEG_ENC_IOCTL_DEINIT))
    {
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;
    }
    close(drvID);
    
    return JPEG_ENC_STATUS_OK;
}

//unsigned int jpegSetAddr(unsigned int srcAddr, unsigned int *hwAddr, unsigned int align, )
//{ 
//  unsigned int ret ;    
//   
//   
//  
//}
 


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
JPEG_ENC_STATUS_ENUM jpegEncConfigEncoder(int drvID, JPEG_ENC_HAL_IN inParams)
{
    JPEG_ENC_DRV_IN drvIn;
    unsigned int Is420 ; 
    unsigned int padding_width  ;
    unsigned int padding_height ;
    unsigned int m4u_flag = 0;
    unsigned int width_even ; 

    if(drvID != currentID)
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;
        
    memset(&drvIn,0,sizeof(JPEG_ENC_DRV_IN));

#ifdef USE_M4U
    pM4uDrv = new MTKM4UDrv();
    m4u_flag = 1;
    
    dst_va = (unsigned int)inParams.dstBufferAddr;
    dst_size = inParams.dstBufferSize;
      
    if(M4U_STATUS_OK != pM4uDrv->m4u_reset_mva_release_tlb(M4U_CLNTMOD_JPEG_ENC))
    {
        xlog("Can't reset mva\n");
        delete pM4uDrv;
        pM4uDrv = NULL;
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;          
    }
    
    if(M4U_STATUS_OK != pM4uDrv->m4u_alloc_mva(M4U_CLNTMOD_JPEG_ENC, dst_va, dst_size, &dst_mva))
    {
        xlog("Can not allocate mva\n");
        delete pM4uDrv;
        pM4uDrv = NULL;
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;        
    }
  
    if(M4U_STATUS_OK != pM4uDrv->m4u_insert_tlb_range(M4U_CLNTMOD_JPEG_ENC, dst_mva, dst_mva + dst_size - 1, RT_RANGE_HIGH_PRIORITY, 1))
    {
        xlog("Can't insert tlb range\n");
        pM4uDrv->m4u_dealloc_mva(M4U_CLNTMOD_JPEG_ENC, dst_va, dst_size, dst_mva);
        delete pM4uDrv;
        pM4uDrv = NULL;
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;  
    }
    
    M4U_PORT_STRUCT m4uPort;

    m4uPort.ePortID = M4U_PORT_JPEG_ENC;
    m4uPort.Virtuality = 1;
    m4uPort.Security = 0;
    m4uPort.Distance = 1;
    m4uPort.Direction = 0;

    if(M4U_STATUS_OK != pM4uDrv->m4u_config_port(&m4uPort))
    {
        xlog("Can not config m4u port\n");
        pM4uDrv->m4u_invalid_tlb_range(M4U_CLNTMOD_JPEG_ENC, dst_mva, dst_mva + dst_size - 1);
        pM4uDrv->m4u_dealloc_mva(M4U_CLNTMOD_JPEG_ENC, dst_va, dst_size, dst_mva);
        delete pM4uDrv;
        pM4uDrv = NULL;
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;         
    }

    pM4uDrv->m4u_cache_sync(M4U_CLNTMOD_JPEG_ENC, M4U_CACHE_FLUSH_BEFORE_HW_WRITE_MEM, dst_va, dst_size);

    drvIn.dstBufferAddr = (unsigned int)dst_mva;
#else

    drvIn.dstBufferAddr = (unsigned int)inParams.dstBufferAddr & (~JPEG_ENC_DST_ADDR_OFFSET_MASK);
    drvIn.dstBufAddrOffset      = 0; 
    drvIn.dstBufAddrOffsetMask  = (inParams.dstBufferAddr & (JPEG_ENC_DST_ADDR_OFFSET_MASK));     
#endif

    Is420 = (inParams.dstFormat == JPEG_SAMPLING_FORMAT_NV12 || inParams.dstFormat == JPEG_SAMPLING_FORMAT_NV21)? 1:0;
    padding_width  = TO_CEIL((inParams.dstWidth), 16);
    padding_height = TO_CEIL((inParams.dstHeight), Is420? 16:8 );
    
    drvIn.dstBufferSize = inParams.dstBufferSize;
    drvIn.encWidth = inParams.dstWidth;      
    drvIn.encHeight = inParams.dstHeight;    
    drvIn.encFormat = (unsigned int)inParams.dstFormat;
    drvIn.encQuality = (unsigned int)inParams.dstQuality;

    drvIn.enableEXIF = (unsigned char)inParams.enableEXIF;
    //drvIn.enableSyncReset = (unsigned char)inParams.enableSyncReset;
    
    width_even = ((inParams.dstWidth +1)>>1)<<1 ;
    if(!Is420)
      width_even = width_even <<1;

    //set hw register directly
    drvIn.restartInterval       = inParams.restartInterval;      
    drvIn.srcBufferAddr         = inParams.srcBufferAddr;   //TBD: check validation
    drvIn.srcChromaAddr         = inParams.srcChromaAddr;   //TBD: check validation 
    drvIn.imgStride             = TO_CEIL(width_even, (Is420? 16: 32) ); 
    drvIn.memStride             = inParams.srcBufferStride;  //TO_CEIL(width_even, 128);
    drvIn.totalEncDU            = ((padding_width>>4)*(padding_height>>(Is420? 4:3))*(Is420? 6:4))-1;

    
    
    drvIn.allocBuffer = 0;
    
    xlog("config: w %x, h %x, pw %x, ph %x, format %x, Q %x, Exif %x, dri %x!! \n", 
    drvIn.encWidth, drvIn.encHeight, padding_width, padding_height, drvIn.encFormat, drvIn.encQuality, drvIn.enableEXIF, drvIn.restartInterval);
    
    xlog("config: 420 %x, Src_Y %x, Src_C %x, imgS %x, memS %x, DU %x!!\n", 
    Is420, drvIn.srcBufferAddr, drvIn.srcChromaAddr, drvIn.imgStride, drvIn.memStride, drvIn.totalEncDU);
    xlog("config: dst_base %x, dst_of %x, dst_ms %x, m4u %x!!\n", 
    drvIn.dstBufferAddr, drvIn.dstBufAddrOffset, drvIn.dstBufAddrOffsetMask, m4u_flag);
    
    //xlog("JpegEncHal: 420 %x, Src_Y %x, Src_C %x, imgS %x, memS %x, DU %x, %x, %x, %x, \n");
    
    if(ioctl(drvID, JPEG_ENC_IOCTL_CONFIG, &drvIn))
    {
#ifdef USE_M4U
        if(pM4uDrv)
        {
            pM4uDrv->m4u_invalid_tlb_range(M4U_CLNTMOD_JPEG_ENC, dst_mva, dst_mva + dst_size - 1);
            pM4uDrv->m4u_dealloc_mva(M4U_CLNTMOD_JPEG_ENC, dst_va, dst_size, dst_mva);
            delete pM4uDrv;
            pM4uDrv = NULL;
        }
#endif
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;
    }
    
    return JPEG_ENC_STATUS_OK;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
JPEG_ENC_STATUS_ENUM jpegEncStart(int drvID)
{
    if(drvID != currentID)
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;

    if(ioctl(drvID, JPEG_ENC_IOCTL_START))
    {
#ifdef USE_M4U
        if(pM4uDrv)
        {
            pM4uDrv->m4u_invalid_tlb_range(M4U_CLNTMOD_JPEG_ENC, dst_mva, dst_mva + dst_size - 1);
            pM4uDrv->m4u_dealloc_mva(M4U_CLNTMOD_JPEG_ENC, dst_va, dst_size, dst_mva);
            delete pM4uDrv;
            pM4uDrv = NULL;     
        }
#endif
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;
    }
    
    return JPEG_ENC_STATUS_OK;    
}
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
JPEG_ENC_STATUS_ENUM jpegEncWaitIRQ(int drvID, long timeout_msec, unsigned int *encodeSize, JPEG_ENC_RESULT_ENUM *status)
{
    JPEG_ENC_DRV_OUT drvOut;
    unsigned int fileSize, encodeResult, cycle;

    if(drvID != currentID)
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;
        
    drvOut.timeout = timeout_msec;
    drvOut.fileSize = &fileSize;
    drvOut.result = &encodeResult;
    drvOut.cycleCount = &cycle ;
  
    if(ioctl(drvID, JPEG_ENC_IOCTL_WAIT, &drvOut))
    {
#ifdef USE_M4U
        if(pM4uDrv)
        {
            pM4uDrv->m4u_invalid_tlb_range(M4U_CLNTMOD_JPEG_ENC, dst_mva, dst_mva + dst_size - 1);
            pM4uDrv->m4u_dealloc_mva(M4U_CLNTMOD_JPEG_ENC, dst_va, dst_size, dst_mva);
            delete pM4uDrv;
            pM4uDrv = NULL;
        }
#endif
        return JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE;
    }
    
    *encodeSize = fileSize;
    
    switch(encodeResult)
    {
        case 0:
            *status = JPEG_ENC_RESULT_DONE;
            break;
        case 1:
            *status = JPEG_ENC_RESULT_PAUSE;
            break;
        case 2:
            *status = JPEG_ENC_RESULT_HALT;
            break;
    }

#ifdef USE_M4U
    if(pM4uDrv)
    {
        pM4uDrv->m4u_invalid_tlb_range(M4U_CLNTMOD_JPEG_ENC, dst_mva, dst_mva + dst_size - 1);
        pM4uDrv->m4u_dealloc_mva(M4U_CLNTMOD_JPEG_ENC, dst_va, dst_size, dst_mva);
        delete pM4uDrv;
        pM4uDrv = NULL;
    }
#endif

    return JPEG_ENC_STATUS_OK;
}



