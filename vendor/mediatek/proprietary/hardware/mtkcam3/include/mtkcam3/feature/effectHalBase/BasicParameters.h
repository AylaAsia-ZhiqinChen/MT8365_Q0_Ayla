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

#ifndef _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_SDK_HAL_IEFFECTBASICPARAMETER_H_
#define _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_SDK_HAL_IEFFECTBASICPARAMETER_H_

#include <utils/String8.h>
#include <utils/Vector.h>
#include <utils/RefBase.h>
#include <utils/KeyedVector.h>
#include <utils/Errors.h>
#include <utils/Singleton.h>
#include <utils/Flattenable.h>

#include <inttypes.h>
namespace NSCam
{
using namespace android;

/**
 *  @brief BasicParameters is the basic class to passing data from java to native.
 *
 */
class BasicParameters : public LightFlattenable<BasicParameters>, public RefBase
{
public:     //LightFlattenable
    inline  bool        isFixedSize() const
    {
        return false;
    }
    size_t              getFlattenedSize() const;

    ///< flatten implementation
    status_t            flatten(void* buffer, size_t size) const;
    ///< unflatten implementation
    status_t            unflatten(void const* buffer, size_t size);

public:     //BasicParameters
    BasicParameters();
    BasicParameters(const String8 &params)
    {
        unflatten(params);
    }

    ///< copy constructor
    BasicParameters(const BasicParameters& other);

    ///< operator constructor
    BasicParameters&    operator=(const BasicParameters& other);

    String8             flatten() const;
    void                unflatten(const String8 &params);

    ///< set parameter with string value
    void                set(const char *key, const char *value);
    ///< set parameter with integer value
    void                set(const char *key, int value);
    ///< set parameter with int64_t value
    void                setInt64(const char *key, int64_t value);
    ///< set parameter with float value
    void                setFloat(const char *key, float value);
    ///< set parameter with pointer value
    void                setPtr(const char *key, void *value);
    ///< get string result according to input string
    const char *        get(const char *key) const;
    ///< get integer result according to input string
    int                 getInt(const char *key) const;
    //< get int64_t result according to input string
    int64_t             getInt64(const char *key) const;
    ///< get float result according to input string
    float               getFloat(const char *key) const;
    ///< get pointer result according to input string
    void*               getPtr(const char *key) const;
    ///< remove key
    void                remove(const char *key);
    /**
     *  @brief      get all keys of this object
     *  @return     return a vector of key
     */
    Vector<String8>     getKeys();
    void                dump();
    int                 parse_pair(const char *str, int *first, int *second, char delim,char **endptr = NULL);

    static const char KEY_PICTURE_SIZE[];
    static const char KEY_PICTURE_FORMAT[];

    static const char KEY_ROTATION[];
    static const char KEY_ZOOM_RATIOS[];

    static const char KEY_JPEG_THUMBNAIL_SIZES[];
    static const char KEY_JPEG_THUMBNAIL_QUALITY[];
    static const char KEY_JPEG_QUALITY[];

    // Pixel color formats for KEY_PREVIEW_FORMAT, KEY_PICTURE_FORMAT,
    // and KEY_VIDEO_FRAME_FORMAT
    static const char PIXEL_FORMAT_YUV422SP[];
    static const char PIXEL_FORMAT_YUV420SP[]; // NV21
    static const char PIXEL_FORMAT_YUV422I[]; // YUY2
    static const char PIXEL_FORMAT_YUV420P[]; // YV12
    static const char PIXEL_FORMAT_RGB565[];
    static const char PIXEL_FORMAT_RGBA8888[];
    static const char PIXEL_FORMAT_JPEG[];

    static const char KEY_IMAGE_WIDTH[];
    static const char KEY_IMAGE_HEIGHT[];

    // Face effect use
    // result
    static const char KEY_DETECTED_FACE_NUM[];
    static const char KEY_DETECTED_FACE_RESULT[];
    static const char KEY_DETECTED_SMILE_RESULT[];
    static const char KEY_DETECTED_GESTURE_RESULT[];
    static const char KEY_DETECTED_SCENE[];
    // control
    static const char KEY_DETECTED_SD_ENABLE[];
    static const char KEY_DETECTED_FD_ENABLE[];
    static const char KEY_DETECTED_GS_ENABLE[];
    static const char KEY_DETECTED_ASD_ENABLE[];
    static const char KEY_DETECTED_ASD_3A_INFO[];
    static const char KEY_DETECTED_USE_FD[];
    static const char KEY_DETECTED_USE_GS[];
    static const char KEY_DETECTED_USE_ASD[];


// protected:
//     MINT32                      mLogLevel;

private:

    /**
     *  key value pair of string type
     */
    DefaultKeyedVector<String8,String8>    mMap;
};


#if 0
class EffectResult : public BasicParameters
{
public:
    //@todo
};


class EffectParameter : public BasicParameters
{
public:
    //@todo
};


class EffectCaptureRequirement : public BasicParameters
{
public:
    //@todo
};
#else
typedef BasicParameters EffectResult;
typedef BasicParameters EffectParameter;
typedef BasicParameters EffectCaptureRequirement;
#endif

} //end namespace NSCam


#endif  //_MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_SDK_HAL_IEFFECTBASICPARAMETER_H_
