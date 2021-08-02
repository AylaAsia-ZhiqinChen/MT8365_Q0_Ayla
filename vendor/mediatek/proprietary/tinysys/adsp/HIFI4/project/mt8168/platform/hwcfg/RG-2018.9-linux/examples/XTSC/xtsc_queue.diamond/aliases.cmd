// Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

;# Use alias w0 to wait until core0 successfully pushes to OPQ
alias w0=sc wait core0.OPQ__driver.m_nb_push_event

;# Use alias w1 to wait until core1 successfully pops from IPQ
alias w1=sc wait core1.IPQ__driver.m_nb_pop_event

;# Use alias wf to wait until Q1 is full (core0 push fails)
alias wf=sc wait core0.OPQ__driver.m_nb_can_push_failed_event

;# Use alias we to wait until Q1 is empty (core1 pop fails)
alias we=sc wait core1.IPQ__driver.m_cannot_pop_event

;# Use alias dq to dump the contents of Q1
alias dq=Q1 dump

;# Use alias d0 to dump the last values going out core0's OPQ interface
alias d0=core0 dump_interface_values OPQ

;# Use alias d1 to dump the last values coming in core1's IPQ interface
alias d1=core1 dump_interface_values IPQ

;# Use alias stall0 to stall core0
alias stall0=core0 set_stall 1

;# Use alias stall1 to stall core1
alias stall1=core1 set_stall 1

;# Use alias run0 to resume running core0
alias run0=core0 set_stall 0

;# Use alias run1 to resume running core1
alias run1=core1 set_stall 0

