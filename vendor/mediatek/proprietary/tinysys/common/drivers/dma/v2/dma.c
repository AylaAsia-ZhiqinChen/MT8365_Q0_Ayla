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

#include <stdio.h>
#include <debug.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "driver_api.h"
#include <tinysys_reg.h>
#include "dma.h"
#include "irq.h"

#define DMA_OFS(x)          ((unsigned int) (x) << 8)

#define DMA_START_STR       ((unsigned int) 1 << 15)
#define DMA_INTSTA_INT      ((unsigned int) 1 << 15)
#define DMA_ACKINT_ACK      ((unsigned int) 1 << 15)
#define DMA_CON_DIR         ((unsigned int) 1 << 18)
#define DMA_CON_WPEN        ((unsigned int) 1 << 17)
#define DMA_CON_WPSD        ((unsigned int) 1 << 16)
#define DMA_CON_ITEN        ((unsigned int) 1 << 15)
#define DMA_CON_BURST_1     ((unsigned int) (0x0) << 8)
#define DMA_CON_BURST_4     ((unsigned int) (0x2) << 8)
#define DMA_CON_B2W         ((unsigned int) 1 << 5)
#define DMA_CON_DRQ         ((unsigned int) 1 << 4)
#define DMA_CON_DINC        ((unsigned int) 1 << 3)
#define DMA_CON_SINC        ((unsigned int) 1 << 2)
#define DMA_CON_SIZE_1B     ((unsigned int) (0x0) << 0)
#define DMA_CON_SIZE_2B     ((unsigned int) (0x1) << 0)
#define DMA_CON_SIZE_4B     ((unsigned int) (0x2) << 0)
#define DMA_ADDR_FMT(addr)  (((unsigned int) addr) & 0xFFFFFFFF)  // 0x13FFFFFF)
#define DMA_ADDR_CHK(addr)  (((unsigned int) addr) & 0x00000001)  // 0xEC000001)  // 2 bytes align
#define DMA_LEN_MASK        ((unsigned int) 0xFFFF)

#define DMA_SRC(x)          (DMA1_SRC + DMA_OFS(x))
#define DMA_DST(x)          (DMA1_DST + DMA_OFS(x))
#define DMA_WPPT(x)         (DMA1_WPPT + DMA_OFS(x))
#define DMA_WPTO(x)         (DMA1_WPTO + DMA_OFS(x))
#define DMA_COUNT(x)        (DMA1_COUNT + DMA_OFS(x))
#define DMA_CON(x)          (DMA1_CON + DMA_OFS(x))
#define DMA_START(x)        (DMA1_START + DMA_OFS(x))
#define DMA_INTSTA(x)       (DMA1_INTSTA + DMA_OFS(x))
#define DMA_ACKINT(x)       (DMA1_ACKINT + DMA_OFS(x))
#define DMA_RLCT(x)         (DMA1_RLCT + DMA_OFS(x))
#define DMA_LIMITER(x)      (DMA1_LIMITER + DMA_OFS(x))

struct dma_descr_t {
    unsigned int status;
    SemaphoreHandle_t sem;
};

static struct dma_descr_t dma_descr[DMA_NUM];

static unsigned int _dma_find_ch(unsigned int dma_ch, struct dma_descr_t **descr)
{
    if (dma_ch != DMA_NUM) {
        if (dma_ch < DMA_NUM && dma_descr[dma_ch].status == 0)
            goto found;

        return DMA_NUM;
    }

    for (dma_ch = 0; dma_ch < DMA_NUM; dma_ch++) {
        if (dma_descr[dma_ch].status == 0)
            break;
    }

    if (dma_ch >= DMA_NUM)
        return DMA_NUM;

found:
    *descr = &dma_descr[dma_ch];

    (*descr)->status = 0x1;

    return dma_ch;
}

static int _dma_ch_setup(unsigned int dma_ch, struct dma_addr_t *src, struct dma_addr_t *dst,
                         unsigned int len, unsigned int flags)
{
    unsigned int val, con, wrap_len;
    struct dma_addr_t *wrap;

    DRV_WriteReg32(DMA_SRC(dma_ch), DMA_ADDR_FMT(src->current));
    DRV_WriteReg32(DMA_DST(dma_ch), DMA_ADDR_FMT(dst->current));

    if ((len & 0x3) == 0) {
        val = len >> 2;
        con = DMA_CON_SIZE_4B;
    } else {
        val = len >> 1;
        con = DMA_CON_SIZE_2B;
    }

    if ((val > DMA_LEN_MASK) || (val == 0))
        return DMA_ERR_SIZE;

    if ((flags & DMA_FLAG_WRAP_SRC) || (flags & DMA_FLAG_WRAP_DST)) {
        wrap = (flags & DMA_FLAG_WRAP_SRC) ? src : dst;

        if (DMA_ADDR_CHK(wrap->base))
            return DMA_ERR_ADDR;

        wrap_len = wrap->wrap_len;
        wrap_len >>= (con == DMA_CON_SIZE_4B) ? 2 : 1;

        if (wrap_len == 0)
            return DMA_ERR_SIZE;

        DRV_WriteReg32(DMA_WPPT(dma_ch), wrap_len);
        DRV_WriteReg32(DMA_WPTO(dma_ch), DMA_ADDR_FMT(wrap->base));

        if (!(flags & DMA_FLAG_WRAP_SRC))
            con |= DMA_CON_WPSD;

        con |= DMA_CON_WPEN;
    }

    DRV_WriteReg32(DMA_COUNT(dma_ch), val);

    // no need to set DREQ and DIR
    con |= DMA_CON_SINC | DMA_CON_DINC | DMA_CON_BURST_4;

    if (flags & DMA_FLAG_INT)
        con |= DMA_CON_ITEN;

    DRV_WriteReg32(DMA_CON(dma_ch), con);

    return 0;
}

static int _dma_ch_start(unsigned int dma_ch)
{
    DRV_WriteReg32(DMA_START(dma_ch), DMA_START_STR);

    return 0;
}

static int _dma_ch_intsta(unsigned int dma_ch)
{
    unsigned int val;

    val = DRV_Reg32(DMA_INTSTA(dma_ch));

    return (val & DMA_INTSTA_INT);
}

static void _dma_ch_ackint(unsigned int dma_ch)
{
    DRV_WriteReg32(DMA_ACKINT(dma_ch), DMA_ACKINT_ACK);
}

unsigned int _dma_ch_poll(unsigned int dma_ch)
{
    unsigned int val, mask;

    mask = 1 << (dma_ch * 2);

    val = DRV_Reg32(DMA_GLBSTA);

    // MD32 can not use this
//  if (val & mask)
    if ((val & mask) != 0)
        return 1;

    DRV_WriteReg32(DMA_START(dma_ch), 0x0);
    dma_descr[dma_ch].status = 0;
    dma_descr[dma_ch].sem = NULL;
    return 0;
}

unsigned int dma_ch_poll(unsigned int dma_ch)
{
    unsigned int ret;

    if (dma_ch >= DMA_NUM)
        return 0;

    taskENTER_CRITICAL();
    ret = _dma_ch_poll(dma_ch);
    taskEXIT_CRITICAL();

    return ret;
}

int dma_transaction(unsigned int dma_ch, struct dma_addr_t *src, struct dma_addr_t *dst, unsigned int len,
                    SemaphoreHandle_t sem, unsigned int flags)
{
    struct dma_descr_t *descr;
    int ret;

    if (DMA_ADDR_CHK(src->current))
        return DMA_ERR_ADDR;

    if (DMA_ADDR_CHK(dst->current))
        return DMA_ERR_ADDR;

    if ((flags & DMA_FLAG_INT) && (flags & DMA_FLAG_WAIT))
        return DMA_ERR_FLAGS;

    if (!(flags & DMA_FLAG_SETCH))
        dma_ch = DMA_NUM;

    if ((flags & DMA_FLAG_WRAP_SRC) && (flags & DMA_FLAG_WRAP_DST))
        return DMA_ERR_FLAGS;

    taskENTER_CRITICAL();

    dma_ch = _dma_find_ch(dma_ch, &descr);

    if (dma_ch == DMA_NUM) {
        ret = -1;
        goto exit;
    }

    if ((ret = _dma_ch_setup(dma_ch, src, dst, len, flags)) != 0)
        goto error;

    if (flags & DMA_FLAG_INT)
        descr->sem = sem;

    if ((ret = _dma_ch_start(dma_ch)) != 0)
        goto error;

    taskEXIT_CRITICAL();

    if (flags & DMA_FLAG_WAIT)
        while (dma_ch_poll(dma_ch)) {};

    return dma_ch;

error:
    descr->status = 0;
exit:
    taskEXIT_CRITICAL();

    return ret;
}

unsigned int dma_isr(void *data)
{
    unsigned int dma_ch;
    struct dma_descr_t *descr;
    SemaphoreHandle_t sem;

    for (dma_ch = 0; dma_ch < DMA_NUM; dma_ch++) {
        if (!_dma_ch_intsta(dma_ch))
            continue;

        _dma_ch_ackint(dma_ch);

        descr = &dma_descr[dma_ch];

        sem = descr->sem;

        if (!_dma_ch_poll(dma_ch) && sem)
            xSemaphoreGiveFromISR(sem, NULL);
    }

    PRINTF_D_ISR("DMA init done\n");

    return 0;
}
