// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 Tensilica Inc.  ALL RIGHTS RESERVED.
//
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// TODO:  Thorough check of PIF byte enables

;# Instructions:
;#   1) Edit the "my" variables below as desired and then run tpp.
;#      For example:
;#        $XTENSA_SW_TOOLS/libexec/tpp master.vec.tpp > master.vec
;#   2) If required, edit MASTER and WAIT in master.vec.
;#   3) Build and simulate your system.  For example a simple xtsc_master
;#      to xtsc_memory system can be simulated using xtsc-run and an include
;#      file called testbench.inc which contains:
;#              --set_master_parm=script_file=master.vec
;#              --create_master=master
;#              // byte_width must match BusByteWidth used to generate master.vec
;#              --set_memory_parm=byte_width=16
;#              --create_memory=memory
;#              --connect_master_memory=master,0,memory
;#      Be sure to define the XTSC logging environment variable:
;#              export XTSC_TEXTLOGGER_CONFIG_FILE=TextLogger.txt
;#      Now run the simulation:
;#              xtsc-run -include=testbench.inc
;#   4) Run the self-describing file checker script to ensure no errors
;#      have occurred (See "Self-Describing Files" in xtsc_ug.pdf).
;#              self_describing_file.checker.pl xtsc.log
;#
;my $MemBase        = 0x60000000;       # Match the desired memory's base address
;my $BusByteWidth   = 16;               # 4|8|16|32|64.  IRAM/IROM/PIF width may not exceed 16 bytes (128 bits)
;my $DoSubWord      = 1;                # Set to 0 for IRAM/IROM
;my $DoRCW          = 1;                # PIF only.  Check Read-Conditional-Write
;my $DoBlock        = 1;                # PIF only.  Check BLOCK_WRITE and BLOCK_READ
;my $DoCWF          = 1;                # PIF only.  Check Critical Word First BLOCK_READ (DoBlock must be true)
;my $DoBurst        = 1;                # PIF only.  Check BURST_WRITE and BURST_READ
;my $DoAddressError = 1;                # PIF only
;#
;sub hx { sprintf("0x%08x", shift); }

// Set MASTER to be the sc_module name of your xtsc_master
#define MASTER=master

// Increase wait if the downstream device is slow/far-away, to ensure
// that the writes complete before the peeks occur that check on them
#define WAIT wait 1

// Adjust SYNC if desired
#define SYNC sync 0.3

info &grep End-Of-Script

info
info  **************************************************************
info  *                    Testing POKE's                          *
info  **************************************************************
info

// Clear contents
now   POKE  ` hx($MemBase + 0x0000)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0010)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0020)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0030)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0040)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0050)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0060)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0070)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0080)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0090)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

/*
delay POKE  address    sz byt0 byt1 byt2 byt3 . . .  */
now   POKE  ` hx($MemBase + 0x0000)` 1  0x00
now   POKE  ` hx($MemBase + 0x0001)` 1  0x01
now   POKE  ` hx($MemBase + 0x0002)` 1  0x02
now   POKE  ` hx($MemBase + 0x0003)` 1  0x03
now   POKE  ` hx($MemBase + 0x0004)` 2  0x04 0x05
now   POKE  ` hx($MemBase + 0x0006)` 2  0x06 0x07
now   POKE  ` hx($MemBase + 0x0008)` 1  0x08
now   POKE  ` hx($MemBase + 0x0009)` 2  0x09 0x0a
now   POKE  ` hx($MemBase + 0x000b)` 2  0x0b 0x0c
now   POKE  ` hx($MemBase + 0x000d)` 3  0x0d 0x0e 0x0f
now   POKE  ` hx($MemBase + 0x0010)` 3  0x10 0x11 0x12
now   POKE  ` hx($MemBase + 0x0013)` 3  0x13 0x14 0x15
now   POKE  ` hx($MemBase + 0x0016)` 3  0x16 0x17 0x18
now   POKE  ` hx($MemBase + 0x0019)` 3  0x19 0x1a 0x1b
now   POKE  ` hx($MemBase + 0x001c)` 4  0x1c 0x1d 0x1e 0x1f
now   POKE  ` hx($MemBase + 0x0020)` 5  0x20 0x21 0x22 0x23 0x24
now   POKE  ` hx($MemBase + 0x0025)` 5  0x25 0x26 0x27 0x28 0x29
now   POKE  ` hx($MemBase + 0x002a)` 5  0x2a 0x2b 0x2c 0x2d 0x2e
now   POKE  ` hx($MemBase + 0x002f)` 5  0x2f 0x30 0x31 0x32 0x33
now   POKE  ` hx($MemBase + 0x0034)` 8  0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b
now   POKE  ` hx($MemBase + 0x003c)` 16 0x3c 0x3d 0x3e 0x3f 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b
now   POKE  ` hx($MemBase + 0x004c)` 32 0x4c 0x4d 0x4e 0x4f 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b
now   POKE  ` hx($MemBase + 0x006c)` 64 0x6c 0x6d 0x6e 0x6f 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab

/*
delay PEEK  address    sz  */
info  Self-description:  &grep peek ` hx($MemBase + 0x0000)`: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
now   PEEK  ` hx($MemBase + 0x0000)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0010)`: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
now   PEEK  ` hx($MemBase + 0x0010)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0020)`: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
now   PEEK  ` hx($MemBase + 0x0020)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0030)`: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
now   PEEK  ` hx($MemBase + 0x0030)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0040)`: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f << Self-description
now   PEEK  ` hx($MemBase + 0x0040)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0050)`: 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f << Self-description
now   PEEK  ` hx($MemBase + 0x0050)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0060)`: 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f << Self-description
now   PEEK  ` hx($MemBase + 0x0060)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0070)`: 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f << Self-description
now   PEEK  ` hx($MemBase + 0x0070)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0080)`: 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f << Self-description
now   PEEK  ` hx($MemBase + 0x0080)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x0090)`: 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f << Self-description
now   PEEK  ` hx($MemBase + 0x0090)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x00a0)`: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab << Self-description
now   PEEK  ` hx($MemBase + 0x00a0)` 12
info  Self-description:


info
info  **************************************************************
info  *                    Testing PEEK's                          *
info  **************************************************************
info

/*
delay PEEK  address    sz */
info  Self-description:  &grep peek ` hx($MemBase + 0x0000)`: 00 << Self-description
now   PEEK  ` hx($MemBase + 0x0000)` 1
info  Self-description:  &grep peek ` hx($MemBase + 0x0001)`: 01 << Self-description
now   PEEK  ` hx($MemBase + 0x0001)` 1
info  Self-description:  &grep peek ` hx($MemBase + 0x0002)`: 02 << Self-description
now   PEEK  ` hx($MemBase + 0x0002)` 1
info  Self-description:  &grep peek ` hx($MemBase + 0x0003)`: 03 << Self-description
now   PEEK  ` hx($MemBase + 0x0003)` 1
info  Self-description:  &grep peek ` hx($MemBase + 0x0004)`: 04 05 << Self-description
now   PEEK  ` hx($MemBase + 0x0004)` 2
info  Self-description:  &grep peek ` hx($MemBase + 0x0006)`: 06 07 << Self-description
now   PEEK  ` hx($MemBase + 0x0006)` 2
info  Self-description:  &grep peek ` hx($MemBase + 0x0008)`: 08 << Self-description
now   PEEK  ` hx($MemBase + 0x0008)` 1
info  Self-description:  &grep peek ` hx($MemBase + 0x0009)`: 09 0a << Self-description
now   PEEK  ` hx($MemBase + 0x0009)` 2
info  Self-description:  &grep peek ` hx($MemBase + 0x000b)`: 0b 0c << Self-description
now   PEEK  ` hx($MemBase + 0x000b)` 2
info  Self-description:  &grep peek ` hx($MemBase + 0x000d)`: 0d 0e 0f << Self-description
now   PEEK  ` hx($MemBase + 0x000d)` 3
info  Self-description:  &grep peek ` hx($MemBase + 0x0010)`: 10 11 12 << Self-description
now   PEEK  ` hx($MemBase + 0x0010)` 3
info  Self-description:  &grep peek ` hx($MemBase + 0x0013)`: 13 14 15 << Self-description
now   PEEK  ` hx($MemBase + 0x0013)` 3
info  Self-description:  &grep peek ` hx($MemBase + 0x0016)`: 16 17 18 << Self-description
now   PEEK  ` hx($MemBase + 0x0016)` 3
info  Self-description:  &grep peek ` hx($MemBase + 0x0019)`: 19 1a 1b << Self-description
now   PEEK  ` hx($MemBase + 0x0019)` 3
info  Self-description:  &grep peek ` hx($MemBase + 0x001c)`: 1c 1d 1e 1f << Self-description
now   PEEK  ` hx($MemBase + 0x001c)` 4
info  Self-description:  &grep peek ` hx($MemBase + 0x0020)`: 20 21 22 23 24 << Self-description
now   PEEK  ` hx($MemBase + 0x0020)` 5
info  Self-description:  &grep peek ` hx($MemBase + 0x0025)`: 25 26 27 28 29 << Self-description
now   PEEK  ` hx($MemBase + 0x0025)` 5
info  Self-description:  &grep peek ` hx($MemBase + 0x002a)`: 2a 2b 2c 2d 2e << Self-description
now   PEEK  ` hx($MemBase + 0x002a)` 5
info  Self-description:  &grep peek ` hx($MemBase + 0x002f)`: 2f 30 31 32 33 << Self-description
now   PEEK  ` hx($MemBase + 0x002f)` 5
info  Self-description:  &grep peek ` hx($MemBase + 0x0001)`: 01 02 03 04 05 06 07 << Self-description
now   PEEK  ` hx($MemBase + 0x0001)` 7
info  Self-description:  &grep peek ` hx($MemBase + 0x0034)`: 34 35 36 37 38 39 3a 3b << Self-description
now   PEEK  ` hx($MemBase + 0x0034)` 8
info  Self-description:  &grep peek ` hx($MemBase + 0x003c)`: 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 4a 4b << Self-description
now   PEEK  ` hx($MemBase + 0x003c)` 16
info  Self-description:  &grep peek ` hx($MemBase + 0x004c)`: 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 6a 6b << Self-description
now   PEEK  ` hx($MemBase + 0x004c)` 32
info  Self-description:  &grep peek ` hx($MemBase + 0x006c)`: 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab << Self-description
now   PEEK  ` hx($MemBase + 0x006c)` 64
info  Self-description:


info
info  **************************************************************
info  *                    Testing WRITE's                         *
info  **************************************************************
info

// Clear contents
now   POKE  ` hx($MemBase + 0x0000)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0010)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0020)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0030)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0040)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0050)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0060)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0070)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0080)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x0090)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x00f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

/*
delay WRITE address                  sz rt id pr pc BE     byt0 byt1 byt2 byt3 */

; if ($DoSubWord) {
1     WRITE ` hx($MemBase + 0x0000)` 1  0  0  0  0  0x000F 0x00
wait  rsp
1     WRITE ` hx($MemBase + 0x0001)` 1  0  0  0  0  0x000F 0x01
wait  rsp
1     WRITE ` hx($MemBase + 0x0002)` 2  0  0  0  0  0x000F 0x02 0x03
wait  rsp
; } else {
1     WRITE ` hx($MemBase + 0x0000)` 4  0  0  0  0  0x000F 0x00 0x01 0x02 0x03
wait  rsp
; }
1     WRITE ` hx($MemBase + 0x0004)` 4  0  0  0  0  0x000F 0x04 0x05 0x06 0x07
wait  rsp
1     WRITE ` hx($MemBase + 0x0008)` 4  0  0  0  0  0x000F 0x08 0x09 0x0a 0x0b
wait  rsp
1     WRITE ` hx($MemBase + 0x000c)` 4  0  0  0  0  0x000F 0x0c 0x0d 0x0e 0x0f
wait  rsp

$(WAIT)
info  &grep peek ` hx($MemBase + 0x0000)`: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
now   PEEK  ` hx($MemBase + 0x0000)` 16
info  Self-description

;if ($BusByteWidth >= 8) {
1     WRITE ` hx($MemBase + 0x0010)` 8  0  0  0  0  0x00FF 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17
wait  rsp
1     WRITE ` hx($MemBase + 0x0018)` 8  0  0  0  0  0x00FF 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait  rsp

$(WAIT)
info  &grep peek ` hx($MemBase + 0x0010)`: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
now   PEEK  ` hx($MemBase + 0x0010)` 16
info  Self-description

; }
; if ($BusByteWidth >= 16) {
1     WRITE ` hx($MemBase + 0x0020)` 16 0  0  0  0  0xFFFF 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait  rsp
1     WRITE ` hx($MemBase + 0x0030)` 16 0  0  0  0  0xFFFF 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait  rsp

$(WAIT)
info  &grep peek ` hx($MemBase + 0x0020)`: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
now   PEEK  ` hx($MemBase + 0x0020)` 32
info  Self-description

; }
; if ($BusByteWidth >= 32) {
1     WRITE ` hx($MemBase + 0x0040)` 32 0  0  0  0  0xFFFFFFFF 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait  rsp
1     WRITE ` hx($MemBase + 0x0060)` 32 0  0  0  0  0xFFFFFFFF 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait  rsp

$(WAIT)
info  &grep peek ` hx($MemBase + 0x0040)`: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f << Self-description
now   PEEK  ` hx($MemBase + 0x0040)` 64
info  Self-description

; }
; if ($BusByteWidth >= 64) {
1     WRITE ` hx($MemBase + 0x0080)` 64 0  0  0  0  0xFFFFFFFFFFFFFFFF 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait  rsp
1     WRITE ` hx($MemBase + 0x00c0)` 64 0  0  0  0  0xFFFFFFFFFFFFFFFF 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff
wait  rsp

$(WAIT)
info  &grep peek ` hx($MemBase + 0x0080)`: 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
now   PEEK  ` hx($MemBase + 0x0080)` 128
info  Self-description

; }
; if ($DoAddressError) {


info
info  **************************************************************
info  *            Testing RSP_ADDRESS_ERROR on WRITE              *
info  **************************************************************
info

info  &grep RSP_ADDRESS_ERROR << Self-description
// This should fail
1     WRITE ` hx($MemBase + 0x0009)`  4 0  0  0  0  0x00FF 0x19 0x1a 0x1b 0x1c
wait  rsp

$(WAIT)
info  Self-description:  &grep 00 01 02 03 04 05 06 07 08 09 0a 0b 0c *$ << Self-description
now   PEEK  ` hx($MemBase + 0x0000)` 13
info  Self-description:

; }


info
info  **************************************************************
info  *                    Testing READ's                          *
info  **************************************************************
info

/*
delay READ  address    sz rt id pr pc  */

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 00 *$ << Self-description
1     READ  ` hx($MemBase + 0x0000)` 1  0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 01 *$ << Self-description
1     READ  ` hx($MemBase + 0x0001)` 1  0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 02 03 *$ << Self-description
1     READ  ` hx($MemBase + 0x0002)` 2  0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 04 05 06 07 *$ << Self-description
1     READ  ` hx($MemBase + 0x0004)` 4  0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 08 09 0a 0b *$ << Self-description
1     READ  ` hx($MemBase + 0x0008)` 4  0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 0c 0d 0e 0f *$ << Self-description
1     READ  ` hx($MemBase + 0x000c)` 4  0  0  0  0
wait  rsp

;if ($BusByteWidth >= 8) {
info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 10 11 12 13 14 15 16 17 *$ << Self-description
1     READ  ` hx($MemBase + 0x0010)` 8  0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f *$ << Self-description
1     READ  ` hx($MemBase + 0x0018)` 8  0  0  0  0
wait  rsp

; }
; if ($BusByteWidth >= 16) {
info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f *$ << Self-description
1     READ  ` hx($MemBase + 0x0020)` 16 0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f *$ << Self-description
1     READ  ` hx($MemBase + 0x0030)` 16 0  0  0  0
wait  rsp

; }
; if ($BusByteWidth >= 32) {
info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f *$ << Self-description
1     READ  ` hx($MemBase + 0x0040)` 32 0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
1     READ  ` hx($MemBase + 0x0060)` 32 0  0  0  0
wait  rsp

; }
; if ($BusByteWidth >= 64) {
info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf *$ << Self-description
1     READ  ` hx($MemBase + 0x0080)` 64 0  0  0  0
wait  rsp

info  Self-description:  &grep INFO.*$(MASTER).*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff *$ << Self-description
1     READ  ` hx($MemBase + 0x00c0)` 64 0  0  0  0
wait  rsp

; }
info  Self-description
; if ($DoAddressError) {


info
info  **************************************************************
info  *            Testing RSP_ADDRESS_ERROR on READ               *
info  **************************************************************
info

info  Self-description:  &grep RSP_ADDRESS_ERROR << Self-description
// This should fail
1     READ  ` hx($MemBase + 0x0009)` 4  0  0  0  0
wait  rsp
info  Self-description:

; }
; if ($DoRCW) {


info
info  **************************************************************
info  *                    Testing RCW's                           *
info  **************************************************************
info

/*
delay RCW         address    sz rt id pr pc num last byt0 byt1 byt2 byt3 */
10    RCW         ` hx($MemBase + 0x2004)`  4 0  0  0  0  2   0    0xba 0xbe 0xfa 0xce
1     RCW         ` hx($MemBase + 0x2004)`  4 0  0  0  0  2   1    0xca 0xfe 0xf0 0x0d
wait  rsp

$(WAIT)
info  Self-description:  &grep! ca fe f0 0d << Self-description
now   PEEK  ` hx($MemBase + 0x2004)` 4
info  Self-description:

now   POKE  ` hx($MemBase + 0x2004)` 4 0xba 0xbe 0xfa 0xce

/*
delay RCW         address    sz rt id pr pc num last byt0 byt1 byt2 byt3 */
10    RCW         ` hx($MemBase + 0x2004)`  4 0  0  0  0  2   0    0xba 0xbe 0xfa 0xce
1     RCW         ` hx($MemBase + 0x2004)`  4 0  0  0  0  2   1    0xca 0xfe 0xf0 0x0d
wait  rsp

$(WAIT)
info  Self-description:  &grep ca fe f0 0d *$ << Self-description
now   PEEK  ` hx($MemBase + 0x2004)` 4
info  Self-description:

; }
; if ($DoBlock) {


info
info  **************************************************************
info  *                Testing BLOCK_WRITE's                       *
info  **************************************************************
info

// Clear contents
now   POKE  ` hx($MemBase + 0x3000)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3010)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3020)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3030)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3040)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3050)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3060)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3070)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3080)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3090)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x30a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x30b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x30c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x30d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x30e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x30f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3100)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3110)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3120)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3130)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3140)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3150)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3160)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3170)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3180)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x3190)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x31a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x31b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x31c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x31d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x31e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x31f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

;if ($BusByteWidth == 4) {
/*
delay BLOCK_WRITE address    sz rt id pr pc num last first  b0 b1 b2 b3 */

10    BLOCK_WRITE ` hx($MemBase + 0x3000)`  4 0  0  0  0  2   0    1       0  1  2  3
10    BLOCK_WRITE ` hx($MemBase + 0x3004)`  4 0  0  0  0  2   1    0       4  5  6  7
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3010)`  4 0  0  0  0  4   0    1      16 17 18 19
10    BLOCK_WRITE ` hx($MemBase + 0x3014)`  4 0  0  0  0  4   0    0      20 21 22 23
10    BLOCK_WRITE ` hx($MemBase + 0x3018)`  4 0  0  0  0  4   0    0      24 25 26 27
10    BLOCK_WRITE ` hx($MemBase + 0x301C)`  4 0  0  0  0  4   1    0      28 29 30 31
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3020)`  4 0  0  0  0  8   0    1      32 33 34 35
10    BLOCK_WRITE ` hx($MemBase + 0x3024)`  4 0  0  0  0  8   0    0      36 37 38 39
10    BLOCK_WRITE ` hx($MemBase + 0x3028)`  4 0  0  0  0  8   0    0      40 41 42 43
10    BLOCK_WRITE ` hx($MemBase + 0x302C)`  4 0  0  0  0  8   0    0      44 45 46 47
10    BLOCK_WRITE ` hx($MemBase + 0x3030)`  4 0  0  0  0  8   0    0      48 49 50 51
10    BLOCK_WRITE ` hx($MemBase + 0x3034)`  4 0  0  0  0  8   0    0      52 53 54 55
10    BLOCK_WRITE ` hx($MemBase + 0x3038)`  4 0  0  0  0  8   0    0      56 57 58 59
10    BLOCK_WRITE ` hx($MemBase + 0x303c)`  4 0  0  0  0  8   1    0      60 61 62 63
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3040)`  4 0  0  0  0 16   0    1      64 65 66 67
10    BLOCK_WRITE ` hx($MemBase + 0x3044)`  4 0  0  0  0 16   0    0      68 69 70 71
10    BLOCK_WRITE ` hx($MemBase + 0x3048)`  4 0  0  0  0 16   0    0      72 73 74 75
10    BLOCK_WRITE ` hx($MemBase + 0x304C)`  4 0  0  0  0 16   0    0      76 77 78 79
10    BLOCK_WRITE ` hx($MemBase + 0x3050)`  4 0  0  0  0 16   0    0      80 81 82 83
10    BLOCK_WRITE ` hx($MemBase + 0x3054)`  4 0  0  0  0 16   0    0      84 85 86 87
10    BLOCK_WRITE ` hx($MemBase + 0x3058)`  4 0  0  0  0 16   0    0      88 89 90 91
10    BLOCK_WRITE ` hx($MemBase + 0x305C)`  4 0  0  0  0 16   0    0      92 93 94 95
10    BLOCK_WRITE ` hx($MemBase + 0x3060)`  4 0  0  0  0 16   0    0      96 97 98 99
10    BLOCK_WRITE ` hx($MemBase + 0x3064)`  4 0  0  0  0 16   0    0      100 101 102 103
10    BLOCK_WRITE ` hx($MemBase + 0x3068)`  4 0  0  0  0 16   0    0      104 105 106 107
10    BLOCK_WRITE ` hx($MemBase + 0x306C)`  4 0  0  0  0 16   0    0      108 109 110 111
10    BLOCK_WRITE ` hx($MemBase + 0x3070)`  4 0  0  0  0 16   0    0      112 113 114 115
10    BLOCK_WRITE ` hx($MemBase + 0x3074)`  4 0  0  0  0 16   0    0      116 117 118 119
10    BLOCK_WRITE ` hx($MemBase + 0x3078)`  4 0  0  0  0 16   0    0      120 121 122 123
10    BLOCK_WRITE ` hx($MemBase + 0x307C)`  4 0  0  0  0 16   1    0      124 125 126 127
wait  rsp

$(WAIT)
info  Self-description:  &grep 00 01 02 03 04 05 06 07 *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3000)` 8
info  Self-description:  &grep 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3010)` 16
info  Self-description:  &grep 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3020)` 32
info  Self-description:  &grep 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3040)` 64
info  Self-description:

; } elsif ($BusByteWidth == 8) {
/*
delay BLOCK_WRITE address    sz rt id pr pc num last first  b0 b1 b2 b3 b4 b5 b6 b7 b8 */

10    BLOCK_WRITE ` hx($MemBase + 0x3000)`  8 0  0  0  0  2   0    1       0  1  2  3  4  5  6  7
10    BLOCK_WRITE ` hx($MemBase + 0x3008)`  8 0  0  0  0  2   1    0       8  9 10 11 12 13 14 15
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3020)`  8 0  0  0  0  4   0    1      32 33 34 35 36 37 38 39
10    BLOCK_WRITE ` hx($MemBase + 0x3028)`  8 0  0  0  0  4   0    0      40 41 42 43 44 45 46 47
10    BLOCK_WRITE ` hx($MemBase + 0x3030)`  8 0  0  0  0  4   0    0      48 49 50 51 52 53 54 55
10    BLOCK_WRITE ` hx($MemBase + 0x3038)`  8 0  0  0  0  4   1    0      56 57 58 59 60 61 62 63
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3040)`  8 0  0  0  0  8   0    1      64 65 66 67 68 69 70 71
10    BLOCK_WRITE ` hx($MemBase + 0x3048)`  8 0  0  0  0  8   0    0      72 73 74 75 76 77 78 79
10    BLOCK_WRITE ` hx($MemBase + 0x3050)`  8 0  0  0  0  8   0    0      80 81 82 83 84 85 86 87
10    BLOCK_WRITE ` hx($MemBase + 0x3058)`  8 0  0  0  0  8   0    0      88 89 90 91 92 93 94 95
10    BLOCK_WRITE ` hx($MemBase + 0x3060)`  8 0  0  0  0  8   0    0      96 97 98 99 100 101 102 103
10    BLOCK_WRITE ` hx($MemBase + 0x3068)`  8 0  0  0  0  8   0    0      104 105 106 107 108 109 110 111
10    BLOCK_WRITE ` hx($MemBase + 0x3070)`  8 0  0  0  0  8   0    0      112 113 114 115 116 117 118 119
10    BLOCK_WRITE ` hx($MemBase + 0x3078)`  8 0  0  0  0  8   1    0      120 121 122 123 124 125 126 127
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3080)`  8 0  0  0  0 16   0    1      128 129 130 131 132 133 134 135
10    BLOCK_WRITE ` hx($MemBase + 0x3088)`  8 0  0  0  0 16   0    0      136 137 138 139 140 141 142 143
10    BLOCK_WRITE ` hx($MemBase + 0x3090)`  8 0  0  0  0 16   0    0      144 145 146 147 148 149 150 151
10    BLOCK_WRITE ` hx($MemBase + 0x3098)`  8 0  0  0  0 16   0    0      152 153 154 155 156 157 158 159
10    BLOCK_WRITE ` hx($MemBase + 0x30a0)`  8 0  0  0  0 16   0    0      160 161 162 163 164 165 166 167
10    BLOCK_WRITE ` hx($MemBase + 0x30a8)`  8 0  0  0  0 16   0    0      168 169 170 171 172 173 174 175
10    BLOCK_WRITE ` hx($MemBase + 0x30b0)`  8 0  0  0  0 16   0    0      176 177 178 179 180 181 182 183
10    BLOCK_WRITE ` hx($MemBase + 0x30b8)`  8 0  0  0  0 16   0    0      184 185 186 187 188 189 190 191
10    BLOCK_WRITE ` hx($MemBase + 0x30c0)`  8 0  0  0  0 16   0    0      192 193 194 195 196 197 198 199
10    BLOCK_WRITE ` hx($MemBase + 0x30c8)`  8 0  0  0  0 16   0    0      200 201 202 203 204 205 206 207
10    BLOCK_WRITE ` hx($MemBase + 0x30d0)`  8 0  0  0  0 16   0    0      208 209 210 211 212 213 214 215
10    BLOCK_WRITE ` hx($MemBase + 0x30d8)`  8 0  0  0  0 16   0    0      216 217 218 219 220 221 222 223
10    BLOCK_WRITE ` hx($MemBase + 0x30e0)`  8 0  0  0  0 16   0    0      224 225 226 227 228 229 230 231
10    BLOCK_WRITE ` hx($MemBase + 0x30e8)`  8 0  0  0  0 16   0    0      232 233 234 235 236 237 238 239
10    BLOCK_WRITE ` hx($MemBase + 0x30f0)`  8 0  0  0  0 16   0    0      240 241 242 243 244 245 246 247
10    BLOCK_WRITE ` hx($MemBase + 0x30f8)`  8 0  0  0  0 16   1    0      248 249 250 251 252 253 254 255
wait  rsp

$(WAIT)
info  Self-description:  &grep 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3000)` 16
info  Self-description:  &grep 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3020)` 32
info  Self-description:  &grep 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3040)` 64
info  Self-description:  &grep 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3080)` 128
info  Self-description:

; } else {
/*
delay BLOCK_WRITE address    sz rt id pr pc num last first  b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 . . .*/

10    BLOCK_WRITE ` hx($MemBase + 0x3000)` 16 0  0  0  0  2   0    1       0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
10    BLOCK_WRITE ` hx($MemBase + 0x3010)` 16 0  0  0  0  2   1    0       16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3040)` 16 0  0  0  0  4   0    1      64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79
10    BLOCK_WRITE ` hx($MemBase + 0x3050)` 16 0  0  0  0  4   0    0      80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95
10    BLOCK_WRITE ` hx($MemBase + 0x3060)` 16 0  0  0  0  4   0    0      96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111
10    BLOCK_WRITE ` hx($MemBase + 0x3070)` 16 0  0  0  0  4   1    0      112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3080)` 16 0  0  0  0  8   0    1      128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143
10    BLOCK_WRITE ` hx($MemBase + 0x3090)` 16 0  0  0  0  8   0    0      144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159
10    BLOCK_WRITE ` hx($MemBase + 0x30a0)` 16 0  0  0  0  8   0    0      160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175
10    BLOCK_WRITE ` hx($MemBase + 0x30b0)` 16 0  0  0  0  8   0    0      176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191
10    BLOCK_WRITE ` hx($MemBase + 0x30c0)` 16 0  0  0  0  8   0    0      192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207
10    BLOCK_WRITE ` hx($MemBase + 0x30d0)` 16 0  0  0  0  8   0    0      208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223
10    BLOCK_WRITE ` hx($MemBase + 0x30e0)` 16 0  0  0  0  8   0    0      224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239
10    BLOCK_WRITE ` hx($MemBase + 0x30f0)` 16 0  0  0  0  8   1    0      240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255
wait  rsp

10    BLOCK_WRITE ` hx($MemBase + 0x3100)` 16 0  0  0  0 16   0    1      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
10    BLOCK_WRITE ` hx($MemBase + 0x3110)` 16 0  0  0  0 16   0    0      16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
10    BLOCK_WRITE ` hx($MemBase + 0x3120)` 16 0  0  0  0 16   0    0      32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
10    BLOCK_WRITE ` hx($MemBase + 0x3130)` 16 0  0  0  0 16   0    0      48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
10    BLOCK_WRITE ` hx($MemBase + 0x3140)` 16 0  0  0  0 16   0    0      64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79
10    BLOCK_WRITE ` hx($MemBase + 0x3150)` 16 0  0  0  0 16   0    0      80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95
10    BLOCK_WRITE ` hx($MemBase + 0x3160)` 16 0  0  0  0 16   0    0      96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111
10    BLOCK_WRITE ` hx($MemBase + 0x3170)` 16 0  0  0  0 16   0    0      112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127
10    BLOCK_WRITE ` hx($MemBase + 0x3180)` 16 0  0  0  0 16   0    0      128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143
10    BLOCK_WRITE ` hx($MemBase + 0x3190)` 16 0  0  0  0 16   0    0      144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159
10    BLOCK_WRITE ` hx($MemBase + 0x31a0)` 16 0  0  0  0 16   0    0      160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175
10    BLOCK_WRITE ` hx($MemBase + 0x31b0)` 16 0  0  0  0 16   0    0      176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191
10    BLOCK_WRITE ` hx($MemBase + 0x31c0)` 16 0  0  0  0 16   0    0      192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207
10    BLOCK_WRITE ` hx($MemBase + 0x31d0)` 16 0  0  0  0 16   0    0      208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223
10    BLOCK_WRITE ` hx($MemBase + 0x31e0)` 16 0  0  0  0 16   0    0      224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239
10    BLOCK_WRITE ` hx($MemBase + 0x31f0)` 16 0  0  0  0 16   1    0      240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255
wait  rsp

$(WAIT)
info  Self-description:  &grep 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3000)` 32
info  Self-description:  &grep 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3040)` 64
info  Self-description:  &grep 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3080)` 128
info  Self-description:  &grep 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff *$ << Self-description
now   PEEK  ` hx($MemBase + 0x3100)` 256
info  Self-description:
info  ---------------------------------------------------------------------------------

; }

info
info  **************************************************************
info  *                Testing BLOCK_READ's                        *
info  **************************************************************
info

/*
delay BLOCK_READ  address    sz rt id pr pc num */
;if ($BusByteWidth == 4) {

10    BLOCK_READ  ` hx($MemBase + 0x3000)`  4 0  0  0  0  2
info  Self-description:  &grep 00 01 02 03 *$ << Self-description
wait  rsp
info  Self-description:  &grep 04 05 06 07 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3010)`  4 0  0  0  0  4
info  Self-description:  &grep 10 11 12 13 *$ << Self-description
wait  rsp
info  Self-description:  &grep 14 15 16 17 *$ << Self-description
wait  rsp
info  Self-description:  &grep 18 19 1a 1b *$ << Self-description
wait  rsp
info  Self-description:  &grep 1c 1d 1e 1f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3020)`  4 0  0  0  0  8
info  Self-description:  &grep 20 21 22 23 *$ << Self-description
wait  rsp
info  Self-description:  &grep 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b *$ << Self-description
wait  rsp
info  Self-description:  &grep 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 *$ << Self-description
wait  rsp
info  Self-description:  &grep 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:  &grep 38 39 3a 3b *$ << Self-description
wait  rsp
info  Self-description:  &grep 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3040)`  4 0  0  0  0  16
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:

; } elsif ($BusByteWidth == 8) {

10    BLOCK_READ  ` hx($MemBase + 0x3000)`  8 0  0  0  0  2
info  Self-description:  &grep 00 01 02 03 04 05 06 07 *$ << Self-description
wait  rsp
info  Self-description:  &grep 08 09 0a 0b 0c 0d 0e 0f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3020)`  8 0  0  0  0  4
info  Self-description:  &grep 20 21 22 23 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:  &grep 38 39 3a 3b 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3040)`  8 0  0  0  0  8
info  Self-description:  &grep 40 41 42 43 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3080)`  8 0  0  0  0  16
info  Self-description:  &grep 80 81 82 83 84 85 86 87 *$ << Self-description
wait  rsp
info  Self-description:  &grep 88 89 8a 8b 8c 8d 8e 8f *$ << Self-description
wait  rsp
info  Self-description:  &grep 90 91 92 93 94 95 96 97 *$ << Self-description
wait  rsp
info  Self-description:  &grep 98 99 9a 9b 9c 9d 9e 9f *$ << Self-description
wait  rsp
info  Self-description:  &grep a0 a1 a2 a3 a4 a5 a6 a7 *$ << Self-description
wait  rsp
info  Self-description:  &grep a8 a9 aa ab ac ad ae af *$ << Self-description
wait  rsp
info  Self-description:  &grep b0 b1 b2 b3 b4 b5 b6 b7 *$ << Self-description
wait  rsp
info  Self-description:  &grep b8 b9 ba bb bc bd be bf *$ << Self-description
wait  rsp
info  Self-description:  &grep c0 c1 c2 c3 c4 c5 c6 c7 *$ << Self-description
wait  rsp
info  Self-description:  &grep c8 c9 ca cb cc cd ce cf *$ << Self-description
wait  rsp
info  Self-description:  &grep d0 d1 d2 d3 d4 d5 d6 d7 *$ << Self-description
wait  rsp
info  Self-description:  &grep d8 d9 da db dc dd de df *$ << Self-description
wait  rsp
info  Self-description:  &grep e0 e1 e2 e3 e4 e5 e6 e7 *$ << Self-description
wait  rsp
info  Self-description:  &grep e8 e9 ea eb ec ed ee ef *$ << Self-description
wait  rsp
info  Self-description:  &grep f0 f1 f2 f3 f4 f5 f6 f7 *$ << Self-description
wait  rsp
info  Self-description:  &grep f8 f9 fa fb fc fd fe ff *$ << Self-description
wait  rsp
info  Self-description:

; } else {

10    BLOCK_READ  ` hx($MemBase + 0x3000)` 16 0  0  0  0  2
info  Self-description:  &grep 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f *$ << Self-description
wait  rsp
info  Self-description:  &grep 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3040)` 16 0  0  0  0  4
info  Self-description:  &grep 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3080)` 16 0  0  0  0  8
info  Self-description:  &grep 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f *$ << Self-description
wait  rsp
info  Self-description:  &grep 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f *$ << Self-description
wait  rsp
info  Self-description:  &grep a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af *$ << Self-description
wait  rsp
info  Self-description:  &grep b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf *$ << Self-description
wait  rsp
info  Self-description:  &grep c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf *$ << Self-description
wait  rsp
info  Self-description:  &grep d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df *$ << Self-description
wait  rsp
info  Self-description:  &grep e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef *$ << Self-description
wait  rsp
info  Self-description:  &grep f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3100)` 16 0  0  0  0  16
info  Self-description:  &grep 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f *$ << Self-description
wait  rsp
info  Self-description:  &grep 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f *$ << Self-description
wait  rsp
info  Self-description:  &grep 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f *$ << Self-description
wait  rsp
info  Self-description:  &grep a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af *$ << Self-description
wait  rsp
info  Self-description:  &grep b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf *$ << Self-description
wait  rsp
info  Self-description:  &grep c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf *$ << Self-description
wait  rsp
info  Self-description:  &grep d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df *$ << Self-description
wait  rsp
info  Self-description:  &grep e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef *$ << Self-description
wait  rsp
info  Self-description:  &grep f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff *$ << Self-description
wait  rsp
info  Self-description:

; }
; if ($DoCWF) {


//
info
info  **************************************************************
info  *        Testing Critical Word First BLOCK_READ's            *
info  **************************************************************
info

/*
delay BLOCK_READ  address    sz rt id pr pc num */
;if ($BusByteWidth == 4) {

10    BLOCK_READ  ` hx($MemBase + 0x3004)`  4 0  0  0  0  2
info  Self-description:  &grep 04 05 06 07 *$ << Self-description
wait  rsp
info  Self-description:  &grep 00 01 02 03 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3014)`  4 0  0  0  0  4
info  Self-description:  &grep 14 15 16 17 *$ << Self-description
wait  rsp
info  Self-description:  &grep 18 19 1a 1b *$ << Self-description
wait  rsp
info  Self-description:  &grep 1c 1d 1e 1f *$ << Self-description
wait  rsp
info  Self-description:  &grep 10 11 12 13 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3018)`  4 0  0  0  0  4
info  Self-description:  &grep 18 19 1a 1b *$ << Self-description
wait  rsp
info  Self-description:  &grep 1c 1d 1e 1f *$ << Self-description
wait  rsp
info  Self-description:  &grep 10 11 12 13 *$ << Self-description
wait  rsp
info  Self-description:  &grep 14 15 16 17 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x301c)`  4 0  0  0  0  4
info  Self-description:  &grep 1c 1d 1e 1f *$ << Self-description
wait  rsp
info  Self-description:  &grep 10 11 12 13 *$ << Self-description
wait  rsp
info  Self-description:  &grep 14 15 16 17 *$ << Self-description
wait  rsp
info  Self-description:  &grep 18 19 1a 1b *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3024)`  4 0  0  0  0  8
info  Self-description:  &grep 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b *$ << Self-description
wait  rsp
info  Self-description:  &grep 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 *$ << Self-description
wait  rsp
info  Self-description:  &grep 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:  &grep 38 39 3a 3b *$ << Self-description
wait  rsp
info  Self-description:  &grep 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3028)`  4 0  0  0  0  8
info  Self-description:  &grep 28 29 2a 2b *$ << Self-description
wait  rsp
info  Self-description:  &grep 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 *$ << Self-description
wait  rsp
info  Self-description:  &grep 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:  &grep 38 39 3a 3b *$ << Self-description
wait  rsp
info  Self-description:  &grep 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 *$ << Self-description
wait  rsp
info  Self-description:  &grep 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x302c)`  4 0  0  0  0  8
info  Self-description:  &grep 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 *$ << Self-description
wait  rsp
info  Self-description:  &grep 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:  &grep 38 39 3a 3b *$ << Self-description
wait  rsp
info  Self-description:  &grep 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 *$ << Self-description
wait  rsp
info  Self-description:  &grep 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3030)`  4 0  0  0  0  8
info  Self-description:  &grep 30 31 32 33 *$ << Self-description
wait  rsp
info  Self-description:  &grep 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:  &grep 38 39 3a 3b *$ << Self-description
wait  rsp
info  Self-description:  &grep 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 *$ << Self-description
wait  rsp
info  Self-description:  &grep 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b *$ << Self-description
wait  rsp
info  Self-description:  &grep 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3034)`  4 0  0  0  0  8
info  Self-description:  &grep 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:  &grep 38 39 3a 3b *$ << Self-description
wait  rsp
info  Self-description:  &grep 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 *$ << Self-description
wait  rsp
info  Self-description:  &grep 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b *$ << Self-description
wait  rsp
info  Self-description:  &grep 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3038)`  4 0  0  0  0  8
info  Self-description:  &grep 38 39 3a 3b *$ << Self-description
wait  rsp
info  Self-description:  &grep 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 *$ << Self-description
wait  rsp
info  Self-description:  &grep 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b *$ << Self-description
wait  rsp
info  Self-description:  &grep 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 *$ << Self-description
wait  rsp
info  Self-description:  &grep 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x303c)`  4 0  0  0  0  8
info  Self-description:  &grep 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 *$ << Self-description
wait  rsp
info  Self-description:  &grep 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b *$ << Self-description
wait  rsp
info  Self-description:  &grep 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 *$ << Self-description
wait  rsp
info  Self-description:  &grep 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:  &grep 38 39 3a 3b *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3044)`  4 0  0  0  0  16
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3048)`  4 0  0  0  0  16
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x304c)`  4 0  0  0  0  16
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3050)`  4 0  0  0  0  16
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3054)`  4 0  0  0  0  16
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3058)`  4 0  0  0  0  16
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x305c)`  4 0  0  0  0  16
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3060)`  4 0  0  0  0  16
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3064)`  4 0  0  0  0  16
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3068)`  4 0  0  0  0  16
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x306c)`  4 0  0  0  0  16
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3070)`  4 0  0  0  0  16
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3074)`  4 0  0  0  0  16
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3078)`  4 0  0  0  0  16
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x307c)`  4 0  0  0  0  16
info  Self-description:  &grep 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 *$ << Self-description
wait  rsp
info  Self-description:  &grep 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b *$ << Self-description
wait  rsp
info  Self-description:  &grep 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 *$ << Self-description
wait  rsp
info  Self-description:  &grep 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b *$ << Self-description
wait  rsp
info  Self-description:  &grep 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 *$ << Self-description
wait  rsp
info  Self-description:  &grep 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b *$ << Self-description
wait  rsp
info  Self-description:  &grep 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 *$ << Self-description
wait  rsp
info  Self-description:  &grep 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:  &grep 78 79 7a 7b *$ << Self-description
wait  rsp
info  Self-description:

; } elsif ($BusByteWidth == 8) {

10    BLOCK_READ  ` hx($MemBase + 0x3008)`  8 0  0  0  0  2
info  Self-description:  &grep 08 09 0a 0b 0c 0d 0e 0f *$ << Self-description
wait  rsp
info  Self-description:  &grep 00 01 02 03 04 05 06 07 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3038)`  8 0  0  0  0  4
info  Self-description:  &grep 38 39 3a 3b 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 24 25 26 27 *$ << Self-description
wait  rsp
info  Self-description:  &grep 28 29 2a 2b 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 34 35 36 37 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3078)`  8 0  0  0  0  8
info  Self-description:  &grep 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 44 45 46 47 *$ << Self-description
wait  rsp
info  Self-description:  &grep 48 49 4a 4b 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 54 55 56 57 *$ << Self-description
wait  rsp
info  Self-description:  &grep 58 59 5a 5b 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 64 65 66 67 *$ << Self-description
wait  rsp
info  Self-description:  &grep 68 69 6a 6b 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 74 75 76 77 *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x30f8)`  8 0  0  0  0  16
info  Self-description:  &grep f8 f9 fa fb fc fd fe ff *$ << Self-description
wait  rsp
info  Self-description:  &grep 80 81 82 83 84 85 86 87 *$ << Self-description
wait  rsp
info  Self-description:  &grep 88 89 8a 8b 8c 8d 8e 8f *$ << Self-description
wait  rsp
info  Self-description:  &grep 90 91 92 93 94 95 96 97 *$ << Self-description
wait  rsp
info  Self-description:  &grep 98 99 9a 9b 9c 9d 9e 9f *$ << Self-description
wait  rsp
info  Self-description:  &grep a0 a1 a2 a3 a4 a5 a6 a7 *$ << Self-description
wait  rsp
info  Self-description:  &grep a8 a9 aa ab ac ad ae af *$ << Self-description
wait  rsp
info  Self-description:  &grep b0 b1 b2 b3 b4 b5 b6 b7 *$ << Self-description
wait  rsp
info  Self-description:  &grep b8 b9 ba bb bc bd be bf *$ << Self-description
wait  rsp
info  Self-description:  &grep c0 c1 c2 c3 c4 c5 c6 c7 *$ << Self-description
wait  rsp
info  Self-description:  &grep c8 c9 ca cb cc cd ce cf *$ << Self-description
wait  rsp
info  Self-description:  &grep d0 d1 d2 d3 d4 d5 d6 d7 *$ << Self-description
wait  rsp
info  Self-description:  &grep d8 d9 da db dc dd de df *$ << Self-description
wait  rsp
info  Self-description:  &grep e0 e1 e2 e3 e4 e5 e6 e7 *$ << Self-description
wait  rsp
info  Self-description:  &grep e8 e9 ea eb ec ed ee ef *$ << Self-description
wait  rsp
info  Self-description:  &grep f0 f1 f2 f3 f4 f5 f6 f7 *$ << Self-description
wait  rsp
info  Self-description:

; } else {

10    BLOCK_READ  ` hx($MemBase + 0x3010)` 16 0  0  0  0  2
info  Self-description:  &grep 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f *$ << Self-description
wait  rsp
info  Self-description:  &grep 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x3070)` 16 0  0  0  0  4
info  Self-description:  &grep 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x30f0)` 16 0  0  0  0  8
info  Self-description:  &grep f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff *$ << Self-description
wait  rsp
info  Self-description:  &grep 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f *$ << Self-description
wait  rsp
info  Self-description:  &grep 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f *$ << Self-description
wait  rsp
info  Self-description:  &grep a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af *$ << Self-description
wait  rsp
info  Self-description:  &grep b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf *$ << Self-description
wait  rsp
info  Self-description:  &grep c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf *$ << Self-description
wait  rsp
info  Self-description:  &grep d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df *$ << Self-description
wait  rsp
info  Self-description:  &grep e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef *$ << Self-description
wait  rsp
info  Self-description:

10    BLOCK_READ  ` hx($MemBase + 0x31f0)` 16 0  0  0  0  16
info  Self-description:  &grep f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff *$ << Self-description
wait  rsp
info  Self-description:  &grep 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f *$ << Self-description
wait  rsp
info  Self-description:  &grep 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f *$ << Self-description
wait  rsp
info  Self-description:  &grep 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f *$ << Self-description
wait  rsp
info  Self-description:  &grep 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f *$ << Self-description
wait  rsp
info  Self-description:  &grep 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f *$ << Self-description
wait  rsp
info  Self-description:  &grep 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f *$ << Self-description
wait  rsp
info  Self-description:  &grep 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f *$ << Self-description
wait  rsp
info  Self-description:  &grep 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f *$ << Self-description
wait  rsp
info  Self-description:  &grep 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f *$ << Self-description
wait  rsp
info  Self-description:  &grep 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f *$ << Self-description
wait  rsp
info  Self-description:  &grep a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af *$ << Self-description
wait  rsp
info  Self-description:  &grep b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf *$ << Self-description
wait  rsp
info  Self-description:  &grep c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf *$ << Self-description
wait  rsp
info  Self-description:  &grep d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df *$ << Self-description
wait  rsp
info  Self-description:  &grep e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef *$ << Self-description
wait  rsp
info  Self-description:
; }



; }
; }
; if ($DoBurst) {


info
info  **************************************************************
info  *                Testing BURST_WRITE's                       *
info  **************************************************************
info

// Clear contents except put 1 in locations that will be overwritten with 0
now   POKE  ` hx($MemBase + 0x4000)` 16 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4010)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4020)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4030)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4040)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4050)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4060)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4070)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4080)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4090)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x40a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x40b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x40c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x40d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x40e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x40f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

now   POKE  ` hx($MemBase + 0x4100)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4110)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4120)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4130)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4140)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4150)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4160)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4170)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4180)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4190)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x41a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x41b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x41c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x41d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x41e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x41f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

now   POKE  ` hx($MemBase + 0x4200)` 16 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4210)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4220)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4230)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4240)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4250)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4260)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4270)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4280)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4290)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x42a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x42b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x42c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x42d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x42e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x42f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

now   POKE  ` hx($MemBase + 0x4300)` 16 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4310)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4320)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4330)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4340)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4350)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4360)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4370)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4380)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4390)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x43a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x43b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x43c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x43d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x43e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x43f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

now   POKE  ` hx($MemBase + 0x4400)` 16 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4410)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4420)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4430)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4440)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4450)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4460)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4470)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4480)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4490)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x44a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x44b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x44c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x44d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x44e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x44f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

now   POKE  ` hx($MemBase + 0x4500)` 16 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4510)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4520)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4530)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4540)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4550)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4560)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4570)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4580)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4590)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x45a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x45b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x45c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x45d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x45e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x45f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

now   POKE  ` hx($MemBase + 0x4600)` 16 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4610)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4620)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4630)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4640)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4650)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4660)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4670)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4680)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x4690)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x46a0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x46b0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x46c0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x46d0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x46e0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
now   POKE  ` hx($MemBase + 0x46f0)` 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

;if ($BusByteWidth == 4) {

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4000)`  4 0   1  0   0  2  1 0x000F ` hx($MemBase + 0x4000)` 0x00 0x01 0x02 0x03
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4004)`  4 0   1  0   0  2  2 0x000F ` hx($MemBase + 0x4000)` 0x04 0x05 0x06 0x07
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4000)`: 00 01 02 03 04 05 06 07 << Self-description
now   PEEK        ` hx($MemBase + 0x4000)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4008)`  4 0   2  0   0  2  1 0x000E ` hx($MemBase + 0x4009)` 0x08 0x09 0x0a 0x0b
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x400c)`  4 0   2  0   0  2  2 0x000F ` hx($MemBase + 0x4009)` 0x0c 0x0d 0x0e 0x0f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4008)`: 00 09 0a 0b 0c 0d 0e 0f << Self-description
now   PEEK        ` hx($MemBase + 0x4008)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4010)`  4 0   3  0   0  2  1 0x000C ` hx($MemBase + 0x4012)` 0x10 0x11 0x12 0x13
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4014)`  4 0   3  0   0  2  2 0x000F ` hx($MemBase + 0x4012)` 0x14 0x15 0x16 0x17
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4010)`: 00 00 12 13 14 15 16 17 << Self-description
now   PEEK        ` hx($MemBase + 0x4010)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4018)`  4 0   4  0   0  2  1 0x0008 ` hx($MemBase + 0x401b)` 0x18 0x19 0x1a 0x1b
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x401c)`  4 0   4  0   0  2  2 0x000F ` hx($MemBase + 0x401b)` 0x1c 0x1d 0x1e 0x1f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4018)`: 00 00 00 1b 1c 1d 1e 1f << Self-description
now   PEEK        ` hx($MemBase + 0x4018)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4020)`  4 0   5  0   0  2  1 0x000F ` hx($MemBase + 0x4020)` 0x20 0x21 0x22 0x23
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4024)`  4 0   5  0   0  2  2 0x0007 ` hx($MemBase + 0x4020)` 0x24 0x25 0x26 0x27
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4020)`: 20 21 22 23 24 25 26 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4020)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4028)`  4 0   6  0   0  2  1 0x000F ` hx($MemBase + 0x4028)` 0x28 0x29 0x2a 0x2b
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x402c)`  4 0   6  0   0  2  2 0x0003 ` hx($MemBase + 0x4028)` 0x2c 0x2d 0x2e 0x2f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4028)`: 28 29 2a 2b 2c 2d 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4028)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4030)`  4 0   7  0   0  2  1 0x000F ` hx($MemBase + 0x4030)` 0x30 0x31 0x32 0x33
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4034)`  4 0   7  0   0  2  2 0x0001 ` hx($MemBase + 0x4030)` 0x34 0x35 0x36 0x37
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4030)`: 30 31 32 33 34 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4030)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x403c)`  4 0   8  0   0  2  1 0x000E ` hx($MemBase + 0x403d)` 0x3c 0x3d 0x3e 0x3f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4040)`  4 0   8  0   0  2  2 0x0007 ` hx($MemBase + 0x403d)` 0x40 0x41 0x42 0x43
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x403c)`: 00 3d 3e 3f 40 41 42 00 << Self-description
now   PEEK        ` hx($MemBase + 0x403c)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4044)`  4 0   9  0   0  2  1 0x000C ` hx($MemBase + 0x4046)` 0x44 0x45 0x46 0x47
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4048)`  4 0   9  0   0  2  2 0x0003 ` hx($MemBase + 0x4046)` 0x48 0x49 0x4a 0x4b
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4044)`: 00 00 46 47 48 49 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4044)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x404c)`  4 0  10  0   0  2  1 0x0008 ` hx($MemBase + 0x404f)` 0x4c 0x4d 0x4e 0x4f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4050)`  4 0  10  0   0  2  2 0x0001 ` hx($MemBase + 0x404f)` 0x50 0x51 0x52 0x53
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x404c)`: 00 00 00 4f 50 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x404c)` 8
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4054)`  4 0  11  0   0  3  1 0x0008 ` hx($MemBase + 0x4057)` 0x54 0x55 0x56 0x57
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4058)`  4 0  11  0   0  3  2 0x000F ` hx($MemBase + 0x4057)` 0x58 0x59 0x5a 0x5b
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x405c)`  4 0  11  0   0  3  3 0x0007 ` hx($MemBase + 0x4057)` 0x5c 0x5d 0x5e 0x5f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4054)`: 00 00 00 57 58 59 5a 5b 5c 5d 5e 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4054)` 12
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4060)`  4 0  12  0   0  4  1 0x000C ` hx($MemBase + 0x4062)` 0x60 0x61 0x62 0x63
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4064)`  4 0  12  0   0  4  2 0x000F ` hx($MemBase + 0x4062)` 0x64 0x65 0x66 0x67
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4068)`  4 0  12  0   0  4  3 0x000F ` hx($MemBase + 0x4062)` 0x68 0x69 0x6a 0x6b
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x406c)`  4 0  12  0   0  4  4 0x0003 ` hx($MemBase + 0x4062)` 0x6c 0x6d 0x6e 0x6f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4060)`: 00 00 62 63 64 65 66 67 68 69 6a 6b 6c 6d 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4060)` 16
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4070)`  4 0  13  0   0  5  1 0x000E ` hx($MemBase + 0x4071)` 0x70 0x71 0x72 0x73
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4074)`  4 0  13  0   0  5  2 0x000F ` hx($MemBase + 0x4071)` 0x74 0x75 0x76 0x77
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4078)`  4 0  13  0   0  5  3 0x000F ` hx($MemBase + 0x4071)` 0x78 0x79 0x7a 0x7b
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x407c)`  4 0  13  0   0  5  4 0x000F ` hx($MemBase + 0x4071)` 0x7c 0x7d 0x7e 0x7f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4080)`  4 0  13  0   0  5  5 0x0001 ` hx($MemBase + 0x4071)` 0x80 0x81 0x82 0x83
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4070)`: 00 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 80 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4070)` 20
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x4084)`  4 0  14  0   0  6  1 0x000E ` hx($MemBase + 0x4085)` 0x84 0x85 0x86 0x87
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4088)`  4 0  14  0   0  6  2 0x000F ` hx($MemBase + 0x4085)` 0x88 0x89 0x8a 0x8b
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x408c)`  4 0  14  0   0  6  3 0x000F ` hx($MemBase + 0x4085)` 0x8c 0x8d 0x8e 0x8f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4090)`  4 0  14  0   0  6  4 0x000F ` hx($MemBase + 0x4085)` 0x90 0x91 0x92 0x93
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4094)`  4 0  14  0   0  6  5 0x000F ` hx($MemBase + 0x4085)` 0x94 0x95 0x96 0x97
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4098)`  4 0  14  0   0  6  6 0x000F ` hx($MemBase + 0x4085)` 0x98 0x99 0x9a 0x9b
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4084)`: 00 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b << Self-description
now   PEEK        ` hx($MemBase + 0x4084)` 24
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x409c)`  4 0  15  0   0  7  1 0x000F ` hx($MemBase + 0x409c)` 0x9c 0x9d 0x9e 0x9f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40a0)`  4 0  15  0   0  7  2 0x000F ` hx($MemBase + 0x409c)` 0xa0 0xa1 0xa2 0xa3
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40a4)`  4 0  15  0   0  7  3 0x000F ` hx($MemBase + 0x409c)` 0xa4 0xa5 0xa6 0xa7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40a8)`  4 0  15  0   0  7  4 0x000F ` hx($MemBase + 0x409c)` 0xa8 0xa9 0xaa 0xab
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40ac)`  4 0  15  0   0  7  5 0x000F ` hx($MemBase + 0x409c)` 0xac 0xad 0xae 0xaf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40b0)`  4 0  15  0   0  7  6 0x000F ` hx($MemBase + 0x409c)` 0xb0 0xb1 0xb2 0xb3
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40b4)`  4 0  15  0   0  7  7 0x0001 ` hx($MemBase + 0x409c)` 0xb4 0xb5 0xb6 0xb7
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x409c)`: 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x409c)` 28
info  Self-description:

$(SYNC)
1     BURST_WRITE ` hx($MemBase + 0x40b8)`  4 0  16  0   0  8  1 0x0008 ` hx($MemBase + 0x40b8)` 0xb8 0xb9 0xba 0xbb
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40bc)`  4 0  16  0   0  8  2 0x000F ` hx($MemBase + 0x40b8)` 0xbc 0xbd 0xbe 0xbf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40c0)`  4 0  16  0   0  8  3 0x000F ` hx($MemBase + 0x40b8)` 0xc0 0xc1 0xc2 0xc3
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40c4)`  4 0  16  0   0  8  4 0x000F ` hx($MemBase + 0x40b8)` 0xc4 0xc5 0xc6 0xc7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40c8)`  4 0  16  0   0  8  5 0x000F ` hx($MemBase + 0x40b8)` 0xc8 0xc9 0xca 0xcb
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40cc)`  4 0  16  0   0  8  6 0x000F ` hx($MemBase + 0x40b8)` 0xcc 0xcd 0xce 0xcf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40d0)`  4 0  16  0   0  8  7 0x000F ` hx($MemBase + 0x40b8)` 0xd0 0xd1 0xd2 0xd3
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40d4)`  4 0  16  0   0  8  8 0x0007 ` hx($MemBase + 0x40b8)` 0xd4 0xd5 0xd6 0xd7
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40b8)`: 00 00 00 bb bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 00 << Self-description
now   PEEK        ` hx($MemBase + 0x40b8)` 32
info  Self-description:

; } elsif ($BusByteWidth == 8) {

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4000)`  8 0   17 0   0  2  1 0x00FF ` hx($MemBase + 0x4000)` 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4008)`  8 0   17 0   0  2  2 0x00FF ` hx($MemBase + 0x4000)` 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4000)`: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
now   PEEK        ` hx($MemBase + 0x4000)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4010)`  8 0   18 0   0  2  1 0x00FE ` hx($MemBase + 0x4011)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4018)`  8 0   18 0   0  2  2 0x00FF ` hx($MemBase + 0x4011)` 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4010)`: 00 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
now   PEEK        ` hx($MemBase + 0x4010)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4020)`  8 0   19 0   0  2  1 0x00FC ` hx($MemBase + 0x4022)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4028)`  8 0   19 0   0  2  2 0x00FF ` hx($MemBase + 0x4022)` 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4020)`: 00 00 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
now   PEEK        ` hx($MemBase + 0x4020)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4030)`  8 0   20 0   0  2  1 0x00F8 ` hx($MemBase + 0x4033)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4038)`  8 0   20 0   0  2  2 0x00FF ` hx($MemBase + 0x4033)` 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4030)`: 00 00 00 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
now   PEEK        ` hx($MemBase + 0x4030)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4040)`  8 0   21 0   0  2  1 0x00F0 ` hx($MemBase + 0x4044)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4048)`  8 0   21 0   0  2  2 0x00FF ` hx($MemBase + 0x4044)` 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4040)`: 00 00 00 00 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f << Self-description
now   PEEK        ` hx($MemBase + 0x4040)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4050)`  8 0   22 0   0  2  1 0x00E0 ` hx($MemBase + 0x4055)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4058)`  8 0   22 0   0  2  2 0x00FF ` hx($MemBase + 0x4055)` 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4050)`: 00 00 00 00 00 55 56 57 58 59 5a 5b 5c 5d 5e 5f << Self-description
now   PEEK        ` hx($MemBase + 0x4050)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4060)`  8 0   23 0   0  2  1 0x00C0 ` hx($MemBase + 0x4066)` 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4068)`  8 0   23 0   0  2  2 0x00FF ` hx($MemBase + 0x4066)` 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4060)`: 00 00 00 00 00 00 66 67 68 69 6a 6b 6c 6d 6e 6f << Self-description
now   PEEK        ` hx($MemBase + 0x4060)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4070)`  8 0   24 0   0  2  1 0x0080 ` hx($MemBase + 0x4077)` 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4078)`  8 0   24 0   0  2  2 0x00FF ` hx($MemBase + 0x4077)` 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4070)`: 00 00 00 00 00 00 00 77 78 79 7a 7b 7c 7d 7e 7f << Self-description
now   PEEK        ` hx($MemBase + 0x4070)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4080)`  8 0   25 0   0  2  1 0x00FF ` hx($MemBase + 0x4080)` 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4088)`  8 0   25 0   0  2  2 0x007F ` hx($MemBase + 0x4080)` 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4080)`: 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4080)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4090)`  8 0   26 0   0  2  1 0x00FF ` hx($MemBase + 0x4090)` 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4098)`  8 0   26 0   0  2  2 0x003F ` hx($MemBase + 0x4090)` 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4090)`: 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4090)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x40a0)`  8 0   27 0   0  2  1 0x00FF ` hx($MemBase + 0x40a0)` 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40a8)`  8 0   27 0   0  2  2 0x001F ` hx($MemBase + 0x40a0)` 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40a0)`: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x40a0)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x40b0)`  8 0   28 0   0  2  1 0x00FF ` hx($MemBase + 0x40b0)` 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40b8)`  8 0   28 0   0  2  2 0x000F ` hx($MemBase + 0x40b0)` 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40b0)`: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x40b0)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x40c0)`  8 0   29 0   0  2  1 0x00FF ` hx($MemBase + 0x40c0)` 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40c8)`  8 0   29 0   0  2  2 0x0007 ` hx($MemBase + 0x40c0)` 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40c0)`: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x40c0)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x40d0)`  8 0   30 0   0  2  1 0x00FF ` hx($MemBase + 0x40d0)` 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40d8)`  8 0   30 0   0  2  2 0x0003 ` hx($MemBase + 0x40d0)` 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40d0)`: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x40d0)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x40e0)`  8 0   31 0   0  2  1 0x00FF ` hx($MemBase + 0x40e0)` 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40e8)`  8 0   31 0   0  2  2 0x0001 ` hx($MemBase + 0x40e0)` 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40e0)`: e0 e1 e2 e3 e4 e5 e6 e7 e8 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x40e0)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4108)`  8 0   32 0   0  2  1 0x00FE ` hx($MemBase + 0x4109)` 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4110)`  8 0   32 0   0  2  2 0x007F ` hx($MemBase + 0x4109)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4108)`: 00 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4108)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4118)`  8 0   33 0   0  2  1 0x00F0 ` hx($MemBase + 0x411c)` 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4120)`  8 0   33 0   0  2  2 0x000F ` hx($MemBase + 0x411c)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4118)`: 00 00 00 00 1c 1d 1e 1f 20 21 22 23 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4118)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4128)`  8 0   34 0   0  2  1 0x0080 ` hx($MemBase + 0x412f)` 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4130)`  8 0   34 0   0  2  2 0x0001 ` hx($MemBase + 0x412f)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4128)`: 00 00 00 00 00 00 00 2f 30 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4128)` 16
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4138)`  8 0   35 0   0  3  1 0x00F0 ` hx($MemBase + 0x413c)` 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4140)`  8 0   35 0   0  3  2 0x00FF ` hx($MemBase + 0x413c)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4148)`  8 0   35 0   0  3  3 0x003F ` hx($MemBase + 0x413c)` 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4138)`: 00 00 00 00 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4138)` 24
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4150)`  8 0   36 0   0  4  1 0x00FC ` hx($MemBase + 0x4152)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4158)`  8 0   36 0   0  4  2 0x00FF ` hx($MemBase + 0x4152)` 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4160)`  8 0   36 0   0  4  3 0x00FF ` hx($MemBase + 0x4152)` 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4168)`  8 0   36 0   0  4  4 0x0003 ` hx($MemBase + 0x4152)` 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4150)`: 00 00 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4150)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4170)`  8 0   37 0   0  5  1 0x00E0 ` hx($MemBase + 0x4175)` 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4178)`  8 0   37 0   0  5  2 0x00FF ` hx($MemBase + 0x4175)` 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4180)`  8 0   37 0   0  5  3 0x00FF ` hx($MemBase + 0x4175)` 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4188)`  8 0   37 0   0  5  4 0x00FF ` hx($MemBase + 0x4175)` 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4190)`  8 0   37 0   0  5  5 0x007F ` hx($MemBase + 0x4175)` 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4170)`: 00 00 00 00 00 75 76 77 78 79 7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4170)` 40
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4198)`  8 0   38 0   0  6  1 0x00F8 ` hx($MemBase + 0x419b)` 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41a0)`  8 0   38 0   0  6  2 0x00FF ` hx($MemBase + 0x419b)` 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41a8)`  8 0   38 0   0  6  3 0x00FF ` hx($MemBase + 0x419b)` 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41b0)`  8 0   38 0   0  6  4 0x00FF ` hx($MemBase + 0x419b)` 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41b8)`  8 0   38 0   0  6  5 0x00FF ` hx($MemBase + 0x419b)` 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41c0)`  8 0   38 0   0  6  6 0x001F ` hx($MemBase + 0x419b)` 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4198)`: 00 00 00 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf c0 c1 c2 c3 c4 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4198)` 48
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x41c8)`  8 0   39 0   0  7  1 0x00C0 ` hx($MemBase + 0x41ce)` 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41d0)`  8 0   39 0   0  7  2 0x00FF ` hx($MemBase + 0x41ce)` 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41d8)`  8 0   39 0   0  7  3 0x00FF ` hx($MemBase + 0x41ce)` 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41e0)`  8 0   39 0   0  7  4 0x00FF ` hx($MemBase + 0x41ce)` 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41e8)`  8 0   39 0   0  7  5 0x00FF ` hx($MemBase + 0x41ce)` 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41f0)`  8 0   39 0   0  7  6 0x00FF ` hx($MemBase + 0x41ce)` 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41f8)`  8 0   39 0   0  7  7 0x00FF ` hx($MemBase + 0x41ce)` 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x41c8)`: 00 00 00 00 00 00 ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
now   PEEK        ` hx($MemBase + 0x41c8)` 56
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4208)`  8 0   40 0   0  8  1 0x00F8 ` hx($MemBase + 0x420b)` 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4210)`  8 0   40 0   0  8  2 0x00FF ` hx($MemBase + 0x420b)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4218)`  8 0   40 0   0  8  3 0x00FF ` hx($MemBase + 0x420b)` 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4220)`  8 0   40 0   0  8  4 0x00FF ` hx($MemBase + 0x420b)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4228)`  8 0   40 0   0  8  5 0x00FF ` hx($MemBase + 0x420b)` 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4230)`  8 0   40 0   0  8  6 0x00FF ` hx($MemBase + 0x420b)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4238)`  8 0   40 0   0  8  7 0x00FF ` hx($MemBase + 0x420b)` 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4240)`  8 0   40 0   0  8  8 0x003F ` hx($MemBase + 0x420b)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4208)`: 00 00 00 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4208)` 64
info  Self-description:

; } else {

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4000)` 16 0   41 0   0  2  1 0xFFFF ` hx($MemBase + 0x4000)` 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4010)` 16 0   41 0   0  2  2 0xFFFF ` hx($MemBase + 0x4000)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4000)`: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
now   PEEK        ` hx($MemBase + 0x4000)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4020)` 16 0   42 0   0  2  1 0xFFFE ` hx($MemBase + 0x4021)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4030)` 16 0   42 0   0  2  2 0xFFFF ` hx($MemBase + 0x4020)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4020)`: 00 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
now   PEEK        ` hx($MemBase + 0x4020)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4040)` 16 0   43 0   0  2  1 0xFFFC ` hx($MemBase + 0x4042)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4050)` 16 0   43 0   0  2  2 0xFFFF ` hx($MemBase + 0x4042)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4040)`: 00 00 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f << Self-description
now   PEEK        ` hx($MemBase + 0x4040)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4060)` 16 0   44 0   0  2  1 0xFFF8 ` hx($MemBase + 0x4063)` 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4070)` 16 0   44 0   0  2  2 0xFFFF ` hx($MemBase + 0x4063)` 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4060)`: 00 00 00 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f << Self-description
now   PEEK        ` hx($MemBase + 0x4060)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4080)` 16 0   45 0   0  2  1 0xFFF0 ` hx($MemBase + 0x4063)` 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4090)` 16 0   45 0   0  2  2 0xFFFF ` hx($MemBase + 0x4063)` 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4080)`: 00 00 00 00 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f << Self-description
now   PEEK        ` hx($MemBase + 0x4080)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x40a0)` 16 0   46 0   0  2  1 0xFFE0 ` hx($MemBase + 0x40a5)` 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40b0)` 16 0   46 0   0  2  2 0xFFFF ` hx($MemBase + 0x40a5)` 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40a0)`: 00 00 00 00 00 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
now   PEEK        ` hx($MemBase + 0x40a0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x40c0)` 16 0   47 0   0  2  1 0xFFC0 ` hx($MemBase + 0x40c6)` 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40d0)` 16 0   47 0   0  2  2 0xFFFF ` hx($MemBase + 0x40c6)` 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40c0)`: 00 00 00 00 00 00 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
now   PEEK        ` hx($MemBase + 0x40c0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x40e0)` 16 0   48 0   0  2  1 0xFF80 ` hx($MemBase + 0x40e7)` 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x40f0)` 16 0   48 0   0  2  2 0xFFFF ` hx($MemBase + 0x40e7)` 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x40e0)`: 00 00 00 00 00 00 00 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
now   PEEK        ` hx($MemBase + 0x40e0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4100)` 16 0   49 0   0  2  1 0xFF00 ` hx($MemBase + 0x4108)` 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4110)` 16 0   49 0   0  2  2 0xFFFF ` hx($MemBase + 0x4108)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4100)`: 00 00 00 00 00 00 00 00 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
now   PEEK        ` hx($MemBase + 0x4100)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4120)` 16 0   50 0   0  2  1 0xFE00 ` hx($MemBase + 0x4129)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4130)` 16 0   50 0   0  2  2 0xFFFF ` hx($MemBase + 0x4129)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4120)`: 00 00 00 00 00 00 00 00 00 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
now   PEEK        ` hx($MemBase + 0x4120)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4140)` 16 0   51 0   0  2  1 0xFC00 ` hx($MemBase + 0x414a)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4150)` 16 0   51 0   0  2  2 0xFFFF ` hx($MemBase + 0x414a)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4140)`: 00 00 00 00 00 00 00 00 00 00 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f << Self-description
now   PEEK        ` hx($MemBase + 0x4140)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4160)` 16 0   52 0   0  2  1 0xF800 ` hx($MemBase + 0x416b)` 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4170)` 16 0   52 0   0  2  2 0xFFFF ` hx($MemBase + 0x416b)` 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4160)`: 00 00 00 00 00 00 00 00 00 00 00 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f << Self-description
now   PEEK        ` hx($MemBase + 0x4160)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4180)` 16 0   53 0   0  2  1 0xF000 ` hx($MemBase + 0x418c)` 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4190)` 16 0   53 0   0  2  2 0xFFFF ` hx($MemBase + 0x418c)` 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4180)`: 00 00 00 00 00 00 00 00 00 00 00 00 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f << Self-description
now   PEEK        ` hx($MemBase + 0x4180)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x41a0)` 16 0   54 0   0  2  1 0xE000 ` hx($MemBase + 0x41ad)` 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41b0)` 16 0   54 0   0  2  2 0xFFFF ` hx($MemBase + 0x41ad)` 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x41a0)`: 00 00 00 00 00 00 00 00 00 00 00 00 00 ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
now   PEEK        ` hx($MemBase + 0x41a0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x41c0)` 16 0   55 0   0  2  1 0xC000 ` hx($MemBase + 0x41ce)` 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41d0)` 16 0   55 0   0  2  2 0xFFFF ` hx($MemBase + 0x41ce)` 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x41c0)`: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
now   PEEK        ` hx($MemBase + 0x41c0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x41e0)` 16 0   56 0   0  2  1 0x8000 ` hx($MemBase + 0x41ef)` 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x41f0)` 16 0   56 0   0  2  2 0xFFFF ` hx($MemBase + 0x41ef)` 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x41e0)`: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
now   PEEK        ` hx($MemBase + 0x41e0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4200)` 16 0   57 0   0  2  1 0xFFFF ` hx($MemBase + 0x4200)` 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4210)` 16 0   57 0   0  2  2 0x7FFF ` hx($MemBase + 0x4200)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4200)`: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4200)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4220)` 16 0   58 0   0  2  1 0xFFFF ` hx($MemBase + 0x4220)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4230)` 16 0   58 0   0  2  2 0x3FFF ` hx($MemBase + 0x4220)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4220)`: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4220)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4240)` 16 0   59 0   0  2  1 0xFFFF ` hx($MemBase + 0x4240)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4250)` 16 0   59 0   0  2  2 0x1FFF ` hx($MemBase + 0x4240)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4240)`: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4240)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4260)` 16 0   60 0   0  2  1 0xFFFF ` hx($MemBase + 0x4260)` 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4270)` 16 0   60 0   0  2  2 0x0FFF ` hx($MemBase + 0x4260)` 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4260)`: 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4260)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4280)` 16 0   61 0   0  2  1 0xFFFF ` hx($MemBase + 0x4280)` 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4290)` 16 0   61 0   0  2  2 0x07FF ` hx($MemBase + 0x4280)` 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4280)`: 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4280)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x42a0)` 16 0   62 0   0  2  1 0xFFFF ` hx($MemBase + 0x42a0)` 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x42b0)` 16 0   62 0   0  2  2 0x03FF ` hx($MemBase + 0x42a0)` 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x42a0)`: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x42a0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x42c0)` 16 0   63 0   0  2  1 0xFFFF ` hx($MemBase + 0x42c0)` 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x42d0)` 16 0   63 0   0  2  2 0x01FF ` hx($MemBase + 0x42c0)` 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x42c0)`: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x42c0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x42e0)` 16 0    0 0   0  2  1 0xFFFF ` hx($MemBase + 0x42e0)` 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x42f0)` 16 0    0 0   0  2  2 0x00FF ` hx($MemBase + 0x42e0)` 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x42e0)`: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x42e0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4300)` 16 0    1 0   0  2  1 0xFFFF ` hx($MemBase + 0x4300)` 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4310)` 16 0    1 0   0  2  2 0x007F ` hx($MemBase + 0x4300)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4300)`: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4300)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4320)` 16 0    2 0   0  2  1 0xFFFF ` hx($MemBase + 0x4320)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4330)` 16 0    2 0   0  2  2 0x003F ` hx($MemBase + 0x4320)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4320)`: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4320)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4340)` 16 0    3 0   0  2  1 0xFFFF ` hx($MemBase + 0x4340)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4350)` 16 0    3 0   0  2  2 0x001F ` hx($MemBase + 0x4340)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4340)`: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4340)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4360)` 16 0    4 0   0  2  1 0xFFFF ` hx($MemBase + 0x4360)` 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4370)` 16 0    4 0   0  2  2 0x000F ` hx($MemBase + 0x4360)` 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4360)`: 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 00 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4360)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4380)` 16 0    5 0   0  2  1 0xFFFF ` hx($MemBase + 0x4380)` 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4390)` 16 0    5 0   0  2  2 0x0007 ` hx($MemBase + 0x4380)` 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4380)`: 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 00 00 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4380)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x43a0)` 16 0    6 0   0  2  1 0xFFFF ` hx($MemBase + 0x43a0)` 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x43b0)` 16 0    6 0   0  2  2 0x0003 ` hx($MemBase + 0x43a0)` 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x43a0)`: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 00 00 00 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x43a0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x43c0)` 16 0    7 0   0  2  1 0xFFFF ` hx($MemBase + 0x43c0)` 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x43d0)` 16 0    7 0   0  2  2 0x0001 ` hx($MemBase + 0x43c0)` 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x43c0)`: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x43c0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x43f0)` 16 0    8 0   0  2  1 0xFFFE ` hx($MemBase + 0x43f0)` 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4400)` 16 0    8 0   0  2  2 0x7FFF ` hx($MemBase + 0x43f0)` 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x43f0)`: 00 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 00 << Self-description
now   PEEK        ` hx($MemBase + 0x43f0)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4410)` 16 0    9 0   0  2  1 0xFF00 ` hx($MemBase + 0x4418)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4420)` 16 0    9 0   0  2  2 0x00FF ` hx($MemBase + 0x4418)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4410)`: 00 00 00 00 00 00 00 00 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4410)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4430)` 16 0   10 0   0  2  1 0x8000 ` hx($MemBase + 0x4438)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4440)` 16 0   10 0   0  2  2 0x0001 ` hx($MemBase + 0x4438)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4430)`: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3f 40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4430)` 32
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4450)` 16 0   11 0   0  3  1 0xFFF0 ` hx($MemBase + 0x4451)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4460)` 16 0   11 0   0  3  2 0xFFFF ` hx($MemBase + 0x4451)` 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4470)` 16 0   11 0   0  3  3 0x07FF ` hx($MemBase + 0x4451)` 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4450)`: 00 00 00 00 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4450)` 48
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4480)` 16 0   12 0   0  4  1 0xFFC0 ` hx($MemBase + 0x4486)` 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4490)` 16 0   12 0   0  4  2 0xFFFF ` hx($MemBase + 0x4486)` 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x44a0)` 16 0   12 0   0  4  3 0xFFFF ` hx($MemBase + 0x4486)` 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x44b0)` 16 0   12 0   0  4  4 0x01FF ` hx($MemBase + 0x4486)` 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4480)`: 00 00 00 00 00 00 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4480)` 64
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x44c0)` 16 0   13 0   0  5  1 0xFE00 ` hx($MemBase + 0x44c9)` 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x44d0)` 16 0   13 0   0  5  2 0xFFFF ` hx($MemBase + 0x44c9)` 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x44e0)` 16 0   13 0   0  5  3 0xFFFF ` hx($MemBase + 0x44c9)` 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x44f0)` 16 0   13 0   0  5  4 0xFFFF ` hx($MemBase + 0x44c9)` 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4500)` 16 0   13 0   0  5  5 0x003F ` hx($MemBase + 0x44c9)` 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x44c0)`: 00 00 00 00 00 00 00 00 00 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff 00 01 02 03 04 05 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x44c0)` 80
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4510)` 16 0   14 0   0  6  1 0xF800 ` hx($MemBase + 0x451a)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4520)` 16 0   14 0   0  6  2 0xFFFF ` hx($MemBase + 0x451a)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4530)` 16 0   14 0   0  6  3 0xFFFF ` hx($MemBase + 0x451a)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4540)` 16 0   14 0   0  6  4 0xFFFF ` hx($MemBase + 0x451a)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4550)` 16 0   14 0   0  6  5 0xFFFF ` hx($MemBase + 0x451a)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4560)` 16 0   14 0   0  6  6 0x001F ` hx($MemBase + 0x451a)` 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4510)`: 00 00 00 00 00 00 00 00 00 00 00 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4510)` 96
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x4570)` 16 0   15 0   0  7  1 0xF000 ` hx($MemBase + 0x457c)` 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4580)` 16 0   15 0   0  7  2 0xFFFF ` hx($MemBase + 0x457c)` 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4590)` 16 0   15 0   0  7  3 0xFFFF ` hx($MemBase + 0x457c)` 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x45a0)` 16 0   15 0   0  7  4 0xFFFF ` hx($MemBase + 0x457c)` 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x45b0)` 16 0   15 0   0  7  5 0xFFFF ` hx($MemBase + 0x457c)` 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x45c0)` 16 0   15 0   0  7  6 0xFFFF ` hx($MemBase + 0x457c)` 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x45d0)` 16 0   15 0   0  7  7 0x0007 ` hx($MemBase + 0x457c)` 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x4570)`: 00 00 00 00 00 00 00 00 00 00 00 00 7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 00 00 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x4570)` 112
info  Self-description:

$(SYNC)
/*
delay BURST_WRITE address                  sz rid id pri pc xf i bytes  hw_address               b0   b1   ...  bN */
1     BURST_WRITE ` hx($MemBase + 0x45e0)` 16 0   16 0   0  8  1 0xC000 ` hx($MemBase + 0x45ee)` 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x45f0)` 16 0   16 0   0  8  2 0xFFFF ` hx($MemBase + 0x45ee)` 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4600)` 16 0   16 0   0  8  3 0xFFFF ` hx($MemBase + 0x45ee)` 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4610)` 16 0   16 0   0  8  4 0xFFFF ` hx($MemBase + 0x45ee)` 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4620)` 16 0   16 0   0  8  5 0xFFFF ` hx($MemBase + 0x45ee)` 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4630)` 16 0   16 0   0  8  6 0xFFFF ` hx($MemBase + 0x45ee)` 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4640)` 16 0   16 0   0  8  7 0xFFFF ` hx($MemBase + 0x45ee)` 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
wait nacc
now   BURST_WRITE ` hx($MemBase + 0x4650)` 16 0   16 0   0  8  8 0x0001 ` hx($MemBase + 0x45ee)` 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f
wait rsp
$(WAIT)
info  Self-description:  &grep master.*peek ` hx($MemBase + 0x45e0)`: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 << Self-description
now   PEEK        ` hx($MemBase + 0x45e0)` 128
info  Self-description:

; }
info  ---------------------------------------------------------------------------------


info
info  **************************************************************
info  *                Testing BURST_READ's                        *
info  **************************************************************
info

// Preload memory contents with known values
now   POKE ` hx($MemBase + 0x5000)` 16 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f 
now   POKE ` hx($MemBase + 0x5010)` 16 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f 
now   POKE ` hx($MemBase + 0x5020)` 16 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f 
now   POKE ` hx($MemBase + 0x5030)` 16 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f 
now   POKE ` hx($MemBase + 0x5040)` 16 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f 
now   POKE ` hx($MemBase + 0x5050)` 16 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f 
now   POKE ` hx($MemBase + 0x5060)` 16 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f 
now   POKE ` hx($MemBase + 0x5070)` 16 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f 
now   POKE ` hx($MemBase + 0x5080)` 16 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 
now   POKE ` hx($MemBase + 0x5090)` 16 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 
now   POKE ` hx($MemBase + 0x50a0)` 16 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf 
now   POKE ` hx($MemBase + 0x50b0)` 16 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf 
now   POKE ` hx($MemBase + 0x50c0)` 16 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf 
now   POKE ` hx($MemBase + 0x50d0)` 16 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 
now   POKE ` hx($MemBase + 0x50e0)` 16 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 
now   POKE ` hx($MemBase + 0x50f0)` 16 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff 

now   POKE ` hx($MemBase + 0x5100)` 16 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f 
now   POKE ` hx($MemBase + 0x5110)` 16 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f 
now   POKE ` hx($MemBase + 0x5120)` 16 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f 
now   POKE ` hx($MemBase + 0x5130)` 16 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f 
now   POKE ` hx($MemBase + 0x5140)` 16 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f 
now   POKE ` hx($MemBase + 0x5150)` 16 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5a 0x5b 0x5c 0x5d 0x5e 0x5f 
now   POKE ` hx($MemBase + 0x5160)` 16 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6a 0x6b 0x6c 0x6d 0x6e 0x6f 
now   POKE ` hx($MemBase + 0x5170)` 16 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7a 0x7b 0x7c 0x7d 0x7e 0x7f 
now   POKE ` hx($MemBase + 0x5180)` 16 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 
now   POKE ` hx($MemBase + 0x5190)` 16 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 
now   POKE ` hx($MemBase + 0x51a0)` 16 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 0xaf 
now   POKE ` hx($MemBase + 0x51b0)` 16 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 0xbd 0xbe 0xbf 
now   POKE ` hx($MemBase + 0x51c0)` 16 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 0xcb 0xcc 0xcd 0xce 0xcf 
now   POKE ` hx($MemBase + 0x51d0)` 16 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 
now   POKE ` hx($MemBase + 0x51e0)` 16 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 
now   POKE ` hx($MemBase + 0x51f0)` 16 0xf0 0xf1 0xf2 0xf3 0xf4 0xf5 0xf6 0xf7 0xf8 0xf9 0xfa 0xfb 0xfc 0xfd 0xfe 0xff 

;if ($BusByteWidth == 4) {

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50fc)`  4 0   1  0   0  2
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  4 0   2  0   0  2
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50fc)`  4 0   3  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  4 0   4  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f4)`  4 0   5  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50fc)`  4 0   6  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  4 0   7  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f4)`  4 0   8  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  4 0   9  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50fc)`  4 0   10 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  4 0   11 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f4)`  4 0   12 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  4 0   13 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50ec)`  4 0   14 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50fc)`  4 0   15 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  4 0   16 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f4)`  4 0   17 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  4 0   18 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50ec)`  4 0   19 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  4 0   20 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50fc)`  4 0   21 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 14 15 16 17 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  4 0   22 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f4)`  4 0   23 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  4 0   24 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50ec)`  4 0   25 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  4 0   26 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e4)`  4 0   27 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50fc)`  4 0   28 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  4 0   29 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 14 15 16 17 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f4)`  4 0   30 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  4 0   31 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50ec)`  4 0   32 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  4 0   32 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e4)`  4 0   33 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)`  4 0   34 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: fc fd fe ff << Self-description
wait rsp
info  Self-description:

; } elsif ($BusByteWidth == 8) {

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  8 0   1  0   0  2
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  8 0   2  0   0  2
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  8 0   3  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  8 0   4  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  8 0   5  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  8 0   6  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  8 0   7  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  8 0   8  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)`  8 0   9  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  8 0   10 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  8 0   11 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  8 0   12 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)`  8 0   13 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d8)`  8 0   14 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  8 0   15 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  8 0   16 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  8 0   17 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)`  8 0   18 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d8)`  8 0   19 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)`  8 0   20 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  8 0   21 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  8 0   22 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  8 0   23 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)`  8 0   24 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d8)`  8 0   25 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)`  8 0   26 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50c8)`  8 0   26 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f8)`  8 0   28 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)`  8 0   29 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e8)`  8 0   30 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)`  8 0   31 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d8)`  8 0   32 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)`  8 0   33 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50c8)`  8 0   33 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50c0)`  8 0   34 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

; } else {

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)` 16 0   1  0   0  2
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)` 16 0   2  0   0  2
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)` 16 0   3  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)` 16 0   4  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)` 16 0   5  0   0  3
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)` 16 0   6  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)` 16 0   7  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)` 16 0   8  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50c0)` 16 0   9  0   0  4
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)` 16 0   10 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)` 16 0   11 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)` 16 0   12 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50c0)` 16 0   13 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50b0)` 16 0   14 0   0  5
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)` 16 0   15 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)` 16 0   16 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)` 16 0   17 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50c0)` 16 0   18 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50b0)` 16 0   19 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50a0)` 16 0   20 0   0  6
info  Self-description:  &grep master.*RSP_OK.*: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)` 16 0   21 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)` 16 0   22 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)` 16 0   23 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50c0)` 16 0   24 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50b0)` 16 0   25 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50a0)` 16 0   26 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x5090)` 16 0   27 0   0  7
info  Self-description:  &grep master.*RSP_OK.*: 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50f0)` 16 0   28 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50e0)` 16 0   29 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50d0)` 16 0   30 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50c0)` 16 0   31 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50b0)` 16 0   32 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x50a0)` 16 0   33 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x5090)` 16 0   34 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f << Self-description
wait rsp
info  Self-description:

$(SYNC)
/*
delay BURST_READ  address                  sz rid id pri pc num_xfers  */
1     BURST_READ  ` hx($MemBase + 0x5080)` 16 0   35 0   0  8
info  Self-description:  &grep master.*RSP_OK.*: 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef << Self-description
wait rsp
info  Self-description:  &grep master.*RSP_OK.*: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff << Self-description
wait rsp
info  Self-description:

; }

; }
wait 10
info End-Of-Script
now stop

