/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <lk/compiler.h>
#include <stdint.h>

__BEGIN_CDECLS

typedef long time_t;

/* For BoringSSL compatibility. */
struct tm {
        int tm_sec;         /* seconds */
        int tm_min;         /* minutes */
        int tm_hour;        /* hours */
        int tm_mday;        /* day of the month */
        int tm_mon;         /* month */
        int tm_year;        /* year */
        int tm_wday;        /* day of the week */
        int tm_yday;        /* day in the year */
        int tm_isdst;       /* daylight saving time */

        long int tm_gmtoff;     /* Seconds east of UTC.  */
        const char *tm_zone;    /* Timezone abbreviation.  */
};

static inline time_t time(time_t *t) {
    return 0;
}

/* Note: not POSIX signatures. */
long gettime(uint32_t clock_id, uint32_t flags, int64_t *time);
long nanosleep(uint32_t clock_id, uint32_t flags, uint64_t sleep_time);

__END_CDECLS
