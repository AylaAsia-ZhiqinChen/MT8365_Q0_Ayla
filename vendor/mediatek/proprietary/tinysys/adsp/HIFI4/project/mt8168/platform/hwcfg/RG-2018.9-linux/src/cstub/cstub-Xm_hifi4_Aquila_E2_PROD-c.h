/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the.
 * copyrighted works and confidential proprietary information of Tensilica Inc..
 * They may not be modified, copied, reproduced, distributed, or disclosed to.
 * third parties in any manner, medium, or form, in whole or in part, without.
 * the prior written consent of Tensilica Inc..
 */

/* Do not modify. This is automatically generated.*/

#ifndef __CSTUB_Xm_hifi4_Aquila_E2_PROD_C_H__
#define __CSTUB_Xm_hifi4_Aquila_E2_PROD_C_H__

/* Included files */
#include "xtensa/tie/Xm_hifi4_Aquila_E2_PROD.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(__GNUC__)
#if defined(__LP64__) 
#error "Error: Compiling c-stub on 64 bit machines is not supported"
#endif /* __LP64__ */ 
#define INLINE inline 
#elif defined(_MSC_VER)
#if defined(_WIN64) 
#error "Error: Compiling c-stub on 64 bit machines is not supported"
#endif /* _WIN64 */
#define INLINE __inline 
#if (_MSC_VER <1500) // Visual Studio 2005 and earlier
unsigned char _BitScanReverse(unsigned long *Index, unsigned long Mask) {
  int idx = 0;
  if (Mask == 0) return 0;
  while (Mask & 1) {
    idx ++; Mask >>= 1;
  }
  return idx;
}
#else
#include "intrin.h"
#endif
#else 
#error "Error: Only GCC/G++ and Visual C++ are supported"
#endif

#define CSTUB_PROC CSTUB_(proc) 
/* Processor states */
typedef struct CSTUB_(states) {
unsigned XTSYNC;
unsigned VECBASE;
unsigned EPC1;
unsigned EPC2;
unsigned EPC3;
unsigned EPC4;
unsigned EXCSAVE1;
unsigned EXCSAVE2;
unsigned EXCSAVE3;
unsigned EXCSAVE4;
unsigned EPS2;
unsigned EPS3;
unsigned EPS4;
unsigned EXCCAUSE;
unsigned PSINTLEVEL;
unsigned PSUM;
unsigned PSWOE;
unsigned PSRING;
unsigned PSEXCM;
unsigned DEPC;
unsigned EXCVADDR;
unsigned WindowBase;
unsigned WindowStart;
unsigned PSCALLINC;
unsigned PSOWB;
unsigned LBEG;
unsigned LEND;
unsigned MEMCTL;
unsigned SAR;
unsigned THREADPTR;
unsigned MISC0;
unsigned MISC1;
unsigned MISC2;
unsigned MISC3;
unsigned MPUNUMENTRIES;
unsigned InOCDMode;
unsigned INTENABLE;
unsigned DBREAKA0;
unsigned DBREAKC0;
unsigned DBREAKA1;
unsigned DBREAKC1;
unsigned IBREAKA0;
unsigned IBREAKA1;
unsigned IBREAKENABLE;
unsigned ICOUNTLEVEL;
unsigned DEBUGCAUSE;
unsigned DBNUM;
unsigned CCOMPARE0;
unsigned CCOMPARE1;
unsigned CCOMPARE2;
unsigned PREFCTL;
unsigned CACHEADRDIS;
unsigned MPUENB;
unsigned CPENABLE;
unsigned ATOMCTL;
unsigned ERI_RAW_INTERLOCK;
unsigned ERACCESS;
unsigned AE_OVERFLOW;
unsigned AE_CBEGIN0;
unsigned AE_CEND0;
unsigned AE_CBEGIN1;
unsigned AE_CEND1;
unsigned AE_SAR;
unsigned AE_CWRAP;
unsigned AE_BITHEAD;
unsigned AE_BITPTR;
unsigned AE_BITSUSED;
unsigned AE_TABLESIZE;
unsigned AE_FIRST_TS;
unsigned AE_NEXTOFFSET;
unsigned AE_SEARCHDONE;
unsigned RoundMode;
unsigned InvalidFlag;
unsigned DivZeroFlag;
unsigned OverflowFlag;
unsigned UnderflowFlag;
unsigned InexactFlag;
unsigned EXPSTATE;
unsigned qi;
unsigned qo;
unsigned qo_trig;
unsigned accreg[2];
} CSTUB_(states_t);

typedef void (* cstub_mulpp_inplace)(unsigned *op_N, unsigned N_bits,
			 	     unsigned *op_M, unsigned M_bits,
				     unsigned *partial0, unsigned R_bits,
				     unsigned *partial1, int is_signed,
				     int negate);

typedef struct cstub_callbacks {
  cstub_mulpp_inplace mulpp_inplace;
} cstub_callbacks_t;

#define cstub_arith_wide_mulpp_inplace CSTUB_(cstub_arith_wide_mulpp_inplace)
void 
cstub_arith_wide_mulpp_inplace(unsigned *op_N, unsigned N_bits,
                               unsigned *op_M, unsigned M_bits,
                               unsigned *partial0, unsigned R_bits,
                               unsigned *partial1,
                               int is_signed,
                               int negate);
#define cstub_mulpp_cache_delete CSTUB_(cstub_mulpp_cache_delete)
extern void 
cstub_mulpp_cache_delete();

/* External interface structs */
typedef struct CSTUB_(ImportWire_Internal) {
  cstub_ImportWire_t import_wire;
  cstub_ImportWire_func_t callback;
} CSTUB_(ImportWire_Internal_t);
typedef struct CSTUB_(ExportState_Internal) {
  cstub_ExportState_t export_state;
  cstub_ExportState_func_t callback;
} CSTUB_(ExportState_Internal_t);
typedef struct CSTUB_(InputQueue_Internal) {
  cstub_InputQueue_t input_queue;
  unsigned *data;
  cstub_InputQueue_Empty_func_t empty_callback;
  cstub_InputQueue_Data_func_t data_callback;
} CSTUB_(InputQueue_Internal_t);
typedef struct CSTUB_(OutputQueue_Internal) {
  cstub_OutputQueue_t output_queue;
  cstub_OutputQueue_Full_func_t full_callback;
  cstub_OutputQueue_Data_func_t data_callback;
} CSTUB_(OutputQueue_Internal_t);
typedef struct CSTUB_(Lookup_Internal) {
  cstub_Lookup_t lookup;
  cstub_Lookup_func_t callback;
} CSTUB_(Lookup_Internal_t);

/* Processor struct */
typedef struct CSTUB_(processor) {
  CSTUB_(states_t) states;
  CSTUB_(ExportState_Internal_t) EXPSTATE;
  CSTUB_(Lookup_Internal_t) ERI_RD;
  CSTUB_(Lookup_Internal_t) ERI_WR;
  CSTUB_(ImportWire_Internal_t) IMPWIRE;
  CSTUB_(ExportState_Internal_t) qi;
  CSTUB_(ExportState_Internal_t) qo;
  CSTUB_(ExportState_Internal_t) qo_trig;
  CSTUB_(Lookup_Internal_t) MFP1;
  CSTUB_(Lookup_Internal_t) MFP2;
  CSTUB_(ImportWire_Internal_t) mfp1status;
  CSTUB_(ImportWire_Internal_t) mfp2status;
  cstub_callbacks_t callbacks;
  void *last;
} CSTUB_(processor_t);

extern CSTUB_(processor_t) CSTUB_(proc);
/* Table declarations */
#define table__xt_bytelane_lsb CSTUB_(table__xt_bytelane_lsb)
extern const unsigned int table__xt_bytelane_lsb[16]; 
#define table__ai4c CSTUB_(table__ai4c)
extern const unsigned int table__ai4c[16]; 
#define table__b4c CSTUB_(table__b4c)
extern const unsigned int table__b4c[16]; 
#define table__b4cu CSTUB_(table__b4cu)
extern const unsigned int table__b4cu[16]; 
#define table__bitmask8 CSTUB_(table__bitmask8)
extern const unsigned int table__bitmask8[8]; 
#define table__ae_ripimmtable CSTUB_(table__ae_ripimmtable)
extern const unsigned int table__ae_ripimmtable[4]; 
#define table__ae_slai72table CSTUB_(table__ae_slai72table)
extern const unsigned int table__ae_slai72table[8]; 
#define table__ae_seliencode CSTUB_(table__ae_seliencode)
extern const unsigned int table__ae_seliencode[16]; 
#define table__RECIP_Data8 CSTUB_(table__RECIP_Data8)
extern const unsigned int table__RECIP_Data8[128]; 
#define table__RSQRT_Data8 CSTUB_(table__RSQRT_Data8)
extern const unsigned int table__RSQRT_Data8[128]; 
#define table__vfpu2_table_mulmux CSTUB_(table__vfpu2_table_mulmux)
extern const unsigned int table__vfpu2_table_mulmux[2]; 
#define table__vfpu2_table_maddmux CSTUB_(table__vfpu2_table_maddmux)
extern const unsigned int table__vfpu2_table_maddmux[8]; 

/* Mask for StoreByteDisable */
#define mask CSTUB_(mask)
extern unsigned mask[16];
/* Memory unit functions */

typedef union CSTUB_(mem) {
  unsigned char  d8;
  unsigned short d16;
  unsigned int   d32;
  unsigned int   d64[2];
  unsigned int   d128[4];
  unsigned int   d256[8];
  unsigned int   d512[16];
} CSTUB_MAY_ALIAS CSTUB_(mem_t);

static INLINE void MemDataIn512(unsigned addr, unsigned opcode, unsigned *data) {
  CSTUB_(mem_t) *d = (CSTUB_(mem_t) *)addr;
  data[0] = d->d512[0];
  data[1] = d->d512[1];
  data[2] = d->d512[2];
  data[3] = d->d512[3];
  data[4] = d->d512[4];
  data[5] = d->d512[5];
  data[6] = d->d512[6];
  data[7] = d->d512[7];
  data[8] = d->d512[8];
  data[9] = d->d512[9];
  data[10] = d->d512[10];
  data[11] = d->d512[11];
  data[12] = d->d512[12];
  data[13] = d->d512[13];
  data[14] = d->d512[14];
  data[15] = d->d512[15];
}

static INLINE void MemDataIn256(unsigned addr, unsigned opcode, unsigned *data) {
  CSTUB_(mem_t) *d = (CSTUB_(mem_t) *)addr;
  data[0] = d->d256[0];
  data[1] = d->d256[1];
  data[2] = d->d256[2];
  data[3] = d->d256[3];
  data[4] = d->d256[4];
  data[5] = d->d256[5];
  data[6] = d->d256[6];
  data[7] = d->d256[7];
}

static INLINE void MemDataIn128(unsigned addr, unsigned opcode, unsigned *data) {
  CSTUB_(mem_t) *d = (CSTUB_(mem_t) *)addr;
  data[0] = d->d128[0];
  data[1] = d->d128[1];
  data[2] = d->d128[2];
  data[3] = d->d128[3];
}

static INLINE void MemDataIn64(unsigned addr, unsigned opcode, unsigned *data) {
  CSTUB_(mem_t) *d = (CSTUB_(mem_t) *)addr;
  data[0] = d->d64[0];
  data[1] = d->d64[1];
}

static INLINE unsigned MemDataIn32(unsigned addr, unsigned opcode) {
  CSTUB_(mem_t) *d = (CSTUB_(mem_t) *)addr;
  return d->d32;
}

static INLINE unsigned short MemDataIn16(unsigned addr, unsigned opcode) {
  CSTUB_(mem_t) *d = (CSTUB_(mem_t) *) addr;
  return d->d16;
}

static INLINE unsigned char MemDataIn8(unsigned addr, unsigned opcode) {
  CSTUB_(mem_t) *d = (CSTUB_(mem_t) *) addr;
  return d->d8;
}

static INLINE void MemDataOut512(unsigned addr, const unsigned *data, const unsigned *disables, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  const unsigned *src = data;
  if ((disables[0] == 0) && (disables[1] == 0)) {  
    dst->d512[0] = src[0];
    dst->d512[1] = src[1];
    dst->d512[2] = src[2];
    dst->d512[3] = src[3];
    dst->d512[4] = src[4];
    dst->d512[5] = src[5];
    dst->d512[6] = src[6];
    dst->d512[7] = src[7];
    dst->d512[8] = src[8];
    dst->d512[9] = src[9];
    dst->d512[10] = src[10];
    dst->d512[11] = src[11];
    dst->d512[12] = src[12];
    dst->d512[13] = src[13];
    dst->d512[14] = src[14];
    dst->d512[15] = src[15];
  } else  if ((disables[0] == 0xffffffff) && (disables[1] == 0xffffffff)) {
    /* no store */
  } else {
    dst->d512[0] = (dst->d512[0] & (~mask[(disables[0] >> 0) & 0xf])) | (src[0] & mask[(disables[0] >> 0) & 0xf]);
    dst->d512[1] = (dst->d512[1] & (~mask[(disables[0] >> 4) & 0xf])) | (src[1] & mask[(disables[0] >> 4) & 0xf]);
    dst->d512[2] = (dst->d512[2] & (~mask[(disables[0] >> 8) & 0xf])) | (src[2] & mask[(disables[0] >> 8) & 0xf]);
    dst->d512[3] = (dst->d512[3] & (~mask[(disables[0] >> 12) & 0xf])) | (src[3] & mask[(disables[0] >> 12) & 0xf]);
    dst->d512[4] = (dst->d512[4] & (~mask[(disables[0] >> 16) & 0xf])) | (src[4] & mask[(disables[0] >> 16) & 0xf]);
    dst->d512[5] = (dst->d512[5] & (~mask[(disables[0] >> 20) & 0xf])) | (src[5] & mask[(disables[0] >> 20) & 0xf]);
    dst->d512[6] = (dst->d512[6] & (~mask[(disables[0] >> 24) & 0xf])) | (src[6] & mask[(disables[0] >> 24) & 0xf]);
    dst->d512[7] = (dst->d512[7] & (~mask[(disables[0] >> 28) & 0xf])) | (src[7] & mask[(disables[0] >> 28) & 0xf]);
    dst->d512[8] = (dst->d512[8] & (~mask[(disables[1] >> 0) & 0xf])) | (src[8] & mask[(disables[1] >> 0) & 0xf]);
    dst->d512[9] = (dst->d512[9] & (~mask[(disables[1] >> 4) & 0xf])) | (src[9] & mask[(disables[1] >> 4) & 0xf]);
    dst->d512[10] = (dst->d512[10] & (~mask[(disables[1] >> 8) & 0xf])) | (src[10] & mask[(disables[1] >> 8) & 0xf]);
    dst->d512[11] = (dst->d512[11] & (~mask[(disables[1] >> 12) & 0xf])) | (src[11] & mask[(disables[1] >> 12) & 0xf]);
    dst->d512[12] = (dst->d512[12] & (~mask[(disables[1] >> 16) & 0xf])) | (src[12] & mask[(disables[1] >> 16) & 0xf]);
    dst->d512[13] = (dst->d512[13] & (~mask[(disables[1] >> 20) & 0xf])) | (src[13] & mask[(disables[1] >> 20) & 0xf]);
    dst->d512[14] = (dst->d512[14] & (~mask[(disables[1] >> 24) & 0xf])) | (src[14] & mask[(disables[1] >> 24) & 0xf]);
    dst->d512[15] = (dst->d512[15] & (~mask[(disables[1] >> 28) & 0xf])) | (src[15] & mask[(disables[1] >> 28) & 0xf]);
  }
}

static INLINE void MemDataOut256(unsigned addr, const unsigned *data, unsigned disables, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  const unsigned *src = data;
  if (disables == 0) {  
    dst->d256[0] = src[0];
    dst->d256[1] = src[1];
    dst->d256[2] = src[2];
    dst->d256[3] = src[3];
    dst->d256[4] = src[4];
    dst->d256[5] = src[5];
    dst->d256[6] = src[6];
    dst->d256[7] = src[7];
  } else  if (disables == 0xffffffff) {
    /* no store */
  } else {
    dst->d256[0] = (dst->d256[0] & (~mask[(disables >> 0) & 0xf])) | (src[0] & mask[(disables >> 0) & 0xf]);
    dst->d256[1] = (dst->d256[1] & (~mask[(disables >> 4) & 0xf])) | (src[1] & mask[(disables >> 4) & 0xf]);
    dst->d256[2] = (dst->d256[2] & (~mask[(disables >> 8) & 0xf])) | (src[2] & mask[(disables >> 8) & 0xf]);
    dst->d256[3] = (dst->d256[3] & (~mask[(disables >> 12) & 0xf])) | (src[3] & mask[(disables >> 12) & 0xf]);
    dst->d256[4] = (dst->d256[4] & (~mask[(disables >> 16) & 0xf])) | (src[4] & mask[(disables >> 16) & 0xf]);
    dst->d256[5] = (dst->d256[5] & (~mask[(disables >> 20) & 0xf])) | (src[5] & mask[(disables >> 20) & 0xf]);
    dst->d256[6] = (dst->d256[6] & (~mask[(disables >> 24) & 0xf])) | (src[6] & mask[(disables >> 24) & 0xf]);
    dst->d256[7] = (dst->d256[7] & (~mask[(disables >> 28) & 0xf])) | (src[7] & mask[(disables >> 28) & 0xf]);
  }
}

static INLINE void MemDataOut128(unsigned addr, const unsigned *data, unsigned disables, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  const unsigned *src = data;
  if (disables == 0) {  
    dst->d128[0] = src[0];
    dst->d128[1] = src[1];
    dst->d128[2] = src[2];
    dst->d128[3] = src[3];
  } else  if (disables == 0xffff) {
    /* no store */
  } else {
    dst->d128[0] = (dst->d128[0] & (~mask[disables & 0xf]))| (src[0] & mask[disables & 0xf]);
    dst->d128[1] = (dst->d128[1] & (~mask[(disables >> 4) & 0xf])) | (src[1] & mask[(disables >> 4) & 0xf]);
    dst->d128[2] = (dst->d128[2] & (~mask[(disables >> 8) & 0xf])) | (src[2] & mask[(disables >> 8) & 0xf]);
    dst->d128[3] = (dst->d128[3] & (~mask[(disables >> 12) & 0xf])) | (src[3] & mask[(disables >> 12) & 0xf]);
  }
}

static INLINE void MemDataOut64(unsigned addr, const unsigned *data, unsigned disables, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  const unsigned *src = data;
  if (disables == 0) {
    dst->d64[0] = src[0];
    dst->d64[1] = src[1];
  } else if (disables == 0xff) {
    /* no store */
  } else {
    dst->d64[0] = (dst->d64[0] & (~mask[disables & 0xf])) | (src[0] & mask[disables & 0xf]);
    dst->d64[1] = (dst->d64[1] & (~mask[(disables >> 4) & 0xf])) | (src[1] & mask[(disables >> 4) & 0xf]);
  }
}

static INLINE void MemDataOut32(unsigned addr, unsigned data, unsigned disables, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  if (disables == 0) {
    dst->d32 = data;
  } else if (disables == 0xf) {
    /* no store */
  } else {
    dst->d32 = (dst->d32 & (~mask[disables])) | (data & mask[disables]);
  }
}

static INLINE void MemDataOut16(unsigned addr, unsigned data, unsigned disables, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  unsigned short src = (unsigned short) data;
  if (disables == 0) 
    dst->d16 = src;
  else if (disables == 0x3) {
    /* no store */
  } else {
    dst->d16 = (dst->d16 & (~mask[disables])) | (src & mask[disables]);
  }
}

static INLINE void MemDataOut8(unsigned addr, unsigned data, unsigned disables, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  if (disables == 0)
    dst->d8 = data;
}

static INLINE void MemDataOut512WODisable(unsigned addr, const unsigned *data, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  const unsigned *src = data;
  dst->d512[0] = src[0];
  dst->d512[1] = src[1];
  dst->d512[2] = src[2];
  dst->d512[3] = src[3];
  dst->d512[4] = src[4];
  dst->d512[5] = src[5];
  dst->d512[6] = src[6];
  dst->d512[7] = src[7];
  dst->d512[8] = src[8];
  dst->d512[9] = src[9];
  dst->d512[10] = src[10];
  dst->d512[11] = src[11];
  dst->d512[12] = src[12];
  dst->d512[13] = src[13];
  dst->d512[14] = src[14];
  dst->d512[15] = src[15];
}

static INLINE void MemDataOut256WODisable(unsigned addr, const unsigned *data, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  const unsigned *src = data;
  dst->d256[0] = src[0];
  dst->d256[1] = src[1];
  dst->d256[2] = src[2];
  dst->d256[3] = src[3];
  dst->d256[4] = src[4];
  dst->d256[5] = src[5];
  dst->d256[6] = src[6];
  dst->d256[7] = src[7];
}

static INLINE void MemDataOut128WODisable(unsigned addr, const unsigned *data, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  const unsigned *src = data;
  dst->d128[0] = src[0];
  dst->d128[1] = src[1];
  dst->d128[2] = src[2];
  dst->d128[3] = src[3];
}

static INLINE void MemDataOut64WODisable(unsigned addr, const unsigned *data, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  const unsigned *src = data;
  dst->d64[0] = src[0];
  dst->d64[1] = src[1];
}

static INLINE void MemDataOut32WODisable(unsigned addr, unsigned data, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  dst->d32 = data;
}

static INLINE void MemDataOut16WODisable(unsigned addr, unsigned data, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  dst->d16 = data;
}

static INLINE void MemDataOut8WODisable(unsigned addr, unsigned data, unsigned opcode) {
  CSTUB_(mem_t) *dst = (CSTUB_(mem_t) *) addr;
  dst->d8 = data;
}

/* Export states, import wires, queues, lookups */

static INLINE void cstub_ImportWire_Internal(CSTUB_(ImportWire_Internal_t) *iw, unsigned *data) {
  if (iw->callback == NULL) {
    fprintf(stderr, "Error: ImportWire \"%s\" is used but not registered\n", iw->import_wire.name);
    exit(1);
  } else {
    (*iw->callback) (&iw->import_wire, data);
  }
}

static INLINE void cstub_ExportState_Internal(CSTUB_(ExportState_Internal_t) *es, unsigned *data) {
  if (es->callback == NULL) {
    fprintf(stderr, "Error: ExportState \"%s\" is used but not registered\n", es->export_state.name);
    exit(1);
  } else {
    (*es->callback) (&es->export_state, data);
  }
}

static INLINE unsigned cstub_InputQueue_NotRdy(CSTUB_(InputQueue_Internal_t) *queue) {
  if (queue->empty_callback == NULL) {
    fprintf(stderr, "Error: InputQueue \"%s\" is used but not registered\n", queue->input_queue.name);
    exit(1);
  } else {
    return (*queue->empty_callback)(&queue->input_queue);
  }
}
static INLINE void cstub_InputQueue_PopReq(CSTUB_(InputQueue_Internal_t) *queue) {
  if (queue->data_callback == NULL) {
    fprintf(stderr, "Error: InputQueue \"%s\" is used but not registered\n", queue->input_queue.name);
    exit(1);
  } else {
    (*queue->data_callback)(&queue->input_queue, 1, queue->data);
  }
}
static INLINE void cstub_InputQueue_Data(CSTUB_(InputQueue_Internal_t) *queue, unsigned is_peek, unsigned *data) {
  if (queue->data_callback == NULL) {
    fprintf(stderr, "Error: InputQueue \"%s\" is used but not registered\n", queue->input_queue.name);
    exit(1);
  } else {
    if (is_peek) {
      if (!(*queue->empty_callback)(&queue->input_queue)) {
        (*queue->data_callback)(&queue->input_queue, is_peek, queue->data);
      }
    } else {
      (*queue->data_callback)(&queue->input_queue, is_peek, queue->data);
    }
    memcpy(data, queue->data, ((queue->input_queue.width + 31) >> 5) * 4);
  }
}

static INLINE unsigned cstub_OutputQueue_NotRdy(CSTUB_(OutputQueue_Internal_t) *queue) {
  if (queue->full_callback == NULL) {
    fprintf(stderr, "Error: OutputQueue \"%s\" is used but not registered\n", queue->output_queue.name);
    exit(1);
  } else {
    return (*queue->full_callback)(&queue->output_queue);
  }
}
static INLINE void cstub_OutputQueue_PushReq(CSTUB_(OutputQueue_Internal_t) *queue) {
  if (queue->data_callback == NULL) {
    fprintf(stderr, "Error: OutputQueue \"%s\" is used but not registered\n", queue->output_queue.name);
    exit(1);
  } else {
    (*queue->data_callback)(&queue->output_queue, 1, 0);
  }
}
static INLINE void cstub_OutputQueue_Data(CSTUB_(OutputQueue_Internal_t) *queue, unsigned *data) {
  if (queue->data_callback == NULL) {
    fprintf(stderr, "Error: OutputQueue \"%s\" is used but not registered\n", queue->output_queue.name);
    exit(1);
  } else {
    (*queue->data_callback)(&queue->output_queue, 0, data);
  }
}

static INLINE void cstub_Lookup_Internal(CSTUB_(Lookup_Internal_t) *lkup, unsigned *out_data, unsigned *in_data) {
  if (lkup->callback == NULL) {
    fprintf(stderr, "Error: Lookup \"%s\" is used but not registered\n", lkup->lookup.name);
    exit(1);
  } else {
    (*lkup->callback)(&lkup->lookup, out_data, in_data);
  }
}

static INLINE void cstub_vaddr_not_aligned(unsigned vaddr) {
  fprintf(stderr, "Error: The config takes exception when the address of an load/store is unaligned. The address 0x%x is not properly aligned.\n", vaddr);
  exit(1);
}

#endif
