// Customer ID=13943; Build=0x75f5e; Copyright (c) 2003-2015 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
//
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.



//`define LOG_PEEK_POKE



/**********************************************************************
 *               -- Module: hifi4_Aquila_E2_PROD_dram1 --
 **********************************************************************/
module hifi4_Aquila_E2_PROD_dram1 (

	DRam1BusyB0,
	DRam1DataB0,
	DRam1BusyB1,
	DRam1DataB1,
	DRam1AddrB0,
	DRam1EnB0,
	DRam1WrB0,
	DRam1ByteEnB0,
	DRam1WrDataB0,
	DRam1AddrB1,
	DRam1EnB1,
	DRam1WrB1,
	DRam1ByteEnB1,
	DRam1WrDataB1,
	CLK);

output        DRam1BusyB0;  
output [63:0] DRam1DataB0;  
output        DRam1BusyB1;  
output [63:0] DRam1DataB1;  
input  [13:0] DRam1AddrB0;  
input         DRam1EnB0;    
input         DRam1WrB0;    
input  [7:0]  DRam1ByteEnB0; 
input  [63:0] DRam1WrDataB0; 
input  [13:0] DRam1AddrB1;  
input         DRam1EnB1;    
input         DRam1WrB1;    
input  [7:0]  DRam1ByteEnB1; 
input  [63:0] DRam1WrDataB1; 
input         CLK;          


   parameter file_name = "readmemh.data/dram1.data";

   initial begin
    if ($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
   end

   initial begin
     repeat (1) @(posedge CLK);
     if ($test$plusargs("preloadmems") && (file_name != "<none>")) begin
       $display("%t Preloading memory %m from %s", $time, file_name);
       $readmemh(file_name, mem.dataArray);
     end
   end

    hifi4_Aquila_E2_PROD_dram1_xtmemory mem(
                        .CLK                (CLK),
                        .address0           (DRam1AddrB0),
                        .enable0            (DRam1EnB0),
                        .byteEn0            (DRam1ByteEnB0),
                        .writeData0         (DRam1WrDataB0),
                        .writeEnable0       (DRam1WrB0),
                        .xtout0             (DRam1DataB0),
                        .address1           (DRam1AddrB1),
                        .enable1            (DRam1EnB1),
                        .byteEn1            (DRam1ByteEnB1),
                        .writeData1         (DRam1WrDataB1),
                        .writeEnable1       (DRam1WrB1),
                        .xtout1             (DRam1DataB1)
                        );


assign DRam1BusyB0 = 1'b0;
assign DRam1BusyB1 = 1'b0;


  int mem_beg  =  32'h1e040000;
  int mem_end  =  32'h1e07ffff;

`ifdef MODEL_TECH
  export "DPI-SC" function peek_hifi4_Aquila_E2_PROD_dram1;
`else
  export "DPI-C" function peek_hifi4_Aquila_E2_PROD_dram1;
`endif
  function int peek_hifi4_Aquila_E2_PROD_dram1;
    input int unsigned address;
    inout int unsigned byte_lanes_3210;
    reg [63:0] peek_data;
    int unsigned index;
    int unsigned word;
    word = address[2:2];
    peek_hifi4_Aquila_E2_PROD_dram1 = 1;
    if (!(address & 2'h3) && ((address >= mem_beg) && (address <= mem_end))) begin
      peek_hifi4_Aquila_E2_PROD_dram1 = 0;
      index = (address - mem_beg) >> 3;
      peek_data = mem.dataArray[index];
      byte_lanes_3210 = (peek_data >> (word * 32)) & 32'hFFFFFFFF;
`ifdef LOG_PEEK_POKE
      $display("%t %m: peek addr=0x%8x data=0x%8x index=0x%8x", $time, address, byte_lanes_3210, index);
`endif
    end
  endfunction


`ifdef MODEL_TECH
  export "DPI-SC" function poke_hifi4_Aquila_E2_PROD_dram1;
`else
  export "DPI-C" function poke_hifi4_Aquila_E2_PROD_dram1;
`endif
  function int poke_hifi4_Aquila_E2_PROD_dram1;
    input int unsigned address;
    input int unsigned byte_lanes_3210;
    reg [63:0] poke_data;
    int unsigned index;
    int unsigned word;
    word = address[2:2];
    poke_hifi4_Aquila_E2_PROD_dram1 = 1;
    if (!(address & 2'h3) && ((address >= mem_beg) && (address <= mem_end))) begin
      index = (address - mem_beg) >> 3;
      poke_data = mem.dataArray[index];
           if (word ==  0) poke_data = { poke_data[ 63: 32], byte_lanes_3210                   };
      else                 poke_data = {                     byte_lanes_3210, poke_data[ 31:0] };
      poke_hifi4_Aquila_E2_PROD_dram1 = 0;
      mem.dataArray[index] = poke_data;
    end
`ifdef LOG_PEEK_POKE
    $display("%t %m: poke addr=0x%8x data=0x%8x, index=0x%8x", $time, address, byte_lanes_3210, index);
`endif
  endfunction


endmodule




/**********************************************************************
 *          -- Module: hifi4_Aquila_E2_PROD_dram1_xtmemory --
 **********************************************************************/
module hifi4_Aquila_E2_PROD_dram1_xtmemory (

	CLK,
	enable0,
	address0,
	writeData0,
	writeEnable0,
	byteEn0,
	xtout0,
	enable1,
	address1,
	writeData1,
	writeEnable1,
	byteEn1,
	xtout1);

parameter bytes=8,
	nwords=32768,
	width=64,
	awidth=14;

input               CLK;         
input               enable0;     
input  [awidth-1:0] address0;    
input  [width-1:0]  writeData0;  
input               writeEnable0; 
input  [bytes-1:0]  byteEn0;     
output [width-1:0]  xtout0;      
input               enable1;     
input  [awidth-1:0] address1;    
input  [width-1:0]  writeData1;  
input               writeEnable1; 
input  [bytes-1:0]  byteEn1;     
output [width-1:0]  xtout1;      

reg    [width-1:0]  dataArray [0:nwords-1];

reg    [width-1:0]  data0;
reg    [width-1:0]  data0_M;
reg                 enable20;

reg                 enable30;
reg    [width-1:0]  data20;

reg    [width-1:0]  data1;
reg    [width-1:0]  data1_M;
reg                 enable21;

reg                 enable31;
reg    [width-1:0]  data21;

wire [awidth+1-1:0] fixed_address0 = (address0 << 1) | 0;
wire [awidth+1-1:0] fixed_address1 = (address1 << 1) | 1;

   //   --- Write --- 
   //   #0 to delay 1 delta cycle so a Read this cycle will return old data
   always @(posedge CLK) #0 begin
      if (enable0 && writeEnable0) begin
            data0_M = dataArray[fixed_address0];
            if (byteEn0[0]) begin
              data0_M[7:0] = writeData0[7:0];
            end
            if (byteEn0[1]) begin
              data0_M[15:8] = writeData0[15:8];
            end
            if (byteEn0[2]) begin
              data0_M[23:16] = writeData0[23:16];
            end
            if (byteEn0[3]) begin
              data0_M[31:24] = writeData0[31:24];
            end
            if (byteEn0[4]) begin
              data0_M[39:32] = writeData0[39:32];
            end
            if (byteEn0[5]) begin
              data0_M[47:40] = writeData0[47:40];
            end
            if (byteEn0[6]) begin
              data0_M[55:48] = writeData0[55:48];
            end
            if (byteEn0[7]) begin
              data0_M[63:56] = writeData0[63:56];
            end
            dataArray[fixed_address0] = data0_M;
      end
      if (enable1 && writeEnable1) begin
            data1_M = dataArray[fixed_address1];
            if (byteEn1[0]) begin
              data1_M[7:0] = writeData1[7:0];
            end
            if (byteEn1[1]) begin
              data1_M[15:8] = writeData1[15:8];
            end
            if (byteEn1[2]) begin
              data1_M[23:16] = writeData1[23:16];
            end
            if (byteEn1[3]) begin
              data1_M[31:24] = writeData1[31:24];
            end
            if (byteEn1[4]) begin
              data1_M[39:32] = writeData1[39:32];
            end
            if (byteEn1[5]) begin
              data1_M[47:40] = writeData1[47:40];
            end
            if (byteEn1[6]) begin
              data1_M[55:48] = writeData1[55:48];
            end
            if (byteEn1[7]) begin
              data1_M[63:56] = writeData1[63:56];
            end
            dataArray[fixed_address1] = data1_M;
      end
   end

   //   --- Read ---
   always @(posedge CLK) begin
      enable20 <= #1 enable0;
      if (enable0) begin
        data0 <= #1 dataArray[fixed_address0];
      end
      enable21 <= #1 enable1;
      if (enable1) begin
        data1 <= #1 dataArray[fixed_address1];
      end
   end

   always @(posedge CLK) begin
        enable30 <= #1 enable20;
        data20   <= #1 data0;
   end
   assign xtout0 = data20;

   always @(posedge CLK) begin
        enable31 <= #1 enable21;
        data21   <= #1 data1;
   end
   assign xtout1 = data21;


endmodule


