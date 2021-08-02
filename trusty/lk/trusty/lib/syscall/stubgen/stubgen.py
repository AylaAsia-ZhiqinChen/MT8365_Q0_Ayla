#!/usr/bin/python

# Copyright 2013-2017 Google Inc. +All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import re
import sys
from optparse import OptionParser

"""
Script to generate syscall stubs from a syscall table file
with definitions like this:

DEF_SYSCALL(nr_syscall, syscall_name, return_type, nr_args, arg_list...)

For e.g.,
DEF_SYSCALL(0x3, read, 3, int fd, void *buf, int size)
DEF_SYSCALL(0x4, write, 4, int fd, void *buf, int size)

FUNCTION(read)
    ldr     r12, =__NR_read
    swi     #0
    bx      lr

FUNCTION(write)
    ldr     r12, =__NR_write
    swi     #0
    bx      lr

Another file with a enumeration of all syscalls is also generated:

#define __NR_read  0x3
#define __NR_write 0x4
...


Only syscalls with 4 or less arguments are supported.
"""

copyright_header = """/*
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
"""

autogen_header = """
/* This file is auto-generated. !!! DO NOT EDIT !!! */

"""
clang_format_off = "/* clang-format off */\n\n"


includes_header = "#include <%s>\n"

# header file defines macros for asm
# see include/asm.h in lk
asm_header = "asm.h"

class Architecture:
    def __init__(self, syscall_stub):
        self.syscall_stub = syscall_stub

arch_dict = {
    "arm" : Architecture (
        syscall_stub = """
.section .text._trusty_%(sys_fn)s
FUNCTION(_trusty_%(sys_fn)s)
    ldr     r12, =__NR_%(sys_fn)s
    swi     #0
    bx      lr
"""),
    "x86_64" : Architecture (
        syscall_stub = """
FUNCTION(_trusty_%(sys_fn)s)
    pushfq
    pushq %%rbp
    pushq %%rbx
    pushq %%r15
    movq $__NR_%(sys_fn)s, %%rax
    leaq .L%(sys_fn)s_sysreturn(%%rip), %%rbx
    movq %%rsp, %%rbp
    sysenter
.L%(sys_fn)s_sysreturn:
    popq %%r15
    popq %%rbx
    popq %%rbp
    popfq
    ret
"""),
}

syscall_define = "#define __NR_%(sys_fn)s\t\t%(sys_nr)s\n"

syscall_proto = "%(sys_rt)s _trusty_%(sys_fn)s(%(sys_args)s);\n"

asm_ifdef = "\n#ifndef ASSEMBLY\n"
asm_endif = "\n#endif\n"

beg_cdecls = "\n__BEGIN_CDECLS\n"
end_cdecls = "\n__END_CDECLS\n"

syscall_def = "DEF_SYSCALL"

syscall_pat = (
    r'DEF_SYSCALL\s*\('
    r'\s*(?P<sys_nr>\d+|0x[\da-fA-F]+)\s*,'      # syscall nr
    r'\s*(?P<sys_fn>\w+)\s*,'                    # syscall name
    r'\s*(?P<sys_rt>[\w*\s]+)\s*,'               # return type
    r'\s*(?P<sys_nr_args>\d+)\s*'                # nr ags
    r'('
    r'\)\s*$|'                                   # empty arg list or
    r',\s*(?P<sys_args>[\w,*\s]+)'               # arg list
    r'\)\s*$'
    r')')

syscall_re = re.compile(syscall_pat)


def fatal_parse_error(line, err_str):
    sys.stderr.write("Error processing line %r:\n%s\n" % (line, err_str))
    sys.exit(2)


BUILTIN_TYPES = set(['char', 'int', 'long', 'void'])
for i in [8, 16, 32, 64]:
    BUILTIN_TYPES.add('int%d_t' % i)
    BUILTIN_TYPES.add('uint%d_t' % i)


def parse_check_def(line, struct_types):
    """
    Parse a DEF_SYSCALL line and check for errors
    Returns various components from the line.
    """

    m = syscall_re.match(line)
    if m is None:
        fatal_parse_error(line, "Line did not match expected pattern.")
    gd = m.groupdict()

    sys_nr_args = int(gd['sys_nr_args'])
    sys_args = gd['sys_args']
    sys_args_list = re.split(r'\s*,\s*', sys_args) if sys_args else []

    if sys_nr_args > 4:
        fatal_parse_error(line, "Only syscalls with up to 4 arguments are "
                          "supported.")

    if sys_nr_args != len(sys_args_list):
        fatal_parse_error(line, "Expected %d syscall arguments, got %d." %
                          (sys_nr_args, len(sys_args_list)))

    # Find struct types in the arguments.
    for arg in sys_args_list:
        # Remove arg name.
        arg = re.sub(r"\s*\w+$", "", arg)
        # Remove trailing pointer.
        arg = re.sub(r"\s*\*$", "", arg)
        # Remove initial const.
        arg = re.sub(r"^const\s+", "", arg)
        # Ignore the type if it's obviously not a struct.
        if arg in BUILTIN_TYPES:
            continue
        # Require explicit struct declarations, because forward declaring
        # typedefs is tricky.
        if not arg.startswith("struct "):
            fatal_parse_error(line, "Not an integer type or explicit struct "
                              "type: %r. Don't use typedefs." % arg)
        struct_types.add(arg)

    # In C, a forward declaration with an empty list of arguments has an
    # unknown number of arguments. Set it to 'void' to declare there are
    # zero arguments.
    if sys_nr_args == 0:
        gd['sys_args'] = 'void'

    return gd


def process_table(table_file, std_file, stubs_file, verify, arch):
    """
    Process a syscall table and generate:
    1. A sycall stubs file
    2. A trusty_std.h header file with syscall definitions
       and function prototypes
    """
    define_lines = ""
    proto_lines = "\n"
    stub_lines = ""

    struct_types = set()

    tbl = open(table_file, "r")
    for line in tbl:
        line = line.strip()

        # skip all lines that don't start with a syscall definition
        # multi-line defintions are not supported.
        if not line.startswith(syscall_def):
            continue

        params = parse_check_def(line, struct_types)

        if not verify:
            define_lines += syscall_define % params
            proto_lines += syscall_proto % params
            stub_lines += arch.syscall_stub % params


    tbl.close()

    if verify:
        return

    if std_file is not None:
        with open(std_file, "w") as std:
            std.writelines(copyright_header + autogen_header)
            std.writelines(clang_format_off)
            std.writelines(define_lines + asm_ifdef)
            std.writelines("\n")
            std.writelines(includes_header % "stdint.h")
            std.writelines(beg_cdecls)
            # Forward declare the struct types.
            std.writelines("\n")
            std.writelines([t + ";\n" for t in sorted(struct_types)])
            std.writelines(proto_lines + end_cdecls + asm_endif)

    if stubs_file is not None:
        with open(stubs_file, "w") as stubs:
            stubs.writelines(copyright_header + autogen_header)
            stubs.writelines(includes_header % asm_header)
            stubs.writelines(includes_header % "trusty_syscalls.h")
            stubs.writelines(stub_lines)


def main():

    usage = "usage:  %prog [options] <syscall-table>"

    op = OptionParser(usage=usage)
    op.add_option("-v", "--verify", action="store_true",
            dest="verify", default=False,
            help="Sanity check syscall table. Do not generate any files.")
    op.add_option("-d", "--std-header", type="string",
            dest="std_file", default=None,
            help="path to syscall defintions header file.")
    op.add_option("-s", "--stubs-file", type="string",
            dest="stub_file", default=None,
            help="path to syscall assembly stubs file.")
    op.add_option("-a", "--arch", type="string",
            dest="arch", default="arm",
            help="arch of stub assembly files: " + str(arch_dict.keys()))

    (opts, args) = op.parse_args()

    if len(args) == 0:
        op.print_help()
        sys.exit(1)

    if not opts.verify:
        if opts.std_file is None and opts.stub_file is None:
            op.print_help()
            sys.exit(1)

    process_table(args[0], opts.std_file, opts.stub_file, opts.verify,
            arch_dict[opts.arch])


if __name__ == '__main__':
    main()
