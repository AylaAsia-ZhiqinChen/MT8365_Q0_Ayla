/* stdlib.h standard header */

#if __cplusplus < 201103
#include <c99/stdlib.h>
#else /* rest of file */

#ifndef _STDLIB
#define _STDLIB
#ifndef _YVALS
 #include <yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
		/* MACROS */
#ifndef NULL
 #define NULL	_NULL
#endif /* NULL */

#if defined(__XTENSA__)
 #include <alloca.h>
#endif /* __XTENSA__ */

#define EXIT_FAILURE	_EXFAIL
#define EXIT_SUCCESS	0

#define MB_CUR_MAX	_CSTD _Getmbcurmax()

 #if _ILONG
  #define RAND_MAX	0x3fffffff

 #else /* _ILONG */
  #define RAND_MAX	0x7fff
 #endif /* _ILONG */

 #ifndef _NO_RETURN
  #define _NO_RETURN(fun)	void fun
  #define _NO_RETURN_NOEXCEPT(fun)	_NO_RETURN(fun)
 #endif /* _NO_RETURN */

		/* TYPE DEFINITIONS */

 #if !defined(_SIZE_T) && !defined(_SIZET) && !defined(_SIZE_T_DEFINED) \
	&& !defined(_BSD_SIZE_T_DEFINED_)
  #define _SIZE_T
  #define _SIZET
  #define _BSD_SIZE_T_DEFINED_
  #define _SIZE_T_DEFINED
  #define _STD_USING_SIZE_T
typedef _Sizet size_t;
 #endif /* !defined(_SIZE_T) etc. */

 #if !defined(_WCHART) && !defined(_WCHAR_T_DEFINED)
  #define _WCHART
  #define _WCHAR_T_DEFINED
typedef _Wchart wchar_t;
 #endif /* _WCHART etc. */

typedef struct
	{	/* result of int divide */
	int quot;
	int rem;
	} div_t;

typedef struct
	{	/* result of long divide */
	long quot;
	long rem;
	} ldiv_t;

 #ifndef _LLDIV_T
typedef struct
	{	/* result of long long divide */
	_Longlong quot;
	_Longlong rem;
	} _Lldiv_t;
 #endif /* _LLDIV_T */

 #if _HAS_C9X
typedef _Lldiv_t lldiv_t;
 #endif /* _IS_C9X */

		/* DECLARATIONS */
_EXTERN_C /* low-level functions */
_NO_RETURN(exit(int));

_NO_RETURN_NOEXCEPT(_Exit(int));	/* added with C99 */
char *getenv(const char *);
int system(const char *);
_END_EXTERN_C

_C_LIB_DECL
int abs(int);
long labs(long);

_NO_RETURN_NOEXCEPT(abort(void));
void *calloc(size_t, size_t);
div_t div(int, int);
void free(void *);
ldiv_t ldiv(long, long);
void *malloc(size_t);
int mblen(const char *, size_t);
size_t mbstowcs(wchar_t *_Restrict,
	const char *_Restrict, size_t);
int mbtowc(wchar_t *_Restrict, const char *_Restrict, size_t);
int rand(void);
void srand(unsigned int);
void *realloc(void *, size_t);
long strtol(const char *_Restrict, char **_Restrict, int);
size_t wcstombs(char *_Restrict,
	const wchar_t *_Restrict, size_t);
int wctomb(char *, wchar_t);

 #if _ADDED_C_LIB
void lcong48(unsigned short [7]);
unsigned short *seed48(unsigned short [3]);
void srand48(long);

double drand48(void);
long lrand48(void);
long mrand48(void);

double erand48(unsigned short [3]);
long nrand48(unsigned short [3]);
long jrand48(unsigned short [3]);

int getopt(int, char *const *, const char *);
void *memalign(size_t, size_t);

 #if 1800 <= _MSC_VER
char *mktemp(char *);

 #else /* 1800 <= _MSC_VER */
char *mktemp(char *);
 #endif /* 1800 <= _MSC_VER */

int putenv(char *);
int rand_r(unsigned *);
char *tempnam(const char *, const char *);

extern char *optarg;
extern int optind, opterr, optopt;
 #endif /* _ADDED_C_LIB */

_C_STD_BEGIN
_C_LIB_DECL
extern float _Stofx(const char *, char **,
	long, int *);
extern double _Stodx(const char *, char **,
	long, int *);
extern long double _Stoldx(const char *, char **,
	long, int *);
extern long _Stolx(const char *, char **,
	int, int *);
extern unsigned long _Stoulx(const char *, char **,
	int, int *);
extern _LONGLONG _Stollx(const char *, char **,
	int, int *);
extern _ULONGLONG _Stoullx(const char *, char **,
	int, int *);
_END_C_LIB_DECL
_C_STD_END

unsigned long _Stoul(const char *, char **, int);
float _Stof(const char *, char **, long);
double _Stod(const char *, char **, long);
long double _Stold(const char *, char **, long);
_Longlong _Stoll(const char *, char **, int);
_ULonglong _Stoull(const char *, char **, int);

float _Stofx(const char *, char **, long, int *);
double _Stodx(const char *, char **, long, int *);
long double _Stoldx(const char *, char **, long, int *);
long _Stolx(const char *, char **, int, int *);
unsigned long _Stoulx(const char *, char **, int, int *);
_Longlong _Stollx(const char *, char **, int, int *);
_ULonglong _Stoullx(const char *, char **, int, int *);

size_t _Getmbcurmax(void);

 #if _HAS_C9X
_Longlong llabs(_Longlong);
lldiv_t lldiv(_Longlong, _Longlong);
 #endif /* _IS_C9X */

 #if _HAS_CPP0X || __STDC_VERSION__ >= 201112L
int at_quick_exit(void (*)(void)) _NOEXCEPT;
_NO_RETURN_NOEXCEPT(quick_exit(int));
void *(aligned_alloc)(size_t alignment, size_t size);
 #endif /* _HAS_CPP0X */

_END_C_LIB_DECL

 #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
_EXTERN_C
typedef void _Atexfun(void);
typedef int _Cmpfun(const void *, const void *);

int atexit(void (*)(void)) _NOEXCEPT;
void *bsearch(const void *, const void *,
	size_t, size_t, _Cmpfun *);
void qsort(void *, size_t, size_t, _Cmpfun *);

_END_EXTERN_C

extern "C++" {
  #if _HAS_STRICT_LINKAGE
typedef int _Cmpfun2(const void *, const void *);

inline int atexit(void (*_Pfn)(void)) _NOEXCEPT
	{	// register a function to call at exit
	return (atexit((_Atexfun *)_Pfn));
	}

inline void *bsearch(const void *_Key, const void *_Base,
	size_t _Nelem, size_t _Size, _Cmpfun2 *_Cmp)
	{	// search by binary chop
	return (bsearch(_Key, _Base, _Nelem, _Size, (_Cmpfun *)_Cmp));
	}

inline void qsort(void *_Base,
	size_t _Nelem, size_t _Size, _Cmpfun2 *_Cmp)
	{	// sort
	qsort(_Base, _Nelem, _Size, (_Cmpfun *)_Cmp);
	}

 #if _HAS_CPP0X
inline int at_quick_exit(void (*_Pfn)(void)) _NOEXCEPT
	{	// register a function to call at exit
	return (at_quick_exit((_Atexfun *)_Pfn));
	}
 #endif /* _HAS_CPP0X */

  #endif /* _HAS_STRICT_LINKAGE */

		// INLINES, FOR C++
_C_LIB_DECL

inline double atof(const char *_Str)
	{	// convert string to double
	return (_Stod(_Str, 0, 0));
	}

inline int atoi(const char *_Str)
	{	// convert string to int
	return ((int)_Stoul(_Str, 0, 10));
	}

inline long atol(const char *_Str)
	{	// convert string to long
	return ((long)_Stoul(_Str, 0, 10));
	}

 #if defined(__APPLE__) && 3 <= __GNUC__	/* compiler test */
double strtod(const char *_Restrict,
	char **_Restrict);
unsigned long strtoul(const char *_Restrict,
	char **_Restrict, int);

 #else /* defined(__APPLE__) && 3 <= __GNUC__ */
inline double strtod(const char *_Restrict _Str,
	char **_Restrict _Endptr)
	{	// convert string to double, with checking
	return (_Stod(_Str, _Endptr, 0));
	}

inline unsigned long strtoul(const char *_Restrict _Str,
	char **_Restrict _Endptr, int _Base)
	{	// convert string to unsigned long, with checking
	return (_Stoul(_Str, _Endptr, _Base));
	}
 #endif /* defined(__APPLE__) && 3 <= __GNUC__ */

 #if _HAS_C9X
inline _Longlong atoll(const char *_Str)
	{	// convert string to long long
	return ((_Longlong)_Stoull(_Str, 0, 10));
	}

inline float strtof(const char *_Restrict _Str,
	char **_Restrict _Endptr)
	{	// convert string to float, with checking
	return (_Stof(_Str, _Endptr, 0));
	}

inline long double strtold(const char *_Restrict _Str,
	char **_Restrict _Endptr)
	{	// convert string to long double, with checking
	return (_Stold(_Str, _Endptr, 0));
	}

inline _Longlong strtoll(const char *_Restrict _Str,
	char **_Restrict _Endptr, int _Base)
	{	// convert string to long long, with checking
	return (_Stoll(_Str, _Endptr, _Base));
	}

inline _ULonglong strtoull(const char *_Restrict _Str,
	char **_Restrict _Endptr, int _Base)
	{	// convert string to unsigned long long, with checking
	return (_Stoull(_Str, _Endptr, _Base));
	}
 #endif /* _IS_C9X */

_END_C_LIB_DECL

		// OVERLOADS, FOR C++

  #if !defined(_LLDIV_T) || defined(_LONGLONG)
inline long abs(long _Left)
	{	// compute abs
	return (labs(_Left));
	}

inline ldiv_t div(long _Left, long _Right)
	{	// compute quotient and remainder
	return (ldiv(_Left, _Right));
	}
  #endif /* !defined(_LLDIV_T) || defined(_LONGLONG) */

 #if _HAS_C9X

  #if !defined(_LLDIV_T) && defined(_LONGLONG)
inline _Longlong abs(_Longlong _Left)
	{	// compute abs
	return (llabs(_Left));
	}

inline _Lldiv_t div(_Longlong _Left, _Longlong _Right)
	{	// compute quotient and remainder
	return (lldiv(_Left, _Right));
	}
  #endif /* !defined(_LLDIV_T) && defined(_LONGLONG) */

 #endif /* _IS_C9X */
}	// extern "C++"

 #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
_C_LIB_DECL
		/* DECLARATIONS AND MACRO OVERRIDES, FOR C */
typedef int _Cmpfun(const void *, const void *);

/* */ int atexit(void (*)(void)) _NOEXCEPT;
void *bsearch(const void *, const void *,
	size_t, size_t, _Cmpfun *);
void qsort(void *, size_t, size_t, _Cmpfun *);

double atof(const char *);
int atoi(const char *);
long atol(const char *);
double strtod(const char *_Restrict, char **_Restrict);
unsigned long strtoul(const char *_Restrict,
	char **_Restrict, int);

#define atof(str)	_Stod(str, 0, 0)
#define atoi(str)	(int)_Stoul(str, 0, 10)
#define atol(str)	(long)_Stoul(str, 0, 10)
#define strtod(str, endptr)	_Stod(str, endptr, 0)
#define strtoul(str, endptr, base)	_Stoul(str, endptr, base)

 #if _HAS_C9X
_Longlong atoll(const char *);
float strtof(const char *_Restrict,
	char **_Restrict);
long double strtold(const char *_Restrict,
	char **_Restrict);
_Longlong strtoll(const char *_Restrict,
	char **_Restrict, int);
_ULonglong strtoull(const char *_Restrict,
	char **_Restrict, int);

#define atoll(str)	(_Longlong)_Stoull(str, 0, 10)
#define strtof(str, endptr)	_Stof(str, endptr, 0)
#define strtold(str, endptr)	_Stold(str, endptr, 0)
#define strtoll(str, endptr, base)	_Stoll(str, endptr, base)
#define strtoull(str, endptr, base)	_Stoull(str, endptr, base)
 #endif /* _IS_C9X */

_END_C_LIB_DECL
 #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

 #ifndef _LLDIV_T
  #define _LLDIV_T
 #endif /* _LLDIV_T */

 #if __STDC_WANT_LIB_EXT1__
_C_LIB_DECL
 #define _EXT1_FAIL(mesg)	_Fail_s(mesg, sizeof (mesg))

int (_Fail_s)(const char *, size_t);

  #if !defined(_ERRNO_T_DEFINED)
   #define _ERRNO_T_DEFINED
typedef int errno_t;
  #endif /* _ERRNO_T_DEFINED */

  #if !defined(_RSIZE_T_DEFINED)
   #define _RSIZE_T_DEFINED
typedef size_t rsize_t;
  #endif /* _RSIZE_T_DEFINED */

typedef void (*constraint_handler_t)(const char *_Restrict,
	void *_Restrict, errno_t);

constraint_handler_t set_constraint_handler_s(
	constraint_handler_t);
void abort_handler_s(const char *_Restrict,
	void *_Restrict, errno_t);
void ignore_handler_s(const char *_Restrict,
	void *_Restrict, errno_t);

errno_t getenv_s(size_t *_Restrict, char *_Restrict,
	rsize_t, const char *_Restrict);
_END_C_LIB_DECL

  #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
_EXTERN_C
typedef int _Cmpfun_s(const void *, const void *, void *);

void *bsearch_s(const void *, const void *,
	rsize_t, rsize_t, _Cmpfun_s *, void *);
errno_t qsort_s(void *,
	rsize_t, rsize_t, _Cmpfun_s *, void *);
_END_EXTERN_C

extern "C++" {
   #if _HAS_STRICT_LINKAGE
typedef int _Cmpfun2_s(const void *, const void *, void *);

inline void *bsearch_s(const void *_Key, const void *_Base,
	rsize_t _Nelem, rsize_t _Size, _Cmpfun2_s *_Cmp, void *_Context)
	{	// search by binary chop
	return (bsearch_s(_Key, _Base, _Nelem, _Size, (_Cmpfun_s *)_Cmp,
		_Context));
		}

inline errno_t qsort_s(void *_Base,
	size_t _Nelem, rsize_t _Size, _Cmpfun2_s *_Cmp, void *_Context)
	{	// sort
	return (qsort_s(_Base, _Nelem, _Size, (_Cmpfun_s *)_Cmp, _Context));
	}
   #endif /* _HAS_STRICT_LINKAGE */
}	// extern "C++"

  #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
_C_LIB_DECL
typedef int _Cmpfun_s(const void *, const void *, void *);

void *bsearch_s(const void *, const void *,
	rsize_t, rsize_t, _Cmpfun_s *, void *);
errno_t qsort_s(void *,
	rsize_t, rsize_t, _Cmpfun_s *, void *);
_END_C_LIB_DECL
  #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

_C_LIB_DECL
errno_t wctomb_s(int *_Restrict,
	char *_Restrict, rsize_t,
	wchar_t);
errno_t mbstowcs_s(size_t *_Restrict,
	wchar_t *_Restrict, rsize_t,
	const char *_Restrict, rsize_t);
errno_t wcstombs_s(size_t *_Restrict,
	char *_Restrict, rsize_t,
	const wchar_t *_Restrict, rsize_t);
_END_C_LIB_DECL
 #endif /* __STDC_WANT_LIB_EXT1__ */

#ifndef _EXT1_FAIL
 #define _EXT1_FAIL(mesg)	(-1)
#endif /* _EXT1_FAIL */
_C_STD_END
#endif /* _STDLIB */

 #if defined(_STD_USING)

  #ifdef _STD_USING_SIZE_T
using _CSTD size_t;
  #endif /* _STD_USING_SIZE_T */

using _CSTD div_t; using _CSTD ldiv_t;

using _CSTD abort; using _CSTD abs; using _CSTD atexit;
using _CSTD atof; using _CSTD atoi; using _CSTD atol;
using _CSTD bsearch; using _CSTD calloc; using _CSTD div;
using _CSTD exit; using _CSTD free; using _CSTD getenv;
using _CSTD labs; using _CSTD ldiv; using _CSTD malloc;
using _CSTD mblen; using _CSTD mbstowcs; using _CSTD mbtowc;
using _CSTD qsort; using _CSTD rand; using _CSTD realloc;
using _CSTD srand; using _CSTD strtod; using _CSTD strtol;
using _CSTD strtoul; using _CSTD system;
using _CSTD wcstombs; using _CSTD wctomb;

 #if _HAS_C9X
using _CSTD lldiv_t;

using _CSTD atoll; using _CSTD llabs; using _CSTD lldiv;
using _CSTD strtof; using _CSTD strtold;
using _CSTD strtoll; using _CSTD strtoull;

 #if !defined(__clang__)
using _CSTD _Exit;
 #endif /* !defined(__clang) */

 #endif /* _IS_C9X */

 #if _HAS_CPP0X
using _CSTD at_quick_exit;
using _CSTD quick_exit;
 #endif /* _HAS_CPP0X */

 #if __STDC_WANT_LIB_EXT1__
using _CSTD errno_t;
using _CSTD rsize_t;
using _CSTD constraint_handler_t;

using _CSTD set_constraint_handler_s;
using _CSTD abort_handler_s;
using _CSTD ignore_handler_s;
using _CSTD getenv_s;
using _CSTD bsearch_s;
using _CSTD qsort_s;
using _CSTD wctomb_s;
using _CSTD mbstowcs_s;
using _CSTD wcstombs_s;
 #endif /* __STDC_WANT_LIB_EXT1__ */

 #endif /* defined(_STD_USING) */

#endif /* __cplusplus < 201103 */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
