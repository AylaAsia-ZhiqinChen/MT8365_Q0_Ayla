#include <mtkcam/utils/TuningUtils/FileDumpNamingRule.h>
#include "CommonRule.h"

namespace NSCam {
namespace TuningUtils {

void genFileName_LCSO(
    char *pFilename,
    int nFilename,
    const FILE_DUMP_NAMING_HINT *pHint,
    const char *pUserString)
{
    if (pUserString == NULL) pUserString = "";
    int t;
    char *ptr = pFilename;
    if (property_get_int32("vendor.debug.enable.normalAEB", 0))
        t = MakePrefix(ptr, nFilename, pHint->UniqueKey, pHint->FrameNo, pHint->RequestNo, pHint->EvValue);
    else
    t = MakePrefix(ptr, nFilename, pHint->UniqueKey, pHint->FrameNo, pHint->RequestNo);
    ptr += t;
    nFilename -= t;

    if (pHint->SensorDev >= 0) {
        t = snprintf(ptr, nFilename, "-%s", SENSOR_DEV_TO_STRING(pHint->SensorDev));
        ptr += t;
        nFilename -= t;
    }

    if (*(pHint->additStr) != '\0') {
        t = snprintf(ptr, nFilename, "-%s", pHint->additStr);
        ptr += t;
        nFilename -= t;
    }

    if (*pUserString != '\0') {
        t = snprintf(ptr, nFilename, "-%s", pUserString);
        ptr += t;
        nFilename -= t;
    }

    t = snprintf(ptr, nFilename, "-PW%d_PH%d_BW%d", pHint->BufWidth, pHint->BufHeight, pHint->BufStride);
    ptr += t;
    nFilename -= t;

    t = snprintf(ptr, nFilename, "__%dx%d_%s_%s",
        pHint->ImgWidth, pHint->ImgHeight,
        IMAGE_FORMAT_TO_BITS(pHint->ImgFormat),
        SENSOR_FORMAT_TO_STRING(pHint->SensorFormatOrder));

    t = snprintf(ptr, nFilename, ".lcso");
    ptr += t;
    nFilename -= t;
}

} //namespace FileDump
} //namespace NSCam

