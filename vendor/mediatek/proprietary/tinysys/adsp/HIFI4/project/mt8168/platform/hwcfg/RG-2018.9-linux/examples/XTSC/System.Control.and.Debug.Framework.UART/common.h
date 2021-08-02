// Customer ID=13943; Build=0x75f5e; Copyright (c) 2016-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#define ADDR_UART_BEG   0x20000000
#define ADDR_UART_WR    0x20000000
#define ADDR_UART_RD    0x20000010
#define ADDR_UART_END   0x200000FF
#define VADR_UART_WR    0x20000000
#define VADR_UART_RD    0x20000010

#define BUF_LEN         256
#define NEW_LINE_CHAR   0x0A

#define QUEUE_DEPTH     256

// Lowest numbered level-sensitive interupt
#define IntNum          0
#define BInterruptXX    BInterrupt00
