#include <utils/Thread.h>
#include <utils/Mutex.h>
#include <utils/Condition.h>
#include "PicDetect.h"
#include "LogUtils.h"
#include "aw_portrait_bokeh.h"
#include "aw_portrait_bokeh_opengl.h"
#include "aw_portrait_common.h"
#include "aw_portrait_mask.h"
#include "BufferUtils.h"

#define LOG_TAG "Portrait/PicDetect"

using namespace NSCam::NSPipelinePlugin;
PicDetect::PicDetect() {
}

PicDetect::~PicDetect() {
    free(this -> inBufferCopy);
    free(this -> maskResultForPreview);
    if (mMasks != nullptr) {
        mMasks->release();
    }
}

void PicDetect::init() {
    FUNCTION_IN;
    if (this -> inBufferCopy == nullptr) {
        this -> inBufferCopy = (unsigned char *)malloc(this -> inSizeW * this -> inSizeH * 1.5);
    }
    if (this -> inBufferCopy != nullptr && this -> inBuffer != nullptr) {
        memcpy((void *)(this -> inBufferCopy),
            (void *)(this -> inBuffer), this -> inSizeW * this -> inSizeH * 1.5);
    }
    FUNCTION_OUT;
}

void PicDetect::uninit() {

}

bool PicDetect::processing() {
    if (mMasks == nullptr) {
        mMasks = new AwPortraitMask();
        char *dir = modelDir.data();
        mMasks->init(dir, 128, 320);
        mMasks->setLogable(true);
    }
    MY_LOGD("PicDetect send request inSize.w = %d,inSize.h = %d,orientation = %d", this -> inSizeW,
        this -> inSizeH, this -> orientation);
    mMasks->detectBuffer(this -> inBufferCopy,
            this -> inSizeW,
            this -> inSizeH,
            PIX_FMT_YUVI420,
            this -> orientation,
            this -> maskResultForPreview,
            MASK_WIDTH,
            MASK_HEIGHT);
    return true;
}