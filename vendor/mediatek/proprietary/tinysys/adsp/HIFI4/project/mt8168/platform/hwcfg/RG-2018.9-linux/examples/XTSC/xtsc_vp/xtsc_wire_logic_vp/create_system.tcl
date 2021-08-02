# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.


variable module                xtsc_wire_logic_vp
variable xtsc_vp_static        [info exists ::env(XTSC_VP_STATIC)]
variable libname               [expr { $xtsc_vp_static ? "xtsc_vp" : "xtsc_vp_sh" }]

::pct::new_project
::pct::open_library $env(XTENSA_SW_TOOLS)/misc/xtsc_vp/${libname}.xml
::pct::open_library $module/$module.xml


::pct::open_library xtsc_wire_source_ext_vp/xtsc_wire_source_ext_vp.xml


set module xtsc_wire_source_ext_vp
::pct::create_instance $module          /HARDWARE source xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/source "Scml Properties"    /Misc/script_file ../../xtsc_wire_logic/source.vec

::pct::set_bounds                       /HARDWARE/source 100 100 150 100

::pct::set_orientation                  /HARDWARE/source/m_control             left
::pct::set_orientation                  /HARDWARE/source/A                     right
::pct::set_orientation                  /HARDWARE/source/B                     right
::pct::set_location_on_owner            /HARDWARE/source/m_control             22
::pct::set_location_on_owner            /HARDWARE/source/A                     22
::pct::set_location_on_owner            /HARDWARE/source/B                     55



set module xtsc_wire_logic_vp
::pct::create_instance $module          /HARDWARE logic xtsc_vp::$module xtsc_vp::$module\()

::pct::set_bounds                       /HARDWARE/logic 400 100 100 1350

::pct::set_orientation                  /HARDWARE/logic/A                      left
::pct::set_orientation                  /HARDWARE/logic/B                      left
::pct::set_orientation                  /HARDWARE/logic/not_A                  right
::pct::set_orientation                  /HARDWARE/logic/A_and_B                right
::pct::set_orientation                  /HARDWARE/logic/A_or_B                 right
::pct::set_orientation                  /HARDWARE/logic/A_xor_B                right
::pct::set_orientation                  /HARDWARE/logic/A0                     right
::pct::set_orientation                  /HARDWARE/logic/A1                     right
::pct::set_orientation                  /HARDWARE/logic/A_dup1                 right
::pct::set_orientation                  /HARDWARE/logic/A_dup2                 right
::pct::set_orientation                  /HARDWARE/logic/A_B                    right
::pct::set_location_on_owner            /HARDWARE/logic/A                      22
::pct::set_location_on_owner            /HARDWARE/logic/B                      55
::pct::set_location_on_owner            /HARDWARE/logic/not_A                  22
::pct::set_location_on_owner            /HARDWARE/logic/A_and_B                172
::pct::set_location_on_owner            /HARDWARE/logic/A_or_B                 322
::pct::set_location_on_owner            /HARDWARE/logic/A_xor_B                472
::pct::set_location_on_owner            /HARDWARE/logic/A0                     622
::pct::set_location_on_owner            /HARDWARE/logic/A1                     772
::pct::set_location_on_owner            /HARDWARE/logic/A_dup1                 922
::pct::set_location_on_owner            /HARDWARE/logic/A_dup2                 1072
::pct::set_location_on_owner            /HARDWARE/logic/A_B                    1222



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE not_A xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/not_A "Template Arguments" DATA_WIDTH 32
::pct::set_param_value                  /HARDWARE/not_A "Scml Properties"    /Misc/write_file not_A.dat

::pct::set_bounds                       /HARDWARE/not_A 700 100 150 100

::pct::set_orientation                  /HARDWARE/not_A/m_write       left
::pct::set_orientation                  /HARDWARE/not_A/m_read        right

::pct::set_location_on_owner            /HARDWARE/not_A/m_write       22
::pct::set_location_on_owner            /HARDWARE/not_A/m_read        22



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE A_and_B xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/A_and_B "Template Arguments" DATA_WIDTH 32
::pct::set_param_value                  /HARDWARE/A_and_B "Scml Properties"    /Misc/write_file A_and_B.dat

::pct::set_bounds                       /HARDWARE/A_and_B 700 250 150 100

::pct::set_orientation                  /HARDWARE/A_and_B/m_write       left
::pct::set_orientation                  /HARDWARE/A_and_B/m_read        right

::pct::set_location_on_owner            /HARDWARE/A_and_B/m_write       22
::pct::set_location_on_owner            /HARDWARE/A_and_B/m_read        22



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE A_or_B xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/A_or_B "Template Arguments" DATA_WIDTH 32
::pct::set_param_value                  /HARDWARE/A_or_B "Scml Properties"    /Misc/write_file A_or_B.dat

::pct::set_bounds                       /HARDWARE/A_or_B 700 400 150 100

::pct::set_orientation                  /HARDWARE/A_or_B/m_write       left
::pct::set_orientation                  /HARDWARE/A_or_B/m_read        right

::pct::set_location_on_owner            /HARDWARE/A_or_B/m_write       22
::pct::set_location_on_owner            /HARDWARE/A_or_B/m_read        22



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE A_xor_B xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/A_xor_B "Template Arguments" DATA_WIDTH 32
::pct::set_param_value                  /HARDWARE/A_xor_B "Scml Properties"    /Misc/write_file A_xor_B.dat

::pct::set_bounds                       /HARDWARE/A_xor_B 700 550 150 100

::pct::set_orientation                  /HARDWARE/A_xor_B/m_write       left
::pct::set_orientation                  /HARDWARE/A_xor_B/m_read        right

::pct::set_location_on_owner            /HARDWARE/A_xor_B/m_write       22
::pct::set_location_on_owner            /HARDWARE/A_xor_B/m_read        22



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE A0 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/A0 "Template Arguments" DATA_WIDTH 1
::pct::set_param_value                  /HARDWARE/A0 "Scml Properties"    /Misc/write_file A0.dat

::pct::set_bounds                       /HARDWARE/A0 700 700 150 100

::pct::set_orientation                  /HARDWARE/A0/m_write       left
::pct::set_orientation                  /HARDWARE/A0/m_read        right

::pct::set_location_on_owner            /HARDWARE/A0/m_write       22
::pct::set_location_on_owner            /HARDWARE/A0/m_read        22



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE A1 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/A1 "Template Arguments" DATA_WIDTH 1
::pct::set_param_value                  /HARDWARE/A1 "Scml Properties"    /Misc/write_file A1.dat

::pct::set_bounds                       /HARDWARE/A1 700 850 150 100

::pct::set_orientation                  /HARDWARE/A1/m_write       left
::pct::set_orientation                  /HARDWARE/A1/m_read        right

::pct::set_location_on_owner            /HARDWARE/A1/m_write       22
::pct::set_location_on_owner            /HARDWARE/A1/m_read        22



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE A_dup1 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/A_dup1 "Template Arguments" DATA_WIDTH 32
::pct::set_param_value                  /HARDWARE/A_dup1 "Scml Properties"    /Misc/write_file A_dup1.dat

::pct::set_bounds                       /HARDWARE/A_dup1 700 1000 150 100

::pct::set_orientation                  /HARDWARE/A_dup1/m_write       left
::pct::set_orientation                  /HARDWARE/A_dup1/m_read        right

::pct::set_location_on_owner            /HARDWARE/A_dup1/m_write       22
::pct::set_location_on_owner            /HARDWARE/A_dup1/m_read        22



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE A_dup2 xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/A_dup2 "Template Arguments" DATA_WIDTH 32
::pct::set_param_value                  /HARDWARE/A_dup2 "Scml Properties"    /Misc/write_file A_dup2.dat

::pct::set_bounds                       /HARDWARE/A_dup2 700 1150 150 100

::pct::set_orientation                  /HARDWARE/A_dup2/m_write       left
::pct::set_orientation                  /HARDWARE/A_dup2/m_read        right

::pct::set_location_on_owner            /HARDWARE/A_dup2/m_write       22
::pct::set_location_on_owner            /HARDWARE/A_dup2/m_read        22



set module xtsc_wire_vp
::pct::create_instance $module          /HARDWARE A_B xtsc_vp::$module xtsc_vp::$module\()
::pct::set_param_value                  /HARDWARE/A_B "Template Arguments" DATA_WIDTH 64
::pct::set_param_value                  /HARDWARE/A_B "Scml Properties"    /Misc/write_file A_B.dat

::pct::set_bounds                       /HARDWARE/A_B 700 1300 150 100

::pct::set_orientation                  /HARDWARE/A_B/m_write       left
::pct::set_orientation                  /HARDWARE/A_B/m_read        right

::pct::set_location_on_owner            /HARDWARE/A_B/m_write       22
::pct::set_location_on_owner            /HARDWARE/A_B/m_read        22




::pct::create_connection A0_            /HARDWARE       /HARDWARE/logic/A0            /HARDWARE/A0/m_write
::pct::create_connection A1_            /HARDWARE       /HARDWARE/logic/A1            /HARDWARE/A1/m_write
::pct::create_connection A_B_           /HARDWARE       /HARDWARE/logic/A_B           /HARDWARE/A_B/m_write
::pct::create_connection A_and_B_       /HARDWARE       /HARDWARE/logic/A_and_B       /HARDWARE/A_and_B/m_write
::pct::create_connection A_dup1_        /HARDWARE       /HARDWARE/logic/A_dup1        /HARDWARE/A_dup1/m_write
::pct::create_connection A_dup2_        /HARDWARE       /HARDWARE/logic/A_dup2        /HARDWARE/A_dup2/m_write
::pct::create_connection A_or_B_        /HARDWARE       /HARDWARE/logic/A_or_B        /HARDWARE/A_or_B/m_write
::pct::create_connection A_xor_B_       /HARDWARE       /HARDWARE/logic/A_xor_B       /HARDWARE/A_xor_B/m_write
::pct::create_connection not_A_         /HARDWARE       /HARDWARE/logic/not_A         /HARDWARE/not_A/m_write
::pct::create_connection A_             /HARDWARE       /HARDWARE/source/A            /HARDWARE/logic/A
::pct::create_connection B_             /HARDWARE       /HARDWARE/source/B            /HARDWARE/logic/B

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



