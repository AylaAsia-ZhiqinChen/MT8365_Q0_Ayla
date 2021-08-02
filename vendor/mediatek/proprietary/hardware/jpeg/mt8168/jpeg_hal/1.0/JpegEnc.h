#ifndef VENDOR_MEDIATEK_HARDWARE_JPEG_V1_0_JPEGENC_H
#define VENDOR_MEDIATEK_HARDWARE_JPEG_V1_0_JPEGENC_H

#include <vendor/mediatek/hardware/jpeg/1.0/IJpegEnc.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace mediatek {
namespace hardware {
namespace jpeg {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct JpegEnc : public IJpegEnc {
    // Methods from ::vendor::mediatek::hardware::jpeg::V1_0::IJpegEnc follow.
    Return<uint32_t> startJpegEnc(const ::vendor::mediatek::hardware::jpeg::V1_0::JpegEncParam &parameter) override;
    Return<uint8_t> queryJpegEncFormat(::vendor::mediatek::hardware::jpeg::V1_0::Format JpegFormat) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" IJpegEnc* HIDL_FETCH_IJpegEnc(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace jpeg
}  // namespace hardware
}  // namespace mediatek
}  // namespace vendor

#endif  // VENDOR_MEDIATEK_HARDWARE_JPEG_V1_0_JPEGENC_H
