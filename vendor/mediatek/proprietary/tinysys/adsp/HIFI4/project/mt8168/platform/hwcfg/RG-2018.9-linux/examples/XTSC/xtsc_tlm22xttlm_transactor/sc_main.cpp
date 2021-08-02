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
#include <xtsc/xtsc_memory.h>
#include <xtsc/xtsc_master_tlm2.h>
#include <xtsc/xtsc_tlm22xttlm_transactor.h>


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

    // Configure and construct core0
    xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
    core_parms.set("SimTargetOutput", "core0_output.log");
    core_parms.extract_parms(argc, argv, "core0");
    xtsc_core core0("core0", core_parms);

    // Configure, construct, and connect a PIF memory
    xtsc_memory_parms pif_parms(core0, "pif");
    pif_parms.extract_parms(argc, argv, "pif");
    xtsc_memory pif("pif", pif_parms);
    xtsc_connect(core0, "pif", "", pif);

    // Configure, construct, and connect a DRAM0 memory
    xtsc_memory_parms dram0_parms(core0, "dram0");
    dram0_parms.extract_parms(argc, argv, "dram0");
    xtsc_memory dram0("dram0", dram0_parms);
    xtsc_connect(core0, "dram0", "", dram0);

    // Configure and construct master
    xtsc_master_tlm2_parms master_tlm2_parms("master_tlm2.vec", core_parms.get_u32("PIFByteWidth"));
    master_tlm2_parms.extract_parms(argc, argv, "master_tlm2");
    xtsc_master_tlm2 master_tlm2("master_tlm2", master_tlm2_parms);

    // Configure and construct tlm22xttlm
    xtsc_tlm22xttlm_transactor_parms tlm22xttlm_parms(core_parms.get_u32("PIFByteWidth"));
    tlm22xttlm_parms.extract_parms(argc, argv, "tlm22xttlm");
    xtsc_tlm22xttlm_transactor tlm22xttlm("tlm22xttlm", tlm22xttlm_parms);

    // Connect master_tlm2 to tlm22xttlm
    xtsc_connect(master_tlm2, "initiator_socket", "target_socket", tlm22xttlm);

    // Connect tlm22xttlm to core0
    xtsc_connect(tlm22xttlm, "master_port", "inbound_pif", core0);

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

