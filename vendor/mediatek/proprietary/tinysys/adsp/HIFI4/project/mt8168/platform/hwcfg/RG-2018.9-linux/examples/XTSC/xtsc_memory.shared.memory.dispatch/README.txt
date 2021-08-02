                             xtsc_memory.shared.memory.dispatch Example



Part A) Overview:

This project illustrates a non-XTSC host program (dispatch) using host OS shared memory to distribute a set of tasks
(NumTasks) to 1 or more Xtensa's (NumCores=M) running either in a single M-core XTSC simulation or in M single-core XTSC
simulations.

Each task consists of one 1024-byte buffer load of data which the Xtensa target program (dsp.out) must process to compute
a single u32 value result.  The Xtensa target program processes the data as 128 pairs of u32 values using a fairly simple
calculation which can be seen in the Crunch section of target/dsp.c.  Basically, the high halfword (16-bits) of the
result is the bitwise AND of all the high halfwords of the 128 pairs of input values and the low halfword of the result
is the sum of the pairwise differences of the low halfwords of each pair.  Because the high halfword of each u32 input
value is the task number, the high halfword of the result should also be the task number.  Because the low halfwords of
each u32 input value form a ramp of values the difference in each pair should be 1 and the sum of differences of the 128
pairs should be 128=0x80.  Therefore the expected result for task 65=0x41 (for example) is 0x00410080.




Part B) Protocol for inter-process communication (IPC) using shared memory:

Because the host program (dispatch) and the XTSC simulation(s) are asynchronous OS processes accessing the same shared
memory something must be done to ensure the integrity of the inter-process communication implemented using that shared
memory.  The approach taken here is to implement a FIFO as a block of contiguous data buffers with write and read indices
in shared memory.  The write index indicates the next data buffer to be written and the read index indicates the next
data buffer to be read.  The protocol requires that only the dispatch program can write to the data buffers and the write
index and that it must populate the next empty data buffer prior to updating the write index.  The protocol likewise
requires that only the Xtensa target program may write the read index and that it must process the data in the next
populated data buffer prior to updating the read index.  When the read and write indices are equal the FIFO is empty and
when the read index is one more than the write index using modular arithmetic the FIFO is full.  An implication of this is
that when the FIFO is full there is still one data buffer in the FIFO which is not in use (not populated with valid data).  

There is also a result FIFO used to communicate results back to the dispatch program which is indexed by the same read
and write indices; however, the indices' role is opposite their name so that the Xtensa target program writes the result
to the location indicated by the read index and the dispatch program reads the result from the location indicated by the
write index.  The Xtensa target program must write the result prior to updating the read index and the dispatch program
must read the result before updating the write index.

The shmemN addresses used for IPC (data buffers, read index, write index, and result FIFO) and a few other constants are
defined in common.h.




Part C) System diagram of the host process and sub-system #N (N = 0 to NumCores-1) - See coreN.inc:


                Host Process                                 Host OS             Host Process
              of Sub-system #N                            Shared Memory          of dispatch 
==================================================        =============        ==================

  xtsc_core         xtsc_router        xtsc_memory
 ===========        ===========        ===========
                    
                                       /---------\
                       /------|        |         |
 /---------\          /      0|<======>| sysmemN |
 |  coreN  |         /        |        |         |
 |         |        |         |        \---------/
 |      PIF|<======>| routerN |                   
 | dsp.out |        |         |        /---------\
 \---------/         \        |        |         |
                      \      1|<======>| shmemN* |                               /~~~~~~~~~~~~\
                       \------|        |         |        |===========|         / Host Process \
                                       |    X<---|------->|   64 KB   |<-------(------>X        )
                                       \---------/        |===========|         \   dispatch   /
                                                                                 \~~~~~~~~~~~~/


shmemN*: Each shmemN has 64 KB of host OS shared memory (see SHMEM_SIZE in common.h and host_shared_memory in coreN.inc).  
         On Linux you can find the shared memory file for user joeuser and N=3 (for example) at:
           /dev/shm/joeuser.shmem3



Note: Although this project uses host OS shared memory for inter-process communication (IPC) between the host program and
      the Xtensa target programs, the same methodology can be used to allow IPC between multiple Xtensa target programs
      themselves each running on a different xtsc_core instance.  These xtsc_core instances can be running in the same
      XTSC simulation or they can be running in separate XTSC simulations on the same host.




Part D) Instructions:

Note: If you encounter problems on Linux, you may need to manually delete the shared memory files in /dev/shm.

1) Adjust common.h as required for your config and to enable/disable XTSC logging and printing results in dispatch.cpp.
2) Open 2 command shells on the same host (i.e. the same computer/workstation) and set up their environment for Xtensa
   tools and your chosen config and an appropriate host compiler.
3) Build the target program (dsp.out) and the host program (dispatch/dispatch.exe):
   Linux:
        make all
   MS Windows:
        xt-make all
   Note: The dispatch host program is not an XTSC program, the XTSC example Makefile's are used just for convenience.
4) In one command shell start dispatch and specify NumTasks and NumCores.  The dispatch program will created the shared
   memory used for host-Xtensa IPC and then wait for you to start the XTSC simulation(s).  
   Usage is:
        dispatch <NumTasks> <NumCores>
   Note: On Linux, if the current working directory is not on your path, then use "./dispatch" instead of "dispatch".
   For example, for 256 tasks on 8 cores (for accurate begin and end timestamps, do not hit enter until Step 6):
        dispatch 256 8
        Start XTSC systems(s) then Hit Enter to begin ...
5) In the other command shell start the XTSC simulation(s) using one of the following formats (shown for M=NumCores=8):
   a)  1 M-core system  using cycle-accurate ISS inside xtsc-run:
          xtsc-run -define=NumCores=8 -define=TURBO=0 -i=multicore.inc
   b)  M 1-core systems using cycle-accurate ISS inside xtsc-run:
          Linux bash:   for (( N=0; N<8; N=N+1 )); do xtsc-run -define=N=$N -define=TURBO=0 -i=coreN.inc&; done
          Linux csh:    foreach N (`seq 0 1 7`)
                          xtsc-run -define=N=$N -define=TURBO=0 -i=coreN.inc &
                        end
          MS Windows:   for /L %N in (0,1,7) do start xtsc-run -define=N=%N -define=TURBO=0 -i=coreN.inc
   c)  1 M-core system  using TurboXim inside xtsc-run:
          xtsc-run -define=NumCores=8 -define=TURBO=1 -i=multicore.inc
   d)  M 1-core systems using TurboXim inside xtsc-run:
          Linux bash:   for (( N=0; N<8; N=N+1 )); do xtsc-run -define=N=$N -define=TURBO=1 -i=coreN.inc&; done
          Linux csh:    foreach N (`seq 0 1 7`)
                          xtsc-run -define=N=$N -define=TURBO=1 -i=coreN.inc &
                        end
          MS Windows:   for /L %N in (0,1,7) do start xtsc-run -define=N=%N -define=TURBO=1 -i=coreN.inc
6) Back at the shell where dispatch is running, hit Enter to begin processing tasks.
7) After the dispatch program distributes the tasks and gets all the results back, it will print the start and finish
   timestamps, signal all the target programs to exit, and then prompt you to:
        Optionally inspect final state of shared memory then Hit Enter to finish ...
8) Optional:  Run a zombie XTSC simulation to inspect the final state of the IPC shared memory.  See Part E below.
9) Back at the shell where dispatch is running, hit Enter to finish.  After you hit enter, dispatch will release the host
   OS shared memory and exit.




Part E) Monitoring the IPC and inspecting final shared memory state using a zombie XTSC multicore simulation:

You can monitor the IPC in real time or inspect the final state of the shared memory before dispatch exits by running a
zombie XTSC simulation.   The zombie XTSC simulation is a single simulation with NumCores cores in it and an XTSC command
prompt from which you enter commands to inspect the shared memory without letting SystemC time advance.  The aliases file
has several aliases in it that you can use after sourcing it (". aliases NumCores=M").  For example:
        xtsc-run -define=NumCores=8 -i=multicore.inc -cmd
        cmd: . aliases NumCores=8
        cmd: aliases
        cmd: info

Note: On Linux, rlwrap can be quite useful when using the XTSC command prompt because it lets you easily repeat and/or
      edit commands.  For example:
        rlwrap xtsc-run -define=NumCores=8 -i=multicore.inc -cmd

Note: On Linux, you can also use a hex file viewer to monitor the IPC shared memory in /dev/shm; however, using a 
      zombie XTSC simulation makes it easy to match shared memory names and to view the read/write indices and
      buffers using the pre-defined aliases in the aliases file.




Part F) Things that affect run time and how to control them:

1) TurboXim vs cycle-accurate ISS (TurboXim is faster):
   Change by setting TURBO to 0 or 1 as shown Part D Step 5 above.
   Also see TURBO in multicore.inc and coreN.inc.

2) XTSC logging (logging off is faster).
   Change according to the LOGGING comments in common.h.
   Also see LOGGING in coreN.inc.

3) Printing task info in dispatch.cpp (not printing is faster).
   Change by defining PRINT_TASK_INFO or not in common.h.
   Also see PRINT_TASK_INFO in dispatch.cpp.

4) Number of free host processor cores used (more cores is faster).  See Part D Step 5-d.





References:
XTSC User's Guide (xtsc_ug.pdf):
  See Section 6.8 "Host OS Shared Memory in XTSC".
XTSC Reference Manual (xtsc_rm.pdf):
  See the sections on the xtsc_memory_parms and xtsc_memory classes.


Other shared memory examples:
../xtsc_memory.shared.memory.mutex/README.txt
../xtsc_queue.shared.memory/README.txt
../xtsc_wire.shared.memory/README.txt
../System.Control.and.Debug.Framework.UART/README.txt
