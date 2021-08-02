/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2010 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef	_SYS_FILE_H
#define	_SYS_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/fcntl.h>

/* Alternate names for values for the WHENCE argument to `lseek'.
   These are the same as SEEK_SET, SEEK_CUR, and SEEK_END, respectively. */
#ifndef L_SET
#define L_SET   0       /* Seek from beginning of file.  */
#define L_INCR  1       /* Seek from current position.  */
#define L_XTND  2       /* Seek from end of file.  */
#endif

#ifdef __cplusplus
}
#endif
#endif
