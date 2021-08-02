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

#ifndef	_GRP_H
#define	_GRP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

  /* Newlib provides all these declarations, but not implementations.
     We will do likewise. */

struct group
{
  char   *gr_name;
  gid_t   gr_gid;
  char  **gr_mem;
};

struct group  *getgrgid(gid_t);
struct group  *getgrnam(const char *);
int            getgrgid_r(gid_t, struct group *, char *,
                   size_t, struct group **);
int            getgrnam_r(const char *, struct group *, char *,
                   size_t , struct group **);
struct group  *getgrent(void);
void           endgrent(void);
void           setgrent(void);

#ifdef __cplusplus
}
#endif

