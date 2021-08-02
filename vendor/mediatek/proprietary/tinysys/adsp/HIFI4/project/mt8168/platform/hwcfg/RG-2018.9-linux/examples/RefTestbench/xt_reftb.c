/* 
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
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

// Utility routines for running on reference testbench

#ifdef __XTENSA__

#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>
#include <xtensa/xt_reftb.h>

#if XCHAL_HAVE_HALT
#include <xtensa/tie/xt_halt.h>
#endif

#endif // __XTENSA__

extern volatile unsigned int *_reftb_exit_location;
extern volatile unsigned int *_reftb_power_toggle;
extern volatile unsigned int *_reftb_int_deassert;
extern volatile unsigned int *_reftb_xtmem_scratch;


// Change exit status location
// You must also change the plusarg "+DVMagicExit" sent to the HW simulator
// or change the argument "--exit_location" sent to the ISS
//
unsigned int* 
set_testbench_exit_location(unsigned int* ex_loc)
{
  _reftb_exit_location = ex_loc;
  return (ex_loc);
}

// Change power toggle location
// You must also change the plusarg "+DVPowerLoc" sent to the HW simulator
//
unsigned int* 
set_testbench_power_toggle_location(unsigned int* ex_loc)
{
  _reftb_power_toggle = ex_loc;
  setup_power_toggle();
  return (ex_loc);
}

// Return exit status location
unsigned int* 
testbench_exit_location()
{
  return (unsigned int*) _reftb_exit_location;
}
// Return power toggle location
unsigned int* 
testbench_power_toggle_location()
{
  return (unsigned int*) _reftb_power_toggle;
}

// Setup for user power toggling
int setup_power_toggle() 
{
#ifdef __XTENSA__
#if XCHAL_HAVE_PIF
  xthal_set_region_attribute((void *)_reftb_power_toggle, 4, XCHAL_CA_BYPASS, 0);
#endif
#endif
  return 1;
}
#if XCHAL_HAVE_PSO && XCHAL_HAVE_PSO_CDM && XCHAL_HAVE_PSO_FULL_RETENTION
// Change wakeup int deassert location
// You must also change the plusarg "+DVWakeupIntClrLoc" sent to the HW simulator
//
unsigned int* 
set_testbench_int_deassert_location(unsigned int* ex_loc)
{
  _reftb_int_deassert = ex_loc;
  setup_int_deassert();
  return (ex_loc);
}

// Return wakeup int deassert location
unsigned int* 
testbench_int_deassert_location()
{
  return (unsigned int*) _reftb_int_deassert;
}

int setup_int_deassert()
{
#ifdef __XTENSA__
#if XCHAL_HAVE_PIF
  xthal_set_region_attribute((void *)_reftb_int_deassert, 4, XCHAL_CA_BYPASS, 0);
#endif
#endif
}
#endif

#if XCHAL_HAVE_PSO && ! XCHAL_HAVE_PSO_CDM
// Change wakeup int deassert location
// You must also change the plusarg "+DVWakeupIntClrLoc" sent to the HW simulator
//
unsigned int* 
set_testbench_xtmem_scratch_location(unsigned int* ex_loc)
{
  _reftb_xtmem_scratch = ex_loc;
  setup_xtmem_scratch();
  return (ex_loc);
}

// Return wakeup int deassert location
unsigned int* 
testbench_xtmem_scratch_location()
{
  return (unsigned int*) _reftb_xtmem_scratch;
}

int setup_xtmem_scratch()
{
#ifdef __XTENSA__
#if XCHAL_HAVE_PIF
  xthal_set_region_attribute((void *)_reftb_xtmem_scratch, 4, XCHAL_CA_BYPASS, 0);
#endif
#endif
}
#endif


// Diags expect status 1 or 2
#define DIAG_PASS_STATUS 1
#define DIAG_FAIL_STATUS 2
#define MAGIC_EXIT_FLAG 13

//
// Set exit status for HW simulation
// Monitors.v will detect the halt and exit the simulation. 
//
int set_diag_status(int stat) 
{

#ifdef __XTENSA__

#if XCHAL_HAVE_PIF
  xthal_set_region_attribute((void *)_reftb_exit_location, 4, XCHAL_CA_BYPASS, 0);
#endif

#if XCHAL_HAVE_HALT
// 1) Write status (PASS or FAIL) to magic address      
// 2) Do a memw to make sure write has completed
// 3) Issue halt instruction
//
  *_reftb_exit_location = stat;  
  XT_HALT();

#else  // XCHAL_HAVE_HALT

// 1) Write MAGIC_EXIT_FLAG to magic address
// 2) Write status (PASS or FAIL) to magic address      
//
  *_reftb_exit_location = MAGIC_EXIT_FLAG;  
  *_reftb_exit_location = stat;  

#endif  // XCHAL_HAVE_HALT
#endif // __XTENSA__

  return stat;
}

//
// Exit routines
// Set status then exit
// 

int diag_pass()
{
  return set_diag_status(DIAG_PASS_STATUS);
}

int diag_fail()
{
  return set_diag_status(DIAG_FAIL_STATUS);
}


//
// Set exit status for HW simulation
// Monitors.v will detect the halt and exit the simulation. 
//
//  This "fast" version does not call XTOS before setting status.
//  It requires that the user has assured the _reftb_exit_location
//  is in bypass memory prior to calling set_diag_status_fast().
int set_diag_status_fast(int stat) 
{

#ifdef __XTENSA__

#if XCHAL_HAVE_HALT
// 1) Write status (PASS or FAIL) to magic address      
// 2) Do a memw to make sure write has completed
// 3) Issue halt instruction
//
  *_reftb_exit_location = stat;  
  XT_HALT();

#else  // XCHAL_HAVE_HALT

// 1) Write MAGIC_EXIT_FLAG to magic address
// 2) Write status (PASS or FAIL) to magic address      
//
  *_reftb_exit_location = MAGIC_EXIT_FLAG;  
  *_reftb_exit_location = stat;  

#endif  // XCHAL_HAVE_HALT
#endif // __XTENSA__

  return stat;
}
