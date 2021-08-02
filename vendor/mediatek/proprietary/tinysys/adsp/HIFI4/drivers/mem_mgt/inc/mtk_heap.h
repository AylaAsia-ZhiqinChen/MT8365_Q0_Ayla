/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef __H_MTK_HEAP_H__
#define __H_MTK_HEAP_H__

#include "FreeRTOS.h"
#include "mtk_heap_type.h"
#include "main.h"



/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE		( ( size_t ) 8 )

#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
#define MTK_HEAP_GUARD_PATTERN		0xabcdabcd
#endif

/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK
{
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
	uint32_t ulGuard;
#endif
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
	uint32_t ulCallerAddr;
#endif
} BlockLink_t;

typedef struct MTK_HeapRegion
{
	uint8_t *pucStartAddress;
	size_t xSizeInBytes;
	MTK_eMemoryType eMemoryType;
	const char* pcName;
} MTK_HeapRegion_t;

typedef struct MTK_Heap
{
	MTK_HeapRegion_t *pxHeapRegion;
	BlockLink_t xStart;
	BlockLink_t *pxEnd;
	size_t xFreeBytesRemaining;
	size_t xMinimumEverFreeBytesRemaining;
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
	BlockLink_t xUsedStart;
	BlockLink_t *pxUsedEnd;
#endif
} MTK_Heap_t;


portBASE_TYPE MTK_xGetHeapSize( MTK_eMemoryType eMemoryType );
void *MTK_pvPortMalloc( size_t xWantedSize, MTK_eMemoryType eMemoryType );
void MTK_vPortFree( void *pv );
void *MTK_pvPortCalloc( size_t nmemb, size_t size, MTK_eMemoryType eMemoryType );
void *MTK_pvPortRealloc( void *ptr, size_t size, MTK_eMemoryType eMemoryType );
size_t MTK_xPortGetFreeHeapSize( MTK_eMemoryType eMemoryType );
size_t MTK_xPortGetMinimumEverFreeHeapSize( MTK_eMemoryType eMemoryType );
void MTK_vPortDefineHeapRegions( const MTK_HeapRegion_t * const pxHeapRegions, size_t xHeapCount );
void MTK_vDumpHeapStatus( void );
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
BaseType_t MTK_xPortCheckHeap( MTK_eMemoryType eMemoryType );
void MTK_vPortPrintHeapList( MTK_eMemoryType eMemoryType );
#else
static inline BaseType_t MTK_xPortCheckHeap( MTK_eMemoryType eMemoryType )
{
	return 0;
}

static inline void MTK_vPortPrintHeapList( MTK_eMemoryType eMemoryType )
{
}
#endif
#endif /* __H_MTK_HEAP_H__ */

