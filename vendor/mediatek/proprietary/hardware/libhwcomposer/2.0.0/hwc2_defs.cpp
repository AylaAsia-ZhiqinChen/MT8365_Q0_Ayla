#include "hwc2_defs.h"

#include "utils/tools.h"

const char* HWC2_PRESENT_VALI_STATE_PRESENT_DONE_STR = "PD";
const char* HWC2_PRESENT_VALI_STATE_CHECK_SKIP_VALI_STR = "CSV";
const char* HWC2_PRESENT_VALI_STATE_VALIDATE_STR = "V";
const char* HWC2_PRESENT_VALI_STATE_VALIDATE_DONE_STR = "VD";
const char* HWC2_PRESENT_VALI_STATE_PRESENT_STR = "P";
const char* HWC2_PRESENT_VALI_STATE_UNKNOWN_STR = "UNK";

const char* getPresentValiStateString(const HWC_VALI_PRESENT_STATE& state)
{
    switch(state)
    {
        case HWC_VALI_PRESENT_STATE_PRESENT_DONE:
            return HWC2_PRESENT_VALI_STATE_PRESENT_DONE_STR;

        case HWC_VALI_PRESENT_STATE_CHECK_SKIP_VALI:
            return HWC2_PRESENT_VALI_STATE_CHECK_SKIP_VALI_STR;

        case HWC_VALI_PRESENT_STATE_VALIDATE:
            return HWC2_PRESENT_VALI_STATE_VALIDATE_STR;

        case HWC_VALI_PRESENT_STATE_VALIDATE_DONE:
            return HWC2_PRESENT_VALI_STATE_VALIDATE_DONE_STR;

        case HWC_VALI_PRESENT_STATE_PRESENT:
            return HWC2_PRESENT_VALI_STATE_PRESENT_STR;

        default:
            HWC_LOGE("%s unknown state:%d", __func__, state);
            return HWC2_PRESENT_VALI_STATE_UNKNOWN_STR;
    }
}
