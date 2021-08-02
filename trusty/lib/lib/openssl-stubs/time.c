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

#include <time.h>

#include <string.h>

struct tm *OPENSSL_gmtime(const time_t *timer, struct tm *result) {
        memset(result, 0, sizeof(*result));
	/* A valid tm_mday is in range of [1, 31]. Fix for keymaster CTS */
        result->tm_mday = 1;

        return result;
}

int OPENSSL_gmtime_adj(struct tm *tm, int offset_day, long offset_sec) {
        return 0;
}
