/* xkeycheck.h internal header */
#ifndef _XKEYCHECK_H
#define _XKEYCHECK_H

 #if !defined(__cplusplus)

  #if defined(auto)	/* check C keywords */  \
	|| defined(break) \
	|| defined(case) \
	|| defined(char) \
	|| defined(const) \
	|| defined(continue) \
	|| defined(default) \
	|| defined(do) \
	|| defined(double) \
	|| defined(else) \
	|| defined(enum) \
	|| defined(extern) \
	|| defined(float) \
	|| defined(for) \
	|| defined(goto) \
	|| defined(if) \
	|| defined(inline) \
	|| defined(int) \
	|| defined(long) \
	|| defined(register) \
	|| defined(restrict) \
	|| defined(return) \
	|| defined(short) \
	|| defined(signed) \
	|| defined(sizeof) \
	|| defined(static) \
	|| defined(struct) \
	|| defined(switch) \
	|| defined(typedef) \
	|| defined(union) \
	|| defined(unsigned) \
	|| defined(void) \
	|| defined(volatile) \
	|| defined(while) \
	|| defined(_Bool) \
	|| defined(_Complex) \
	|| defined(_Imaginary)
   #error keyword defined before including C standard header
  #endif /* defined... */

 #else /* __cplusplus is defined */

  #if defined(alignas)	/* check C++ keywords */ \
	|| defined(alignof) \
	|| defined(asm) \
	|| defined(auto) \
	|| defined(bool) \
	|| defined(break) \
	|| defined(case) \
	|| defined(catch) \
	|| defined(char) \
	|| defined(char16_t) \
	|| defined(char32_t) \
	|| defined(class) \
	|| defined(const) \
	|| defined(const_cast) \
	|| defined(constexpr) \
	|| defined(continue) \
	|| defined(decltype) \
	|| defined(default) \
	|| defined(delete) \
	|| defined(do) \
	|| defined(double) \
	|| defined(dynamic_cast) \
	|| defined(else) \
	|| defined(enum) \
	|| defined(explicit) \
	|| defined(export) \
	|| defined(extern) \
	|| defined(false) \
	|| defined(float) \
	|| defined(for) \
	|| defined(friend) \
	|| defined(goto) \
	|| defined(if) \
	|| defined(inline) \
	|| defined(int) \
	|| defined(long) \
	|| defined(mutable) \
	|| defined(namespace) \
	|| defined(new) && defined(_ENFORCE_BAN_OF_MACRO_NEW) \
	|| defined(noexcept) \
	|| defined(nullptr) \
	|| defined(operator) \
	|| defined(private) \
	|| defined(protected) \
	|| defined(public) \
	|| defined(register) \
	|| defined(reinterpret_cast) \
	|| defined(return) \
	|| defined(short) \
	|| defined(signed) \
	|| defined(sizeof) \
	|| defined(static) \
	|| defined(static_assert) \
	|| defined(static_cast) \
	|| defined(struct) \
	|| defined(switch) \
	|| defined(template) \
	|| defined(this) \
	|| defined(thread_local) \
	|| defined(throw) \
	|| defined(true) \
	|| defined(try) \
	|| defined(typedef) \
	|| defined(typeid) \
	|| defined(typename) \
	|| defined(union) \
	|| defined(unsigned) \
	|| defined(using) \
	|| defined(virtual) \
	|| defined(void) \
	|| defined(volatile) \
	|| defined(wchar_t) \
	|| defined(while)
   #error keyword defined before including C++ standard header
  #endif /* defined... */

 #endif /* defined(__cplusplus) */
#endif /* _XKEYCHECK_H */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
