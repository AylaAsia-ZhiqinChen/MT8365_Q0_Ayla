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

#ifndef _MTK_HARDWARE_INCLUDE_MTKCAM_HAL_IHALCAMIO_ISP3_H_
#define _MTK_HARDWARE_INCLUDE_MTKCAM_HAL_IHALCAMIO_ISP3_H_

#include <mtkcam/def/common.h>
#include <mtkcam/utils/imgbuf/IImageBuffer.h>
#include <mtkcam/drv/IHalSensor.h>
#include <vector>
#include <map>
#include "../Port.h"
#include <mtkcam/drv/def/ispio_sw_scenario.h>
#include <mtkcam/utils/imgbuf/IImageBuffer.h>

/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace NSIoPipeIsp3 {
namespace NSCamIOPipe {
/******************************************************************************
 * @enum EPipeSignal
 * @brief Pipe Signal event
 ******************************************************************************/

enum EPipeSignal
{
    EPipeSignal_NONE    = 0x0000,           /*!< signal None */
    EPipeSignal_SOF     = 0x0001,           /*!< signal Start Of Frame */
    EPipeSignal_EOF     = 0x0002,           /*!< signal End of Frame */
};

typedef enum
{
   EPipe_P1Sel_SGG = 0, //0
   EPipe_P1Sel_SGG_EN,
   EPipe_P1Sel_LCS,
   EPipe_P1Sel_LCS_EN,
   EPipe_P1Sel_IMG,
   EPipe_P1Sel_UFE,     //5
   EPipe_P1Sel_EIS,
   EPipe_P1Sel_W2G,

   //raw-D
   EPipe_P1Sel_SGG_D = 0x100,
   EPipe_P1Sel_SGG_EN_D,
   EPipe_P1Sel_LCS_D,
   EPipe_P1Sel_LCS_EN_D,
   EPipe_P1Sel_IMG_D,
   EPipe_P1Sel_UFE_D,   //5
   EPipe_P1Sel_EIS_D,
   EPipe_P1Sel_W2G_D,

   EPipe_P1SEL_RSVD = 0xFFFFFFFF,

}EPipe_P1SEL;

enum EPipeRawFmt
{
    EPipe_PROCESSED_RAW = 0x0000,           /*!< Processed raw */
    EPipe_PURE_RAW      = 0x0001,           /*!< Pure raw */
};

//call back when enque request be dropped by drv
//for L-camera
typedef void (*fp_DropCB)(MUINT32 magic, void* cookie);

/******************************************************************************
 * @struct ResultMetadata
 *
 * @brief Pipe output metadata for describing given frame
 *
 * @param[out] mCropRect: Input CROP is applied BEFORE transforming and resizing.
 *                        This is based on the pipe's input port (TG)
 *
 * @param[out] mDstSize:  w/h of out image which's on dram
 *
 * @param[out] mTransform: ROTATION CLOCKWISE is applied AFTER FLIP_{H|V}.
 *
 * @param[out] mMagicNum_hal: magic number passed from middleware.
 *                            not sync if it is different from mMagicNum_tuning
 *
 * @param[out] mMagicNum_tuning: magic number passed from isp tuning
 *                               not sync if it is different from mMagicNum_hal
 *
 ******************************************************************************/
struct ResultMetadata
{
    MRect                           mCrop_s;    //scaler crop
    MSize                           mDstSize;   //scaler scaledown size
    MRect                           mCrop_d;    //damo   crop
    MUINT32                         mTransform;
    MBOOL                           m_bDummyFrame;          //deque: Ture if dummy frame.   enque: bypass
    MUINT32                         mMagicNum_hal;
    MUINT32                         mMagicNum_tuning;
    MUINT32                         mRawType;   //0 represents processed raw, 1 represents pure raw
    MINT64                          mTimeStamp;
    MVOID                           *mPrivateData; //refer to stISP_BUF_INFO*
    MUINT32                         mPrivateDataSize;

#if 0
                                    //
                                    ResultMetadata(
                                        MRect     rCropRect = MRect(MPoint(0,0), MSize(0,0)),
                                        MUINT32   rTranform = 0,
                                        MUINT32   rNum1 = 0,
                                        MUINT32   rNum2 = 0,
                                        MVOID*    privateData = 0,
                                        MUINT32   privateDataSize = 0
                                    )
                                        : mCropRect(rCropRect)
                                        , mTransform(rTranform)
                                        , mMagicNum_hal(rNum1)
                                        , mMagicNum_tuning(rNum2)
                                        , mPrivateData(privateData)
                                        , mPrivateDataSize(privateDataSize)
                                    {
                                    }
#endif
                                    ResultMetadata(
                                        MRect     rCropRect = MRect(MPoint(0,0), MSize(0,0)),
                                        MUINT32   rTranform = 0,
                                        MUINT32   rNum1 = 0,
                                        MUINT32   rNum2 = 0,
                                        MUINT32   rRawType = 0,
                                        MUINT64   TimeStamp = 0,
                                        MVOID*    privateData = 0,
                                        MUINT32   privateDataSize = 0
                                    )
                                        : mCrop_s(rCropRect)
                                        , mCrop_d(rCropRect)
                                        , mTransform(rTranform)
                                        , mMagicNum_hal(rNum1)
                                        , mMagicNum_tuning(rNum2)
                                        , mRawType(rRawType)
                                        , mTimeStamp(TimeStamp)
                                        , mPrivateData(privateData)
                                        , mPrivateDataSize(privateDataSize)
                                    {
                                        mDstSize = MSize(0,0);
                                        m_bDummyFrame = MFALSE;
                                    }
};



/******************************************************************************
 * @struct BufInfo
 *
 * @brief Pipe's output port parameter
 *
 * @param[in/out] mPortID: The output port ID of the pipe.
 *
 * @param[in/out] mBuffer: A pointer to an image buffer.
 *
 * @param[in] FrameBased: IO/Magic# for current image. supported only in FrameBased mode(camera 3.0)
 *
 ******************************************************************************/
struct  BufInfo
{
    PortID                          mPortID;                //deque: portID.                enque: PortID
    IImageBuffer*                   mBuffer;                //deque: input buf infor.       enque: output buf infor
    ResultMetadata                  mMetaData;              //deuqe: deque result.          enque: bypass
    //used when replace buffer , bypass
    MINT32                          mBufIdx;                //deque: bypass.                enque: bypass
    MUINT32                         mRawOutFmt;             //deque: bypass.                enque: pure_raw, processed_raw
    MUINT32                         mSecon;
    //this struct supported only in FrameBased Mode(camera 3.0)
    struct{
        MUINT32                         mMagicNum_tuning;       //deque: bypass.           enque: magic number for camera 3.0
        MSize                           mDstSize;               //w/h of out image which's on dram
        MRect                           mCropRect;              //crop image in TG coordinate axis(before scaler)
        MUINT32                         mSOFidx;                //deque: bypass.           enque: sof idx for camera 3.0
    }FrameBased;
    //TODO remove later. for can't test IImageHeap on LDVT.???
    MUINT32 mSize;
    MUINTPTR mVa;
    MUINTPTR mPa;
    MUINT64  mShutterTimeNs;
                                    //LDVT development used
                                    BufInfo(
                                        PortID const&   rPortID,
                                        IImageBuffer*   buffer,
                                        MUINT32         size,
                                        MUINTPTR        va,
                                        MUINTPTR        pa
                                    )
                                        : mPortID(rPortID)
                                        , mBuffer(buffer)
                                        , mSize(size)
                                        , mVa(va)
                                        , mPa(pa)
                                    {
                                        mBufIdx = 0xFFFF;
                                        mRawOutFmt = EPipe_PROCESSED_RAW;
                                        FrameBased.mMagicNum_tuning= 0x40000000;
                                        FrameBased.mDstSize = MSize(0,0);
                                        FrameBased.mCropRect = MRect(MPoint(0,0),MSize(0,0));
                                        FrameBased.mSOFidx = 0xFFFF;
                                        mShutterTimeNs = 0;
                                        mSecon = 0;
                                    }


                                    BufInfo(
                                        PortID const&   rPortID,
                                        IImageBuffer*   buffer,
                                        MUINT32         size,
                                        MUINTPTR        va,
                                        MUINTPTR        pa,
                                        ResultMetadata& metaData
                                    )
                                        : mPortID(rPortID)
                                        , mBuffer(buffer)
                                        , mMetaData(metaData)
                                        , mSize(size)
                                        , mVa(va)
                                        , mPa(pa)
                                    {
                                        mBufIdx = 0xFFFF;
                                        mRawOutFmt = EPipe_PROCESSED_RAW;
                                        FrameBased.mMagicNum_tuning= 0x40000000;
                                        FrameBased.mDstSize = MSize(0,0);
                                        FrameBased.mCropRect = MRect(MPoint(0,0),MSize(0,0));
                                        FrameBased.mSOFidx = 0xFFFF;
                                        mShutterTimeNs = 0;
                                        mSecon = 0;
                                    }

                                    BufInfo(
                                        PortID const&   rPortID,
                                        IImageBuffer*   buffer,
                                        MUINT32         size,
                                        MUINTPTR        va,
                                        MUINTPTR        pa,
                                        ResultMetadata& metaData,
                                        MINT32          BufIdx
                                    )
                                        : mPortID(rPortID)
                                        , mBuffer(buffer)
                                        , mMetaData(metaData)
                                        , mBufIdx(BufIdx)
                                        , mSize(size)
                                        , mVa(va)
                                        , mPa(pa)
                                    {
                                        mRawOutFmt = EPipe_PROCESSED_RAW;
                                        FrameBased.mMagicNum_tuning= 0x40000000;
                                        FrameBased.mDstSize = MSize(0,0);
                                        FrameBased.mCropRect = MRect(MPoint(0,0),MSize(0,0));
                                        FrameBased.mSOFidx = 0xFFFF;
                                        mShutterTimeNs = 0;
                                        mSecon = 0;
                                    }


                                    //ALPS_camera 1.0
                                    BufInfo(
                                        PortID const&   rPortID,
                                        IImageBuffer*   buffer,
                                        MINT32          idx = 0xFFFF
                                    )
                                        : mPortID(rPortID)
                                        , mBuffer(buffer)
                                        , mBufIdx(idx)
                                    {
                                        mRawOutFmt = EPipe_PROCESSED_RAW;
                                        mSize = 0;
                                        mVa = (MUINTPTR)NULL;
                                        mPa = (MUINTPTR)NULL;
                                        mShutterTimeNs = 0;
                                        mSecon = 0;
                                    }

                                    //ALPS_camera 3.0
                                    BufInfo(
                                        PortID const&   rPortID,
                                        IImageBuffer*   buffer,
                                        MSize           outsize,
                                        MRect           crop,
                                        MUINT32         magic,
                                        MINT32          idx = 0xFFFF
                                    )
                                        : mPortID(rPortID)
                                        , mBuffer(buffer)
                                        , mBufIdx(idx)
                                    {
                                            mRawOutFmt = EPipe_PROCESSED_RAW;
                                            FrameBased.mDstSize = outsize;
                                            FrameBased.mCropRect = crop;
                                            FrameBased.mMagicNum_tuning = magic;
                                            FrameBased.mSOFidx = 0xFFFF;
                                            mSize = 0;
                                            mVa = (MUINTPTR)NULL;
                                            mPa = (MUINTPTR)NULL;
                                            mShutterTimeNs = 0;
                                            mSecon = 0;
                                    }
                                    //ALPS_camera 3.0, iHDR
                                    BufInfo(
                                        PortID const&   rPortID,
                                        IImageBuffer*   buffer,
                                        MSize           outsize,
                                        MRect           crop,
                                        MUINT32         magic,
                                        MUINT32         sofidx,
                                        MINT32          idx = 0xFFFF
                                    )
                                        : mPortID(rPortID)
                                        , mBuffer(buffer)
                                        , mBufIdx(idx)
                                    {
                                            mRawOutFmt = EPipe_PROCESSED_RAW;
                                            FrameBased.mDstSize = outsize;
                                            FrameBased.mCropRect = crop;
                                            FrameBased.mMagicNum_tuning = magic;
                                            FrameBased.mSOFidx = sofidx;
                                            mSize = 0;
                                            mVa = (MUINTPTR)NULL;
                                            mPa = (MUINTPTR)NULL;
                                            mShutterTimeNs = 0;
                                            mSecon = 0;
                                    }
                                    //ALPS_camera 3.0, iHDR, RawOurFormat
                                    BufInfo(
                                        PortID const&   rPortID,
                                        IImageBuffer*   buffer,
                                        MSize           outsize,
                                        MRect           crop,
                                        MUINT32         magic,
                                        MUINT32         sofidx,
                                        MUINT32         rawFmt,
                                        MINT32          idx = 0xFFFF
                                    )
                                        : mPortID(rPortID)
                                        , mBuffer(buffer)
                                        , mBufIdx(idx)
                                    {
                                            mRawOutFmt = rawFmt;
                                            FrameBased.mDstSize = outsize;
                                            FrameBased.mCropRect = crop;
                                            FrameBased.mMagicNum_tuning = magic;
                                            FrameBased.mSOFidx = sofidx;
                                            mSize = 0;
                                            mVa = (MUINTPTR)NULL;
                                            mPa = (MUINTPTR)NULL;
                                            mShutterTimeNs = 0;
                                            mSecon = 0;
                                    }
                                    BufInfo(){
                                        mRawOutFmt = EPipe_PROCESSED_RAW;
                                        FrameBased.mMagicNum_tuning= 0x40000000;
                                        FrameBased.mDstSize = MSize(0,0);
                                        FrameBased.mCropRect = MRect(MPoint(0,0),MSize(0,0));
                                        FrameBased.mSOFidx = 0xFFFF;
                                        mBuffer = NULL;
                                        mBufIdx = 0;
                                        mSize = 0;
                                        mVa = (MUINTPTR)NULL;
                                        mPa = (MUINTPTR)NULL;
                                        mShutterTimeNs = 0;
                                        mSecon = 0;
                                    }
};




/******************************************************************************
 * @struct QBufInfo
 *
 * @brief vector of Pipe output's ports
 *
 * @param[in/out] mvOut: indicate vector of pipe output's ports
 *
 ******************************************************************************/
struct  QBufInfo
{
    MVOID*                          mpCookie;
    MUINT64                         mShutterTimeNs;
    std::vector<BufInfo>            mvOut;
                                    //
                                    QBufInfo()
                                        : mpCookie(NULL)
                                        , mShutterTimeNs(0)
                                        , mvOut()
                                    {
                                    }
};


/******************************************************************************
 * @struct FrameParam
 *
 * @brief frame setting per buffer
 *
 * @param[in] mPortID: The output port of the pipe
 *
 * @param[in] mDstSize: The desired size of the output buffer
 *
 * @param[in] mCropRect: The desired crop region based on the input port(from TG)
 *
 * @param[in] mTransform: The desired transformation.
 *                        ROTATION CLOCKWISE is applied AFTER FLIP_{H|V}.
 ******************************************************************************/
struct  FrameParam
{
    PortID                          mPortID;
    MSize                           mDstSize;
    MRect                           mCropRect;
    MUINT32                         mTransform;
                                    //
                                    FrameParam(
                                        PortID const&   rPortID,
                                        MSize const&    dstSize,
                                        MRect const&    cropRect,
                                        MUINT32 const   transform = 0
                                    )
                                        : mPortID(rPortID)
                                        , mDstSize(dstSize)
                                        , mCropRect(cropRect)
                                        , mTransform(transform)
                                    {
                                    }
};


/******************************************************************************
 * @struct QFrameParam
 *
 * @brief frame setting per frame (with multiple frames, if necessary)
 *
 * @param[in] mMagicNum: as an indicator for synchronization
 *
 * @param[in] mvOut: vector of FrameParam
 *
 ******************************************************************************/
struct  QFrameParam
{
    MUINT32                         mMagicNum;
    MUINT32                         mRawOutFmt;
    std::vector<FrameParam>         mvOut;
                                    //
                                    QFrameParam()
                                        : mMagicNum(0)
                                        , mRawOutFmt(0)
                                        , mvOut()
                                    {
                                    }
};


/******************************************************************************
 * @struct QInitParam
 *
 * @brief frame setting only in initialization
 *
 * @param[in] mPureRaw: 1: pure raw; 0: pre-process raw
 *
 * @param[in] mBitdepth: desired bitdepth. It can be different from tg input
 *
 * @note: mDstSize can <= CropRect.size if and only if this port have a build-in resizer.
 *        Otherwise, mDstSize == CropRect.size
 ******************************************************************************/
struct portInfo {
    PortID                          mPortID;
    EImageFormat                    mFmt;
    MSize                           mDstSize;
    MRect                           mCropRect;
    int                             mStride[3];
    MBOOL                           mPureRaw;
    MBOOL                           mPureRawPak;
    MBOOL                           mSecon;
                                portInfo(PortID _mPortID,
                                          EImageFormat   _mFmt,
                                          MSize  _mDstSize,
                                          MRect  _mCropRect,
                                          int    _mStride0,
                                          int    _mStride1,
                                          int    _mStride2,
                                          MBOOL  _mPureRaw = 0,
                                          MBOOL  _mPureRawPak = 0,
                                          MBOOL  _mSecon = 0)
                                    : mPortID(_mPortID)
                                    , mFmt(_mFmt)
                                    , mDstSize(_mDstSize)
                                    , mCropRect(_mCropRect)
                                    , mPureRaw(_mPureRaw)
                                    , mPureRawPak(_mPureRawPak)
                                    , mSecon(_mSecon)
                                {
                                    mStride[0]=_mStride0;
                                    mStride[1]=_mStride1;
                                    mStride[2]=_mStride2;
                                }
};
/******************************************************************************
 * @enum E_SEC_LEVEL_SEL
 *
 * @input cam secure level
 *
 ******************************************************************************/
typedef enum {
    E_SEC_LEVEL_NORMAL = 0,
    E_SEC_LEVEL_PROTECTED,
    E_SEC_LEVEL_SECURE
} E_SEC_LEVEL_SEL;

/******************************************************************************
 * @struct SecInfo
 *
 * @config secure cam's extra function
 *
 ******************************************************************************/
struct SecInfo {
    E_SEC_LEVEL_SEL buf_type;
    MUINTPTR chk_handle;
};


struct QInitParam
{
    MUINT32                             mRawType;   //ctrl sensor output test pattern or not
    MUINT32                             mBitdepth;
    std::vector<IHalSensor::ConfigParam>mSensorCfg;
    std::vector<portInfo>               mPortInfo;
    fp_DropCB                           m_DropCB;   //call back for drop enque request
    void*                               m_returnCookie;
    MBOOL                               m_PerFrameCtl;//1:supports per frame control
    MBOOL                               m_DynamicRawType;//1:dynamically switch processed/pure raw
    MBOOL                               m_bN3D;
    SecInfo                             m_SecureCam;
                                    //
                                    QInitParam(
                                               MUINT32 rRawType,
                                               MUINT32 const& rBitdepth,
                                               std::vector<IHalSensor::ConfigParam> const & rSensorCfg,
                                               std::vector<portInfo> const & rPortInfo
                                    )
                                        : mRawType(rRawType)
                                        , mBitdepth(rBitdepth)
                                        , mSensorCfg(rSensorCfg)
                                        , mPortInfo(rPortInfo)
                                    {
                                        m_DropCB = NULL;
                                        m_returnCookie = NULL;
                                        m_PerFrameCtl = MTRUE;
                                        m_DynamicRawType = MTRUE;
                                        m_SecureCam.buf_type = E_SEC_LEVEL_NORMAL;
                                        m_SecureCam.chk_handle = NULL;
                                    }
                                    //
                                    QInitParam(
                                               MUINT32 rRawType,
                                               MUINT32 const& rBitdepth,
                                               std::vector<IHalSensor::ConfigParam> const & rSensorCfg,
                                               std::vector<portInfo> const & rPortInfo,
                                               MBOOL dynamicSwt
                                    )
                                        : mRawType(rRawType)
                                        , mBitdepth(rBitdepth)
                                        , mSensorCfg(rSensorCfg)
                                        , mPortInfo(rPortInfo)
                                        , m_DynamicRawType(dynamicSwt)
                                    {
                                        m_DropCB = NULL;
                                        m_returnCookie = NULL;
                                        m_PerFrameCtl = MTRUE;
                                        m_bN3D = MFALSE;
                                        m_SecureCam.buf_type = E_SEC_LEVEL_NORMAL;
                                        m_SecureCam.chk_handle = NULL;
                                    }
};


/******************************************************************************
 *
 ******************************************************************************/
class ISignalConsumer
{

private:
    const int       mTid;
    const char*     mName;
    //
public:
    explicit ISignalConsumer(int _tid, const char* _name)
        : mTid(_tid),
          mName(_name)
        {}
    int                 getTid()                const   { return mTid; }
    const char*         getName()               const   { return mName; }
};


/******************************************************************************
 * @class IHalCamIO
 *
 * @brief CamIO base interface
 *
 * @details TG --> CamIO --> MEM
 ******************************************************************************/
class IHalCamIO
{
protected:
    virtual        ~IHalCamIO(){};

public:

    virtual MVOID   destroyInstance(char const* szCallerName) = 0;

    virtual MBOOL   start() = 0;

    virtual MBOOL   stop()  = 0;

    virtual MBOOL   abort()  = 0;

    virtual MBOOL   init(MBOOL EnableSec = MFALSE) = 0;

    virtual MBOOL   uninit()  = 0;

    virtual MBOOL   enque(QBufInfo const& rQBuf)   = 0;

    //virtual MBOOL   deque(QBufInfo& rQBuf, ResultMetadata& rResultMetadata, MUINT32 u4TimeoutMs = 0xFFFFFFFF) = 0;
    virtual MBOOL   deque(QBufInfo& rQBuf, MUINT32 u4TimeoutMs = 0xFFFFFFFF) = 0;

    virtual MBOOL   Reset() = 0;

    //one-time conifg
    virtual MBOOL   configPipe(QInitParam const& vInPorts)  = 0;

    //run-time config
    virtual MBOOL   configFrame(QFrameParam const& rQParam) = 0;

    //replace buffer
    virtual MBOOL   replace(BufInfo const& bufOld, BufInfo const& bufNew) = 0;

    virtual MBOOL sendCommand(MINT32 cmd, MINTPTR arg1, MINTPTR arg2, MINTPTR arg3) = 0;

public:
    virtual MVOID   attach(ISignalConsumer *pConsumer, MINT32 sensorIdx, EPipeSignal eType) = 0;

    virtual MBOOL   wait(ISignalConsumer *pConsumer, MINT32 sensorIdx, EPipeSignal eType, uint64_t ns_timeout = 0xFFFFFFFF) = 0;

    virtual MUINT32 getSensorTg(MINT32 sensorIdx) = 0;

    virtual MVOID   abortDeque() = 0;
//public:
    //virtual MUINT32 getIspReg(::ESoftwareScenario scen) = 0;

};


};
};
};

#endif
