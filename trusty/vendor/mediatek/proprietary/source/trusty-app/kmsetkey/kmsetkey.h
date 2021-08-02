/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#ifndef __KMSETKEY_H__
#define __KMSETKEY_H__

#include <sys/types.h>

#define KMSETKEY_PORT		"com.mediatek.kmsetkey"
#define MAX_MSG_SIZE		4096
#define IPC_MSG_SIZE		16
#define RESP_FLAG		0x80000000
#define DONE_FLAG		0x40000000

enum kmsetkey_cmd {
	KEY_LEN = 0x0,
	KEY_BUF = 0x1,
	SET_KEY = 0x2,
};

struct kmsetkey_msg {
	uint32_t cmd;
	uint8_t payload[0];
};

#endif
