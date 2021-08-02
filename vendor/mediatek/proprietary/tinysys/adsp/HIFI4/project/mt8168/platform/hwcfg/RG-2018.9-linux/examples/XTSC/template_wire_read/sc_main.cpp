// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <iostream>
#include <xtsc/xtsc_core.h>
#include <xtsc/xtsc_memory.h>
#include "template_wire_read.h"

using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;
using namespace xtsc_component;

int sc_main(int argc, char *argv[]) {

  try {

    // Initialize
    xtsc_initialize_parms init_parms("../TextLogger.txt");
    init_parms.extract_parms(argc, argv, "xtsc");
    xtsc_initialize(init_parms);

    // Configure and construct core0
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, TDK_DIR);
    core_parms.set("SimTargetOutput", "core0_output.log");
    core_parms.extract_parms(argc, argv, "core0");
    xtsc_core core0("core0", core_parms);

    // Configure and construct null wire
    u32 bit_width = core0.get_tie_bit_width("control");
    template_wire_read_parms null_wire_parms(bit_width);
    null_wire_parms.extract_parms(argc, argv, "null_wire");
    template_wire_read null_wire("null_wire", null_wire_parms);

    // Connect the wire
    core0.get_import_wire("control")(null_wire);

    // Configure, construct, and connect pif memory for core0
    xtsc_memory_parms core0_pif_parms(core0, "pif");
    core0_pif_parms.extract_parms(argc, argv, "core0_pif");
    xtsc_memory core0_pif("core0_pif", core0_pif_parms);
    xtsc_connect(core0, "pif", "", core0_pif);

    // Load programs
    core0.load_program("target/sink.out");

    // Set-up debugging according to command line arguments, if any
    xtsc_core::setup_multicore_debug(argc, argv);

    // Start and run the simulation
    sc_start();

    // Clean-up
    xtsc_finalize();

  }
  catch (const exception& error) {
    ostringstream oss;
    oss << "std::exception caught:" << endl;
    oss << "what(): " << error.what() << endl;
    xtsc_log_multiline(TextLogger::getInstance("sc_main"), FATAL_LOG_LEVEL, oss.str(), 2);
    cerr << oss.str();
  }

  return 0;
}
