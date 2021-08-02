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
 * MediaTek Inc. (C) 2019. All rights reserved.
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

#define LOG_TAG "mtkcam-mtk_scenario_mgr"
//
#include <cutils/properties.h>
//
#include <mtkcam/utils/std/Log.h>
#include <mtkcam/utils/std/ULog.h>
#include <mtkcam/utils/std/Trace.h>
#include <mtkcam/utils/std/common.h>
//
#include <mtkcam3/3rdparty/mtk/mtk_scenario_mgr_ISP.h>
//
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

CAM_ULOG_DECLARE_MODULE_ID(MOD_FEATURE_SETTING_POLICY);

/******************************************************************************
 *
 ******************************************************************************/
using namespace android;
using namespace NSCam;
using namespace NSCam::NSPipelinePlugin;
using namespace NSCam::v3::pipeline::policy::scenariomgr;
/******************************************************************************
 *
 ******************************************************************************/

#define MY_LOGV(fmt, arg...)        CAM_ULOGMV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_ULOGMD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_ULOGMI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_ULOGMW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_ULOGME("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_ULOGM_FATAL("[%s] " fmt, __FUNCTION__, ##arg)
//
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
// ======================================================================================================
// TODO: Feature Combinations for MTK
// #define  <feature combination>              (key feature         | post-processing features | ...)
//
// isp hidl feature combination
#define MTK_FEATURE_COMBINATION_SINGLE_FULL         (NO_FEATURE_NORMAL | MTK_FEATURE_DCE| MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_DSDN| MTK_FEATURE_NR| MTK_FEATURE_CZ)
#define MTK_FEATURE_COMBINATION_AINR_FULL           (MTK_FEATURE_AINR  | MTK_FEATURE_DCE| MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_DSDN| MTK_FEATURE_NR| MTK_FEATURE_CZ)
#define MTK_FEATURE_COMBINATION_MFNR_FULL           (MTK_FEATURE_MFNR  | MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_NR| MTK_FEATURE_CZ)

// single frame in
#define MTK_FEATURE_COMBINATION_SINGLE_RAW_TO_YUV   (NO_FEATURE_NORMAL | MTK_FEATURE_DCE| MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_DSDN| MTK_FEATURE_NR| MTK_FEATURE_CZ)
#define MTK_FEATURE_COMBINATION_SINGLE_RAW_TO_JPEG  (NO_FEATURE_NORMAL | MTK_FEATURE_DCE| MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_DSDN| MTK_FEATURE_NR| MTK_FEATURE_CZ)
#define MTK_FEATURE_COMBINATION_SINGLE_YUV_TO_YUV   (NO_FEATURE_NORMAL) // image transform and crop/resize only
#define MTK_FEATURE_COMBINATION_SINGLE_YUV_TO_JPEG  (NO_FEATURE_NORMAL) // encode yuv to jpeg only.
// multiple frame in
#define MTK_FEATURE_COMBINATION_AINR_RAW_TO_RAW     (MTK_FEATURE_AINR)  // output AINR RAW only
#define MTK_FEATURE_COMBINATION_AINR_RAW_TO_YUV     (MTK_FEATURE_AINR  | MTK_FEATURE_DCE| MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_NR| MTK_FEATURE_CZ)
#define MTK_FEATURE_COMBINATION_AINR_RAW_TO_JPEG    (MTK_FEATURE_AINR  | MTK_FEATURE_DCE| MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_NR| MTK_FEATURE_CZ)
#define MTK_FEATURE_COMBINATION_MFNR_RAW_TO_YUV     (MTK_FEATURE_MFNR  | MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_NR| MTK_FEATURE_CZ)
#define MTK_FEATURE_COMBINATION_MFNR_RAW_TO_JPEG    (MTK_FEATURE_MFNR  | MTK_FEATURE_DRE| MTK_FEATURE_HFG| MTK_FEATURE_NR| MTK_FEATURE_CZ)

// ======================================================================================================
//
/******************************************************************************
 *
 ******************************************************************************/

namespace NSCam {
namespace v3 {
namespace pipeline {
namespace policy {
namespace scenariomgr {

const std::vector<std::unordered_map<int32_t, ScenarioFeatures>>  gMtkIspHidlScenarioFeaturesMaps =
{
    {
        // capture
        CAMERA_SCENARIO_START(MTK_ISP_HIDL_SCENARIO_CAPTURE_ALL)
        ADD_CAMERA_FEATURE_SET(MTK_FEATURE_AINR,     MTK_FEATURE_COMBINATION_AINR_FULL)
        ADD_CAMERA_FEATURE_SET(MTK_FEATURE_MFNR,     MTK_FEATURE_COMBINATION_MFNR_FULL)
        ADD_CAMERA_FEATURE_SET(NO_FEATURE_NORMAL,    MTK_FEATURE_COMBINATION_SINGLE_FULL)
        CAMERA_SCENARIO_END
        //
        CAMERA_SCENARIO_START(MTK_ISP_HIDL_SCENARIO_CAPTURE_SINGLE_RAW_TO_YUV) // encode single raw to yuv
        ADD_CAMERA_FEATURE_SET(NO_FEATURE_NORMAL,    MTK_FEATURE_COMBINATION_SINGLE_RAW_TO_YUV)
        CAMERA_SCENARIO_END
        //
        CAMERA_SCENARIO_START(MTK_ISP_HIDL_SCENARIO_CAPTURE_SINGLE_RAW_TO_JPEG) // encode single raw to yuv and encode to jepg
        ADD_CAMERA_FEATURE_SET(NO_FEATURE_NORMAL,    MTK_FEATURE_COMBINATION_SINGLE_RAW_TO_JPEG)
        CAMERA_SCENARIO_END
        //
        CAMERA_SCENARIO_START(MTK_ISP_HIDL_SCENARIO_CAPTURE_SINGLE_YUV_TO_YUV) // single yuv reprocessing
        ADD_CAMERA_FEATURE_SET(NO_FEATURE_NORMAL,    MTK_FEATURE_COMBINATION_SINGLE_YUV_TO_YUV)
        CAMERA_SCENARIO_END
        //
        CAMERA_SCENARIO_START(MTK_ISP_HIDL_SCENARIO_CAPTURE_SINGLE_YUV_TO_JPEG) // encode single yuv to jpeg
        ADD_CAMERA_FEATURE_SET(NO_FEATURE_NORMAL,    MTK_FEATURE_COMBINATION_SINGLE_YUV_TO_JPEG)
        CAMERA_SCENARIO_END
        //
        CAMERA_SCENARIO_START(MTK_ISP_HIDL_SCENARIO_CAPTURE_MULTIPLE_RAW_TO_RAW) // multiframe raw to single raw
        ADD_CAMERA_FEATURE_SET(MTK_FEATURE_AINR,     MTK_FEATURE_COMBINATION_AINR_RAW_TO_RAW)
        CAMERA_SCENARIO_END
        //
        CAMERA_SCENARIO_START(MTK_ISP_HIDL_SCENARIO_CAPTURE_MULTIPLE_RAW_TO_YUV) // multiframe raw to single yuv
        ADD_CAMERA_FEATURE_SET(MTK_FEATURE_AINR,     MTK_FEATURE_COMBINATION_AINR_RAW_TO_YUV)
        ADD_CAMERA_FEATURE_SET(MTK_FEATURE_MFNR,     MTK_FEATURE_COMBINATION_MFNR_RAW_TO_YUV)
        CAMERA_SCENARIO_END
        //
        CAMERA_SCENARIO_START(MTK_ISP_HIDL_SCENARIO_CAPTURE_MULTIPLE_RAW_TO_JPEG) // multiframe raw to jpeg
        ADD_CAMERA_FEATURE_SET(MTK_FEATURE_AINR,     MTK_FEATURE_COMBINATION_AINR_RAW_TO_JPEG)
        ADD_CAMERA_FEATURE_SET(MTK_FEATURE_MFNR,     MTK_FEATURE_COMBINATION_MFNR_RAW_TO_JPEG)
        CAMERA_SCENARIO_END
    },
};

auto mtk_get_isp_hidl_capture_scenario(
    int32_t &scenario, /*eCameraScenario*/
    const ScenarioHint& scenarioHint,
    IMetadata const* pAppMetadata
) -> bool
{
    if (CC_UNLIKELY(pAppMetadata == nullptr)) {
        MY_LOGE("pAppMetadata is invalid nullptr!");
        return false;
    }

    CAM_ULOGM_APILIFE();

    scenario = CUSTOMER_ISP_HIDL_SCENARIO_UNKNOW;
    MY_LOGD("scenarioHint(type:%d, hint:%d)", scenarioHint.ispProcessInOutType, scenarioHint.ispTuningHint);
    switch( scenarioHint.ispProcessInOutType )
    {
        case eIspProcessInOutType_SingleRawIn_YuvOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_SINGLE_RAW_TO_YUV;
            break;
        case eIspProcessInOutType_SingleRawIn_JpegOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_SINGLE_RAW_TO_JPEG;
            break;
        case eIspProcessInOutType_SingleYuvIn_YuvOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_SINGLE_YUV_TO_YUV;
            break;
        case eIspProcessInOutType_SingleYuvIn_JpegOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_SINGLE_YUV_TO_JPEG;
            break;
        case eIspProcessInOutType_MultipleRawIn_RawOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_MULTIPLE_RAW_TO_RAW;
            break;
        case eIspProcessInOutType_MultipleRawIn_YuvOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_MULTIPLE_RAW_TO_YUV;
            break;
        case eIspProcessInOutType_MultipleRawIn_JpegOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_MULTIPLE_RAW_TO_JPEG;
            break;
        case eIspProcessInOutType_MultipleYuvIn_YuvOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_MULTIPLE_YUV_TO_YUV;
            break;
        case eIspProcessInOutType_MultipleYuvIn_JpegOut:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_MULTIPLE_YUV_TO_JPEG;
            break;
        case eIspProcessInOutType_QuerySupportedKeyFeature:
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_ALL;
            break;
        default:
            MY_LOGD("no dedicated scenario, choose default all scenario");
            scenario = MTK_ISP_HIDL_SCENARIO_CAPTURE_ALL;
            break;
    }

    MY_LOGD("scenario:%d", scenario);

    return true;
}

auto mtk_get_isp_hidl_features_table_by_scenario(
    int32_t const scenario, /*eCameraScenario*/
    ScenarioFeatures& scenarioFeatures
) -> bool
{
    //CAM_ULOGM_APILIFE();

    size_t tableSize = gMtkIspHidlScenarioFeaturesMaps.size();
    MY_LOGD("scenario:%d, table size:%zu", scenario, tableSize);

    auto scenarioFeaturesMap = gMtkIspHidlScenarioFeaturesMaps[0];

    auto iter_got = scenarioFeaturesMap.find(scenario);
    if ( iter_got != scenarioFeaturesMap.end()) {
        scenarioFeatures = iter_got->second;
        MY_LOGI("find features for scenario(%d : %s)", scenario, scenarioFeatures.scenarioName.c_str());
    }
    else{
        MY_LOGE("cannot find features for scenario(%d) in gScenarioFeaturesMap", scenario);
        return false;
    }

    return true;
}

auto mtk_get_isp_hidl_capture_config_by_scenario(
    const int32_t scenario __unused, /*eCameraScenario*/
    CaptureScenarioConfig& captureConfig __unused
) -> bool
{
    // reserved
    return true;
}

bool mtk_get_isp_hidl_capture_scenario(
    int32_t &scenario,
    ScenarioFeatures &scenarioFeatures,
    CaptureScenarioConfig &captureConfig,
    const ScenarioHint &scenarioHint,
    const IMetadata *pAppMetadata
)
{
    return mtk_get_isp_hidl_capture_scenario(scenario, scenarioHint, pAppMetadata) &&
           mtk_get_isp_hidl_capture_config_by_scenario(scenario, captureConfig) &&
           mtk_get_isp_hidl_features_table_by_scenario(scenario, scenarioFeatures);
}

};  //namespace scenariomgr
};  //namespace policy
};  //namespace pipeline
};  //namespace v3
};  //namespace NSCam

