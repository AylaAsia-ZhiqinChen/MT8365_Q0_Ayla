/**
* @file ts_UT.h
*
* ts_UT thread Header File
*/

//! \file  ts_camio.cpp
//! \brief
#define _USE_IMAGIO_
#if 0
#define _USE_THREAD_QUE_
#endif

#define LOG_TAG "Imageio_FrmB_Test"

#include <vector>

using namespace std;

//#include <linux/cache.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>
//
#include <errno.h>
#include <fcntl.h>

#include <mtkcam/common.h>
#include <mtkcam/common/hw/hwstddef.h>

//camio
#include <mtkcam/campipe/_buffer.h>
#include <mtkcam/campipe/pipe_mgr_drv.h>
#include <mtkcam/campipe/IPipe.h>
#include <mtkcam/campipe/ICamIOPipe.h>

//imageio
#include <mtkcam/imageio/IPipe.h>
#include <mtkcam/imageio/ICamIOPipe.h>
#include <mtkcam/imageio/ispio_utility.h>
#include "PipeImp_FrmB.h"//../pipe/inc/
#include "CamIOPipe_FrmB.h"//../pipe/inc/
#include "CampipeImgioPipeMapper.h" //core\campipe\inc

using namespace NSIoPipe;
using namespace NSImageio_FrmB;
using namespace NSIspio_FrmB;

//
#include <mtkcam/hal/sensor_hal.h>
#include <mtkcam/drv_common/imem_drv.h>
#include <iopipe_FrmB/CamIO/PortMap_FrmB.h>

//thread
#include <semaphore.h>
#include <pthread.h>
#include <utils/threads.h>
#include <utils/Mutex.h>    // For android::Mutex.
#include <mtkcam/v1/config/PriorityDefs.h>

using namespace android;

/*******************************************************************************
*
********************************************************************************/
#include <mtkcam/Log.h>
#define MY_LOGV(fmt, arg...)    CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)    CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)    CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)    CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)    CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)

#define BUF_NUM          3
#define PREVIEW_WIDTH    1600
#define PREVIEW_HEIGHT   1200

#ifndef _MTK_CAMERA_CORE_CAMIOPIPE_TEST_UT_H_
#define _MTK_CAMERA_CORE_CAMIOPIPE_TEST_UT_H_

/*******************************************************************************
*
********************************************************************************/

namespace NSImageio_FrmB {
namespace NSIspio_FrmB{

    class Ts_UT
    {
        public:     ////    Constructor/Destructor.
        Ts_UT();
        /**
        * @brief  Destructor
        */
        virtual ~Ts_UT();

        public:
            virtual int main_ts_CamIO_ZSD(int count);
            virtual int main_ts_CamIO_2port(int count);
            virtual int main_ts_CamIO_Update(int count);
            virtual int main_SetSensor_init();
            virtual int main_SetSensor_uninit();
            virtual MVOID freeRawMem();

            #ifdef _USE_THREAD_QUE_
                virtual MVOID TS_Thread_Init(int count);
                virtual MBOOL TS_Thread_UnInit();
            #endif // _USE_THREAD_QUE_

        private:
            virtual void mapBufInfo(NSCamHW::BufInfo &rCamPipeBufInfo, BufInfo const &rBufInfo);
            virtual ERawPxlID mapRawPixelID(MUINT32 const u4PixelID);
            /* Inport & OutPort settings */
            virtual MBOOL queryPipeProperty(NSCamPipe::ESWScenarioID const eSWScenarioID,vector<NSCamPipe::PortProperty> &vInPorts, vector<NSCamPipe::PortProperty> &vOutPorts);
            virtual MBOOL querySensorInfo(MUINT32 const u4DeviceID, MUINT32 const u4Scenario, MUINT32 const u4BitDepth, EImageFormat &eFmt,  MUINT32 &u4Width, MUINT32 &u4Height, MUINT32 & u4RawPixelID);
            virtual MBOOL configSensor(MUINT32 const u4DeviceID, MUINT32 const u4Scenario, MUINT32 const u4Width, MUINT32 const u4Height, MUINT32 const byPassDelay, MUINT32 const u4ByPassScenario, MBOOL const fgIsContinuous);
            virtual MBOOL setConfigPortInfo(MINT32 nOutPort);
            virtual MBOOL skipFrame(MUINT32 const u4SkipCount);

        virtual MBOOL start();
        virtual MBOOL dequeHWBuf(MUINT32 const u4TimeoutMs = 0xFFFFFFFF);

#ifdef _USE_THREAD_QUE_
        static MVOID* endeque_Thread(MVOID *arg);
#endif
        virtual MBOOL enqueBuf(NSCamPipe::PortID const ePortID, NSCamPipe::QBufInfo const& rQBufInfo);
        virtual MBOOL dequeBuf(NSCamPipe::PortID const ePortID, NSCamPipe::QTimeStampBufInfo& rQBufInfo, MUINT32 const u4TimeoutMs = 0xFFFFFFFF);

        public:
        IMemDrv *mpIMemDrv;
        int mode;//0: preview / else:capture

        private:
        ICamIOPipe *mpCamIOPipe;
        SensorHal* mpSensorHal;

        NSCamPipe::SensorPortInfo mrSensorPortInfo;
        NSCamPipe::QTimeStampBufInfo mrRawQTBufInfo;
        NSCamPipe::QTimeStampBufInfo mrYuvQTBufInfo;
        //            NSCamPipe::QBufInfo rRawBuf;
        QBufInfo rRawBuf;

        vector<IMEM_BUF_INFO> vRawMem[2];
        vector<IMEM_BUF_INFO> vDequeMem[2];

        MUINT32 mu4DeviceID;
        MUINT32 mu4SkipFrame;

        /************************
        * Sensor settings
        ************************/
        MBOOL mfgIsYUVPortON;
        MINT32 continuous;
        MINT32 is_yuv_sensor;//cfg_sensor_yuv;

        halSensorDev_e sensorDevId;

        /*sensor width/height*/
        MUINT32 u4SensorFullWidth;
        MUINT32 u4SensorFullHeight;
        MUINT32 u4SensorHalfWidth;
        MUINT32 u4SensorHalfHeight;

        MUINT32 u4SensorWidth;
        MUINT32 u4SensorHeight;

#ifdef _USE_THREAD_QUE_ //for enque/deque thread
        pthread_t m_TestEnDequethread;
        Mutex mLock; //mutable
        volatile MINT32 mInitCount;
        sem_t m_semTestEnDequethread;
#endif
        };
    }
}
#endif
