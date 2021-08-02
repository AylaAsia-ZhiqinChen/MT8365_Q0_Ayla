#include "FakeSensor.h"
#include <mtkcam/drv/IHalSensor.h>
#include <kd_imgsensor_define.h>

//4176x3088, 2088x1544, 1600x1200, 1280x720
#define SENSOR_PREVIEW_MODE     (0)
#define SENSOR_CAPTURE_MODE     (1)
#define SENSOR_MODE             (1)
#define UNUSED(x) (void)(x)

#if (SENSOR_MODE == SENSOR_PREVIEW_MODE)
#define SENSOR_WIDTH        (1600)
#define SENSOR_HEIGHT       (1200)
#elif (SENSOR_MODE == SENSOR_CAPTURE_MODE)
#define SENSOR_WIDTH        (2096)//(2088) //(4176) //
#define SENSOR_HEIGHT       (1550)//(1544) //(3088) //
#else
#define SENSOR_WIDTH        (1600)
#define SENSOR_HEIGHT       (1200)
#endif

int sensor_type = SENSOR_TYPE_RAW;
int tg_info = CAM_TG_1;

SensorStaticInfo FakeSensorList::mSInfo;
IMetadata FakeSensorList::mDummyMetadata;

IHalSensorList *FakeSensorList::getTestModel()
{
    static FakeSensorList single;

    return &single;
}

IHalSensor *FakeSensorList::createSensor (
                char const* szCallerName,
                MUINT const uCountOfIndex,
                MUINT const*pArrayOfIndex)
{
    UNUSED(szCallerName);
    UNUSED(uCountOfIndex);
    UNUSED(pArrayOfIndex);
    static FakeSensor single;
    return &single;
}

MVOID FakeSensorList::querySensorStaticInfo(
        MUINT sensorDevIdx,
        SensorStaticInfo *pSensorStaticInfo) const
{
    UNUSED(sensorDevIdx);
    pSensorStaticInfo[0].previewWidth = SENSOR_WIDTH;
    pSensorStaticInfo[0].previewHeight = SENSOR_HEIGHT;
    // get sensor full range
    pSensorStaticInfo[0].captureWidth = SENSOR_WIDTH;
    pSensorStaticInfo[0].captureHeight = SENSOR_HEIGHT;
    // get sensor video range
    pSensorStaticInfo[0].videoWidth = SENSOR_WIDTH;
    pSensorStaticInfo[0].videoHeight = SENSOR_HEIGHT;
    // get sensor video1 range
    pSensorStaticInfo[0].video1Width = SENSOR_WIDTH;
    pSensorStaticInfo[0].video1Height = SENSOR_HEIGHT;
    // get sensor video2 range
    pSensorStaticInfo[0].video2Width = SENSOR_WIDTH;
    pSensorStaticInfo[0].video2Height = SENSOR_HEIGHT;
    // get sensor video3 range
    pSensorStaticInfo[0].SensorCustom1Width = SENSOR_WIDTH;
    pSensorStaticInfo[0].SensorCustom1Height = SENSOR_HEIGHT;
    // get sensor video4 range
    pSensorStaticInfo[0].SensorCustom2Width = SENSOR_WIDTH;
    pSensorStaticInfo[0].SensorCustom2Height = SENSOR_HEIGHT;
    // get sensor video5 range
    pSensorStaticInfo[0].SensorCustom3Width = SENSOR_WIDTH;
    pSensorStaticInfo[0].SensorCustom3Height = SENSOR_HEIGHT;
    // get sensor video6 range
    pSensorStaticInfo[0].SensorCustom4Width = SENSOR_WIDTH;
    pSensorStaticInfo[0].SensorCustom4Height = SENSOR_HEIGHT;
    // get sensor video7 range
    pSensorStaticInfo[0].SensorCustom5Width = SENSOR_WIDTH;
    pSensorStaticInfo[0].SensorCustom5Height = SENSOR_HEIGHT;
    // get RAW or YUV
    pSensorStaticInfo[0].sensorType = sensor_type; //1:RAW 2:YUV
    //get sernsor orientation angle
    pSensorStaticInfo[0].orientationAngle = 90;
    //get sensor format info
    pSensorStaticInfo[0].rawSensorBit = RAW_SENSOR_12BIT; //RAW10
    if(sensor_type == SENSOR_TYPE_RAW)
        pSensorStaticInfo[0].sensorFormatOrder = SENSOR_FORMAT_ORDER_RAW_Gb;//3;
    else
        pSensorStaticInfo[0].sensorFormatOrder = SENSOR_FORMAT_ORDER_UYVY;//for yuv fmt

    pSensorStaticInfo[0].SensorModeNum = 1;

}

static MUINT32 mPowerOnTG[] = {

    0x00004410, 0x00000005,
    0x00004418, ((((SENSOR_WIDTH+0x0002)&0x7FFF)<<16) | 0x0002),    // TG_SEN_GRAB_PXL
    0x0000441C, ((((SENSOR_HEIGHT+0x0002)&0x1FFF)<<16) | 0x0002),   // TG_SEN_GRAB_LIN

    0x00050410, 0x00000005,
    0x00050418, ((((SENSOR_WIDTH+0x0002)&0x7FFF)<<16) | 0x0002),    // TG_SEN_GRAB_PXL
    0x0005041C, ((((SENSOR_HEIGHT+0x0002)&0x1FFF)<<16) | 0x0002),   // TG_SEN_GRAB_LIN 

    0x00050C10, 0x00000005,
    0x00050C18, ((((SENSOR_WIDTH+0x0002)&0x7FFF)<<16) | 0x0002),    // TG_SEN_GRAB_PXL
    0x00050C1C, ((((SENSOR_HEIGHT+0x0002)&0x1FFF)<<16) | 0x0002),   // TG_SEN_GRAB_LIN

};


static MUINT32 mPowerOnTM[] = {


    0x00000000, 0x00000C00, //SENINF_TOP_CTRL
    0x00000008, 0x00003210,
    0x00000010, 0x76540000,
    
    0x00000200, 0x00001001,
    0x00000204, 0x00000002,
    0x00000208, 0x00000000,
    0x0000020C, 0x00000000,
    0x00000210, 0x00000000,
    0x00000214, 0x00000000,
    0x00000218, 0x00000000,
    0x0000021C, 0x00000000,
    0x00000220, 0x86DF1080,
    0x00000224, 0x8000007F,
//  0x0000024C, 0x000E2000,    
    0x00000D00, 0x86DF1080,     //[8]: SENINF_PIX_SEL (sel_ext, sel)
    0x00000D0C, 0x00000000,
    0x00000D38, 0x00000000,
    0x00000D3C, 0x00000000,    //[4]: SENINF_PIX_SEL_EXT
    0x00000608, 0x00FF0131, // TG1_TM_CTL
                            // [7:4]: green(3), red(5), blue(6), H_BAR(12), V_BAR(13)
    0x0000060C, 0x0FA01388, // TG1_TM_SIZE
    0x00000610, 0x00000008,  // TG1_TM_CLK

};

MBOOL FakeSensor::powerOn(
        char const* szCallerName,
        MUINT const uCountOfIndex,
        MUINT const*pArrayOfIndex)
{
    MUINT32 nNum = 0, i = 0, ret = 0;
    UNUSED(szCallerName);
    UNUSED(uCountOfIndex);
    UNUSED(pArrayOfIndex);
    m_pIspDrv = IspDrv::createInstance();
    if (NULL == m_pIspDrv) {
        TS_LOGD("Error: IspDrv CreateInstace fail");
        return 0;
    }

    ret = m_pIspDrv->init("FakeSeninf");
    if (ret < 0) {
        TS_LOGD("Error: IspDrv init fail");
        return 0;
    }
    if (m_pIspReg != NULL){
        m_pIspReg = NULL;
    }
    m_pIspReg = (isp_reg_t *)m_pIspDrv->getRegAddr();
    TS_LOGD("m_pIspReg:0x%p ", m_pIspReg);
    if (m_pIspReg == NULL) {
        TS_LOGD("getVirRegAddr fail ");
        return -1;
    }

    nNum = (sizeof(mPowerOnTM) / sizeof(mPowerOnTM[0])) / 2;
    TS_LOGD(" Total %d registers", nNum);
    for (i = 0; i < nNum; i++) {
        m_pIspDrv->writeReg(mPowerOnTM[i<<1], mPowerOnTM[(i<<1)+1],ISP_DRV_USER_SENF);
    }

    for (i = 0; i < nNum; i++) {
        TS_LOGD(" Reg[x%08x] = x%08x/x%08x", mPowerOnTM[i<<1],
            m_pIspDrv->readReg(mPowerOnTM[i<<1]), mPowerOnTM[(i<<1)+1]);
    }

    if (m_pIspDrv) {
        m_pIspDrv->uninit("FakeSeninf");
    }

    ret = m_pIspDrv->init("FakeTG");
    if (ret < 0) {
        TS_LOGD("Error: IspDrv init fail");
        return 0;
    }
    nNum = (sizeof(mPowerOnTG) / sizeof(mPowerOnTG[0])) / 2;
    TS_LOGD(" Total %d registers", nNum);
    for (i = 0; i < nNum; i++) {
        m_pIspDrv->writeReg(mPowerOnTG[i<<1], mPowerOnTG[(i<<1)+1],ISP_DRV_USER_SENF);
    }
    for (i = 0; i < nNum; i++) {
        TS_LOGD(" Reg[x%08x] = x%08x/x%08x", mPowerOnTG[i<<1],
            m_pIspDrv->readReg(mPowerOnTG[i<<1]), mPowerOnTG[(i<<1)+1]);
    }

    if (m_pIspDrv) {
        m_pIspDrv->uninit("FakeTG");
        m_pIspDrv->destroyInstance();
        m_pIspDrv = NULL;
    }


    return 1;
}
MBOOL FakeSensor::powerOff(
        char const* szCallerName,
        MUINT const uCountOfIndex,
        MUINT const*pArrayOfIndex)
{
    int ret = 0;
    UNUSED(szCallerName);
    UNUSED(uCountOfIndex);
    UNUSED(pArrayOfIndex);
    if (m_pIspDrv) {
        ret = m_pIspDrv->uninit("FakeSeninf");
        if (ret < 0) {
            TS_LOGE("m_pIspDrv->uninit() fail");
            return ret;
        }
        m_pIspDrv->destroyInstance();
        m_pIspDrv = NULL;
    }

    return 1;
}
MBOOL FakeSensor::configure(
        MUINT const         uCountOfParam,
        IHalSensor::ConfigParam const*  pArrayOfParam)
{
    UNUSED(uCountOfParam);
    UNUSED(pArrayOfParam);
    return 1;
}
MBOOL FakeSensor::querySensorDynamicInfo(
          MUINT32 sensorIdx,
          SensorDynamicInfo *pSensorDynamicInfo)
{
    UNUSED(sensorIdx);
    if(sensorIdx == 2)
        pSensorDynamicInfo->TgInfo = CAM_SV_2;
    else
        pSensorDynamicInfo->TgInfo = tg_info;
    pSensorDynamicInfo->pixelMode = 0; //ONE_PIXEL_MODE, TWO_PIXEL_MODE

    return 1;
}



MINT FakeSensor::sendCommand(
           MUINT sensorDevIdx,
           MUINTPTR cmd,
           MUINTPTR arg1,
           MUINTPTR arg2,
           MUINTPTR arg3)
{
    UNUSED(sensorDevIdx);
    UINT32 subCmd = *(MUINT32 *)arg1;
    switch (cmd) {
    case 0xFFF0:
        if(subCmd == 0)
        {
            if(*(MUINT32 *)arg2 == SENSOR_TYPE_RAW)
                sensor_type = SENSOR_TYPE_RAW;
            else
                sensor_type = SENSOR_TYPE_YUV;
        }
        else if(subCmd == 1)
        {
            if(*(MUINT32 *)arg2 == CAM_SV_1)
                tg_info = CAM_SV_1;
            else if(*(MUINT32 *)arg2 == CAM_SV_2)
                tg_info = CAM_SV_2;
            else
                tg_info = CAM_TG_1;
        }
        break;
    default:
        break;
    };
    return 1; 

}


