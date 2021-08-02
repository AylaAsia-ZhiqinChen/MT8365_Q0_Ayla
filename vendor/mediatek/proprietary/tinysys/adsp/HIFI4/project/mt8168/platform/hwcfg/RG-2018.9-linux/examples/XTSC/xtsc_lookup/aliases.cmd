// Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

;# Use alias w to wait for a lut lookup value on core0
alias w=sc wait core0.lut__driver.m_nb_get_data_event

;# Use alias d to dump the lut interface of core0
;#  Dump format is: <Adddress> <Data> <Rdy>
alias d=core0 dump_interface_values lut

