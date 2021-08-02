#include <errno.h>
#include <sys/select.h>
#include "TEESecureClock.h"

#undef LOG_TAG
#define LOG_TAG "DrmSecureTime"

using namespace DrmSecureTime;

int main(void)
{
    const int           CountFreq                   = 5;
    const int           SaveFileFreq                = 10 * 60; // store secure time per 10 minutes
    TZ_RESULT           update_ret                  = TZ_RESULT_SUCCESS;
    unsigned int        check_count                 = 0;
    struct timeval      delay;
    TEESecureClock      secureClock;

    update_ret = secureClock.TEE_update_gb_time_intee ();
    if ( update_ret == GB_NO_SECURETD_FILE || update_ret == GB_TIME_FILE_ERROR_SIGN )
    {
        secureClock.TEE_update_gb_time_infile ();
        secureClock.TEE_update_gb_time_intee ();
    }

    while ( true )
    {
        delay.tv_sec  = CountFreq;
        delay.tv_usec = 0;
        while ( select( 0, NULL, NULL, NULL, &delay ) < 0 && errno == EINTR );

        secureClock.TEE_Icnt_time();
        check_count += CountFreq;

        if ( check_count >= SaveFileFreq )
        {
            secureClock.TEE_update_gb_time_infile();
            check_count = 0;
        }
    }

    return 0;
}
