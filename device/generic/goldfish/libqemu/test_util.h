/*
 * Copyright (C) 2011 The Android Open Source Project
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
#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stddef.h>
#include "qemu_pipe.h"


double now_secs(void);

typedef struct {
    int  socket;
} Pipe;

int  pipe_openSocket( Pipe*  pipe, int port );
int  pipe_openQemuPipe( Pipe*  pipe, const char* pipename );
int  pipe_send( Pipe*  pipe, const void* buff, size_t  bufflen );
int  pipe_recv( Pipe*  pipe, void* buff, size_t bufflen );
void pipe_close( Pipe*  pipe );

#endif /* TEST_UTIL_H */
