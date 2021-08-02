// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <ctime>
#include "xtsc_core_vsp/xtsc_core_vsp.h"
#include <xtsc/xtsc_memory.h>
#include <xtsc/xtsc_arbiter.h>


using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;
using namespace xtsc_component;
using namespace xtsc_vsp;


static TextLogger& logger = TextLogger::getInstance("sc_main");



int sc_main(int argc, char *argv[]) {

  try {

    // Initialize
    xtsc_vsp_initialize_parms init_parms("../../TextLogger.txt");
    // Next line internally calls extract_parms()
    xtsc_vsp_initialize(init_parms);

    // Configuration parameters for first core
    xtsc_core_vsp_parms core0_parms;
    core0_parms.set("SimTargetOutput", "core0_output.log");

    // Construct the first core
    // Next line internally calls extract_parms
    xtsc_core_vsp core0("core0", core0_parms);

    // Configuration parameters for second core
    xtsc_core_vsp_parms core1_parms;
    core1_parms.set("SimTargetOutput", "core1_output.log");

    // Construct the second core
    // Next line internally calls extract_parms
    xtsc_core_vsp core1("core1", core1_parms);

    // Configure and construct a shared PIF memory 
    xtsc_memory_parms memory_parms(core0.get_xtsc_core(), "pif");
    memory_parms.extract_parms(argc, argv, "shr_mem");
    xtsc_memory shr_mem("shr_mem", memory_parms);

    // Configure and construct a PIF arbiter which gives 2 cores access
    // to the samed shared memory and that does independent address translations
    // for each core.
    xtsc_arbiter_parms arbiter_parms(2);
    arbiter_parms.set("translation_file", "../../xtsc_arbiter/address_translations.txt");
    arbiter_parms.extract_parms(argc, argv, "arbiter");
    xtsc_arbiter arbiter("arbiter", arbiter_parms);

    // Connect everything together
    xtsc_connect(arbiter, "master_port", "", shr_mem);
    xtsc_connect(core0, "pif", "slave_port[0]", arbiter);
    xtsc_connect(core1, "pif", "slave_port[1]", arbiter);

    // Load programs
    core0.get_xtsc_core().load_program("target/core0.out");
    core1.get_xtsc_core().load_program("target/core1.out");

    // Set-up debugging according to command line arguments, if any
    xtsc_core::setup_multicore_debug(argc, argv);

    XTSC_INFO(logger, "Starting SystemC simulation.");

    // Start and run the simulation
    sc_start();

    XTSC_INFO(logger, "SystemC simulation ended.");

    // Clean-up
    xtsc_vsp_finalize();

  }
  catch (const exception& error) {
    ostringstream oss;
    oss << "std::exception caught: " << endl;
    oss << error.what() << endl;
    xtsc_log_multiline(logger, FATAL_LOG_LEVEL, oss.str(), 2);
    cerr << oss.str();
  }

  return 0;
}

