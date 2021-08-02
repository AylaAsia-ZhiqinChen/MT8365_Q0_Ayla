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
 *       LEGAL DISCLAIMER
 *
 *       (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *       BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *       THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *       FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *       ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *       INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *       A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *       WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *       INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *       ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *       NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *       OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *       BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *       RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *       FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *       THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *       OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#include "MyHdr.h"
#define MHAL_CAM_THUMB_ADDR_OFFSET  (64 * 1024)


//systrace
#if 1
#ifndef ATRACE_TAG
#define ATRACE_TAG                           ATRACE_TAG_CAMERA
#endif
#include <utils/Trace.h>

#define HDR_TRACE_CALL()                      ATRACE_CALL()
#define HDR_TRACE_NAME(name)                  ATRACE_NAME(name)
#define HDR_TRACE_BEGIN(name)                 ATRACE_BEGIN(name)
#define HDR_TRACE_END()                       ATRACE_END()
#else
#define HDR_TRACE_CALL()
#define HDR_TRACE_NAME(name)
#define HDR_TRACE_BEGIN(name)
#define HDR_TRACE_END()
#endif
using namespace android;

///////////////////////////////////////////////////////////////////////////
/// @brief Do small image normalization.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL HdrShot::do_Normalization()
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;

    MBOOL ret = MTRUE;
    MUINT32 u4OutputFrameNum = OutputFrameNumGet();

    HDR_PIPE_CONFIG_PARAM rHdrPipeConfigParam;
    rHdrPipeConfigParam.eHdrRound = 1;

    // prepare HDR parameters
    switch (mHdrRoundTotal)
    {
        case 1:
            rHdrPipeConfigParam.u4SourceImgWidth  = mu4W_yuv;
            rHdrPipeConfigParam.u4SourceImgHeight = mu4H_yuv;
            for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
            {
                rHdrPipeConfigParam.pSourceImgBufAddr[i] =
                    (MUINT8*)mpSourceImgBuf[i].virtAddr;
                rHdrPipeConfigParam.pSmallImgBufAddr[i] =
                    (MUINT8*)mpSmallImgBuf[i].virtAddr;
            }
            break;
        // TODO: check if the second round is used
        case 2:
            rHdrPipeConfigParam.u4SourceImgWidth = mu4W_first;
            rHdrPipeConfigParam.u4SourceImgHeight = mu4H_first;
            for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
            {
                rHdrPipeConfigParam.pSourceImgBufAddr[i] =
                    (MUINT8*)mpFirstRunSourceImgBuf[i].virtAddr;
                rHdrPipeConfigParam.pSmallImgBufAddr[i] =
                    (MUINT8*)mpSmallImgBuf[i].virtAddr;
            }
            break;
    }

    // configure HDR parameters
    ret &= mpHdrHal->HdrSmallImgBufSet(rHdrPipeConfigParam);
    if (ret != MTRUE)
    {
        MY_ERR("Failed to configure HDR parameters");
        return ret;
    }

    // normalize small images; the normalized images are put back to SmallImgbuf[]
    {
        HDR_TRACE_NAME("Do_Normalization");
        ret &= mpHdrHal->Do_Normalization();
    }

    // save normalized small image for debug
    if (HDR_DEBUG_SAVE_NORMALIZED_SMALL_IMAGE || mDebugMode)
    {
        for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
        {
            char szFileName[100];
            ::sprintf(szFileName, HDR_DEBUG_OUTPUT_FOLDER "%04d_3_normalized_mpSmallImgBuf[%d]_%dx%d_r%d.y",
                    mu4RunningNumber, i, mu4W_small, mu4H_small, mHdrRound);
            dumpToFile(szFileName,
                    (MUINT8*)mpSmallImgBuf[i].virtAddr, mu4SmallImgSize);
        }
    }

    FUNCTION_LOG_END;
    return ret;
}

///////////////////////////////////////////////////////////////////////////
/// @brief Do SE to get GMV.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_SE(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;
    MUINT32 u4OutputFrameNum = OutputFrameNumGet();

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_SE");
#endif

    // Prepare SE Input Info.
    HDR_PIPE_SE_INPUT_INFO rHdrPipeSEInputInfo;
    rHdrPipeSEInputInfo.u2SEImgWidth    = mu4W_se;
    rHdrPipeSEInputInfo.u2SEImgHeight = mu4H_se;
    for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
    {
        rHdrPipeSEInputInfo.pSEImgBufAddr[i] = (MUINT8*)mpSEImgBuf[i].virtAddr;
    }

    // Do SE.
    ret = mpHdrHal->Do_SE(rHdrPipeSEInputInfo);


#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: do_SE Time");
#endif

    FUNCTION_LOG_END;
    return    ret;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Do Feature Extraction.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_FeatureExtraction(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;
    MUINT32 u4OutputFrameNum = OutputFrameNumGet();

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_FeatureExtraction");
#endif

    //       Config MAV hal init Info.
    HDR_PIPE_FEATURE_EXTRACT_INPUT_INFO rHdrPipeFeatureExtractInputInfo;
    rHdrPipeFeatureExtractInputInfo.u2SmallImgW = mu4W_small;
    rHdrPipeFeatureExtractInputInfo.u2SmallImgH = mu4H_small;
    for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
    {
        rHdrPipeFeatureExtractInputInfo.pSmallImgBufAddr[i] = (MUINT8*)mpSmallImgBuf[i].virtAddr;
    }
    //     Assign working buffer
    rHdrPipeFeatureExtractInputInfo.pWorkingBuffer = (MUINT8*)mpHdrWorkingBuf.virtAddr;
    //MY_DBG("[do_FeatureExtraction] u2SmallImgW=%d", rHdrPipeFeatureExtractInputInfo.u2SmallImgW);
    //MY_DBG("[do_FeatureExtraction] u2SmallImgH=%d", rHdrPipeFeatureExtractInputInfo.u2SmallImgH);
    //MY_DBG("[do_FeatureExtraction] pWorkingBuffer=0x%x", rHdrPipeFeatureExtractInputInfo.pWorkingBuffer);

    //       Do Feature Extraction and Feature Matching.
    mpHdrHal->Do_FeatureExtraction(rHdrPipeFeatureExtractInputInfo);


#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: Do_FeatureExtraction Time");
#endif

    FUNCTION_LOG_END;
    return    ret;

}


///////////////////////////////////////////////////////////////////////////
/// @brief Do Alignment.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_Alignment(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;
    MUINT32 u4OutputFrameNum = OutputFrameNumGet();

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_Alignment");
#endif


    ret =
            mpHdrHal->HdrWorkingBufSet((MUINT8*)mpHdrWorkingBuf.virtAddr, mu4HdrWorkingBufSize)
#if (HDR_PROFILE_CAPTURE)
        &&    DbgTmr.print("HdrProfiling:: HdrWorkingBufSet Time")
#endif
          //  ()  must set bmap buffer before do alignment
        &&     do_SetBmapBuffer()
        &&    mpHdrHal->Do_Alignment()
            ;

#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: Do_Alignment Time");
#endif

    FUNCTION_LOG_END;
    return    ret;
}


MBOOL
HdrShot::do_SetBmapBuffer(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;

    // Set the resulting Weighting Map.
    MY_DBG("[do_SetBmapBuffer] bmap_width=%d", mHdrSetBmapInfo.bmap_width);
    MY_DBG("[do_SetBmapBuffer] bmap_height=%d", mHdrSetBmapInfo.bmap_height);
    MY_DBG("[do_SetBmapBuffer] bmap_image_size=%d", mHdrSetBmapInfo.bmap_image_size);
    for(MUINT32 i=0; i<OutputFrameNumGet(); i++) {
        MY_DBG("[do_SetBmapBuffer] bmap_image_addr[%d]=%p"
                , i
                , mHdrSetBmapInfo.bmap_image_addr[i]);
    }
    ret = mpHdrHal->WeightingMapInfoSet(&mHdrSetBmapInfo);

    FUNCTION_LOG_END;
    return ret;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get original Weighting Table.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_OriWeightMapGet(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;
    MUINT32 u4OutputFrameNum = OutputFrameNumGet();

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_OriWeightMapGet");
#endif


    // Get the resulting Weighting Map.
    mpHdrHal->WeightingMapInfoGet(OriWeight);
    // Show obtained OriWeightingTbl info.

#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: do_OriWeightMapGet Time");
#endif

    for (MUINT32 i = 0; i < u4OutputFrameNum; i++) {
        MY_DBG("[do_OriWeightMapGet] OriWeight[%d]->W/H: (%d, %d). Addr: %p.", i, OriWeight[i]->weight_table_width, OriWeight[i]->weight_table_height, OriWeight[i]->weight_table_data);
        if(OriWeight[i]->weight_table_data != mHdrSetBmapInfo.bmap_image_addr[i]) {
            MY_ERR("OriWeight[%d]->weight_table_data=%p, it should be %p"
                    , i
                    , OriWeight[i]->weight_table_data
                    , mHdrSetBmapInfo.bmap_image_addr[i]);
        }
    }

    if(HDR_DEBUG_SAVE_WEIGHTING_MAP || mDebugMode) {
        for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
        {
            char szFileName[100];
            ::sprintf(szFileName, HDR_DEBUG_OUTPUT_FOLDER "%04d_5_WeightMap%d_%dx%d_r%d.y", mu4RunningNumber, i, OriWeight[i]->weight_table_width, OriWeight[i]->weight_table_height, mHdrRound);
            dumpToFile(szFileName, OriWeight[i]->weight_table_data, OriWeight[i]->weight_table_width * OriWeight[i]->weight_table_height);
        }
    }

    FUNCTION_LOG_END;
    return    ret;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Do Down-scale Weighting Map.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_DownScaleWeightMap(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;
    MUINT32 u4OutputFrameNum = OutputFrameNumGet();

    #if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_DownScaleWeightMap");
    #endif

    #if 0    //test
    //@TODO speed this up
    mpWeightMapBuf[0].size = OriWeight[0]->weight_table_width * OriWeight[0]->weight_table_height;
    allocMem(&mpWeightMapBuf[0]);
    for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
    {
        MY_DBG("[do_DownScaleWeightMap] CDPResize %d/%d", i, u4OutputFrameNum);

        ::memcpy((void*)mpWeightMapBuf[0].virtAddr,
                (void*)OriWeight[i]->weight_table_data,
                OriWeight[i]->weight_table_width * OriWeight[i]->weight_table_height);

        ret = CDPResize(
                &mpWeightMapBuf[0], OriWeight[i]->weight_table_width, OriWeight[i]->weight_table_height, eImgFmt_Y800,
                &mpDownSizedWeightMapBuf[i], mu4W_dsmap, mu4H_dsmap, eImgFmt_Y800, 0);
    }
    deallocMem(&mpWeightMapBuf[0]);
    #else
    for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
    {
        MY_DBG("[do_DownScaleWeightMap] CDPResize %d/%d", i, u4OutputFrameNum);
        MY_DBG("[do_DownScaleWeightMap] src[%d]=%zu", i, mWeightingBuf[i].virtAddr);
        MY_DBG("[do_DownScaleWeightMap] des[%d]=%zu", i, mpDownSizedWeightMapBuf[i].virtAddr);

        ret = CDPResize(
                &mWeightingBuf[i], OriWeight[i]->weight_table_width, OriWeight[i]->weight_table_height, eImgFmt_Y800,
                &mpDownSizedWeightMapBuf[i], mu4W_dsmap, mu4H_dsmap, eImgFmt_Y800, 0);
    }
    #endif

#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: Down-scaleWeightMap Time");
#endif

    if(HDR_DEBUG_SAVE_DOWNSCALED_WEIGHTING_MAP || mDebugMode) {
        for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
        {
            char szFileName[100];
            ::sprintf(szFileName, HDR_DEBUG_OUTPUT_FOLDER "%04d_6_mpDownSizedWeightMapBuf[%d]_%dx%d_r%d.y", mu4RunningNumber, i, mu4W_dsmap, mu4H_dsmap, mHdrRound);
            dumpToFile(szFileName, (MUINT8*)mpDownSizedWeightMapBuf[i].virtAddr, mu4DownSizedWeightMapSize);
        }
    }

    FUNCTION_LOG_END;
    return    ret;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Do Up-scale Weighting Map.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_UpScaleWeightMap(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;

    MUINT32 u4OutputFrameNum = OutputFrameNumGet();

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_UpScaleWeightMap");
#endif


    // Up-sample Down-sized Weighting Map to make them blurry. And Save them for debug.
    for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
    {
        ret = CDPResize(&mpDownSizedWeightMapBuf[i], mu4W_dsmap, mu4H_dsmap, eImgFmt_Y800,
                        &mpBlurredWeightMapBuf[i], BlurredWeight[i]->weight_table_width, BlurredWeight[i]->weight_table_height, eImgFmt_Y800, 0);
    }

#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: Up-scaleWeightMap Time");
#endif

    // Show info.
    for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
        MY_VERB("[do_UpScaleWeightMap] BlurredWeight[%d]->W/H: (%d, %d). Addr: 0x%08X.", i, BlurredWeight[i]->weight_table_width, BlurredWeight[i]->weight_table_height, BlurredWeight[i]->weight_table_height);

    if(HDR_DEBUG_SAVE_BLURRED_WEIGHTING_MAP || mDebugMode) {
        for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
        {
            char szFileName[100];
            ::sprintf(szFileName, HDR_DEBUG_OUTPUT_FOLDER "%04d_7_blurred_WeightMap%d_%dx%d_r%d.y", mu4RunningNumber, i, BlurredWeight[i]->weight_table_width, BlurredWeight[i]->weight_table_height, mHdrRound);
            dumpToFile(szFileName, BlurredWeight[i]->weight_table_data, BlurredWeight[i]->weight_table_width * BlurredWeight[i]->weight_table_height);
        }
    }

    FUNCTION_LOG_END;
    return    ret;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Do Fusion.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_Fusion(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;
    MUINT32 u4OutputFrameNum = OutputFrameNumGet();

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_Fusion");
#endif

    //[ION]
    ret = ret && mpHdrHal->ResultBufferSet((MUINT8 *)mBlendingBuf.virtAddr, mBlendingBuf.size);

    // Do Fusion.
    ret = ret && mpHdrHal->Do_Fusion(BlurredWeight);

#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: do_Fusion Time");
#endif

    FUNCTION_LOG_END;
    return    ret;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Do .
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_HdrCroppedResultGet(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_HdrCroppedResultGet");
#endif

    // Get HDR result.
    ret = mpHdrHal->HdrCroppedResultGet(mrHdrCroppedResult);
    MUINT32 u4HdrCroppedResultSize = mrHdrCroppedResult.output_image_width * mrHdrCroppedResult.output_image_height * 3 / 2;

#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: do_HdrCroppedResultGet Time");
#endif

    MY_DBG("[do_HdrCroppedResultGet] rCroppedHdrResult:: W/H: (%d, %d). Addr: %p. Size: %d."
            , mrHdrCroppedResult.output_image_width
            , mrHdrCroppedResult.output_image_height
            , mrHdrCroppedResult.output_image_addr
            , u4HdrCroppedResultSize);    // *3/2: YUV420 size.

    if(HDR_DEBUG_SAVE_HDR_RESULT || mDebugMode)
    {
        char szFileName[100];
        ::sprintf(szFileName, HDR_DEBUG_OUTPUT_FOLDER "%04d_8_HdrResult_%dx%d_r%d.i420", mu4RunningNumber, mrHdrCroppedResult.output_image_width, mrHdrCroppedResult.output_image_height, mHdrRound);
        dumpToFile(szFileName, (MUINT8 *)mrHdrCroppedResult.output_image_addr, u4HdrCroppedResultSize);
    }

    FUNCTION_LOG_END;
    return    ret;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Do .
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_CroppedResultResize(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_CroppedResultResize");
#endif


#if 0    //test
    // Resize to original image size and convert to I420.
    IMEM_BUF_INFO    mpCroppedResultBuf;
    MUINT32 u4HdrCroppedResultSize = mrHdrCroppedResult.output_image_width * mrHdrCroppedResult.output_image_height * 3 / 2;
    mpCroppedResultBuf.size = u4HdrCroppedResultSize;
    allocMem(&mpCroppedResultBuf);
    memcpy((void*)mpCroppedResultBuf.virtAddr, (void*)mrHdrCroppedResult.output_image_addr, u4HdrCroppedResultSize);
    ret = CDPResize(&mpCroppedResultBuf, mrHdrCroppedResult.output_image_width, mrHdrCroppedResult.output_image_height, eImgFmt_I420,
                    &mpResultImgBuf, mu4W_yuv, mu4H_yuv, eImgFmt_I420, 0);
#else
    if((MUINTPTR)mrHdrCroppedResult.output_image_addr != mBlendingBuf.virtAddr) {
        MY_ERR("mrHdrCroppedResult.output_image_addr=%p must equal mBlendingBuf.virtAddr=%zu"
                , mrHdrCroppedResult.output_image_addr
                , mBlendingBuf.virtAddr);
    }
    ret = CDPResize(&mBlendingBuf, mrHdrCroppedResult.output_image_width, mrHdrCroppedResult.output_image_height, eImgFmt_I420,
                    &mpResultImgBuf, mu4W_yuv, mu4H_yuv, eImgFmt_I420, 0);
#endif


#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: do_CroppedResultResize Time");
#endif

    if(HDR_DEBUG_SAVE_RESIZE_HDR_RESULT || mDebugMode)
    {
        char szFileName[100];
        ::sprintf(szFileName, HDR_DEBUG_OUTPUT_FOLDER "%04d_9_final_HdrResult_%dx%d_r%d.i420", mu4RunningNumber, mu4W_yuv, mu4H_yuv, mHdrRound);
        dumpToFile(szFileName, (MUINT8 *)mpResultImgBuf.virtAddr, mu4ResultImgSize);    // *2: YUV422 size.
    }

#if 0    //test
    deallocMem(&mpCroppedResultBuf);
#endif

    FUNCTION_LOG_END;
    return    ret;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Do HDR setting clear.
///
/// @return SUCCDSS (TRUE) or Fail (FALSE).
///////////////////////////////////////////////////////////////////////////
MBOOL
HdrShot::
do_HdrSettingClear(void)
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("do_HdrSettingClear");
#endif


#if 0
    mpHdrHal->SaveHdrLog(mu4RunningNumber);
#endif

    // Clear HDR Setting.
    mpHdrHal->HdrSettingClear();

#if (HDR_PROFILE_CAPTURE)
    DbgTmr.print("HdrProfiling:: do_HdrSettingClear Time");
#endif

    FUNCTION_LOG_END;
    return    ret;

}



/******************************************************************************
*
*******************************************************************************/
MBOOL
HdrShot::
do_SecondRound()
{
    HDR_TRACE_CALL();
    FUNCTION_LOG_START;
    MBOOL  ret = MTRUE;

    MUINT32 u4OutputFrameNum = OutputFrameNumGet();
    MY_DBG("[do_SecondRound] - E. u4OutputFrameNum: %d.", u4OutputFrameNum);

#if (HDR_PROFILE_CAPTURE)
    MyDbgTimer DbgTmr("pass2");
#endif


    // Config HDR Parameters.
    HDR_PIPE_CONFIG_PARAM rHdrPipeConfigParam;
    rHdrPipeConfigParam.eHdrRound = 2;
    rHdrPipeConfigParam.u4SourceImgWidth = mu4W_yuv;
    rHdrPipeConfigParam.u4SourceImgHeight = mu4H_yuv;
    for (MUINT32 i = 0; i < u4OutputFrameNum; i++)
    {
        rHdrPipeConfigParam.pSourceImgBufAddr[i] = (MUINT8*)mpSourceImgBuf[i].virtAddr;
        //rHdrPipeConfigParam.pSmallImgBufAddr[i] = mpSmallImgBuf[i].virtAddr;
    }

    ret = mpHdrHal->HdrSmallImgBufSet(rHdrPipeConfigParam)
        //&& mpHdrHal->Do_Alignment()
    ;

    FUNCTION_LOG_END;
    return ret;
}



