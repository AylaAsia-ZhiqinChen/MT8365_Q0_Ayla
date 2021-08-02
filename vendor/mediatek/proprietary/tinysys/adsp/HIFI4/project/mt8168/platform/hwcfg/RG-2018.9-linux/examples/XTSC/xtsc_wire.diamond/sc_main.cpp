// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <ctime>
#include <xtsc/xtsc_core.h>
#include <xtsc/xtsc_wire.h>
#include <xtsc/xtsc_memory.h>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
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

    // Configuration parameters for first core
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY);
    core_parms.set("SimTargetOutput", "core0_output.log");
    core_parms.extract_parms(argc, argv, "core0");

    // Construct core0
    xtsc_core core0("core0", core_parms);

    //core0.load_client("trace --level 6 core0_trace.log");

    // Change 1 parameter core1
    core_parms.set("SimTargetOutput", "core1_output.log");
    core_parms.extract_parms(argc, argv, "core1");

    // Construct core1
    xtsc_core core1("core1", core_parms);

    //core1.load_client("trace --level 6 core1_trace.log");

    // Configure, construct, and connect pif memory for core0
    xtsc_memory_parms core0_pif_parms(core0, "pif");
    core0_pif_parms.extract_parms(argc, argv, "core0_pif");
    xtsc_memory core0_pif("core0_pif", core0_pif_parms);
    xtsc_connect(core0, "pif", "", core0_pif);

    // Configure, construct, and connect pif memory for core1
    xtsc_memory_parms core1_pif_parms(core1, "pif");
    core1_pif_parms.extract_parms(argc, argv, "core1_pif");
    xtsc_memory core1_pif("core1_pif", core1_pif_parms);
    xtsc_connect(core1, "pif", "", core1_pif);


    /*
     *   core0 (source.out)                           core1 (sink.out)
     *   ====================                         ====================
     *   |                  |                         |                  |
     *   |                  |                         |                  |
     *   |                  |===  core0_to_core1   ===|                  |
     *   |         EXPSTATE |  |==================>|  | IMPWIRE          |
     *   |                  |===                   ===|                  |
     *   |                  |                         |                  |
     *   |                  |                         |                  |
     *   ====================                         ====================
     */

    // Configuration parameters for an xtsc_wire
    xtsc_wire_parms wire_parms(core0, "EXPSTATE");
    wire_parms.extract_parms(argc, argv, "core0_to_core1");

    // Construct the wire
    xtsc_wire core0_to_core1("core0_to_core1", wire_parms);

    // Connect the wire
    xtsc_connect(core0, "EXPSTATE", "wire_write", core0_to_core1);
    xtsc_connect(core1, "IMPWIRE",  "wire_read",  core0_to_core1);

    // Load programs
    core0.load_program("target/source.out");
    core1.load_program("target/sink.out");

    // Set-up debugging according to command line arguments, if any
    xtsc_core::setup_multicore_debug(argc, argv);

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

