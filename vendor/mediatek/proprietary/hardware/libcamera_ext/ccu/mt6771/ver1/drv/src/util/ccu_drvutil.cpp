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
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "ccudrvutil"

#include <time.h>
#include "ccu_drvutil.h"
#include "ccu_log.h"

EXTERN_DBG_LOG_VARIABLE(ccu_drv);

/*******************************************************************************
*
********************************************************************************/
namespace NSCcuIf {

enum ccu_tg_info CcuDrvUtil::sensorDevToTgInfo(ESensorDev_T sensorDev, MUINT32 sensorIdx)
{
    LOG_DBG("+:%s\n",__FUNCTION__);
    
    LOG_DBG("sensorDev(%d), sensorIdx(%d)", sensorDev, sensorIdx);

    // Query TG info
    IHalSensorList* const pIHalSensorList = MAKE_HalSensorList();
    IHalSensor* pIHalSensor = pIHalSensorList->createSensor("ccu_ctrl", sensorIdx);
    SensorDynamicInfo rSensorDynamicInfo = {0};

    switch(sensorDev) {
        case ESensorDev_Main:
            pIHalSensor->querySensorDynamicInfo(SENSOR_DEV_MAIN, &rSensorDynamicInfo);
            break;
        case ESensorDev_Sub:
            pIHalSensor->querySensorDynamicInfo(SENSOR_DEV_SUB, &rSensorDynamicInfo);
            break;
        case ESensorDev_MainSecond:
            pIHalSensor->querySensorDynamicInfo(SENSOR_DEV_MAIN_2, &rSensorDynamicInfo);
            break;
        case ESensorDev_MainThird:
            pIHalSensor->querySensorDynamicInfo(SENSOR_DEV_MAIN_3, &rSensorDynamicInfo);
            break;
        case ESensorDev_SubSecond:
            pIHalSensor->querySensorDynamicInfo(SENSOR_DEV_SUB_2, &rSensorDynamicInfo);
            break;
        default:    //  Shouldn't happen.
            LOG_ERR("Invalid sensor device: %d", sensorDev);
    }

    if(pIHalSensor) {
        pIHalSensor->destroyInstance("ccu_ctrl");
    }

    LOG_DBG( "TG = %d\n", rSensorDynamicInfo.TgInfo);

    switch(rSensorDynamicInfo.TgInfo){
        case CAM_TG_1:
            return CCU_CAM_TG_1;
        case CAM_TG_2:
            return CCU_CAM_TG_2;
        default:
            LOG_ERR("undefined rSensorDynamicInfo.TgInfo = %d", rSensorDynamicInfo.TgInfo);
            LOG_ERR("sensorDev(%d), sensorIdx(%d)", sensorDev, sensorIdx);
            return CCU_CAM_TG_NONE;
    }

    LOG_DBG("-:%s\n",__FUNCTION__);
}

double CcuDrvUtil::getTimeUs()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    return (1000000.0*ts.tv_sec) + (ts.tv_nsec/1000);
}

};  //namespace NSCcuIf

