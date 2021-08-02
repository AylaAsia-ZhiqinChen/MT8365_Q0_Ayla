// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.


#include <iostream>
#include <xtsc/xtsc.h>
#include <xtsc/xtsc_core.h>
#include <xtsc/xtsc_master.h>
#include <xtsc/xtsc_router.h>
#include "simple_memory.h"



using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;
using namespace xtsc_component;

static TextLogger& logger = TextLogger::getInstance("sc_main");

void memory_size_check(u32 line_number) {
  u32 sysramsize = 0x80000000;
  u32 sysromsize = 0x00040000;
  u32 maximum = 512*1024*1024; // Arbitrary limit of 512 MBytes
  if (sysramsize + sysromsize > maximum) {
    ostringstream oss;
    oss << "The combined size of sysram (0x" << hex << sysramsize << ") and sysrom (0x" << sysromsize << ") exceeds 0x"
        << maximum << endl;
    oss << "Because the simple_memory module allocates all memory up front, a large " << endl;
    oss << "target system memory may exceed this computer's memory facility limits." << endl;
    oss << "If you still want to try to run the simple_memory example with a xtsc_core, " << endl;
    oss << "you can comment out this check in line " << dec << line_number << " of " << __FILE__ << endl;
    throw xtsc_exception(oss.str());
  }
}

int sc_main(int argc, char *argv[]) {

  // Get command-line arguments regarding master
  bool test_using_master = false;
  for (int i=1; i<argc; ++i) {
    string arg = argv[i];
    if ((arg == "--master") || (arg == "-master")) {
      test_using_master = true;
    }
  }

  try {

    if (test_using_master) {

      xtsc_initialize_parms init_parms("TextLogger.txt");
      init_parms.extract_parms(argc, argv, "xtsc");
      xtsc_initialize(init_parms);

      xtsc_master_parms master_parms("../pif2sb_bridge/master.vec");
      master_parms.extract_parms(argc, argv, "master");
      xtsc_master master("master", master_parms);

      simple_memory_parms memory_parms(8, 0x80000000, 0x40020000);
      memory_parms.extract_parms(argc, argv, "memory");
      simple_memory memory("memory", memory_parms);

      master.m_request_port(memory.m_request_export);
      memory.m_respond_port(master.m_respond_export);

      sc_start();

    }
    else {

      xtsc_initialize_parms init_parms("../TextLogger.txt");
      init_parms.extract_parms(argc, argv, "xtsc");
      xtsc_initialize(init_parms);

      memory_size_check(__LINE__);

      // Configure and construct a core
      xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
      core_parms.extract_parms(argc, argv, "core0");
      xtsc_core core0("core0", core_parms);
      core0.load_program("target/hello.out");

      // Configure and construct a router with two slaves.
      // Make port 1 (sysram) the default port.
      xtsc_router_parms router_parms(2, true, "routing.tab", 1);
      router_parms.extract_parms(argc, argv, "router");
      xtsc_router router("router", router_parms);

      // Create System ROM using simple_memory
      simple_memory_parms rom_parms(8, 0x00040000, 0x40000000);
      rom_parms.extract_parms(argc, argv, "rom");
      simple_memory rom("rom", rom_parms);

      // Create System RAM using simple_memory
      simple_memory_parms ram_parms(8, 0x80000000, 0x40020000);
      ram_parms.extract_parms(argc, argv, "ram");
      simple_memory ram("ram", ram_parms);

      // Connect everything together
      xtsc_connect(core0, "pif", "slave_port", router);
      (*router.m_request_ports[0])(rom.m_request_export);
      (*router.m_request_ports[1])(ram.m_request_export);
      rom.m_respond_port(*router.m_respond_exports[0]);
      ram.m_respond_port(*router.m_respond_exports[1]);

      // Set-up debugging according to command line arguments, if any
      core0.setup_debug(argc, argv);

      sc_start();
    }

  }
  catch (const exception& error) {
    ostringstream oss;
    oss << "std::exception caught: " << endl;
    oss << error.what() << endl;
    xtsc_log_multiline(logger, FATAL_LOG_LEVEL, oss.str(), 2);
  }

  return 0;
}
