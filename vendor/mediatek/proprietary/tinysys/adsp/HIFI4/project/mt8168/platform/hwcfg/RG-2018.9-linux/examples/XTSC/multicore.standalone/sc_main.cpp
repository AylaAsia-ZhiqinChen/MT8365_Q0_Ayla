// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// One include file for each component
#include <xtsc/xtsc_core.h>
#include <xtsc/xtsc_memory.h>

// Add "using namespace" statements to simplify coding
using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;
using namespace xtsc_component;

// Get a logger for use as needed
static TextLogger& logger = TextLogger::getInstance("sc_main");

int sc_main(int argc, char *argv[]) {

  try {

    // Initialize XTSC
    xtsc_initialize_parms init_parms("TextLogger.txt");
    init_parms.extract_parms(argc, argv, "xtsc");
    xtsc_initialize(init_parms);

    // Configuration parameters for the first Xtensa core
    xtsc_core_parms core0_parms(CONFIG0_NAME, XTENSA0_REGISTRY, TDK0_DIR);
    core0_parms.extract_parms(argc, argv, "core0");

    // Construct the first core
    xtsc_core core0("core0", core0_parms);

    // Configuration parameters for the second Xtensa core
    xtsc_core_parms core1_parms(CONFIG1_NAME, XTENSA1_REGISTRY, TDK1_DIR);
    core1_parms.extract_parms(argc, argv, "core1");

    // Construct the second core
    xtsc_core core1("core1", core1_parms);

    // Configuration parameters for a PIF memory for core0
    xtsc_memory_parms core0_pif_parms(core0, "pif");
    core0_pif_parms.extract_parms(argc, argv, "core0_pif");

    // Construct the PIF memory for core0
    xtsc_memory core0_pif("core0_pif", core0_pif_parms);

    // Connect the PIF memory to core0
    xtsc_connect(core0, "pif", "", core0_pif);

    // Configuration parameters for a PIF memory for core1
    xtsc_memory_parms core1_pif_parms(core1, "pif");
    core1_pif_parms.extract_parms(argc, argv, "core1_pif");

    // Construct the PIF memory for core1
    xtsc_memory core1_pif("core1_pif", core1_pif_parms);

    // Connect the PIF memory to core1
    xtsc_connect(core1, "pif", "", core1_pif);

    // Load the target programs
    core0.load_program(TARGET0_PROGRAM);
    core1.load_program(TARGET1_PROGRAM);

    // Set-up debugging according to command line arguments, if any
    xtsc_core::setup_multicore_debug(argc, argv);

    XTSC_INFO(logger, "Starting SystemC simulation.");

    // Start and run the simulation
    sc_start();

    XTSC_INFO(logger, "SystemC simulation ended.");

    // Clean-up
    xtsc_finalize();

  }
  // Catch and log exceptions
  catch (const exception& error) {
    ostringstream oss;
    oss << "std::exception caught: " << endl;
    oss << error.what() << endl;
    xtsc_log_multiline(logger, FATAL_LOG_LEVEL, oss.str(), 2);
  }

  return 0;
}
