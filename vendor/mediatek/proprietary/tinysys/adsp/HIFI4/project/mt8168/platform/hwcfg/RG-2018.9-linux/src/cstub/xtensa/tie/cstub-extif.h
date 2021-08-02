/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the.
 * copyrighted works and confidential proprietary information of Tensilica Inc..
 * They may not be modified, copied, reproduced, distributed, or disclosed to.
 * third parties in any manner, medium, or form, in whole or in part, without.
 * the prior written consent of Tensilica Inc..
 */

/* Do not modify. This is automatically generated.*/

#ifndef CSTUB_EXTIF_HEADER
#define CSTUB_EXTIF_HEADER
/* Export states, import wires, queues, lookups */

/* External interface structure definition */
typedef struct cstub_ImportWire_struct {
  void *user_object;  /* ImportWire device */
  const char *name;   /* Name of ImportWire */
  unsigned width;     /* Width of ImportWire */
} cstub_ImportWire_t;
typedef struct cstub_ExportState_struct {
  void *user_object;  /* ExportState device */
  const char *name;   /* Name of ExportState */
  unsigned width;     /* Width of ExportState */
} cstub_ExportState_t;
typedef struct cstub_InputQueue_struct {
  void *user_object;  /* InputQueue device */
  const char *name;   /* Name of InputQueue */
  unsigned width;     /* Width of InputQueue */
} cstub_InputQueue_t;
typedef struct cstub_OutputQueue_struct {
  void *user_object;  /* OutputQueue device */
  const char *name;   /* Name of OutputQueue */
  unsigned width;     /* Width of OutputQueue */
} cstub_OutputQueue_t;
typedef struct cstub_Lookup_struct {
  void *user_object;  /* Lookup device */
  const char *name;   /* Name of lookup */
  unsigned out_width; /* Output width of lookup */
  unsigned in_width;  /* Input width of lookup */
} cstub_Lookup_t;

/* Callback functions for external interfaces */
typedef void (*cstub_ImportWire_func_t)(const cstub_ImportWire_t *data_struct, 
                                        unsigned *in_data);
typedef void (*cstub_ExportState_func_t)(const cstub_ExportState_t *data_struct, 
                                         const unsigned *out_data);
typedef unsigned (*cstub_InputQueue_Empty_func_t)(const cstub_InputQueue_t *data_struct);
typedef void (*cstub_InputQueue_Data_func_t) (const cstub_InputQueue_t *data_struct, 
                                              unsigned is_peek, 
                                              unsigned *in_data);
typedef unsigned (*cstub_OutputQueue_Full_func_t)(const cstub_OutputQueue_t *data_struct);
typedef void (*cstub_OutputQueue_Data_func_t) (const cstub_OutputQueue_t *data_struct, 
                                               unsigned reserve_only, 
                                               const unsigned *out_data);
typedef void (*cstub_Lookup_func_t)(const cstub_Lookup_t *data_struct, 
                                    const unsigned *out_data, 
                                    unsigned *in_data);

#endif /* CSTUB_EXTIF_HEADER */

