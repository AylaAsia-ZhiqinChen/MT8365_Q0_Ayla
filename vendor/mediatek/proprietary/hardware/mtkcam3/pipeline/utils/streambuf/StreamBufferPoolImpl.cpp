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

#define LOG_TAG "MtkCam/StreamBufferPoolImpl"
//
#include "MyUtils.h"
#include <mtkcam3/pipeline/utils/streambuf/StreamBufferPoolImpl.h>
#include <mtkcam/utils/std/ULog.h>

CAM_ULOG_DECLARE_MODULE_ID(MOD_PIPELINE_UTILITY);
//
using namespace android;
using namespace NSCam;

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_ULOGMV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_ULOGMD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_ULOGMI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_ULOGMW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_ULOGME("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace v3 {
namespace Utils {

/******************************************************************************
 *
 ******************************************************************************/
StreamBufferPoolImpl::
StreamBufferPoolImpl(char const* szPoolName)
    : android::Thread()
    , mPoolName((std::string("<Pool> ") + szPoolName))
    , mSystraceName_acquireFromPool(mPoolName + " acquireFromPool")
    , mSystraceName_releaseToPool(mPoolName + " releaseToPool")
{
    mLogLevel = ::property_get_int32("vendor.debug.camera.log.sbpool", 0);
}


StreamBufferPoolImpl::
~StreamBufferPoolImpl()
{
    MY_LOGD_IF(mLogLevel>1, "destroy pool %s", mPoolName.c_str());
}


/******************************************************************************
 *
 ******************************************************************************/

MERROR
StreamBufferPoolImpl::
initPoolImpl(
    char const* szCallerName,
    size_t maxNumberOfBuffers,
    size_t minNumberOfInitialCommittedBuffers
)
{
    MY_LOGD("initPoolImpl %s, max %zu, min %zu",
            szCallerName, maxNumberOfBuffers, minNumberOfInitialCommittedBuffers);
    if( szCallerName == NULL ||
        maxNumberOfBuffers == 0 ||
        maxNumberOfBuffers < minNumberOfInitialCommittedBuffers )
    {
        MY_LOGE("wrong params: %s, %zu, %zu",
                szCallerName,
                maxNumberOfBuffers,
                minNumberOfInitialCommittedBuffers);
        return INVALID_OPERATION;
    }

    {
        android::Mutex::Autolock _l(mLock);

        for( size_t i = minNumberOfInitialCommittedBuffers; i-- > 0; )
        {
            MUINT32 construct_result = 0;
            if( do_construct(construct_result) == NO_MEMORY ) {
                MY_LOGE("do_construct allocate buffer failed");
                return NO_MEMORY;
            }
            mlAvailableBuf.push_back(construct_result);
            miAvailableCount++;
            CAM_TRACE_INT(mPoolName.c_str(), miAvailableCount.load());
        }

        muToAllocCnt = maxNumberOfBuffers - minNumberOfInitialCommittedBuffers;
        ////
    }

    return OK;
}  //initPool


MVOID
StreamBufferPoolImpl::
signalUserLocked()
{
    if( mWaitingList.size() )
        (*mWaitingList.begin())->signal();
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
StreamBufferPoolImpl::
dumpPoolImpl() const
{
    android::Mutex::Autolock _l(mLock);
    MY_LOGD("dumpPoolImpl +");

    // available
    MUINT32 counter = 0;
    if( !mlAvailableBuf.empty() )
    {
        typename android::List<MUINT32>::const_iterator iter = mlAvailableBuf.begin();
        while( iter != mlAvailableBuf.end() ) {//getInUseBuf(counter++)
            MY_LOGD("available buf %d", *iter);
            iter++;
        }
    }

    // in use
    counter = 0;
    if (!mlInUseBuf.empty()) {
        typename android::List<MUINT32>::const_iterator iter = mlInUseBuf.begin();
        while( iter != mlInUseBuf.end() ) {
            MY_LOGD("in use buf %d", *iter);
            iter++;
        }
    }

    //
    MY_LOGD("dumpPoolImpl -");
}

MVOID
StreamBufferPoolImpl::
uninitPoolImpl(
    char const* szCallerName
)
{
    MY_LOGD_IF( mLogLevel>1, "caller(%s) uninit pool(%s)", szCallerName, mPoolName.c_str());
    if( requestExitAndWait() != OK )
    {
        MY_LOGE("request exit thread failed");
    }

    {
        android::Mutex::Autolock _l(mLock);
        miAvailableCount = 0;
        mlAvailableBuf.clear();
        mlInUseBuf.clear();
        muToAllocCnt = 0;
        CAM_TRACE_INT(mPoolName.c_str(), miAvailableCount.load());
    }
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
StreamBufferPoolImpl::
commitPoolImpl(
    char const* szCallerName
)
{
    MY_LOGD_IF( mLogLevel>1, "caller(%s) commit pool(%s)", szCallerName, mPoolName.c_str());
    if( muToAllocCnt > 0 )
    {
        if( run(szCallerName) != OK )
        {
            MY_LOGE("run thread failed");
            return UNKNOWN_ERROR;
        }
    }

    //return join();
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
StreamBufferPoolImpl::
acquireFromPoolImpl(
    char const*     szCallerName,
    MUINT32&        rpBufferIndex,
    nsecs_t         nsTimeout
)
{
    CAM_TRACE_NAME(mSystraceName_acquireFromPool.c_str());

    android::Mutex::Autolock _l(mLock);
    MY_LOGD_IF( mLogLevel>2, "caller(%s) aquires buffer from pool(%s), available(%zu)",
                szCallerName, mPoolName.c_str(), mlAvailableBuf.size() );

    if( !mlAvailableBuf.empty() )
    {
        typename android::List<MUINT32>::iterator iter = mlAvailableBuf.begin();
        mlInUseBuf.push_back(*iter);
        rpBufferIndex = *iter;
        mlAvailableBuf.erase(iter);
        miAvailableCount--;
        CAM_TRACE_INT(mPoolName.c_str(), miAvailableCount.load());
        //
        return OK;
    }
    else if( ! isRunning() && muToAllocCnt > 0 )
    {
        MUINT32 construct_result = 0;
        mLock.unlock();
        MERROR err = do_construct(construct_result);
        mLock.lock();
        if(err == NO_MEMORY ) {
            MY_LOGE("do_construct allocate buffer failed");
            return NO_MEMORY;
        }
        mlInUseBuf.push_back(construct_result);
        rpBufferIndex = construct_result;

        --muToAllocCnt;

        return OK;

    }


    android::Condition cond;
    mWaitingList.push_back(&cond);

    //wait for buffer
    MY_LOGD("%s waiting %" PRId64 "ns (%s)", mSystraceName_acquireFromPool.c_str(), nsTimeout, szCallerName);
    cond.waitRelative(mLock, nsTimeout);

    android::List<android::Condition*>::iterator pCond = mWaitingList.begin();
    while( pCond != mWaitingList.end() ) {
        if( (*pCond) == &cond ) {
            mWaitingList.erase(pCond);
            break;
        }
        pCond++;
    }

    if( !mlAvailableBuf.empty() )
    {
        typename android::List<MUINT32>::iterator iter = mlAvailableBuf.begin();
        mlInUseBuf.push_back(*iter);
        rpBufferIndex = *iter;
        mlAvailableBuf.erase(iter);
        miAvailableCount--;
        CAM_TRACE_INT(mPoolName.c_str(), miAvailableCount.load());
        //
        return OK;
    }

    MY_LOGW("%s timeout (%s): buffer available:%d|%zu, toAlloc:%d",
            mSystraceName_acquireFromPool.c_str(), szCallerName,
            miAvailableCount.load(), mlAvailableBuf.size(), muToAllocCnt);
    return TIMED_OUT;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
StreamBufferPoolImpl::
releaseToPoolImpl(
    char const*     szCallerName,
    MUINT32         pBufferIndex
)
{
    CAM_TRACE_NAME(mSystraceName_releaseToPool.c_str());

    MY_LOGD_IF( mLogLevel>2, "caller(%s) release buffer to pool(%s), available(%zu)",
                szCallerName, mPoolName.c_str(), mlAvailableBuf.size() );
    {
        android::Mutex::Autolock _l(mLock);
        typename android::List<MUINT32>::iterator iter = mlInUseBuf.begin();
        while( iter != mlInUseBuf.end() )
        {
            if( *iter == pBufferIndex )
            {
                mlAvailableBuf.push_back(*iter);
                miAvailableCount++;
                mlInUseBuf.erase(iter);
                CAM_TRACE_INT(mPoolName.c_str(), miAvailableCount.load());
                //
                signalUserLocked();
                return OK;
            }
            iter++;
        }
    }

    MY_LOGE("%s (%s) cannot find buffer index %d",
        mSystraceName_releaseToPool.c_str(), szCallerName, pBufferIndex);
    dumpPoolImpl();

    return INVALID_OPERATION;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
StreamBufferPoolImpl::
threadLoop()
{
    //return false;
    bool next = false;
    MUINT32 construct_result = 0;
    if( do_construct(construct_result) == NO_MEMORY ) {
        MY_LOGE("do_construct allocate buffer failed");
        return NO_MEMORY;
    }

    {
        android::Mutex::Autolock _l(mLock);
        mlAvailableBuf.push_back(construct_result);
        miAvailableCount++;
        CAM_TRACE_INT(mPoolName.c_str(), miAvailableCount.load());

        next = (--muToAllocCnt) > 0;
        signalUserLocked();
    }
    return next;
}

};  //namespace Utils
};  //namespace v3
};  //namespace NSCam

