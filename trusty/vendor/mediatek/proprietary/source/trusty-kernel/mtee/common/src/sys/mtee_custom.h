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

/* Define custom part. */

#ifndef __MTEE_CUSTOM_H__
#define __MTEE_CUSTOM_H__

#define VIRT_OFFSET (0xffffffffc0000000)
struct tee_memory_config {
        unsigned long phys_memory_base;      // Start of secure memory
        unsigned int total_memory_size;      // Total secure memory (including chunk)
        unsigned int chunk_buffer_size;      // Chunk memory size.
        unsigned long phys_boot_share;       // Pysical memory for boot shared memory.
        unsigned int version;                // Memory config version
};
extern struct tee_memory_config tee_memory_config;

struct tee_secure_func_config {
        unsigned int stack_num;    // stack number
        unsigned int stack_size;    // stack size
};
extern const struct tee_secure_func_config tee_secure_func_config;

struct tee_tzmem_release_config {
        unsigned int cm_size;    // chunkmem release size
};
extern const struct tee_tzmem_release_config tee_tzmem_release_config;


#endif /* __MTEE_CUSTOM_H__ */
