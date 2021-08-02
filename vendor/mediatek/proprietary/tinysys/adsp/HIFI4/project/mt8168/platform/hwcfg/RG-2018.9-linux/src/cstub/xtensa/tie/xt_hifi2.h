#ifndef __cstub_xt_hifi2_h__
#define __cstub_xt_hifi2_h__

#include "Xm_hifi4_Aquila_E2_PROD.h"

#undef AE_L32X2F24_RIP
#undef AE_L32X2_RIP
#undef AE_S32X2_RIP
#undef AE_S16X4_RIP
#undef AE_L16X4_RIP
#ifdef CSTUB_
#define AE_L32X2_RIP_1_ARGS(data, pointer)              CSTUB_(_TIE_xt_hifi2_AE_L32X2_RIP)(data, (const CSTUB_(ae_int32x2) **) &(pointer), -8) 
#define AE_L32X2_RIP_2_ARGS(data, pointer, size)        CSTUB_(_TIE_xt_hifi2_AE_L32X2_RIP)(data, (const CSTUB_(ae_int32x2) **) &(pointer), size) 
#define AE_S32X2_RIP_1_ARGS(data, pointer)              CSTUB_(_TIE_xt_hifi2_AE_S32X2_RIP)(data, (CSTUB_(ae_int32x2) **) &(pointer), -8) 
#define AE_S32X2_RIP_2_ARGS(data, pointer, size)        CSTUB_(_TIE_xt_hifi2_AE_S32X2_RIP)(data, (CSTUB_(ae_int32x2) **) &(pointer), size) 
#define AE_S16X4_RIP_1_ARGS(data, pointer)              CSTUB_(_TIE_xt_hifi2_AE_S16X4_RIP)(data, (CSTUB_(ae_int16x4) **) &(pointer), -8) 
#define AE_S16X4_RIP_2_ARGS(data, pointer, size)        CSTUB_(_TIE_xt_hifi2_AE_S16X4_RIP)(data, (CSTUB_(ae_int16x4) **) &(pointer), size) 
#else
#define AE_L32X2_RIP_1_ARGS(data, pointer)              _TIE_xt_hifi2_AE_L32X2_RIP(data, pointer, -8) 
#define AE_L32X2_RIP_2_ARGS(data, pointer, size)        _TIE_xt_hifi2_AE_L32X2_RIP(data, pointer, size) 
#define AE_S32X2_RIP_1_ARGS(data, pointer)              _TIE_xt_hifi2_AE_S32X2_RIP(data, pointer, -8) 
#define AE_S32X2_RIP_2_ARGS(data, pointer, size)        _TIE_xt_hifi2_AE_S32X2_RIP(data, pointer, size) 
#define AE_S16X4_RIP_1_ARGS(data, pointer)              _TIE_xt_hifi2_AE_S16X4_RIP(data, pointer, -8) 
#define AE_S16X4_RIP_2_ARGS(data, pointer, size)        _TIE_xt_hifi2_AE_S16X4_RIP(data, pointer, size) 
#endif
#define AE_GET_FOURTH_ARG(arg1, arg2, arg3, arg4, ...) arg4 
#define AE_GET_FOURTH_ARG_(tuple) AE_GET_FOURTH_ARG tuple 
#define AE_L32X2_RIP_MACRO_CHOOSER(...) AE_GET_FOURTH_ARG_((__VA_ARGS__, AE_L32X2_RIP_2_ARGS, AE_L32X2_RIP_1_ARGS)) 
#define AE_L32X2_RIP(...) AE_L32X2_RIP_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__) 
#define AE_S32X2_RIP_MACRO_CHOOSER(...) AE_GET_FOURTH_ARG_((__VA_ARGS__, AE_S32X2_RIP_2_ARGS, AE_S32X2_RIP_1_ARGS)) 
#define AE_S32X2_RIP(...) AE_S32X2_RIP_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__) 
#define AE_S16X4_RIP_MACRO_CHOOSER(...) AE_GET_FOURTH_ARG_((__VA_ARGS__, AE_S16X4_RIP_2_ARGS, AE_S16X4_RIP_1_ARGS)) 
#define AE_S16X4_RIP(...) AE_S16X4_RIP_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__) 
#ifdef CSTUB_
#define AE_L32X2F24_RIP_1_ARGS(data, pointer)              CSTUB_(_TIE_xt_hifi2_AE_L32X2F24_RIP)(data, (const CSTUB_(ae_f24x2) **) &(pointer), -8) 
#define AE_L32X2F24_RIP_2_ARGS(data, pointer, size)        CSTUB_(_TIE_xt_hifi2_AE_L32X2F24_RIP)(data, (const CSTUB_(ae_f24x2) **) &(pointer), size) 
#else
#define AE_L32X2F24_RIP_1_ARGS(data, pointer)              _TIE_xt_hifi2_AE_L32X2F24_RIP(data, pointer, -8) 
#define AE_L32X2F24_RIP_2_ARGS(data, pointer, size)        _TIE_xt_hifi2_AE_L32X2F24_RIP(data, pointer, size) 
#endif
#define AE_GET_FOURTH_ARG(arg1, arg2, arg3, arg4, ...) arg4 
#define AE_GET_FOURTH_ARG_(tuple) AE_GET_FOURTH_ARG tuple 
#define AE_L32X2F24_RIP_MACRO_CHOOSER(...) AE_GET_FOURTH_ARG_((__VA_ARGS__, AE_L32X2F24_RIP_2_ARGS, AE_L32X2F24_RIP_1_ARGS)) 
#define AE_L32X2F24_RIP(...) AE_L32X2F24_RIP_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__) 
#ifdef CSTUB_
#define AE_L16X4_RIP_1_ARGS(data, pointer)              CSTUB_(_TIE_xt_hifi2_AE_L16X4_RIP)(data, (const CSTUB_(ae_int16x4) **) &(pointer), -8) 
#define AE_L16X4_RIP_2_ARGS(data, pointer, size)        CSTUB_(_TIE_xt_hifi2_AE_L16X4_RIP)(data, (const CSTUB_(ae_int16x4) **) &(pointer), size) 
#else
#define AE_L16X4_RIP_1_ARGS(data, pointer)              _TIE_xt_hifi2_AE_L16X4_RIP(data, pointer, -8) 
#define AE_L16X4_RIP_2_ARGS(data, pointer, size)        _TIE_xt_hifi2_AE_L16X4_RIP(data, pointer, size) 
#endif
#define AE_GET_FOURTH_ARG(arg1, arg2, arg3, arg4, ...) arg4 
#define AE_GET_FOURTH_ARG_(tuple) AE_GET_FOURTH_ARG tuple 
#define AE_L16X4_RIP_MACRO_CHOOSER(...) AE_GET_FOURTH_ARG_((__VA_ARGS__, AE_L16X4_RIP_2_ARGS, AE_L16X4_RIP_1_ARGS)) 
#define AE_L16X4_RIP(...) AE_L16X4_RIP_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__) 

#endif
