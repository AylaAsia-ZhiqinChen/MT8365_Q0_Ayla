                                 xtsc_queue.shared.memory Example



Part A) Overview:

This project uses xtsc-run to illustrate using host OS shared memory in xtsc_queue instances so that, if desired, the
producer and consumer can run in different host OS processes running at the same time on the same workstation.  

Typically the different host OS processes are different xtsc-run simulations that each have an xtsc_core instance in them
(see System A); however, it is also possible for some of them to not have any xtsc_core instances in them and instead use
Lua scripts to produce and/or consume queue data (see System B and hybred System B-A-B).  It is even possible for some of
them to be non-XTSC processes as long as they follow the prescribed protocol for accessing the shared memory queue (see Part J).

The project contains the following systems:

System A: Multiple xtsc_core instances connected in sequence using xtsc_queue instances.  See Part B, Part C, and Part D.

System B: A stand-alone xtsc_queue instance driven by two Lua scripts.  See Part E, Part F, and Part G.

System B-A-B: A hybred of System A and System B.  See Part H and Part I.




                                                      System A


Part B) System A description:

This system can be configured at launch time for between 2 and 16 xtsc_core instances connected in sequence by 1 to 15
xtsc_queue instances.  The number of cores is specified by the NumCores=M macro and, depending on how you launch the
system, you can have either a single M-core xtsc-run simulation or M single-core xtsc-run simulations.

The program running on each core (main.out) pops one queue datum from the upstream queue (except core0 which just starts
with a datum of 0), writes a counter in its allotted bit field within the datum, and then pushes the datum into the
downstream queue (except the last core which drops the datum on the floor).  The size of the bit field is determined by
how many cores there are (see num_bits_tab[17] in target/main.c) and the position of the bit field is shifted by the bit
field width multiplied by the core's index (N).  The counter in coreN is offset by N+1 (core0 starts at 1, core1 starts at
2, core2 starts at 3, etc.).  See diagram below.  By default, each core pushes and/or pops 10 items but this can be changed
(see TOTAL below).  By default, each queue can hold 10 items at a time; however this can be changed (see DEPTH below).


Part C) System A block diagram for NumCores=3 showing first 5 items to pass through each queue (see multicore.inc and coreN.inc):

  xtsc_core          xtsc_queue          xtsc_core          xtsc_queue          xtsc_core
=============        ==========        =============        ==========        =============
                   
/-----------\                          /-----------\                          /-----------\
|   core0   |                          |   core1   |                          |   core2   |
|           |        |========|        |           |        |========|        |           |
|      OUTQ1|=======>|  Q01   |=======>|INQ1  OUTQ1|=======>|  Q12   |=======>|INQ1       |
| main.out  |        |========|        | main.out  |        |========|        | main.out  |
\-----------/            |             \-----------/            |             \-----------/
                         |                                      |
                         |                                      |
             0x000000000000000000000001             0x000000000000000200000001 
             0x000000000000000000000002             0x000000000000000300000002 
             0x000000000000000000000003             0x000000000000000400000003 
             0x000000000000000000000004             0x000000000000000500000004 
             0x000000000000000000000005             0x000000000000000600000005 


Notes:
- There can be 2 to 16 cores and 1 to 15 queues, correspondingly, in the simulation.
- Core names have a trailing hex digit to indicate their position in the dataflow:
    core0, core1, core2, ..., core9, coreA, coreB, coreC, coreD, coreE, coreF
- Queue names have 2 trailing hex digits (the 1st from the upstream core, the 2nd from the
  downstream core) to indicate their position in the dataflow:
    Q01, Q12, Q23, ..., Q89, Q9A, QAB, QBC, QCD, QDE, QEF
- When this system is run using TurboXim (TURBO=1) in separate host OS processes, then special handling has to applied to
  ensure the system doesn't hang because TurboXim is waiting for a queue to become non-full or non-empty.  See Lua sript
  files: turbo.producer.lua and turbo.consumer.lua.



Part D) System A Instructions:

Note: If you encounter problems on Linux, you may need to manually delete the shared memory files in /dev/shm.

1) Open a command shell and set up its environment for Xtensa tools and your chosen config which must be compatible with 
   the XTSC example user TIE code.  See INQ1 and OUTQ1 in ../TIE/example.tie.
2) Build the example user TDK and the target program (main.out):
   Linux:
        make target
   MS Windows:
        xt-make target
3) Start the XTSC simulation(s) using one of the following formats (examples shown for NumCores=M=3):
   a) 1 M-core simulation using cycle-accurate ISS inside XTSC:
        xtsc-run -define=NumCores=3 -i=multicore.inc 
   b) M 1-core simulations using cycle-accurate ISS inside XTSC.  An OS sleep command is used to allow the preceding
      simulation process enough time to create the shared memory queue and then the go file is used to end the elaboration
      phase and start the simulation phase in each simulation at approximately the same wall time.  See eoe.lua:
      Linux bash:
        rm go
        for (( N=0; N<3; N=N+1 )); do xtsc-run -define=NumCores=3 -define=N=$N -i=coreN.inc&; sleep 1; done
        touch go
      Linux csh:
        rm go
        foreach N (`seq 0 1 2`)
          xtsc-run -define=NumCores=3 -define=N=$N -i=coreN.inc &
          sleep 1
        end
        touch go
      MS Windows:
        del go
        for /L %N in (0,1,2) do start cmd /K xtsc-run -define=NumCores=3 -define=N=%N -i=coreN.inc & sleep 1
        copy nul go
   Notes:
   - When LOG=1, each core's output is sent to a log file (core0.log, core1,log, etc)
   - Commands can be passed to xtsc-run to control various things:
     - To change queue depth:           -define=DEPTH=5
     - To use TurboXim:                 -define=TURBO=1
     - To disable XTSC logging:         -define=LOGGING=0
                            or:         -define=LOGGING=elab_only
     - To disable printing queue data:  -define=LOG=0
     - To do 100 queue items per core:  -define=TOTAL=100
                    or to run forever:  -define=TOTAL=0




                                                      System B


Part E) System B description:

In this system an xtsc_queue is serviced on both sides by Lua scripts (producer.lua and consumer.lua).  There are two
variants.  In the first variant there is only one xtsc-run simulation process and the Lua scripts yield to the SystemC
kernel for one delta cycle if the queue is full (producer) or empty (consumer).  In the second variant there are two
xtsc-run simulation processes (one for each Lua script) and the Lua scripts never yield to the SystemC kernel.  In both
variants the xtsc-run simulations are considered zombie simulations because SystemC time is not allowed to advanced.



Part F) System B block diagram showing first 5 items to pass through the queue (see no_core_queue.inc and producer.lua):

                           xtsc_queue
                          with  Host OS
   Lua script             Shared Memory             Lua script       
   ==========           =================           ==========
                    
  /~~~~~~~~~~\                                     /~~~~~~~~~~\
 /            \         |===============|         /            \
( producer.lua )------->| no_core_queue |------->( consumer.lua )
 \            /         |===============|         \            /
  \~~~~~~~~~~/                  |                  \~~~~~~~~~~/
                                |
                                |
                    0x0000000000000000babe0000
                    0x00000000babe0000babe0001
                    0xbabe0000babe0001babe0002
                    0xbabe0001babe0002babe0003
                    0xbabe0002babe0003babe0004


Part G) System B Instructions:

Note: If you encounter problems on Linux, you may need to manually delete the shared memory files in /dev/shm.

1) Open a command shell and set up its environment for Xtensa tools.  No Xtensa config is required.
2) Start the XTSC simulation(s) using one of the following formats:
   a) One xtsc-run simulation with an xtsc_queue and a producer.lua script and a consumer.lua script:
        xtsc-run -define=PRODUCER=1 -define=CONSUMER=1 -i=no_core_queue.inc
   b) Two xtsc-run simulations each with an xtsc_queue backed by the same host OS shared memory.  The first simulation
      has the producer.lua script and the second simulation has the consumer.lua script.
      Linux:
        xtsc-run -define=PRODUCER=1 -i=no_core_queue.inc &; sleep 1; xtsc-run -define=CONSUMER=1 -i=no_core_queue.inc &
      MS Windows:
        start cmd /K xtsc-run -define=PRODUCER=1 -i=no_core_queue.inc & sleep 1 & start cmd /K xtsc-run -define=CONSUMER=1 -i=no_core_queue.inc 
   Notes:
   - Commands can be passed to xtsc-run to control various things:
     - To change queue depth:           -define=DEPTH=5
     - To disable printing queue data:  -define=LOG=0
     - To disable XTSC logging:         -define=LOGGING=0
                            or:         -define=LOGGING=elab_only
     - To do 100 queue items per core:  -define=TOTAL=100
                    or to run forever:  -define=TOTAL=0
     - To use a different queue name:   -define=QUEUE=Q01




                                                      System B-A-B


Part H) System B-A-B description:

System B-A-B is a hybred system that looks like System A except that the first core has been replaced with the
producer.lua script of System B and the last core has been replaced by the consumer.lua script of System B.



Part I) System B-A-B Instructions:
1) Setup System A as explained in Part D.
2) Run the system as three xtsc-run simulations.  Shown here for NumCores of 3 (but only core1 is created, because core0 is
   replaced by producer.lua and core2 is replaced by consumer.lua)
      Linux bash:
        xtsc-run -define=PRODUCER=1 -define=QUEUE=Q01 -i=no_core_queue.inc &; sleep 1; \
        rm go; for (( N=1; N<2; N=N+1 )); do xtsc-run -define=NumCores=3 -define=N=$N -i=coreN.inc&; sleep 1; done; touch go; \
        xtsc-run -define=CONSUMER=1 -define=QUEUE=Q12 -i=no_core_queue.inc &
      Linux csh:
        xtsc-run -define=PRODUCER=1 -define=QUEUE=Q01 -i=no_core_queue.inc &; sleep 1
        rm go
        foreach N (`seq 1 1 1`)
          xtsc-run -define=NumCores=3 -define=N=$N -i=coreN.inc &; sleep 1
        end
        touch go
        xtsc-run -define=CONSUMER=1 -define=QUEUE=Q12 -i=no_core_queue.inc &
      MS Windows:
        del go & start cmd /K xtsc-run -define=PRODUCER=1 -define=QUEUE=Q01 -i=no_core_queue.inc & sleep 1  
        for /L %N in (1,1,1) do start cmd /K xtsc-run -define=NumCores=3 -define=N=%N -i=coreN.inc & sleep 1
        start cmd /K xtsc-run -define=CONSUMER=1 -define=QUEUE=Q12 -i=no_core_queue.inc 
        copy nul go




                                                     Other Points



Part J) Using a non-XTSC program to service the host OS shared memory backing an xtsc_queue:

If desired, you can use a non-XTSC program to service one end of the queue as long as the program honors the queue layout
and protocol used by xtsc_queue.  See "Shared Memory Layout and Access Protocol" associated with class xtsc_queue in the
XTSC RM or in the doxygen comments in file xtsc_queue.h.  Also see "host_shared_memory" and "shared_memory_name" in
doxygen/XTSC RM for class xtsc_queue_parms.  

For example C++ code which creates host OS shared memory, see ../xtsc_memory.shared.memory.dispatch/dispatch.cpp



Part K) Monitoring host OS shared memory.

This can be done several ways.  Two of them are:

1)  Use xtsc_queue in another zombie XTSC system on the same workstation/PC.  For example:
        xtsc-run -define=Q=QEF -i=monitor.with.xtsc_queue.inc
        cmd: man xtsc_queue
        cmd: QEF dump
        Row   0: 64773 0x00000000043210fedcba9876
        Row   1: 64774 0x000000000543210fedcba987
        Row   2: 64775 0x0000000006543210fedcba98
        ridx=1 widx=0
        cmd: 

2) Use XTSC shmem* commands from the XTSC command prompt in an empty XTSC system on the same workstation/PC (or from the
   Lua command prompt or a Lua script). For example:
        xtsc-run -cmd
        cmd: man shmem*
        cmd: xtsc shmem_get joeuser.Q01 0x0 80
        cmd: xtsc shmem_dump joeuser.Q01
                     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f 
        0x00000000: 53 cf 0d 00 00 00 00 00 54 00 00 00 00 00 00 00 S.......T.......
        0x00000010: 00 00 00 00 00 00 00 00 51 cf 0d 00 00 00 00 00 ........Q.......
        0x00000020: 52 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R...............
        0x00000030: 52 cf 0d 00 00 00 00 00 53 00 00 00 00 00 00 00 R.......S.......
        0x00000040: 00 00 00 00 00 00 00 00 02 00 00 00 01 00 00 00 ................
        cmd:
   Note: The host OS shared memory files that XTSC creates are shown in the xtsc.log file,  In addition on Linux, you can
         use "ls -al /dev/shm" to see the shared memory files.  On MS Windows, the procexp program from Microsoft's 
         sysinternals can be used to see the names and sizes of shared memory handles (called Section in procexp) of the
         currently selected running programs using drop-down menu sequences:
                View>Show Lower Pane         (Ctrl-L)
                View>Lower Pane View>Handles (Ctrl-H)



Part L) Performance impact of queue depth, TurboXim and using separate host OS processes:

Note: All of these are with logging disabled (LOG=0 and LOGGING=0).

TurboXim (TURBO=1):  8 1-core simulations vs 1 8-core simulation:
When using TurboXim in separate OS processes the queue depth can make a 25x differences (see DEPTH above).  
When using a large queue (DEPTH>=100) we saw a 3x to 4x speed up when using 8 1-core simulations as opposed to 1 8-core
simulation.  When using a very small queue (DEPTH<=2), we saw a 2x to 3x slow down!

Cycle-Accurate (CA) ISS (TURBO=0):  8 1-core simulations vs 1 8-core simulation:
When using the CA ISS in 8 separate OS processes (each with 1 core), we saw a 8x speed up with a large queue (DEPTH=100)
and a 5x speed up when using a small queue (DEPTH=1).  Both of these are when compared to running 1 8-core simulation.

TurboXim vs Cycle-Accurate (CA) ISS:
For System A in 1 8-core simulation, TurboXim ran 200-300x faster then CA ISS as DEPTH varied from 1-100.
For System A in 8 1-core simulations, TurboXim ran 10-100x faster then CA ISS as DEPTH varied from 1-100.




References:
XTSC User's Guide (xtsc_ug.pdf):
  See Section 6.8 "Host OS Shared Memory in XTSC".
XTSC Reference Manual (xtsc_rm.pdf):
  See the sections on the xtsc_queue_parms and xtsc_queue classes.


Other shared memory examples:
../xtsc_memory.shared.memory.dispatch/README.txt
../xtsc_memory.shared.memory.mutex/README.txt
../xtsc_wire.shared.memory/README.txt
../System.Control.and.Debug.Framework.UART/README.txt

