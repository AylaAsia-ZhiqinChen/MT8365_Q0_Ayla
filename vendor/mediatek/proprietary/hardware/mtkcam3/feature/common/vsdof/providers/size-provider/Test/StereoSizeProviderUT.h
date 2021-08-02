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
#ifndef _STEREO_SIZE_PROVIDER_UT_H_
#define _STEREO_SIZE_PROVIDER_UT_H_

#include <limits.h>
#include <gtest/gtest.h>
#include <mtkcam3/feature/stereo/hal/stereo_size_provider.h>
#include <mtkcam3/feature/stereo/hal/stereo_setting_provider.h>
#include <mtkcam/utils/LogicalCam/IHalLogicalDeviceList.h>
#include <mtkcam/aaa/IHal3A.h>
#include <mtkcam/drv/IHalSensor.h>

using namespace NSCam;
using namespace android;
using namespace NSCam::v1::Stereo;
using namespace StereoHAL;

#define PRINT_SIZE  1

inline void print(const char *tag, MSize size)
{
#if PRINT_SIZE
    printf("%s: %dx%d\n", tag, size.w, size.h);
#endif
}

inline void print(const char *tag, MRect rect)
{
#if PRINT_SIZE
    printf("%s: (%d, %d), %dx%d\n", tag, rect.p.x, rect.p.y, rect.s.w, rect.s.h);
#endif
}

inline void print(const char *tag, StereoArea area)
{
#if PRINT_SIZE
    if(area.padding.w != 0 || area.padding.h != 0) {
        printf("%s: Size %dx%d, Padding %dx%d, StartPt (%d, %d), ContentSize %dx%d\n", tag,
               area.size.w, area.size.h, area.padding.w, area.padding.h,
               area.startPt.x, area.startPt.y, area.contentSize().w, area.contentSize().h);
    } else {
        printf("%s: Size %dx%d\n", tag, area.size.w, area.size.h);
    }
#endif
}

template<class T>
inline bool isEqual(T value, T expect)
{
    if(value != expect) {
        print("[Value ]", value);
        print("[Expect]", expect);

        return false;
    }

    return true;
}

#define MYEXPECT_EQ(val1, val2) EXPECT_TRUE(isEqual(val1, val2))

class StereoSizeProviderUTBase: public ::testing::Test
{
public:
    StereoSizeProviderUTBase()
    {
        sizeProvider = StereoSizeProvider::getInstance();

        int input;
        const size_t SENSOR_COUNT = MAKE_HalSensorList()->queryNumberOfSensors();
        if(SENSOR_COUNT == 3) {
            StereoSettingProvider::setLogicalDeviceID(SENSOR_COUNT);
        } else {
            printf("Set logical device(>= %zu):", SENSOR_COUNT);
            cin >> input;
            StereoSettingProvider::setLogicalDeviceID(input);
        }

        printf("Select scenario: [0]Preview [1]Capture: ");
        cin >> input;
        scenario = (0 == input) ? eSTEREO_SCENARIO_PREVIEW : eSTEREO_SCENARIO_CAPTURE;

        // printf("Select feature mode: [0]ActiveStereo [1] VSDoF: ");
        // cin >> input;
        // StereoSettingProvider::setStereoFeatureMode((0 == input) ? E_STEREO_FEATURE_ACTIVE_STEREO : E_STEREO_FEATURE_VSDOF);
        StereoSettingProvider::setStereoFeatureMode(E_STEREO_FEATURE_VSDOF);

        //Update sensor sceanrio
        MUINT sensorScenarioMain1;
        MUINT sensorScenarioMain2;
        StereoSettingProvider::getSensorScenario(
                                StereoSettingProvider::getStereoFeatureMode(),
                                false,
                                sensorScenarioMain1,
                                sensorScenarioMain2,
                                true);

        printf("Select image ratio:\n[0]4:3\n[1]16:9\n[2]18:9\n[3]19:9\n[4]1:1:\n:");
        ENUM_STEREO_RATIO ratio = eRatio_4_3;
        cin >> input;
        switch(input) {
            case 0:
            default:
                ratio = eRatio_4_3;
            break;
            case 1:
                ratio = eRatio_16_9;
            break;
            case 2:
                ratio = MToNToImageRatio(18, 9);
            break;
            case 3:
                ratio = MToNToImageRatio(19, 9);
            break;
            case 4:
                ratio = MToNToImageRatio(1, 1);
            break;

        }
        StereoSettingProvider::setImageRatio(ratio);

        printf("============================\n");
        printf("Scenario: %s\n", (scenario == eSTEREO_SCENARIO_PREVIEW) ? "Preview" : "Capture");
        switch(StereoSettingProvider::getStereoFeatureMode()) {
        case E_STEREO_FEATURE_VSDOF:
        default:
            printf("Feature Mode: VSDoF\n");
            break;
        // case E_STEREO_FEATURE_ACTIVE_STEREO:
        //     printf("Feature Mode: ActiveStereo\n");
        //     break;
        }

        int m, n;
        imageRatioMToN(StereoSettingProvider::imageRatio(), m, n);
        printf("Image Ratio: %d:%d\n", m, n);

        printf("============================\n");
    }

    virtual ~StereoSizeProviderUTBase() {}

protected:
    virtual void SetUp() {
        if(ignoreTest())
        {
            return;
        }

        sizeProvider->getPass1Size( eSTEREO_SENSOR_MAIN1,
                                    eImgFmt_FG_BAYER10,
                                    EPortIndex_RRZO,
                                    scenario,
                                    //below are outputs
                                    tgCropRect[0],
                                    szRRZO[0],
                                    junkStride);

        sizeProvider->getPass1Size( eSTEREO_SENSOR_MAIN2,
                                    eImgFmt_FG_BAYER10,
                                    EPortIndex_RRZO,
                                    scenario,
                                    //below are outputs
                                    tgCropRect[1],
                                    szRRZO[1],
                                    junkStride);

        sizeProvider->getPass1Size( eSTEREO_SENSOR_MAIN1,
                                    eImgFmt_BAYER10,
                                    EPortIndex_IMGO,
                                    scenario,
                                    //below are outputs
                                    tgCropRect[2],
                                    szMainIMGO,
                                    junkStride);

        sizeProvider->getPass1Size( eSTEREO_SENSOR_MAIN1,
                                    eImgFmt_NV16,
                                    EPortIndex_CRZO_R2,
                                    eSTEREO_SCENARIO_PREVIEW,
                                    //below are outputs
                                    tgCropRectYUV,
                                    szP1YUV,
                                    junkStride);

        sizeProvider->getPass1ActiveArrayCrop(eSTEREO_SENSOR_MAIN1, activityArray[0]);
        sizeProvider->getPass1ActiveArrayCrop(eSTEREO_SENSOR_MAIN2, activityArray[1]);

        //=============================================================================
        //  PASS 2
        //=============================================================================
        sizeProvider->getPass2SizeInfo(PASS2A,          scenario,   pass2SizeInfo[PASS2A]);
        sizeProvider->getPass2SizeInfo(PASS2A_2,        scenario,   pass2SizeInfo[PASS2A_2]);
        sizeProvider->getPass2SizeInfo(PASS2A_3,        scenario,   pass2SizeInfo[PASS2A_3]);
        sizeProvider->getPass2SizeInfo(PASS2A_P,        scenario,   pass2SizeInfo[PASS2A_P]);
        sizeProvider->getPass2SizeInfo(PASS2A_P_2,      scenario,   pass2SizeInfo[PASS2A_P_2]);
        sizeProvider->getPass2SizeInfo(PASS2A_P_3,      scenario,   pass2SizeInfo[PASS2A_P_3]);
        sizeProvider->getPass2SizeInfo(PASS2A_B,        scenario,   pass2SizeInfo[PASS2A_B]);
        sizeProvider->getPass2SizeInfo(PASS2A_CROP,     eSTEREO_SCENARIO_CAPTURE,   pass2SizeInfo[PASS2A_CROP]);
        sizeProvider->getPass2SizeInfo(PASS2A_B_CROP,   eSTEREO_SCENARIO_CAPTURE,   pass2SizeInfo[PASS2A_B_CROP]);
        sizeProvider->getPass2SizeInfo(PASS2A_P_CROP,   eSTEREO_SCENARIO_CAPTURE,   pass2SizeInfo[PASS2A_P_CROP]);
    }

    // virtual void TearDown() {}

    virtual bool ignoreTest() = 0;

    virtual void init()
    {

    }

protected:
    //Init
    StereoSizeProvider *sizeProvider;
    ENUM_STEREO_SCENARIO scenario;

    //Pass 1
    MUINT32 junkStride;
    MRect tgCropRect[3];
    MSize szRRZO[2];
    MRect activityArray[2];
    MSize szMainIMGO;
    //P1 YUV for preview
    MRect tgCropRectYUV;
    MSize szP1YUV;

    //Pass 2
    Pass2SizeInfo pass2SizeInfo[PASS2_ROUND_COUNT];

    //FOV cropping
    float FOV_RATIO  = 1.0f;
    bool IS_FOV_CROP = false;
};

#endif