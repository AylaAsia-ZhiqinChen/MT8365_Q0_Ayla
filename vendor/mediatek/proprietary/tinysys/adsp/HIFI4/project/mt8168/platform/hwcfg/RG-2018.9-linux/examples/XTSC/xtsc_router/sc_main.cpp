// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
#include <xtsc/xtsc_router.h>

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

    // Configure and construct core0
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
    core_parms.set("SimTargetOutput", "core0_output.log");
    core_parms.extract_parms(argc, argv, "core0");
    xtsc_core core0("core0", core_parms);

    // Configure and construct a router with two slaves.
    // Make port 1 (sysram) the default port.
    xtsc_router_parms router_parms(2, true, "routing.tab", 1);
    router_parms.extract_parms(argc, argv, "router");
    xtsc_router router("router", router_parms);

    // Configure and construct two pif memories:
    // one is system ROM and the other is system RAM.
    xtsc_memory_parms pif_parms(core0, "pif");
    pif_parms.extract_parms(argc, argv, "sysrom");
    xtsc_memory sysrom("sysrom", pif_parms);
    pif_parms.extract_parms(argc, argv, "sysram");
    xtsc_memory sysram("sysram", pif_parms);

    // Connect everything together
    xtsc_connect(core0,  "pif",            "slave_port", router);
    xtsc_connect(router, "master_port[0]", "slave_port", sysrom);
    xtsc_connect(router, "master_port[1]", "slave_port", sysram);

    // Load program
    core0.load_program("target/router_test.out");

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

