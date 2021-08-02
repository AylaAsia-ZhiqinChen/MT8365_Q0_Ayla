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
::pct::set_param_value                  /HARDWARE/core0 "Scml Properties"    /Target/SimTargetProgram target/producer.out

::pct::set_bounds                       /HARDWARE/core0 100 100 100 500

::pct::set_orientation                  /HARDWARE/core0/TIE_INQ1_PopReq         left
::pct::set_orientation                  /HARDWARE/core0/TIE_INQ1_Empty          left
::pct::set_orientation                  /HARDWARE/core0/TIE_INQ1                left
::pct::set_orientation                  /HARDWARE/core0/TIE_OUTQ1_PushReq       right
::pct::set_orientation                  /HARDWARE/core0/TIE_OUTQ1_Full          right
::pct::set_orientation                  /HARDWARE/core0/TIE_OUTQ1               right
::pct::set_location_on_owner            /HARDWARE/core0/TIE_INQ1_PopReq         447
::pct::set_location_on_owner            /HARDWARE/core0/TIE_INQ1_Empty          414
::pct::set_location_on_owner            /HARDWARE/core0/TIE_INQ1                381
::pct::set_location_on_owner            /HARDWARE/core0/TIE_OUTQ1_PushReq       447
::pct::set_location_on_owner            /HARDWARE/core0/TIE_OUTQ1_Full          414
::pct::set_location_on_owner            /HARDWARE/core0/TIE_OUTQ1               381

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
::pct::set_param_value                  /HARDWARE/core1 "Scml Properties"    /Target/SimTargetProgram target/consumer.out

::pct::set_bounds                       /HARDWARE/core1 750 100 100 500

::pct::set_orientation                  /HARDWARE/core1/TIE_INQ1_PopReq         left
::pct::set_orientation                  /HARDWARE/core1/TIE_INQ1_Empty          left
::pct::set_orientation                  /HARDWARE/core1/TIE_INQ1                left
::pct::set_orientation                  /HARDWARE/core1/TIE_OUTQ1_PushReq       right
::pct::set_orientation                  /HARDWARE/core1/TIE_OUTQ1_Full          right
::pct::set_orientation                  /HARDWARE/core1/TIE_OUTQ1               right
::pct::set_location_on_owner            /HARDWARE/core1/TIE_INQ1_PopReq         447
::pct::set_location_on_owner            /HARDWARE/core1/TIE_INQ1_Empty          414
::pct::set_location_on_owner            /HARDWARE/core1/TIE_INQ1                381
::pct::set_location_on_owner            /HARDWARE/core1/TIE_OUTQ1_PushReq       447
::pct::set_location_on_owner            /HARDWARE/core1/TIE_OUTQ1_Full          414
::pct::set_location_on_owner            /HARDWARE/core1/TIE_OUTQ1               381

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



set module xtsc_queue_pin_vp
::pct::create_instance $module          /HARDWARE queue xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/queue "Template Arguments" DATA_WIDTH    96
::pct::set_param_value                  /HARDWARE/queue "Scml Properties"    /Misc/depth 2

::pct::set_bounds                       /HARDWARE/queue 410 459 150 128

::pct::set_orientation                  /HARDWARE/queue/m_data_in     left
::pct::set_orientation                  /HARDWARE/queue/m_full        left
::pct::set_orientation                  /HARDWARE/queue/m_push        left
::pct::set_orientation                  /HARDWARE/queue/m_data_out    right
::pct::set_orientation                  /HARDWARE/queue/m_empty       right
::pct::set_orientation                  /HARDWARE/queue/m_pop         right

::pct::set_location_on_owner            /HARDWARE/queue/m_data_in     22
::pct::set_location_on_owner            /HARDWARE/queue/m_full        55
::pct::set_location_on_owner            /HARDWARE/queue/m_push        88
::pct::set_location_on_owner            /HARDWARE/queue/m_data_out    22
::pct::set_location_on_owner            /HARDWARE/queue/m_empty       55
::pct::set_location_on_owner            /HARDWARE/queue/m_pop         88



::pct::create_connection push_data /HARDWARE /HARDWARE/core0/TIE_OUTQ1         /HARDWARE/queue/m_data_in
::pct::create_connection full      /HARDWARE /HARDWARE/core0/TIE_OUTQ1_Full    /HARDWARE/queue/m_full
::pct::create_connection push      /HARDWARE /HARDWARE/core0/TIE_OUTQ1_PushReq /HARDWARE/queue/m_push

::pct::create_connection pop_data  /HARDWARE /HARDWARE/core1/TIE_INQ1          /HARDWARE/queue/m_data_out
::pct::create_connection empty     /HARDWARE /HARDWARE/core1/TIE_INQ1_Empty    /HARDWARE/queue/m_empty
::pct::create_connection pop       /HARDWARE /HARDWARE/core1/TIE_INQ1_PopReq   /HARDWARE/queue/m_pop

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



