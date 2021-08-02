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

//Notice:
// You must add "LOCAL_CPPFLAGS += -fexceptions" to your Android.mk for exception handling
#ifndef LOGICALCAM_JSON_UTIL_H_
#define LOGICALCAM_JSON_UTIL_H_

#include <mtkcam/utils/json/json.hpp>
#include <mtkcam/utils/json/fifo_map.hpp>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <mtkcam/utils/std/Log.h>
#include <camera_custom_stereo.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <mtkcam/drv/IHalSensor.h>
#pragma GCC diagnostic pop
#include <mtkcam/utils/LogicalCam/Type.h>
#include <mtkcam/utils/LogicalCam/LogicalCamJSONKeys.h>
#include <cutils/properties.h>

#ifndef CAM_ULOG_MODULE_ID
#define CAM_ULOGMV(fmt, arg...)        ALOGV(fmt, ##arg)
#define CAM_ULOGMD(fmt, arg...)        ALOGD(fmt, ##arg)
#define CAM_ULOGMI(fmt, arg...)        ALOGI(fmt, ##arg)
#define CAM_ULOGMW(fmt, arg...)        ALOGW(fmt, ##arg)
#define CAM_ULOGME(fmt, arg...)        ALOGE(fmt, ##arg)
#endif

using namespace nlohmann;
// A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
template<class K, class V, class dummy_compare, class A>
using KeepOrderJsonMap = fifo_map<K, V, fifo_map_compare<K>, A>;
using KeepOrderJSON = basic_json<KeepOrderJsonMap>;

#define STEREO_SETTING_FILE_PATH   "/sdcard/stereo_setting.json"
#define STEREO_TUNING_FILE_PATH    "/sdcard/stereo_tuning.json"

#define SENSOR_DRVNAME_PREFIX std::string("SENSOR_DRVNAME_")

namespace NSCam {

enum ENUM_JSON_PARSE_STATUS
{
    E_JSON_PARSE_FAILED        = 0,
    E_JSON_PARSE_SUCCEED       = 1,
    E_JSON_PARSE_FROM_DEFAULT  = 1<<1,
    E_JSON_PARSE_FROM_EXTERNAL = 1<<2,
};

#define IS_JSON_FROM_EXTERNAL(status) (status & E_JSON_PARSE_FROM_EXTERNAL)
#define IS_JSON_FROM_DEFAULT(status) (status & E_JSON_PARSE_FROM_DEFAULT)

class LogicalCamJSONUtil
{
public:
    static bool HasMember(const json &obj, const char *key)
    {
        return (obj.find(key) != obj.end());
    }

    static bool HasMember(json &obj, const char *key)
    {
        return (obj.find(key) != obj.end());
    }

    static bool HasMember(const json &obj, std::string &key)
    {
        return (obj.find(key) != obj.end());
    }

    static bool HasMember(json &obj, std::string &key)
    {
        return (obj.find(key) != obj.end());
    }

    template<class Json_T>
    static int parseLogicalCamCustomSetting(Json_T &output)
    {
        //Init
        int result = E_JSON_PARSE_SUCCEED;
        struct stat st;
        bool useCustomSetting = true;
        if(0 == stat(STEREO_SETTING_FILE_PATH, &st)) {
            std::ifstream fin(STEREO_SETTING_FILE_PATH);
            if(fin) {
                try
                {
                    output = KeepOrderJSON::parse(fin, nullptr, false);
                    result = E_JSON_PARSE_SUCCEED|E_JSON_PARSE_FROM_EXTERNAL;
                    useCustomSetting = false;
                }
                catch (json::exception& e)
                {
                    CAM_ULOGMW("Parse error: %s", e.what());
                }

                if(!output.is_discarded()) {
                    useCustomSetting = false;
                }
            }

            if(useCustomSetting) {
                CAM_ULOGMW("Cannot load setting from file, use default setting instead");
            }
        }

        if(useCustomSetting) {
            CAM_ULOGMD("Parse DEFAULT_STEREO_SETTING in camera_custom_stereo_setting.h");
            try
            {
                output = Json_T::parse(DEFAULT_STEREO_SETTING);
                result = E_JSON_PARSE_SUCCEED|E_JSON_PARSE_FROM_DEFAULT;
            }
            catch (json::exception& e)
            {
                CAM_ULOGMW("Parse error: %s", e.what());
                result = E_JSON_PARSE_FAILED;
            }

            if(output.is_discarded()) {
                result = E_JSON_PARSE_FAILED;
            }
        }

        return result;
    }

    template<class Json_T>
    static int parseStereoCustomTuning(Json_T &output)
    {
        //Init
        int result = E_JSON_PARSE_SUCCEED;
        struct stat st;
        bool useCustomTuning = true;
        if(0 == stat(STEREO_TUNING_FILE_PATH, &st)) {
            std::ifstream fin(STEREO_TUNING_FILE_PATH);
            if(fin) {
                try
                {
                    output = KeepOrderJSON::parse(fin, nullptr, false);
                    result = E_JSON_PARSE_SUCCEED|E_JSON_PARSE_FROM_EXTERNAL;
                    useCustomTuning = false;
                }
                catch (json::exception& e)
                {
                    CAM_ULOGMW("Parse error: %s", e.what());
                }

                if(!output.is_discarded()) {
                    useCustomTuning = false;
                }
            }

            if(useCustomTuning) {
                CAM_ULOGMW("Cannot load tuning from file, use default tuning instead");
            }
        }

        if(useCustomTuning) {
            CAM_ULOGMD("Parse DEFAULT_STEREO_TUNING in camera_custom_stereo_tuning.h");
            try
            {
                output = Json_T::parse(DEFAULT_STEREO_TUNING);
                result = E_JSON_PARSE_SUCCEED|E_JSON_PARSE_FROM_DEFAULT;
            }
            catch (json::exception& e)
            {
                CAM_ULOGMW("Parse error: %s", e.what());
                result = E_JSON_PARSE_FAILED;
            }

            if(output.is_discarded()) {
                result = E_JSON_PARSE_FAILED;
            }
        }

        return result;
    }

    static std::vector<NSCam::LogicalSensorStruct> getLogicalDevices(void)
    {
        std::vector<struct NSCam::LogicalSensorStruct> gCustomDevList;
        NSCam::IHalSensorList *pSensorList = MAKE_HalSensorList();
        if (NULL == pSensorList) {
            CAM_ULOGME("Cannot get sensor list");
            return gCustomDevList;
        }

        std::map<std::string, int> sensorNames;
        int32_t sensorCount = pSensorList->queryNumberOfSensors();
        for(int index = 0; index < sensorCount; ++index) {
            sensorNames[pSensorList->queryDriverName(index)] = index;
        }

        gCustomDevList.clear();

        json jsonObj;
        LogicalCamJSONUtil::parseLogicalCamCustomSetting(jsonObj);

        // Sensor Combinations
        for(auto &settingValue : jsonObj[CUSTOM_KEY_SENSOR_COMBINATIONS]) {
            // Logical Device
            if(LogicalCamJSONUtil::HasMember(settingValue, CUSTOM_KEY_LOGICAL_DEVICE)) {
                const json &logcalDeviceJson = settingValue[CUSTOM_KEY_LOGICAL_DEVICE];
                NSCam::LogicalSensorStruct logicalDevice;
                // Name
                std::string name = logcalDeviceJson[CUSTOM_KEY_NAME].get<std::string>();

                ::memset(logicalDevice.Name, 0, sizeof(logicalDevice.Name));
                strncpy(logicalDevice.Name, name.c_str(), std::min(sizeof(logicalDevice.Name), name.length()));
                CAM_ULOGMD("Name: %s", logicalDevice.Name);

                logicalDevice.NumofCombinSensor = 0;
                logicalDevice.NumofDefinition   = 1;

                // Sensors
                for(auto &sensor : settingValue[CUSTOM_KEY_SENSORS])
                {
                    name = sensor.get<std::string>();
                    if(std::string::npos == name.find(SENSOR_DRVNAME_PREFIX)) {
                        name = SENSOR_DRVNAME_PREFIX + name;
                    }

                    if(sensorNames.find(name) != sensorNames.end()) {
                        logicalDevice.Sensorlist.push_back(name);
                        CAM_ULOGMD("Sensor %d: %s", logicalDevice.NumofCombinSensor, logicalDevice.Sensorlist[logicalDevice.NumofCombinSensor].c_str());
                        logicalDevice.NumofCombinSensor++;
                    } else {
                        CAM_ULOGMD("Sensor %s not found in sensor list", sensor.get<std::string>().c_str());
                        break;
                    }
                }

                if(0 == settingValue[CUSTOM_KEY_SENSORS].size() ||
                   (size_t)logicalDevice.NumofCombinSensor < settingValue[CUSTOM_KEY_SENSORS].size())
                {
                    CAM_ULOGMD("NumofCombinSensor %d != sensors in setting %zu", logicalDevice.NumofCombinSensor, settingValue[CUSTOM_KEY_SENSORS].size());
                    continue;
                }

                CAM_ULOGMD("NumofCombinSensor %d", logicalDevice.NumofCombinSensor);

                // Features
                logicalDevice.Feature = 0;
                for(auto &feature : logcalDeviceJson[CUSTOM_KEY_FEATURES])
                {
                    std::string name = feature.get<std::string>();
                    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

                    if(name == CUSTOM_KEY_VSDOF)
                    {
                        logicalDevice.Feature |= NSCam::DEVICE_FEATURE_VSDOF;
                        CAM_ULOGMD("%s", CUSTOM_KEY_VSDOF);
                    }
                    else if(name == CUSTOM_KEY_ZOOM)
                    {
                        logicalDevice.Feature |= NSCam::DEVICE_FEATURE_ZOOM;
                        CAM_ULOGMD("%s", CUSTOM_KEY_ZOOM);
                    }
                    else if(name == CUSTOM_KEY_DENOISE)
                    {
                        logicalDevice.Feature |= NSCam::DEVICE_FEATURE_DENOISE;
                        CAM_ULOGMD("%s", CUSTOM_KEY_DENOISE);
                    }
                    else if(name == CUSTOM_KEY_3RD_Party)
                    {
                        logicalDevice.Feature |= NSCam::DEVICE_FEATURE_VSDOF;//NSCam::DEVICE_FEATURE_3RD_PARTY;
                        CAM_ULOGMD("%s(%s)", CUSTOM_KEY_VSDOF, CUSTOM_KEY_3RD_Party);
                    }
                    else if(name == CUSTOM_KEY_MTK_DEPTHMAP)
                    {
                        logicalDevice.Feature |= NSCam::DEVICE_FEATURE_VSDOF;//NSCam::DEVICE_FEATURE_MTK_DEPTHMAP;
                        CAM_ULOGMD("%s(%s)", CUSTOM_KEY_VSDOF, CUSTOM_KEY_MTK_DEPTHMAP);
                    }
                    else if(name == CUSTOM_KEY_SECURE_CAMERA)
                    {
                        logicalDevice.Feature |= NSCam::DEVICE_FEATURE_SECURE_CAMERA;
                        CAM_ULOGMD("%s", CUSTOM_KEY_SECURE_CAMERA);
                    }
                    else
                    {
                        CAM_ULOGMD("Unsupported feature: %s", name.c_str());
                    }
                }

                gCustomDevList.push_back(logicalDevice);
            }
        }

        return gCustomDevList;
    }
};

};
#endif
