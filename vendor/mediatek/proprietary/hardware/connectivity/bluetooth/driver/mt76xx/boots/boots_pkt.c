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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "boots_pkt.h"
#include "boots_stress.h"

//---------------------------------------------------------------------------
#define LOG_TAG "boots_pkt"

#define HCE_INQUIRY_COMPLETE            0x01
#define HCE_INQUIRY_RESULT              0x02
#define HCE_CONNECTION_COMPLETE         0x03
#define HCE_COMMAND_COMPLETE            0x0E
#define HCE_INQUIRY_RESULT_WITH_RSSI    0x22
#define HCE_EXT_INQUIRY_RESULT          0x2F

#define DEFAULT_WIFI_CFG "/vendor/firmware/wifi.cfg"
#define E2P_MODE         "EfuseBufferModeCal"

//---------------------------------------------------------------------------
typedef struct {
    uint8_t pt;     // Packet Type
    uint16_t len;   // Data Length
} packet_info_s;

typedef enum {
    WR_FLAG_NO,
    WR_FLAG_READ,
    WR_FLAG_WRITE
} wr_flag_e;

typedef struct {
    uint32_t addr;
    uint32_t value;
    wr_flag_e wr_flag;
} efuse_reg_wr_s;

static const packet_info_s tx_pi[] = {  // TX packet info
    {0x00, 0x0000}, {0x01, 0x0000}, {0x02, 0x0000}, {0x03, 0x0011},
    {0x04, 0x001B}, {0x05, 0x000A}, {0x06, 0x0014}, {0x07, 0x001E},
    {0x08, 0x0013}, {0x09, 0x000A}, {0x0A, 0x0079}, {0x0B, 0x00B7},
    {0x0E, 0x00E0}, {0x0F, 0x0153}, {0x17, 0x001E}, {0x1C, 0x0078},
    {0x1D, 0x00B4}, {0x24, 0x0036}, {0x28, 0x0053}, {0x2A, 0x016F},
    {0x2B, 0x0228}, {0x2E, 0x02A7}, {0x2F, 0x03FD}, {0x36, 0x003C},
    {0x37, 0x005A}, {0x3C, 0x0168}, {0x3D, 0x021C}};

static const packet_info_s rx_pi[] = {  // RX packet info
    {0x03, 0x0011}, {0x04, 0x001B}, {0x0A, 0x0079}, {0x0B, 0x00B7},
    {0x0E, 0x00E0}, {0x0F, 0x0153}, {0x24, 0x0036}, {0x28, 0x0053},
    {0x2A, 0x016F}, {0x2B, 0x0053}, {0x2E, 0x02A7}, {0x2F, 0x03FD}};

static efuse_reg_wr_s efuse_reg_wr = {0, 0, WR_FLAG_NO};
static int rssi_channels[79];
static int rssi_count = 0;

extern int boots_loop_timer;
extern rssi_set_s rssi_setting;

//---------------------------------------------------------------------------
static void boots_write_stpbtfwlog(char *buf);

//---------------------------------------------------------------------------
void boots_pkt_cleanup_report_rssi(uint8_t stop)
{
    int i = 0;

    if ((rssi_setting.report == true) && (rssi_count != 0) && (stop == 1)) {
        BPRINT_I_RAW("\nPackage count: %d, RSSI: \n", rssi_count);
        for (i = 0; i < 79 ; i++) {
            rssi_channels[i] = rssi_channels[i] / rssi_count;
            if (i >= rssi_setting.ch_start && i <= rssi_setting.ch_end) {
                BPRINT_I_RAW("% 4d ", rssi_channels[i]);
            } else {
                BPRINT_I_RAW("        ");
            }
            if ((i + 1) % 10 == 0 || i == 78) {
                BPRINT_I_RAW("\n");
            }
        }
    }
    rssi_count = 0;
    memset(rssi_channels, 0x00, sizeof(rssi_channels));
    memset(&rssi_setting, 0x00, sizeof(rssi_set_s));
}

//---------------------------------------------------------------------------
pkt_list_s *boots_pkt_node_push(uint8_t s_type, uint8_t p_type, void *content,
        size_t len, pkt_list_s *front, pkt_list_s *head)
{
    // Create node
    pkt_list_s *node = (pkt_list_s *)malloc(sizeof(pkt_list_s));

    if (!node) {
        BPRINT_D("%s: List allocate fail", __func__);
        return NULL;
    }

    node->s_type = s_type;
    switch (s_type) {
    case SCRIPT_NONE:
    case SCRIPT_TX:
    case SCRIPT_RX:
    case SCRIPT_WAITRX:
    case SCRIPT_CMD:
    case SCRIPT_STRESS:
    case SCRIPT_LOOPBACK:
    case SCRIPT_LPTIMER:
        node->p_type = p_type;
        if (len > 0)
            node->u_cnt.data = (uint8_t *)malloc(len);
        if (content && node->u_cnt.data) {
            memcpy(node->u_cnt.data, (uint8_t *)content, len);
            node->len = len;
        }
        break;
    case SCRIPT_TITLE:
    case SCRIPT_PROC:
        if (len > 0)
            node->u_cnt.msg = (char *)malloc(len);
        if (content && node->u_cnt.msg) {
            memcpy(node->u_cnt.msg, (char *)content, len);
            node->len = len;
        }
        break;
    case SCRIPT_RSSI:
        if (len > 0)
            node->u_cnt.rssi_s = (rssi_set_s *)malloc(len);
        if (content && node->u_cnt.rssi_s) {
            memcpy(node->u_cnt.rssi_s, content, len);
            node->len = len;
        }
        break;
    case SCRIPT_LOOP:
        if (content)
            node->u_cnt.loop = *((int *)content);
        break;
    case SCRIPT_TIMEOUT:
        if (content)
            node->u_cnt.timo = *((int *)content);
        break;
    case SCRIPT_WAIT:
        if (content)
            node->u_cnt.wait = *((int *)content);
        break;
    case SCRIPT_USBALT:
        if (content)
            node->u_cnt.usbalt = *((int *)content);
        break;
    case SCRIPT_LOOPEND:
    case SCRIPT_END:
        break;
    default:
        BPRINT_W("Incorrect Script Type: %d", s_type);
        break;
    }
    if (front)
        front->next = node;
    node->next = head ? head : node;

    // return node point
    return node;
}

//---------------------------------------------------------------------------
pkt_list_s *boots_pkt_node_pop(pkt_list_s *list, uint8_t *buf, size_t *len)
{
    pkt_list_s *tmp = NULL;

    if (!list) return NULL;
    //BPRINT_D("%s node: %p, next: %p", __func__, list, list->next);

    if (buf && len) {
        memcpy(buf, list->u_cnt.data, list->len);
        *len = list->len;
    }

    // Remove this node from head
    if (list->next == list) {
        tmp = NULL;
    } else {
        pkt_list_s *tail = NULL;

        tmp = list->next;
        // round circle
        tail = tmp;
        while (tail->next != list) {
            tail = tail->next;
        }
        tail->next = tmp;
    }

    switch (list->s_type) {
    case SCRIPT_NONE:
    case SCRIPT_TX:
    case SCRIPT_RX:
    case SCRIPT_WAITRX:
    case SCRIPT_CMD:
        free(list->u_cnt.data);
        break;
    case SCRIPT_TITLE:
    case SCRIPT_PROC:
        free(list->u_cnt.msg);
        break;
    case SCRIPT_STRESS:
    case SCRIPT_LOOPBACK:
    case SCRIPT_LPTIMER:
        // return list directly, result in infinite loop.
        return list;
    case SCRIPT_RSSI:
        free(list->u_cnt.rssi_s);
        break;
    default:
        break;
    }
    free(list);

    return tmp;
}

//---------------------------------------------------------------------------
void boots_pkt_list_destroy(pkt_list_s *list)
{
    pkt_list_s *cur = list;
    pkt_list_s *next = NULL;

    if (!list) return;

    do {
        if (next) cur = next;
        next = cur->next;
        //BPRINT_D("%s cur: %p, next: %p", __func__, cur, next);
        switch (cur->s_type) {
        case SCRIPT_NONE:
        case SCRIPT_TX:
        case SCRIPT_RX:
        case SCRIPT_WAITRX:
        case SCRIPT_CMD:
            free(cur->u_cnt.data);
            break;
        case SCRIPT_TITLE:
        case SCRIPT_PROC:
            free(cur->u_cnt.msg);
            break;
        default:
            break;
        }
        free(cur);
    } while (next != list && next != NULL);
}

//---------------------------------------------------------------------------
size_t boots_pkt_list_amount(pkt_list_s *list)
{
    size_t amount = 1;
    pkt_list_s *tmp = list;

    if (!list) return 0;

    while (tmp->next != list) {
        amount++;
        tmp = tmp->next;
    }
    return amount;
}

//---------------------------------------------------------------------------
static pkt_list_s *boots_efuse_write(pkt_list_s *list, uint8_t *buf)
{
    pkt_list_s * pkt = NULL;
    uint8_t efuse[] = {0x01, 0x6F, 0xFC, 0x18,
                               0x01, 0x0D, 0x14, 0x00, 0x01, 0x01,
                               0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00};
    size_t len = 0;
    uint16_t addr = 0;

    boots_pkt_node_pop(list, efuse, &len);
    if (len != sizeof(efuse)) {
        BPRINT_E("Error! %d != %d", (int)len, (int)sizeof(efuse));
    }

    addr = efuse_reg_wr.addr / 16 * 4;
    efuse[10] = addr & 0xFF;
    efuse[11] = (addr & 0xFF00) >> 8;
    memcpy(&efuse[12], &buf[2], 4);
    memcpy(&efuse[16], &buf[8], 4);
    memcpy(&efuse[20], &buf[14], 4);
    memcpy(&efuse[24], &buf[20], 4);

    memcpy(&efuse[12 + efuse_reg_wr.addr % 16], &efuse_reg_wr.value, 1);

    pkt = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, efuse, sizeof(efuse),
            NULL, NULL);

    return pkt;
}

//---------------------------------------------------------------------------
ssize_t boots_file_wr(uint8_t *buf, ssize_t *len)
{
    int fd = 0;
    uint32_t addr = 0;
    uint8_t value = 0;
    uint8_t wr = 0;
    size_t ret = 0; // For compiling error

    /****************************************************
    format:
    CMD: 01 01 FF LEN WR ADDR VALUE FILE_PATH
    EVENT: 04 0E LEN 01 01 FF ERRNO WR ADDR VALUE

    LEN: 1 byte
    WR: 1 byte
    ADDR: 4 bytes
    VALUE: 1 byte
    ERRNO: 2 bytes
    FILE_PATH:
    ****************************************************/

    errno = 0;

    fd = open((char *)&buf[10], O_RDWR);
    if (fd < 0) {
        printf("open file %s failed, errno = %s(%d)\n", &buf[10], strerror(errno), errno);
    } else {
        memcpy(&addr, &buf[5], sizeof(addr));
        wr = buf[4];

        (void)lseek(fd, addr, SEEK_SET);

        if (wr == WR_FLAG_WRITE) {
            value = buf[9];
            ret = write(fd, &value, 1);
        } else {
            ret = read(fd, &value, 1);
        }

        close(fd);
    }

    buf[0] = 0x04;
    buf[1] = 0x0E;
    buf[2] = 0x0B;
    buf[3] = 0x01;
    buf[4] = 0x01;
    buf[5] = 0xFF;
    buf[6] = errno & 0xFF;
    buf[7] = (errno & 0xFF00) >> 8;
    buf[8] = wr;
    memcpy(&buf[9], &addr, sizeof(addr));
    buf[13] = value;
    *len = 14;
    return ret;
}

//--------------------------------------------------------------------------
void boots_pkt_parsing(uint8_t *buf, size_t len)
{
    int i = 0;
    float alpha_filter = 0;

    if (!buf || !len) return;

    if (buf[0] == HCI_EVENT_PKT) {
        if (buf[1] == HCE_COMMAND_COMPLETE && len >= 7) {
            switch (STREAM_TO_UINT16(buf + 4)) {
            case 0x0C03:    // RESET
                BPRINT_I_RAW("(HCI_RESET)");
                break;
            case 0x1009:    // Read BD address
                if (len >= 13)
                    BPRINT_I_RAW("(Read BDADD: 0x%02X-%02X-%02X-%02X-%02X-%02X)",
                            buf[12], buf[11], buf[10], buf[9], buf[8], buf[7]);
                break;
            case 0xFC0D:    // RF test
                if (buf[2] == 20) {   // length is 20 for RF RX results
                    uint32_t result = 0;

                    BPRINT_I_RAW("\n");
                    result = STREAM_TO_UINT32(buf + 7);
                    BPRINT_I_RAW("\t       RX Packet Count: %d\n", result);
                    result = STREAM_TO_UINT32(buf + 11);
                    BPRINT_I_RAW("\t         RX Error Rate: %f%%\n", (float)result / 1000000.f);
                    result = STREAM_TO_UINT32(buf + 15);
                    BPRINT_I_RAW("\t         RX Byte Count: %d\n", result);
                    result = STREAM_TO_UINT32(buf + 19);
                    BPRINT_I_RAW("\t        Bit Error Rate: %f%%", (float)result / 1000000.f);
                }
                break;
            case 0x201F:    // BLE RX test
                BPRINT_I_RAW("\n");
                if (buf[2] >= 6 && buf[6] == 0) {
                    uint16_t pktcount = STREAM_TO_UINT16(buf + 7);
                    BPRINT_I_RAW("\t       RX Packet Count: %d", pktcount);
                } else {
                    BPRINT_W("status or length is incorrect");
                }
                break;
            case 0xFC61:    // packet rssi
                BPRINT_I_RAW("\n");
                BPRINT_I_RAW("\t       Packet handle is 0x%02X%02X, RSSI is %d\n", buf[8], buf[7], (int8_t)buf[9]);
                break;
            case 0xFCD1:    // register read
                BPRINT_I_RAW("(register addr: 0x%08X, value: 0x%02X%02X%02X%02X)", efuse_reg_wr.addr, buf[10], buf[9], buf[8], buf[7]);
                break;
            case 0xFF01:    // file read/write
                if (buf[6] == 0 && buf[7] == 0) {
                    if (buf[8] == WR_FLAG_READ)
                        BPRINT_I_RAW("(offset: 0x%02X%02X%02X%02X, value: 0x%02X)", buf[12], buf[11], buf[10], buf[9], buf[13]);
                } else {
                    uint16_t err = buf[6] | (buf[7] << 8);
                    BPRINT_I_RAW("(Error(%d): %s)", err, strerror(err));
                }
                break;
            case 0x1802:    // loopback
                BPRINT_I_RAW("(LOOP_BACK)");
                break;
            }
        } else if (buf[1] == HCE_INQUIRY_RESULT || buf[1] == HCE_INQUIRY_RESULT_WITH_RSSI
                || buf[1] == HCE_EXT_INQUIRY_RESULT) {
            BPRINT_I_RAW("(BDADDR:0x%02X-%02X-%02X-%02X-%02X-%02X",
                    buf[9], buf[8], buf[7], buf[6], buf[5], buf[4]);
            if (buf[1] == HCE_INQUIRY_RESULT_WITH_RSSI || buf[1] == HCE_EXT_INQUIRY_RESULT) {
                int8_t rssi = ~buf[17] + 1;
                BPRINT_I_RAW(", RSSI:%s%d", buf[17] > 0x7F ? "-" : "",
                        buf[17] > 0x7F ? rssi : buf[17]);
            }
            if (buf[1] == HCE_EXT_INQUIRY_RESULT) {
                size_t i = 18;
                while (buf[i]) {
                    if (buf[i + 1] == 8 || buf[i + 1] == 9) {
                        char name[256] = {0};
                        memcpy(name, &buf[i + 2], buf[i] - 1);
                        name[buf[i] - 1] = '\0';
                        BPRINT_I_RAW(", Name:%s", name);
                    }
                    i += (buf[i] + 1);
                }
            }
            BPRINT_I_RAW(")");
        } else if (buf[1] == HCE_INQUIRY_COMPLETE) {
            BPRINT_I_RAW("(Inquiry_Complete)");
        } else if (buf[1] == HCE_CONNECTION_COMPLETE) {
            if (buf[12] == 0x01) {
                BPRINT_I_RAW("(Connection_Complete)(ConnHandle:0x%04X)(ACL)", (((buf[5] << 8) & 0xff00) | buf[4]));
            } else if(buf[12] == 0x00) {
                BPRINT_I_RAW("(Connection_Complete)(ConnHandle:0x%04X)(SCO)", (((buf[5] << 8) & 0xff00) | buf[4]));
            } else {
                BPRINT_I_RAW("(Connection_Complete)(ConnHandle:%d)", ((buf[5] << 8 & 0xff00) | buf[4]));
            }
        } else if (buf[1] == 0xE4 && efuse_reg_wr.wr_flag == WR_FLAG_READ) {
            if ((efuse_reg_wr.addr / 16 * 4) != (buf[9] | (buf[10] << 8))) {
                BPRINT_I_RAW("(Read efuse error!)");
            } else {
                int8_t offset = 9 + ((efuse_reg_wr.addr / 4) % 4) * 6 + 2 + (efuse_reg_wr.addr % 4);

                BPRINT_I_RAW("(efuse addr: 0x%04X, value: 0x%02X)", efuse_reg_wr.addr, buf[offset]);
            }
        } else if (buf[1] == 0x4F && buf[2] == 0x50 && buf[3] == 0xCC) {
            for (i = 0; i < 79 ; i++) {
                if (rssi_setting.alpha_filter > 0) {
                    if (rssi_count == 0)
                        rssi_channels[i] += (int8_t)buf[4 + i] * -1;
                    else {
                        alpha_filter = 1.0 / rssi_setting.alpha_filter;
                        rssi_channels[i] = (int)((rssi_channels[i] * (1 - alpha_filter)) + ((int8_t)buf[4 + i] * -1 * alpha_filter));
                    }
                } else {
                    rssi_channels[i] += (int8_t)buf[4 + i] * -1;
                }
            }
            rssi_count++;
            if (rssi_setting.report == false) {
                if (rssi_count == rssi_setting.avg_package || rssi_setting.alpha_filter > 0) {
                    BPRINT_I_RAW("\nRSSI: \n");
                    for (i = 0; i < 79 ; i++) {
                        if (rssi_setting.alpha_filter == 0)
                            rssi_channels[i] = rssi_channels[i] / rssi_count;
                        if (i >= rssi_setting.ch_start && i <= rssi_setting.ch_end) {
                            BPRINT_I_RAW("% 4d ", rssi_channels[i]);
                        } else {
                            BPRINT_I_RAW("      ");
                        }
                        if ((i + 1) % 10 == 0 || i == 78) {
                            BPRINT_I_RAW("\n");
                        }
                    }
                    if (rssi_setting.one_shot == 1) {
                        boots_pkt_cleanup_report_rssi(1);
                        boots_write_stpbtfwlog("01 73 FD 01 00");
                    }
                    if (rssi_setting.alpha_filter == 0) {
                        for (i = 0; i < 79; i++)
                            rssi_channels[i] = 0;
                        rssi_count = 0;
                    }
                }
            }
        }
    }
}

size_t boots_pkt_handler_parse(uint8_t *buf, size_t len, pkt_list_s **pList)
{
    size_t i = 0;
    size_t ret = 0;
    pkt_list_s *list = NULL;
    uint16_t show_len = BOOTS_MSG_LVL_DEFAULT <= BOOTS_MSG_LVL_INFO ? DEF_SHOW_LEN : 0xFFFF;

    if (pList)
        list = *pList;

    if ((pList == NULL && !(buf[1] == 0x4F && buf[2] == 0x50 && buf[3] == 0xCC))||
        (list && list->s_type != SCRIPT_STRESS && list->s_type != SCRIPT_LOOPBACK && list->s_type != SCRIPT_LPTIMER) ||
        (list && (list->s_type == SCRIPT_STRESS || list->s_type == SCRIPT_LOOPBACK || list->s_type == SCRIPT_LPTIMER) && BOOTS_STRESS_SHOW_ALL_EVENT))
    {
        switch (buf[0]) {
        case HCI_CMD_PKT:
            BPRINT_I_RAW("\t  CMD:");
            break;
        case HCI_ACL_PKT:
            BPRINT_I_RAW("\t  ACL:");
            break;
        case HCI_SCO_PKT:
            BPRINT_I_RAW("\t  SCO:");
            break;
        case HCI_EVENT_PKT:
            BPRINT_I_RAW("\tEvent:");
            break;
        default:
            BPRINT_E("Receive unknonw type %02X", buf[0]);
            BPRINT_I_RAW(" Unknown %02X:", buf[0]);
            exit(0);
        }

        // exclude extended
        for (i = 1; i < len && i <= show_len; i++) {
            BPRINT_I_RAW(" %02X", buf[i]);
        }
        BPRINT_I_RAW("%s ", len > show_len ? "..." : "");
        boots_pkt_parsing(buf, len);    // Parsing
        BPRINT_I_RAW("\n");
    }

    if (len > 2 && buf[0] == HCI_EVENT_PKT) {
        if (buf[1] == HCE_COMMAND_COMPLETE || buf[1] == HCE_INQUIRY_COMPLETE) {
            // means received Command_Complete or Inquiry_Complete
            ret = 0;
        } else if (buf[1] == 0xE4 && buf[2] == 0x1E
                && efuse_reg_wr.wr_flag == WR_FLAG_WRITE) {
            if (pList) {
                *pList = boots_efuse_write(list, &buf[9]);
                if (*pList) ret = 1;
            }
        } else if (buf[1] == 0x4F && buf[2] == 0x50 && buf[3] == 0xCC) {
            boots_pkt_parsing(buf, len);    // Parsing
            ret = 1;
        } else {
            ret = 1;
        }
    }
    return ret;
}

//--------------------------------------------------------------------------
size_t boots_pkt_handler(uint8_t *buf, size_t len, pkt_list_s **pList)
{
    UNUSED(boots_btif);
    size_t i = 0;
    size_t ret = 0;
    pkt_list_s *list = NULL;
    size_t pkt_len = 0;
    uint8_t *buf_parse = 0;
    size_t buf_index = 0;

    if (!buf || !len) return 0;

    buf_parse = &buf[0];

    while (len > 0) {
        if (HCI_CMD_PKT == buf_parse[0])
            pkt_len = len;
        else if (HCI_EVENT_PKT == buf_parse[0])
            pkt_len = buf_parse[2] + HCI_EVENT_PKT_HDR_LEN;
        else if (HCI_ACL_PKT == buf_parse[0])
            pkt_len = (buf_parse[4] << 8) + buf_parse[3] + HCI_ACL_PKT_HDR_LEN;
        else {
            BPRINT_E("Receive unknonw type %02X %02X %02X %02X", buf_parse[0], buf_parse[1],buf_parse[2], buf_parse[3]);
            exit(0);
        }

        if (boots_pkt_handler_parse(buf_parse, pkt_len, pList))
            ret = 1;
        len -= pkt_len;
        buf_index += pkt_len;
        buf_parse = &buf[buf_index];
    }

    if (pList)
        list = *pList;

    /** For next action */
    if (list) {
        if (list->p_type == HCI_EVENT_PKT && buf[0] == HCI_EVENT_PKT) {
            if (memcmp(buf, list->u_cnt.data, list->len)) {
                BPRINT_I("Received is not match with script");
                BPRINT_I_RAW("\tScript:");
                for (i = 0; i < list->len; i++)
                    BPRINT_I_RAW("%02X ", list->u_cnt.data[i]);
                BPRINT_I_RAW("\n");

                if (list->s_type == SCRIPT_WAITRX) {
                    // waiting specific RX
                    BPRINT_I("Retry to get specific event");
                    return 1;
                }
                boots_pkt_list_destroy(list);
                *pList = NULL;
                return 0;

            }
            *pList = boots_pkt_node_pop(list, NULL, NULL);
        }
        if (*pList) ret = 1;
    }
    return ret;
}

//---------------------------------------------------------------------------
static void boots_write_stpbtfwlog(char *buf)
{
    int fd = -1;

    fd = open("/dev/stpbtfwlog", O_WRONLY);
    if (fd < 0) {
        BPRINT_E("Open /dev/stpbtfwlog fail");
        return;
    }
    write(fd, buf, strlen(buf));
    close(fd);
}

//--------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_reset(char *cmd[], size_t len)
{
    uint8_t reset[] = {0x01, 0x03, 0x0C, 0x00};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h')
        return NULL;

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, reset, sizeof(reset),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_inquiry(char *cmd[], size_t len)
{
    uint8_t inq_mode[] = {0x01, 0x45, 0x0C, 0x01, 0x02};
    uint8_t inq[] = {0x01, 0x01, 0x04, 0x05, 0x33, 0x8B, 0x9E, 2, 0x00};
    pkt_list_s *list = NULL;

    BPRINT_D("%s", __func__);
    if (len > 2 && *(cmd[1] + 1) == 't') {
        inq[7] = strtol(cmd[2], NULL, 16);
    } else if (len > 1) {
        return NULL;
    }
    list = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, inq_mode, sizeof(inq_mode),
            NULL, NULL);
    if (list)
        boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, inq, sizeof(inq), list, list);
    return list;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_enter_dut(char *cmd[], size_t len)
{
    uint8_t reset[] = {0x01, 0x03, 0x0C, 0x00};
    uint8_t scan_enable[] = {0x01, 0x1A, 0x0C, 0x01, 0x03};
    uint8_t event_filter[] = {0x01, 0x05, 0x0C, 0x03, 0x02, 0x00, 0x02};
    uint8_t dut[] = {0x01, 0x03, 0x18, 0x00};
    pkt_list_s *list = NULL;
    pkt_list_s *tmp = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h')
        return NULL;

    list = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, reset, sizeof(reset),
            NULL, NULL);
    if (list)
        tmp = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, scan_enable,
            sizeof(scan_enable), list, list);
    if (tmp)
        tmp = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, event_filter,
            sizeof(event_filter), tmp, list);
    if (tmp)
        boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, dut, sizeof(dut), tmp, list);

    return list;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_read_bd_addr(char *cmd[], size_t len)
{
    uint8_t raddr[] = {0x01, 0x09, 0x10, 0x00};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h')
        return NULL;

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, raddr, sizeof(raddr),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_write_bd_addr(char *cmd[], size_t len)
{
    uint8_t waddr[] = {0x01, 0x1A, 0xFC, 0x06, 0, 0, 0, 0, 0, 0};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h')
        return NULL;

    if (len >= 2 && strlen(cmd[1]) == 17) {
        waddr[9] = strtol(cmd[1], NULL, 16);
        waddr[8] = strtol(cmd[1] + 3, NULL, 16);
        waddr[7] = strtol(cmd[1] + 6, NULL, 16);
        waddr[6] = strtol(cmd[1] + 9, NULL, 16);
        waddr[5] = strtol(cmd[1] + 12, NULL, 16);
        waddr[4] = strtol(cmd[1] + 15, NULL, 16);
    /** Don't suppport separated address
    } else if (len == 7) {
        int i = 0;
        for (i = 0; i < 6; i++)
            waddr[9 - i] = strtol(cmd[i + 1], NULL, 16);
    */
    } else {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    if (!waddr[4] && !waddr[5] && !waddr[6] && !waddr[7] && !waddr[8] && !waddr[9]) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, waddr, sizeof(waddr),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_set_power_level(char *cmd[], size_t len)
{
    uint8_t power_level[] = {0x01, 0x17, 0xFC, 0x04, 0x00, 0x00, 0, 0x00};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    power_level[6] = strtol(cmd[1], NULL, 16);
    if (power_level[6] > 9 || (power_level[6] == 0 && *cmd[1] != '0')) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    BPRINT_W("%s: NOTE, this command only affects RFTX", __func__);
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, power_level,
            sizeof(power_level), NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_set_whiten(char *cmd[], size_t len)
{
    uint8_t whiten[] = {0x01, 0x15, 0xFC, 0x01, 0};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    whiten[4] = strtol(cmd[1], NULL, 16);
    if (whiten[4] > 1 || (whiten[4] == 0 && *cmd[1] != '0')) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, whiten, sizeof(whiten),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_rf_tx(char *cmd[], size_t len)
{
    uint8_t rf_test[] = {0x01, 0x0D, 0xFC, 0x17, 0x00, 0x00,
        0,      // pattern
        1,      // hopping:1, fixed channel:0
        0,      // channel if fixed
        0,      // channel if fixed
        0x00,
        0x02,   // poll period, unit: 1.25ms
        0,      // packet type
        0, 0,   // packet length
        0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    pkt_list_s *node = NULL;
    size_t i = 1, j = 0;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'p':       // Pattern
            rf_test[6] = strtol(cmd[i + 1], NULL, 16);
            if (rf_test[6] == 0x0A) {
                rf_test[7] = 0; // fix channel
            } else if (rf_test[6] != 1 && rf_test[6] != 2 && rf_test[6] != 3
                    && rf_test[6] != 4 && rf_test[6] != 9 && rf_test[6] != 0xa) {
                BPRINT_E("%s: Incorrect format: -p %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'c':       // channel
            rf_test[7] = 0;
            rf_test[8] = strtol(cmd[i + 1], NULL, 10);
            rf_test[9] = rf_test[8];
            if (rf_test[8] > 78) {
                BPRINT_E("%s: Incorrect format: -c %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 't':       // packet type
            rf_test[12] = strtol(cmd[i + 1], NULL, 16);
            while (j < ARRAY_SIZE(tx_pi)) {
                if (tx_pi[j].pt == rf_test[12])
                    break;
                else
                    j++;
            }
            if (j >= ARRAY_SIZE(tx_pi)) {
                BPRINT_E("%s: Incorrect format: -t %s", __func__, cmd[i + 1]);
                return NULL;
            }
            if (STREAM_TO_UINT16(rf_test + 13) == 0)
                UINT16_TO_STREAM(rf_test + 13, tx_pi[j].len);
            i += 2;
            BPRINT_D("t/l: %02X %02X", rf_test[13], rf_test[14]);
            break;
        case 'l':
            UINT16_TO_STREAM(rf_test + 13, strtol(cmd[i + 1], NULL, 10));
            i += 2;
            BPRINT_D("l: %02X %02X", rf_test[13], rf_test[14]);
            break;
        case 'o':       // Period
            rf_test[11] = (uint8_t)strtol(cmd[i + 1], NULL, 10);
            if (rf_test[11] < 1 || rf_test[11] > 255) {
                BPRINT_E("%s: Incorrect format: -o %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, rf_test,
            sizeof(rf_test), NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_rf_rx(char *cmd[], size_t len)
{
    uint8_t rf_test[] = {0x01, 0x0D, 0xFC, 0x17, 0x00,
        0,                      // pattern
        0x0B,                   // rx start:0x0B, rx stop:0xFF, rx result:0x0C
        0x00, 0x00,
        0,                      // channel
        0x00, 0x01,
        3,                      // packet type
        0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0xA5, 0xF0, 0xC3, // address
        0x00, 0x00};
    pkt_list_s *node = NULL;
    size_t i = 1, j = 0;

    BPRINT_D("%s: %s", __func__, cmd[0]);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    if (!memcmp("rfr_stop", cmd[0], strlen("rfr_stop"))) {
        rf_test[6] = 0xFF;
    }

    if (!memcmp("rfr_result", cmd[0], strlen("rfr_result"))) {
        rf_test[6] = 0x0C;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'p':       // Pattern
            rf_test[5] = strtol(cmd[i + 1], NULL, 16);
            if (rf_test[5] != 1 && rf_test[5] != 2 && rf_test[5] != 3
                    && rf_test[5] != 4 && rf_test[5] != 9) {
                BPRINT_E("%s: Incorrect format: -p %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'c':       // channel
            rf_test[9] = strtol(cmd[i + 1], NULL, 10);
            if (rf_test[9] > 78) {
                BPRINT_E("%s: Incorrect format: -c %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 't':       // packet type
            rf_test[12] = strtol(cmd[i + 1], NULL, 16);
            while (j < ARRAY_SIZE(rx_pi)) {
                if (rx_pi[j].pt == rf_test[12])
                    break;
                else
                    j++;
            }
            if (j >= ARRAY_SIZE(rx_pi)) {
                BPRINT_E("%s: Incorrect format: -t %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'a':
            if (strlen(cmd[i + 1]) == 11) {
                rf_test[21] = strtol(cmd[i + 1], NULL, 16);
                rf_test[22] = strtol(cmd[i + 1] + 3, NULL, 16);
                rf_test[23] = strtol(cmd[i + 1] + 6, NULL, 16);
                rf_test[24] = strtol(cmd[i + 1] + 9, NULL, 16);
            } else {
                BPRINT_E("%s: Incorrect format", __func__);
                return NULL;
            }
            if (!rf_test[21] && !rf_test[22] && !rf_test[23] && !rf_test[24]) {
                BPRINT_E("%s: Incorrect format", __func__);
                return NULL;
            }
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    if (!rf_test[21] && !rf_test[22] && !rf_test[23] && !rf_test[24]) {
        BPRINT_E("Please input Tester Address");
        return NULL;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, rf_test,
            sizeof(rf_test), NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_rf_stop(char *cmd[], size_t len)
{
    uint8_t rf_stop[] = {0x01, 0x0D, 0xFC, 0x17, 0x00, 0x00,
        0xFF,         /* 0xFF: rf stop */
        0x00, 0x00,
        0x00,
        0x00, 0x00,
        0x00,
        0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, rf_stop,
            sizeof(rf_stop), NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_ble_tx(char *cmd[], size_t len)
{
    uint8_t ble_tx[] = {0x01, 0x1E, 0x20, 0x03, 0, 37, 0};
    uint16_t length = 0;
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'c':   // Channel
            ble_tx[4] = strtol(cmd[i + 1], NULL, 10);
            if (ble_tx[4] > 39) {
                BPRINT_E("%s: Incorrect format: -c %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'p':   // Pattern
            ble_tx[6] = strtol(cmd[i + 1], NULL, 16);
            if (ble_tx[6] > 7) {
                BPRINT_E("%s: Incorrect format: -p %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'l':  // length
            length = strtol(cmd[i + 1], NULL, 10);
            if ((length == 0 && strtol(cmd[i + 1], NULL, 16) != 0) || length > 255) {
                BPRINT_E("%s: Incorrect format: -l %s", __func__, cmd[i + 1]);
                return NULL;
            }
            ble_tx[5] = length;
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, ble_tx, sizeof(ble_tx),
            NULL, NULL);

    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_ble_rx(char *cmd[], size_t len)
{
    uint8_t ble_rx[] = {0x01, 0x1D, 0x20, 0x01, 0 /* channel */};
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'c':   // Channel
            ble_rx[4] = strtol(cmd[i + 1], NULL, 10);
            if (ble_rx[4] > 39) {
                BPRINT_E("%s: Incorrect format: -c %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, ble_rx, sizeof(ble_rx),
            NULL, NULL);

    return node;
}

//--------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_ble_test_stop(char *cmd[], size_t len)
{
    uint8_t ble_stop[] = {0x01, 0x1F, 0x20, 0x00};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h')
        return NULL;

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, ble_stop, sizeof(ble_stop), NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_ble_etx(char *cmd[], size_t len)
{
    uint8_t ble_etx[] = {0x01, 0x34, 0x20, 0x04, 0, 0, 0, 1};
    uint16_t length = 0;
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'c':   // Channel
            ble_etx[4] = strtol(cmd[i + 1], NULL, 10);
            if (ble_etx[4] > 39) {
                BPRINT_E("%s: Incorrect format: -c %s", __func__, cmd[i + 1]);
                return NULL;
            }
            break;
        case 'l':   // Packet length in bytes
            length = strtol(cmd[i + 1], NULL, 10);
            if ((length == 0 && strtol(cmd[i + 1], NULL, 16) != 0) || length > 255) {
                BPRINT_E("%s: Incorrect format: -l %s", __func__, cmd[i + 1]);
                return NULL;
            }
            ble_etx[5] = length;
            break;
        case 'p':   // Packet Payload
            ble_etx[6] = strtol(cmd[i + 1], NULL, 10);
            if (ble_etx[6] > 0x07) { // So far 0x08-0xFF reserved for future use
                BPRINT_E("%s: Incorrect format: -p %s", __func__, cmd[i + 1]);
                return NULL;
            }
            break;
        case 'y':   // PHY
            ble_etx[7] = strtol(cmd[i + 1], NULL, 10);
            if (ble_etx[7] == 0 || ble_etx[7] > 0x05) { // So far 0 and 0x05-0xFF reserved for future use
                BPRINT_E("%s: Incorrect format: -y %s", __func__, cmd[i + 1]);
                return NULL;
            }
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
        i += 2;
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, ble_etx, sizeof(ble_etx),
            NULL, NULL);

    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_ble_erx(char *cmd[], size_t len)
{
    uint8_t ble_erx[] = {0x01, 0x33, 0x20, 0x03, 0, 1, 0};
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'c': // Channel
            ble_erx[4] = strtol(cmd[i + 1], NULL, 10);
            if (ble_erx[4] > 39) {
                BPRINT_E("%s: Incorrect format: -c %s", __func__, cmd[i + 1]);
                return NULL;
            }
            break;
        case 'y': // PHY
            ble_erx[5] = strtol(cmd[i + 1], NULL, 10);
            /** So far 0 and 0x04-0xFF reserved for future use
                7668/6632 not support 0x03:LE Coded PHY */
            if (ble_erx[5] == 0 || ble_erx[5] > 0x03) {
                BPRINT_E("%s: Incorrect format: -y %s", __func__, cmd[i + 1]);
                return NULL;
            }
            break;
        case 'm': // Modulation Index
            ble_erx[6] = strtol(cmd[i + 1], NULL, 10);
            if (ble_erx[6] > 0x01) {
                BPRINT_E("%s: Incorrect format: -m %s", __func__, cmd[i + 1]);
                return NULL;
            }
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
        i += 2;
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, ble_erx, sizeof(ble_erx),
            NULL, NULL);

    return node;
}
//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_tx_tone(char *cmd[], size_t len)
{
    uint8_t txtone[] = {0x01, 0xD5, 0xFC, 0x05, 0, 0, 0, 0, 0};
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 't':   // Tone Type
            txtone[5] = strtol(cmd[i + 1], NULL, 10);
            if (txtone[5] > 4) {
                BPRINT_E("%s: Incorrect format: -t %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'c':
            txtone[4] = strtol(cmd[i + 1], NULL, 10);
            if (txtone[4] > 78) {
                BPRINT_E("%s: Incorrect format: -c %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'm':
            txtone[6] = strtol(cmd[i + 1], NULL, 10);
            if (txtone[6] > 1) {
                BPRINT_E("%s: Incorrect format: -m %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'd':
            txtone[7] = strtol(cmd[i + 1], NULL, 10);
            if (txtone[7] > 2) {
                BPRINT_E("%s: Incorrect format: -d %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        case 'p':
            txtone[8] = strtol(cmd[i + 1], NULL, 10);
            if (txtone[8] > 1) {
                BPRINT_E("%s: Incorrect format: -p %s", __func__, cmd[i + 1]);
                return NULL;
            }
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    if (txtone[5] <= 3) {
        // Only for Modulation Tone
        txtone[6] = txtone[7] = txtone[8] = 0;
    } else if (txtone[6] == 1) {
        // Only for BT mode
        txtone[7] = 0;
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, txtone, sizeof(txtone),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_radio_setting(char *cmd[], size_t len)
{
    uint8_t set_radio[] = {0x01, 0x79, 0xFC, 0x08,
                           7, 0x00, 0x00, 0x00, 7, 7, 0, 0};
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    /**
     * for 7668, 7663
     * Level: radio setting -b <0~7> -e <0~7> -m <0~7> -s <0~3> -o <0/1>
     *
     * for Buzzard
     * dbm: radio setting -b <-29~20> -e <-29~20> -m <-29~20> -n <0/1> -o <0/1>
     */

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'b':   // BT TX power level/dbm
            set_radio[4] = strtol(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        case 'e':   // BLE TX power level/dbm
            set_radio[8] = strtol(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        case 'm':   // MAX TX power level/dbm
            set_radio[9] = strtol(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        case 's':   // 1db adjust
            set_radio[10] = strtol(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        case 'n':   // enable Lv9
            set_radio[10] = strtol(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        case 'o':   // 3db diff mode
            set_radio[11] = strtol(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, set_radio, sizeof(set_radio),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_tx_power_offset(char *cmd[], size_t len)
{
    /* 4 is header, 7668 MAX has 22 bytes, 7663 MAX has 20 bytes */
    uint8_t txpoweroffset[] = {0x01, 0x93, 0xFC, 0x06,
                               /* g: power offset, for 7662/7668 only 6 groups, but in 7663 have 16 */
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               /* r/i: power offset/PDC value for level 8 ONLY for 7668. */
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               /* e: ePA offset ONLY for 7668 */
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t hybridmode[] = {0x01, 0x74, 0xFD, 0x01, 0x02};
    pkt_list_s *list = NULL;
    pkt_list_s *node = NULL;
    size_t i = 1;
    size_t cmd_len = 10; /* 16 bytes command supporting since 2018/5, 22 bytes command supporting since 2018/7 */
    int8_t temp = 0x00;
    uint8_t lvl_8_temp[] = {0, 0, 0, 0, 0, 0};
    bool hybrid = false;
    bool lvl_8 = false;
    bool pwr16 = false; /* false: 7662/7668 has 6 groups., true: 7663 has 16 groups */

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2) && memcmp(cmd[1], "g16", strlen("g16")) != 0) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    /**
     * TX power offset -g0 <-16~8> -g1 <-16~8> -g2 <-16~8>
     *      -g3 <-16~8> -g4 <-16~8> -g5 <-16~8> ...
     * TX power offset for ePA -e0 <-16~8> -e1 <-16~8> -e2 <-16~8>
     *      -e3 <-16~8> -e4 <-16~8> -e5 <-16~8>
     * TX power offset for level 8 RFCR -r0 <-2~4> -r1 <-2~4> -r2 <-2~4>
     *      -r3 <-2~4> -r4 <-2~4> -r5 <-2~4>
     * TX power offset for level 8 DPC -i0 <-6~6> -i1 <-6~6> -i2 <-6~6>
     *      -i3 <-6~6> -i4 <-6~6> -i5 <-6~6>
     *  One step is +/-0.25dbm
     */

    if (memcmp(cmd[i], "g16", strlen("g16")) == 0) {
        pwr16 = true;
        i++;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'g': {     // Tone Type
            uint8_t gpidx = strtol(cmd[i] + 2, NULL, 10);

            if (hybrid == true) {
                BPRINT_E("%s: Incorrect format: %s - You are in hybrid mode", __func__, cmd[i]);
                return NULL;
            }
            if (gpidx < 16 && gpidx > 5)
                pwr16 = true;
            else if (gpidx >= 16) {
                BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
                return NULL;
            }
            temp = (int8_t)strtol(cmd[i + 1], NULL, 10);
            if (temp > 8 || temp < -16) {
                    BPRINT_E("%s: Incorrect range: %s %d", __func__, cmd[i], temp);
                    return NULL;
                }
            txpoweroffset[gpidx + 4] = temp;
            i += 2;
            } break;
        case 'r':      // lvl 8 offset RFCR value
            if (pwr16 == true) {
                BPRINT_E("%s: Incorrect format: %s - 7668 groups should only 0~5", __func__, cmd[i]);
                return NULL;
            }

            if (cmd_len == 10) {
                cmd_len = 22;
                txpoweroffset[3] = 0x12;
            }
            hybrid = true;
            lvl_8 = true;
            switch (*(cmd[i] + 2)) {
            case '0':
                temp = (int8_t)strtol(cmd[i + 1], NULL, 10);
                if (temp > 4 || temp < -2) {
                    BPRINT_E("%s: Incorrect range: %s", __func__, cmd[i]);
                    return NULL;
                }
                txpoweroffset[10] |= ((temp << 4) & 0xF0);
                i += 2;
                break;
            case '1':
                temp = (int8_t)strtol(cmd[i + 1], NULL, 10);
                if (temp > 4 || temp < -2) {
                    BPRINT_E("%s: Incorrect range: %s", __func__, cmd[i]);
                    return NULL;
                }
                txpoweroffset[11] |= ((temp << 4) & 0xF0);
                i += 2;
                break;
            case '2':
                temp = (int8_t)strtol(cmd[i + 1], NULL, 10);
                if (temp > 4 || temp < -2) {
                    BPRINT_E("%s: Incorrect range: %s", __func__, cmd[i]);
                    return NULL;
                }
                txpoweroffset[12] |= ((temp << 4) & 0xF0);
                i += 2;
                break;
            case '3':
                temp = (int8_t)strtol(cmd[i + 1], NULL, 10);
                if (temp > 4 || temp < -2) {
                    BPRINT_E("%s: Incorrect range: %s", __func__, cmd[i]);
                    return NULL;
                }
                txpoweroffset[13] |= ((temp << 4) & 0xF0);
                i += 2;
                break;
            case '4':
                temp = (int8_t)strtol(cmd[i + 1], NULL, 10);
                if (temp > 4 || temp < -2) {
                    BPRINT_E("%s: Incorrect range: %s", __func__, cmd[i]);
                    return NULL;
                }
                txpoweroffset[14] |= ((temp << 4) & 0xF0);
                i += 2;
                break;
            case '5':
                temp = (int8_t)strtol(cmd[i + 1], NULL, 10);
                if (temp > 4 || temp < -2) {
                    BPRINT_E("%s: Incorrect range: %s", __func__, cmd[i]);
                    return NULL;
                }
                txpoweroffset[15] |= ((temp << 4) & 0xF0);
                i += 2;
                break;
            default:
                BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
                return NULL;
            }
            break;
        case 'i':     // lvl 8 offset DPC value
            if (pwr16 == true) {
                BPRINT_E("%s: Incorrect format: %s - 7668 groups should only 0~5", __func__, cmd[i]);
                return NULL;
            }
            if (cmd_len == 10) {
                cmd_len = 22;
                txpoweroffset[3] = 0x12;
            }
            hybrid = true;
            switch (*(cmd[i] + 2)) {
            case '0':
                lvl_8_temp[0] = (char)strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '1':
                lvl_8_temp[1] = (char)strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '2':
                lvl_8_temp[2] = (char)strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '3':
                lvl_8_temp[3] = (char)strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '4':
                lvl_8_temp[4] = (char)strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '5':
                lvl_8_temp[5] = (char)strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            default:
                BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
                return NULL;
            }
            break;
        case 'e':    // ePA offset
            if (pwr16 == true) {
                BPRINT_E("%s: Incorrect format: %s - 7668 groups should only 0~5", __func__, cmd[i]);
                return NULL;
            }
            if (cmd_len == 10) {
                cmd_len = 22;
                txpoweroffset[3] = 0x12;
            }
            hybrid = true;
            switch (*(cmd[i] + 2)) {
            case '0':
                txpoweroffset[16] = strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '1':
                txpoweroffset[17] = strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '2':
                txpoweroffset[18] = strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '3':
                txpoweroffset[19] = strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '4':
                txpoweroffset[20] = strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            case '5':
                txpoweroffset[21] = strtol(cmd[i + 1], NULL, 10);
                i += 2;
                break;
            default:
                BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
                return NULL;
            }
            break;
        default:
            i++;
        }
    }
    if (pwr16 == true) {
        txpoweroffset[3] = 0x10;
        cmd_len = txpoweroffset[3] + 4;
        BPRINT_I("%s: "BLUE"This is 16 groups setting for 7663"NONE, __func__);
    } else if (lvl_8 == true) {
        for (i = 0 ; i < 6 ; i++)
            txpoweroffset[i + 10] |= (lvl_8_temp[i] & 0x0F);
        hybridmode[4] = 0x02;
    } else {
        BPRINT_I("%s: "BLUE"This is 6 groups setting for 7662, 7668 etc."NONE, __func__);
        for (i = 0 ; i < 6 ; i++)
            txpoweroffset[i + 10] = lvl_8_temp[i];
        hybridmode[4] = 0x01;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, txpoweroffset, cmd_len , NULL, NULL);
    if (hybrid == true) {
        list = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, hybridmode, sizeof(hybridmode), node, node);
        return list;
    }
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_efuse_wr(char *cmd[], size_t len)
{
    uint8_t efuse_r[] = {0x01, 0x6F, 0xFC, 0x0E,
                               0x01, 0x0D, 0x0A, 0x00, 0x02, 0x04,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00};
    uint8_t efuse_w[] = {0x01, 0x6F, 0xFC, 0x18,
                               0x01, 0x0D, 0x14, 0x00, 0x01, 0x01,
                               0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00};
    pkt_list_s *node = NULL;
    uint16_t addr = 0;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len % 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    /*
    * efuse -w -a <> -v <>
    * efuse -r -a <>
    */

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'w':
            /* read value first
            efuse[3] = 24;
            efuse[6] = 0x14;
            efuse[8] = 0x01;
            efuse[8] = 0x01;
            */
            efuse_r[3] = 14;      /* length */
            efuse_r[6] = 0x0A;
            efuse_r[8] = 0x02;
            efuse_r[9] = 0x04;
            efuse_reg_wr.wr_flag = WR_FLAG_WRITE;
            i += 1;
            break;
        case 'r':
            efuse_r[3] = 14;      /* length */
            efuse_r[6] = 0x0A;
            efuse_r[8] = 0x02;
            efuse_r[9] = 0x04;
            efuse_reg_wr.wr_flag = WR_FLAG_READ;
            i += 1;
            break;
        case 'a':
            efuse_reg_wr.addr = strtoul(cmd[i + 1], NULL, 16);
            addr = efuse_reg_wr.addr / 16 * 4;
            efuse_r[10] = addr & 0xFF;
            efuse_r[11] = (addr & 0xFF00) >> 8;
            efuse_r[12] = (addr + 1) & 0xFF;
            efuse_r[13] = ((addr + 1) & 0xFF00) >> 8;
            efuse_r[14] = (addr + 2) & 0xFF;
            efuse_r[15] = ((addr + 2) & 0xFF00) >> 8;
            efuse_r[16] = (addr + 3) & 0xFF;
            efuse_r[17] = ((addr + 3) & 0xFF00) >> 8;
            i += 2;
            break;
        case 'v':
            efuse_reg_wr.value = strtoul(cmd[i + 1], NULL, 16);
            i += 2;
            break;

        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, efuse_r, (efuse_r[3] + 4),
            NULL, NULL);
    if (efuse_reg_wr.wr_flag == WR_FLAG_WRITE && node)
    {
        boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, efuse_w, (efuse_w[3] + 4),
            node, node);
    }
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_register_wr(char *cmd[], size_t len)
{
    uint8_t reg[] = {0x01, 0x00, 0xFC, 0x00,
                               0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00};
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len % 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    /*
    * register -w -a <> -v <>
    * register -r -a <>
    */

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'w':
            reg[1] = 0xD0;
            reg[3] = 0x08;     /* length */
            efuse_reg_wr.wr_flag = WR_FLAG_WRITE;
            i += 1;
            break;
        case 'r':
            reg[1] = 0xD1;
            reg[3] = 0x04;     /* length */
            efuse_reg_wr.wr_flag = WR_FLAG_READ;
            i += 1;
            break;
        case 'a':
            efuse_reg_wr.addr = strtoul(cmd[i + 1], NULL, 16);
            reg[4] = efuse_reg_wr.addr & 0xFF;
            reg[5] = (efuse_reg_wr.addr & 0xFF00) >> 8;
            reg[6] = (efuse_reg_wr.addr & 0xFF0000) >> 16;
            reg[7] = (efuse_reg_wr.addr & 0xFF000000) >> 24;
            i += 2;
            break;
        case 'v':
            efuse_reg_wr.value = strtoul(cmd[i + 1], NULL, 16);
            reg[8] = efuse_reg_wr.value & 0xFF;
            reg[9] = (efuse_reg_wr.value & 0xFF00) >> 8;
            reg[10] = (efuse_reg_wr.value & 0xFF0000) >> 16;
            reg[11] = (efuse_reg_wr.value & 0xFF000000) >> 24;
            i += 2;
            break;

        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, reg, (reg[3] + 4),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_file_wr(char *cmd[], size_t len)
{
    uint8_t file[256] = {0x01, 0x01, 0xFF, 0x06,
                          0x00,
                          0x00, 0x00, 0x00, 0x00,
                          0x00,
                          0x00};
    uint32_t addr = 0;
    uint8_t file_path = 0;
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len % 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    /****************************************************
    format:
    CMD: 01 01 FF LEN WR ADDR VALUE FILE_PATH
    EVENT: 04 0E LEN 01 01 FF ERRNO WR ADDR VALUE

    LEN: 1 byte
    WR: 1 byte
    ADDR: 4 bytes
    VALUE: 1 byte
    ERRNO: 2 bytes
    FILE_PATH:
    ****************************************************/

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'w':
            file[4] = WR_FLAG_WRITE;
            i += 1;
            break;
        case 'r':
            file[4] = WR_FLAG_READ;
            i += 1;
            break;
        case 'a':
            addr = strtoul(cmd[i + 1], NULL, 16);
            file[5] = addr & 0xFF;
            file[6] = (addr & 0xFF00) >> 8;
            file[7] = (addr & 0xFF0000) >> 16;
            file[8] = (addr & 0xFF000000) >> 24;
            i += 2;
            break;
        case 'v':
            file[9] = strtol(cmd[i + 1], NULL, 16);
            i += 2;
            break;
        case 'f':
            strncpy((char *)&file[10], cmd[i + 1], strlen(cmd[i + 1]) < (256 - 10 - 1) ? strlen(cmd[i + 1]) : (256 - 10 - 1));
            file[3] += strlen((char *)&file[10]) + 1;
            file_path = 1;
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    if (file_path == 0)
    {
        BPRINT_E("%s: please input the file path and name(-f)", __func__);
        return NULL;
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, file, (file[3] + 4),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_sync(char *cmd[], size_t len)
{
    uint8_t access_code[] = {0x01, 0x12, 0xFC, 0x08, 0, 0, 0, 0, 0, 0, 0, 0};
    pkt_list_s *node = NULL;

    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len >= 2 && strlen(cmd[1]) == 23) {
        access_code[11] = strtol(cmd[1], NULL, 16);
        access_code[10] = strtol(cmd[1] + 3, NULL, 16);
        access_code[9] = strtol(cmd[1] + 6, NULL, 16);
        access_code[8] = strtol(cmd[1] + 9, NULL, 16);
        access_code[7] = strtol(cmd[1] + 12, NULL, 16);
        access_code[6] = strtol(cmd[1] + 15, NULL, 16);
        access_code[5] = strtol(cmd[1] + 18, NULL, 16);
        access_code[4] = strtol(cmd[1] + 21, NULL, 16);
    } else {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    if (!access_code[4] && !access_code[5] && !access_code[6] && !access_code[7]
            && !access_code[8] && !access_code[9] && !access_code[10] && !access_code[11]) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, access_code,
            sizeof(access_code), NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_stress_test(char *cmd[], size_t len)
{
    uint8_t cmd_buf[BOOTS_STRESS_STRESS_TEST_MAX_PKT_SIZE + HCI_CMD_PKT_HDR_LEN] = {0};
    pkt_list_s *node = NULL;
    uint32_t packetSize = 1;
    uint32_t i;

    if (len == 2 && *(cmd[1] + 1) == 'h')
        return NULL;

    if (len == 2)
        packetSize = atoi(cmd[1]);

    if (packetSize > BOOTS_STRESS_STRESS_TEST_MAX_PKT_SIZE) {
        BPRINT_E("Packet Size should be smaller than %d!", BOOTS_STRESS_STRESS_TEST_MAX_PKT_SIZE);
        return NULL;
    }

    BPRINT_I("%s : Test with 'Write Local Name', Packet Size = %d", __func__, packetSize);
    cmd_buf[0] = 0x01;
    cmd_buf[1] = 0x13;
    cmd_buf[2] = 0x0c;
    cmd_buf[3] = packetSize;
    for (i = 0; i < packetSize; i++)
        cmd_buf[i + HCI_CMD_PKT_HDR_LEN] = ((i + 1) & 0xff);

    if (BOOTS_STRESS_MEASURE_IN_BOOTS)
        boots_stress_init();

    node = boots_pkt_node_push(SCRIPT_STRESS, HCI_CMD_PKT, cmd_buf, packetSize + HCI_CMD_PKT_HDR_LEN, NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_loop_back_test(char *cmd[], size_t len)
{
    uint8_t cmd_reset[] = {0x01, 0x03, 0x0C, 0x00};
    uint8_t cmd_loopback[] = {0x01, 0x02, 0x18, 0x01, 0x01};
    uint8_t cmd_buf[BOOTS_STRESS_LOOPBACK_TEST_MAX_PKT_SIZE + HCI_ACL_PKT_HDR_LEN] = {0};
    pkt_list_s *node = NULL;
    pkt_list_s *node_tmp = NULL;
    uint32_t packetSize = 1;
    uint32_t i;
    int32_t wait = 1;

    if (len == 2 && *(cmd[1] + 1) == 'h')
        return NULL;

    if (len == 2)
        packetSize = atoi(cmd[1]);

    if (packetSize > BOOTS_STRESS_LOOPBACK_TEST_MAX_PKT_SIZE) {
        BPRINT_E("Packet Size should be smaller than %d!", BOOTS_STRESS_LOOPBACK_TEST_MAX_PKT_SIZE);
        return NULL;
    }

    BPRINT_I("%s : Test with ACL loopback, Packet Size = %d", __func__, packetSize);
    cmd_buf[0] = 0x02;
    cmd_buf[1] = 0x32;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = (packetSize & 0x00ff);
    cmd_buf[4] = ((packetSize & 0xff00) >> 8);
    for (i = 0; i < packetSize; i++)
        cmd_buf[i + HCI_ACL_PKT_HDR_LEN] = ((i + 1) & 0xff);

    if (BOOTS_STRESS_MEASURE_IN_BOOTS)
        boots_stress_init();

    node = boots_pkt_node_push(SCRIPT_CMD, HCI_CMD_PKT, cmd_reset, sizeof(cmd_reset), NULL, NULL);
    if (node)
        node_tmp = boots_pkt_node_push(SCRIPT_CMD, HCI_CMD_PKT, cmd_loopback, sizeof(cmd_loopback), node, node);
    if (node_tmp)
        node_tmp = boots_pkt_node_push(SCRIPT_WAITRX, HCI_EVENT_PKT, (void*)&wait, 0, node_tmp, node);
    if (node_tmp)
        node_tmp = boots_pkt_node_push(SCRIPT_WAITRX, HCI_EVENT_PKT, (void*)&wait, 0, node_tmp, node);
    if (node_tmp)
        node_tmp = boots_pkt_node_push(SCRIPT_WAITRX, HCI_EVENT_PKT, (void*)&wait, 0, node_tmp, node);
    if (node_tmp)
        boots_pkt_node_push(SCRIPT_LOOPBACK, HCI_ACL_PKT, cmd_buf, packetSize + HCI_ACL_PKT_HDR_LEN, node_tmp, node);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_loop_timer_test(char *cmd[], size_t len)
{
    uint8_t cmd_reset[] = {0x01, 0x03, 0x0C, 0x00};
    uint8_t cmd_loopback[] = {0x01, 0x02, 0x18, 0x01, 0x01};
    uint8_t cmd_buf[BOOTS_STRESS_LOOPBACK_TEST_MAX_PKT_SIZE + HCI_ACL_PKT_HDR_LEN] = {0};
    pkt_list_s *node = NULL;
    pkt_list_s *node_tmp = NULL;
    uint32_t packetSize = 1;
    uint32_t i = 1;
    int32_t wait = 1;

    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    boots_loop_timer = 0;

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 't':   // timer
            boots_loop_timer = atoi(cmd[i + 1]);
            i += 2;
            break;
        case 's':   // package size
            packetSize = atoi(cmd[i + 1]);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    if (packetSize > BOOTS_STRESS_LOOPBACK_TEST_MAX_PKT_SIZE) {
        BPRINT_E("Packet Size should be smaller than %d!", BOOTS_STRESS_LOOPBACK_TEST_MAX_PKT_SIZE);
        return NULL;
    }

    BPRINT_I("%s : Test with ACL loopback, Timer = %d, Packet Size = %d", __func__, boots_loop_timer, packetSize);
    cmd_buf[0] = 0x02;
    cmd_buf[1] = 0x32;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = (packetSize & 0x00ff);
    cmd_buf[4] = ((packetSize & 0xff00) >> 8);
    for (i = 0; i < packetSize; i++)
        cmd_buf[i + HCI_ACL_PKT_HDR_LEN] = ((i + 1) & 0xff);

    if (BOOTS_STRESS_MEASURE_IN_BOOTS)
        boots_stress_init();

    node = boots_pkt_node_push(SCRIPT_CMD, HCI_CMD_PKT, cmd_reset, sizeof(cmd_reset), NULL, NULL);
    if (node)
        node_tmp = boots_pkt_node_push(SCRIPT_CMD, HCI_CMD_PKT, cmd_loopback, sizeof(cmd_loopback), node, node);
    if (node_tmp)
        node_tmp = boots_pkt_node_push(SCRIPT_WAITRX, HCI_EVENT_PKT, (void*)&wait, 0, node_tmp, node);
    if (node_tmp)
        node_tmp = boots_pkt_node_push(SCRIPT_WAITRX, HCI_EVENT_PKT, (void*)&wait, 0, node_tmp, node);
    if (node_tmp)
        node_tmp = boots_pkt_node_push(SCRIPT_WAITRX, HCI_EVENT_PKT, (void*)&wait, 0, node_tmp, node);
    if (node_tmp)
        boots_pkt_node_push(SCRIPT_LPTIMER, HCI_ACL_PKT, cmd_buf, packetSize + HCI_ACL_PKT_HDR_LEN, node_tmp, node);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_set_pa(char *cmd[], size_t len)
{
    uint8_t epa[] = {0x01, 0x70, 0xFD, 0x09,
                           0x00,                                             // 00 : Disable ePA,  01 : Enable ePA
                           0x07, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00    // power Level, same as pwrlvl and radio
                    };
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    epa[4] = strtol(cmd[1], NULL, 16);
    if (epa[4] > 1 || (epa[4] == 0 && *cmd[1] != '0')) {
        BPRINT_E("%s: Incorrect format: %d", __func__, epa[4]);
        return NULL;
    }
    if (epa[4] == 1) {
        BPRINT_D("ePA mode, change power level to level 9.");
        epa[5] = 0x09;
        epa[9] = 0x09;
        epa[10] = 0x09;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, epa, sizeof(epa),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_set_ant(char *cmd[], size_t len)
{
    uint8_t ant[] = {0x01, 0x71, 0xFD, 0x01, 0}; //00 : BT uses ANT2,FDD  01 : BT uses ANT1,TDD
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    ant[4] = strtol(cmd[1], NULL, 16);
    if (ant[4] > 1 || (ant[4] == 0 && *cmd[1] != '0')) {
        BPRINT_E("%s: Incorrect format: %d", __func__, ant[4]);
        return NULL;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, ant, sizeof(ant),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_background_rssi_scan(char *cmd[], size_t len)
{
    uint8_t ant[] = {0x01, 0x73, 0xFD, 0x01, 0}; //00 : disable  01 : enable
    pkt_list_s *list = NULL;
    pkt_list_s *node = NULL;
    rssi_set_s rssi;
    uint32_t i = 2;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }
    ant[4] = strtol(cmd[1], NULL, 16);

    rssi.avg_package = 1;
    rssi.ch_start = 0;
    rssi.ch_end = 78;
    rssi.report = false;
    rssi.stop = !ant[4];
    rssi.alpha_filter = 0;
    rssi.one_shot = 0;

    if (ant[4] > 1 || (ant[4] == 0 && *cmd[1] != '0')) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'o':   // output report when stop
            rssi.report = atoi(cmd[i + 1]);
            i += 2;
            break;
        case 's':   // start channel
            rssi.ch_start = atoi(cmd[i + 1]);
            if (rssi.ch_start < 0 || rssi.ch_start > 78)
                BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            i += 2;
            break;
        case 'e':   // end chennel
            rssi.ch_end = atoi(cmd[i + 1]);
            if (rssi.ch_end < 0 || rssi.ch_end > 78)
                BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            i += 2;
            break;
        case 'a':   // average package
            rssi.avg_package = atoi(cmd[i + 1]);
            i += 2;
            break;
        case 'f':   // alpha filter
            rssi.alpha_filter = atoi(cmd[i + 1]);
            i += 2;
            break;
        case 'n':   // report only one
            rssi.one_shot = atoi(cmd[i + 1]);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    if (rssi.ch_end < rssi.ch_start)
        BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);

    node = boots_pkt_node_push(SCRIPT_RSSI, HCI_CMD_PKT, &rssi, sizeof(rssi), NULL, NULL);
    list = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, ant, sizeof(ant), node, node);

    return list;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_packet_rssi(char *cmd[], size_t len)
{
    uint8_t rssi[] = {0x01, 0x61, 0xFC, 0x02, 0x32, 0x00};
    pkt_list_s *node = NULL;
    uint16_t handle = 0;
    uint32_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'a':   // handle, default 0x0032, opt.
            handle = (uint16_t)strtoul(cmd[i + 1], NULL, 16);
            rssi[4] = (uint8_t)(handle & 0x00FF);
            rssi[5] = (uint8_t)((handle & 0xFF00) >> 8);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, &rssi, sizeof(rssi), NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_set_buffer_mode(char *cmd[], size_t len)
{
    FILE *fp = NULL;
    ssize_t read = 0;
    size_t line_len = 0;
    char *wbuf = NULL;
    char *line = NULL;
    int fp_len = 0;
    char path[64] = DEFAULT_WIFI_CFG;
    char path_cp[64] = DEFAULT_WIFI_CFG;
    size_t i = 1;
    ssize_t w_size = 0;
    int enable = 0;
    char *pch = NULL;
    char str_buf[64];

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'f':
            memset(path, 0x00, sizeof(path));
            strncpy(path, cmd[i + 1], sizeof(path) - 1);
            strncpy(path_cp, cmd[i + 1], sizeof(path_cp) - 1);
            i += 2;
            break;
        case 'e':
            enable = atoi(cmd[i + 1]);
            i += 2;
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    fp = fopen(path, "r+");
    if (fp != NULL) {
        fseek(fp, 0L, SEEK_END);
        fp_len = ftell(fp);
        wbuf = malloc(fp_len + 1 + 21);
        if (!wbuf) {
            BPRINT_E("%s: Alloc wbuf failed", __func__);
            fclose(fp);
            return NULL;
        }
        fseek(fp, 0L, SEEK_SET);

        while ((read = getline(&line, &line_len, fp)) != -1) {
            pch = strstr(line, E2P_MODE);
            if (!pch) {
                memcpy(&wbuf[w_size], line, read);
                w_size += read;
            } else
                continue;
        }
        free(line);
        fclose(fp);
    } else {
        wbuf = malloc(21);
        if (!wbuf) {
            BPRINT_E("%s: Alloc wbuf failed", __func__);
            return NULL;
        }
    }

    i = snprintf(str_buf, sizeof(str_buf),"%s %d\n", E2P_MODE, enable);
    memcpy(&wbuf[w_size], str_buf, i);
    w_size += i;
    fp = fopen(path_cp, "w");
    if (fp == NULL) {
        BPRINT_E("open file %s failed, errno = %s(%d)\n", path_cp, strerror(errno), errno);
        free(wbuf);
        return NULL;
    }
    fwrite(wbuf, 1, w_size, fp);
    fclose(fp);
    free(wbuf);
    // Please note driver also need support this after ...[TODO]
    boots_write_stpbtfwlog("reload_setting");
    BPRINT_I("Set %s:%d, if no efficacy please reboot platform.", E2P_MODE, enable);
    return NULL;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_set_tx_hopping(char *cmd[], size_t len)
{
    uint8_t tx_hopping[] = {0x01, 0x90, 0xFD, 0x01, 0x00};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len != 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    if (!memcmp("low", cmd[1], strlen("low"))) {
        tx_hopping[4] = 0x00;
    } else if (!memcmp("middle", cmd[1], strlen("middle"))) {
        tx_hopping[4] = 0x01;
    } else if (!memcmp("high", cmd[1], strlen("high"))) {
        tx_hopping[4] = 0x02;
    } else {
        BPRINT_E("%s: Incorrect format: %s)", __func__, cmd[1]);
        return NULL;
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, tx_hopping,
            sizeof(tx_hopping), NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_switch_bt(char *cmd[], size_t len)
{
    uint8_t switch_bt[] = {0x01, 0xF4, 0xFC, 0x02, 0xFD, 0}; // 0 : BT0  1 : BT1
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    switch_bt[5] = strtoul(cmd[1], NULL, 16);

    if (switch_bt[5] > 1 || switch_bt[5] < 0) {
        BPRINT_E("%s: Incorrect format: %d)", __func__, switch_bt[5]);
        return NULL;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, switch_bt, sizeof(switch_bt),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_power_compensation(char *cmd[], size_t len)
{
    uint8_t power_comp[] = {0x01, 0xEA, 0xFC, 0x0A, 0x02, 0x0A,
        0, // ANT index
        0, // offset level
        0, 0, 0, 0, 0, 0}; // group value
    pkt_list_s *node = NULL;
    size_t i = 1;
    int8_t temp = 0x00;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'a':   // ANT index
            power_comp[6] = strtoul(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        case 'l':   // offset level
            power_comp[7] = strtoul(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        case 'g': {     // group value
            uint8_t gpidx = strtoul(cmd[i] + 2, NULL, 10);
            if (gpidx > 5 || gpidx < 0) {
                BPRINT_E("%s: Incorrect format: %d", __func__, gpidx);
                return NULL;
            }
            temp = (int8_t)strtol(cmd[i + 1], NULL, 10);
            if (temp > 8 || temp < -16) {
                BPRINT_E("%s: Incorrect range: %s %d", __func__, cmd[i], temp);
                return NULL;
            }
            power_comp[gpidx + 8] = temp;
            i += 2;
            }
            break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, power_comp, sizeof(power_comp),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_power_boundary(char *cmd[], size_t len)
{
    uint8_t power_bdry[] = {0x01, 0xEA, 0xFC, 0x09, 0x02, 0x0B,
        0, // ANT index
        0, // reserve
        0, 0, 0, 0, 0}; // group channel boundry
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (!(len % 2)) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'a':   // ANT index
            power_bdry[6] = strtol(cmd[i + 1], NULL, 10);
            i += 2;
            break;
        case 'b': {     // boundary value
            uint8_t bdidx = strtol(cmd[i] + 2, NULL, 10);
            if (bdidx > 4 || bdidx < 0) {
                BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
                return NULL;
            }
            power_bdry[bdidx + 8] = strtol(cmd[i + 1], NULL, 10);
            i += 2;
            } break;
        default:
            BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
            return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, power_bdry, sizeof(power_bdry),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_set_sx(char *cmd[], size_t len)
{
    uint8_t sxtrx[] = {0x01, 0xEA, 0xFC, 0x03, 0x02,
        0, // 0x06 : st_tx  0x07 : st_rx
        0}; // as below
    /****************************
    BT0 SX Configration 0x15[3:0]
    4'b0001:DIV2 for Rx
    4'b0011:SSB 8/3 for T/Rx
    4'b0100:SSB 16/5 for T/Rx
    4'b0101:DIV2 LP for Rx
    BT1 SX Configration 0x15[3:0]
    4'b0000:VCDL 5/2 for T/Rx
    4'b0001:DIV2 for Rx
    4'b0010:VCDL 7/2 for T/Rx
    4'b0101:DIV2 LP for Rx
    ****************************/
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    if (!memcmp("sxtx", cmd[0], strlen("sxtx"))) {
        sxtrx[5] = 0x06;
    }

    if (!memcmp("sxrx", cmd[0], strlen("sxrx")) ||
        !memcmp("swrxmode", cmd[0], strlen("swrxmode"))) {
        sxtrx[5] = 0x07;
    }

    sxtrx[6] = strtol(cmd[1], NULL, 10);

    if (sxtrx[6] > 5 || sxtrx[6] < 0) {
        BPRINT_E("%s: Incorrect format: %d", __func__, sxtrx[6]);
        return NULL;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, sxtrx, sizeof(sxtrx),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_ant_to_pin_loss(char *cmd[], size_t len)
{
    uint8_t a2ploss[] = {0x01, 0xEA, 0xFC, 0x03, 0x02, 0x09, 0};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    a2ploss[6] = strtol(cmd[1], NULL, 10);

    if (a2ploss[6] > 19 || a2ploss[6] < 0) {
        BPRINT_E("%s: Incorrect format: %d", __func__, a2ploss[6]);
        return NULL;
    }
    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, a2ploss, sizeof(a2ploss),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_read_edr_info(char *cmd[], size_t len)
{
    uint8_t redrinfo[] = {0x01, 0xEA, 0xFC, 0x03, 0x02, 0x08, 0x00};
    pkt_list_s *node = NULL;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h')
        return NULL;

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, redrinfo, sizeof(redrinfo),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
static pkt_list_s *mtk_boots_cmd_epa_elna(char *cmd[], size_t len)
{
    uint8_t palna[] = {0x01, 0x97, 0xFC, 0x03, 0x02, 0x02, 0};
    pkt_list_s *node = NULL;
    size_t i = 1;

    BPRINT_D("%s", __func__);
    if (len == 2 && *(cmd[1] + 1) == 'h') {
        return NULL;
    } else if (len < 2) {
        BPRINT_E("%s: Incorrect format", __func__);
        return NULL;
    }

    while (i + 1 < len) {
        switch (*(cmd[i] + 1)) {
        case 'p':   // epa
            palna[6] |= strtol(cmd[i + 1], NULL, 16);
            i += 2;
            break;
        case 'l':   // elna
            palna[6] |= strtol(cmd[i + 1], NULL, 16) << 4;
            i += 2;
            break;

        default:
                BPRINT_E("%s: Incorrect format: %s", __func__, cmd[i]);
                return NULL;
        }
    }

    node = boots_pkt_node_push(SCRIPT_NONE, HCI_CMD_PKT, palna, sizeof(palna),
            NULL, NULL);
    return node;
}

//---------------------------------------------------------------------------
#define USAGE_RESET_DETAILS \
    "      EX: # ./boots -c reset"
#define USAGE_INQ_DETAILS \
    "      EX: # ./boots -c inq -t 5"
#define USAGE_DUT_DETAILS \
    "      EX: # ./boots -c dut"
#define USAGE_RADDR_DETAILS \
    "      EX: # ./boots -c raddr"
#define USAGE_WADDR_DETAILS \
    "      NAP-UAP-LAP\n" \
    "      EX: # ./boots -c waddr 0E:8D:76:68:00:01"
#define USAGE_PWRL_DETAILS \
    "      EX: # ./boots -c pwrlvl 7"
#define USAGE_RFTX_PREFACE \
    "RF TX test [-p <XX>] [-c <0-78>] [-t <XX>] [-l <DEC>] [-o <DEC>]"
#define USAGE_RFTX_DETAILS \
    "      [p] Pattern:\n" \
    "          01:0000      02:1111\n" \
    "          03:1010      04:Pseudo Random Bit Sequence 9(Default)\n" \
    "          09:11110000  0A:Tx single tone\n" \
    "      [c] Channel: 0-78, hopping if ignore this\n" \
    "      [t] Packet Type:\n" \
    "          00:NULL  01:POLL  02:FHS   03:DM1   04:DH1   05:HV1   06:HV2   07:HV3\n" \
    "          08:DV    09:AUX   0A:DM3   0B:DH3   0E:DM5   0F:DH5   17:EV3   1C:EV4\n" \
    "          1D:EV5   24:2DH1  28:3DH1  2A:2DH3  2B:3DH3  2E:2DH5  2F:3DH5  36:2EV3\n" \
    "          37:3EV3  3C:2EV5  3D:3EV5\n" \
    "      [l] Length: Decimal value., If ignore this boots will use the default value\n" \
    "      [o] Poll Period: Decimal value. Unit: 1.25ms. If ignore this boots will use the default value\n" \
    "      NOTE: For other parameters command order is Whiten -> RFTX -> Sync -> PowerLvl\n" \
    "      EX: # ./boots -c rft -p A -c 78 -t 3D"
#define USAGE_RFRX_PREFACE \
    "RF RX test start [-p <XX>] [-c <0-78>] [-t <XX>] [-a <XX:XX:XX:XX>]"
#define USAGE_RF_STOP_PREFACE \
    "RF TX/RX test stop(Except SignalTone \"rft -p a ...\", for that need \"reset\" to stop)"
#define USAGE_RF_STOP_DETAILS \
    "      EX: # ./boots -c rf_stop"
#define USAGE_RFRX_STOP_PREFACE \
    "RF RX test stop [-p <XX>] [-c <0-78>] [-t <XX>] [-a <XX:XX:XX:XX>]"
#define USAGE_RFRX_RESULT_PREFACE \
    "RF RX test result [-p <XX>] [-c <0-78>] [-t <XX>] [-a <XX:XX:XX:XX>]"
#define USAGE_RFRX_DETAILS \
    "      [p] Pattern:\n" \
    "          01:0000      02:1111\n" \
    "          03:1010      04:Pseudo Random Bit Sequence 9(Default)\n" \
    "          09:11110000\n" \
    "      [c] Channel: 0-78\n" \
    "      [t] Packet Type:\n" \
    "          03:DM1   04:DH1   0A:DM3   0B:DH3   0E:DM5   0F:DH5\n" \
    "          24:2DH1  28:3DH1  2A:2DH3  2B:3DH3  2E:2DH5  2F:3DH5\n" \
    "      [a] Tester Address: UAP:LAP1:LAP2:LAP3(Default: 0x00A5F0C3 if ignore it)\n"
#define RFR_EX \
    "      EX: # ./boots -c rfr -p 9 -c 78 -t 2F"
#define RFR_S_EX \
    "      EX: # ./boots -c rfr_stop -p 9 -c 78 -t 2F"
#define RFR_R_EX \
    "      EX: # ./boots -c rfr_result -p 9 -c 78 -t 2F"
#define USAGE_BLETX_PREFACE \
    "BLE TX test [-c <0-39>] [-l <0-255>] [-p <0-7>]"
#define USAGE_BLETX_DETAILS \
BLUE"      Can replace this by ble_etx, EX: \"ble_etx -y 1 ...\"\n"NONE \
    "      [c] Channel: 0-39\n" \
    "      [l] Length in bytes of packet: 0-255(DEC, Default is 37 if ignore this)\n" \
    "      [p] Pattern:\n" \
    "          0:PRBS9      1:11110000      2:10101010  3:PRBS15\n" \
    "          4:11111111   5:00000000      6:00001111  7:01010101\n" \
    "      EX: # ./boots -c ble_tx -c 39 -p 2"
#define USAGE_BLERX_DETAILS \
    "      EX: # ./boots -c ble_rx -c 39"
#define USAGE_BLEETX_PREFACE \
    "BLE Enhanced TX test [-c <0-39>] [-l <0-255>] [-p <0-7>] [-y <1-4>]"
#define USAGE_BLEETX_DETAILS \
    "      [c] Channel: 0-39\n" \
    "      [l] Length in bytes of packet: 0-255(DEC)\n" \
    "      [p] Payload of packet:\n" \
    "          0:PRBS9      1:11110000      2:10101010  3:PRBS15\n" \
    "          4:11111111   5:00000000      6:00001111  7:01010101\n" \
    "      [y] Phy:\n" \
    "          1:LE 1M  2:LE 2M  3:LE Coded(S=8)  4:LE Coded(S=2)\n" \
    "          MT7668 only supports LE-1M and LE-2M\n" \
    "      EX: # ./boots -c ble_etx -c 39 -l 255 -p 7 -y 2"
#define USAGE_BLEERX_PREFACE \
    "BLE Enhanced RX test [-c <0-39>] [-y <1-2>] [-m <0-1>]"
#define USAGE_BLEERX_DETAILS \
    "      [c] Channel: 0-39\n" \
    "      [y] Phy: 1:LE 1M   2:LE 2M\n" \
    "      [m] Modulation: 0:Standard 1:Stable\n" \
    "      EX: # ./boots -c ble_erx -c 39 -y 2 -m 0"
#define USAGE_BLE_S_DETAILS \
    "      EX: # ./boots -c ble_stop"
#define USAGE_TXTONE_PREFACE \
    "TX tone test [-t <0-4>] [-c <0-78>] [-m <0/1>] [-d <0-2>] [-p <0/1>]"
#define USAGE_TXTONE_DETAILS \
    "      [t] Tone Type:\n" \
    "          0:SingleTone_DC  1:SingleTone_250K  2:SignleTone_400K\n" \
    "          3:SignleTone_1M  4:ModulationTone\n" \
    "      [c] Channel: 0-78\n" \
    "      The following parameters only for Modulation Tone\n" \
    "      [m] Mode: 0:BT  1:BLE\n" \
    "      [d] Modulation(Only for BT mode): 0:1M  1:2M  2:3M\n" \
    "      [p] Pattern: 0:PRBS9  1:PRBS15\n" \
    "      EX: # ./boots -c txtone -t 4 -c 78 -m 0 -d 0 -p 1"
#define USAGE_RADIOSETTING_PREFACE \
    "Radio setting"RED"(lvl)"NONE" [-b <0~7>] [-e <0~7>] [-m <0~7>] [-s <0~3>] [-o <0/1>]\n" \
    "          Radio setting"RED"(dbm)"NONE" [-b <-29~20>] [-e <-29~20>] [-m <-29~20>] [-n <0/1>] [-o <0/1>]"
#define USAGE_RADIOSETTING_DETAILS \
    "      [b] BT TX power level\n" \
    "      [e] BLE TX power level\n" \
    "      [m] Max TX power level\n" \
    "      [s] use for 1db adjust\n" \
    "      [o] 0 - Default 3db diff, 1 - 0db diff mode to BR/EDR\n" \
    "      EX: # ./boots -c radio -b 7 -e 7 -m 7 -s 3 -o 0\n" \
    RED"\n      for Buzzard below\n" \
    "      [b] BT TX power dbm\n" \
    "      [e] BLE TX power dbm\n" \
    "      [m] Max TX power dbm\n" \
    "      [n] 0 - Default disable Lv9, 1 - enable Lv9\n" \
    "      [o] 0 - Default 3db diff, 1 - 0db diff mode to BR/EDR\n" \
    "      EX: # ./boots -c radio -b 7 -e 7 -m 7 -n 1 -o 0\n"NONE \

#define USAGE_TXPOWEROFFSET_PREFACE \
    "TX power offset\n" \
    "      7662/7668(6 groups)., 7663(16 groups):\n" \
    "      [g16]\n" \
    "      [-g0 <-16~8>] [-g1 <-16~8>] [-g2 <-16~8>] [-g3 <-16~8>] [-g4 <-16~8>] [-g5 <-16~8>] ...\n" \
    "      The following ONLY for 7668\n" \
    "      [-r0 <-2~4>]  [-r1 <-2~4>]  [-r2 <-2~4>] [-r3 <-2~4>]  [-r4 <-2~4>]  [-r5 <-2~4>]\n" \
    "      [-i0 <-6~6> or <-16~8>]  [-i1 <-6~6> or <-16~8>]  [-i2 <-6~6> or <-16~8>]\n" \
    "      [-i3 <-6~6> or <-16~8>]  [-i4 <-6~6> or <-16~8>]  [-i5 <-6~6> or <-16~8>]\n" \
    "      [-e0 <-16~8>] [-e1 <-16~8>] [-e2 <-16~8>] [-e3 <-16~8>] [-e4 <-16~8>] [-e5 <-16~8>]"
#define USAGE_TXPOWEROFFSET_DETAILS \
    "\n      [g16]: Force setting 16 groups, ONLY for 7663\n" \
    "      [g] GroupX: 7662/7668 have 6 groups, 7663 have 16 groups\n" \
    "        7668 group\n" \
    "          Group0: CH0  - 12    Group1: CH13 - 25    Group2: CH26 - 38\n" \
    "          Group3: CH39 - 51    Group4: CH52 - 64    Group5: CH65 - 78\n" \
    "        7663 group\n" \
    "          Group0: CH0  - 3     Group1: CH4  - 8     Group2: CH9  - 13    Group3: CH14 - 18\n" \
    "          Group4: CH19 - 23    Group5: CH24 - 28    Group6: CH29 - 33    Group7: CH34 - 38\n" \
    "          Group8: CH39 - 43    Group9: CH44 - 48    Group10:CH49 - 53    Group11:CH54 - 58\n" \
    "          Group12:CH59 - 63    Group13:CH64 - 68    Group14:CH69 - 73    Group15:CH74 - 78\n" \
    "      The following ONLY for 7668\n" \
    "      [r] RF CR for level 8 GroupX:\n" \
    "      [i] Level 8 GroupX:\n" \
    "      [e] ePA GroupX:\n" \
    "        Group0: CH0  - 12    Group1: CH13 - 25    Group2: CH26 - 38\n" \
    "        Group3: CH39 - 51    Group4: CH52 - 64    Group5: CH65 - 78\n" \
    "      <-16~8>: Unit is +/-0.25db per step and 0 is default value\n" \
    "      Note: set -i only, Boots will use stage 1, set -r and -i together, Boots will use stage 2.\n" \
    "      EX: [7662/7668] # ./boots -c txpwoffset -g0 6 -g1 5 -g2 4 -g3 3 -g4 2 -g5 1\n" \
    "          [7668 only] # ./boots -c txpwoffset -g0 6 ... -r0 4 ... -i0 6 ... -e0 8 ...\n" \
    "          [7663] # ./boots -c txpwoffset -g0 6 ... -g15 8\n" \
    "          [7663] # ./boots -c txpwoffset g16 -g0 6"
#define USAGE_WH_DETAILS \
    "      EX: # ./boots -c whiten 1"
#define USAGE_SYNC_DETAILS \
    "      EX: # ./boots -c sync 11:33:55:77:99:BB:DD:FF"
#define USAGE_EFUSE_WR_PREFACE \
    "Read/Write eFuse [-r/w] [-a <HEX:0-0xFFFF>] [-v <HEX:0-0xFF>]"
#define USAGE_EFUSE_WR_DETAILS \
    "      [w] Write: write efuse value\n" \
    "      [r] Read: read efuse value\n" \
    "      [a] Addr: address of efuse\n" \
    "      [v] Value: value of efuse\n" \
    "      EX: # ./boots -c efuse -r -a 1A5F"
#define USAGE_REGISTER_WR_PREFACE \
    "Read/Write Register [-r/w] [-a <HEX:0-0xFFFFFFFF>] [-v <HEX:0-0xFFFFFFFF>]"
#define USAGE_REGISTER_WR_DETAILS \
    "      [w] Write: write register value\n" \
    "      [r] Read: read register value\n" \
    "      [a] Addr: address of register\n" \
    "      [v] Value: value of register\n" \
    "      EX: # ./boots -c reg -r -a 11111111"
#define USAGE_FILE_WR_PREFACE \
    "Read/Write File [-r/w] [-a <HEX>] [-v <HEX:0-0xFF>] [-f <file_path>]"
#define USAGE_FILE_WR_DETAILS \
    "      [w] Write: write bin_file value\n" \
    "      [r] Read: read bin_file value\n" \
    "      [a] Addr: offset of bin_file\n" \
    "      [v] Value: value of bin_file\n" \
    "      [f] File path: full path and name of bin file\n" \
    "      EX: # ./boots -c file -w -a 0 -v 6 -f boots.bin"
#define USAGE_STT_PREFACE \
    "Stress test using \"Write Local Name HCI Command\" <pkt size 1-248>"
#define USAGE_STT_DETAILS \
    "      EX: # ./boots -c stt 248"
#define USAGE_LBT_PREFACE \
    "Loopback test using \"ACL packet\" <pkt size 1-1021>"
#define USAGE_LBT_DETAILS \
    "      EX: # ./boots -c lbt 255"
#define USAGE_LTT_PREFACE \
    "Loopback test with timer using \"ACL packet\" [-t <ms>] [-s <pkt size>]"
#define USAGE_LTT_DETAILS \
    "      EX: # ./boots -c ltt -t 100 -s 255"
#define USAGE_PA_DETAILS \
    "      EX: # ./boots -c pa 1"
#define USAGE_DUPLEX_DETAILS \
    "      EX: # ./boots -c duplex 1"
#define USAGE_BG_RSSI_PREFACE \
    "Enable/Disable background RSSI scan <0/1> [-o <0/1>] [-s <0-78>] [-e <0-78>] [-a <DEC>] [-f <DEC>] [-n <0/1>]"
#define USAGE_BG_RSSI_DETAILS \
    "      <0/1> 0:disable 1:enable\n" \
    "        1: mt7668 will continue report rssi value for each channels and packages\n" \
    "           Boots will output it to the log depend on the setting parameter.\n" \
    "      [o] Output: output report when disable background rssi, 0:disable 1:enable\n" \
    "      [s] Start channel: 0-78. default:0\n" \
    "      [e] End channel: 0-78. default:78\n" \
    "      [a] Average package(s): calculate average when gatting n rssi channel packages. default:1\n" \
    "      [f] Alpha filter: calculate 1/n alpha filter for each rssi channel packages.\n" \
    "      [n] Report once: display only one report and stop background rssi immediately. 0:disable 1:enable\n" \
    "      EX: # ./boots -c rssi 1 -o 1 -s 0 -e 78 -f 100"
#define USAGE_PK_RSSI_DETAILS \
    "      [a] Handle: handle to get rssi value, if ignore this boots will use the default value: 0x0032\n" \
    "      EX: # ./boots -c pk_rssi"
#define USAGE_BUF_MODE_DETAILS \
    "      EX: # ./boots -c buf_mode -e 1"
#define CMD_BUF_MODE "buf_mode"
#define USAGE_TX_HOPPING_PREFACE \
    "Set hopping 20 channel: low/middle/high\n" \
    "      low:    hopping 2402~2421MHz\n" \
    "      middle: hopping 2432~2451MHz\n" \
    "      high:   hopping 2461~2480MHz"
#define USAGE_TX_HOPPING_DETAILS \
    "      EX: # ./boots -c tx_hopping low"
#define USAGE_SWITCH_BT_PREFACE \
    "Switch BT0/1 <0:BT0/1:BT1>" \
    RED"(for 7921)"NONE
#define USAGE_SWITCH_BT_DETAILS \
    "      EX: # ./boots -c swbt 0"
#define USAGE_POWER_COMPENSATION_PREFACE \
    "Power compensation [-a <0/1>] [-l <0-3>]" \
    RED"(for 7921)\n"NONE \
    "      7921(6 groups):\n" \
    "      [-g0 <-16~8>] [-g1 <-16~8>] [-g2 <-16~8>] [-g3 <-16~8>] [-g4 <-16~8>] [-g5 <-16~8>]"
#define USAGE_POWER_COMPENSATION_DETAILS \
    "      [a] Antenna index: 0: Antenna0 1: Antenna1\n" \
    "      [l] Offset level: 0: LMAX offset   1: LMAX-1 offset\n" \
    "                        2: LMAX-2 offset 3: LMAX-3 offset\n" \
    "      [g] GroupX: 7921 have 6 groups\n" \
    "      EX: # ./boots -c pwcomp -a 0 -l 1 -g0 6 -g1 5 -g2 4 -g3 3 -g4 2 -g5 1"
#define USAGE_POWER_BOUNDARY_PREFACE \
    "Power boundary [-a <0/1>]" \
    RED"(for 7921)\n"NONE \
    "      7921(5 boundaries):\n" \
    "      [-b0 <0~80>] [-b1 <0~79>] [-b2 <0~79>] [-b3 <0~79>] [-b4 <0~79>]"
#define USAGE_POWER_BOUNDARY_DETAILS \
    "      [a] Antenna index: 0: Antenna0, 1: Antenna1\n" \
    "      [b] BoundaryX: 7921 can set 5 boundaries\n" \
    "      EX: # ./boots -c pwbdry -a 0 -b0 12 -b1 21 -b2 45 -b3 60 -b4 79"
#define USAGE_SET_SX_TX_PREFACE \
    "Set SX TX Type" \
    RED"(for 7921)\n"NONE \
    "      BT0/1 according to SWITCH_BT command setting\n" \
    "      BT0 SX Configration: 1: DIV2 for Rx      ,  3: SSB 8/3 for T/Rx\n" \
    "                           4: SSB 16/5 for T/Rx,  5: DIV2 LP for Rx\n" \
    "      BT1 SX Configration: 0: VCDL 5/2 for T/Rx,  1: DIV2 for Rx\n" \
    "                           2: VCDL 7/2 for T/Rx,  5: DIV2 LP for Rx"
#define USAGE_SET_SX_TX_DETAILS \
    "      EX: # ./boots -c sxtx 0"
#define USAGE_SET_SX_RX_PREFACE \
    "Set SX RX Type" \
    RED"(for 7921)\n"NONE \
    "      BT0/1 according to SWITCH_BT command setting\n" \
    "      BT0 SX Configration: 1: DIV2 for Rx      ,  3: SSB 8/3 for T/Rx\n" \
    "                           4: SSB 16/5 for T/Rx,  5: DIV2 LP for Rx\n" \
    "      BT1 SX Configration: 0: VCDL 5/2 for T/Rx,  1: DIV2 for Rx\n" \
    "                           2: VCDL 7/2 for T/Rx,  5: DIV2 LP for Rx"
#define USAGE_SET_SX_RX_DETAILS \
    "      EX: # ./boots -c sxrx 0"
#define USAGE_ANT_TO_PIN_LOSS_PREFACE \
    "Set ANT-to-ChipPin loss 1dB/step, suggested range from  0-5dB" \
    RED"(for 7921)\n"NONE \
    "      <0~19> 0.25dB/step"
#define USAGE_ANT_TO_PIN_LOSS_DETAILS \
    "      EX: # ./boots -c a2ploss 0"
#define USAGE_READ_EDR_INFO_PREFACE \
    "Read EDR infomation" \
    RED"(for 7921)"NONE
#define USAGE_READ_EDR_INFO_DETAILS \
    "      EX: # ./boots -c redrinfo"
#define USAGE_EPA_ELNA_PREFACE \
    "ePA/eLNA 0: disable 1: enable" \
    RED"(for 7921)"NONE
#define USAGE_EPA_ELNA_DETAILS \
    "      [p] ePA: 0: disable, 1: enable\n" \
    "      [l] eLNA: 0: disable, 1: enable\n" \
    "      EX: # ./boots -c palna -p 0 -l 0"
#define USAGE_SWITCH_RX_MODE_PREFACE \
    "Switch RX normal mode/low power mode" \
    RED"(for 7921)\n"NONE \
    "      1: div2 normal mode, 5: div2 low power mode"
#define USAGE_SWITCH_RX_MODE_DETAILS \
    "      EX: # ./boots -c  swrxmode 1"


boots_cmds_s commands[] = {
    { "reset", mtk_boots_cmd_reset, "HCI Reset", USAGE_RESET_DETAILS, false },
    { "inq", mtk_boots_cmd_inquiry, "Inquiry [-t <X * 1.28 sec>]", USAGE_INQ_DETAILS, false },
    { "dut", mtk_boots_cmd_enter_dut, "Enter DUT mode", USAGE_DUT_DETAILS, false },
    { "raddr", mtk_boots_cmd_read_bd_addr, "Read BD address", USAGE_RADDR_DETAILS, false },
    { "waddr", mtk_boots_cmd_write_bd_addr, "Write BD address <0E:8D:76:68:00:01>", USAGE_WADDR_DETAILS, false },
    { "pwrlvl", mtk_boots_cmd_set_power_level, "Set power level(0xFC17) ONLY for RFTX since per link handle <0-7>", USAGE_PWRL_DETAILS, false },
    { "radio", mtk_boots_cmd_radio_setting, USAGE_RADIOSETTING_PREFACE, USAGE_RADIOSETTING_DETAILS, false },
    { "txpwoffset", mtk_boots_cmd_tx_power_offset, USAGE_TXPOWEROFFSET_PREFACE, USAGE_TXPOWEROFFSET_DETAILS, false },
    { "whiten", mtk_boots_cmd_set_whiten, "Set whiten <0:disable/1:enable>", USAGE_WH_DETAILS, false },
    { "sync", mtk_boots_cmd_sync, "Access Code <XX:XX:XX:XX:XX:XX:XX:XX>", USAGE_SYNC_DETAILS, false },
    { "rft", mtk_boots_cmd_rf_tx, USAGE_RFTX_PREFACE, USAGE_RFTX_DETAILS, false },
    { "rfr", mtk_boots_cmd_rf_rx, USAGE_RFRX_PREFACE, USAGE_RFRX_DETAILS RFR_EX, false },
    { "rf_stop", mtk_boots_cmd_rf_stop, USAGE_RF_STOP_PREFACE, USAGE_RF_STOP_DETAILS, false },
    { "rfr_stop", mtk_boots_cmd_rf_rx, USAGE_RFRX_STOP_PREFACE, USAGE_RFRX_DETAILS RFR_S_EX, false },
    { "rfr_result", mtk_boots_cmd_rf_rx, USAGE_RFRX_RESULT_PREFACE, USAGE_RFRX_DETAILS RFR_R_EX, false },
    { "ble_tx", mtk_boots_cmd_ble_tx, USAGE_BLETX_PREFACE, USAGE_BLETX_DETAILS, false },
    { "ble_rx", mtk_boots_cmd_ble_rx, "BLE RX test [-c <0-39>]", USAGE_BLERX_DETAILS, false },
    { "ble_etx", mtk_boots_cmd_ble_etx, USAGE_BLEETX_PREFACE, USAGE_BLEETX_DETAILS, false },
    { "ble_erx", mtk_boots_cmd_ble_erx, USAGE_BLEERX_PREFACE, USAGE_BLEERX_DETAILS, false },
    { "ble_stop", mtk_boots_cmd_ble_test_stop, "Stop BLE test", USAGE_BLE_S_DETAILS, false },
    { "txtone", mtk_boots_cmd_tx_tone, USAGE_TXTONE_PREFACE, USAGE_TXTONE_DETAILS, false },
    { "efuse", mtk_boots_cmd_efuse_wr, USAGE_EFUSE_WR_PREFACE, USAGE_EFUSE_WR_DETAILS, false },
    { "reg", mtk_boots_cmd_register_wr, USAGE_REGISTER_WR_PREFACE, USAGE_REGISTER_WR_DETAILS, true },
    { "file", mtk_boots_cmd_file_wr, USAGE_FILE_WR_PREFACE, USAGE_FILE_WR_DETAILS, true },
    { "stt", mtk_boots_cmd_stress_test, USAGE_STT_PREFACE, USAGE_STT_DETAILS, false },
    { "lbt", mtk_boots_cmd_loop_back_test, USAGE_LBT_PREFACE, USAGE_LBT_DETAILS, false },
    { "ltt", mtk_boots_cmd_loop_timer_test, USAGE_LTT_PREFACE, USAGE_LTT_DETAILS, false },
    { "pa", mtk_boots_cmd_set_pa, "Set ePA or iPA <0:iPA/1:ePA> ONLY for 7668", USAGE_PA_DETAILS, false },
    { "duplex", mtk_boots_cmd_set_ant, "Set TDD mode(ANT1) or FDD mode(ANT2) <0:FDD/1:TDD>", USAGE_DUPLEX_DETAILS, false },
    { "rssi", mtk_boots_cmd_background_rssi_scan, USAGE_BG_RSSI_PREFACE, USAGE_BG_RSSI_DETAILS, false },
    { "pk_rssi", mtk_boots_cmd_packet_rssi, "Get BT packet RSSI [-a <HEX>]", USAGE_PK_RSSI_DETAILS, false },
    { CMD_BUF_MODE, mtk_boots_cmd_set_buffer_mode, "Set buffer mode [-f <file_path>] [-e <0:disable/1:enable>]", USAGE_BUF_MODE_DETAILS, false },
    { "tx_hopping", mtk_boots_cmd_set_tx_hopping, USAGE_TX_HOPPING_PREFACE, USAGE_TX_HOPPING_DETAILS, false },
    { "swbt", mtk_boots_cmd_switch_bt, USAGE_SWITCH_BT_PREFACE, USAGE_SWITCH_BT_DETAILS, false },
    { "pwcomp", mtk_boots_cmd_power_compensation, USAGE_POWER_COMPENSATION_PREFACE, USAGE_POWER_COMPENSATION_DETAILS, false },
    { "pwbdry", mtk_boots_cmd_power_boundary, USAGE_POWER_BOUNDARY_PREFACE, USAGE_POWER_BOUNDARY_DETAILS, false },
    { "sxtx", mtk_boots_cmd_set_sx, USAGE_SET_SX_TX_PREFACE, USAGE_SET_SX_TX_DETAILS, true },
    { "sxrx", mtk_boots_cmd_set_sx, USAGE_SET_SX_RX_PREFACE, USAGE_SET_SX_RX_DETAILS, true },
    { "a2ploss", mtk_boots_cmd_ant_to_pin_loss, USAGE_ANT_TO_PIN_LOSS_PREFACE, USAGE_ANT_TO_PIN_LOSS_DETAILS, false },
    { "redrinfo", mtk_boots_cmd_read_edr_info, USAGE_READ_EDR_INFO_PREFACE, USAGE_READ_EDR_INFO_DETAILS, false },
    { "palna", mtk_boots_cmd_epa_elna, USAGE_EPA_ELNA_PREFACE, USAGE_EPA_ELNA_DETAILS, false },
    { "swrxmode", mtk_boots_cmd_set_sx, USAGE_SWITCH_RX_MODE_PREFACE, USAGE_SWITCH_RX_MODE_DETAILS, true },
    { NULL, NULL, NULL, NULL, NULL }
};

//---------------------------------------------------------------------------
pkt_list_s *boots_raw_cmd_handler(char *raw[], size_t len)
{
    size_t i = 0;
    uint8_t type = 0;
    pkt_list_s *node = NULL;

    if (!raw || len < 4) {
        BPRINT_E("Incorrect input(len: %d)", (int)len);
        return 0;
    }

    if (!memcmp(raw[0], "CMD", strlen(raw[0])) || !memcmp(raw[0], "cmd", strlen(raw[0]))) {
        size_t rawlen = strtol(raw[3], NULL, 16);
        if (len != rawlen + 4) {
            BPRINT_E("Incorrect command length: %d", (int)rawlen);
            return 0;
        }
        type = 0x01;
        i = 1;
    } else if (!memcmp(raw[0], "ACL", strlen(raw[0])) || !memcmp(raw[0], "acl", strlen(raw[0]))) {
        type = 0x02;
        i = 1;
    } else if (!memcmp(raw[0], "SCO", strlen(raw[0])) || !memcmp(raw[0], "sco", strlen(raw[0]))) {
        BPRINT_E("%s: SCO not support yet", __func__);
        return 0;
    } else {
        BPRINT_E("%s: Incorrect argument", __func__);
        return 0;
    }

    node = boots_pkt_node_push(SCRIPT_NONE, type, NULL, len, NULL, NULL);
    if (!node) {
        BPRINT_D("%s: node push fail", __func__);
        return NULL;
    }

    node->u_cnt.data[0] = type;
    while (raw[i] && i < len) {
        node->u_cnt.data[i] = strtol(raw[i], NULL, 16);
        i++;
    }
    node->len = i;
    // TODO check packet length & content
    return node;   // actually raw data length
}

//---------------------------------------------------------------------------
pkt_list_s *boots_cmd_set_handler(char *cmd[], size_t len)
{
    uint8_t i = 0;
    pkt_list_s *list = NULL;

    if (!cmd || !len) return NULL;

    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        if (commands[i].cmd && !memcmp(cmd[0], commands[i].cmd, strlen(cmd[0]))) {
            if ((list = commands[i].func(cmd, len)) == NULL) {
                // ignore this command since no controller operation
                if (memcmp(*cmd, CMD_BUF_MODE, strlen(CMD_BUF_MODE)))
                    BPRINT_I("%s\n%s", commands[i].comment, commands[i].details);
                return NULL;
            } else {
                return list;
            }
        }
    }
    BPRINT_I("%s: Command Not Found(%s)", __func__, cmd[0]);

    return NULL;
}

//---------------------------------------------------------------------------
