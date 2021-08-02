#ifndef __IAINRFEFM_H__
#define __IAINRFEFM_H__

#include "AinrDefs.h"
#include "AinrTypes.h"

// STL
#include <memory>
//
/**
 *  IFeFm has responsibility handle two input buffers (Base frame and referenced frame)
 *  matched data. Ususally used by algorithm to do homography postprocessing. IFeFm output
 *  data FMO would indicate matching point of two frames. Be aware of that FMO is confidential
 *  format we should parsing result by algorithm before using it.
 */
using namespace NSCam;

namespace ainr {
class IAinrFeFm {
public:
    struct ConfigParams
    {
        int     openId;
        MSize   bufferSize;
        int     ispProfile;
        int     captureNum;
        int     index;
        std::function<void(int)> cbMethod;
        ConfigParams()
            : openId(0)
            , bufferSize(MSize())
            , ispProfile(0)
            , captureNum(0)
            , index(0)
        {};
        ConfigParams(int sensorId)
            : openId(sensorId)
            , bufferSize(MSize())
            , ispProfile(0)
            , captureNum(0)
            , index(0)
        {};
    };
    //
    struct BufferPackage
    {
        IImageBuffer *inBase;
        IImageBuffer *inRef;
        IImageBuffer *outFeoBase;
        IImageBuffer *outFeoRef;
        IImageBuffer *outFmoBase;
        IImageBuffer *outFmoRef;
        BufferPackage()
            : inBase(nullptr)
            , inRef(nullptr)
            , outFeoBase(nullptr)
            , outFeoRef(nullptr)
            , outFmoBase(nullptr)
            , outFmoRef(nullptr)
            {};
    };
    struct MetaDataPackage
    {
        // Metadata base frame
        IMetadata* appMetaBase;
        IMetadata* halMetaBase;
        IMetadata* dynamicMetaBase; //p1node output app meta
        // Metadata ref frame
        IMetadata* appMetaRef;
        IMetadata* halMetaRef;
        IMetadata* dynamicMetaRef; //p1node output app meta
        MetaDataPackage()
            : appMetaBase(nullptr)
            , halMetaBase(nullptr)
            , dynamicMetaBase(nullptr)
            , appMetaRef(nullptr)
            , halMetaRef(nullptr)
            , dynamicMetaRef(nullptr)
            {};
    };
    struct DataPackage
    {
        // Metadata base frame
        IMetadata    *appMeta;
        IMetadata    *halMeta;
        IImageBuffer *inBuf;
        IImageBuffer *outBuf;
        // It is used for bittrue and it is obtional
        IImageBuffer *outYuv;
        DataPackage()
            : appMeta(nullptr)
            , halMeta(nullptr)
            , inBuf(nullptr)
            , outBuf(nullptr)
            , outYuv(nullptr)
            {};
    };
public:
    static std::shared_ptr<IAinrFeFm> createInstance(void);

/* interfaces */
public:

    /* Feed config data into FEFM.
     *  @param params               There're different NVRAM settings in the
     *                              different params.
     */
    virtual enum AinrErr init(IAinrFeFm::ConfigParams const& params) = 0;
    /* Get FEFM output buffer size. Should be called after init.
     *  @param inputBuffer          Input buffer size
     *  @output                     feo size
     *
     */
    virtual enum AinrErr getFeFmSize(MSize &feoSize, MSize &fmoSize) = 0;

    /* Returns work succed or not.
     *  @param [out] DataPackage    pass in/out and meta
     *                              null.
     *  @return                     Address of the buffer, if read fail returns
     *                              null.
     */
    virtual enum AinrErr buildFe(IAinrFeFm::DataPackage *package) = 0;

    /* Returns work succed or not.
     *  @param [out] DataPackage    pass in/out and meta
     *                              null.
     *  @return                     Address of the buffer, if read fail returns
     *                              null.
     */
    virtual enum AinrErr buildFm(IImageBuffer *baseBuf, IImageBuffer *refBuf, IImageBuffer *fmo, int index, bool needCb) = 0;

    /* Returns work succed or not.
     *  @param [out] DataPackage    pass in/out and meta
     *                              null.
     *  @return                     Address of the buffer, if read fail returns
     *                              null.
     */
    virtual enum AinrErr doFeFm() = 0;
protected:
    virtual ~IAinrFeFm() { };
};
}; /* namespace ainr */

#endif//__IAINRFEFM_H__