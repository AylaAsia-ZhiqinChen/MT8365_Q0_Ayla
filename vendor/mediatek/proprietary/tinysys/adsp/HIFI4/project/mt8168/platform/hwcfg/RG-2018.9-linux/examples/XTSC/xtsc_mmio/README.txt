                The xtsc_mmio Example

This system is meant to illustrate some of the capabilities of
the xtsc_mmio model.  

The system is comprised of the following XTSC models:
  1.  Two xtsc_core objects (core0 and core1)
  2.  Two xtsc_memory objects (core0_pif and core1_pif)
  3.  One xtsc_router (router)
  4.  One xtsc_mmio (mmio)

The PIF of core0 is connected to the slave interface of the
router.  The router routes most of the address space to the
core0_pif memory; however, a 32 byte wide aperture in the address
space gets translated to addresses 0x10000000-0x1000001F and
is then routed to the mmio device (see routing.tab).

The mmio device has two registers: one, located at 0x10000010, is
called EXPSTATE and the other, located at 0x10000000, is called
BInterruptMN (MN depend upon the configuration).  See mmio.txt.
Both registers are readable and writable from core0 using its
PIF.  In addition, the EXPSTATE register is connected to a
wire-level input of the mmio device (i.e. an sc_export) which is
driven by the TIE export state of core1 called EXPSTATE.  The
BInterruptMN register is connected to a wire-level output of the
mmio device (i.e. an sc_port).  This sc_port drives the BInterrupMN
system-level input of core1.

The PIF of core1 is connected directly to the core1_pif memory and
is not used for memory-mapped I/O.

The program on core1 enables the interrupt corresponding to 
BInterruptMN and then proceeds to write random 32-bit values
to its TIE export state called EXPSTATE until such time as it
receives its first interrupt.  As soon as it receives an
interrupt, it writes 0xDEADBEEF to EXPSTATE and exits.

The program on core0 sits in a polling loop and uses memory-mapped 
I/O to read the EXPSTATE register of the mmio device.  Whenever it
finds a value in this register that is divisible by 16, it uses
memory-mapped I/O to write a 1 to the BInterruptMN register.  If it
ever finds the value of 0xDEADBEEF in the EXPSTATE register, it
exits.

