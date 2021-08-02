// Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

/*
 * Instructions:
 *
 *   To use with the xtsc_arbiter executable:
 *      ./xtsc_arbiter -xtsc_command_prompt=true
 *      cmd: . turbo.switch.aliases.cmd
 *
 *   To use with xtsc-run:
 *     xtsc-run -include=xtsc_arbiter.inc -set_xtsc_parm=xtsc_command_prompt=true
 *      cmd: . turbo.switch.aliases.cmd
 *   
 */

;# Use wtx to wait until TurboXim finishes a quantum sync.
alias wtx=sc wait xtsc_driver_wait_event

;# Use wca to wait for the cycle-accurate ISS scheduler event.
alias wca=sc wait xtsc_iss_scheduler_event

;# To switch from cycle-accurate ISS to TurboXim, first:
;#   Use prep repeatedly until it prints 1 (1=Ready 0=NotReady).
;#   Tip: Enter prep then hit return repeatedly until 1 prints.
alias prep=sc wait 1;xtsc xtsc_prepare_to_switch_sim_mode 1

;#  After prep prints 1, then use turbo to actually do the switch.
alias turbo=xtsc xtsc_switch_sim_mode 1

;# Use ca to switch from TurboXim to cycle-accurate ISS.
alias ca=xtsc xtsc_prepare_to_switch_sim_mode 0;xtsc xtsc_switch_sim_mode 0

