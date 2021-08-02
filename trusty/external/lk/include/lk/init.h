/*
 * Copyright (c) 2013-2015 Travis Geiselbrecht
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
#pragma once

#include <compiler.h>
#include <sys/types.h>

/*
 * LK's init system
 */

typedef void (*lk_init_hook)(uint level);

enum lk_init_level {
    LK_INIT_LEVEL_EARLIEST = 1,

    LK_INIT_LEVEL_ARCH_EARLY     = 0x10000,
    LK_INIT_LEVEL_PLATFORM_EARLY = 0x20000,
    LK_INIT_LEVEL_TARGET_EARLY   = 0x30000,
    LK_INIT_LEVEL_HEAP           = 0x40000,
    LK_INIT_LEVEL_VM             = 0x50000,
    LK_INIT_LEVEL_KERNEL         = 0x60000,
    LK_INIT_LEVEL_THREADING      = 0x70000,
    LK_INIT_LEVEL_ARCH           = 0x80000,
    LK_INIT_LEVEL_PLATFORM       = 0x90000,
    LK_INIT_LEVEL_TARGET         = 0xa0000,
    LK_INIT_LEVEL_APPS           = 0xb0000,

    LK_INIT_LEVEL_LAST = UINT_MAX,
};

/**
 * enum lk_init_flags - Flags specifying init hook type.
 *
 * Flags passed to LK_INIT_HOOK_FLAGS to specify when the hook should be called.
 */
enum lk_init_flags {
    /**
     * @LK_INIT_FLAG_PRIMARY_CPU: Call init hook when booting primary CPU.
     */
    LK_INIT_FLAG_PRIMARY_CPU     = 0x1,

    /**
     * @LK_INIT_FLAG_SECONDARY_CPUS: Call init hook when booting secondary CPUs.
     */
    LK_INIT_FLAG_SECONDARY_CPUS  = 0x2,

    /**
     * @LK_INIT_FLAG_ALL_CPUS: Call init hook when booting any CPU.
     */
    LK_INIT_FLAG_ALL_CPUS        = LK_INIT_FLAG_PRIMARY_CPU | LK_INIT_FLAG_SECONDARY_CPUS,

    /**
     * @LK_INIT_FLAG_CPU_ENTER_IDLE: Call init hook before a CPU enters idle.
     *
     * The CPU may lose state after this, but it should respond to interrupts.
     */
    LK_INIT_FLAG_CPU_ENTER_IDLE  = 0x4,

    /**
     * @LK_INIT_FLAG_CPU_OFF: Call init hook before a CPU goes offline.
     *
     * The CPU may lose state after this, and it should not respond to
     * interrupts.
     */
    LK_INIT_FLAG_CPU_OFF         = 0x8,

    /**
     * @LK_INIT_FLAG_CPU_SUSPEND: Call init hook before a CPU loses state.
     *
     * Alias to call hook for both LK_INIT_FLAG_CPU_ENTER_IDLE and
     * LK_INIT_FLAG_CPU_OFF events.
     */
    LK_INIT_FLAG_CPU_SUSPEND     = LK_INIT_FLAG_CPU_ENTER_IDLE | LK_INIT_FLAG_CPU_OFF,

    /**
     * @LK_INIT_FLAG_CPU_EXIT_IDLE: Call init hook after a CPU exits idle.
     *
     * LK_INIT_FLAG_CPU_ENTER_IDLE should have been called before this.
     */
    LK_INIT_FLAG_CPU_EXIT_IDLE   = 0x10,

    /**
     * @LK_INIT_FLAG_CPU_ON: Call init hook after a CPU turns on.
     *
     * LK_INIT_FLAG_CPU_OFF should have been called before this. The first time
     * a CPU turns on LK_INIT_FLAG_PRIMARY_CPU or LK_INIT_FLAG_SECONDARY_CPUS
     * is called instead of this.
     */
    LK_INIT_FLAG_CPU_ON          = 0x20,

    /**
     * @LK_INIT_FLAG_CPU_RESUME: Call init hook after a CPU exits idle.
     *
     * Alias to call hook for both LK_INIT_FLAG_CPU_EXIT_IDLE and
     * LK_INIT_FLAG_CPU_ON events.
     */
    LK_INIT_FLAG_CPU_RESUME      = LK_INIT_FLAG_CPU_EXIT_IDLE | LK_INIT_FLAG_CPU_ON,
};

void lk_init_level(enum lk_init_flags flags, uint start_level, uint stop_level);

static inline void lk_primary_cpu_init_level(uint start_level, uint stop_level)
{
    lk_init_level(LK_INIT_FLAG_PRIMARY_CPU, start_level, stop_level);
}

static inline void lk_init_level_all(enum lk_init_flags flags)
{
    lk_init_level(flags, LK_INIT_LEVEL_EARLIEST, LK_INIT_LEVEL_LAST);
}

struct lk_init_struct {
    uint level;
    uint flags;
    lk_init_hook hook;
    const char *name;
};

#if MODULE_STATIC_LIB
#define LK_INIT_HOOK_FLAGS(a,b,c,d) _Pragma("GCC error \"init hooks are not fully compatible with static libraries\"")
#else
#define LK_INIT_HOOK_FLAGS(_name, _hook, _level, _flags) \
    const struct lk_init_struct _init_struct_##_name __ALIGNED(sizeof(void *)) __SECTION(".lk_init") = { \
        .level = _level, \
        .flags = _flags, \
        .hook = _hook, \
        .name = #_name, \
    };
#endif

#define LK_INIT_HOOK(_name, _hook, _level) \
    LK_INIT_HOOK_FLAGS(_name, _hook, _level, LK_INIT_FLAG_PRIMARY_CPU)
