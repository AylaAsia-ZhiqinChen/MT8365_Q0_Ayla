// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <ostream>
#include <string>
#include <xtsc/xtsc_core.h>
#include <xtsc/xtsc_memory.h>
#include <xtsc/xtsc_lookup.h>

using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;
using namespace xtsc_component;

static TextLogger& logger = TextLogger::getInstance("sc_main");


int sc_main(int argc, char *argv[]) {

  try {

    // Initialize
    xtsc_initialize_parms init_parms("../TextLogger.txt");
    init_parms.extract_parms(argc, argv, "xtsc");
    xtsc_initialize(init_parms);

    // Configuration parameters for xtsc_core
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, TDK_DIR);
    core_parms.set("SimTargetOutput", "core0_output.log");
    core_parms.extract_parms(argc, argv, "core0");

    // Construct the core
    xtsc_core core0("core0", core_parms);

    // Configure, construct, and connect pif memory for core0
    xtsc_memory_parms core0_pif_parms(core0, "pif");
    core0_pif_parms.extract_parms(argc, argv, "core0_pif");
    xtsc_memory core0_pif("core0_pif", core0_pif_parms);
    xtsc_connect(core0, "pif", "", core0_pif);

    // Configure the lookup-based ram
    xtsc_lookup_parms lram_parms(core0, "lookup_ram", NULL, "0xBAD1BAD1", true);
    lram_parms.extract_parms(argc, argv, "lram");

    // Construct the lookup-based ram
    xtsc_lookup lram("lram", lram_parms);

    // Connect the lookup-based ram
    xtsc_connect(core0, "lookup_ram", "", lram);

    // Load program
    core0.load_program("target/ram_test.out");

    // Set-up debugging according to command line arguments, if any
    core0.setup_debug(argc, argv);

    XTSC_INFO(logger, "Starting SystemC simulation.");

    // Start and run the simulation
    sc_start();

    XTSC_INFO(logger, "SystemC simulation ended.");

    // Clean-up
    xtsc_finalize();

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

