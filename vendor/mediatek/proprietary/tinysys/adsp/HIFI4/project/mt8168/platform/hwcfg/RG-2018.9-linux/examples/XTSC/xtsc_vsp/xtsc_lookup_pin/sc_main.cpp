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
#include <xtsc/xtsc_lookup_pin.h>

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
    init_parms.set("turbo", false);   // TurboXim doesn't work with pin-level lookups
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

    // Create a file for signal-level tracing
    sc_trace_file *p_tf = sc_create_vcd_trace_file("waveforms");

    // Create a clock just to enhance the tracing
    sc_clock clk("clk", xtsc_get_system_clock_period());
    sc_trace(p_tf, clk, "clk");

    // Instantiate the lookup table
    u32 address_width   = core0.get_xtsc_core().get_pin_bit_width("TIE_lut_Out");
    u32 data_width      = core0.get_xtsc_core().get_pin_bit_width("TIE_lut_In");
    xtsc_lookup_pin_parms tbl_parms(address_width, data_width, true, "../../xtsc_lookup_pin/lut.rom", "0xDEADBEEF", p_tf);
    tbl_parms.extract_parms(argc, argv, "tbl");
    xtsc_lookup_pin tbl("tbl", tbl_parms);

    // Connect core0 to the lookup
    xtsc_connect(core0, "lut", "", tbl);

    // Load program
    core0.get_xtsc_core().load_program("target/lookup_test.out");

    // Set-up debugging according to command line arguments, if any
    core0.get_xtsc_core().setup_debug(argc, argv);

    XTSC_INFO(logger, "Starting SystemC simulation.");

    // Start and run the simulation
    sc_start();

    XTSC_INFO(logger, "SystemC simulation ended.");

    // Close the trace file
    sc_close_vcd_trace_file(p_tf);

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

