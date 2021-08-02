// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// Header files for each module
#include <xtsc/xtsc_core.h>
#include <xtsc/xtsc_master.h>
#include <xtsc/xtsc_memory.h>
#include <xtsc/xtsc_pin2tlm_memory_transactor.h>
#include <xtsc/xtsc_tlm2pin_memory_transactor.h>


// Declare some namespaces for convenience
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

    // Create VCD file "waveforms.vcd"
    sc_trace_file *waveforms = sc_create_vcd_trace_file("waveforms");

    // Create clock clk
    sc_clock clk("clk", 1000*sc_get_time_resolution(), 0.5, 0*sc_get_time_resolution(), true);
    sc_trace(waveforms, clk, "clk");

    // Configure and construct core0
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
    core_parms.set("SimTargetOutput", "core0_output.log");
    core_parms.extract_parms(argc, argv, "core0");
    xtsc_core core0("core0", core_parms);

    // Configure, construct, and connect a PIF memory
    xtsc_memory_parms core0_pif_parms(core0, "pif");
    core0_pif_parms.extract_parms(argc, argv, "core0_pif");
    xtsc_memory core0_pif("core0_pif", core0_pif_parms);
    xtsc_connect(core0, "pif", "", core0_pif);

    // Configure, construct, and connect a DRAM0 memory
    xtsc_memory_parms core0_dram0_parms(core0, "dram0");
    core0_dram0_parms.extract_parms(argc, argv, "core0_dram0");
    xtsc_memory core0_dram0("core0_dram0", core0_dram0_parms);
    xtsc_connect(core0, "dram0", "", core0_dram0);

    // Configure and construct master
    xtsc_master_parms master_parms("master.vec");
    master_parms.extract_parms(argc, argv, "master");
    xtsc_master master("master", master_parms);

    // Configure and construct tlm2pin
    xtsc_tlm2pin_memory_transactor_parms tlm2pin_parms(core0, "pif");
    tlm2pin_parms.set("inbound_pif",     true);
    tlm2pin_parms.set("write_responses", false);
    tlm2pin_parms.set("vcd_handle",      waveforms);
    tlm2pin_parms.extract_parms(argc, argv, "tlm2pin");
    xtsc_tlm2pin_memory_transactor tlm2pin("tlm2pin", tlm2pin_parms);

    // Connect master to tlm2pin
    xtsc_connect(master, "master_port", "slave_port", tlm2pin);

    // Configure and construct pin2tlm
    xtsc_pin2tlm_memory_transactor_parms pin2tlm_parms(core0, "pif");
    pin2tlm_parms.set("inbound_pif",    true);
    pin2tlm_parms.set("vcd_handle",     waveforms);
    pin2tlm_parms.extract_parms(argc, argv, "pin2tlm");
    xtsc_pin2tlm_memory_transactor pin2tlm("pin2tlm", pin2tlm_parms);

    // Connect tlm2pin to pin2tlm
    xtsc_connect(tlm2pin, "pif", "pif", pin2tlm);

    // Connect pin2tlm to core0
    xtsc_connect(core0, "inbound_pif", "master_port", pin2tlm);

    // Load program
    core0.load_program("target/hello.out");

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

