// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
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

#include "pif2sb_bridge.h"

#include "simple_bus/simple_bus.h"
#include "simple_bus/simple_bus_fast_mem.h"
#include "simple_bus/simple_bus_slow_mem.h"
#include "simple_bus/simple_bus_arbiter.h"


using namespace std;
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
    oss << "Because the memory models in the OSCI simple_bus example allocate all memory up front," << endl;
    oss << "a large target system memory may exceed this computer's memory facility limits." << endl;
    oss << "If you still want to try to run the pif2sb_bridge example, you can comment out this " << endl;
    oss << "check in line " << dec << line_number << " of " << __FILE__ << endl;
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

    xtsc_initialize_parms init_parms("../TextLogger.txt");
    init_parms.extract_parms(argc, argv, "xtsc");
    xtsc_initialize(init_parms);

    memory_size_check(__LINE__);

    pif2sb_bridge_parms         bridge_parms            (8, 0x100);   // 0x100 = larger number = lower priority
    pif2sb_bridge               bridge                  ("bridge", bridge_parms);

    xtsc_master_parms           traffic_master_parms    ("traffic.vec", true);
    xtsc_master                 traffic_master          ("traffic_master", traffic_master_parms);

    pif2sb_bridge_parms         traffic_bridge_parms    (8, 0x0);     // 0x0 = smaller number = higher priority
    pif2sb_bridge               traffic_bridge          ("traffic_bridge", traffic_bridge_parms);

    sc_clock                    clk                     ("clk");
    simple_bus                  bus                     ("bus");
    simple_bus_arbiter          arbiter                 ("arbiter");
    simple_bus_slow_mem         sysrom                  ("sysrom", 0xC0020000, 0xC005FFFF, 2);  // 2 wait states on ROM
    simple_bus_fast_mem         sysram                  ("sysram", 0x40020000, 0xC001FFFF);

    bridge.m_sb_direct_port     (bus);
    bridge.m_sb_blocking_port   (bus);

    bus.clock                   (clk);
    bus.arbiter_port            (arbiter);
    bus.slave_port              (sysrom);
    bus.slave_port              (sysram);

    sysrom.clock                (clk);

    traffic_master.m_request_port    (traffic_bridge.m_pif_request_export);
    traffic_bridge.m_pif_respond_port(traffic_master.m_respond_export);

    traffic_bridge.m_sb_direct_port  (bus);
    traffic_bridge.m_sb_blocking_port(bus);


    if (test_using_master) {
      xtsc_master_parms       master_parms("master.vec");
      xtsc_master             master("master", master_parms);

      master.m_request_port    (bridge.m_pif_request_export);
      bridge.m_pif_respond_port(master.m_respond_export);

      sc_start(1000*clk.period());
    }
    else {
      xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
      core_parms.extract_parms(argc, argv, "core0");
      xtsc_core core0("core0", core_parms);
      core0.get_request_port("pif")(bridge.m_pif_request_export);
      bridge.m_pif_respond_port(core0.get_respond_export("pif"));
      core0.load_program("target/hello.out");

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
