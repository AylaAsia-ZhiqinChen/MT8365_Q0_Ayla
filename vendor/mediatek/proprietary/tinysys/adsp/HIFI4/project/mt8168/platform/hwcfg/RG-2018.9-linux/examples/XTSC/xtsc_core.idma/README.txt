This example only illustrates making iDMA related connections to xtsc_core.  The target program itself does not exercise the iDMA; however,
if you have built the libidma examples in ../../libidma, you can run them in this system by any of the following methods:

Note: Use xt-make on MS Windows

1) Using xtsc-run:  Edit the value assigned to core_program in xtsc_core.idma.inc and run xtsc-run:
        make

2) Using sc_main.cpp: Edit the value assigned to core_program in xtsc_core.idma.inc and regenerated sc_main.cpp and compile and run it:
        make sc_main.cpp all run

3) Using sc_main.cpp: Edit the value in the call to load_program in the generated sc_main.cpp and then compile and run it:
        make all run

4) Using an already compiled sc_main.cpp: Override the target program at run time like this:
        ./xtsc_core.idma -SimTargetProgram=../../libidma/test_task_poll-os.test  
