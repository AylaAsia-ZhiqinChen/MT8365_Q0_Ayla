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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "boots.h"
#include "boots_osi.h"
#include "boots_mt6xx.h"

/**************************************************************************
 *                       D E F I N I T I O N S                            *
***************************************************************************/
#define LOG_TAG "boots_mt6xx"
 /* BT NVRAM FILE */
#define BT_NVRAM_MANUFACTURE "bt_init_value"

/**************************************************************************
 *                  G L O B A L   V A R I A B L E S                       *
***************************************************************************/

static HCI_CMD_T hciCmd;
static BT_INIT_VAR_T btinit[1];
static INT32  bt_com_port;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S                 *
***************************************************************************/
// Common
static ENUM_BT_STATUS_T GORMcmd_HCC_Set_Local_BD_Addr(VOID);
static ENUM_BT_STATUS_T GORMcmd_HCC_Set_PCM(VOID);
static ENUM_BT_STATUS_T GORMcmd_HCC_Set_Radio(VOID);
static ENUM_BT_STATUS_T GORMcmd_HCC_Set_TX_Power_Offset(VOID);
static ENUM_BT_STATUS_T GORMcmd_HCC_Set_Sleep_Timeout(VOID);
static ENUM_BT_STATUS_T GORMcmd_HCC_RESET(VOID);
static ENUM_BT_STATUS_T GORMcmd_HCC_Coex_Performance_Adjust(VOID);

//===================================================================
// Combo chip
HCI_SEQ_T bt_init_script_6630[] =
{
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_PCM                 }, /*0xFC72*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    {  GORMcmd_HCC_Coex_Performance_Adjust }, /*0xFC22*/
    {  0  },
};

HCI_SEQ_T bt_init_script_8167[] =
{
    {  GORMcmd_HCC_Set_Local_BD_Addr       }, /*0xFC1A*/
    {  GORMcmd_HCC_Set_Radio               }, /*0xFC79*/
    {  GORMcmd_HCC_Set_TX_Power_Offset     }, /*0xFC93*/
    {  GORMcmd_HCC_Set_Sleep_Timeout       }, /*0xFC7A*/
    {  GORMcmd_HCC_RESET                   }, /*0x0C03*/
    {  0  },
};

static ap_nvram_btradio_struct stBtDefault_6630 =
{
    {0x00, 0x00, 0x46, 0x66, 0x30, 0x01},
    {0x60, 0x00}, //not used
#if defined(__MTK_MERGE_INTERFACE_SUPPORT__)
    {0x63, 0x10, 0x00, 0x00},
#elif defined(__MTK_BT_I2S_SUPPORT__)
    {0x03, 0x10, 0x00, 0x02},
#else
    {0x23, 0x10, 0x00, 0x00},
#endif
    {0x06, 0x80, 0x00, 0x06, 0x05, 0x06},       // Align 8516 others product
    {0x03, 0x40, 0x1F, 0x40, 0x1F, 0x00, 0x04},
    {0x80, 0x00}, //not used
    {0xFF, 0xFF, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00}, // not used
    {0x00, 0x00, 0x00, 0x00}, // not used
    ///////////// Reserved /////////////
    {0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

static ap_nvram_btradio_struct stBtDefault_8167 =
{
    {0x00, 0x00, 0x46, 0x81, 0x67, 0x01},
    {0x60, 0x00}, //not used
#if defined(__MTK_MERGE_INTERFACE_SUPPORT__)
    {0x63, 0x10, 0x00, 0x00},
#elif defined(__MTK_BT_I2S_SUPPORT__)
    {0x03, 0x10, 0x00, 0x02},
#else
    {0x23, 0x10, 0x00, 0x00},
#endif
    {0x07, 0x80, 0x00, 0x06, 0x05, 0x07},
    {0x03, 0x40, 0x1F, 0x40, 0x1F, 0x00, 0x04},
    {0x80, 0x00}, //not used
    {0xFF, 0xFF, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00}, // not used
    {0x00, 0x00, 0x00, 0x00}, // not used
    ///////////// Reserved /////////////
    {0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/**************************************************************************
 *                          F U N C T I O N S                                                                                 *
***************************************************************************/
static BOOL is_memzero(unsigned char *buf, int size)
{
    int i;
    for (i = 0; i < size; i++) {
        if (*(buf+i) != 0) return FALSE;
    }
    return TRUE;
}

static void get_bt_structure(unsigned char *pucBTStructure)
{
    int fd = -1;
    int i;
    char bt_structure[128] = {0};
    char buf[3] = {0};
    int size = 0;
    BPRINT_D("Get BT structure");

    fd = open(BT_NVRAM_MANUFACTURE, O_RDONLY);
    if (fd < 0) {
        BPRINT_E("Open BT structure fails");
        return;
    }

    size = read(fd, bt_structure, sizeof(bt_structure));

    if (size < 0 || size != 128) {
        BPRINT_E("Read BT structure fails, size:%d", size);
        close(fd);
        return;
    }

    for (i = 0; i < 128; i += 2) {
        buf[0] = bt_structure[i];
        buf[1] = bt_structure[i + 1];
        pucBTStructure[i >> 1] = (unsigned char)strtoul(buf, NULL, 16);
    }

    close(fd);
    BPRINT_D("Get BT structure PASS");
}

static int write_com_port(int fd, unsigned char *buf, unsigned int len)
{
    int nWritten = 0;
    unsigned int bytesToWrite = len;

    if (fd < 0) {
        BPRINT_E("No available com port");
        return -EIO;
    }

    while (bytesToWrite > 0) {
        nWritten = write(fd, buf, bytesToWrite);
        if (nWritten < 0) {
            if (errno == EINTR || errno == EAGAIN)
                break;
            else
                return -errno; /* errno used for whole chip reset */
        }
        bytesToWrite -= nWritten;
        buf += nWritten;
    }

    return (len - bytesToWrite);
}

static int read_com_port(int fd, unsigned char *buf, unsigned int len)
{
    int nRead = 0;
    unsigned int bytesToRead = len;

    if (fd < 0) {
        BPRINT_E("No available com port");
        return -EIO;
    }

    nRead = read(fd, buf, bytesToRead);
    if (nRead < 0) {
        if(errno == EINTR || errno == EAGAIN)
            return 0;
        else
            return -errno; /* errno used for whole chip reset */
    }

    return nRead;
}

static int bt_send_data(int fd, unsigned char *buf, unsigned int len)
{
    int bytesWritten = 0;
    unsigned int bytesToWrite = len;

    /* Try to send len bytes data in buffer */
    while (bytesToWrite > 0) {
        bytesWritten = write_com_port(fd, buf, bytesToWrite);
        if (bytesWritten < 0) {
            return -1;
        }
        bytesToWrite -= bytesWritten;
        buf += bytesWritten;
    }

    return 0;
}

static int bt_receive_data(int fd, unsigned char *buf, unsigned int len)
{
    int bytesRead = 0;
    unsigned int bytesToRead = len;

    int ret = 0;
    struct timeval tv;
    fd_set readfd;

    tv.tv_sec = 5; /* SECOND */
    tv.tv_usec = 0; /* USECOND */
    FD_ZERO(&readfd);

    /* Try to receive len bytes */
    while (bytesToRead > 0) {

        FD_SET(fd, &readfd);
        ret = select(fd + 1, &readfd, NULL, NULL, &tv);

        if (ret > 0) {
            bytesRead = read_com_port(fd, buf, bytesToRead);
            if (bytesRead < 0) {
                return -1;
            }
            else {
                bytesToRead -= bytesRead;
                buf += bytesRead;
            }
        }
        else if (ret == 0) {
            BPRINT_E("Read com port timeout 5000ms!");
            return -1;
        }
        else if ((ret == -1) && (errno == EINTR)) {
            BPRINT_E("select error EINTR");
        }
        else {
            BPRINT_E("select error %s(%d)!", strerror(errno), errno);
            return -1;
        }
    }

    return 0;
}

static BOOL BT_SendHciCommand(INT32 comPort, HCI_CMD_T *pHciCommand)
{
    UINT8 ucHciCmd[256+4] = {0x01, 0x00, 0x00, 0x00};

    ucHciCmd[1] = (UINT8)pHciCommand->opCode;
    ucHciCmd[2] = (UINT8)(pHciCommand->opCode >> 8);
    ucHciCmd[3] = pHciCommand->len;

    BPRINT_D("OpCode 0x%04x len %d", pHciCommand->opCode, (INT32)pHciCommand->len);

    if (pHciCommand->len) {
        memcpy(&ucHciCmd[4], pHciCommand->parms, pHciCommand->len);
    }

    if (bt_send_data(comPort, ucHciCmd, pHciCommand->len + 4) < 0) {
        BPRINT_E("Write HCI command fails errno %d", errno);
        return FALSE;
    }

    return TRUE;
}

static BOOL BT_ReadPacketHeader(
    INT32   comPort,
    UINT8*  pPacketType, /* Command, Event, ACL data, SCO data */
    UINT16* pRemainLen, /* Remaining len for parameters */
    UINT16* pOpCode, /* Command OpCode */
    UINT16* pConnHandle, /* Connection handle */
    UINT8*  pEventCode /* Event code */
    )
{
    UINT8   ucCmdHdr[3];
    UINT8   ucAclHdr[4];
    UINT8   ucScoHdr[3];
    UINT8   ucEventHdr[2];
    UINT8   type = 0;

    /* Read UART header */
    if (bt_receive_data(comPort, &type, 1) < 0) {
        BPRINT_E("Read packet header fails");
        return FALSE;
    }

    *pPacketType = type;

    switch (type) {
      case 1: /* Command */
        if (bt_receive_data(comPort, ucCmdHdr, 3) < 0) {
            BPRINT_E("Read command header fails %d", errno);
            return FALSE;
        }

        *pOpCode = (((UINT16)ucCmdHdr[0]) | (((UINT16)ucCmdHdr[1]) << 8));
        *pRemainLen = ucCmdHdr[2];
        break;

      case 2: /* ACL data */
        if (bt_receive_data(comPort, ucAclHdr, 4) < 0) {
            BPRINT_E("Read ACL header fails %d", errno);
            return FALSE;
        }

        *pConnHandle = (((UINT16)ucAclHdr[0]) | (((UINT16)ucAclHdr[1]) << 8));
        *pRemainLen = (((UINT16)ucAclHdr[2]) | (((UINT16)ucAclHdr[3]) << 8));
        break;

      case 3: /* SCO data */
        if (bt_receive_data(comPort, ucScoHdr, 3) < 0) {
            BPRINT_E("Read SCO header fails %d", errno);
            return FALSE;
        }

        *pConnHandle = (((UINT16)ucScoHdr[0]) | (((UINT16)ucScoHdr[1]) << 8));
        *pRemainLen = ucScoHdr[2];
        break;

      case 4: /* Event */
        if (bt_receive_data(comPort, ucEventHdr, 2) < 0) {
            BPRINT_E("Read event header fails %d", errno);
            return FALSE;
        }

        *pEventCode = ucEventHdr[0];
        *pRemainLen = ucEventHdr[1];
        break;

      default: /* other */
        BPRINT_E("Unknown packet type %02x", type);
        return FALSE;
        break;
    }

    return TRUE;
}

static BOOL BT_ReadPacket(
    INT32   comPort,
    UINT8*  pPacket,
    UINT32  u4MaxBufSz,
    UINT32* pu4PktLen
    )
{
    UINT8   packetType;
    UINT16  remainLen;
    UINT16  opCode = 0, connHandle = 0;
    UINT8   eventCode = 0;
    UINT32  u4PktLen = 0;

    if (u4MaxBufSz == 0) {
        BPRINT_E("Read buffer size is zero!");
        return FALSE;
    }

    if (BT_ReadPacketHeader(
          comPort,
          &packetType,
          &remainLen,
          &opCode,
          &connHandle,
          &eventCode) == FALSE) {

        BPRINT_E("Read packet header fails");
        return FALSE;
    }

    pPacket[0] = packetType;
    u4PktLen ++;

    /* Command packet */
    if (packetType == 1) {
        if (u4MaxBufSz < (UINT32)(4 + remainLen)) {
            BPRINT_E("Read command buffer is too short!");
            return FALSE;
        }

        pPacket[u4PktLen] = (UINT8)opCode;
        pPacket[u4PktLen + 1] = (UINT8)(opCode >> 8);
        u4PktLen += 2;

        pPacket[u4PktLen] = (UINT8)remainLen;
        u4PktLen ++;

        if (bt_receive_data(comPort, pPacket + u4PktLen, remainLen) < 0) {
            BPRINT_E("Read remain packet fails %d", errno);
            return FALSE;
        }

        u4PktLen += remainLen;
        *pu4PktLen = u4PktLen;

        return TRUE;
    }

    /* ACL data */
    if (packetType == 2) {
        if (u4MaxBufSz < (UINT32)(5 + remainLen)) {
            BPRINT_E("Read ACL buffer is too short!");
            return FALSE;
        }

        pPacket[u4PktLen] = (UINT8)connHandle;
        pPacket[u4PktLen + 1] = (UINT8)(connHandle >> 8);
        u4PktLen += 2;

        pPacket[u4PktLen] = (UINT8)remainLen;
        pPacket[u4PktLen + 1] = (UINT8)(remainLen >> 8);
        u4PktLen += 2;

        if (bt_receive_data(comPort, pPacket + u4PktLen, remainLen) < 0) {
            BPRINT_E("Read remain packet fails %d", errno);
            return FALSE;
        }

        u4PktLen += remainLen;
        *pu4PktLen = u4PktLen;

        return TRUE;
    }

    /* SCO data */
    if (packetType == 3) {
        if (u4MaxBufSz < (UINT32)(4 + remainLen)) {
            BPRINT_E("Read SCO buffer is too short!");
            return FALSE;
        }

        pPacket[u4PktLen] = (UINT8)connHandle;
        pPacket[u4PktLen + 1] = (UINT8)(connHandle >> 8);
        u4PktLen += 2;

        pPacket[u4PktLen] = (UINT8)remainLen;
        u4PktLen ++;

        if (bt_receive_data(comPort, pPacket + u4PktLen, remainLen) < 0) {
            BPRINT_E("Read remain packet fails %d", errno);
            return FALSE;
        }

        u4PktLen += remainLen;
        *pu4PktLen = u4PktLen;

        return TRUE;
    }

    /* Event packet */
    if (packetType == 4) {
        if(u4MaxBufSz < (UINT32)(3 + remainLen)) {
            BPRINT_E("Read event buffer is too short!");
            return FALSE;
        }

        pPacket[u4PktLen] = eventCode;
        pPacket[u4PktLen + 1] = (UINT8)remainLen;
        u4PktLen += 2;

        if (bt_receive_data(comPort, pPacket + u4PktLen, remainLen) < 0) {
            BPRINT_E("Read remain packet fails %d", errno);
            return FALSE;
        }

        u4PktLen += remainLen;
        *pu4PktLen = u4PktLen;

        return TRUE;
    }

    BPRINT_E("Unknown packet type");

    return FALSE;
}

BOOL BT_ReadExpectedEvent(
    INT32   comPort,
    UINT8*  pEventPacket,
    UINT32  u4MaxBufSz,
    UINT32* pu4PktLen,
    UINT8   ucExpectedEventCode,
    BOOL    fCheckCompleteOpCode,/* whether to check command OpCode */
    UINT16  u2ExpectedOpCode,
    BOOL    fCheckCommandStatus, /* whether to check command status */
    UINT8   ucExpectedStatus
    )
{
    UINT16  u2OpCode;
    UINT8   ucEventCode, ucStatus;

    if (BT_ReadPacket(
          comPort,
          pEventPacket,
          u4MaxBufSz,
          pu4PktLen) == FALSE) {

        BPRINT_E("Read packet fails");
        return FALSE;
    }

    /* Expect Event only */
    if (pEventPacket[0] != 4) {
        BPRINT_E("Unexpected packet type");
        return FALSE;
    }

    ucEventCode = pEventPacket[1];

    if (ucEventCode != ucExpectedEventCode) {
        BPRINT_E("Unexpected event code");
        return FALSE;
    }

    if (ucEventCode == 0x0E) {
        if (fCheckCompleteOpCode) {
            u2OpCode = ((UINT16)pEventPacket[4]) | (((UINT16)pEventPacket[5]) << 8);

            if (u2OpCode != u2ExpectedOpCode) {
                BPRINT_E("Unexpected OpCode");
                return FALSE;
            }
        }
        if (fCheckCommandStatus) {
            ucStatus = pEventPacket[6];

            if (ucStatus != ucExpectedStatus) {
                BPRINT_E("Unexpected status %02x", ucStatus);
                return FALSE;
            }
        }
    }

    if (ucEventCode == 0x0F) {
        if (fCheckCompleteOpCode) {
            u2OpCode = ((UINT16)pEventPacket[5]) | (((UINT16)pEventPacket[6]) << 8);

            if (u2OpCode != u2ExpectedOpCode) {
                BPRINT_E("Unexpected OpCode");
                return FALSE;
            }
        }

        if (fCheckCommandStatus) {
            ucStatus = pEventPacket[3];

            if (ucStatus != ucExpectedStatus) {
                BPRINT_E("Unexpected status %02x", ucStatus);
                return FALSE;
            }
        }
    }

    return TRUE;
}

static BOOL WriteBDAddrToNvram(UCHAR *pucBDAddr)
{
    UNUSED(pucBDAddr);
    // TODO
    return TRUE;
}

static BOOL BT_Get_Local_BD_Addr(UCHAR *pucBDAddr)
{
    HCI_CMD_T cmd;
    UINT32 u4ReadLen = 0;
    UINT8 ucAckEvent[20];

    cmd.opCode = 0x1009;
    cmd.len = 0;

    BPRINT_D("BT_Get_Local_BD_Addr");

    if (BT_SendHciCommand(bt_com_port, &cmd) == FALSE) {
        BPRINT_E("Write get BD address command fails");
        return FALSE;
    }

    /* Read local BD address from F/W */
    if (BT_ReadExpectedEvent(
          bt_com_port,
          ucAckEvent,
          sizeof(ucAckEvent),
          &u4ReadLen,
          0x0E,
          TRUE,
          0x1009,
          TRUE,
          0x00) == FALSE) {

        BPRINT_E("Read local BD address fails");
        return FALSE;
    }

    BPRINT_W("Local BD address: %02x-%02x-%02x-%02x-%02x-%02x",
            ucAckEvent[12], ucAckEvent[11], ucAckEvent[10],
            ucAckEvent[9], ucAckEvent[8], ucAckEvent[7]);

    pucBDAddr[0] = ucAckEvent[12];
    pucBDAddr[1] = ucAckEvent[11];
    pucBDAddr[2] = ucAckEvent[10];
    pucBDAddr[3] = ucAckEvent[9];
    pucBDAddr[4] = ucAckEvent[8];
    pucBDAddr[5] = ucAckEvent[7];

    return TRUE;
}

#ifdef BD_ADDR_AUTOGEN
static VOID GetRandomValue(UCHAR string[6])
{
    INT32 iRandom = 0;
    INT32 fd = 0;
    UINT32 seed;

    BPRINT_W("Enable random generation");

    /* Initialize random seed */
    srand(time(NULL));
    iRandom = rand();
    BPRINT_W("iRandom = [%d]", iRandom);
    string[0] = (((iRandom>>24|iRandom>>16) & (0xFE)) | (0x02)); /* Must use private bit(1) and no BCMC bit(0) */

    /* second seed */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
    iRandom = rand();
    BPRINT_W("iRandom = [%d]", iRandom);
    string[1] = ((iRandom>>8) & 0xFF);

    /* third seed */
    fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        if (read(fd, &seed, sizeof(UINT32)) > 0) {
            srand(seed);
            iRandom = rand();
        }
        close(fd);
    }

    BPRINT_W("iRandom = [%d]", iRandom);
    string[5] = (iRandom & 0xFF);

    return;
}
#endif // BD_ADDR_AUTOGEN

static ENUM_BT_STATUS_T GORMcmd_HCC_Set_Local_BD_Addr(VOID)
{
    UCHAR ucDefaultAddr[6] = {0};
    UCHAR ucZeroAddr[6] = {0};

    hciCmd.opCode = 0xFC1A;
    hciCmd.len = 6;

    BPRINT_D("GORMcmd_HCC_Set_Local_BD_Addr");

    switch (btinit->chip_id) {
      case 0x6630:
        memcpy(ucDefaultAddr, stBtDefault_6630.addr, 6);
        break;
      case 0x8167:
        memcpy(ucDefaultAddr, stBtDefault_8167.addr, 6);
        break;
      default:
        BPRINT_E("Unknown combo chip id: %04x", btinit->chip_id);
        break;
    }

    if ((0 == memcmp(btinit->bt_nvram.fields.addr, ucDefaultAddr, 6)) ||
        (0 == memcmp(btinit->bt_nvram.fields.addr, ucZeroAddr, 6))) {
        BPRINT_W("NVRAM BD address default value");
        /* Want to retrieve module eFUSE address on combo chip */
        BT_Get_Local_BD_Addr(btinit->bt_nvram.fields.addr);

        if ((0 == memcmp(btinit->bt_nvram.fields.addr, ucDefaultAddr, 6)) ||
            (0 == memcmp(btinit->bt_nvram.fields.addr, ucZeroAddr, 6))) {
            BPRINT_W("eFUSE address default value");
            #ifdef BD_ADDR_AUTOGEN
            GetRandomValue(btinit->bt_nvram.fields.addr);
            #endif
        }
        else {
            BPRINT_W("eFUSE address has valid value");
        }

        /* Save BD address to NVRAM */
        WriteBDAddrToNvram(btinit->bt_nvram.fields.addr);
    }
    else {
        BPRINT_W("NVRAM BD address has valid value");
    }

    hciCmd.parms[0] = btinit->bt_nvram.fields.addr[5];
    hciCmd.parms[1] = btinit->bt_nvram.fields.addr[4];
    hciCmd.parms[2] = btinit->bt_nvram.fields.addr[3];
    hciCmd.parms[3] = btinit->bt_nvram.fields.addr[2];
    hciCmd.parms[4] = btinit->bt_nvram.fields.addr[1];
    hciCmd.parms[5] = btinit->bt_nvram.fields.addr[0];

    BPRINT_W("Write BD address: %02x-%02x-%02x-%02x-%02x-%02x",
            hciCmd.parms[5], hciCmd.parms[4], hciCmd.parms[3],
            hciCmd.parms[2], hciCmd.parms[1], hciCmd.parms[0]);


    if (BT_SendHciCommand(bt_com_port, &hciCmd) == TRUE) {
        return BT_STATUS_SUCCESS;
    }
    else {
        return BT_STATUS_FAILED;
    }
}

static ENUM_BT_STATUS_T GORMcmd_HCC_Set_PCM(VOID)
{
    hciCmd.opCode = 0xFC72;
    hciCmd.len = 4;

    hciCmd.parms[0] = btinit->bt_nvram.fields.Codec[0];
    hciCmd.parms[1] = btinit->bt_nvram.fields.Codec[1];
    hciCmd.parms[2] = btinit->bt_nvram.fields.Codec[2];
    hciCmd.parms[3] = btinit->bt_nvram.fields.Codec[3];

    BPRINT_D("GORMcmd_HCC_Set_PCM");

    if (BT_SendHciCommand(bt_com_port, &hciCmd) == TRUE) {
        return BT_STATUS_SUCCESS;
    }
    else {
        return BT_STATUS_FAILED;
    }
}

static ENUM_BT_STATUS_T GORMcmd_HCC_Set_Radio(VOID)
{
    hciCmd.opCode = 0xFC79;

    if (btinit->chip_id != 0x6632) {
        hciCmd.len = 6;
        hciCmd.parms[0] = btinit->bt_nvram.fields.Radio[0];
        hciCmd.parms[1] = btinit->bt_nvram.fields.Radio[1];
        hciCmd.parms[2] = btinit->bt_nvram.fields.Radio[2];
        hciCmd.parms[3] = btinit->bt_nvram.fields.Radio[3];
        hciCmd.parms[4] = btinit->bt_nvram.fields.Radio[4];
        hciCmd.parms[5] = btinit->bt_nvram.fields.Radio[5];
    } else {
        hciCmd.len = 8;
        hciCmd.parms[0] = btinit->bt_nvram.fields.Radio[0];
        hciCmd.parms[1] = btinit->bt_nvram.fields.Radio[1];
        hciCmd.parms[2] = btinit->bt_nvram.fields.Radio[2];
        hciCmd.parms[3] = btinit->bt_nvram.fields.Radio[3];
        hciCmd.parms[4] = btinit->bt_nvram.fields.Radio[4];
        hciCmd.parms[5] = btinit->bt_nvram.fields.Radio[5];
        hciCmd.parms[6] = btinit->bt_nvram.fields.Radio_ext[0];
        hciCmd.parms[7] = btinit->bt_nvram.fields.Radio_ext[1];
    }

    BPRINT_D("GORMcmd_HCC_Set_Radio");

    if (BT_SendHciCommand(bt_com_port, &hciCmd) == TRUE) {
        return BT_STATUS_SUCCESS;
    }
    else {
        return BT_STATUS_FAILED;
    }
}

static ENUM_BT_STATUS_T GORMcmd_HCC_Set_TX_Power_Offset(VOID)
{
    hciCmd.opCode = 0xFC93;

    if (btinit->chip_id != 0x6632) {
        hciCmd.len = 3;
        hciCmd.parms[0] = btinit->bt_nvram.fields.TxPWOffset[0];
        hciCmd.parms[1] = btinit->bt_nvram.fields.TxPWOffset[1];
        hciCmd.parms[2] = btinit->bt_nvram.fields.TxPWOffset[2];
    } else {
        hciCmd.len = 6;
        hciCmd.parms[0] = btinit->bt_nvram.fields.TxPWOffset[0];
        hciCmd.parms[1] = btinit->bt_nvram.fields.TxPWOffset[1];
        hciCmd.parms[2] = btinit->bt_nvram.fields.TxPWOffset[2];
        hciCmd.parms[3] = btinit->bt_nvram.fields.TxPWOffset_ext[0];
        hciCmd.parms[4] = btinit->bt_nvram.fields.TxPWOffset_ext[1];
        hciCmd.parms[5] = btinit->bt_nvram.fields.TxPWOffset_ext[2];
    }

    BPRINT_D("GORMcmd_HCC_Set_TX_Power_Offset");

    if (BT_SendHciCommand(bt_com_port, &hciCmd) == TRUE) {
        return BT_STATUS_SUCCESS;
    }
    else {
        return BT_STATUS_FAILED;
    }
}

static ENUM_BT_STATUS_T GORMcmd_HCC_Set_Sleep_Timeout(VOID)
{
    hciCmd.opCode = 0xFC7A;
    hciCmd.len = 7;

    hciCmd.parms[0] = btinit->bt_nvram.fields.Sleep[0];
    hciCmd.parms[1] = btinit->bt_nvram.fields.Sleep[1];
    hciCmd.parms[2] = btinit->bt_nvram.fields.Sleep[2];
    hciCmd.parms[3] = btinit->bt_nvram.fields.Sleep[3];
    hciCmd.parms[4] = btinit->bt_nvram.fields.Sleep[4];
    hciCmd.parms[5] = btinit->bt_nvram.fields.Sleep[5];
    hciCmd.parms[6] = btinit->bt_nvram.fields.Sleep[6];

    BPRINT_D("GORMcmd_HCC_Set_Sleep_Timeout");

    if (BT_SendHciCommand(bt_com_port, &hciCmd) == TRUE) {
        return BT_STATUS_SUCCESS;
    }
    else {
        return BT_STATUS_FAILED;
    }
}

static ENUM_BT_STATUS_T GORMcmd_HCC_RESET(VOID)
{
    hciCmd.opCode = 0x0C03;
    hciCmd.len = 0;

    BPRINT_D("GORMcmd_HCC_RESET");

    if (BT_SendHciCommand(bt_com_port, &hciCmd) == TRUE) {
        return BT_STATUS_SUCCESS;
    }
    else {
        return BT_STATUS_FAILED;
    }
}

static ENUM_BT_STATUS_T GORMcmd_HCC_Coex_Performance_Adjust(VOID)
{
    hciCmd.opCode = 0xFC22;
    hciCmd.len = 6;

    hciCmd.parms[0] = btinit->bt_nvram.fields.CoexAdjust[0];
    hciCmd.parms[1] = btinit->bt_nvram.fields.CoexAdjust[1];
    hciCmd.parms[2] = btinit->bt_nvram.fields.CoexAdjust[2];
    hciCmd.parms[3] = btinit->bt_nvram.fields.CoexAdjust[3];
    hciCmd.parms[4] = btinit->bt_nvram.fields.CoexAdjust[4];
    hciCmd.parms[5] = btinit->bt_nvram.fields.CoexAdjust[5];

    BPRINT_D("GORMcmd_HCC_Coex_Performance_Adjust");

    if (BT_SendHciCommand(bt_com_port, &hciCmd) == TRUE) {
        return BT_STATUS_SUCCESS;
    }
    else{
        return BT_STATUS_FAILED;
    }
}

static ENUM_BT_STATUS_T GORM_Init(
    INT32   comPort,
    UINT32  chipId,
    PUCHAR  pucPatchExtData,
    UINT32  u4PatchExtLen,
    PUCHAR  pucPatchData,
    UINT32  u4PatchLen,
    PUCHAR  pucNvRamData,
    UINT32  u4Baud,
    UINT32  u4HostBaud,
    UINT32  u4FlowControl
    )
{
    INT32   i = 0;
    ENUM_BT_STATUS_T status;
    UINT8   ucEventBuf[MAX_EVENT_SIZE];
    UINT32  u4EventLen;
    UNUSED(pucPatchExtData);
    UNUSED(u4PatchExtLen);
    UNUSED(pucPatchData);
    UNUSED(u4PatchLen);
    UNUSED(u4Baud);
    UNUSED(u4HostBaud);
    UNUSED(u4FlowControl);

    BPRINT_D("GORM_Init");

    /* Save com port fd for GORMcmds */
    bt_com_port = comPort;
    /* Save chip id */
    btinit->chip_id = chipId;
    /* Copy NVRAM data */
    memcpy(btinit->bt_nvram.raw, pucNvRamData, sizeof(ap_nvram_btradio_struct));

    /* General init script */
    switch (btinit->chip_id) {
      case 0x6630:
        btinit->cur_script = bt_init_script_6630;
        break;
      case 0x8167:
        btinit->cur_script = bt_init_script_8167;
        break;
      default:
        BPRINT_E("Unknown combo chip id: %04x", btinit->chip_id);
        break;
    }

    /* Can not find matching script, simply skip */
    if ((btinit->cur_script) == NULL) {
        BPRINT_E("No matching init script");
        return BT_STATUS_FAILED;
    }

    i = 0;

    while (btinit->cur_script[i].command_func)
    {
        status = btinit->cur_script[i].command_func();
        if (status == BT_STATUS_CANCELLED) {
            i ++;
            continue; /*skip*/
        }

        if (status == BT_STATUS_FAILED) {
            BPRINT_E("Command %d fails", i);
            return status;
        }

        if (BT_ReadExpectedEvent(
              comPort,
              ucEventBuf,
              MAX_EVENT_SIZE,
              &u4EventLen,
              0x0E,
              TRUE,
              hciCmd.opCode,
              TRUE,
              0x00) == FALSE) {

            BPRINT_E("Read event of command %d fails", i);
            return BT_STATUS_FAILED;
        }

        i ++;
    }

    return BT_STATUS_SUCCESS;
}

uint32_t mtk_66xx_detect(void)
{
    uint32_t  chipId = 0;
#ifdef MTK_ANDROID_PLATFORM
    char     chipId_val[64];
    int      chipId_ready_retry = 0;

    do {
        if (osi_property_get(WCN_COMBO_LOADER_CHIP_ID_PROP, chipId_val, NULL) &&
                strcmp(chipId_val, "-1") != 0) {
            chipId = (uint32_t)strtoul(chipId_val, NULL, 16);
            break;
        } else {
            chipId_ready_retry++;
            usleep(500000);
        }
    } while (chipId_ready_retry < 10);

#elif defined(MTK_CHIP_COMBO)
    chipId = 0x6630;
#elif defined(MTK_CHIP_CONSYS)
    chipId = 0x8167;
#else
    // Pure Linux system detect chip id
    char *chipId_val = getenv(WCN_COMBO_LOADER_CHIP_ID_PROP);
    if (chipId_val)
        chipId = (uint32_t)strtoul(chipId_val, NULL, 16);
    else
        BPRINT_D("Can't find \"%s\" environment variable for chip ID",
                WCN_COMBO_LOADER_CHIP_ID_PROP);
#endif

    BPRINT_D("chip id %x", chipId);
    return chipId;
}

int mtk_66xx_Init(int comPort, unsigned int chipId)
{
    UNUSED(boots_btif);
    ENUM_BT_STATUS_T status;
    unsigned char ucNvRamData[sizeof(ap_nvram_btradio_struct)] = {0};

    get_bt_structure(ucNvRamData);

    if (is_memzero(ucNvRamData, sizeof(ap_nvram_btradio_struct))) {
        BPRINT_E("Read NVRAM data fails or NVRAM data all zero!!");
        BPRINT_W("Use %x default value", chipId);
        switch (chipId) {
        case 0x6630:
            /* Use MT6630 default value */
            memcpy(ucNvRamData, &stBtDefault_6630, sizeof(ap_nvram_btradio_struct));
            break;
        case 0x8167:
            /* Use MT8167 default value */
            memcpy(ucNvRamData, &stBtDefault_8167, sizeof(ap_nvram_btradio_struct));
            break;
        default:
            BPRINT_E("Unknown combo chip id: %04x\n", chipId);
            return -1;
        }
    }

    BPRINT_I("[BDAddr:%02x-%02x-%02x-%02x-%02x-%02x]",
            ucNvRamData[0], ucNvRamData[1], ucNvRamData[2],
            ucNvRamData[3], ucNvRamData[4], ucNvRamData[5]);
    BPRINT_I("[Voice :%02x %02x]", ucNvRamData[6], ucNvRamData[7]);
    BPRINT_I("[Codec :%02x %02x %02x %02x]",
            ucNvRamData[8], ucNvRamData[9], ucNvRamData[10], ucNvRamData[11]);
    BPRINT_I("[Radio :%02x %02x %02x %02x %02x %02x]",
            ucNvRamData[12], ucNvRamData[13], ucNvRamData[14],
            ucNvRamData[15], ucNvRamData[16], ucNvRamData[17]);
    BPRINT_I("[Sleep :%02x %02x %02x %02x %02x %02x %02x]",
            ucNvRamData[18], ucNvRamData[19], ucNvRamData[20], ucNvRamData[21],
                ucNvRamData[22], ucNvRamData[23], ucNvRamData[24]);
    BPRINT_I("[BtFTR: %02x %02x]", ucNvRamData[25], ucNvRamData[26]);
    BPRINT_I("[TxPWOffset:%02x %02x %02x]", ucNvRamData[27], ucNvRamData[28], ucNvRamData[29]);
    BPRINT_I("[CoexAdjust:%02x %02x %02x %02x %02x %02x]",
            ucNvRamData[30], ucNvRamData[31], ucNvRamData[32],
            ucNvRamData[33], ucNvRamData[34], ucNvRamData[35]);
    BPRINT_I("[Radio_ext :%02x %02x]", ucNvRamData[36], ucNvRamData[37]);
    BPRINT_I("[TxPWOffset_ext:%02x %02x %02x]", ucNvRamData[38], ucNvRamData[39], ucNvRamData[40]);

    status = GORM_Init(comPort, chipId, NULL, 0, NULL, 0, ucNvRamData, 0, 0, 0);

    return (int)status;
}

//---------------------------------------------------------------------------
