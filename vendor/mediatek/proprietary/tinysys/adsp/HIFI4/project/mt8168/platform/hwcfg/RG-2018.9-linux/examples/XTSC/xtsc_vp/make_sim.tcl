# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2018 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.


variable ::xtsc_vp_static       [info exists ::env(XTSC_VP_STATIC)]
variable libname                [expr { $::xtsc_vp_static ? "xtsc_vp" : "xtsc_vp_sh" }]

::pct::open_library $env(XTENSA_SW_TOOLS)/misc/xtsc_vp/$libname.xml

::scsh::open-project
::scsh::cosim::enable_hdl_sdi
::scsh::build-options -skip-elab off
::scsh::build-options -cache-objects off

::scsh::build
::scsh::elab sim



