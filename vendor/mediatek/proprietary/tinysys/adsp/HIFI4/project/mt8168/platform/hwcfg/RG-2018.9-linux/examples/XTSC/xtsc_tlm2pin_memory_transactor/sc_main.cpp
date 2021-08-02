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
#include <xtsc/xtsc_tlm2pin_memory_transactor.h>
#include <xtsc/xtsc_memory_pin.h>


using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;
using namespace xtsc_component;


// Create a logger for sc_main
static TextLogger& logger = TextLogger::getInstance("sc_main");



int sc_main(int argc, char *argv[]) {

  try {

    // Initialize
    xtsc_initialize_parms init_parms("../TextLogger.txt");
    init_parms.extract_parms(argc, argv, "xtsc");
    xtsc_initialize(init_parms);

    // Configuration parameters for xtsc_core
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
    core_parms.set("SimTargetOutput", "core0_output.log");
    core_parms.extract_parms(argc, argv, "core0");

    // Construct the core
    xtsc_core core0("core0", core_parms);

    // Tracing
    sc_trace_file *p_tf = sc_create_vcd_trace_file("waveforms");
    sc_clock *p_clk = new sc_clock("clk", xtsc_get_system_clock_period());
    sc_trace(p_tf, *p_clk, p_clk->name());

    // Iterate all possible memory ports 
    xtsc_core::memory_port p;
    xtsc_tlm2pin_memory_transactor *p_tlm2pin = NULL;
    xtsc_memory_pin *p_mem = NULL;
    for (p=xtsc_core::MEM_FIRST; p<=xtsc_core::MEM_LAST; ++p) {
      // Get the memory port name
      // Does this core have such a memory port?
      if (core0.has_memory_port(p)) {
        // Skip all but the first port in a multi-ported interface because
        // they have already been taken care of.
        if (xtsc_core::is_multi_port_zero(p)) {
          const char *port_name = xtsc_core::get_memory_port_name(p, true);

          // Create a tlm2pin transactor name
          string tlm2pin_name = string("tlm2pin_") + string(xtsc_core::get_memory_port_name(p, true));
          // Create tlm2pin transactor parameters corresponding to this memory port
          xtsc_tlm2pin_memory_transactor_parms tlm2pin_parms(core0, port_name);
          tlm2pin_parms.extract_parms(argc, argv, tlm2pin_name);
          // Construct the tlm2pin transactor
          p_tlm2pin = new xtsc_tlm2pin_memory_transactor(tlm2pin_name.c_str(), tlm2pin_parms);

          // Create a memory name
          string mem_name = string("mem_") + string(xtsc_core::get_memory_port_name(p, true));
          // Create memory parameters corresponding to this memory port
          xtsc_memory_pin_parms memory_parms(core0, port_name);
          memory_parms.set("vcd_handle", p_tf);
          memory_parms.extract_parms(argc, argv, mem_name);
          // Construct the memory
          p_mem = new xtsc_memory_pin(mem_name.c_str(), memory_parms);

          // Connect the tlm2pin transactor
          xtsc_connect(core0, port_name, "slave_ports", *p_tlm2pin);

          // Connect the memory
          xtsc_connect(*p_tlm2pin, port_name, port_name, *p_mem);
          xtsc_connect(*p_tlm2pin, "debug_ports", "debug_exports", *p_mem);
        }
      }
    }

    // Load program
    core0.load_program("target/memory_test.out");

    // Set-up debugging according to command line arguments, if any
    xtsc_core::setup_multicore_debug(argc, argv);

    // Start and run the simulation
    sc_start();

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


