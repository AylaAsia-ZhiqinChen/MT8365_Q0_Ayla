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
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "StereoSizeProvider"

#include <math.h>
#include <algorithm>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <mtkcam/utils/metastore/IMetadataProvider.h>
#pragma GCC diagnostic pop
#include <mtkcam/utils/metadata/client/mtk_metadata_tag.h>

#include <mtkcam3/feature/stereo/hal/stereo_setting_provider.h>
#include <mtkcam3/feature/stereo/hal/stereo_size_provider.h>
#include <mtkcam/utils/std/Log.h>
#include <mtkcam/utils/std/ULog.h>
#include "pass2/pass2A_size_providers.h"
#include <stereo_crop_util.h>
#include <vsdof/hal/ProfileUtil.h>
#include <stereo_tuning_provider.h>
#define CAM_ULOG_MODULE_ID MOD_MULTICAM_PROVIDER
CAM_ULOG_DECLARE_MODULE_ID(CAM_ULOG_MODULE_ID);
#include "../setting-provider/stereo_setting_provider_kernel.h"
#include "base_size.h"

#if (1==HAS_AIDEPTH)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <MTKAIDepth.h>
#include <MTKVideoAIDepth.h>
#pragma GCC diagnostic pop
#endif

using namespace android;
using namespace NSCam;

#define STEREO_SIZE_PROVIDER_DEBUG

#ifdef STEREO_SIZE_PROVIDER_DEBUG    // Enable debug log.

#undef __func__
#define __func__ __FUNCTION__

#define MY_LOGD(fmt, arg...)    CAM_ULOGMD("[%s]" fmt, __func__, ##arg)
#define MY_LOGI(fmt, arg...)    CAM_ULOGMI("[%s]" fmt, __func__, ##arg)
#define MY_LOGW(fmt, arg...)    CAM_ULOGMW("[%s] WRN(%5d):" fmt, __func__, __LINE__, ##arg)
#define MY_LOGE(fmt, arg...)    CAM_ULOGME("[%s] %s ERROR(%5d):" fmt, __func__,__FILE__, __LINE__, ##arg)

#else   // Disable debug log.
#define MY_LOGD(a,...)
#define MY_LOGI(a,...)
#define MY_LOGW(a,...)
#define MY_LOGE(a,...)
#endif  // STEREO_SIZE_PROVIDER_DEBUG

#define MY_LOGV_IF(cond, arg...)    if (cond) { MY_LOGV(arg); }
#define MY_LOGD_IF(cond, arg...)    if (cond) { MY_LOGD(arg); }
#define MY_LOGI_IF(cond, arg...)    if (cond) { MY_LOGI(arg); }
#define MY_LOGW_IF(cond, arg...)    if (cond) { MY_LOGW(arg); }
#define MY_LOGE_IF(cond, arg...)    if (cond) { MY_LOGE(arg); }

// STEREO_BASE_SIZE_4_3 is defined in base_size.h under platform folder or using default if not exist

std::map<int, StereoArea> sizeCache1x;
StereoArea
StereoSize::getStereoArea1x(const StereoSizeConfig &config)
{
    StereoArea area = STEREO_BASE_SIZE_4_3;

    const bool IS_VERTICAL_MODULE = (StereoSettingProvider::getModuleRotation() & 0x2);
    if(!StereoSizeProvider::getInstance()->__getCustomziedSize(area)) {
        ENUM_STEREO_RATIO imageRatio = StereoSettingProvider::imageRatio();
        if(StereoSettingProvider::isDeNoise()) {
           imageRatio = eRatio_4_3;
        }

        if(sizeCache1x.find(imageRatio) != sizeCache1x.end()) {
            area = sizeCache1x[imageRatio];
        } else {
            int m, n;
            imageRatioMToN(imageRatio, m, n);

            ENUM_IMAGE_RATIO_ALIGNMENT alignment = E_KEEP_AREA_SIZE;
#ifdef CUSTOMIZE_RATIO_ALIGNMENT
            alignment = CUSTOMIZE_RATIO_ALIGNMENT;
#endif
            if(E_KEEP_WIDTH == alignment) {
                if(m * 3 / 4 < n) {
                    alignment = E_KEEP_HEIGHT;
                }
            }
            else if(E_KEEP_HEIGHT == alignment)
            {
                if(n * 4 / 3 < m) {
                    alignment = E_KEEP_WIDTH;
                }
            }

            area.applyRatio(imageRatio, alignment);
            area.rotatedByModule();

            MSize contentSize = area.contentSize();
            int nAlign = 16;
#ifdef CUSTOMIZE_BUFFER_ALIGNMENT
            nAlign = CUSTOMIZE_BUFFER_ALIGNMENT;
#endif
            applyNAlign(nAlign, contentSize.w);
            if(IS_VERTICAL_MODULE) {
                contentSize.h = contentSize.w * m / n;
            } else {
                contentSize.h = contentSize.w * n / m;
            }
            contentSize.h = (contentSize.h+1) & (~1);

            area.size = area.padding + contentSize;

            sizeCache1x[imageRatio] = area;

            char msg[128];
            sprintf(msg, "StereoArea1x(%d:%d)", m, n);
            area.print(LOG_TAG, msg);
        }
    }

    //Vertical size is rotated, only rotate for horizontal request
    if(IS_VERTICAL_MODULE &&
       !config.needRotate)
    {
        area.rotate(eRotate_90);
    }

    if(!config.hasPadding) {
        area.removePadding();
    }

    return area;
}

//===============================================================
//  StereoSizeProvider implementation
//===============================================================

//===============================================================
//  Singleton and init operations
//===============================================================
Mutex StereoSizeProvider::__lock;
Mutex StereoSizeProvider::__P1Lock;
Mutex StereoSizeProvider::__activeCropLock;

StereoSizeProvider *
StereoSizeProvider::getInstance()
{
    Mutex::Autolock lock(__lock);
    static StereoSizeProvider _instance;
    return &_instance;
}

StereoSizeProvider::StereoSizeProvider()
    : LOG_ENABLED(StereoSettingProvider::isLogEnabled())
{
}

bool
StereoSizeProvider::getPass1Size( ENUM_STEREO_SENSOR sensor,
                                  EImageFormat format,
                                  EPortIndex port,
                                  ENUM_STEREO_SCENARIO scenario,
                                  MRect &tgCropRect,
                                  MSize &outSize,
                                  MUINT32 &strideInBytes,
                                  ENUM_STEREO_RATIO requiredRatio
                                )
{
    // AutoProfileUtil profile(LOG_TAG, "getPass1Size");

    ENUM_STEREO_RATIO currentRatio = (EPortIndex_IMGO == port) ? eRatio_4_3 : StereoSettingProvider::imageRatio();
    if(requiredRatio != eRatio_Unknown){
        MY_LOGD_IF(LOG_ENABLED, "user required ratio:%d", requiredRatio);
        currentRatio = requiredRatio;
    }

    int sensorScenario = (eSTEREO_SENSOR_MAIN1 == sensor)
                         ? StereoSettingProvider::getSensorScenarioMain1()
                         : StereoSettingProvider::getSensorScenarioMain2();
    if(SENSOR_SCENARIO_ID_UNNAMED_START == sensorScenario) {
        MY_LOGE("Unknown sensor scenario for sensor %d", sensor-1);
        return false;
    }

    enum {
        SENSOR_BITS          = 1,
        PORT_BITS            = 2,   //RRZO, IMGO, P1 YUV
        SENSOR_SCENARIO_BITS = 4,
        RATIO_BITS           = 3,
        LOGICAL_DEVICE_BITS  = 3,
        PORTRAIT_MODE_BITS   = 1,
        FEATURE_MODE_BITS    = 6,
    };

    MUINT32 key = (eSTEREO_SENSOR_MAIN1 == sensor) ? 0 : 1;
    int shiftBits = SENSOR_BITS;
    switch(port) {
        case EPortIndex_IMGO:
            key |= 1<<shiftBits;
            break;
        case EPortIndex_CRZO_R2:
            key |= 2<<shiftBits;
            break;
        default:
            break;
    }

    shiftBits += PORT_BITS;
    key |= sensorScenario << shiftBits;
    shiftBits += SENSOR_SCENARIO_BITS;
    key |= currentRatio << shiftBits;
    shiftBits += RATIO_BITS;
    key |= StereoSettingProvider::getLogicalDeviceID() << shiftBits;
    shiftBits += LOGICAL_DEVICE_BITS;
    int featureMode = StereoSettingProvider::getStereoFeatureMode();
    bool isPortraitMode = featureMode & E_STEREO_FEATURE_PORTRAIT_FLAG;
    key |= isPortraitMode << shiftBits;
    shiftBits += PORTRAIT_MODE_BITS;
    featureMode &= ~E_STEREO_FEATURE_PORTRAIT_FLAG;
    key |= featureMode << shiftBits;
    shiftBits += FEATURE_MODE_BITS;
    key |= format << shiftBits; //put this line at last

    int m, n;
    imageRatioMToN(currentRatio, m, n);

    bool isCached = false;
    {
        Mutex::Autolock lock(__P1Lock);
        if(__pass1TgCropRectMap.find(key) != __pass1TgCropRectMap.end()) {
            tgCropRect    = __pass1TgCropRectMap.find(key)->second;
            outSize       = __pass1OutSizeMap.find(key)->second;
            strideInBytes = __pass1StrideMap.find(key)->second;
            isCached = true;
        }
    }

    if(!isCached) {
        // Get sensor senario
        int sensorScenario = (eSTEREO_SENSOR_MAIN1 == sensor)
                             ? StereoSettingProvider::getSensorScenarioMain1()
                             : StereoSettingProvider::getSensorScenarioMain2();

        // Prepare sensor hal
        IHalSensorList* sensorList = MAKE_HalSensorList();
        if(NULL == sensorList) {
            MY_LOGE("Cannot get sensor list");
            return false;
        }

        MINT32 err = 0;
        int32_t main1Idx, main2Idx;
        StereoSettingProvider::getStereoSensorIndex(main1Idx, main2Idx);
        int sendorDevIndex = sensorList->querySensorDevIdx((eSTEREO_SENSOR_MAIN1 == sensor) ? main1Idx : main2Idx);
        IHalSensor* pIHalSensor = sensorList->createSensor(LOG_TAG, (eSTEREO_SENSOR_MAIN1 == sensor) ? main1Idx : main2Idx);
        if(NULL == pIHalSensor) {
            MY_LOGE("Cannot get hal sensor");
            return false;
        }

        //Get pixel mode
        int defaultFPS = 0; //result will be 10xFPS, e.g. if 30 fps, defaultFPS = 300
        err = pIHalSensor->sendCommand(sendorDevIndex, SENSOR_CMD_GET_DEFAULT_FRAME_RATE_BY_SCENARIO,
                                       (MINTPTR)&sensorScenario, (MINTPTR)&defaultFPS, 0);
        if(err) {
            MY_LOGE("Cannot get default frame rate");
            pIHalSensor->destroyInstance(LOG_TAG);
            return false;
        }

        //Get pixel format
        MUINT32 pixelMode;
        defaultFPS /= 10;
        err = pIHalSensor->sendCommand(sendorDevIndex, SENSOR_CMD_GET_SENSOR_PIXELMODE,
                                       (MINTPTR)&sensorScenario, (MINTPTR)&defaultFPS, (MINTPTR)&pixelMode);
        if(err) {
            MY_LOGE("Cannot get pixel mode");
            pIHalSensor->destroyInstance(LOG_TAG);
            return false;
        }

        //Get sensor crop win info
        SensorCropWinInfo rSensorCropInfo;
        err = pIHalSensor->sendCommand(sendorDevIndex, SENSOR_CMD_GET_SENSOR_CROP_WIN_INFO,
                                       (MUINTPTR)&sensorScenario, (MUINTPTR)&rSensorCropInfo, 0);
        if(err) {
            MY_LOGE("Cannot get sensor crop win info");
            return false;
        }

        tgCropRect.p.x = 0;
        tgCropRect.p.y = 0;
        tgCropRect.s.w = rSensorCropInfo.w2_tg_size;
        tgCropRect.s.h = rSensorCropInfo.h2_tg_size;

        outSize.w = rSensorCropInfo.w2_tg_size;
        outSize.h = rSensorCropInfo.h2_tg_size;

        //Calculate TG CROP without FOV cropping
        if(StereoSettingProvider::is3rdParty(scenario) &&
           EPortIndex_RRZO == port)
        {
            CropUtil::cropRectByImageRatio(tgCropRect, currentRatio);
            outSize = (eSTEREO_SENSOR_MAIN1 == sensor) ? __rrzoYuvSize[0] : __rrzoYuvSize[1];
            //Check min RRZO size
            NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryInfo queryRst;
            NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryIn input;
            NSCam::NSIoPipe::NSCamIOPipe::INormalPipeModule::get()->query(
                port,
                NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_BS_RATIO,
                format,
                input,
                queryRst);
            const int RRZ_CAPIBILITY = queryRst.bs_ratio;
            MSize MIN_RRZ_SIZE((tgCropRect.s.w * RRZ_CAPIBILITY/100 + 1)&~1,
                               (tgCropRect.s.h * RRZ_CAPIBILITY/100 + 1)&~1);

            if(outSize.w < MIN_RRZ_SIZE.w ||
               outSize.h < MIN_RRZ_SIZE.h)
            {
                MY_LOGD_IF(LOG_ENABLED, "RRZO %dx%d->%dx%d (limited by RRZ capibility: %.2f)",
                           outSize.w, outSize.h, MIN_RRZ_SIZE.w, MIN_RRZ_SIZE.h,
                           RRZ_CAPIBILITY/100.0f);
                outSize = MIN_RRZ_SIZE;
            }

            MY_LOGD_IF(LOG_ENABLED, "3rdParty RRZO size=%dx%d,sensor=%d",outSize.w,outSize.h,sensor);
        }
        else if( !StereoSettingProvider::isDeNoise() &&
                EPortIndex_RRZO == port )
        {
            float CROP_RATIO = (eSTEREO_SENSOR_MAIN1 == sensor)
                                ? StereoSettingProvider::getMain1FOVCropRatio()
                                : StereoSettingProvider::getMain2FOVCropRatio();
            if(CROP_RATIO < 1.0f &&
               CROP_RATIO > 0.0f)
            {
                CropUtil::cropRectByFOV(tgCropRect, CROP_RATIO, currentRatio);
                MY_LOGD_IF(LOG_ENABLED, "Crop TG by FOV ratio %.2f, image ratio: %d:%d, result: tg (%d, %d) %dx%d",
                                        CROP_RATIO, m, n, tgCropRect.p.x, tgCropRect.p.y, tgCropRect.s.w, tgCropRect.s.h);
            }

            CropUtil::cropRectByImageRatio(tgCropRect, currentRatio);
            MY_LOGD_IF(LOG_ENABLED, "Crop TG by image ratio %d:%d, result: offset (%d, %d) size %dx%d",
                                    m, n, tgCropRect.p.x, tgCropRect.p.y, tgCropRect.s.w, tgCropRect.s.h);

            //Make sure output size meet the following rules:
            // 1. image ratio fit 4:3 or 16:9
            // 2. smaller than TG size
            // 3. Is even number
            outSize = tgCropRect.s;
            int baseSize = std::min(outSize.w/(m*2), outSize.h/(n*2));
            outSize.w = baseSize * m * 2;
            outSize.h = baseSize * n * 2;

            //Use smaller size for RRZO if possible
            Pass2SizeInfo p2SizeInfo;
            MSize p2SizeNeeded;

            //RRZO is for preview/record only, if scenario is capture, query by preview
            ENUM_STEREO_SCENARIO queryScenario = (eSTEREO_SCENARIO_CAPTURE == scenario)
                                                 ? eSTEREO_SCENARIO_PREVIEW
                                                 : scenario;
            if(eSTEREO_SENSOR_MAIN1 == sensor) {
                //Compare with PASS2A.WDMA (Preview size)
                Pass2SizeInfo p2SizeInfo;
                getPass2SizeInfo(PASS2A, queryScenario, p2SizeInfo);
                p2SizeNeeded = p2SizeInfo.areaWDMA;
            } else {
                //Compare with PASS2A_P.WROT (FE input size)
                Pass2SizeInfo p2APSizeInfo;
                Pass2SizeInfo p2AP2SizeInfo;
                getPass2SizeInfo(PASS2A_P, queryScenario, p2APSizeInfo);
                getPass2SizeInfo(PASS2A_P_2, queryScenario, p2AP2SizeInfo);
                MSize p2aWROTSize = p2APSizeInfo.areaWROT;
                MSize p2ap2WDMASize = p2AP2SizeInfo.areaWDMA;
                if(StereoSettingProvider::getModuleRotation() & 0x2) {
                    std::swap(p2aWROTSize.w, p2aWROTSize.h);
                    std::swap(p2ap2WDMASize.w, p2ap2WDMASize.h);
                }

                if(p2aWROTSize.w > p2ap2WDMASize.w) {
                    p2SizeNeeded = p2aWROTSize;
                } else {
                    p2SizeNeeded = p2ap2WDMASize;
                }

                //Make sure RRZO output fit ratio
                baseSize = std::ceil(std::max(p2SizeNeeded.w/(m*2.0f), p2SizeNeeded.h/(n*2.0f)));
                p2SizeNeeded.w = baseSize * m * 2;
                p2SizeNeeded.h = baseSize * n * 2;
            }

            NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryInfo queryRst;
            NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryIn input;
            NSCam::NSIoPipe::NSCamIOPipe::INormalPipeModule::get()->query(
                port,
                NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_BS_RATIO,
                format,
                input,
                queryRst);
            const int RRZ_CAPIBILITY = queryRst.bs_ratio;
            MSize MIN_RRZ_SIZE((tgCropRect.s.w * RRZ_CAPIBILITY/100 + 1)&~1,
                               (tgCropRect.s.h * RRZ_CAPIBILITY/100 + 1)&~1);

            if(p2SizeNeeded.w < MIN_RRZ_SIZE.w ||
               p2SizeNeeded.h < MIN_RRZ_SIZE.h)
            {
                p2SizeNeeded = MIN_RRZ_SIZE;
                MY_LOGD_IF(LOG_ENABLED, "RRZO %dx%d->%dx%d(%d%%) (scenario %d, limited by: RRZ capibility %.2f)",
                       outSize.w, outSize.h, p2SizeNeeded.w, p2SizeNeeded.h,
                       (int)(p2SizeNeeded.w*p2SizeNeeded.h*100.0f/(outSize.w*outSize.h)),
                       queryScenario, RRZ_CAPIBILITY/100.0f);
            }
            else if(p2SizeNeeded.w > outSize.w ||
                    p2SizeNeeded.h > outSize.h)
            {
                p2SizeNeeded = outSize;
                MY_LOGD_IF(LOG_ENABLED, "RRZO %dx%d->%dx%d(%d%%) (scenario %d, limited by: RRZO cannot scale up)",
                       outSize.w, outSize.h, p2SizeNeeded.w, p2SizeNeeded.h,
                       (int)(p2SizeNeeded.w*p2SizeNeeded.h*100.0f/(outSize.w*outSize.h)), queryScenario);
            } else {
                MY_LOGD_IF(LOG_ENABLED, "RRZO %dx%d->%dx%d(%d%%) (scenario %d)",
                       outSize.w, outSize.h, p2SizeNeeded.w, p2SizeNeeded.h,
                       (int)(p2SizeNeeded.w*p2SizeNeeded.h*100.0f/(outSize.w*outSize.h)),
                       queryScenario);
            }

            if(p2SizeNeeded.w < outSize.w &&
               p2SizeNeeded.h < outSize.h )
            {
                outSize = p2SizeNeeded;
            }
        }
        else if( EPortIndex_CRZO_R2 == port ) //P1 YUV
        {
            float CROP_RATIO = (eSTEREO_SENSOR_MAIN1 == sensor)
                                ? StereoSettingProvider::getMain1FOVCropRatio()
                                : StereoSettingProvider::getMain2FOVCropRatio();
            if(CROP_RATIO < 1.0f &&
               CROP_RATIO > 0.0f)
            {
                CropUtil::cropRectByFOV(tgCropRect, CROP_RATIO, currentRatio);
                MY_LOGD_IF(LOG_ENABLED, "Crop TG by FOV ratio %.2f, image ratio: %d:%d, result: tg (%d, %d) %dx%d",
                                        CROP_RATIO, m, n, tgCropRect.p.x, tgCropRect.p.y, tgCropRect.s.w, tgCropRect.s.h);
            }

            CropUtil::cropRectByImageRatio(tgCropRect, currentRatio);
            MY_LOGD_IF(LOG_ENABLED, "Crop TG by image ratio %d:%d, result: offset (%d, %d) size %dx%d",
                                    m, n, tgCropRect.p.x, tgCropRect.p.y, tgCropRect.s.w, tgCropRect.s.h);

            //Make sure output size meet the following rules:
            // 1. image ratio fit 4:3 or 16:9
            // 2. smaller than TG size
            // 3. Is even number
            outSize = tgCropRect.s;
            int baseSize = std::min(outSize.w/(m*2), outSize.h/(n*2));
            outSize.w = baseSize * m * 2;
            outSize.h = baseSize * n * 2;

            //P1 YUV is used only for preview
            StereoArea outputSize = getBufferSize(E_RECT_IN_M, eSTEREO_SCENARIO_PREVIEW);
            outputSize.rotatedByModule();   //P1 YUV does not support rotation
            if(outputSize.size.w < outSize.w ||
               outputSize.size.h < outSize.h)
            {
                outSize = outputSize.size;
            }
        }

        NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryInfo queryRst;
        NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryIn input;
        input.width = outSize.w;
        input.pixMode = NSCam::NSIoPipe::NSCamIOPipe::ePixMode_NONE; //? (NSCam::NSIoPipe::NSCamIOPipe::_1_PIX_MODE) : NSCam::NSIoPipe::NSCamIOPipe::_2_PIX_MODE;
        NSCam::NSIoPipe::NSCamIOPipe::INormalPipeModule::get()->query(
                port,
                NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_X_PIX|
                NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_STRIDE_BYTE,
                format,
                input,
                queryRst);

        strideInBytes = queryRst.stride_byte;
        outSize.w = queryRst.x_pix;
        if(EPortIndex_IMGO != port &&
           outSize.w != queryRst.x_pix)
        {
            //width/height may be limited by hardware, so height/width needs to update as well

            // Apply 2-align
            outSize.w &= ~0x1;
            outSize.h &= ~0x1;

            // Make sure all sizes <= IMGO size
            outSize.w = std::min((int)rSensorCropInfo.w2_tg_size, outSize.w);
            outSize.h = std::min((int)rSensorCropInfo.h2_tg_size, outSize.h);

            // Keep flexibility for 3rd party to config sizes
            if(!StereoSettingProvider::is3rdParty(scenario))
            {
                if(m * 3 / 4 >= n) {
                    outSize.h = outSize.w * n / m;
                } else {
                    outSize.w = outSize.h * m / n;
                }
            }
        }

        pIHalSensor->destroyInstance(LOG_TAG);

        {
            Mutex::Autolock lock(__P1Lock);
            __pass1TgCropRectMap[key] = tgCropRect;
            __pass1OutSizeMap[key]    = outSize;
            __pass1StrideMap[key]     = strideInBytes;
        }
    }

    MY_LOGD_IF(LOG_ENABLED, "%s result of (sensor %d, port %s, format %d, sensor scenario %d, ratio %d:%d, logical device %d, feature 0x%X(portrait %d)): tg (%d,%d)(%dx%d), outSize %dx%d, stride %d",
                            (isCached)?"Use":"New",
                            sensor-1, (EPortIndex_RRZO == port) ? "RRZO" : ((EPortIndex_IMGO == port) ? "IMGO" : "P1YUV(CRZO_R2)"), format, sensorScenario, m, n,
                            StereoSettingProvider::getLogicalDeviceID(),
                            featureMode, isPortraitMode,
                            tgCropRect.p.x, tgCropRect.p.y, tgCropRect.s.w, tgCropRect.s.h, outSize.w, outSize.h, strideInBytes);

    return true;
}

template <typename T>
inline MBOOL
tryGetMetadata(
    IMetadata const* pMetadata,
    MUINT32 const tag,
    T & rVal
)
{
    if (pMetadata == NULL) {
        MY_LOGW("pMetadata == NULL");
        return MFALSE;
    }
    //
    IMetadata::IEntry entry = pMetadata->entryFor(tag);
    if(!entry.isEmpty()) {
        rVal = entry.itemAt(0, Type2Type<T>());
        return MTRUE;
    }
    //
    return MFALSE;
}

bool
StereoSizeProvider::getPass1ActiveArrayCrop(ENUM_STEREO_SENSOR sensor, MRect &cropRect, bool isCropNeeded)
{
    enum {
        SENSOR_BITS          = 6,
        RATIO_BITS           = 3,
        NEED_CROP_BITS       = 1,
    };

    int main1Id, main2Id;
    if(!StereoSettingProvider::getStereoSensorIndex(main1Id, main2Id))
    {
        MY_LOGE("Cannot get sensor index");
        return false;
    }

    int sensorId = (eSTEREO_SENSOR_MAIN1 == sensor) ? main1Id : main2Id;
    MUINT32 key = sensorId;
    ENUM_STEREO_RATIO imageRatio = StereoSettingProvider::imageRatio();
    int shiftBits = SENSOR_BITS;
    key |= imageRatio << shiftBits;
    shiftBits += RATIO_BITS;
    key |= isCropNeeded << shiftBits;

    bool isCached = false;
    {
        Mutex::Autolock lock(__activeCropLock);
        if(__activeCropMap.find(key) != __activeCropMap.end()) {
            cropRect = __activeCropMap.find(key)->second;
            isCached = true;
        }
    }

    if(!isCached)
    {
        // Prepare sensor hal
        IHalSensorList* pSensorList = MAKE_HalSensorList();
        if(NULL == pSensorList) {
            MY_LOGE("Cannot get sensor list");
            return false;
        }

        int sendorDevIndex = pSensorList->querySensorDevIdx(sensorId);
        SensorStaticInfo sensorStaticInfo;
        memset(&sensorStaticInfo, 0, sizeof(SensorStaticInfo));
        pSensorList->querySensorStaticInfo(sendorDevIndex, &sensorStaticInfo);

        cropRect.p = MPOINT_ZERO;
        cropRect.s = MSize(sensorStaticInfo.captureWidth, sensorStaticInfo.captureHeight);

        if(isCropNeeded)
        {
            // Use preview to query since capture should align preview's setting
            if(!StereoSettingProvider::is3rdParty(eSTEREO_SCENARIO_PREVIEW)) {
                float CROP_RATIO = (eSTEREO_SENSOR_MAIN1 == sensor)
                                    ? StereoSettingProvider::getMain1FOVCropRatio()
                                    : StereoSettingProvider::getMain2FOVCropRatio();
                if(CROP_RATIO < 1.0f &&
                   CROP_RATIO > 0.0f)
                {
                    CropUtil::cropRectByFOV(cropRect, CROP_RATIO, imageRatio);
                }
            }
            CropUtil::cropRectByImageRatio(cropRect, imageRatio);
        }

        {
            Mutex::Autolock lock(__activeCropLock);
            __activeCropMap[key] = cropRect;
        }
    }

    if(LOG_ENABLED)
    {
        int m, n;
        imageRatioMToN(imageRatio, m, n);
        MY_LOGD("Active domain for sensor %d(%s): offset (%d, %d), size %dx%d, ratio %d:%d, is crop %d(cache %d)",
                sensorId, (eSTEREO_SENSOR_MAIN1 == sensor) ? "Main1" : "Main2",
                cropRect.p.x, cropRect.p.y, cropRect.s.w, cropRect.s.h, m, n, isCropNeeded, isCached);
    }

    return true;
}

bool
StereoSizeProvider::getPass2SizeInfo(ENUM_PASS2_ROUND round, ENUM_STEREO_SCENARIO eScenario, Pass2SizeInfo &pass2SizeInfo) const
{
    bool isSuccess = true;
    switch(round) {
        case PASS2A:
            pass2SizeInfo = Pass2A_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_2:
            pass2SizeInfo = Pass2A_2_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_3:
            pass2SizeInfo = Pass2A_3_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_P:
            pass2SizeInfo = Pass2A_P_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_P_2:
            pass2SizeInfo = Pass2A_P_2_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_P_3:
            pass2SizeInfo = Pass2A_P_3_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_B:
            pass2SizeInfo = Pass2A_B_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_CROP:
            pass2SizeInfo = Pass2A_Crop_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_B_CROP:
            pass2SizeInfo = Pass2A_B_Crop_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_P_CROP:
            pass2SizeInfo = Pass2A_P_Crop_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        default:
            isSuccess = false;
    }

    return isSuccess;
}

#if (1==HAS_AIDEPTH)
MSize getAIDepthModelSize()
{
    MSize result(896, 512);
    MTKAIDepth *pAIDepth = MTKAIDepth::createInstance(DRV_AIDEPTH_OBJ_SW);
    if(pAIDepth) {
        AIDepthModelInfo info;
#if 0
        if(StereoSettingProvider::getModuleRotation()&0x2)
        {
            if(eRatio_4_3 == StereoSettingProvider::imageRatio())
            {
                pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_3_4, (void *)&info);
            }
            else
            {
                pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_9_16, (void *)&info);
            }
            result.w = info.modelHeight;
            result.h = info.modelWidth;
        }
        else
        {
            if(eRatio_4_3 == StereoSettingProvider::imageRatio())
            {
                pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_4_3, (void *)&info);
            }
            else
            {
                pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_16_9, (void *)&info);
            }
            result.w = info.modelWidth;
            result.h = info.modelHeight;
        }
#else
        pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_9_16, (void *)&info);
        result.w = info.modelHeight;
        result.h = info.modelWidth;
#endif
        pAIDepth->AIDepthReset();
        pAIDepth->destroyInstance(pAIDepth);
        pAIDepth = NULL;
    }

    return result;
}

MSize getVideoAIDepthModelSize()
{
    MSize result(896, 512);
    MTKVideoAIDepth *pAIDepth = MTKVideoAIDepth::createInstance(DRV_VIDEOAIDEPTH_OBJ_SW);
    if(pAIDepth) {
        VideoAIDepthModelInfo info;
#if 0
        if(StereoSettingProvider::getModuleRotation()&0x2)
        {
            if(eRatio_4_3 == StereoSettingProvider::imageRatio())
            {
                pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_3_4, (void *)&info);
            }
            else
            {
                pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_9_16, (void *)&info);
            }
            result.w = info.modelHeight;
            result.h = info.modelWidth;
        }
        else
        {
            if(eRatio_4_3 == StereoSettingProvider::imageRatio())
            {
                pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_4_3, (void *)&info);
            }
            else
            {
                pAIDepth->AIDepthGetModelInfo(AIDEPTH_FEATURE_16_9, (void *)&info);
            }
            result.w = info.modelWidth;
            result.h = info.modelHeight;
        }
#else
        pAIDepth->VideoAIDepthGetModelInfo(AIDEPTH_FEATURE_9_16, (void *)&info);
        result.w = info.modelHeight;
        result.h = info.modelWidth;
#endif
        pAIDepth->VideoAIDepthReset();
        pAIDepth->destroyInstance(pAIDepth);
        pAIDepth = NULL;
    }

    return result;
}
#endif

StereoArea
StereoSizeProvider::getBufferSize(ENUM_BUFFER_NAME eName,
                                  ENUM_STEREO_SCENARIO eScenario,
                                  ENUM_ROTATION capOrientation)
{
#if (1 == STEREO_DENOISE_SUPPORTED)
    const bool IS_DENOISE_CAPTURE = (StereoSettingProvider::isDeNoise()
                                     && DENOISE_MODE == E_DENOISE_MODE_NORMAL
                                     && eScenario == eSTEREO_SCENARIO_CAPTURE);

    const int PADDING = 16;
#endif

    const StereoSizeConfig CONFIG_WO_PADDING_WO_ROTATE(STEREO_AREA_WO_PADDING, STEREO_AREA_WO_ROTATE);
    const StereoSizeConfig CONFIG_WO_PADDING_W_ROTATE(STEREO_AREA_WO_PADDING, STEREO_AREA_W_ROTATE);
    const StereoSizeConfig CONFIG_W_PADDING_W_ROTATE(STEREO_AREA_W_PADDING, STEREO_AREA_W_ROTATE);

    auto __refineDLDP2Size = [&](StereoArea &area)
    {
        StereoArea MAX_AREA = StereoArea(1920, 1080).applyRatio(StereoSettingProvider::imageRatio(), E_KEEP_AREA_SIZE);
        if(area.size.w > MAX_AREA.size.w ||
           area.size.h > MAX_AREA.size.h)
        {
            MY_LOGW("Customized size > limitation, change from %dx%d to %dx%d",
                    area.size.w, area.size.h, MAX_AREA.size.w, MAX_AREA.size.h);
            area = MAX_AREA;
        } else {
            //Get FEFM input size(WROT)
            Pass2SizeInfo p2SizeInfo;
            getPass2SizeInfo(PASS2A, eSTEREO_SCENARIO_CAPTURE, p2SizeInfo);
            p2SizeInfo.areaWROT.rotatedByModule();
            if(area.size.w < p2SizeInfo.areaWROT.size.w ||
               area.size.h < p2SizeInfo.areaWROT.size.h)
            {
                MY_LOGW("Customized size < limitation, change from %dx%d to %dx%d",
                        area.size.w, area.size.h, p2SizeInfo.areaWROT.size.w, p2SizeInfo.areaWROT.size.h);
                area = p2SizeInfo.areaWROT;
            }
        }
    };

    switch(eName) {
        //P2A output for DL-Depth
        case E_DLD_P2A_M:
            {
                //Rule:
                //1. Max: 1920x1080
                //2. Min: FEFM input size
                //3. Can beCustomized in setting
                //4. Default value is decided by experiences(in thirdPartyDepthmapSize)
                StereoArea area = thirdPartyDepthmapSize(StereoSettingProvider::imageRatio(), eScenario);
                __refineDLDP2Size(area);
                return area.apply64Align();
            }
            break;
        case E_DLD_P2A_S:
            {
                StereoArea area = getBufferSize(E_DLD_P2A_M);
                //Get FOV Cropped size
                area *= StereoSettingProvider::getStereoCameraFOVRatio();
                __refineDLDP2Size(area);
                return area.apply64Align();
            }
            break;

        //DL Depth
        case E_DL_DEPTHMAP:
            {
                StereoArea area = thirdPartyDepthmapSize(StereoSettingProvider::imageRatio(), eScenario);
                return area.rotatedByModule();
            }
            break;

#if (1==HAS_AIDEPTH)
        case E_VAIDEPTH_DEPTHMAP:
            return StereoArea(getVideoAIDepthModelSize()).rotatedByModule();
            break;
#endif

        case E_MASK_IN_M:
        case E_RECT_IN_M:
#if (0==HAS_HW_DPE2)
            {
                StereoArea area;
                if(eSTEREO_SCENARIO_CAPTURE == eScenario) {
                    area = StereoSize::getStereoArea2x(CONFIG_WO_PADDING_W_ROTATE);
                } else {
                    area = StereoSize::getStereoArea1x(CONFIG_WO_PADDING_W_ROTATE);
                    if(StereoSettingProvider::getStereoBaseline() <= DOUBLE_WIDTH_THRESHOLD) {
                        area.applyDoubleWidth();
                    }
                }

                area.apply32Align(true, false);    //For GPU
                return area;
            }
#else
            if(eSTEREO_SCENARIO_CAPTURE == eScenario) {
#if (1==HAS_AIDEPTH)
                return StereoArea(getAIDepthModelSize()).rotatedByModule();
#else
                StereoArea area = StereoSize::getStereoArea2x(CONFIG_WO_PADDING_W_ROTATE);
                area.apply32Align(true, false);    //For GPU
                return area;
#endif
            } else {
                return StereoSize::getStereoArea1x(CONFIG_WO_PADDING_W_ROTATE);
            }
#endif
            break;
        case E_MASK_IN_S:
#if (0==HAS_WPE || 0==HAS_HW_DPE2)
            return getBufferSize(E_MASK_IN_M, eScenario);
#endif
        case E_RECT_IN_S:
            {
                StereoArea area = getBufferSize(E_RECT_IN_M, eScenario);
                if(StereoSettingProvider::isActiveStereo())
                {
                    return area;
                }

                area.removePadding();

                //For quality enhancement
                const float WARPING_IMAGE_RATIO = 1.0f; //For better quality
                const float FOV_RATIO           = StereoSettingProvider::getStereoCameraFOVRatio();
                const float IMAGE_RATIO         = WARPING_IMAGE_RATIO * FOV_RATIO;
                area.enlargeWith2AlignedRounding(IMAGE_RATIO);

                if(eSTEREO_SCENARIO_CAPTURE != eScenario) {
#if (0==HAS_WPE || 0==HAS_HW_DPE2)
                    //Work around for GPU: input size must >= output size
                    MSize outputSize = getBufferSize(E_MV_Y, eScenario);
                    if(area.size.w < outputSize.w ||
                       area.size.h < outputSize.h)
                    {
                        MSize contentSize = area.size;
                        area.size.w = std::max(outputSize.w, contentSize.w);
                        area.size.h = std::max(outputSize.h, contentSize.h);
                        area.padding = area.size - contentSize;
                    }

                    area.apply32Align(true, false); //For GPU
#else
                    area.apply64Align(true, false); //For WPE
#endif

                } else {
                    area.apply64Align(false);       //For AIDepth
                }

                return area;
            }
            break;

        //N3D before MDP for capture
        case E_MV_Y_LARGE:
        case E_MASK_M_Y_LARGE:
        case E_SV_Y_LARGE:
        case E_MASK_S_Y_LARGE:
            //Denoise capture size will align DPE input size(E_DMP_H)
#if (1 == STEREO_DENOISE_SUPPORTED)
            if(!IS_DENOISE_CAPTURE)
#endif
            {
                StereoArea area = StereoSize::getStereoArea2x(CONFIG_W_PADDING_W_ROTATE);
                area.apply32Align(true);    //For GPU

                return area;
            }   //no need break here since denoise has only one kind of size

        //N3D Output
        case E_LDC:
            if(eScenario != eSTEREO_SCENARIO_CAPTURE) {
                return StereoSize::getStereoArea1x(CONFIG_W_PADDING_W_ROTATE);
            }   //no need break here
        case E_MV_Y:
        case E_MASK_M_Y:
#if (1==HAS_AIDEPTH)
            if(eScenario == eSTEREO_SCENARIO_CAPTURE)
            {
                return getBufferSize(E_RECT_IN_M, eScenario);
            }   // no need break here
#endif
        case E_SV_Y:
        case E_MASK_S_Y:
            {
#if (1==HAS_AIDEPTH)
                if(eScenario == eSTEREO_SCENARIO_CAPTURE)
                {
                    return getBufferSize(E_RECT_IN_M, eScenario);
                }   // no need break here
#endif
                StereoArea area = StereoSize::getStereoArea1x(CONFIG_W_PADDING_W_ROTATE);
                if(eScenario == eSTEREO_SCENARIO_CAPTURE) {
                    area *= 2;
                    // area.apply32Align();    //For GPU
                }
#if (0==HAS_HW_DPE2)
                else if(StereoSettingProvider::getStereoBaseline() <= DOUBLE_WIDTH_THRESHOLD)
                {
                    area.applyDoubleWidth();
                }

                //1. For GPU, must be 32-pixel aligned
                area.apply32Align(true);
#else
                //2. For WPE, image block has better performance if 64 bytes-aligned
                area.apply64Align(true);
#endif

                return area;
            }
        break;

        //DPE Output
        case E_DMP_H:
        case E_CFM_H:
        case E_RESPO:
#if (0==HAS_HW_DPE2)
            return getBufferSize(E_MV_Y, eScenario);
#else
        case E_DV_LR:   //In DPE2
            {
                StereoArea area = StereoSize::getStereoArea1x(CONFIG_W_PADDING_W_ROTATE);
                area.apply128AlignToWidth(true, false);

                if(E_DV_LR == eName) {
                    area.applyDoubleHeight();
                }

                return area;
            }
#endif
            break;

        //For WPE
        case E_WARP_MAP_M:
        case E_WARP_MAP_S:
            {
                StereoArea area = getBufferSize(E_SV_Y, eScenario);
                //Only keep size
                area.padding = MSIZE_ZERO;
                area.startPt = MPOINT_ZERO;
                area.rotatedByModule();
                area /= 2;

                //Check hardware ability
                if(area.size.w > WPE_MAX_SIZE.w ||
                   area.size.h > WPE_MAX_SIZE.h)
                {
                    area = StereoArea(WPE_MAX_SIZE);
                    ENUM_STEREO_RATIO ratio = StereoSettingProvider::imageRatio();
                    int m, n;
                    imageRatioMToN(ratio, m, n);

                    if(m*WPE_MAX_SIZE.h/WPE_MAX_SIZE.w >= n) {
                        area.applyRatio(StereoSettingProvider::imageRatio(), E_KEEP_WIDTH);
                    } else {
                        area.applyRatio(StereoSettingProvider::imageRatio(), E_KEEP_HEIGHT);
                    }
                }

                //1. WPE has better performance if 128 byte aligned, and WARP_MAP is 4 bytes/pixel
                //2. GPU must be 32 pixel aligned
                return area.rotatedByModule().apply32Align();
            }
            break;

        case E_SV_CC_Y:
        case E_PRE_MASK_S_Y:
            {
                Pass2SizeInfo pass2SizeInfo;
                getInstance()->getPass2SizeInfo(PASS2A_P_2, eScenario, pass2SizeInfo);
                return pass2SizeInfo.areaWDMA;
            }
            break;

        //OCC Output
        case E_MY_S:
            if(!StereoSettingProvider::isMTKDepthmapMode()) {
                StereoArea area = StereoSize::getStereoArea1x(CONFIG_WO_PADDING_W_ROTATE);
                int factor = 1;
                if(eSTEREO_SCENARIO_CAPTURE == eScenario) {
                    factor *= 2;
                }

                if(E_WMF_Y_CHANNEL != StereoTuningProvider::getWMFInputChannel()) {
                    factor *= 2;
                }

                if(factor > 1) {
                    area *= factor;
                }

                return area;
            }   //no need break here for mtk size
        case E_DMH:
        case E_OCC:
        //DPE2 output
        case E_NOC:
        case E_CFM_M:
        case E_ASF_CRM:
        case E_ASF_RD:
        case E_ASF_HF:
        //WMF Output
        case E_DMW:
            {
                StereoArea area = StereoSize::getStereoArea1x(CONFIG_WO_PADDING_W_ROTATE);
                if(E_ASF_CRM == eName) {
                    area.size.h *= 4;
                }

#if (0==HAS_HW_DPE2)
                if(StereoSettingProvider::isMTKDepthmapMode()) {
                    area.applyDoubleWidth();
                }
#else
                area.apply128AlignToWidth(true, false);
#endif

                return area;
            }
            break;
        case E_BOKEH_PACKED_BUFFER:
            {
                StereoArea area = getBufferSize(E_DMW, eScenario);
                if(capOrientation & 0x2) {  //90 or 270 degree
                    MSize::value_type s = area.size.w;
                    area.size.w = area.size.h;
                    area.size.h = s;
                }

                area.size.h *= 4;
                return area;
            }
            break;
        //GF Input
        case E_GF_IN_IMG_4X:
            {
                return StereoSize::getStereoArea4x(CONFIG_WO_PADDING_W_ROTATE);
            }
            break;

        case E_GF_IN_IMG_2X:
            {
                return StereoSize::getStereoArea2x(CONFIG_WO_PADDING_W_ROTATE);
            }
            break;

        //GF Output
        case E_DMG:
        case E_DMBG:
            {
                StereoArea area = StereoSize::getStereoArea1x(CONFIG_WO_PADDING_WO_ROTATE);
                if(eSTEREO_SCENARIO_CAPTURE == eScenario) {
                    area *= 2;
                }
#if (1==HAS_HW_DPE2)
                area /= 2;
#endif

                return area;
            }
            break;

        case E_DEPTH_MAP:
#if (1==HAS_AIDEPTH)
            if(eSTEREO_SCENARIO_CAPTURE == eScenario) {
                return getBufferSize(E_DL_DEPTHMAP, eScenario);
            }
#endif
            if(StereoSettingProvider::is3rdParty(eScenario)){
                return thirdPartyDepthmapSize(StereoSettingProvider::imageRatio());
            }
            else if(StereoSettingProvider::isMTKDepthmapMode())
            {
#if (0==HAS_HW_DPE2)
                return getBufferSize(E_OCC, eScenario);
#else
                return StereoSize::getStereoArea1x(CONFIG_WO_PADDING_W_ROTATE);
#endif
            }
            else
            {
                return getBufferSize(E_DMG, eScenario);
            }
            break;

        case E_INK:
            return StereoSize::getStereoArea2x(CONFIG_WO_PADDING_WO_ROTATE);
            break;

        //Bokeh Output
        case E_BOKEH_WROT: //Saved image
            switch(eScenario) {
                case eSTEREO_SCENARIO_PREVIEW:
                    return STEREO_AREA_ZERO;
                    break;
                case eSTEREO_SCENARIO_RECORD:
                    {
                        Pass2SizeInfo pass2SizeInfo;
                        getInstance()->getPass2SizeInfo(PASS2A, eScenario, pass2SizeInfo);
                        return pass2SizeInfo.areaWDMA;
                    }
                    break;
                case eSTEREO_SCENARIO_CAPTURE:
                    {
                        return StereoArea(__captureSize);
                    }
                    break;
                default:
                    break;
            }
            break;
        case E_BOKEH_WDMA:
            switch(eScenario) {
                case eSTEREO_SCENARIO_PREVIEW:  //Display
                case eSTEREO_SCENARIO_RECORD:
                    {
                        Pass2SizeInfo pass2SizeInfo;
                        getInstance()->getPass2SizeInfo(PASS2A, eScenario, pass2SizeInfo);
                        return pass2SizeInfo.areaWDMA;
                    }
                    break;
                case eSTEREO_SCENARIO_CAPTURE:  //Clean image
                    {
                        return StereoArea(__captureSize);
                    }
                    break;
                default:
                    break;
            }
            break;
        case E_BOKEH_3DNR:
            switch(eScenario) {
                case eSTEREO_SCENARIO_PREVIEW:
                case eSTEREO_SCENARIO_RECORD:
                    {
                        Pass2SizeInfo pass2SizeInfo;
                        getInstance()->getPass2SizeInfo(PASS2A, eScenario, pass2SizeInfo);
                        return pass2SizeInfo.areaWDMA;
                    }
                    break;
                default:
                    break;
            }
            break;
#if (1 == STEREO_DENOISE_SUPPORTED)
        case E_BM_PREPROCESS_FULLRAW_CROP_1:
            return StereoArea(__BMDeNoiseFullRawCropSize_main1.s.w, __BMDeNoiseFullRawCropSize_main1.s.h,0,0,__BMDeNoiseFullRawCropSize_main1.p.x,__BMDeNoiseFullRawCropSize_main1.p.y);
        case E_BM_PREPROCESS_FULLRAW_CROP_2:
            return StereoArea(__BMDeNoiseFullRawCropSize_main2.s.w, __BMDeNoiseFullRawCropSize_main2.s.h,0,0,__BMDeNoiseFullRawCropSize_main2.p.x,__BMDeNoiseFullRawCropSize_main2.p.y);
        case E_BM_PREPROCESS_FUSION_OUT:
        case E_BM_PREPROCESS_OUT_YUV_IMG3O:
        case E_BM_PREPROCESS_MFBO_1:
        case E_BM_PREPROCESS_OUT_W_1_IMG3O:
        case E_BM_PREPROCESS_OUT_MFBO:
            return StereoArea(__BMDeNoiseFullRawSize_main1);
        case E_BM_PREPROCESS_MFBO_2:
            return StereoArea(__BMDeNoiseFullRawSize_main2);
        case E_BM_PREPROCESS_MFBO_FINAL_1:  //bayer raw
            return StereoArea(__BMDeNoiseFullRawSize_main1.w+PADDING*2, __BMDeNoiseFullRawSize_main1.h+PADDING*2, PADDING*2, PADDING*2, 0, 0)
                   .rotatedByModule(false);
        case E_BM_PREPROCESS_MFBO_FINAL_2:  //mono raw
            {
                // first we get a 32-align buffer size
                StereoArea temp(__BMDeNoiseFullRawSize_main2.w, __BMDeNoiseFullRawSize_main2.h);
                temp.rotatedByModule(false);

                MUINT32 contentWidth;
                MUINT32 contentHeight;
                contentWidth = temp.size.w;
                contentHeight = temp.size.h;

                temp.apply32AlignToWidth();

                // then return an area with dummy pixels in padding
                return StereoArea(temp.size.w, temp.size.h,
                                  temp.size.w-contentWidth, temp.size.h-contentHeight, 0, 0);
            }
        case E_BM_PREPROCESS_W_1:   //bayerW
            return getBufferSize(E_BM_PREPROCESS_MFBO_FINAL_1);
        case E_BM_PREPROCESS_W_2:
            // W2 is not used, just allocate a small dummy buffer for pass2 to work normally
            return StereoArea(160, 90)
                   .applyRatio(StereoSettingProvider::imageRatio(), E_KEEP_WIDTH)
                   .rotatedByModule(false);
        case E_BM_DENOISE_HAL_OUT:  //output
            return getBufferSize(E_BM_PREPROCESS_MFBO_FINAL_1);
        case E_BM_DENOISE_FINAL_RESULT:
        case E_BM_DENOISE_SWNR_IN_YV12:
            return StereoArea(__BMDeNoiseAlgoSize_main1);
        case E_BM_PREPROCESS_OUT_YUV:
        case E_BM_DENOISE_HAL_OUT_ROT_BACK:
        case E_BM_DENOISE_HAL_OUT_ROT_BACK_IMG3O:
            return StereoArea(__BMDeNoiseFullRawSize_main1);
        case E_BM_PREPROCESS_SMALL_YUV:
            return StereoArea(__BMDeNoiseFullRawSize_main2.w/2, __BMDeNoiseFullRawSize_main2.h/2);
#endif
        default:
            break;
    }

    return STEREO_AREA_ZERO;
}

MSize
StereoSizeProvider::getSBSImageSize()
{
    Pass2SizeInfo pass2SizeInfo;
    getPass2SizeInfo(PASS2A_2, eSTEREO_SCENARIO_CAPTURE, pass2SizeInfo);
    MSize result = pass2SizeInfo.areaWDMA.size;
    result.w *= 2;

    return result;
}

void
StereoSizeProvider::setCaptureImageSize(int w, int h)
{
    __captureSize.w = w;
    __captureSize.h = h;
    MY_LOGD("Set capture size %dx%d", w, h);
}

bool
StereoSizeProvider::getcustomYUVSize( ENUM_STEREO_SENSOR sensor,
                                      EPortIndex port,
                                      MSize &outSize
                                    )
{
    outSize = MSIZE_ZERO;
    if( EPortIndex_IMGO == port)
    {
        outSize = (eSTEREO_SENSOR_MAIN1 == sensor) ? __imgoYuvSize[0] : __imgoYuvSize[1];

        if(0 == outSize.w ||
           0 == outSize.h)
        {
            Pass2SizeInfo p2SizeInfo;
            ENUM_PASS2_ROUND queryRound = (eSTEREO_SENSOR_MAIN1 == sensor) ? PASS2A_CROP : PASS2A_P_CROP;
            getPass2SizeInfo(queryRound, eSTEREO_SCENARIO_CAPTURE, p2SizeInfo);
            outSize = p2SizeInfo.areaWDMA.contentSize();
        }

        //apply 4 align for MFLL
        StereoHAL::applyNAlign(4, outSize.w);
        StereoHAL::applyNAlign(4, outSize.h);
    }
    else
    {
        //Use preview to query since RRZO is for preview
        if(StereoSettingProvider::is3rdParty(eSTEREO_SCENARIO_PREVIEW)) {
            outSize = (eSTEREO_SENSOR_MAIN1 == sensor) ? __rrzoYuvSize[0] : __rrzoYuvSize[1];

            if(0 == outSize.w ||
               0 == outSize.h)
            {
                Pass2SizeInfo p2SizeInfo;
                ENUM_PASS2_ROUND queryRound = (eSTEREO_SENSOR_MAIN1 == sensor) ? PASS2A_CROP : PASS2A_P_CROP;
                getPass2SizeInfo(queryRound, eSTEREO_SCENARIO_CAPTURE, p2SizeInfo);
                outSize = p2SizeInfo.areaWDMA.contentSize();
                outSize.w /= 2;
                outSize.h /= 2;
            }
        } else {
            MUINT32 junkStride;
            MRect   tgCropRect;

            getPass1Size(sensor, eImgFmt_FG_BAYER10, EPortIndex_RRZO, eSTEREO_SCENARIO_PREVIEW,
                         //below are outputs
                         tgCropRect, outSize, junkStride);
        }
    }

    return true;
}

MSize
StereoSizeProvider::thirdPartyDepthmapSize(ENUM_STEREO_RATIO ratio, ENUM_STEREO_SCENARIO senario) const
{
    StereoSensorConbinationSetting_T *sensorCombination = StereoSettingProviderKernel::getInstance()->getSensorCombinationSetting();
    MSize depthmapSize;
    int m, n;
    imageRatioMToN(ratio, m, n);
    if(eSTEREO_SCENARIO_PREVIEW == senario) {
        if(sensorCombination) {
            if(sensorCombination->depthmapSize.find(ratio) != sensorCombination->depthmapSize.end()) {
                depthmapSize = sensorCombination->depthmapSize[ratio];
            }
        }

        if(0 == depthmapSize.w ||
           0 == depthmapSize.h)
        {
            StereoArea area(480, 360);
            if(m * 3 / 4 >= n) {
                area.applyRatio(ratio, E_KEEP_WIDTH);
            } else {
                area.applyRatio(ratio, E_KEEP_HEIGHT);
            }
            depthmapSize = area.size;
        }

        MY_LOGD("3rd party preview depthmap size for %d:%d: %dx%d", m, n, depthmapSize.w, depthmapSize.h);
    } else {
        if(sensorCombination) {
            if(sensorCombination->depthmapSizeCapture.find(ratio) != sensorCombination->depthmapSizeCapture.end()) {
                depthmapSize = sensorCombination->depthmapSizeCapture[ratio];
            }
        }

        if(0 == depthmapSize.w ||
           0 == depthmapSize.h)
        {
            Pass2SizeInfo p2SizeInfo;
            getPass2SizeInfo(PASS2A_CROP, eSTEREO_SCENARIO_CAPTURE, p2SizeInfo);
            p2SizeInfo.areaWDMA /= 4;
            p2SizeInfo.areaWDMA.removePadding().apply2Align();
            depthmapSize = p2SizeInfo.areaWDMA.contentSize();
        }

        MY_LOGD("3rd party capture depthmap size for %d:%d: %dx%d", m, n, depthmapSize.w, depthmapSize.h);
    }

    return depthmapSize;
}

void
StereoSizeProvider::setPreviewSize(NSCam::MSize size)
{
    __previewSize = size;

    {
        Mutex::Autolock lock(__P1Lock);
        __pass1TgCropRectMap.clear();
        __pass1OutSizeMap.clear();
        __pass1StrideMap.clear();
    }
}

bool
StereoSizeProvider::
getDualcamP2IMGOYuvCropResizeInfo(const int SENSOR_INDEX, NSCam::MRect &cropRect, MSize &targetSize)
{
    int sensorIndex[2] = {0};
    bool isOK = StereoSettingProvider::getStereoSensorIndex(sensorIndex[0], sensorIndex[1]);
    if(!isOK) {
        return false;
    }

    ENUM_PASS2_ROUND queryRound = PASS2A_CROP;
    ENUM_STEREO_SENSOR sensor;
    if(SENSOR_INDEX == sensorIndex[0]) {
        queryRound = PASS2A_CROP;
        sensor = eSTEREO_SENSOR_MAIN1;
    } else if(SENSOR_INDEX == sensorIndex[1]) {
        queryRound = PASS2A_P_CROP;
        sensor = eSTEREO_SENSOR_MAIN2;
    } else {
        MY_LOGE("Invalid sensor index %d. Available sensor indexes: %d, %d", SENSOR_INDEX, sensorIndex[0], sensorIndex[1]);
        return false;
    }

    //Get Crop
    Pass2SizeInfo p2SizeInfo;
    getPass2SizeInfo(queryRound, eSTEREO_SCENARIO_CAPTURE, p2SizeInfo);
    cropRect.s = p2SizeInfo.areaWDMA.contentSize();
    cropRect.p = p2SizeInfo.areaWDMA.startPt;

    //Get size
    getcustomYUVSize(sensor, EPortIndex_IMGO, targetSize);

    MY_LOGD_IF(LOG_ENABLED, "Crop rect: (%d, %d) %dx%d, Size %dx%d",
                            cropRect.p.x, cropRect.p.y, cropRect.s.w, cropRect.s.h,
                            targetSize.w, targetSize.h);

    return true;
}

#if (1 == STEREO_DENOISE_SUPPORTED)
bool
StereoSizeProvider::__updateBMDeNoiseSizes()
{
    int main1Id = -1, main2Id = -1;
    IHalSensorList* sensorList = nullptr;

    StereoSettingProvider::getStereoSensorIndex(main1Id, main2Id);

    // Prepare sensor hal
    sensorList = MAKE_HalSensorList();
    if(NULL == sensorList) {
        MY_LOGE("Cannot get sensor list");
        return false;
    }

    // main1 full size
    {
        int sendorDevIndex = sensorList->querySensorDevIdx(main1Id);

        SensorStaticInfo sensorStaticInfo;
        memset(&sensorStaticInfo, 0, sizeof(SensorStaticInfo));
        sensorList->querySensorStaticInfo(sendorDevIndex, &sensorStaticInfo);

        MSize main1_full_size = MSize(sensorStaticInfo.captureWidth, sensorStaticInfo.captureHeight);

        MRect crop;
        __getCenterCrop(main1_full_size, crop);

        __BMDeNoiseFullRawSize_main1 = main1_full_size;
        __BMDeNoiseFullRawCropSize_main1 = crop;
        __BMDeNoiseAlgoSize_main1 = crop.s;
    }

    // main2 full size
    {
        int sendorDevIndex = sensorList->querySensorDevIdx(main2Id);

        SensorStaticInfo sensorStaticInfo;
        memset(&sensorStaticInfo, 0, sizeof(SensorStaticInfo));
        sensorList->querySensorStaticInfo(sendorDevIndex, &sensorStaticInfo);

        MSize main2_full_size = MSize(sensorStaticInfo.captureWidth, sensorStaticInfo.captureHeight);

        MRect crop;
        __getCenterCrop(main2_full_size, crop);

        __BMDeNoiseFullRawSize_main2 = main2_full_size;
        __BMDeNoiseFullRawCropSize_main2 = crop;
        __BMDeNoiseAlgoSize_main2 = crop.s;
    }

    return true;
}
#endif
/*******************************************************************************
 *
 ********************************************************************************/
bool
StereoSizeProvider::__getCenterCrop(MSize &srcSize, MRect &rCrop )
{
    // calculate the required image hight according to image ratio
    int iHeight = srcSize.h;
    int m, n;
    imageRatioMToN(StereoSettingProvider::imageRatio(), m, n);
    if(eRatio_4_3 != StereoSettingProvider::imageRatio()) {
        iHeight = srcSize.w * n / m;
        applyNAlign(2, iHeight);
    }

    if(abs(iHeight-srcSize.h) == 0)
    {
        rCrop.p = MPoint(0,0);
        rCrop.s = srcSize;
    }
    else
    {
        rCrop.p.x = 0;
        rCrop.p.y = (srcSize.h - iHeight)/2;
        rCrop.s.w = srcSize.w;
        rCrop.s.h = iHeight;
    }

    MY_LOGD_IF(LOG_ENABLED, "srcSize:(%d,%d) ratio:%d, rCropStartPt:(%d, %d) rCropSize:(%d,%d)",
                            srcSize.w, srcSize.h, StereoSettingProvider::imageRatio(),
                            rCrop.p.x, rCrop.p.y, rCrop.s.w, rCrop.s.h);

    // apply 16-align to height
    if(eRatio_4_3 != StereoSettingProvider::imageRatio()) {
        applyNAlign(16, rCrop.s.h);
        MY_LOGD_IF(LOG_ENABLED, "srcSize after 16 align:(%d,%d) ratio:%d, rCropStartPt:(%d, %d) rCropSize:(%d,%d)",
                                srcSize.w, srcSize.h, StereoSettingProvider::imageRatio(),
                                rCrop.p.x, rCrop.p.y, rCrop.s.w, rCrop.s.h);
    }

    return MTRUE;
}
