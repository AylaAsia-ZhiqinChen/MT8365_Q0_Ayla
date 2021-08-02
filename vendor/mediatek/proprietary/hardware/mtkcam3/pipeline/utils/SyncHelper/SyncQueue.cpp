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

#define LOG_TAG "MtkCam/SyncQueue"
//
#include <assert.h>
#include <mtkcam/utils/std/Log.h>
#include <mtkcam/utils/std/ULog.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>

#include "SyncQueue.h"

CAM_ULOG_DECLARE_MODULE_ID(MOD_P1_SYNCHELPER);

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_ULOGMV("(%4d) [%s] " fmt, __LINE__, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_ULOGMD("(%4d) [%s] " fmt, __LINE__, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_ULOGMI("(%4d) [%s] " fmt, __LINE__, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_ULOGMW("(%4d) [%s] " fmt, __LINE__, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_ULOGME("(%4d) [%s] " fmt, __LINE__, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%4d) [%s] " fmt, __LINE__, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%4d) [%s] " fmt, __LINE__, __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

using namespace NSCam::v3::Utils::Imp;
/******************************************************************************
 *
 ******************************************************************************/
SyncQueue::
SyncQueue(
    int type
) : mType(type)
{
    MY_LOGD("create type (%d)", mType);
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
addUser(
    int camId
)
{
    std::lock_guard<std::mutex> lk(mUserListLock);
    auto iter = std::find(
                            mvUserList.begin(),
                            mvUserList.end(),
                            camId);
    if(iter != mvUserList.end()) {
        MY_LOGE("Please check flow, id(%d) already exist", camId);
    }
    else {
        MY_LOGI("user(%d)", camId);
        mvUserList.push_back(camId);
    }
    return true;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
removeUser(
    int camId
)
{
    std::lock_guard<std::mutex> lk(mUserListLock);
    auto iter = std::find(
                            mvUserList.begin(),
                            mvUserList.end(),
                            camId);
    if(iter != mvUserList.end()) {
        MY_LOGI("user(%d)", camId);
        mvUserList.erase(iter);
    }
    else {
        MY_LOGE("Please check flow, id(%d) already exist", camId);
    }
    return true;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
setCallback(
    std::weak_ptr<ISyncQueueCB> pCb
)
{
    if(auto p = pCb.lock()) {
        mpCb = pCb;
    }
    else {
        MY_LOGE("set callback fail");
    }
    return true;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
sequenceCheck(
    int const& frameno,
    std::vector<int> &dropFrameNoList
)
{
    std::lock_guard<std::mutex> lk(mLock);
    for(auto&& item:mvQueue) {
        // remove frame no less than target frameno.
        if(item.first < frameno) {
            MY_LOGD("frame(%d) is less than target(%d), remove",
                                    item.first,
                                    frameno);
            dropFrameNoList.push_back(item.first);
        }
        else if(item.first > frameno) {
            MY_LOGD("target(%d) is less than frame(%d), remove",
                                    item.first,
                                    frameno);
            dropFrameNoList.push_back(frameno);
        }
    }
    return (dropFrameNoList.size() == 0);
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
enque(
    string callerName,
    int const& frameno,
    SyncResultInputParams const& input,
    SyncResultOutputParams &output
)
{
    // =====================================================================
    mLock.lock();
    // =====================================================================
    // check still is user or not.
    if(!isSyncTargetExist(input)) {
        return false;
    }
    // check exist in mvQueue or not.
    std::shared_ptr<SyncElement> pSyncElement = nullptr;
    enqueDataUpdate(frameno, input, output, pSyncElement);
    if(pSyncElement != nullptr) {
        auto &data = pSyncElement->mSyncData;
        auto iter_data = std::find(
                            data.mSyncTarget.begin(),
                            data.mSyncTarget.end(),
                            input.CamId);
        // camid is exist in sync target.
        if(iter_data != data.mSyncTarget.end()) {
            std::unique_lock<std::mutex> condition_lk(pSyncElement->mConditionLock);
            data.miSyncTartgetSize++;
            if(data.mSyncTarget.size() == data.miSyncTartgetSize) {
                // wait all done.
                if(auto p = mpCb.lock()) {
                    MY_LOGD("[%s][f%d] CamId = %d notify", callerName.c_str(), frameno, input.CamId);
                    p->onEvent(
                            ISyncQueueCB::Type::SyncDone,
                            (SyncDataUpdaterType)mType,
                            data);
                }
                pSyncElement->mCV.notify_all();
                mLock.unlock();
                MY_LOGD("[%s][f%d] CamId = %d unlock", callerName.c_str(), frameno, input.CamId);
            }
            else {
                MY_LOGD("[%s][f%d] CamId = %d wait+", callerName.c_str(), frameno, input.CamId);
                // =====================================================================
                // before wait, it has to unlock this mutex.
                mLock.unlock();
                // =====================================================================
                pSyncElement->mCV.wait(condition_lk);
                MY_LOGD("[%s][f%d] CamId = %d wait-", callerName.c_str(), frameno, input.CamId);
            }
        }
    }
    else {
        // =====================================================================
        mLock.unlock();
        // =====================================================================
    }
    return true;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
dropItem(
    int const& frameno
)
{
    auto pSyncElement = getSyncElement(frameno);
    std::lock_guard<std::mutex> lk(mLock);
    if(!!pSyncElement) {
        if(auto p = mpCb.lock()) {
            auto &data = pSyncElement->mSyncData;
            MY_LOGD("[f%d] notify", frameno);
            p->onEvent(
                    ISyncQueueCB::Type::Drop,
                    (SyncDataUpdaterType)mType,
                    data);
        }
        pSyncElement->mCV.notify_all();
        MY_LOGD("[f%d] unlock", frameno);
    }
    auto iter = mvQueue.find(frameno);
    if(iter != mvQueue.end()) {
        mvQueue.erase(iter);
    }
    return true;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
removeItem(
    int const& frameno
)
{
    bool ret = false;
    std::lock_guard<std::mutex> lk(mLock);
    auto iter = mvQueue.find(frameno);
    if(iter != mvQueue.end()) {
        ret = true;
        mvQueue.erase(iter);
    }
    else {
        ret = false;
    }
    return ret;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
clear()
{
    std::lock_guard<std::mutex> lk(mLock);
    for(auto&& item:mvQueue) {
        if(item.second != nullptr) {
            if(auto p = mpCb.lock()) {
                MY_LOGI("[f%d] notify", item.second->mSyncData.mFrameNo);
                p->onEvent(
                        ISyncQueueCB::Type::SyncDone,
                        (SyncDataUpdaterType)mType,
                        item.second->mSyncData);
            }
        }
        item.second->mCV.notify_all();
        MY_LOGD("[r%d:f%d] unlock",
                        item.second->mSyncData.mRequestNo,
                        item.second->mSyncData.mFrameNo);
    }
    mvQueue.clear();
    return true;
}
/******************************************************************************
 *
 ******************************************************************************/
SyncDataUpdaterType
SyncQueue::
getSyncDataUpdateType() const
{
    return (SyncDataUpdaterType)mType;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
getFrameNoList(
    std::vector<int> &list
)
{
    std::lock_guard<std::mutex> lk(mLock);
    for(auto&& item:mvQueue) {
        list.push_back(item.first);
    }
    return true;
}
/******************************************************************************
 *
 ******************************************************************************/
std::vector<int>
SyncQueue::
getSyncTartgetList(
    SyncResultInputParams const& input
)
{
    std::vector<int> ret;
    // add itself
    ret.push_back(input.CamId);
    //
    IMetadata::IEntry entry = input.HalControl->entryFor(MTK_FRAMESYNC_ID);
    if(!entry.isEmpty()) {
        for (MUINT i=0; i<entry.count(); i++) {
            ret.push_back(entry.itemAt(i, Type2Type<MINT32>()));
        }
    }

    return ret;
}
/******************************************************************************
 *
 ******************************************************************************/
std::vector<int>
SyncQueue::
getMWMasterSlaveList(
    SyncResultInputParams const& input
)
{
    std::vector<int> ret;
    // set MW master slave id
    IMetadata::IEntry entry = input.HalControl->entryFor(MTK_STEREO_SYNC2A_MASTER_SLAVE);
    if(!entry.isEmpty()) {
        for (MUINT i=0; i<entry.count(); i++) {
            ret.push_back(entry.itemAt(i, Type2Type<MINT32>()));
        }
    }
    return ret;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
isSyncTargetExist(
    SyncResultInputParams const& input
)
{
    std::lock_guard<std::mutex> lk(mUserListLock);
    auto syncTargetList = getSyncTartgetList(input);
    bool ret = true;
    for(auto&& id:syncTargetList) {
        auto iter = std::find(
                                mvUserList.begin(),
                                mvUserList.end(),
                                id);
        // anyone not exist in user list, it means this request no need to sync.
        if(iter == mvUserList.end()) {
            ret &= false;
        }
    }
    return ret;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
enqueDataUpdate(
    int const& frameno,
    SyncResultInputParams const& input,
    SyncResultOutputParams &output,
    std::shared_ptr<SyncElement>& syncElement
)
{
    // if frame number not exist in mvQueue, add new element.
    // otherwise, update sync data element.
    auto iter = mvQueue.find(frameno);
    if(iter != mvQueue.end() && iter->second != nullptr) {
        auto input_iter =
                iter->second->mSyncData.mvInputParams.find(input.CamId);
        if(input_iter != iter->second->mSyncData.mvInputParams.end()) {
            MY_LOGA("(input) Duplicate insert! frameno(%d) input cam(%d)",
                        frameno, input.CamId);
        }
        else {
            SyncResultInputParams *p = const_cast<SyncResultInputParams*>(&input);
            iter->second->mSyncData.mvInputParams.insert({input.CamId, p});
        }
        auto output_iter =
                iter->second->mSyncData.mvOutputParams.find(input.CamId);
        if(output_iter != iter->second->mSyncData.mvOutputParams.end()) {
            MY_LOGA("(output) Duplicate insert! frameno(%d) input cam(%d)",
                        frameno, input.CamId);
        }
        else {
            SyncResultOutputParams *p = &output;
            iter->second->mSyncData.mvOutputParams.insert({input.CamId, p});
        }
        syncElement = iter->second;
    }
    else {
        // need create new.
        syncElement = std::make_shared<SyncElement>();
        assert(!syncElement);
        genSyncData(syncElement->mSyncData, input, output);
        mvQueue.insert({syncElement->mSyncData.mFrameNo, syncElement});
        MY_LOGA_IF(mvQueue.size() > 10, "please check flow");
    }
    return true;
}
/******************************************************************************
 *
 ******************************************************************************/
std::shared_ptr<SyncElement>
SyncQueue::
getSyncElement(
    int const& frameno
)
{
    std::shared_ptr<SyncElement> pSyncElement;
    {
        std::lock_guard<std::mutex> lk(mLock);
        auto iter = mvQueue.find(frameno);
        if(iter != mvQueue.end()) {
            pSyncElement = iter->second;
        }
        else {
            MY_LOGE("cannot get sync element");
            return nullptr;
        }
    }
    return pSyncElement;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
SyncQueue::
genSyncData(
    SyncData &syncdata,
    SyncResultInputParams const& input,
    SyncResultOutputParams &output
)
{
    auto syncTargetList = getSyncTartgetList(input);
    //
    syncdata.mFrameNo = input.frameNum;
    syncdata.mRequestNo = input.requestNum;
    syncdata.mSyncTarget = syncTargetList;
    syncdata.mMWMasterSlaveList = getMWMasterSlaveList(input);
    SyncResultInputParams *p_in = const_cast<SyncResultInputParams*>(&input);
    syncdata.mvInputParams.insert({input.CamId, p_in});
    SyncResultOutputParams *p_out = &output;
    syncdata.mvOutputParams.insert({input.CamId, p_out});
    return true;
}