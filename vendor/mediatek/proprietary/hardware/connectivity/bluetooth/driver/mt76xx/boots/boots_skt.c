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

//- vim: set ts=4 sts=4 sw=4 et: --------------------------------------------
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include "boots.h"
#include "boots_skt.h"

//---------------------------------------------------------------------------
#define LOG_TAG "boots_skt"

//---------------------------------------------------------------------------
int boots_sk_create(int * sk, struct sockaddr_un * sockaddr, socklen_t * socklen,
        char * name, int client)
{
    UNUSED(boots_btif);
    int retry_counter = 10;

    // Create server socket
    *sk = socket(client ? PF_LOCAL : AF_UNIX, SOCK_DGRAM, 0);
    if (*sk < 0) {
        BPRINT_E("Create socket failed: %s %s(%d)", name, strerror(errno), errno);
        return errno;
    }

    // Init server socket
    sockaddr->sun_family = AF_UNIX;
    memset(sockaddr->sun_path, 0, sizeof(sockaddr->sun_path));
    strncpy(sockaddr->sun_path, name, strlen(name));
    *socklen = (offsetof(struct sockaddr_un, sun_path) + strlen(sockaddr->sun_path) + 1);

    if (!client) {
        // Remove the socket first
        unlink(sockaddr->sun_path);
        if (bind(*sk, (struct sockaddr *)sockaddr, *socklen) < 0) {
            BPRINT_E("Bind socket failed: %s(%d)", strerror(errno), errno);
            return errno;
        }

    } else {
        int memsize = HCI_BUF_SIZE;
        if (setsockopt(*sk, SOL_SOCKET, SO_RCVBUF, &memsize, sizeof(memsize)) < 0)
            BPRINT_E("Set socket buffer size failed: %s(%d)", strerror(errno), errno);

        while (retry_counter > 0) {
            retry_counter--;
            if (connect(*sk, (struct sockaddr *)sockaddr, *socklen) < 0) {
                if (retry_counter > 0) {
                    BPRINT_W("Failed to connect to socket: %s(%d). Retry...", strerror(errno), errno);
                    sleep(1);
                    continue;
                } else {
                    BPRINT_E("Failed to connect to socket: %s(%d). Stop Retry!", strerror(errno), errno);
                    return errno;
                }
            } else {
                break;
            }
        }
    }

    BPRINT_D("Socket %s create success", name);
    return 0;
}

//---------------------------------------------------------------------------
int boots_sk_close(int * sk)
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
ssize_t boots_sk_send(int s, const void * buf, size_t len, struct sockaddr_un * to,
        char * name)
{
    ssize_t ret = 0;

    if (!buf || !len) {
        BPRINT_E("%s: Incorrect argument", __func__);
        return -1;
    }
    if (s < 0) {
        BPRINT_E("%s: Incorrect FD(%d)", __func__, s);
        return -1;
    }
#if 0
    ret = sendto(s, buf, len, 0, (const struct sockaddr *)to,
            sizeof(struct sockaddr_un));
#else
    UNUSED(to);
    UNUSED(name);
    ret = send(s, buf, len, 0);
#endif
    if (ret < 0)
        BPRINT_E("%s: Send message failed: %s(%d)", __func__, strerror(errno), errno);

    return ret;
}

//---------------------------------------------------------------------------
ssize_t boots_sk_recv(int s, const void * buf, size_t len, int flags,
        struct sockaddr * src_addr, socklen_t * addrlen)
{
    ssize_t ret = 0;

    if (!buf || !len) {
        BPRINT_E("%s: Incorrect argument", __func__);
        return -1;
    }
    if (s < 0) {
        BPRINT_E("%s: Incorrect FD(%d)", __func__, s);
        return -1;
    }

    ret = recvfrom(s, (void *)buf, len, flags, src_addr, addrlen);
    if (ret < 0)
        BPRINT_E("%s: Receive message failed: %s(%d)", __func__, strerror(errno), errno);

    return ret;
}

//---------------------------------------------------------------------------
