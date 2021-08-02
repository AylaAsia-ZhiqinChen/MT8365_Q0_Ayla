#ifndef _TEE_SECURECLOCK_H_
#define _TEE_SECURECLOCK_H_

#include <tz_cross/trustzone.h>
#include <tz_cross/ta_mem.h>
#include <tz_cross/ta_securetime.h>
#include <utils/RefBase.h>
#include <utils/String8.h>
#include <utils/Log.h>

namespace DrmSecureTime
{

using namespace android;
using namespace std;

const String8 TimeFileSavePath("/data/vendor/securetime/SecureTD");

typedef unsigned long long  UINT64;
typedef unsigned int        UREE_SESSION_HANDLE;

class TEESecureClock : public RefBase
{
public:
    TEESecureClock ();
    ~TEESecureClock ();
    TZ_RESULT TEE_update_gb_time_intee ();
    TZ_RESULT TEE_update_gb_time_infile ();
    TZ_RESULT TEE_Icnt_time ();

private:
    UREE_SESSION_HANDLE icntSession;
};

}

#endif  //_TEE_SECURECLOCK_H_
