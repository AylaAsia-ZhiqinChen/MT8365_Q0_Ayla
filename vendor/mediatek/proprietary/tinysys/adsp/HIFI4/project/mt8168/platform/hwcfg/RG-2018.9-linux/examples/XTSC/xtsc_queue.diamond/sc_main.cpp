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
#include <xtsc/xtsc_memory.h>
#include <xtsc/xtsc_queue.h>

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

    // Can do this to speed up simulation (or pass -xtsc.text_logging_disable=true on the command line)
    //xtsc_enable_text_logging(false);

    // Configuration parameters for first core
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, TDK_DIR);
    core_parms.set("SimTargetOutput", "core0_output.log");
    core_parms.extract_parms(argc, argv, "core0");

    // Construct core0
    xtsc_core core0("core0", core_parms);

    // Change 1 parameter for core1
    core_parms.set("SimTargetOutput", "core1_output.log");
    core_parms.extract_parms(argc, argv, "core1");

    // Construct core1
    xtsc_core core1("core1", core_parms);

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

    // Configuration parameters for a TIE queue of depth 2
    xtsc_queue_parms Q1_parms(core0, "OPQ", 2);
    Q1_parms.extract_parms(argc, argv, "Q1");

    // Construct the queue
    xtsc_queue Q1("Q1", Q1_parms);

    // Connect the queue
    xtsc_connect(core0, "OPQ", "queue_push", Q1);
    xtsc_connect(core1, "IPQ", "queue_pop",  Q1);

    // Load programs
    core0.load_program("target/producer.out");
    core1.load_program("target/consumer.out");

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

