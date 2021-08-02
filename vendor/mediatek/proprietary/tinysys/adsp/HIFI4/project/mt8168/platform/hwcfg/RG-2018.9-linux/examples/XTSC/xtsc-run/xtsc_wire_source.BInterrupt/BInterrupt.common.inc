// Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// Wait for a base amount of time
wait 10000

// Wait for an additional interval then assert the interrupt
$(INTERVAL)  1

// Wait for target program to tell us to continue
wait control change

// Now wait 1 cycle then de-assert the interrupt
1      0

