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
#ifndef __BOOTS_PKT_H__
#define __BOOTS_PKT_H__

#include "boots.h"

/** Bluetooth Packet Header Length */
#define HCI_CMD_PKT_HDR_LEN 4
#define HCI_EVENT_PKT_HDR_LEN 3
#define HCI_ACL_PKT_HDR_LEN 5

/** Bluetooth Packet Type */
#define HCI_CMD_PKT     0x01
#define HCI_ACL_PKT     0x02
#define HCI_SCO_PKT     0x03
#define HCI_EVENT_PKT   0x04

/** Script Action Type */
#define SCRIPT_NONE     0x00
#define SCRIPT_TITLE    0x01
#define SCRIPT_PROC     0x02
#define SCRIPT_TX       0x03
#define SCRIPT_RX       0x04
#define SCRIPT_WAITRX   0x05
#define SCRIPT_LOOP     0x06
#define SCRIPT_LOOPEND  0x07
#define SCRIPT_TIMEOUT  0x08
#define SCRIPT_WAIT     0x09
#define SCRIPT_USBALT   0x0A
#define SCRIPT_CMD      0x0B
#define SCRIPT_STRESS   0x0C
#define SCRIPT_LOOPBACK 0x0D
#define SCRIPT_LPTIMER  0x0E
#define SCRIPT_RSSI     0x0F
#define SCRIPT_END      0x10    // script end

/** Script setting */
typedef struct script_set {
    FILE *script;           /** script FD */
    int loop;               /** loop counter */
    long loop_pos;          /** record loop header */
    int timo;               /** timeout */
    int wait;               /** sleep */
} script_set_s;

/** Background RSSI setting */
typedef struct rssi_set {
    int ch_start;           /** start channel */
    int ch_end;             /** end channel */
    int avg_package;        /** number of average package */
    int alpha_filter;       /** alpha filter */
    uint8_t report;         /** olny create RSSI report in stop command */
    uint8_t stop;           /** stop rssi reporting */
    uint8_t one_shot;       /** only report once */
} rssi_set_s;

/** Store packet list */
typedef struct pkt_list {
    uint8_t     s_type;     /** script file type, for SCRIPT_TX/RX/WAITRX please fill packet type */
    uint8_t     p_type;     /** packet type */
    union {
        uint8_t    *data;      /** for command, ACL, SCO & event content */
        char       *msg;       /** for title & proc would show message */
        int        loop;       /** for loop tims */
        int        timo;       /** for timeout */
        int        wait;       /** for wait */
        int        usbalt;     /** for USB alternate */
        rssi_set_s *rssi_s;    /** for rssi setting */
    } u_cnt;
    size_t      len;
    struct pkt_list    *next;
} pkt_list_s;

/** Modular commands */
typedef struct {
    char        *cmd;                                   // Shows command
    pkt_list_s  *(*func)(char *cmd[], size_t len);      // Implementeded functions
    char        *comment;                               // Shows comment
    char        *details;                               // Command details
    bool        hidden;                                 // hidden command
} boots_cmds_s;

//---------------------------------------------------------------------------

void boots_pkt_cleanup_report_rssi(uint8_t stop);
pkt_list_s *boots_pkt_node_push(uint8_t s_type, uint8_t p_type, void *content,
        size_t len, pkt_list_s *front, pkt_list_s *head);
pkt_list_s *boots_pkt_node_pop(pkt_list_s *list, uint8_t *buf, size_t *len);
void boots_pkt_list_destroy(pkt_list_s *list);
size_t boots_pkt_list_amount(pkt_list_s *list);

size_t boots_pkt_handler(uint8_t *buf, size_t len, pkt_list_s **pList);
pkt_list_s *boots_raw_cmd_handler(char *raw[], size_t len);
pkt_list_s *boots_cmd_set_handler(char *raw[], size_t len);

FILE *boots_script_open(char *file);
void boots_script_close(FILE *fd);
void boots_script_loop(FILE * fd, long size);
pkt_list_s *boots_script_get(FILE *fd);

ssize_t boots_file_wr(uint8_t *buf, ssize_t *len);

//---------------------------------------------------------------------------
#endif // __BOOTS_PKT_H__
