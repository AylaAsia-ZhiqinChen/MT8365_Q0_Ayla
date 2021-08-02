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

#ifndef ATCID_SERIAL_H
#define ATCID_SERIAL_H

#include "atcid.h"
#include "atcid_util.h"
#include "property/mtk_properties.h"

#define MAX_AT_RESPONSE 2048
#define MAX_DATA_SIZE 6144
#define MAX_QUEUE_MMI_CMD_IDX 10

#define INVALIDE_SOCKET_FD -1

#define SOCKET_NAME_RILD    "rild-atci"
#define SOCKET_NAME_RILD2   "rild-atci-md2"
#define SOCKET_NAME_RILD3   "rild-atci-c2k"
#define SOCKET_NAME_RILPROXY    "rilproxy-atci"
#define SOCKET_NAME_AUDIO   "atci-audio"
#define SOCKET_NAME_GENERIC    "atci-service"
#define SOCKET_NAME_META "meta-atci"

#define TTY_GS0         "/dev/ttyGS0"
#define TTY_GS1         "/dev/ttyGS1"
#define TTY_GS4         "/dev/ttyGS4"

#define ATCI_IN_USERMODE_PROP "persist.vendor.service.atci.usermode"
#define ATCI_IN_PCT_PROP "persist.vendor.service.atci.pct.mode"
#define ATCI_IN_OPERATOR "persist.vendor.operator.optr"

#ifdef MTK_TC1_FEATURE
#define MAX_DEVICE_VCOM_NUM 2
#else
#define MAX_DEVICE_VCOM_NUM 1
#endif
#define MAX_ADB_SKT_NUM 2
#define MAX_DEVICE_NUM  MAX_DEVICE_VCOM_NUM + MAX_ADB_SKT_NUM + 1
#define ADB_SKT_SERVER_NUM MAX_DEVICE_VCOM_NUM
#define ADB_SKT_CLIENT_NUM ADB_SKT_SERVER_NUM + 1
#define HIDL_CONN_NUM ADB_SKT_CLIENT_NUM + 1
#define BACK_LOG 2
#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))

typedef enum {
    UNKNOWN_TYPE = 0,
    RIL_TYPE = 1,
    AUDIO_TYPE = 2,
    PLATFORM_TYPE = 3,
    ATCI_TYPE = 4,
    GENERIC_TYPE = 5,
    BATTERY_TYPE = 6,
    AUDIO_EXTERNAL_TYPE =7,
    META_TYPE = 8,
} ATCI_DataType;

typedef enum{
    FIND_A,
    FIND_T,
    FIND_END,
    FIND_DONE
}AtStates;

typedef enum{
    SOCKET_UNKNOWN,
    SOCKET_MD1,
    SOCKET_MD2,
    SOCKET_MD3
}SocketId;


typedef struct Serial
{
    char devicename[MAX_DEVICE_NUM][64];
    int fd[MAX_DEVICE_NUM];
    int echo[MAX_DEVICE_NUM];
    MuxerStates state;
    char ATBuffer[MAX_DATA_SIZE+1];
    int totalSize;
    AtStates  at_state;
    int currDevice;
} Serial;

static const char *s_MsgResponses[] = {
   "OK",
   "ERROR",
   "+CMS ERROR:",
   "+CMGW:",
   "+CMGS:",
   "+CMGC:",
   "+CNMA:",
   "\r\nOK",
   "\r\nERROR",
   "\r\n+CMS ERROR:",
   "\r\n+CMGW:",
   "\r\n+CMGS:",
   "\r\n+CMGC:",
   "\r\n+CNMA:",
};

typedef struct numericResponse
{
    char Response[20];
    char Number[1];
} numericResponse_t;

//ITU-T Recommendation V.250
static numericResponse_t numericResponse_table[] = {
    {"OK", "0"},
    {"CONNECT", "1"},
    {"RING", "2"},
    {"NO CARRIER", "3"},
    {"ERROR", "4"},
    {"NO DIALTONE", "6"},
    {"BUSY", "7"},
    {"NO ANSWER", "8"},
};

void *readerLoop(void *arg);
void rildReaderLoop();
void audioReaderLoop();
void serviceReaderLoop();
void initSerialDevice(Serial *serial);
int open_serial_device(Serial * serial, char* devicename);
void writeDataToSerial(char* input, int length);
void writeDataToserialByResponseType(ATRESPONSE_t response);
void setEchoOption(int flag);
int isDTSupport();
void setSocketConnect(SocketId socketId);
SocketId getSocketConnect();
void connectTarget(ATCI_DataType dataType);
int sendDataToGenericService(char* line);
int serviceReaderLoopWithResult(char* line);
int adb_socket_listen(int socketFd);
int adb_socket_accept(int socketFd);
void Response2Numeric(char *input, char *output);
int isMsgResponse(const char *line);
int isMsgCmd(char* line);
int sendDataToRildSync(char* line, char* response);
#endif
