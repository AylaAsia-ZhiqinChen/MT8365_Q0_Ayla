// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2013 Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.


// Header files for each module
#include <xtsc/xtsc_arbiter.h>
#include <xtsc/xtsc_core.h>
#include <xtsc/xtsc_dma_engine.h>
#include <xtsc/xtsc_memory.h>
#include <xtsc/xtsc_mmio.h>
#include <xtsc/xtsc_router.h>


// Declare some namespaces for convenience
using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace xtsc;
using namespace xtsc_component;


// In case you want to log something
static log4xtensa::TextLogger& logger = log4xtensa::TextLogger::getInstance("sc_main");


int sc_main(int argc, char *argv[]) {

  try {

    // Declare each module pointer
    xtsc_core                      *core0;
    xtsc_router                    *core0_router;
    xtsc_dma_engine                *dma;
    xtsc_router                    *dma_router;
    xtsc_arbiter                   *mmio_arbiter;
    xtsc_mmio                      *mmio;
    xtsc_arbiter                   *pif_arbiter;
    xtsc_memory                    *pifmem;
    xtsc_memory                    *dram0;

    // Initialize XTSC
    xtsc_initialize_parms init_parms("../TextLogger.txt");;
    init_parms.extract_parms(argc, argv, "xtsc");
    xtsc_initialize(init_parms);

    // Configure core core0
    xtsc_core_parms core0_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
    core0_parms.extract_parms(argc, argv, "core0");

    // Create core core0
    core0 = new xtsc_core("core0", core0_parms);

    // Configure router core0_router
    xtsc_router_parms core0_router_parms;
    core0_router_parms.set("immediate_timing", true);
    core0_router_parms.set("num_slaves", 3);
    core0_router_parms.set("default_port_num", 0x0);
    core0_router_parms.set("routing_table", "core0_router.tab");
    core0_router_parms.extract_parms(argc, argv, "core0_router");

    // Create router core0_router
    core0_router = new xtsc_router("core0_router", core0_router_parms);

    // Configure dma dma
    xtsc_dma_engine_parms dma_parms(0x00000000);
    dma_parms.set("byte_width", 8);
    dma_parms.set("clear_notify_value", true);
    dma_parms.set("reg_base_address", 0x20001000);
    dma_parms.extract_parms(argc, argv, "dma");

    // Create dma dma
    dma = new xtsc_dma_engine("dma", dma_parms);

    // Configure router dma_router
    xtsc_router_parms dma_router_parms;
    dma_router_parms.set("immediate_timing", true);
    dma_router_parms.set("num_slaves", 3);
    dma_router_parms.set("default_port_num", 0x0);
    dma_router_parms.set("routing_table", "dma_router.tab");
    dma_router_parms.extract_parms(argc, argv, "dma_router");

    // Create router dma_router
    dma_router = new xtsc_router("dma_router", dma_router_parms);

    // Configure arbiter mmio_arbiter
    xtsc_arbiter_parms mmio_arbiter_parms;
    mmio_arbiter_parms.set("num_masters", 0x2);
    mmio_arbiter_parms.extract_parms(argc, argv, "mmio_arbiter");

    // Create arbiter mmio_arbiter
    mmio_arbiter = new xtsc_arbiter("mmio_arbiter", mmio_arbiter_parms);

    // Configure mmio mmio
    xtsc_mmio_parms mmio_parms("");
    mmio_parms.set("definition_file", "mmio.def");
    mmio_parms.set("swizzle_bytes", false);
    mmio_parms.extract_parms(argc, argv, "mmio");

    // Create mmio mmio
    mmio = new xtsc_mmio("mmio", mmio_parms);

    // Configure arbiter pif_arbiter
    xtsc_arbiter_parms pif_arbiter_parms;
    pif_arbiter_parms.set("num_masters", 0x2);
    pif_arbiter_parms.extract_parms(argc, argv, "pif_arbiter");

    // Create arbiter pif_arbiter
    pif_arbiter = new xtsc_arbiter("pif_arbiter", pif_arbiter_parms);

    // Configure memory pifmem
    xtsc_memory_parms pifmem_parms(*core0, "pif");
    pifmem_parms.set("check_alignment", true);
    pifmem_parms.set("initial_value_file", "pifmem.dat");
    pifmem_parms.extract_parms(argc, argv, "pifmem");

    // Create memory pifmem
    pifmem = new xtsc_memory("pifmem", pifmem_parms);

    // Configure memory dram0
    xtsc_memory_parms dram0_parms(*core0, "dram0");
    dram0_parms.extract_parms(argc, argv, "dram0");

    // Create memory dram0
    dram0 = new xtsc_memory("dram0", dram0_parms);

    // Connect core0 to core0_router
    xtsc_connect(*core0, "pif", "slave_port", *core0_router);

    // Connect core0_router to pif_arbiter
    xtsc_connect(*core0_router, "master_port[0]", "slave_port[1]", *pif_arbiter);

    // Connect core0_router to port 0 of dma
    xtsc_connect(*core0_router, "master_port[1]", "slave_port", *dma);

    // Connect dma to dma_router
    xtsc_connect(*dma, "master_port", "slave_port", *dma_router);

    // Connect dma_router to pif_arbiter
    xtsc_connect(*dma_router, "master_port[0]", "slave_port[0]", *pif_arbiter);

    // Connect dma_router to core0
    xtsc_connect(*dma_router, "master_port[1]", "inbound_pif", *core0);

    // Connect core0_router to mmio_arbiter
    xtsc_connect(*core0_router, "master_port[2]", "slave_port[0]", *mmio_arbiter);

    // Connect dma_router to mmio_arbiter
    xtsc_connect(*dma_router, "master_port[2]", "slave_port[1]", *mmio_arbiter);

    // Connect mmio_arbiter to mmio
    xtsc_connect(*mmio_arbiter, "master_port", "slave_port", *mmio);

    // Connect mmio to core0
    xtsc_connect(*mmio, "BInterrupt06", "BInterrupt06", *core0);

    // Connect pif_arbiter to port 0 of pifmem
    xtsc_connect(*pif_arbiter, "master_port", "", *pifmem);

    // Connect core0 to dram0
    xtsc_connect(*core0, "dram0", "", *dram0);

    // Load program
    core0->load_program("target/main.out");

    // Set-up debugging according to command line arguments, if any
    xtsc_core::setup_multicore_debug(argc, argv, 0xFFFFFFFF, true, false, true, 0x0);

    // Simulate
    sc_start();

    // Finalize XTSC
    xtsc_finalize();

    // Delete each sc_module we've created
    delete dram0;
    delete pifmem;
    delete pif_arbiter;
    delete mmio;
    delete mmio_arbiter;
    delete dma_router;
    delete dma;
    delete core0_router;
    delete core0;

  }
  catch (const exception& error) {
    ostringstream oss;
    oss << "std::exception caught: " << endl;
    oss << error.what() << endl;
    xtsc_log_multiline(logger, log4xtensa::FATAL_LOG_LEVEL, oss.str(), 2);
    cerr << oss.str();
  }

  return 0;
}
