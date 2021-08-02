// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// Step 1: One include file for each component
#include <xtsc/xtsc_core.h>
#include <xtsc/xtsc_memory.h>

// Step 2: Add "using namespace" statements to simplify coding
using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;
using namespace xtsc_component;

// Step 3: Get a logger for use as needed
static TextLogger& logger = TextLogger::getInstance("sc_main");

int sc_main(int argc, char *argv[]) {

  try {

    // Step 4: Configuration parameters for XTSC initialization
    xtsc_initialize_parms init_parms("../TextLogger.txt");
    init_parms.extract_parms(argc, argv, "xtsc");

    // Step 5: Initialize XTSC
    xtsc_initialize(init_parms);

    // Step 6: Configuration parameters for an Xtensa core
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
    core_parms.extract_parms(argc, argv, "core0");

    // Step 7: Construct the core
    xtsc_core core0("core0", core_parms);

    // Step 8: Configuration parameters for a PIF memory
    xtsc_memory_parms memory_parms(core0, "pif");
    memory_parms.extract_parms(argc, argv, "core0_pif");

    // Step 9: Construct the PIF memory
    xtsc_memory core0_pif("core0_pif", memory_parms);

    // Step 10: Connect the PIF memory
    xtsc_connect(core0, "pif", "", core0_pif);

    // Step 11: Load the target program
    core0.load_program("target/hello.out");

    // Step 12: Set-up debugging according to command line arguments, if any
    xtsc_core::setup_multicore_debug(argc, argv);

    XTSC_INFO(logger, "Starting SystemC simulation.");

    // Step 13: Start and run the simulation
    sc_start();

    XTSC_INFO(logger, "SystemC simulation ended.");

    // Step 14: Clean-up
    xtsc_finalize();

  }
  // Step 15: Catch and log exceptions
  catch (const exception& error) {
    ostringstream oss;
    oss << "std::exception caught: " << endl;
    oss << error.what() << endl;
    xtsc_log_multiline(logger, FATAL_LOG_LEVEL, oss.str(), 2);
  }

  return 0;
}
