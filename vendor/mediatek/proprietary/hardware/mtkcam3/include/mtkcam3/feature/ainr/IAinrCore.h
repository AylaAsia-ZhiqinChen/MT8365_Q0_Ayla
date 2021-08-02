#ifndef __IAINRCORE_H__
#define __IAINRCORE_H__

#include "AinrDefs.h"
#include "AinrTypes.h"
#include "IAinrNvram.h"
#include <utils/RefBase.h> // android::RefBase

#include <memory>
#include <vector>
#include <string>
#include <deque> // std::deque


using android::sp;
using std::vector;

namespace ainr {

class IAinrCore {
public:
    /**
     *  Morden AINR supports some derived mutli-frame features. For each core
     *  version, these derived features may be supported.
     */
    enum class Type
    {
        DEFAULT     = 0,
        // Default use MFNR
        AINR        = DEFAULT,
        // AIS equalis to MFNR (until now)
        AIS         = AINR,
    };

public:
    /**
     *  Caller should always create instance via INTERFACE::createInstance method.
     *  And caller also have responsibility to destroy it by invoking destroyInstance.
     *  If caller uses android::sp to manager life cycle of instance who doesn't need to invoke
     *  destroyInstance to destroy instance.
     *
     *  @param major        - Major version, if the value is or smaller to 0, use default.
     *  @param minor        - Minor version, if the value is or smaller to 0, use default.
     *  @param featured     - Featured version.
     *  @return             - An IAinrCore instance, caller has responsibility to manager it's lifetime.
     */
    static std::shared_ptr<IAinrCore> createInstance(
                          int  major    = 0,
                          int  minor    = 0,
                          Type featured = Type::DEFAULT
                      );

public:
    /**
     *  Init AinrCore, if using MrpMode_BestPerformance, a thread will be create
     *  to allocate memory chunks, notice that, this thread will be joined while
     *  destroying AinrCore.
     *  @param cfg          - AinrConfig_t structure to describe usage
     *  @return             - Returns AinrErr_Ok if works
     */
    virtual enum AinrErr init (const AinrConfig_t &cfg) = 0;

    virtual enum AinrErr doAinr() = 0;

    /**
     *  Tell MFLL to cancel this operation
     */
    virtual enum AinrErr doCancel() = 0;

    /**
     *  User may also set a Nvram Provider to AinrCore. If Nvram Provider hasn't been set,
     *  AinrCore will invoke IAinrNvram::createInstance to create IAinrNvram instance when need.
     *
     *  @param nvramProvider - A strong pointer contains reference of IAinrNvram instance.
     *  @return             - If ok returns AinrErr_Ok
     *  @note               - This operation is thread-safe
     */
    virtual enum AinrErr setNvramProvider(const std::shared_ptr<IAinrNvram> &nvramProvider) = 0;


    /**
     *  addd buffer & metadata to ainr core
     *  @param inputPack    - Structure of buffers and metadata
     *  @return             - If OK returns AinrErr_Ok.
     */
     virtual enum AinrErr addInput(
            const std::vector<AinrPipelinePack>&     inputPack
            ) = 0;

    /**
     *  addd outputbuffer ainr core
     *  @param output    - Structure of buffers and metadata
     *  @return             - If OK returns AinrErr_Ok.
     */
     virtual enum AinrErr addOutput(
            NSCam::IImageBuffer *outBuffer
            ) = 0;

    /**
     *  Query whether algo is supported for specific resolution
     *  @NSCam::MSize size  - Input image size
     *  @return             - If support returns true.
     */
     virtual bool queryAlgoSupport(NSCam::MSize size) = 0;

     virtual void registerCallback(std::function<void(MINT32)> cb) = 0;

protected:
    virtual ~IAinrCore(void) {};

}; /* class IAinrCore */
}; /* namespace ainr */
#endif

