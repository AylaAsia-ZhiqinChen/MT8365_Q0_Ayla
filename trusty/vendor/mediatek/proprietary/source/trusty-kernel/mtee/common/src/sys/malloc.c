/*
* Copyright (c) 2015 MediaTek Inc.
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

#include<malloc.h>
#include<sys/sys_malloc.h>
#include<tz_private/log.h>

/* FIX US -START */
void secure_meminfo(KERNELMEM_PARAM *outInfo)
{
}

void cm_meminfo(KERNELMEM_PARAM *outInfo)
{
}

uint32_t secure_memrank (void *buffer)
{
    return -1;
}
/* FIX US -END */

void *secure_malloc(uint32_t size)
{
	void *buffer;
	buffer = malloc(size);

	/* debug information */
	if (NULL == buffer)
	{
		MTEE_LOG(MTEE_LOG_LVL_BUG, "Error: fail to execute secure_malloc() (req: %d)\n", size );
	}

	MTEE_LOG(MTEE_LOG_LVL_INFO, "====> secure_malloc: %p\n", buffer);
	return buffer;
}

void *secure_calloc(uint32_t element, uint32_t elem_size)
{
	void *buffer;
	buffer = calloc(element, elem_size);

	/* debug information */
	if (NULL == buffer)
	{
		MTEE_LOG(MTEE_LOG_LVL_BUG, "Error: fail to execute secure_calloc() (req: %d)\n", element*elem_size );
	}

	MTEE_LOG(MTEE_LOG_LVL_INFO, "====> secure_calloc: %p\n", buffer);
	return buffer;
}

void *secure_memalign (uint32_t alignment, uint32_t size)
{
	void *buffer;
	
	buffer = memalign(alignment, size);
	
	/* debug information */
	if (NULL == buffer)
	{
        	MTEE_LOG(MTEE_LOG_LVL_BUG, "Error: fail to execute secure_malloc() (req: %d, alignment: %d)\n", size, alignment);
	}
	MTEE_LOG(MTEE_LOG_LVL_INFO, "====> secure_memalign: %p\n", buffer);

	return buffer;
}

void *secure_realloc(void *old_buffer, uint32_t size)
{
	void *buffer;
	buffer = realloc(old_buffer, size);

	/* debug information */
	if (NULL == buffer)
	{
		MTEE_LOG(MTEE_LOG_LVL_BUG, "Error: fail to execute secure_realloc() (req: %d)\n", size );
	}                                                                           
	MTEE_LOG(MTEE_LOG_LVL_INFO, "====> secure_realloc: %p\n", buffer);

	return buffer;
}

void secure_free(void *buffer)
{
	free(buffer);
	MTEE_LOG(MTEE_LOG_LVL_INFO, "====> secure_free: %p\n", buffer);
	return;
}

/* temporarily use secure instead of chunk */
void *cm_malloc(uint32_t size)
{
	return secure_malloc(size);
}

void *cm_memalign (uint32_t alignment, uint32_t size)
{
	return secure_memalign(alignment, size);
}

void *cm_calloc(uint32_t element, uint32_t elem_size)
{
	return secure_calloc(element, elem_size);
}

void *cm_realloc(void *old_buffer, uint32_t size)
{
	return secure_realloc(old_buffer, size);
}

void cm_free(void *buffer)
{
	return secure_free(buffer);
}

/* temporarily use secure instead of onchip */
void *onchip_malloc(uint32_t size)
{
	return secure_malloc(size);
}

void *onchip_memalign (uint32_t alignment, uint32_t size)
{
	return secure_memalign(alignment, size);
}

void *onchip_calloc(uint32_t element, uint32_t elem_size)
{
	return secure_calloc(element, elem_size);
}

void *onchip_realloc(void *old_buffer, uint32_t size)
{
	return secure_realloc(old_buffer, size);
}

void onchip_free(void *buffer)
{
	return secure_free(buffer);
}


#define DEBUG_HEAP 0
// structure placed at the beginning every allocation
struct alloc_struct_begin {
#if LK_DEBUGLEVEL > 1
	unsigned int magic;
#endif
	void *ptr;
	size_t size;
#if DEBUG_HEAP
	void *padding_start;
	size_t padding_size;
#endif
};

uint32_t secure_memsize (void *buffer)
{
	struct alloc_struct_begin *as;

	as = (struct alloc_struct_begin *)buffer;
	as--;

	return as->size;
}
