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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cutils/log.h>
#include <utils/Errors.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cutils/log.h>

#include <linux/ion.h>
#include <linux/ion_drv.h>

#include "jpeg_enc_hal.h"
#include "jpeg_hal.h"
#include "m4u_lib.h"
#include "img_mmu.h"

#include "img_common_def.h"

#include <utils/Trace.h>

#ifdef JENC_LOCK_VIDEO
#include <val_types_private.h>
#include <val_api_private.h>
#include <vdec_drv_base.h>
#endif

extern "C" {
#include <uree/system.h>
#include <uree/mem.h>
#include <tz_cross/ta_mem.h>
#include <tz_cross/ta_jpeg.h>
}

#include "jpeg_enc_sec.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "JPEG UREE"

#define JPG_LOGI(fmt, arg...)    ALOGI(LOG_TAG fmt, ##arg)
#define JPG_LOGD(fmt, arg...)    ALOGD(LOG_TAG fmt, ##arg)
#define JPG_LOGW(fmt, arg...)    ALOGW(LOG_TAG fmt, ##arg)
#define JPG_LOGE(fmt, arg...)    ALOGE(LOG_TAG fmt, ##arg)

//#define JPEG_ENABLE_DUMP

#define IS_NO_ALIGN(x,a) ((x)&((a)-1))

MTK_JENC_SECURE_RESULT UREE_JpegEncSecStart(JPEG_ENC_HAL_IN *JpgEncParam)
{
    TZ_RESULT ret;
    UREE_SESSION_HANDLE JpegEncSession = 0;
    UREE_SESSION_HANDLE JpegMemSession = 0;
    UREE_SHAREDMEM_HANDLE JpegEncParamShareHandle = 0;
    UREE_SHAREDMEM_PARAM  JpegEncsharedParam;
    MTEEC_PARAM JpegEncparam[4];

    /*Create mem session*/
    ret = UREE_CreateSession(TZ_TA_MEM_UUID, &JpegMemSession);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE (" Mem CreateSession Error: %d, line:%d\n", ret, __LINE__);
        return MTK_JPEG_ENC_SECURE_FAIL;
    }

	/*Register share memory*/
    JpegEncsharedParam.buffer = JpgEncParam;
    JpegEncsharedParam.size = sizeof(JPEG_ENC_HAL_IN);
    ret = UREE_RegisterSharedmemWithTag(JpegMemSession, &JpegEncParamShareHandle, &JpegEncsharedParam, "UREE_JpegEncSecStart");
    if (ret != TZ_RESULT_SUCCESS)
    {
        ALOGE ("UREE_RegisterSharedmemWithTag Error: %d, line:%d", ret, __LINE__);
    }

	/*Create JpegEnc session*/
    ret = UREE_CreateSession(TZ_TA_JPEG_UUID, &JpegEncSession);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE (" Jpeg Enc CreateSession Error: %d, line:%d\n", ret, __LINE__);
        return MTK_JPEG_ENC_SECURE_FAIL;
    }

    /* dump input buf for debug, it should be closed by default. */
#ifdef JPEG_ENABLE_DUMP
    uint8_t *SrcTempBuf = NULL, *SrcTempChromaBuf = NULL;
    JPEG_ENC_SEC_MEM_PARA *SrcMemParam = NULL;
    FILE *fp;

    UREE_SHAREDMEM_HANDLE SrcTempBufShareHandle = 0;
    UREE_SHAREDMEM_HANDLE SrcTempChromaBufShareHandle = 0;
    UREE_SHAREDMEM_HANDLE SrcMemParamShareHandle = 0;

    SrcTempBuf = (uint8_t *)malloc(JpgEncParam->srcBufferSize);
    memset(SrcTempBuf, 0, JpgEncParam->srcBufferSize);

    JpegEncsharedParam.buffer = SrcTempBuf;
    JpegEncsharedParam.size = JpgEncParam->srcBufferSize;
    ret = UREE_RegisterSharedmemWithTag(JpegMemSession, &SrcTempBufShareHandle, &JpegEncsharedParam, "UREE_JpegEncSecStart");
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("UREE_RegisterSharedmemWithTag Error: %d, line:%d", ret, __LINE__);
    }

    if (JpgEncParam->bIsSrc2p)
    {
        SrcTempChromaBuf = (uint8_t *)malloc(JpgEncParam->srcChromaSize);
        memset(SrcTempChromaBuf, 0, JpgEncParam->srcChromaSize);

        JpegEncsharedParam.buffer = SrcTempChromaBuf;
        JpegEncsharedParam.size = JpgEncParam->srcChromaSize;
        ret = UREE_RegisterSharedmemWithTag(JpegMemSession, &SrcTempChromaBufShareHandle, &JpegEncsharedParam, "UREE_JpegEncSecStart");
        if (ret != TZ_RESULT_SUCCESS)
        {
            JPG_LOGE ("UREE_RegisterSharedmemWithTag Error: %d, line:%d", ret, __LINE__);
        }
    }

	SrcMemParam = (JPEG_ENC_SEC_MEM_PARA *)malloc(sizeof(JPEG_ENC_SEC_MEM_PARA));
    memset(SrcMemParam, 0, sizeof(JPEG_ENC_SEC_MEM_PARA));

	SrcMemParam->SrcHandle1 = JpgEncParam->srcLumaSecHandle;
	SrcMemParam->SrcHandle2 = JpgEncParam->srcChromaSecHandle;
	SrcMemParam->DstHandle1 = SrcTempBufShareHandle;
	SrcMemParam->DstHandle2 = SrcTempChromaBufShareHandle;
	SrcMemParam->Size1 = JpgEncParam->srcBufferSize;
	SrcMemParam->Size2 = JpgEncParam->srcChromaSize;

    JpegEncsharedParam.buffer = SrcMemParam;
    JpegEncsharedParam.size = sizeof(JPEG_ENC_SEC_MEM_PARA);
    ret = UREE_RegisterSharedmemWithTag(JpegMemSession, &SrcMemParamShareHandle, &JpegEncsharedParam, "UREE_JpegEncSecStart");
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("UREE_RegisterSharedmemWithTag Error: %d, line:%d", ret, __LINE__);
    }

    JpegEncparam[0].memref.handle = (unsigned int)SrcMemParamShareHandle;
    JpegEncparam[0].memref.size = sizeof(JPEG_ENC_SEC_MEM_PARA);
    JpegEncparam[0].memref.offset = 0;
    ret = UREE_TeeServiceCall(JpegEncSession, TZCMD_JPEG_ENC_DUMP_INPUT, TZ_ParamTypes1(TZPT_MEMREF_INPUT), JpegEncparam);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE (" UREE_TeeServiceCall failed, ret: 0x%x, line:%d \n", ret, __LINE__);

        return MTK_JPEG_ENC_SECURE_FAIL;
    }

    fp = fopen("/data/jpegtest/jpeg_enc_luma.raw", "w");
    fwrite(SrcTempBuf, 1, JpgEncParam->srcBufferSize, fp);
#if 0
    int index;
    for(index = 0 ; index < enc_size ; index++)
    {
        fprintf(fp, "%c", dst_va[index]);
    }
#endif
    fclose(fp);

    ret = UREE_UnregisterSharedmem (JpegMemSession, SrcTempBufShareHandle);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("UREE_UnregisterSharedmem Error: %d, line:%d", ret, __LINE__);
        return MTK_JPEG_ENC_SECURE_FAIL;
    }

    if (JpgEncParam->bIsSrc2p)
    {
        ret = UREE_UnregisterSharedmem (JpegMemSession, SrcTempChromaBufShareHandle);
        if (ret != TZ_RESULT_SUCCESS)
        {
            JPG_LOGE ("UREE_UnregisterSharedmem Error: %d, line:%d", ret, __LINE__);
            return MTK_JPEG_ENC_SECURE_FAIL;
        }
    }

    ret = UREE_UnregisterSharedmem (JpegMemSession, SrcMemParamShareHandle);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("UREE_UnregisterSharedmem Error: %d, line:%d", ret, __LINE__);
        return MTK_JPEG_ENC_SECURE_FAIL;
    }

    if (JpgEncParam->bIsSrc2p)
    {
        fp = fopen("/data/jpegtest/jpeg_enc_chroma.raw", "w");
        fwrite(SrcTempChromaBuf, 1, JpgEncParam->srcChromaSize, fp);
        fclose(fp);
    }

    if (SrcTempBuf != NULL)
        free(SrcTempBuf);

    if (JpgEncParam->bIsSrc2p && SrcTempChromaBuf != NULL)
        free(SrcTempChromaBuf);

    if (SrcMemParam != NULL)
        free(SrcMemParam);
#endif

	/*TEE service call*/
    JpegEncparam[0].memref.handle = (unsigned int)JpegEncParamShareHandle;
    JpegEncparam[0].memref.size = sizeof(JPEG_ENC_HAL_IN);
    JpegEncparam[0].memref.offset = 0;

    ret = UREE_TeeServiceCall(JpegEncSession, TZCMD_JPEG_ENC_START, TZ_ParamTypes1(TZPT_MEMREF_INPUT), JpegEncparam);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE (" UREE_TeeServiceCall failed, ret: 0x%x, line:%d \n", ret, __LINE__);

        return MTK_JPEG_ENC_SECURE_FAIL;
    }

	/* dump output buf for debug, it should be closed by default. */
#ifdef JPEG_ENABLE_DUMP
    uint8_t *DstTempBuf = NULL;
    JPEG_ENC_SEC_MEM_PARA *DstMemParam = NULL;

    UREE_SHAREDMEM_HANDLE DstTempBufShareHandle = 0;
    UREE_SHAREDMEM_HANDLE DstMemParamShareHandle = 0;

    DstTempBuf = (uint8_t *)malloc(JpgEncParam->dstBufferSize);
    memset(DstTempBuf, 0, JpgEncParam->dstBufferSize);

    JpegEncsharedParam.buffer = DstTempBuf;
    JpegEncsharedParam.size = JpgEncParam->dstBufferSize;
    ret = UREE_RegisterSharedmemWithTag(JpegMemSession, &DstTempBufShareHandle, &JpegEncsharedParam, "UREE_JpegEncSecStart");
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("UREE_RegisterSharedmemWithTag Error: %d, line:%d", ret, __LINE__);
    }

	DstMemParam = (JPEG_ENC_SEC_MEM_PARA *)malloc(sizeof(JPEG_ENC_SEC_MEM_PARA));
    memset(DstMemParam, 0, sizeof(JPEG_ENC_SEC_MEM_PARA));

	DstMemParam->SrcHandle1 = JpgEncParam->dstSecHandle;
	DstMemParam->SrcHandle2 = 0;
	DstMemParam->DstHandle1 = DstTempBufShareHandle;
	DstMemParam->DstHandle2 = 0;
	DstMemParam->Size1 = JpgEncParam->dstBufferSize;
	DstMemParam->Size2 = 0;

    JpegEncsharedParam.buffer = DstMemParam;
    JpegEncsharedParam.size = sizeof(JPEG_ENC_SEC_MEM_PARA);
    ret = UREE_RegisterSharedmemWithTag(JpegMemSession, &DstMemParamShareHandle, &JpegEncsharedParam, "UREE_JpegEncSecStart");
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("UREE_RegisterSharedmemWithTag Error: %d, line:%d", ret, __LINE__);
    }

    JpegEncparam[0].memref.handle = (unsigned int)DstMemParamShareHandle;
    JpegEncparam[0].memref.size = sizeof(JPEG_ENC_SEC_MEM_PARA);
    JpegEncparam[0].memref.offset = 0;

    ret = UREE_TeeServiceCall(JpegEncSession, TZCMD_JPEG_ENC_DUMP_OUTPUT, TZ_ParamTypes1(TZPT_MEMREF_INPUT), JpegEncparam);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE (" UREE_TeeServiceCall failed, ret: 0x%x, line:%d \n", ret, __LINE__);

        return MTK_JPEG_ENC_SECURE_FAIL;
    }

    fp = fopen("/data/jpegtest/jpeg_enc_out.jpg", "w");
    fwrite(DstTempBuf, 1, JpgEncParam->dstDatasize, fp);
#if 0
    int index;
    for(index = 0 ; index < enc_size ; index++)
    {
        fprintf(fp, "%c", dst_va[index]);
    }
#endif
    fclose(fp);

    ret = UREE_UnregisterSharedmem (JpegMemSession, DstTempBufShareHandle);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("UREE_UnregisterSharedmem Error: %d, line:%d", ret, __LINE__);
        return MTK_JPEG_ENC_SECURE_FAIL;
    }

    ret = UREE_UnregisterSharedmem (JpegMemSession, DstMemParamShareHandle);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("UREE_UnregisterSharedmem Error: %d, line:%d", ret, __LINE__);
        return MTK_JPEG_ENC_SECURE_FAIL;
    }

    if (DstTempBuf != NULL)
        free(DstTempBuf);

    if (DstMemParam != NULL)
        free(DstMemParam);
#endif

	/*Unregister shared memory*/
    if (JpegEncParamShareHandle != 0)
    {
        ret = UREE_UnregisterSharedmem (JpegMemSession, JpegEncParamShareHandle);

        if (ret != TZ_RESULT_SUCCESS)
        {
            JPG_LOGE ("UREE_UnregisterSharedmem Error: %d, line:%d", ret, __LINE__);
            return MTK_JPEG_ENC_SECURE_FAIL;
        }
    }

	/*Close JpegEnc session*/
    ret = UREE_CloseSession(JpegEncSession);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("Jpeg UREE_CloseSession Error: %d, line:%d", ret, __LINE__);
        return MTK_JPEG_ENC_SECURE_FAIL;
    }

    /*Close mem Session*/
    ret = UREE_CloseSession(JpegMemSession);
    if (ret != TZ_RESULT_SUCCESS)
    {
        JPG_LOGE ("Jpeg UREE_CloseSession Error: %d, line:%d", ret, __LINE__);
        return MTK_JPEG_ENC_SECURE_FAIL;
    }

	return MTK_JPEG_ENC_SECURE_SUCCESS;
}

