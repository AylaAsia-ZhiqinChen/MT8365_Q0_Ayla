/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#include <vector>
#include <cstdlib>
#include <iostream>
#include <exception>

#include <sys/mman.h>

#include <ion/ion.h>                // stardard ion
#include <linux/ion_drv.h>          // for ION_CMDS, ION_CACHE_SYNC_TYPE, define for ion_mm_data_t
#include <libion_mtk/include/ion.h> // interface for mtk ion

#include "vpuCommon.h"
#include "vpu_buf.h"
#include "vpuImpl.h"

typedef struct vbuf_info	vbuf_info_t;
typedef struct vbuf_alloc	vbuf_alloc_t;
typedef struct vbuf_free	vbuf_free_t;
typedef struct vbuf_sync	vbuf_sync_t;

//-----------------------------------------
// definition
struct vpuMemBufferImpl :VpuMemBuffer {
public:
    /* memory type */
    enum vpuMemType type;
    /* stored necessory info in handle */
    union {
        vpuMemIonHandle ionHnd;
        vpuMemDmaHandle dmaHnd;
        vpuMemMallocHandle mallocHnd;
    };
};

//-----------------------------------------
// ion buffer
ionAllocator::ionAllocator()
{
#ifdef VPU_HAVE_ION
	/* open ion dev fd */
	mDevFd = mt_ion_open(__FILE__);
	if(mDevFd == 0)
	{
		VPU_BUFFER_ERR("ionAllocator::ionAllocator: open ion dev fd failed\n");
	}
#else
	mDevFd = 0;
#endif
	return;
}

ionAllocator::~ionAllocator()
{
#ifdef VPU_HAVE_ION
    /* close ion dev fd */
    ion_close(mDevFd);
#endif
    return;
}

bool ionAllocator::allocMem(uint32_t len, uint32_t align, vpuMemIonHandle &hnd)
{
#ifdef VPU_HAVE_ION
    int flags = 0;
    int sharedFd = 0;
    struct ion_mm_data mmData;
    struct ion_sys_data sysData;
    ion_user_handle_t ionBufHnd;
    uint64_t va;

    /* check argument */
    if(len == 0)
    {
        return false;
    }

    /* allocate ion buffer from MM HEAP */
    //flags = ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC; //don't need cache, for memory pool flush duration issue
    if(ion_alloc_mm(mDevFd, (size_t)len, (size_t)align, flags, &ionBufHnd))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: get ion buffer handle failed\n");
        return false;
    }

    /* get ion buffer share handle */
    if(ion_share(mDevFd, ionBufHnd, &sharedFd))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: share ion buffer failed\n");
        goto ion_allocate_err2;
    }

    /* get buffer virtual address */
    va = (uint64_t)ion_mmap(mDevFd, NULL, (size_t)len, PROT_READ|PROT_WRITE, MAP_SHARED, sharedFd, 0);
    if(va == 0)
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: get ion buffer va failed\n");
        goto ion_allocate_err1;
    }

    /* config buffer */
    memset(&mmData, 0, sizeof(mmData));
    mmData.mm_cmd = ION_MM_CONFIG_BUFFER;
    mmData.config_buffer_param.handle      = ionBufHnd;
    mmData.config_buffer_param.eModuleID   = M4U_PORT_VPU1;
    mmData.config_buffer_param.security    = 0;
    mmData.config_buffer_param.coherent    = 0;
    if(ion_custom_ioctl(mDevFd, ION_CMD_MULTIMEDIA, &mmData))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: config ion buffer failed\n");
        goto ion_allocate_err0;
    }

    /* get mva from ion buffer */
    memset(&sysData, 0, sizeof(sysData));
    sysData.sys_cmd = ION_SYS_GET_PHYS;
    sysData.get_phys_param.handle = ionBufHnd;
    if(ion_custom_ioctl(mDevFd, ION_CMD_SYSTEM, &sysData))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: get ion buffer mva failed\n");
        goto ion_allocate_err0;
    }

    /* assign value */
    hnd.handle = ionBufHnd;
    hnd.sharedFd = sharedFd;
    hnd.va = va;
    hnd.mva = sysData.get_phys_param.phy_addr;
    hnd.mvaLen= sysData.get_phys_param.len;

    return true;

    /* error handle*/
ion_allocate_err0:
    if(ion_munmap(mDevFd, (void*)va, len))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: unmap failed\n");
    }
ion_allocate_err1:
    if(ion_share_close(mDevFd, hnd.sharedFd))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: close shared failed\n");
    }
ion_allocate_err2:
    if(ion_free(mDevFd, ionBufHnd))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: free failed\n");
    }
#endif
    return false;
}

bool ionAllocator::importMem(int fd, uint32_t len, vpuMemIonHandle &hnd)
{
#ifdef VPU_HAVE_ION
    ion_user_handle_t ionBufHnd;
    struct ion_mm_data mmData;
    struct ion_sys_data sysData;
    uint64_t va = 0;

    /* import fd */
    if(ion_import(mDevFd, fd, &ionBufHnd))
    {
        VPU_BUFFER_ERR("ionAllocator::importMem: fail to import buffer\n");
        return false;
    }

    /* get buffer virtual address */
    va = (uint64_t)ion_mmap(mDevFd, NULL, (size_t)len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(va == 0)
    {
        VPU_BUFFER_ERR("ionAllocator::importMem: get ion buffer va failed\n");
        goto ion_import_fail;
    }

    /* config buffer */
    memset(&mmData, 0, sizeof(mmData));
    mmData.mm_cmd = ION_MM_CONFIG_BUFFER;
    mmData.config_buffer_param.handle      = ionBufHnd;
    mmData.config_buffer_param.eModuleID   = M4U_PORT_VPU1;
    mmData.config_buffer_param.security    = 0;
    mmData.config_buffer_param.coherent    = 0;
    if(ion_custom_ioctl(mDevFd, ION_CMD_MULTIMEDIA, &mmData))
    {
        VPU_BUFFER_ERR("ionAllocator::importMem: config ion buffer failed\n");
        goto ion_import_fail;
    }

    /* get mva from ion buffer */
    memset(&sysData, 0, sizeof(sysData));
    sysData.sys_cmd = ION_SYS_GET_PHYS;
    sysData.get_phys_param.handle = ionBufHnd;
    if(ion_custom_ioctl(mDevFd, ION_CMD_SYSTEM, &sysData))
    {
        VPU_BUFFER_ERR("ionAllocator::importMem: get ion buffer mva failed\n");
        goto ion_import_fail;
    }

    /* assign value */
    hnd.handle = ionBufHnd;
    hnd.sharedFd = fd; /* import buffer don't need store fd for free */
    hnd.va = va;
    hnd.mva = sysData.get_phys_param.phy_addr;
    hnd.mvaLen= sysData.get_phys_param.len;

    /* pair to ion_import */
    if(ion_free(mDevFd, hnd.handle))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: free failed\n");
    }

    return true;

ion_import_fail:
    if(ion_free(mDevFd, hnd.handle))
    {
        VPU_BUFFER_ERR("ionAllocator::allocMem: free failed\n");
    }
#endif
    return false;
}

bool ionAllocator::freeMem(vpuMemIonHandle &hnd)
{
#ifdef VPU_HAVE_ION
    if(mDevFd <=0)
    {
        return false;
    }

    if(ion_munmap(mDevFd, (void*)hnd.va, hnd.mvaLen))
    {
        VPU_BUFFER_ERR("ionAllocator::freeMem: unmap failed\n");
    }

    if(ion_share_close(mDevFd, hnd.sharedFd))
    {
        VPU_BUFFER_ERR("ionAllocator::freeMem: close shared failed\n");
    }

    if(ion_free(mDevFd, hnd.handle))
    {
        VPU_BUFFER_ERR("ionAllocator::freeMem: free failed\n");
    }
#endif
    return true;
}

bool ionAllocator::syncMem(vpuMemIonHandle &hnd, enum vpuSyncType type)
{
#ifdef VPU_HAVE_ION
    struct ion_sys_data sysData;

    if(hnd.sharedFd < 0 || mDevFd <= 0)
    {
        VPU_BUFFER_ERR("ionAllocator::syncMem: invalid arguments\n");
        return false;
    }

    sysData.sys_cmd = ION_SYS_CACHE_SYNC;
    sysData.cache_sync_param.handle = hnd.handle;

	if (type == VPU_SYNC_TYPE_FLUSH)
    	sysData.cache_sync_param.sync_type = ION_CACHE_FLUSH_BY_RANGE;
	else
		sysData.cache_sync_param.sync_type = ION_CACHE_INVALID_BY_RANGE;

    sysData.cache_sync_param.va = (void *)hnd.va;
    sysData.cache_sync_param.size = hnd.mvaLen;

    if(ion_custom_ioctl(mDevFd, ION_CMD_SYSTEM, &sysData))
    {
        VPU_BUFFER_ERR("ionAllocator::syncMem: sync failed\n");
        return false;
    }

#endif
    return true;
}

//-----------------------------------------
// dma buffer
dmaAllocator::dmaAllocator(int devfd)
{
    /* store dma dev fd */
    mDevFd = devfd;
    return;
}

dmaAllocator::~dmaAllocator()
{
    return;
}

bool dmaAllocator::allocMem(uint32_t len, uint32_t align, vpuMemDmaHandle &hnd)
{
	vbuf_alloc_t st_alloc;
	vbuf_free_t st_free;

	st_alloc.type = VBUF_TYPE_ALLOC;
	st_alloc.req_size = len;
	st_alloc.cache_flag = 0;
	st_alloc.exp_flag = 1;

	int ret = ioctl(mDevFd, VPU_IOCTL_VBUF_ALLOC, &st_alloc);
	if (ret < 0)
	{
		VPU_BUFFER_ERR("fail to alloc buf, %s, errno = %d", strerror(errno), errno);
		return false;
	}

	hnd.handle = st_alloc.handle;
	hnd.iova = st_alloc.iova_addr;
	hnd.dma_fd = st_alloc.exp_fd;
	hnd.size = st_alloc.buf_size;

	hnd.va = (uint64_t)mmap (0, st_alloc.buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, st_alloc.exp_fd, 0);
	if (hnd.va == NULL) {
		st_free.handle = hnd.handle;
		ioctl(mDevFd, VPU_IOCTL_VBUF_FREE, &st_free);
		return false;
	}

	return true;
}

bool dmaAllocator::importMem(int fd, uint32_t len, vpuMemDmaHandle &hnd)
{
	vbuf_alloc_t st_alloc;
	vbuf_free_t st_free;

	st_alloc.type = VBUF_TYPE_IMPORT;
	st_alloc.imp_fd = dup(fd);

	int ret = ioctl(mDevFd, VPU_IOCTL_VBUF_ALLOC, &st_alloc);
	if (ret < 0)
	{
        	VPU_BUFFER_ERR("fail to import buf, %s, errno = %d", strerror(errno), errno);
		close(st_alloc.imp_fd);
        	return false;
	}

	hnd.handle = st_alloc.handle;
	hnd.iova = st_alloc.iova_addr;
	hnd.dma_fd = st_alloc.imp_fd;
	hnd.size = st_alloc.buf_size;

	hnd.va = (uint64_t)mmap (0, st_alloc.buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, st_alloc.imp_fd, 0);
	if (hnd.va == NULL) {
		st_free.handle = hnd.handle;
		ioctl(mDevFd, VPU_IOCTL_VBUF_FREE, &st_free);
		close(st_alloc.imp_fd);
		return false;
	}

    return true;
}

bool dmaAllocator::freeMem(vpuMemDmaHandle &hnd)
{
	vbuf_free_t st_free;
	st_free.handle = hnd.handle;

	if (hnd.va) {
		munmap((void*)hnd.va, hnd.size);
	}

	int ret = ioctl(mDevFd, VPU_IOCTL_VBUF_FREE, &st_free);
	if (ret < 0) {
		VPU_BUFFER_ERR("fail to free buf, %s, errno = %d", strerror(errno), errno);
		return false;
	}

	if (hnd.dma_fd)
		close(hnd.dma_fd);

	return true;
}

bool dmaAllocator::syncMem(vpuMemDmaHandle &hnd, enum vpuSyncType type)
{
	vbuf_sync_t st_sync;
	st_sync.handle = hnd.handle;

	if (type == VPU_SYNC_TYPE_FLUSH)
		st_sync.direction = VBUF_SYNC_TO_DEVICE;
	else
		st_sync.direction = VBUF_SYNC_TO_CPU;

	int ret = ioctl(mDevFd, VPU_IOCTL_VBUF_SYNC, &st_sync);
	if (ret < 0) {
		VPU_BUFFER_ERR("fail to sync buf, %s, errno = %d", strerror(errno), errno);
		return false;
	}

    return true;
}

//-----------------------------------------
// user malloc buffer
userAllocator::userAllocator()
{
    return;
}

userAllocator::~userAllocator()
{
    return;
}

bool userAllocator::allocMem(uint32_t len, uint32_t align, vpuMemMallocHandle &hnd)
{
    uint64_t va = 0;

    va = (uint64_t)malloc(len);
    if (va == 0)
    {
        return false;
    }

    hnd.va = va;
    VPU_BUFFER_DEBUG("userAllocator::allocMem: va = 0x%llx\n", hnd.va);

    return true;
}

bool userAllocator::importMem(int fd, uint32_t len, vpuMemMallocHandle &hnd)
{
    int ret = false;

    return ret;
}

bool userAllocator::freeMem(vpuMemMallocHandle &hnd)
{
    VPU_BUFFER_DEBUG("userAllocator::freeMem: va = 0x%llx\n", hnd.va);

    if (hnd.va == 0)
    {
        VPU_BUFFER_ERR("userAllocator::freeMem: invalid va address\n");
        return false;
    }
    free((void *)hnd.va);

    return true;
}

bool userAllocator::syncMem(vpuMemMallocHandle &hnd, enum vpuSyncType type)
{
    return true;
}

//-----------------------------------------
vpuMemAllocator::vpuMemAllocator(int dmaDevFd):
    mIonAllocator(),mUserAllocator(),mDmaAllocator(dmaDevFd)
{
    /* init alloc list */
    mAllocList.clear();
}

vpuMemAllocator::~vpuMemAllocator()
{
    /* pop and free all buffer in allocate list */
    while (!mAllocList.empty())
    {
        auto b = mAllocList.back();
        freeMem(b); /* call vpuMemAllocator::freeMem */
    }

    mAllocList.clear();
}

VpuMemBuffer* vpuMemAllocator::allocMem(uint32_t len, uint32_t align, enum vpuMemType type)
{
    vpuMemBufferImpl *buf = nullptr;
    bool ret = false;

    /* check arguments */
    if (len == 0)
    {
        VPU_BUFFER_ERR("vpuMemAllocator::allocMem: invalid arg(%d)\n", type);
        return nullptr;
    }

    buf = new vpuMemBufferImpl;
    if (buf == nullptr)
    {
        VPU_BUFFER_ERR("vpuMemAllocator::allocMem: alloc buffer failed(%d)\n", type);
        return nullptr;
    }
    VPU_BUFFER_INFO("vpuMemAllocator::allocMem: len = %d, type = %d\n", len, type);

    /* dispatch allocator by type */
    switch (type)
    {
        case VPU_MEM_TYPE_ION:
            ret = mIonAllocator.allocMem(len, align, buf->ionHnd);
            buf->va = buf->ionHnd.va;
            buf->pa = buf->ionHnd.mva;
            buf->len = len;
            buf->dma_fd = buf->ionHnd.sharedFd;
            break;

        case VPU_MEM_TYPE_DMA:
            ret = mDmaAllocator.allocMem(len,align, buf->dmaHnd);
            buf->va = buf->dmaHnd.va;
            buf->pa = buf->dmaHnd.iova;
            buf->len = len;
            buf->dma_fd = buf->dmaHnd.dma_fd;
            break;

        case VPU_MEM_TYPE_MALLOC:
            ret = mUserAllocator.allocMem(len, align, buf->mallocHnd);
            buf->va = buf->mallocHnd.va;
            buf->pa = buf->mallocHnd.va;
            buf->len = len;
            buf->dma_fd = 0;
            break;

        default:
            ret = false;
            break;
    }

    /* allocate failed, delete buffer and return null ptr */
    if (ret != true)
    {
        VPU_BUFFER_ERR("vpuMemAllocator::allocMem: failed to alloc type(%d) buffer\n", type);
        delete buf;
        return nullptr;
    }

    buf->type = type;
    VPU_BUFFER_DEBUG("vpuMemAllocator::allocMem: type = %d, va = 0x%llx, pa = 0x%llx, len = %d\n", buf->type, buf->va, buf->pa, buf->len);

    /* add memBuffer to allocate list */
    mAllocList.push_back((VpuMemBuffer *)buf);

    return (VpuMemBuffer *)buf;
}

VpuMemBuffer* vpuMemAllocator::importMem(int fd, uint32_t len, enum vpuMemType type)
{
    vpuMemBufferImpl *buf = nullptr;
    int ret = false;

    /* check arguments */
    if (fd <= 0 || len == 0)
    {
        VPU_BUFFER_ERR("vpuMemAllocator::importMem: invalid arg(%d)\n", type);
        return nullptr;
    }

    buf = new vpuMemBufferImpl;
    if (buf == nullptr)
    {
        VPU_BUFFER_ERR("vpuMemAllocator::importMem: alloc buffer failed(%d)\n", type);
        return nullptr;
    }

    /* dispatch allocator by type */
    switch (type)
    {
        case VPU_MEM_TYPE_ION:
            ret = mIonAllocator.importMem(fd, len, buf->ionHnd);
            buf->va = buf->ionHnd.va;
            buf->pa = buf->ionHnd.mva;
            buf->len = len;
            buf->dma_fd = buf->ionHnd.sharedFd;
            break;

        case VPU_MEM_TYPE_DMA:
            ret = mDmaAllocator.importMem(fd, len, buf->dmaHnd);
            buf->va = buf->dmaHnd.va;
            buf->pa = buf->dmaHnd.iova;
            buf->len = len;
            buf->dma_fd = buf->dmaHnd.dma_fd;
            break;

        case VPU_MEM_TYPE_MALLOC:
            ret = mUserAllocator.importMem(fd, len, buf->mallocHnd);
            buf->va = buf->mallocHnd.va;
            buf->pa = buf->mallocHnd.va;
            buf->len = len;
            buf->dma_fd = 0;
            break;

        default:
            ret = false;
            break;
    }

    /* import failed, delete buffer and return null ptr */
    if (ret != true)
    {
        VPU_BUFFER_ERR("vpuMemAllocator::importMem: failed to alloc type(%d) buffer\n", type);
        delete buf;
        return nullptr;
    }

    buf->type = type;
    VPU_BUFFER_DEBUG("vpuMemAllocator::importMem: type = %d, va = 0x%llx, pa = 0x%llx, len = %d\n", buf->type, buf->va, buf->pa, buf->len);

    /* add memBuffer to allocate list */
    mAllocList.push_back((VpuMemBuffer *)buf);

    return (VpuMemBuffer *)buf;
}

bool vpuMemAllocator::freeMem(VpuMemBuffer* buf)
{
    bool ret = false;
    vpuMemBufferImpl *memBuf = (vpuMemBufferImpl *)buf;
    auto iter = mAllocList.begin();

    /* check arguments */
    if (buf == nullptr)
    {
        VPU_BUFFER_ERR("vpuMemAllocator::freeMem: invalid argument\n");
        return false;
    }

    /* check allocate list and delete VpuMemBuffer*/
    for (iter = mAllocList.begin(); iter != mAllocList.end(); iter++)
    {
        /* get list element */
        if (*iter == buf)
        {
            /* dispatch allocator by type */
            switch (memBuf->type)
            {
            case VPU_MEM_TYPE_ION:
                ret = mIonAllocator.freeMem(memBuf->ionHnd);
                break;

            case VPU_MEM_TYPE_DMA:
                ret = mDmaAllocator.freeMem(memBuf->dmaHnd);
                break;

            case VPU_MEM_TYPE_MALLOC:
                ret = mUserAllocator.freeMem(memBuf->mallocHnd);
                break;

            default:
                ret = false;
                break;
            }
            break;
        }
    }

    if (ret != true)
    {
        return false;
    }

    VPU_BUFFER_DEBUG("vpuMemAllocator::freeMem: type = %d, va = 0x%llx, pa = 0x%llx, len = %d\n", memBuf->type, memBuf->va, memBuf->pa, memBuf->len);

    /* delete mem bufeer from alloc list */
    delete memBuf;
    mAllocList.erase(iter);

    return true;
}

bool vpuMemAllocator::syncMem(VpuMemBuffer* buf, enum vpuSyncType type)
{
    vpuMemBufferImpl *memBuf = (vpuMemBufferImpl *)buf;

    if (buf == nullptr)
    {
        VPU_BUFFER_ERR("vpuMemAllocator::syncMem: invalid argument\n");
        return false;
    }

    switch (memBuf->type)
    {
        case VPU_MEM_TYPE_ION:
            return mIonAllocator.syncMem(memBuf->ionHnd, type);

        case VPU_MEM_TYPE_DMA:
            return mDmaAllocator.syncMem(memBuf->dmaHnd, type);

        case VPU_MEM_TYPE_MALLOC:
            return mUserAllocator.syncMem(memBuf->mallocHnd, type);

        default:
            return false;
    }

    //return ((vpuMemBufferImpl *)buf)->syncMemBuffer();
}

uint64_t vpuMemAllocator::queryVa(uint64_t pa)
{
    auto iterator = mAllocList.begin();

    for (iterator = mAllocList.begin(); iterator != mAllocList.end(); iterator++)
    {
        if (pa >= (*iterator)->pa && pa < (*iterator)->pa + (*iterator)->len)
        {
            return (*iterator)->va + (pa - (*iterator)->pa);
        }
    }

    return 0;
}

uint64_t vpuMemAllocator::queryPa(uint64_t va)
{
    auto iterator = mAllocList.begin();

    for (iterator = mAllocList.begin(); iterator != mAllocList.end(); iterator++)
    {
        if (va >= (*iterator)->va && va < (*iterator)->va + (*iterator)->len)
        {
            return (*iterator)->va + (va - (*iterator)->va);
        }
    }

    return 0;
}
