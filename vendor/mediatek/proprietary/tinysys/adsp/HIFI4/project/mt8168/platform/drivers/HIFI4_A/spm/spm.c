#include <mt_reg_base.h>
#include <driver_api.h>
#include "spm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "systimer.h"

static int spm_setting;

int spm_res_set(int req)
{
    int last = spm_setting;
    int ret = 0;
    unsigned long long time_start, time_end;

    if (spm_setting == req)
        return ret;

    DRV_WriteReg32(DSP_AUDIO_DSP2SPM_INT, req);
    spm_setting = req;
    /* some resource release, do not need to check result */
    if (req < last)
        return ret;

    /* check if spm ack is right */
    time_start = read_systimer_stamp_ns();
    while((DRV_Reg32(DSP_AUDIO_DSP2SPM_INT_ACK) & req) != req) {
       vTaskDelay(1/portTICK_PERIOD_MS);

       time_end = read_systimer_stamp_ns();
       /* if wait time over 10ms, trigger timeout error */
       if (time_end - time_start > 6000000) {
           ret = -1;
           PRINTF_E("Warnning spm request wait(%x) timeout:%llu\n", req, (time_end - time_start));
           break;
       }
    }

    return ret;
}

int spm_res_get(void)
{
    return spm_setting;
}
