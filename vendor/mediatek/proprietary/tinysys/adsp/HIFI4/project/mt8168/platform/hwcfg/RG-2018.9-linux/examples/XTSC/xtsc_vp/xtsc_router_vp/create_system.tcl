# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
::pct::set_param_value                  /HARDWARE/core0 "Scml Properties"    /Target/SimTargetProgram target/router_test.out

::pct::set_bounds                       /HARDWARE/core0 100 100 100 125

::pct::set_orientation                  /HARDWARE/core0/pif_req             right
::pct::set_orientation                  /HARDWARE/core0/pif_rsp             right
::pct::set_location_on_owner            /HARDWARE/core0/pif_req             22
::pct::set_location_on_owner            /HARDWARE/core0/pif_rsp             55

::pct::set_bounds                       /HARDWARE/core0 100 100 100 125



set module xtsc_router_vp
::pct::create_instance $module          /HARDWARE router xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/router "Template Arguments" DATA_WIDTH             64
::pct::set_param_value                  /HARDWARE/router "Template Arguments" NUM_SLAVES             2
::pct::set_param_value                  /HARDWARE/router "Scml Properties"    /Misc/default_port_num 1
::pct::set_param_value                  /HARDWARE/router "Scml Properties"    /Misc/routing_table    ../../xtsc_router/routing.tab

::pct::set_bounds                       /HARDWARE/router 400 100 100 275

::pct::set_orientation                  /HARDWARE/router/m_request_export          left
::pct::set_orientation                  /HARDWARE/router/m_respond_port            left
::pct::set_orientation                  /HARDWARE/router/m_request_ports\[0\]      right
::pct::set_orientation                  /HARDWARE/router/m_respond_exports\[0\]    right
::pct::set_orientation                  /HARDWARE/router/m_request_ports\[1\]      right
::pct::set_orientation                  /HARDWARE/router/m_respond_exports\[1\]    right
::pct::set_location_on_owner            /HARDWARE/router/m_respond_exports\[1\]    205
::pct::set_location_on_owner            /HARDWARE/router/m_request_ports\[1\]      172
::pct::set_location_on_owner            /HARDWARE/router/m_respond_exports\[0\]    55
::pct::set_location_on_owner            /HARDWARE/router/m_request_ports\[0\]      22
::pct::set_location_on_owner            /HARDWARE/router/m_respond_exports\[0\]    55
::pct::set_location_on_owner            /HARDWARE/router/m_respond_port            55
::pct::set_location_on_owner            /HARDWARE/router/m_request_export          22



set module xtsc_memory_vp
::pct::create_instance $module          /HARDWARE sysrom xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/sysrom "Template Arguments" DATA_WIDTH                    64
::pct::set_param_value                  /HARDWARE/sysrom "Template Arguments" NUM_PORTS                     1
::pct::set_param_value                  /HARDWARE/sysrom "Scml Properties"    /Timing/block_read_delay      1
::pct::set_param_value                  /HARDWARE/sysrom "Scml Properties"    /Timing/block_write_delay     1
::pct::set_param_value                  /HARDWARE/sysrom "Scml Properties"    /Timing/block_write_repeat    1
::pct::set_param_value                  /HARDWARE/sysrom "Scml Properties"    /Timing/block_write_response  1
::pct::set_param_value                  /HARDWARE/sysrom "Scml Properties"    /Timing/rcw_response          1
::pct::set_param_value                  /HARDWARE/sysrom "Scml Properties"    /Timing/read_delay            1
::pct::set_param_value                  /HARDWARE/sysrom "Scml Properties"    /Timing/write_delay           1

::pct::set_bounds                       /HARDWARE/sysrom 750 100 100 100

::pct::set_orientation                  /HARDWARE/sysrom/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/sysrom/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/sysrom/m_respond_ports\[0\]      top
::pct::set_location_on_owner            /HARDWARE/sysrom/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/sysrom/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/sysrom/m_respond_ports\[0\]      left
::pct::set_location_on_owner            /HARDWARE/sysrom/m_respond_ports\[0\]      55



set module xtsc_memory_vp
::pct::create_instance $module          /HARDWARE sysram xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/sysram "Template Arguments" DATA_WIDTH                    64
::pct::set_param_value                  /HARDWARE/sysram "Template Arguments" NUM_PORTS                     1
::pct::set_param_value                  /HARDWARE/sysram "Scml Properties"    /Timing/block_read_delay      1
::pct::set_param_value                  /HARDWARE/sysram "Scml Properties"    /Timing/block_write_delay     1
::pct::set_param_value                  /HARDWARE/sysram "Scml Properties"    /Timing/block_write_repeat    1
::pct::set_param_value                  /HARDWARE/sysram "Scml Properties"    /Timing/block_write_response  1
::pct::set_param_value                  /HARDWARE/sysram "Scml Properties"    /Timing/rcw_response          1
::pct::set_param_value                  /HARDWARE/sysram "Scml Properties"    /Timing/read_delay            1
::pct::set_param_value                  /HARDWARE/sysram "Scml Properties"    /Timing/write_delay           1

::pct::set_bounds                       /HARDWARE/sysram 750 250 100 100

::pct::set_orientation                  /HARDWARE/sysram/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/sysram/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/sysram/m_respond_ports\[0\]      top
::pct::set_location_on_owner            /HARDWARE/sysram/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/sysram/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/sysram/m_respond_ports\[0\]      left
::pct::set_location_on_owner            /HARDWARE/sysram/m_respond_ports\[0\]      55



::pct::create_connection pif_req   /HARDWARE    /HARDWARE/core0/pif_req                 /HARDWARE/router/m_request_export
::pct::create_connection pif_rsp   /HARDWARE    /HARDWARE/core0/pif_rsp                 /HARDWARE/router/m_respond_port

::pct::create_connection rom_req   /HARDWARE    /HARDWARE/router/m_request_ports\[0\]   /HARDWARE/sysrom/m_request_exports\[0\]
::pct::create_connection rom_rsp   /HARDWARE    /HARDWARE/router/m_respond_exports\[0\] /HARDWARE/sysrom/m_respond_ports\[0\]

::pct::create_connection ram_req   /HARDWARE    /HARDWARE/router/m_request_ports\[1\]   /HARDWARE/sysram/m_request_exports\[0\]
::pct::create_connection ram_rsp   /HARDWARE    /HARDWARE/router/m_respond_exports\[1\] /HARDWARE/sysram/m_respond_ports\[0\]

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



