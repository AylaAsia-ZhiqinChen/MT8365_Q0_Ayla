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

#define LOG_TAG "3dnr_hal"
//
#include <mtkcam/utils/std/Log.h>
#include <mtkcam/utils/std/ULog.h>
#include <3dnr_hal.h>
#include "hal/inc/camera_custom_3dnr.h"
#include <sys/resource.h>
#include <cutils/properties.h>  // For property_get().
#include <cutils/atomic.h>
#include <mtkcam/drv/def/ispio_sw_scenario.h>
#include <mtkcam3/feature/lmv/lmv_ext.h>
#include <mtkcam/aaa/IIspMgr.h>
#include <mtkcam/aaa/IHal3A.h>
#include <mtkcam/utils/std/Misc.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>
#include <mtkcam3/feature/3dnr/3dnr_isp_defs.h>
#include <math.h>

CAM_ULOG_DECLARE_MODULE_ID(MOD_3DNR_HAL);

using namespace NSCam;
using namespace NSIoPipe;
using namespace NSIoPipe::NSPostProc;

using namespace NSImageio;
using namespace NSImageio::NSIspio;
using namespace NS3Av3;
using NSCam::Utils::SENSOR_TYPE_GYRO;
using namespace NSCam::NR3D;

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_ULOGMV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_ULOGMD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_ULOGMI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_ULOGMW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_ULOGME("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_ULOGM_FATAL("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_ULOGM_FATAL("[%s] " fmt, __FUNCTION__, ##arg)

#define LOG_DBG(fmt, arg...)        CAM_ULOGMD("[%s] " fmt, __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

//
#if 0
#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")
#else
#define FUNC_START
#define FUNC_END
#endif


#define MAKE_HAL3DNR_OBJ(id) \
    template <> Hal3dnrObj<id>* Hal3dnrObj<id>::spInstance = 0; \
    template <> Mutex Hal3dnrObj<id>::s_instMutex(::Mutex::PRIVATE);

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define UNUSED(var) (void)(var)
#define NR3D_DUMP_FOLDER_PATH "/data/vendor/camera_dump/3dnr"


/*******************************************************************************
*
********************************************************************************/
template<const unsigned int sensorIdx>
Hal3dnr *Hal3dnrObj<sensorIdx>::getInstance(char const *userName)
{
    if(userName == NULL)
    {
        MY_LOGE("userName can't be NULL");
        assert();
    }

    AutoMutex lock(s_instMutex);
    if (NULL == spInstance)
    {
        spInstance = new Hal3dnrObj(userName);
    }
    else
    {
        MY_LOGD("User(%s) gets the existed 3dnr<%u> instance", userName, sensorIdx);
    }
    return spInstance;
}

template<const unsigned int sensorIdx>
void Hal3dnrObj<sensorIdx>::destroyInstance(char const *userName)
{
    AutoMutex lock(s_instMutex);

    if(userName == NULL)
    {
        MY_LOGE("userName can't be NULL");
        assert();
    }

    if (NULL != spInstance)
    {
        MY_LOGD("User(%s) to destroy 3dnr<%u> instance", userName, sensorIdx);
        delete spInstance;
        spInstance = NULL;
    }
}

// prepare 6 HAL3DNR_OBJ instances
// change the number if more than 6
MAKE_HAL3DNR_OBJ(0);
MAKE_HAL3DNR_OBJ(1);
MAKE_HAL3DNR_OBJ(2);
MAKE_HAL3DNR_OBJ(3);
MAKE_HAL3DNR_OBJ(4);
MAKE_HAL3DNR_OBJ(5);

// === Utility function ===
static MBOOL CreateMemBuf(MUINT32 memSize, android::sp<IImageBuffer>& spImageBuf)
{
    MBOOL ret = MTRUE;
    IImageBufferAllocator* pImageBufferAlloc = IImageBufferAllocator::getInstance();
    IImageBufferAllocator::ImgParam bufParam((size_t)memSize, 0);
    spImageBuf = pImageBufferAlloc->alloc("3dnr_HAL", bufParam);
    MY_LOGD("tnr mem size(%u) buf(%p)", memSize, spImageBuf.get());
    return ret;
}

static MBOOL DestroyMemBuf(android::sp<IImageBuffer>& spImageBuf)
{
    MBOOL ret = MTRUE;
    IImageBufferAllocator* pImageBufferAlloc = IImageBufferAllocator::getInstance();
    MY_LOGD("Destroy tnr buf(%p)", spImageBuf.get());
    if (spImageBuf != NULL)
    {
        pImageBufferAlloc->free(spImageBuf.get());
        spImageBuf = NULL;
    }

    return ret;
}

// === LIB3DNR related ====
Lib3dnrInfo::Lib3dnrInfo(const char *pUserName)
{
    if (pUserName != NULL)
    {
        strncpy(userName, pUserName, LIB_3DNR_USER_NAME_LEN-1);
//        MY_LOGD("Lib3dnrInfo(%s) constructed", userName);
    }
    else
    {
        MY_LOGE("!!warn: userName for lib3dnrInfo should NOT be NULL");
        assert();
    }
}

void Lib3dnrInfo::cleanup()
{
    if (pTnrLib)
    {
        if (::property_get_int32("vendor.debug.3dnr.lib", 0))
        {
            if (pTnrLib->TnrFeatureCtrl(TNR_FEATURE_SAVE_LOG, (MVOID *)&tnrLastestTS, NULL) != S_TNR_OK)
            {
                MY_LOGW("user(%s): !!warn: TNR_FEATURE_SAVE_LOG failed t=%" PRId64, userName, tnrLastestTS);
            }
        }
        pTnrLib->TnrReset();
        pTnrLib->destroyInstance(pTnrLib);
        pTnrLib = NULL;
    }
    if (tnrWorkBuf != NULL)
    {
        tnrWorkBuf->unlockBuf(userName);
        DestroyMemBuf(tnrWorkBuf);
    }
//    MY_LOGD("Lib3dnrInfo(%s) resource cleanup", userName);
}

Lib3dnrInfo::~Lib3dnrInfo()
{
    cleanup();
}

// 0: success
//-1: fail
MTKTnr* Lib3dnrInfo::createTnrLib()
{
    this->pTnrLib = MTKTnr::createInstance();
    MY_LOGD("user(%s): pTnrLib(%p) init, libMode=%d", userName, pTnrLib, libMode);
    if (pTnrLib)
    {
        TNR_SET_ENV_INFO_STRUCT tnrInitInfo;
        memset(&tnrInitInfo, 0, sizeof(TNR_SET_ENV_INFO_STRUCT));
        tnrInitInfo.GyroValid = MFALSE;
        tnrInitInfo.en_gyro_fusion = MFALSE;
        tnrInitInfo.GMV_mode = libMode;
        tnrInitInfo.debug = ::property_get_int32("vendor.debug.3dnr.lib", 0) ? MTRUE : MFALSE;

        if (pTnrLib->TnrInit(&tnrInitInfo) != S_TNR_OK)
        {
            MY_LOGE("!!err: user(%s): TnrInit failed", userName);
        }

        TNR_GET_PROC_INFO_STRUCT tnrGetProcInfo;
        if (pTnrLib->TnrFeatureCtrl(TNR_FEATURE_GET_PROC_INFO, NULL, &tnrGetProcInfo) != S_TNR_OK)
        {
            MY_LOGE("!!err: user(%s): TNR_FEATURE_GET_PROC_INFO failed", userName);
            goto err_createTnrLib;
        }
        else
        {
            if (CreateMemBuf(tnrGetProcInfo.ext_mem_size, tnrWorkBuf))
            {
                tnrWorkBuf->lockBuf("TnrWorkBuf", eBUFFER_USAGE_SW_MASK);
                if ( !tnrWorkBuf->getBufVA(0))
                {
                    MY_LOGE("!!err: user(%s), tnrWorkBuf create ImageBuffer fail", userName);
                    goto err_createTnrLib;
                }
                else
                {
                    TNR_SET_WORKING_BUFFER_STRUCT tnrSetProcInfo;
                    tnrSetProcInfo.extMemStartAddr = (MVOID *)tnrWorkBuf->getBufVA(0);
                    tnrSetProcInfo.extMemSize = tnrGetProcInfo.ext_mem_size;

                    if (pTnrLib->TnrFeatureCtrl(TNR_FEATURE_SET_WORK_BUF_INFO, &tnrSetProcInfo, NULL) != S_TNR_OK)
                    {
                        MY_LOGE("!!err: s(%s): TNR_FEATURE_SET_WORK_BUF_INFO failed", userName);
                        goto err_createTnrLib;
                    }
                }
            }
        }
    }

    return this->pTnrLib;

err_createTnrLib:

    cleanup();
    return NULL;
}

// Hal3dnr related ===
/*******************************************************************************
*
********************************************************************************/

static NR3D_PATH_ENUM determine3DNRPath(const NR3DHALParam &nr3dHalParam)
{
    if (nr3dHalParam.isIMGO == MFALSE && nr3dHalParam.isCRZUsed == MTRUE)
    {
        // RRZ + CRZ
        return NR3D_PATH_RRZO_CRZ;
    }
    else if (nr3dHalParam.isIMGO == MTRUE)
    {
        // IMGO crop
        return NR3D_PATH_IMGO;
    }
    else
    {
        // RRZ only
        return NR3D_PATH_RRZO;
    }
}

MUINT32 Hal3dnr::checkIso(const NR3DHALParam &nr3dHalParam)
{
// mktodo: make checkIso static function

    if (nr3dHalParam.needChkIso != MTRUE) // ex: dsdn2.0
    {
        MY_LOGD_IF(mLogLevel >= 2, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
             "3dnr/dsdn: do not check iso here for smoothness"
           , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey);
        return NR3D_ERROR_NONE;
    }

    MUINT32 errStatus = NR3D_ERROR_NONE;
    MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): iso=%d, Poweroff threshold=%d"
        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
        , nr3dHalParam.iso, nr3dHalParam.isoThreshold);

    if (nr3dHalParam.iso < nr3dHalParam.isoThreshold)
    {
        errStatus |= NR3D_ERROR_UNDER_ISO_THRESHOLD;
        return errStatus;
    }

    return errStatus;
}

MUINT32 Hal3dnr::checkVipiImgiFrameSize(const NR3DHALParam &nr3dHalParam,
    const Hal3dnrSavedFrameInfo &preSavedFrameInfo)
{
    MUINT32 errStatus = NR3D_ERROR_NONE;

    if (nr3dHalParam.vipiInfo.isValid != MTRUE)
    {
        errStatus |= NR3D_ERROR_NO_VIPI_INFO;
        return errStatus;
    }

    MSize vipiFrameSize = nr3dHalParam.vipiInfo.imgSize;
    const MRect &pImg3oFrameRect = nr3dHalParam.dstRect;

    // W/H of buffer (i.e. Current frame size) is determined, so check previous vs. current frame size for 3DNR.
    if (pImg3oFrameRect.s == vipiFrameSize)
    {
        return errStatus;
    }
    else
    {
        if (mSupportZoom3DNR)
        {
            // Zoom case
            if (vipiFrameSize.w > nr3dHalParam.dstRect.s.w)
            {
                MY_LOGD_IF(mLogLevel >= 1,
                    "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
                    "!!WARN: mkdbg_zoom: VIPI(%dx%d) > IMGI(%dx%d)"
                    , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                    , nr3dHalParam.vipiInfo.imgSize.w, nr3dHalParam.vipiInfo.imgSize.h
                    , nr3dHalParam.dstRect.s.w, nr3dHalParam.dstRect.s.h
                    );
            }
            else if (vipiFrameSize.w < nr3dHalParam.dstRect.s.w)
            {
                MY_LOGD_IF(mLogLevel >= 1,
                    "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
                    "!!WARN: mkdbg_zoom: IMGI(%dx%d) > VIPI(%dx%d)"
                    , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                    , nr3dHalParam.vipiInfo.imgSize.w, nr3dHalParam.vipiInfo.imgSize.h
                    , nr3dHalParam.dstRect.s.w, nr3dHalParam.dstRect.s.h
                    );
            }

            MINT32 nr3dPathID = determine3DNRPath(nr3dHalParam);
            switch(nr3dPathID)
            {
                case NR3D_PATH_RRZO: // === Rule: IMGO --> support IMGO-only, NOT support RRZO/IMGO switch ===
                    break;
                case NR3D_PATH_RRZO_CRZ: // === Rule:  RRZ + CRZ --> 3DNR OFF ===
                    errStatus |= NR3D_ERROR_NOT_SUPPORT;
                    return errStatus;
                case NR3D_PATH_IMGO: // === Rule: IMGO --> support IMGO-only, NOT support RRZO/IMGO switch ===
                    break;
                default:
                    MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
                        "invalid path ID(%d)"
                        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                        , nr3dPathID);
                    errStatus |= NR3D_ERROR_NOT_SUPPORT;
                    return errStatus;
            }

            if (preSavedFrameInfo.isCRZUsed != nr3dHalParam.isCRZUsed || preSavedFrameInfo.isIMGO != nr3dHalParam.isIMGO)
            {
                // 2017/03/29: Rule: IMGO/RRZO input switch: 3DNR default on by Algo's request
                MBOOL isInputChg3DNROn = ::property_get_int32("vendor.debug.3dnr.inputchg.on", 1);
                if (isInputChg3DNROn)
                {
                    MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
                        "RRZO/IMGO input change: nr3dPathID: %d, CRZUsed=%d -> %d, isIMGO=%d->%d --> 3DNR on"
                        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                        , nr3dPathID, preSavedFrameInfo.isCRZUsed, nr3dHalParam.isCRZUsed, preSavedFrameInfo.isIMGO, nr3dHalParam.isIMGO);
                }
                else
                {
                    MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
                        "RRZO/IMGO input change: nr3dPathID: %d, CRZUsed=%d -> %d, isIMGO=%d->%d --> 3DNR off"
                        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                        , nr3dPathID, preSavedFrameInfo.isCRZUsed, nr3dHalParam.isCRZUsed, preSavedFrameInfo.isIMGO, nr3dHalParam.isIMGO);
                    errStatus |= NR3D_ERROR_INPUT_SRC_CHANGE;
                    return errStatus;
                }
            }

            // === Binning: Sensor / Frontal ===
            if (nr3dHalParam.isBinning)
            {
                // TODO:
            }
        }
        else
        {
            // Current frame don't do 3DNR, but IMG3O still needs to output current frame for next run use.
            errStatus |= NR3D_ERROR_FRAME_SIZE_CHANGED;
            return errStatus;
        }
    }

    return errStatus;
}

MUINT32 Hal3dnr::check3DNRPolicy(const NR3DHALParam &nr3dHalParam, const NR3DHALResult &nr3dHalResult)
{
    MUINT32 errStatus = NR3D_ERROR_NONE;

    if ((errStatus=checkIso(nr3dHalParam)) != NR3D_ERROR_NONE)
    {
        return errStatus;
    }

    if ((errStatus=checkVipiImgiFrameSize(nr3dHalParam, nr3dHalResult.prevSavedFrameInfo)) != NR3D_ERROR_NONE)
    {
        return errStatus;
    }

    return errStatus;
}

void Hal3dnr::calCMV(const Hal3dnrSavedFrameInfo &prevSavedFrameInfo, NR3DMVInfo &gmvInfo)
{
    // For EIS 1.2 (use CMV). gmv_crp (t) = gmv(t) - ( cmv(t) - cmv(t-1) )

    // Use GMV and CMV
    gmvInfo.gmvX = (gmvInfo.gmvX - (gmvInfo.x_int - prevSavedFrameInfo.CmvX));
    gmvInfo.gmvY = (gmvInfo.gmvY - (gmvInfo.y_int - prevSavedFrameInfo.CmvY));
}

void Hal3dnr::calGMV(const NR3DHALParam &nr3dHalParam, NR3DMVInfo &gmvInfo,
    NR3DHALResult &nr3dHalResult)
{
    // The unit of Gmv is 256x 'pixel', so /256 to change unit to 'pixel'.  >> 1 << 1: nr3d_on_w must be even, so make mNmvX even. Discussed with James Liang.
    // The unit of Gmv is 256x 'pixel', so /256 to change unit to 'pixel'.  >> 1 << 1: nr3d_on_h must be even when image format is 420, so make mNmvX even. Discussed with TC & Christopher.
    gmvInfo.gmvX = (-(gmvInfo.gmvX) / LMV_GMV_VALUE_TO_PIXEL_UNIT);
    gmvInfo.gmvY = (-(gmvInfo.gmvY) / LMV_GMV_VALUE_TO_PIXEL_UNIT);

    MINT32 nr3dPathID = determine3DNRPath(nr3dHalParam);

    switch(nr3dPathID)
    {
        case NR3D_PATH_RRZO:
        case NR3D_PATH_IMGO:
            // Use GMV only.
            break;
        case NR3D_PATH_RRZO_CRZ:
            calCMV(nr3dHalResult.prevSavedFrameInfo, gmvInfo);
            break;
        default:
            MY_LOGE("r(%4d), f(%4d), uK(%d): invalid path ID(%d)"
                ,nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey, nr3dPathID);
            break;
    }

    gmvInfo.gmvX = gmvInfo.gmvX & ~1;    // Make it even.
    gmvInfo.gmvY = gmvInfo.gmvY & ~1;    // Make it even.
}

MUINT32 Hal3dnr::handleState(MUINT32 errStatus, NR3D_STATE_ENUM &stateMachine)
{
    MUINT32 result = errStatus;

    if (NR3D_ERROR_NONE == result)
    {
        if (stateMachine == NR3D_STATE_PREPARING) // Last frame is NR3D_STATE_PREPARING.
        {
            stateMachine = NR3D_STATE_WORKING;   // NR3D, IMG3O, VIPI all enabled.
        }
        else if (stateMachine == NR3D_STATE_STOP)
        {
            stateMachine = NR3D_STATE_PREPARING;
        }

    }
    else
    {
        if (stateMachine == NR3D_STATE_WORKING) stateMachine = NR3D_STATE_PREPARING;
    }

    return result;
}


MBOOL Hal3dnr::handleFrameZoomAlign(const NR3DHALParam &nr3dHalParam,
    const NR3DMVInfo &gmvInfo, NR3DHwParam &outNr3dHwParam,
    NR3DHALResult &nr3dHalResult)
{
    FUNC_START;
    MINT32 nr3dPathID = determine3DNRPath(nr3dHalParam);

    // === Algo code: start: align vipi/imgi ===
    MINT32 adjustGMVX = gmvInfo.gmvX;
    MINT32 adjustGMVY = gmvInfo.gmvY;

    MINT32 i4GMVX = adjustGMVX /LMV_GMV_VALUE_TO_PIXEL_UNIT;
    MINT32 i4GMVY  = adjustGMVY /LMV_GMV_VALUE_TO_PIXEL_UNIT;

    MINT32 i4PVOfstX1 = 0;
    MINT32 i4PVOfstY1 = 0;
    MINT32 i4CUOfstX1 = 0;
    MINT32 i4CUOfstY1 = 0;
    MINT32 i4FrmWidthCU = nr3dHalParam.dstRect.s.w;
    MINT32 i4FrmHeightCU = nr3dHalParam.dstRect.s.h;
    MINT32 i4FrmWidthPV = nr3dHalParam.vipiInfo.imgSize.w;
    MINT32 i4FrmHeightPV = nr3dHalParam.vipiInfo.imgSize.h;
    MINT32 i4CUOfstX2 = 0;
    MINT32 i4CUOfstY2 = 0;
    MINT32 i4PVOfstX2 = 0;
    MINT32 i4PVOfstY2 = 0;
    MINT32 i4OvlpWD = 0;
    MINT32 i4OvlpHT = 0;
    MINT32 NR3D_WD = 0;
    MINT32 NR3D_HT = 0;
    MINT32 VIPI_OFST_X = 0;
    MINT32 VIPI_OFST_Y = 0;
    MINT32 VIPI_WD = 0;
    MINT32 VIPI_HT = 0;
    MINT32 NR3D_ON_EN = 0;
    MINT32 NR3D_ON_OFST_X = 0;
    MINT32 NR3D_ON_OFST_Y = 0;
    MINT32 NR3D_ON_WD = 0;
    MINT32 NR3D_ON_HT = 0;
    MINT32 nmvX = 0;
    MINT32 nmvY = 0;

    switch(nr3dPathID)
    {
        case NR3D_PATH_RRZO: // RRZO only
            break;
        case NR3D_PATH_RRZO_CRZ: // CRZ case
            nr3dHalResult.currSavedFrameInfo.CmvX = gmvInfo.x_int;
            nr3dHalResult.currSavedFrameInfo.CmvY = gmvInfo.y_int;
            nmvX = (-i4GMVX - (nr3dHalResult.currSavedFrameInfo.CmvX - nr3dHalResult.prevSavedFrameInfo.CmvX)) & ~1;
            nmvY = (-i4GMVY - (nr3dHalResult.currSavedFrameInfo.CmvY - nr3dHalResult.prevSavedFrameInfo.CmvY)) & ~1;
            i4GMVX = -nmvX;
            i4GMVY = -nmvY;

            // Record last frame CMV X.
            nr3dHalResult.prevSavedFrameInfo.CmvX = nr3dHalResult.currSavedFrameInfo.CmvX;
            // Record last frame CMV Y.
            nr3dHalResult.prevSavedFrameInfo.CmvY = nr3dHalResult.currSavedFrameInfo.CmvY;
            break;

        case NR3D_PATH_IMGO: // IMGO only
            break;
        default:
            MY_LOGE("s(%u): fm(%d), r(%4d), f(%4d), uK(%d): !!err: should not happen"
                , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey);
            break;
    }

    if (i4GMVX<=0) { i4PVOfstX1 = -i4GMVX; i4CUOfstX1=0; }
    else { i4PVOfstX1 = 0; i4CUOfstX1=i4GMVX; }
    if (i4GMVY<=0) { i4PVOfstY1 = -i4GMVY; i4CUOfstY1=0; }
    else { i4PVOfstY1 = 0; i4CUOfstY1=i4GMVY; }

    if ((i4FrmWidthCU<=i4FrmWidthPV)&&(i4FrmHeightCU<=i4FrmHeightPV)) {   // case: vipi >= imgi
        i4CUOfstX2 = 0;
        i4CUOfstY2 = 0;
        i4PVOfstX2 = (i4FrmWidthPV-i4FrmWidthCU)/2;
        i4PVOfstY2 = (i4FrmHeightPV-i4FrmHeightCU)/2;
    }

    if ((i4FrmWidthCU >= i4FrmWidthPV)&&(i4FrmHeightCU>=i4FrmHeightPV)) {  // case: vipi <= imgi
        i4CUOfstX2 = (i4FrmWidthCU-i4FrmWidthPV)/2;
        i4CUOfstY2 = (i4FrmHeightCU-i4FrmHeightPV)/2;
        i4PVOfstX2 = 0;
        i4PVOfstY2 = 0;
    }

    i4OvlpWD = MIN(i4FrmWidthCU,i4FrmWidthPV)-abs(i4GMVX);
    i4OvlpHT = MIN(i4FrmHeightCU,i4FrmHeightPV)-abs(i4GMVY);

    NR3D_WD=i4FrmWidthCU;
    NR3D_HT=i4FrmHeightCU;

    VIPI_OFST_X=i4PVOfstX1+i4PVOfstX2;
    VIPI_OFST_Y=i4PVOfstY1+i4PVOfstY2;
    VIPI_WD=i4FrmWidthCU;
    VIPI_HT=i4FrmHeightCU;

    NR3D_ON_EN=1;
    NR3D_ON_OFST_X=i4CUOfstX1+i4CUOfstX2;
    NR3D_ON_OFST_Y=i4CUOfstY1+i4CUOfstY2;
    NR3D_ON_WD=i4OvlpWD;
    NR3D_ON_HT=i4OvlpHT;
    // === Algo code: end: align vipi/imgi ===

    // === save the vipi/imgi align info
    MUINT32 u4PixelToBytes = 0;
    MINT imgFormat = nr3dHalParam.vipiInfo.imgFmt;
    if (imgFormat== eImgFmt_YUY2) u4PixelToBytes = 2;
    if (imgFormat == eImgFmt_YV12 || imgFormat == eImgFmt_NV21) u4PixelToBytes = 1;
    // mktodo: f (imgFormat == UFBC) --->

    outNr3dHwParam.vipi_offst = VIPI_OFST_Y * nr3dHalParam.vipiInfo.bufStridesInBytes_p0 + VIPI_OFST_X * u4PixelToBytes;
    outNr3dHwParam.vipi_offst &= ~1;
    outNr3dHwParam.vipi_readW = i4FrmWidthCU &~1;
    outNr3dHwParam.vipi_readH = i4FrmHeightCU &~1;
    outNr3dHwParam.onSiz_onWd = i4OvlpWD &~1;
    outNr3dHwParam.onSiz_onHt = i4OvlpHT &~1;
    outNr3dHwParam.onOff_onOfStX = NR3D_ON_OFST_X &~1;
    outNr3dHwParam.onOff_onOfStY = NR3D_ON_OFST_Y &~1;
    outNr3dHwParam.ctrl_onEn = NR3D_ON_EN;

    FUNC_END;
    return MTRUE;
}


MBOOL Hal3dnr::handleFrameAlign(const NR3DHALParam &nr3dHalParam,
    const NR3DMVInfo &gmvInfo, NR3DHwParam &outNr3dHwParam)
{
    FUNC_START;
    if (nr3dHalParam.vipiInfo.isValid != MTRUE)
    {
        MY_LOGE("!!err: no vipi info");
        FUNC_END;
        return MFALSE;
    }

    // Config VIPI for 3DNR previous frame input.
    MUINT32 u4VipiOffset_X = 0, u4VipiOffset_Y = 0;
    MUINT32 u4Nr3dOffset_X = 0, u4Nr3dOffset_Y = 0;
    MSize imgSizeOvlp;
    MINT32 mvX = gmvInfo.gmvX;
    MINT32 mvY = gmvInfo.gmvY;

    // Calculate VIPI Offset X/Y according to NMV X/Y.
    u4VipiOffset_X = ((mvX >= 0) ? (mvX): (0));
    u4VipiOffset_Y = ((mvY >= 0) ? (mvY): (0));

    MUINT32 vipiW = nr3dHalParam.vipiInfo.imgSize.w;
    MUINT32 vipiH = nr3dHalParam.vipiInfo.imgSize.h;

    MINT imgFormat = nr3dHalParam.vipiInfo.imgFmt;
    size_t stride = nr3dHalParam.vipiInfo.bufStridesInBytes_p0;

    MUINT32 u4PixelToBytes = 0;
    if (imgFormat == eImgFmt_YUY2) u4PixelToBytes = 2;
    else if (imgFormat == eImgFmt_YV12 || imgFormat == eImgFmt_NV21) u4PixelToBytes = 1;

    // Calculate VIPI start addr offset.
    MUINT32 vipi_offst = u4VipiOffset_Y * stride + u4VipiOffset_X * u4PixelToBytes;    //in byte

    // Calculate VIPI valid region w/h.
    imgSizeOvlp.w = vipiW - abs(mvX);  // valid region w
    imgSizeOvlp.h = vipiH - abs(mvY);  // valid region h

    // Calculate NR3D Offset X/Y according to NMV X/Y.
    u4Nr3dOffset_X = ((mvX >= 0) ? (0) : (-mvX));
    u4Nr3dOffset_Y = ((mvY >= 0) ? (0) : (-mvY));

    // save align info into NR3DAlignParam
    outNr3dHwParam.onOff_onOfStX = u4Nr3dOffset_X;
    outNr3dHwParam.onOff_onOfStY = u4Nr3dOffset_Y;
    outNr3dHwParam.onSiz_onWd = imgSizeOvlp.w;
    outNr3dHwParam.onSiz_onHt = imgSizeOvlp.h;

    outNr3dHwParam.vipi_offst = vipi_offst;
    outNr3dHwParam.vipi_readW = imgSizeOvlp.w;    //in pixel
    outNr3dHwParam.vipi_readH = imgSizeOvlp.h;    //in pixel

    outNr3dHwParam.ctrl_onEn = 1;

    // check Nr3dHwParam result
    if ( ((outNr3dHwParam.onOff_onOfStX + outNr3dHwParam.onSiz_onWd) > nr3dHalParam.dstRect.s.w) ||
         ((outNr3dHwParam.onOff_onOfStY + outNr3dHwParam.onSiz_onHt) > nr3dHalParam.dstRect.s.h) ||
         (( u4VipiOffset_X + outNr3dHwParam.vipi_readW) > vipiW) ||
         (( u4VipiOffset_Y + outNr3dHwParam.vipi_readH) > vipiH)
       )
    {
        MY_LOGE("!!err: NR3DHwParam: s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
            "vipiOvlp(fmt=%d, offset=%d, p(%d,%d), w/h(%d,%d), vipiSize(%dx%d), "
            "onRegion(p(%d,%d), w/h(%dx%d)), dstSize(%dx%d), "
            "gmvInfogmvXY(%d,%d), confXY(%d,%d)), "
            // basic info
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
            // vipi
            , nr3dHalParam.vipiInfo.imgFmt, outNr3dHwParam.vipi_offst
                , u4VipiOffset_X, u4VipiOffset_Y
                , outNr3dHwParam.vipi_readW, outNr3dHwParam.vipi_readH
                , vipiW, vipiH
            // onRegion
            , outNr3dHwParam.onOff_onOfStX, outNr3dHwParam.onOff_onOfStY
                , outNr3dHwParam.onSiz_onWd, outNr3dHwParam.onSiz_onHt
                , nr3dHalParam.dstRect.s.w, nr3dHalParam.dstRect.s.h
            // gmvInfo
            , gmvInfo.gmvX, gmvInfo.gmvY, gmvInfo.confX, gmvInfo.confY
        );
    }
    else
    {
        MY_LOGD_IF(mLogLevel >= 2, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
            "vipiOvlp(fmt=%d, offset=%d, p(%d,%d), w/h(%d,%d), vipiSize(%dx%d), "
            "onRegion(p(%d,%d), w/h(%dx%d)), dstSize(%dx%d), "
            "gmvInfo(gmvXY(%d,%d), confXY(%d,%d)), "
            // basic info
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
            // vipi
            , nr3dHalParam.vipiInfo.imgFmt, outNr3dHwParam.vipi_offst
                , u4VipiOffset_X, u4VipiOffset_Y
                , outNr3dHwParam.vipi_readW, outNr3dHwParam.vipi_readH
                , vipiW, vipiH
            // onRegion
            , outNr3dHwParam.onOff_onOfStX, outNr3dHwParam.onOff_onOfStY
                , outNr3dHwParam.onSiz_onWd, outNr3dHwParam.onSiz_onHt
                , nr3dHalParam.dstRect.s.w, nr3dHalParam.dstRect.s.h
            // gmvInfo
            , gmvInfo.gmvX, gmvInfo.gmvY, gmvInfo.confX, gmvInfo.confY
        );
    }


    FUNC_END;
    return MTRUE;
}

/*******************************************************************************
*
********************************************************************************/
Hal3dnrBase*
Hal3dnr::getInstance(char const *userName, const MUINT32 sensorIdx)
{
    if (userName == NULL)
    {
        MY_LOGE("'userName(%s) for s(%u) is NULL", userName, sensorIdx);
        assert();
    }

    switch(sensorIdx)
    {
        case 0 : return Hal3dnrObj<0>::getInstance(userName);
        case 1 : return Hal3dnrObj<1>::getInstance(userName);
        case 2 : return Hal3dnrObj<2>::getInstance(userName);
        case 3 : return Hal3dnrObj<3>::getInstance(userName);
        case 4 : return Hal3dnrObj<4>::getInstance(userName);
        case 5 : return Hal3dnrObj<5>::getInstance(userName);
        default :
            MY_LOGW("s(%u:): Out of range: use 0 as default", sensorIdx);
            return Hal3dnrObj<0>::getInstance(userName);
    }
//    MY_LOGD("User(%s) gets 3dnr<%u> instance", userName, sensorIdx);
}

/*******************************************************************************
*
********************************************************************************/
MVOID
Hal3dnr::destroyInstance(char const *userName, const MUINT32 sensorIdx)
{
    Mutex::Autolock lock(mLock);
    if (userName == NULL)
    {
        MY_LOGE("'userName(%s) for s(%u) is NULL", userName, sensorIdx);
        assert();
    }
    MY_LOGD_IF(mLogLevel >= 1, "User(%s): s(%u) has %d users", userName, mSensorIdx, mUsers);

    //====== Check Reference Count ======
    if(mUsers <= 0)
    {
        switch(sensorIdx)
        {
            case 0 :
                Hal3dnrObj<0>::destroyInstance(userName);
                break;
            case 1 :
                Hal3dnrObj<1>::destroyInstance(userName);
                break;
            case 2 :
                Hal3dnrObj<2>::destroyInstance(userName);
                break;
            case 3 :
                Hal3dnrObj<3>::destroyInstance(userName);
                break;
            case 4 :
                Hal3dnrObj<4>::destroyInstance(userName);
                break;
            case 5 :
                Hal3dnrObj<5>::destroyInstance(userName);
                break;
            default :
                 MY_LOGW(" s(%u) is not exist", mSensorIdx);
                 MY_LOGW(" s(%u) is not exist", mSensorIdx);
        }
    }

}

/*******************************************************************************
*
********************************************************************************/
Hal3dnr::Hal3dnr(const char *userName, const MUINT32 sensorIdx)
    : mSensorIdx(sensorIdx)
{
    memcpy((void*) mUserName, (void*) userName, HAL_3DNR_USER_NAME_LEN-1);

    mLogLevel = ::property_get_int32("vendor.debug.camera.3dnr.log.level", 0);
    mSupportZoom3DNR = ::property_get_int32("vendor.debug.3dnr.zoom", 1); // zoom_3dnr default ON

    MY_LOGD_IF(mLogLevel >= 1, "User(%s) constructs 3dnr<%u> instance", mUserName, mSensorIdx);
}



/*******************************************************************************
*
********************************************************************************/
Hal3dnr::~Hal3dnr()
{
    MY_LOGD_IF(mLogLevel >= 1, "User(%s) destructs 3dnr<%u> instance", mUserName, mSensorIdx);
}

/*******************************************************************************
*
********************************************************************************/
MTKTnr *Hal3dnr::createTnrLib(MUINT32 featMask)
{
    Lib3dnrInfo *pTargetLib3dnrInfo = NULL;

    if (HAS_3DNR_BASIC(featMask))
    {
        pTargetLib3dnrInfo = &mTnrLibInfo_basic;
        pTargetLib3dnrInfo->libMode = 2; // rsc + lmv
    }
    else if (HAS_3DNR_BASIC_DSDN20(featMask))
    {
        pTargetLib3dnrInfo = &mTnrLibInfo_basic_dsdn20;
        pTargetLib3dnrInfo->libMode = 4; // rsc + lmv -> 1st-gmv -> 2nd-gmv
    }
    else if (HAS_3DNR_BASIC_DSDN25(featMask))
    {
        pTargetLib3dnrInfo = &mTnrLibInfo_basic_dsdn25;
        pTargetLib3dnrInfo->libMode = 5; // rsc + lmv + m-run
    }
    else if (HAS_3DNR_SMVRB(featMask))
    {
        pTargetLib3dnrInfo = &mTnrLibInfo_smvrB;
        pTargetLib3dnrInfo->libMode = 1; // lmv
    }
    else if (HAS_3DNR_SMVRB_DSDN20(featMask))
    {
        pTargetLib3dnrInfo = &mTnrLibInfo_smvrB_dsdn20;
        pTargetLib3dnrInfo->libMode = 1; // lmv
    }
    else if (HAS_3DNR_SMVRB_DSDN25(featMask))
    {
        pTargetLib3dnrInfo = &mTnrLibInfo_smvrB_dsdn25;
        pTargetLib3dnrInfo->libMode = 1; // lmv
    }
    else if (HAS_3DNR_VSDOF(featMask) || HAS_3DNR_W_T(featMask) || HAS_3DNR_TRICAM(featMask))
    {
        pTargetLib3dnrInfo = &mTnrLibInfo_multicam;
        pTargetLib3dnrInfo->libMode = 1;
    }
    else
    {
        // do nothing
    }

    if (pTargetLib3dnrInfo == NULL)
    {
        MY_LOGE("!!warn: no target lib3dnrInfo found for featMask(0x%x)", featMask);
        return NULL;
    }

    return (pTargetLib3dnrInfo->pTnrLib != nullptr) ? pTargetLib3dnrInfo->pTnrLib : pTargetLib3dnrInfo->createTnrLib();
}


MBOOL
Hal3dnr::init(const char *userName)
{
    Mutex::Autolock lock(mLock);

    FUNC_START;

    if (userName == NULL)
    {
        MY_LOGE("userName can't be NULL");
        assert();
    }
    MY_LOGD("User(%s) inits 3dnr<%u> instance", userName, mSensorIdx);

    //====== Check Reference Count ======

    if(mUsers < 0)
    {
        MY_LOGW("!!warn: s(%u): #users: %d is strange", mSensorIdx, mUsers);
    }

    MY_LOGD("s(%u): has %d users, to be %d", mSensorIdx, mUsers, mUsers+1);
    //====== Increase User Count ======
    android_atomic_inc(&mUsers);

    FUNC_END;
    return MTRUE;
}

MBOOL
Hal3dnr::uninit(const char *userName)
{
    Mutex::Autolock lock(mLock);

    FUNC_START;
    if (userName == NULL)
    {
        MY_LOGE("userName can't be NULL");
        assert();
    }
    MY_LOGD("User(%s) uninits 3dnr<%u> instance", userName, mSensorIdx);

    //====== Check Reference Count ======

    if(mUsers <= 0)
    {
        MY_LOGD("s(%u) has 0 users already", mSensorIdx);
        return MTRUE;
    }
    //====== Uninitialize ======

    MY_LOGD("s(%u) has %d users, to be %d", mSensorIdx, mUsers, mUsers-1);
    android_atomic_dec(&mUsers);    //decrease referebce count

    if (mpSensorProvider != NULL)
    {
        mpSensorProvider->disableSensor(SENSOR_TYPE_GYRO);
        mpSensorProvider = NULL;
    }

    FUNC_END;
    return MTRUE;
}


MBOOL
Hal3dnr::do3dnrFlow(
    const NR3DHALParam& nr3dHalParam, NR3DHALResult &nr3dHalResult)
{
    FUNC_START;

    MBOOL ret = MTRUE;

    // NOTES: check 3dnrlib-calculated GMV should be done outside Hal3dnr
    if (nr3dHalResult.isGMVInfoUpdated != MTRUE)
    {
        MY_LOGW("!!warn: GMV not updated yet, please call update3DNRMvInfo before do3dnrFlow");
    }

    // copy updated-gmvinfo from NR3DHALResult to NR3DHalParam
    nr3dHalParam.gmvInfo = nr3dHalResult.gmvInfo;
    print_NR3DHALParam(nr3dHalParam);    // print after gmvInfo is aligned

    if (savedFrameInfo(nr3dHalParam, nr3dHalResult) != MTRUE)
    {
        MY_LOGW_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
            "3DNR off: savedFrameInfo failed"
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey);
        // set NR3D off
        ret = MFALSE;
        goto exit_do3dnrFlow;
    }

    if (handle3DNROnOffPolicy(nr3dHalParam, nr3dHalResult) != MTRUE)
    {
        MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): 3DNR off: handle3DNROnOffPolicy failed"
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey);
        // set NR3D off
        ret = MFALSE;
        goto exit_do3dnrFlow;
    }

    if (handleAlignVipiIMGI(nr3dHalParam, nr3dHalResult.nr3dHwParam, nr3dHalResult) != MTRUE)
    {
        MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): 3DNR off: handleAlignVipiIMGI failed"
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey);
        // set NR3D off
        ret = MFALSE;
        goto exit_do3dnrFlow;
    }

exit_do3dnrFlow:

    print_NR3DHALResult(nr3dHalParam, nr3dHalResult);

    FUNC_END;

    return ret;
}

// !!NOTES:
// configNF3D is only applied to isp_50 or older
void Hal3dnr::configNR3D_legacy(void *pTuningData, void *p3A, const MRect& dstRect,
    const NR3DHwParam &nr3dHwParam)
{
    NS3Av3::NR3D_Config_Param param;

    if (nr3dHwParam.ctrl_onEn != 0)
    {
        param.enable = MTRUE;
        param.onRegion.p.x = nr3dHwParam.onOff_onOfStX & ~1;
        param.onRegion.p.y = nr3dHwParam.onOff_onOfStY & ~1;
        param.onRegion.s.w = nr3dHwParam.onSiz_onWd & ~1;
        param.onRegion.s.h = nr3dHwParam.onSiz_onHt & ~1;

        param.fullImg.p.x = dstRect.p.x & ~1;
        param.fullImg.p.y = dstRect.p.y & ~1;
        param.fullImg.s.w = dstRect.s.w & ~1;
        param.fullImg.s.h = dstRect.s.h & ~1;

        param.vipiOffst = nr3dHwParam.vipi_offst & ~1;
        param.vipiReadSize.w = nr3dHwParam.vipi_readW & ~1;
        param.vipiReadSize.h = nr3dHwParam.vipi_readH & ~1;

        if (p3A)
        {
            // turn ON 'pull up ISO value to gain FPS'

            NS3Av3::AE_Pline_Limitation_T params;
            params.bEnable = MTRUE;
            params.bEquivalent= MTRUE;
            // use property "camera.3dnr.forceisolimit" to control max_iso_increase_percentage
            // ex: setprop camera.3dnr.forceisolimit 200
            params.u4IncreaseISO_x100= get_3dnr_max_iso_increase_percentage();
            params.u4IncreaseShutter_x100= 100;
            ((NS3Av3::IHal3A*)p3A)->send3ACtrl(
                E3ACtrl_SetAEPlineLimitation, (MINTPTR)&params, 0);

            MY_LOGD_IF(mLogLevel >= 2, "s(%u): turn ON 'pull up ISO value to gain FPS': "
                "max: %d"
                , mSensorIdx, get_3dnr_max_iso_increase_percentage());
        }
    }
    else
    {
        param.enable = MFALSE;

        if (p3A)
        {
            // turn OFF 'pull up ISO value to gain FPS'

            //mp3A->modifyPlineTableLimitation(MTRUE, MTRUE,  100, 100);
            NS3Av3::AE_Pline_Limitation_T params;
            params.bEnable = MFALSE; // disable
            params.bEquivalent= MTRUE;
            params.u4IncreaseISO_x100= 100;
            params.u4IncreaseShutter_x100= 100;
            ((NS3Av3::IHal3A*)p3A) ->send3ACtrl(
                E3ACtrl_SetAEPlineLimitation, (MINTPTR)&params, 0);

            MY_LOGD_IF(mLogLevel >= 2, "s(%u): turn OFF  'pull up ISO value to gain FPS'"
                 , mSensorIdx);
       }
    }

    if (pTuningData != NULL)
    {
        void *pIspPhyReg = pTuningData;

        // log keyword for auto test
        MY_LOGD_IF(mLogLevel >= 1, "s(%u): postProcessNR3D: EN(%d)"
            , mSensorIdx, param.enable);
        if  ( auto pIspMgr = MAKE_IspMgr() ) {
            pIspMgr->postProcessNR3D(mSensorIdx, param, pIspPhyReg);
        }
    }

    FUNC_END;
    return;
}
MTKTnr *Hal3dnr::getTnrLibPtr(const NR3DHALParam &nr3dHalParam)
{
    MTKTnr *pTnrLib = nullptr;
    MUINT32 featMask = nr3dHalParam.featMask;

    const char *pTagName = "unknown";
    if (HAS_3DNR_BASIC(featMask))
    {
        pTnrLib = (mTnrLibInfo_basic.pTnrLib != nullptr) ? mTnrLibInfo_basic.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_BASIC();
    }
    else if (HAS_3DNR_BASIC_DSDN20(featMask))
    {
        pTnrLib = (mTnrLibInfo_basic_dsdn20.pTnrLib != nullptr) ? mTnrLibInfo_basic_dsdn20.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_BASIC_DSDN20();
    }
    else if (HAS_3DNR_BASIC_DSDN25(featMask))
    {
        pTnrLib = (mTnrLibInfo_basic_dsdn25.pTnrLib != nullptr) ? mTnrLibInfo_basic_dsdn25.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_BASIC_DSDN25();
    }
    else if (HAS_3DNR_SMVRB(featMask))
    {
        pTnrLib = (mTnrLibInfo_smvrB.pTnrLib != nullptr) ? mTnrLibInfo_smvrB.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_SMVRB();
    }
    else if (HAS_3DNR_SMVRB_DSDN20(featMask))
    {
        pTnrLib = (mTnrLibInfo_smvrB_dsdn20.pTnrLib != nullptr) ? mTnrLibInfo_smvrB_dsdn20.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_SMVRB_DSDN20();
    }
    else if (HAS_3DNR_SMVRB_DSDN25(featMask))
    {
        pTnrLib = (mTnrLibInfo_smvrB_dsdn25.pTnrLib != nullptr) ? mTnrLibInfo_smvrB_dsdn25.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_SMVRB_DSDN25();
    }
    else if (HAS_3DNR_VSDOF(featMask))
    {
        pTnrLib = (mTnrLibInfo_multicam.pTnrLib != nullptr) ? mTnrLibInfo_multicam.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_VSDOF();
    }
    else if (HAS_3DNR_W_T(featMask))
    {
        pTnrLib = (mTnrLibInfo_multicam.pTnrLib != nullptr) ? mTnrLibInfo_multicam.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_W_T();
    }
    else if (HAS_3DNR_TRICAM(featMask))
    {
        pTnrLib = (mTnrLibInfo_multicam.pTnrLib != nullptr) ? mTnrLibInfo_multicam.pTnrLib : createTnrLib(featMask);
        pTagName = TAG_TRICAM();
    }
    else
    {
        pTnrLib = NULL;
        pTagName = "unknown";
    }

    MY_LOGD_IF(mLogLevel >= 2, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
            "pTnrLib(%s): %p"
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
            , pTagName, pTnrLib);

    return pTnrLib;
}

MBOOL Hal3dnr::update3DNRMvInfo(const NR3DHALParam& nr3dHalParam, NR3D::NR3DMVInfo &gmvInfoResult,
    MBOOL &outIsGMVInfoUpdated)
{
    FUNC_START;

    MBOOL ret = MFALSE;
    gmvInfoResult = nr3dHalParam.gmvInfo;

    MTKTnr *pTnrLib = getTnrLibPtr(nr3dHalParam);
    if (pTnrLib != nullptr)
    {
        // mktodo: fillTnrSetProcInfo(nr3dHalParam)
        //====== 3dnr lib ======
        TNR_SET_PROC_INFO_STRUCT tnrSetProcInfo;
        memset(&tnrSetProcInfo, 0, sizeof(TNR_SET_PROC_INFO_STRUCT));
        const NR3D::NR3DMVInfo &gmvInfo = nr3dHalParam.gmvInfo;
        tnrSetProcInfo.lmv_info.lmv_gmv_conf[0] = gmvInfo.confX;
        tnrSetProcInfo.lmv_info.lmv_gmv_conf[1] = gmvInfo.confY;
        tnrSetProcInfo.lmv_info.lmv_gmv[0] = gmvInfo.gmvX / LMV_GMV_VALUE_TO_PIXEL_UNIT;
        tnrSetProcInfo.lmv_info.lmv_gmv[1] = gmvInfo.gmvY / LMV_GMV_VALUE_TO_PIXEL_UNIT;
        if (gmvInfo.maxGMVX != 0 && gmvInfo.maxGMVY != 0)
        {
            tnrSetProcInfo.lmv_info.lmv_max_gmv[0] = gmvInfo.maxGMVX;
            tnrSetProcInfo.lmv_info.lmv_max_gmv[1] = gmvInfo.maxGMVY;
        }
        else
        {
            tnrSetProcInfo.lmv_info.lmv_max_gmv[0] = gmvInfo.maxGMV;
            tnrSetProcInfo.lmv_info.lmv_max_gmv[1] = gmvInfo.maxGMV;
        }

        // dsdn related -- start
        if (HAS_3DNR_BASIC_DSDN20(nr3dHalParam.featMask) )
        {
            tnrSetProcInfo.gmv_result_info.tnr_gmv_int[0] = -(gmvInfo.gmvX / LMV_GMV_VALUE_TO_PIXEL_UNIT); // '-' for restore last run gmv
            tnrSetProcInfo.gmv_result_info.tnr_gmv_int[1] = -(gmvInfo.gmvY / LMV_GMV_VALUE_TO_PIXEL_UNIT); // '-' for restore last run gmv
            tnrSetProcInfo.gmv_result_info.tnr_gmv_float[0] = -(gmvInfo.gmvX_f_in_pixel); // '-' for restore last run gmv
            tnrSetProcInfo.gmv_result_info.tnr_gmv_float[1] = -(gmvInfo.gmvY_f_in_pixel); // '-' for restore last run gmv
            tnrSetProcInfo.gmv_result_info.tnr_gmv_conf[0] = gmvInfo.confX;
            tnrSetProcInfo.gmv_result_info.tnr_gmv_conf[1] = gmvInfo.confY;
            tnrSetProcInfo.gmv_result_info.tnr_max_gmv[0] = gmvInfo.maxGMVX;
            tnrSetProcInfo.gmv_result_info.tnr_max_gmv[1] = gmvInfo.maxGMVY;

            tnrSetProcInfo.AfCropWidth = nr3dHalParam.p2aSrcCrop.s.w;
            tnrSetProcInfo.AfCropHeight = nr3dHalParam.p2aSrcCrop.s.h;
            tnrSetProcInfo.AfResizeWidth = nr3dHalParam.dstRect.s.w;
            tnrSetProcInfo.AfResizeHeight = nr3dHalParam.dstRect.s.h;
        }
        // dsdn related -- end

        const NR3DRSCInfo &RSCInfo = nr3dHalParam.rscInfo;
        tnrSetProcInfo.rsc_info.fbuf_in_rsc_mv = (MUINT8*)(RSCInfo.pMV);
        tnrSetProcInfo.rsc_info.fbuf_in_rsc_var = (MUINT8*)(RSCInfo.pBV);
        tnrSetProcInfo.RSSoWidth = RSCInfo.rssoSize.w;
        tnrSetProcInfo.RSSoHeight = RSCInfo.rssoSize.h;

        tnrSetProcInfo.imgiWidth = (RSCInfo.isValid) ? RSCInfo.rrzoSize.w : nr3dHalParam.dstRect.s.w;
        tnrSetProcInfo.imgiHeight = (RSCInfo.isValid) ? RSCInfo.rrzoSize.h : nr3dHalParam.dstRect.s.h;
        tnrSetProcInfo.CRZoWidth = RSCInfo.rrzoSize.w;
        tnrSetProcInfo.CRZoHeight = RSCInfo.rrzoSize.h;
        tnrSetProcInfo.frame_t = gmvInfo.ts;

        tnrSetProcInfo.FrameUniqueKey = nr3dHalParam.uniqueKey;
        tnrSetProcInfo.FrameNumber = nr3dHalParam.frameNo;
        tnrSetProcInfo.FrameRequestNumber = nr3dHalParam.requestNo;

        MY_LOGD_IF(mLogLevel >= 2, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
            "TNR set info lmv(gmv(%6.2f,%6.2f), conf(%6d,%6d), max_gmv(%6d,%6d) ) rsc(%p,%p,%d,%d) img(%dx%d) crz(%dx%d) t=%" PRId64
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
            , tnrSetProcInfo.lmv_info.lmv_gmv[0], tnrSetProcInfo.lmv_info.lmv_gmv[1], tnrSetProcInfo.lmv_info.lmv_gmv_conf[0]
            , tnrSetProcInfo.lmv_info.lmv_gmv_conf[1], tnrSetProcInfo.lmv_info.lmv_max_gmv[0], tnrSetProcInfo.lmv_info.lmv_max_gmv[1]
            , tnrSetProcInfo.rsc_info.fbuf_in_rsc_mv, tnrSetProcInfo.rsc_info.fbuf_in_rsc_var, tnrSetProcInfo.RSSoWidth, tnrSetProcInfo.RSSoHeight
            , tnrSetProcInfo.imgiWidth, tnrSetProcInfo.imgiHeight, tnrSetProcInfo.CRZoWidth, tnrSetProcInfo.CRZoHeight
            , tnrSetProcInfo.frame_t);

        // print 3dnrlib input param
        print_TNR_SET_PROC_INFO(nr3dHalParam, tnrSetProcInfo);

        if (pTnrLib->TnrFeatureCtrl(TNR_FEATURE_SET_PROC_INFO, &tnrSetProcInfo, NULL) != S_TNR_OK)
        {
            MY_LOGW("s(%u): fm(%d), r(%4d), f(%4d), uK(%d): TNR_FEATURE_SET_PROC_INFO failed"
                , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey);
        }
        else
        {
            TNR_RESULT_INFO_STRUCT tnrResult;
            if (pTnrLib->TnrMain(&tnrResult) != S_TNR_OK)
            {
                MY_LOGW("s(%u): fm(%d), r(%4d), f(%4d), uK(%d): TnrMain failed"
                , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey);
            }
            else
            {
                // print 3dnrlib result
                print_TNR_RESULT_INFO(nr3dHalParam, tnrResult);

                if ((MINT32)fabs(tnrResult.tnr_gmv_int[0]) > tnrResult.tnr_max_gmv[0])
                {
                    MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): TNR gmvX(%d) > %d, force to max"
                        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                        , (MINT32)tnrResult.tnr_gmv_int[0], tnrResult.tnr_max_gmv[0]);
                    tnrResult.tnr_gmv_int[0] = tnrResult.tnr_gmv_int[0] > 0 ? tnrResult.tnr_max_gmv[0] : -tnrResult.tnr_max_gmv[0];
                }
                if ((MINT32)fabs(tnrResult.tnr_gmv_int[1]) > tnrResult.tnr_max_gmv[1])
                {
                    MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): TNR gmvY(%d) > %d, force to max"
                        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                        , (MINT32)tnrResult.tnr_gmv_int[1], tnrResult.tnr_max_gmv[1]);
                    tnrResult.tnr_gmv_int[1] = tnrResult.tnr_gmv_int[1] > 0 ? tnrResult.tnr_max_gmv[1] : -tnrResult.tnr_max_gmv[1];
                }
                gmvInfoResult.gmvX = -(tnrResult.tnr_gmv_int[0] * LMV_GMV_VALUE_TO_PIXEL_UNIT);
                gmvInfoResult.gmvY = -(tnrResult.tnr_gmv_int[1] * LMV_GMV_VALUE_TO_PIXEL_UNIT);


                // tnr_gmv_float[2]: add minus '-' for later 3dnr_hal GMV flow
                gmvInfoResult.gmvX_f_in_pixel = -tnrResult.tnr_gmv_float[0];
                gmvInfoResult.gmvY_f_in_pixel = -tnrResult.tnr_gmv_float[1];

                if (mLogLevel >= 2)
                {
                    MINT32 gmvXProp = ::property_get_int32("vendor.camera.3dnr.forcegmv.x", gmvInfoResult.gmvX);
                    if (gmvXProp != gmvInfoResult.gmvX)
                    {
                        MY_LOGD_IF(mLogLevel >= 2, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): gmvX property change: (%d) -> (%d)"
                            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                            , gmvInfoResult.gmvX, gmvXProp);
                        gmvInfoResult.gmvX = gmvXProp;
                        gmvInfoResult.gmvX_f_in_pixel = (float) gmvXProp;
                    }
                    MINT32 gmvYProp = ::property_get_int32("vendor.camera.3dnr.forcegmv.y", gmvInfoResult.gmvY);
                    if (gmvYProp != gmvInfoResult.gmvY)
                    {
                        MY_LOGD_IF(mLogLevel >= 2, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): gmvY property change: (%d) -> (%d)"
                            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                            , gmvInfoResult.gmvY, gmvYProp);
                        gmvInfoResult.gmvY = gmvYProp;
                        gmvInfoResult.gmvY_f_in_pixel = (float) gmvYProp;
                    }
                }
                gmvInfoResult.confX = tnrResult.tnr_gmv_conf[0];
                gmvInfoResult.confY = tnrResult.tnr_gmv_conf[1];
                gmvInfoResult.maxGMV = tnrResult.tnr_max_gmv[0];
                gmvInfoResult.maxGMVX = tnrResult.tnr_max_gmv[0];
                gmvInfoResult.maxGMVY = tnrResult.tnr_max_gmv[1];

                MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
                    "TNR valid(%d) GMV(%6d,%6d,%6d,%6d,%6d %6d)->(%6d,%6d,%6d,%6d,%6d %6d)"
                    , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                    , gmvInfo.status, gmvInfo.gmvX/ LMV_GMV_VALUE_TO_PIXEL_UNIT, gmvInfo.gmvY/ LMV_GMV_VALUE_TO_PIXEL_UNIT, gmvInfo.confX, gmvInfo.confY, gmvInfo.maxGMV, gmvInfo.maxGMV
                    , gmvInfoResult.gmvX/ LMV_GMV_VALUE_TO_PIXEL_UNIT, gmvInfoResult.gmvY/ LMV_GMV_VALUE_TO_PIXEL_UNIT, gmvInfoResult.confX, gmvInfoResult.confY, gmvInfoResult.maxGMVX, gmvInfoResult.maxGMVY);

                outIsGMVInfoUpdated = MTRUE;
                ret = MTRUE;

           }
        }
    }

    FUNC_END;
    return ret;
}

MBOOL Hal3dnr::savedFrameInfo(
    const NR3DHALParam& nr3dHalParam, NR3DHALResult &nr3dHalResult)
{
    // save data from current frame to previous frame
    nr3dHalResult.prevSavedFrameInfo = nr3dHalResult.currSavedFrameInfo;

    nr3dHalResult.currSavedFrameInfo.CmvX = nr3dHalParam.gmvInfo.x_int;
    nr3dHalResult.currSavedFrameInfo.CmvY = nr3dHalParam.gmvInfo.y_int;
    nr3dHalResult.currSavedFrameInfo.isCRZUsed = nr3dHalParam.isCRZUsed;
    nr3dHalResult.currSavedFrameInfo.isIMGO = nr3dHalParam.isIMGO;
    nr3dHalResult.currSavedFrameInfo.isBinning = nr3dHalParam.isBinning;

    return MTRUE;
}

MBOOL Hal3dnr::handle3DNROnOffPolicy(
    const NR3DHALParam& nr3dHalParam, NR3DHALResult &nr3dHalResult)
{
    FUNC_START;

    MUINT32 errStatus = check3DNRPolicy(nr3dHalParam, nr3dHalResult);

    // handle state
    NR3D_STATE_ENUM preState = nr3dHalResult.state;
    errStatus = handleState(errStatus,  nr3dHalResult.state);

    if(NR3D_ERROR_NONE != errStatus)
    {
        MY_LOGW_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): 3dnr state=(%d->%d), errStatus(%d)"
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
            , preState, nr3dHalResult.state, errStatus);
    }

    FUNC_END;
    return (NR3D_ERROR_NONE == errStatus) ? MTRUE : MFALSE;
}

MBOOL Hal3dnr::handleAlignVipiIMGI(const NR3DHALParam &nr3dHalParam,
    NR3DHwParam &outNr3dHwParam,
    NR3DHALResult &nr3dHalResult)
{

    FUNC_START;
    if (nr3dHalParam.vipiInfo.isValid != MTRUE)
    {
        MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): Invalid Vipi Info"
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey);

        FUNC_END;
        return MFALSE;
    }

    MSize vipiFrameSize = nr3dHalParam.vipiInfo.imgSize;
    const MRect &pImg3oFrameRect = nr3dHalParam.dstRect;

    NR3DMVInfo gmvInfo = nr3dHalParam.gmvInfo;
    const NSCam::NR3D::GyroData &gyroData = nr3dHalParam.gyroData;

    MTKTnr *pTnrLib = getTnrLibPtr(nr3dHalParam);
    if  (pTnrLib == NULL) // always trust 3dnrlib if available, otherwise, use the old logic
    {
        NR3DCustom::AdjustmentInput adjInput;
        adjInput.force3DNR = true;
        adjInput.setGmv(gmvInfo.confX, gmvInfo.confY, gmvInfo.gmvX, gmvInfo.gmvY);
        adjInput.setGyro(gyroData.isValid, gyroData.x, gyroData.y, gyroData.z);
        const NR3DRSCInfo &RSCInfo = nr3dHalParam.rscInfo;
        MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
                "RSCData: MV(%p), BV(%p), rrzo(%d,%d), rsso(%d,%d), rsc_sta(%x) valid(%d)"
                , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                , (void*) RSCInfo.pMV, (void*) RSCInfo.pBV
                , RSCInfo.rrzoSize.w, RSCInfo.rrzoSize.h, RSCInfo.rssoSize.w, RSCInfo.rssoSize.h, RSCInfo.staGMV, RSCInfo.isValid);
        adjInput.setRsc(RSCInfo.isValid, RSCInfo.pMV, RSCInfo.pBV, RSCInfo.rrzoSize.w, RSCInfo.rrzoSize.h,
                        RSCInfo.rssoSize.w, RSCInfo.rssoSize.h, RSCInfo.staGMV);
        MY_LOGD_IF(mLogLevel >= 2, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
            "Gyro isValid(%d), value(%.2f,%.2f,%.2f)"
            , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
            , (gyroData.isValid ? 1 : 0), gyroData.x, gyroData.y, gyroData.z);

        NR3DCustom::AdjustmentOutput adjOutput;
        NR3DCustom::adjust_parameters(adjInput, adjOutput, NULL);
        if (adjOutput.isGmvOverwritten)
        {
            MY_LOGW_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
                "AfterAdjusting: (confX,confY)=(%d,%d), gmvX(%d->%d), gmvY(%d->%d)"
                , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
                , gmvInfo.confX, gmvInfo.confY, gmvInfo.gmvX, adjOutput.gmvX, gmvInfo.gmvY, adjOutput.gmvY);
            gmvInfo.gmvX = adjOutput.gmvX;
            gmvInfo.gmvY = adjOutput.gmvY;
        }
    }

    //
    calGMV(nr3dHalParam, gmvInfo, nr3dHalResult);
    if (pImg3oFrameRect.s == vipiFrameSize)
    {
        if (handleFrameAlign(nr3dHalParam, gmvInfo, outNr3dHwParam) != MTRUE)
        {
            MY_LOGW("getFrameAlignInfo error");
            FUNC_END;
            return MFALSE;
        }
    }
    else // zoom-flow
    {
        // === Algo code: start: align vipi/imgi ===
        if (handleFrameZoomAlign(nr3dHalParam, gmvInfo,
            outNr3dHwParam, nr3dHalResult) != MTRUE)
        {
            MY_LOGW("getFrameZoomAlignInfo error");
            FUNC_END;
            return MFALSE;
        }
    }

    MY_LOGD_IF(mLogLevel >= 1, "s(%u): fm(%d), r(%4d), f(%4d), uK(%d): "
        "3dnr: ST=%d, p(%d), gmvX/Y org(%5d,%5d) new(%5d,%5d), int_x/y=(%5d,%5d), confX/Y(%d, %d), Gyro valid(%d), value(%f,%f,%f) "
        "f:%d, iszooming(%d) offst(%d) (%d,%d)->(%d,%d,%d,%d)"
        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
        , nr3dHalResult.state, (MINT32)determine3DNRPath(nr3dHalParam), nr3dHalParam.gmvInfo.gmvX/LMV_GMV_VALUE_TO_PIXEL_UNIT, nr3dHalParam.gmvInfo.gmvY/LMV_GMV_VALUE_TO_PIXEL_UNIT
        , gmvInfo.gmvX, gmvInfo.gmvY, gmvInfo.x_int, gmvInfo.y_int, gmvInfo.confX, gmvInfo.confY
        , (gyroData.isValid ? 1 : 0), gyroData.x, gyroData.y, gyroData.z
        , nr3dHalParam.frameNo, (pImg3oFrameRect.s != vipiFrameSize), outNr3dHwParam.vipi_offst
        , outNr3dHwParam.vipi_readW, outNr3dHwParam.vipi_readH
        , outNr3dHwParam.onOff_onOfStX, outNr3dHwParam.onOff_onOfStY, outNr3dHwParam.onSiz_onWd, outNr3dHwParam.onSiz_onHt);

    FUNC_END;
    return MTRUE;
}


MBOOL Hal3dnr::fillGyroForAdjustment(void *__adjInput)
{
    FUNC_START;
    NR3DCustom::AdjustmentInput *pAdjInput = static_cast<NR3DCustom::AdjustmentInput*>(__adjInput);

    NSCam::Utils::SensorData sensorData;
    if (mpSensorProvider != NULL &&
        mpSensorProvider->getLatestSensorData(SENSOR_TYPE_GYRO, sensorData))
    {
        pAdjInput->setGyro(true, sensorData.gyro[0], sensorData.gyro[1], sensorData.gyro[2]);
    }
    else
    {
        pAdjInput->isGyroValid = false;
    }

    MY_LOGD_IF(mLogLevel >= 2, "Gyro isValid(%d), value(%f,%f,%f)",
        (pAdjInput->isGyroValid ? 1 : 0), pAdjInput->gyroX, pAdjInput->gyroY, pAdjInput->gyroZ);

    FUNC_END;
    return (pAdjInput->isGyroValid ? MTRUE : MFALSE);
}

MBOOL Hal3dnr::updateISPMetadata(
    NSCam::IMetadata *pMeta_InHal, const NSCam::NR3D::NR3DTuningInfo &tuningInfo)
{
    FUNC_START;
    if (pMeta_InHal == NULL)
    {
        MY_LOGE("s(%u): no meta inHal: %p", mSensorIdx, pMeta_InHal);
        return MFALSE;
    }

    MY_LOGD_IF(mLogLevel >= 1,
        "s(%u): Enable:%d, GMV status:%d, XY(%d,%d); Input size:(%d,%d), Crop:(%d,%d)/(%d,%d)",
        mSensorIdx, tuningInfo.canEnable3dnrOnFrame,
        tuningInfo.mvInfo.status, tuningInfo.mvInfo.gmvX, tuningInfo.mvInfo.gmvY,
        tuningInfo.inputSize.w, tuningInfo.inputSize.h,
        tuningInfo.inputCrop.p.x, tuningInfo.inputCrop.p.y, tuningInfo.inputCrop.s.w, tuningInfo.inputCrop.s.h);

    if (tuningInfo.inputCrop.s.w > 0 && tuningInfo.inputCrop.s.h > 0)
    {
        const MSize &sl2eOriSize = tuningInfo.inputSize;
        const MRect &sl2eCropInfo = tuningInfo.inputCrop;
        const MSize &sl2eRrzSize = tuningInfo.inputCrop.s;

        IMetadata::setEntry<MSize>(pMeta_InHal, MTK_ISP_P2_ORIGINAL_SIZE, sl2eOriSize);
        IMetadata::setEntry<MRect>(pMeta_InHal, MTK_ISP_P2_CROP_REGION, sl2eCropInfo);
        IMetadata::setEntry<MSize>(pMeta_InHal, MTK_ISP_P2_RESIZER_SIZE, sl2eRrzSize);
    }
    else
    {
        MY_LOGE("s(%u): zero input size", mSensorIdx);
    }

    // mktodo: to remove Custom::Version
    //isp data for 3DNR version after 3.5
//    if (NR3DCustom::isEnabled3DNR35())
    if (1)
    {
        const NR3DHwParam &rNr3dParam = tuningInfo.nr3dHwParam;
        NR3DIspParam nr3dIspParam;
        nr3dIspParam.debugEnable = mForce3DNR ? MTRUE : MFALSE;
        NR3DIspAdaptiveInfo &adpI = nr3dIspParam.adaptiveInfo;
        NR3DIspConfigInfo &cofI = nr3dIspParam.configInfo;
        adpI.isGMVValid = tuningInfo.mvInfo.status ? MTRUE : MFALSE;
        adpI.GMVX = tuningInfo.mvInfo.gmvX;
        adpI.GMVY = tuningInfo.mvInfo.gmvY;
        adpI.confX = tuningInfo.mvInfo.confX;
        adpI.confY = tuningInfo.mvInfo.confY;
        if (tuningInfo.mvInfo.maxGMVX != 0 && tuningInfo.mvInfo.maxGMVY != 0)
        {
            adpI.MAX_GMVX = tuningInfo.mvInfo.maxGMVX;
            adpI.MAX_GMVY = tuningInfo.mvInfo.maxGMVY;
        }
        else
        {
            adpI.MAX_GMVX = tuningInfo.mvInfo.maxGMV;
            adpI.MAX_GMVY = tuningInfo.mvInfo.maxGMV;
        }
        adpI.frameReset = rNr3dParam.ctrl_onEn ? MFALSE : MTRUE;
        adpI.ISO_cutoff = tuningInfo.isoThreshold;
        adpI.isGyroValid = tuningInfo.gyroData.isValid;
        adpI.gyroXAccelX1000 = (MINT32)(tuningInfo.gyroData.x*1000);
        adpI.gyroYAccelX1000 = (MINT32)(tuningInfo.gyroData.y*1000);
        adpI.gyroZAccelX1000 = (MINT32)(tuningInfo.gyroData.z*1000);

        cofI.enable = rNr3dParam.ctrl_onEn ? MTRUE : MFALSE;
        cofI.onRegion.p.x = rNr3dParam.onOff_onOfStX & ~1;
        cofI.onRegion.p.y = rNr3dParam.onOff_onOfStY & ~1;
        cofI.onRegion.s.w = rNr3dParam.onSiz_onWd & ~1;
        cofI.onRegion.s.h = rNr3dParam.onSiz_onHt & ~1;
        cofI.fullImg.p.x = tuningInfo.inputCrop.p.x & ~1;
        cofI.fullImg.p.y = tuningInfo.inputCrop.p.y & ~1;
        cofI.fullImg.s.w = tuningInfo.inputCrop.s.w & ~1;
        cofI.fullImg.s.h = tuningInfo.inputCrop.s.h & ~1;
        cofI.vipiOffst = rNr3dParam.vipi_offst & ~1;
        cofI.vipiReadSize.w = rNr3dParam.vipi_readW & ~1;
        cofI.vipiReadSize.h = rNr3dParam.vipi_readH & ~1;

        MY_LOGD_IF(mLogLevel, "s(%u): 3dnr update ISP meta: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
            mSensorIdx,
            adpI.isGMVValid, adpI.GMVX, adpI.GMVY, adpI.confX, adpI.confX, adpI.MAX_GMVX, adpI.MAX_GMVY, adpI.frameReset, adpI.ISO_cutoff,
            adpI.isGyroValid, adpI.gyroXAccelX1000, adpI.gyroYAccelX1000, adpI.gyroZAccelX1000,
            cofI.enable, cofI.onRegion.p.x, cofI.onRegion.p.y, cofI.onRegion.s.w, cofI.onRegion.s.h,
            cofI.fullImg.p.x, cofI.fullImg.p.y, cofI.fullImg.s.w, cofI.fullImg.s.h, cofI.vipiOffst, cofI.vipiReadSize.w, cofI.vipiReadSize.h);

        IMetadata::Memory nr3dConfigData;
        nr3dConfigData.resize(sizeof(NR3DIspParam));
        memcpy(nr3dConfigData.editArray(), &nr3dIspParam, sizeof(NR3DIspParam));

        IMetadata::IEntry nr3dConfigEntry(MTK_3A_ISP_NR3D_HW_PARAMS);
        nr3dConfigEntry.push_back(nr3dConfigData, Type2Type< IMetadata::Memory >());
        pMeta_InHal->update(nr3dConfigEntry.tag(), nr3dConfigEntry);
    }
    else
    {
        // If 3DNR suspended on some frame, we have to reset the internal state of algorithm
        MINT32 frameReset = tuningInfo.canEnable3dnrOnFrame ? 0 : 1;

        // for backward compability. 3DNR version before 3.5
        IMetadata::IEntry entry(MTK_3A_ISP_NR3D_SW_PARAMS);//refer to ISP_NR3D_META_INFO_T
        entry.push_back(tuningInfo.mvInfo.gmvX, Type2Type< MINT32 >());
        entry.push_back(tuningInfo.mvInfo.gmvY, Type2Type< MINT32 >());
        entry.push_back(tuningInfo.mvInfo.confX, Type2Type< MINT32 >());
        entry.push_back(tuningInfo.mvInfo.confY, Type2Type< MINT32 >());
        entry.push_back(tuningInfo.mvInfo.maxGMV, Type2Type< MINT32 >());
        entry.push_back(frameReset, Type2Type< MINT32 >());
        entry.push_back(tuningInfo.mvInfo.status, Type2Type< MINT32 >()); //GMV_Status 0: invalid state
        entry.push_back(tuningInfo.isoThreshold, Type2Type< MINT32 >());
        entry.push_back((MINT32)tuningInfo.gyroData.isValid, Type2Type< MINT32 >());
        entry.push_back((MINT32)(tuningInfo.gyroData.x*1000), Type2Type< MINT32 >());
        entry.push_back((MINT32)(tuningInfo.gyroData.y*1000), Type2Type< MINT32 >());
        entry.push_back((MINT32)(tuningInfo.gyroData.z*1000), Type2Type< MINT32 >());

        pMeta_InHal->update(MTK_3A_ISP_NR3D_SW_PARAMS, entry);
    }
    FUNC_END;

    return MTRUE;
}


// === debug API: 3dnr flow ===
void Hal3dnr::print_NR3DHALParam(const NR3DHALParam& nr3dHalParam)
{
    MY_LOGD_IF(mLogLevel >= 1,
        " " // to align  print_NR3DHALResult
        " === s(%u): fm(%d), r(%4d), f(%4d), uK(%d) === { "
        "pTuningData = %p, "
        "p3A = %p, "
        "uniqueKey = %d, "
        "requestNo = %d, "
        "frameNo = %d, "
        "iso = %d, "
        "isoThreshold = %d, "
        "isCRZUsed = %d, "
        "isIMGO = %d, "
        "isBinning=%d, "
        "p2aSrcCrop: ((%d,%d), %dx%d),  "
        "RSCInfo: (pMv=%p, pBV=%p, rrzoSize: %dx%d, rssoSize: %dx%d, staGMV=%d, isValid=%d), "
        "gmvInfo: (status=%d, x_int=%d, y_int=%d, gmv(%d,%d)(256x), gmvX_f(%.2f,%.2f), gmvX_ds(%d,%d)(256x), conf(%d,%d), maxGmv(def=%d, %d, %d), ts=%" PRIu64 "), "
        "vipiInfo: (valid=%d %dx%d), "
        "dstRect: (%d,%d), %dx%d, "
        "gyroData(%.2f, %.2f, %.2f), isValid=%d, "
        " }"
        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
        , nr3dHalParam.pTuningData
        , nr3dHalParam.p3A
        , nr3dHalParam.uniqueKey
        , nr3dHalParam.requestNo
        , nr3dHalParam.frameNo
        , nr3dHalParam.iso
        , nr3dHalParam.isoThreshold
        , nr3dHalParam.isCRZUsed
        , nr3dHalParam.isIMGO
        , nr3dHalParam.isBinning
        , nr3dHalParam.p2aSrcCrop.p.x, nr3dHalParam.p2aSrcCrop.p.y, nr3dHalParam.p2aSrcCrop.s.w, nr3dHalParam.p2aSrcCrop.s.h
        , (void*)nr3dHalParam.rscInfo.pMV, (void*)nr3dHalParam.rscInfo.pBV, nr3dHalParam.rscInfo.rrzoSize.w, nr3dHalParam.rscInfo.rrzoSize.h, nr3dHalParam.rscInfo.rssoSize.w, nr3dHalParam.rscInfo.rssoSize.h, nr3dHalParam.rscInfo.staGMV, nr3dHalParam.rscInfo.isValid
        , nr3dHalParam.gmvInfo.status, nr3dHalParam.gmvInfo.x_int, nr3dHalParam.gmvInfo.y_int, nr3dHalParam.gmvInfo.gmvX, nr3dHalParam.gmvInfo.gmvY, nr3dHalParam.gmvInfo.gmvX_f_in_pixel, nr3dHalParam.gmvInfo.gmvY_f_in_pixel, nr3dHalParam.gmvInfo.gmvX_ds, nr3dHalParam.gmvInfo.gmvY_ds, nr3dHalParam.gmvInfo.confX, nr3dHalParam.gmvInfo.confY, nr3dHalParam.gmvInfo.maxGMV, nr3dHalParam.gmvInfo.maxGMVX, nr3dHalParam.gmvInfo.maxGMVY, nr3dHalParam.gmvInfo.ts
        , nr3dHalParam.vipiInfo.isValid
            , nr3dHalParam.vipiInfo.imgSize.w
            , nr3dHalParam.vipiInfo.imgSize.h
        , nr3dHalParam.dstRect.p.x, nr3dHalParam.dstRect.p.y, nr3dHalParam.dstRect.s.w, nr3dHalParam.dstRect.s.h
        , nr3dHalParam.gyroData.x, nr3dHalParam.gyroData.y, nr3dHalParam.gyroData.z, nr3dHalParam.gyroData.isValid
    );
}

void Hal3dnr::print_NR3DHALResult(const NR3DHALParam& nr3dHalParam,
    const NR3DHALResult& result)
{
    MY_LOGD_IF(mLogLevel >= 1,
        " === s(%u): fm(%d), r(%4d), f(%4d), uK(%d) === { "
        "nr3dHwParam (ctrl_onEn=%d, onOff_onOfStX=%d, onOff_onOfStY=%d, onSiz_onWd=%d, onSiz_onHt=%d, vipi_offst=%d, vipi_readW=%d, vipi_readH = %d), "
        "isgmvInfoUpdated=%d, "
        "gmvInfo: (status=%d, x_int=%d, y_int=%d, gmv(%d,%d)(256x), gmvX_f(%.2f,%.2f), gmvX_ds(%d,%d)(256x), conf(%d,%d), maxGmv(def=%d, %d, %d), ts=%" PRIu64 "), "
        " }"
        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
        , result.nr3dHwParam.ctrl_onEn, result.nr3dHwParam.onOff_onOfStX, result.nr3dHwParam.onOff_onOfStY, result.nr3dHwParam.onSiz_onWd, result.nr3dHwParam.onSiz_onHt, result.nr3dHwParam.vipi_offst, result.nr3dHwParam.vipi_readW, result.nr3dHwParam.vipi_readH
        , result.isGMVInfoUpdated
        , result.gmvInfo.status, result.gmvInfo.x_int, result.gmvInfo.y_int, result.gmvInfo.gmvX, result.gmvInfo.gmvY, result.gmvInfo.gmvX_f_in_pixel, result.gmvInfo.gmvY_f_in_pixel, result.gmvInfo.gmvX_ds, result.gmvInfo.gmvY_ds, result.gmvInfo.confX, result.gmvInfo.confY, result.gmvInfo.maxGMV, result.gmvInfo.maxGMVX, result.gmvInfo.maxGMVY, result.gmvInfo.ts
    );
}

// === debug API: 3dnr lib ===
void Hal3dnr::print_TNR_SET_PROC_INFO(const NR3DHALParam &nr3dHalParam, const TNR_SET_PROC_INFO_STRUCT& info)
{
   MY_LOGD_IF(mLogLevel >= 1,
        " === s(%u): fm(%d), r(%4d), f(%4d), uK(%d) === { "
        "lmv_info: (lmv_gmv_conf(%d,%d), lmv_gmv(%.2f,%.2f), lmv_max_gmv(%d,%d) ), "
        "gyro_info: (gyro_in_mv=%p, valid_gyro_num=%p), "
        "rsc_info: (fbuf_in_rsc_mv=%p, fbuf_in_rsc_var=%p), "
        "\t gmv_result_info: (tnr_gmv_int(%d,%d), tnr_gmv_float(%.2f,%.2f), tnr_gmv_conf(%d,%d), tnr_max_gmv(%d,%d) ), "
        "\t RSSoWidth = %d, "
        "\t RSSoHeight = %d, "
        "\t imgiWidth = %d, "
        "\t imgiHeight = %d, "
        "\t CRZoWidth = %d, "
        "\t CRZoHeight = %d, "
        "\t AfCropWidth = %d, "
        "\t AfCropHeight = %d, "
        "\t AfResizeWidth = %d, "
        "\t AfResizeHeight = %d, "
        "\t frame_t=%" PRIu64 ", "
        "\t FrameUniqueKey = %d, "
        "\t FrameNumber = %d, "
        "\t FrameRequestNumber = %d, "
        " }, "
        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
        , info.lmv_info.lmv_gmv_conf[0], info.lmv_info.lmv_gmv_conf[1], info.lmv_info.lmv_gmv[0], info.lmv_info.lmv_gmv[1], info.lmv_info.lmv_max_gmv[0], info.lmv_info.lmv_max_gmv[1]
        , info.gyro_info.gyro_in_mv, info.gyro_info.valid_gyro_num
        , info.rsc_info.fbuf_in_rsc_mv, info.rsc_info.fbuf_in_rsc_var
        , info.gmv_result_info.tnr_gmv_int[0], info.gmv_result_info.tnr_gmv_int[1], info.gmv_result_info.tnr_gmv_float[0], info.gmv_result_info.tnr_gmv_float[1]
            , info.gmv_result_info.tnr_gmv_conf[0], info.gmv_result_info.tnr_gmv_conf[1], info.gmv_result_info.tnr_max_gmv[0], info.gmv_result_info.tnr_max_gmv[1]
        , info.RSSoWidth
        , info.RSSoHeight
        , info.imgiWidth
        , info.imgiHeight
        , info.CRZoWidth
        , info.CRZoHeight
        , info.AfCropWidth
        , info.AfCropHeight
        , info.AfResizeWidth
        , info.AfResizeHeight
        , info.frame_t
        , info.FrameUniqueKey
        , info.FrameNumber
        , info.FrameRequestNumber
   );
   return;
}

void Hal3dnr::print_TNR_RESULT_INFO(const NR3DHALParam &nr3dHalParam, const TNR_RESULT_INFO_STRUCT& info)
{
    MY_LOGD_IF(mLogLevel >= 1,
        "  " // align  print_TNR_SET_PROC_INFO
        " === s(%u): fm(%d), r(%4d), f(%4d), uK(%d) === { "
        "(tnr_gmv_int(%d,%d), tnr_gmv_float(%.2f,%.2f), tnr_gmv_conf(%d,%d), tnr_max_gmv(%d,%d) ), "
        " }, "
        , mSensorIdx, nr3dHalParam.featMask, nr3dHalParam.requestNo, nr3dHalParam.frameNo, nr3dHalParam.uniqueKey
        , info.tnr_gmv_int[0], info.tnr_gmv_int[1], info.tnr_gmv_float[0], info.tnr_gmv_float[1]
            ,info.tnr_gmv_conf[0], info.tnr_gmv_conf[1], info.tnr_max_gmv[0], info.tnr_max_gmv[1]
   );
   return;
}

