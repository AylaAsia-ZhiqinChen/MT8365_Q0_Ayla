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

    // Set up for waveform tracing
    sc_trace_file *p_tf = sc_create_vcd_trace_file("waveforms");

    // Configuration parameters for first core
    const char *core0_pli[] = { "status", NULL };
    xtsc_core_parms core0_parms(CONFIG_NAME, XTENSA_REGISTRY, TDK_DIR);
    core0_parms.set("SimTargetOutput", "core0_output.log");
    core0_parms.set("SimPinLevelInterfaces", core0_pli);
    //core0_parms.set("SimVcdHandle", p_tf);
    core0_parms.extract_parms(argc, argv, "core0");

    // Construct core0
    xtsc_core core0("core0", core0_parms);

    // Turn on PC trace to a log file (this is not a VCD trace)
    //core0.load_client("trace --level 6 core0_trace.log");

    // Configuration parameters for second core
    const char *core1_pli[] = { "control", NULL };
    xtsc_core_parms core1_parms(CONFIG_NAME, XTENSA_REGISTRY, TDK_DIR);
    core1_parms.set("SimTargetOutput", "core1_output.log");
    core1_parms.set("SimPinLevelInterfaces", core1_pli);
    //core1_parms.set("SimVcdHandle", p_tf);
    core1_parms.extract_parms(argc, argv, "core1");

    // Construct core1
    xtsc_core core1("core1", core1_parms);

    // To turn on PC trace 
    //core1.load_client("trace --level 6 core1_trace.log");

    /*
     *   core0 (source.out)                           core1 (sink.out)
     *   ====================                         ====================
     *   |                  |                         |                  |
     *   |                  |                         |                  |
     *   |                  |     core0_to_core1      |                  |
     *   |       TIE_status |========================>| TIE_control      |
     *   |                  |                         |                  |
     *   |                  |                         |                  |
     *   |                  |                         |                  |
     *   ====================                         ====================
     */

    // Connect core0 to core1 using signal which is traced
    u32 bits = core0.get_pin_bit_width("TIE_status");
    xtsc_signal_sc_bv_base core0_to_core1("core0_to_core1", bits);
    sc_trace(p_tf, core0_to_core1, "core0_to_core1");
    core0.get_output_pin("TIE_status" )(core0_to_core1);
    core1.get_input_pin ("TIE_control")(core0_to_core1);

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

    // Load programs
    core0.load_program("target/source.out");
    core1.load_program("target/sink.out");

    // Set-up debugging according to command line arguments, if any
    xtsc_core::setup_multicore_debug(argc, argv);

    XTSC_INFO(logger, "Starting SystemC simulation.");

    // Start and run the simulation
    sc_start();

    XTSC_INFO(logger, "SystemC simulation ended.");

    sc_close_vcd_trace_file(p_tf);

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

