// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <xtensa/hal.h>
#include <xtensa/tie/xt_ivp32.h>
#include <xtensa/tie/xt_interrupt.h>

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)                    { TIMESTAMP; fprintf(stdout, msg);                   fflush(stdout); }
#define LOG1(msg, arg1)             { TIMESTAMP; fprintf(stdout, msg, arg1);             fflush(stdout); }
#define LOG2(msg, arg1, arg2)       { TIMESTAMP; fprintf(stdout, msg, arg1, arg2);       fflush(stdout); }

// uDMA interrupt
#define INT_NUM_DMA       15
#define INT_NUM_NMI       16

// Memory initial values
#define RAM_SIZE            512
#ifndef DATARAM_BUF_SIZE
  #define DATARAM_BUF_SIZE  256
#endif
#define ALIGN (DATARAM_BUF_SIZE << 2 )
#define __LM_DRAM0__ __attribute__((aligned(ALIGN),section(".dram0.data")))
#define __LM_DRAM1__ __attribute__((aligned(ALIGN),section(".dram1.data")))
#define __DMA_DESCR_ATTR__ __LM_DRAM1__

volatile unsigned int dram0buf[RAM_SIZE] __LM_DRAM0__ = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
};

volatile unsigned int dram1buf[RAM_SIZE] __LM_DRAM1__ = {
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

volatile unsigned int dma_sram_mem[RAM_SIZE] __attribute__((aligned(ALIGN), section(".sram.data"))) = {
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};


// The DMA programming registers
#define DMA_CONFIG          0xd0000000
#define DMA_STATUS          0xd0000001
#define DMA_CURR_SRC_PTR    0xd0000002
#define DMA_CURR_DST_PTR    0xd0000003
#define DMA_DESC_FIRST_PTR  0xd0000010
#define DMA_DESC_LAST_PTR   0xd0000011
#define DMA_DESC_CUR_PTR    0xd0000012
#define DMA_DESC_NUM        0xd0000013
#define DMA_DESC_NUM_INCR   0xd0000014

// uDMA Descriptor
typedef struct {
  void* src_addr;                       // Source address
  void* dst_addr;                       // Destination address
  // Fields of the control word
  unsigned int interrupt:1;             // Bit  0        controls interrupt
  unsigned int dummy1:3;                // Bits 1-3      reserved
  unsigned int blk_size:2;              // Bits 4-5      block size
  unsigned int dummy2:2;                // Bits 6-7      reserved
  unsigned int num_outstanding:4;       // Bits 8-11     number of outstanding requests
  unsigned int dummy3:4;                // Bits 12-15    reserved
  unsigned int num_bytes:16;            // Bits 16-31    number of bytes
  // Number of rows, word 3
  unsigned int num_rows:16;             // Bits 0-15     number of rows
  unsigned int dummy4:16;               // Bits 16-31    reserved
  // Pitch, word 4,5
  unsigned int src_pitch;               // Source pitch
  unsigned int dst_pitch;               // Destination pitch
  // Reserved words, word 6,7
  unsigned int reserved_word1;          // Reserved
  unsigned int reserved_word2;          // Reserved
} D2_DMA_desc;

// Target program list of descriptors
#define MAX_DMA_DESCR       16
volatile D2_DMA_desc dma_desc[MAX_DMA_DESCR] __DMA_DESCR_ATTR__;

unsigned int dma_index = 0;
unsigned int addr_mask_partial[16] = {
  0xffffffff, 0xfffffffe, 0xfffffffc, 0x0,
  0xfffffff8, 0x0,        0x0,        0x0,
  0xffffff0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned int addr_mask_block[4] = { 0xffffffe0, 0xffffffc0, 0xffffff80, 0xffffff00 };

// Add a descriptor to the list
inline int add_2d_dma_descr(void* src, void* dst, int num_bytes, int blk_size,
                            int num_outstanding, int num_rows, int src_pitch, int dst_pitch, int interrupt)
{
  unsigned int align_mask = ( num_bytes <= 16 ) ?
    addr_mask_partial[num_bytes >>1] :
    addr_mask_block[blk_size];

  dma_index = dma_index % MAX_DMA_DESCR;
  dma_desc[dma_index].src_addr        = src;
  dma_desc[dma_index].dst_addr        = dst;
  dma_desc[dma_index].num_bytes       = num_bytes & 0xffff;
  dma_desc[dma_index].blk_size        = blk_size & 0x3;
  dma_desc[dma_index].num_outstanding = num_outstanding & 0xf;
  dma_desc[dma_index].num_rows        = num_rows;
  dma_desc[dma_index].src_pitch       = src_pitch;
  dma_desc[dma_index].dst_pitch       = dst_pitch;
  dma_desc[dma_index].interrupt       = interrupt & 0x1;
  dma_index ++;
  return dma_index;
}

// Program uDMA
inline void program_dma(int start_index, int num_descr) {
  unsigned int idx0 = start_index % MAX_DMA_DESCR;
  unsigned int idx1 = (start_index + num_descr - 1 ) % MAX_DMA_DESCR;
  IVP_WER(0x2, DMA_CONFIG);                                    // Disable DMA
  IVP_WER((unsigned int)&dma_desc[idx0], DMA_DESC_FIRST_PTR);  // First  pointer
  IVP_WER((unsigned int)&dma_desc[idx1], DMA_DESC_LAST_PTR);   // Last   pointer
  IVP_WER(num_descr, DMA_DESC_NUM);                            // Number of descriptors to execute
  IVP_WER(0x3, DMA_CONFIG);                                    // Enable DMA
}

// Start uDMA
inline void start_dma(void *first_ptr, void * last_ptr, unsigned int num_desc) {
  IVP_WER(0x2, DMA_CONFIG);                                    // Disable DMA
  IVP_WER((unsigned int)first_ptr, DMA_DESC_FIRST_PTR);        // First  pointer
  IVP_WER((unsigned int)last_ptr, DMA_DESC_LAST_PTR);          // Last   pointer
  IVP_WER(num_desc, DMA_DESC_NUM);                             // Number of descriptors to execute
  IVP_WER(0x3, DMA_CONFIG);                                    // Enable DMA
}

// Read uDMA status register
inline unsigned int read_dma_status() {
  return IVP_RER(DMA_STATUS);
}


volatile int udma_done      __attribute__((section(".dram0.data"))) = 0;

// uDMA interrupt handler
void interrupt_handler(int num) {
  udma_done = 1;
}

// Wait until DMA completes
void wait_for_dma(void) {
  while (!udma_done) {
    ;  // Spin
  }
  udma_done = 0;
}


int main(void) {
  int result = 1;
  int idx, i;
  unsigned int *ptr_slave;
  unsigned int *dma_src_ptr0, *dma_dst_ptr0;
  volatile unsigned int status ;
  volatile int done=0;

  // Interrrupt handlers
  _xtos_set_interrupt_handler_arg(INT_NUM_DMA, interrupt_handler, (void*)INT_NUM_DMA);
  _xtos_ints_on(1 << INT_NUM_DMA);

  // DMA descriptors from local RAM (dram0) to system RAM
  dma_src_ptr0 = (unsigned int *)&dram0buf[0];
  dma_dst_ptr0 = (unsigned int *)&dma_sram_mem[0];
  idx = add_2d_dma_descr((void*)dma_src_ptr0, (void *)dma_dst_ptr0, 256, 0, 15, 2, 256, 256, 1);

  // DMA descriptors from system RAM to local RAM (dram1)
  dma_src_ptr0 = (unsigned int *)&dma_sram_mem[0];
  dma_dst_ptr0 = (unsigned int *)&dram1buf[0];
  idx = add_2d_dma_descr((void*)dma_src_ptr0, (void *)dma_dst_ptr0, 32, 0, 15, 16, 32, 32, 1);

  //-----------------------------------------------------------------------
  // DMA start
  //-----------------------------------------------------------------------
  program_dma(0, 1); // From local RAM to system RAM

  // Wait for DMA complete
  done=0;
  while (!done) {
    status = read_dma_status();
    done = ((status & 0x1) == 0 );
  }

  program_dma(1, 1); // From system RAM to local RAM

  // Wait for DMA done interrupt
  wait_for_dma();

  for (i=0; i<128; i++) {
    ptr_slave = (unsigned int *)&dram1buf[i];
    if (*ptr_slave != i) result |= 2;
  }

  if (result==1) {
    LOG("uDMA passed! \n");
  }
  else {
    LOG("uDMA failed! \n");
  }
}
