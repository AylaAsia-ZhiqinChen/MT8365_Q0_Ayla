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


::pct::open_library xtsc_mmio_vp/xtsc_mmio_vp.xml

set module xtsc_core_vp
::pct::create_instance $module          /HARDWARE core0 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_shape                        /HARDWARE/core0 core
::pct::set_param_value                  /HARDWARE/core0 "Scml Properties"    /Target/SimTargetOutput  core0_output.log
::pct::set_param_value                  /HARDWARE/core0 "Scml Properties"    /Target/SimTargetProgram target/core0.out

::pct::set_bounds                       /HARDWARE/core0 100 100 100 125

::pct::set_orientation                  /HARDWARE/core0/EXPSTATE               left
::pct::set_orientation                  /HARDWARE/core0/BInterrupt06           left
::pct::set_orientation                  /HARDWARE/core0/pif_rsp                right
::pct::set_orientation                  /HARDWARE/core0/pif_req                right
::pct::set_location_on_owner            /HARDWARE/core0/EXPSTATE               66
::pct::set_location_on_owner            /HARDWARE/core0/BInterrupt06           25
::pct::set_location_on_owner            /HARDWARE/core0/pif_rsp                55
::pct::set_location_on_owner            /HARDWARE/core0/pif_req                22



set module xtsc_router_vp
::pct::create_instance $module          /HARDWARE router xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/router "Template Arguments" DATA_WIDTH             64
::pct::set_param_value                  /HARDWARE/router "Template Arguments" NUM_SLAVES             2
::pct::set_param_value                  /HARDWARE/router "Scml Properties"    /Misc/default_port_num 0
::pct::set_param_value                  /HARDWARE/router "Scml Properties"    /Misc/routing_table    ../../xtsc_mmio/routing.tab

::pct::set_bounds                       /HARDWARE/router 350 100 100 275

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

::pct::set_bounds                       /HARDWARE/core0_pif 700 100 100 100

::pct::set_orientation                  /HARDWARE/core0_pif/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/core0_pif/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/core0_pif/m_respond_ports\[0\]      top
::pct::set_location_on_owner            /HARDWARE/core0_pif/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/core0_pif/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/core0_pif/m_respond_ports\[0\]      left
::pct::set_location_on_owner            /HARDWARE/core0_pif/m_respond_ports\[0\]      55



set module xtsc_mmio_vp
::pct::create_instance $module          /HARDWARE mmio xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/mmio "Template Arguments" DATA_WIDTH 64
::pct::set_param_value                  /HARDWARE/mmio "Scml Properties"    /Misc/swizzle_bytes false

::pct::set_bounds                       /HARDWARE/mmio 700 250 100 125

::pct::set_orientation                  /HARDWARE/mmio/m_respond_port          left
::pct::set_orientation                  /HARDWARE/mmio/m_request_export        left
::pct::set_orientation                  /HARDWARE/mmio/EXPSTATE                right
::pct::set_orientation                  /HARDWARE/mmio/BInterrupt06            right
::pct::set_location_on_owner            /HARDWARE/mmio/m_respond_port          55
::pct::set_location_on_owner            /HARDWARE/mmio/m_request_export        22
::pct::set_location_on_owner            /HARDWARE/mmio/EXPSTATE                66
::pct::set_location_on_owner            /HARDWARE/mmio/BInterrupt06            25



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

::pct::set_bounds                       /HARDWARE/core1_pif 700 400 100 100

::pct::set_orientation                  /HARDWARE/core1_pif/m_request_exports\[0\]    right
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      right
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      top
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      right
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_respond_ports\[0\]      55



set module xtsc_core_vp
::pct::create_instance $module          /HARDWARE core1 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_shape                        /HARDWARE/core1 core
::pct::set_param_value                  /HARDWARE/core1 "Scml Properties"    /Target/SimTargetOutput  core1_output.log
::pct::set_param_value                  /HARDWARE/core1 "Scml Properties"    /Target/SimTargetProgram target/core1.out

::pct::set_bounds                       /HARDWARE/core1 1000 250 100 275

::pct::set_orientation                  /HARDWARE/core1/pif_rsp                left
::pct::set_orientation                  /HARDWARE/core1/pif_req                left
::pct::set_orientation                  /HARDWARE/core1/EXPSTATE               left
::pct::set_orientation                  /HARDWARE/core1/BInterrupt06           left
::pct::set_location_on_owner            /HARDWARE/core1/pif_rsp                205
::pct::set_location_on_owner            /HARDWARE/core1/pif_req                172
::pct::set_location_on_owner            /HARDWARE/core1/EXPSTATE               66
::pct::set_location_on_owner            /HARDWARE/core1/BInterrupt06           25



::pct::create_connection core0_req   /HARDWARE    /HARDWARE/core0/pif_req                 /HARDWARE/router/m_request_export
::pct::create_connection core0_rsp   /HARDWARE    /HARDWARE/core0/pif_rsp                 /HARDWARE/router/m_respond_port

::pct::create_connection pif_req   /HARDWARE    /HARDWARE/router/m_request_ports\[0\]   /HARDWARE/core0_pif/m_request_exports\[0\]
::pct::create_connection pif_rsp   /HARDWARE    /HARDWARE/router/m_respond_exports\[0\] /HARDWARE/core0_pif/m_respond_ports\[0\]

::pct::create_connection core1_req /HARDWARE    /HARDWARE/core1/pif_req                 /HARDWARE/core1_pif/m_request_exports\[0\]
::pct::create_connection core1_rsp /HARDWARE    /HARDWARE/core1/pif_rsp                 /HARDWARE/core1_pif/m_respond_ports\[0\]

::pct::create_connection mmio_req     /HARDWARE    /HARDWARE/router/m_request_ports\[1\]           /HARDWARE/mmio/m_request_export
::pct::create_connection mmio_rsp     /HARDWARE    /HARDWARE/router/m_respond_exports\[1\]         /HARDWARE/mmio/m_respond_port

::pct::create_connection BInterrupt06 /HARDWARE    /HARDWARE/core1/BInterrupt06       /HARDWARE/mmio/BInterrupt06
::pct::create_connection EXPSTATE     /HARDWARE    /HARDWARE/core1/EXPSTATE           /HARDWARE/mmio/EXPSTATE

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



