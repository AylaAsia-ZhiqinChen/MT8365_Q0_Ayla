// Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=../TextLogger.txt

// Configure and create source to drive A and B
-set_source_parm=definition_file=source.def
-set_source_parm=script_file=source.vec
-create_source=source

// Configure and create wire logic
-set_logic_parm=definition_file=logic.def
-create_logic=logic

// Configure and create a wire to record output not_A
-set_wire_parm=bit_width=32
-set_wire_parm=write_file=not_A.dat
-create_wire=not_A

// Configure and create a wire to record output A_and_B
-set_wire_parm=bit_width=32
-set_wire_parm=write_file=A_and_B.dat
-create_wire=A_and_B

// Configure and create a wire to record output A_or_B
-set_wire_parm=bit_width=32
-set_wire_parm=write_file=A_or_B.dat
-create_wire=A_or_B

// Configure and create a wire to record output A_xor_B
-set_wire_parm=bit_width=32
-set_wire_parm=write_file=A_xor_B.dat
-create_wire=A_xor_B

// Configure and create a wire to record output A0
-set_wire_parm=bit_width=1
-set_wire_parm=write_file=A0.dat
-create_wire=A0

// Configure and create a wire to record output A1
-set_wire_parm=bit_width=1
-set_wire_parm=write_file=A1.dat
-create_wire=A1

// Configure and create a wire to record output A_dup1
-set_wire_parm=bit_width=32
-set_wire_parm=write_file=A_dup1.dat
-create_wire=A_dup1

// Configure and create a wire to record output A_dup2
-set_wire_parm=bit_width=32
-set_wire_parm=write_file=A_dup2.dat
-create_wire=A_dup2

// Configure and create a wire to record output A_B
-set_wire_parm=bit_width=64
-set_wire_parm=write_file=A_B.dat
-create_wire=A_B

// Connect inputs
-connect=source,A,A,logic
-connect=source,B,B,logic

// Connect outputs
-connect=logic,not_A,wire_write,not_A
-connect=logic,A_and_B,wire_write,A_and_B
-connect=logic,A_or_B,wire_write,A_or_B
-connect=logic,A_xor_B,wire_write,A_xor_B
-connect=logic,A0,wire_write,A0
-connect=logic,A1,wire_write,A1
-connect=logic,A_dup1,wire_write,A_dup1
-connect=logic,A_dup2,wire_write,A_dup2
-connect=logic,A_B,wire_write,A_B
