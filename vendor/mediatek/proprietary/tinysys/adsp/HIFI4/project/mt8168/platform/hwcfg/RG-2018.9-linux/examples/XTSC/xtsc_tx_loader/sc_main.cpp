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
#include <xtsc/xtsc_tx_loader.h>
#include <xtsc/xtsc_memory.h>
#include <xtsc/xtsc_queue.h>
#include <xtsc/xtsc_queue_producer.h>
#include <xtsc/xtsc_wire_logic.h>

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

    // Configure xtsc_queue_producer loader_driver
    xtsc_queue_producer_parms loader_driver_parms;
    loader_driver_parms.set("script_file", "loader_driver.vec");
    loader_driver_parms.set("control", true);
    loader_driver_parms.extract_parms(argc, argv, "loader_driver");

    // Create xtsc_queue_producer loader_driver
    xtsc_queue_producer loader_driver("loader_driver", loader_driver_parms);

    // Configure xtsc_tx_loader loader
    xtsc_tx_loader_parms loader_parms;
    loader_parms.extract_parms(argc, argv, "loader");

    // Create xtsc_tx_loader loader
    xtsc_tx_loader loader("loader", loader_parms);

    // Configuration parameters for first core
    xtsc_core_parms core0_parms(CONFIG_NAME, XTENSA_REGISTRY, TDK_DIR);
    core0_parms.set("ProcessorID", 0x0);
    core0_parms.extract_parms(argc, argv, "core0");

    // Construct core0
    xtsc_core core0("core0", core0_parms);

    // Configuration parameters for second core
    xtsc_core_parms core1_parms(CONFIG_NAME, XTENSA_REGISTRY, TDK_DIR);
    core1_parms.set("ProcessorID", 0x1);
    core1_parms.extract_parms(argc, argv, "core1");

    // Construct core1
    xtsc_core core1("core1", core1_parms);

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

    // Configure xtsc_queue Q01
    xtsc_queue_parms Q01_parms;
    Q01_parms.set("depth", 0x2);
    Q01_parms.set("bit_width", 96);
    Q01_parms.extract_parms(argc, argv, "Q01");

    // Create xtsc_queue Q01
    xtsc_queue Q01("Q01", Q01_parms);

    // Configure xtsc_queue Q10
    xtsc_queue_parms Q10_parms;
    Q10_parms.set("depth", 0x2);
    Q10_parms.set("bit_width", 96);
    Q10_parms.extract_parms(argc, argv, "Q10");

    // Create xtsc_queue Q10
    xtsc_queue Q10("Q10", Q10_parms);

    // Configure xtsc_wire_logic logic
    xtsc_wire_logic_parms logic_parms("");
    logic_parms.set("definition_file", "logic.def");
    logic_parms.extract_parms(argc, argv, "logic");

    // Create xtsc_wire_logic logic
    xtsc_wire_logic logic("logic", logic_parms);

    // Connect core0 to Q01
    xtsc_connect(core0, "OUTQ1", "queue_push", Q01);

    // Connect Q01 to core1
    xtsc_connect(core1, "INQ1", "queue_pop", Q01);

    // Connect core1 to Q10
    xtsc_connect(core1, "OUTQ1", "queue_push", Q10);

    // Connect Q10 to core0
    xtsc_connect(core0, "INQ1", "queue_pop", Q10);

    // Connect xtsc_queue_producer loader_driver to queue input of loader
    xtsc_connect(loader_driver, "queue_push", "queue_push", loader);

    // Connect XFER output of loader to XFER input of core0
    xtsc_connect(loader, "tx_xfer_out", "tx_xfer_in", core0);

    // Connect core0 to core1
    xtsc_connect(core0, "tx_xfer_out", "tx_xfer_in", core1);

    // Connect output XFER interface of core1 to XFER input of loader
    xtsc_connect(core1, "tx_xfer_out", "tx_xfer_in", loader);

    // Connect CoreHalted output of core0 to CoreHalted0 input of xtsc_wire_logic logic
    xtsc_connect(core0, "CoreHalted", "CoreHalted0", logic);

    // Connect CoreHalted output of core1 to CoreHalted1 input of xtsc_wire_logic logic
    xtsc_connect(core1, "CoreHalted", "CoreHalted1", logic);

    // Connect JobDone output of xtsc_wire_logic logic to control input of xtsc_queue_producer loader_driver
    xtsc_connect(logic, "JobDone", "control", loader_driver);

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

