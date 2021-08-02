# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.



proc bt         {}      { tlmcpu -where }
proc dasm       {args}  { eval tlmcpu -disassemble $args }
proc si         {args}  { stepi $args; dasm }
proc cmd        {args}  { scparam -deposit command $args }
proc core0      {args}  {
                          tlmcpu -focus sc_main.core0;
                          scope sc_main.core0;
                          if { [llength $args] } { scparam -deposit command $args; }
                        }
proc core1      {args}  {
                          tlmcpu -focus sc_main.core1;
                          scope sc_main.core1;
                          if { [llength $args] } { scparam -deposit command $args; }
                        }

if { ([info exists ::env(core1_PROGRAM)]) && ($::env(core1_PROGRAM) != "") } {
scope sc_main.core1
tlmcpu -focus sc_main.core1
tlmcpu -image $::env(core1_PROGRAM)
}

scope sc_main.core0
tlmcpu -focus sc_main.core0
if { ([info exists ::env(core0_PROGRAM)]) && ($::env(core0_PROGRAM) != "") } {
tlmcpu -image $::env(core0_PROGRAM)
}

tlmcpu -describe

