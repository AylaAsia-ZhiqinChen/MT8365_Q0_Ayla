/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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
 * THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
 * applicable license agreements with MediaTek Inc.
 */

#include <string.h>
#include "debug.h"
#include "ipi.h"
#include "ipi_types.h"
#include "ipi_table.h"


/* used for IPI module isr to sync with its task */
SemaphoreHandle_t sema_ipi_task[TOTAL_RECV_PIN];
SemaphoreHandle_t mutex_ipi_reg;

#ifdef MET_TRACE_ENABLE
static char bsema_name[] = "IPI00";
static char ackbs_name[] = "ACK00";
#endif
#ifdef MET_TAG
#include "met_tag.h"
int tagid_ipiack;
#endif

#define IPI_MONITOR
#ifdef IPI_MONITOR
#define IPI_MONITOR_TIMESTAMP
struct ipi_monitor {
    unsigned int has_time: 1,    /* 0: has no timestamp of t1/t2/t3 otherwise 1*/
                 state   : 2,    /* 0: no IPI, 1: t1 finished, 2: t2 finished, 3: t3 finished */
                 seqno   : 29;   /* count of the IPI pin used */
#ifdef IPI_MONITOR_TIMESTAMP
    unsigned long long t1;
    unsigned long long t2;
    unsigned long long t3;
#endif /* IPI_MONITOR_TIMESTAMP */
};
static struct ipi_monitor ipimon[IPI_ID_TOTAL];
static int ipi_last;
#endif

//#define GET_IPI_TIMESTAMP
#ifdef GET_IPI_TIMESTAMP
#define IPI_TS_TEST_MAX     20
#define IPI_TS_TEST_PIN     IPI_ID_PMIC_WRAP
static unsigned long long ipi_t1[IPI_TS_TEST_MAX];
static unsigned long long ipi_t2[IPI_TS_TEST_MAX];
static unsigned long long ipi_t3[IPI_TS_TEST_MAX];
static int ipi_test_cnt = 0;
#endif

#if defined (GET_IPI_TIMESTAMP) || defined (IPI_MONITOR_TIMESTAMP)
#include "ostimer.h"
static unsigned long long ipi_get_timestamp(void)
{
    struct sys_time_t apmcu_ts;
    int ret;

    if (is_in_isr())
        ret = ts_apmcu_time_isr(&apmcu_ts);
    else /* CTX_TASK */
        ret = ts_apmcu_time(&apmcu_ts);

    if (0 == ret) { /* APMCU in running mode */
        return apmcu_ts.ts;
    } else { /* APMCU in suspend mode */
        return apmcu_ts.ts + apmcu_ts.off_ns;
    }
}
#endif /* GET_IPI_TIMESTAMP or IPI_MONITOR_TIMESTAMP */

static void ipi_isr_cb(unsigned int mbno, unsigned int base, unsigned int irq);

int ipi_init(void)
{
    int i, mcnt;

    mutex_ipi_reg = xSemaphoreCreateMutex();
    // configASSERT(mutex_ipi_reg);
    for (i = 0; i < TOTAL_SEND_PIN; i++) {
        send_pintable[i].mutex_send = xSemaphoreCreateMutex();
        // configASSERT(send_pintable[i].mutex_send);
        send_pintable[i].sema_ack = xSemaphoreCreateBinary();
        // configASSERT(send_pintable[i].sema_ack);
#ifdef MET_TRACE_ENABLE
        //snprintf(bsema_name + 3, 2, "%02d", mid);
        ackbs_name[3] = (i / 10) + '0';
        ackbs_name[4] = (i % 10) + '0';
        vQueueAddToRegistry(send_pintable[i].sema_ack, ackbs_name);
#endif
    }
#ifdef MET_TAG
    tagid_ipiack = met_tag_create("IPIACK");
#endif

#ifdef IPI_MONITOR_TIMESTAMP
    for (i = 0; i < IPI_ID_TOTAL; i++) {
        ipimon[i].has_time = 1;
    }
#endif

    /* IPI HW initialize and ISR registration */
    mcnt = mbox_init(IPI_MBOX_MODE, maddr, IPI_MBOX_TOTAL, ipi_isr_cb);

    /* PRINTF_E("IPI init mode=%x, total mbox=%d, rpin=%d, spin=%d\n",
             IPI_MBOX_MODE, mcnt, TOTAL_RECV_PIN, TOTAL_SEND_PIN); */

    return gen_table_tag(mcnt);
}

int ipi_recv_registration(int mid, ipi_action_t *act)
{
    ipi_action_t *tmp;
    pin_recv_t *pin = NULL;

    if ((mid < 0) || (mid >= TOTAL_RECV_PIN))
        return (IPI_SERVICE_NOT_AVAILABLE);
    if (act == NULL)
        return (IPI_REG_ACTION_ERROR);

    pin = &(recv_pintable[mid]);
    act->next = NULL;
    act->id = mid;

    xSemaphoreTake(mutex_ipi_reg, portMAX_DELAY);
    if (pin->act != NULL) {
        xSemaphoreGive(mutex_ipi_reg);
        return (IPI_REG_ALREADY);
    }
    pin->act = act;

    xSemaphoreGive(mutex_ipi_reg);
    sema_ipi_task[mid] = xSemaphoreCreateBinary();

    if (sema_ipi_task[mid] == NULL)
        return (IPI_REG_SEMAPHORE_FAIL);

#ifdef MET_TRACE_ENABLE
    //snprintf(bsema_name + 3, 2, "%02d", mid);
    bsema_name[3] = (mid / 10) + '0';
    bsema_name[4] = (mid % 10) + '0';
    vQueueAddToRegistry(sema_ipi_task[mid], bsema_name);
#endif

    return (IPI_REG_OK);
}

int ipi_recv_wait(int mid)
{
    xSemaphoreTake(sema_ipi_task[mid], portMAX_DELAY);
#ifdef GET_IPI_TIMESTAMP
    if ((mid == IPI_TS_TEST_PIN) && (ipi_test_cnt < IPI_TS_TEST_MAX)) {
        ipi_t2[ipi_test_cnt] = ipi_get_timestamp();
    }
#endif /* GET_IPI_TIMESTAMP */
#ifdef IPI_MONITOR
#ifdef IPI_MONITOR_TIMESTAMP
    ipimon[mid].t2 = ipi_get_timestamp();;
#endif /* IPI_MONITOR_TIMESTAMP */
    ipimon[mid].state = 2;
#endif /* IPI_MONITOR */

    return (0);
}

SemaphoreHandle_t ipi_get_wait(int mid)
{
    return sema_ipi_task[mid];
}

static void ipi_do_ack(mbox_info_t *mbox, unsigned int in_irq, unsigned int base, BaseType_t *ret_wake)
{
    int idx_end = mbox->end;
    int idx_start = mbox->start;
    int i;
    pin_send_t *pin;
    BaseType_t need_wake = 0;

    (*ret_wake) = 0;
    pin = &(send_pintable[idx_start]);
    for (i = idx_start; i <= idx_end; i++, pin++) {
        if ((in_irq & 0x01) == 0x01) { /* irq bit enable */
            if (pin->retdata) {
                pin->prdata = (uint32_t*)(base + ((pin->slot) * MBOX_SLOT_SIZE));
            }
            /* give retval semaphore to ipi_send_async_wait() */
            xSemaphoreGiveFromISR(pin->sema_ack, &need_wake);
            (*ret_wake) |= need_wake;
        }
        in_irq >>= 1;
    }
}

static void ipi_do_recv(mbox_info_t *mbox, unsigned int in_irq, unsigned int base, BaseType_t *ret_wake)
{
    /* executed from ISR */
    /* get the value from INT_IRQ_x (MD32 side) or OUT_IRQ_0 (Linux side) */
    int idx_end = mbox->end;
    int idx_start = mbox->start;
    int i;
    pin_recv_t *pin;
    ipi_action_t *action;
    BaseType_t need_wake = 0;

    if (in_irq == 0)
        return;

    /* check each bit for interrupt triggered */
    /* the bit is used to determine the index of callback array */
    (*ret_wake) = 0;
    pin = &(recv_pintable[idx_start]);
    for (i = idx_start; i <= idx_end; i++, pin++) {
        if ((in_irq & 0x01) == 0x01) { /* irq bit enable */

#ifdef GET_IPI_TIMESTAMP
            if ((i == IPI_TS_TEST_PIN) && (ipi_test_cnt < IPI_TS_TEST_MAX)) {
                ipi_t1[ipi_test_cnt] = ipi_get_timestamp();
            }
#endif /* GET_IPI_TIMESTAMP */
#ifdef IPI_MONITOR
            ipimon[i].seqno++;
#ifdef IPI_MONITOR_TIMESTAMP
            ipimon[i].t1 = ipi_get_timestamp();
            ipimon[i].t2 = ipimon[i].t3 = 0;
#endif /* IPI_MONITOR_TIMESTAMP */
            ipimon[i].state = 1;
#endif /* IPI_MONITOR */

            action = pin->act;
            if (action != NULL) {
                /* do the action */
                if (action->data != NULL) {
                    memcpy(action->data, (void*)(base + (pin->slot * MBOX_SLOT_SIZE)),
                           pin->size * MBOX_SLOT_SIZE);
                }
                xSemaphoreGiveFromISR(sema_ipi_task[i], &need_wake);
                (*ret_wake) |= need_wake;
            }
        } /* check bit is enabled */
        in_irq >>= 1;
    } /* check INT_IRQ bits */
}

int ipi_send_sync(int mid, int opts, void *buffer, int len, void *retbuf)
{
    int ret;

    if (opts & IPI_OPT_NO_WAIT_ACK) {
        return (IPI_SERVICE_NOT_AVAILABLE);
    }

    ret = ipi_send_async(mid, 0, buffer, len);
    if (ret != 0) {
        return (ret);
    }
    ret = ipi_send_async_wait(mid, opts, retbuf);
    return (ret);
}

int ipi_send_sync_ex(int mid, int opts, void *buffer, int len, void *retbuf, int retlen)
{
    int ret;

    if (opts & IPI_OPT_NO_WAIT_ACK) {
        return (IPI_SERVICE_NOT_AVAILABLE);
    }

    ret = ipi_send_async(mid, 0, buffer, len);
    if (ret != 0) {
        return (ret);
    }
    ret = ipi_send_async_wait_ex(mid, opts, retbuf, retlen);
    return (ret);
}

int ipi_send_async(int mid, int opts, void *buffer, int len)
{
    int mbno, ret;
    pin_send_t *pin;
    mbox_info_t *mbox;

    if ((mid < 0) || (mid >= TOTAL_SEND_PIN))
        return (IPI_SERVICE_NOT_AVAILABLE);

    pin = &(send_pintable[mid]);

    if (len > pin->size) {
        return (IPI_NO_MEMORY);
    }

    if (opts & IPI_OPT_NO_WAIT_ACK) {
        if (xSemaphoreTake(pin->mutex_send, 0) != pdTRUE) {
            return (IPI_BUSY);
        }
    }
    else {
        if (xSemaphoreTake(pin->mutex_send, portMAX_DELAY) != pdTRUE) {
            return (IPI_TIMEOUT_AVL);
        }
    }

    mbno = pin->mbox;
    mbox = &(mbox_table[mbno]);
    /* note: the bit of INT(OUT)_IRQ is depending on mid */
    if (len == 0) {
        len = pin->size;
    }
    ret = mbox_send(mbno, pin->slot, mid - mbox->start, buffer, len);
    if (ret != 0) {
        xSemaphoreGive(pin->mutex_send);
        return (IPI_HW_ERROR);
    }

    if (opts & IPI_OPT_NO_WAIT_ACK) {
        xSemaphoreGive(pin->mutex_send);
    }
    return (IPI_DONE);
}

int ipi_send_async_wait(int mid, int opts, void *retbuf)
{
    int len = 1;
    if (retbuf == NULL) {
        len = 0;
    }
    return ipi_send_async_wait_ex(mid, opts, retbuf, len);
}

int ipi_send_async_wait_ex(int mid, int opts, void *retbuf, int retlen)
{
    pin_send_t *pin;

    if ((mid < 0) || (mid >= TOTAL_SEND_PIN))
        return (IPI_SERVICE_NOT_AVAILABLE);

    pin = &(send_pintable[mid]);
    if (retlen > pin->size) {
        xSemaphoreGive(pin->mutex_send);
        return (IPI_NO_MEMORY);
    }
    /* fix me later: assign the delay value or use portMAX_DELAY? */
    if (xSemaphoreTake(pin->sema_ack, portMAX_DELAY) != pdTRUE) {
        xSemaphoreGive(pin->mutex_send);
        return (IPI_TIMEOUT_AVL);
    }
    if ((pin->retdata != 0) && (retbuf != NULL)) {
        /* copy return value to retbuf */
        memcpy(retbuf, pin->prdata, (MBOX_SLOT_SIZE * retlen));
    }

    xSemaphoreGive(pin->mutex_send);
    return (IPI_DONE);
}

int ipi_send_ack(int mid, unsigned int *data)
{
    int len = 1;
    if (data == NULL) {
        len = 0;
    }
    return ipi_send_ack_ex(mid, (void*)data, len);
}

int ipi_send_ack_ex(int mid, void *data, int retlen)
{
    pin_recv_t *pin;
    mbox_info_t *mbox;
    int len, mbno, irq, slot, ret;

    if ((mid < 0) || (mid >= TOTAL_RECV_PIN))
        return (IPI_SERVICE_NOT_AVAILABLE);

    pin = &(recv_pintable[mid]);
    if (retlen > pin->size) {
        return (IPI_NO_MEMORY);
    }
    mbno = pin->mbox;
    mbox = &(mbox_table[mbno]);
    irq = mid - (mbox->start);
    /* return data length */
    if ((pin->retdata != 0) && (data != NULL))
        len = retlen;
    else
        len = 0;
    /* where to put the return data */
    slot = pin->slot;

    ret = mbox_send(mbno, slot, irq, data, len);
    if (ret) {
        return (-1);
    }

#ifdef GET_IPI_TIMESTAMP
    if ((mid == IPI_TS_TEST_PIN) && (ipi_test_cnt < IPI_TS_TEST_MAX)) {
        ipi_t3[ipi_test_cnt] = ipi_get_timestamp();
        ipi_test_cnt++;
    }
    if (ipi_test_cnt >= IPI_TS_TEST_MAX) {
        int i;
        for (i=0; i<IPI_TS_TEST_MAX; i++) {
            PRINTF_E("IPI %d: t1=%lld, t2=%lld, t3=%lld, ts=%lld\n",
                     i, ipi_t1[i], ipi_t2[i], ipi_t3[i], ipi_get_timestamp());
        }
        ipi_test_cnt = 0;
    }
#endif /* GET_IPI_TIMESTAMP */
#ifdef IPI_MONITOR
#ifdef IPI_MONITOR_TIMESTAMP
    ipimon[mid].t3 = ipi_get_timestamp();;
#endif /* IPI_MONITOR_TIMESTAMP */
    ipimon[mid].state = 3;
    ipi_last = mid;
#endif /* IPI_MONITOR */

#ifdef MET_TAG
    met_tag_call(tagid_ipiack, mid, CTX_TASK, MID_TAG_ONESHOT);
#endif
    return (0);
}

static void ipi_isr_cb(unsigned int mbno, unsigned int base, unsigned int irq)
{
    BaseType_t ret_wake = 0;
    mbox_info_t *mbox;

    if (mbno >= IPI_MBOX_TOTAL)
        return;

#if defined(MET_TRACE_ENABLE) || defined(PBFR_SUPPORT_REC_ISR)
    traceISR_ENTER();
#endif

    mbox = &(mbox_table[mbno]);

    if (mbox->mode == 2) { /* ipi_do_ack */
        ipi_do_ack(mbox, irq, base, &ret_wake);
    } else if (mbox->mode == 1) { /* ipi_do_recv */
        ipi_do_recv(mbox, irq, base, &ret_wake);
    }

    portYIELD_FROM_ISR(ret_wake);

#if defined(MET_TRACE_ENABLE) || defined(PBFR_SUPPORT_REC_ISR)
    traceISR_EXIT();
#endif

    return;
}

#if 0
#ifdef IPI_MONITOR
void ipi_monitor_print(void)
{
    int i;
    for (i=0; i<IPI_ID_TOTAL; i++) {
#ifdef IPI_MONITOR_TIMESTAMP
        PRINTF_E("IPI %d: seqno=%d, state=%d, t1=%lld, t2=%lld, t3=%lld\n",
                 i, ipimon[i].seqno, ipimon[i].state,
                 ipimon[i].t1, ipimon[i].t2, ipimon[i].t3);
#else
        PRINTF_E("IPI %d: seqno=%d, state=%d\n",
                 i, ipimon[i].seqno, ipimon[i].state);
#endif /* IPI_MONITOR_TIMESTAMP */
    }
}
#endif /* IPI_MONITOR */
#endif
