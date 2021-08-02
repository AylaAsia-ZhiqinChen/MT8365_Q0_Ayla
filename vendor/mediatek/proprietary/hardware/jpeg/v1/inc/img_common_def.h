#ifndef __IMG_COMMON_DEF_H__
#define __IMG_COMMON_DEF_H__



#define TO_CEIL(x,a) ( ( (unsigned int)(x) + ((a)-1)) & ~((a)-1) )

#define TO_FLOOR(x,a) ( (unsigned int)(x) & ~((a)-1) )

#define TO_MASK(x,a) ( (x) & (a) )



#endif
