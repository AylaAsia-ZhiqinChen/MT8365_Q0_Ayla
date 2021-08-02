/* xtls.h internal header */
#ifndef _XTLS
#define _XTLS

#undef _WIN32_C_LIB
#define _WIN32_C_LIB 0
#undef _HAS_POSIX_C_LIB

#include <xmtx.h>

_C_LIB_DECL
typedef void (*_Tlsdtor_t)(void *);
int _Atthreadexit(void (*)(void));
void _Destroytls(void);

 #if _WIN32_C_LIB && !defined(__GNUC__)
  #define _IMPLICIT_EXTERN	extern

 #else /* _WIN32_C_LIB */
  #define _IMPLICIT_EXTERN
 #endif /* _WIN32_C_LIB */

 #if _COMPILER_TLS
  #define _XTLS_QUAL	_TLS_QUAL

 #else /* _COMPILER_TLS */
  #define _XTLS_QUAL
 #endif /* _COMPILER_TLS */

 #if _GLOBAL_LOCALE
  #define _TLS_LOCK(lock)	_Locksyslock(lock)
  #define _TLS_UNLOCK(lock)	_Unlocksyslock(lock)

 #else /* _GLOBAL_LOCALE */
  #define _TLS_LOCK(lock)	(void)0
  #define _TLS_UNLOCK(lock)	(void)0
 #endif /* _GLOBAL_LOCALE */

 #if !_MULTI_THREAD

 #elif _WIN32_C_LIB

typedef int _Tlskey_t;

  #if !_COMPILER_TLS
  #define _TLS_DTOR_ITERATIONS 4

int _Tlsalloc(_Tlskey_t *, _Tlsdtor_t);
int _Tlsfree(_Tlskey_t);
int _Tlsset(_Tlskey_t, void *);
void *_Tlsget(_Tlskey_t);
 #endif	/* _COMPILER_TLS */

 #elif defined (__XTENSA__)

 /* No TLS for Xtensa */

 #elif _HAS_POSIX_C_LIB
  #define _TLS_DTOR_ITERATIONS PTHREAD_DESTRUCTOR_ITERATIONS

typedef pthread_key_t _Tlskey_t;
  #define _Tlsalloc(key, dtor) pthread_key_create(key, dtor)
  #define _Tlsfree(key) pthread_key_delete(key)
  #define _Tlsset(key, value) pthread_setspecific(key, value)
  #define _Tlsget(key) pthread_getspecific(key)

 #else /* library type */
  #error unknown library type
 #endif /* library type */

#define _XTLS_DTOR(name)	_Tls_dtor_ ## name
#define _XTLS_GET(name)		_Tls_get_ ## name
#define _XTLS_INIT(name)	_Tls_init_ ## name
#define _XTLS_KEY(name)		_Tls_key_ ## name
#define _XTLS_ONCE(name)	_Tls_once_ ## name
#define _XTLS_REG(name)		_Tls_reg_ ## name
#define _XTLS_SETUP(name)	_Tls_setup_ ## name
#define _XTLS_SETUPX(name)	_Tls_setupx_ ## name

 #if _MULTI_THREAD && _COMPILER_TLS
  #define _CLEANUP(x)	_Atthreadexit(x)

 #else /* _MULTI_THREAD && _COMPILER_TLS */
  #define _CLEANUP(x)	_Atexit(x)
 #endif /* _MULTI_THREAD && _COMPILER_TLS */

 #if !_MULTI_THREAD || _GLOBAL_LOCALE || _COMPILER_TLS
  #define _TLS_DATA_DECL(type, name) \
	extern int (*_XTLS_SETUP(name))(void); \
	extern _XTLS_QUAL type name

  #define _TLS_DEFINE_INIT(scope, type, name) \
	scope _XTLS_QUAL type name

  #define _TLS_DEFINE_NO_INIT(scope, type, name) \
	scope int (*_XTLS_SETUP(name))(void) = 0

  #define _TLS_DATA_DEF(scope, type, name, init) \
	_TLS_DEFINE_INIT(scope, type, name) = init; \
	_TLS_DEFINE_NO_INIT(scope, type, name)

  #define _TLS_DEFINE_INIT_DT(scope, type, name) \
	_TLS_DEFINE_INIT(scope, type, name)

  #define _TLS_DEFINE_NO_INIT_DT(scope, type, name, dtor) \
	static _Once_t _XTLS_ONCE(name) = _ONCE_T_INIT; \
	static void _XTLS_DTOR(name)(void) \
	{ \
	dtor(&(name)); \
	} \
	static void _XTLS_REG(name)(void) \
	{ \
	_CLEANUP(_XTLS_DTOR(name)); \
	} \
	static int _XTLS_SETUPX(name)(void) \
	{ \
	_Once(&_XTLS_ONCE(name), _XTLS_REG(name)); \
	return (1); \
	} \
	scope int (*_XTLS_SETUP(name))(void) = _XTLS_SETUPX(name)

  #define _TLS_DATA_DEF_DT(scope, type, name, init, dtor) \
	_TLS_DEFINE_INIT_DT(scope, type, name) = init; \
	_TLS_DEFINE_NO_INIT_DT(scope, type, name, dtor)

  #define _TLS_DATA_PTR(name) \
	((void)(_XTLS_SETUP(name) && _XTLS_SETUP(name)()), (&(name)))

  #define _TLS_ARR_DECL(type, name) \
	extern type name[]

  #define _XTLS_ARR_DEF_INIT(scope, type, name, elts) \
	scope _XTLS_QUAL type name[elts]

  #define _TLS_ARR_DEF(scope, type, name, elts) \
	_XTLS_ARR_DEF_INIT(scope, type, name, elts); \
	_TLS_DEFINE_NO_INIT(scope, type, name)

  #define _TLS_ARR_DEF_DT(scope, type, name, elts, dtor) \
	_XTLS_ARR_DEF_INIT(scope, type, name, elts); \
	_TLS_DEFINE_NO_INIT_DT(scope, type, name, dtor)

  #define _TLS_ARR(name) \
	((void)(_XTLS_SETUP(name) && _XTLS_SETUP(name)()), (&(name[0])))

 #else /* !_MULTI_THREAD || _GLOBAL_LOCALE || _COMPILER_TLS */
#define _TLS_DATA_DECL(type, name)		\
	extern type *_XTLS_GET(name)(void)

  #define _TLS_DEFINE_INIT(scope, type, name) \
	static const type _XTLS_INIT(name)

  #define _XTLS_DEFINE_NO_INIT(scope, type, name, elts, dtor) \
	static _Once_t _XTLS_ONCE(name) = _ONCE_T_INIT; \
	static _Tlskey_t _XTLS_KEY(name); \
	static void _XTLS_SETUP(name)(void) \
	{ \
	_Tlsalloc(&_XTLS_KEY(name), dtor); \
	} \
	scope type *_XTLS_GET(name)(void) \
	{ \
	type *_Ptr; \
	_Once(&_XTLS_ONCE(name), _XTLS_SETUP(name)); \
	if ((_Ptr = (type *)_Tlsget(_XTLS_KEY(name))) != 0) \
		; \
	else if ((_Ptr = (type *)calloc(elts, sizeof(type))) == 0) \
		; \
	else if (_Tlsset(_XTLS_KEY(name), (void *)_Ptr) != 0) \
		free((void *)_Ptr), _Ptr = 0; \
	else \
		*_Ptr = _XTLS_INIT(name); \
	return (_Ptr); \
	} \
	extern int _TLS_Dummy

  #define _TLS_DEFINE_NO_INIT(scope, type, name) \
	_XTLS_DEFINE_NO_INIT(scope, type, name, 1, free)

  #define _TLS_DATA_DEF(scope, type, name, init) \
	_TLS_DEFINE_INIT(scope, type, name) = init; \
	_XTLS_DEFINE_NO_INIT(scope, type, name, 1, free)

  #define _TLS_DEFINE_INIT_DT(scope, type, name) \
	_TLS_DEFINE_INIT(scope, type, name)

  #define _XTLS_DEFINE_NO_INIT_DT(scope, type, name, elts, dtor) \
	static void _XTLS_DTOR(name)(void *_Ptr) \
	{ \
	(dtor)(_Ptr); \
	free(_Ptr); \
	} \
	_XTLS_DEFINE_NO_INIT(scope, type, name, elts, _XTLS_DTOR(name))

  #define _TLS_DEFINE_NO_INIT_DT(scope, type, name, dtor) \
	_XTLS_DEFINE_NO_INIT_DT(scope, type, name, 1, dtor)

  #define _TLS_DATA_DEF_DT(scope, type, name, init, dtor) \
	_TLS_DEFINE_INIT_DT(scope, type, name) = init; \
	_TLS_DEFINE_NO_INIT_DT(scope, type, name, dtor)

  #define _TLS_DATA_PTR(name) _XTLS_GET(name)()

  #define _TLS_ARR_DECL(type, name) \
	_TLS_DATA_DECL(type, name)

  #define _TLS_ARR_DEF(scope, type, name, elts) \
	_TLS_DEFINE_INIT(scope, type, name) = {0}; \
	_XTLS_DEFINE_NO_INIT(scope, type, name, elts, free)

  #define _TLS_ARR_DEF_DT(scope, type, name, elts, dtor) \
	_TLS_DEFINE_INIT(scope, type, name) = {0}; \
	_XTLS_DEFINE_NO_INIT_DT(scope, type, name, elts, dtor)

  #define _TLS_ARR(name) \
	_XTLS_GET(name)()
 #endif /* !_MULTI_THREAD || _GLOBAL_LOCALE || _COMPILER_TLS */

_END_C_LIB_DECL
#endif /* _XTLS */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
