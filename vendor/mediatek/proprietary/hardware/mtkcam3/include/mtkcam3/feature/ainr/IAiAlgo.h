#ifndef __IAIALGO_H__
#define __IAIALGO_H__

#include "AinrDefs.h"
#include "AinrTypes.h"

#include <mtkcam3/feature/ainr/IAinrNvram.h>

// STL
#include <memory>
#include <utility>  // std::pair

namespace ainr {

class IAiAlgo {
public:
    static std::shared_ptr<IAiAlgo> createInstance(AinrFeatureType type);
    virtual ~IAiAlgo() = default;
/* interfaces */
public:
    /* Init aiAlgo with nvram.
     *  @param nvram               A trunk of memory contains
     *                             tuning data used by AI-Multiframe(AINR/AIHDR) feature.
     */
    virtual void init(std::shared_ptr<IAinrNvram> nvram) = 0;

    /* Do homography calculation. Thread safe method.
     *  @param params               HG parm with FE/FM as input and gridMap
     *                              as output.
     */
    virtual enum AinrErr doHG(const AiHgParam &params) = 0;

    /* Do reference unpack. Unpack packed raw (which warped) to unpacked and channel based.
     * Thread safe method.
     *  @AiParam params             AiParam indicate basic buffer information.
     *  @AiRefPackage bufPackage   Input/Output buffers.
     */
    virtual enum AinrErr refBufUpk(const AiParam &params, const AiRefPackage &bufPackage) = 0;

    /* Do base frame unpack. Unpack packed raw to unpacked and channel based. Thread safe method.
     *  @AiParam params             AiParam indicate basic buffer information.
     *  @AiBasePackage bufPackage   Input/Output buffers.
     */
    virtual enum AinrErr baseBufUpk(const AiParam &params, const AiBasePackage &bufPackage) = 0;

    /* Do APU processing. Use AI to do post-processing.
     *  @AiParam params             AiParam indicate basic buffer information.
     *  @AiCoreParam coreParam      AiCoreParam specify algo mdla requirement.
     */
    virtual enum AinrErr doNrCore(const AiParam &params, const AiCoreParam &coreParam) = 0;

    /* Do dynamic range compress processing
     *  @NSCam::IImageBuffer inBuf/outBuf input/output buffer.
     *  @NSCam::IMetadata appMeta/halMeta app and hal metadata.
     */
    virtual enum AinrErr doDRC(NSCam::IImageBuffer* rawBuf, NSCam::IMetadata* appMeta
                                    , NSCam::IMetadata* halMeta, NSCam::IMetadata* dynamicMeta) = 0;
};
}; /* namespace ainr */

#endif//__IAIALGO_H__