#include <cutils/log.h>
#include "JpegEnc.h"
#include "mhal_jpeg.h"
#include "img_common_def.h"

#include <ion/ion.h>
#include <ion.h>
#include <linux/mtk_ion.h>
#include <linux/ion_drv.h>
#include <hidl/HidlTransportSupport.h>
#include <HandleImporter.h>

#define JPG_DBG(fmt, arg...)    ALOGD(fmt, ##arg)
#define JPG_WARN(fmt, arg...)    ALOGW(fmt, ##arg)
#define JPG_ERR(fmt, arg...)    ALOGE(fmt, ##arg)

namespace vendor {
namespace mediatek {
namespace hardware {
namespace jpeg {
namespace V1_0 {
namespace implementation {

using ::vendor::mediatek::hardware::jpeg::V1_0::Result;
using ::vendor::mediatek::hardware::jpeg::V1_0::JpegEncParam;
using ::vendor::mediatek::hardware::jpeg::V1_0::Format;

using ::android::hardware::hidl_handle;
using ::android::hardware::camera::common::V1_0::helper::HandleImporter;

/*
  * get secure handle from ion buf fd
 */
uint32_t Jpeg_Get_Sec_Handle(int32_t ionFD, int32_t bufFD) {
    int32_t ret;
    struct ion_sys_data sys_data;
    ion_user_handle_t ionHandle;

    ret = ion_import(ionFD, bufFD, &ionHandle);
    if (ret < 0) {
        JPG_ERR("import ionHandle fail.\n");
        return 0;
    }

    sys_data.sys_cmd = ION_SYS_GET_PHYS;
    sys_data.get_phys_param.handle = ionHandle;

    if (ion_custom_ioctl(ionFD, ION_CMD_SYSTEM, &sys_data)) {
        JPG_ERR("ion_custom_ioctl failed to get secure handle\n");
        return 0;
    }

    JPG_DBG("Secure memory: handle(0x%x) size(%lu)\n",
            sys_data.get_phys_param.phy_addr, sys_data.get_phys_param.len);

    return sys_data.get_phys_param.phy_addr;
}

// Methods from ::vendor::mediatek::hardware::jpeg::V1_0::IJpegEnc follow.
Return<uint32_t> JpegEnc::startJpegEnc(const ::vendor::mediatek::hardware::jpeg::V1_0::JpegEncParam &parameter) {
    // TODO implement
    //create a MHAL_JPEG_ENC_START_IN struct and bypass parameter to it
    MHAL_JPEG_ENC_START_IN *JpegEncParam = new MHAL_JPEG_ENC_START_IN;
    JpegEncParam->encSize = new uint32_t;
    uint32_t bs_size;

    int32_t ionFD;

    const native_handle_t *luma_handle = nullptr;
    const native_handle_t *chroma_handle = nullptr;
    const native_handle_t *bs_handle = nullptr;
    HandleImporter hanle_importer;
    //UREE_SECUREMEM_HANDLE SrcLumaSecHandle = 0;
    //UREE_SECUREMEM_HANDLE SrcChromaSecHandle = 0;
    //UREE_SECUREMEM_HANDLE DstSecHandle = 0;

    JpegEncParam->srcAddr = nullptr;
    JpegEncParam->srcChromaAddr = nullptr;
    JpegEncParam->dstAddr = nullptr;

    JpegEncParam->bSecure = parameter.bSecure;

    //get secure handle from hidl handle
    // get ion fd from hidl_handle
    luma_handle = parameter.LumaHandle.getNativeHandle();
    hanle_importer.importBuffer(luma_handle);
    JPG_DBG("luma_handle: %p, version: 0x%x, numFds: 0x%x, numInts: 0x%x, handle: 0x%x, native handle: 0x%x\n",
            luma_handle, luma_handle->version, luma_handle->numFds,
            luma_handle->numInts, luma_handle->data[0], parameter.LumaHandle.getNativeHandle()->data[0]);

    if ((parameter.bContin == 0) &&
        ((parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV12) || (parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV21))) {
        chroma_handle = parameter.ChromaHandle.getNativeHandle();
        hanle_importer.importBuffer(chroma_handle);
        JPG_DBG("chroma_handle: %p, version: 0x%x, numFds: 0x%x, numInts: 0x%x, handle: 0x%x, native handle: 0x%x\n",
                chroma_handle, chroma_handle->version, chroma_handle->numFds,
                chroma_handle->numInts, chroma_handle->data[0], parameter.ChromaHandle.getNativeHandle()->data[0]);
    }

    bs_handle = parameter.DstHandle.getNativeHandle();
    hanle_importer.importBuffer(bs_handle);
    JPG_DBG("bs_handle: %p, version: 0x%x, numFds: 0x%x, numInts: 0x%x, handle: 0x%x, native handle: 0x%x\n",
            bs_handle, bs_handle->version, bs_handle->numFds,
            bs_handle->numInts, bs_handle->data[0], parameter.DstHandle.getNativeHandle()->data[0]);

    ionFD = open("/dev/ion", O_RDONLY);
    if (ionFD < 0) {
        JPG_ERR(" open ION fail!\n");
        return 0;
    }

    if (parameter.bSecure) {
        // get sec handle from ion fd
        JpegEncParam->SrcLumaSecHandle = Jpeg_Get_Sec_Handle(ionFD, luma_handle->data[0]);
        if ((parameter.bContin == 0) &&
            ((parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV12) || (parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV21)))
            JpegEncParam->SrcChromaSecHandle = Jpeg_Get_Sec_Handle(ionFD, chroma_handle->data[0]);

        JpegEncParam->DstSecHandle = Jpeg_Get_Sec_Handle(ionFD, bs_handle->data[0]);

        JPG_DBG("bSecure: %d, SrcLumaSecHandle: 0x%x, SrcChromaSecHandle: 0x%x, DstSecHandle: 0x%x\n",
                JpegEncParam->bSecure, JpegEncParam->SrcLumaSecHandle, JpegEncParam->SrcChromaSecHandle,
                JpegEncParam->DstSecHandle);
    } else {
        //parameter.SrcLumaSecHandle = luma_handle->data[0];
        JpegEncParam->srcFD = luma_handle->data[0];
        if ((parameter.bContin == 0) &&
            ((parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV12) || (parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV21)))
            {
                //parameter.SrcChromaSecHandle = chroma_handle->data[0];
                JpegEncParam->srcChromaFD = chroma_handle->data[0];
            }
        //parameter.DstSecHandle = bs_handle->data[0];
        JpegEncParam->dstFD = bs_handle->data[0];
        JPG_DBG("srcFD: 0x%x, srcChromaFD: 0x%x, dstFD: 0x%x\n",
                JpegEncParam->srcFD, JpegEncParam->srcChromaFD, JpegEncParam->dstFD);
    }

    JpegEncParam->srcBufferSize = parameter.SrcLumaBufSize;
    if ((parameter.bContin == 0) &&
        ((parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV12) || (parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV21)))
    JpegEncParam->srcChromaBufferSize = parameter.SrcChromaBufSize;
    JpegEncParam->dstBufferSize = parameter.DstBufSize;
    JpegEncParam->imgWidth = parameter.Width;
    JpegEncParam->imgHeight = parameter.Height;
    //JpegEncParam->imgStride = parameter.memStride;
    JpegEncParam->enableSOI = parameter.enableSOI;
    JpegEncParam->quality = parameter.Quality;
    JpegEncParam->bOnePlane = parameter.bContin;
    if ((parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV12) || (parameter.jpegFormat == Format::JPEG_ENC_FORMAT_NV21))
        JpegEncParam->imgStride = TO_CEIL(parameter.Width, parameter.Walign);
    else
        JpegEncParam->imgStride = TO_CEIL((parameter.Width << 1), parameter.Walign);
    JpegEncParam->AlignedH = TO_CEIL(parameter.Height, parameter.Halign);

    switch (parameter.jpegFormat) {
    case Format::JPEG_ENC_FORMAT_YUY2:
        JpegEncParam->encFormat = JPEG_ENC_FORMAT_YUY2;
        break;
    case Format::JPEG_ENC_FORMAT_UYVY:
        JpegEncParam->encFormat = JPEG_ENC_FORMAT_UYVY;
        break;
    case Format::JPEG_ENC_FORMAT_NV12:
        JpegEncParam->encFormat = JPEG_ENC_FORMAT_NV12;
        break;
    case Format::JPEG_ENC_FORMAT_NV21:
        JpegEncParam->encFormat = JPEG_ENC_FORMAT_NV21;
        break;
    default:
        JpegEncParam->encFormat = JPEG_ENC_FORMAT_UNKNOWN;
        JPG_ERR("unsupported format: %d\n", parameter.jpegFormat);
        return 0;
    }

    JPG_DBG("Walign: %d, Halign: %d\n", parameter.Walign, parameter.Halign);
    JPG_DBG("bContin: %d, srcBufferSize: 0x%x, srcChromaBufferSize: 0x%x, dstBufferSize: 0x%x\n",
            JpegEncParam->bOnePlane, JpegEncParam->srcBufferSize, JpegEncParam->srcChromaBufferSize,
            JpegEncParam->dstBufferSize);
    JPG_DBG("imgWidth: %d, imgHeight: %d, imgStride: %d\n",
            JpegEncParam->imgWidth, JpegEncParam->imgHeight,
            JpegEncParam->imgStride);
    JPG_DBG("enableSOI: %d, quality: %d, encFormat: %d\n",
            JpegEncParam->enableSOI, JpegEncParam->quality,
            JpegEncParam->encFormat);

    //call mHalJpgEncStart()
    mHalJpgEncStart(JpegEncParam);

    bs_size = *JpegEncParam->encSize;
    JPG_DBG("bs_size: 0x%x\n", bs_size);

    delete JpegEncParam->encSize;
    delete JpegEncParam;

    return bs_size;

    //return uint32_t {};
}

Return<uint8_t> JpegEnc::queryJpegEncFormat(::vendor::mediatek::hardware::jpeg::V1_0::Format JpegFormat) {
    // TODO implement
    uint8_t support = 0;

    switch (JpegFormat) {
    case Format::JPEG_ENC_FORMAT_YUY2:
    case Format::JPEG_ENC_FORMAT_UYVY:
    case Format::JPEG_ENC_FORMAT_NV12:
    case Format::JPEG_ENC_FORMAT_NV21:
        support = 1;
        break;
    default:
        JPG_WARN("[WARN] jpeg does not support the fromat: %d\n", JpegFormat);
        break;
    }

    return support;
    //return uint8_t {};
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//IJpegEnc* HIDL_FETCH_IJpegEnc(const char* /* name */) {
//return new JpegEnc();
//}
//
}  // namespace implementation
}  // namespace V1_0
}  // namespace jpeg
}  // namespace hardware
}  // namespace mediatek
}  // namespace vendor
