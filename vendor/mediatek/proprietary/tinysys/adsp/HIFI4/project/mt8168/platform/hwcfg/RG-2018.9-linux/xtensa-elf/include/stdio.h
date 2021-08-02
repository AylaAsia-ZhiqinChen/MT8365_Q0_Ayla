/* stdio.h standard header */

#if __cplusplus < 201103
#include <c99/stdio.h>
#else /* rest of file */

#ifndef _STDIO
#define _STDIO
#ifndef _YVALS
 #include <yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
 #ifndef _HAS_DINKUM_CLIB
  #define _HAS_DINKUM_CLIB	1
 #endif /* _HAS_DINKUM_CLIB */

 #if defined(_WIN32_C_LIB) && _WIN32_C_LIB != 0	/* compiler test */ \
	&& !defined(__BORLANDC__) && !defined(__MINGW32__)
  #define fflush	_DNK_fflush	/* kludge for VC++ */
 #endif /* defined(_WIN32_C_LIB) */

		/* MACROS */
#ifndef NULL
 #define NULL	_NULL
#endif /* NULL */

#define _IOFBF		0
#define _IOLBF		1
#define _IONBF		2

#define BUFSIZ		512
#define EOF			(-1)
#define FILENAME_MAX	_FNAMAX
#define FOPEN_MAX		_FOPMAX
#define L_tmpnam		_TNAMAX
#define TMP_MAX			32

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define stdin		(&_CSTD _Stdin)
#define stdout		(&_CSTD _Stdout)
#define stderr		(&_CSTD _Stderr)

 #if _MULTI_THREAD
  #define _Lockfile(str)	_Lockfilelock(str)
  #define _Unlockfile(str)	_Unlockfilelock(str)

 #else /* _MULTI_THREAD */
  #define _Lockfile(x)		(void)0
  #define _Unlockfile(x)	(void)0
 #endif /* _MULTI_THREAD */

		/* TYPE DEFINITIONS */
#ifndef _MBSTATET
#include <mbstatet.h>
#endif /* _MBSTATET */

 #if !defined(_SIZE_T) && !defined(_SIZET) && !defined(_SIZE_T_DEFINED) \
	&& !defined(_BSD_SIZE_T_DEFINED_)
  #define _SIZE_T
  #define _SIZET
  #define _BSD_SIZE_T_DEFINED_
  #define _SIZE_T_DEFINED
  #define _STD_USING_SIZE_T
typedef _Sizet size_t;
 #endif /* !defined(_SIZE_T) etc. */

typedef struct fpos_t
	{	/* file position */
	_Longlong _Off;	/* can be system dependent */
	_Mbstatet _Wstate;
	} fpos_t;

 #define _FPOSOFF(fp)	((fp)._Off)

struct _Dnk_filet
	{	/* file control information */
	unsigned short _Mode;
	unsigned char _Idx;
	_FD_TYPE _Handle;

	unsigned char *_Buf, *_Bend, *_Next;
	unsigned char *_Rend, *_Wend, *_Rback;

	_Wchart *_WRback, _WBack[2];
	unsigned char *_Rsave, *_WRend, *_WWend;

	_Mbstatet _Wstate;
	char *_Tmpnam;
	unsigned char _Back[_MBMAX], _Cbuf;
	};

 #ifndef _FILET
  #define _FILET
typedef struct _Dnk_filet _Filet;
 #endif /* _FILET */

typedef _Filet FILE;

		/* declarations */
_C_LIB_DECL
extern FILE _Stdin, _Stdout, _Stderr;

void clearerr(FILE *);
int fclose(FILE *);
int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
int fgetc(FILE *);
int fgetpos(FILE *_Restrict, fpos_t *_Restrict);
char *fgets(char *_Restrict, int, FILE *_Restrict);
FILE *fopen(const char *_Restrict, const char *_Restrict);

 #if __EDG__	/* compiler test */
  #pragma __printf_args
 #endif /* __EDG__ */

int fprintf(FILE *_Restrict, const char *_Restrict, ...);
int fputc(int, FILE *);
int fputs(const char *_Restrict, FILE *_Restrict);
size_t fread(void *_Restrict, size_t, size_t, FILE *_Restrict);
FILE *freopen(const char *_Restrict, const char *_Restrict,
	FILE *_Restrict);

 #if __EDG__	/* compiler test */
  #pragma __scanf_args
 #endif /* __EDG__ */

int fscanf(FILE *_Restrict , const char *_Restrict, ...);
int fseek(FILE *, long, int);
int fsetpos(FILE *, const fpos_t *);
long ftell(FILE *);
size_t fwrite(const void *_Restrict, size_t, size_t,
	FILE *_Restrict);
char *gets(char *);
void perror(const char *);

 #if __EDG__	/* compiler test */
  #pragma __printf_args
 #endif /* __EDG__ */

int printf(const char *_Restrict, ...);
int puts(const char *);

 #if 1800 <= _MSC_VER
int remove(_In_z_ const char *);
int rename(_In_z_ const char *, const char *);

 #else /* 1800 <= _MSC_VER */
int remove(const char *);
int rename(const char *, const char *);
 #endif /* 1800 <= _MSC_VER */

void rewind(FILE *);

 #if __EDG__	/* compiler test */
  #pragma __scanf_args
 #endif /* __EDG__ */

int scanf(const char *_Restrict, ...);
void setbuf(FILE *_Restrict , char *_Restrict);
int setvbuf(FILE *_Restrict , char *_Restrict, int, size_t);

 #if __EDG__	/* compiler test */
  #pragma __printf_args
 #endif /* __EDG__ */

int sprintf(char *_Restrict, const char *_Restrict, ...);

 #if __EDG__	/* compiler test */
  #pragma __scanf_args
 #endif /* __EDG__ */

int sscanf(const char *_Restrict, const char *_Restrict, ...);
FILE *tmpfile(void);
char *tmpnam(char *);
int ungetc(int, FILE *);
int vfprintf(FILE *_Restrict, const char *_Restrict, _Va_list);
int vprintf(const char *_Restrict, _Va_list);
int vsprintf(char *_Restrict, const char *_Restrict, _Va_list);

 #if _ADDED_C_LIB
FILE *fdopen(_FD_TYPE, const char *);
int fileno(FILE *);
int getw(FILE *);
int putw(int, FILE *);
 #endif /* _ADDED_C_LIB */

long _Fgpos(FILE *, fpos_t *);
int _Flocale(FILE *, const char *, int);
void _Fsetlocale(FILE *, int);
int _Fspos(FILE *, const fpos_t *, long, int);

 #if _MULTI_THREAD
void _Lockfilelock(_Filet *);
void _Unlockfilelock(_Filet *);
 #endif /* _MULTI_THREAD */

extern FILE *_Files[FOPEN_MAX];

 #if _HAS_C9X

 #if __EDG__	/* compiler test */
  #pragma __printf_args
 #endif /* __EDG__ */

int snprintf(char *_Restrict, size_t,
	const char *_Restrict, ...);
int vsnprintf(char *_Restrict, size_t,
	const char *_Restrict, _Va_list);
int vfscanf(FILE *_Restrict,
	const char *_Restrict, _Va_list);
int vscanf(const char *_Restrict, _Va_list);
int vsscanf(const char *_Restrict,
	const char *_Restrict, _Va_list);

 #else /* _HAS_C9X */

 #if _ADDED_C_LIB

  #if __EDG__	/* compiler test */
   #pragma __printf_args
  #endif /* __EDG__ */

int snprintf(char *_Restrict, size_t,
	const char *_Restrict, ...);
int vsnprintf(char *_Restrict, size_t,
	const char *_Restrict, _Va_list);
 #endif /* _ADDED_C_LIB */

 #endif /* _IS_C9X */
_END_C_LIB_DECL

 #if _MULTI_THREAD && _FILE_OP_LOCKS
		/* declarations only */
_C_LIB_DECL
int getc(FILE *);
int getchar(void);
int putc(int, FILE *);
int putchar(int);
_END_C_LIB_DECL

 #else /* _MULTI_THREAD && _FILE_OP_LOCKS */

  #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
		// INLINES, for C++

 #if !defined(__GNUC__)	/* compiler test */
_C_LIB_DECL
 #endif /* !defined(__GNUC__) */

inline int getc(FILE *_Str)
	{	// get a character
	return ((_Str->_Next < _Str->_Rend
		? *_Str->_Next++ : fgetc(_Str)));
		}

inline int getchar()
	{	// get a character from stdin
	return ((_Files[0]->_Next < _Files[0]->_Rend
	? *_Files[0]->_Next++ : fgetc(_Files[0])));
	}

inline int putc(int _Ch, FILE *_Str)
	{	// put a character
	return ((_Str->_Next < _Str->_Wend
		? (*_Str->_Next++ = (char)_Ch) : fputc(_Ch, _Str)));
		}

inline int putchar(int _Ch)
	{	// put a character to stdout
	return ((_Files[1]->_Next < _Files[1]->_Wend
	? (*_Files[1]->_Next++ = (char)_Ch) : fputc(_Ch, _Files[1])));
	}

 #if !defined(__GNUC__)	/* compiler test */
_END_C_LIB_DECL
 #endif /* !defined(__GNUC__) */

  #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
		/* declarations and macro overrides, for C */
_C_LIB_DECL
int getc(FILE *);
int getchar(void);
int putc(int, FILE *);
int putchar(int);
_END_C_LIB_DECL

 #define getc(str)	((str)->_Next < (str)->_Rend \
	? *(str)->_Next++ : (fgetc)(str))

 #define getchar()	(_Files[0]->_Next < _Files[0]->_Rend \
	? *_Files[0]->_Next++ : (fgetc)(_Files[0]))

 #define putc(ch, str)	((str)->_Next < (str)->_Wend \
	? (*(str)->_Next++ = ch) : (fputc)(ch, str))

 #define putchar(ch)	(_Files[1]->_Next < _Files[1]->_Wend \
	? (*_Files[1]->_Next++ = ch) : (fputc)(ch, _Files[1]))
  #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

 #endif /* _MULTI_THREAD && _FILE_OP_LOCKS */

 #if __STDC_WANT_LIB_EXT1__
  #define L_tmpnam_s	L_tmpnam
  #define TMP_MAX_S		TMP_MAX

  #if TMP_MAX_S < 25
   #error TMP_MAX_S too small
  #endif /* TMP_MAX_S < 25 */

_C_LIB_DECL

  #if !defined(_ERRNO_T_DEFINED)
   #define _ERRNO_T_DEFINED
typedef int errno_t;
  #endif /* _ERRNO_T_DEFINED */

  #if !defined(_RSIZE_T_DEFINED)
   #define _RSIZE_T_DEFINED
typedef size_t rsize_t;
  #endif /* _RSIZE_T_DEFINED */

errno_t tmpfile_s(FILE *_Restrict *_Restrict);
errno_t tmpnam_s(char *, rsize_t);

errno_t fopen_s(FILE *_Restrict *_Restrict,
	const char *_Restrict, const char *_Restrict);
errno_t freopen_s(FILE *_Restrict *_Restrict,
	const char *_Restrict, const char *_Restrict, FILE *_Restrict);

int fprintf_s(FILE *_Restrict, const char *_Restrict, ...);
int fscanf_s(FILE *_Restrict, const char *_Restrict, ...);
int printf_s(const char *_Restrict, ...);
int scanf_s(const char *_Restrict, ...);
int snprintf_s(char *_Restrict, rsize_t,
	const char *_Restrict, ...);
int sscanf_s(const char *_Restrict,
	const char *_Restrict, ...);
int sprintf_s(char *_Restrict, rsize_t,
	const char *_Restrict, ...);
int vfprintf_s(FILE *_Restrict,
	const char *_Restrict,
	_Va_list);
int vfscanf_s(FILE *_Restrict,
	const char *_Restrict,
	_Va_list);
int vprintf_s(const char *_Restrict,
	_Va_list);
int vscanf_s(const char *_Restrict,
	_Va_list);
int vsnprintf_s(char *_Restrict, rsize_t,
	const char *_Restrict,
	_Va_list);
int vsprintf_s(char *_Restrict, rsize_t,
	const char *_Restrict,
	_Va_list);
int vsscanf_s(const char *_Restrict,
	const char *_Restrict,
	_Va_list);

char *gets_s(char *, rsize_t);
_END_C_LIB_DECL
 #endif /* __STDC_WANT_LIB_EXT1__ */
_C_STD_END
#include <xtensa/tie/xt_scmpr.h>
#endif /* _STDIO */

 #if defined(_STD_USING)
using _CSTD _Filet; using _CSTD _Mbstatet;

  #ifdef _STD_USING_SIZE_T
using _CSTD size_t;
  #endif /* _STD_USING_SIZE_T */

using _CSTD fpos_t; using _CSTD FILE;
using _CSTD clearerr; using _CSTD fclose; using _CSTD feof;
using _CSTD ferror; using _CSTD fflush; using _CSTD fgetc;
using _CSTD fgetpos; using _CSTD fgets; using _CSTD fopen;
using _CSTD fprintf; using _CSTD fputc; using _CSTD fputs;
using _CSTD fread; using _CSTD freopen; using _CSTD fscanf;
using _CSTD fseek; using _CSTD fsetpos; using _CSTD ftell;
using _CSTD fwrite; using _CSTD getc; using _CSTD getchar;
using _CSTD gets; using _CSTD perror;
using _CSTD putc; using _CSTD putchar;
using _CSTD printf; using _CSTD puts; using _CSTD remove;
using _CSTD rename; using _CSTD rewind; using _CSTD scanf;
using _CSTD setbuf; using _CSTD setvbuf; using _CSTD sprintf;
using _CSTD sscanf; using _CSTD tmpfile; using _CSTD tmpnam;
using _CSTD ungetc; using _CSTD vfprintf; using _CSTD vprintf;
using _CSTD vsprintf;

 #if _HAS_C9X
using _CSTD snprintf; using _CSTD vsnprintf;
using _CSTD vfscanf; using _CSTD vscanf; using _CSTD vsscanf;

 #else /* _HAS_C9X */

 #if _ADDED_C_LIB
using _CSTD snprintf; using _CSTD vsnprintf;
 #endif /* _ADDED_C_LIB */

 #endif /* _IS_C9X */

 #if _ADDED_C_LIB
using _CSTD fdopen; using _CSTD fileno; using _CSTD getw; using _CSTD putw;
 #endif /* _ADDED_C_LIB */

 #if __STDC_WANT_LIB_EXT1__
using _CSTD errno_t;
using _CSTD rsize_t;

using _CSTD tmpfile_s;
using _CSTD tmpnam_s;
using _CSTD fopen_s;
using _CSTD freopen_s;
using _CSTD fprintf_s;
using _CSTD fscanf_s;
using _CSTD printf_s;
using _CSTD scanf_s;
using _CSTD snprintf_s;
using _CSTD sscanf_s;
using _CSTD sprintf_s;
using _CSTD vfprintf_s;
using _CSTD vfscanf_s;
using _CSTD vprintf_s;
using _CSTD vscanf_s;
using _CSTD vsnprintf_s;
using _CSTD vsprintf_s;
using _CSTD vsscanf_s;
using _CSTD gets_s;
 #endif /* __STDC_WANT_LIB_EXT1__ */



#endif /* STDIO */

#endif /* __cplusplus < 201103 */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
