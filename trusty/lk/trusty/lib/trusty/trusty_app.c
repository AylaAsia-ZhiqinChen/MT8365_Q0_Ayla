/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * Modification based on code covered by the below mentioned copyright
 * and/or permission notice(S).
 */

/*
 * Copyright (c) 2012-2013, NVIDIA CORPORATION. All rights reserved
 * Copyright (c) 2013, Google, Inc. All rights reserved
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

#include <lib/trusty/trusty_app.h>

#include <arch.h>
#include <assert.h>
#include <compiler.h>
#include <debug.h>
#include "elf.h"
#include <err.h>
#include <kernel/event.h>
#include <kernel/mutex.h>
#include <kernel/thread.h>
#include <lib/trusty/ipc.h>
#include <malloc.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <lk/init.h>
#include <trace.h>

#define LOCAL_TRACE 0
/*
 * Layout of .trusty_app.manifest section in the trusted application is the
 * required UUID followed by an abitrary number of configuration options.
 *
 * Note: Ensure that the manifest definition is kept in sync with the
 * one userspace uses to build the trusty apps.
 */

enum {
    TRUSTY_APP_CONFIG_KEY_MIN_STACK_SIZE        = 1,
    TRUSTY_APP_CONFIG_KEY_MIN_HEAP_SIZE         = 2,
    TRUSTY_APP_CONFIG_KEY_MAP_MEM               = 3,
    TRUSTY_APP_CONFIG_KEY_MGMT_FLAGS            = 4,
    TRUSTY_APP_CONFIG_KEY_START_PORT            = 5,
};

enum trusty_app_mgmt_flags {
    TRUSTY_APP_MGMT_FLAGS_NONE                   = 0x0,
    /* Restart application on exit */
    TRUSTY_APP_MGMT_FLAGS_RESTART_ON_EXIT        = 0x1,
    /* Don't start the application at boot */
    TRUSTY_APP_MGMT_FLAGS_DEFERRED_START         = 0x2,
};

#define DEFAULT_MGMT_FLAGS TRUSTY_APP_MGMT_FLAGS_NONE

typedef struct trusty_app_manifest {
    uuid_t      uuid;
    uint32_t    config_options[];
} trusty_app_manifest_t;

#define TRUSTY_APP_START_ADDR   0x8000

#ifndef TRUSTY_APP_STACK_TOP
#define TRUSTY_APP_STACK_TOP    0x1000000 /* 16MB */
#endif

#ifndef DEFAULT_HEAP_SIZE
#define DEFAULT_HEAP_SIZE       (4 * PAGE_SIZE)
#endif

#define PAGE_MASK               (PAGE_SIZE - 1)

static u_int trusty_next_app_id;
static struct list_node trusty_app_list = LIST_INITIAL_VALUE(trusty_app_list);

/* These symbols are linker defined and are declared as unsized arrays to prevent
 * compiler(clang) optimizations that break when the list is empty and the symbols alias
 */
extern struct trusty_app_img __trusty_app_list_start[];
extern struct trusty_app_img __trusty_app_list_end[];

static bool apps_started;
static mutex_t apps_lock = MUTEX_INITIAL_VALUE(apps_lock);
static struct list_node app_notifier_list = LIST_INITIAL_VALUE(app_notifier_list);
uint als_slot_cnt;
static event_t app_mgr_event = EVENT_INITIAL_VALUE(app_mgr_event, 0,
                                                   EVENT_FLAG_AUTOUNSIGNAL);

#define PRINT_TRUSTY_APP_UUID(tid,u)                    \
    dprintf(SPEW,                                       \
            "trusty_app %d uuid: 0x%x 0x%x 0x%x 0x%x%x 0x%x%x%x%x%x%x\n",\
            tid,                                        \
            (u)->time_low, (u)->time_mid,               \
            (u)->time_hi_and_version,                   \
            (u)->clock_seq_and_node[0],                 \
            (u)->clock_seq_and_node[1],                 \
            (u)->clock_seq_and_node[2],                 \
            (u)->clock_seq_and_node[3],                 \
            (u)->clock_seq_and_node[4],                 \
            (u)->clock_seq_and_node[5],                 \
            (u)->clock_seq_and_node[6],                 \
            (u)->clock_seq_and_node[7]);

static bool address_range_within_bounds(const void *range_start,
                                        size_t range_size,
                                        const void *lower_bound,
                                        const void *upper_bound)
{
    const void *range_end = range_start + range_size;

    if (upper_bound < lower_bound) {
        LTRACEF("upper bound(%p) is below upper bound(%p)\n",
                upper_bound, lower_bound);
        return false;
    }

    if (range_end < range_start) {
        LTRACEF("Range overflows. start:%p size:%zd end:%p\n", range_start,
                range_size, range_end);
        return false;
    }

    if (range_start < lower_bound) {
        LTRACEF("Range starts(%p) before lower bound(%p)\n", range_start,
                lower_bound);
        return false;
    }

    if (range_end > upper_bound) {
        LTRACEF("Range ends(%p) past upper bound(%p)\n", range_end,
                upper_bound);
        return false;
    }

   return true;
}

static inline bool address_range_within_img(const void *range_start,
                                            size_t range_size,
                                            const struct trusty_app_img *appimg)
{
    return address_range_within_bounds(range_start, range_size,
                                       (const void *)appimg->img_start,
                                       (const void *)appimg->img_end);
}

static bool compare_section_name(Elf32_Shdr *shdr, const char *name,
                                 char *shstbl, uint32_t shstbl_size)
{
  return shstbl_size - shdr->sh_name > strlen(name) &&
         !strcmp(shstbl + shdr->sh_name, name);
}

static void finalize_registration(void)
{
    mutex_acquire(&apps_lock);
    apps_started = true;
    mutex_release(&apps_lock);
}

status_t trusty_register_app_notifier(trusty_app_notifier_t *n)
{
    status_t ret = NO_ERROR;

    mutex_acquire(&apps_lock);
    if (!apps_started)
        list_add_tail(&app_notifier_list, &n->node);
    else
        ret = ERR_ALREADY_STARTED;
    mutex_release(&apps_lock);
    return ret;
}

int trusty_als_alloc_slot(void)
{
    int ret;

    mutex_acquire(&apps_lock);
    if (!apps_started)
        ret = ++als_slot_cnt;
    else
        ret = ERR_ALREADY_STARTED;
    mutex_release(&apps_lock);
    dprintf(INFO, "als_slot_cnt %u ret %d\n", als_slot_cnt, ret);
    return ret;
}

static int trusty_thread_startup(void *arg)
{
    struct trusty_thread *trusty_thread = current_trusty_thread();

    vmm_set_active_aspace(trusty_thread->app->aspace);

    arch_enter_uspace(trusty_thread->entry,
                      ROUNDDOWN(trusty_thread->stack_start, 8),
                      ARCH_ENTER_USPACE_FLAG_32BIT, 0);

    __UNREACHABLE;
}

static status_t trusty_thread_start(struct trusty_thread *trusty_thread)
{
    DEBUG_ASSERT(trusty_thread && trusty_thread->thread);

    return thread_resume(trusty_thread->thread);
}

void __NO_RETURN trusty_thread_exit(int retcode)
{
    struct trusty_thread *trusty_thread = current_trusty_thread();
    vaddr_t stack_bot;

    ASSERT(trusty_thread);

    stack_bot = trusty_thread->stack_start - trusty_thread->stack_size;

    vmm_free_region(trusty_thread->app->aspace, stack_bot);

    thread_exit(retcode);
}

static struct trusty_thread *
trusty_thread_create(const char *name, vaddr_t entry, int priority,
                     vaddr_t stack_start, size_t stack_size,
                     trusty_app_t *trusty_app)
{
    struct trusty_thread *trusty_thread;
    status_t err;
    vaddr_t stack_bot = stack_start - stack_size;

    trusty_thread = calloc(1, sizeof(struct trusty_thread));
    if (!trusty_thread)
        return NULL;

    err = vmm_alloc(trusty_app->aspace, "stack", stack_size,
                    (void **)&stack_bot, PAGE_SIZE_SHIFT,
                    VMM_FLAG_VALLOC_SPECIFIC,
                    ARCH_MMU_FLAG_PERM_USER | ARCH_MMU_FLAG_PERM_NO_EXECUTE);

    if (err != NO_ERROR) {
        dprintf(CRITICAL, "failed(%d) to create thread stack(0x%lx) for app %u\n",
                err, stack_bot, trusty_app->app_id);
        goto err_stack;
    }

    ASSERT(stack_bot == stack_start - stack_size);

    trusty_thread->thread = thread_create(name, trusty_thread_startup, NULL,
                                          priority, DEFAULT_STACK_SIZE);
    if (!trusty_thread->thread)
        goto err_thread;

    trusty_thread->app = trusty_app;
    trusty_thread->entry = entry;
    trusty_thread->stack_start = stack_start;
    trusty_thread->stack_size = stack_size;
    trusty_thread->thread->tls[TLS_ENTRY_TRUSTY] = (uintptr_t)trusty_thread;

    return trusty_thread;

err_thread:
    vmm_free_region(trusty_app->aspace, stack_bot);
err_stack:
    free(trusty_thread);
    return NULL;
}

static status_t load_app_config_options(trusty_app_t *trusty_app,
                                        Elf32_Shdr *shdr)
{
    char *manifest_data;
    const char *port_name;
    uint32_t port_name_size;
    uint32_t port_flags;
    u_int *config_blob, config_blob_size;
    u_int i;
    status_t ret;

    /* have to at least have a valid UUID */
    if (shdr->sh_size < sizeof(uuid_t)) {
        dprintf(CRITICAL, "app %u manifest too small %u\n", trusty_app->app_id,
                shdr->sh_size);
        return ERR_NOT_VALID;
    }

    /* init default config options before parsing manifest */
    trusty_app->props.min_heap_size = DEFAULT_HEAP_SIZE;
    trusty_app->props.min_stack_size = DEFAULT_STACK_SIZE;
    trusty_app->props.mgmt_flags = DEFAULT_MGMT_FLAGS;

    manifest_data = (char *)(trusty_app->app_img->img_start + shdr->sh_offset);

    if (!address_range_within_img(manifest_data, shdr->sh_size,
                                  trusty_app->app_img)) {
        dprintf(CRITICAL, "app %u manifest data out of bounds\n",
                trusty_app->app_id);
        return ERR_NOT_VALID;
    }

    memcpy(&trusty_app->props.uuid, (uuid_t *)manifest_data, sizeof(uuid_t));

    PRINT_TRUSTY_APP_UUID(trusty_app->app_id, &trusty_app->props.uuid);

    manifest_data += sizeof(trusty_app->props.uuid);

    config_blob = (u_int *)manifest_data;
    config_blob_size = (shdr->sh_size - sizeof(uuid_t));

    trusty_app->props.config_entry_cnt = config_blob_size / sizeof (u_int);

    /* if no config options we're done */
    if (trusty_app->props.config_entry_cnt == 0) {
        return NO_ERROR;
    }

    /* save off configuration blob start so it can be accessed later */
    trusty_app->props.config_blob = config_blob;

    /*
     * Step thru configuration blob.
     *
     * Save off some configuration data while we are here but
     * defer processing of other data until it is needed later.
     */
    for (i = 0; i < trusty_app->props.config_entry_cnt; i++) {
        switch (config_blob[i]) {
        case TRUSTY_APP_CONFIG_KEY_MIN_STACK_SIZE:
            /* MIN_STACK_SIZE takes 1 data value */
            if ((trusty_app->props.config_entry_cnt - i) < 2) {
                dprintf(CRITICAL, "app %u manifest missing MIN_STACK_SIZE value\n",
                        trusty_app->app_id);
                return ERR_NOT_VALID;
            }
            trusty_app->props.min_stack_size = ROUNDUP(config_blob[++i], 4096);
            if (trusty_app->props.min_stack_size == 0) {
                dprintf(CRITICAL, "app %u manifest MIN_STACK_SIZE is 0\n",
                        trusty_app->app_id);
                return ERR_NOT_VALID;
            }
            break;
        case TRUSTY_APP_CONFIG_KEY_MIN_HEAP_SIZE:
            /* MIN_HEAP_SIZE takes 1 data value */
            if ((trusty_app->props.config_entry_cnt - i) < 2) {
                dprintf(CRITICAL, "app %u manifest missing MIN_HEAP_SIZE value\n",
                        trusty_app->app_id);
                return ERR_NOT_VALID;
            }
            trusty_app->props.min_heap_size = config_blob[++i];
            break;
        case TRUSTY_APP_CONFIG_KEY_MAP_MEM:
            /* MAP_MEM takes 3 data values */
            if ((trusty_app->props.config_entry_cnt - i) < 4) {
                dprintf(CRITICAL, "app %u manifest missing MAP_MEM value\n",
                        trusty_app->app_id);
                return ERR_NOT_VALID;
            }
            trusty_app->props.map_io_mem_cnt++;
            i += 3;
            break;
        case TRUSTY_APP_CONFIG_KEY_MGMT_FLAGS:
            /* MGMT_FLAGS takes 1 data value */
            if (trusty_app->props.config_entry_cnt - i < 2) {
                dprintf(CRITICAL, "app %u manifest missing MGMT_FLAGS value\n",
                        trusty_app->app_id);
                return ERR_NOT_VALID;
            }
            trusty_app->props.mgmt_flags = config_blob[++i];
            break;
        case TRUSTY_APP_CONFIG_KEY_START_PORT:
            /* START_PORT takes at least 3 data values */
            if (trusty_app->props.config_entry_cnt - i < 4) {
                dprintf(CRITICAL, "app %u manifest missing START_PORT values\n",
                        trusty_app->app_id);
                return ERR_NOT_VALID;
            }

            port_flags = config_blob[++i];
            port_name_size = config_blob[++i];
            port_name = (const char *)&config_blob[++i];

            if (!address_range_within_bounds(port_name, port_name_size,
                                             config_blob,
                                             config_blob + config_blob_size)) {
                dprintf(CRITICAL, "app %u manifest string out of bounds: %p size: 0x%x config_blob: %p config_blob_size: 0x%x\n",
                        trusty_app->app_id, port_name, port_name_size,
                        config_blob, config_blob_size);
                return ERR_NOT_VALID;
            }

            ret = ipc_register_startup_port(trusty_app, port_name,
                                            port_name_size, port_flags);
            if (ret != NO_ERROR) {
                dprintf(CRITICAL, "app %u failed to register port: %s\n",
                        trusty_app->app_id, port_name);
                return ret;
            }

            i += DIV_ROUND_UP(port_name_size, sizeof(uint32_t)) - 1;

            break;
        default:
            dprintf(CRITICAL, "app %u manifest contains unknown config key %u at %p\n",
                    trusty_app->app_id, config_blob[i], &config_blob[i]);
            return ERR_NOT_VALID;
        }
    }

    LTRACEF("trusty_app %p: stack_sz=0x%x\n", trusty_app,
            trusty_app->props.min_stack_size);
    LTRACEF("trusty_app %p: heap_sz=0x%x\n", trusty_app,
            trusty_app->props.min_heap_size);
    LTRACEF("trusty_app %p: num_io_mem=%d\n", trusty_app,
            trusty_app->props.map_io_mem_cnt);

    return NO_ERROR;
}

static status_t init_brk(trusty_app_t *trusty_app, vaddr_t hint)
{
    status_t status;
    uint arch_mmu_flags;
    vaddr_t start_brk;
    vaddr_t hint_page_end;
    size_t remaining;

    status = arch_mmu_query(&trusty_app->aspace->arch_aspace, hint, NULL,
                            &arch_mmu_flags);
    if(status != NO_ERROR) {
        dprintf(CRITICAL, "app %u heap hint page is not mapped %u\n",
                trusty_app->app_id, status);
        return ERR_NOT_VALID;
    }

    hint_page_end = ROUNDUP(hint, PAGE_SIZE);

    if (!(arch_mmu_flags & ARCH_MMU_FLAG_PERM_RO)) {
        start_brk = ROUNDUP(hint, CACHE_LINE);
        remaining = hint_page_end - start_brk;
    } else {
        start_brk = ROUNDUP(hint, PAGE_SIZE);
        remaining = 0;
    }

    if (remaining < trusty_app->props.min_heap_size) {
        status = vmm_alloc(trusty_app->aspace, "heap",
                           trusty_app->props.min_heap_size - remaining,
                           (void**)&hint_page_end,
                           PAGE_SIZE_SHIFT, VMM_FLAG_VALLOC_SPECIFIC,
                           ARCH_MMU_FLAG_PERM_USER |
                           ARCH_MMU_FLAG_PERM_NO_EXECUTE);

        if (status != NO_ERROR) {
            dprintf(CRITICAL, "failed(%d) to create heap(0x%lx) for app %u\n",
                    status, hint_page_end, trusty_app->app_id);
            return ERR_NO_MEMORY;
        }

        ASSERT(hint_page_end == ROUNDUP(hint, PAGE_SIZE));
    }

    trusty_app->start_brk = start_brk;
    trusty_app->cur_brk = trusty_app->start_brk;
    trusty_app->end_brk = trusty_app->start_brk +
                          trusty_app->props.min_heap_size;

    return NO_ERROR;
}

static status_t alloc_address_map(trusty_app_t *trusty_app)
{
    Elf32_Ehdr *elf_hdr = (Elf32_Ehdr *)trusty_app->app_img->img_start;
    void *trusty_app_image;
    Elf32_Phdr *prg_hdr;
    u_int i;
    status_t ret;
    vaddr_t start_code = ~0;
    vaddr_t start_data = 0;
    vaddr_t end_code = 0;
    vaddr_t end_data = 0;
    vaddr_t last_mem = 0;
    trusty_app_image = (void *)trusty_app->app_img->img_start;

    prg_hdr = (Elf32_Phdr *)(trusty_app_image + elf_hdr->e_phoff);

    if (!address_range_within_img(prg_hdr,
                                  sizeof(Elf32_Phdr) * elf_hdr->e_phnum,
                                  trusty_app->app_img)) {
        dprintf(CRITICAL, "ELF program headers table out of bounds\n");
        return ERR_NOT_VALID;
    }

    /* create mappings for PT_LOAD sections */
    for (i = 0; i < elf_hdr->e_phnum; i++, prg_hdr++) {
        vaddr_t first, last;


        LTRACEF("trusty_app %d: ELF type 0x%x, vaddr 0x%08x, paddr 0x%08x"
                " rsize 0x%08x, msize 0x%08x, flags 0x%08x\n",
                trusty_app->app_id, prg_hdr->p_type, prg_hdr->p_vaddr,
                prg_hdr->p_paddr, prg_hdr->p_filesz, prg_hdr->p_memsz,
                prg_hdr->p_flags);

        if (prg_hdr->p_type != PT_LOAD)
            continue;

        /* skip PT_LOAD if it's below trusty_app start or above .bss */
        if ((prg_hdr->p_vaddr < TRUSTY_APP_START_ADDR) ||
            (prg_hdr->p_vaddr >= trusty_app->end_bss))
            continue;

        /* check for overlap into user stack range */
        vaddr_t stack_bot = TRUSTY_APP_STACK_TOP -
                            trusty_app->props.min_stack_size;

        if (stack_bot < prg_hdr->p_vaddr + prg_hdr->p_memsz) {
            dprintf(CRITICAL,
                    "failed to load trusty_app: (overlaps user stack 0x%lx)\n",
                    stack_bot);
            return ERR_TOO_BIG;
        }

        vaddr_t vaddr = prg_hdr->p_vaddr;
        vaddr_t img_kvaddr = (vaddr_t)(trusty_app_image + prg_hdr->p_offset);
        size_t mapping_size;

        if (vaddr & PAGE_MASK) {
            dprintf(CRITICAL, "app %u segment %u load address 0x%lx in not page aligned\n",
                    trusty_app->app_id, i, vaddr);
            return ERR_NOT_VALID;
        }

        if (img_kvaddr & PAGE_MASK) {
            dprintf(CRITICAL, "app %u segment %u image address 0x%lx in not page aligned\n",
                    trusty_app->app_id, i, img_kvaddr);
            return ERR_NOT_VALID;
        }

        uint arch_mmu_flags = ARCH_MMU_FLAG_PERM_USER;
        if (!(prg_hdr->p_flags & PF_X)) {
            arch_mmu_flags += ARCH_MMU_FLAG_PERM_NO_EXECUTE;
        }

        if (prg_hdr->p_flags & PF_W) {
            paddr_t upaddr;
            void *load_kvaddr;
            size_t copy_size;
            size_t file_size;
            mapping_size = ROUNDUP(prg_hdr->p_memsz, PAGE_SIZE);

            if (!address_range_within_img((void *)img_kvaddr, prg_hdr->p_filesz,
                                          trusty_app->app_img)) {
                dprintf(CRITICAL, "ELF Program segment %u out of bounds\n", i);
                return ERR_NOT_VALID;
            }

            ret = vmm_alloc(trusty_app->aspace, "elfseg", mapping_size,
                            (void **)&vaddr, PAGE_SIZE_SHIFT,
                            VMM_FLAG_VALLOC_SPECIFIC,
                            arch_mmu_flags);

            if (ret != NO_ERROR) {
                dprintf(CRITICAL, "failed(%d) to allocate data segment(0x%lx) %u for app %u\n",
                        ret, vaddr, i, trusty_app->app_id);
                return ret;
            }

            ASSERT(vaddr == prg_hdr->p_vaddr);

            file_size = prg_hdr->p_filesz;
            while (file_size > 0) {
                ret = arch_mmu_query(&trusty_app->aspace->arch_aspace, vaddr,
                                     &upaddr, NULL);
                if (ret != NO_ERROR) {
                    dprintf(CRITICAL, "Could not copy data segment: %d\n", ret);
                    return ret;
                }

                load_kvaddr = paddr_to_kvaddr(upaddr);
                ASSERT(load_kvaddr);
                copy_size = MIN(file_size,PAGE_SIZE);
                memcpy(load_kvaddr, (void *)img_kvaddr, copy_size);
                file_size -= copy_size;
                vaddr += copy_size;
                img_kvaddr += copy_size;
            }

        } else {
            mapping_size = ROUNDUP(prg_hdr->p_filesz, PAGE_SIZE);

            if (!address_range_within_img((void *)img_kvaddr, mapping_size,
                                          trusty_app->app_img)) {
                dprintf(CRITICAL, "ELF Program segment %u out of bounds\n", i);
                return ERR_NOT_VALID;
            }

            paddr_t paddr = vaddr_to_paddr((void *)img_kvaddr);

            ASSERT(paddr && !(paddr & PAGE_MASK));

            arch_mmu_flags += ARCH_MMU_FLAG_PERM_RO;
            ret = vmm_alloc_physical(trusty_app->aspace,
                                     "elfseg", mapping_size, (void **)&vaddr,
                                     PAGE_SIZE_SHIFT, paddr,
                                     VMM_FLAG_VALLOC_SPECIFIC,
                                     arch_mmu_flags);
            if (ret != NO_ERROR) {
                dprintf(CRITICAL, "failed(%d) to map RO segment(0x%lx) %u for app %u\n",
                        ret, vaddr, i, trusty_app->app_id);
                return ret;
            }

            ASSERT(vaddr == prg_hdr->p_vaddr);
        }

        LTRACEF("trusty_app %d: load vaddr 0x%08lx, paddr 0x%08lx,"
                " rsize 0x%08zx, msize 0x%08x, access r%c%c,"
                " flags 0x%x\n",
                trusty_app->app_id, vaddr, vaddr_to_paddr((void *)vaddr),
                mapping_size, prg_hdr->p_memsz,
                arch_mmu_flags & ARCH_MMU_FLAG_PERM_RO ? '-' : 'w',
                arch_mmu_flags & ARCH_MMU_FLAG_PERM_NO_EXECUTE ? '-' : 'x',
                arch_mmu_flags);

        /* start of code/data */
        first = prg_hdr->p_vaddr;
        if (first < start_code)
            start_code = first;
        if (start_data < first)
            start_data = first;

        /* end of code/data */
        last = prg_hdr->p_vaddr + prg_hdr->p_filesz;
        if ((prg_hdr->p_flags & PF_X) && end_code < last)
            end_code = last;
        if (end_data < last)
            end_data = last;

        /* hint for start of brk */
        last_mem = MAX(last_mem, prg_hdr->p_vaddr + prg_hdr->p_memsz);
    }

    ret = init_brk(trusty_app, last_mem);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "failed to load trusty_app: trusty_app heap creation error\n");
        return ret;
    }

    dprintf(SPEW, "trusty_app %d: code: start 0x%08lx end 0x%08lx\n",
            trusty_app->app_id, start_code, end_code);
    dprintf(SPEW, "trusty_app %d: data: start 0x%08lx end 0x%08lx\n",
            trusty_app->app_id, start_data, end_data);
    dprintf(SPEW, "trusty_app %d: bss:                end 0x%08lx\n",
            trusty_app->app_id, trusty_app->end_bss);
    dprintf(SPEW, "trusty_app %d: brk:  start 0x%08lx end 0x%08lx\n",
            trusty_app->app_id, trusty_app->start_brk, trusty_app->end_brk);
    dprintf(SPEW, "trusty_app %d: entry 0x%08x\n", trusty_app->app_id,
            elf_hdr->e_entry);

    return NO_ERROR;
}

/*
 * Create a trusty_app from its memory image and add it to the global list of
 * apps
 */
static status_t trusty_app_create(struct trusty_app_img *app_img)
{
    Elf32_Ehdr *ehdr;
    Elf32_Shdr *shdr;
    Elf32_Shdr *bss_shdr, *manifest_shdr;
    char *shstbl;
    uint32_t shstbl_size;
    trusty_app_t *trusty_app;
    u_int i;
    status_t ret;

    if (app_img->img_start & PAGE_MASK || app_img->img_end & PAGE_MASK) {
        dprintf(CRITICAL, "app image is not page aligned start 0x%lx end 0x%lx\n",
                app_img->img_start, app_img->img_end);
        return ERR_NOT_VALID;
    }

    dprintf(SPEW, "trusty_app: start %p size 0x%08lx end %p\n",
            (void *)app_img->img_start,
            app_img->img_end - app_img->img_start,
            (void *)app_img->img_end);

    trusty_app = (trusty_app_t *) calloc(1, sizeof(trusty_app_t));
    if (!trusty_app) {
        dprintf(CRITICAL, "trusty_app: failed to allocate memory for trusty app\n");
        return ERR_NO_MEMORY;
    }

    ehdr = (Elf32_Ehdr *)app_img->img_start;
    if (!address_range_within_img(ehdr, sizeof(Elf32_Ehdr), app_img)) {
        dprintf(CRITICAL, "trusty_app_create: ELF header out of bounds\n");
        ret = ERR_NOT_VALID;
        goto err_hdr;
    }

    if (strncmp((char *)ehdr->e_ident, ELFMAG, SELFMAG)) {
        dprintf(CRITICAL, "trusty_app_create: ELF header not found\n");
        ret = ERR_NOT_VALID;
        goto err_hdr;
    }

    shdr = (Elf32_Shdr *) ((intptr_t)ehdr + ehdr->e_shoff);
    if (!address_range_within_img(shdr, sizeof(Elf32_Shdr) * ehdr->e_shnum,
                                  app_img)) {
        dprintf(CRITICAL, "trusty_app_create: ELF section headers out of bounds\n");
        ret = ERR_NOT_VALID;
        goto err_hdr;
    }

    if (ehdr->e_shstrndx >= ehdr->e_shnum) {
        dprintf(CRITICAL, "trusty_app_create: ELF names table section header out of bounds\n");
        ret = ERR_NOT_VALID;
        goto err_hdr;
    }

    shstbl = (char *)((intptr_t)ehdr + shdr[ehdr->e_shstrndx].sh_offset);
    shstbl_size = shdr[ehdr->e_shstrndx].sh_size;
    if (!address_range_within_img(shstbl, shstbl_size, app_img)) {
        dprintf(CRITICAL, "trusty_app_create: ELF section names out of bounds\n");
        ret = ERR_NOT_VALID;
        goto err_hdr;
    }

    bss_shdr = manifest_shdr = NULL;

    for (i = 0; i < ehdr->e_shnum; i++) {

        if (shdr[i].sh_type == SHT_NULL)
            continue;

        LTRACEF("trusty_app: sect %d, off 0x%08x, size 0x%08x, flags 0x%02x, name %s\n",
                i, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_flags,
                shstbl + shdr[i].sh_name);

        /* track bss and manifest sections */
        if (compare_section_name(shdr + i, ".bss", shstbl, shstbl_size)) {
            bss_shdr = shdr + i;
            trusty_app->end_bss = bss_shdr->sh_addr + bss_shdr->sh_size;
        }
        else if (compare_section_name(shdr + i, ".trusty_app.manifest",
                                      shstbl, shstbl_size)) {
            manifest_shdr = shdr + i;
        }
    }

    /* we need these sections */
    if (!bss_shdr) {
        dprintf(CRITICAL, "bss section header not found\n");
        ret = ERR_NOT_VALID;
        goto err_hdr;

    }

    if (!manifest_shdr) {
        dprintf(CRITICAL, "manifest section header not found\n");
        ret = ERR_NOT_VALID;
        goto err_hdr;

    }

    trusty_app->app_id = trusty_next_app_id++;
    trusty_app->app_img = app_img;
    trusty_app->state = APP_NOT_RUNNING;

    ret = load_app_config_options(trusty_app, manifest_shdr);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "manifest processing failed\n");
        goto err_load;
    }

    list_add_tail(&trusty_app_list, &trusty_app->node);

    return NO_ERROR;

err_load:
    trusty_next_app_id--;
err_hdr:
    free(trusty_app);
    return ret;
}

status_t trusty_app_setup_mmio(trusty_app_t *trusty_app, u_int mmio_id,
                               vaddr_t *vaddr, uint32_t map_size)
{
    status_t ret;
    u_int i;
    u_int id, offset, size;
    uint32_t port_name_size;

    /* step thru configuration blob looking for I/O mapping requests */
    for (i = 0; i < trusty_app->props.config_entry_cnt; i++) {
        switch (trusty_app->props.config_blob[i]) {
        case TRUSTY_APP_CONFIG_KEY_MAP_MEM:
            id = trusty_app->props.config_blob[++i];
            offset = trusty_app->props.config_blob[++i];
            size = ROUNDUP(trusty_app->props.config_blob[++i],
                           PAGE_SIZE);

            if (id != mmio_id)
                continue;

            map_size = ROUNDUP(map_size, PAGE_SIZE);
            if (map_size > size)
                return ERR_INVALID_ARGS;
            ret = vmm_alloc_physical(trusty_app->aspace, "mmio",
                                     map_size, (void **)vaddr,
                                     PAGE_SIZE_SHIFT, offset,
                                     0,
                                     ARCH_MMU_FLAG_UNCACHED_DEVICE |
                                     ARCH_MMU_FLAG_PERM_USER);
            dprintf(SPEW, "mmio: vaddr 0x%lx, paddr 0x%x, ret %d\n",
                    *vaddr, offset, ret);
            return ret;
        case TRUSTY_APP_CONFIG_KEY_START_PORT:
            /* START_PORT takes 2 data values plus the aligned port name size */
            port_name_size = trusty_app->props.config_blob[i + 2];
            i += 2 + DIV_ROUND_UP(port_name_size, sizeof(uint32_t));
            break;
        case TRUSTY_APP_CONFIG_KEY_MIN_STACK_SIZE:
        case TRUSTY_APP_CONFIG_KEY_MIN_HEAP_SIZE:
        case TRUSTY_APP_CONFIG_KEY_MGMT_FLAGS:
            i++;
            break;
        default:
            panic("unknown config key 0x%x at %p in config blob of app %d\n",
                  trusty_app->props.config_blob[i],
                  &trusty_app->props.config_blob[i], trusty_app->app_id);
        }
    }

    return ERR_NOT_FOUND;
}

static status_t trusty_app_start(trusty_app_t *trusty_app)
{
    char name[32];
    struct trusty_thread *trusty_thread;
    struct trusty_app_notifier *n;
    Elf32_Ehdr *elf_hdr;
    int ret;

    DEBUG_ASSERT(trusty_app->state == APP_STARTING);

    snprintf(name, sizeof(name), "trusty_app_%d_%08x-%04x-%04x",
             trusty_app->app_id,
             trusty_app->props.uuid.time_low,
             trusty_app->props.uuid.time_mid,
             trusty_app->props.uuid.time_hi_and_version);

    ret = vmm_create_aspace(&trusty_app->aspace, name, 0);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "Failed(%d) to allocate address space for %s\n", ret,
                name);
        goto err_aspace;
    }

    ret = alloc_address_map(trusty_app);
    if (ret != NO_ERROR) {
        dprintf(CRITICAL, "failed(%d) to load address map for %s\n", ret, name);
        goto err_map;
    }

    /* attach als_cnt */
    trusty_app->als = calloc(1, als_slot_cnt * sizeof(void*));
    if (!trusty_app->als) {
        dprintf(CRITICAL, "failed to allocate local storage for %s\n", name );
        ret = ERR_NO_MEMORY;
        /* alloc_address_map gets cleaned up by destroying the address space */
        goto err_alloc;
    }

    /* call all registered startup notifiers */
    list_for_every_entry(&app_notifier_list, n, struct trusty_app_notifier,
                         node) {

        if (!n->startup)
            continue;

        ret = n->startup(trusty_app);
        if (ret != NO_ERROR) {
            dprintf(CRITICAL, "failed(%d) to invoke startup notifier for %s\n",
                    ret, name);
            goto err_notifier;
        }
    }

    elf_hdr = (Elf32_Ehdr *)trusty_app->app_img->img_start;
    trusty_thread = trusty_thread_create(name, elf_hdr->e_entry,
                                         DEFAULT_PRIORITY,
                                         TRUSTY_APP_STACK_TOP,
                                         trusty_app->props.min_stack_size,
                                         trusty_app);
    if (!trusty_thread) {
        dprintf(CRITICAL, "failed to allocate trusty thread for %s\n", name);
        ret = ERR_NO_MEMORY;
        goto err_thread;
    }

    trusty_app->thread = trusty_thread;

    trusty_app->state = APP_RUNNING;
    ret = trusty_thread_start(trusty_app->thread);

    ASSERT(ret == NO_ERROR);

    return ret;

err_thread:
err_notifier:
    for (n = list_prev_type(&app_notifier_list, &n->node,
                            struct trusty_app_notifier, node);
         n != NULL;
         n = list_prev_type(&app_notifier_list, &n->node,
                            struct trusty_app_notifier, node)) {
        if (!n->shutdown)
            continue;

        if (n->shutdown(trusty_app) != NO_ERROR)
            panic("failed to invoke shutdown notifier for %s\n",
                  name);
    }

    free(trusty_app->als);
err_alloc:
err_map:
    vmm_free_aspace(trusty_app->aspace);
err_aspace:
    return ret;
}

void trusty_app_exit(int status)
{
    status_t ret;
    struct trusty_app *app;
    struct trusty_app_notifier *notifier;

    app = current_trusty_app();

    DEBUG_ASSERT(app->state == APP_RUNNING);

    LTRACEF("app %u exiting...\n", app->app_id);

    app->state = APP_TERMINATING;

    list_for_every_entry(&app_notifier_list, notifier,
                         struct trusty_app_notifier, node) {

        if(!notifier->shutdown)
            continue;

        ret = notifier->shutdown(app);
        if (ret != NO_ERROR)
            panic("shutdown notifier for app %u failed(%d)\n",
                  app->app_id, ret);
    }

    free(app->als);
    event_signal(&app_mgr_event, false);
    trusty_thread_exit(status);
}

static status_t app_mgr_handle_starting(struct trusty_app *app)
{
    status_t ret;

    DEBUG_ASSERT(app->state == APP_STARTING);
    LTRACEF("starting app %u\n", app->app_id);

    ret = trusty_app_start(app);

    if (ret != NO_ERROR)
        app->state = APP_NOT_RUNNING;

    return ret;
}

static status_t app_mgr_handle_terminating(struct trusty_app *app)
{
    status_t ret;
    int retcode;

    DEBUG_ASSERT(app->state == APP_TERMINATING);
    LTRACEF("waiting for app %u to exit \n", app->app_id);

    ret = thread_join(app->thread->thread, &retcode, INFINITE_TIME);
    ASSERT(ret == NO_ERROR);
    free(app->thread);
    ret = vmm_free_aspace(app->aspace);

    if (app->props.mgmt_flags & TRUSTY_APP_MGMT_FLAGS_RESTART_ON_EXIT) {
        app->state = APP_STARTING;
        event_signal(&app_mgr_event, false);
    }
    else {
        app->state = APP_NOT_RUNNING;
    }

    ipc_handle_app_exit(app);

    return ret;
}

static int app_mgr(void *arg)
{
    status_t ret;
    struct trusty_app *app;

    while (true) {
        LTRACEF("app manager waiting for events\n");
        event_wait(&app_mgr_event);
        list_for_every_entry(&trusty_app_list, app, struct trusty_app, node) {
            switch (app->state) {
            case APP_TERMINATING:
                ret = app_mgr_handle_terminating(app);
                if (ret != NO_ERROR)
                    panic("failed(%d) to terminate app %u\n", ret, app->app_id);
                break;
            case APP_NOT_RUNNING:
                break;
            case APP_STARTING:
                ret = app_mgr_handle_starting(app);
                if (ret != NO_ERROR)
                    panic("failed(%d) to start app %u\n", ret, app->app_id);
                break;
            case APP_RUNNING:
                break;
            default:
                panic("app %u in unknown state %u\n", app->app_id,
                      app->state);
            }
        }
    }
}

static void app_mgr_init(void)
{
    status_t err;
    thread_t *app_mgr_thread;

    LTRACEF("Creating app manager thread\n");
    app_mgr_thread = thread_create("app manager", &app_mgr, NULL,
                                   DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);

    if (!app_mgr_thread)
        panic("Failed to create app manager thread\n");

    err = thread_resume(app_mgr_thread);
    if (err != NO_ERROR)
        panic("Failed to start app manager thread\n");
}

status_t trusty_app_request_start(struct trusty_app *app)
{
    int oldstate;

    oldstate = atomic_cmpxchg((int *)&app->state, APP_NOT_RUNNING,
                              APP_STARTING);

    if (oldstate == APP_NOT_RUNNING) {
        event_signal(&app_mgr_event, false);
        return NO_ERROR;
    }

    return ERR_ALREADY_STARTED;
}

void trusty_app_init(void)
{
    struct trusty_app_img *app_img;

    finalize_registration();

    app_mgr_init();

    for (app_img = __trusty_app_list_start;
         app_img != __trusty_app_list_end; app_img++) {
        if (trusty_app_create(app_img) != NO_ERROR)
            panic("Failed to create builtin apps\n");
    }
}

trusty_app_t *trusty_app_find_by_uuid(uuid_t *uuid)
{
    trusty_app_t *ta;

    /* find app for this uuid */
    list_for_every_entry(&trusty_app_list, ta, trusty_app_t, node)
        if (!memcmp(&ta->props.uuid, uuid, sizeof(uuid_t)))
            return ta;

    return NULL;
}

/* rather export trusty_app_list?  */
void trusty_app_forall(void (*fn)(trusty_app_t *ta, void *data), void *data)
{
    trusty_app_t *ta;

    if (fn == NULL)
        return;

    list_for_every_entry(&trusty_app_list, ta, trusty_app_t, node)
        fn(ta, data);
}

static void start_apps(uint level)
{
    trusty_app_t *trusty_app;

    list_for_every_entry(&trusty_app_list, trusty_app, trusty_app_t, node) {

        if (trusty_app->props.mgmt_flags & TRUSTY_APP_MGMT_FLAGS_DEFERRED_START)
            continue;

        trusty_app_request_start(trusty_app);
    }
}

LK_INIT_HOOK(libtrusty_apps, start_apps, LK_INIT_LEVEL_APPS + 1);
