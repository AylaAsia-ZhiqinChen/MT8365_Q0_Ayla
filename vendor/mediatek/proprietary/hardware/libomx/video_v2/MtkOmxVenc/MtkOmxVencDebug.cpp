#include <sched.h>
#include <assert.h>
#include <signal.h>
#include <cutils/log.h>
#include <utils/Trace.h>
#include <cutils/trace.h>
#include <cutils/properties.h>
#include "properties.h"

#include "osal_utils.h"
#include "MtkOmxVenc.h"
#include "OMX_IndexExt.h"
#include "mmdump_wrp.h"

// MM PATH
/* always output systrace */
#define ATRACE_TAG ATRACE_TAG_ALWAYS
/* define output systrace macro */
#define MMPATH_TRACE(fmt, ...)  \
if (ATRACE_ENABLED()) {  \
    char ___buf[256];  \
    snprintf(___buf, sizeof(___buf), "MMPath:" fmt, ##__VA_ARGS__);  \
    android::ScopedTrace ___bufTracer(ATRACE_TAG, ___buf);  \
}

#undef LOG_TAG
#define LOG_TAG "MtkOmxVencDebug"

void MtkOmxVenc::MtkOmxVencDebug()
{
    mmdump = NULL;
    GET_SYSTEM_PROP(bool, bEnableMMDump, "vendor.debug.venc.enable_mm_buffer_dump", "0");
    GET_SYSTEM_PROP(bool, bEnableMMPath, "vendor.debug.venc.enable_mm_bwdata_path", "0");
}
void MtkOmxVenc::deMtkOmxVencDebug()
{

}

void MtkOmxVenc::ComponentInitDebug(AndroidVendorConfigExtensionStore* ext)
{
    if(!Debug_Enabled()) return;

    if(bEnableMMDump || bEnableMMPath)
        mmdump = new MMDump("Venc");
}
void MtkOmxVenc::ComponentDeInitDebug()
{
    if(!Debug_Enabled()) return;

    if(mmdump) delete mmdump;
}

// InitHW before stream on
void MtkOmxVenc::Debug_OnInit(video_encode_param* param)
{
    if(!Debug_Enabled()) return;
    pEncParam = param;
    // mmdump
    Debug_MMDump_Init(pEncParam);

    Debug_AppInsight_Init(pEncParam);
}
OMX_ERRORTYPE MtkOmxVenc::Debug_SetMode()
{
    return OMX_ErrorNone;
}
// DeInitHW after stream off
void MtkOmxVenc::Debug_OnDeInit()
{
    if(!Debug_Enabled()) return;
}

bool MtkOmxVenc::Debug_Enabled()
{
    return bEnableMMDump || bEnableMMPath || appinsight.enable();
}

void MtkOmxVenc::Debug_EmptyThisBuffer(OMX_BUFFERHEADERTYPE* pInputBuf)
{
    if(!Debug_Enabled()) return;
    if(pInputBuf == NULL) return;
}
void MtkOmxVenc::Debug_FillThisBuffer(OMX_BUFFERHEADERTYPE* pOutputBuf)
{
    if(!Debug_Enabled()) return;
    if(pOutputBuf == NULL) return;
}

// before generating v4l2_buffer from omx buffer (may idendical to Debug_UpdateInputV4L2Buffer)
void MtkOmxVenc::Debug_BufferModifyBeforeEnc(OMX_BUFFERHEADERTYPE** ppInputBuf)
{
    if(!Debug_Enabled()) return;
    if(ppInputBuf == NULL || *ppInputBuf == NULL) return;

    OMX_BUFFERHEADERTYPE* pInputBuf = *ppInputBuf;

    Debug_MMDump(pInputBuf);
    Debug_MMPath(pInputBuf);
    Debug_AppInsight(pInputBuf);
}

void MtkOmxVenc::Debug_CollectBsBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf)
{
    if(!Debug_Enabled()) return;
    if(ppOutputBuf == NULL || *ppOutputBuf == NULL) return;
}

// before enque: after v4l2_buffer is generated
void MtkOmxVenc::Debug_UpdateInputV4L2Buffer(OMX_BUFFERHEADERTYPE *pInputBuf, struct v4l2_buffer* qbuf)
{
    if(!Debug_Enabled()) return;
    if(pInputBuf == NULL || qbuf == NULL) return;
    if(pInputBuf->nFlags & OMX_BUFFERFLAG_EOS && pInputBuf->nFilledLen == 0) return;
}
void MtkOmxVenc::Debug_UpdateOutputV4L2Buffer(OMX_BUFFERHEADERTYPE *pOutputBuf, struct v4l2_buffer* qbuf)
{
    if(!Debug_Enabled()) return;
    if(pOutputBuf == NULL) return;
    if(qbuf == NULL) return;
}

// after perform qbuf
void MtkOmxVenc::Debug_OnEnqueBuffer(OMX_BUFFERHEADERTYPE** ppInputBuf)
{
    if(!Debug_Enabled()) return;
    if(ppInputBuf == NULL || *ppInputBuf == NULL) return;
}
void MtkOmxVenc::Debug_OnEnqueOutputBuffer(OMX_BUFFERHEADERTYPE** ppOutputBuf)
{
    if(!Debug_Enabled()) return;
    if(ppOutputBuf  == NULL || *ppOutputBuf == NULL) return;
}

// after acquire deque buffer, before calling Empty/FillBufferDone
void MtkOmxVenc::Debug_DequeOnGetInputBuffer(OMX_BUFFERHEADERTYPE* pInputBuf, struct v4l2_buffer* dqbuf)
{
    if(!Debug_Enabled()) return;
    if(pInputBuf == NULL) return;
    if(dqbuf == NULL) return;
}
void MtkOmxVenc::Debug_DequeOnGetOutputBuffer(OMX_BUFFERHEADERTYPE* pOutputBuf, struct v4l2_buffer* dqbuf)
{
    if(!Debug_Enabled()) return;
    if(pOutputBuf == NULL) return;
    if(dqbuf == NULL) return;
}

OMX_ERRORTYPE MtkOmxVenc::Debug_SetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig)
{
    OMX_ERRORTYPE err = OMX_ErrorUnsupportedIndex;

    switch (nConfigIndex)
    {
        case OMX_IndexConfigVideoFramerate:
        {
            if(appinsight.enable())
            {
                OMX_CONFIG_FRAMERATETYPE* pFrameRateType = (OMX_CONFIG_FRAMERATETYPE *)pCompConfig;
                appinsight.add(AppInsight::FRAMERATE, pFrameRateType->xEncodeFramerate>>16);
            }
            break;
        }
        case OMX_IndexConfigVideoBitrate:
        {
            if(appinsight.enable())
            {
                OMX_VIDEO_CONFIG_BITRATETYPE* pConfigBitrate = (OMX_VIDEO_CONFIG_BITRATETYPE *)pCompConfig;
                appinsight.add(AppInsight::BITRATE, pConfigBitrate->nEncodeBitrate);
            }
            break;
        }
        default:
            err = OMX_ErrorUnsupportedIndex;
    }
    return err;
}
OMX_ERRORTYPE MtkOmxVenc::Debug_GetConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig)
{
    return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE MtkOmxVenc::Debug_GetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam)
{
    return OMX_ErrorUnsupportedIndex;
}
OMX_ERRORTYPE MtkOmxVenc::Debug_SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam)
{
    OMX_ERRORTYPE err = OMX_ErrorUnsupportedIndex;

    switch (nParamIndex)
    {
        case OMX_IndexParamVideoAndroidImageGrid:
        {
            if(appinsight.enable())
            {
                OMX_VIDEO_PARAM_ANDROID_IMAGEGRIDTYPE *pImageGrid = (OMX_VIDEO_PARAM_ANDROID_IMAGEGRIDTYPE *)pCompParam;

                appinsight.add(AppInsight::GRID_WIDTH, pImageGrid->nGridRows);
                appinsight.add(AppInsight::GRID_HEIGHT, pImageGrid->nGridCols);
                appinsight.add(AppInsight::TILE_WIDTH, pImageGrid->nTileWidth);
                appinsight.add(AppInsight::TILE_HEIGHT, pImageGrid->nTileHeight);
            }
            break;
        }
        default:
            err = OMX_ErrorUnsupportedIndex;
    }

    return err;
}
OMX_ERRORTYPE MtkOmxVenc::Debug_GetExtensionIndex(OMX_STRING parameterName, OMX_INDEXTYPE *pIndexType)
{
    return OMX_ErrorUnsupportedIndex;
}

void MtkOmxVenc::Debug_MMDump_Init(video_encode_param* param)
{
    if(!bEnableMMDump && !bEnableMMPath) return;

    MTK_OMX_LOGD_ENG("MMDUMP Init");
    mmdump->set<MMDump::width, MMDump::height>(param->width, param->height);
    mmdump->set<MMDump::stride, MMDump::sliceheight>(param->buf_w, param->buf_h);
    mmdump->set<MMDump::color_format, MMDump::codec_type>(param->format, param->codec);
    mmdump->set<MMDump::framerate>(param->framerate);
    mmdump->set<MMDump::ufo_mode>(1); // always true by driver owner
}

void MtkOmxVenc::Debug_MMDump(OMX_BUFFERHEADERTYPE* pInputBuf)
{
    if(!bEnableMMDump) return;
    if(pInputBuf == NULL) return;

    OMX_U8 *aInputBuf   = pInputBuf->pBuffer + pInputBuf->nOffset;
    OMX_U32 aInputSize  = pInputBuf->nFilledLen;
    OMX_U8 *aCnvtBuf = (OMX_U8 *)pInputBuf->pPlatformPrivate;

    OMX_U32 handle = 0;
    GetMetaHandleFromBufferPtr(aInputBuf, &handle);

    if(handle == 0) {
        MTK_OMX_LOGD_ENG("[Debug_MMDump] GetMetaHandleFromBufferPtr %p handle %d", aInputBuf, handle);
        return;
    }

    VENC_DRV_PARAM_FRM_BUF_T aFrameBuf;
    if(OMX_FALSE == GetVEncDrvFrmBuffer(aInputBuf, aInputSize, aCnvtBuf, 0, &aFrameBuf))
    {
        MTK_OMX_LOGD_ENG("[Debug_MMDump] GetVEncDrvFrmBuffer %p,%p failed", aInputBuf, aCnvtBuf);
        return;
    }

    unsigned int size = 0;
    unsigned long alloc_id = 0;
    gralloc_extra_query((buffer_handle_t)handle, GRALLOC_EXTRA_GET_ID, &alloc_id);
    gralloc_extra_query((buffer_handle_t)handle, GRALLOC_EXTRA_GET_ALLOC_SIZE, &size);

    mmdump->set<MMDump::size>(size);
    mmdump->dump((void*)aFrameBuf.rFrmBufAddr.u4VA, alloc_id);

    MTK_OMX_LOGD_ENG("MMDUMP 0x%lx, %d", aFrameBuf.rFrmBufAddr.u4VA, alloc_id);
}

void MtkOmxVenc::Debug_MMPath(OMX_BUFFERHEADERTYPE* pInputBuf)
{
    if(!bEnableMMPath) return;
    if(pInputBuf == NULL) return;

    OMX_U8 *aInputBuf   = pInputBuf->pBuffer + pInputBuf->nOffset;
    OMX_U32 aInputSize  = pInputBuf->nFilledLen;
    OMX_U8 *aCnvtBuf = (OMX_U8 *)pInputBuf->pPlatformPrivate;

    VENC_DRV_PARAM_FRM_BUF_T aFrameBuf;
    if(OMX_FALSE == GetVEncDrvFrmBuffer(aInputBuf, aInputSize, aCnvtBuf, 0, &aFrameBuf))
    {
        MTK_OMX_LOGD_ENG("[Debug_MMPath] GetVEncDrvFrmBuffer %p,%p failed", aInputBuf, aCnvtBuf);
        return;
    }

    typedef struct {const char* k; unsigned int v; bool useHex;} kv;
    kv list[] = {
        {"mva", aFrameBuf.rFrmBufAddr.u4VA, true},
        {"codec", mmdump->get<MMDump::codec_type>(), true},
        {"fmt", mmdump->get<MMDump::color_format>(), true},
        {mmdump->getName<MMDump::width>(), mmdump->get<MMDump::width>(), false},
        {mmdump->getName<MMDump::height>(), mmdump->get<MMDump::height>(), false},
        {mmdump->getName<MMDump::framerate>(), mmdump->get<MMDump::framerate>(), false},
        {mmdump->getName<MMDump::ufo_mode>(), mmdump->get<MMDump::ufo_mode>(), false}
    };

    int offset = 0;
    char buffer[512];
    const int size = sizeof(buffer);

    offset += snprintf(buffer+offset, size-offset, ",HW=VENC");
    for(int i=0; i<sizeof(list)/sizeof(kv); i++) {
        if(list[i].useHex) offset += snprintf(buffer+offset, size-offset, ",%s=0x%x", list[i].k, list[i].v);
        else offset += snprintf(buffer+offset, size-offset, ",%s=%u", list[i].k, list[i].v);
    }

    MTK_OMX_LOGD_ENG("MMPATH: %s", buffer+1);

    MMPATH_TRACE("%s", buffer+1);
}

void MtkOmxVenc::Debug_AppInsight_Init(video_encode_param* param)
{
    if(!appinsight.enable()) return;

    appinsight.add(AppInsight::WIDTH, param->width);
    appinsight.add(AppInsight::HEIGHT, param->height);
    appinsight.add(AppInsight::CODECTYPE, param->codec); //fourcc
    appinsight.add(AppInsight::PIXEL_FORMAT, param->format); // fourcc
    appinsight.add(AppInsight::BITRATE, param->bitrate);
    appinsight.add(AppInsight::FRAMERATE, param->framerate);
    appinsight.add(AppInsight::PROFILE, param->profile);
    appinsight.add(AppInsight::LEVEL, param->level);
    appinsight.add(AppInsight::REGION_DECODE, -1); // hint as encoder

    appinsight.commit();
}

void MtkOmxVenc::Debug_AppInsight(OMX_BUFFERHEADERTYPE* pInputBuf)
{
    if(!appinsight.enable()) return;

    appinsight.commit();
}