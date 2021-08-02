#ifndef PicDetect_H_
#define PicDetect_H_
#include <stdint.h>
#include <vector>
#include <string>
#include <utils/Thread.h>
#include "aw_portrait_bokeh.h"
#include "aw_portrait_bokeh_opengl.h"
#include "aw_portrait_common.h"
#include "aw_portrait_mask.h"
#include <plugin/PipelinePluginType.h>
#include "ProcessingThread.h"

class PicDetect: public Request {
    public:
        unsigned char* inBuffer = nullptr;
        unsigned char* inBufferCopy = nullptr;
        aw_face_orientation orientation = FACE_LEFT;
        int inSizeW = 0;
        int inSizeH = 0;
        static const int MASK_WIDTH = 128;
        static const int MASK_HEIGHT = 128;
        unsigned char* maskResultForPreview = (unsigned char *)malloc(MASK_WIDTH * MASK_HEIGHT * 1.5);

        PicDetect();
        ~PicDetect();
        virtual void init();
        virtual void uninit();
        virtual bool processing();
    protected:
        string modelDir = "/system/etc/modeldir";
        AwPortraitMask *mMasks = nullptr;
};

#endif /* PicDetect_H_ */