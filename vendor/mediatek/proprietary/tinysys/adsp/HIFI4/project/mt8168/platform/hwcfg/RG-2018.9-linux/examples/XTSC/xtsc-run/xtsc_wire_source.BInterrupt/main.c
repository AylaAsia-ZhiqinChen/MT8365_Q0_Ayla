// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of Cadence Design Systems, Inc.
// They may not be modified, copied, reproduced, distributed, or disclosed to
// third parties in any manner, medium, or form, in whole or in part, without
// the prior written consent of Cadence Design Systems, Inc.
//

// Test of Interrupts.

#include <stdio.h>
#include <stdarg.h>
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>

typedef unsigned int u32;

static u32 interrupt_count[32] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


// Hopefully, this hard-coded address doesn't conflict with anything
volatile u32 *p_mmio = (u32 *) 0x20000000;

u32 interrupt = 0x00000000;

void myhandler(int num)   {
  interrupt ^= (1 << num);
  *p_mmio = interrupt;
  interrupt_count[num] += 1;
}



void interrupt_test_init(void) {
  int i, extint_num = 0;
  // Register generic handler only for external interrupts:
  for (i = 0; i < Xthal_num_interrupts; i++) {
    switch (Xthal_inttype[i]) {
      case XTHAL_INTTYPE_EXTERN_EDGE: {
        _xtos_set_interrupt_handler_arg(i, myhandler, (void*)i);
        _xtos_ints_on(1 << i);
        printf("Edge:  interrupt[%d] <= BInterrupt[%d] \n", i, extint_num);
        extint_num += 1;
        break;
      }
      case XTHAL_INTTYPE_EXTERN_LEVEL: {
        _xtos_set_interrupt_handler_arg(i, myhandler, (void*)i);
        _xtos_ints_on(1 << i);
        printf("Level: interrupt[%d] <= BInterrupt[%d] \n", i, extint_num);
        extint_num += 1;
        break;
      }
      case XTHAL_INTTYPE_NMI: {
        // Non-maskable interrupts have a default handling
        printf("NMI:   interrupt[%d] <= BInterrupt[%d] (not using myhandler) \n", i, extint_num);
        extint_num += 1;
        break;
      }
      default: {
        break;
      }
    }
  }
}


int main(int argc, char *argv[]) {
  int i;

  printf("Interrupt mapping:\n");
  interrupt_test_init();

  // Trigger the interrupt source drivers
  *p_mmio = 0xFFFFFFFF;
  interrupt = 0x00000000;
  *p_mmio = interrupt;

  printf("\n");
  printf("Chant a song to pass some time . . .\n");
  for (i=99; i>78; --i) {
    printf("%d bottles of beer on the wall, %d bottles of beer, take 1 down, pass it around, %d bottles of beer on the wall.\n",i,i,i-1);
  }

  printf(". . . \n");
  printf("\n");
  printf("Summary of interrupt counts: \n");
  for (i=0; i<32; ++i) {
    if (interrupt_count[i]) {
      printf("interrupt_count[%d] = %d\n", i, interrupt_count[i]);
    }
  }

  return 0;
}

