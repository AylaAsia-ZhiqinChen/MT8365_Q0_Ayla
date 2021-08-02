                                        System.Control.and.Debug.Framework.UART
                                                        Example



Part A) Overview:

The purpose of this project is to illustrate the system control and debug framework in XTSC by composing a UART model using only
Lua scripts and existing XTSC models rather than writing a new model using SystemC/C++ (that is, rather then creating a new
sc_module sub-class for the UART).  See Section 6.4, "The XTSC System Control and Debug Framework" in the "XTSC Core Library"
chapter of xtsc_ug.pdf.

The project runs as two separate host OS processes (two separate XTSC simulations):
1)  The first process runs the real system, modeling the SoC, which includes an Xtensa processor, a system memory, and a UART
    (see system.inc).  The Xtensa processor (xtsc_core instance core0) is construed to be a DSP which takes commands that arrive
    by way of the UART.  
2)  The second process runs a zombie system which models the UART as seen by the terminal being used to deliver commands to the
    DSP by way of the UART (see term.inc).

Note: This UART example is provided to illustrate the system control and debug framework in XTSC.  Although it can easily be
      added to a larger system to provide UART functionality, in most cases a better approach is to just use the standard
      C library I/O functions (e.g. printf/scanf) or to use xt_iss_simcall(), both of which are provided by the semi-hosting
      capabilities of the ISS in the sim linker support package (LSP).

Note: The UART model in this example was designed following the steps outlined in Section 29.0.2, "Composing Models using the
      Test Bench Classes" in "The Test Bench Classes" chapter of xtsc_ug.pdf.





Part B) Instructions:

Note: If you encounter problems on Linux, you may need to manually delete the shared memory files in /dev/shm.

1) Adjust common.h if required for your config (which must have a PIF system memory interface).
2) Open 2 command shells on the same host (i.e. the same computer/workstation) and set up their environment for Xtensa
   tools and your chosen config.
3) In the first command shell build the target program dsp.out (see target/dsp.c):
   Linux:
        make target
   MS Windows:
        xt-make target
4) In the first command shell start the real system which models the SoC:
        xtsc-run -include=system.inc
5) In the second command shell start the zombie system which models the terminal controlling the DSP in the real system using the UART.
        xtsc-run -include=term.inc
        Use 'quit' to quit.
        term> help
        ...
        term> quit


Some other things you can try:

- By default the real system uses the cycle-accurate ISS.  You can cause it to use TurboXim instead like this:
        xtsc-run -define=TURBO=1 -include=system.inc

- By default when Xtensa attempts to write a byte to the UART that cannot be done at the current time because xtensa_wr_queue is
  full, the UART follows protocol to indicate the byte write failed.  Similarly, when Xtensa attempts to read a byte from the
  UART that cannot be done at the current time because xtensa_rd_queue is empty, the UART follows protocol to indicate the byte
  read failed.  Rather than indicating failure, you can instead have the UART block on the request and suspend the simulation
  when Xtensa makes a write or read request that would normally fail.  The simulation will remain suspended until xtensa_wr_queue
  becomes non-full (for a write) or xtensa_rd_queue becomes non-empty (for a read).  To cause the UART to block instead of
  indicating failure:
        xtsc-run -define=BLOCK=1 -include=system.inc
  Note: The UART suspends the simulation by calling xtsc_host_sleep().  See uart_wr.lua and uart_rd.vec.


- You can combine blocking with TurboXim like this:
        xtsc-run -define=TURBO=1 -define=BLOCK=1 -include=system.inc

- You can define the XTSC_LOG_UNIQUIFIER environment variable in each shell to give the XTSC log file for each process a fixed
  name.  For example, give the XTSC log file the name xtsc-soc.log for the real system and the name xtsc-term.log for the zombie
  system of the terminal.
                               -----------Linux bash-----------   -----------Linux csh------------   ---------MS Windows----------
  First shell (system.inc):    export XTSC_LOG_UNIQUIFIER=-soc    setenv XTSC_LOG_UNIQUIFIER -soc    set XTSC_LOG_UNIQUIFIER=-soc
  Second shell (term.inc):     export XTSC_LOG_UNIQUIFIER=-term   setenv XTSC_LOG_UNIQUIFIER -term   set XTSC_LOG_UNIQUIFIER=-term

- You can see in the xtsc-soc.log file each time Xtensa read or wrote the UART:
  Linux:
        grep "Popped\|Pushed" xtsc-soc.log
  MS Windows:
        findstr "Popped Pushed" xtsc-soc.log



Part C) Combined system diagram showing both processes (Real System for the SoC and Zombie System for the terminal)


                                                  Real System (system.inc)
|--------------------------------------------------------------------------------------------------------------------------|

                                                                                                                      Zombie System (term.inc)
                                                                                                         |--------------------------------------------|

                                                                                 xtsc_memory                  Host OS 
    xtsc_wire_source                                                             Lua Snippet               Shared Memory
      Lua Snippet                  xtsc_core            xtsc_router              Lua Script                  xtsc_queue              Lua Script 
=========================       ================        ============        ======================       ===================       ==============

                                /--------------\            /------|        /--------------------\
/-----------------------\       |    core0     |           /      0|<======>|       sysmem       |
|     BInterruptXX*     |       |              |          /        |        \--------------------/
|                       |======>|BInterruptXX  |         /         |
|     "script_file"     |       |              |        |          |        /--------------------\
|   /~~~~~~Lua~~~~~~\   |       |           PIF|<======>|  router  |        |     uartregs*      |
|  /                 \  |       |              |        |          |        |                    |
| ( BInterruptXX.vec  ) |       |              |         \         |        |    "script_file"   |
|  \ xtensa_rd_queue /  |       | target/dsp.c |          \        |        |   /~~~~Lua~~~~~\   |                                   /~~Lua~~~\
|   \~~~~~~~~~~~~~~~/   |       |    dsp.out   |           \      1|<======>|  / got_uart_rd  \  |       |=================|        / term.lua \
\-----------------------/       \--------------/            \------|        | (  uart_rd.vec   )<--------| xtensa_rd_queue*|<------(push     pop)<----\
                                                                            |  \ ADDR_UART_RD /  |       |=================|        \   term>  /      |
                                                                            |   \~~~~~~~~~~~~/   |                                   \~~~~~~~~/       |
                                                                            |                    |                                                    |
                                                                            |    ADDR_UART_WR    |                                                    |
                                                                            |   uart_wr_filter   |                                                    |
                                                                            |    watchfilter     |                                                    |
                                                                            \---------:----------/                                                    |
                                                                                      :                                                               |
                                                                                      : notify uart_wr_event                                          |
                                                                                      :                                                               |
                                                                                      V                                                               |
                                                                                /~~~~Lua~~~~\                                                         |
                                                                               /             \           |=================|                          |
                                                                              (  uart_wr.lua  )--------->| xtensa_wr_queue*|------------------------->/
                                                                               \             /           |=================|
                                                                                \~~~~~~~~~~~/    




BInterruptXX*: A Lua snippet in this xtsc_wire_source asserts a level-sensitive interrupt to core0 whenever xtensa_rd_queue
               is not empty.  There is no SystemC port-binding between xtensa_rd_queue (an xtsc_queue) and BInterruptXX
               (an xtsc_wire_source), instead the XTSC command facility is used by the Lua snippet to query the queue.  See
               can_pop in BInterruptXX.vec.

uartregs*: This xtsc_memory instance models a small portion of the 4GB address space containing UART registers (see common.h).


xtensa_rd_queue*: This xtsc_queue uses host OS shared memory and is instantiated in both processes (system.inc and term.inc).
                  Each queue element is a single character in the UART on the datapath from the terminal to the SoC.  On Linux
                  you can find the shared memory proxy file for user joeuser (for example) at:
                    /dev/shm/joeuser.xtensa_rd_queue


xtensa_wr_queue*: This xtsc_queue uses host OS shared memory and is instantiated in both processes (system.inc and term.inc).
                  Each queue element is a single character in the UART on the datapath from the SoC to the terminal.  On Linux
                  you can find the shared memory proxy file for user joeuser (for example) at:
                    /dev/shm/joeuser.xtensa_wr_queue




Part D) The UART Model and Xtensa Usage Protocol

Here is a simplified diagram of an abstract but fairly-canonical full-duplex UART showing both endpoint devices.  As you can see,
a UART is actually a pair of identical devices that are cross-wired.  We will refer to one of these devices as a "half-UART".

                                               UART
                         |-----------------------------------------------|

                          Left half-UART                  Right half-UART
                         |---------------|               |---------------|


                         /---------------\               /---------------\
                         |  _________    |               |    _________  |
                         |  | | | | |->TX|--->\     /<---|TX<-| | | | |  |
/-----------------\      |  ---------    |     \   /     |    ---------  |      /-----------------\
|                 |      |               |      \ /      |               |      |                 |
| Endpoint Device |<====>|EndPoint IF    |       X       |    EndPoint IF|<====>| Endpoint Device |
| (e.g. Processor)|      |               |      / \      |               |      | (e.g. Terminal) |
\-----------------/      |  _________    |     /   \     |    _________  |      \-----------------/
                         |  | | | | |<-RX|<---/     \--->|RX->| | | | |  |
                         |  ---------    |               |    ---------  |
                         \---------------/               \---------------/


And here is how the UART is modeled in this project from the point-of-view of each endpoint device:

From Xtensa's point-of-view, the UART model is comprised of the following parts which are composed from XTSC models and Lua
snippets and scripts specified in system.inc:

1) UART registers:  Modeled using xtsc_memory instance uartregs.  See common.h for addresses.
   - ADDR_UART_RD/VADR_UART_RD:  The read control and status register.  To read a byte from the UART, Xtensa (core0) must do a
     single unsigned 32-bit read of ADDR_UART_RD.  If the value read is 0xFFFFFFFF then no data was available.  If the value read
     is not 0xFFFFFFFF then byte read from the UART is in the LSB (0x000000FF) for little endian configs and in the MSB
     (0xFF000000) for big endian configs.  See read_char() in target/dsp.c.
   - ADDR_UART_WR/VADR_UART_WR:  The write control and status register.  To write a byte to the UART, Xtensa must first do an
     unsigned 32-bit write to ADDR_UART_WR with the byte in the LSB (0x000000FF) for little endian configs and in the MSB
     (0xFF000000) for big endian configs and then do an unsigned 32-bit read from ADDR_UART_WR.  If the read returns 0, the byte
     write was successful, if the read returns 0xFFFFFFFF, then the byte write failed.  See write_char() in target/dsp.c.

2) UART datapaths:
   - Xtensa read  datapath:  Modeled using xtsc_queue instance xtensa_rd_queue using host OS shared memory plus ADDR_UART_RD in
     xtsc_memory instance uartregs.  The xtensa_rd_queue models the TX fifo in Right half-UART plus the RX fifo in Left half-UART.
   - Xtensa write datapath:  Modeled using xtsc_queue instance xtensa_wr_queue using host OS shared memory plus ADDR_UART_WR in
     xtsc_memory instance uartregs.  The xtensa_wr_queue models the TX fifo in Left half-UART plus the RX fifo in Right half-UART.

3) UART interrupt:  Modeled using xtsc_wire_source instance BInterruptXX.

4) UART processes:
   - Read  process:  Modeled using Lua function got_uart_rd() defined in the Lua snippet in uart_rd.vec which is specified to 
     xtsc_memory instance uartregs using its "script_file" parameter in system.inc.
   - Write process:  Modeled using Lua script uart_wr.lua which is added to the system using the -add_lua_script_file xtsc-run
     command in system.inc.
   - Interrupt process:  Modeled using the Lua snippet in BInterruptXX.vec which is specified to xtsc_wire_source instance
     BInterruptXX using its "script_file" parameter.


From the terminal's point-of-view, the UART model is comprised of the following parts which are composed from xtsc_queue
instances using host OS shared memory and a Lua script specified in term.inc:

1) UART datapaths:
   - Terminal write datapath:  Modeled using xtsc_queue instance xtensa_rd_queue using host OS shared memory.
   - Terminal read  datapath:  Modeled using xtsc_queue instance xtensa_wr_queue using host OS shared memory.

2) UART processes:  Modeled using Lua script term.lua.




Part E) Elements of the system control and debug frameworks in XTSC

In accordance with its purpose, this project illustrates most of the elements discussed in Section 6.4, "The XTSC System Control
and Debug Framework" in the "XTSC Core Library" chapter of xtsc_ug.pdf, including:

- The XTSC command facility.    See the xtsc.cmd() lines in term.lua, uart_wr.lua, uart_rd.vec, and BInterruptXX.vec.

- Named SystemC events.         See uart_wr_event in uart_wr.lua and xtsc_driver_wait_event in BInterruptXX.vec.

- Watchfilters.                 See uart_wr_filter in uart_wr.lua.

- Lua scripting.                See Lua script files term.lua and uart_wr.lua.  Also see these xtsc_script_files which contain
                                Lua snippets inside #lua_beg/#lua_end blocks: uart_rd.vec and BInterruptXX.vec.

- User-defined state.           See user_state in system.inc and term.inc.  Also see xtsc_user_state_get in uart_wr.lua,
                                uart_rd.vec, and term.lua.

- Testbench Classes.            This project uses the xtsc_wire_source testbench model as well as the testbench capabilities of
                                xtsc_memory.  See "script_file" in system.inc.

- Host OS shared memory.        See host_shared_memory in system.inc and term.inc.





References:

XTSC User's Guide (xtsc_ug.pdf):
  See Section 6.4, "The XTSC System Control and Debug Framework" in the "XTSC Core Library" chapter.
  See Section 29.0.2, "Composing Models using the Test Bench Classes" in "The Test Bench Classes" chapter.

XTSC Reference Manual (xtsc_rm.pdf):
  See the sections on the xtsc_queue_parms and xtsc_queue classes.


Other shared memory examples:

../xtsc_memory.shared.memory.dispatch/README.txt
../xtsc_memory.shared.memory.mutex/README.txt
../xtsc_queue.shared.memory/README.txt
../xtsc_wire.shared.memory/README.txt


