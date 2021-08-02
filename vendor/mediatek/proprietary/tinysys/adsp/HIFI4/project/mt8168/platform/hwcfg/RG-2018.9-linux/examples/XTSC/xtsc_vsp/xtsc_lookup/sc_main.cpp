// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <ostream>
#include <string>
#include "xtsc_core_vsp/xtsc_core_vsp.h"
#include <xtsc/xtsc_memory.h>
#include <xtsc/xtsc_lookup.h>

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

    // Configuration parameters for xtsc_core
    xtsc_core_vsp_parms core_parms;
    core_parms.set("SimTargetOutput", "core0_output.log");

    // Construct the core
    // Next line internally calls extract_parms
    xtsc_core_vsp core0("core0", core_parms);

    // Configure, construct, and connect pif memory for core0
    xtsc_memory_parms core0_pif_parms(core0.get_xtsc_core(), "pif");
    core0_pif_parms.extract_parms(argc, argv, "core0_pif");
    xtsc_memory core0_pif("core0_pif", core0_pif_parms);
    xtsc_connect(core0, "pif", "", core0_pif);

    // Configure the lookup table
    xtsc_lookup_parms tbl_parms(core0.get_xtsc_core(), "lut", "../../xtsc_lookup/lut.rom", "0xDEADBEEF");
    tbl_parms.set("enforce_latency", false);  // TurboXim (turbo = true) ignores latency
    tbl_parms.extract_parms(argc, argv, "tbl");

    // Construct the lookup table
    xtsc_lookup tbl("tbl", tbl_parms);

    // Connect the lookup table
    xtsc_connect(core0, "lut", "", tbl);

    // Load program
    core0.get_xtsc_core().load_program("target/lookup_test.out");

    // Set-up debugging according to command line arguments, if any
    core0.get_xtsc_core().setup_debug(argc, argv);

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

