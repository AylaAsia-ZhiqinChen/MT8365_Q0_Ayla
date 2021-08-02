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
#include "MediaTypes.h"
#include "mhal_jpeg.h"
#include "jpeg_hal.h"

extern "C" {
#include <pthread.h>
#include <semaphore.h>
}

#include <cutils/log.h>

#define xlog(...) \
        do { \
            ALOGW(__VA_ARGS__); \
        } while (0)

#undef LOG_TAG
#define LOG_TAG "mHalJpgEnc"

static JpgEncHal* jpgEncoder = NULL;

static pthread_mutex_t resMutex = PTHREAD_MUTEX_INITIALIZER;

static int _return_false(const char msg[])
{
    if(jpgEncoder != NULL)
    {
        delete jpgEncoder;
        jpgEncoder = NULL;
    }

    xlog("%s", msg);
    
    return JPEG_ERROR_INVALID_DRIVER;
}

int mHalJpgEncStart(MHAL_JPEG_ENC_START_IN *encInParams)
{
    //JpgEncHal* jpgEncoder = new JpgEncHal();
    pthread_mutex_lock(&resMutex);
    
    if(jpgEncoder != NULL) 
    {
        xlog("hw encoder is busy");
        pthread_mutex_unlock(&resMutex);
        return JPEG_ERROR_INVALID_DRIVER;
    }
    jpgEncoder = new JpgEncHal();
    
    pthread_mutex_unlock(&resMutex);
    
    if(!jpgEncoder->lock())
    {
        return _return_false("can't lock resource");
    }
    
    jpgEncoder->setSrcWidth(encInParams->srcWidth);
    jpgEncoder->setSrcHeight(encInParams->srcHeight);
    jpgEncoder->setDstWidth(encInParams->dstWidth);
    jpgEncoder->setDstHeight(encInParams->dstHeight);
    
    switch(encInParams->outFormat)
    {
        case JPEG_ENC_YUV444:
            jpgEncoder->setEncFormat(JpgEncHal::kYUV_444_Format);
            break;
            
        case JPEG_ENC_YUV422:
            jpgEncoder->setEncFormat(JpgEncHal::kYUV_422_Format);
            break;
            
        case JPEG_ENC_YUV411:
            jpgEncoder->setEncFormat(JpgEncHal::kYUV_411_Format);
            break;

        case JPEG_ENC_YUV420:
            jpgEncoder->setEncFormat(JpgEncHal::kYUV_420_Format);
            break;
            
        case JPEG_ENC_GRAYLEVEL:
            jpgEncoder->setEncFormat(JpgEncHal::kYUV_400_Format);
            break;

        default:
            jpgEncoder->unlock();
            return _return_false("unsupport out format");
    }
    
    switch(encInParams->inFormat)
    {
        case JPEG_IN_FORMAT_RGB565:
            jpgEncoder->setSrcFormat(JpgEncHal::kRGB_565_Format);
            break;
            
        case JPEG_IN_FORMAT_RGB888:
            jpgEncoder->setSrcFormat(JpgEncHal::kRGB_888_Format);
            break;

        case JPEG_IN_FORMAT_ARGB8888:
            jpgEncoder->setSrcFormat(JpgEncHal::kABGR_8888_Format); // Android color order is RGBA
            break;
            
        default:
            jpgEncoder->unlock();
            return _return_false("unsupport input format");         
    }

    if(encInParams->quality > 100) 
    {
        jpgEncoder->unlock();
        return _return_false("unsupport quality(0~100)");
    }
    
    jpgEncoder->setQuality(encInParams->quality);
    jpgEncoder->setSrcAddr((void *)encInParams->srcAddr);
    jpgEncoder->setDstAddr((void *)encInParams->dstAddr);
    jpgEncoder->setDstSize(encInParams->dstSize);
    jpgEncoder->enableSOI(true);

    if(!jpgEncoder->start(encInParams->encSize))
    {
        jpgEncoder->unlock();
        return _return_false("encode failed~~~");
    }
    
    jpgEncoder->unlock();
    delete jpgEncoder;
    jpgEncoder = NULL;

    return 0;
}
