/*
 * Copyright (C) 2012 The Android Open Source Project
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

#ifndef _VOLDUTIL_H
#define _VOLDUTIL_H

#include <fstab/fstab.h>
#include <sys/cdefs.h>

extern struct fstab *fstab_default;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

void get_blkdev_size(int fd, unsigned long* nr_sec);

namespace android {
namespace os {
namespace IVold  {
const int STORAGE_FLAG_DE = 1;
const int STORAGE_FLAG_CE = 2;
}  // namespace IVold
}  // namespace os
}  // namespace android

#endif
