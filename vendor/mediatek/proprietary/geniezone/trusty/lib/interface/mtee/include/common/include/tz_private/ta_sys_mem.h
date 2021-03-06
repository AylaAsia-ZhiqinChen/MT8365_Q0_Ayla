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

/** Commands for TA memory **/

#ifndef __TRUSTZONE_TA_SYS_MEM__
#define __TRUSTZONE_TA_SYS_MEM__

#include "tz_cross/gz_version.h"
#include "tz_cross/ta_mem.h"
#include "tz_private/sys_mem.h"


/**
 * Memory object define
 *
 * @param buffer    A pointer to memory buffer.
 * @param size    memory size in bytes.
 * @param alignment    Memory alignment in bytes.
 * @param reference    reference count.
 */
typedef struct {
    void *buffer;
    void *extra_info;
    uint32_t size;
    uint32_t alignment;
    uint32_t reference;
} MTEE_MEM_OBJ;

/**
 * Memory object define
 *
 * @param buffer    A pointer to memory buffer.
 * @param size    memory size in bytes.
 * @param mmutbl    A pointer to MMU table.
 */
typedef struct {
    void *buffer;
    uint32_t size;
    uint32_t offset;
    uint32_t page_num;
    //uint32_t *mmutbl;
    uint64_t *mmutbl;
    int control; // 0 = no remap, 1 = remap
    uint64_t pa;
} MTEE_SHAREDMEM_OBJ;

/**
 * Query registered shared memory
 *
 * To query shared memory buffer by handle.
 * It is used when handle is send from REE to get the memory parameters.
 *
 * @param shaerdMem    Shared Memory handle.
 * @param param    The pointer to the shared memory parameters.
 * @return    return code.
 */
TZ_RESULT TA_Mem_QuerySharedmem (MTEE_SHAREDMEM_HANDLE handle, MTEE_SHAREDMEM_PARAM *param);

/**
 * Query registered shared memory
 *
 * To query memory buffer by handle.
 * It queries memory, which is allocated in secure world by related APIs.
 * It is used when handle is send from REE to get the memory parameters.
 *
 * @param handle    Memory handle.
 * @param param    The pointer to the shared memory parameters.
 * @return    return code.

 */
TZ_RESULT TA_Mem_QueryMem (uint32_t handle, MTEE_MEM_PARAM *param);

/**
 * Shared memory
 *
 * A shared memory is normal memory, which can be seen by Normal world and Secure world.
 * It is used to create the comminicattion between two worlds.
 * Currently, zero-copy data transfer is supportted, for simple and efficient design.
 *
 * The shared memory lifetime:
 * 1. CA (Client Application at REE) prepares memory
 * 2. CA registers it to TEE scope.
 * 3. A handle is returned. CA can use it to communicate with TEE.
 * 4. If shared memory is not used, CA deregisters it.
 * 5. CA frees memory.
 *
 * Note: Because shared memory can be seen by both Normal and Secure world.
 * It is a possible weakpoint to bed attacked or leak secure data.
 */

/**
 * Register shared memory
 *
 * @param handle    A pointer to shared memory handle.
 * @param param    A pointer to shared memory paramete, defined as MTEE_SHAREDMEM_PARAM
 * @return    If success, return address. If fail, return NULL.
 */
void *TA_Mem_RegisterSharedMem(MTEE_SHAREDMEM_HANDLE *shm_handle, MTEE_SHAREDMEM *param);

/**
 * Register chunk memory
 *
 * @param handle   A pointer to chunk memory handle.
 * @param param    A pointer to chunk memory parameter, defined as MTEE_SHAREDMEM 
 *  (limitation: input PAs (PA array, same with shared mem.) must be continuous currently)
 * @return    If success, return address. If fail, return NULL.
 */

//void *MTEE_AppendMultiChunkmem(MTEE_SHAREDMEM_HANDLE *shm_handle, MTEE_SHAREDMEM *param, uint32_t cmd);

/**
 * Register shared memory With Tag
 *
 * @param handle    A pointer to shared memory handle.
 * @param param    A pointer to shared memory paramete, defined as MTEE_SHAREDMEM_PARAM
 * @param tag    A string for debugging purpose
 * @return    If success, return address. If fail, return NULL.
 */
void *TA_Mem_RegisterSharedMemWithTag(MTEE_SHAREDMEM_HANDLE *handle, MTEE_SHAREDMEM *param, const char *tag);

/**
 * Unregister shared memory
 *
 * @param shaerdMem    Shared memory handle.
 * @return    return code.
 */
TZ_RESULT TA_Mem_UnregisterSharedmem (MTEE_SHAREDMEM_HANDLE handle);
//TZ_RESULT MTEE_ReleaseMultiChunkmem (MTEE_SHAREDMEM_HANDLE handle, uint32_t cmd);

/**
 * Secure memory
 *
 * A secure memory can be seen only in Secure world.
 * Secure memory, here, is defined as external memory (ex: external DRAM) protected by trustzone.
 * It can protect from software attack very well, but can not protect from physical attack, like memory probe.
 *
 * Secure memory spec.:
 * 1. Protected by trustzone (NS = 0).
 * 2. External memory (ex: external DRAM).
 * 3. With cache.
 * 4. Physically continuous.
 *
 * All secure memory APIs are defined at <stdlib.h> as well-known memory APIs, ex: malloc.
 * Here, for programming style, some APIs are provided as TEE API style by macro define.
 */

/**
 * Secure memory allocation
 *
 * Allocate one memory.
 * If memory is allocated successfully, a pointer to the memory will be provided.
 *
 * Memory lifetime:
 * 1. Allocate memory, and get the memory handle and pointer. Reference count will be 1 after allocating.
 * Memory pointer can be used directly, and memory handle is used to pass memory between sessions.
 * 2. If another session wants to use the same memory, reference it.
 * 3. If it stops to use it, unreference it. Notice that, reference number must be equal to unreference number
 * to prevent memory leak.
 * 4. Unreference it (for free memory), if the memory is not used. If reference count is 1, do unreference will free it.
 *
 * Simple rules:
 * 1. start by allocate, end by unreference (free).
 * 2. start by reference, end by unreference.
 *
 * @param handle    [out] A pointer to memory handle
 * @param align    Memory alignment in bytes.
 * @param size    The size of the buffer to be allocated in bytes.
 * @return    If success, return address. If fail, return NULL.
 */
void *TA_Mem_AllocMem (MTEE_SECUREMEM_HANDLE *handle, uint32_t alignment, uint32_t size);
void *TA_Mem_ZallocMem (MTEE_SECUREMEM_HANDLE *handle, uint32_t alignment, uint32_t size);
void *TA_Mem_AllocMemWithTag (MTEE_SECUREMEM_HANDLE *handle, uint32_t alignment, uint32_t size, char *tag);
void *TA_Mem_ZallocMemWithTag (MTEE_SECUREMEM_HANDLE *handle, uint32_t alignment, uint32_t size, char *tag);

/**
 * Secure memory reference
 *
 * Reference memory.
 * Referen count will be increased by 1 after reference.
 *
 * Reference lifetime:
 * 1. Reference the memory before using it.
 * 2. Unreference it if it is not used.
 *
 * @param buffer    The pointer to the memory block to be referenced
 * @return    If success, return address. If fail, return NULL.
 */
void *TA_Mem_ReferenceMem (MTEE_SECUREMEM_HANDLE handle);

/**
 * Secure memory unreference
 *
 * Unreference memory.
 * Reference count will be decreased 1 after unreference.
 * After unreference, if reference count is zero, memory will be freed.
 *
 * @param buffer    The pointer to the memory block to be referenced
 * @return    0 if success. <0 for error. otherwise for reference count
 */
int TA_Mem_UnreferenceMem (MTEE_SECUREMEM_HANDLE handle);

/**
 * Secure chunk memory
 *
 * A secure chunk memory can be seen only in Secure world.
 * It is a kind of secure memory but with difference characteristic:
 * 1. It is designed and optimized for chunk memory usage.
 * 2. For future work, it can be released as normal memory for more flexible memory usage.
 *
 * Secure chunk memory spec.:
 * 1. Protected by trustzone (NS = 0).
 * 2. External memory (ex: external DRAM).
 * 3. With cache.
 * 4. For future, it can be released to normal world.
 */
void *TA_Mem_AllocMultiChunkmem(MTEE_SHAREDMEM_HANDLE chm_handle, MTEE_SECUREMEM_HANDLE *mem_handle, uint32_t alignment, uint32_t size, uint64_t in_key);
void *TA_Mem_ZallocMultiChunkmem(MTEE_SHAREDMEM_HANDLE chm_handle, MTEE_SECUREMEM_HANDLE *mem_handle, uint32_t alignment, uint32_t size, uint64_t in_key);
void *TA_Mem_ReferenceChunkmem (MTEE_SECUREMEM_HANDLE handle, uint64_t in_key);
int TA_Mem_UnreferenceMultiChunkmem (MTEE_SECUREMEM_HANDLE mem_handle, uint64_t in_key);
uint64_t TA_Mem_QueryChm_GZ_PA (MTEE_SECUREMEM_HANDLE mem_handle, uint64_t in_key);

#if (GZ_API_MAIN_VERSION > 2)
/**
 * Secure chunk memory allocation
 *
 * Allocate one memory.
 * If memory is allocated successfully, a pointer to the memory will be provided.
 *
 * Memory lifetime:
 * 1. Allocate memory, and get the memory handle and pointer. Reference count will be 1 after allocating.
 * Memory pointer can be used directly, and memory handle is used to pass memory between sessions.
 * 2. If another session wants to use the same memory, reference it.
 * 3. If it stops to use it, unreference it. Notice that, reference number must be equal to unreference number
 * to prevent memory leak.
 * 4. Unreference it (for free memory), if the memory is not used. If reference count is 1, do unreference will free it.
 *
 * Simple rules:
 * 1. start by allocate, end by unreference (free).
 * 2. start by reference, end by unreference.
 *
 * @param handle    [out] A pointer to memory handle
 * @param align    Memory alignment in bytes.
 * @param size    The size of the buffer to be allocated in bytes.
 * @return    If success, return address. If fail, return NULL.
 */
void *TA_Mem_AllocChunkmem (MTEE_SECUREMEM_HANDLE *handle, uint32_t alignment, uint32_t size);

/**
 * Secure chunk memory reference
 *
 * Reference memory.
 * Referen count will be increased by 1 after reference.
 *
 * Reference lifetime:
 * 1. Reference the memory before using it.
 * 2. Unreference it if it is not used.
 *
 * @param buffer    The pointer to the memory block to be referenced
 * @return    If success, return address. If fail, return NULL.
 */
//void *TA_Mem_ReferenceChunkmem (MTEE_SECUREMEM_HANDLE handle);
void *TA_Mem_ReferenceChunkmem (MTEE_SECUREMEM_HANDLE handle, uint64_t in_key);

/**
 * Secure chunk memory unreference
 *
 * Unreference memory.
 * Reference count will be decreased 1 after unreference.
 * After unreference, if reference count is zero, memory will be freed.
 *
 * @param buffer    The pointer to the memory block to be referenced
 * @return    0 if success. <0 for error. otherwise for reference count
 */
int TA_Mem_UnreferenceChunkmem (MTEE_SECUREMEM_HANDLE handle);
#endif /* chunk memory */

/**
 * Secure memory set extra information
 *
 * Setting the extra information which will be binding to the handle.
 *
 * extra information can be use to store some attribute for the memory buffer.
 *
 * @param handle    The handle of the memory block
 * @param pex_info  The extra inforamtion to bind with the memory handle
 * @return    If success, return zero. If fail, return non-zero.
 */
int TA_Mem_SetExtraInfo(uint32_t handle, void *pex_info);


/**
 * Secure memory get extra information
 *
 * Getting the extra information which is binding to the handle.
 *
 * extra information can be use to store some attribute for the memory buffer.
 *
 * @param handle    The handle of the memory block
 * @param ppex_info The returned extra inforamtion which bound to the memory handle
 * @return    If success, return zero. If fail, return non-zero.
 */
int TA_Mem_GetExtraInfo(uint32_t handle, void **ppex_info);

/**
 * copy extra information from one handle to another
 *
 * copy the extra information which is binding to the handle to another handle.
 *
 * extra information can be use to store some attribute for the memory buffer.
 *
 * @param src_handle    The source of extra info binding handle
 * @param dst_handle    The destination of extra info binding handle
 * @return    If success, return zero. If fail, return non-zero.
 */
int TA_Mem_CopyExtraInfo(uint32_t src_handle, uint32_t dst_handle);

/*fix mtee sync*/
/**
 * the function pointer typedef for iommu_func_t
 */
typedef int (*iommu_func_t)(void *domain, void *va, unsigned long pa, uint32_t size, uint32_t port);

#if (GZ_API_MAIN_VERSION > 2)
/**
 * Get secure virtual mapped memory range
 *
 * @param base    The pointer to the virtual mapped memory base
 * @param size    The size of the virtual mapped memory
 * @return    0 if success. <0 for error.
 */
int TA_Mem_GetVirtMappedmemRange (void **base, uint32_t *size);

#if 0
/**
 * the function pointer typedef for iommu_func_t
 */
typedef int (*iommu_func_t)(void *domain, void *va, unsigned long pa, uint32_t size, uint32_t port);
#endif

/**
 * Hook the secure virtual mapped function for iommu
 *
 * @param mapfunc    The function pointer to map virtual mapped memory for devi
 * @param unmapfunc    The function pointer to unmap virtual memory for devices
 * @return    0 if success. <0 for error.
 */
int TA_Mem_HookVirtMappingFunc(iommu_func_t mapfunc, iommu_func_t unmapfunc);
#endif

int TA_Mem_ConfigChunkMemInfo(uint64_t pa, uint32_t size, uint32_t region_id);

#endif /* __TRUSTZONE_TA_SYSMEM__ */

