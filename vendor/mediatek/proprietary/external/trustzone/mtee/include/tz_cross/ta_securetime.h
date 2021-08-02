#ifndef __TRUSTZONE_TA_SECURE_TIMER__
#define __TRUSTZONE_TA_SECURE_TIMER__


#define TZ_TA_SECURETIME_UUID         "b25bf100-d276-11e2-9c9c-9c9c9c9c9c9c"


#define TZ_SECURETIME_BIRTHDATE                         1392967151
#define TZCMD_SECURETIME_GET_CURRENT_COUNTER            0x00000000  // used for getting encrypted prtime struct to save file when shutdown and suspend or after THREAD_SAVEFILE_VALUE second
#define TZCMD_SECURETIME_SET_CURRENT_COUNTER            0x00000001  //  used for set new playready time using the current rtc counter and encrypted saved prtime struct when resume and bootup
#define TZCMD_SECURETIME_INC_CURRENT_COUNTER            0x00000002  //  used for increase current counter at least PR_TIME_INC_COUNTER secs and no more than PR_TIME_MAX_COUNTER_OFFSET secs
#define TZCMD_SECURETIME_SET_CURRENT_PRTIME             0x00000003   // used for network time-sync module to sync pr_time
#define GB_TIME_INC_COUNTER                             5
#define GB_TIME_MAX_COUNTER_OFFSET                      8

#define GB_TIME_FILE_BASE                               50000
#define GB_TIME_FILE_ERROR_SIGN                         (GB_TIME_FILE_BASE + 1)
#define GB_TIME_FILE_OK_SIGN                            (GB_TIME_FILE_BASE + 2)
#define GB_NO_SECURETD_FILE                             (GB_TIME_FILE_BASE + 3)
#define GB_TIME_ERROR_SETPRTIME                         (GB_TIME_FILE_BASE + 4)


typedef struct TZ_GB_SECURETIME_INFO{
    volatile uint64_t hwcounter;
    volatile uint64_t gb_time;
    uint8_t signature[8];
} TZ_GB_SECURETIME_INFO;

typedef struct TM_GB {
   int     tm_sec;         /* seconds */
   int     tm_min;         /* minutes */
   int     tm_hour;        /* hours */
   int     tm_mday;        /* day of the month */
   int     tm_mon;         /* month */
   int     tm_year;        /* year */
   int     tm_wday;        /* day of the week */
   int     tm_yday;        /* day in the year */
   int     tm_isdst;       /* daylight saving time */

   long int tm_gmtoff;     /* Seconds east of UTC.  */
   const char *tm_zone;    /* Timezone abbreviation.  */
} TM_GB;


uint64_t GetTee_SecureTime(); //only be used in tee, in user or kernel, should call the tee_service call.
void SetTee_SecureTime(uint64_t newtime);  //only be used in tee, and just beused to update TEE time from reliable server.

#endif /* __TRUSTZONE_TA_PLAYREADY__ */

