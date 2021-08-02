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

#define LOG_TAG "Metadata2/MetadataConverter"
//
#include <mtkcam/utils/std/ULog.h>
#include <mtkcam/utils/std/common.h>
#include <mtkcam/def/common.h>
#include "MetadataConverter.h"

#include <inttypes.h>

//#include <hardware/camera3.h>
#include <MetadataConverter.h>
#include <mtkcam/utils/metadata/mtk_metadata_types.h>
using namespace android;
using namespace NSCam;
//

sp<IMetadataConverter>
IMetadataConverter::
createInstance(IMetadataTagSet const &pTagInfo)
{
    #if (PLATFORM_SDK_VERSION >= 21)
    return new MetadataConverter(pTagInfo);
    #else
    return NULL;
    #endif
}


MetadataConverter::
MetadataConverter(IMetadataTagSet const &pTagInfo)
    : mpTagInfo(pTagInfo)
{
}


size_t
MetadataConverter::
getCameraMetadataSize(const camera_metadata *metadata) const
{
    return ::get_camera_metadata_size(metadata);
}


void
MetadataConverter::
freeCameraMetadata(camera_metadata *metadata) const
{
    if ( CC_LIKELY(metadata!=nullptr) ) {
        ::free_camera_metadata(metadata);
    }
}


MVOID
MetadataConverter::
dumpAll(const IMetadata &rMetadata, int frameNo = -1) const
{
    #if (PLATFORM_SDK_VERSION >= 21)
    META_LOGD("dump all metadata for frameNo %d count: %d", frameNo, rMetadata.count());
    for(size_t i = 0; i < rMetadata.count(); i++)
    {
        MUINT32 mtk_tag = rMetadata.entryAt(i).tag();
        MUINT32 mtk_type = getTagInfo()->getType(mtk_tag);
        MUINT32 android_tag = getTagInfo()->getAndroidTag((MUINT32)mtk_tag);
        MUINT32 android_type = get_camera_metadata_tag_type(android_tag);
        //META_LOGD("0x%x, %s", mtk_tag, getTagInfo()->getName((MUINT32)mtk_tag));
        const char *tag_name, *tag_section;
        tag_section = get_camera_metadata_section_name(android_tag);
        if (tag_section == NULL) {
            tag_section = "unknownSection";
        }
        tag_name = get_camera_metadata_tag_name(android_tag);
        if (tag_name == NULL) {
            tag_name = "unknownTag";
        }
        const char *type_name;
        if (android_type >= NUM_TYPES) {
            type_name = "unknown";
        } else {
            type_name = camera_metadata_type_names[android_type];
        }
        String8 str = String8().format("%s.%s (%05x): %s[%" PRIu32 "]",
             tag_section, tag_name, mtk_tag, type_name, rMetadata.entryAt(i).count());
        // META_LOGD("%s", str.string());
        print(rMetadata, mtk_tag, mtk_type, str);
    }
    #endif
}

MVOID
MetadataConverter::
dump(const IMetadata &rMetadata, int frameNo = -1)  const
{
    #if (PLATFORM_SDK_VERSION >= 21)
    META_LOGD("dump partial metadata for frameNo %d", frameNo);
    for(size_t i = 0; i < rMetadata.count(); i++)
    {
        MUINT32 mtk_tag = rMetadata.entryAt(i).tag();
        MUINT32 mtk_type = getTagInfo()->getType(mtk_tag);
        MUINT32 android_tag = getTagInfo()->getAndroidTag((MUINT32)mtk_tag);
        MUINT32 android_type = get_camera_metadata_tag_type(android_tag);
        switch (android_tag)
        {
            case ANDROID_CONTROL_AE_TARGET_FPS_RANGE:
            case ANDROID_SENSOR_FRAME_DURATION:
            case ANDROID_CONTROL_AE_COMPENSATION_STEP:
            case ANDROID_CONTROL_AF_REGIONS:
            case ANDROID_CONTROL_AE_REGIONS:
            case ANDROID_SCALER_CROP_REGION:
            case ANDROID_SENSOR_EXPOSURE_TIME:
            case ANDROID_SENSOR_SENSITIVITY:
            case ANDROID_CONTROL_POST_RAW_SENSITIVITY_BOOST:
            {
                //META_LOGD("0x%x, %s", mtk_tag, getTagInfo()->getName((MUINT32)mtk_tag));
                const char *tag_name, *tag_section;
                tag_section = get_camera_metadata_section_name(android_tag);
                if (tag_section == NULL) {
                    tag_section = "unknownSection";
                }
                tag_name = get_camera_metadata_tag_name(android_tag);
                if (tag_name == NULL) {
                    tag_name = "unknownTag";
                }
                const char *type_name;
                if (android_type >= NUM_TYPES) {
                    type_name = "unknown";
                } else {
                    type_name = camera_metadata_type_names[android_type];
                }
                String8 str = String8().format("%s.%s (%05x): %s[%" PRIu32 "]",
                     tag_section, tag_name, mtk_tag, type_name, rMetadata.entryAt(i).count());
                // META_LOGD("%s", str.string());
                print(rMetadata, mtk_tag, mtk_type, str);
            }
            break;
            default:
            break;
        }
    }
    #endif
}


MVOID
MetadataConverter::
print(const IMetadata &rMetadata, const MUINT32 tag, const MUINT32 type, const String8& str) const
{
    #if (PLATFORM_SDK_VERSION >= 21)
    String8 str_val;
    switch (type) {
        case TYPE_MUINT8:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            for(size_t i = 0; i < entry.count(); i++) {
               str_val += String8::format("%d ", entry.itemAt(i, Type2Type< MUINT8 >()));
            }
        }
        break;

        case TYPE_MINT32:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            for(size_t i = 0; i < entry.count(); i++) {
               str_val += String8::format("%d ", entry.itemAt(i, Type2Type< MINT32 >()));
            }
        }
        break;

        case TYPE_MFLOAT:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            //META_LOGD("%d %d %f", entry.tag(), entry.count(), entry.itemAt(0, Type2Type< MFLOAT >()));
            for(size_t i = 0; i < entry.count(); i++) {
                str_val += String8::format("%f ", entry.itemAt(i, Type2Type< MFLOAT >()));
            }
        }
        break;

        case TYPE_MINT64:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            for(size_t i = 0; i < entry.count(); i++) {
                MINT64 val = entry.itemAt(i, Type2Type< MINT64 >());
                str_val += String8::format("%" PRId64 " ", val);
            }
        }
        break;
        case TYPE_MDOUBLE:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            for(size_t i = 0; i < entry.count(); i++) {
                str_val += String8::format("%f ", entry.itemAt(i, Type2Type< MDOUBLE >()));
            }
        }
        break;
        case TYPE_MRational:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            for(size_t i = 0; i < entry.count(); i++) {
                MRational item = entry.itemAt(i, Type2Type< MRational >());
                str_val += String8::format("[%d / %d] ", item.numerator, item.numerator);
            }
        }
        break;
        case TYPE_MPoint:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            for(size_t i = 0; i < entry.count(); i++) {
                MPoint item = entry.itemAt(i, Type2Type< MPoint >());
                str_val += String8::format("(%d, %d) ", item.x, item.y);
            }
        }
        break;
        case TYPE_MSize:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            for(size_t i = 0; i < entry.count(); i++) {
                MSize item = entry.itemAt(i, Type2Type< MSize >());
                str_val += String8::format("(%d,%d) ", item.w, item.h);
            }
        }
        break;
        case TYPE_MRect:
        {
            IMetadata::IEntry entry = rMetadata.entryFor(tag);
            for(size_t i = 0; i < entry.count(); i++) {
                MRect item = entry.itemAt(i, Type2Type< MRect >());
                str_val += String8::format("(%d, %d, %d, %d) ", item.p.x, item.p.y, item.s.w, item.s.h);
            }
        }
        break;
        default:
        break;
    }

    META_LOGD("%s => %s", str.string(), str_val.string());
    #endif
}

