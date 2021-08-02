// Customer ID=13943; Build=0x75f5e; Copyright (c) 2008-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.




//`define LOG_PEEK_POKE


// To enable route ID, uncomment the following and change xxx to the number of bits minus 1
// `define hifi4_Aquila_E2_PROD_idma0_ROUTE_ID



module hifi4_Aquila_E2_PROD_idma0(
  CLK,

  POReqValid_iDMA,
  PIReqRdy_iDMA,
  POReqCntl_iDMA,
  POReqAdrs_iDMA,
  POReqData_iDMA,
  POReqDataBE_iDMA,
  POReqId_iDMA,
  POReqPriority_iDMA,
  POReqAttribute_iDMA,

  PIRespValid_iDMA,
  PORespRdy_iDMA,
  PIRespCntl_iDMA,
  PIRespData_iDMA,
  PIRespId_iDMA,
  PIRespPriority_iDMA

`ifdef hifi4_Aquila_E2_PROD_idma0_ROUTE_ID
  ,
  POReqRouteId_iDMA,
  PIRespRouteId_iDMA
`endif
);

  input                 CLK;

  input                 POReqValid_iDMA;
  output                PIReqRdy_iDMA;
  input         [  7:0] POReqCntl_iDMA;
  input         [ 31:0] POReqAdrs_iDMA;
  input         [ 63:0] POReqData_iDMA;
  input         [  7:0] POReqDataBE_iDMA;
  input         [  5:0] POReqId_iDMA;
  input         [  1:0] POReqPriority_iDMA;
  input         [ 11:0] POReqAttribute_iDMA;

  output                PIRespValid_iDMA;
  input                 PORespRdy_iDMA;
  output        [  7:0] PIRespCntl_iDMA;
  output        [ 63:0] PIRespData_iDMA;
  output        [  5:0] PIRespId_iDMA;
  output        [  1:0] PIRespPriority_iDMA;

`ifdef hifi4_Aquila_E2_PROD_idma0_ROUTE_ID
  // Change xxx to the number of bits minus 1
  input         [xxx:0] POReqRouteId_iDMA;
  output        [xxx:0] PIRespRouteId_iDMA;
  reg           [xxx:0] PIRespRouteId_iDMA           =  0;
`endif

  reg           [  7:0] PIRespCntl_iDMA              =   8'h0;
  reg           [  5:0] PIRespId_iDMA                =   6'h0;
  reg           [  1:0] PIRespPriority_iDMA          =   2'h0;
  reg                   PIReqRdy_iDMA                =   1'h0;
  reg                   PIRespValid_iDMA             =   1'h0;

  reg                   sysram_write                 =   1'h0;
  reg           [ 27:0] sysram_index                 =  28'h0;
  reg           [  7:0] sysram_byte_enables          =   8'h0;
  reg           [ 63:0] sysram_write_data            =  64'h0;
  wire          [ 63:0] sysram_read_data;

  reg           [  0:0] sysrom_index                 =   1'h0;
  reg           [  7:0] sysrom_byte_enables          =   8'h0;
  wire          [ 63:0] sysrom_read_data;

  parameter file_name = "<default>";



  integer               bus_byte_width          = 8;

  integer               READ                    =   4'b0000;
  integer               WRITE                   =   4'b1000;
  integer               BLOCK_READ              =   4'b0001;
  integer               BURST_READ              =   4'b0011;
  integer               BLOCK_WRITE             =   4'b1001;
  integer               BURST_WRITE             =   4'b1011;
  integer               RCW                     =   4'b0101;

  integer               RSP_NONE                =   8'hFF;       // Value to drive when there is no response
  integer               RSP_OK                  =   7'b0000000_; // PIRespCntl_iDMA[7:1]
  integer               RSP_ADDR_ERROR          =   7'b0000001_; // PIRespCntl_iDMA[7:1]

  integer               request_prev            =   1'h0;        // There was a request on the prev cycle which was accepted
  integer               request_last            =   1'h0;        // The accepted request was a last-transfer request
  integer               num_responses           =   0;           // Num of responses expected due to accepted last-xfer req
  integer               response_prev           =   1'h0;        // There was a response on the prev cycle which was accepted
  integer               response_count          =   0;           // Count down of remaining responses

  integer               sysram_hit              =   1'h0;        // Request accepted last cycle targets sysram
  integer               sysram_start            =  32'h40020000; // Start address of sysram
  integer               sysram_end              =  32'hC001FFFF; // End address of sysram

  integer               sysrom_hit              =   1'h0;        // Request accepted last cycle targets sysrom
  integer               sysrom_start            =  32'h00000000; // Start address of sysrom
  integer               sysrom_end              =  32'hFFFFFFFF; // End address of sysrom

  integer unsigned      address                 =  32'h00000000; // Save POReqAdrs_iDMA of "first" transfer
  integer               error_resp_pending      =   1'h0;        // Keep track until last transfer request that error response is due 
  integer               first_transfer          =   1'h1;        // The next request to come will be a "first" transfer
  integer               write_beat              =   5'h0;        // Keep track of BLOCK_WRITE|BURST_WRITE beats
  integer               read_beat               =   5'h0;        // Keep track of BLOCK_READ|BURST_READ beats
  integer               block_read_max_beat     =  0;            // Support BLOCK_READ wrap-around (for critical word first)
  integer               block_read_min_beat     =  0;            // Support BLOCK_READ wrap-around (for critical word first)
  integer               is_rcw                  =   1'h0;        // RCW
  integer               is_write                =   1'h0;        // WRITE|BLOCK_WRITE|BURST_WRITE|RCW
  integer               addr_byte_lanes         =   3'h0;        // Bits of POReqAdrs_iDMA which indicate byte lane
  integer               aligned_ok              =   1'h0;        // Legal combination of address low-order bits and byte enables
  integer               do_write                =   1'h0;        // WRITE|BLOCK_WRITE|BURST_WRITE|(RCW#2 && rcw_data_matches)
  integer               start_read              =   1'h0;        // READ|BLOCK_READ|BURST_READ|RCW#1
  integer               read_byte_enables       =   8'h0;        // Remember read byte enables
  integer               reading_sysram          =   1'h0;        // Remember if reads are from sysram or sysrom
  wire                  reading_sysram_ud;                       // Remember if reads are from sysram or sysrom (unit delay)
  integer               reading                 =   1'h0;        // Keep track of whether reads are in progress
  integer               reading_delayed         =   1'h0;        // delayed one cycle
  integer               do_read                 =   1'h0;        // Do a read this cycle
  wire                  do_read_ud;                              // Do a read this cycle (unit delay)
  integer               compare_rcw_data        =   1'h0;        // Flag to trigger an RCW compare at negedge clock
  integer               rcw_data_matches        =   1'h0;        // True if RCW compare data matches memory
  reg           [ 63:0] rcw_compare_data        =  64'h0;        // POReqData_iDMA from RCW#1
  reg           [ 63:0] rcw_old_data            =  64'h0;        // Original contents of memory at RCW address
  reg           [  7:0] rcw_byte_enables        =   8'h0;        // POReqDataBE_iDMA from RCW#1



  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
  end



  always @(posedge CLK) begin
    reading_delayed     = reading;
    response_prev       = PIRespValid_iDMA && PORespRdy_iDMA;
    request_prev        = POReqValid_iDMA && PIReqRdy_iDMA;
    request_last        = request_prev && POReqCntl_iDMA[0];
    is_rcw              = request_prev && (POReqCntl_iDMA[7:4] == RCW);
    compare_rcw_data    = (is_rcw && first_transfer) ? 1 : 0;
    is_write            = request_prev && ((POReqCntl_iDMA[7:4] == WRITE) ||
                                           (POReqCntl_iDMA[7:4] == BLOCK_WRITE) ||
                                           (POReqCntl_iDMA[7:4] == BURST_WRITE) ||
                                           (POReqCntl_iDMA[7:4] == RCW));
    addr_byte_lanes     = POReqAdrs_iDMA[2:0];
    aligned_ok          = request_prev && (
                          ((addr_byte_lanes == 3'h0) && (POReqDataBE_iDMA == 8'b0000_0001)) ||
                          ((addr_byte_lanes == 3'h1) && (POReqDataBE_iDMA == 8'b0000_0010)) ||
                          ((addr_byte_lanes == 3'h2) && (POReqDataBE_iDMA == 8'b0000_0100)) ||
                          ((addr_byte_lanes == 3'h3) && (POReqDataBE_iDMA == 8'b0000_1000)) ||
                          ((addr_byte_lanes == 3'h4) && (POReqDataBE_iDMA == 8'b0001_0000)) ||
                          ((addr_byte_lanes == 3'h5) && (POReqDataBE_iDMA == 8'b0010_0000)) ||
                          ((addr_byte_lanes == 3'h6) && (POReqDataBE_iDMA == 8'b0100_0000)) ||
                          ((addr_byte_lanes == 3'h7) && (POReqDataBE_iDMA == 8'b1000_0000)) ||
                          ((addr_byte_lanes == 3'h0) && (POReqDataBE_iDMA == 8'b0000_0011)) ||
                          ((addr_byte_lanes == 3'h2) && (POReqDataBE_iDMA == 8'b0000_1100)) ||
                          ((addr_byte_lanes == 3'h4) && (POReqDataBE_iDMA == 8'b0011_0000)) ||
                          ((addr_byte_lanes == 3'h6) && (POReqDataBE_iDMA == 8'b1100_0000)) ||
                          ((addr_byte_lanes == 3'h0) && (POReqDataBE_iDMA == 8'b0000_1111)) ||
                          ((addr_byte_lanes == 3'h4) && (POReqDataBE_iDMA == 8'b1111_0000)) ||
                          ((addr_byte_lanes == 3'h0) && (POReqDataBE_iDMA == 8'b1111_1111)) ||
                          0);
    sysram_hit          = request_prev && (POReqAdrs_iDMA >= sysram_start) && (POReqAdrs_iDMA <= sysram_end);
    error_resp_pending  = (error_resp_pending && !response_prev) ||
                          (request_prev && !sysram_hit && !sysrom_hit) ||
                          (request_prev && !aligned_ok) ||
                          (is_write && sysrom_hit);
    num_responses       = error_resp_pending                  ? 1                          :  // An error response is pending
                          !request_last                       ? 0                          :  // Last cycle didn't have a last-xfer req accepted
                          (POReqCntl_iDMA[7:4] == BLOCK_READ) ? (2 << POReqCntl_iDMA[2:1]) :  // BLOCK_READ
                          (POReqCntl_iDMA[7:4] == BURST_READ) ? (POReqCntl_iDMA[3:1] + 1)  :  // BURST_READ
                                                           1;                       // All others
    response_count      = (request_prev ? num_responses : (response_count - (response_prev ? 1 : 0)));
    PIRespValid_iDMA    = (response_count != 0);
    PIReqRdy_iDMA       = (response_count == 0);
    do_write            = request_prev && !error_resp_pending && ((POReqCntl_iDMA[7:4] == WRITE) ||
                                                                  (POReqCntl_iDMA[7:4] == BLOCK_WRITE) ||
                                                                  (POReqCntl_iDMA[7:4] == BURST_WRITE) ||
                                                                 ((POReqCntl_iDMA[7:4] == RCW) && request_last && rcw_data_matches));
    start_read          = request_prev && !error_resp_pending && ((POReqCntl_iDMA[7:4] == READ) ||
                                                                  (POReqCntl_iDMA[7:4] == BLOCK_READ) ||
                                                                  (POReqCntl_iDMA[7:4] == BURST_READ) ||
                                                                 ((POReqCntl_iDMA[7:4] == RCW) && !request_last));
    do_read             = start_read || (reading && (response_count != 0));
    write_beat          = !do_write ? write_beat : first_transfer ? 0 : (write_beat + (is_rcw ? 0 : 1));
    read_beat           = !do_read  ? read_beat  : start_read     ? 0 : (read_beat  + ((is_rcw || !response_prev) ? 0 : 1));

    if (request_prev && first_transfer) begin
      address           = POReqAdrs_iDMA;
      PIRespId_iDMA     = POReqId_iDMA;
`ifdef hifi4_Aquila_E2_PROD_idma0_ROUTE_ID
      PIRespRouteId_iDMA = POReqRouteId_iDMA;
`endif
      PIRespPriority_iDMA = POReqPriority_iDMA;
      if (POReqCntl_iDMA[7:4] == BLOCK_READ) begin
        block_read_max_beat = num_responses - ((address % (bus_byte_width * num_responses)) / bus_byte_width) - 1;
        block_read_min_beat = block_read_max_beat - num_responses + 1;
      end
      else begin
        block_read_max_beat = 16;
        block_read_min_beat = 0;
      end
    end

    if (do_read && (read_beat > block_read_max_beat)) begin
      read_beat         = block_read_min_beat;
    end

    rcw_compare_data    = compare_rcw_data ? POReqData_iDMA   :  64'h0;
    rcw_byte_enables    = compare_rcw_data ? POReqDataBE_iDMA :   8'h0;

    if (start_read) begin
      reading_sysram    = sysram_hit;
      read_byte_enables = POReqDataBE_iDMA;
    end

    sysram_write                =   1'h0;
    sysram_index                =  28'h0;
    sysram_byte_enables         =   8'h0;
    sysram_write_data           =  64'h0;

    sysrom_index                =   1'h0;
    sysrom_byte_enables         =   8'h0;

    if (do_write) begin
      sysram_write              = 1'h1;
      sysram_index              = ((address - sysram_start) >> 3 ) + write_beat;
      sysram_byte_enables       = POReqDataBE_iDMA;
      sysram_write_data         = POReqData_iDMA;
    end
    else if (do_read) begin
      if (reading_sysram) begin
        sysram_index            = ((address - sysram_start) >> 3 ) + read_beat;
        sysram_byte_enables     = read_byte_enables;
      end
      else begin
        sysrom_index            = ((address - sysrom_start) >> 3 ) + read_beat;
        sysrom_byte_enables     = read_byte_enables;
      end
    end

    PIRespCntl_iDMA    <= #1 ((response_count == 0) ? RSP_NONE : 
                              { (error_resp_pending ? RSP_ADDR_ERROR : RSP_OK), ((response_count == 1) ? 1'h1 : 1'h0)});

    reading            <= #1 (start_read || (reading_delayed && PIRespValid_iDMA));
    first_transfer     <= #1 request_prev ? request_last : first_transfer;
  end


  always @(negedge CLK) begin
    rcw_data_matches = 1'h0;
    if (compare_rcw_data) begin
      rcw_old_data = sysram_read_data;
      rcw_data_matches = 1'h1;
      if ((rcw_byte_enables[ 0] && (rcw_compare_data[  7:  0] !== rcw_old_data[  7:  0])) ||
          (rcw_byte_enables[ 1] && (rcw_compare_data[ 15:  8] !== rcw_old_data[ 15:  8])) ||
          (rcw_byte_enables[ 2] && (rcw_compare_data[ 23: 16] !== rcw_old_data[ 23: 16])) ||
          (rcw_byte_enables[ 3] && (rcw_compare_data[ 31: 24] !== rcw_old_data[ 31: 24])) ||
          (rcw_byte_enables[ 4] && (rcw_compare_data[ 39: 32] !== rcw_old_data[ 39: 32])) ||
          (rcw_byte_enables[ 5] && (rcw_compare_data[ 47: 40] !== rcw_old_data[ 47: 40])) ||
          (rcw_byte_enables[ 6] && (rcw_compare_data[ 55: 48] !== rcw_old_data[ 55: 48])) ||
          (rcw_byte_enables[ 7] && (rcw_compare_data[ 63: 56] !== rcw_old_data[ 63: 56]))) rcw_data_matches = 1'h0;
    end
  end


  assign #1 do_read_ud          = do_read;
  assign #1 reading_sysram_ud   = reading_sysram;
  assign PIRespData_iDMA        = do_read_ud ? (reading_sysram_ud ? sysram_read_data : sysrom_read_data) : 64'h0;


  hifi4_Aquila_E2_PROD_idma0ram #(.file_name(file_name)) sysram(
    .CLK          (CLK),
    .write        (sysram_write),
    .index        (sysram_index),
    .byte_enables (sysram_byte_enables),
    .write_data   (sysram_write_data),
    .read_data    (sysram_read_data)
  );


`ifdef MODEL_TECH
  export "DPI-SC" function peek_hifi4_Aquila_E2_PROD_idma0;
`else
  export "DPI-C"  function peek_hifi4_Aquila_E2_PROD_idma0;
`endif
  function int peek_hifi4_Aquila_E2_PROD_idma0;
    input int unsigned address;
    inout int unsigned byte_lanes_3210;
    reg [63:0] peek_data;
    int unsigned index;
    int unsigned word;
    word = address[2:2];
    peek_hifi4_Aquila_E2_PROD_idma0 = 1;
    if (!(address & 2'h3) && (((address >= sysram_start) && (address <= sysram_end)) ||
                              ((address >= sysrom_start) && (address <= sysrom_end)))
    ) begin
      peek_hifi4_Aquila_E2_PROD_idma0 = 0;
      if ((address >= sysram_start) && (address <= sysram_end)) begin
        index = (address - sysram_start) >> 3;
        peek_data = sysram.mem[index];
      end
      else begin
        index = (address - sysrom_start) >> 3;
      end
      byte_lanes_3210 = (peek_data >> (word * 32)) & 32'hFFFFFFFF;
`ifdef LOG_PEEK_POKE
      $display("%t %m: peek addr=0x%8x data=0x%8x index=0x%8x", $time, address, byte_lanes_3210, index);
`endif
    end
  endfunction


`ifdef MODEL_TECH
  export "DPI-SC" function poke_hifi4_Aquila_E2_PROD_idma0;
`else
  export "DPI-C"  function poke_hifi4_Aquila_E2_PROD_idma0;
`endif
  function int poke_hifi4_Aquila_E2_PROD_idma0;
    input int unsigned address;
    input int unsigned byte_lanes_3210;
    reg [63:0] poke_data;
    int unsigned index;
    int unsigned word;
    word = address[2:2];
    poke_hifi4_Aquila_E2_PROD_idma0 = 1;
    if (!(address & 2'h3) && (((address >= sysram_start) && (address <= sysram_end)) ||
                              ((address >= sysrom_start) && (address <= sysrom_end)))
    ) begin
      poke_hifi4_Aquila_E2_PROD_idma0 = 0;
      if ((address >= sysram_start) && (address <= sysram_end)) begin
        index = (address - sysram_start) >> 3;
        poke_data = sysram.mem[index];
             if (word == 0) poke_data = { poke_data[ 63:32], byte_lanes_3210                  };
        else                poke_data = {                    byte_lanes_3210, poke_data[31:0] };
        sysram.mem[index] = poke_data;
      end
      else begin
        index = (address - sysrom_start) >> 3;
      end
`ifdef LOG_PEEK_POKE
      $display("%t %m: poke addr=0x%8x data=0x%8x, index=0x%8x", $time, address, byte_lanes_3210, index);
`endif
    end
    else begin
      $display("%t %m: poke failed addr=0x%8h sysrom_start=0x%8h sysrom_end=0x%h8", $time, address, sysrom_start, sysrom_end);
    end
  endfunction


endmodule





// Model for system RAM

module hifi4_Aquila_E2_PROD_idma0ram(
  CLK,                  // clock
  write,                // 1=write, 0=read
  index,                // index into mem array
  byte_enables,         // byte enables
  write_data,           // write data
  read_data             // read data
);

  input                         CLK;
  input                         write;
  input                 [ 27:0] index;
  input                 [  7:0] byte_enables;
  input                 [ 63:0] write_data;

  output                [ 63:0] read_data;

  reg   /*sparse*/      [ 63:0] mem[0:28'hFFFFFFF];
  reg                   [ 63:0] wdata;
  wire                  [ 63:0] rdata;

  parameter file_name = "<default>";


  initial begin
    if (!$test$plusargs("preloadmems") || (file_name == "") || (file_name == "<none>")) begin
      $display("%t Not preloading memory %m.", $time);
    end
    else if (file_name == "<default>") begin
      $display("%t Preloading memory %m from readmemh.data/pif.data.sysram", $time);
      $readmemh("readmemh.data/pif.data.sysram", mem);
    end
    else begin
      $display("%t Preloading memory %m from %s", $time, {file_name, ".sysram"});
      $readmemh({file_name, ".sysram"}, mem);
    end
  end


  assign rdata = mem[index];

  assign #1 read_data = {
                         (byte_enables[ 7] ? rdata[ 63: 56] : 8'h0),
                         (byte_enables[ 6] ? rdata[ 55: 48] : 8'h0),
                         (byte_enables[ 5] ? rdata[ 47: 40] : 8'h0),
                         (byte_enables[ 4] ? rdata[ 39: 32] : 8'h0),
                         (byte_enables[ 3] ? rdata[ 31: 24] : 8'h0),
                         (byte_enables[ 2] ? rdata[ 23: 16] : 8'h0),
                         (byte_enables[ 1] ? rdata[ 15:  8] : 8'h0),
                         (byte_enables[ 0] ? rdata[  7:  0] : 8'h0)};

  always @(posedge CLK) begin
    if (write) begin
      wdata = mem[index];
      if (byte_enables[ 0]) wdata[  7:  0] = write_data[  7:  0];
      if (byte_enables[ 1]) wdata[ 15:  8] = write_data[ 15:  8];
      if (byte_enables[ 2]) wdata[ 23: 16] = write_data[ 23: 16];
      if (byte_enables[ 3]) wdata[ 31: 24] = write_data[ 31: 24];
      if (byte_enables[ 4]) wdata[ 39: 32] = write_data[ 39: 32];
      if (byte_enables[ 5]) wdata[ 47: 40] = write_data[ 47: 40];
      if (byte_enables[ 6]) wdata[ 55: 48] = write_data[ 55: 48];
      if (byte_enables[ 7]) wdata[ 63: 56] = write_data[ 63: 56];
      mem[index] = wdata;
    end
  end

endmodule





