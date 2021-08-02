/* xthreads.h -- internal header for threads library */
#ifndef _THREADS_XTHREADS_H
#define _THREADS_XTHREADS_H
#include <c99/Dinkum/threads/xthrcommon.h>
#include <c99/Dinkum/threads/xtimec.h>
#include <limits.h>


_C_LIB_DECL
enum {	/* return codes */
	_Thrd_success,
	_Thrd_nomem,
	_Thrd_timedout,
	_Thrd_busy,
	_Thrd_error
	};

	/* threads */
typedef _Thrd_imp_t _Thrd_t;
typedef int (*_Thrd_start_t)(void *);

_CRTIMP2P int _CDECL _Thrd_create(_Thrd_t *, _Thrd_start_t, void *);

 #if _HAS_STRICT_LINKAGE
 #ifdef __cplusplus
extern "C++" {	// C++ linkage
inline int _Thrd_create(_Thrd_t *_Thrd, _Thrd_callback_t _Fp, void *_Arg)
	{	/* create thread */
	return (_Thrd_create(_Thrd, (_Thrd_callback_t)_Fp, _Arg));
	}
}	// extern "C++"
 #endif /* defined(__cplusplus) */
 #else
 #endif /* _HAS_STRICT_LINKAGE */

_CRTIMP2P int _CDECL _Thrd_detach(_Thrd_t);
_CRTIMP2P void _CDECL _Thrd_exit(int);
_CRTIMP2P int _CDECL _Thrd_join(_Thrd_t, int *);
_CRTIMP2P void _CDECL _Thrd_sleep(const xtime*);
_CRTIMP2P void _CDECL _Thrd_yield(void);
_CRTIMP2P int _CDECL _Thrd_equal(_Thrd_t, _Thrd_t);
_CRTIMP2P int _CDECL _Thrd_lt(_Thrd_t, _Thrd_t);
_CRTIMP2P _Thrd_t _CDECL _Thrd_current(void);

	/* mutexes */
typedef _Mtx_imp_t _Mtx_t;
enum {	/* mutex types */
	_Mtx_plain = 0x01,
	_Mtx_try	= 0x02,
	_Mtx_timed	= 0x04,
	_Mtx_recursive = 0x100
	};

_CRTIMP2P int _CDECL _Mtx_init(_Mtx_t *, int);
_CRTIMP2P void _CDECL _Mtx_destroy(_Mtx_t *);
_CRTIMP2P int _CDECL _Mtx_current_owns(_Mtx_t *);
_CRTIMP2P int _CDECL _Mtx_lock(_Mtx_t *);
_CRTIMP2P int _CDECL _Mtx_trylock(_Mtx_t *);
_CRTIMP2P int _CDECL _Mtx_timedlock(_Mtx_t *, const xtime *);
_CRTIMP2P int _CDECL _Mtx_unlock(_Mtx_t *);


	/* condition variables */
typedef _Cnd_imp_t _Cnd_t;

_CRTIMP2P int _CDECL _Cnd_init(_Cnd_t *);
_CRTIMP2P void _CDECL _Cnd_destroy(_Cnd_t *);
_CRTIMP2P int _CDECL _Cnd_wait(_Cnd_t *, _Mtx_t *);
_CRTIMP2P int _CDECL _Cnd_timedwait(_Cnd_t *, _Mtx_t *, const xtime *);
_CRTIMP2P int _CDECL _Cnd_broadcast(_Cnd_t *);
_CRTIMP2P int _CDECL _Cnd_signal(_Cnd_t *);
_CRTIMP2P int _CDECL _Cnd_register_at_thread_exit(_Cnd_t*, _Mtx_t*, int*);
_CRTIMP2P int _CDECL _Cnd_unregister_at_thread_exit(_Mtx_t *);
_CRTIMP2P int _CDECL _Cnd_do_broadcast_at_thread_exit(void);

	/* thread specific storage */
typedef _Tss_imp_t _Tss_t;
typedef void (*_Tss_dtor_t)(void *);
_CRTIMP2P int _CDECL _Tss_create(_Tss_t *, _Tss_dtor_t);

 #if _HAS_STRICT_LINKAGE
 #ifdef __cplusplus
extern "C++" {	// C++ linkage
typedef void (*_Tss_dtor_cpp_t)(void *);

inline int _Tss_create(_Tss_t *_Key, _Tss_dtor_cpp_t _Dtor)
	{	/* create thread-specific storage */
	return (_Tss_create(_Key, (_Tss_dtor_t)_Dtor));
	}
}	// extern "C++"
 #endif /* defined(__cplusplus) */
 #else
 #endif /* _HAS_STRICT_LINKAGE */

_CRTIMP2P int _CDECL _Tss_delete(_Tss_t);
_CRTIMP2P int _CDECL _Tss_set(_Tss_t, void *);
_CRTIMP2P void *_CDECL _Tss_get(_Tss_t);

#define _TSS_DTOR_ITERATIONS	_TSS_DTOR_ITERATIONS_IMP

 #if _TSS_USE_MACROS
  #define _Tss_create(key, dtor)    _TSS_CREATE(key, dtor)
  #define _Tss_delete(key)		    _TSS_DELETE(key)
  #define _Tss_set(key, value)	    _TSS_SET(key, value)
  #define _Tss_get(key)			    _TSS_GET(key)
 #endif	/* TSS_USE_MACROS */

	/* once functions */
#define _ONCE_FLAG_INIT		_ONCE_FLAG_INIT_IMP
#define _ONCE_FLAG_CPP_INIT	0

typedef _Once_flag_imp_t _Once_flag;
typedef int _Once_flag_cpp;
typedef void (_CDECL *_Once_callback_t)(void);
typedef void (_CDECL *_Once_callbackEx_t)(void *);

_CRTIMP2P void _CDECL _Call_once(_Once_flag *, _Once_callback_t);
_CRTIMP2P void _CDECL _Call_onceEx(_Once_flag_cpp *, _Once_callbackEx_t,
	void *);

 #if _HAS_STRICT_LINKAGE
 #ifdef __cplusplus
extern "C++" {	// C++ linkage
typedef void (*_Once_callback_cpp_t)(void);
inline void _Call_once(_Once_flag *_Flag, _Once_callback_cpp_t _Fp)
	{	/* call function just once */
	_Call_once(_Flag, (_Once_callback_t)_Fp);
	}
}	// extern "C++"
 #endif /* defined(__cplusplus) */
 #else
 #endif /* _HAS_STRICT_LINKAGE */

	/* utility functions */
_CRTIMP2P void _CDECL _Thrd_abort(const char *);
_END_C_LIB_DECL
#endif	/* _THREADS_XTHREADS_H */

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * (c) Copyright William E. Kempf 2001
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation. William E. Kempf makes no representations
 * about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
