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

#ifndef _SYS_REENT_H
#define _SYS_REENT_H

#if __cplusplus
extern "C" {
#endif

/* To get mbstate_t and FOPEN_MAX */
#include <stdio.h>
/* To get wint_t */
#include <wchar.h>

/* asctime.c */
#define TBUF_SIZE       sizeof("Day Mon dd hh:mm:ss yyyy\n")

/* strerror.c */
#define INIT_STR        "error #xxx"
#define EBUF_SIZE       sizeof (INIT_STR)

/* rand.c */
#define RBUF_SIZE	32 /* must be power of two */


struct _reent
{
  int		_errno;            /* last error code */
  char *	_strtok_p;         /* strtok() location pointer */
  char		_tbuf[TBUF_SIZE];  /* asctime datetime buffer */
  char		_ebuf[EBUF_SIZE];  /* strerror buffer */

  /* Used by mbrlen.c and friends */
  mbstate_t	_mbrlen_state;
  mbstate_t	_mbrtowc_state;
  mbstate_t	_mbsrtowcs_state;
  mbstate_t	_mbtowc_state;
  mbstate_t	_wctomb_state;
  mbstate_t	_wcrtomb_state;
  mbstate_t	_wcsrtombs_state;

  /* Used by rand/srand. Keep these right at the end so we can skip
     allocating these if rand is not used in the application. */

  char		_Randinit;
  unsigned long	_Randseed;
  unsigned long	_idx;
  unsigned long	_rv[RBUF_SIZE];
};

#define _REENT_SIZE		sizeof(struct _reent)
#define _REENT_SIZE_SMALL	offsetof(struct _reent, _Randinit)

/* Global pointer to current _reent area */
extern struct _reent * _reent_ptr;

/* Inits the _reent structure */
extern void _init_reent(struct _reent * pr);

#if __cplusplus
}
#endif

#endif /* _SYS_REENT_H */

