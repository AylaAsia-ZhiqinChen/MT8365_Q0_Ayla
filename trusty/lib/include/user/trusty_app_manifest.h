/*
 * Copyright (c) 2013, Google, Inc. All rights reserved
 * Copyright (c) 2012-2013, NVIDIA CORPORATION. All rights reserved
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __LIB_TRUSTY_APP_MANIFEST_H
#define __LIB_TRUSTY_APP_MANIFEST_H

#include <sys/types.h>

#include "trusty_uuid.h"

/*
 * Layout of .trusty_app.manifest section in the trusted application is the
 * required UUID followed by an abitrary number of configuration options.
 */
typedef struct trusty_app_manifest {
	uuid_t uuid;
	uint32_t config_options[];
} trusty_app_manifest_t;


struct trusty_app_manifest_entry_port {
	uint32_t config_key;
	uint32_t port_flags;
	uint32_t port_name_size;
	char port_name[];
};

#define CONCAT(x, y) x##y
#define XCONCAT(x, y) CONCAT(x, y)

enum {
	TRUSTY_APP_CONFIG_KEY_MIN_STACK_SIZE	= 1,
	TRUSTY_APP_CONFIG_KEY_MIN_HEAP_SIZE	= 2,
	TRUSTY_APP_CONFIG_KEY_MAP_MEM		= 3,
	TRUSTY_APP_CONFIG_KEY_MGMT_FLAGS	= 4,
	TRUSTY_APP_CONFIG_KEY_START_PORT	= 5,
};

enum trusty_app_mgmt_flags {
	TRUSTY_APP_MGMT_FLAGS_NONE			= 0x0,
	/* Restart the application on exit */
	TRUSTY_APP_MGMT_FLAGS_RESTART_ON_EXIT		= 0x1,
	/* Don't start the application at boot */
	TRUSTY_APP_MGMT_FLAGS_DEFERRED_START		= 0x2,
};

#define TRUSTY_APP_CONFIG_MIN_STACK_SIZE(sz) \
	TRUSTY_APP_CONFIG_KEY_MIN_STACK_SIZE, sz

#define TRUSTY_APP_CONFIG_MIN_HEAP_SIZE(sz) \
	TRUSTY_APP_CONFIG_KEY_MIN_HEAP_SIZE, sz

#define TRUSTY_APP_CONFIG_MAP_MEM(id,off,sz) \
	TRUSTY_APP_CONFIG_KEY_MAP_MEM, id, off, sz

#define TRUSTY_APP_CONFIG_MGMT_FLAGS(mgmt_flags) \
	TRUSTY_APP_CONFIG_KEY_MGMT_FLAGS, mgmt_flags

/* Valid flags: IPC_PORT_ALLOW_* */
#define TRUSTY_APP_START_PORT(name, flags) \
	struct trusty_app_manifest_entry_port \
		__attribute((section(".trusty_app.manifest.entry"))) \
		XCONCAT(trusty_app_manifest_entry_port_, __COUNTER__) = { \
			.config_key = TRUSTY_APP_CONFIG_KEY_START_PORT, \
			.port_flags = flags, \
			.port_name_size = sizeof(name), \
			.port_name = name, \
		};

/* manifest section attributes */
#define TRUSTY_APP_MANIFEST_ATTRS \
	const __attribute((aligned(4)))\
		__attribute((section(".trusty_app.manifest")))

#endif

