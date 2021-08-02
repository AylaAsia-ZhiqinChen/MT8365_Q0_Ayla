/*
 * Copyright (c) 2012-2018 LK Trusty Authors. All Rights Reserved.
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

/* This file is auto-generated. !!! DO NOT EDIT !!! */

#include <asm.h>
#include <trusty_syscalls.h>

.section .text._trusty_write
FUNCTION(_trusty_write)
    ldr     r12, =__NR_write
    swi     #0
    bx      lr

.section .text._trusty_brk
FUNCTION(_trusty_brk)
    ldr     r12, =__NR_brk
    swi     #0
    bx      lr

.section .text._trusty_exit_etc
FUNCTION(_trusty_exit_etc)
    ldr     r12, =__NR_exit_etc
    swi     #0
    bx      lr

.section .text._trusty_read
FUNCTION(_trusty_read)
    ldr     r12, =__NR_read
    swi     #0
    bx      lr

.section .text._trusty_ioctl
FUNCTION(_trusty_ioctl)
    ldr     r12, =__NR_ioctl
    swi     #0
    bx      lr

.section .text._trusty_nanosleep
FUNCTION(_trusty_nanosleep)
    ldr     r12, =__NR_nanosleep
    swi     #0
    bx      lr

.section .text._trusty_gettime
FUNCTION(_trusty_gettime)
    ldr     r12, =__NR_gettime
    swi     #0
    bx      lr

.section .text._trusty_mmap
FUNCTION(_trusty_mmap)
    ldr     r12, =__NR_mmap
    swi     #0
    bx      lr

.section .text._trusty_munmap
FUNCTION(_trusty_munmap)
    ldr     r12, =__NR_munmap
    swi     #0
    bx      lr

.section .text._trusty_prepare_dma
FUNCTION(_trusty_prepare_dma)
    ldr     r12, =__NR_prepare_dma
    swi     #0
    bx      lr

.section .text._trusty_finish_dma
FUNCTION(_trusty_finish_dma)
    ldr     r12, =__NR_finish_dma
    swi     #0
    bx      lr

.section .text._trusty_port_create
FUNCTION(_trusty_port_create)
    ldr     r12, =__NR_port_create
    swi     #0
    bx      lr

.section .text._trusty_connect
FUNCTION(_trusty_connect)
    ldr     r12, =__NR_connect
    swi     #0
    bx      lr

.section .text._trusty_accept
FUNCTION(_trusty_accept)
    ldr     r12, =__NR_accept
    swi     #0
    bx      lr

.section .text._trusty_close
FUNCTION(_trusty_close)
    ldr     r12, =__NR_close
    swi     #0
    bx      lr

.section .text._trusty_set_cookie
FUNCTION(_trusty_set_cookie)
    ldr     r12, =__NR_set_cookie
    swi     #0
    bx      lr

.section .text._trusty_handle_set_create
FUNCTION(_trusty_handle_set_create)
    ldr     r12, =__NR_handle_set_create
    swi     #0
    bx      lr

.section .text._trusty_handle_set_ctrl
FUNCTION(_trusty_handle_set_ctrl)
    ldr     r12, =__NR_handle_set_ctrl
    swi     #0
    bx      lr

.section .text._trusty_wait
FUNCTION(_trusty_wait)
    ldr     r12, =__NR_wait
    swi     #0
    bx      lr

.section .text._trusty_wait_any
FUNCTION(_trusty_wait_any)
    ldr     r12, =__NR_wait_any
    swi     #0
    bx      lr

.section .text._trusty_get_msg
FUNCTION(_trusty_get_msg)
    ldr     r12, =__NR_get_msg
    swi     #0
    bx      lr

.section .text._trusty_read_msg
FUNCTION(_trusty_read_msg)
    ldr     r12, =__NR_read_msg
    swi     #0
    bx      lr

.section .text._trusty_put_msg
FUNCTION(_trusty_put_msg)
    ldr     r12, =__NR_put_msg
    swi     #0
    bx      lr

.section .text._trusty_send_msg
FUNCTION(_trusty_send_msg)
    ldr     r12, =__NR_send_msg
    swi     #0
    bx      lr
