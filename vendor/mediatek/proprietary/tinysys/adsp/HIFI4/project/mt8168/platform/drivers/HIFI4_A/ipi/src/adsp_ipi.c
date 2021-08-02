/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */


#include <string.h>
#include <stdio.h>
#include <mt_reg_base.h>
#include <driver_api.h>
//#include <platform_mtk.h>
#include <interrupt.h>
#include "FreeRTOS.h"
#include "task.h"
#include "systimer.h"
#include "semphr.h"
#include "hw_semaphore.h"
#include <adsp_ipi.h>
#include <adsp_ipi_queue.h>
#include <adsp_ipi_platform.h>


static volatile uint32_t g_ipi_count = 0;
static struct ipi_desc_t ipi_desc[NR_IPI];
static struct ipc_desc_t ipc_desc[IPC_MAX];
static struct share_obj *adsp_send_obj, *adsp_rcv_obj;
static struct adsp_common_info* adsp_info;
static SemaphoreHandle_t xSemaphore_ipi = NULL;
static uint32_t ipi_init_ready = 0;
static uint32_t adsp_ipi_owner = 0;
static uint32_t adsp_ipi_id_record = 0;
static uint32_t adsp_ipi_id_record_count = 0;
static uint32_t adsp_to_ap_ipi_count = 0;
static uint32_t ap_to_adsp_ipi_count = 0;
static uint32_t adsp_current_ipi_id = 0;
static uint32_t adsp_max_duration_ipc_id = 0;

static enum ipi_status adsp_ipi_send_to_host(int id, void *buf, unsigned int  len);

void ipi_info_dump(enum ipi_id id)
{
    PRINTF_E("%u\t%u\t%u\t%u\t%u\t%u\t%u\t%s\n\r",
                 id,
                 ipi_desc[id].recv_count,
                 ipi_desc[id].init_count,
                 ipi_desc[id].is_wakeup_src,
                 ipi_desc[id].success_count,
                 ipi_desc[id].busy_count,
                 ipi_desc[id].error_count,
                 ipi_desc[id].name);
#if IPI_STAMP_DUMP
                /*time stamp*/
                int32_t i;
                for ( i = 0; i < ADSP_IPI_ID_STAMP_SIZE; i++){
                    if (ipi_desc[id].recv_timestamp[i] != 0) {
                        PRINTF_E("send:%u time%llu\n",
                                ipi_desc[id].recv_flag[i],
                                ipi_desc[id].recv_timestamp[i]);
                    }
                }
                for ( i = 0; i < ADSP_IPI_ID_STAMP_SIZE; i++){
                    if(ipi_desc[id].send_timestamp[i] != 0) {
                        PRINTF_E("recv:%u time:%llu\n",
                                ipi_desc[id].send_flag[i],
                                ipi_desc[id].send_timestamp[i]);
                    }
                }
#endif
}
void ipi_status_dump(void)
{
    int32_t id;

    PRINTF_E("id\trecv\tinit\twake\tsuccess\tbusy\terror\tname\n\r");
    for (id = 0; id < NR_IPI; id++) {
        if (ipi_desc[id].recv_count > 0 || ipi_desc[id].success_count > 0
            || ipi_desc[id].busy_count > 0 || ipi_desc[id].error_count > 0) {
            ipi_info_dump(id);
        }
    }
    PRINTF_E("ap->adsp total=%u adsp->ap total=%u\n\r",ap_to_adsp_ipi_count ,adsp_to_ap_ipi_count);
}

void ipi_status_dump_id(enum ipi_id id)
{
    PRINTF_E("id\trecv\tinit\twake\tsuccess\tbusy\terror\tname\n\r");
    ipi_info_dump(id);
}

/*
 * send ipi to ap
@param id:       IPI ID
*/
static void trigger_irq_to_host(void)
{
    xSemaphoreTake(xSemaphore_ipi, pdMS_TO_TICKS(0));
    hw_semaphore_get(ADSP_HW_SEMA_IPI, 0);
    /* DSP->CPU wakeup int */
    *(unsigned int *)DSP_RG_INT2CIRQ &= ~(1 << 2);
    /* DSP->CPU int */
    *(unsigned int *)DSP_RG_INT2CIRQ |= (1 << 1);
    hw_semaphore_release(ADSP_HW_SEMA_IPI);
    xSemaphoreGive(xSemaphore_ipi);
}

static void ipi_adsp2host(enum ipi_id id)
{
    adsp_to_ap_ipi_count++;
    /*ipi send success*/
    ipi_desc[id].success_count++;
    //printf("IPI_ID = %d\n",id);
#ifdef CFG_IPI_STAMP_SUPPORT
    uint32_t flag = 0;

    flag = ipi_desc[id].success_count % ADSP_IPI_ID_STAMP_SIZE;
    if (flag < ADSP_IPI_ID_STAMP_SIZE) {
        ipi_desc[id].send_flag[flag] = ipi_desc[id].success_count;
#ifdef CFG_XGPT_SUPPORT
        ipi_desc[id].send_timestamp[flag] = timer_get_global_timer_tick();
#endif
    }

#endif

    /*trigger interrupt to AP to rcv message*/
    trigger_irq_to_host();
}

static void ipc_handler()
{
    /* the higher the high priority */
    int i;
    uint64_t ipi_duration;

    for(i = IPC_MAX; i>=IPC0; i--) {
        if (adsp_info->host_to_adsp_status & (1 << i)) {
            if(ipc_desc[i].handler) {
#ifdef CFG_XGPT_SUPPORT
                ipc_desc[i].last_enter = timer_get_global_timer_tick();
#endif
                ipc_desc[i].handler();
#ifdef CFG_XGPT_SUPPORT
                ipc_desc[i].last_exit = timer_get_global_timer_tick();
#endif
                /*monitor ipc handler*/
                if(ipc_desc[i].last_exit > ipc_desc[i].last_enter) {
                    adsp_max_duration_ipc_id = i;
                    ipi_duration = ipc_desc[i].last_exit - ipc_desc[i].last_enter;
                    if (ipi_duration > ipc_desc[i].max_duration) {
                        ipc_desc[i].max_duration = ipi_duration;
                    }
                }

            }
            /*
             * after dispatch, the shared buf has been copyed
             */
            adsp_info->host_to_adsp_status &= ~(1 << i);
        }
    }
}

static void ipi_handler(void)
{
    //PRINTF_D("ipi id:%d, reg:0x%x\n\r", adsp_rcv_obj->id, GIPC_TO_ADSP_REG);
    if (adsp_rcv_obj->id >= NR_IPI) {
        PRINTF_E("wrong id:%d\n", adsp_rcv_obj->id);
    } else if (ipi_desc[adsp_rcv_obj->id].handler) {
        ap_to_adsp_ipi_count++;
        adsp_current_ipi_id = adsp_rcv_obj->id;
        ipi_desc[adsp_rcv_obj->id].recv_count ++;
#ifdef CFG_IPI_STAMP_SUPPORT
        uint32_t flag = 0;

        flag = ipi_desc[adsp_rcv_obj->id].recv_count % ADSP_IPI_ID_STAMP_SIZE;
        if (flag < ADSP_IPI_ID_STAMP_SIZE) {
            ipi_desc[adsp_rcv_obj->id].recv_flag[flag] = ipi_desc[adsp_rcv_obj->id].recv_count;
#ifdef CFG_XGPT_SUPPORT
            ipi_desc[adsp_rcv_obj->id].recv_timestamp[flag] = timer_get_global_timer_tick();
#endif
        }

#endif

#ifdef CFG_IPI_STAMP_SUPPORT
        ipi_desc[adsp_rcv_obj->id].last_handled = (uint32_t)timer_get_global_timer_tick();
#else
        ipi_desc[adsp_rcv_obj->id].last_handled = xTaskGetTickCountFromISR();
#endif
        //ipi_desc[adsp_rcv_obj->id].handler(adsp_rcv_obj->id, adsp_rcv_obj->share_buf, adsp_rcv_obj->len);
        scp_dispatch_ipi_hanlder_to_queue(
                HOST_TARGET_ID,
                adsp_rcv_obj->id,
                adsp_rcv_obj->share_buf,
                adsp_rcv_obj->len,
                ipi_desc[adsp_rcv_obj->id].handler);
    }
}

#if IPI_TEST
#define TEST_BUFFER_LEN 272
static int check_test_buffer(char* buffer,int len,unsigned int excepted)
{
    int i=0;
    for (i=0; i<len; i+=4) {
        if(*(unsigned int*)(buffer + i) != excepted)
            return 0;
        excepted += 1;
    }
    return 1;
}

static void fill_test_buffer(char* buffer,int len,unsigned int val)
{
    int i=0;
    for (i=0; i<len; i+=4) {
        *(unsigned int*)(buffer + i) = val;
            val += 1;
    }
}

#ifndef FAKE_HOST_IPC_UT_TEST
static int adsp_ipi_debug(int id, void *data, unsigned int len)
{
    char test_buffer[TEST_BUFFER_LEN] = {0};
    static unsigned int excepted = 0;
    if (!check_test_buffer(data, len, excepted))
        PRINTF_E("adsp get unexcepted data %d,excepted %d\n", *(unsigned int*)data, excepted);
    else
        PRINTF_E("adsp get excepted data %d\n", excepted);
    excepted += 1;
    fill_test_buffer(test_buffer, TEST_BUFFER_LEN, excepted);
    adsp_ipi_send(IPI_TEST1, test_buffer, TEST_BUFFER_LEN, 1, 0);
    excepted += 1;

    return 0;
}

#else
static int adsp_ipi_debug(int id, void *data, unsigned int len)
{
    char test_buffer[TEST_BUFFER_LEN] = {0};
    static unsigned int excepted = 0;
    if (!check_test_buffer(data, len, excepted))
        PRINTF_E("adsp get unexcepted data %d,excepted %d\n", *(unsigned int*)data, excepted);
    else
        PRINTF_E("adsp get excepted data %d\n", excepted);
    excepted += 1;
    fill_test_buffer(test_buffer, TEST_BUFFER_LEN, excepted);
    adsp_ipi_send(IPI_TEST_UT, test_buffer, TEST_BUFFER_LEN, 1, 0);
    excepted += 1;

    return 0;
}
static int adsp_ipi_debug_ut(int id, void *data, unsigned int len)
{
    char test_buffer[TEST_BUFFER_LEN] = {0};
    static unsigned int excepted = 1;
    if (!check_test_buffer(data, len, excepted))
        PRINTF_E("adsp get unexcepted data %d,excepted %d\n", *(unsigned int*)data, excepted);
    else
        PRINTF_E("adsp get excepted data %d\n", excepted);
    excepted += 1;
    fill_test_buffer(test_buffer, TEST_BUFFER_LEN, excepted);
    adsp_ipi_send(IPI_TEST1, test_buffer, TEST_BUFFER_LEN, 1, 0);
    excepted += 1;

    return 0;
}
static void adsp_ipi_debug_ut_thread(void *data)
{
    unsigned int begin = 0;
    char test_buffer[TEST_BUFFER_LEN] = {0};

    fill_test_buffer(test_buffer, TEST_BUFFER_LEN, begin);
    adsp_ipi_registration(IPI_TEST_UT, adsp_ipi_debug_ut, "IPIDebugUT");
    vTaskDelay(10000/portTICK_PERIOD_MS);
    PRINTF_E("audio ipi ut test\n");
    adsp_ipi_send(IPI_TEST1, test_buffer,
    TEST_BUFFER_LEN, 1, 0);
    /*
     * bootup mission is now accomplished,
     * the kthead will end self-life.
     */
    vTaskDelete(NULL);
}
#endif
#endif

static TaskHandle_t notify_thread_handle;

static void ipc_notify_thread(void* data)
{
    PRINTF_E("ipc notify thread\n");
    while (1) {
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        ipc_handler();
    }
}

static void ipc_notify_lisr(void)
{
    if(adsp_info->host_to_adsp_status & IPC_MESSAGE_READY) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(notify_thread_handle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    xSemaphoreTakeFromISR(xSemaphore_ipi, NULL);
    hw_semaphore_get(ADSP_HW_SEMA_IPI, 0);
    /* clear CPU->DSP int */
    *(unsigned int *)DSP_RG_INT2CIRQ &= ~(1 << 0);
    hw_semaphore_release(ADSP_HW_SEMA_IPI);
    xSemaphoreGiveFromISR(xSemaphore_ipi, NULL);
}

#define ALIGNED_16(x) ((x+15)/16*16)
void adsp_ipi_init(void)
{
    int32_t id;

    scp_ipi_queue_init(HOST_TARGET_ID);

    /*
     * shared memory layout
     * [adsp_common_info] [24 bytes]
     * [send obj(adsp view)] [288 bytes]
     * [rcv obj(adsp view)] [288 bytes]
     */
    adsp_info = (struct adsp_common_info *)ADSP_COMMON_INFO_ADDR;
    adsp_send_obj = (struct share_obj *)((uint32_t)adsp_info + ALIGNED_16(sizeof(struct adsp_common_info)));
    adsp_rcv_obj = (struct share_obj *)((uint32_t)adsp_send_obj + ALIGNED_16(sizeof(struct share_obj)));

    memset(adsp_info, 0, ALIGNED_16(sizeof(struct adsp_common_info)));
    memset(adsp_send_obj, 0, ALIGNED_16(sizeof(struct share_obj)));
    memset(adsp_rcv_obj, 0, ALIGNED_16(sizeof(struct share_obj)));

    adsp_info->magic = ADSP_COMMON_INFO_MAGIC;
    adsp_info->status = 0;
    adsp_info->adsp_rcv_obj_addr = (uint32_t)adsp_send_obj - (uint32_t)adsp_info; //from host view
    adsp_info->adsp_send_obj_addr = (uint32_t)adsp_rcv_obj - (uint32_t)adsp_info; //from host view
    adsp_info->adsp_to_host_status = 0;
    adsp_info->host_to_adsp_status = 0;

    PRINTF_E("adsp_info = %p \n", adsp_info);
    PRINTF_E("adsp_send_obj = %p \n", adsp_send_obj);
    PRINTF_E("adsp_rcv_obj = %p \n", adsp_rcv_obj);
    PRINTF_E("adsp_rcv_obj->share_buf = %p \n", adsp_rcv_obj->share_buf);
    /*init ipi_desc*/
    for (id = 0; id < NR_IPI; id++) {
        ipi_desc[id].name = "";
        ipi_desc[id].init_count++;
        ipi_desc[id].handler = NULL;
    }
    /*init ipc_desc*/
    for (id = 0; id < IPC_MAX; id++) {
        ipc_desc[id].name = "";
        ipc_desc[id].max_duration = 0;
        ipc_desc[id].last_exit = 0;
        ipc_desc[id].last_enter = 0;
        ipc_desc[id].handler = NULL;
    }

    xSemaphore_ipi = xSemaphoreCreateBinary();

    kal_xTaskCreate(
        ipc_notify_thread,
        "ipc notify",
        configMINIMAL_STACK_SIZE,
        NULL,
        configMAX_PRIORITIES - 1,
        &notify_thread_handle);

    request_irq(IPC_INT_NUM, ipc_notify_lisr, "ipc_notify");

    request_ipc(IPC0, ipi_handler, "IPI");
    ipi_init_ready = 1;

#if IPI_TEST
    adsp_ipi_registration(IPI_TEST1, adsp_ipi_debug, "IPIDebug");
#ifdef FAKE_HOST_IPC_UT_TEST
    kal_xTaskCreate(
         adsp_ipi_debug_ut_thread,
         "ipiut",
         configMINIMAL_STACK_SIZE,
         NULL,
         configMAX_PRIORITIES - 1,
         NULL);
#endif
#endif
}

/*
@param id:       IPI ID
@param handler:  IPI handler
@param name:     IPI name
*/
ipi_status adsp_ipi_registration(uint32_t id, ipi_handler_t handler, const char *name)
{
    if (id < NR_IPI && ipi_init_ready == 1) {
        ipi_desc[id].name = name;

        if (handler == NULL)
            return ERROR;

        ipi_desc[id].handler = handler;
        return DONE;

    } else {
        PRINTF_E("[IPI]id:%d, ipi_init_ready:%u\n", id, ipi_init_ready);
        return ERROR;
    }
}

/*
@param id:       IPI ID
*/
ipi_status adsp_ipi_unregistration(uint32_t id)
{
    if (id < NR_IPI && ipi_init_ready == 1) {
        ipi_desc[id].handler = NULL;
        return DONE;
    } else {
        PRINTF_E("[IPI]unregi err, id:%d, init_ready:%u\n", id, ipi_init_ready);
        return ERROR;
    }
}

/*
@param id:       IPI ID
@param buf:      the pointer of data
@param len:      data length
@param wait:     If true, wait (atomically) until data have been gotten by Host
*/
ipi_status adsp_ipi_send(uint32_t id, void* buf, uint32_t len, uint32_t wait, enum ipi_dir dir)
{
    if (id >= NR_IPI) {
        PRINTF_E("[IPI] adsp ipi id %u is larger than NR_IRI %u\n", id, NR_IPI);
        return ERROR;
    }
    return (scp_send_msg_to_queue(HOST_TARGET_ID,id, buf, len, adsp_ipi_send_to_host) == 0) ? DONE : ERROR;
}


ipi_status adsp_ipi_send_internal(enum ipi_id id, void* buf, uint32_t len, uint32_t wait, enum ipi_dir dir)
{
   // uint32_t ipi_idx;
    UBaseType_t uxSavedInterruptStatus;
   /*add timer count*/
   unsigned long long systimer_start = 0;
   unsigned long long systimer_end = 0;
   unsigned long long systimer_diff = 0;

    /*avoid adsp log print too much*/
    if (adsp_ipi_id_record == id)
        adsp_ipi_id_record_count++;
    else
        adsp_ipi_id_record_count = 0;

    adsp_ipi_id_record = id;

    if (is_in_isr() && wait) {
        /*prevent from infinity wait when be in isr context*/
        configASSERT(0);
    }

    if (id < NR_IPI) {
        if (len > sizeof(adsp_send_obj->share_buf) || buf == NULL) {
            /*ipi send error*/
            ipi_desc[id].error_count++;
            return ERROR;
        }
        uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        /*check if there is already an ipi pending in AP*/
        if (DRV_Reg32(&adsp_info->adsp_to_host_status) & IPC_MESSAGE_READY) { //liang modi: 0 to assert irq
            /*If the following conditions meet,
             * 1)there is an ipi pending in AP
             * 2)the coming IPI is a wakeup IPI
             * so it assumes that AP is in suspend state
             * send a AP wakeup request to SPM
             * */
            /*the coming IPI will be checked if it's a wakeup source*/
          /*  try_to_wakeup_ap(id);  --liang temp mark for FPGA */

            /* avoid adsp log print too much
             * %==0 : switch between different IPI ID
             * %==1 : the same IPI ID comes again
             */
            if ((adsp_ipi_id_record_count % IPI_PRINT_THRESHOLD == 0) ||
                    (adsp_ipi_id_record_count % IPI_PRINT_THRESHOLD == 1)) {
                PRINTF_E("ipi busy,owner:%d\n", adsp_ipi_owner);
            }
            portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
            /*ipi send busy*/
            ipi_desc[id].busy_count++;
            return BUSY;
        }

        adsp_send_obj->len = len;
        adsp_send_obj->id = id;
        memcpy((void *)adsp_send_obj->share_buf, buf, len);
        //PRINTF_E("ipi id:%d len:%d\n", id, len);
        DRV_SetReg32(&adsp_info->adsp_to_host_status,IPC_MESSAGE_READY);
        ipi_adsp2host(id);
        /*get ipi owner*/
        adsp_ipi_owner = id;
       // g_ipi_count++;
       // ipi_idx = g_ipi_count;
        portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
	systimer_start = read_systimer_stamp_ns();
        if (wait)
            while (DRV_Reg32(&adsp_info->adsp_to_host_status) & IPC_MESSAGE_READY){
			systimer_end = read_systimer_stamp_ns();
			systimer_diff = systimer_end - systimer_start;
			if (systimer_diff >= 2000000)
				vTaskDelay(1/portTICK_PERIOD_MS);
		}
    } else
        return ERROR;

    return DONE;
}

ipi_status adsp_ipi_send_for_ut(enum ipi_id id, void* buf, uint32_t len, uint32_t wait, enum ipi_dir dir)
{
   // uint32_t ipi_idx;
    UBaseType_t uxSavedInterruptStatus;
    /*avoid adsp log print too much*/
    if (adsp_ipi_id_record == id)
        adsp_ipi_id_record_count++;
    else
        adsp_ipi_id_record_count = 0;

    adsp_ipi_id_record = id;

    if (is_in_isr() && wait) {
        /*prevent from infinity wait when be in isr context*/
        configASSERT(0);
    }

    if (id < NR_IPI) {
        if (len > sizeof(adsp_rcv_obj->share_buf) || buf == NULL) {
            /*ipi send error*/
            ipi_desc[id].error_count++;
            return ERROR;
        }
        uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        /*check if there is already an ipi pending in AP*/
        if (DRV_Reg32(&adsp_info->host_to_adsp_status) & IPC_MESSAGE_READY) { //liang modi: 0 to assert irq
            /*If the following conditions meet,
             * 1)there is an ipi pending in AP
             * 2)the coming IPI is a wakeup IPI
             * so it assumes that AP is in suspend state
             * send a AP wakeup request to SPM
             * */
            /*the coming IPI will be checked if it's a wakeup source*/
          /*  try_to_wakeup_ap(id);  --liang temp mark for FPGA */

            /* avoid adsp log print too much
             * %==0 : switch between different IPI ID
             * %==1 : the same IPI ID comes again
             */
            if ((adsp_ipi_id_record_count % IPI_PRINT_THRESHOLD == 0) ||
                    (adsp_ipi_id_record_count % IPI_PRINT_THRESHOLD == 1)) {
                PRINTF_E("ipi busy,owner:%d\n", adsp_ipi_owner);
            }
            portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
            /*ipi send busy*/
            ipi_desc[id].busy_count++;
            return BUSY;
        }

        adsp_rcv_obj->len = len;
        adsp_rcv_obj->id = id;
        memcpy((void *)adsp_rcv_obj->share_buf, buf, len);
        //PRINTF_E("ipi id:%d len:%d\n", id, len);
        DRV_SetReg32(&adsp_info->host_to_adsp_status,IPC_MESSAGE_READY);
        /*get ipi owner*/
        adsp_ipi_owner = id;
       // g_ipi_count++;
       // ipi_idx = g_ipi_count;
        portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);

        /* wakeup ipc notify thread */
        vTaskNotifyGiveFromISR(notify_thread_handle, NULL);

        if (wait)
            while (DRV_Reg32(&adsp_info->host_to_adsp_status) & IPC_MESSAGE_READY);
    } else
        return ERROR;

    return DONE;
}

static enum ipi_status adsp_ipi_send_to_host(int id, void *buf, unsigned int  len)
{
#ifndef FAKE_HOST_IPC_UT
    return adsp_ipi_send_internal(id, buf, len, 1, IPI_ADSP2AP);
#else
    return adsp_ipi_send_for_ut(id, buf, len, 1, IPI_ADSP2AP);
#endif
}

/**
 * @brief check ADSP -> AP IPI is using now
 * @return pdFALSE, IPI is NOT using now
 * @return pdTRUE, IPI is using, and AP does not receive the IPI yet.
 */
uint32_t is_ipi_busy(void)
{
    return (adsp_info->adsp_to_host_status & IPC_MESSAGE_READY) ? pdTRUE:pdFALSE;
}

/** register a ipc handler
*
*  @param ipc number
*  @param ipc handler
*  @param ipc name
*
*  @returns
*    no return
*/
void request_ipc(uint32_t ipc_num, ipc_handler_t handler, const char *name)
{
    if (ipc_num < IPC_MAX) {
        ipc_desc[ipc_num].handler = handler;
        ipc_desc[ipc_num].name = name;
    }
}

