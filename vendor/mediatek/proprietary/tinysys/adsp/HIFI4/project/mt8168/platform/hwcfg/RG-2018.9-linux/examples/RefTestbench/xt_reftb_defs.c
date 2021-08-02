/* 
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any
 * adapted or modified version may be disclosed or distributed to
 * third parties in any manner, medium, or form, in whole or in part,
 * without the prior written consent of Tensilica Inc.
 *
 * This software and its derivatives are to be executed solely on
 * products incorporating a Tensilica processor.
 */

// Parameter definitions for running on reference testbench

#ifdef __XTENSA__

#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>
#include <xtensa/xt_reftb.h>
#include <stdlib.h>


// Write exit status to this address
// You may change this location using the routine set_testbench_exit_location()
// or by compiling with "-DTB_EXIT_LOCATION=<address>"
// Then you must also change the plusarg "+DVMagicExit" sent to the HW simulator
// or change the argument "--exit_location" sent to the ISS
//
#define TB_EXIT_OFFSET 0
#define TB_POWER_OFFSET 12

#ifndef TB_EXIT_LOCATION
#define TB_EXIT_LOCATION (XSHAL_MAGIC_EXIT + TB_EXIT_OFFSET)
#endif 

// Change power toggle location
// You must also change the plusarg "+DVPowerLoc" sent to the HW simulator
//
#ifndef TB_POWER_LOCATION
#define TB_POWER_LOCATION (XSHAL_MAGIC_EXIT + TB_POWER_OFFSET)
#endif

#if XCHAL_HAVE_PSO && XCHAL_HAVE_PSO_CDM && XCHAL_HAVE_PSO_FULL_RETENTION
// Change deassert wakeup interrupt location
// You must also change the plusarg "+DVWakeupIntClrLoc" sent to the HW simulator
//
#ifndef TB_INT_DEASSERT_LOCATION 
#define TB_INT_DEASSERT_LOCATION (XSHAL_MAGIC_EXIT + XSHAL_MAGIC_INT_DEASSERT_OFFSET)
#endif
#endif

#if XCHAL_HAVE_PSO && ! XCHAL_HAVE_PSO_CDM
// Change location to store any values for xtmem domain cores for power down
//
#ifndef TB_XTMEM_SCRATCH_LOCATION 
#define TB_XTMEM_SCRATCH_LOCATION (XSHAL_MAGIC_EXIT + XSHAL_MAGIC_PSO_SINGLE_SCRATCH_OFFSET)
#endif
#endif

#define weak __attribute__((weak))

#else // __XTENSA__

#define weak

#endif // __XTENSA__




/*  Default to NULL:  */

#ifndef TB_EXIT_LOCATION
#define TB_EXIT_LOCATION		NULL
#endif

#ifndef TB_POWER_LOCATION
#define TB_POWER_LOCATION		NULL
#endif

#ifndef TB_INT_DEASSERT_LOCATION
#define TB_INT_DEASSERT_LOCATION	NULL
#endif

#ifndef TB_XTMEM_SCRATCH_LOCATION
#define TB_XTMEM_SCRATCH_LOCATION	NULL
#endif


// Write diag status result to this address
// This location must be in bypass region
volatile weak unsigned int *_reftb_exit_location = (unsigned int*) TB_EXIT_LOCATION;
volatile weak unsigned int *_reftb_power_toggle  = (unsigned int*) TB_POWER_LOCATION;
volatile weak unsigned int *_reftb_int_deassert  = (unsigned int*) TB_INT_DEASSERT_LOCATION;
volatile weak unsigned int *_reftb_xtmem_scratch = (unsigned int*) TB_XTMEM_SCRATCH_LOCATION;

