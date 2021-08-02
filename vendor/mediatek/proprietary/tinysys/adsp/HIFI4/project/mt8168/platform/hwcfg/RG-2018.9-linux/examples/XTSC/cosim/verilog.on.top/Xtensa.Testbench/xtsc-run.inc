// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#environment
#ifeq ($(XTSC_RUN_COSIM_USE_DSO),)
#define EXTRA_PLUS_ARGS=+preloadmems
#endif

#include "../common.inc"

-set_make_parm=testbench_name=Testbench
-set_make_parm=verilog_files=$(XTSC_SCRIPT_FILE_PATH)/Testbench.v,$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_pifmem.sv,$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_iram0.sv,$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_iram1.sv,$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_dram0.sv,$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_dram1.sv

#include "../extra.wrapper.IO.inc"
-set_wrapper_parm=extra_inputs=$(EXTRA_INPUTS)
-set_wrapper_parm=extra_outputs=$(EXTRA_OUTPUTS)
-set_wrapper_parm=vcd_handle=waveforms
-set_wrapper_parm=fully_connect_cores=core0
#ifeq ($(XTSC_RUN_COSIM_USE_DSO),)
-set_wrapper_parm=readmemh_cores=core0
-set_wrapper_parm=shadow_memory=true
#endif
-create_wrapper=Xm_hifi4_Aquila_E2_PRODXtensa

-set_core_parm=SimTargetProgram=$(XTSC_SCRIPT_FILE_PATH)/target/memory_test.out
-set_core_parm=SimTargetOutput=core0_output.log
-set_core_parm=SimLogDisassembly=true
-set_core_parm=SimPinLevelInterfaces=*
-create_core=core0


#ifneq ($(XTSC_RUN_COSIM_USE_DSO),)

-memory_pin_port=pif
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_pifmem.so
-set_tlm2pin_parm=dso_cookie=Testbench.xtensa0.pif
-set_tlm2pin_parm=cosim=true
-create_tlm2pin=tlm2pin_pif
-connect=core0,pif,slave_ports,tlm2pin_pif
-connect=tlm2pin_pif,pif,,Xm_hifi4_Aquila_E2_PRODXtensa

-memory_pin_port=iram0
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_iram0.so
-set_tlm2pin_parm=dso_cookie=Testbench.xtensa0.iram0
-set_tlm2pin_parm=cosim=true
-create_tlm2pin=tlm2pin_iram0
-connect=core0,iram0,slave_ports,tlm2pin_iram0
-connect=tlm2pin_iram0,iram0,,Xm_hifi4_Aquila_E2_PRODXtensa

-memory_pin_port=iram1
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_iram1.so
-set_tlm2pin_parm=dso_cookie=Testbench.xtensa0.iram1
-set_tlm2pin_parm=cosim=true
-create_tlm2pin=tlm2pin_iram1
-connect=core0,iram1,slave_ports,tlm2pin_iram1
-connect=tlm2pin_iram1,iram1,,Xm_hifi4_Aquila_E2_PRODXtensa

-memory_pin_port=dram0
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_dram0.so
-set_tlm2pin_parm=dso_cookie=Testbench.xtensa0.dram0
-set_tlm2pin_parm=cosim=true
-create_tlm2pin=tlm2pin_dram0
-connect=core0,dram0,slave_ports,tlm2pin_dram0
-connect=tlm2pin_dram0,dram0,,Xm_hifi4_Aquila_E2_PRODXtensa

-memory_pin_port=dram1
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_dram1.so
-set_tlm2pin_parm=dso_cookie=Testbench.xtensa0.dram1
-set_tlm2pin_parm=cosim=true
-create_tlm2pin=tlm2pin_dram1
-connect=core0,dram1,slave_ports,tlm2pin_dram1
-connect=tlm2pin_dram1,dram1,,Xm_hifi4_Aquila_E2_PRODXtensa

#endif
