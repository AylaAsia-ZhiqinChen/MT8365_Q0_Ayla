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
#define LOG_TAG "NormalStream"

//using namespace NSImageio;
//using namespace NSIspio;

#include "NormalStream.h"
#include "PortMap.h"
#include <mtkcam/imageio/IPipe.h>
#include <mtkcam/imageio/IPostProcPipe.h>
#include <mtkcam/ispio_sw_scenario.h>
#include <vector>
#include <cutils/atomic.h>

/*************************************************************************************
* Log Utility
*************************************************************************************/
#ifndef USING_MTK_LDVT   // Not using LDVT.
#define  DBG_LOG_TAG        ""
#else
#define  DBG_LOG_TAG        LOG_TAG
#endif

#include <imageio_log.h>    // Note: DBG_LOG_TAG/LEVEL will be used in header file, so header must be included after definition.
DECLARE_DBG_LOG_VARIABLE(P2NormalStream);
// Clear previous define, use our own define.
#undef LOG_VRB
#undef LOG_DBG
#undef LOG_INF
#undef LOG_WRN
#undef LOG_ERR
#undef LOG_AST
#define LOG_VRB(fmt, arg...)        do { if (P2NormalStream_DbgLogEnable_VERBOSE) { BASE_LOG_VRB(fmt, ##arg); } } while(0)
#define LOG_DBG(fmt, arg...)        do { if (P2NormalStream_DbgLogEnable_DEBUG  ) { BASE_LOG_DBG(fmt, ##arg); } } while(0)
#define LOG_INF(fmt, arg...)        do { if (P2NormalStream_DbgLogEnable_INFO   ) { BASE_LOG_INF(fmt, ##arg); } } while(0)
#define LOG_WRN(fmt, arg...)        do { if (P2NormalStream_DbgLogEnable_WARN   ) { BASE_LOG_WRN(fmt, ##arg); } } while(0)
#define LOG_ERR(fmt, arg...)        do { if (P2NormalStream_DbgLogEnable_ERROR  ) { BASE_LOG_ERR(fmt, ##arg); } } while(0)
#define LOG_AST(cond, fmt, arg...)  do { if (P2NormalStream_DbgLogEnable_ASSERT ) { BASE_LOG_AST(cond, fmt, ##arg); } } while(0)



#define FUNCTION_LOG_START      LOG_DBG("+");
#define FUNCTION_LOG_END        LOG_DBG("-");
#define ERROR_LOG               LOG_ERR("Error");
//

using namespace NSCam;
using namespace NSIoPipe;
using namespace NSPostProc;

ESoftwareScenario const gSenDevSwScenMap[ENormalStreamTag_total][SENSOR_DEV_MAIN_3D+1]=
{
    {eSoftwareScenario_Main_Normal_Stream,eSoftwareScenario_Main_Normal_Stream,eSoftwareScenario_Sub_Normal_Stream,\
        eSoftwareScenario_total_num,eSoftwareScenario_total_num,eSoftwareScenario_total_num},//ENormalStreamTag_Stream
    {eSoftwareScenario_Main_VSS_Capture,eSoftwareScenario_Main_VSS_Capture,eSoftwareScenario_Sub_VSS_Capture,\
        eSoftwareScenario_total_num,eSoftwareScenario_total_num,eSoftwareScenario_total_num},//ENormalStreamTag_Vss
    {eSoftwareScenario_Main_Pure_Raw_Stream,eSoftwareScenario_Main_Pure_Raw_Stream,eSoftwareScenario_Sub_Pure_Raw_Stream,\
        eSoftwareScenario_total_num,eSoftwareScenario_total_num,eSoftwareScenario_total_num}//ENormalStreamTag_PureRaw
};

extern SwHwScenarioPathMapping mSwHwPathMapping[eSoftwareScenario_total_num];
/******************************************************************************
 *
 ******************************************************************************/
 #if 0//CHRISTOPHER, do not need in current stage
namespace
{
    using namespace NSCam::NSIoPipe;
    //
    Mutex                           gPipeCapMutex;
    IMetadata                       gPipeCap;
    //
    //  IN PORT
    IMetadata                       gPortCap_IMGI;
    IMetadata                       gPortCap_VIPI;
    //
    //  OUT PORT
    IMetadata                       gPortCap_IMG2O;
    IMetadata                       gPortCap_IMG3O;
    IMetadata                       gPortCap_WROTO;
    IMetadata                       gPortCap_WDMAO;
    IMetadata                       gPortCap_JPEGO;
    //
};
/******************************************************************************
 *
 ******************************************************************************/
static
IMetadata const&
constructPortCapability()
{
    //CHRISTOPHER, do not need in current stage
    Mutex::Autolock _l(gPipeCapMutex);
    //
    if  ( ! gPipeCap.isEmpty() )
    {
        return  gPipeCap;
    }
    //
    //  IMGI
    {
        IMetadata& cap = gPortCap_IMGI;
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_PORT_ID);
            entry.push_back(PORT_IMGI, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_TRANSFORM);
            entry.push_back(eTransform_FLIP_V, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_CROP);
            entry.push_back(MTK_IOPIPE_INFO_CROP_NOT_SUPPORT, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_FORMATS);
            entry.push_back(eImgFmt_BAYER8, Type2Type<MINT32>());
            entry.push_back(eImgFmt_BAYER10,Type2Type<MINT32>());
            entry.push_back(eImgFmt_BAYER12,Type2Type<MINT32>());
            entry.push_back(eImgFmt_YUY2,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_YVYU,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_UYVY,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_VYUY,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_RGB565, Type2Type<MINT32>());
            entry.push_back(eImgFmt_ARGB888,Type2Type<MINT32>());
            entry.push_back(eImgFmt_RGB888, Type2Type<MINT32>());
            entry.push_back(eImgFmt_FG_BAYER8, Type2Type<MINT32>());
            entry.push_back(eImgFmt_FG_BAYER10,Type2Type<MINT32>());
            entry.push_back(eImgFmt_FG_BAYER12,Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        //
        cap.sort();
    }
    //  VIPI
    {
        IMetadata& cap = gPortCap_VIPI;
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_PORT_ID);
            entry.push_back(PORT_VIPI, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_TRANSFORM);
            entry.push_back(eTransform_FLIP_V, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_CROP);
            entry.push_back(MTK_IOPIPE_INFO_CROP_NOT_SUPPORT, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_FORMATS);
            //yuv422
            entry.push_back(eImgFmt_YUY2,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_YVYU,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_UYVY,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_VYUY,   Type2Type<MINT32>());
            //420 2p(y plane, uv would be vip2i)
            entry.push_back(eImgFmt_NV12,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_NV21,   Type2Type<MINT32>());
            //420 3p(y plane, u would be vip2i and v would be vip3i)
            entry.push_back(eImgFmt_YV12,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_I420,   Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        //
        cap.sort();
    }

    //  IMG2O
    {
        IMetadata& cap = gPortCap_IMG2O;
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_PORT_ID);
            entry.push_back(PORT_IMG2O, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_TRANSFORM);
            entry.push_back(eTransform_FLIP_V, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_CROP);
            entry.push_back(MTK_IOPIPE_INFO_CROP_SYMMETRIC|MTK_IOPIPE_INFO_CROP_ASYMMETRIC, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_FORMATS);
            entry.push_back(eImgFmt_YUY2,   Type2Type<MINT32>());   //need match with the setting of imgi, ex,imgi(yuyv)->img2o(yuyv)
            entry.push_back(eImgFmt_YVYU,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_UYVY,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_VYUY,   Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        //
        cap.sort();
    }
    //  IMG3O
    {
        IMetadata& cap = gPortCap_IMG3O;
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_PORT_ID);
            entry.push_back(PORT_IMG3O, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_TRANSFORM);
            entry.push_back(eTransform_FLIP_V, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_CROP);
            entry.push_back(MTK_IOPIPE_INFO_CROP_SYMMETRIC|MTK_IOPIPE_INFO_CROP_ASYMMETRIC, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_FORMATS);
            //yuv422
            entry.push_back(eImgFmt_YUY2,   Type2Type<MINT32>());   //need match with the setting of vipi, ex,vipi(yuyv)->img3o(yuyv)
            entry.push_back(eImgFmt_YVYU,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_UYVY,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_VYUY,   Type2Type<MINT32>());
            //420 2p(y plane, uv would be img3bo)
            entry.push_back(eImgFmt_NV12,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_NV21,   Type2Type<MINT32>());
            //420 3p(y plane, u would be img3bo and v would be imb3co)
            entry.push_back(eImgFmt_YV12,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_I420,   Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        //
        cap.sort();
    }
    //  WROTO
    {
        IMetadata& cap = gPortCap_WROTO;
        {
        IMetadata::IEntry entry(MTK_IOPIPE_INFO_PORT_ID);
        entry.push_back(PORT_WROTO, Type2Type<MINT32>());
        cap.update(entry.tag(), entry);
        }
        {
        IMetadata::IEntry entry(MTK_IOPIPE_INFO_TRANSFORM);
        entry.push_back(eTransform_FLIP_H|eTransform_ROT_90|eTransform_ROT_180|eTransform_ROT_270, Type2Type<MINT32>());
        cap.update(entry.tag(), entry);
        }
        {
        IMetadata::IEntry entry(MTK_IOPIPE_INFO_CROP);
        entry.push_back(MTK_IOPIPE_INFO_CROP_SYMMETRIC|MTK_IOPIPE_INFO_CROP_ASYMMETRIC, Type2Type<MINT32>());
        cap.update(entry.tag(), entry);
        }
        {
        IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_FORMATS);
        entry.push_back(eImgFmt_YUY2, Type2Type<MINT32>());
        entry.push_back(eImgFmt_UYVY, Type2Type<MINT32>());
        entry.push_back(eImgFmt_NV12,   Type2Type<MINT32>());
        entry.push_back(eImgFmt_NV21,   Type2Type<MINT32>());
        entry.push_back(eImgFmt_NV16,   Type2Type<MINT32>());
        entry.push_back(eImgFmt_NV61,   Type2Type<MINT32>());
        entry.push_back(eImgFmt_YV12,   Type2Type<MINT32>());
        entry.push_back(eImgFmt_I420,   Type2Type<MINT32>());
        entry.push_back(eImgFmt_YV16,   Type2Type<MINT32>());
        entry.push_back(eImgFmt_I422,   Type2Type<MINT32>());
        entry.push_back(eImgFmt_RGB565, Type2Type<MINT32>());
        entry.push_back(eImgFmt_ARGB888,Type2Type<MINT32>());
        entry.push_back(eImgFmt_RGB888, Type2Type<MINT32>());
        //TODO
        cap.update(entry.tag(), entry);
        }
        //
        cap.sort();
    }
    //  WDMAO
    {
        IMetadata& cap = gPortCap_WDMAO;
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_PORT_ID);
            entry.push_back(PORT_WDMAO, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_TRANSFORM);
            entry.push_back(0, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_CROP);
            entry.push_back(MTK_IOPIPE_INFO_CROP_SYMMETRIC|MTK_IOPIPE_INFO_CROP_ASYMMETRIC, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_FORMATS);
            entry.push_back(eImgFmt_YUY2, Type2Type<MINT32>());
            entry.push_back(eImgFmt_UYVY, Type2Type<MINT32>());
            entry.push_back(eImgFmt_RGB565, Type2Type<MINT32>());
            entry.push_back(eImgFmt_RGB888, Type2Type<MINT32>());
            entry.push_back(eImgFmt_ARGB888, Type2Type<MINT32>());
            entry.push_back(eImgFmt_NV21, Type2Type<MINT32>());
            entry.push_back(eImgFmt_NV12, Type2Type<MINT32>());
            entry.push_back(eImgFmt_YV12,   Type2Type<MINT32>());
            entry.push_back(eImgFmt_I420,   Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        //
        cap.sort();
    }
    //JPEG
    {
        IMetadata& cap = gPortCap_JPEGO;
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_PORT_ID);
            entry.push_back(PORT_JPEGO, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_TRANSFORM);
            entry.push_back(0, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_CROP);
            entry.push_back(0, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        {
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_FORMATS);
            entry.push_back(eImgFmt_JPEG, Type2Type<MINT32>());
            cap.update(entry.tag(), entry);
        }
        //
        cap.sort();
    }
    //
    //  Pipe
    {
        IMetadata& cap = gPipeCap;
        {   // in port
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_IN_PORT_INFO);
            entry.push_back(gPortCap_IMGI, Type2Type<IMetadata>());
            entry.push_back(gPortCap_VIPI, Type2Type<IMetadata>());       //only for nr3d in normal streaming
            cap.update(entry.tag(), entry);
        }
        {   // out port
            IMetadata::IEntry entry(MTK_IOPIPE_INFO_AVAILABLE_OUT_PORT_INFO);
            entry.push_back(gPortCap_IMG2O, Type2Type<IMetadata>());
            entry.push_back(gPortCap_IMG3O, Type2Type<IMetadata>());      //only for nr3d in normal streaming
            entry.push_back(gPortCap_WROTO, Type2Type<IMetadata>());
            entry.push_back(gPortCap_WDMAO, Type2Type<IMetadata>());
            entry.push_back(gPortCap_JPEGO, Type2Type<IMetadata>());
            cap.update(entry.tag(), entry);
        }
        cap.sort();
    }
    //
    LOG_DBG("count:%d", gPipeCap.count());
    return  gPipeCap;
}
#endif

/******************************************************************************
 *
 ******************************************************************************/
INormalStream*
INormalStream::
createInstance(
    char const* szCallerName,
    NSCam::NSIoPipe::ENormalStreamTag streamTag,
    MUINT32 openedSensorIndex,
    NSCam::NSIoPipe::EScenarioFormat scenFmt)
{
    FUNCTION_LOG_START;
    //[1] create HalPipeWrapper
    FUNCTION_LOG_END;
    return new NormalStream(streamTag,openedSensorIndex,scenFmt);
}
/*******************************************************************************
*
********************************************************************************/
MBOOL INormalStream::queryCapability(IMetadata& rCapability)
{
    FUNCTION_LOG_START;
    #if 0
    rCapability = constructPortCapability();
    FUNCTION_LOG_END;
    return  !rCapability.isEmpty();
    #else
    LOG_ERR("do not support this function now ");
    return MTRUE;
    #endif
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
NormalStream::
destroyInstance(
    char const* szCallerName)
{
    FUNCTION_LOG_START;
    if(mpHalPipeWrapper==NULL)
    {
        LOG_INF("sTag(%d),NULL mpHalPipeWrapper.",mStreamTag);
    }
    else
    {
        //mpHalPipeWrapper->destroyInstance();
        mpHalPipeWrapper=NULL;
    }
    //
    if(mpPostProcPipe==NULL)
    {
        LOG_INF("sTag(%d),NULL mpPostProcPipe.",mStreamTag);
    }
    else
    {
        if ( ! mpPostProcPipe->uninit())
        {
            LOG_ERR("mpPostProcPipe uninit fail");
        }
        mpPostProcPipe->destroyInstance();
        mpPostProcPipe=NULL;
    }
    FUNCTION_LOG_END;
    delete this;
}
/*******************************************************************************
*
********************************************************************************/
NormalStream::
NormalStream(
    NSCam::NSIoPipe::ENormalStreamTag streamTag,
    MUINT32 openedSensorIndex,
    NSCam::NSIoPipe::EScenarioFormat scenFmt)    //change to use sensor index
            : mpHalPipeWrapper(NULL)
            , mpPostProcPipe(NULL)
            , mOpenedSensor(SENSOR_DEV_NONE)
            , mHalSensorList(NULL)
            , mJpegWorkBufSize(0)
            , mStreamTag(ENormalStreamTag_Stream)
            , mSWScen(eSoftwareScenario_total_num)
            , mScenFmt(NSCam::NSIoPipe::eScenarioFormat_RAW)
{
   //
   mStreamTag=streamTag;
   //
   //mpHalPipeWrapper=HalPipeWrapper::createInstance();

   #if 0//CHRISTOPHER, do not need in current stage
   if(openedSensorIndex==0xFFFF)
   {    //pure pass2
        mOpenedSensor=SENSOR_DEV_NONE;
   }
   else
   {   //p1+p2, get sensor dev firstly

       #ifndef USING_MTK_LDVT
       mHalSensorList=IHalSensorList::get();
       if(mHalSensorList)
       {
           //[1] get sensor dev index from sensorID
           MINT32 sensorDEVIdx=mHalSensorList->querySensorDevIdx(openedSensorIndex);
           mOpenedSensor=static_cast<MUINT32>(sensorDEVIdx);
       }
       else
       {
            LOG_ERR("NULL mHalSensorList,tag/sidx/sdev(%d/0x%x/0x%x)",mStreamTag,openedSensorIndex,mOpenedSensor);
       }
       #else
       //default use main sensor in ldvt load is user pass in sensor index
       mOpenedSensor=SENSOR_DEV_MAIN;
       LOG_INF("in LDVT load, sdev(0x%x)",mOpenedSensor);
       #endif


   }
   #else    //sensor index 0 and 1 means main/sub sensor repectively in v1 driver
   switch (openedSensorIndex)
   {
       case 0:
           mOpenedSensor=SENSOR_DEV_NONE;
           break;
       case 1:
           mOpenedSensor=SENSOR_DEV_MAIN;
           break;
       default:
           mOpenedSensor=SENSOR_DEV_SUB;
           break;
   }
   mScenFmt=scenFmt;
   #endif
   //default value for jpeg
   //mJpegCfg.soi_en=1; //CHRISTOPHER, do not need in current stage
   //mJpegCfg.fQuality=90;//CHRISTOPHER, do not need in current stage
   LOG_INF("tag/sidx/sdev(%d/0x%x/0x%x),swPipe fmt(%d)",mStreamTag,openedSensorIndex,mOpenedSensor,mScenFmt);
}


/*******************************************************************************
*
********************************************************************************/
NormalStream::~NormalStream()
{

}



/******************************************************************************
 *
 ******************************************************************************/
MBOOL
NormalStream::
init()
{
    FUNCTION_LOG_START;
    Mutex::Autolock autoLock(mModuleMtx);
    bool ret = true;
    MUINT32 cropPathNum=0;
    //[1] init HalPipeWrapper, and caller postproc pipe object would be created at this stage according to different sw scenario
    mSWScen=gSenDevSwScenMap[mStreamTag][(MINT32)mOpenedSensor];
    if(mSWScen==eSoftwareScenario_total_num)
    {
        LOG_ERR("No support, sTag/sensorDev (%d)/(0x%x)!",mStreamTag,mOpenedSensor);
        ret=false;
        return ret;
    }
    else
    {
        LOG_INF("sTag/sensorDev/swScen/scenFmt (%d)/(0x%x)/(%d)/(%d)",mStreamTag,mOpenedSensor,mSWScen,mScenFmt);
        //mpHalPipeWrapper->init(mSWScen,(MUINTPTR&)(this->mpPostProcPipe),mScenFmt);
    }

    CAM_TRACE_BEGIN("PostProcPipe::init");
    mpPostProcPipe = NSImageio::NSIspio::IPostProcPipe::createInstance(
        NSImageio::NSIspio::eScenarioID_VSS,
        NSImageio::NSIspio::eScenarioFmt_YUV);
    if (NULL == mpPostProcPipe)
    {
        LOG_ERR("mpPostProcPipe init fail\n");
        CAM_TRACE_END();
        return -1;
    }
    ret = mpPostProcPipe->init();
    if (ret == false)
    {
        LOG_ERR("mpPostProcPipe init fail\n");
        CAM_TRACE_END();
        return -1;
    }
    
    CAM_TRACE_END();

    ret = mpPostProcPipe->sendCommand((MINT32)NSImageio::NSIspio::EPIPECmd_SET_CQ_CHANNEL,
        (MINT32)NSImageio::NSIspio::EPIPE_PASS2_CQ1,0,0);
    ret = mpPostProcPipe->sendCommand((MINT32)NSImageio::NSIspio::EPIPECmd_SET_CONFIG_STAGE,
        (MINT32)NSImageio::NSIspio::eConfigSettingStage_Init,0,0);
    FUNCTION_LOG_END;
    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
NormalStream::
uninit()
{
    FUNCTION_LOG_START;
    Mutex::Autolock autoLock(mModuleMtx);
    bool ret = true;
    //[1] uninit postproc
    if (NULL != mpPostProcPipe)
    {
        if (true != mpPostProcPipe->uninit())
        {
            LOG_ERR("sTag(%d),caller PostProc uninit fail",mStreamTag);
            ret = false;
            return ret;
        }
    }

    //[2] uninit HalPipeWrapper
    if(mSWScen==eSoftwareScenario_total_num)
    {
        LOG_ERR("No support, sTag/sensorDev (%d)/(0x%x)!",mStreamTag,mOpenedSensor);
        ret=false;
        return ret;
    }
    else
    {
        LOG_INF("sTag/sensorDev/swScen (%d)/(0x%x)/(%d)",mStreamTag,mOpenedSensor,mSWScen);
        //mpHalPipeWrapper->uninit(mSWScen,this->mpPostProcPipe);
    }
    FUNCTION_LOG_END;
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
#define ALIGN_16(x) ( (~15) & ((15) + (x)) )
#define ALIGN_32(x) ( (~31) & ((31) + (x)) )
static
MUINT32
queryImgStride(EImageFormat const eFmt, MUINT32 const u4Width, MUINT32 const u4PlaneIndex)
{
    MUINT32 u4Stride = 0;
    //
    switch(eFmt)
    {
        // YUV 420 format
        case eImgFmt_NV21:
        case eImgFmt_NV21_BLK:
        case eImgFmt_NV12:
        case eImgFmt_NV12_BLK:
            u4Stride = (u4PlaneIndex == 2) ? (0) : u4Width ;
            break;
        case eImgFmt_YV12:
            u4Stride = (u4PlaneIndex == 0) ? ALIGN_32( u4Width ) : ALIGN_16( u4Width>>1 );
            break;
        case eImgFmt_I420:
            u4Stride = (u4PlaneIndex == 0) ? ALIGN_32( u4Width ) : ALIGN_16( u4Width>>1 );
            break;
        // YUV 422 format , RGB565
        case eImgFmt_YUY2:
        case eImgFmt_UYVY:
        case eImgFmt_VYUY:
        case eImgFmt_YVYU:
        case eImgFmt_RGB565:
            u4Stride = (u4PlaneIndex == 0) ? (u4Width) : 0;
            break;
        case eImgFmt_YV16:
        case eImgFmt_I422:
        case eImgFmt_NV16:
        case eImgFmt_NV61:
            u4Stride = (u4PlaneIndex == 0) ? (u4Width) : (u4Width >> 1);
            break;
        case eImgFmt_RGB888:
            u4Stride = u4Width;
            break;
        case eImgFmt_ARGB888:
            u4Stride = u4Width;
            break;
        case eImgFmt_JPEG:
            u4Stride = u4Width ;
            break;
        case eImgFmt_Y800:
            u4Stride = (u4PlaneIndex == 0) ? (u4Width) : (0);
            break;
        default:
            u4Stride = u4Width;
            break;
    }
    return u4Stride;
}

/******************************************************************************
 *
 ******************************************************************************/
MBOOL
NormalStream::
enque(
    QParams const& rParams)
{
    //FUNCTION_LOG_START;
    Mutex::Autolock autoLock(mModuleMtx);
    bool ret = true;
    if(mSWScen==eSoftwareScenario_total_num)
    {
        LOG_ERR("No support, sTag/sensorDev (%d)/(0x%x)!",mStreamTag,mOpenedSensor);
        ret=false;
        return ret;
    }
    else
    {
        EBufferTag tag=EBufferTag_Blocking;
        switch(mSWScen)
        {
            case eSoftwareScenario_Main_VSS_Capture:
            case eSoftwareScenario_Sub_VSS_Capture:
                tag=EBufferTag_Vss;
                break;
            default:
                break;
        }
        //QFeatureData dummyData;
        //ret=mpHalPipeWrapper->enque(mSWScen,(rParams),this->mpPostProcPipe,this->mDequeuedBufList,tag,dummyData);
        if(!ret)
        {
            LOG_ERR("enque Fail");
        }
    }

    vector<NSImageio::NSIspio::PortInfo const*> vPostProcInPorts;
    vector<NSImageio::NSIspio::PortInfo const*> vPostProcOutPorts;
    int inPorts=0,outPorts=0;
    for(int i=0;i<rParams.mvIn.size();i++)
    {
        inPorts++;
    }
    for(int i=0;i<rParams.mvOut.size();i++)
    {
        outPorts++;
    }
    vPostProcInPorts.resize(inPorts);
    vPostProcOutPorts.resize(outPorts);
    NSImageio::NSIspio::PortInfo ports[inPorts+outPorts];
    LOG_DBG("in/out(%d/%d)",inPorts,outPorts);
    int cnt = 0;
    int inCnt = 0,outCnt = 0;
    int planeNum = 0;
    int port_type = 0, port_idx = 0;
    for(int i = 0; i < rParams.mvIn.size(); i++)
    {
        //actually, only imgi need to set crop information
        planeNum = rParams.mvIn[i].mBuffer->getPlaneCount();
        ports[cnt].u4ImgWidth  = rParams.mvIn[i].mBuffer->getImgSize().w;
        ports[cnt].u4ImgHeight = rParams.mvIn[i].mBuffer->getImgSize().h;

        ports[cnt].crop.x      = 0;
        ports[cnt].crop.floatX = 0;
        ports[cnt].crop.y      = 0;
        ports[cnt].crop.floatY = 0;
        ports[cnt].crop.w      = 0;
        ports[cnt].crop.h      = 0;
        for(int k=0;k<rParams.mvCropRsInfo.size();k++)
        {
            switch(rParams.mvCropRsInfo[k].mGroupID)
            {
                case 1:
                    ports[cnt].crop.x      = rParams.mvCropRsInfo[k].mCropRect.p_integral.x;
                    ports[cnt].crop.floatX = rParams.mvCropRsInfo[k].mCropRect.p_fractional.x;
                    ports[cnt].crop.y      = rParams.mvCropRsInfo[k].mCropRect.p_integral.y;
                    ports[cnt].crop.floatY = rParams.mvCropRsInfo[k].mCropRect.p_fractional.y;
                    ports[cnt].crop.w      = rParams.mvCropRsInfo[k].mCropRect.s.w;
                    ports[cnt].crop.h      = rParams.mvCropRsInfo[k].mCropRect.s.h;
                    break;
                default:
                    LOG_INF("DO not support crop group %d", rParams.mvCropRsInfo[k].mGroupID);
                    break;
            }
        }
        LOG_DBG("[Crop] g1 x/fx/y/fy/w/h/(%d/%d/%d/%d/%d/%d)",\
            ports[cnt].crop.x,ports[cnt].crop.floatX,ports[cnt].crop.y,ports[cnt].crop.floatY,ports[cnt].crop.w,ports[cnt].crop.h);

        ports[cnt].eImgFmt     = (EImageFormat)(rParams.mvIn[i].mBuffer->getImgFormat());
        ports[cnt].u4IsRunSegment = 0;
        ports[cnt].type = NSImageio::NSIspio::EPortType_Memory;
        ports[cnt].index = NSImageio::NSIspio::EPortIndex_IMGI;
        ports[cnt].inout  = NSImageio::NSIspio::EPortDirection_In;
        ports[cnt].pipePass = NSImageio::NSIspio::EPipePass_PASS2;

        ports[cnt].u4Stride[0] = ports[cnt].u4ImgWidth;
        ports[cnt].u4Stride[1] = ports[cnt].u4Stride[2] = 0;
        //imgi only have 1 plane
        ports[cnt].u4BufSize= rParams.mvIn[i].mBuffer->getBufSizeInBytes(0);
        ports[cnt].u4BufVA  = rParams.mvIn[i].mBuffer->getBufVA(0);
        ports[cnt].u4BufPA  = rParams.mvIn[i].mBuffer->getBufPA(0);
        ports[cnt].memID    = rParams.mvIn[i].mBuffer->getFD(0);

        LOG_INF("[mvIn (%d)] fmt(%d),w/h(%d/%d),stride(%d/%d/%d),mem( va(0x%x)/pa(0x%x)/size(0x%x) )",\
            ports[cnt].index,ports[cnt].eImgFmt,ports[cnt].u4ImgWidth,ports[cnt].u4ImgHeight,\
            ports[cnt].u4Stride[0],ports[cnt].u4Stride[1],ports[cnt].u4Stride[2],ports[cnt].u4BufVA,ports[cnt].u4BufPA,ports[cnt].u4BufSize);

        vPostProcInPorts.at(inCnt)=&(ports[cnt]);
        cnt++;
        inCnt++;
    }

    for(int i=0;i<rParams.mvOut.size();i++)
    {
        ports[cnt].u4Stride[0]=ports[cnt].u4Stride[1]=ports[cnt].u4Stride[2]=0;
        planeNum = rParams.mvOut[i].mBuffer->getPlaneCount();

        switch(rParams.mvOut[i].mPortID.index)
        {
            case NSImageio::NSIspio::EPortIndex_VIDO:
            case NSImageio::NSIspio::EPortIndex_WROTO:
                port_type = NSImageio::NSIspio::EPortType_VID_RDMA;
                port_idx = NSImageio::NSIspio::EPortIndex_VIDO;
                break;
            case NSImageio::NSIspio::EPortIndex_DISPO:
            case NSImageio::NSIspio::EPortIndex_WDMAO:
                port_type = NSImageio::NSIspio::EPortType_DISP_RDMA;
                port_idx = NSImageio::NSIspio::EPortIndex_DISPO;
                break;
            case NSImageio::NSIspio::EPortIndex_IMG2O:
            default:
                LOG_ERR("unsupported port index %d", rParams.mvOut[i].mPortID.index);
                break;
        }

        //
        ports[cnt].eImgFmt     = (EImageFormat)(rParams.mvOut[i].mBuffer->getImgFormat());
        ports[cnt].type        = port_type;
        ports[cnt].index       = port_idx; //need map to index defined in imageio
        ports[cnt].inout       = rParams.mvOut[i].mPortID.inout;
        ports[cnt].eImgRot     = NSImageio::NSIspio::eImgRot_0;
        ports[cnt].eImgFlip    = NSImageio::NSIspio::eImgFlip_OFF;
        ports[cnt].u4ImgWidth  = rParams.mvOut[i].mBuffer->getImgSize().w;
        ports[cnt].u4ImgHeight = rParams.mvOut[i].mBuffer->getImgSize().h;
        for(int k=0;k<planeNum;k++)
        {
            //ports[cnt].u4Stride[k] = rParams.mvOut[i].mBuffer->getBufStridesInBytes(k);
            ports[cnt].u4Stride[k] = queryImgStride(ports[cnt].eImgFmt, ports[cnt].u4ImgWidth, k);
            #if 0 //CHRISTOPHER, do not need in current stage
            ports[cnt].u4BufSize[k]= rParams.mvOut[i].mBuffer->getBufSizeInBytes(k);
            ports[cnt].u4BufVA[k]  = rParams.mvOut[i].mBuffer->getBufVA(k);
            ports[cnt].u4BufPA[k]  = rParams.mvOut[i].mBuffer->getBufPA(k);
            ports[cnt].memID[k]    = rParams.mvOut[i].mBuffer->getFD(k);
            #else //do not support non-continuous buffer in v1 driver
            ports[cnt].u4BufSize += rParams.mvOut[i].mBuffer->getBufSizeInBytes(0);
            ports[cnt].u4BufVA  = rParams.mvOut[i].mBuffer->getBufVA(0);
            ports[cnt].u4BufPA  = rParams.mvOut[i].mBuffer->getBufPA(0);
            ports[cnt].memID    = rParams.mvOut[i].mBuffer->getFD(0);
            #endif
        }
        LOG_INF("[mvOut (%d)] fmt(%d),w/h(%d/%d),stride(%d/%d/%d),mem( va(0x%x)/pa(0x%x)/size(0x%x) )",\
            ports[cnt].index,ports[cnt].eImgFmt,ports[cnt].u4ImgWidth,ports[cnt].u4ImgHeight,\
            ports[cnt].u4Stride[0],ports[cnt].u4Stride[1],ports[cnt].u4Stride[2],ports[cnt].u4BufVA,ports[cnt].u4BufPA,ports[cnt].u4BufSize);

        vPostProcOutPorts.at(outCnt)=&(ports[cnt]);
        cnt++;
        outCnt++;
    }

    CAM_TRACE_BEGIN("PostProc configPipe");
    ret = mpPostProcPipe->configPipe(vPostProcInPorts, vPostProcOutPorts);
    CAM_TRACE_END();
    if(!ret)
    {
        LOG_ERR("postprocPipe config fail");
        return ret;
    }

        //enque buffer for input
    NSImageio::NSIspio::PortID rPortID(NSImageio::NSIspio::EPortType_Memory,NSImageio::NSIspio::EPortIndex_IMGI,0);
    NSImageio::NSIspio::QBufInfo rQBufInfo;
    for(int i=0;i<vPostProcInPorts.size();i++)
    {
        rQBufInfo.vBufInfo.resize(1);
        rQBufInfo.vBufInfo[0].u4BufSize = vPostProcInPorts[i]->u4BufSize; //bytes
        rQBufInfo.vBufInfo[0].u4BufVA = vPostProcInPorts[i]->u4BufVA;
        rQBufInfo.vBufInfo[0].u4BufPA = vPostProcInPorts[i]->u4BufPA;
        rQBufInfo.vBufInfo[0].memID = vPostProcInPorts[i]->memID;
        CAM_TRACE_BEGIN("PostProc enque in");
        ret=mpPostProcPipe->enqueInBuf(rPortID, rQBufInfo);
        CAM_TRACE_END();
        if(!ret)
        {
            LOG_ERR("postprocPipe enque in buffer fail");
            return ret;
        }
    }
            //enque buffer for output
    for(int i=0;i<vPostProcOutPorts.size();i++)
    {
        rPortID.type = vPostProcOutPorts[i]->type;
        rPortID.index = vPostProcOutPorts[i]->index;

        rQBufInfo.vBufInfo.resize(1);
        rQBufInfo.vBufInfo[0].u4BufSize = vPostProcOutPorts[i]->u4BufSize; //bytes
        rQBufInfo.vBufInfo[0].u4BufVA = vPostProcOutPorts[i]->u4BufVA;
        rQBufInfo.vBufInfo[0].u4BufPA = vPostProcOutPorts[i]->u4BufPA;
        rQBufInfo.vBufInfo[0].memID = vPostProcOutPorts[i]->memID;
        CAM_TRACE_BEGIN("PostProc enque out");
        ret=mpPostProcPipe->enqueOutBuf(rPortID, rQBufInfo);
        CAM_TRACE_END();
        if(!ret)
        {
            LOG_ERR("postprocPipe enque in buffer fail");
            return ret;
        }
    }

    mpPostProcPipe->sendCommand(NSImageio::NSIspio::EPIPECmd_SET_CONFIG_STAGE, (MINT32)NSImageio::NSIspio::eConfigSettingStage_UpdateTrigger, 0, 0);
    //FUNCTION_LOG_END;
    return ret;

}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
NormalStream::
deque(
    QParams& rParams,
    MINT64 i8TimeoutNs)
{
    FUNCTION_LOG_START;
    Mutex::Autolock autoLock(mModuleMtx);
    bool ret = false;

    if(i8TimeoutNs == -1)   //temp solution for infinite wait
    {
        LOG_INF("no timeout set, infinite wait");
        i8TimeoutNs=8000000000;
    }

    LOG_DBG("sTag(%d),i8TimeoutNs(%lld)",mStreamTag,i8TimeoutNs);
    //QFeatureData dummyData;
    //ret=mpHalPipeWrapper->deque((rParams),this->mpPostProcPipe,0/*(MUINT32)(&mDequeuedBufList)*/,i8TimeoutNs,dummyData);

    mpPostProcPipe->sendCommand(NSImageio::NSIspio::EPIPECmd_SET_CURRENT_BUFFER, NSImageio::NSIspio::EPortIndex_IMGI,0,0);

    for(int i = 0; i < rParams.mvOut.size(); i++) {
        int port_idx = -1;
        switch(rParams.mvOut[i].mPortID.index)
        {
            case NSImageio::NSIspio::EPortIndex_VIDO:
            case NSImageio::NSIspio::EPortIndex_WROTO:
                port_idx = NSImageio::NSIspio::EPortIndex_VIDO;
                break;
            case NSImageio::NSIspio::EPortIndex_DISPO:
            case NSImageio::NSIspio::EPortIndex_WDMAO:
                port_idx = NSImageio::NSIspio::EPortIndex_DISPO;
                break;
            case NSImageio::NSIspio::EPortIndex_IMG2O:
            default:
                LOG_ERR("unsupported port index %d", rParams.mvOut[i].mPortID.index);
                break;
        }
        if(port_idx != -1)
            mpPostProcPipe->sendCommand(NSImageio::NSIspio::EPIPECmd_SET_CURRENT_BUFFER, port_idx, 0, 0);
    }
    //
    CAM_TRACE_BEGIN("PostProc start");
    mpPostProcPipe->start();
    CAM_TRACE_END();
    mpPostProcPipe->irq(NSImageio::NSIspio::EPipePass_PASS2, NSImageio::NSIspio::EPIPEIRQ_PATH_DONE);

    for(int i = 0; i < rParams.mvOut.size(); i++) {
        NSImageio::NSIspio::PortID rPortID;
        rPortID.index = -1;
        QTimeStampBufInfo bufInfo;
        switch(rParams.mvOut[i].mPortID.index)
        {
            case NSImageio::NSIspio::EPortIndex_VIDO:
            case NSImageio::NSIspio::EPortIndex_WROTO:
                rPortID.index = NSImageio::NSIspio::EPortIndex_VIDO;
                break;
            case NSImageio::NSIspio::EPortIndex_DISPO:
            case NSImageio::NSIspio::EPortIndex_WDMAO:
                rPortID.index = NSImageio::NSIspio::EPortIndex_DISPO;
                break;
            case NSImageio::NSIspio::EPortIndex_IMG2O:
            default:
                LOG_DBG("unsupported port index %d", rParams.mvOut[i].mPortID.index);
                break;
        }
        if(rPortID.index != -1) {
            CAM_TRACE_BEGIN("PostProc deque");
            ret = mpPostProcPipe->dequeOutBuf(rPortID, bufInfo);
            CAM_TRACE_END();
            if(!ret)
                LOG_ERR("deque port %d failed", rPortID.index);
        }
    }

    {
        NSImageio::NSIspio::PortID rPortID;
        rPortID.index = NSImageio::NSIspio::EPortIndex_IMGI;
        QTimeStampBufInfo dummy;
        CAM_TRACE_BEGIN("PostProc deque imgi");
        mpPostProcPipe->dequeInBuf(rPortID, dummy);
        CAM_TRACE_END();
    }
    mpPostProcPipe->stop();
    rParams.mDequeSuccess = ret;
    FUNCTION_LOG_END;
    return ret;
}


/******************************************************************************
*
******************************************************************************/
MBOOL
NormalStream::
queryCropInfo(
    android::Vector<MCropPathInfo>& mvCropPathInfo)
{
    #if 0 //CHRISTOPHER, do not need in current stage
    for(int i=0;i<mCropPaths.size();i++)
    {

        MCropPathInfo crop;
        crop.mGroupIdx=mCropPaths[i].u4CropGroup;
        for(int j=0;j<mCropPaths[i].PortIdxVec.size();j++)
        {
            crop.mvPorts.push_back(mCropPaths[i].PortIdxVec[j]);
        }
        mvCropPathInfo.push_back(crop);
    }
    #else
    LOG_ERR("do not support this function now");
    #endif
    return true;
}
/******************************************************************************
*
******************************************************************************/

MBOOL
NormalStream::
setJpegParam(
    EJpgCmd jpgCmd,
    int arg1,
    int arg2)
{
    FUNCTION_LOG_START;
    bool ret = true;
    #if 0 //CHRISTOPHER, do not need in current stage

    if(mpPostProcPipe==NULL)
    {
        LOG_ERR("sTag(%d),NULL mpPostProcPipe",mStreamTag);
        return false;
    }
    switch(jpgCmd)
    {
        case EJpgCmd_SetWorkBufSize:    //setJpegParam(EJpgCmd_SetWorkBufSize,0,0)
            mpPostProcPipe->sendCommand(EPIPECmd_SET_JPEG_WORKBUF_SIZE, mJpegWorkBufSize,0,0);
            break;
        case EJpgCmd_SetQualityParam:
            mJpegCfg.soi_en=arg1;
            mJpegCfg.fQuality=arg2;     //setJpegParam(EJpgCmd_SetQualityParam,soi_en,fQuality)
            mpPostProcPipe->sendCommand(EPIPECmd_SET_JPEG_CFG, (MINT32)&mJpegCfg,0,0);
            break;
        default:
            break;
    }
    #else
    LOG_ERR("do not support this function now ");
    #endif
    FUNCTION_LOG_END;
    return ret;
}
/******************************************************************************
*
******************************************************************************/
MBOOL
NormalStream::
startVideoRecord(
    MINT32 wd,
    MINT32 ht,
    MINT32 fps)
{
    //LOG_ERR("[Error]camera1.0 not support this function");
    return true;
}

/******************************************************************************
*
******************************************************************************/
MBOOL
NormalStream::
stopVideoRecord()
{
    //LOG_ERR("[Error]camera1.0 not support this function");
    return true;
}

/******************************************************************************
*
******************************************************************************/
MBOOL
NormalStream::
deTuningQue(
    unsigned int& size,
    void* &pTuningQueBuf)
{
    //LOG_ERR("[Error]camera1.0 not support this function");
    //
    return true;
}
/******************************************************************************
*
******************************************************************************/
MBOOL
NormalStream::
enTuningQue(
    void* pTuningQueBuf)
{
    //LOG_ERR("[Error]camera1.0 not support this function");
    //
    return true;
}




