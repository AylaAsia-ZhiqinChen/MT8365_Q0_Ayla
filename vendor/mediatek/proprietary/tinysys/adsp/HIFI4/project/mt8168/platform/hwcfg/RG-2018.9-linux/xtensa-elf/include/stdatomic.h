/*===---- stdatomic.h - Standard header for atomic types and operations -----===
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *===-----------------------------------------------------------------------===
 */

/* Adapted from the CLANG version of stdatomic.h */

#if (__STDC_VERSION__ >= 201112L)

#ifndef _STDATOMIC
#define _STDATOMIC

#include <stddef.h>
#include <stdint.h>

#include <xtensa/config/core.h>
#include <xtensa/tie/xt_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 7.17.1 Introduction */

#define ATOMIC_BOOL_LOCK_FREE       __GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE       __GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE   __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE   __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE    __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_T_LOCK_FREE    __GCC_ATOMIC_SHORT_T_LOCK_FREE
#define ATOMIC_INT_T_LOCK_FREE      __GCC_ATOMIC_INT_T_LOCK_FREE
#define ATOMIC_LONG_T_LOCK_FREE     __GCC_ATOMIC_LONG_T_LOCK_FREE
#define ATOMIC_LLONG_T_LOCK_FREE    __GCC_ATOMIC_LLONG_T_LOCK_FREE
#define ATOMIC_POINTER_T_LOCK_FREE  __GCC_ATOMIC_POINTER_T_LOCK_FREE

/* 7.17.2 Initialization */

#define ATOMIC_VAR_INIT(value) (value)
#define atomic_init __c11_atomic_init

/* 7.17.3 Order and consistency */

typedef enum memory_order {
  memory_order_relaxed = __ATOMIC_RELAXED,
  memory_order_consume = __ATOMIC_CONSUME,
  memory_order_acquire = __ATOMIC_ACQUIRE,
  memory_order_release = __ATOMIC_RELEASE,
  memory_order_acq_rel = __ATOMIC_ACQ_REL,
  memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

#define kill_dependency(y) (y)

/* 7.17.4 Fences */

void atomic_thread_fence(memory_order order);
void atomic_signal_fence(memory_order order);

#define atomic_thread_fence(order) __c11_atomic_thread_fence(order)
#define atomic_signal_fence(order) __c11_atomic_signal_fence(order)

/* 7.17.5 Lock-free property */

#define atomic_is_lock_free(obj)   __c11_atomic_is_lock_free(sizeof(*(obj)))

/* 7.17.6 Atomic integer types */

#ifdef __cplusplus
typedef _Atomic(bool)               atomic_bool;
#else
typedef _Atomic(_Bool)              atomic_bool;
#endif
typedef _Atomic(char)               atomic_char;
typedef _Atomic(signed char)        atomic_schar;
typedef _Atomic(unsigned char)      atomic_uchar;
typedef _Atomic(short)              atomic_short;
typedef _Atomic(unsigned short)     atomic_ushort;
typedef _Atomic(int)                atomic_int;
typedef _Atomic(unsigned int)       atomic_uint;
typedef _Atomic(long)               atomic_long;
typedef _Atomic(unsigned long)      atomic_ulong;
typedef _Atomic(long long)          atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(uint_least16_t)     atomic_char16_t;
typedef _Atomic(uint_least32_t)     atomic_char32_t;
typedef _Atomic(wchar_t)            atomic_wchar_t;
typedef _Atomic(int_least8_t)       atomic_int_least8_t;
typedef _Atomic(uint_least8_t)      atomic_uint_least8_t;
typedef _Atomic(int_least16_t)      atomic_int_least16_t;
typedef _Atomic(uint_least16_t)     atomic_uint_least16_t;
typedef _Atomic(int_least32_t)      atomic_int_least32_t;
typedef _Atomic(uint_least32_t)     atomic_uint_least32_t;
typedef _Atomic(int_least64_t)      atomic_int_least64_t;
typedef _Atomic(uint_least64_t)     atomic_uint_least64_t;
typedef _Atomic(int_fast8_t)        atomic_int_fast8_t;
typedef _Atomic(uint_fast8_t)       atomic_uint_fast8_t;
typedef _Atomic(int_fast16_t)       atomic_int_fast16_t;
typedef _Atomic(uint_fast16_t)      atomic_uint_fast16_t;
typedef _Atomic(int_fast32_t)       atomic_int_fast32_t;
typedef _Atomic(uint_fast32_t)      atomic_uint_fast32_t;
typedef _Atomic(int_fast64_t)       atomic_int_fast64_t;
typedef _Atomic(uint_fast64_t)      atomic_uint_fast64_t;
typedef _Atomic(intptr_t)           atomic_intptr_t;
typedef _Atomic(uintptr_t)          atomic_uintptr_t;
typedef _Atomic(size_t)             atomic_size_t;
typedef _Atomic(ptrdiff_t)          atomic_ptrdiff_t;
typedef _Atomic(intmax_t)           atomic_intmax_t;
typedef _Atomic(uintmax_t)          atomic_uintmax_t;

/* These should be provided by libc implementation or other library */

#if 0 /* Not required for CLANG */
extern char      __atomic_load_1(char *obj, int order);
extern short     __atomic_load_2(short *obj, int order);
extern int       __atomic_load_4(int *obj, int order);
extern long long __atomic_load_8(long long *obj, int order);

extern void      __atomic_store_1(char *obj, char val, int order);
extern void      __atomic_store_2(short *obj, short val, int order);
extern void      __atomic_store_4(int *obj, int val, int order);
extern void      __atomic_store_8(long long *obj, long long val, int order);

extern char      __atomic_exchange_1(char *obj, char val, int order);
extern short     __atomic_exchange_2(short *obj, short val, int order);
extern int       __atomic_exchange_4(int *arg, int val, int order);
extern long long __atomic_exchange_8(long long *arg, long long val, int order);

extern char      __atomic_compare_exchange_1(char *obj, char *exp, char val, int order1, int order2);
extern short     __atomic_compare_exchange_2(short *obj, short *exp, short val, int order1, int order2);
extern int       __atomic_compare_exchange_4(int *obj, int *exp, int val, int order1, int order2);
extern long long __atomic_compare_exchange_8(long long *obj, long long *exp, long long val, int order1, int order2);

extern char  __atomic_fetch_add_1(char *obj, char val, int order);
extern short __atomic_fetch_add_2(short *obj, short val, int order);
extern int   __atomic_fetch_add_4(int *obj, int val, int order);

extern char  __atomic_fetch_sub_1(char *obj, char val, int order);
extern short __atomic_fetch_sub_2(short *obj, short val, int order);
extern int   __atomic_fetch_sub_4(int *obj, int val, int order);

extern char  __atomic_fetch_and_1(char *obj, char val, int order);
extern short __atomic_fetch_and_2(short *obj, short val, int order);
extern int   __atomic_fetch_and_4(int *obj, int val, int order);

extern char  __atomic_fetch_or_1(char *obj, char val, int order);
extern short __atomic_fetch_or_2(short *obj, short val, int order);
extern int   __atomic_fetch_or_4(int *obj, int val, int order);

extern char  __atomic_fetch_xor_1(char *obj, char val, int order);
extern short __atomic_fetch_xor_2(short *obj, short val, int order);
extern int   __atomic_fetch_xor_4(int *obj, int val, int order);

extern void __atomic_load_generic(size_t size, void *ptr, void *ret, int order);
extern void __atomic_store_generic(size_t size, void *ptr, void *val, int order);
extern void __atomic_exchange_generic(size_t size, void *ptr, void *val, void *ret, int order);
extern int  __atomic_compare_exchange_generic(size_t size, void *ptr, void *exp, void *val, int success_order, int failure_order);
#endif

/* 7.17.7 Operations on atomic types */

#define atomic_store(object, desired) __c11_atomic_store(object, desired, __ATOMIC_SEQ_CST)
#define atomic_store_explicit __c11_atomic_store

#define atomic_load(object) __c11_atomic_load(object, __ATOMIC_SEQ_CST)
#define atomic_load_explicit __c11_atomic_load

#define atomic_exchange(object, desired) __c11_atomic_exchange(object, desired, __ATOMIC_SEQ_CST)
#define atomic_exchange_explicit __c11_atomic_exchange

#define atomic_compare_exchange_strong(object, expected, desired) __c11_atomic_compare_exchange_strong(object, expected, desired, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_strong_explicit __c11_atomic_compare_exchange_strong

#define atomic_compare_exchange_weak(object, expected, desired) __c11_atomic_compare_exchange_weak(object, expected, desired, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_weak_explicit __c11_atomic_compare_exchange_weak

#define atomic_fetch_add(object, operand) __c11_atomic_fetch_add(object, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_add_explicit __c11_atomic_fetch_add

#define atomic_fetch_sub(object, operand) __c11_atomic_fetch_sub(object, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_sub_explicit __c11_atomic_fetch_sub

#define atomic_fetch_or(object, operand) __c11_atomic_fetch_or(object, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_or_explicit __c11_atomic_fetch_or

#define atomic_fetch_xor(object, operand) __c11_atomic_fetch_xor(object, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_xor_explicit __c11_atomic_fetch_xor

#define atomic_fetch_and(object, operand) __c11_atomic_fetch_and(object, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_and_explicit __c11_atomic_fetch_and

#if 0 /* Not required for CLANG */
#define __atomic_load(obj, order) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_load_1((char *)obj, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_load_2((short *)obj, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(int)))       ? __atomic_load_4((int *)obj, order) : \
        __atomic_load_8((long long *)obj, order)

#define __atomic_store(obj, desired, order) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_store_1((char *)obj, desired, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_store_2((short *)obj, desired, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(int)))       ? __atomic_store_4((int *)obj, desired, order) : \
        __atomic_store_8((long long *)obj, desired, order)

#define __atomic_exchange(obj, desired, order) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_exchange_1((char *)obj, desired, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_exchange_2((short *)obj, desired, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(int)))       ? __atomic_exchange_4((int *)obj, desired, order) : \
        __atomic_exchange_8((long long *)obj, desired, order)

#define __atomic_compare_exchange(obj, expected, desired, order1, order2) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_compare_exchange_1((char *)obj, expected, desired, order1, order2) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_compare_exchange_2((short *)obj, expected, desired, order1, order2) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(int)))       ? __atomic_compare_exchange_4((int *)obj, expected. desired, order1, order2) : \
        __atomic_compare_exchange_8((long long *)obj, expected. desired, order1, order2)

#define __atomic_fetch_add(obj, val, order) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_fetch_add_1((char *)obj, val, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_fetch_add_2((short *)obj, val, order) : \
        __atomic_fetch_add_4((int *)obj, val, order)

#define __atomic_fetch_sub(obj, val, order) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_fetch_sub_1((char *)obj, val, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_fetch_sub_2((short *)obj, val, order) : \
        __atomic_fetch_sub_4((int *)obj, val, order)

#define __atomic_fetch_and(obj, val, order) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_fetch_and_1((char *)obj, val, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_fetch_and_2((short *)obj, val, order) : \
        __atomic_fetch_and_4((int *)obj, val, order)

#define __atomic_fetch_or(obj, val, order) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_fetch_or_1((char *)obj, val, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_fetch_or_2((short *)obj, val, order) : \
        __atomic_fetch_or_4((int *)obj, val, order)

#define __atomic_fetch_xor(obj, val, order) \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(char)))      ? __atomic_fetch_xor_1((char *)obj, val, order) : \
        (sizeof(__typeof__(*(obj))) == sizeof(__typeof__(short)))     ? __atomic_fetch_xor_2((short *)obj, val, order) : \
        __atomic_fetch_xor_4((int *)obj, val, order)
#endif

/* 7.17.8 Atomic flag type and operations */

typedef atomic_uint atomic_flag;

#define ATOMIC_FLAG_INIT { 0 }

/* These should be provided by the libc implementation. */
#ifdef __cplusplus
bool atomic_flag_test_and_set(volatile atomic_flag *);
bool atomic_flag_test_and_set_explicit(volatile atomic_flag *, memory_order);
#else
_Bool atomic_flag_test_and_set(volatile atomic_flag *);
_Bool atomic_flag_test_and_set_explicit(volatile atomic_flag *, memory_order);
#endif
void atomic_flag_clear(volatile atomic_flag *);
void atomic_flag_clear_explicit(volatile atomic_flag *, memory_order);

#define atomic_flag_test_and_set(object) __c11_atomic_exchange(object, 1, __ATOMIC_SEQ_CST)
#define atomic_flag_test_and_set_explicit(object, order) __c11_atomic_exchange(object, 1, order)

#define atomic_flag_clear(object) __c11_atomic_store(object, 0, __ATOMIC_SEQ_CST)
#define atomic_flag_clear_explicit(object, order) __c11_atomic_store(object, 0, order)

#ifdef __cplusplus
}
#endif

#endif /* _STDATOMIC */

#endif /* __STDC_VERSION__ */

