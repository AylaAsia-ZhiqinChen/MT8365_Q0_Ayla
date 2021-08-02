#include <sched.h>
#include <assert.h>
#include <signal.h>
#include <cutils/log.h>
#include <utils/Trace.h>
#include <cutils/properties.h>
#include "properties.h"

#include "osal_utils.h"
#include "MtkOmxVenc.h"
#include "OMX_IndexExt.h"

#undef LOG_TAG
#define LOG_TAG "MtkOmxVencHDR"

typedef enum
{
    HEVC_ColorPrimaries_Reserved_1  = 0,
    HEVC_ColorPrimaries_BT709_5     = 1,
    HEVC_ColorPrimaries_Unspecified = 2,
    HEVC_ColorPrimaries_Reserved_2  = 3,
    HEVC_ColorPrimaries_BT470_6M    = 4,
    HEVC_ColorPrimaries_BT601_6_625 = 5,
    HEVC_ColorPrimaries_BT601_6_525 = 6,
    HEVC_ColorPrimaries_SMPTE_240M  = 7,
    HEVC_ColorPrimaries_GenericFilm = 8,
    HEVC_ColorPrimaries_BT2020      = 9,
    HEVC_ColorPrimaries_SMPTE_ST428 = 10
} HEVCColorPrimaries;

typedef enum
{
    HEVC_Transfer_Reserved_1    = 0,
    HEVC_Transfer_BT709_5       = 1,
    HEVC_Transfer_Unspecified   = 2,
    HEVC_Transfer_Reserved_2    = 3,
    HEVC_Transfer_Gamma22       = 4,
    HEVC_Transfer_Gamma28       = 5,
    HEVC_Transfer_BT601_6_625   = 6,
    HEVC_Transfer_SMPTE_240M    = 7,
    HEVC_Transfer_Linear        = 8,
    HEVC_Transfer_Log_1         = 9,
    HEVC_Transfer_Log_2         = 10,
    HEVC_Transfer_IEC_61966_2_4 = 11,
    HEVC_Transfer_BT1361        = 12,
    HEVC_Transfer_IEC_61966_2_1 = 13,
    HEVC_Transfer_BT2020_1      = 14,
    HEVC_Transfer_BT2020_2      = 15,
    HEVC_Transfer_SMPTE_ST_2048 = 16,
    HEVC_Transfer_SMPTE_ST_428  = 17,
    HEVC_Transfer_HLG           = 18
} HEVCTransfer;

typedef enum
{
    HEVC_MatrixCoeff_Identity    = 0,
    HEVC_MatrixCoeff_BT709_5     = 1,
    HEVC_MatrixCoeff_Unspecified = 2,
    HEVC_MatrixCoeff_Reserved    = 3,
    HEVC_MatrixCoeff_BT470_6M    = 4,
    HEVC_MatrixCoeff_BT601_6     = 5,
    HEVC_MatrixCoeff_SMPTE_170M  = 6,
    HEVC_MatrixCoeff_SMPTE_240M  = 7,
    HEVC_MatrixCoeff_YCgCo       = 8,
    HEVC_MatrixCoeff_BT2020      = 9,
    HEVC_MatrixCoeff_BT2020Cons  = 10
} HEVCMatrixCoeff;

static HEVCColorPrimaries HDRColorPrimariesMap[] =
{
    HEVC_ColorPrimaries_Unspecified,  // PrimariesUnspecified
    HEVC_ColorPrimaries_BT709_5,      // PrimariesBT709_5
    HEVC_ColorPrimaries_Reserved_1,   // PrimariesBT470_6M
    HEVC_ColorPrimaries_Reserved_1,   // PrimariesBT601_6_625
    HEVC_ColorPrimaries_Reserved_1,   // PrimariesBT601_6_525
    HEVC_ColorPrimaries_Reserved_1,   // PrimariesGenericFilm
    HEVC_ColorPrimaries_BT2020        // PrimariesBT2020
};

static HEVCTransfer HDRTransferMap[] =
{
    HEVC_Transfer_Unspecified,     // TransferUnspecified
    HEVC_Transfer_Reserved_1,      // TransferLinear
    HEVC_Transfer_Reserved_1,      // TransferSRGB
    HEVC_Transfer_BT709_5,         // TransferSMPTE170M
    HEVC_Transfer_Reserved_1,      // TransferGamma22
    HEVC_Transfer_Reserved_1,      // TransferGamma28
    HEVC_Transfer_SMPTE_ST_2048,   // TransferST2084
    HEVC_Transfer_HLG              // TransferHLG
};

static HEVCMatrixCoeff HDRMatrixCoeffMap[] =
{
    HEVC_MatrixCoeff_Unspecified,   // MatrixUnspecified
    HEVC_MatrixCoeff_BT709_5,       // MatrixBT709_5
    HEVC_MatrixCoeff_Reserved,      // MatrixBT470_6M
    HEVC_MatrixCoeff_Reserved,      // MatrixBT601_6
    HEVC_MatrixCoeff_Reserved,      // MatrixSMPTE240M
    HEVC_MatrixCoeff_BT2020,        // MatrixBT2020
    HEVC_MatrixCoeff_BT2020Cons     // MatrixBT2020Constant
};

#define HDR_Color_Primaries_Map_SIZE    (sizeof(HDRColorPrimariesMap)/sizeof(HEVCColorPrimaries))
#define HDR_Transfer_Map_SIZE    (sizeof(HDRTransferMap)/sizeof(HEVCTransfer))
#define HDR_Matrix_Coeff_Map_SIZE    (sizeof(HDRMatrixCoeffMap)/sizeof(HEVCMatrixCoeff))

static void HdrMoveInfoGrallocExtraToDriver(ge_hdr10p_dynamic_metadata_t*, VENC_DRV_HDR_DYNAMIC_INFO_T*);
static void HdrGenerateDummyInfoToDriver(VENC_DRV_HDR_DYNAMIC_INFO_T*);

void MtkOmxVenc::MtkOmxVencHDR()
{
    MTK_OMX_MEMSET(&mDescribeStaticInfo, 0, sizeof(DescribeHDRStaticInfoParams));
    MTK_OMX_MEMSET(&mDescribeColorAspects, 0, sizeof(DescribeColorAspectsParams));

    pEnabledHdr10Toggle = NULL;
    mEnabledStatic = 0;
    mEnabled10Plus = 0;
    mDynamicInfoBufferSize = sizeof(VENC_DRV_HDR_DYNAMIC_INFO_T);

    GET_SYSTEM_PROP(int, mEnabled10Plus, "vendor.mtk.venc.hdr10plus.enable", "0");
    GET_SYSTEM_PROP(int, mEnabled10PlusDummydata, "vendor.mtk.venc.hdr10plus.dummy", "0");
    if(mEnabled10PlusDummydata) {
        MTK_OMX_LOGD_ENG("[HDR10P] Dummy data enabled");
        mEnabled10Plus = 1;
    }

    mDynamicInfoBufferMVAMgr = new OmxMVAManager("ion", "MtkOmxVencHDR");

    INIT_MUTEX(mDynamicInfoBufferTableLock);
    INIT_MUTEX(mDynamicInfoFreeBufferListLock);
}
void MtkOmxVenc::deMtkOmxVencHDR()
{
    if(pEnabledHdr10Toggle != NULL)
    {
        free((void*)pEnabledHdr10Toggle);
        pEnabledHdr10Toggle = NULL;
    }

    if(mDynamicInfoBufferMVAMgr != NULL) {
        delete mDynamicInfoBufferMVAMgr;
        mDynamicInfoBufferMVAMgr = NULL;
    }

    DESTROY_MUTEX(mDynamicInfoBufferTableLock);
    DESTROY_MUTEX(mDynamicInfoFreeBufferListLock);
}
void MtkOmxVenc::ComponentInitHDR(AndroidVendorConfigExtensionStore* ext)
{
    InitOMXParams(&mDescribeStaticInfo);
    InitOMXParams(&mDescribeColorAspects);

    if(pEnabledHdr10Toggle) free((void*)pEnabledHdr10Toggle);

    pEnabledHdr10Toggle =
        AndroidVendorConfigExtensionFactory::createAndroidVendorExtension
        ("hdr10plus", {{"enable", OMX_AndroidVendorValueInt32}});

    if (pEnabledHdr10Toggle == NULL)
    {
        MTK_OMX_LOGE("[%s] createAndroidVendorExtension fail!", __func__);
        return;
    }

    ext->registerExtension(pEnabledHdr10Toggle);

    MTK_OMX_LOGD_ENG("[ComponentInitHDR] registered vendor index: %d", pEnabledHdr10Toggle->nIndex);
}
void MtkOmxVenc::ComponentDeInitHDR()
{
}

// to-do
// before stream on: InitHW
void MtkOmxVenc::HDR_OnInit(video_encode_param* param)
{
}

// after stream off: DeInitHW
void MtkOmxVenc::HDR_OnDeInit()
{
}

// before stream on: after InitHW, in case you need to set runtime config
OMX_ERRORTYPE MtkOmxVenc::HDR_SetMode()
{
    if(!HDR_Enabled()) return OMX_ErrorNone;
    // 7: HDR, 8: HDR10+
    MTK_OMX_LOGD_ENG("[SetMode] HDR10: %d, HDR10P %d", mEnabledStatic, mEnabled10Plus);

    int nRet;
    if(mEnabledStatic != 0)
    {
        nRet = ioctl_runtime_config(V4L2_CID_MPEG_MTK_ENCODE_SCENARIO, 7);
        if (-1 == nRet)
        {
            MTK_OMX_LOGE("[ERROR] set HDR scenario failed");
        }
    }

    if(mEnabled10Plus != 0)
    {
        nRet = ioctl_runtime_config(V4L2_CID_MPEG_MTK_ENCODE_SCENARIO, 8);
        if (-1 == nRet)
        {
            MTK_OMX_LOGE("[ERROR] set HDR10+ scenario fail");
        }
    }

    HDR_SetStaticInfo();

    return OMX_ErrorNone;
}

void MtkOmxVenc::HDR_HandleStateSet(OMX_U32 nNewState)
{

}

void MtkOmxVenc::HDR_NotifyStopCommand()
{
    IN_FUNC();

    OUT_FUNC();
}


void MtkOmxVenc::HDR_SetStaticInfo()
{
    if(!HDR_Enabled()) return;

    int nRet;
    VENC_DRV_HDR_STATIC_INFO_T staticInfo;
    memset(&staticInfo, 0, sizeof(VENC_DRV_HDR_STATIC_INFO_T));

    auto colorAspects = mDescribeColorAspects.sAspects;
    if(colorAspects.mPrimaries < HDR_Color_Primaries_Map_SIZE)
        staticInfo.u4ColorPrimaries = HDRColorPrimariesMap[colorAspects.mPrimaries];

    if(colorAspects.mTransfer < HDR_Transfer_Map_SIZE)
        staticInfo.u4TransformCharacter = HDRTransferMap[colorAspects.mTransfer];

    if(colorAspects.mMatrixCoeffs < HDR_Matrix_Coeff_Map_SIZE)
        staticInfo.u4MatrixCoeffs = HDRMatrixCoeffMap[colorAspects.mMatrixCoeffs];

    auto staticInfoType1 = mDescribeStaticInfo.sInfo.sType1;
    staticInfo.u4DisplayPrimariesX[0] = staticInfoType1.mR.x;
    staticInfo.u4DisplayPrimariesY[0] = staticInfoType1.mR.y;
    staticInfo.u4DisplayPrimariesX[1] = staticInfoType1.mG.x;
    staticInfo.u4DisplayPrimariesY[1] = staticInfoType1.mG.y;
    staticInfo.u4DisplayPrimariesX[2] = staticInfoType1.mB.x;
    staticInfo.u4DisplayPrimariesY[2] = staticInfoType1.mB.y;
    staticInfo.u4WhitePointX = staticInfoType1.mW.x;
    staticInfo.u4WhitePointY = staticInfoType1.mW.y;
    staticInfo.u4MaxDisplayMasteringLuminance = staticInfoType1.mMaxDisplayLuminance;
    staticInfo.u4MinDisplayMasteringLuminance = staticInfoType1.mMinDisplayLuminance;
    staticInfo.u4MaxCLL = staticInfoType1.mMaxContentLightLevel;
    staticInfo.u4MaxFALL = staticInfoType1.mMaxFrameAverageLightLevel;

    int addr = (int)((void*)&staticInfo);
    int size = sizeof(VENC_DRV_HDR_STATIC_INFO_T);
    nRet = ioctl_runtime_config(V4L2_CID_MPEG_MTK_COLOR_DESC, addr, size);
    if(nRet == -1)
    {
        MTK_OMX_LOGE("[ERROR] set HDR static info failed, &Info(%p) addr(%x) size(%d)", &staticInfo, addr, size);
        mEnabledStatic = 0;
    }
}

bool MtkOmxVenc::HDR_Enabled()
{
    return (mEnabledStatic != 0 || mEnabled10Plus != 0);
}

// At the end of HandleEmpty/FillThisBuffer
void MtkOmxVenc::HDR_EmptyThisBuffer(OMX_BUFFERHEADERTYPE* pInputBuf)
{
    if(!HDR_Enabled()) return;
    if(pInputBuf == NULL) return;
}

void MtkOmxVenc::HDR_FillThisBuffer(OMX_BUFFERHEADERTYPE* pOutputBuf)
{
    if(!HDR_Enabled()) return;
    if(pOutputBuf == NULL) return;
}

// before generating v4l2_buffer from omx buffer (may idendical to HDR_UpdateInputV4L2Buffer)
void MtkOmxVenc::HDR_BufferModifyBeforeEnc(OMX_BUFFERHEADERTYPE** ppInputBuf)
{
    if(!HDR_Enabled()) return;
    if(ppInputBuf == NULL || *ppInputBuf == NULL) return;
}

// before qbuf, set NULL to skip enque buffer to kernel
void MtkOmxVenc::HDR_CollectBsBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf)
{
    if(!HDR_Enabled()) return;
    if(ppOutputBuf == NULL || *ppOutputBuf == NULL) return;
}

// before qbuf: after v4l2_buffer is generated
void MtkOmxVenc::HDR_UpdateInputV4L2Buffer(OMX_BUFFERHEADERTYPE *pInputBuf, struct v4l2_buffer* qbuf)
{
    if(!HDR_Enabled()) return;
    if(mEnabled10Plus == 0) return;
    if(pInputBuf == NULL || qbuf == NULL) return;
    if(pInputBuf->nFlags & OMX_BUFFERFLAG_EOS && pInputBuf->nFilledLen == 0) return;

    OMX_U8* pInfoBuf = NULL;
    unsigned int size = 0;
    OMX_U8 *aInputBuf = pInputBuf->pBuffer + pInputBuf->nOffset;

    // get available buffer for dynamic info
    HDR_GetFreeInfoBuffer(&pInfoBuf, &size);
    assert(pInfoBuf != NULL && size > 0);

    if(mEnabled10PlusDummydata)
    {
        // dummy
        HdrGenerateDummyInfoToDriver((VENC_DRV_HDR_DYNAMIC_INFO_T*)pInfoBuf);
    }
    else
    {
        OMX_U32 handle = 0;
        if(OMX_FALSE == GetMetaHandleFromBufferPtr(aInputBuf, &handle))
        {
            MTK_OMX_LOGE("[HDR] Get handle failed");
            //HDR_SetFreeInfoBuffer(pInfoBuf);
            //return;
        }

        ge_hdr10p_dynamic_metadata_t mGeInfo;
        if(0 != gralloc_extra_query((buffer_handle_t)handle, GRALLOC_EXTRA_GET_HDR10P_INFO, &mGeInfo))
        {
            // gralloc extra query error
            MTK_OMX_LOGE("[HDR] ge query failed");
            //HDR_SetFreeInfoBuffer(pInfoBuf);
            //return;
            memset(&mGeInfo, 0, sizeof(ge_hdr10p_dynamic_metadata_t));
        }
        HdrMoveInfoGrallocExtraToDriver(&mGeInfo, (VENC_DRV_HDR_DYNAMIC_INFO_T*)pInfoBuf);
    }

    VBufInfo bufInfo;
    mDynamicInfoBufferMVAMgr->getOmxInfoFromVA(pInfoBuf, &bufInfo);

    qbuf->reserved2 = bufInfo.iIonFd;
    qbuf->flags |= V4L2_BUF_FLAG_HDR_META;

    // Map v4l2 buffer index <==> Buffer info
    LOCK(mDynamicInfoBufferTableLock);
    mDynamicInfoBufferTable.add(qbuf->index, pInfoBuf);
    UNLOCK(mDynamicInfoBufferTableLock);

    MTK_OMX_LOGD_ENG("V4L2_index %d, Info Buffer fd %d", qbuf->index, qbuf->reserved2);
}

void MtkOmxVenc::HDR_UpdateOutputV4L2Buffer(OMX_BUFFERHEADERTYPE *pOutputBuf, struct v4l2_buffer* qbuf)
{
    if(!HDR_Enabled()) return;
    if(pOutputBuf == NULL) return;
    if(qbuf == NULL) return;
}

// to-do:
// after qbuf, before notify Deque thread (set *ppInputBuf to NULL to avoid EBD after dqbuf)
void MtkOmxVenc::HDR_OnEnqueBuffer(OMX_BUFFERHEADERTYPE** ppInputBuf)
{
    if(!HDR_Enabled()) return;
    if(ppInputBuf == NULL || *ppInputBuf == NULL) return;
}

// after qbuf, before continue to enque any others bs buffer
void MtkOmxVenc::HDR_OnEnqueOutputBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf)
{
    if(!HDR_Enabled()) return;
    if(ppOutputBuf  == NULL || *ppOutputBuf == NULL) return;
}

// after acquire deque buffer, before calling Empty/FillBufferDone
void MtkOmxVenc::HDR_DequeOnGetInputBuffer(OMX_BUFFERHEADERTYPE* pInputBuf, struct v4l2_buffer* dqbuf)
{
    if(!HDR_Enabled()) return;
    if(mEnabled10Plus == 0) return;
    if(pInputBuf == NULL) return;
    if(dqbuf == NULL) return;

    OMX_U8* pInfoBuf = NULL;

    LOCK(mDynamicInfoBufferTableLock);
    pInfoBuf = mDynamicInfoBufferTable.valueFor(dqbuf->index);

    int tableIndex = mDynamicInfoBufferTable.indexOfKey(dqbuf->index);
    if (tableIndex < 0)
    {
        MTK_OMX_LOGE("[%s] tableIndex is %d\n", __func__, tableIndex);
        return;
    }
    mDynamicInfoBufferTable.removeItemsAt(tableIndex);
    UNLOCK(mDynamicInfoBufferTableLock);

    HDR_SetFreeInfoBuffer(pInfoBuf);
}
void MtkOmxVenc::HDR_DequeOnGetOutputBuffer(OMX_BUFFERHEADERTYPE* pOutputBuf, struct v4l2_buffer* dqbuf)
{
    if(!HDR_Enabled()) return;
    if(pOutputBuf == NULL) return;
    if(dqbuf == NULL) return;
}

OMX_ERRORTYPE MtkOmxVenc::HDR_SetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig)
{
    OMX_ERRORTYPE err = OMX_ErrorNone;

    switch (nConfigIndex)
    {
        case OMX_GoogleAndroidIndexDescribeHDRStaticInfo:
            err = HandleSetConfigDescribeHDRStaticInfo((DescribeHDRStaticInfoParams*)pCompConfig);
            break;
        case OMX_GoogleAndroidIndexDescribeColorAspects:
            err = HandleSetConfigDescribeColorAspects((DescribeColorAspectsParams*)pCompConfig);
            break;
        case OMX_GoogleAndroidIndexDescribeHDR10PlusInfo:
            err = HandleSetConfigDescribeHDR10PlusInfo((DescribeHDR10PlusInfoParams*)pCompConfig);
            break;
        case OMX_IndexConfigAndroidVendorExtension:
            err = HandleSetConfigHDRAndroidVendorExtension((OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE*)pCompConfig);
            break;
        default:
            err = OMX_ErrorUnsupportedIndex;
    }
    return err;
}

OMX_ERRORTYPE MtkOmxVenc::HDR_GetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig)
{
    OMX_ERRORTYPE err = OMX_ErrorNone;

    switch (nConfigIndex)
    {
        case OMX_GoogleAndroidIndexDescribeHDRStaticInfo:
            err = HandleGetConfigDescribeHDRStaticInfo((DescribeHDRStaticInfoParams*)pCompConfig);
            break;
        case OMX_GoogleAndroidIndexDescribeColorAspects:
            err = HandleGetConfigDescribeColorAspects((DescribeColorAspectsParams*)pCompConfig);
            break;
        case OMX_GoogleAndroidIndexDescribeHDR10PlusInfo:
            err = HandleGetConfigDescribeHDR10PlusInfo((DescribeHDR10PlusInfoParams*)pCompConfig);
            break;
        default:
            err = OMX_ErrorUnsupportedIndex;
    }
    return err;
}

OMX_ERRORTYPE MtkOmxVenc::HDR_GetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam)
{
    OMX_ERRORTYPE err = OMX_ErrorNone;

    switch (nParamIndex)
    {
        default:
            err = OMX_ErrorUnsupportedIndex;
    }
    return err;
}

OMX_ERRORTYPE MtkOmxVenc::HDR_SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam)
{
    OMX_ERRORTYPE err = OMX_ErrorNone;

    switch (nParamIndex)
    {
        default:
            err = OMX_ErrorUnsupportedIndex;
    }
    return err;
}

struct ExtensionIndex {
    const OMX_STRING name;
    const OMX_INDEXTYPE index;
};

static struct ExtensionIndex hdrExt[] = {
    {"OMX.google.android.index.describeHDRStaticInfo", OMX_GoogleAndroidIndexDescribeHDRStaticInfo},
    {"OMX.google.android.index.describeColorAspects", OMX_GoogleAndroidIndexDescribeColorAspects},
    {"OMX.google.android.index.describeHDR10PlusInfo", OMX_GoogleAndroidIndexDescribeHDR10PlusInfo}
};

OMX_ERRORTYPE MtkOmxVenc::HDR_GetExtensionIndex(OMX_STRING parameterName, OMX_INDEXTYPE *pIndexType)
{
    int extCount = sizeof(hdrExt)/sizeof(struct ExtensionIndex);
    for(int i=0; i<extCount; i++)
    {
        if(!strncmp(parameterName,hdrExt[i].name,strlen(hdrExt[i].name)))
        {
            *pIndexType = (OMX_INDEXTYPE) hdrExt[i].index;
            return OMX_ErrorNone;
        }
    }
    return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE MtkOmxVenc::HandleSetConfigDescribeHDRStaticInfo(DescribeHDRStaticInfoParams* pConfig)
{
    memcpy(&mDescribeStaticInfo, pConfig, sizeof(mDescribeStaticInfo));

    auto refType1 = mDescribeStaticInfo.sInfo.sType1;

    // any value not 0 will trigger hdr10
    if( refType1.mR.x || refType1.mR.y || refType1.mG.x || refType1.mG.y ||
        refType1.mB.x || refType1.mB.y || refType1.mW.x || refType1.mW.y ||
        refType1.mMinDisplayLuminance || refType1.mMaxDisplayLuminance ||
        refType1.mMaxContentLightLevel || refType1.mMaxFrameAverageLightLevel)
        mEnabledStatic = 1;

    MTK_OMX_LOGD_ENG("SetConfig HDRStaticInfo: R %u %u,G %u %u,B %u %u,W %u %u"
        "DispL %u %u ContentLL %u, avgL %u",
        refType1.mR.x, refType1.mR.y, refType1.mG.x, refType1.mG.y,
        refType1.mB.x, refType1.mB.y, refType1.mW.x, refType1.mW.y,
        refType1.mMinDisplayLuminance, refType1.mMaxDisplayLuminance,
        refType1.mMaxContentLightLevel, refType1.mMaxFrameAverageLightLevel);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::HandleGetConfigDescribeHDRStaticInfo(DescribeHDRStaticInfoParams* pConfig)
{
    memcpy(pConfig, &mDescribeStaticInfo, sizeof(mDescribeStaticInfo));

    MTK_OMX_LOGD_ENG("GetConfig HDR Color Aspects: request %d dataspace %d change %d, "
        "Range: %d, Primaries: %d, Transfer: %d, MatCoef: %d",
        mDescribeColorAspects.bRequestingDataSpace, mDescribeColorAspects.nDataSpace, mDescribeColorAspects.bDataSpaceChanged,
        mDescribeColorAspects.sAspects.mRange, mDescribeColorAspects.sAspects.mPrimaries,
        mDescribeColorAspects.sAspects.mTransfer, mDescribeColorAspects.sAspects.mMatrixCoeffs);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::HandleSetConfigDescribeColorAspects(DescribeColorAspectsParams* pConfig)
{
    memcpy(&mDescribeColorAspects, pConfig, sizeof(mDescribeColorAspects));

    auto refAspect = mDescribeColorAspects.sAspects;

    // any value not 0 will trigger hdr10
    //if( refAspect.mRange || refAspect.mPrimaries ||
    //    refAspect.mTransfer || refAspect.mMatrixCoeffs)
    //    mEnabledStatic = 1;

    MTK_OMX_LOGD_ENG("SetConfig HDR Color Aspects: request %d dataspace %d change %d, "
        "Range: %d, Primaries: %d, Transfer: %d, MatCoef: %d",
        mDescribeColorAspects.bRequestingDataSpace, mDescribeColorAspects.nDataSpace, mDescribeColorAspects.bDataSpaceChanged,
        refAspect.mRange, refAspect.mPrimaries, refAspect.mTransfer, refAspect.mMatrixCoeffs);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::HandleGetConfigDescribeColorAspects(DescribeColorAspectsParams* pConfig)
{
    memcpy(pConfig, &mDescribeColorAspects, sizeof(mDescribeColorAspects));

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::HandleSetConfigHDRAndroidVendorExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* pConfig)
{
    // pEnabledHdr10Toggle
    if(pEnabledHdr10Toggle->nIndex == pConfig->nIndex)
    {
        memcpy(pEnabledHdr10Toggle, pConfig, pEnabledHdr10Toggle->nSize);

        mEnabled10Plus = pEnabledHdr10Toggle->param[0].nInt32;

        MTK_OMX_LOGD_ENG("SetConfig HDR10Plus index: %d, %s.%s = %d, Enabled10Plus %d",
            pConfig->nIndex, pConfig->cName, pConfig->param[0].cKey, pConfig->param[0].nInt32, mEnabled10Plus);

        return OMX_ErrorNone;
    }

    return OMX_ErrorUnsupportedIndex;
}
OMX_ERRORTYPE MtkOmxVenc::HandleGetConfigHDRAndroidVendorExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE* pConfig)
{
    // will handle by common SetConfig
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::HandleSetConfigDescribeHDR10PlusInfo(DescribeHDR10PlusInfoParams* pConfig)
{
    // get hdr10plus on/off value
    int dataSize = (pConfig->nParamSizeUsed > sizeof(mEnabled10Plus))? sizeof(mEnabled10Plus) : pConfig->nParamSizeUsed;

    memcpy(&mEnabled10Plus, &pConfig->nValue, dataSize);

    MTK_OMX_LOGD_ENG("SetConfig HDR10Plus Enable: %d", mEnabled10Plus);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MtkOmxVenc::HandleGetConfigDescribeHDR10PlusInfo(DescribeHDR10PlusInfoParams* pConfig)
{
    // handle nothing
    return OMX_ErrorNone;
}

void MtkOmxVenc::HDR_GetFreeInfoBuffer(OMX_U8** ppInfoBuffer, unsigned* pBufferSize)
{
    OMX_U8* freeBufferPtr = 0;

    LOCK(mDynamicInfoFreeBufferListLock);
    if(mDynamicInfoFreeBufferList.size() > 0)
    {
        freeBufferPtr = mDynamicInfoFreeBufferList[0];
        mDynamicInfoFreeBufferList.removeAt(0);
    }
    UNLOCK(mDynamicInfoFreeBufferListLock);

    if(freeBufferPtr != 0)
    {
        *ppInfoBuffer = freeBufferPtr;
        *pBufferSize = mDynamicInfoBufferSize;

        MTK_OMX_LOGV_ENG("Acquire free dynamic info buf %p [%d]", *ppInfoBuffer, *pBufferSize);
    }
    else
    {
        mDynamicInfoBufferMVAMgr->newOmxMVAandVA(128, mDynamicInfoBufferSize, NULL, (void**)ppInfoBuffer);
        *pBufferSize = mDynamicInfoBufferSize;

        MTK_OMX_LOGV_ENG("Allocate new dynamic info buf %p [%d]", *ppInfoBuffer, *pBufferSize);
    }
}

void MtkOmxVenc::HDR_SetFreeInfoBuffer(OMX_U8* pInfoBuffer)
{
    if(pInfoBuffer == NULL) return;

    MTK_OMX_LOGV_ENG("HDR_SetFreeInfoBuffer: %p", pInfoBuffer);

    LOCK(mDynamicInfoFreeBufferListLock);
    mDynamicInfoFreeBufferList.add(pInfoBuffer);
    UNLOCK(mDynamicInfoFreeBufferListLock);
}

static
void HdrMoveInfoGrallocExtraToDriver(ge_hdr10p_dynamic_metadata_t* pGeInfo, VENC_DRV_HDR_DYNAMIC_INFO_T* pDriverInfo)
{
    pDriverInfo->u4MaxSclR = pGeInfo->maxscl[0];
    pDriverInfo->u4MaxSclG = pGeInfo->maxscl[1];
    pDriverInfo->u4MaxSclB = pGeInfo->maxscl[2];
    pDriverInfo->u4AverageMaxrgb = pGeInfo->average_maxrgb;

    // pDriverInfo->distribution_values <= pGeInfo->distribution_maxrgb_percentages
    for(int i=0; i<8; i++)
    {
        pDriverInfo->u4DistributionValues[i] = pGeInfo->distribution_maxrgb_percentiles[i];
    }

    if (pGeInfo->targeted_system_display_maximum_luminance == 0 &&
        pGeInfo->tone_mapping_flag == 0) // profile A
    {
        // ignore 98% element
        pDriverInfo->u4DistributionValues[8] = pGeInfo->distribution_maxrgb_percentiles[9];
    }
    else
    {
        // NOT support profile B
    }
}

static
void HdrGenerateDummyInfoToDriver(VENC_DRV_HDR_DYNAMIC_INFO_T* pDriverInfo)
{
    pDriverInfo->u4MaxSclR = 64;
    pDriverInfo->u4MaxSclG = 64;
    pDriverInfo->u4MaxSclB = 64;
    pDriverInfo->u4AverageMaxrgb = 64;

    for(int i=0; i<9; i++)
    {
        pDriverInfo->u4DistributionValues[i] = 64;
    }
}
