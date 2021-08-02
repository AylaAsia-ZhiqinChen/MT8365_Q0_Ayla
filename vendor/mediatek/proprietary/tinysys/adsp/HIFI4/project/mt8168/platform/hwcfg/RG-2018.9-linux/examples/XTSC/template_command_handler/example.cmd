// Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// This file can be sourced from the XTSC command prompt to exercise the
// template_command_handler help and commands
// To use it:
//   ./template_command_handler   -xtsc_command_prompt=true
//   template_command_handler.exe -xtsc_command_prompt=true
// Then source this file from the cmd: prompt using the period (.) command:
//   cmd: . example.cmd

;# Get a list of command handlers
?

;# Get a list of commands supported by this template_command_handler
help tch

;# Get a list of commands supported by this template_command_handler with descriptions
man tch

;# Dump xtsc_core info
tch dump_core_info

;# Run the simulation 1000 clock periods
sc wait 1000

;# Again dump xtsc_core info
tch dump_core_info

;# Type 'c' if desired to continue the simulation, type 2 forward slashes (/) to exit
