                                   xtsc_memory.shared.memory.mutex Example


Note:  Host mutex support in XTSC is an experimental feature.

Note:  See Part G) Limitations, Caveats, and Recommendations When Using "host_mutex"


Part A) Background:

When the "host_shared_memory" parameter is true, XTSC causes the backing store of an memory instance to be host OS named
shared memory.  This allows a system comprised of multiple Xtensa cores to be simulated using multiple host OS processes
which can greatly speed up simulation on a multiple core workstation.  

One issue that can arise when specifying "host_shared_memory" by itself is that the atomic instructions of Xtensa (S32C1I
and L32EX/S32EX) are not reliable because a second host process may modify the contents of the atomic address while the
first host process is performing the atomic operation (so that the operation is, in fact, no longer atomic).  To remedy
this the "host_mutex" parameter may also be specified for memory instances containing addresses targeted by Xtensa atomic
instructions.  The effect of this parameter is to cause the memory instance to create an xtsc_host_mutex that it locks
during all writes and pokes (which take place in 0 SystemC time) and for the duration of all atomic operations (which
span multiple clock periods).




Part B) Overview:

This project illustrates using the "host_mutex" parameter of xtsc_memory in combination with its "host_shared_memory"
parameter to support Xtensa atomic instructions (S32C1I or L32EX/S32EX) to system memory modeled using host OS named
shared memory which is accessed by multiple host processes.

The project runs from 1 to 256 sub-systems in parallel (see Part C below).  The number of sub-systems is specified by
NumCores.  Each sub-system has one xtsc_core instance (coreNN) and one private system memory (sysmemNN).  They all also
have two additional xtsc_memory instances which have the same name in all subsystems (shmem_lock and shmem_work).  The
shmem_work instances all have "host_shared_memory" set to true so that they are all backed by the same host OS named
shared memory in workstation RAM.  The same is true for shmem_lock.  It addition, all the shmem_lock instances have
"host_mutex" set to true because Xtensa atomic instructions will be used to target a location in shmem_lock.

All sub-systems run the same target program that, briefly, first locks access to a common work area, then modifies
the work area, then checks its modifications remain intact, and then finally releases the common work area.  This whole
process is repeated some number of times.

In more detail the system setup and what the target program of each sub-system does is:
1.  At system startup, the address specified as the lock address contains the value 0 which indicates the common work
    area is free.  See SHMEM_LOCK_BASE in common.h and lock_addr in target/main.c.
2.  Upon entry to main, the target program extracts the following run-time parameters from argv[]:
    argv[1]:  N, its core index, which it uses to compute a personalized watermark equal to 0xBABE0000 + N.
    argv[2]:  TOTAL, the number of iterations to do (0 means to run forever).
    argv[3]:  WORDS, the number of u32 words to write to the work area (between 1 and 256).
    argv[4]:  CHECKS, the number of times to check the work area for consistency (between 1 and 256).
3.  The program then repeats the follow steps a number of times as specified by TOTAL.
    a.  The program uses Xtensa atomic instructions to try to change the value at lock_addr from 0 to its personalized
        watermark.  This step is repeated until it succeeds.
    b.  The program then stores its watermark and which iteration it is on in the flag area.  See SHMEM_WORK_FLAGS in
        common.h and flag_area in target/main.c.
    c.  The program then writes its watermark to the work area at the number of locations specified by WORDS.
    d.  The program then reads back all the values just written to the work area to check that they haven't changed (for
        example, by some other sub-system that also thinks it has access to the work area).  If any errors are discovered,
        the program writes the error count to the error area.  See SHMEM_WORK_ERRORS in common.h and error_area in
        target/main.c.  This step is repeated the number of times specified by CHECKS.
    e.  The program then releases its lock on the work area by writing 0 to lock_addr.


Note: This project is meant to illustrate how to use the "host_mutex" feature.  It is not meant to illustrate how to do
      parallel processing.  On the contrary, it is a good example of how NOT to do parallel processing because only one
      core at a time is ever allowed to do its work!




Part C) System diagram of sub-system #NN (NN is 00 or 01 or ... or FF, the upper limit being NumCores-1) - See coreNN.inc:



   xtsc_wire_source              xtsc_core           xtsc_router           xtsc_memory           xtsc_wire
=======================       ===============        ============        ===============      ===============


                              /-------------\           /-------|        /-------------\
                              |   coreNN    |          /       0|<======>|  sysmemNN   |
/---------------------\       |             |         /         |        \-------------/
| coreNN_BInterruptXX*|======>|BInterrupt00 |        | routerNN |
\---------------------/       |             |        |          |        /------------\
                              |          PIF|<======>|         1|<======>| shmem_lock*|
                              |             |        |          |        \------------/
                              |             |        |          |
                              |             |         \         |        /------------\
                              |             |          \       2|<======>| shmem_work*|
                              |             |           \-------|        \------------/
                              |             |
                              |             |                                                 /-------------\
                              |    PWaitMode|================================================>| PWaitModeNN*|
                              |             |                                                 \-------------/
                              |   main.out  |
                              \-------------/


coreNN_BInterruptXX*: This xtsc_wire_source asserts a level-sensitive interrupt to coreNN whenever the work area is free
                      (that is when lock_addr contains 0).  In a real system the interrupt would probably be initiated by
                      GPIO from the core relinguishing the work area.  That could easily be modelled directly in XTSC;
                      however, even then when using separate host processes one would need an xtsc_wire_source (or
                      something like it) to monitor the GPIO for changes and drive the interrupt.  
                      See the Lua script in coreNN_BInterruptXX.vec.

PWaitModeNN*: This xtsc_wire is not required.  It is included in the system just to provide a clear indication in the
              xtsc-*.log file of when coreNN is waiting for an interrupt.

shmem_lock*: Each shmem_lock is backed by the same 256 bytes of host OS shared memory (see SHMEM_LOCK_BASE and
             SHMEM_LOCK_SIZE in common.h and host_shared_memory and host_mutex in coreNN.inc).

shmem_work*: Each shmem_work is backed by the same 4 KB of host OS shared memory (see SHMEM_WORK_BASE and SHMEM_WORK_SIZE
             in common.h and host_shared_memory in coreNN.inc).

On Linux you can find the shared memory files for user joeuser (for example) at:
           /dev/shm/joeuser.shmem_lock
           /dev/shm/joeuser.shmem_work
And the semaphore file used for xtsc_host_mutex at:
           /dev/shm/sem.joeuser.shmem_lock





Part D) Instructions:

Note: If you encounter problems on Linux, you may need to manually delete the shared memory files in /dev/shm.

1) Adjust common.h if required for your config.
2) Open a command shell and set up its environment for Xtensa tools and your chosen config.
3) Build the target program (main.out):
   Linux:
        make target
   MS Windows:
        xt-make target
4) Using discretion for your particular situation!!!!, do any required clean-up of previous runs:
   Linux:
        rm go
        rm xtsc-*.log
        rm /dev/shm/*shmem_lock
        rm /dev/shm/*shmem_work
        killall xtsc-run
   MS Windows:
        del stop
        del go
        del xtsc-*.log
5) Start the XTSC simulation(s) using one of the following formats (shown for M=NumCores=4):
   a)  1 M-core system  using cycle-accurate ISS inside xtsc-run:
          xtsc-run -define=NumCores=4 -define=TURBO=0 -i=multicore.inc
   b)  M 1-core systems using cycle-accurate ISS inside xtsc-run:
          Linux bash:   for (( N=0; N<4; N=N+1 )); do xtsc-run -define=NumCores=4 -define=N=$N -define=TURBO=0 -i=coreNN.inc&; done
                        touch go
          Linux csh:    foreach N (`seq 0 1 3`)
                          xtsc-run -define=NumCores=4 -define=N=$N -define=TURBO=0 -i=coreNN.inc &
                        end
                        touch go
          MS Windows:   for /L %N in (0,1,3) do start xtsc-run -define=NumCores=4 -define=N=%N -define=TURBO=0 -i=coreNN.inc
                        copy nul go
   c)  1 M-core system  using TurboXim inside xtsc-run:
          xtsc-run -define=NumCores=4 -define=TURBO=1 -i=multicore.inc
   d)  M 1-core systems using TurboXim inside xtsc-run:
          Linux bash:   for (( N=0; N<4; N=N+1 )); do xtsc-run -define=NumCores=4 -define=N=$N -define=TURBO=1 -i=coreNN.inc&; done
                        touch go
          Linux csh:    foreach N (`seq 0 1 3`)
                          xtsc-run -define=NumCores=4 -define=N=$N -define=TURBO=1 -i=coreNN.inc &
                        end
                        touch go
          MS Windows:   for /L %N in (0,1,3) do start xtsc-run -define=NumCores=4 -define=N=%N -define=TURBO=1 -i=coreNN.inc
                        copy nul go
6) Optional:  Run a zombie XTSC simulation to inspect the final state of the shared memories and mutex.  See Part F below.




Part E) Run Variations and Miscellaneous Info

- Commands can be passed to xtsc-run to control various things:
  - To use TurboXim:                            -define=TURBO=1
  - To do 10 iterations (default 5):            -define=TOTAL=10
       or to run forever:                       -define=TOTAL=0
  - To write 4 words to work area (default 16): -define=WORDS=4
  - To do check 5 times (default 10):           -define=CHECK=5
  - To disable XTSC logging:                    -define=LOGGING=0
                     better:                    -define=LOGGING=elab_only

- When doing multiple single-core simulations (5-b or 5-d in Part D above), the eoe.lua script is run which does two
  things:
  1.  It synchronizes the host processes so they all start the simulation phase at near the same time.  It does this by
      holdiing each sub-system in the end_of_elaboration() callback until a file named go is found in the current working
      directory.  
  2.  It creates a watchfilter that causes the XTSC cmd prompt to come up for that sub-system if coreNN does a write to
      error_addr.  See eoe.lua, coreNN.inc, and target/main.c.

- Also when doing multiple single-core simulations, the eos.lua script is run during the end_of_simulation callback.  On
  MS Windows, this script holds the last sub-system created (which is not necessarily the last one to finish) in the
  end_of_simulation callback until a file named stop is found in the current working directory.  This is to allow you
  the opportunity to connect a zombie sub-system to inspect the final state of shared memory (see Part F below).  This is
  needed on MS Windows because the shared memory will be removed by the OS once the last process that has it opens
  terminates.  On Linux, the eos.lua script doesn't do anything because Linux will leave the shared memory in place until
  the user explicitly deletes it.  See eos.lua and coreNN.inc.

- XTSC logging with multiple simultaneous simulations:  In this project, because multiple host processes are running at
  the same time and in the same directory, we need a way to tell XTSC to use a different file name for each XTSC log
  file.  This can easily be done using the XTSC_LOG_UNIQUIFIER feature.  See XTSC_LOG_UNIQUIFIER in TextLogger.txt.




Part F) Monitoring progress and inspecting final state of shared memory and mutex using a zombie XTSC simulation:

You can monitor the shared memory in real time or inspect its final state by running a zombie XTSC simulation (on MS
Windows this needs to be done before the final sub-system exits).   The zombie XTSC simulation is a single sub-system
with an XTSC command prompt from which you enter commands to inspect the shared memory without letting SystemC time
advance.  The alias file has several aliases in it that you can use after sourcing it.  For example:
        xtsc-run -define=NumCores=4 -define=N=1 -i=coreNN.inc -cmd
        cmd: . aliases
        cmd: aliases

Note: On Linux, rlwrap can be quite useful when using the XTSC command prompt because it lets you easily repeat and/or
      edit commands.  For example:
        rlwrap xtsc-run -define=NumCores=4 -define=N=1 -i=coreNN.inc -cmd

Note: On Linux, you can also use a hex file viewer to monitor the shared memory in /dev/shm; however, using a zombie XTSC
      simulation makes it easy to match shared memory names and to view the content using the pre-defined aliases in the
      aliases file.




Part G) Limitations, Caveats, and Recommendations When Using "host_mutex"

- Simulation deadlock is possible whenever multiple mutexes are used.  To avoid potential simulation deadlock, Cadence
  recommends that at most one mutex be used per host OS simulation process.

- When using "host_mutex" with an exclusive Ld/St config, the L32EX/S32EX atomic access instructions must always execute
  in pairs.  This is a limitation of the "host_mutex" feature and is not a requirment of the L32EX/S32EX protocol itself.
  Because of this you may need to write special atomic access functions that comply with this limitation in order to use
  this feature.  For an example, see atomic_write() in target/main.c.

- The "host_mutex" parameter of xtsc_memory does not work when the memory is a local DataRAM of one core that is targeted
  by L32EX/S32EX instructions of another core because exclusive Ld/St configs do not have a DataRAM lock signal.

- Using "host_mutex" with TurboXim requires that fast access be denied to all atomic access addresses.  The suggested way
  to do this is to locate all atomic addessses in a single, small xtsc_memory instance configured with "use_fast_access"
  false.

- There is a problem using TurboXim when you have S32C1I to an address in another cores DataRAM, because TurboXim does
  not support inbound PIF requests to local memory locations that do not support fast access.  If the xtsc_core instance
  receiving the inbound PIF request is a zombie core, then you can replace it with a xtsc_router that connects the
  requests directly to the xtsc_memory instances modelling the local memories.

- There is a potentially large simulation performance impact if a large, frequently-written memory is locked.  The
  suggested way to reduce the simulation performance impact (while avoiding potential simulation deadlock) is to locate
  all atomic addessses in a single, small xtsc_memory instance.

- Regardless of whether or not "host_mutex" is being used, interrupts which cross between host processes require special
  handling.  For edge-sensitive interrupts, something must do done (such as queueing them up) to ensure they are neither
  missed nor appear hung on the receiving system side.  In addition, for both level and edge-sensitive interrupts,
  something must be done (such as polling) to ensure they get driven into the receiving core.




References:
XTSC User's Guide (xtsc_ug.pdf):
  See Section 6.8 "Host OS Shared Memory in XTSC".
XTSC Reference Manual (xtsc_rm.pdf):
  See the sections on the xtsc_memory_parms and xtsc_memory classes.


Other shared memory examples:
../xtsc_memory.shared.memory.dispatch/README.txt
../xtsc_queue.shared.memory/README.txt
../xtsc_wire.shared.memory/README.txt
../System.Control.and.Debug.Framework.UART/README.txt

