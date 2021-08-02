// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#include "../common.inc"

#ifeq ($(XTSC_RUN_COSIM_VENDOR),synopsys)
#define READMEMH_FILE_NAME=
#define TOP=sYsTeMcToP
#define SYNOPSYS_NAME_DOUBLING
#else
#define READMEMH_FILE_NAME=<none>
#define TOP=sc_main
#endif

-set_core_parm=SimTargetOutput=core0_output.log
-set_core_parm=SimTargetProgram=$(XTSC_SCRIPT_FILE_PATH)/target/memory_test.out
-create_core=core0

-memory_pin_port=pif
-set_tlm2pin_parm=cosim=true
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_pifmem.so
#ifdef SYNOPSYS_NAME_DOUBLING
-set_tlm2pin_parm=dso_cookie=$(TOP).pif.pif
#else
-set_tlm2pin_parm=dso_cookie=$(TOP).pif
#endif
-create_tlm2pin=tlm2pin_pif
-set_proxy_parm=vcd_handle=waveforms
-set_proxy_parm=module_name=hifi4_Aquila_E2_PROD_pifmem
-set_proxy_parm=readmemh_file_name=$(READMEMH_FILE_NAME)
-set_proxy_parm=verilog_file=$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_pifmem.sv
-create_proxy=pif
-connect=core0,pif,slave_port,tlm2pin_pif
-connect=tlm2pin_pif,pif,,pif
-connect_clock_proxy=CLK,pif

-memory_pin_port=iram0
-set_tlm2pin_parm=cosim=true
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_iram0.so
#ifdef SYNOPSYS_NAME_DOUBLING
-set_tlm2pin_parm=dso_cookie=$(TOP).iram0.iram0
#else
-set_tlm2pin_parm=dso_cookie=$(TOP).iram0
#endif
-create_tlm2pin=tlm2pin_iram0
-set_proxy_parm=vcd_handle=waveforms
-set_proxy_parm=module_name=hifi4_Aquila_E2_PROD_iram0
-set_proxy_parm=readmemh_file_name=$(READMEMH_FILE_NAME)
-set_proxy_parm=verilog_file=$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_iram0.sv
-create_proxy=iram0
-connect=core0,iram0,slave_port,tlm2pin_iram0
-connect=tlm2pin_iram0,iram0,,iram0
-connect_clock_proxy=CLK,iram0

-memory_pin_port=iram1
-set_tlm2pin_parm=cosim=true
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_iram1.so
#ifdef SYNOPSYS_NAME_DOUBLING
-set_tlm2pin_parm=dso_cookie=$(TOP).iram1.iram1
#else
-set_tlm2pin_parm=dso_cookie=$(TOP).iram1
#endif
-create_tlm2pin=tlm2pin_iram1
-set_proxy_parm=vcd_handle=waveforms
-set_proxy_parm=module_name=hifi4_Aquila_E2_PROD_iram1
-set_proxy_parm=readmemh_file_name=$(READMEMH_FILE_NAME)
-set_proxy_parm=verilog_file=$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_iram1.sv
-create_proxy=iram1
-connect=core0,iram1,slave_port,tlm2pin_iram1
-connect=tlm2pin_iram1,iram1,,iram1
-connect_clock_proxy=CLK,iram1

-memory_pin_port=dram0
-set_tlm2pin_parm=cosim=true
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_dram0.so
#ifdef SYNOPSYS_NAME_DOUBLING
-set_tlm2pin_parm=dso_cookie=$(TOP).dram0.dram0
#else
-set_tlm2pin_parm=dso_cookie=$(TOP).dram0
#endif
-create_tlm2pin=tlm2pin_dram0
-set_proxy_parm=vcd_handle=waveforms
-set_proxy_parm=module_name=hifi4_Aquila_E2_PROD_dram0
-set_proxy_parm=readmemh_file_name=$(READMEMH_FILE_NAME)
-set_proxy_parm=verilog_file=$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_dram0.sv
-create_proxy=dram0
-connect=core0,dram0,slave_ports,tlm2pin_dram0
-connect=tlm2pin_dram0,dram0,,dram0
-connect_clock_proxy=CLK,dram0

-memory_pin_port=dram1
-set_tlm2pin_parm=cosim=true
-set_tlm2pin_parm=dso_name=$(XTSC_SCRIPT_FILE_PATH)/dso/$(XTSC_RUN_COSIM_VENDOR)/lib_hifi4_Aquila_E2_PROD_dram1.so
#ifdef SYNOPSYS_NAME_DOUBLING
-set_tlm2pin_parm=dso_cookie=$(TOP).dram1.dram1
#else
-set_tlm2pin_parm=dso_cookie=$(TOP).dram1
#endif
-create_tlm2pin=tlm2pin_dram1
-set_proxy_parm=vcd_handle=waveforms
-set_proxy_parm=module_name=hifi4_Aquila_E2_PROD_dram1
-set_proxy_parm=readmemh_file_name=$(READMEMH_FILE_NAME)
-set_proxy_parm=verilog_file=$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/hifi4_Aquila_E2_PROD_dram1.sv
-create_proxy=dram1
-connect=core0,dram1,slave_ports,tlm2pin_dram1
-connect=tlm2pin_dram1,dram1,,dram1
-connect_clock_proxy=CLK,dram1

