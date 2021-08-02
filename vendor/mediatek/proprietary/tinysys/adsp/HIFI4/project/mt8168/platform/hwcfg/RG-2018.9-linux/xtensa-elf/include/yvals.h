/* yvals.h internal header for conforming compilers on various systems */

#if __cplusplus > 0
  #if __cplusplus >= 201103
    #include <c++11/yvals.h>
  #else
    #include <c++98/yvals.h>
  #endif
#else
  #include <c99/yvals.h>
#endif

