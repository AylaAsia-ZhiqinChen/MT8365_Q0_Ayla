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

FUNCTION(_trusty_write)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_write, %rax
    leaq .Lwrite_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lwrite_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_brk)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_brk, %rax
    leaq .Lbrk_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lbrk_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_exit_etc)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_exit_etc, %rax
    leaq .Lexit_etc_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lexit_etc_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_read)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_read, %rax
    leaq .Lread_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lread_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_ioctl)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_ioctl, %rax
    leaq .Lioctl_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lioctl_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_nanosleep)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_nanosleep, %rax
    leaq .Lnanosleep_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lnanosleep_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_gettime)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_gettime, %rax
    leaq .Lgettime_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lgettime_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_mmap)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_mmap, %rax
    leaq .Lmmap_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lmmap_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_munmap)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_munmap, %rax
    leaq .Lmunmap_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lmunmap_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_prepare_dma)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_prepare_dma, %rax
    leaq .Lprepare_dma_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lprepare_dma_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_finish_dma)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_finish_dma, %rax
    leaq .Lfinish_dma_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lfinish_dma_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_port_create)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_port_create, %rax
    leaq .Lport_create_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lport_create_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_connect)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_connect, %rax
    leaq .Lconnect_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lconnect_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_accept)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_accept, %rax
    leaq .Laccept_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Laccept_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_close)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_close, %rax
    leaq .Lclose_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lclose_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_set_cookie)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_set_cookie, %rax
    leaq .Lset_cookie_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lset_cookie_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_handle_set_create)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_handle_set_create, %rax
    leaq .Lhandle_set_create_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lhandle_set_create_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_handle_set_ctrl)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_handle_set_ctrl, %rax
    leaq .Lhandle_set_ctrl_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lhandle_set_ctrl_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_wait)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_wait, %rax
    leaq .Lwait_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lwait_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_wait_any)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_wait_any, %rax
    leaq .Lwait_any_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lwait_any_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_get_msg)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_get_msg, %rax
    leaq .Lget_msg_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lget_msg_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_read_msg)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_read_msg, %rax
    leaq .Lread_msg_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lread_msg_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_put_msg)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_put_msg, %rax
    leaq .Lput_msg_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lput_msg_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret

FUNCTION(_trusty_send_msg)
    pushfq
    pushq %rbp
    pushq %rbx
    pushq %r15
    movq $__NR_send_msg, %rax
    leaq .Lsend_msg_sysreturn(%rip), %rbx
    movq %rsp, %rbp
    sysenter
.Lsend_msg_sysreturn:
    popq %r15
    popq %rbx
    popq %rbp
    popfq
    ret
