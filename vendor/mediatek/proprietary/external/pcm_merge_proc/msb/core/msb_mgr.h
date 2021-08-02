#ifndef MSB_MGR_H
#define MSB_MGR_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include "../inc/msb_inc.h"

#define CBLK_SERVER_INVALID		0x01
#define CBLK_CLIENT_INVALID		0x02
#define CBLK_SERVER_WAIT		0x4
#define CBLK_CLIENT_WAIT		0x8
#define CBLK_SERVER_WAKE		0x10
#define CBLK_CLIENT_WAKE		0x20
#define CBLK_SERVER_INTERRUPT		0x40
#define CBLK_CLIENT_INTERRUPT		0x80


typedef struct _msb_cblk {
	size_t wptr;
	size_t rptr;
	size_t buf_size;
	size_t data_size;
	volatile int  state;
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutexattr;
	pthread_cond_t  cond;
	pthread_condattr_t condattr;
}msb_cblk_t;

typedef struct _msb_mgr {
	uint8_t *base;
	uint8_t *end;
	msb_cblk_t *cblk;
	int server;
	int init;
	int range;
}msb_mgr_t;

#endif