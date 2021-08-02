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
#include <stddef.h>

__BEGIN_CDECLS
void *malloc(size_t n) __MALLOC __WARN_UNUSED_RESULT;
void *calloc(size_t n_elements, size_t element_size) __WARN_UNUSED_RESULT;
void *realloc(void *p, size_t n) __WARN_UNUSED_RESULT;
void free(void *p);
void *memalign(size_t alignment, size_t n) __MALLOC __WARN_UNUSED_RESULT;
__END_CDECLS

