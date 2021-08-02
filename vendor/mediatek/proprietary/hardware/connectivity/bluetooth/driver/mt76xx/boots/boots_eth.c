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
 * MediaTek Inc. (C) 2016~2017. All rights reserved.
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

#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include "boots.h"
#include "boots_eth.h"

//---------------------------------------------------------------------------
#define LOG_TAG "boots_eth"

//---------------------------------------------------------------------------
int boots_eth_create(int * sk, struct sockaddr_in * sockaddr, int client)
{
    UNUSED(boots_btif);

    // Create server socket
    *sk = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*sk < 0) {
        BPRINT_E("Create socket failed: %s(%d)", strerror(errno), errno);
        return errno;
    }

    // Init server socket
    if (!client) {
        memset(sockaddr, 0, sizeof(struct sockaddr_in));
        sockaddr->sin_family = AF_INET;
        sockaddr->sin_port = ETH_UDP_SRV_PORT;
        sockaddr->sin_addr.s_addr = INADDR_ANY;
        if (bind(*sk, (struct sockaddr *)sockaddr, sizeof(struct sockaddr)) < 0) {
            BPRINT_E("Bind socket failed: %s(%d)", strerror(errno), errno);
            return errno;
        }
    }

    BPRINT_D("Socket %s create success", client ? "Client" : "Server");
    return 0;
}

//---------------------------------------------------------------------------
int boots_eth_close(int * sk)
{
    if (sk == NULL) {
        BPRINT_E("%s: Incorrect socket", __func__);
        return -1;
    }
    close(*sk);
    *sk = -1;
    return 0;
}

//---------------------------------------------------------------------------
ssize_t boots_eth_send(int s, const void * buf, size_t len, struct sockaddr_in * to)
{
    ssize_t ret = 0;

    if (!buf || !len) {
        BPRINT_E("%s: Incorrect parameters", __func__);
        return -1;
    }
#if 1
    ret = sendto(s, buf, len, 0, (const struct sockaddr *)to,
            sizeof(struct sockaddr));
#else
    ret = send(s, buf, len, 0);
#endif
    if (ret < 0)
        BPRINT_E("%s: Send message failed: %s(%d)", __func__, strerror(errno), errno);

    return ret;
}

//---------------------------------------------------------------------------
ssize_t boots_eth_recv(int s, const void * buf, size_t len, int flags)
{
    ssize_t ret = 0;
    struct sockaddr srcaddr;
    socklen_t srcaddr_len = sizeof(struct sockaddr);

    if (!buf || !len) {
        BPRINT_E("%s: Incorrect parameters", __func__);
        return -1;
    }

    ret = recvfrom(s, (void *)buf, len, flags, &srcaddr, &srcaddr_len);
    if (ret < 0)
        BPRINT_E("%s: Receive message failed: %s(%d)", __func__, strerror(errno), errno);

    return ret;
}

