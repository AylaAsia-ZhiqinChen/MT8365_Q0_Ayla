# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
::pct::set_param_value                  /HARDWARE/core0 "Scml Properties"    /Target/SimTargetProgram target/main.out

::pct::set_bounds                       /HARDWARE/core0 100 100 100 1000

::pct::set_orientation                  /HARDWARE/core0/inbound_pif_req        right
::pct::set_orientation                  /HARDWARE/core0/inbound_pif_rsp        right
::pct::set_orientation                  /HARDWARE/core0/dram0p1_rsp            right
::pct::set_orientation                  /HARDWARE/core0/dram0p1_req            right
::pct::set_orientation                  /HARDWARE/core0/dram0p0_rsp            right
::pct::set_orientation                  /HARDWARE/core0/dram0p0_req            right
::pct::set_orientation                  /HARDWARE/core0/pif_rsp                right
::pct::set_orientation                  /HARDWARE/core0/pif_req                right
::pct::set_orientation                  /HARDWARE/core0/BInterrupt06           right
::pct::set_location_on_owner            /HARDWARE/core0/inbound_pif_req        955
::pct::set_location_on_owner            /HARDWARE/core0/inbound_pif_rsp        922
::pct::set_location_on_owner            /HARDWARE/core0/dram0p1_rsp            805
::pct::set_location_on_owner            /HARDWARE/core0/dram0p1_req            772
::pct::set_location_on_owner            /HARDWARE/core0/dram0p0_rsp            655
::pct::set_location_on_owner            /HARDWARE/core0/dram0p0_req            622
::pct::set_location_on_owner            /HARDWARE/core0/pif_rsp                255
::pct::set_location_on_owner            /HARDWARE/core0/pif_req                222
::pct::set_location_on_owner            /HARDWARE/core0/BInterrupt06           22




set module xtsc_memory_vp
::pct::create_instance $module          /HARDWARE dram0 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/dram0 "Template Arguments" DATA_WIDTH                    64
::pct::set_param_value                  /HARDWARE/dram0 "Template Arguments" NUM_PORTS                     2
::pct::set_param_value                  /HARDWARE/dram0 "Scml Properties"    /Timing/block_read_delay      1
::pct::set_param_value                  /HARDWARE/dram0 "Scml Properties"    /Timing/block_write_delay     1
::pct::set_param_value                  /HARDWARE/dram0 "Scml Properties"    /Timing/block_write_repeat    1
::pct::set_param_value                  /HARDWARE/dram0 "Scml Properties"    /Timing/block_write_response  1
::pct::set_param_value                  /HARDWARE/dram0 "Scml Properties"    /Timing/rcw_response          1
::pct::set_param_value                  /HARDWARE/dram0 "Scml Properties"    /Timing/read_delay            1
::pct::set_param_value                  /HARDWARE/dram0 "Scml Properties"    /Timing/write_delay           1

::pct::set_bounds                       /HARDWARE/dram0 400 700 100 300

::pct::set_orientation                  /HARDWARE/dram0/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/dram0/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/dram0/m_request_exports\[1\]    left
::pct::set_orientation                  /HARDWARE/dram0/m_respond_ports\[1\]      left
::pct::set_orientation                  /HARDWARE/dram0/m_respond_ports\[1\]      top
::pct::set_location_on_owner            /HARDWARE/dram0/m_respond_ports\[1\]      205
::pct::set_location_on_owner            /HARDWARE/dram0/m_request_exports\[1\]    172
::pct::set_location_on_owner            /HARDWARE/dram0/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/dram0/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/dram0/m_respond_ports\[1\]      left
::pct::set_location_on_owner            /HARDWARE/dram0/m_respond_ports\[1\]      205



set module xtsc_router_vp
::pct::create_instance $module          /HARDWARE core0_router xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/core0_router "Template Arguments" DATA_WIDTH               64
::pct::set_param_value                  /HARDWARE/core0_router "Template Arguments" NUM_SLAVES               3
::pct::set_param_value                  /HARDWARE/core0_router "Scml Properties"    /Misc/default_port_num   0
::pct::set_param_value                  /HARDWARE/core0_router "Scml Properties"    /Misc/routing_table      ../../xtsc_dma_engine/core0_router.tab
::pct::set_param_value                  /HARDWARE/core0_router "Scml Properties"    /Timing/immediate_timing true

::pct::set_bounds                       /HARDWARE/core0_router 400 150 150 450

::pct::set_orientation                  /HARDWARE/core0_router/m_request_export left
::pct::set_orientation                  /HARDWARE/core0_router/m_respond_port  left
::pct::set_orientation                  /HARDWARE/core0_router/m_respond_exports\[0\] right
::pct::set_orientation                  /HARDWARE/core0_router/m_request_ports\[0\] right
::pct::set_orientation                  /HARDWARE/core0_router/m_respond_exports\[1\] right
::pct::set_orientation                  /HARDWARE/core0_router/m_request_ports\[1\] right
::pct::set_orientation                  /HARDWARE/core0_router/m_respond_exports\[2\] right
::pct::set_orientation                  /HARDWARE/core0_router/m_request_ports\[2\] right
::pct::set_location_on_owner            /HARDWARE/core0_router/m_request_export 172
::pct::set_location_on_owner            /HARDWARE/core0_router/m_respond_port  205
::pct::set_location_on_owner            /HARDWARE/core0_router/m_respond_exports\[0\] 355
::pct::set_location_on_owner            /HARDWARE/core0_router/m_request_ports\[0\] 322
::pct::set_location_on_owner            /HARDWARE/core0_router/m_respond_exports\[1\] 205
::pct::set_location_on_owner            /HARDWARE/core0_router/m_request_ports\[1\] 172
::pct::set_location_on_owner            /HARDWARE/core0_router/m_respond_exports\[2\] 55
::pct::set_location_on_owner            /HARDWARE/core0_router/m_request_ports\[2\] 22



set module xtsc_dma_engine_vp
::pct::create_instance $module          /HARDWARE dma xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/dma "Template Arguments" DATA_WIDTH              64
::pct::set_param_value                  /HARDWARE/dma "Scml Properties"    /DMA/clear_notify_value true
::pct::set_param_value                  /HARDWARE/dma "Scml Properties"    /DMA/reg_base_address   0x20001000

::pct::set_bounds                       /HARDWARE/dma 800 300 150 150

::pct::set_orientation                  /HARDWARE/dma/m_request_export         left
::pct::set_orientation                  /HARDWARE/dma/m_respond_port           left
::pct::set_orientation                  /HARDWARE/dma/m_request_port           right
::pct::set_orientation                  /HARDWARE/dma/m_respond_export         right
::pct::set_location_on_owner            /HARDWARE/dma/m_request_export         22
::pct::set_location_on_owner            /HARDWARE/dma/m_respond_port           55
::pct::set_location_on_owner            /HARDWARE/dma/m_request_port           22
::pct::set_location_on_owner            /HARDWARE/dma/m_respond_export         55



set module xtsc_router_vp
::pct::create_instance $module          /HARDWARE dma_router xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/dma_router "Template Arguments" DATA_WIDTH               64
::pct::set_param_value                  /HARDWARE/dma_router "Template Arguments" NUM_SLAVES               3
::pct::set_param_value                  /HARDWARE/dma_router "Scml Properties"    /Misc/default_port_num   0
::pct::set_param_value                  /HARDWARE/dma_router "Scml Properties"    /Misc/routing_table      ../../xtsc_dma_engine/dma_router.tab
::pct::set_param_value                  /HARDWARE/dma_router "Scml Properties"    /Timing/immediate_timing true

::pct::set_bounds                       /HARDWARE/dma_router 1200 300 150 450

::pct::set_orientation                  /HARDWARE/dma_router/m_request_export  left
::pct::set_orientation                  /HARDWARE/dma_router/m_respond_port    left
::pct::set_orientation                  /HARDWARE/dma_router/m_respond_exports\[0\] right
::pct::set_orientation                  /HARDWARE/dma_router/m_request_ports\[0\] right
::pct::set_orientation                  /HARDWARE/dma_router/m_respond_exports\[1\] right
::pct::set_orientation                  /HARDWARE/dma_router/m_request_ports\[1\] right
::pct::set_orientation                  /HARDWARE/dma_router/m_respond_exports\[2\] right
::pct::set_orientation                  /HARDWARE/dma_router/m_request_ports\[2\] right
::pct::set_location_on_owner            /HARDWARE/dma_router/m_request_export  22
::pct::set_location_on_owner            /HARDWARE/dma_router/m_respond_port    55
::pct::set_location_on_owner            /HARDWARE/dma_router/m_respond_exports\[0\] 355
::pct::set_location_on_owner            /HARDWARE/dma_router/m_request_ports\[0\] 322
::pct::set_location_on_owner            /HARDWARE/dma_router/m_respond_exports\[1\] 205
::pct::set_location_on_owner            /HARDWARE/dma_router/m_request_ports\[1\] 172
::pct::set_location_on_owner            /HARDWARE/dma_router/m_respond_exports\[2\] 55
::pct::set_location_on_owner            /HARDWARE/dma_router/m_request_ports\[2\] 22



set module xtsc_arbiter_vp
::pct::create_instance $module          /HARDWARE pif_arbiter xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/pif_arbiter "Template Arguments" DATA_WIDTH             64
::pct::set_param_value                  /HARDWARE/pif_arbiter "Template Arguments" NUM_MASTERS            2

::pct::set_bounds                       /HARDWARE/pif_arbiter 1600 600 150 300

::pct::set_orientation                  /HARDWARE/pif_arbiter/m_request_port            right
::pct::set_orientation                  /HARDWARE/pif_arbiter/m_respond_export          right
::pct::set_orientation                  /HARDWARE/pif_arbiter/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/pif_arbiter/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/pif_arbiter/m_request_exports\[1\]    left
::pct::set_orientation                  /HARDWARE/pif_arbiter/m_respond_ports\[1\]      left
::pct::set_location_on_owner            /HARDWARE/pif_arbiter/m_respond_ports\[1\]      205
::pct::set_location_on_owner            /HARDWARE/pif_arbiter/m_request_exports\[1\]    172
::pct::set_location_on_owner            /HARDWARE/pif_arbiter/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/pif_arbiter/m_request_exports\[0\]    22
::pct::set_location_on_owner            /HARDWARE/pif_arbiter/m_respond_export          55
::pct::set_location_on_owner            /HARDWARE/pif_arbiter/m_request_port            22



set module xtsc_memory_vp
::pct::create_instance $module          /HARDWARE pifmem xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/pifmem "Template Arguments" DATA_WIDTH                    64
::pct::set_param_value                  /HARDWARE/pifmem "Template Arguments" NUM_PORTS                     1
::pct::set_param_value                  /HARDWARE/pifmem "Scml Properties"    /Misc/initial_value_file ../../xtsc_dma_engine/pifmem.dat
::pct::set_param_value                  /HARDWARE/pifmem "Scml Properties"    /Timing/block_read_delay      1
::pct::set_param_value                  /HARDWARE/pifmem "Scml Properties"    /Timing/block_write_delay     1
::pct::set_param_value                  /HARDWARE/pifmem "Scml Properties"    /Timing/block_write_repeat    1
::pct::set_param_value                  /HARDWARE/pifmem "Scml Properties"    /Timing/block_write_response  1
::pct::set_param_value                  /HARDWARE/pifmem "Scml Properties"    /Timing/rcw_response          1
::pct::set_param_value                  /HARDWARE/pifmem "Scml Properties"    /Timing/read_delay            1
::pct::set_param_value                  /HARDWARE/pifmem "Scml Properties"    /Timing/write_delay           1

::pct::set_bounds                       /HARDWARE/pifmem 2000 600 100 300

::pct::set_orientation                  /HARDWARE/pifmem/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/pifmem/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/pifmem/m_respond_ports\[0\]      top
::pct::set_location_on_owner            /HARDWARE/pifmem/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/pifmem/m_request_exports\[0\]    22
::pct::set_orientation                  /HARDWARE/pifmem/m_respond_ports\[0\]      left
::pct::set_location_on_owner            /HARDWARE/pifmem/m_respond_ports\[0\]      55



set module xtsc_mmio_vp
::pct::create_instance $module          /HARDWARE mmio xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/mmio "Template Arguments" DATA_WIDTH 64
::pct::set_param_value                  /HARDWARE/mmio "Scml Properties"    /Misc/swizzle_bytes false

::pct::set_bounds                       /HARDWARE/mmio 2000 150 100 150

::pct::set_orientation                  /HARDWARE/mmio/m_request_export        left
::pct::set_orientation                  /HARDWARE/mmio/m_respond_port          left
::pct::set_orientation                  /HARDWARE/mmio/BInterrupt06            right
::pct::set_location_on_owner            /HARDWARE/mmio/m_request_export        22
::pct::set_location_on_owner            /HARDWARE/mmio/m_respond_port          55
::pct::set_location_on_owner            /HARDWARE/mmio/BInterrupt06            22



set module xtsc_arbiter_vp
::pct::create_instance $module          /HARDWARE mmio_arbiter xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/mmio_arbiter "Template Arguments" DATA_WIDTH             64
::pct::set_param_value                  /HARDWARE/mmio_arbiter "Template Arguments" NUM_MASTERS            2

::pct::set_bounds                       /HARDWARE/mmio_arbiter 1600 150 150 300

::pct::set_orientation                  /HARDWARE/mmio_arbiter/m_request_port            right
::pct::set_orientation                  /HARDWARE/mmio_arbiter/m_respond_export          right
::pct::set_orientation                  /HARDWARE/mmio_arbiter/m_request_exports\[0\]    left
::pct::set_orientation                  /HARDWARE/mmio_arbiter/m_respond_ports\[0\]      left
::pct::set_orientation                  /HARDWARE/mmio_arbiter/m_request_exports\[1\]    left
::pct::set_orientation                  /HARDWARE/mmio_arbiter/m_respond_ports\[1\]      left
::pct::set_location_on_owner            /HARDWARE/mmio_arbiter/m_respond_ports\[1\]      205
::pct::set_location_on_owner            /HARDWARE/mmio_arbiter/m_request_exports\[1\]    172
::pct::set_location_on_owner            /HARDWARE/mmio_arbiter/m_respond_ports\[0\]      55
::pct::set_location_on_owner            /HARDWARE/mmio_arbiter/m_request_exports\[0\]    22
::pct::set_location_on_owner            /HARDWARE/mmio_arbiter/m_respond_export          55
::pct::set_location_on_owner            /HARDWARE/mmio_arbiter/m_request_port            22



::pct::create_connection dram0p0_req /HARDWARE    /HARDWARE/core0/dram0p0_req                 /HARDWARE/dram0/m_request_exports\[0\]
::pct::create_connection dram0p0_rsp /HARDWARE    /HARDWARE/core0/dram0p0_rsp                 /HARDWARE/dram0/m_respond_ports\[0\]

::pct::create_connection dram0p1_req /HARDWARE    /HARDWARE/core0/dram0p1_req                 /HARDWARE/dram0/m_request_exports\[1\]
::pct::create_connection dram0p1_rsp /HARDWARE    /HARDWARE/core0/dram0p1_rsp                 /HARDWARE/dram0/m_respond_ports\[1\]


::pct::create_connection core0_pif_req   /HARDWARE    /HARDWARE/core0/pif_req                 /HARDWARE/core0_router/m_request_export
::pct::create_connection core0_pif_rsp   /HARDWARE    /HARDWARE/core0/pif_rsp                 /HARDWARE/core0_router/m_respond_port

::pct::create_connection pif_req   /HARDWARE    /HARDWARE/pif_arbiter/m_request_port        /HARDWARE/pifmem/m_request_exports\[0\]
::pct::create_connection pif_rsp   /HARDWARE    /HARDWARE/pif_arbiter/m_respond_export      /HARDWARE/pifmem/m_respond_ports\[0\]

::pct::create_connection rte_pif_req   /HARDWARE    /HARDWARE/core0_router/m_request_ports\[0\]   /HARDWARE/pif_arbiter/m_request_exports\[1\]
::pct::create_connection rte_pif_rsp   /HARDWARE    /HARDWARE/core0_router/m_respond_exports\[0\] /HARDWARE/pif_arbiter/m_respond_ports\[1\]

::pct::create_connection rte_dma_req   /HARDWARE    /HARDWARE/core0_router/m_request_ports\[1\]   /HARDWARE/dma/m_request_export
::pct::create_connection rte_dma_rsp   /HARDWARE    /HARDWARE/core0_router/m_respond_exports\[1\] /HARDWARE/dma/m_respond_port

::pct::create_connection dma_req       /HARDWARE    /HARDWARE/dma/m_request_port                  /HARDWARE/dma_router/m_request_export
::pct::create_connection dma_rsp       /HARDWARE    /HARDWARE/dma/m_respond_export                /HARDWARE/dma_router/m_respond_port

::pct::create_connection dma_pif_req   /HARDWARE    /HARDWARE/dma_router/m_request_ports\[0\]     /HARDWARE/pif_arbiter/m_request_exports\[0\]
::pct::create_connection dma_pif_rsp   /HARDWARE    /HARDWARE/dma_router/m_respond_exports\[0\]   /HARDWARE/pif_arbiter/m_respond_ports\[0\]

::pct::create_connection inbound_req   /HARDWARE    /HARDWARE/dma_router/m_request_ports\[1\]     /HARDWARE/core0/inbound_pif_req
::pct::create_connection inbound_rsp   /HARDWARE    /HARDWARE/dma_router/m_respond_exports\[1\]   /HARDWARE/core0/inbound_pif_rsp

::pct::create_connection core_mmio_req /HARDWARE    /HARDWARE/core0_router/m_request_ports\[2\]   /HARDWARE/mmio_arbiter/m_request_exports\[0\]
::pct::create_connection core_mmio_rsp /HARDWARE    /HARDWARE/core0_router/m_respond_exports\[2\] /HARDWARE/mmio_arbiter/m_respond_ports\[0\]

::pct::create_connection dma_mmio_req  /HARDWARE    /HARDWARE/dma_router/m_request_ports\[2\]     /HARDWARE/mmio_arbiter/m_request_exports\[1\]
::pct::create_connection dma_mmio_rsp  /HARDWARE    /HARDWARE/dma_router/m_respond_exports\[2\]   /HARDWARE/mmio_arbiter/m_respond_ports\[1\]

::pct::create_connection mmio_req      /HARDWARE    /HARDWARE/mmio_arbiter/m_request_port         /HARDWARE/mmio/m_request_export
::pct::create_connection mmio_rsp      /HARDWARE    /HARDWARE/mmio_arbiter/m_respond_export       /HARDWARE/mmio/m_respond_port

::pct::create_connection BInterrupt06  /HARDWARE    /HARDWARE/mmio/BInterrupt06                   /HARDWARE/core0/BInterrupt06

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



