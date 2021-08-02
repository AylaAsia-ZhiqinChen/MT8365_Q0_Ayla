#include "AudioALSACaptureHandlerNull.h"
#include "AudioType.h"
#include "AudioUtility.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "AudioALSACaptureHandlerNull"

namespace android
{

AudioALSACaptureHandlerNull::AudioALSACaptureHandlerNull(stream_attribute_t *stream_attribute_target) :
    AudioALSACaptureHandlerBase(stream_attribute_target)
{
    ALOGD("%s()", __FUNCTION__);

    init();
}

AudioALSACaptureHandlerNull::~AudioALSACaptureHandlerNull()
{
    ALOGD("%s()", __FUNCTION__);
}


status_t AudioALSACaptureHandlerNull::init()
{
    ALOGD("%s()", __FUNCTION__);
    mBytesPerSecond = mStreamAttributeTarget->sample_rate * mStreamAttributeTarget->num_channels *
                      audio_bytes_per_sample(mStreamAttributeTarget->audio_format);
    return NO_ERROR;
}

status_t AudioALSACaptureHandlerNull::open()
{
    ALOGD("%s(), input_device = 0x%x, input_source = 0x%x",
          __FUNCTION__, mStreamAttributeTarget->input_device, mStreamAttributeTarget->input_source);

    return NO_ERROR;
}

status_t AudioALSACaptureHandlerNull::close()
{
    ALOGD("%s()", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSACaptureHandlerNull::routing(const audio_devices_t input_device)
{
    ALOGD("%s() NOT SUPPORT input_device %d", __FUNCTION__, input_device);
    return INVALID_OPERATION;
}

ssize_t AudioALSACaptureHandlerNull::read(void *buffer, ssize_t bytes)
{
    ALOGV("%s()", __FUNCTION__);

    memset(buffer, 0, bytes);

    usleep((bytes * 1000 / mBytesPerSecond) * 1000);

    return bytes;
}

} // end of namespace android
