/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

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
#define LOG_TAG "camsv_buf_mgr"

#ifndef ENABLE_MY_LOG
#define ENABLE_MY_LOG       (1)
#endif

#define ENABLE_AE_MVHDR_STAT       (1)

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <cutils/atomic.h>

#include <sys/stat.h>
#include <cutils/properties.h>
#include <string.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <aaa_log.h>
#include <camera_custom_nvram.h>
#include <awb_feature.h>
#include <awb_param.h>
#include <awb_tuning_custom.h>
//#include <ispdrv_mgr.h>
#include "drv/tuning_mgr.h"
#include <isp_tuning.h>
//#include <linux/cache.h>
#include <utils/threads.h>
#include <list>
#include <drv/isp_reg.h>
#include <mtkcam/drv/IHalSensor.h>
#include "camsv_buf_mgr.h"
#include <sys/mman.h>
#include <drv/isp_reg.h>
#include <imageio/ispio_utility.h>
#include <iopipe/CamIO/PortMap.h>
#include <semaphore.h>
#include <mtkcam/def/PriorityDefs.h>

using namespace std;
using namespace android;
using namespace NS3Av3;
using namespace NSIspTuning;
using namespace NSCam;
using namespace NSIoPipe;
//using namespace NSCamIOPipe;

typedef list<BufInfo_T> BufInfoList_T;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  AAO buffer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define AAO_OFFSET_ADDR (0)
#define AAO_YSIZE (0)
#define AAO_STRIDE_BUS_SIZE (3)

#define MAX_AAO_BUFFER_CNT (3)



typedef struct
{
    MUINT8                 m_u1NumBit; //8 bit:3, 10bit:2, 12bit:1, 14bit:0
    MUINT32                m_u4AAOBufSize;
    MUINT32                m_u4AAOXSize;
    MUINT32                m_u4AAOYSize;
    BufInfo_T              m_rAAOBufInfo[MAX_AAO_BUFFER_CNT];
} SAE_BUF_CTRL_T;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CamSVBufMgr_Thread;

class CamSVBufMgr
{
friend class CamSVBufMgr_Thread;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Ctor/Dtor.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    //  Copy constructor is disallowed.
    CamSVBufMgr(CamSVBufMgr const&);
    //  Copy-assignment operator is disallowed.
    CamSVBufMgr& operator=(CamSVBufMgr const&);

public:
    CamSVBufMgr(ESensorDev_T const eSensorDev);
    ~CamSVBufMgr();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    MBOOL debugPrint();
    MBOOL allocateBuf(BufInfo_T &rBufInfo, MUINT32 u4BufSize);
    MBOOL freeBuf(BufInfo_T &rBufInfo);
    MBOOL deleteBuf();

public:
    static CamSVBufMgr& getInstance(MINT32 const i4SensorDev);
    MBOOL AAStatEnable(MBOOL En);
    MBOOL init(MINT32 const i4SensorIdx);
    MBOOL uninit();
    MBOOL DMAInit();
    MBOOL DMAUninit();
    MBOOL enqueueHwBuf(BufInfo_T& rBufInfo, MINT32 PDSel=0);
    MBOOL dequeueHwBuf(BufInfo_T& rBufInfo, MINT32 i4PDSel=0, MUINT32 i4FrmNum=0, ESTT_CACHE_SYNC_POLICY cacheSyncPolicy=ESTT_CacheInvalidByRange);
    MBOOL updateDMABaseAddr(MUINT32 u4BaseAddr, MINT32 PDSel=0);
    MUINT32 getCurrHwBuf();
    MUINT32 getNextHwBuf(MINT32 PDSel=0);
    MBOOL newBuf();
    void changeHwBufStatus();
    void clearHwBufStatus();
    MUINT32 getAvailHwBuf();
    void dumpMyBuff();

    inline MBOOL setTGInfo(MINT32 const i4TGInfo)
    {
        MY_LOG("[%s()] i4TGInfo: %d \n", __FUNCTION__, i4TGInfo);

        switch (i4TGInfo)
        {
        case CAM_TG_1:
            m_eSensorTG = ESensorTG_1;
            m_TgInfo = CAM_TG_1;
            break;
        case CAM_TG_2:
            m_eSensorTG = ESensorTG_2;
            m_TgInfo = CAM_TG_2;
            break;
        case CAM_SV_1:
            m_TgInfo = CAM_SV_1;
            break;
        case CAM_SV_2:
            m_TgInfo = CAM_SV_2;
            break;
        default:
            MY_ERR("i4TGInfo = %d", i4TGInfo);
            return MFALSE;
        }

        return MTRUE;
    }


    inline MBOOL setSensorMode(MINT32 i4NewSensorMode)
    {
        MY_LOG("[%s()] m_eSensorDev: %d Sensor mode: %d \n", __FUNCTION__, m_eSensorDev, i4NewSensorMode);

        m_SensorMode = i4NewSensorMode;

        return MTRUE;
    }


    inline MBOOL isDMAInit( MBOOL &isInited)
    {
        isInited = m_u4IsDMAinited;
        return MTRUE;
    }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data member
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    ESensorDev_T const     m_eSensorDev;
    ESensorTG_T            m_eSensorTG;
    IMemDrv*               m_pIMemDrv;
    IspDrv*                m_pIspDrv;
    MUINT32                m_u4AEStateSize;
    MUINT32                m_u4AEHistSize;
    MUINT32                m_u4AWBStateSize;
    MUINT                  m_pixelMode;
    SAE_BUF_CTRL_T     m_PDBufCtrl;
    SAE_BUF_CTRL_T     m_MVHDRBufCtrl;

    BufInfoList_T          m_rPDHwBufList;
    BufInfoList_T          m_rMVHDRHwBufList;
    volatile MINT32        m_Users;
    mutable android::Mutex m_Lock;
    MBOOL                  m_EnMVHDR;
    MBOOL                  m_EnPDAF;
    MBOOL                  m_u4IsDMAinited;
    MBOOL                  m_bDebugEnable;
    MINT32                 m_i4SensorIdx;
    unsigned long *mpIspHwRegAddr;
    int mfd;
    MINT32                 m_i4CurrSensorId;
    MINT32             m_SensorMode;
    MBOOL                  m_FirstTimeCreateBuf;
    sem_t                  m_semPD;
    sem_t                  m_semMVHDR;

    MUINT                  m_TgInfo;
    BufInfo_T              m_rPDHwDeqBuf;
    BufInfo_T              m_rMVHDRHwDeqBuf;
    MINT32                 m_i4CamSVIrqKey;
    CamSVBufMgr_Thread*       m_pCamSVBufMgr_Thread;
};

/*****************************************************
*
*****************************************************/
class CamSVBufMgr_Thread
{
public:
    CamSVBufMgr_Thread(CamSVBufMgr* camsvBufMgr);
    ~CamSVBufMgr_Thread(){};
    static CamSVBufMgr_Thread* CreateInstance(CamSVBufMgr* obj);
    MBOOL DestroyInstance(void);
    MBOOL init(void);
    MBOOL uninit(void);
    MBOOL start(void);
    MBOOL stop(void);
    static MVOID* DmaUpdateThread(void *arg);
private:
    sem_t                  m_semSof;
    MBOOL                  m_bDmaUpdateStart;
    pthread_t              m_DmaUpdateThread;
    CamSVBufMgr*              m_pCamSVBufMgr;
    mutable android::Mutex m_Lock;
};

CamSVBufMgr_Thread::CamSVBufMgr_Thread(CamSVBufMgr* camsvBufMgr)
{
    m_pCamSVBufMgr = camsvBufMgr;
    m_bDmaUpdateStart = MFALSE;
    m_semSof.count = 0;
    m_DmaUpdateThread = 0;
}

CamSVBufMgr_Thread* CamSVBufMgr_Thread::CreateInstance(CamSVBufMgr* obj)
{
    if(obj == 0)
    {
        MY_LOG("CamSVBufMgr_Thread create instance fail(0x%x)",(MUINTPTR)obj);
        return NULL;
    }

    return new CamSVBufMgr_Thread(obj);
}

MBOOL CamSVBufMgr_Thread::DestroyInstance(void)
{
    delete this;
    return MTRUE;
}

MBOOL CamSVBufMgr_Thread::init(void)
{
    MBOOL ret = MTRUE;

    // Init semphore
    ::sem_init(&this->m_semSof, 0, 0);

    // Create main thread for preview and capture
    pthread_create(&this->m_DmaUpdateThread, NULL, DmaUpdateThread, this);
    //
    return ret;
}
MBOOL CamSVBufMgr_Thread::uninit(void)
{
    return MTRUE;
}
MBOOL CamSVBufMgr_Thread::start(void)
{
    if (!this->m_bDmaUpdateStart) {
        this->m_bDmaUpdateStart = MTRUE;
        ::sem_post(&this->m_semSof);
    }
    return MTRUE;
}
MBOOL CamSVBufMgr_Thread::stop(void)
{
    this->m_bDmaUpdateStart = MFALSE;
    ::sem_wait(&this->m_semSof); // wait until thread prepare to leave
    return MTRUE;
}

MVOID* CamSVBufMgr_Thread::DmaUpdateThread(void *arg)
{
    CamSVBufMgr_Thread* _this = reinterpret_cast<CamSVBufMgr_Thread*>(arg);

    /// set policy/priority
    int const policy    = SCHED_OTHER;
    int const priority  = NICE_CAMERA_AF;
    //
    struct sched_param sched_p;

    ::sched_getparam(0, &sched_p);
    if(policy == SCHED_OTHER)
    {
        NSCam::Utils::setThreadPriority(policy, priority);
    }
    else
    {   //  Note: "priority" is real-time priority.
        sched_p.sched_priority = priority;
        ::sched_setscheduler(0, policy, &sched_p);
    }
    //  detach thread => cannot be join, it means that thread would release resource after exit
    ::pthread_detach(::pthread_self());

    ::sem_wait(&_this->m_semSof);
    MY_LOG("Warning:temp borrow NICE_CAMERA_AF priority, need to create self priority\n");
    //
    while (1)
    {

        _this->m_Lock.lock();
        if(_this->m_bDmaUpdateStart == MFALSE){
            _this->m_Lock.unlock();
            MY_LOG("DmaUpdateThread leave\n");
            break;
        }
        _this->m_Lock.unlock();
        //
        // TODO: wait SOF IRQ and update DMA address
        ISP_DRV_WAIT_IRQ_STRUCT rWaitIrq;

        rWaitIrq.Clear            = ISP_DRV_IRQ_CLEAR_WAIT;
        rWaitIrq.UserInfo.UserKey = _this->m_pCamSVBufMgr->m_i4CamSVIrqKey;
        rWaitIrq.Timeout          = 500;//ms
        rWaitIrq.bDumpReg         = 0;
        rWaitIrq.UserNumber       = 0; //must be 0 for Hal3 path

        switch (_this->m_pCamSVBufMgr->m_TgInfo)
        {
            case CAM_SV_1: // 0x10
                rWaitIrq.UserInfo.Type    = ISP_DRV_IRQ_TYPE_INT_CAMSV; //CamSv path
                rWaitIrq.UserInfo.Status  = CAMSV_INT_STATUS_TG_SOF1_ST; //CamSV SOF
                break;
            case CAM_SV_2: // 0x11
                rWaitIrq.UserInfo.Type    = ISP_DRV_IRQ_TYPE_INT_CAMSV2; //CamSV2 path
                rWaitIrq.UserInfo.Status  = CAMSV2_INT_STATUS_TG_SOF1_ST; //CamSV2 SOF
                break;
            default:
                MY_ERR("[%s()]m_TgInfo is invalid: %d", __FUNCTION__, _this->m_pCamSVBufMgr->m_TgInfo);
                break;
        }

        // wait CAMSV DONE
        if (_this->m_pCamSVBufMgr->m_pIspDrv->waitIrq(&rWaitIrq) <= 0) {
            MY_ERR("[%s()]waitIrq CAMSV/CAMSV2 SOF fail", __FUNCTION__);
            break;
        }

        // update next DMA address
        switch (_this->m_pCamSVBufMgr->m_TgInfo)
        {
            case CAM_SV_1:
                _this->m_pCamSVBufMgr->m_Lock.lock();
                _this->m_pCamSVBufMgr->updateDMABaseAddr(_this->m_pCamSVBufMgr->getNextHwBuf(!_PD_BUF_SEL_), !_PD_BUF_SEL_);
                _this->m_pCamSVBufMgr->m_Lock.unlock();
                break;
            case CAM_SV_2:
                _this->m_pCamSVBufMgr->m_Lock.lock();
                _this->m_pCamSVBufMgr->changeHwBufStatus();
                _this->m_pCamSVBufMgr->updateDMABaseAddr(_this->m_pCamSVBufMgr->getAvailHwBuf(), _PD_BUF_SEL_);
                _this->m_pCamSVBufMgr->m_Lock.unlock();
                break;
            default:
                MY_ERR("[%s()] updateDMABaseAddr fail\n", __FUNCTION__);
                goto _exit;
                break;
        }

        //check again for stop isp, it will flush DmaUpdateThread SOF IRQ
        _this->m_Lock.lock();
        if(_this->m_bDmaUpdateStart == MFALSE){
            _this->m_Lock.unlock();
            MY_LOG("[%s()]DmaUpdateThread leave after wait SOF\n", __FUNCTION__);
            break;
        }
        _this->m_Lock.unlock();
        //
    }
_exit:
    _this->m_bDmaUpdateStart = MFALSE; //stop for exceptional case
    ::sem_post(&_this->m_semSof);
    //

    return NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template <ESensorDev_T const eSensorDev>
class CamSVBufMgrDev : public CamSVBufMgr
{
public:
    static
    CamSVBufMgr&
    getInstance()
    {
        static CamSVBufMgrDev<eSensorDev> singleton;
        return singleton;
    }

    CamSVBufMgrDev()
        : CamSVBufMgr(eSensorDev)
    {}

    virtual ~CamSVBufMgrDev() {}
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ICamSVBufMgr::
init(MINT32 const i4SensorDev, MINT32 const i4SensorIdx)
{
    MBOOL ret_main, ret_sub, ret_main2;
    ret_main = ret_sub = ret_main2 = MTRUE;

    if (i4SensorDev & ESensorDev_Main)
        ret_main = CamSVBufMgr::getInstance(ESensorDev_Main).init(i4SensorIdx);
    if (i4SensorDev & ESensorDev_Sub)
        ret_sub = CamSVBufMgr::getInstance(ESensorDev_Sub).init(i4SensorIdx);
    if (i4SensorDev & ESensorDev_MainSecond)
        ret_main2 = CamSVBufMgr::getInstance(ESensorDev_MainSecond).init(i4SensorIdx);

    return ret_main && ret_sub && ret_main2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ICamSVBufMgr::
uninit(MINT32 const i4SensorDev)
{
    MBOOL ret_main, ret_sub, ret_main2;
    ret_main = ret_sub = ret_main2 = MTRUE;

    if (i4SensorDev & ESensorDev_Main)
        ret_main = CamSVBufMgr::getInstance(ESensorDev_Main).uninit();
    if (i4SensorDev & ESensorDev_Sub)
        ret_sub = CamSVBufMgr::getInstance(ESensorDev_Sub).uninit();
    if (i4SensorDev & ESensorDev_MainSecond)
        ret_main2 = CamSVBufMgr::getInstance(ESensorDev_MainSecond).uninit();

    return ret_main && ret_sub && ret_main2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ICamSVBufMgr::
start(MINT32 const i4SensorDev)
{
    MBOOL ret_main, ret_sub, ret_main2;
    ret_main = ret_sub = ret_main2 = MTRUE;

    if (i4SensorDev & ESensorDev_Main)
        ret_main = CamSVBufMgr::getInstance(ESensorDev_Main).DMAInit();
    if (i4SensorDev & ESensorDev_Sub)
        ret_sub = CamSVBufMgr::getInstance(ESensorDev_Sub).DMAInit();
    if (i4SensorDev & ESensorDev_MainSecond)
        ret_main2 = CamSVBufMgr::getInstance(ESensorDev_MainSecond).DMAInit();

    return ret_main && ret_sub && ret_main2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ICamSVBufMgr::
stop(MINT32 const i4SensorDev)
{
    MBOOL ret_main, ret_sub, ret_main2;
    ret_main = ret_sub = ret_main2 = MTRUE;

    if (i4SensorDev & ESensorDev_Main)
        ret_main = CamSVBufMgr::getInstance(ESensorDev_Main).DMAUninit();
    if (i4SensorDev & ESensorDev_Sub)
        ret_sub = CamSVBufMgr::getInstance(ESensorDev_Sub).DMAUninit();
    if (i4SensorDev & ESensorDev_MainSecond)
        ret_main2 = CamSVBufMgr::getInstance(ESensorDev_MainSecond).DMAUninit();

    return ret_main && ret_sub && ret_main2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ICamSVBufMgr::
AAStatEnable(MINT32 const i4SensorDev, MBOOL En)
{
    MBOOL ret_main, ret_sub, ret_main2;
    ret_main = ret_sub = ret_main2 = MTRUE;

    if (i4SensorDev & ESensorDev_Main)
        ret_main = CamSVBufMgr::getInstance(ESensorDev_Main).AAStatEnable(En);
    if (i4SensorDev & ESensorDev_Sub)
        ret_sub = CamSVBufMgr::getInstance(ESensorDev_Sub).AAStatEnable(En);
    if (i4SensorDev & ESensorDev_MainSecond)
        ret_main2 = CamSVBufMgr::getInstance(ESensorDev_MainSecond).AAStatEnable(En);

    return ret_main && ret_sub && ret_main2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ICamSVBufMgr::
enqueueHwBuf(MINT32 const i4SensorDev, BufInfo_T& rBufInfo, MINT32 PDSel)
{
    if (i4SensorDev & ESensorDev_Main)
        return CamSVBufMgr::getInstance(ESensorDev_Main).enqueueHwBuf(rBufInfo, PDSel);
    else if (i4SensorDev & ESensorDev_Sub)
        return CamSVBufMgr::getInstance(ESensorDev_Sub).enqueueHwBuf(rBufInfo, PDSel);
    else if (i4SensorDev & ESensorDev_MainSecond)
        return CamSVBufMgr::getInstance(ESensorDev_MainSecond).enqueueHwBuf(rBufInfo, PDSel);

    MY_ERR("Incorrect sensor device: i4SensorDev = %d", i4SensorDev);
    return MFALSE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ICamSVBufMgr::
dequeueHwBuf(MINT32 const i4SensorDev, BufInfo_T& rBufInfo, MINT32 i4PDSel, MUINT32 i4FrmNum, ESTT_CACHE_SYNC_POLICY cacheSyncPolicy)
{
    if (i4SensorDev & ESensorDev_Main)
        return CamSVBufMgr::getInstance(ESensorDev_Main).dequeueHwBuf(rBufInfo, i4PDSel, i4FrmNum, cacheSyncPolicy);
    else if (i4SensorDev & ESensorDev_Sub)
        return CamSVBufMgr::getInstance(ESensorDev_Sub).dequeueHwBuf(rBufInfo, i4PDSel, i4FrmNum, cacheSyncPolicy);
    else if (i4SensorDev & ESensorDev_MainSecond)
        return CamSVBufMgr::getInstance(ESensorDev_MainSecond).dequeueHwBuf(rBufInfo, i4PDSel, i4FrmNum, cacheSyncPolicy);

    MY_ERR("Incorrect sensor device: i4SensorDev = %d", i4SensorDev);
    return MFALSE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
MBOOL
ICamSVBufMgr::
updateDMABaseAddr(MINT32 const i4SensorDev, MINT32 PDSel)
{
    if (i4SensorDev & ESensorDev_Main)
        return CamSVBufMgr::getInstance(ESensorDev_Main).updateDMABaseAddr(CamSVBufMgr::getInstance(ESensorDev_Main).getCurrHwBuf(), PDSel);
    else if (i4SensorDev & ESensorDev_Sub)
        return CamSVBufMgr::getInstance(ESensorDev_Sub).updateDMABaseAddr(CamSVBufMgr::getInstance(ESensorDev_Sub).getCurrHwBuf(), PDSel);
    else if (i4SensorDev & ESensorDev_MainSecond)
        return CamSVBufMgr::getInstance(ESensorDev_MainSecond).updateDMABaseAddr(CamSVBufMgr::getInstance(ESensorDev_MainSecond).getCurrHwBuf(), PDSel);

    MY_ERR("Incorrect sensor device: i4SensorDev = %d", i4SensorDev);
    return MFALSE;
}
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ICamSVBufMgr::
setTGInfo(MINT32 const i4SensorDev, MINT32 const i4TGInfo)
{
    if (i4SensorDev & ESensorDev_Main)
        return CamSVBufMgr::getInstance(ESensorDev_Main).setTGInfo(i4TGInfo);
    else if (i4SensorDev & ESensorDev_Sub)
        return CamSVBufMgr::getInstance(ESensorDev_Sub).setTGInfo(i4TGInfo);
    else if (i4SensorDev & ESensorDev_MainSecond)
        return CamSVBufMgr::getInstance(ESensorDev_MainSecond).setTGInfo(i4TGInfo);

    MY_ERR("Incorrect sensor device: i4SensorDev = %d", i4SensorDev);
    return MFALSE;
}


MRESULT
ICamSVBufMgr::
setSensorMode(MINT32 const i4SensorDev, MINT32 i4NewSensorMode)
{
    MINT32 ret_main, ret_sub, ret_main2;
    ret_main = ret_sub = ret_main2 = S_AE_OK;

    if (i4SensorDev & ESensorDev_Main)
        ret_main = CamSVBufMgr::getInstance(ESensorDev_Main).setSensorMode(i4NewSensorMode);
    if (i4SensorDev & ESensorDev_Sub)
        ret_sub = CamSVBufMgr::getInstance(ESensorDev_Sub).setSensorMode(i4NewSensorMode);
    if (i4SensorDev & ESensorDev_MainSecond)
        ret_main2 = CamSVBufMgr::getInstance(ESensorDev_MainSecond).setSensorMode(i4NewSensorMode);

    return (ret_main | ret_sub | ret_main2);
}


MRESULT
ICamSVBufMgr::
isDMAInit(MINT32 const i4SensorDev, MBOOL &isInited)
{
    MINT32 ret_main, ret_sub, ret_main2;
    ret_main = ret_sub = ret_main2 = S_AE_OK;

    if (i4SensorDev & ESensorDev_Main)
        ret_main = CamSVBufMgr::getInstance(ESensorDev_Main).isDMAInit(isInited);
    if (i4SensorDev & ESensorDev_Sub)
        ret_sub = CamSVBufMgr::getInstance(ESensorDev_Sub).isDMAInit(isInited);
    if (i4SensorDev & ESensorDev_MainSecond)
        ret_main2 = CamSVBufMgr::getInstance(ESensorDev_MainSecond).isDMAInit(isInited);

    return (ret_main | ret_sub | ret_main2);
}

MBOOL
ICamSVBufMgr::
configPipe(MINT32 const i4SensorDev)
{
    if (i4SensorDev & ESensorDev_Main)
        CamSVBufMgr::getInstance(ESensorDev_Main).newBuf();
    if (i4SensorDev & ESensorDev_Sub)
        CamSVBufMgr::getInstance(ESensorDev_Sub).newBuf();
    if (i4SensorDev & ESensorDev_MainSecond)
        CamSVBufMgr::getInstance(ESensorDev_MainSecond).newBuf();

    return MFALSE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CamSVBufMgr&
CamSVBufMgr::
getInstance(MINT32 const i4SensorDev)
{
    switch (i4SensorDev)
    {
    case ESensorDev_Main: //  Main Sensor
        return  CamSVBufMgrDev<ESensorDev_Main>::getInstance();
    case ESensorDev_MainSecond: //  Main Second Sensor
        return  CamSVBufMgrDev<ESensorDev_MainSecond>::getInstance();
    case ESensorDev_Sub: //  Sub Sensor
        return  CamSVBufMgrDev<ESensorDev_Sub>::getInstance();
    default:
        MY_ERR("i4SensorDev = %d", i4SensorDev);
        return  CamSVBufMgrDev<ESensorDev_Main>::getInstance();
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CamSVBufMgr::
CamSVBufMgr(ESensorDev_T const eSensorDev)
    : m_eSensorDev(eSensorDev)
    , m_eSensorTG(ESensorTG_None)
    , m_pIMemDrv(IMemDrv::createInstance())
    , m_pIspDrv(MNULL)
    , m_u4AEStateSize(0)
    , m_u4AEHistSize(0)
    , m_u4AWBStateSize(0)
    , m_pixelMode(0)
    , m_Users(0)
    , m_Lock()
    , m_EnMVHDR(MFALSE)
    , m_EnPDAF(MFALSE)
    , m_u4IsDMAinited(MFALSE)
    , m_bDebugEnable(MFALSE)
    , m_i4SensorIdx(0)
    , m_TgInfo(0)
    , mpIspHwRegAddr(MNULL)
    , mfd(0)
    , m_i4CurrSensorId(0)
    , m_SensorMode(0)
    , m_FirstTimeCreateBuf(MFALSE)
    , m_semPD()
    , m_semMVHDR()
    , m_i4CamSVIrqKey(0)
    , m_pCamSVBufMgr_Thread(NULL)
{
    memset(&m_PDBufCtrl, 0, sizeof(SAE_BUF_CTRL_T));
    memset(&m_MVHDRBufCtrl, 0, sizeof(SAE_BUF_CTRL_T));
    memset(&m_rPDHwDeqBuf, 0, sizeof(BufInfo_T));
    memset(&m_rMVHDRHwDeqBuf, 0, sizeof(BufInfo_T));

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CamSVBufMgr::
~CamSVBufMgr()
{

}

#define ISP_DEV_NAME     "/dev/camera-isp"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
init(MINT32 const i4SensorIdx)
{
#if ENABLE_AE_MVHDR_STAT
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.debug.camsv_buf_mgr.enable", value, "0");
    m_bDebugEnable = atoi(value);
    MINT32 cam_isp_addr = ISP_CAMSV_BASE_HW;
    MINT32 i4Coverity = 0;

    m_pCamSVBufMgr_Thread = CamSVBufMgr_Thread::CreateInstance(this);

    if (m_pCamSVBufMgr_Thread == NULL) {
        MY_LOG("[%s()] SOF polling thread create fail\n", __FUNCTION__);
        return MFALSE;
    }
    m_pCamSVBufMgr_Thread->init();

    // sensor index
    m_i4SensorIdx = i4SensorIdx;

    MY_LOG("[%s()] m_eSensorDev: %d, m_i4SensorIdx: %d, m_Users: %d \n", __FUNCTION__, m_eSensorDev, m_i4SensorIdx, m_Users);

    Mutex::Autolock lock(m_Lock);

    if (m_Users > 0)
    {
        MY_LOG("%d has created \n", m_Users);
        i4Coverity = android_atomic_inc(&m_Users);
        return MTRUE;
    }

    // imem driver init
    m_pIMemDrv->init();

    //init parameters.
    m_EnPDAF = MFALSE;
    m_EnMVHDR = MFALSE;
    m_u4IsDMAinited = MFALSE;
    m_FirstTimeCreateBuf = MTRUE;

    if (m_pIspDrv == NULL) {
        m_pIspDrv = IspDrv::createInstance();
        if (!m_pIspDrv) {
            MY_ERR("IspDrv createInstance fail");
            return MFALSE;
        }

        if (m_pIspDrv->init(LOG_TAG) < 0) {
            MY_ERR("IspDrv init fail");
            return MFALSE;
        }

        m_i4CamSVIrqKey = m_pIspDrv->registerIrq(LOG_TAG); //register userkey

        if (m_i4CamSVIrqKey < 0) {
            MY_ERR("IspDrv registerIrq fail");
            return MFALSE;
        }
    }

    // Open isp driver
    mfd = open(ISP_DEV_NAME, O_RDWR);
    if (mfd < 0)
    {
        MY_ERR("error open kernel driver, %d, %s\n", errno, strerror(errno));
        return -1;
    }

    mpIspHwRegAddr = (unsigned long *) mmap(0, ISP_CAMSV_BASE_RANGE, (PROT_READ | PROT_WRITE), MAP_SHARED, mfd, cam_isp_addr);
    if (mpIspHwRegAddr == MAP_FAILED)
    {
        MY_ERR("mmap err(1), %d, %s \n", errno, strerror(errno));
        return -4;
    }
    MY_LOG("%s() m_eSensorDev(%d) ISPAddr:0x%p\n", __FUNCTION__, m_eSensorDev, (void *)mpIspHwRegAddr);



    i4Coverity = android_atomic_inc(&m_Users);
#endif
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
uninit()
{
    MUINT8 wait_vs_num = 1;
    camsv_reg_t *pisp = (camsv_reg_t *) mpIspHwRegAddr;
#if ENABLE_AE_MVHDR_STAT
    IMEM_BUF_INFO buf_info;

    MY_LOG("[%s()] m_eSensorDev: %d, m_Users: %d \n", __FUNCTION__, m_eSensorDev, m_Users);

    Mutex::Autolock lock(m_Lock);

    // If no more users, return directly and do nothing.
    if (m_Users <= 0)
    {
        return MTRUE;
    }

    // More than one user, so decrease one User.
    android_atomic_dec(&m_Users);

    if (m_Users == 0) // There is no more User after decrease one User
    {

        m_FirstTimeCreateBuf = MTRUE;

        // wait IRQ
        ISP_DRV_WAIT_IRQ_STRUCT rWaitIrq;

        rWaitIrq.Clear            = ISP_DRV_IRQ_CLEAR_WAIT;
        rWaitIrq.UserInfo.UserKey = m_i4CamSVIrqKey;
        rWaitIrq.Timeout          = 500;//ms
        rWaitIrq.bDumpReg         = 0;
        rWaitIrq.UserNumber       = 0; //must be 0 for Hal3 path

        switch (m_TgInfo)
        {
            case CAM_SV_1:
                rWaitIrq.UserInfo.Type    = ISP_DRV_IRQ_TYPE_INT_CAMSV;
                rWaitIrq.UserInfo.Status  =  CAMSV_INT_STATUS_VS1_ST;
                // 1. disable VFDATA
                if (ISP_REG(pisp, CAMSV_TG_VF_CON) != 0) {
                    ISP_BITS(pisp,CAMSV_TG_VF_CON, VFDATA_EN) = 0;
                    MY_LOG("[uninit] disable CAMSV VFDATA done");
                }

                while (wait_vs_num--) {
                    // 2. wait VSYNC
                    MBOOL ret = m_pIspDrv->waitIrq(&rWaitIrq);
                    if ( MFALSE == ret ) {
                        MY_LOG("[uninit] waitIrq( CAMSV VSYNC ) fail");
                    } else {
                        // 3. check if CAMSV TG is IDLE
                        if(!(ISP_BITS(pisp,CAMSV_TG_INTER_ST,SYN_VF_DATA_EN) == 0 && ISP_BITS(pisp,CAMSV_TG_INTER_ST,OUT_REQ) == 0)){
                            MY_LOG("[uninit] CAMSV TG is not idle.(0x%x)\n", ISP_REG(pisp,CAMSV_TG_INTER_ST));
                        }
                        // 4. reset CAMSV
                        m_pIspDrv->reset(ISP_DRV_RST_CAMSV);
                    }
                }
                MY_LOG("[uninit] CAMSV TG status.(0x%x)\n", ISP_REG(pisp,CAMSV_TG_INTER_ST));
                break;
            case CAM_SV_2: // 0x11
                rWaitIrq.UserInfo.Type    = ISP_DRV_IRQ_TYPE_INT_CAMSV2;
                rWaitIrq.UserInfo.Status  =  CAMSV2_INT_STATUS_VS1_ST;
                // 1. disable VFDATA
                if (ISP_REG(pisp, CAMSV_TG2_VF_CON) != 0) {
                    ISP_BITS(pisp,CAMSV_TG2_VF_CON, VFDATA_EN) = 0;
                    MY_LOG("[uninit] disable CAMSV2 VFDATA done");
                }

                while (wait_vs_num--) {
                    // 2. wait VSYNC
                    MBOOL ret = m_pIspDrv->waitIrq(&rWaitIrq);
                    if ( MFALSE == ret ) {
                        MY_LOG("[uninit] waitIrq( CAMSV2 VSYNC ) fail");
                    } else {
                        // 3. check if CAMSV2 TG is IDLE
                        if(!(ISP_BITS(pisp,CAMSV_TG2_INTER_ST,SYN_VF_DATA_EN) == 0 && ISP_BITS(pisp,CAMSV_TG2_INTER_ST,OUT_REQ) == 0)){
                            MY_LOG("[uninit] CAMSV2 TG is not idle.(0x%x)\n", ISP_REG(pisp,CAMSV_TG2_INTER_ST));
                        }
                        // 4. reset CAMSV2
                        m_pIspDrv->reset(ISP_DRV_RST_CAMSV2);
                    }
                }
                MY_LOG("[uninit] CAMSV2 TG status.(0x%x)\n", ISP_REG(pisp,CAMSV_TG2_INTER_ST));
                break;
            default:
                MY_ERR("m_TgInfo is invalid: %d", m_TgInfo);
                return MFALSE;
        }

        if (m_pIspDrv) {
            m_pIspDrv->uninit(LOG_TAG);
            m_pIspDrv->destroyInstance();
            m_pIspDrv = NULL;
        } else {
            MY_LOG("m_pIspDrv is NULL");
        }

        // safely free buffer
        deleteBuf();

        // imem driver ininit
        m_pIMemDrv->uninit();

        if ( 0 != mpIspHwRegAddr )
        {
            munmap(mpIspHwRegAddr, ISP_CAMSV_BASE_RANGE);
            mpIspHwRegAddr = NULL;
        }

        if(mfd >= 0)
        {
            close(mfd);
            mfd = -1;
        }
    }
    else    // There are still some users.
    {
        MY_LOG_IF(m_bDebugEnable,"Still %d users \n", m_Users);
    }

    if (m_pCamSVBufMgr_Thread != NULL) {
        m_pCamSVBufMgr_Thread->uninit();
    }
#endif
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
debugPrint()
{
    if( m_bDebugEnable)
    {
        BufInfoList_T::iterator it;

        for (it = m_rPDHwBufList.begin(); it != m_rPDHwBufList.end(); it++ )
            MY_LOG("[PD] memID:[0x%x]/virtAddr:[0x%x]/phyAddr:[0x%x] \n", it->memID, it->virtAddr,it->phyAddr);

        for (it = m_rMVHDRHwBufList.begin(); it != m_rMVHDRHwBufList.end(); it++ )
            MY_LOG("[MVHDR] memID:[0x%x]/virtAddr:[0x%x]/phyAddr:[0x%x] \n", it->memID,it->virtAddr,it->phyAddr);
    }
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
enqueueHwBuf(BufInfo_T& rBufInfo, MINT32 PDSel)
{
    MBOOL ret=MFALSE;

#if ENABLE_AE_MVHDR_STAT
    Mutex::Autolock lock(m_Lock);

    MY_LOG_IF(m_bDebugEnable,"%s m_eSensorDev %d (PDSel=%d)\n", __FUNCTION__, m_eSensorDev, PDSel);

    if( PDSel==_PD_BUF_SEL_ && m_EnPDAF)
    {
        // add element at the end
        if (!m_rPDHwDeqBuf.size) {
        m_rPDHwBufList.push_back(rBufInfo);
        } else {
            //update status from inner to available
            m_rPDHwDeqBuf.status = PDAF_BUFCTRL_ENUM_AVAIL;
            m_rPDHwBufList.push_back(m_rPDHwDeqBuf);
        }
        // TODO: need enable ?
        //m_pIMemDrv->cacheSyncbyRange(IMEM_CACHECTRL_ENUM_INVALID, &rBufInfo);
        ret = MTRUE;

        int Val;
        ::sem_getvalue( &m_semPD, &Val);
        ::sem_post( &m_semPD);
    }
    else if( PDSel!=_PD_BUF_SEL_ && m_EnMVHDR)
    {
        // add element at the end
        if (!m_rMVHDRHwDeqBuf.size) {
        m_rMVHDRHwBufList.push_back(rBufInfo);
        } else {
            m_rMVHDRHwBufList.push_back(m_rMVHDRHwDeqBuf);
        }
        //m_pIMemDrv->cacheSyncbyRange(IMEM_CACHECTRL_ENUM_INVALID, &rBufInfo);
        ret = MTRUE;
    }

    debugPrint();
#endif
    return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
dequeueHwBuf(BufInfo_T& rBufInfo, MINT32 i4PDSel, MUINT32 i4FrmNum, ESTT_CACHE_SYNC_POLICY cacheSyncPolicy)
{
    MBOOL ret=MFALSE;

#if ENABLE_AE_MVHDR_STAT
    ISP_DRV_WAIT_IRQ_STRUCT rWaitIrq;

    rWaitIrq.Clear            = ISP_DRV_IRQ_CLEAR_WAIT;
    rWaitIrq.UserInfo.UserKey = m_i4CamSVIrqKey;
    rWaitIrq.Timeout          = 500;//ms
    rWaitIrq.bDumpReg         = 0;
    rWaitIrq.UserNumber       = 0; //must be 0 for Hal3 path

    switch (m_TgInfo)
    {
        case CAM_SV_1: // 0x10
            rWaitIrq.UserInfo.Type    = ISP_DRV_IRQ_TYPE_INT_CAMSV;
            rWaitIrq.UserInfo.Status  =  CAMSV_INT_STATUS_PASS1_DON_ST;
            break;
        case CAM_SV_2: // 0x11
            rWaitIrq.UserInfo.Type    = ISP_DRV_IRQ_TYPE_INT_CAMSV2;
            rWaitIrq.UserInfo.Status  =  CAMSV2_INT_STATUS_PASS1_DON_ST;
            break;
        default:
            MY_ERR("m_TgInfo is invalid: %d", m_TgInfo);
            return ret;
    }

    // wait CAMSV DONE
    if (m_pIspDrv->waitIrq(&rWaitIrq) <= 0) {
        MY_ERR("waitIrq CAMSV DONE fail");
        return ret;
    }

    memset( &rBufInfo, 0, sizeof(BufInfo_T));

    MY_LOG_IF(m_bDebugEnable,"\n\n%s CAMSV DONE m_eSensorDev %d (PDSel=%d) FrmNum%d\n", __FUNCTION__, m_eSensorDev, i4PDSel, i4FrmNum);

    {
        char value[PROPERTY_VALUE_MAX] = {'\0'};
        property_get("vendor.vc.dump.enable", value, "0");
        MBOOL bEnable = atoi(value);

        if( i4PDSel==_PD_BUF_SEL_ && m_EnPDAF)
        {
            if (m_rPDHwBufList.size())
            {
                int Val;
                ::sem_getvalue( &m_semPD, &Val);
                if( Val==0)
                {
                    MY_LOG( "%s [WAR] m_semPD = %d, buffer is no buffer is enqueued.(buffer not ready!!)", __FUNCTION__, Val);
                }
                else if( Val==MAX_AAO_BUFFER_CNT)
                {
                    MY_LOG_IF( m_bDebugEnable, "%s m_semPD = %d\n", __FUNCTION__, Val);
                }
                else
                {
                    MY_LOG( "%s [WAR] m_semPD = %d, not all buffers are enqueued", __FUNCTION__, Val);
                }
                ::sem_wait( &m_semPD);

                m_Lock.lock();
                rBufInfo = m_rPDHwBufList.front();
                m_rPDHwDeqBuf = rBufInfo;
                m_rPDHwBufList.pop_front();

                for (MINT32 i = 0; i < MAX_AAO_BUFFER_CNT; i++)
                {
                    if( m_EnPDAF)
                    {
                        if (m_PDBufCtrl.m_rAAOBufInfo[i].phyAddr == rBufInfo.phyAddr) {
                            MY_LOG_IF(m_bDebugEnable, "PDAF cache sync PA(x%p) policy(%d)", (void*)m_PDBufCtrl.m_rAAOBufInfo[i].phyAddr, cacheSyncPolicy);
                            switch(cacheSyncPolicy){
                                case ESTT_CacheBypass:
                                    MY_ERR("PDAF Camsv bypass cache sync.");
                                    break;
                                case ESTT_CacheInvalidAll:
                                    m_pIMemDrv->cacheSyncbyRange(IMEM_CACHECTRL_ENUM_INVALID_ALL, &(m_PDBufCtrl.m_rAAOBufInfo[i]));
                                    break;
                                case ESTT_CacheInvalidByRange:
                                default:
                                    m_pIMemDrv->cacheSyncbyRange(IMEM_CACHECTRL_ENUM_INVALID    , &(m_PDBufCtrl.m_rAAOBufInfo[i]));
                                    break;
                            }
                        }
                    }

                    if( m_EnMVHDR)
                    {
                        if (m_MVHDRBufCtrl.m_rAAOBufInfo[i].phyAddr == rBufInfo.phyAddr) {
                            MY_LOG_IF(m_bDebugEnable, "MVHDR cache sync PA(x%p) policy(%d)", (void*)m_MVHDRBufCtrl.m_rAAOBufInfo[i].phyAddr, cacheSyncPolicy);
                            switch(cacheSyncPolicy){
                                case ESTT_CacheBypass:
                                    MY_ERR("MVHDR Camsv bypass cache sync.");
                                    break;
                                case ESTT_CacheInvalidAll:
                                    m_pIMemDrv->cacheSyncbyRange(IMEM_CACHECTRL_ENUM_INVALID_ALL, &(m_MVHDRBufCtrl.m_rAAOBufInfo[i]));
                                    break;
                                case ESTT_CacheInvalidByRange:
                                default:
                                    m_pIMemDrv->cacheSyncbyRange(IMEM_CACHECTRL_ENUM_INVALID    , &(m_MVHDRBufCtrl.m_rAAOBufInfo[i]));
                                    break;
                            }
                        }
                    }
                }



                ret = MTRUE;
                m_Lock.unlock();

            }

            if (bEnable)
            {
                char fileName[64];
                sprintf(fileName, "/sdcard/vc/%d_dma.raw", i4FrmNum);
                FILE *fp = fopen(fileName, "w");
                if( fp)
                {
                    MY_LOG_IF(m_bDebugEnable,"%s\n", fileName);
                    fwrite(reinterpret_cast<void *>(rBufInfo.virtAddr), 1, rBufInfo.size, fp);
                    fclose(fp);
                }
                else
                {
                    MY_ERR("fail to open file to save img: %s", fileName);
                    MINT32 err = mkdir("/sdcard/vc", S_IRWXU | S_IRWXG | S_IRWXO);
                    MY_LOG("err = %d", err);
                }
            }
        }
        else if( i4PDSel!=_PD_BUF_SEL_ && m_EnMVHDR)
        {
            static MINT32 frameCnt = 0;

            if (m_rMVHDRHwBufList.size())
            {
                m_Lock.lock();
                rBufInfo = m_rMVHDRHwBufList.front();
                m_rMVHDRHwDeqBuf = rBufInfo;
                m_rMVHDRHwBufList.pop_front();
                ret = MTRUE;
                m_Lock.unlock();
            }

            if (bEnable)
            {
                char fileName[64];
                sprintf(fileName, "/sdcard/vc/%d_ae.raw", frameCnt++);
                FILE *fp = fopen(fileName, "w");
                if( fp)
                {
                    MY_LOG_IF(m_bDebugEnable,"%s\n", fileName);
                    fwrite(reinterpret_cast<void *>(rBufInfo.virtAddr), 1, rBufInfo.size, fp);
                    fclose(fp);
                }
                else
                {
                    MY_ERR("fail to open file to save img: %s", fileName);
                    MINT32 err = mkdir("/sdcard/vc", S_IRWXU | S_IRWXG | S_IRWXO);
                    MY_LOG("err = %d", err);
                }
            }
            else
            {
                frameCnt = 0;
            }


        }
    }
_exit_deq:
    if( m_bDebugEnable)
    {
        camsv_reg_t *pisp = (camsv_reg_t *) mpIspHwRegAddr;
        MY_LOG_IF(m_bDebugEnable, "%s m_eSensorDev(%d) ISPAddr:0x%p 0x%0x\n", __FUNCTION__, m_eSensorDev, (void *)mpIspHwRegAddr, (MUINTPTR)pisp);

        MY_LOG_IF(m_bDebugEnable, "(0x9414) CAMSV_TG_VF_CON :0x%0x\n", ISP_REG(pisp, CAMSV_TG_VF_CON));
        MY_LOG_IF(m_bDebugEnable, "(0x9414) CAMSV_TG2_VF_CON :0x%0x\n", ISP_REG(pisp, CAMSV_TG2_VF_CON));
        MY_LOG_IF(m_bDebugEnable, "(0x9C48) CAMSV_TG2_FRMSIZE_ST :0x%0x\n", ISP_REG(pisp, CAMSV_TG2_FRMSIZE_ST));

        if( i4PDSel==_PD_BUF_SEL_ && m_EnPDAF)
        {
            MY_LOG_IF(m_bDebugEnable, "(0x9820) CAMSV2_CLK_EN : 0x%0x\n", ISP_REG(pisp, CAMSV2_CLK_EN));
            MY_LOG_IF(m_bDebugEnable, "(0x9808) CAMSV2_INT_EN : 0x%0x\n", ISP_REG(pisp, CAMSV2_INT_EN));
            MY_LOG_IF(m_bDebugEnable, "(0x9800) CAMSV2_MODULE_EN : 0x%0x\n", ISP_REG(pisp, CAMSV2_MODULE_EN));
            MY_LOG_IF(m_bDebugEnable, "(0x983C) CAMSV2_PAK : 0x%0x\n", ISP_REG(pisp, CAMSV2_PAK));
            MY_LOG_IF(m_bDebugEnable, "(0x9804) CAMSV2_FMT_SEL : 0x%0x\n", ISP_REG(pisp, CAMSV2_FMT_SEL));
            MY_LOG_IF(m_bDebugEnable, "(0x9228) CAMSV_IMGO_SV_D_BASE_ADDR : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_D_BASE_ADDR));
            MY_LOG_IF(m_bDebugEnable, "(0x922C) CAMSV_IMGO_SV_D_OFST_ADDR : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_D_OFST_ADDR));
            MY_LOG_IF(m_bDebugEnable, "(0x9230) CAMSV_IMGO_SV_D_XSIZE : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_D_XSIZE));
            MY_LOG_IF(m_bDebugEnable, "(0x9234) CAMSV_IMGO_SV_D_YSIZE : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_D_YSIZE));
            MY_LOG_IF(m_bDebugEnable, "(0x9238) CAMSV_IMGO_SV_D_STRIDE : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_D_STRIDE));
            MY_LOG_IF(m_bDebugEnable, "(0x9C48) CAMSV_TG2_FRMSIZE_ST :0x%0x\n", ISP_REG(pisp, CAMSV_TG2_FRMSIZE_ST));
            MY_LOG_IF(m_bDebugEnable, "(0x9C18) CAMSV_TG2_SEN_GRAB_PXL :0x%0x\n", ISP_REG(pisp, CAMSV_TG2_SEN_GRAB_PXL));
            MY_LOG_IF(m_bDebugEnable, "(0x9C1C) CAMSV_TG2_SEN_GRAB_LIN :0x%0x\n", ISP_REG(pisp, CAMSV_TG2_SEN_GRAB_LIN));
        }
        else if( i4PDSel!=_PD_BUF_SEL_ && m_EnMVHDR)
        {
            MY_LOG_IF(m_bDebugEnable, "(0x9020) CAMSV_CLK_EN : 0x%0x\n", ISP_REG(pisp, CAMSV_CLK_EN));
            MY_LOG_IF(m_bDebugEnable, "(0x9008) CAMSV_INT_EN : 0x%0x\n", ISP_REG(pisp, CAMSV_INT_EN));
            MY_LOG_IF(m_bDebugEnable, "(0x9000) CAMSV_MODULE_EN : 0x%0x\n", ISP_REG(pisp, CAMSV_MODULE_EN));
            MY_LOG_IF(m_bDebugEnable, "(0x903C) CAMSV_PAK : 0x%0x\n", ISP_REG(pisp, CAMSV_PAK));
            MY_LOG_IF(m_bDebugEnable, "(0x9004) CAMSV_FMT_SEL : 0x%0x\n", ISP_REG(pisp, CAMSV_FMT_SEL));
            MY_LOG_IF(m_bDebugEnable, "(0x9208) CAMSV_IMGO_SV_BASE_ADDR : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_BASE_ADDR));
            MY_LOG_IF(m_bDebugEnable, "(0x920C) CAMSV_IMGO_SV_OFST_ADDR : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_OFST_ADDR));
            MY_LOG_IF(m_bDebugEnable, "(0x9210) CAMSV_IMGO_SV_XSIZE : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_XSIZE));
            MY_LOG_IF(m_bDebugEnable, "(0x9214) CAMSV_IMGO_SV_YSIZE : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_YSIZE));
            MY_LOG_IF(m_bDebugEnable, "(0x9218) CAMSV_IMGO_SV_STRIDE : 0x%0x\n", ISP_REG(pisp, CAMSV_IMGO_SV_STRIDE));
        }

    }


    MY_LOG_IF(m_bDebugEnable,"rBufInfo.virtAddr:[0x%x]/phyAddr:[0x%x] \n",rBufInfo.virtAddr,rBufInfo.phyAddr);


#endif
    return ret;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
updateDMABaseAddr(MUINT32 u4BaseAddr, MINT32 PDSel)
{
    MBOOL ret=MFALSE;

#if ENABLE_AE_MVHDR_STAT
    camsv_reg_t *pisp = (camsv_reg_t *) mpIspHwRegAddr;

    if( PDSel==_PD_BUF_SEL_ && m_EnPDAF)
    {
        if( u4BaseAddr)
        {
            MY_LOG("%s [PD] m_eSensorDev(%d) u4BaseAddr=0x%x ISPAddr:0x%p 0x%0x", __FUNCTION__, m_eSensorDev, u4BaseAddr, (void *)mpIspHwRegAddr, (MUINTPTR)pisp);
        }
        else
        {
            MY_LOG_IF(m_bDebugEnable, "%s [PD] u4BaseAddr is NULL", __FUNCTION__);
            return E_ISPMGR_NULL_ADDRESS;
        }
    }
    else if( PDSel!=_PD_BUF_SEL_ && m_EnMVHDR)
    {
        if( u4BaseAddr)
        {
            MY_LOG("%s [MVHDR] m_eSensorDev(%d) u4BaseAddr=0x%x ISPAddr:0x%p 0x%0x", __FUNCTION__, m_eSensorDev, u4BaseAddr, (void *)mpIspHwRegAddr, (MUINTPTR)pisp);
        }
        else
        {
            MY_ERR("%s [MVHDR] u4BaseAddr is NULL", __FUNCTION__);
            return E_ISPMGR_NULL_ADDRESS;
        }
    }
    else
    {
        MY_LOG("%s DEV(%d) Addr(0x%x), Sel(%d), En: CamSv1(%d), CamSv2(%d)", __FUNCTION__, m_eSensorDev, u4BaseAddr, PDSel, m_EnMVHDR, m_EnPDAF);
        return E_ISPMGR_INVALID_SETTING;
    }



    if (PDSel==_PD_BUF_SEL_ && m_EnPDAF)
    {
        ISP_REG(pisp, CAMSV2_PAK) = 0x7&((m_pixelMode<<2)|(0x3&m_PDBufCtrl.m_u1NumBit));
        ISP_REG(pisp, CAMSV_IMGO_SV_D_BASE_ADDR) = u4BaseAddr;
        ISP_REG(pisp, CAMSV_IMGO_SV_D_OFST_ADDR) = AAO_OFFSET_ADDR;
        ISP_REG(pisp, CAMSV_IMGO_SV_D_XSIZE) = m_PDBufCtrl.m_u4AAOXSize;
        ISP_REG(pisp, CAMSV_IMGO_SV_D_YSIZE) = m_PDBufCtrl.m_u4AAOYSize;
        if(m_pixelMode==ONE_PIXEL_MODE)
        {
            ISP_REG(pisp, CAMSV_IMGO_SV_D_STRIDE) = m_PDBufCtrl.m_u4AAOXSize+1; //one pixel mode
            ISP_REG(pisp, CAMSV2_FMT_SEL) = 0x1000000;
        }
        else
        {
            ISP_REG(pisp, CAMSV_IMGO_SV_D_STRIDE) = 0x8B0000|(0xffff&(m_PDBufCtrl.m_u4AAOXSize+1)) ;
            ISP_REG(pisp, CAMSV2_FMT_SEL) = 0x3000000;
        }
        MY_LOG("[PD] pixel mode %d, XSIZE=0x%x, YSIZE=0x%x\n",  m_pixelMode, m_PDBufCtrl.m_u4AAOXSize, m_PDBufCtrl.m_u4AAOYSize);
        ret = MTRUE;
    }
    else if( PDSel!=_PD_BUF_SEL_ && m_EnMVHDR)
    {
        ISP_REG(pisp, CAMSV_PAK) = 0x7&((m_pixelMode<<2)|(0x3&m_MVHDRBufCtrl.m_u1NumBit));
        ISP_REG(pisp, CAMSV_IMGO_SV_BASE_ADDR) = u4BaseAddr;
        ISP_REG(pisp, CAMSV_IMGO_SV_OFST_ADDR) = AAO_OFFSET_ADDR;
        ISP_REG(pisp, CAMSV_IMGO_SV_XSIZE) = m_MVHDRBufCtrl.m_u4AAOXSize;
        ISP_REG(pisp, CAMSV_IMGO_SV_YSIZE) = m_MVHDRBufCtrl.m_u4AAOYSize;
        ISP_REG(pisp, CAMSV_IMGO_SV_STRIDE) = 0x1000;
        if(m_pixelMode==ONE_PIXEL_MODE)
            ISP_REG(pisp, CAMSV_IMGO_SV_STRIDE) = m_MVHDRBufCtrl.m_u4AAOXSize+1; //one pixel mode
        else
            ISP_REG(pisp, CAMSV_IMGO_SV_STRIDE) = 0x8B0000|(0xffff&(m_MVHDRBufCtrl.m_u4AAOXSize+1)) ;
        MY_LOG("[MVHDR] pixel mode %d, XSIZE=0x%x, YSIZE=0x%x\n", m_pixelMode, m_MVHDRBufCtrl.m_u4AAOXSize, m_MVHDRBufCtrl.m_u4AAOYSize);
        ret = MTRUE;
    }



#endif
    return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
DMAInit()
{
    MY_LOG("%s + Dev(%d) SensorMode(%d)\n", __FUNCTION__, m_eSensorDev, m_SensorMode);
#if ENABLE_AE_MVHDR_STAT
    if (m_TgInfo == CAM_SV_1 || m_TgInfo == CAM_SV_2)
    {
        //separate new buffer as a standalone step for timing concern.
        //newBuf();

        if( m_EnPDAF)
        {
            //update to outer register before first SOF
            updateDMABaseAddr(getCurrHwBuf(), _PD_BUF_SEL_);
        }

        if( m_EnMVHDR)
        {
            updateDMABaseAddr(getNextHwBuf());
        }

        m_u4IsDMAinited = MTRUE;

    }
    else
    {
        MY_LOG("(%s) m_eSensorDev(%d) No TG:%d\n", __FUNCTION__, m_eSensorDev, m_eSensorTG);
    }

#endif
    //SOF polling thread start
    if (m_pCamSVBufMgr_Thread != NULL) {
        m_pCamSVBufMgr_Thread->start();
    }

    MY_LOG("%s - Dev(%d) SensorMode(%d)\n", __FUNCTION__, m_eSensorDev, m_SensorMode);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
DMAUninit()
{
#if ENABLE_AE_MVHDR_STAT
    MY_LOG_IF(m_bDebugEnable,"%s() m_eSensorDev(%d)\n", __FUNCTION__, m_eSensorDev);
    if (m_pCamSVBufMgr_Thread != NULL) {
        m_pCamSVBufMgr_Thread->stop();
    }
    clearHwBufStatus();
#endif
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MUINT32
CamSVBufMgr::
getCurrHwBuf()
{
#if ENABLE_AE_MVHDR_STAT
    if (m_rPDHwBufList.size() > 0)
    {
        /*
         * In START stage, 3 buffers are available
         * update the first buffer to outer status
         */
        m_rPDHwBufList.front().status =  PDAF_BUFCTRL_ENUM_OUTER;
        return m_rPDHwBufList.front().phyAddr;
    }
    else
    {
        // No free buffer
        MY_ERR("No free buffer\n");
        return 0;
    }
#else
    return 0;
#endif
}

/*
 * Design concept: Mark buffer status to avail(0)/outer(1)/Inner(2)
 * in normal usage, a buffer would go through 0->1->2->0 status.
 * now, create 3 buffers when initial and all are available.(a:0|b:0|c:0)
 * start:(a:1|b:0|c:0)
 * sof1(a:2|b:1|c:0) -> p1_don(a:2|b:1|c:0) -> deq(b:1|c:0) -> enq(b:1|c:0:|a:0) ->
 * sof2(b:2|c:1|a:0) -> ...
 */
void
CamSVBufMgr::
changeHwBufStatus()
{
    BufInfoList_T::iterator it;
    for (it = m_rPDHwBufList.begin(); it != m_rPDHwBufList.end(); it++) {
        if (it->status == PDAF_BUFCTRL_ENUM_OUTER) {
            it->status = PDAF_BUFCTRL_ENUM_INNER;
        }
    }
}

void
CamSVBufMgr::
clearHwBufStatus()
{
    BufInfoList_T::iterator it;
    for (it = m_rPDHwBufList.begin(); it != m_rPDHwBufList.end(); it++) {
        it->status = PDAF_BUFCTRL_ENUM_AVAIL;
    }
}

void
CamSVBufMgr::
dumpMyBuff()
{
    BufInfoList_T::iterator it;
    MUINT32 i=0;
    for (it = m_rPDHwBufList.begin(); it != m_rPDHwBufList.end(); it++) {
        MY_ERR("buf(%d) status(%d)", i++, it->status);
    }
}

MUINT32
CamSVBufMgr::
getAvailHwBuf()
{
    BufInfoList_T::iterator it;

    for (it = m_rPDHwBufList.begin(); it != m_rPDHwBufList.end(); it++) {
        //find the first availbe buffer in queue
        if (it->status == PDAF_BUFCTRL_ENUM_AVAIL) {
            it->status = PDAF_BUFCTRL_ENUM_OUTER;
            MY_LOG("getAvailHwBuf addr pa=0x%x, va=0x%x",
                               it->phyAddr,
                               it->virtAddr);
            return it->phyAddr;
        }
    }
    MY_LOG_IF(m_bDebugEnable, "getAvailHwBuf - No free buffer\n");
    return 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MUINT32
CamSVBufMgr::
getNextHwBuf(MINT32 PDSel)
{
#if ENABLE_AE_MVHDR_STAT
    BufInfoList_T::iterator it;

    if(PDSel==_PD_BUF_SEL_ && m_EnPDAF)
    {
        if (m_rPDHwBufList.size() > 1)
        {
            it = m_rPDHwBufList.begin();
            it++;

            if( m_bDebugEnable)
            {
                MUINT32 i=0;
                for(BufInfoList_T::iterator itt=m_rPDHwBufList.begin(); itt!=m_rPDHwBufList.end(); itt++)
                {
                    MY_LOG_IF( m_bDebugEnable,
                               "PD HW Buffer dump (%d) addr pa=0x%x, va=0x%x",
                               i++,
                               itt->phyAddr,
                               itt->virtAddr);
                }

                MY_LOG_IF( m_bDebugEnable,
                           "Buffer information for configuring HW using index at 2 ( addr pa=0x%x, va=0x%x )",
                           it->phyAddr,
                           it->virtAddr);


            }

            return it->phyAddr;
        }
        else
        {
            // No free buffer
            MY_ERR("No free buffer\n");
            return 0;
        }
    }
    else if( PDSel!=_PD_BUF_SEL_ && m_EnMVHDR)
    {
        if (m_rMVHDRHwBufList.size() > 1)
        {
            it = m_rMVHDRHwBufList.begin();
            it++;
            return it->phyAddr;
        }
        else
        {
            // No free buffer
            MY_ERR("No free buffer\n");
            return 0;
        }

    }
    return 0;
#else
    return 0;
#endif
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
freeBuf(BufInfo_T &rBufInfo)
{
#if ENABLE_AE_MVHDR_STAT
#if 1
    if (m_pIMemDrv->unmapPhyAddr(&rBufInfo))
    {
        MY_ERR("m_pIMemDrv->unmapPhyAddr() error");
        return MFALSE;
    }

    if (m_pIMemDrv->freeVirtBuf(&rBufInfo))
    {
        MY_ERR("m_pIMemDrv->freeVirtBuf() error");
        return MFALSE;
    }
#endif
    MY_LOG("%s() memID = 0x%x\n", __FUNCTION__, rBufInfo.memID);
#endif
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::deleteBuf()
{
    if( m_EnPDAF)
    {
        for (MINT32 i = 0; i < MAX_AAO_BUFFER_CNT; i++)
        {
            int Val;
            ::sem_getvalue( &m_semPD, &Val);
            // Val should not be 0
            if( Val)
            {
                MY_LOG_IF( m_bDebugEnable, "%s m_semPD = %d, free buffer", __FUNCTION__, Val);
                ::sem_wait( &m_semPD);
            }
            else
            {
                MY_LOG( "%s [WAR] m_semPD = %d, all buffer is not enqueued before free buffer", __FUNCTION__, Val);
            }

        }

        //After all buffer is enqueued, release all buffer.
        for( MINT32 i = 0; i < MAX_AAO_BUFFER_CNT; i++)
        {
            freeBuf( m_PDBufCtrl.m_rAAOBufInfo[i]);
            MY_LOG( "%s free buffer #%d", __FUNCTION__, i);
        }
    }


    if( m_EnMVHDR)
    {
        for (MINT32 i = 0; i < MAX_AAO_BUFFER_CNT; i++)
        {
            freeBuf(m_MVHDRBufCtrl.m_rAAOBufInfo[i]);
        }
    }



    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
allocateBuf(BufInfo_T &rBufInfo, MUINT32 u4BufSize)
{
#if ENABLE_AE_MVHDR_STAT
    rBufInfo.size = u4BufSize;
#if 1
    if (m_pIMemDrv->allocVirtBuf(&rBufInfo))
    {
        MY_ERR("m_pIMemDrv->allocVirtBuf() error");
        return MFALSE;
    }

    if (m_pIMemDrv->mapPhyAddr(&rBufInfo))
    {
        MY_ERR("m_pIMemDrv->mapPhyAddr() error");
        return MFALSE;
    }
#endif
    MY_LOG("%s (memID=0x%x)\n", __FUNCTION__, rBufInfo.memID);
#endif
    return MTRUE;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::newBuf()
{
    MUINT32 PDXSize=0, MVHDRXSize=0;
    MUINT32 PDYSize=0, MVHDRYSize=0;
    MUINT8  PDNumBits=0, MVHDRNumBits=0;

    IHalSensorList* const pIHalSensorList = IHalSensorList::get();
    IHalSensor* pIHalSensor = pIHalSensorList->createSensor(LOG_TAG, m_i4SensorIdx);
    SensorStaticInfo rSensorStaticInfo;
    SensorDynamicInfo rSensorDynamicInfo;
    MINT32 i4SensorSenraio = m_SensorMode;
    SensorVCInfo rSensorVCInfo;

    memset(&rSensorStaticInfo, 0, sizeof(SensorStaticInfo));
    memset(&rSensorDynamicInfo, 0, sizeof(SensorDynamicInfo));
    memset(&rSensorVCInfo, 0, sizeof(SensorVCInfo));

    // DMA buffer init
    if( m_FirstTimeCreateBuf==MTRUE)
    {
        m_FirstTimeCreateBuf = MFALSE;
    }
    else
    {
        deleteBuf();
    }


    //query sensor information.
    switch(m_eSensorDev)
    {
    case ESensorDev_Main:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
        pIHalSensor->sendCommand(NSCam::SENSOR_DEV_MAIN, SENSOR_CMD_GET_SENSOR_VC_INFO, (MUINTPTR)&rSensorVCInfo, (MUINTPTR)&i4SensorSenraio, 0);
        pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_MAIN, &rSensorDynamicInfo);
        break;

    case ESensorDev_Sub:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
        pIHalSensor->sendCommand(NSCam::SENSOR_DEV_SUB, SENSOR_CMD_GET_SENSOR_VC_INFO, (MUINTPTR)&rSensorVCInfo, (MUINTPTR)&i4SensorSenraio, 0);
        pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_SUB, &rSensorDynamicInfo);
        break;

    case ESensorDev_MainSecond:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
        pIHalSensor->sendCommand(NSCam::SENSOR_DEV_MAIN_2, SENSOR_CMD_GET_SENSOR_VC_INFO, (MUINTPTR)&rSensorVCInfo, (MUINTPTR)&i4SensorSenraio, 0);
        pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorDynamicInfo);
        break;

    default:
        MY_ERR("Invalid sensor device: %d", m_eSensorDev);
    }
    if(pIHalSensor) pIHalSensor->destroyInstance(LOG_TAG);


    //set information
    m_i4CurrSensorId = rSensorStaticInfo.sensorDevID;
    m_pixelMode = TWO_PIXEL_MODE; //camsv can only work on two pixel mode

    MY_LOG("SensorID 0x%x, PDAF_Support mode %d, Senraio %d, m_pixelMode %d", m_i4CurrSensorId, rSensorStaticInfo.PDAF_Support, i4SensorSenraio, m_pixelMode);
    MY_LOG("VC_Num %d, ModeSelect %d, EXPO_Ratio %d, ODValue %d, RG_STATSMODE %d", rSensorVCInfo.VC_Num, rSensorVCInfo.ModeSelect, rSensorVCInfo.EXPO_Ratio, rSensorVCInfo.ODValue, rSensorVCInfo.RG_STATSMODE);
    MY_LOG("VC0, ID %d, DataType 0x%04x, SIZEH 0x%04x, SIZEV 0x%04x", rSensorVCInfo.VC0_ID, rSensorVCInfo.VC0_DataType, rSensorVCInfo.VC0_SIZEH, rSensorVCInfo.VC0_SIZEV);

    //for MVHDR
    MY_LOG("VC1, ID %d, DataType 0x%04x, SIZEH 0x%04x BYTE, SIZEV 0x%04x", rSensorVCInfo.VC1_ID, rSensorVCInfo.VC1_DataType, rSensorVCInfo.VC1_SIZEH, rSensorVCInfo.VC1_SIZEV);
    if( rSensorVCInfo.VC1_DataType!=0 &&
            rSensorVCInfo.VC1_SIZEH !=0 &&
            rSensorVCInfo.VC1_SIZEV!=0)
    {
        m_EnMVHDR = MTRUE;


        if( rSensorVCInfo.VC1_DataType==0x2b) //10 bit
        {
            MVHDRXSize = rSensorVCInfo.VC1_SIZEH;
            MVHDRNumBits = 0x2;
        }
        else //8 bit
        {
            MVHDRXSize = rSensorVCInfo.VC1_SIZEH;
            MVHDRNumBits = 0x3;
        }
        MVHDRYSize = rSensorVCInfo.VC1_SIZEV;

        //check pixel mode
        if( m_pixelMode==TWO_PIXEL_MODE)
        {
            //align 4
            if( (MVHDRXSize&0x03)!=0)
            {
                MVHDRXSize = MVHDRXSize+(4-(0x03&MVHDRXSize));
            }
        }
        else
        {
            //do nothing.
        }

        MVHDRXSize -= 1;
        MVHDRYSize -= 1;
    }
    else
    {
        m_EnMVHDR = MFALSE;
        MVHDRXSize = 0;
        MVHDRYSize = 0;
    }

    //for PDAF
    MY_LOG("VC2, ID %d, DataType 0x%04x, SIZEH 0x%04x BYTE, SIZEV 0x%04x", rSensorVCInfo.VC2_ID, rSensorVCInfo.VC2_DataType, rSensorVCInfo.VC2_SIZEH, rSensorVCInfo.VC2_SIZEV);
    if( rSensorStaticInfo.PDAF_Support==2 &&
            rSensorVCInfo.VC2_DataType!=0 &&
            rSensorVCInfo.VC2_SIZEH !=0 &&
            rSensorVCInfo.VC2_SIZEV!=0)
    {
        m_EnPDAF = MTRUE;

        if( rSensorVCInfo.VC2_DataType==0x2b) //10 bit
        {
            PDXSize = rSensorVCInfo.VC2_SIZEH;
            PDNumBits = 0x2;
        }
        else //8 bit
        {
            PDXSize = rSensorVCInfo.VC2_SIZEH;
            PDNumBits = 0x3;
        }
        PDYSize = rSensorVCInfo.VC2_SIZEV;

        //check pixel mode
        if( m_pixelMode==TWO_PIXEL_MODE)
        {
            //align 4
            if( (PDXSize&0x03)!=0)
            {
                PDXSize = PDXSize+(4-(0x03&PDXSize));
            }
        }
        else
        {
            //do nothing.
        }

        PDXSize -= 1;
        PDYSize -= 1;
    }
    else
    {
        m_EnPDAF = MFALSE;
        PDXSize = 0;
        PDYSize = 0;
    }


    MY_LOG("VC3, ID %d, DataType 0x%04x, SIZEH 0x%04x BYTE, SIZEV 0x%04x", rSensorVCInfo.VC3_ID, rSensorVCInfo.VC3_DataType, rSensorVCInfo.VC3_SIZEH, rSensorVCInfo.VC3_SIZEV);


    // removes all elements from the list container
    m_rPDHwBufList.clear();
    m_rMVHDRHwBufList.clear();
    memset(&m_rPDHwDeqBuf, 0, sizeof(BufInfo_T));
    memset(&m_rMVHDRHwDeqBuf, 0, sizeof(BufInfo_T));

//
    m_PDBufCtrl.m_u4AAOXSize = PDXSize;
    m_PDBufCtrl.m_u4AAOYSize = PDYSize;
    m_PDBufCtrl.m_u1NumBit = PDNumBits;
    m_PDBufCtrl.m_u4AAOBufSize = (m_PDBufCtrl.m_u4AAOXSize+1)*(m_PDBufCtrl.m_u4AAOYSize+1);

    m_MVHDRBufCtrl.m_u4AAOXSize = MVHDRXSize;
    m_MVHDRBufCtrl.m_u4AAOYSize = MVHDRYSize;
    m_MVHDRBufCtrl.m_u1NumBit = MVHDRNumBits;
    m_MVHDRBufCtrl.m_u4AAOBufSize = (m_MVHDRBufCtrl.m_u4AAOXSize+1)*(m_MVHDRBufCtrl.m_u4AAOYSize+1);

    MY_LOG("== PDBufCtrl En(%d)==", m_EnPDAF);
    MY_LOG("BufSize   = 0x%04x", m_PDBufCtrl.m_u4AAOBufSize);
    MY_LOG("XSize     = 0x%04x", m_PDBufCtrl.m_u4AAOXSize);
    MY_LOG("YSize     = 0x%04x", m_PDBufCtrl.m_u4AAOYSize);
    MY_LOG("NumBit    = 0x%04x", m_PDBufCtrl.m_u1NumBit);
    MY_LOG("== MVHDRBufCtrl En(%d)==", m_EnMVHDR);
    MY_LOG("BufSize   = 0x%04x", m_MVHDRBufCtrl.m_u4AAOBufSize);
    MY_LOG("XSize     = 0x%04x", m_MVHDRBufCtrl.m_u4AAOXSize);
    MY_LOG("YSize     = 0x%04x", m_MVHDRBufCtrl.m_u4AAOYSize);
    MY_LOG("NumBit    = 0x%04x", m_MVHDRBufCtrl.m_u1NumBit);

    sem_init( &m_semPD,    0, 0);
    sem_init( &m_semMVHDR, 0, 0);

    // allocate and enqueue HW buffer
    for (MINT32 i = 0; i < MAX_AAO_BUFFER_CNT; i++)
    {
        if( m_EnPDAF)
        {
            m_PDBufCtrl.m_rAAOBufInfo[i].useNoncache = 0; // improve the performance
            m_PDBufCtrl.m_rAAOBufInfo[i].status = PDAF_BUFCTRL_ENUM_AVAIL;
            allocateBuf( m_PDBufCtrl.m_rAAOBufInfo[i], m_PDBufCtrl.m_u4AAOBufSize);
            enqueueHwBuf(m_PDBufCtrl.m_rAAOBufInfo[i], _PD_BUF_SEL_);
        }

        if( m_EnMVHDR)
        {
            m_MVHDRBufCtrl.m_rAAOBufInfo[i].useNoncache = 0;  // improve the performance
            allocateBuf(m_MVHDRBufCtrl.m_rAAOBufInfo[i],m_MVHDRBufCtrl.m_u4AAOBufSize);
            enqueueHwBuf(m_MVHDRBufCtrl.m_rAAOBufInfo[i]);
        }
    }

    int Val;
    ::sem_getvalue( &m_semPD, &Val);
    if( Val==MAX_AAO_BUFFER_CNT)
    {
        MY_LOG( "%s m_semPD = %d, init buffer done", __FUNCTION__, Val);
    }
    else
    {
        MY_LOG( "%s [WAR] m_semPD = %d, init buffer may not be correct", __FUNCTION__, Val);
    }



    return MTRUE;

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
CamSVBufMgr::
AAStatEnable(MBOOL En)
{
    MY_LOG_IF(m_bDebugEnable,"m_eSensorDev(%d) AAStatEnable(%d)\n",m_eSensorDev, En);
    return MTRUE;
}
