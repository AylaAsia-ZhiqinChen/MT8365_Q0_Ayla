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

/*
 * A sample implementation of pvPortMalloc() that allows the heap to be defined
 * across multiple non-contigous blocks and combines (coalescences) adjacent
 * memory blocks as they are freed.
 *
 * See heap_1.c, heap_2.c, heap_3.c and heap_4.c for alternative
 * implementations, and the memory management pages of http://www.FreeRTOS.org
 * for more information.
 *
 * Usage notes:
 *
 * vPortDefineHeapRegions() ***must*** be called before pvPortMalloc().
 * pvPortMalloc() will be called if any task objects (tasks, queues, event
 * groups, etc.) are created, therefore vPortDefineHeapRegions() ***must*** be
 * called before any other objects are defined.
 *
 * vPortDefineHeapRegions() takes a single parameter.  The parameter is an array
 * of HeapRegion_t structures.  HeapRegion_t is defined in portable.h as
 *
 * typedef struct HeapRegion
 * {
 *	uint8_t *pucStartAddress; << Start address of a block of memory that will be part of the heap.
 *	size_t xSizeInBytes;	  << Size of the block of memory.
 * } HeapRegion_t;
 *
 * The array is terminated using a NULL zero sized region definition, and the
 * memory regions defined in the array ***must*** appear in address order from
 * low address to high address.  So the following is a valid example of how
 * to use the function.
 *
 * HeapRegion_t xHeapRegions[] =
 * {
 * 	{ ( uint8_t * ) 0x80000000UL, 0x10000 }, << Defines a block of 0x10000 bytes starting at address 0x80000000
 * 	{ ( uint8_t * ) 0x90000000UL, 0xa0000 }, << Defines a block of 0xa0000 bytes starting at address of 0x90000000
 * 	{ NULL, 0 }                << Terminates the array.
 * };
 *
 * vPortDefineHeapRegions( xHeapRegions ); << Pass the array into vPortDefineHeapRegions().
 *
 * Note 0x80000000 is the lower address so appears in the array first.
 *
 */
#include <stdlib.h>
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#include "mtk_heap.h"
#include "heap_4_debug.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
	#error This file must not be used if configSUPPORT_DYNAMIC_ALLOCATION is 0
#endif

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( xHeapStructSize << 1 ) )

/*-----------------------------------------------------------*/

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void MTK_prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert, MTK_eMemoryType eMemoryType );

#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
/*
  * Remove a block of memory that is being freed from used list
  */
static void MTK_prvRemoveBlockFromUsedList( BlockLink_t *pxBlockToRemove, MTK_eMemoryType eMemoryType );
#endif

void MTK_vTraceMALLOC( void *pvAddress, size_t uiSize, MTK_eMemoryType eMemoryType ) __attribute__((weak));
void MTK_vTraceFREE( void *pvAddress, size_t uiSize, MTK_eMemoryType eMemoryType ) __attribute__((weak));

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize	= ( sizeof( BlockLink_t ) + ( ( size_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) portBYTE_ALIGNMENT_MASK );

/* Create a couple of list links to mark the start and end of the list. */
//static BlockLink_t xStart, *pxEnd = NULL;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
//static size_t xFreeBytesRemaining = 0U;
//static size_t xMinimumEverFreeBytesRemaining = 0U;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static size_t xBlockAllocatedBit = 0;

static MTK_Heap_t *MTK_pxHeap = NULL;
static portBASE_TYPE MTK_xHeapCount = 0;

/*-----------------------------------------------------------*/

portBASE_TYPE MTK_prvGetHeapCount( void )
{
	return MTK_xHeapCount;
}
/*-----------------------------------------------------------*/

static MTK_Heap_t *MTK_prvGetHeap( MTK_eMemoryType eMemoryType )
{
portBASE_TYPE i = 0;

	for( i = 0; i < MTK_prvGetHeapCount(); i++ )
	{
		if( MTK_pxHeap[ i ].pxHeapRegion->eMemoryType == eMemoryType )
			return &( MTK_pxHeap[ i ] );
	}
	return NULL;
}
/*-----------------------------------------------------------*/

static MTK_Heap_t * MTK_prvGetHeapByAddr( uint32_t addr )
{
portBASE_TYPE i = 0;
uint32_t uHeapStart, uHeapSize;

	for( i = 0; i < MTK_prvGetHeapCount(); i++ )
	{
		uHeapStart = ( uint32_t )MTK_pxHeap[ i ].pxHeapRegion->pucStartAddress;
		uHeapSize = MTK_pxHeap[ i ].pxHeapRegion->xSizeInBytes;

		if( addr >=  uHeapStart && addr < uHeapStart + uHeapSize )
			return &( MTK_pxHeap[ i ] );
	}
	return NULL;
}
/*-----------------------------------------------------------*/

portBASE_TYPE MTK_xGetHeapSize( MTK_eMemoryType eMemoryType )
{
portBASE_TYPE i = 0;

	for( i = 0; i < MTK_prvGetHeapCount(); i++ )
	{
		if( MTK_pxHeap[ i ].pxHeapRegion->eMemoryType == eMemoryType )
			return MTK_pxHeap[ i ].pxHeapRegion->xSizeInBytes;
	}
	return 0;
}
/*-----------------------------------------------------------*/

#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
static void MTK_vPortPrintUsedList( MTK_eMemoryType eMemoryType )
{
BlockLink_t *pxBlock;
MTK_Heap_t *pxHeap = NULL;

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );
	pxBlock = &( pxHeap->xUsedStart );

	printf( "[MTK_HEAP(%d) USED]Print used list\n", eMemoryType );
	while( pxBlock != NULL )
	{
		printf("[MTK_HEAP(%d) USED]%p: guard:0x%08x, size:0x%08x, next:0x%08x, caller:0x%x\n",
				eMemoryType, pxBlock, pxBlock->ulGuard,
				pxBlock->xBlockSize & ~xBlockAllocatedBit,
				(uint32_t)pxBlock->pxNextFreeBlock, pxBlock->ulCallerAddr);

		if( pxBlock->ulGuard != MTK_HEAP_GUARD_PATTERN )
		{
			return;
		}

		pxBlock = pxBlock->pxNextFreeBlock;
	}
}
/*-----------------------------------------------------------*/

static void MTK_vPortPrintFreeList( MTK_eMemoryType eMemoryType )
{
BlockLink_t *pxBlock;
MTK_Heap_t *pxHeap = NULL;

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );
	pxBlock = &( pxHeap->xStart );

	printf( "[MTK_HEAP(%d) FREE]Print free list\n", eMemoryType );
	while( pxBlock != NULL )
	{
		printf("[MTK_HEAP(%d) FREE]%p: guard:0x%08x, size:0x%08x, next:0x%08x, caller:0x%x\n",
				eMemoryType, pxBlock, pxBlock->ulGuard,
				pxBlock->xBlockSize & ~xBlockAllocatedBit,
				(uint32_t)pxBlock->pxNextFreeBlock, pxBlock->ulCallerAddr);

		if( pxBlock->ulGuard != MTK_HEAP_GUARD_PATTERN )
		{
			return;
		}

		pxBlock = pxBlock->pxNextFreeBlock;
	}
}
/*-----------------------------------------------------------*/

void MTK_vPortPrintHeapList( MTK_eMemoryType eMemoryType )
{
	if( eMemoryType == MTK_eMemDefault )
	{
#ifdef CFG_MALLOC_DEBUG
		vPortPrintHeapList();
#else
		printf("Please enable CFG_MALLOC_DEBUG config firstly!\n");
#endif
	}
	else
	{
		MTK_vPortPrintUsedList( eMemoryType );
		MTK_vPortPrintFreeList( eMemoryType );
	}
}
/*-----------------------------------------------------------*/

BaseType_t MTK_xPortCheckHeap( MTK_eMemoryType eMemoryType )
{
BlockLink_t *pxBlock;
MTK_Heap_t *pxHeap = NULL;
BaseType_t xErrorFlag = 0;

	if( eMemoryType == MTK_eMemDefault )
	{
#ifdef CFG_MALLOC_DEBUG
		return xPortCheckHeap();
#else
		printf("Please enable CFG_MALLOC_DEBUG config firstly!\n");
		return 0;
#endif
	}

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );

	/* check free list */
	pxBlock = &( pxHeap->xStart );
	while( pxBlock != NULL )
	{
		if( pxBlock->ulGuard != MTK_HEAP_GUARD_PATTERN )
		{
			printf( "Found corruption in heap(%d) free list\n", eMemoryType );
			xErrorFlag = 1;
			break;
		}
		pxBlock = pxBlock->pxNextFreeBlock;
	}

	/* check used list */
	pxBlock = &( pxHeap->xUsedStart );
	while( pxBlock != NULL )
	{
		if( pxBlock->ulGuard != MTK_HEAP_GUARD_PATTERN )
		{
			printf( "Found corruption in heap(%d) used list\n", eMemoryType );
			xErrorFlag = 1;
			break;
		}
		pxBlock = pxBlock->pxNextFreeBlock;
	}

	return xErrorFlag;
}
#endif
/*-----------------------------------------------------------*/

void *MTK_pvPortMalloc( size_t xWantedSize, MTK_eMemoryType eMemoryType )
{
BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
void *pvReturn = NULL;
MTK_Heap_t *pxHeap = NULL;
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
uint32_t ulCallerAddr;
#endif

	if( eMemoryType == MTK_eMemDefault )
		return pvPortMalloc( xWantedSize);

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );
	if( !pxHeap )
	{
		printf("%s: ERROR: Donot support the memory type (%d)\n",
			__func__, eMemoryType);
		return NULL;
	}

	/* The heap must be initialised before the first call to
	prvPortMalloc(). */
	configASSERT( pxHeap->pxEnd );
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
	if( MTK_xPortCheckHeap( eMemoryType ) )
		MTK_vPortPrintHeapList( eMemoryType );

	ulCallerAddr = (uint32_t)__builtin_return_address(0);
#endif

	vTaskSuspendAll();
	{
		/* Check the requested block size is not so large that the top bit is
		set.  The top bit of the block size member of the BlockLink_t structure
		is used to determine who owns the block - the application or the
		kernel, so it must be free. */
		if( ( xWantedSize & xBlockAllocatedBit ) == 0 )
		{
			/* The wanted size is increased so it can contain a BlockLink_t
			structure in addition to the requested amount of bytes. */
			if( xWantedSize > 0 )
			{
				xWantedSize += xHeapStructSize;

				/* Ensure that blocks are always aligned to the required number
				of bytes. */
				if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0x00 )
				{
					/* Byte alignment required. */
					xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			if( ( xWantedSize > 0 ) && ( xWantedSize <= pxHeap->xFreeBytesRemaining ) )
			{
				/* Traverse the list from the start	(lowest address) block until
				one	of adequate size is found. */
				pxPreviousBlock = &(pxHeap->xStart);
				pxBlock = pxHeap->xStart.pxNextFreeBlock;
				while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != NULL ) )
				{
					pxPreviousBlock = pxBlock;
					pxBlock = pxBlock->pxNextFreeBlock;
				}

				/* If the end marker was reached then a block of adequate size
				was	not found. */
				if( pxBlock != pxHeap->pxEnd )
				{
					/* Return the memory space pointed to - jumping over the
					BlockLink_t structure at its start. */
					pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->pxNextFreeBlock ) + xHeapStructSize );

					/* This block is being returned for use so must be taken out
					of the list of free blocks. */
					pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

					/* If the block is larger than required it can be split into
					two. */
					if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
					{
						/* This block is to be split into two.  Create a new
						block following the number of bytes requested. The void
						cast is used to prevent byte alignment warnings from the
						compiler. */
						pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );

						/* Calculate the sizes of two blocks split from the
						single block. */
						pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
						pxNewBlockLink->ulGuard = MTK_HEAP_GUARD_PATTERN;
#endif
						pxBlock->xBlockSize = xWantedSize;

						/* Insert the new block into the list of free blocks. */
						MTK_prvInsertBlockIntoFreeList( ( pxNewBlockLink ), eMemoryType );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					pxHeap->xFreeBytesRemaining -= pxBlock->xBlockSize;

					if( pxHeap->xFreeBytesRemaining < pxHeap->xMinimumEverFreeBytesRemaining )
					{
						pxHeap->xMinimumEverFreeBytesRemaining = pxHeap->xFreeBytesRemaining;
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					/* The block is being returned - it is allocated and owned
					by the application and has no "next" block. */
					pxBlock->xBlockSize |= xBlockAllocatedBit;
					pxBlock->pxNextFreeBlock = NULL;
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
					pxBlock->ulCallerAddr = ulCallerAddr;
					pxHeap->pxUsedEnd->pxNextFreeBlock = pxBlock;
					pxHeap->pxUsedEnd = pxBlock;
#endif
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
		if( MTK_vTraceMALLOC )
			MTK_vTraceMALLOC( pvReturn, xWantedSize, eMemoryType );
	}
	( void ) xTaskResumeAll();

	#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if( pvReturn == NULL )
		{
			void MTK_vApplicationMallocFailedHook( MTK_eMemoryType eMemoryType ) __attribute__((weak));
			MTK_vApplicationMallocFailedHook( eMemoryType );
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}
	#endif

	return pvReturn;
}
/*-----------------------------------------------------------*/

void MTK_vPortFree( void *pv )
{
uint8_t *puc = ( uint8_t * ) pv;
BlockLink_t *pxLink;
MTK_Heap_t *pxHeap = NULL;

	pxHeap = MTK_prvGetHeapByAddr(( uint32_t )pv );
	/* if cannot find heap from mtk_heap,
	 * we think it is FreeRTOS default heap */
	if( pxHeap == NULL )
	{
		vPortFree( pv);
		pv = NULL;
		return ;
	}

#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
	if( MTK_xPortCheckHeap( pxHeap->pxHeapRegion->eMemoryType ) )
		MTK_vPortPrintHeapList( pxHeap->pxHeapRegion->eMemoryType );
#endif

	if( pv != NULL )
	{
		/* The memory being freed will have an BlockLink_t structure immediately
		before it. */
		puc -= xHeapStructSize;

		/* This casting is to keep the compiler from issuing warnings. */
		pxLink = ( void * ) puc;

		/* Check the block is actually allocated. */
		configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
#ifndef CFG_MTK_HEAP_MALLOC_DEBUG
		configASSERT( pxLink->pxNextFreeBlock == NULL );
#else
		configASSERT( pxLink->ulGuard == MTK_HEAP_GUARD_PATTERN );
#endif

		if( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 )
		{
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
			if( 1 )
#else
			if( pxLink->pxNextFreeBlock == NULL )
#endif
			{
				/* The block is being returned to the heap - it is no longer
				allocated. */
				pxLink->xBlockSize &= ~xBlockAllocatedBit;

				vTaskSuspendAll();
				{
					/* Add this block to the list of free blocks. */
					pxHeap->xFreeBytesRemaining += pxLink->xBlockSize;
					if( MTK_vTraceFREE )
						MTK_vTraceFREE( pv, pxLink->xBlockSize, pxHeap->pxHeapRegion->eMemoryType );
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
					MTK_prvRemoveBlockFromUsedList( ( ( BlockLink_t * ) pxLink ),
							pxHeap->pxHeapRegion->eMemoryType );
#endif
					MTK_prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ),
							pxHeap->pxHeapRegion->eMemoryType );
				}
				( void ) xTaskResumeAll();
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
		pv = NULL;
	}
}
/*-----------------------------------------------------------*/

void *MTK_pvPortCalloc( size_t nmemb, size_t size, MTK_eMemoryType eMemoryType )
{
void *pvReturn = NULL;

	pvReturn = MTK_pvPortMalloc( nmemb * size, eMemoryType);
	if( pvReturn )
	{
		memset( pvReturn, 0x0, nmemb * size);
	}
	return pvReturn;
}
/*-----------------------------------------------------------*/

/**
 * The realloc() function changes the size of the memory block pointed to by
 * ptr to size bytes. The contents will be unchanged in the range from the
 * start of the region up to the minimum of the old and new sizes.
 * If the new size is larger than the old size, the added memory will not be
 * initialized.
 * If ptr is NULL, then the call is equivalent to malloc(size), for all values
 * of size; if size is equal to zero, and ptr is not NULL, then the call is
 * equivalent to free(ptr).
 * Unless ptr is NULL, it must have been returned by an earlier call to
 * malloc(), calloc() or realloc(). If the area pointed to was moved, a
 * free(ptr) is done.
 */
void *MTK_pvPortRealloc( void *ptr, size_t size, MTK_eMemoryType eMemoryType )
{
void *pvReturn = NULL;
size_t xBlockSize = 0;
size_t xOldWantedSize = 0;
uint8_t *puc = ( uint8_t * ) ptr;
BlockLink_t *pxLink = NULL;

	if( ptr == NULL && size != 0 )
	{
		pvReturn = MTK_pvPortMalloc( size, eMemoryType );
		return pvReturn;
	}

	if( ptr != NULL && size == 0)
	{
		MTK_vPortFree( ptr );
		return NULL;
	}

	pvReturn = MTK_pvPortMalloc( size, eMemoryType );
	if( !pvReturn )
		return NULL;

	/* The memory being freed will have an BlockLink_t structure immediately
	before it. */
	puc -= xHeapStructSize;

	/* This casting is to keep the compiler from issuing warnings. */
	pxLink = ( void * ) puc;

	xBlockSize = pxLink->xBlockSize & ~xBlockAllocatedBit;
	xOldWantedSize = xBlockSize - xHeapStructSize;

	if( xOldWantedSize < size )
		memcpy( pvReturn, ptr, xOldWantedSize );
	else
		memcpy( pvReturn, ptr, size );

	MTK_vPortFree( ptr );
	return pvReturn;
}
/*-----------------------------------------------------------*/

size_t MTK_xPortGetFreeHeapSize( MTK_eMemoryType eMemoryType )
{
MTK_Heap_t *pxHeap = NULL;

	if( eMemoryType == MTK_eMemDefault )
		return xPortGetFreeHeapSize();

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );

	return pxHeap->xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

size_t MTK_xPortGetMinimumEverFreeHeapSize( MTK_eMemoryType eMemoryType )
{
MTK_Heap_t *pxHeap = NULL;

	if( eMemoryType == MTK_eMemDefault )
		return xPortGetMinimumEverFreeHeapSize();

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );

	return pxHeap->xMinimumEverFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

static void MTK_prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert, MTK_eMemoryType eMemoryType )
{
BlockLink_t *pxIterator;
uint8_t *puc;
MTK_Heap_t *pxHeap = NULL;

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );

	/* Iterate through the list until a block is found that has a higher address
	than the block being inserted. */
	for( pxIterator = &(pxHeap->xStart); pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock )
	{
		/* Nothing to do here, just iterate to the right position. */
	}

	/* Do the block being inserted, and the block it is being inserted after
	make a contiguous block of memory? */
	puc = ( uint8_t * ) pxIterator;
	if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert )
	{
		pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
		pxBlockToInsert = pxIterator;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	/* Do the block being inserted, and the block it is being inserted before
	make a contiguous block of memory? */
	puc = ( uint8_t * ) pxBlockToInsert;
	if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pxNextFreeBlock )
	{
		if( pxIterator->pxNextFreeBlock != pxHeap->pxEnd )
		{
			/* Form one big block from the two blocks. */
			pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
			pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
		}
		else
		{
			pxBlockToInsert->pxNextFreeBlock = pxHeap->pxEnd;
		}
	}
	else
	{
		pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
	}

	/* If the block being inserted plugged a gab, so was merged with the block
	before and the block after, then it's pxNextFreeBlock pointer will have
	already been set, and should not be set here as that would make it point
	to itself. */
	if( pxIterator != pxBlockToInsert )
	{
		pxIterator->pxNextFreeBlock = pxBlockToInsert;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}
}
/*-----------------------------------------------------------*/

#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
static void MTK_prvRemoveBlockFromUsedList( BlockLink_t *pxBlockToRemove, MTK_eMemoryType eMemoryType )
{
BlockLink_t *pxBlock, *pxPreviousBlock;
MTK_Heap_t *pxHeap = NULL;

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );

	pxPreviousBlock = &(pxHeap->xUsedStart);
	pxBlock = pxPreviousBlock->pxNextFreeBlock;

	while (pxBlock->pxNextFreeBlock != NULL && pxBlock != pxBlockToRemove) {
		pxPreviousBlock = pxBlock;
		pxBlock = pxBlock->pxNextFreeBlock;
	}

	if (pxBlock == pxBlockToRemove) {
		if (pxBlock->pxNextFreeBlock == NULL) {
			pxHeap->pxUsedEnd = pxPreviousBlock;
		}
		pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;
		pxBlock->pxNextFreeBlock = NULL;
	}
}
#endif

static const char* MTK_prvGetHeapName( MTK_eMemoryType eMemoryType )
{
MTK_Heap_t *pxHeap = NULL;

	pxHeap = MTK_prvGetHeap( eMemoryType );
	configASSERT( pxHeap );

	return pxHeap->pxHeapRegion->pcName;
}
/*-----------------------------------------------------------*/

void MTK_vDumpHeapStatus( void )
{
int i;
MTK_eMemoryType eMemType;

	for( i = 0; i < MTK_prvGetHeapCount(); i++ )
	{
		eMemType = MTK_pxHeap[ i ].pxHeapRegion->eMemoryType;
		PRINTF_E("%s:\n", MTK_prvGetHeapName( eMemType ));
		PRINTF_E("\tFree: %d\n", MTK_xPortGetFreeHeapSize( eMemType ));
		PRINTF_E("\tTotal: %d\n", MTK_xGetHeapSize( eMemType ));
		PRINTF_E("\tMinimumEverFree: %d\n",
			MTK_xPortGetMinimumEverFreeHeapSize( eMemType ));
	}
}
/*-----------------------------------------------------------*/

NORMAL_SECTION_FUNC void MTK_vPortDefineHeapRegions( const MTK_HeapRegion_t * const pxHeapRegions, size_t xHeapCount )
{
BlockLink_t *pxFirstFreeBlockInRegion = NULL;
size_t xAlignedHeap;
size_t xTotalRegionSize;
size_t xAddress;
const MTK_HeapRegion_t *pxHeapRegion;
portBASE_TYPE i;

	MTK_xHeapCount = xHeapCount;
	MTK_pxHeap = ( MTK_Heap_t * )pvPortMalloc( sizeof( MTK_Heap_t ) * xHeapCount );
	for( i = 0; i < xHeapCount; i++ )
	{
		MTK_pxHeap[ i ].pxHeapRegion = (MTK_HeapRegion_t *)&pxHeapRegions[i];
		MTK_pxHeap[ i ].pxEnd = NULL;
		MTK_pxHeap[ i ].xFreeBytesRemaining = 0;
		MTK_pxHeap[ i ].xMinimumEverFreeBytesRemaining = 0;

		/* Can only call once! */
		configASSERT( MTK_pxHeap[ i ].pxEnd == NULL );

		pxHeapRegion = MTK_pxHeap[ i ].pxHeapRegion;

		xTotalRegionSize = pxHeapRegion->xSizeInBytes;

		/* Ensure the heap region starts on a correctly aligned boundary. */
		xAddress = ( size_t ) pxHeapRegion->pucStartAddress;
		if( ( xAddress & portBYTE_ALIGNMENT_MASK ) != 0 )
		{
			xAddress += ( portBYTE_ALIGNMENT - 1 );
			xAddress &= ~portBYTE_ALIGNMENT_MASK;

			/* Adjust the size for the bytes lost to alignment. */
			xTotalRegionSize -= xAddress - ( size_t ) pxHeapRegion->pucStartAddress;
		}

		xAlignedHeap = xAddress;

		/* xStart is used to hold a pointer to the first item in the list of
		free blocks.  The void cast is used to prevent compiler warnings. */
		MTK_pxHeap[ i ].xStart.pxNextFreeBlock = ( BlockLink_t * ) xAlignedHeap;
		MTK_pxHeap[ i ].xStart.xBlockSize = ( size_t ) 0;
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
		MTK_pxHeap[ i ].xStart.ulGuard = MTK_HEAP_GUARD_PATTERN;
		MTK_pxHeap[ i ].xStart.ulCallerAddr = 0;
#endif

		/* pxEnd is used to mark the end of the list of free blocks and is
		inserted at the end of the region space. */
		xAddress = xAlignedHeap + xTotalRegionSize;
		xAddress -= xHeapStructSize;
		xAddress &= ~portBYTE_ALIGNMENT_MASK;
		MTK_pxHeap[ i ].pxEnd = ( BlockLink_t * ) xAddress;
		MTK_pxHeap[ i ].pxEnd->xBlockSize = 0;
		MTK_pxHeap[ i ].pxEnd->pxNextFreeBlock = NULL;
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
		MTK_pxHeap[ i ].pxEnd->ulGuard = MTK_HEAP_GUARD_PATTERN;
		MTK_pxHeap[ i ].pxEnd->ulCallerAddr = 0;
#endif

		/* To start with there is a single free block in this region that is
		sized to take up the entire heap region minus the space taken by the
		free block structure. */
		pxFirstFreeBlockInRegion = ( BlockLink_t * ) xAlignedHeap;
		pxFirstFreeBlockInRegion->xBlockSize = xAddress - ( size_t ) pxFirstFreeBlockInRegion;
		pxFirstFreeBlockInRegion->pxNextFreeBlock = MTK_pxHeap[i].pxEnd;
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
		pxFirstFreeBlockInRegion->ulGuard = MTK_HEAP_GUARD_PATTERN;
		pxFirstFreeBlockInRegion->ulCallerAddr = 0;
#endif

		MTK_pxHeap[ i ].xMinimumEverFreeBytesRemaining = pxFirstFreeBlockInRegion->xBlockSize;
		MTK_pxHeap[ i ].xFreeBytesRemaining = pxFirstFreeBlockInRegion->xBlockSize;

		/* Check something was actually defined before it is accessed. */
		configASSERT( pxFirstFreeBlockInRegion->xBlockSize );

#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
		MTK_pxHeap[ i ].xUsedStart.pxNextFreeBlock = NULL;
		MTK_pxHeap[ i ].xUsedStart.xBlockSize = ( size_t ) 0;
#ifdef CFG_MTK_HEAP_MALLOC_DEBUG
		MTK_pxHeap[ i ].xUsedStart.ulGuard = MTK_HEAP_GUARD_PATTERN;
		MTK_pxHeap[ i ].xUsedStart.ulCallerAddr = 0;
		MTK_pxHeap[ i ].pxUsedEnd = &( MTK_pxHeap[ i ].xUsedStart );
#endif
#endif
	}
	/* Work out the position of the top bit in a size_t variable. */
	xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );
}

