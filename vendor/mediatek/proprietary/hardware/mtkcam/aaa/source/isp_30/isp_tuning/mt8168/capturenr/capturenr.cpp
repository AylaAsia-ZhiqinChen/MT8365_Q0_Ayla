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
#define LOG_TAG "capturenr"
//
/*******************************************************************************
*
********************************************************************************/
#include <mtkcam/utils/std/Log.h>
#define SWNR_LOGV(fmt, arg...)    CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define SWNR_LOGD(fmt, arg...)    CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define SWNR_LOGI(fmt, arg...)    CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define SWNR_LOGW(fmt, arg...)    CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define SWNR_LOGE(fmt, arg...)    CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
//
#include <mtkcam/def/common.h>
//
#include <utils/StrongPointer.h>
using namespace android;
//
//
#include "capturenr.h"
#include <sys/mman.h>
#include <camera_custom_capture_nr.h>

// custom tuning
#include <mtkcam/drv/IHalSensor.h>
#include <camera_custom_nvram.h>
#include <nvbuf_util.h>
//
#include <debug_exif/dbg_id_param.h>
#include <debug_exif/cam/dbg_cam_param.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>
#include <mtkcam/utils/metastore/IMetadataProvider.h>
//
#include <aaa_types.h>
#include <isp_tuning.h>
#include <isp_tuning_cam_info.h>
#include <isp_tuning_idx.h>
#include <isp_tuning_custom.h>
#include <isp_tuning/isp_tuning_custom_swnr.h>
#include <isp_interpolation.h>
#include <cutils/properties.h>
#include <vendor/mediatek/hardware/power/2.0/IPower.h>
using namespace vendor::mediatek::hardware::power::V2_0;

#include "paramctrl.h"
#include "ui/GraphicBuffer.h"
#include "mtkcam/utils/imgbuf/IGrallocImageBufferHeap.h"

#define SWNR_ASSERT(x)                                                      \
do {                                                                        \
    MBOOL __x = (x);                                                        \
    if (!__x) {                                                             \
        SWNR_LOGE("[%s:%d] ASSERT FAILED: %s", __FUNCTION__, __LINE__, #x); \
        goto lbExit;                                                        \
    }                                                                       \
} while(0);


using namespace NSCam;
using namespace NSIspTuningv3;
/*******************************************************************************
*
********************************************************************************/
typedef enum {
    SWNR_SCENARIO_NORMAL = 0,
    SWNR_SCENARIO_MFNR,
    SWNR_SCENARIO_DUAL,
} SWNR_SCENARIO;


static const char* SWNR_SCENARIO_toString(SWNR_SCENARIO scenario)
{
    switch (scenario)
    {
    case SWNR_SCENARIO_MFNR:
        return "MFNR";
    case SWNR_SCENARIO_DUAL:
        return "DUAL";
    case SWNR_SCENARIO_NORMAL:
        return "NORM";
    default:
        return "UNKNOWN";
    }
}


/*******************************************************************************
*
********************************************************************************/
inline MINT32 generateSeed() { return (MINT32)::systemTime(); }

#define DUMP_PARAM(fp, fmt, arg...)            \
    do {                                       \
        if (fp ) fprintf(fp, fmt "\n", ##arg); \
        else SWNR_LOGD(fmt, ##arg);              \
    } while(0)

/*******************************************************************************
*
********************************************************************************/
static const char* NRType_String(DrvNRObject_e NRType)
{
    switch (NRType)
    {
    case DRV_NR_OBJ_SW2:
        return "Normal SwNR";
    /*case DRV_NR_OBJ_SW2:
        return "Fast SwNR";
    case DRV_NR_OBJ_SW2_HIGH_PERF:
        return "Fast SwNR (high performance)";
    case DRV_NR_OBJ_SW2_LOW_POWER:
        return "Fast SwNR (low power)";*/
    case DRV_NR_OBJ_NONE:
    case DRV_NR_OBJ_UNKNOWN:
    default:
        return "Unsupported SwNR Type";
    }
}


static DrvNRObject_e ESWNRType_2_DrvNRObject(ESWNRType CustomerNRType)
{
    switch (CustomerNRType)
    {
    case eSWNRType_Default:
    case eSWNRType_IIR:
        return DRV_NR_OBJ_SW2;
        break;
    default:
        return DRV_NR_OBJ_NONE;
        break;
    }
}


static MVOID dumpParams(NRTuningInfo* pNR, FILE* fp = NULL)
{
    if (! pNR) {
        SWNR_LOGE("null");
        return;
    }
#define DUMP_NR(param)                               \
    do {                                             \
        DUMP_PARAM(fp, "%s:%d", #param, pNR->param); \
    } while(0)
    DUMP_NR(ANR_Y_LUMA_SCALE_RANGE);
    DUMP_NR(ANR_C_CHROMA_SCALE);
    DUMP_NR(ANR_Y_SCALE_CPY0);
    DUMP_NR(ANR_Y_SCALE_CPY1);
    DUMP_NR(ANR_Y_SCALE_CPY2);
    DUMP_NR(ANR_Y_SCALE_CPY3);
    DUMP_NR(ANR_Y_SCALE_CPY4);
    DUMP_NR(ANR_Y_CPX1);
    DUMP_NR(ANR_Y_CPX2);
    DUMP_NR(ANR_Y_CPX3);
    DUMP_NR(ANR_CEN_GAIN_LO_TH);
    DUMP_NR(ANR_CEN_GAIN_HI_TH);
    DUMP_NR(ANR_PTY_GAIN_TH);
    DUMP_NR(ANR_KSIZE_LO_TH);
    DUMP_NR(ANR_KSIZE_HI_TH);
    DUMP_NR(ANR_KSIZE_LO_TH_C);
    DUMP_NR(ANR_KSIZE_HI_TH_C);
    DUMP_NR(ITUNE_ANR_PTY_STD);
    DUMP_NR(ITUNE_ANR_PTU_STD);
    DUMP_NR(ITUNE_ANR_PTV_STD);
    DUMP_NR(ANR_ACT_TH_Y);
    DUMP_NR(ANR_ACT_BLD_BASE_Y);
    DUMP_NR(ANR_ACT_BLD_TH_Y);
    DUMP_NR(ANR_ACT_SLANT_Y);
    DUMP_NR(ANR_ACT_TH_C);
    DUMP_NR(ANR_ACT_BLD_BASE_C);
    DUMP_NR(ANR_ACT_BLD_TH_C);
    DUMP_NR(ANR_ACT_SLANT_C);
    DUMP_NR(RADIUS_H);
    DUMP_NR(RADIUS_V);
    DUMP_NR(RADIUS_H_C);
    DUMP_NR(RADIUS_V_C);
    DUMP_NR(ANR_PTC_HGAIN);
    DUMP_NR(ANR_PTY_HGAIN);
    DUMP_NR(ANR_LPF_HALFKERNEL);
    DUMP_NR(ANR_LPF_HALFKERNEL_C);
    DUMP_NR(ANR_ACT_MODE);
    DUMP_NR(ANR_LCE_SCALE_GAIN);
    DUMP_NR(ANR_LCE_C_GAIN);
    DUMP_NR(ANR_LCE_GAIN0);
    DUMP_NR(ANR_LCE_GAIN1);
    DUMP_NR(ANR_LCE_GAIN2);
    DUMP_NR(ANR_LCE_GAIN3);
    DUMP_NR(ANR_MEDIAN_LOCATION);
    DUMP_NR(ANR_CEN_X);
    DUMP_NR(ANR_CEN_Y);
    DUMP_NR(ANR_R1);
    DUMP_NR(ANR_R2);
    DUMP_NR(ANR_R3);
    DUMP_NR(LUMA_ON_OFF);
#undef DUMP_NR
}

static const NVRAM_CAMERA_FEATURE_SWNR_STRUCT *getTuningFromNvram(MUINT32 openId, SWNR_SCENARIO scenario, MUINT32 idx)
{
    int err;
    NVRAM_CAMERA_FEATURE_STRUCT *pNvram;
    const NVRAM_CAMERA_FEATURE_SWNR_STRUCT *pNRNvram;

    if (idx >= eNUM_OF_ISO_IDX) {
        SWNR_LOGE("wrong nvram idx %d", idx);
        return NULL;
    }

    // load some setting from nvram
    MUINT sensorDev = MAKE_HalSensorList()->querySensorDevIdx(openId);
    //
    err = NvBufUtil::getInstance().getBufAndRead(
            CAMERA_NVRAM_DATA_FEATURE, sensorDev, (void*&)pNvram);
    if (err != 0)
    {
        SWNR_LOGE("getBufAndRead fail, err=%d", err);
        return NULL;
    }

    pNRNvram = &(pNvram->swnr[idx]);
#if 0
    if( sizeof(NVRAM_CAMERA_FEATURE_SWNR_STRUCT) != sizeof(NRTuningInfo) )
    {
        MY_LOGE("NVRAM_CAMERA_FEATURE_SWNR_STRUCT size %d, NRTuningInfo size %d",
                sizeof(NVRAM_CAMERA_FEATURE_SWNR_STRUCT), sizeof(NRTuningInfo) );
        return MFALSE;
    }
#endif
    return pNRNvram;
}


static MBOOL getSensorDevice(MUINT* pSensorDev, MUINT *pSensorDevId, MUINT32 openId)
{
    MBOOL ret = MFALSE;
    IHalSensorList* const pHalSensorList = MAKE_HalSensorList();
    *pSensorDev = MAKE_HalSensorList()->querySensorDevIdx(openId);
    SensorStaticInfo rSensorStaticInfo;
    switch (*pSensorDev)
    {
    case SENSOR_DEV_MAIN:
        pHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
        break;

    case SENSOR_DEV_SUB:
        pHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
        break;

    case SENSOR_DEV_MAIN_2:
        pHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
        break;

    default:    //  Shouldn't happen.
        MY_ERR("Invalid sensor device: %d", *pSensorDev);
        SWNR_ASSERT(0);
    }

    *pSensorDevId = rSensorStaticInfo.sensorDevID;
    SWNR_LOGD("sensorDev(%d), u4SensorID(%d)", *pSensorDev, *pSensorDevId);

    ret = MTRUE;
lbExit:
    return ret;
}

static MBOOL allocWorkingBuf(sp<NSCam::IImageBuffer>& pImgBuf,  NRWorkingBufferInfo &MyWorkingBufInfo)
{
    // 4. set buffer address
    MBOOL ret = MFALSE;
    MUINT32 i;
    MUINT8 *pBuffer;
    MINT32 GBWorkBufNum =0;
    for (i=0; i<MyWorkingBufInfo.WorkingBufferNum; i++)
    {
        NRBuffer *pWorkingBuffer = &(MyWorkingBufInfo.WorkingBuffer[i]);
        if(pWorkingBuffer->MemType == NR_MEMORY_ION) {
            if(pWorkingBuffer->ImgFmt != NR_IMAGE_Y8 ||
               pWorkingBuffer->Size == 0 ||
               pWorkingBuffer->BufferNum != 1) {
                SWNR_LOGE("Get NR_FEATURE_GET_WORKBUF_SIZE info error (MemType: %d, ImgFmt: %d, Size: %d, BufferNum: %d,)",
                    pWorkingBuffer->MemType,
                    pWorkingBuffer->ImgFmt,
                    pWorkingBuffer->Size,
                    pWorkingBuffer->BufferNum);
                SWNR_ASSERT(0);
            }
            size_t bufBoundaryInBytes[3] = {0};
            if(!pImgBuf.get()){
                IImageBufferAllocator::ImgParam imgParam =
                            IImageBufferAllocator::ImgParam(
                                pWorkingBuffer->Size,
                                (*bufBoundaryInBytes)
                            );

                IImageBufferAllocator* allocator = IImageBufferAllocator::getInstance();
                pImgBuf = allocator->alloc_ion(
                                LOG_TAG,
                                imgParam
                                );
                pImgBuf->lockBuf(LOG_TAG, eBUFFER_USAGE_SW_MASK);
            }
            if(pWorkingBuffer->Size > pImgBuf->getBufSizeInBytes(0))
                SWNR_LOGE("SWNRAlgo want %d bytes wxh(%dx%d), but real size is %d", pWorkingBuffer->Size, pWorkingBuffer->Width, pWorkingBuffer->Height, pImgBuf->getBufSizeInBytes(0));
            pBuffer = (MUINT8*)pImgBuf->getBufVA(0);
            pWorkingBuffer->pBuff[0] = (void*)pBuffer;
            pWorkingBuffer->BuffFD[0] = pImgBuf->getFD(0);
            SWNR_LOGD("Working Buffer FD %d", pImgBuf->getFD(0));
        } else if(pWorkingBuffer->MemType == NR_MEMORY_GRALLOC_Y8) {
            SWNR_LOGD("working buffer: GrphicBuffer");
            const int gbTexUsage =  GraphicBuffer::USAGE_SW_WRITE_RARELY;   // GraphicBuffer::USAGE_HW_TEXTURE |
            const int gbTexFormat = HAL_PIXEL_FORMAT_Y8; //PIXEL_FORMAT_RGBA_8888/ PIXEL_FORMAT_RGB_888

            sp<GraphicBuffer> *gb_work_buf = new sp<GraphicBuffer>;
            *gb_work_buf = new GraphicBuffer(pWorkingBuffer->Width, pWorkingBuffer->Height, gbTexFormat, gbTexUsage);
            gb_work_buf->get()->incStrong(gb_work_buf->get());
            pWorkingBuffer->pBuff[0] = (void*)gb_work_buf;
            ++GBWorkBufNum;
        } else {
            SWNR_LOGE("Shuld Not Be Here!!");
            SWNR_ASSERT(0);
        }
    }

    ret = MTRUE;
lbExit:
    return ret;
}


static void freeWorkingBuf(sp<NSCam::IImageBuffer>& pImgBuf, NRWorkingBufferInfo &MyWorkingBufInfo)
{
    MUINT32 i;
    for (i=0; i<MyWorkingBufInfo.WorkingBufferNum; i++)
    {
        NRBuffer *pWorkingBuffer = &(MyWorkingBufInfo.WorkingBuffer[i]);
        void *pBuffer = pWorkingBuffer->pBuff[0];
        if (pBuffer)
        {
            if (pWorkingBuffer->MemType == NR_MEMORY_ION) {
                 if(pImgBuf.get() != NULL)
                    pImgBuf->unlockBuf(LOG_TAG);
                 IImageBufferAllocator* allocator = IImageBufferAllocator::getInstance();
                 allocator->free(pImgBuf.get());
                 pImgBuf = NULL;
            } else if(pWorkingBuffer->MemType == NR_MEMORY_GRALLOC_Y8) {
                sp<GraphicBuffer> *gb_work_buf = (sp<GraphicBuffer>*)pBuffer;
                gb_work_buf->get()->decStrong(gb_work_buf->get());
                delete gb_work_buf;
            }
            else
                SWNR_LOGW("CAN't free buffer MemType 0x%x", pWorkingBuffer->MemType);
        }
        pWorkingBuffer->pBuff[0] = NULL;
    }
}

static MUINT32 getNeighber(SWNR_SCENARIO scenario, MUINT32 iso, MUINT32& iso_low, MUINT32& iso_high)
{
    MUINT32 idx = map_ISO_value_to_index(iso, scenario, 1);
    // find lower and upper bound
    {
        MUINT32 iso2;
        iso2 = map_index_to_ISO_value((EIdxSwNR)idx);
        if (iso2 > iso) {
            if (idx > 0) {
                idx -= 1;
                iso_low = map_index_to_ISO_value((EIdxSwNR)idx);
                iso_high = iso2;
            } else {
                iso_low = iso_high = iso2;
            }
        } else {
            iso_low = iso2;
            iso_high = map_index_to_ISO_value((EIdxSwNR)(idx + 1));
        }
    }
    return idx;
}

static MBOOL getTuningInfo(
    SwNR::TUNINGINFO *pTuningInfo,  // output
    DrvNRObject_e *pNRType,         // output
    MUINT32 iso,
    SWNR_SCENARIO scenario,
    MUINT32 openId)
{
    MBOOL ret = MFALSE;
    IspTuningCustom *pIspTuningCustom = NULL;
    DrvNRObject_e NRType = DRV_NR_OBJ_NONE;

    MUINT sensorDevID;
    NVRAM_CAMERA_FEATURE_SWNR_STRUCT NvramData;
    IHalSensorList* const pHalSensorList = MAKE_HalSensorList();
    MUINT sensorDev = MAKE_HalSensorList()->querySensorDevIdx(openId);
    Paramctrl* pParamctrl = Paramctrl::getInstance((ESensorDev_T)sensorDev, openId);
    MUINT32 iso_low, iso_high;
    MUINT32 idx;

    SWNR_ASSERT(getSensorDevice(&sensorDev, &sensorDevID, openId) == MTRUE);

    pIspTuningCustom = IspTuningCustom::createInstance(
            static_cast<NSIspTuning::ESensorDev_T>(sensorDev),
            sensorDevID);
    SWNR_ASSERT(pIspTuningCustom != NULL);

    // 1. update swnr type
    NRType = ESWNRType_2_DrvNRObject((ESWNRType)get_swnr_type());//always return eSWNRType_Default
    {
        // force use swnr (for debug)
        MINT32 type = property_get_int32("vendor.debug.swnr.type", -1);
        if (type != -1) {
            NRType = ESWNRType_2_DrvNRObject((ESWNRType)type);
            SWNR_LOGD("force use SwNR type: %s", NRType_String(NRType));
        }
    }


    SWNR_ASSERT(NRType != DRV_NR_OBJ_UNKNOWN);

    iso = pIspTuningCustom/*ispCustom.get()*/->remap_ISO_value(iso);
    idx = getNeighber(scenario, iso, iso_low, iso_high);

    SWNR_LOGD("%d: iso=%d (%d~%d), isMfll=%d", openId, iso, iso_low, iso_high, scenario);


    // 2. read from nvram & smooth
    if (is_to_invoke_swnr_interpolation(scenario, iso) && (iso_low != iso_high))
    {
        const NVRAM_CAMERA_FEATURE_SWNR_STRUCT *pNvramLow   = getTuningFromNvram(openId, scenario, idx);
        const NVRAM_CAMERA_FEATURE_SWNR_STRUCT *pNvramHigh  = getTuningFromNvram(openId, scenario, idx + 1);
        if (!pNvramLow || !pNvramHigh)
        {
            SWNR_LOGE("update from nvram failed");
            return MFALSE;
        }

        // interpolation
        auto interpolation_func = SmoothSWNR ;
        interpolation_func(
                iso,
                iso_high, iso_low,
                *pNvramHigh,
                *pNvramLow,
                NvramData);
    } else {
        // get nvram
        const NVRAM_CAMERA_FEATURE_SWNR_STRUCT *pTuningNvram = getTuningFromNvram(openId, scenario, idx);
        if (!pTuningNvram) {
            SWNR_LOGE("update from nvram failed");
            return MFALSE;
        }

        // update
        memcpy(&NvramData, pTuningNvram, sizeof(NVRAM_CAMERA_FEATURE_SWNR_STRUCT));
    }
    // get tuning info from NVRam
    memcpy(&pTuningInfo->u.NR , &NvramData , sizeof(NRTuningInfo));

    {
        char value[PROPERTY_VALUE_MAX] = {'\0'};
        property_get("vendor.debug.swnr.enable", value, "0");
        MBOOL bDebug = atoi(value);

        if (bDebug) {
            if (NRType == DRV_NR_OBJ_SW2) {
                dumpParams(&pTuningInfo->u.NR);
            }
        }
    }

    ret = MTRUE;
lbExit:
    *pNRType = NRType;
    if (pIspTuningCustom) pIspTuningCustom->destroyInstance();
    if (pParamctrl) pParamctrl->destroyInstance();
    return ret;
}
/******************************************************************************
 *
 ******************************************************************************/
template <typename T>
inline MBOOL
tryGetMetadata(
    IMetadata* pMetadata,
    MUINT32 const tag,
    T & rVal
)
{
    if( pMetadata == NULL ) {
        MY_LOGE("pMetadata == NULL");
        return MFALSE;
    }

    IMetadata::IEntry entry = pMetadata->entryFor(tag);
    if( !entry.isEmpty() ) {
        rVal = entry.itemAt(0, Type2Type<T>());
        return MTRUE;
    }
    return MFALSE;
}

ISwNR*
ISwNR::
createInstance(MUINT32 const openId)
{
    return new SwNR(openId);
}

static MBOOL setImageInfo(NRImageInfo &MyNRImageInfo, const IImageBuffer* pBuf, DrvNRObject_e NRType)
{
    MBOOL ret = MFALSE;
    MTK_NR_IMAGE_FMT_ENUM NRFmt = NR_IMAGE_Y8;
    IImageBufferHeap *heap = NULL;

    switch( pBuf->getImgFormat() )
    {
        case eImgFmt_Y8:
            NRFmt = NR_IMAGE_Y8;
            break;
        case eImgFmt_I420:
            NRFmt = NR_IMAGE_YUV420;
            break;
        case eImgFmt_YV12:
            NRFmt = NR_IMAGE_YV12;
            break;
        default:
            SWNR_LOGE("not supported format: 0x%x", pBuf->getImgFormat() );
            goto lbExit;
    }

    // set image info
    MyNRImageInfo.Width = pBuf->getImgSize().w;
    MyNRImageInfo.Height = pBuf->getImgSize().h;

    if (NRType==DRV_NR_OBJ_SW2 || NRType==DRV_NR_OBJ_SW2_VPU) {
        MyNRImageInfo.pImg[0] = (void*)pBuf->getBufVA(0);
        MyNRImageInfo.pImg[1] = pBuf->getPlaneCount() > 1 ? (void*)pBuf->getBufVA(1) : (void*)0;
        MyNRImageInfo.pImg[2] = pBuf->getPlaneCount() > 2 ? (void*)pBuf->getBufVA(2) : (void*)0;
    }
#if GPUVERSION
    else {
        SWNR_LOGD("[%s:%d] plane=%d", __FUNCTION__, __LINE__, pBuf->getPlaneCount());
        heap = pBuf->getImageBufferHeap();
        MyNRImageInfo.pImg[0] = (void*)heap->getGraphicBuffer(0);
        MyNRImageInfo.pImg[1] = pBuf->getPlaneCount() > 1 ? (void*)heap->getGraphicBuffer(1) : (void*)0;
        MyNRImageInfo.pImg[2] = pBuf->getPlaneCount() > 2 ? (void*)heap->getGraphicBuffer(2) : (void*)0;
    }
#endif
    MyNRImageInfo.ImgFD[0] = pBuf->getFD(0);
    MyNRImageInfo.ImgFD[1] = pBuf->getPlaneCount() > 1 ? pBuf->getFD(1) : -1;
    MyNRImageInfo.ImgFD[2] = pBuf->getPlaneCount() > 2 ? pBuf->getFD(2) : -1;
    MyNRImageInfo.ImgFmt = NRFmt;
    SWNR_LOGD("Buffer FD %d/%d/%d", MyNRImageInfo.ImgFD[0], MyNRImageInfo.ImgFD[1], MyNRImageInfo.ImgFD[2]);
    ret = MTRUE;
lbExit:
    return ret;
}

SwNR::SwNR(MUINT32 const openId)
    : muOpenId(openId)
    , mNRType(DRV_NR_OBJ_NONE)
    , mpWb(nullptr)
{
    mDbgReuseWb = ::property_get_bool("ro.config.low_ram", false) ? 0 : 1;

    if(mDbgReuseWb) {
        sp<IMetadataProvider> pMetadataProvider = NSMetadataProviderManager::valueFor(openId);
        if(!pMetadataProvider.get())
            SWNR_LOGE(" !pMetadataProvider.get() ");
        IMetadata static_meta = pMetadataProvider->getMtkStaticCharacteristics();
        MRect activeArray;
        if (tryGetMetadata<MRect>(&static_meta, MTK_SENSOR_INFO_ACTIVE_ARRAY_REGION, activeArray)) {
            //CAM_TRACE_BEGIN("swnr:allocWb");
            size_t bufBoundaryInBytes[3] = {0};
            IImageBufferAllocator::ImgParam imgParam =
                        IImageBufferAllocator::ImgParam(
                            NR2_BUFFER_SIZE(activeArray.s.w, activeArray.s.h),
                            (*bufBoundaryInBytes)
                        );
            IImageBufferAllocator* allocator = IImageBufferAllocator::getInstance();
            mpWb = allocator->alloc_ion(
                            LOG_TAG,
                            imgParam
                            );
            mpWb->lockBuf(LOG_TAG, eBUFFER_USAGE_SW_MASK);
            //CAM_TRACE_END();
            SWNR_LOGD("get active array(%d, %d, %dx%d) to alloc Wb(%d)",
                      activeArray.p.x, activeArray.p.y,
                      activeArray.s.w, activeArray.s.h, (int)NR2_BUFFER_SIZE(activeArray.s.w, activeArray.s.h));
        } else {
            SWNR_LOGE("no static info: MTK_SENSOR_INFO_ACTIVE_ARRAY_REGION");
        }
    }
    memset(&mTuningInfo, 0, sizeof(mTuningInfo));
}

SwNR::~SwNR()
{
    if(mDbgReuseWb) {
        if(mpWb.get() != NULL)
            mpWb->unlockBuf(LOG_TAG);
        IImageBufferAllocator* allocator = IImageBufferAllocator::getInstance();
        allocator->free(mpWb.get());
        mpWb = NULL;
    }
}

MBOOL SwNR::doSwNR(SWNRParam const& param, NSCam::IImageBuffer* pBuf)
{
    MBOOL ret = MFALSE;
    SWNR_SCENARIO scenario;

    MTKNR2* MyNR = NULL;
    MTKNRGPU* MyGPU = NULL;

    NRInitInfo          MyNRInitInfo;
    NRWorkingBufferInfo MyWorkingBufInfo;
    NRImageInfo         MyNRImageInfo;
    sp<IPower> pPerf = IPower::getService();
    MINT32 handle_perf_serv_iir = -1;
    MUINT32 coreNum = 0;
    MUINT32 numOfCluster = 0;
    MUINT32 numOfCPU = 0;
    // TODO:Check pWorkingImgBuf
    sp<IImageBuffer> pWorkingImgBuf = mDbgReuseWb ? mpWb : NULL;
    NR2TuningInfo    newNrTunning;
    NR2TuningInfo    *fetchTuning = nullptr;

    SWNR_LOGD("idx %d: iso %d perf %d, buf %p", muOpenId, param.iso, param.perfLevel, pBuf);

    memset(&MyWorkingBufInfo, 0, sizeof(MyWorkingBufInfo));

    // get tuning info
    if (param.isMfll) scenario = SWNR_SCENARIO_MFNR;
    else if (param.isDualCam) scenario = SWNR_SCENARIO_DUAL;
    else scenario = SWNR_SCENARIO_NORMAL;
    char filename[PROPERTY_VALUE_MAX] = {'\0'};
    SWNR_ASSERT(getTuningInfo(&mTuningInfo, &mNRType, param.iso, scenario, muOpenId) != 0);
    // debug: read from file
    #if 0
    if( property_get("vendor.debug.swnr.readfile", filename, "") > 0 ) {
        memset(&mTuningInfo, 0, sizeof(mTuningInfo));
        getTuningFromFile(filename, mTuningInfo);
    }
    #endif
    SWNR_LOGD("SwNR type: %s", NRType_String(mNRType));

    numOfCluster = pPerf->querySysInfo(MtkQueryCmd::CMD_GET_CLUSTER_NUM, 0);

    // set MyNRInitInfo
    for(int i=0;i<numOfCluster;i++)
    {
        numOfCPU = pPerf->querySysInfo(MtkQueryCmd::CMD_GET_CLUSTER_CPU_NUM, i);
        coreNum += numOfCPU;
    }

    fetchTuning = (NR2TuningInfo *)&mTuningInfo.u.NR;
    newNrTunning.NR_K = fetchTuning->NR_K;
    newNrTunning.NR_S = fetchTuning->NR_S;
    MyNRInitInfo.pNR2TuningInfo    = (NR2TuningInfo *)&newNrTunning;
    MyNRInitInfo.CoreNumber     = coreNum;
    MyNRInitInfo.NumOfExecution = 1;
    MyNRInitInfo.pCCRTuningInfo = nullptr;
    MyNRInitInfo.pHFGTuningInfo = nullptr;

    // ISP 3.0 only NR_K and NR_S can be tuned
    SWNR_LOGD("Swnr tuning data NR_K(%d) NR_S(%d)", MyNRInitInfo.pNR2TuningInfo->NR_K
                                                , MyNRInitInfo.pNR2TuningInfo->NR_S);

    // 1. create instance
    MyNR = MTKNR2::createInstance(mNRType); // DRV_NR_OBJ_SW2_LOW_POWER, DRV_NR_OBJ_SW2_LOW_POWER, DRV_NR_OBJ_SW2_HIGH_PERF
    SWNR_ASSERT(MyNR != NULL);
#if GPUVERSION
    if (mNRType==DRV_NR_OBJ_SW2_HIGH_PERF || mNRType==DRV_NR_OBJ_SW2_LOW_POWER)
    {
        MyGPU = MTKNRGPU::createInstance(mNRType);
        SWNR_ASSERT(MyGPU != NULL);
        SWNR_ASSERT(MyGPU->NRGPUInit(NULL)==S_NR_OK);
    }
#endif
    // 2. init
    SWNR_ASSERT(MyNR->NRInit(&MyNRInitInfo, 0) == S_NR_OK);

    // 3. get buffer size
    MyWorkingBufInfo.Width = pBuf->getImgSize().w;
    MyWorkingBufInfo.Height = pBuf->getImgSize().h;
    SWNR_ASSERT(MyNR->NRFeatureCtrl(NR_FEATURE_GET_WORKBUF_SIZE, (void*)&MyWorkingBufInfo, NULL) == S_NR_OK);

    SWNR_ASSERT(allocWorkingBuf(pWorkingImgBuf, MyWorkingBufInfo) == MTRUE);
    // 4. set buffer address
    SWNR_ASSERT(MyNR->NRFeatureCtrl(NR_FEATURE_SET_WORKBUF_ADDR, (void*)&MyWorkingBufInfo, NULL) == S_NR_OK);

    // 5. get src image requirement
    SWNR_ASSERT(MyNR->NRFeatureCtrl(NR_FEATURE_GET_INPUT_FMT, (void *)&MyNRImageInfo, NULL) == S_NR_OK);
    SWNR_ASSERT(setImageInfo(MyNRImageInfo, pBuf, mNRType) == MTRUE);

    //CAM_TRACE_BEGIN("enablePerformanceService");
    enablePerformanceService(handle_perf_serv_iir, param.perfLevel);
    //CAM_TRACE_END();
    // 6. Main
    MyNRImageInfo.Instance = MyGPU;
    NRVerInfo Version;
    SWNR_ASSERT(MyNR->NRFeatureCtrl(NR_FEATURE_GET_VERSION, NULL, &Version) == S_NR_OK);
    //CAM_TRACE_BEGIN("ALGO:swnr main");
    SWNR_ASSERT(MyNR->NRMain(&MyNRImageInfo, NULL) == S_NR_OK);
    //CAM_TRACE_END();
    //if(mNRType!=DRV_NR_OBJ_SW2_VPU||mTuningInfo.HFG.HFG_ENABLE||mTuningInfo.CCR.CCR_ENABLE)
    {
        //CAM_TRACE_BEGIN("sysncCache");
        SWNR_LOGD("sync cache+");
        pBuf->syncCache(eCACHECTRL_FLUSH);
        SWNR_LOGD("sync cache-");
        //CAM_TRACE_END();
    }
    //CAM_TRACE_BEGIN("disablePerformanceService");
    disablePerformanceService(handle_perf_serv_iir);
    //CAM_TRACE_END();
    // 7. reset
    MyNR->NRReset();

    ret = MTRUE;
lbExit:
    // TODO: Check mDbgReuseWb
    // freeWorkingBuf
    if(!mDbgReuseWb) {
        freeWorkingBuf(pWorkingImgBuf, MyWorkingBufInfo);
        pWorkingImgBuf = NULL;
    }
#if GPUVERSION
    // 8. destroy (MyGPU)
    if (MyGPU) {
        MyGPU->NRGPUReset();
        MyGPU->destroyInstance(MyGPU);
        MyGPU = NULL;
    }
#endif
    // 8. destroy (MyNR)
    if (MyNR) {
        MyNR->destroyInstance(MyNR);
        MyNR = NULL;
    }

    return ret;
}


MBOOL SwNR::getDebugInfo(IMetadata& halMetadata) const
{
    bool haveExif = false;
    {
        IMetadata::IEntry entry = halMetadata.entryFor(MTK_HAL_REQUEST_REQUIRE_EXIF);
        if (! entry.isEmpty()  && entry.itemAt(0, Type2Type<MUINT8>()) )
                haveExif = true;
    }
    //
    if (haveExif )
    {
        IMetadata::Memory memory_dbgInfo;
        memory_dbgInfo.resize(sizeof(DEBUG_RESERVEA_INFO_T));
        DEBUG_RESERVEA_INFO_T& dbgInfo =
            *reinterpret_cast<DEBUG_RESERVEA_INFO_T*>(memory_dbgInfo.editArray());
        ssize_t idx = 0;
#define addPair(debug_info, index, id, value)           \
        do{                                             \
            debug_info.Tag[index].u4FieldID = (0x01000000 | id); \
            debug_info.Tag[index].u4FieldValue = value; \
            index++;                                    \
        } while(0)
        //
        addPair(dbgInfo, idx, RESERVEA_TAG_VERSION, RESERVEA_DEBUG_TAG_VERSION);
        //
        if (mNRType == DRV_NR_OBJ_SW2 ) // normal swnr
        {
            addPair(dbgInfo , idx , ANR_Y_LUMA_SCALE_RANGE , mTuningInfo.u.NR.ANR_Y_LUMA_SCALE_RANGE );
            addPair(dbgInfo , idx , ANR_C_CHROMA_SCALE     , mTuningInfo.u.NR.ANR_C_CHROMA_SCALE     );
            addPair(dbgInfo , idx , ANR_Y_SCALE_CPY0       , mTuningInfo.u.NR.ANR_Y_SCALE_CPY0       );
            addPair(dbgInfo , idx , ANR_Y_SCALE_CPY1       , mTuningInfo.u.NR.ANR_Y_SCALE_CPY1       );
            addPair(dbgInfo , idx , ANR_Y_SCALE_CPY2       , mTuningInfo.u.NR.ANR_Y_SCALE_CPY2       );
            addPair(dbgInfo , idx , ANR_Y_SCALE_CPY3       , mTuningInfo.u.NR.ANR_Y_SCALE_CPY3       );
            addPair(dbgInfo , idx , ANR_Y_SCALE_CPY4       , mTuningInfo.u.NR.ANR_Y_SCALE_CPY4       );
            addPair(dbgInfo , idx , ANR_Y_CPX2             , mTuningInfo.u.NR.ANR_Y_CPX2             );
            addPair(dbgInfo , idx , ANR_Y_CPX3             , mTuningInfo.u.NR.ANR_Y_CPX3             );
            addPair(dbgInfo , idx , ANR_CEN_GAIN_LO_TH     , mTuningInfo.u.NR.ANR_CEN_GAIN_LO_TH     );
            addPair(dbgInfo , idx , ANR_CEN_GAIN_HI_TH     , mTuningInfo.u.NR.ANR_CEN_GAIN_HI_TH     );
            addPair(dbgInfo , idx , ANR_PTY_GAIN_TH        , mTuningInfo.u.NR.ANR_PTY_GAIN_TH        );
            addPair(dbgInfo , idx , ANR_KSIZE_LO_TH        , mTuningInfo.u.NR.ANR_KSIZE_LO_TH        );
            addPair(dbgInfo , idx , ANR_KSIZE_HI_TH        , mTuningInfo.u.NR.ANR_KSIZE_HI_TH        );
            addPair(dbgInfo , idx , ANR_KSIZE_LO_TH_C      , mTuningInfo.u.NR.ANR_KSIZE_LO_TH_C      );
            addPair(dbgInfo , idx , ANR_KSIZE_HI_TH_C      , mTuningInfo.u.NR.ANR_KSIZE_HI_TH_C      );
            addPair(dbgInfo , idx , ITUNE_ANR_PTY_STD      , mTuningInfo.u.NR.ITUNE_ANR_PTY_STD      );
            addPair(dbgInfo , idx , ITUNE_ANR_PTU_STD      , mTuningInfo.u.NR.ITUNE_ANR_PTU_STD      );
            addPair(dbgInfo , idx , ITUNE_ANR_PTV_STD      , mTuningInfo.u.NR.ITUNE_ANR_PTV_STD      );
            addPair(dbgInfo , idx , ANR_ACT_TH_Y           , mTuningInfo.u.NR.ANR_ACT_TH_Y           );
            addPair(dbgInfo , idx , ANR_ACT_BLD_BASE_Y     , mTuningInfo.u.NR.ANR_ACT_BLD_BASE_Y     );
            addPair(dbgInfo , idx , ANR_ACT_BLD_TH_Y       , mTuningInfo.u.NR.ANR_ACT_BLD_TH_Y       );
            addPair(dbgInfo , idx , ANR_ACT_SLANT_Y        , mTuningInfo.u.NR.ANR_ACT_SLANT_Y        );
            addPair(dbgInfo , idx , ANR_ACT_TH_C           , mTuningInfo.u.NR.ANR_ACT_TH_C           );
            addPair(dbgInfo , idx , ANR_ACT_BLD_BASE_C     , mTuningInfo.u.NR.ANR_ACT_BLD_BASE_C     );
            addPair(dbgInfo , idx , ANR_ACT_BLD_TH_C       , mTuningInfo.u.NR.ANR_ACT_BLD_TH_C       );
            addPair(dbgInfo , idx , ANR_ACT_SLANT_C        , mTuningInfo.u.NR.ANR_ACT_SLANT_C        );
            addPair(dbgInfo , idx , RADIUS_H               , mTuningInfo.u.NR.RADIUS_H               );
            addPair(dbgInfo , idx , RADIUS_V               , mTuningInfo.u.NR.RADIUS_V               );
            addPair(dbgInfo , idx , RADIUS_H_C             , mTuningInfo.u.NR.RADIUS_H_C             );
            addPair(dbgInfo , idx , RADIUS_V_C             , mTuningInfo.u.NR.RADIUS_V_C             );
            addPair(dbgInfo , idx , ANR_PTC_HGAIN          , mTuningInfo.u.NR.ANR_PTC_HGAIN          );
            addPair(dbgInfo , idx , ANR_PTY_HGAIN          , mTuningInfo.u.NR.ANR_PTY_HGAIN          );
            addPair(dbgInfo , idx , ANR_LPF_HALFKERNEL     , mTuningInfo.u.NR.ANR_LPF_HALFKERNEL     );
            addPair(dbgInfo , idx , ANR_LPF_HALFKERNEL_C   , mTuningInfo.u.NR.ANR_LPF_HALFKERNEL_C   );
            addPair(dbgInfo , idx , ANR_ACT_MODE           , mTuningInfo.u.NR.ANR_ACT_MODE           );
            addPair(dbgInfo , idx , ANR_LCE_SCALE_GAIN     , mTuningInfo.u.NR.ANR_LCE_SCALE_GAIN     );
            addPair(dbgInfo , idx , ANR_LCE_C_GAIN         , mTuningInfo.u.NR.ANR_LCE_C_GAIN         );
            addPair(dbgInfo , idx , ANR_LCE_GAIN0          , mTuningInfo.u.NR.ANR_LCE_GAIN0          );
            addPair(dbgInfo , idx , ANR_LCE_GAIN1          , mTuningInfo.u.NR.ANR_LCE_GAIN1          );
            addPair(dbgInfo , idx , ANR_LCE_GAIN2          , mTuningInfo.u.NR.ANR_LCE_GAIN2          );
            addPair(dbgInfo , idx , ANR_LCE_GAIN3          , mTuningInfo.u.NR.ANR_LCE_GAIN3          );
            addPair(dbgInfo , idx , ANR_MEDIAN_LOCATION    , mTuningInfo.u.NR.ANR_MEDIAN_LOCATION    );
            addPair(dbgInfo , idx , ANR_CEN_X              , mTuningInfo.u.NR.ANR_CEN_X              );
            addPair(dbgInfo , idx , ANR_CEN_Y              , mTuningInfo.u.NR.ANR_CEN_Y              );
            addPair(dbgInfo , idx , ANR_R1                 , mTuningInfo.u.NR.ANR_R1                 );
            addPair(dbgInfo , idx , ANR_R2                 , mTuningInfo.u.NR.ANR_R2                 );
            addPair(dbgInfo , idx , ANR_R3                 , mTuningInfo.u.NR.ANR_R3                 );
            addPair(dbgInfo , idx , LUMA_ON_OFF            , mTuningInfo.u.NR.LUMA_ON_OFF            );
        }
        else {
            SWNR_LOGE("not supported type %d", mNRType);
            return MFALSE;
        }
        // set zeros to remaining
        while( idx < RESERVEA_DEBUG_TAG_SIZE) {
            addPair(dbgInfo , idx , 0, 0);
        }
        //
#undef addPair
        //
        IMetadata exifMeta;
        // query from hal metadata first
        {
            IMetadata::IEntry entry= halMetadata.entryFor(MTK_3A_EXIF_METADATA);
            if (! entry.isEmpty() )
                exifMeta = entry.itemAt(0, Type2Type<IMetadata>());
        }
        // update
        IMetadata::IEntry entry_key(MTK_POSTNR_EXIF_DBGINFO_NR_KEY);
        entry_key.push_back(DEBUG_EXIF_MID_CAM_RESERVE1, Type2Type<MINT32>());
        exifMeta.update(entry_key.tag(), entry_key);
        //
        IMetadata::IEntry entry_data(MTK_POSTNR_EXIF_DBGINFO_NR_DATA);
        entry_data.push_back(memory_dbgInfo, Type2Type<IMetadata::Memory>());
        exifMeta.update(entry_data.tag(), entry_data);
        //
        IMetadata::IEntry entry_exif(MTK_3A_EXIF_METADATA);
        entry_exif.push_back(exifMeta, Type2Type<IMetadata>());
        halMetadata.update(entry_exif.tag(), entry_exif);
    }
    else
    {
        SWNR_LOGD("no need to dump exif");
    }
    //
    return MTRUE;
}


MVOID SwNR::dumpParam(char * const filename)
{
    // write to file
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        SWNR_LOGE("fopen fail: %s", filename);
        return;
    }

    fprintf(fp, "SwNR type: %s\n\n", NRType_String(mNRType));

    dumpParams(&mTuningInfo.u.NR, fp);

    fclose(fp);
}
MVOID SwNR::enablePerformanceService(
    MINT32& handle_perf_serv_iir,
    MINT32 const perfLevel )
{
    MINT32 policy = property_get_int32("vendor.debug.swnr.changecpupolicy", 1);
    sp<IPower> pPowerHal = IPower::getService();
    if(pPowerHal.get() == nullptr) return;
    handle_perf_serv_iir = pPowerHal->scnReg();
    if (handle_perf_serv_iir == -1)
    {
        SWNR_LOGD("[%s] PerfServiceNative_userRegScn fail!\n", LOG_TAG);
    }
    else
    {
        SWNR_LOGD("[%s] PerfServiceNative_userRegScn succeed! handle = %d policy(%d)\n", LOG_TAG, handle_perf_serv_iir, policy);
        if(policy==1)
        {
            pPowerHal-> scnConfig(handle_perf_serv_iir, MtkPowerCmd::CMD_SET_FG_BOOST_VALUE, 100, 0, 0, 0);
            pPowerHal-> scnConfig(handle_perf_serv_iir, MtkPowerCmd::CMD_SET_MTK_PREFER_IDLE, 1, 0, 0, 0);
            pPowerHal-> scnEnable(handle_perf_serv_iir, 0);
        }
        else if(policy==2)
        {
            pPowerHal-> scnConfig(handle_perf_serv_iir, MtkPowerCmd::CMD_SET_CPU_PERF_MODE, 1, 0, 0, 0);
            pPowerHal-> scnConfig(handle_perf_serv_iir, MtkPowerCmd::CMD_SET_SCHED_MODE, 0, 0, 0, 0);
            pPowerHal-> scnEnable(handle_perf_serv_iir, 0);
        }
        else {
        }
    }

}

MVOID SwNR::disablePerformanceService(MINT32 const handle_perf_serv_iir)
{
    sp<IPower> pPowerHal = IPower::getService();
    if(pPowerHal.get() == nullptr) return;
    if (handle_perf_serv_iir != -1) // registered
    {
        pPowerHal-> scnDisable (handle_perf_serv_iir);
        pPowerHal-> scnUnreg (handle_perf_serv_iir);
    }
}

