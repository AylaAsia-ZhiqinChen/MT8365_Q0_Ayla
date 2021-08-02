#include <signal.h>
#include <cutils/log.h>

#include <utils/Trace.h>
#include <utils/AndroidThreads.h>

#include "osal_utils.h"
#include "MtkOmxVenc.h"
#include "MtkOmx2V4L2.h"

#include <cutils/properties.h>
#include <sched.h>
#include <assert.h>

#undef LOG_TAG
#define LOG_TAG "MtkOmxVencWFD"

#include "OMX_IndexExt.h"

#ifdef ATRACE_TAG
#undef ATRACE_TAG
#define ATRACE_TAG ATRACE_TAG_VIDEO
#endif//ATRACE_TAG

#include <utils/Trace.h>
//#include <utils/AndroidThreads.h>

void MtkOmxVenc::MtkOmxVencWFD()
{
    mSetWFDMode = OMX_FALSE;
    mWFDMode = OMX_FALSE;
    mEnableDummy = (OMX_BOOL) MtkVenc::EnableDummy("1");

    mTeeEncType = NONE_TEE;
    mIsSecureInst = OMX_FALSE;
    INIT_MUTEX(mDummyFrmLock);
}

void MtkOmxVenc::deMtkOmxVencWFD()
{
    DESTROY_MUTEX(mDummyFrmLock);
    DeInitSecEncParams();
}

void MtkOmxVenc::ComponentInitWFD(AndroidVendorConfigExtensionStore* ext)
{
    if(!strcmp(mComponentName, "OMX.MTK.VIDEO.ENCODER.AVC.secure")){
        InitSecEncParams();
        if(mIsSecureInst)
            property_set("vendor.mtk.secure.venc.alive","1");
    }
    MTK_OMX_LOGD("%s init sec 2 mIsSecureInst %d", __FUNCTION__,mIsSecureInst);
#ifdef SUPPORT_NATIVE_HANDLE
    if (!strcmp(mComponentName, "OMX.MTK.VIDEO.ENCODER.AVC.secure"))
    {
        BWC bwc;
        bwc.Profile_Change(BWCPT_VIDEO_WIFI_DISPLAY, true);
        MTK_OMX_LOGD("enter WFD BWCPT_VIDEO_WIFI_DISPLAY");
        mIsChangeBWC4WFD = OMX_TRUE;
    }
#endif
}
void MtkOmxVenc::ComponentDeInitWFD()
{
    if(mIsSecureInst)
        property_set("vendor.mtk.secure.venc.alive","0");
#ifdef SUPPORT_NATIVE_HANDLE
    if (OMX_TRUE == mIsChangeBWC4WFD)
    {
        BWC bwc;
        bwc.Profile_Change(BWCPT_VIDEO_WIFI_DISPLAY, false);
        MTK_OMX_LOGD("leave WFD BWCPT_VIDEO_WIFI_DISPLAY  !");
        mIsChangeBWC4WFD = OMX_FALSE;
    }
#endif
}
void MtkOmxVenc::WFD_OnInit(video_encode_param* param)
{
    IN_FUNC();
    if(OMX_TRUE == mIsSecureInst)
    {
        if (INHOUSE_TEE ==  mTeeEncType)
        {
            param->format = V4L2_PIX_FMT_YVU420;
        }
        else
        {
            param->format = V4L2_PIX_FMT_YVU420M;
        }
        setEncodeMode(mTeeEncType);
    }
    OUT_FUNC();
}
void MtkOmxVenc::WFD_OnDeInit()
{
}
void MtkOmxVenc::WFD_HandleStateSet(OMX_U32 nNewState)
{
}
void MtkOmxVenc::WFD_NotifyStopCommand()
{
}
OMX_ERRORTYPE MtkOmxVenc::WFD_EnableToggle()
{
        mSetWFDMode = OMX_TRUE;
        mWFDMode = OMX_TRUE;

        return OMX_ErrorNone;

}

OMX_ERRORTYPE MtkOmxVenc::WFD_DisableToggle()
{
    mSetWFDMode = OMX_FALSE;
    mWFDMode = OMX_FALSE;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::WFD_SetMode()
{
    int rRet = 0;

    if (mSetWFDMode != OMX_TRUE) return OMX_ErrorNotReady;

    mSetWFDMode = OMX_FALSE;
    mWFDMode = OMX_TRUE;

    rRet = ioctl_runtime_config(V4L2_CID_MPEG_MTK_ENCODE_SCENARIO, 1);
    if (-1 == rRet)
    {
        MTK_OMX_LOGE("[ERROR] set WFD mode fail");
        mWFDMode = OMX_FALSE;
    }

    if(!mEnableDummy) return OMX_ErrorNone;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::WFD_SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam)
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    switch (nParamIndex)
    {
        case OMX_IndexParamVideoBitrate:
            err = HandleWFDSetVideoBitrate((OMX_VIDEO_PARAM_BITRATETYPE *)pCompParam);
            break;
        case OMX_GoogleAndroidIndexEnableAndroidNativeHandle:
            WFD_EnableToggle();
            mEnableDummy = OMX_FALSE;
            MTK_OMX_LOGE("%s calls WFD_EnableToggle mEnableDummy %d, ", __func__, mEnableDummy);
            break;
        default:
            err = OMX_ErrorUnsupportedIndex;
    }
    return err;
}
OMX_ERRORTYPE MtkOmxVenc::HandleWFDSetVideoBitrate(OMX_VIDEO_PARAM_BITRATETYPE* pBitrateType)
{
    if (pBitrateType->eControlRate == OMX_Video_ControlRateMtkWFD)
    {
        WFD_EnableToggle();
    }
    else
    {
        if(!mIsSecureInst){
            WFD_DisableToggle();
        }
    }
    return OMX_ErrorNone;
}
OMX_ERRORTYPE MtkOmxVenc::WFD_BufferModifyBeforeEnc(OMX_BUFFERHEADERTYPE** ppInputBuf)
{
    if (mSetWFDMode != OMX_TRUE) return OMX_ErrorNotReady;
    if (!ppInputBuf || !*ppInputBuf) return OMX_ErrorNotReady;
    OMX_BUFFERHEADERTYPE* pInputBuf = *ppInputBuf;

    if (OMX_TRUE == mStoreMetaDataInBuffers)
    {
        OMX_U8 *aInputBuf   = pInputBuf->pBuffer + pInputBuf->nOffset;
        OMX_U32 _handle = 0;
        GetMetaHandleFromBufferPtr(aInputBuf, &_handle);

        gralloc_extra_ion_sf_info_t ext_info;
        gralloc_extra_query((buffer_handle_t)_handle, GRALLOC_EXTRA_GET_IOCTL_ION_SF_INFO, &ext_info);
        //MTK_OMX_LOGD("get sequence info: %u", ext_info.sequence);
        pInputBuf->nTickCount = (OMX_U32)ext_info.sequence;

        return OMX_ErrorNone;
    }
    return OMX_ErrorNotReady;
}

OMX_ERRORTYPE MtkOmxVenc::WFD_DrawYUVStripeLine(
    unsigned char** pY, unsigned char** pU, unsigned char** pV, OMX_U32 iDrawWidth, OMX_U32 iDrawHeight
    )
{
    if (OMX_TRUE == mWFDMode)
    {
        *pU = *pY + VENC_ROUND_N(iDrawWidth, 16) * VENC_ROUND_N(iDrawHeight, 16);
        *pV = *pU + ((VENC_ROUND_N((iDrawWidth >> 1), 16) * VENC_ROUND_N(iDrawHeight, 16) >> 1));
        //pU = pY+VENC_ROUND_N(iDrawWidth, 16)*VENC_ROUND_N(iDrawHeight, 32);
        //pV = pU+((VENC_ROUND_N((iDrawWidth>>1), 16)*VENC_ROUND_N(iDrawHeight, 32)>>1));

        return OMX_ErrorNone;
    }

    return OMX_ErrorNotReady;
}

void MtkOmxVenc::WFD_UpdateInputV4L2Buffer(OMX_BUFFERHEADERTYPE *pInputBuf, struct v4l2_buffer* qbuf)
{
    if(OMX_FALSE == mWFDMode) return;
    if(qbuf == NULL) return;
    if(mIsSecureInst)
        qbuf->flags |= V4L2_BUF_FLAG_NO_CACHE_INVALIDATE | V4L2_BUF_FLAG_NO_CACHE_CLEAN;
    if(pInputBuf == NULL) return;
    if(pInputBuf->nFlags & OMX_BUFFERFLAG_EOS && pInputBuf->nFilledLen == 0) return;
    int lumaSize, chromaSize;
    if( mEncParam.format == V4L2_PIX_FMT_YVU420M /*|| mEncParam.format == V4L2_PIX_FMT_YUV420M*/ ) {
        lumaSize = VENC_ROUND_N(mEncParam.buf_w,16) * mEncParam.buf_h;
        chromaSize = VENC_ROUND_N(VENC_ROUND_N(mEncParam.buf_w,16)>>1, 16) * (mEncParam.buf_h>>1);
    }
    else {
        lumaSize = VENC_ROUND_N(mEncParam.buf_w,16) * mEncParam.buf_h;
        chromaSize = lumaSize >> 2;
    }
    if(qbuf->length <= 1) {
        qbuf->m.planes[0].bytesused = lumaSize + (chromaSize << 1);
        qbuf->m.planes[0].data_offset = 0;
    }
}
void MtkOmxVenc::WFD_UpdateOutputV4L2Buffer(OMX_BUFFERHEADERTYPE *pOutputBuf, struct v4l2_buffer* qbuf)
{
    if(OMX_FALSE == mWFDMode) return;
    if(qbuf == NULL) return;
    if(mIsSecureInst)
        qbuf->flags |= V4L2_BUF_FLAG_NO_CACHE_INVALIDATE | V4L2_BUF_FLAG_NO_CACHE_CLEAN;
    if(pOutputBuf == NULL) return;
}
OMX_ERRORTYPE MtkOmxVenc::WFD_OnEnqueOutputBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf)
{
    if (mWFDMode != OMX_TRUE || mEnableDummy != OMX_TRUE) return OMX_ErrorNotReady;
    if (ppOutputBuf == NULL || *ppOutputBuf == NULL) return OMX_ErrorBadParameter;
    if (OMX_TRUE == mIsAllocateOutputNativeBuffers) return OMX_ErrorNone;

    int dummyIdx = DequeueBufferAdvance(&mFillThisBufQ);

    if(dummyIdx < 0) return OMX_ErrorNotReady;

    int outputBufferIndex = EncodeT_Output_GetBufferIndex(*ppOutputBuf);
    OMX_BUFFERHEADERTYPE* pDummyOutputBufHdr = EncodeT_Output_GetBufferHeaderPtr(dummyIdx);

    LOCK(mDummyFrmLock);
    mDummyBufferTable.add(outputBufferIndex, pDummyOutputBufHdr);
    UNLOCK(mDummyFrmLock);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::WFD_OnEnqueBuffer(OMX_BUFFERHEADERTYPE** ppInputBuf)
{
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::WFD_DequeOnGetInputBuffer(OMX_BUFFERHEADERTYPE* pInputBuf, struct v4l2_buffer* dqbuf)
{
    return OMX_ErrorNone;
}
OMX_ERRORTYPE MtkOmxVenc::WFD_DequeOnGetOutputBuffer(OMX_BUFFERHEADERTYPE* pOutputBufHdr, void* dqbuf)
{
    if (OMX_TRUE == mIsAllocateOutputNativeBuffers)
    {
        struct v4l2_buffer* buf = (struct v4l2_buffer*)dqbuf;
        native_handle_t *handle = (native_handle_t *)(pOutputBufHdr->pBuffer + pOutputBufHdr->nOffset);
        handle->data[2]  = buf->m.planes[0].bytesused;
        MTK_OMX_LOGE("[DEBUG] Set data[2] %d", handle->data[2]);
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::WFD_HandleDequeFrame(OMX_BUFFERHEADERTYPE* pOutputBufHdr)
{
    if (mWFDMode != OMX_TRUE) return OMX_ErrorNotReady;
    if (pOutputBufHdr == NULL) return OMX_ErrorNotReady;
    if (OMX_TRUE == mIsAllocateOutputNativeBuffers) return OMX_ErrorNone;

    int outputBufferIndex = EncodeT_Output_GetBufferIndex(pOutputBufHdr);

    LOCK(mDummyFrmLock);
    OMX_BUFFERHEADERTYPE* pDummyOutputBufHdr = mDummyBufferTable.valueFor(outputBufferIndex);
    mDummyBufferTable.add(outputBufferIndex, NULL);
    UNLOCK(mDummyFrmLock);

    if(pDummyOutputBufHdr == NULL)
        return OMX_ErrorNotReady;

    OMX_U8 *aOutputBuf = pDummyOutputBufHdr->pBuffer + pDummyOutputBufHdr->nOffset;

    /*if (mStoreMetaDataInOutBuffers)
    {
        if(1) // SetBitstreamSize4Framework
        {
            int *bitstreamLen = (int*)(pDummyOutputBufHdr + sizeof(struct VideoNativeHandleMetadata));
            *bitstreamLen = pDummyOutputBufHdr->nFilledLen;
            pDummyOutputBufHdr->nFilledLen = 8 + 4;
        }
    }*/

    if(!mStoreMetaDataInOutBuffers)
    {
        GenerateDummyBuffer(pDummyOutputBufHdr);
    }

    pDummyOutputBufHdr->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
    pDummyOutputBufHdr->nFlags |= OMX_BUFFERFLAG_DUMMY_NALU;
    pDummyOutputBufHdr->nTickCount = pOutputBufHdr->nTickCount;
    pDummyOutputBufHdr->nTimeStamp = pOutputBufHdr->nTimeStamp;

    dumpOutputBuffer(pDummyOutputBufHdr, aOutputBuf, pDummyOutputBufHdr->nFilledLen);
    HandleFillBufferDone(pDummyOutputBufHdr);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::GenerateDummyBuffer(OMX_BUFFERHEADERTYPE* pDummyOutputBufHdr)
{
    switch (mEncParam.codec)
    {
        case V4L2_PIX_FMT_H264:
            GenerateDummyBufferH264Enc(pDummyOutputBufHdr);
            break;
        default:
            pDummyOutputBufHdr->nFilledLen = 0;
            return OMX_ErrorNotReady;
    }

    return OMX_ErrorNone;
}
OMX_BOOL MtkOmxVenc::InitSecEncParams(void)
{
    int svp = MtkVenc::SvpSupport();
    mTeeEncType = NONE_TEE;
    if (svp)
    {
        if (MtkVenc::TrustTonicTeeSupport())
        {
            mTeeEncType = TRUSTONIC_TEE;
        }
        else if (MtkVenc::InHouseReady())
        {
            MTK_OMX_LOGE("%s ro.mtk_venc_inhouse_ready",__FUNCTION__);
            mTeeEncType = INHOUSE_TEE;
        }
        else if (MtkVenc::MicroTrustTeeSupport())
        {
            mTeeEncType = MICROTRUST_TEE;
        }
    }
    mIsSecureInst = (svp && mTeeEncType != NONE_TEE) ? OMX_TRUE : OMX_FALSE;
    if(mIsSecureInst)
        mIsSecureSrc = mIsSecureInst; // MtkOmxVencBuffer
    MTK_OMX_LOGD("%s svp support: %d, type: %d, mIsSecureInst %d, mIsSecureSrc %d",
    __FUNCTION__, svp, mTeeEncType, mIsSecureInst, mIsSecureSrc);
    if (OMX_TRUE == mIsSecureInst)
    {
        if (mTeeEncType == TRUSTONIC_TEE || mTeeEncType == MICROTRUST_TEE)
        {
            memset(&tlc, 0, sizeof(tlc));
            tlc.tlcLib = dlopen("/vendor/lib/AVCSecureVencCA.so", RTLD_LAZY);
            if (tlc.tlcLib != NULL) {
                tlc.tlcHandleCreate = (void *(*)())dlsym(tlc.tlcLib, "MtkH264SecVenc_tlcHandleCreate");
                tlc.tlcHandleRelease = (void (*)(void *))dlsym(tlc.tlcLib, "MtkH264SecVenc_tlcHandleRelease");
                tlc.tlcDumpSecMem = (int (*)(void *, uint32_t, void *, uint32_t))dlsym(tlc.tlcLib, "MtkH264SecVenc_tlcDumpSecMem");
                tlc.tlcFillSecMem = (int (*)(void *, void *, uint32_t, uint32_t))dlsym(tlc.tlcLib, "MtkH264SecVenc_tlcFillSecMem");
                bHasSecTlc = true;
            }
            else {
                MTK_OMX_LOGE("[ERROR] don't get secure lib");
                bHasSecTlc = false;
            }
        }
    }
    if (mInputMVAMgr != NULL)
    {
        mInputMVAMgr->setBoolProperty("SEC", mIsSecureInst);
    }
    return mIsSecureInst;
}
OMX_BOOL MtkOmxVenc::DeInitSecEncParams(void)
{
    DeInitSecCnvtBuffer();
    if (bHasSecTlc)
    {
        dlclose(tlc.tlcLib);
    }
    return OMX_TRUE;
}
int MtkOmxVenc::setEncodeMode(teeEnvType teeType)
{
    struct v4l2_control v4l2_ctrl;
    int fd = mV4L2fd;
    if(fd == -1) return -1;
    IN_FUNC();
    v4l2_ctrl.id = V4L2_CID_MPEG_MTK_SEC_ENCODE;
    v4l2_ctrl.value = teeType;
    IOCTL_OR_ERROR_RETURN_VALUE(mV4L2fd, VIDIOC_S_CTRL, &v4l2_ctrl, 0);
    OUT_FUNC();
    return 0;
}
