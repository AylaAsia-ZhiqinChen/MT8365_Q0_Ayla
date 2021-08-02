// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 Tensilica Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include <iostream>
#include <xtsc/xtsc_master.h>
#include "template_memory.h"

using namespace std;
using namespace sc_core;
using namespace log4xtensa;
using namespace xtsc;
using namespace xtsc_component;

int sc_main(int argc, char *argv[]) {
  try {

    xtsc_initialize_parms init_parms("TextLogger.txt");
    init_parms.extract_parms(argc, argv, "xtsc");
    xtsc_initialize(init_parms);

    xtsc_master_parms master_parms("master.vec");
    master_parms.extract_parms(argc, argv, "master");
    xtsc_master master("master", master_parms);

    template_memory_parms null_memory_parms(4, 0xff, 500);
    null_memory_parms.extract_parms(argc, argv, "null_memory");
    template_memory null_memory("null_memory", null_memory_parms);

    master.m_request_port(null_memory.m_request_export);
    null_memory.m_respond_port(master.m_respond_export);

    sc_start();

  }
  catch (const exception& error) {
    ostringstream oss;
    oss << "std::exception caught:" << endl;
    oss << "what(): " << error.what() << endl;
    xtsc_log_multiline(TextLogger::getInstance("sc_main"), FATAL_LOG_LEVEL, oss.str(), 2);
    cerr << oss.str();
  }

  return 0;
}
