#ifndef __FIXBORING__H__
#define __FIXBORING__H__

// from inttypes.h
# if __WORDSIZE == 64
#  define __PRI64_PREFIX        "l"
#  define __PRIPTR_PREFIX       "l"
# else
#  define __PRI64_PREFIX        "ll"
#  define __PRIPTR_PREFIX
# endif
# define PRIu64         __PRI64_PREFIX "u"

#endif
