// Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.


// Header files for each module
#include <xtsc/xtsc_wire.h>
#include <xtsc/xtsc_wire_logic.h>
#include <xtsc/xtsc_wire_source.h>


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
    xtsc_wire_source               *source;
    xtsc_wire_logic                *logic;
    xtsc_wire                      *not_A;
    xtsc_wire                      *A_and_B;
    xtsc_wire                      *A_or_B;
    xtsc_wire                      *A_xor_B;
    xtsc_wire                      *A0;
    xtsc_wire                      *A1;
    xtsc_wire                      *A_dup1;
    xtsc_wire                      *A_dup2;
    xtsc_wire                      *A_B;

    // Initialize XTSC
    xtsc_initialize_parms init_parms("../TextLogger.txt");
    init_parms.extract_parms(argc, argv, "xtsc");
    xtsc_initialize(init_parms);

    // Configure source source
    xtsc_wire_source_parms source_parms(1, "");
    source_parms.set("definition_file", "source.def");
    source_parms.set("script_file", "source.vec");
    source_parms.extract_parms(argc, argv, "source");

    // Create source source
    source = new xtsc_wire_source("source", source_parms);

    // Configure logic logic
    xtsc_wire_logic_parms logic_parms("");
    logic_parms.set("definition_file", "logic.def");
    logic_parms.extract_parms(argc, argv, "logic");

    // Create logic logic
    logic = new xtsc_wire_logic("logic", logic_parms);

    // Configure wire not_A
    xtsc_wire_parms not_A_parms;
    not_A_parms.set("bit_width", 0x20);
    not_A_parms.set("write_file", "not_A.dat");
    not_A_parms.extract_parms(argc, argv, "not_A");

    // Create wire not_A
    not_A = new xtsc_wire("not_A", not_A_parms);

    // Configure wire A_and_B
    xtsc_wire_parms A_and_B_parms;
    A_and_B_parms.set("bit_width", 0x20);
    A_and_B_parms.set("write_file", "A_and_B.dat");
    A_and_B_parms.extract_parms(argc, argv, "A_and_B");

    // Create wire A_and_B
    A_and_B = new xtsc_wire("A_and_B", A_and_B_parms);

    // Configure wire A_or_B
    xtsc_wire_parms A_or_B_parms;
    A_or_B_parms.set("bit_width", 0x20);
    A_or_B_parms.set("write_file", "A_or_B.dat");
    A_or_B_parms.extract_parms(argc, argv, "A_or_B");

    // Create wire A_or_B
    A_or_B = new xtsc_wire("A_or_B", A_or_B_parms);

    // Configure wire A_xor_B
    xtsc_wire_parms A_xor_B_parms;
    A_xor_B_parms.set("bit_width", 0x20);
    A_xor_B_parms.set("write_file", "A_xor_B.dat");
    A_xor_B_parms.extract_parms(argc, argv, "A_xor_B");

    // Create wire A_xor_B
    A_xor_B = new xtsc_wire("A_xor_B", A_xor_B_parms);

    // Configure wire A0
    xtsc_wire_parms A0_parms;
    A0_parms.set("bit_width", 0x1);
    A0_parms.set("write_file", "A0.dat");
    A0_parms.extract_parms(argc, argv, "A0");

    // Create wire A0
    A0 = new xtsc_wire("A0", A0_parms);

    // Configure wire A1
    xtsc_wire_parms A1_parms;
    A1_parms.set("bit_width", 0x1);
    A1_parms.set("write_file", "A1.dat");
    A1_parms.extract_parms(argc, argv, "A1");

    // Create wire A1
    A1 = new xtsc_wire("A1", A1_parms);

    // Configure wire A_dup1
    xtsc_wire_parms A_dup1_parms;
    A_dup1_parms.set("bit_width", 0x20);
    A_dup1_parms.set("write_file", "A_dup1.dat");
    A_dup1_parms.extract_parms(argc, argv, "A_dup1");

    // Create wire A_dup1
    A_dup1 = new xtsc_wire("A_dup1", A_dup1_parms);

    // Configure wire A_dup2
    xtsc_wire_parms A_dup2_parms;
    A_dup2_parms.set("bit_width", 0x20);
    A_dup2_parms.set("write_file", "A_dup2.dat");
    A_dup2_parms.extract_parms(argc, argv, "A_dup2");

    // Create wire A_dup2
    A_dup2 = new xtsc_wire("A_dup2", A_dup2_parms);

    // Configure wire A_B
    xtsc_wire_parms A_B_parms;
    A_B_parms.set("bit_width", 0x40);
    A_B_parms.set("write_file", "A_B.dat");
    A_B_parms.extract_parms(argc, argv, "A_B");

    // Create wire A_B
    A_B = new xtsc_wire("A_B", A_B_parms);

    // Connect source to logic
    xtsc_connect(*source, "A", "A", *logic);

    // Connect source to logic
    xtsc_connect(*source, "B", "B", *logic);

    // Connect logic to not_A
    xtsc_connect(*logic, "not_A",   "wire_write", *not_A);

    // Connect logic to A_and_B
    xtsc_connect(*logic, "A_and_B", "wire_write", *A_and_B);

    // Connect logic to A_or_B
    xtsc_connect(*logic, "A_or_B",  "wire_write", *A_or_B);

    // Connect logic to A_xor_B
    xtsc_connect(*logic, "A_xor_B", "wire_write", *A_xor_B);

    // Connect logic to A0
    xtsc_connect(*logic, "A0",      "wire_write", *A0);

    // Connect logic to A1
    xtsc_connect(*logic, "A1",      "wire_write", *A1);

    // Connect logic to A_dup1
    xtsc_connect(*logic, "A_dup1",  "wire_write", *A_dup1);

    // Connect logic to A_dup2
    xtsc_connect(*logic, "A_dup2",  "wire_write", *A_dup2);

    // Connect logic to A_B
    xtsc_connect(*logic, "A_B",     "wire_write", *A_B);

    // Simulate
    sc_start();

    // Finalize XTSC
    xtsc_finalize();

    // Delete each sc_module we've created
    delete A_B;
    delete A_dup2;
    delete A_dup1;
    delete A1;
    delete A0;
    delete A_xor_B;
    delete A_or_B;
    delete A_and_B;
    delete not_A;
    delete logic;
    delete source;

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
