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
 *               -- Module: hifi4_Aquila_E2_PROD_iram1 --
 **********************************************************************/
module hifi4_Aquila_E2_PROD_iram1 (

	IRam1Busy,
	IRam1Data,
	IRam1Addr,
	IRam1En,
	IRam1Wr,
	IRam1WordEn,
	IRam1WrData,
	IRam1LoadStore,
	CLK);

output         IRam1Busy;     
output [127:0] IRam1Data;     
input  [11:0]  IRam1Addr;     
input          IRam1En;       
input          IRam1Wr;       
input  [3:0]   IRam1WordEn;   
input  [127:0] IRam1WrData;   
input          IRam1LoadStore; 
input          CLK;           


   parameter file_name = "readmemh.data/iram1.data";

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

    hifi4_Aquila_E2_PROD_iram1_xtmemory mem(
                        .CLK                (CLK),
                        .address0           (IRam1Addr),
                        .enable0            (IRam1En),
                        .wordEn0            (IRam1WordEn),
                        .writeData0         (IRam1WrData),
                        .writeEnable0       (IRam1Wr),
                        .xtout0             (IRam1Data)
                        );


assign IRam1Busy = 1'b0;


  int mem_beg  =  32'h40010000;
  int mem_end  =  32'h4001ffff;

`ifdef MODEL_TECH
  export "DPI-SC" function peek_hifi4_Aquila_E2_PROD_iram1;
`else
  export "DPI-C" function peek_hifi4_Aquila_E2_PROD_iram1;
`endif
  function int peek_hifi4_Aquila_E2_PROD_iram1;
    input int unsigned address;
    inout int unsigned byte_lanes_3210;
    reg [127:0] peek_data;
    int unsigned index;
    int unsigned word;
    word = address[3:2];
    peek_hifi4_Aquila_E2_PROD_iram1 = 1;
    if (!(address & 2'h3) && ((address >= mem_beg) && (address <= mem_end))) begin
      peek_hifi4_Aquila_E2_PROD_iram1 = 0;
      index = (address - mem_beg) >> 4;
      peek_data = mem.dataArray[index];
      byte_lanes_3210 = (peek_data >> (word * 32)) & 32'hFFFFFFFF;
`ifdef LOG_PEEK_POKE
      $display("%t %m: peek addr=0x%8x data=0x%8x index=0x%8x", $time, address, byte_lanes_3210, index);
`endif
    end
  endfunction


`ifdef MODEL_TECH
  export "DPI-SC" function poke_hifi4_Aquila_E2_PROD_iram1;
`else
  export "DPI-C" function poke_hifi4_Aquila_E2_PROD_iram1;
`endif
  function int poke_hifi4_Aquila_E2_PROD_iram1;
    input int unsigned address;
    input int unsigned byte_lanes_3210;
    reg [127:0] poke_data;
    int unsigned index;
    int unsigned word;
    word = address[3:2];
    poke_hifi4_Aquila_E2_PROD_iram1 = 1;
    if (!(address & 2'h3) && ((address >= mem_beg) && (address <= mem_end))) begin
      index = (address - mem_beg) >> 4;
      poke_data = mem.dataArray[index];
           if (word ==  0) poke_data = { poke_data[127: 32], byte_lanes_3210                   };
      else if (word ==  1) poke_data = { poke_data[127: 64], byte_lanes_3210, poke_data[ 31:0] };
      else if (word ==  2) poke_data = { poke_data[127: 96], byte_lanes_3210, poke_data[ 63:0] };
      else                 poke_data = {                     byte_lanes_3210, poke_data[ 95:0] };
      poke_hifi4_Aquila_E2_PROD_iram1 = 0;
      mem.dataArray[index] = poke_data;
    end
`ifdef LOG_PEEK_POKE
    $display("%t %m: poke addr=0x%8x data=0x%8x, index=0x%8x", $time, address, byte_lanes_3210, index);
`endif
  endfunction


endmodule




/**********************************************************************
 *          -- Module: hifi4_Aquila_E2_PROD_iram1_xtmemory --
 **********************************************************************/
module hifi4_Aquila_E2_PROD_iram1_xtmemory (

	CLK,
	enable0,
	address0,
	writeData0,
	writeEnable0,
	wordEn0,
	xtout0);

parameter words=4,
	nwords=4096,
	width=128,
	awidth=12;

input               CLK;         
input               enable0;     
input  [awidth-1:0] address0;    
input  [width-1:0]  writeData0;  
input               writeEnable0; 
input  [words-1:0]  wordEn0;     
output [width-1:0]  xtout0;      

reg    [width-1:0]  dataArray [0:nwords-1];

reg    [width-1:0]  data0;
reg    [width-1:0]  data0_M;
reg                 enable20;

reg                 enable30;
reg    [width-1:0]  data20;

wire   [awidth-1:0] fixed_address0 = address0;

   //   --- Write --- 
   //   #0 to delay 1 delta cycle so a Read this cycle will return old data
   always @(posedge CLK) #0 begin
      if (enable0 && writeEnable0) begin
            data0_M = dataArray[fixed_address0];
            if (wordEn0[0]) begin
              data0_M[31:0] = writeData0[31:0];
            end
            if (wordEn0[1]) begin
              data0_M[63:32] = writeData0[63:32];
            end
            if (wordEn0[2]) begin
              data0_M[95:64] = writeData0[95:64];
            end
            if (wordEn0[3]) begin
              data0_M[127:96] = writeData0[127:96];
            end
            dataArray[fixed_address0] = data0_M;
      end
   end

   //   --- Read ---
   always @(posedge CLK) begin
      enable20 <= #1 enable0;
      if (enable0) begin
        data0 <= #1 dataArray[fixed_address0];
      end
   end

   always @(posedge CLK) begin
        enable30 <= #1 enable20;
        data20   <= #1 data0;
   end
   assign xtout0 = data20;


endmodule


