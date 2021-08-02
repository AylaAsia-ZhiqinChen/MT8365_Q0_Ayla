#ifndef MSB_INC_H
#define MSB_INC_H
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MSB_SERVER		1
#define MSB_CLIENT		0
#define MSB_SERVER_MASK		0x1
#define MSB_INIT		0x2
#define MSB_INIT_MASK		0x2
#define MSB_PROCESS		0x4
#define MSB_THREAD		0
#define MSB_PROCESS_MASK		0x4

#define MSB_OBTAIN_FULL						0x1
#define MSB_OBTAIN_BLOCK					0x2
#define MSB_OBTAIN_NON_BLOCK			0x4

typedef void msb_handle_t;

typedef struct msb_buffer {
	union
	{
		void      *buf;
		uint8_t   *u8buf;
		int8_t    *s8buf;
		uint16_t  *u16buf;
		int16_t   *s16buf;
		uint32_t  *u32buf;
		int32_t   *s32buf;
	};
	size_t buf_size;
	size_t data_size;
}msb_buffer_t;



size_t msb_get_alloc_size(size_t size);
int msb_create(msb_handle_t **handle, void *shm, size_t size, int flag);
int msb_destroy(msb_handle_t *handle);
int msb_interrupt(msb_handle_t *handle);
int msb_obtain(msb_handle_t *handle, msb_buffer_t *buf, size_t req_size, int flag);
int msb_release(msb_handle_t *handle, const msb_buffer_t *buf);
size_t msb_get_remain_size(msb_handle_t *handle);
size_t msb_get_buffer_size(msb_handle_t *handle);
void *msb_get_start_address(msb_handle_t *handle);
int msb_reset_status(msb_handle_t *handle);

#ifdef __cplusplus
}
#endif
#endif