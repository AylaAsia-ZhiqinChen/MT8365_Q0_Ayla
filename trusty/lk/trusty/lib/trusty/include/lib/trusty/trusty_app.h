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

#ifndef __LIB_TRUSTY_APP_H
#define __LIB_TRUSTY_APP_H

#include <assert.h>
#include <list.h>
#include <sys/types.h>
#include <kernel/thread.h>
#include <kernel/vm.h>
#include <lib/trusty/uuid.h>

enum app_state {
    APP_NOT_RUNNING = 0,
    APP_STARTING,
    APP_RUNNING,
    APP_TERMINATING,
};

typedef struct
{
    uuid_t      uuid;
    uint32_t    mgmt_flags;
    uint32_t    min_stack_size;
    uint32_t    min_heap_size;
    uint32_t    map_io_mem_cnt;
    uint32_t    config_entry_cnt;
    uint32_t    *config_blob;
} trusty_app_props_t;

struct trusty_app_img {
    uintptr_t       img_start;
    uintptr_t       img_end;
};

typedef struct trusty_app trusty_app_t;

struct trusty_thread
{
    vaddr_t stack_start;
    size_t stack_size;
    vaddr_t entry;
    thread_t *thread;
    trusty_app_t *app;
};

typedef struct trusty_app
{
    /* corresponds to the order in which the apps were started */
    u_int app_id;
    enum app_state state;
    vmm_aspace_t *aspace;
    vaddr_t end_bss;
    vaddr_t start_brk;
    vaddr_t cur_brk;
    vaddr_t end_brk;
    trusty_app_props_t props;
    struct trusty_app_img *app_img;
    struct trusty_thread *thread;
    /* app local storage */
    void **als;
    struct list_node node;
} trusty_app_t;

void trusty_app_init(void);

/**
 * trusty_app_request_start() - Request that an application be started
 * @app: application to be started
 *
 * If the application is already running then this function has no effect.
 * Otherwise the application will be started.
 *
 * Return: ERR_ALREADY_STARTED if the application is already running. NO_ERROR
 * otherwise.
 */
status_t trusty_app_request_start(struct trusty_app *app);

void trusty_app_exit(int status) __NO_RETURN;
status_t trusty_app_setup_mmio(trusty_app_t *trusty_app,
                               u_int mmio_id, vaddr_t *vaddr, uint32_t size);
trusty_app_t *trusty_app_find_by_uuid(uuid_t *uuid);
void trusty_app_forall(void (*fn)(trusty_app_t *ta, void *data), void *data);
void trusty_thread_exit(int status);

typedef struct trusty_app_notifier
{
    struct list_node node;
    status_t (*startup)(trusty_app_t *app);
    status_t (*shutdown)(trusty_app_t *app);
} trusty_app_notifier_t;


/*
 * All app notifiers registration has to be complete before
 * libtrusty is initialized which is happening at LK_INIT_LEVEL_APPS-1
 * init level.
 */
status_t trusty_register_app_notifier(trusty_app_notifier_t *n);

/*
 * All als slots must be allocated before libtrusty is initialized
 * which is happening at LK_INIT_LEVEL_APPS-1 init level.
 */
int trusty_als_alloc_slot(void);

extern uint als_slot_cnt;

static inline void *trusty_als_get(struct trusty_app *app, int slot_id)
{
    uint slot = slot_id - 1;
    ASSERT(slot < als_slot_cnt);
    return app->als[slot];
}

static inline void trusty_als_set(struct trusty_app *app, int slot_id,
                                  void *ptr)
{
    uint slot = slot_id - 1;
    ASSERT(slot < als_slot_cnt);
    app->als[slot] = ptr;
}

static inline struct trusty_thread *current_trusty_thread(void)
{
    return (struct trusty_thread *)tls_get(TLS_ENTRY_TRUSTY);
}

static inline trusty_app_t *current_trusty_app(void)
{
    return current_trusty_thread()->app;
}

#endif
