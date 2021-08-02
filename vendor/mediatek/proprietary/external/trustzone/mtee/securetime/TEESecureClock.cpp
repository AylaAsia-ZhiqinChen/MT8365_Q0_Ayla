#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <uree/system.h>
#include <uree/mem.h>
#include "TEESecureClock.h"

#undef LOG_TAG
#define LOG_TAG "TEESecureClock"

namespace DrmSecureTime
{

TEESecureClock::TEESecureClock ()
{
    if ( TZ_RESULT_SUCCESS != UREE_CreateSession( TZ_TA_SECURETIME_UUID, &icntSession ) )
    {
        ALOGE( "updateSecureTime CreateSession error\n" );
    }
}

TEESecureClock::~TEESecureClock ()
{
    UREE_CloseSession( icntSession );
}

TZ_RESULT TEESecureClock::TEE_update_gb_time_intee ()
{
    TZ_RESULT                       tz_ret          = TZ_RESULT_SUCCESS;
    TZ_RESULT                       file_result     = GB_TIME_FILE_OK_SIGN;
    size_t                          timeSize        = sizeof(struct TZ_GB_SECURETIME_INFO);
    uint32_t                        paramTypes      = 0;
    UINT64                          cur_counter     = 0;
    MTEEC_PARAM                     param[4];
    struct TZ_GB_SECURETIME_INFO    secure_time;
    memset( param, 0, sizeof(MTEEC_PARAM) * 4 );
    memset( &secure_time, 0, timeSize );

    ALOGD( "Enter  TEE_update_gb_time_intee.\n" );

    paramTypes = TZ_ParamTypes3( TZPT_MEM_INPUT, TZPT_MEM_INPUT, TZPT_VALUE_OUTPUT );
    param[0].mem.buffer = (void *) &secure_time;
    param[0].mem.size   = timeSize;
    param[1].mem.buffer = (void *) &cur_counter;
    param[1].mem.size   = sizeof(UINT64);

    int32_t fd = open( TimeFileSavePath.string(), O_RDWR );
    if ( fd >= 0 )
    {
        read( fd, (void *) &secure_time, timeSize );
        close( fd );
    }
    else
    {
        file_result = GB_NO_SECURETD_FILE;
        return file_result;
    }

#if DebugSecureTime
    for ( uint32_t i = 0; i < timeSize; i++ )
    {
        ALOGI( "%02x",((char*)&secure_time)[i] );
    }
    ALOGD( "\n" );
#endif

    cur_counter = (UINT64)time( NULL );

    tz_ret = UREE_TeeServiceCall( icntSession, TZCMD_SECURETIME_SET_CURRENT_COUNTER, paramTypes, param );
    if ( tz_ret != TZ_RESULT_SUCCESS )
    {
        ALOGE( "[securetime]ServiceCall TZCMD_SECURETIME_SET_CURRENT_COUNTER error: %s\n", TZ_GetErrorString( tz_ret ) );
        return tz_ret;
    }

    if ( param[2].value.a == GB_TIME_FILE_ERROR_SIGN )
    {
        file_result = GB_TIME_FILE_ERROR_SIGN;
        ALOGI( "[securetime]ServiceCall TZCMD_SECURETIME_SET_CURRENT_COUNTER file_result %d\n", file_result );
    }
    else if ( param[2].value.a == GB_TIME_FILE_OK_SIGN )
    {
        file_result = GB_TIME_FILE_OK_SIGN;
        ALOGI( "[securetime]ServiceCall TZCMD_SECURETIME_SET_CURRENT_COUNTER file_result %d\n", file_result );
    }

    return file_result;
}

TZ_RESULT TEESecureClock::TEE_update_gb_time_infile ()
{
    TZ_RESULT                       tz_ret          = TZ_RESULT_SUCCESS;
    size_t                          timeSize        = sizeof(struct TZ_GB_SECURETIME_INFO);
    uint32_t                        paramTypes      = 0;
    MTEEC_PARAM                     param[4];
    struct TZ_GB_SECURETIME_INFO    shm_p;
    memset( param, 0, sizeof(MTEEC_PARAM) * 4 );
    memset( &shm_p, 0, timeSize );

    ALOGD( "Enter  TEE_update_gb_time_infile.\n" );

    paramTypes = TZ_ParamTypes3( TZPT_MEM_OUTPUT, TZPT_VALUE_INPUT, TZPT_VALUE_OUTPUT );
    param[0].mem.buffer = (void *) &shm_p;
    param[0].mem.size   = timeSize;
    param[1].value.a    = 0;

    tz_ret = UREE_TeeServiceCall( icntSession, TZCMD_SECURETIME_GET_CURRENT_COUNTER, paramTypes, param );
    if ( tz_ret != TZ_RESULT_SUCCESS )
    {
        ALOGE("ServiceCall error %d\n", tz_ret);
        return tz_ret;
    }

    int32_t fd = open( TimeFileSavePath.string(), O_RDWR|O_TRUNC|O_CREAT, S_IRUSR|S_IWUSR|S_IXUSR );
    if ( fd >= 0 )
    {
        if ( write( fd, (void *)&shm_p, timeSize ) != (int32_t)timeSize )
        {
            ALOGE( "Write time file error.\n" );
        }
        close( fd );
    }
    else
    {
        ALOGE( "Open time file failed.\n" );
        return TZ_RESULT_ERROR_GENERIC;
    }

    return tz_ret;
}

TZ_RESULT TEESecureClock::TEE_Icnt_time ()
{
    TZ_RESULT                       tz_ret          = TZ_RESULT_SUCCESS;
    size_t                          timeSize        = sizeof(struct TM_GB);
    unsigned long                   inc_counter     = 0;
    uint32_t                        paramTypes      = 0;
    MTEEC_PARAM                     param[4];
    struct TM_GB                    shm_p;
    memset( param, 0, sizeof(MTEEC_PARAM) * 4 );
    memset( &shm_p, 0, timeSize );

    paramTypes = TZ_ParamTypes2(TZPT_VALUE_INPUT, TZPT_MEM_OUTPUT);
    param[1].mem.buffer = (void *) &shm_p;
    param[1].mem.size   = timeSize;

    inc_counter = time( NULL );
    param[0].value.a = inc_counter;

    tz_ret = UREE_TeeServiceCall( icntSession, TZCMD_SECURETIME_INC_CURRENT_COUNTER, paramTypes, param );
    if ( tz_ret != TZ_RESULT_SUCCESS )
    {
        ALOGE( "ServiceCall error %d\n", tz_ret );
        return tz_ret;
    }

#if DebugSecureTime
    ALOGI("%s,%d:securetime increase result: %d %d %d %d %d %d %d\n", __FUNCTION__, __LINE__,
                                                                      shm_p.tm_yday,
                                                                      shm_p.tm_year + 1900,
                                                                      shm_p.tm_mon + 1,
                                                                      shm_p.tm_mday,
                                                                      shm_p.tm_hour,
                                                                      shm_p.tm_min,
                                                                      shm_p.tm_sec);
#endif

    return tz_ret;
}

}
