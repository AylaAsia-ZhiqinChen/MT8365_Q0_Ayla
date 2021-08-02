/******************************************************************************
 *
 *  Copyright (C) 2016 ST Microelectronics S.A.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

// command-line test client connecting to the ST MTK tools daemon
// to send test commands.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "st_mtktools_msg.h"

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <cutils/properties.h>
#include <errno.h>
#include <nativehelper/scoped_local_ref.h>
#include <nativehelper/scoped_primitive_array.h>
#include <nativehelper/scoped_utf_chars.h>
#include <semaphore.h>

#ifndef VENDOR_VERSION
#define LOG_TAG "ST_FactoryTest"
#else
#define LOG_TAG "ST_FactoryTestVendor"
#endif
#define MYLOG(__level, __format, ...) \
  { printf("[" #__level "] " __format "\n", ##__VA_ARGS__); }

static int send_msg(int cmd, int len, unsigned char *payload);
static int recv_msg(int exp_cmd, int exp_len, unsigned char *rcv_payload);

static int sockfd;

using android::base::StringPrintf;

#define TEST_VERSION 0
#define TEST_ANTENNA 1
#define TEST_SWP_SIM1 2
#define TEST_SWP_eSE1 3
#define TEST_SWP_SIM2 4
//#define TEST_SWP_eSE2
#define TEST_READER 5
#define TEST_CE_SIM1 6
#define TEST_HCE 7

void usage(char *cmd) {
  printf("Usage: %s [testnr]\n", cmd);
  printf(" where testnr is:\n");
  printf("  0 (default): get versions information\n");
  printf("  1: antenna diagnostic\n");
  printf("  2: check SWP with SIM1\n");
  printf("  3: check SWP with eSE\n");
  printf("  4: check SWP with SIM2\n");
  printf("  5: reader mode test -- read a tag within 30 seconds\n");
  printf("  6: CE test -- insert SIM1 and use POS withing 30 seconds\n");
  printf("  7: HCE mode test -- use a POS within 30 seconds\n");
}

int parse_args(int argc, char *argv[]) {
  int ret = 0;
  if (argc == 1) return TEST_VERSION;

  if (argc != 2) {
    usage(argv[0]);
    return -1;
  }

  if (sscanf(argv[1], "%i", &ret) != 1) {
    usage(argv[0]);
    return -1;
  }

  return ret;
}

int main(int argc, char *argv[]) {
  struct sockaddr_un address;

  int testnr;
  if ((testnr = parse_args(argc, argv)) < 0) {
    return -1;
  }

  // open the socket to the server
  sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (sockfd < 0) {
    MYLOG(ERROR, "socket failed: %s", strerror(errno));
    return -1;
  }

  address.sun_family = AF_LOCAL; /* AF_UNIX; */
  strncpy(address.sun_path,
#ifndef VENDOR_VERSION
          MTK_SOCKET_NAME,
#else
          MTK_SOCKET_NAME_FM,
#endif
          sizeof(address.sun_path));

  MYLOG(INFO, "Connecting...");
  if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    pid_t pid = fork();

    if (pid == (pid_t)0) {
      /* Try starting nfcstackp, make it silent */
#ifndef VENDOR_VERSION
      system("/system/bin/nfcstackp > /dev/null 2>&1");
#else
      system("/vendor/bin/nfcstackp-vendor > /dev/null 2>&1");
#endif
      return 0;
    }
    /* wait 1 second */
    usleep(1000000);

    MYLOG(INFO, "Retrying...");
    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      MYLOG(ERROR, "connection failed - %s", strerror(errno));
      return (-1);
    }
  }

  // OK now we can start sending messages

  // initialize the NFC stack on server
  if (0 != send_msg(MTK_NFC_EM_START_CMD, 0, NULL)) {
    goto end;
  }
  // no response.

  switch (testnr) {
    case TEST_VERSION: {
      struct mtk_msg_SW_VERSION_QUERY req;
      struct mtk_msg_SW_VERSION_RESPONSE resp;
      if (0 != send_msg(MTK_NFC_SW_VERSION_QUERY, sizeof(req) - sizeof(req.hdr),
                        NULL)) {
        goto end;
      }
      if (0 != recv_msg(MTK_NFC_SW_VERSION_RESPONSE,
                        sizeof(resp) - sizeof(resp.hdr),
                        ((unsigned char *)(&resp)) + sizeof(resp.hdr))) {
        goto end;
      }
      printf("FW version: %02hhx%02hhx%02hhx%02hhx\n", resp.chip_ver[1],
             resp.chip_ver[0], resp.fw_ver[1], resp.fw_ver[0]);
      printf("HW version: %02hhx%02hhx\n", resp.hw_ver[1], resp.hw_ver[0]);
    } break;

    case TEST_ANTENNA: {
      struct mtk_msg_FM_ANTENNA_TEST_REQ req;
      struct mtk_msg_FM_ANTENNA_TEST_RSP resp;
      req.action_le = le_to_h_32(NFC_EM_ACT_START);
      if (0 != send_msg(MTK_NFC_FM_ANTENNA_TEST_REQ,
                        sizeof(req) - sizeof(req.hdr),
                        (unsigned char *)&req.action_le)) {
        goto end;
      }

      if (0 != recv_msg(MTK_NFC_FM_ANTENNA_TEST_RSP,
                        sizeof(resp) - sizeof(resp.hdr),
                        (unsigned char *)&resp.result_le)) {
        goto end;
      }
      if (resp.result_le != 0) {
        MYLOG(ERROR, "MTK_NFC_FM_ANTENNA_TEST_RSP with error");
        goto end;
      }

      printf("Antenna diagnostic result:\n");
      printf("  Diff Amp : %hu\n", resp.Diff_Amplitude);
      printf("  Diff Pha : %hu\n", resp.Diff_Phase);
      printf("  RFO1 Amp : %hu\n", resp.RFO1_Amplitude);
      printf("  RFO1 Pha : %hu\n", resp.RFO1_Phase);
      printf("  RFO2 Amp : %hu\n", resp.RFO2_Amplitude);
      printf("  RFO2 Pha : %hu\n", resp.RFO2_Phase);
      printf("  VDD_RF   : %hhu\n", resp.VDDRF);
      {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char s[64];
        strftime(s, sizeof(s), "%c", tm);
        printf(
            "Date , Diff_Amplitude , Diff_Phase , RFO1_Amplitude , "
            "RFO1_Phase , RFO2_Amplitude , RFO2_Phase , VDDRF\n");
        printf("%s , %hu , %hu , %hu , %hu , %hu , %hu , %hhu\n", s,
               resp.Diff_Amplitude, resp.Diff_Phase, resp.RFO1_Amplitude,
               resp.RFO1_Phase, resp.RFO2_Amplitude, resp.RFO2_Phase,
               resp.VDDRF);
      }
    } break;

    case TEST_SWP_SIM1:
    case TEST_SWP_eSE1:
    case TEST_SWP_SIM2: {
      struct mtk_msg_FM_SWP_TEST_REQ req;
      struct mtk_msg_FM_SWP_TEST_RSP resp;
      int se_to_test = 0;
      if (testnr == TEST_SWP_SIM1) se_to_test |= 1;
      if (testnr == TEST_SWP_SIM2) se_to_test |= 2;
      if (testnr == TEST_SWP_eSE1) se_to_test |= 4;
      req.action_le = le_to_h_32(NFC_EM_ACT_START);
      req.opt.swionr_le = le_to_h_32(se_to_test);
      if (0 != send_msg(MTK_NFC_FM_SWP_TEST_REQ, sizeof(req) - sizeof(req.hdr),
                        (unsigned char *)(&req.hdr + 1))) {
        goto end;
      }

      if (0 != recv_msg(MTK_NFC_FM_SWP_TEST_RSP,
                        sizeof(resp) - sizeof(resp.hdr),
                        (unsigned char *)(&resp.hdr + 1))) {
        goto end;
      }
      if (resp.result_le != 0) {
        MYLOG(ERROR, "MTK_NFC_FM_SWP_TEST_RSP with error");
        printf("SWP activation failed\n");
        goto end;
      }
      req.action_le = le_to_h_32(NFC_EM_ACT_STOP);
      if (0 != send_msg(MTK_NFC_FM_SWP_TEST_REQ, sizeof(req) - sizeof(req.hdr),
                        (unsigned char *)(&req.hdr + 1))) {
        goto end;
      }

      if (0 != recv_msg(MTK_NFC_FM_SWP_TEST_RSP,
                        sizeof(resp) - sizeof(resp.hdr),
                        (unsigned char *)(&resp.hdr + 1))) {
        goto end;
      }
      printf("SWP activation successful\n");
    } break;

    case TEST_READER: {
      struct mtk_msg_FM_READ_DEP_TEST_REQ req;
      struct mtk_msg_FM_READ_DEP_TEST_RSP resp;
      req.action_le = le_to_h_32(NFC_EM_ACT_START);
      req.type_le = le_to_h_32(EM_ALS_READER_M_TYPE_A);
      req.type_a_rate_le =
          le_to_h_32(EM_ALS_READER_M_SPDRATE_106 | EM_ALS_READER_M_SPDRATE_212 |
                     EM_ALS_READER_M_SPDRATE_424 | EM_ALS_READER_M_SPDRATE_848);

      if (0 != send_msg(MTK_NFC_FM_READ_DEP_TEST_REQ,
                        sizeof(req) - sizeof(req.hdr),
                        (unsigned char *)(&req.hdr + 1))) {
        goto end;
      }
      printf("Reader mode started, please tap your type A card\n");

      if (0 != recv_msg(MTK_NFC_FM_READ_DEP_TEST_RSP,
                        sizeof(resp) - sizeof(resp.hdr),
                        (unsigned char *)(&resp.hdr + 1))) {
        goto end;
      }
      if (resp.result_le != 0) {
        MYLOG(ERROR, "MTK_NFC_FM_READ_DEP_TEST_RSP with error");
        printf("An error was received\n");
        goto end;
      }
      printf("Tag reading successful\n");
    } break;

    case TEST_CE_SIM1: {
      {
        struct mtk_msg_FM_CARD_MODE_TEST_REQ req;
        struct mtk_msg_FM_CARD_MODE_TEST_RSP resp;
        req.action_le = le_to_h_32(NFC_EM_ACT_START);
        req.swionr_le = le_to_h_32(1);
        req.type_le = le_to_h_32(
            EM_ALS_READER_M_TYPE_A);  // actually we are in merge mode
        req.hce_le = le_to_h_32(0);
        if (0 != send_msg(MTK_NFC_FM_CARD_MODE_TEST_REQ,
                          sizeof(req) - sizeof(req.hdr),
                          (unsigned char *)(&req.hdr + 1))) {
          goto end;
        }

        if (0 != recv_msg(MTK_NFC_FM_CARD_MODE_TEST_RSP,
                          sizeof(resp) - sizeof(resp.hdr),
                          (unsigned char *)(&resp.hdr + 1))) {
          goto end;
        }
        if (resp.result_le != 0) {
          LOG(ERROR) << StringPrintf(
              "MTK_NFC_FM_CARD_MODE_TEST_RSP with error");
          printf("Failed to start Card Emulation on SIM1\n");
          goto end;
        }
      }

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("Sleeping for 10 sec");
      printf(
          "Card Emulation on SIM1 is now active, please test (10 secs) ...\n");
      printf("  note: after 1st read, the device will become mute.\n");
      sleep(10);
      printf("Stopping Card Emulation. Did you read UID successfully?\n");

      // Disable CE
      {
        struct mtk_msg_FM_CARD_MODE_TEST_REQ req;
        struct mtk_msg_FM_CARD_MODE_TEST_RSP resp;
        req.action_le = le_to_h_32(NFC_EM_ACT_STOP);
        if (0 != send_msg(MTK_NFC_FM_CARD_MODE_TEST_REQ,
                          sizeof(req) - sizeof(req.hdr),
                          (unsigned char *)(&req.hdr + 1))) {
          goto end;
        }

        if (0 != recv_msg(MTK_NFC_FM_CARD_MODE_TEST_RSP,
                          sizeof(resp) - sizeof(resp.hdr),
                          (unsigned char *)(&resp.hdr + 1))) {
          goto end;
        }
        if (resp.result_le != 0) {
          LOG(ERROR) << StringPrintf(
              "MTK_NFC_FM_CARD_MODE_TEST_RSP with error");
          goto end;
        }
      }
    } break;

    case TEST_HCE: {
      {
        struct mtk_msg_FM_VIRTUAL_CARD_REQ req;
        struct mtk_msg_FM_VIRTUAL_CARD_RSP resp;
        req.action_le = le_to_h_32(NFC_EM_ACT_START);
        req.type_le =
            le_to_h_32(EM_ALS_READER_M_TYPE_A | EM_ALS_READER_M_TYPE_B |
                       EM_ALS_READER_M_TYPE_F);
        req.type_f_rate_le = le_to_h_32(EM_ALS_READER_M_SPDRATE_212 |
                                        EM_ALS_READER_M_SPDRATE_424);
        if (0 != send_msg(MTK_NFC_FM_VIRTUAL_CARD_REQ,
                          sizeof(req) - sizeof(req.hdr),
                          (unsigned char *)(&req.hdr + 1))) {
          goto end;
        }

        if (0 != recv_msg(MTK_NFC_FM_VIRTUAL_CARD_RSP,
                          sizeof(resp) - sizeof(resp.hdr),
                          (unsigned char *)(&resp.hdr + 1))) {
          goto end;
        }
        if (resp.result_le != 0) {
          LOG(ERROR) << StringPrintf("MTK_NFC_FM_VIRTUAL_CARD_RSP with error");
          printf("Failed to start Card Emulation on SIM1\n");
          goto end;
        }
      }

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("Sleeping for 10 sec");
      printf(
          "Virtual Card Emulation (HCE) is now active, please test (10 secs) "
          "...\n");
      printf("  note: after 1st read, the device will become mute.\n");
      sleep(10);
      printf("Stopping Card Emulation\n");

      // Disable CE
      {
        struct mtk_msg_FM_VIRTUAL_CARD_REQ req;
        struct mtk_msg_FM_VIRTUAL_CARD_RSP resp;
        req.action_le = le_to_h_32(NFC_EM_ACT_STOP);
        if (0 != send_msg(MTK_NFC_FM_VIRTUAL_CARD_REQ,
                          sizeof(req) - sizeof(req.hdr),
                          (unsigned char *)(&req.hdr + 1))) {
          goto end;
        }

        if (0 != recv_msg(MTK_NFC_FM_VIRTUAL_CARD_RSP,
                          sizeof(resp) - sizeof(resp.hdr),
                          (unsigned char *)(&resp.hdr + 1))) {
          goto end;
        }
        if (resp.result_le != 0) {
          LOG(ERROR) << StringPrintf("MTK_NFC_EM_ALS_CARD_MODE_RSP with error");
          goto end;
        }
      }
    } break;
  }

  // The end, uninit the NFC stack
  if (0 != send_msg(MTK_NFC_EM_STOP_CMD, 0, NULL)) {
    goto end;
  }
  // no response.

  // Done
end:
  MYLOG(INFO, "Exiting...");
  close(sockfd);

  return 0;
}

static int send_msg(int cmd, int len, unsigned char *payload) {
  struct {
    uint32_t cmd_le;
    uint32_t len_le;
    unsigned char payload[1024];
  } msg;
  unsigned char *p = (unsigned char *)&msg;
  ssize_t ret;
  int offset = 0;

  msg.cmd_le = le_to_h_32(cmd);
  msg.len_le = le_to_h_32(len);
  if (len) {
    memcpy(msg.payload, payload, len);
  }

  // send over the socket
  do {
    ret = write(sockfd, p + offset, sizeof(struct mtk_msg_hdr) + len - offset);
    if (ret <= 0) {
      break;
    }
    offset += ret;
  } while (offset < (int)sizeof(struct mtk_msg_hdr) + len);

  return (ret <= 0) ? -1 : 0;
}

static int recv_msg(int exp_cmd, int exp_len, unsigned char *rcv_payload) {
  struct mtk_msg_hdr msg;
  unsigned char *p = (unsigned char *)&msg;
  ssize_t ret;
  int offset = 0;

  // read header from socket
  do {
    ret = read(sockfd, p + offset, sizeof(struct mtk_msg_hdr) - offset);
    if (ret <= 0) {
      break;
    }
    offset += ret;
  } while (offset < (int)sizeof(struct mtk_msg_hdr));
  if (ret <= 0) {
    return -1;
  }

  if (le_to_h_32(msg.cmd_le) != (uint32_t)exp_cmd) {
    MYLOG(ERROR, "Expected resp %d, got %d", exp_cmd, le_to_h_32(msg.cmd_le));
    return -1;
  }
  if (le_to_h_32(msg.len_le) != (uint32_t)exp_len) {
    MYLOG(ERROR, "Expected resp %d with len %d, got %d", exp_cmd, exp_len,
          le_to_h_32(msg.len_le));
    return -1;
  }
  p = rcv_payload;
  offset = 0;
  do {
    ret = read(sockfd, p + offset, exp_len - offset);
    if (ret <= 0) {
      break;
    }
    offset += ret;
  } while (offset < exp_len);
  return (ret <= 0) ? -1 : 0;
}
