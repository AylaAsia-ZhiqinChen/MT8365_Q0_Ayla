/*
 * Copyright (c) 2014 TRUSTONIC LIMITED
 * All rights reserved
 *
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#include <assert.h>
#include <debug.h>
#include <string.h>
#include <tzc400.h>
#include <platform.h>
#include <mmio.h>
#include <plat_def.h>
#include "plat_private.h"

#include <arch_helpers.h>
#include <tbase_private.h>

/* Defines */
#define PLAT_TBASE_INPUT_HWIDENTITY      (0x1)
#define PLAT_TBASE_INPUT_HWKEY           (0x2)
#define PLAT_TBASE_INPUT_RNG             (0x3)

#define PLAT_TBASE_INPUT_OK              ((uint32_t)0)
#define PLAT_TBASE_INPUT_ERROR           ((uint32_t)-1)


/* Code */
/* <t-base handler for SWd fastcall INPUT */
uint32_t plat_tbase_input(uint64_t DataId,uint64_t* Length,void* out)
{
        uint32_t Status = PLAT_TBASE_INPUT_OK;
        uint32_t length = 0, idx = 0;

        /* Implement associated actions */
        switch( DataId )
        {
        case PLAT_TBASE_INPUT_HWIDENTITY:
        {
                atf_arg_t_ptr teearg = (atf_arg_t_ptr)(uintptr_t)TEE_BOOT_INFO_ADDR;
                /* Maybe we can move this to a memcpy instead */
                for (idx=0; idx<(*Length/sizeof(uint32_t)) && idx<(sizeof(teearg->hwuid)/sizeof(uint32_t)); idx++)
                {
                  ((uint32_t*)out)[idx] = teearg->hwuid[idx];
                  length += sizeof(uint32_t);
                }

                break;
        }
        case PLAT_TBASE_INPUT_HWKEY:
        {
                atf_arg_t_ptr teearg = (atf_arg_t_ptr)(uintptr_t)TEE_BOOT_INFO_ADDR;
                /* Maybe we can move this to a memcpy instead */
                for (idx=0; idx<(*Length/sizeof(uint32_t)) && idx<(sizeof(teearg->HRID)/sizeof(uint32_t)) ; idx++)
                {
                  ((uint32_t*)out)[idx] = teearg->HRID[idx];
                  length += sizeof(uint32_t);
                }
                *Length = length;
                break;
        }
        case PLAT_TBASE_INPUT_RNG:
        {
                uint32_t value = 0;
                mmio_write_32(TRNG_PDN_CLR, 0x100);
                value = mmio_read_32(TRNG_CTRL);
                value |= TRNG_CTRL_START;
                mmio_write_32(TRNG_CTRL, value);                
                while (0 == (mmio_read_32(TRNG_CTRL) & TRNG_CTRL_RDY));
                ((uint32_t*)out)[0] = mmio_read_32(TRNG_DATA);
                value = mmio_read_32(TRNG_CTRL);
                value &= ~TRNG_CTRL_START;
                mmio_write_32(TRNG_CTRL, value);
                mmio_write_32(TRNG_PDN_SET, 0x100);
                *Length = 4;
                break;
        }
        default:
                /* Unsupported request */
                Status = PLAT_TBASE_INPUT_ERROR;
                break;
        }

        return Status;
}

/* <t-base handler for SWd fastcall DUMP */
uint32_t plat_tbase_dump(void)
{
        uint32_t linear_id = platform_get_core_pos(read_mpidr());

        printf( "core %d is dumped !\n", (int)linear_id );

        /* FIX-ME: mrdump not implement yet
        if (mrdump_run(linear_id))
                return 0;
        */
        aee_wdt_dump();
        
        //while(1);
        return 0;
}

