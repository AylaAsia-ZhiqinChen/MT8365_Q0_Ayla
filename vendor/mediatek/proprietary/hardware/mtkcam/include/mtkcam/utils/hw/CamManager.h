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

#ifndef _MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_UTILS_HW_CAMMANAGER_H_
#define _MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_UTILS_HW_CAMMANAGER_H_
//
#include <utils/Mutex.h>
#include <utils/Vector.h>
#include <utils/RefBase.h>
#include <utils/Condition.h>


/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace Utils {


/******************************************************************************
 *
 ******************************************************************************/
class CamManager {

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                Operations.
                                CamManager()
                                  : mLockMtx()
                                  , mLockFps()
                                  , mLockConfig()
                                  , mbRecord(false)
                                  , mbAvailable(true)
                                  , mbStereo(false)
                                  , mDeviceInConfig(-1)
                                  , mFrameRate0(0)
                                  , mFrameRate1(0)
                                {
                                    mvOpenId.clear();
                                    mvUsingId.clear();
                                }

    static CamManager*          getInstance();


    void                        incDevice(int32_t openId);

    void                        decDevice(int32_t openId);

    int32_t                     getFirstOpenId() const;

    //should be called before config sensor, must call configUsingDeviceDone() and decUsingDevice() after calling incUsingDevice()
    void                        incUsingDevice(int32_t openId);

    //should be called after config sensor, must call configUsingDeviceDone() after calling incUsingDevice()
    void                        configUsingDeviceDone(int32_t openId);

    //should be called after release sensor, must call decUsingDevice() after calling incUsingDevice()
    void                        decUsingDevice(int32_t openId);

    int32_t                     getFirstUsingId() const;

    uint32_t                    getDeviceCount() const;

    void                        setFrameRate(uint32_t const id, uint32_t const frameRate);

    uint32_t                    getFrameRate(uint32_t const id) const;

    bool                        isMultiDevice() const;

    bool                        setThermalPolicy(char* policy, bool usage);

    void                        getStartPreviewLock();

    void                        releaseStartPreviewLock();

    /**
     *  Set recording mode hint.
     *  The default value is FALSE. This should be called after starting/stopping recod.
     *
     *  param
     *      TRUE if camera start record.
     *      FALSE if camera stop record.
     *
     *  return
     *      N/A
     *
     */
    void                        setRecordingHint(bool const isRecord);


    /**
     *  Set HW available hint.
     *  The default value is TRUE. This should be called after starting/stopping preview.
     *
     *  param
     *      TRUE if driver support app to open another camera.
     *      FALSE if driver does NOT support app to open another camera.
     *
     *  return
     *      N/A
     *
     */
    void                        setAvailableHint(bool const isAvailable);

    /**
     *  Set stereo mode hint.
     *  The default value is FALSE. This should be called after init/uninit stereo mode.
     *
     *  param
     *      TRUE if camera in stereo mode.
     *      FALSE if camera not in stereo mode.
     *
     *  return
     *      N/A
     *
     */
    void                        setStereoHint(bool const isStereo);

    /**
     *  increase sensor power on count
     *
     *  param
     *      caller: caller name.
     *
     *  return
     *      N/A
     *
     */
    void                        incSensorCount(const char* caller);

    /**
     *  decrease sensor power on count
     *
     *  param
     *      caller: caller name.
     *
     *  return
     *      N/A
     *
     */
    void                        decSensorCount(const char* caller);

    /**
     *  get current sensor count
     *
     *  param
     *      N/A
     *
     *  return
     *      N/A
     *
     */
    int32_t                     getSensorCount() const;

    /**
     *  Get permission to open camera or to start preview.
     *
     *  param
     *      N/A
     *
     *  return
     *      TRUE: app can operate on this camera.
     *      FALSE: app can NOT operate on this camera.
     *
     */
    bool                        getPermission() const;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data Members.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    mutable android::Mutex              mLockMtx;
    mutable android::Mutex              mLockFps;
    mutable android::Mutex              mLockConfig;
    mutable android::Mutex              mLockStartPreview;
    android::Condition                  mConfigCond;
    bool                                mbRecord;
    bool                                mbAvailable;
    bool                                mbStereo;
    int32_t                             mDeviceInConfig;
    android::Vector<int32_t>            mvOpenId;
    android::Vector<int32_t>            mvUsingId;
    uint32_t                            mFrameRate0;
    uint32_t                            mFrameRate1;
    int32_t                             mSensorPowerCount = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    class UsingDeviceHelper : public virtual android::RefBase {
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //  Interfaces.
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        public:
                    UsingDeviceHelper(int32_t openId)
                        : mOpenId(openId)
                        , mbConfigDone(false)
                    {
                        CamManager::getInstance()->incUsingDevice(mOpenId);
                    }

            void    onLastStrongRef( const void* /*id*/)
                    {
                        if( !mbConfigDone )
                        {
                            CamManager::getInstance()->configUsingDeviceDone(mOpenId);
                        }
                        CamManager::getInstance()->decUsingDevice(mOpenId);
                    }

            void    configDone()
                    {
                        CamManager::getInstance()->configUsingDeviceDone(mOpenId);
                        mbConfigDone = true;
                    }

            bool    isFirstUsingDevice()
                    {
                        if( CamManager::getInstance()->getFirstUsingId() == mOpenId )
                        {
                            return true;
                        }
                        return false;
                    }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //  Data Members.
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        private:
            int32_t mOpenId;
            bool    mbConfigDone;
    };
};
/******************************************************************************
 *
 ******************************************************************************/

};  // namespace Utils
};  // namespace NSCam
#endif  //_MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_UTILS_HW_CAMMANAGER_H_

