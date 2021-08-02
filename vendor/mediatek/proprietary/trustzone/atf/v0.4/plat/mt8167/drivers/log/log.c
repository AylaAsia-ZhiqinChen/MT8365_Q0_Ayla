/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <log.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <platform.h>
#include <arch.h>
#include <arch_helpers.h>
#include <mmio.h>
#include <plat_def.h>

#define DEBUG_LOG_SERVICE 1
#if DEBUG_LOG_SERVICE
#define debug_print(...) printf(__VA_ARGS__)
#else
#define debug_print(...) ((void)0)
#endif

extern void bl31_log_service_register(int (*lock_get)(),
    int (*log_putc)(unsigned char),
    int (*lock_release)());

static unsigned int mt_log_buf_start = 0;
static unsigned int mt_log_buf_size = 0;
static unsigned int mt_log_buf_end = 0;
static atf_log_ctrl_t *p_atf_log_ctrl = 0;

void mt_log_suspend_flush()
{
    if( mt_log_buf_size != 0 )
    {
        flush_dcache_range((uint64_t)mt_log_buf_start, (uint64_t)mt_log_buf_size);
    }
}

/* don't use any print function here in this function */
int mt_log_lock_acquire()
{
    spin_lock(&(p_atf_log_ctrl->info.atf_buf_lock));

    return 0;
}

/* don't use any print function here in this function */
int mt_log_write(unsigned char c)
{
    *(unsigned char*)(uintptr_t)p_atf_log_ctrl->info.atf_write_pos = c;

    p_atf_log_ctrl->info.atf_total_write_count++;

    if( p_atf_log_ctrl->info.atf_write_pos < mt_log_buf_end )
        p_atf_log_ctrl->info.atf_write_pos++;
    else
        p_atf_log_ctrl->info.atf_write_pos = mt_log_buf_start + ATF_LOG_CTRL_BUF_SIZE;

    return 0;
}

int mt_crash_log_dump(uint8_t *crash_log_addr, uint32_t crash_log_size)
{
	// 
	uint64_t ret = 0;
	uint64_t read_count;
	uint64_t offset;
	uint64_t pos;
	uint64_t write_count = p_atf_log_ctrl->info.atf_total_write_count;
	uint8_t *r_ptr = NULL;
	uint8_t *w_ptr = NULL;
	uint8_t *start_ptr = (uint8_t*)(uintptr_t)(mt_log_buf_start + ATF_LOG_CTRL_BUF_SIZE);

	if(write_count == 0)
		return 0;
	
	if(write_count > crash_log_size) {
		offset = crash_log_size-1;
		read_count = crash_log_size;
	} else {
		offset = write_count-1;
		read_count = write_count;
	}

	pos = p_atf_log_ctrl->info.atf_write_pos-1;
	if(pos < (mt_log_buf_start + ATF_LOG_CTRL_BUF_SIZE))
		pos = mt_log_buf_end;
	r_ptr = (uint8_t *)((uintptr_t)p_atf_log_ctrl->info.atf_write_pos);
	
	w_ptr = crash_log_addr + offset;
	while(read_count) {
		*(w_ptr--) = *(r_ptr--);
		if(r_ptr < start_ptr)
			r_ptr = (uint8_t*)(uintptr_t)mt_log_buf_end;
		read_count--;
		ret++;
	}
	
	return ret;
}

static int mt_get_unread_log_size()
{
    if(p_atf_log_ctrl->info.atf_read_pos == p_atf_log_ctrl->info.atf_write_pos)
    {
        return 0;
    }
    else if(p_atf_log_ctrl->info.atf_read_pos < p_atf_log_ctrl->info.atf_write_pos)
    {
        return (p_atf_log_ctrl->info.atf_write_pos - p_atf_log_ctrl->info.atf_read_pos);
    }

    //Wrap around case
    return (p_atf_log_ctrl->info.atf_write_pos + p_atf_log_ctrl->info.atf_buf_size)
        - p_atf_log_ctrl->info.atf_read_pos;
}

/* don't use any print function here in this function */
int mt_log_lock_release()
{
    unsigned int atf_buf_unread_size = mt_get_unread_log_size();
    
    //check if need to notify normal world to update
    if(atf_buf_unread_size >= ATF_LOG_SIGNAL_THRESHOLD_SIZE)
    {
    	// Clean caches before re-entering normal world
        // dcsw_op_louis(DCCSW); //Level of Unification inner shareable
        // dcsw_op_all(DCCSW); //Flush all

        // Notify normal world
        if(p_atf_log_ctrl->info.atf_reader_alive)
        {
            mt_atf_trigger_irq();
            p_atf_log_ctrl->info.atf_irq_count++;
        }
    }
    p_atf_log_ctrl->info.atf_buf_unread_size = atf_buf_unread_size;

    //release the lock
    spin_unlock(&(p_atf_log_ctrl->info.atf_buf_lock));

    return 0;
}

uint32_t is_power_on_boot(void)
{
    uint32_t wdt_sta, wdt_interval;
    wdt_sta = mmio_read_32(MTK_WDT_STATUS);
    wdt_interval = mmio_read_32(MTK_WDT_INTERVAL);
    printf("sta=0x%x int=0x%x\r\n", wdt_sta, wdt_interval);

    // Bit 2: IS_POWER_ON_RESET. 
    // (bit2 will be set in preloader when reboot_from power on)
    if(wdt_interval & (1<<2))
        return 1;
    return 0;
}

void mt_log_set_crash_flag(void)
{
    p_atf_log_ctrl->info.atf_crash_flag = ATF_CRASH_MAGIC_NO;
}

void mt_log_setup(unsigned int start, unsigned int size, unsigned int aee_buf_size)
{
    uint32_t dump_ret=0;
    mt_log_buf_start = start;
    mt_log_buf_size = size;
    mt_log_buf_end = start + size - 1 - aee_buf_size - ATF_CRASH_BUF_SIZE;

    p_atf_log_ctrl = (atf_log_ctrl_t*)(uintptr_t)start;
    
    if(is_power_on_boot()) {
        debug_print("power_on: true\n");
        memset((void*)(uintptr_t)start, 0x0, size);
    } else {
        debug_print("power_on: false\n");
        if(p_atf_log_ctrl->info.atf_crash_flag == ATF_CRASH_MAGIC_NO) {
            debug_print("crash flag: 0x%x\n", p_atf_log_ctrl->info.atf_crash_flag);
            debug_print("atf_buf_addr : 0x%x\n", p_atf_log_ctrl->info.atf_buf_addr);
            debug_print("atf_buf_size : 0x%x\n", p_atf_log_ctrl->info.atf_buf_size);
            debug_print("dump crashlog\n");
            p_atf_log_ctrl->info.atf_crash_log_addr = (start + ATF_LOG_CTRL_BUF_SIZE) + 
                (size - ATF_LOG_CTRL_BUF_SIZE - aee_buf_size - ATF_CRASH_BUF_SIZE);
            p_atf_log_ctrl->info.atf_crash_log_size = ATF_CRASH_BUF_SIZE;
            debug_print("atf_crash_log_addr: 0x%x\n", p_atf_log_ctrl->info.atf_crash_log_addr);
            dump_ret = mt_crash_log_dump((uint8_t*)(uintptr_t)p_atf_log_ctrl->info.atf_crash_log_addr, p_atf_log_ctrl->info.atf_crash_log_size);
            debug_print("dump_ret=%d.\n", dump_ret);
            // 
            // |--- Ctrl  ---|--- Ring Buffer ---|--- Crash Log ---|--- AEE buffer --|
            //      clean         clean               keep              clean  
            //
            memset((void*)(uintptr_t)start, 0x0, size-aee_buf_size-ATF_CRASH_BUF_SIZE);
            memset((void*)(uintptr_t)(start+(size-aee_buf_size)), 0x0, aee_buf_size);
        } else {
            memset((void*)(uintptr_t)start, 0x0, size);
        }
    }

    p_atf_log_ctrl->info.atf_buf_addr = start + ATF_LOG_CTRL_BUF_SIZE;
    p_atf_log_ctrl->info.atf_buf_size = size - ATF_LOG_CTRL_BUF_SIZE - aee_buf_size - ATF_CRASH_BUF_SIZE;
    p_atf_log_ctrl->info.atf_write_pos = p_atf_log_ctrl->info.atf_buf_addr;
    p_atf_log_ctrl->info.atf_read_pos = p_atf_log_ctrl->info.atf_buf_addr;
    p_atf_log_ctrl->info.atf_buf_lock.lock = 0;

    debug_print("mt_log_setup - atf_buf_addr : 0x%x\n", p_atf_log_ctrl->info.atf_buf_addr);
    debug_print("mt_log_setup - atf_buf_size : 0x%x\n", p_atf_log_ctrl->info.atf_buf_size);
    debug_print("mt_log_setup - atf_write_pos : 0x%x\n", p_atf_log_ctrl->info.atf_write_pos);
    debug_print("mt_log_setup - atf_read_pos : 0x%x\n", p_atf_log_ctrl->info.atf_read_pos);
    debug_print("mt_log_setup - atf_buf_lock : 0x%x\n", p_atf_log_ctrl->info.atf_buf_lock.lock);
    debug_print("mt_log_setup - mt_log_buf_end : 0x%x\n", mt_log_buf_end);

    if(dump_ret) {
        // write crash info back to control buffer
        p_atf_log_ctrl->info.atf_crash_log_addr = (start + ATF_LOG_CTRL_BUF_SIZE) + 
            (size - ATF_LOG_CTRL_BUF_SIZE - aee_buf_size - ATF_CRASH_BUF_SIZE);
        p_atf_log_ctrl->info.atf_crash_log_size = ATF_CRASH_BUF_SIZE;
        p_atf_log_ctrl->info.atf_crash_flag = ATF_CRASH_MAGIC_NO;
    }

    
    bl31_log_service_register(&mt_log_lock_acquire, &mt_log_write, 
        &mt_log_lock_release);
}


#define MT_LOG_SECURE_OS_BUFFER_MAX_LENGTH 120
#define TBASE_TAG "TBASE"
static unsigned char mt_log_secure_os_buf[MT_LOG_SECURE_OS_BUFFER_MAX_LENGTH+1] = {0};
static unsigned int mt_log_secure_os_pos = 0;

void mt_log_secure_os_print(int c)
{
    mt_log_secure_os_buf[mt_log_secure_os_pos] = c;

    //Force to flush the buffer if find end of line
    if( c == '\n' )
    {
        mt_log_secure_os_buf[mt_log_secure_os_pos+1] = '\0';
        printf("[%s]%s", TBASE_TAG, mt_log_secure_os_buf);
        mt_log_secure_os_pos = 0;
        return;
    }
    
    mt_log_secure_os_pos++;

    //Already reach the end of buffer, force to flush the buffer
    if( mt_log_secure_os_pos == MT_LOG_SECURE_OS_BUFFER_MAX_LENGTH )
    {
        mt_log_secure_os_buf[mt_log_secure_os_pos] = '\0';
        printf("[%s]%s", TBASE_TAG, mt_log_secure_os_buf);
        mt_log_secure_os_pos = 0;
    }    
}

