# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.


variable module                xtsc_core_vp
variable xtsc_vp_static        [info exists ::env(XTSC_VP_STATIC)]
variable libname               [expr { $xtsc_vp_static ? "xtsc_vp" : "xtsc_vp_sh" }]

::pct::new_project
::pct::open_library $env(XTENSA_SW_TOOLS)/misc/xtsc_vp/${libname}.xml
::pct::open_library $module/$module.xml



set module xtsc_core_vp
::pct::create_instance $module          /HARDWARE core0 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_shape                        /HARDWARE/core0 core
::pct::set_param_value                  /HARDWARE/core0 "Scml Properties"    /Target/SimTargetOutput  core0_output.log
::pct::set_param_value                  /HARDWARE/core0 "Scml Properties"    /Target/SimTargetProgram target/source.out

::pct::set_bounds                       /HARDWARE/core0 100 100 100 500

::pct::set_orientation                  /HARDWARE/core0/TIE_control             left
::pct::set_orientation                  /HARDWARE/core0/TIE_status              right
::pct::set_location_on_owner            /HARDWARE/core0/TIE_control             447
::pct::set_location_on_owner            /HARDWARE/core0/TIE_status              447

::pct::set_orientation                  /HARDWARE/core0/pif_req                 right
::pct::set_orientation                  /HARDWARE/core0/pif_rsp                 right
::pct::set_location_on_owner            /HARDWARE/core0/pif_rsp                 55
::pct::set_location_on_owner            /HARDWARE/core0/pif_req                 22
::pct::set_location_on_owner            /HARDWARE/core0/pif_rsp                 55

::pct::set_bounds                       /HARDWARE/core0 100 100 100 500



set module xtsc_memory_vp
::pct::create_instance $module          /HARDWARE core0_pif xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/core0_pif "Template Arguments" DATA_WIDTH                    64
::pct::set_param_value                  /HARDWARE/core0_pif "Template Arguments" NUM_PORTS                     1
::pct::set_param_value                  /HARDWARE/core0_pif "Scml Properties"    /Timing/block_read_delay      1
::pct::set_param_value                  /HARDWARE/core0_pif "Scml Properties"    /Timing/block_write_delay     1
::pct::set_param_value                  /HARDWARE/core0_pif "Scml Properties"    /Timing/block_write_repeat    1
::pct::set_param_value                  /HARDWARE/core0_pif "Scml Properties"    /Timing/block_write_response  1
::pct::set_param_value                  /HARDWARE/core0_pif "Scml Properties"    /Timing/rcw_response          1
::pct::set_param_value                  /HARDWARE/core0_pif "Scml Properties"    /Timing/read_delay            1
::pct::set_param_value                  /HARDWARE/core0_pif "Scml Properties"    /Timing/write_delay           1

::pct::set_bounds                       /HARDWARE/core0_pif 450 100 150 125

::pct::set_orientation                  /HARDWARE/core0_pif/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/core0_pif/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/core0_pif/m_respond_ports\[0\]      top
::pct::set_location_on_owner            /HARDWARE/core0_pif/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/core0_pif/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/core0_pif/m_respond_ports\[0\]      left
::pct::set_location_on_owner            /HARDWARE/core0_pif/m_respond_ports\[0\]      55


::pct::create_connection core0_req /HARDWARE    /HARDWARE/core0/pif_req                 /HARDWARE/core0_pif/m_request_exports\[0\]
::pct::create_connection core0_rsp /HARDWARE    /HARDWARE/core0/pif_rsp                 /HARDWARE/core0_pif/m_respond_ports\[0\]



set module xtsc_core_vp
::pct::create_instance $module          /HARDWARE core1 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_shape                        /HARDWARE/core1 core
::pct::set_param_value                  /HARDWARE/core1 "Scml Properties"    /Target/SimTargetOutput  core1_output.log
::pct::set_param_value                  /HARDWARE/core1 "Scml Properties"    /Target/SimTargetProgram target/sink.out

::pct::set_bounds                       /HARDWARE/core1 750 100 100 500

::pct::set_orientation                  /HARDWARE/core1/TIE_control             left
::pct::set_orientation                  /HARDWARE/core1/TIE_status              right
::pct::set_location_on_owner            /HARDWARE/core1/TIE_control             447
::pct::set_location_on_owner            /HARDWARE/core1/TIE_status              447

::pct::set_orientation                  /HARDWARE/core1/pif_req                 right
::pct::set_orientation                  /HARDWARE/core1/pif_rsp                 right
::pct::set_location_on_owner            /HARDWARE/core1/pif_rsp                 55
::pct::set_location_on_owner            /HARDWARE/core1/pif_req                 22
::pct::set_location_on_owner            /HARDWARE/core1/pif_rsp                 55

::pct::set_bounds                       /HARDWARE/core1 750 100 100 500



set module xtsc_memory_vp
::pct::create_instance $module          /HARDWARE core1_pif xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/core1_pif "Template Arguments" DATA_WIDTH                    64
::pct::set_param_value                  /HARDWARE/core1_pif "Template Arguments" NUM_PORTS                     1
::pct::set_param_value                  /HARDWARE/core1_pif "Scml Properties"    /Timing/block_read_delay      1
::pct::set_param_value                  /HARDWARE/core1_pif "Scml Properties"    /Timing/block_write_delay     1
::pct::set_param_value                  /HARDWARE/core1_pif "Scml Properties"    /Timing/block_write_repeat    1
::pct::set_param_value                  /HARDWARE/core1_pif "Scml Properties"    /Timing/block_write_response  1
::pct::set_param_value                  /HARDWARE/core1_pif "Scml Properties"    /Timing/rcw_response          1
::pct::set_param_value                  /HARDWARE/core1_pif "Scml Properties"    /Timing/read_delay            1
::pct::set_param_value                  /HARDWARE/core1_pif "Scml Properties"    /Timing/write_delay           1

::pct::set_bounds                       /HARDWARE/core1_pif 1100 100 150 125

::pct::set_orientation                  /HARDWARE/core1_pif/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      top
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      left
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_respond_ports\[0\]      55


::pct::create_connection core1_req /HARDWARE    /HARDWARE/core1/pif_req                 /HARDWARE/core1_pif/m_request_exports\[0\]
::pct::create_connection core1_rsp /HARDWARE    /HARDWARE/core1/pif_rsp                 /HARDWARE/core1_pif/m_respond_ports\[0\]



::pct::create_connection core0_to_core1 /HARDWARE /HARDWARE/core0/TIE_status  /HARDWARE/core1/TIE_control 

::pct::save_system system.xml



::pct::create_simulation_build_project .

::pct::set_simulation_build_project_setting Debug "Disable Elaboration"              true
::pct::set_simulation_build_project_setting Debug "Enable Instrumentation"           false
::pct::set_simulation_build_project_setting Debug "Fast Linking"                     false
::pct::set_simulation_build_project_setting Debug "Include Path Substitution"        false
::pct::set_simulation_build_project_setting Debug "Backdoor Mode"                    true

::pct::set_simulation_build_project_setting Debug "Include Paths" " \
                                     $env(XTENSA_SW_TOOLS)/include \
                                     "

::pct::set_preference_value /Messages/FilterIDs "ESA0022 ESA0029"
::pct::export_simulation



