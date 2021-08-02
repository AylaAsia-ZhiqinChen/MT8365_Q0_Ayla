                                   xtsc_wire.shared.memory Example



Part A) Overview:

This project uses xtsc-run to illustrate using host OS shared memory with xtsc_wire so that the wire writer (source) and
wire reader (sink) can run in different host OS processes running at the same time on the same workstation.  

Typically when using host OS shared memory with xtsc_wire there are two host OS XTSC processes that each have an
xtsc_core instance in them; however, it is possible for either of them to not have any xtsc_core instances in them and
instead use Lua scripts to write and/or read wire data. It is also possible for one of them to be a non-XTSC processes.




Part B) General Instructions:

Note: If you encounter problems on Linux, you may need to manually delete the shared memory files in /dev/shm.

1) Open a command shell and set up its environment for Xtensa tools and your chosen config which must be compatible with 
   the XTSC example user TIE code.  See status and control in ../TIE/example.tie.
   Note: No config is required for System B 
2) Build the example user TDK and the target programs (source.out and sink.out):
   Note: Not required for System B 
   Linux:
        make target
   MS Windows:
        xt-make target
3) Start the XTSC simulation(s) as shown on the appropriate Run line with each system below.




Part C) Systems:

This project contains the following 4 systems (the last 2 systems are hybrids of the first 2 systems).  Each system can
be run in a single OS process (i.e. a single xtsc-run simulation) or in two separate OS processes (i.e. two separate
xtsc-run simulations).

Note:  In the cases where two separate XTSC simulations are launched, an OS sleep command is used to allow the first
       simulation process enough time to create the shared memory wire and then a go file is used to end the
       elaboration phase and start the simulation phase in each simulation at approximately the same wall time.
       See eoe.lua.


                                        System A

This system has two xtsc_core instances: core0 writes to the wire using its TIE export state called status and core1
reads from the wire using its TIE import wire called control.  See write_status() in target/source.c and read_control()
in target/sink.c:

                             Host OS 
                          Shared Memory
     xtsc_core              xtsc_wire               xtsc_core
   =============        =================         =============
                                                   
   /-----------\                                  /-----------\
   |   core0   |                                  |   core1   |
   |           |        |================|        |           |
   |     status|------->| core0_to_core1 |------->|control    |
   |source.out |        |================|        | sink.out  |
   \-----------/                                  \-----------/

Run as a single process:
  xtsc-run -define=core_source=1 -define=core_sink=1 -i=system.inc
Run as two processes on Linux:
  rm go; xtsc-run -define=core_source=1 -i=system.inc &; sleep 1; xtsc-run -define=core_sink=1   -i=system.inc &; sleep 1; touch go
Run as two processes on MS Windows:
  del go & start cmd /K xtsc-run -define=core_source=1 -i=system.inc & sleep 1 & start cmd /K xtsc-run -define=core_sink=1   -i=system.inc & sleep 1 & copy nul go




                                        System B

This system has Lua scripts in lieu of xtsc_core instances.  Lua script source.lua takes the role of core0/source.out and
writes to the wire, while Lua script sink.lua takes the role of core1/sink.out and reads from the wire:

Note: Timing is non-deterministic when this system is run in two separate OS processes.  This may cause the 2nd process
      to run forever because sink.lua is trying to get the expected number of different wire values.

                             Host OS 
                          Shared Memory
   Lua script               xtsc_wire                Lua script       
   ==========           ==================           ==========
                                                   
  /~~~~~~~~~~\                                      /~~~~~~~~~~\
 /            \         |================|         /            \
(  source.lua  )------->| core0_to_core1 |------->(   sink.lua   )
 \            /         |================|         \            /
  \~~~~~~~~~~/                                      \~~~~~~~~~~/

Run as a single process:
  xtsc-run -define=lua_source=1 -define=lua_sink=1 -define=YIELD=1 -i=system.inc
Run as two processes on Linux:
  rm go; xtsc-run -define=lua_source=1 -i=system.inc &; sleep 1; xtsc-run -define=lua_sink=1 -i=system.inc &; sleep 1; touch go
Run as two processes on MS Windows:
  del go & start cmd /K xtsc-run -define=lua_source=1 -i=system.inc & sleep 1 & start cmd /K xtsc-run -define=lua_sink=1 -i=system.inc & sleep 1 & copy nul go




                                        System A-B

This system is a hybrid of System A and System B with core0/source.out writing to the wire while Lua script sink.lua
reads from it:

                             Host OS 
                          Shared Memory
     xtsc_core              xtsc_wire                Lua script       
   =============        =================            ==========
                                                   
   /-----------\                                   
   |   core0   |                                    /~~~~~~~~~~\
   |           |        |================|         /            \
   |     status|------->| core0_to_core1 |------->(   sink.lua   )
   |source.out |        |================|         \            /
   \-----------/                                    \~~~~~~~~~~/

Run as a single process:
  xtsc-run -define=core_source=1 -define=lua_sink=1 -define=YIELD=1 -i=system.inc
Run as two processes on Linux:
  rm go; xtsc-run -define=core_source=1 -i=system.inc &; sleep 1; xtsc-run -define=lua_sink=1 -i=system.inc &; sleep 1; touch go
Run as two processes on MS Windows:
  del go & start cmd /K xtsc-run -define=core_source=1 -i=system.inc & sleep 1 & start cmd /K xtsc-run -define=lua_sink=1 -i=system.inc & sleep 1 & copy nul go




                                        System B-A

This system is also a hybrid of System A and System B but this time Lua script source.lua writes to the wire while
core1/sink.out reads from it:

                             Host OS 
                          Shared Memory
   Lua script               xtsc_wire               xtsc_core
   ==========           ==================        =============
                                                   
                                                  /-----------\ 
  /~~~~~~~~~~\                                    |   core1   |
 /            \         |================|        |           |
(  source.lua  )------->| core0_to_core1 |------->|control    |
 \            /         |================|        | sink.out  |
  \~~~~~~~~~~/                                    \-----------/

Run as a single process:
  xtsc-run -define=lua_source=1 -define=core_sink=1 -define=YIELD=10000 -i=system.inc
Run as two processes on Linux:
  rm go; xtsc-run -define=lua_source=1 -define=TOTAL=100000 -define=LOG=0 -i=system.inc &; sleep 1; xtsc-run -define=core_sink=1 -i=system.inc &; sleep 1; touch go
Run as two processes on MS Windows:
  del go & start cmd /K xtsc-run -define=lua_source=1 -define=TOTAL=100000 -define=LOG=0 -i=system.inc & sleep 1 & start cmd /K xtsc-run -define=core_sink=1 -i=system.inc & sleep 1 & copy nul go
                                                  



Part D) Variations, hints, and tips


- Commands can be passed to xtsc-run to control various things:
  - Cause the Lua script to yield:   -define=YIELD=1
  - To disable printing wire values: -define=LOG=0
  - To use TurboXim:                 -define=TURBO=1
  - To do 100 wire items:            -define=TOTAL=100
       or to run forever:            -define=TOTAL=0
  - To disable XTSC logging:         -define=LOGGING=0
                         or:         -define=LOGGING=elab_only


- To debug the system model or just see what it is, uncomment -dump_commands in xtsc_wire.shared.memory.inc and then search the log files
  for the commands:
        grep commands xtsc-*.log
        findstr commands xtsc-*.log


- The term "word tearing" describes the situation that can occur with shared memory if one entity reads a value from
  shared memory that another entity has only partially written (that is, some bytes of the value have been written but
  others have not yet been written).
  - If your wire bit width is greater then 32 bits on a 32-bit OS, then word tearing may occur between 32-bit words.
  - If your wire bit width is greater then 64 bits on a 64-bit OS, then word tearing may occur between 64-bit words.


- If desired, you can use a non-XTSC program to read or write the wire.  See "host_shared_memory" and "shared_memory_name"
  in doxygen/XTSC RM for class xtsc_wire_parms.  For example C++ code which creates host OS shared memory, see
  ../xtsc_memory.shared.memory.dispatch/dispatch.cpp


- If desired, you can monitor the host OS shared memory.  This can be done several ways.  Two of them are:
  1)  Use xtsc_wire in another zombie XTSC system on the same workstation/PC.  For example:
          xtsc-run -define=W=core0_to_core1 -i=monitor.with.xtsc_wire.inc
          cmd: man xtsc_wire
          cmd: core0_to_core1 read
          0x0ba9876547e40
          cmd: 
          0x0ba9876548848
          cmd: //
  2) Use XTSC shmem* commands from the XTSC command prompt in an empty XTSC system on the same workstation/PC (or from the
     Lua command prompt or a Lua script). For example:
          xtsc-run -cmd
          cmd: man shmem*
          cmd: xtsc shmem_get joeuser.core0_to_core1 0x0 7
          cmd: xtsc shmem_dump joeuser.core0_to_core1 0 7
                       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f 
          0x00000000: e6 8f 55 76 98 ba 00                            ..Uv...         
          cmd: 
                       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f 
          0x00000000: 2c 95 55 76 98 ba 00                            ,.Uv...         
          cmd: //
     Note: The host OS shared memory files that XTSC creates are shown in the xtsc.log file,  In addition on Linux, you can
           use "ls -al /dev/shm/*" to see the shared memory files.  On MS Windows, the procexp program from Microsoft's 
           sysinternals can be used to see the names and sizes of shared memory handles (called Section in procexp) of the
           currently selected running programs using drop-down menu sequences:
                  View>Show Lower Pane         (Ctrl-L)
                  View>Lower Pane View>Handles (Ctrl-H)




References:
XTSC User's Guide (xtsc_ug.pdf):
  See Section 6.8 "Host OS Shared Memory in XTSC".
XTSC Reference Manual (xtsc_rm.pdf):
  See the sections on the xtsc_wire_parms and xtsc_wire classes.


Other shared memory examples:
../xtsc_memory.shared.memory.dispatch/README.txt
../xtsc_memory.shared.memory.mutex/README.txt
../xtsc_queue.shared.memory/README.txt
../System.Control.and.Debug.Framework.UART/README.txt

