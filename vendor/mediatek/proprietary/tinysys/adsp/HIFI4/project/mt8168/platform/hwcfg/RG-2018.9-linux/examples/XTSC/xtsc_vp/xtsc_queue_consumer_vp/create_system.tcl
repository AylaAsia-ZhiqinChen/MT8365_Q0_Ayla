# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.

;
variable module                
variable xtsc_vp_static        [info exists ::env(XTSC_VP_STATIC)]
variable libname               [expr { $xtsc_vp_static ? "xtsc_vp" : "xtsc_vp_sh" }]

::pct::new_project
::pct::open_library $env(XTENSA_SW_TOOLS)/misc/xtsc_vp/${libname}.xml



set module xtsc_queue_producer_vp
::pct::create_instance $module          /HARDWARE producer xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/producer "Template Arguments" DATA_WIDTH        48
::pct::set_param_value                  /HARDWARE/producer "Scml Properties"    /Misc/script_file ../../xtsc-run/xtsc_queue_consumer/producer.vec

::pct::set_bounds                       /HARDWARE/producer 100 100 150 150

::pct::set_orientation                  /HARDWARE/producer/m_queue             right
::pct::set_location_on_owner            /HARDWARE/producer/m_queue             22



set module xtsc_queue_vp
::pct::create_instance $module          /HARDWARE queue xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/queue "Template Arguments" DATA_WIDTH    48
::pct::set_param_value                  /HARDWARE/queue "Scml Properties"    /Misc/depth 3

::pct::set_bounds                       /HARDWARE/queue 400 100 150 150

::pct::set_orientation                  /HARDWARE/queue/m_producer    left
::pct::set_orientation                  /HARDWARE/queue/m_consumer    right

::pct::set_location_on_owner            /HARDWARE/queue/m_producer    22
::pct::set_location_on_owner            /HARDWARE/queue/m_consumer    22



set module xtsc_queue_consumer_vp
::pct::create_instance $module          /HARDWARE consumer xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/consumer "Template Arguments" DATA_WIDTH        48
::pct::set_param_value                  /HARDWARE/consumer "Scml Properties"    /Misc/script_file ../../xtsc-run/xtsc_queue_consumer/consumer.vec

::pct::set_bounds                       /HARDWARE/consumer 700 100 150 150

::pct::set_orientation                  /HARDWARE/consumer/m_queue             left
::pct::set_location_on_owner            /HARDWARE/consumer/m_queue             22



::pct::create_connection push /HARDWARE    /HARDWARE/producer/m_queue    /HARDWARE/queue/m_producer
::pct::create_connection pop  /HARDWARE    /HARDWARE/consumer/m_queue    /HARDWARE/queue/m_consumer

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

if { $xtsc_vp_static != "1" } {
  ::pct::set_simulation_build_project_setting Debug "Libraries"            [::xtsc_vp::get_Libraries            1]
  ::pct::set_simulation_build_project_setting Debug "Library Search Paths" [::xtsc_vp::get_Library_Search_Paths 1]
  ::pct::set_simulation_build_project_setting Debug "Linker Flags"         [::xtsc_vp::get_Linker_Flags         1]
}

::pct::set_preference_value /Messages/FilterIDs "ESA0022 ESA0029"
::pct::export_simulation


