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


::pct::open_library xtsc_wire_logic_vp/xtsc_wire_logic_vp.xml


set module xtsc_queue_producer_vp
::pct::create_instance $module          /HARDWARE loader_driver xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/loader_driver "Template Arguments" DATA_WIDTH  32
::pct::set_param_value                  /HARDWARE/loader_driver "Scml Properties"    /Misc/script_file ../../xtsc_tx_loader/loader_driver.vec

::pct::set_bounds                       /HARDWARE/loader_driver -200 153 200 100




::pct::set_orientation                  /HARDWARE/loader_driver/m_control      bottom

set module xtsc_tx_loader_vp
::pct::create_instance $module          /HARDWARE loader xtsc_vp::$module xtsc_vp::$module\()

::pct::set_bounds                       /HARDWARE/loader 150 75 150 150

::pct::set_orientation                  /HARDWARE/loader/m_tx_xfer_export      left
::pct::set_orientation                  /HARDWARE/loader/m_consumer            left
::pct::set_orientation                  /HARDWARE/loader/m_producer            left
::pct::set_orientation                  /HARDWARE/loader/m_tx_xfer_port        right
::pct::set_orientation                  /HARDWARE/loader/m_done                right
::pct::set_orientation                  /HARDWARE/loader/m_mode                right
::pct::set_location_on_owner            /HARDWARE/loader/m_tx_xfer_export      22
::pct::set_location_on_owner            /HARDWARE/loader/m_consumer            75
::pct::set_location_on_owner            /HARDWARE/loader/m_producer            100
::pct::set_location_on_owner            /HARDWARE/loader/m_tx_xfer_port        22
::pct::set_location_on_owner            /HARDWARE/loader/m_done                75
::pct::set_location_on_owner            /HARDWARE/loader/m_mode                100



set module xtsc_wire_logic_vp
::pct::create_instance $module          /HARDWARE logic xtsc_vp::$module xtsc_vp::$module\()

::pct::set_bounds                       /HARDWARE/logic 150 800 150 150

::pct::set_orientation                  /HARDWARE/logic/JobDone                left
::pct::set_orientation                  /HARDWARE/logic/CoreHalted0            right
::pct::set_orientation                  /HARDWARE/logic/CoreHalted1            right
::pct::set_location_on_owner            /HARDWARE/logic/JobDone                22
::pct::set_location_on_owner            /HARDWARE/logic/CoreHalted0            22
::pct::set_location_on_owner            /HARDWARE/logic/CoreHalted1            97



set module xtsc_core_vp
::pct::create_instance $module          /HARDWARE core0 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_shape                        /HARDWARE/core0 core
::pct::set_param_value                  /HARDWARE/core0 "Scml Properties"    /Hardware/ProcessorID 0

::pct::set_bounds                       /HARDWARE/core0 525 75 150 800

::pct::set_orientation                  /HARDWARE/core0/tx_xfer_export          left
::pct::set_orientation                  /HARDWARE/core0/CoreHalted              left
::pct::set_orientation                  /HARDWARE/core0/tx_xfer_port            right
::pct::set_orientation                  /HARDWARE/core0/INQ1                    right
::pct::set_orientation                  /HARDWARE/core0/OUTQ1                   right
::pct::set_location_on_owner            /HARDWARE/core0/CoreHalted              747
::pct::set_location_on_owner            /HARDWARE/core0/INQ1                    747
::pct::set_location_on_owner            /HARDWARE/core0/OUTQ1                   597

::pct::set_orientation                  /HARDWARE/core0/pif_req                 right
::pct::set_orientation                  /HARDWARE/core0/pif_rsp                 right
::pct::set_location_on_owner            /HARDWARE/core0/pif_rsp                 205
::pct::set_location_on_owner            /HARDWARE/core0/pif_req                 172
::pct::set_location_on_owner            /HARDWARE/core0/pif_rsp                 205

::pct::set_location_on_owner            /HARDWARE/core0/tx_xfer_port            22
::pct::set_location_on_owner            /HARDWARE/core0/tx_xfer_export          22
::pct::set_bounds                       /HARDWARE/core0 525 75 150 800



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

::pct::set_bounds                       /HARDWARE/core0_pif 875 225 150 125

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
::pct::set_param_value                  /HARDWARE/core1 "Scml Properties"    /Hardware/ProcessorID 1

::pct::set_bounds                       /HARDWARE/core1 1125 75 150 875

::pct::set_orientation                  /HARDWARE/core1/tx_xfer_export          left
::pct::set_orientation                  /HARDWARE/core1/CoreHalted              left
::pct::set_orientation                  /HARDWARE/core1/OUTQ1                   left
::pct::set_orientation                  /HARDWARE/core1/INQ1                    left
::pct::set_orientation                  /HARDWARE/core1/tx_xfer_port            right
::pct::set_location_on_owner            /HARDWARE/core1/CoreHalted              822
::pct::set_location_on_owner            /HARDWARE/core1/OUTQ1                   672
::pct::set_location_on_owner            /HARDWARE/core1/INQ1                    522

::pct::set_orientation                  /HARDWARE/core1/pif_req                 right
::pct::set_orientation                  /HARDWARE/core1/pif_rsp                 right
::pct::set_location_on_owner            /HARDWARE/core1/pif_rsp                 205
::pct::set_location_on_owner            /HARDWARE/core1/pif_req                 172
::pct::set_location_on_owner            /HARDWARE/core1/pif_rsp                 205

::pct::set_location_on_owner            /HARDWARE/core1/tx_xfer_port            22
::pct::set_location_on_owner            /HARDWARE/core1/tx_xfer_export          22
::pct::set_bounds                       /HARDWARE/core1 1125 75 150 875



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

::pct::set_bounds                       /HARDWARE/core1_pif 1475 225 150 125

::pct::set_orientation                  /HARDWARE/core1_pif/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      top
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/core1_pif/m_respond_ports\[0\]      left
::pct::set_location_on_owner            /HARDWARE/core1_pif/m_respond_ports\[0\]      55


::pct::create_connection core1_req /HARDWARE    /HARDWARE/core1/pif_req                 /HARDWARE/core1_pif/m_request_exports\[0\]
::pct::create_connection core1_rsp /HARDWARE    /HARDWARE/core1/pif_rsp                 /HARDWARE/core1_pif/m_respond_ports\[0\]



::pct::set_location_on_owner /HARDWARE/core1/INQ1  597
::pct::set_location_on_owner /HARDWARE/core1/OUTQ1 747

set module xtsc_queue_vp
::pct::create_instance $module          /HARDWARE Q01 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/Q01 "Template Arguments" DATA_WIDTH    96
::pct::set_param_value                  /HARDWARE/Q01 "Scml Properties"    /Misc/depth 2

::pct::set_bounds                       /HARDWARE/Q01 850 650 150 75

::pct::set_orientation                  /HARDWARE/Q01/m_producer    left
::pct::set_orientation                  /HARDWARE/Q01/m_consumer    right

::pct::set_location_on_owner            /HARDWARE/Q01/m_producer    22
::pct::set_location_on_owner            /HARDWARE/Q01/m_consumer    22



set module xtsc_queue_vp
::pct::create_instance $module          /HARDWARE Q10 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/Q10 "Template Arguments" DATA_WIDTH    96
::pct::set_param_value                  /HARDWARE/Q10 "Scml Properties"    /Misc/depth 2

::pct::set_bounds                       /HARDWARE/Q10 850 800 150 75

::pct::set_orientation                  /HARDWARE/Q10/m_producer    right
::pct::set_orientation                  /HARDWARE/Q10/m_consumer    left

::pct::set_location_on_owner            /HARDWARE/Q10/m_producer    22
::pct::set_location_on_owner            /HARDWARE/Q10/m_consumer    22



::pct::create_connection        xfer_beg        /HARDWARE       /HARDWARE/loader/m_tx_xfer_port         /HARDWARE/core0/tx_xfer_export
::pct::create_connection        xfer_01         /HARDWARE       /HARDWARE/core0/tx_xfer_port            /HARDWARE/core1/tx_xfer_export
::pct::create_connection        xfer_end        /HARDWARE       /HARDWARE/core1/tx_xfer_port            /HARDWARE/loader/m_tx_xfer_export
::pct::create_connection        cmd             /HARDWARE       /HARDWARE/loader_driver/m_queue         /HARDWARE/loader/m_producer
::pct::create_connection        JobDone         /HARDWARE       /HARDWARE/logic/JobDone                 /HARDWARE/loader_driver/m_control
::pct::create_connection        CoreHalted0     /HARDWARE       /HARDWARE/core0/CoreHalted              /HARDWARE/logic/CoreHalted0
::pct::create_connection        CoreHalted1     /HARDWARE       /HARDWARE/core1/CoreHalted              /HARDWARE/logic/CoreHalted1
::pct::create_connection        core02q         /HARDWARE       /HARDWARE/core0/OUTQ1                   /HARDWARE/Q01/m_producer
::pct::create_connection        q2core1         /HARDWARE       /HARDWARE/Q01/m_consumer                /HARDWARE/core1/INQ1
::pct::create_connection        core12q         /HARDWARE       /HARDWARE/core1/OUTQ1                   /HARDWARE/Q10/m_producer
::pct::create_connection        q2core0         /HARDWARE       /HARDWARE/Q10/m_consumer                /HARDWARE/core0/INQ1


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


