/*
 * Copyright (C) 2013-2015 The Android Open Source Project
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

#include <printf.h>
#include <stdio.h>
#include <string.h>
#include <trusty_syscalls.h>
#include <uapi/err.h>
#include <unistd.h>

#define LINE_BUFFER_SIZE 128

struct file_buffer {
	char data[LINE_BUFFER_SIZE];
	size_t pos;
};

struct file_context {
	io_handle_t io_handle;
	int fd;
	struct file_buffer *buffer;
};

static int buffered_put(struct file_buffer *buffer, int fd, char c)
{
	int result = 0;

	buffer->data[buffer->pos++] = c;
	if (buffer->pos == sizeof(buffer->data) || c == '\n') {
		result = _trusty_write(fd, buffer->data, buffer->pos);
		buffer->pos = 0;
	}
	return result;
}

static ssize_t buffered_write(struct io_handle *handle, const char *str, size_t sz)
{
	unsigned i;
	struct file_context *ctx = containerof(handle, struct file_context,
	                                       io_handle);

	if (!ctx->buffer) {
		return ERR_INVALID_ARGS;
	}

	for (i = 0; i < sz; i++) {
		int result = buffered_put(ctx->buffer, ctx->fd, str[i]);
		if (result < 0) {
			return result;
		}
	}

	return sz;
}

static io_handle_hooks_t __stdio_io_handle_hooks = {
	.write = buffered_write,
};

struct file_buffer stdout_buffer = {.pos = 0};
struct file_buffer stderr_buffer = {.pos = 0};
struct file_context fctx[3] = {
	{
		.io_handle = IO_HANDLE_INITIAL_VALUE(&__stdio_io_handle_hooks),
		.fd = 0,
		.buffer = NULL,
	},
	{
		.io_handle = IO_HANDLE_INITIAL_VALUE(&__stdio_io_handle_hooks),
		.fd = 1,
		.buffer = &stdout_buffer,
	},
	{
		.io_handle = IO_HANDLE_INITIAL_VALUE(&__stdio_io_handle_hooks),
		.fd = 2,
		.buffer = &stderr_buffer,
	},
};

#define DEFINE_STDIO_DESC(fctx)					\
	{							\
		.io		= &((fctx)->io_handle),		\
	}

FILE __stdio_FILEs[3] = {
	DEFINE_STDIO_DESC(&fctx[0]), /* stdin */
	DEFINE_STDIO_DESC(&fctx[1]), /* stdout */
	DEFINE_STDIO_DESC(&fctx[2]), /* stderr */
};
#undef DEFINE_STDIO_DESC
