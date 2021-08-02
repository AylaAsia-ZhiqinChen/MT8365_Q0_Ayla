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
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#include "boots.h"
#include "boots_skt.h"
#include "boots_uart.h"
#include "boots_eth.h"
#include "boots_pkt.h"
#include "boots_stress.h"
#include "boots_country.h"
#include "boots_osi.h"

//---------------------------------------------------------------------------
#define LOG_TAG "boots"

//---------------------------------------------------------------------------
static boots_if_s boots_if;
static uint8_t boots_local_exe;
int boots_loop_timer;
rssi_set_s rssi_setting;

//---------------------------------------------------------------------------
static void boots_chk_if(int argc, char **argv)
{
    // Please follow the boots_if_e
    char *c_inf[] = {"None", "stpbt", "hci", "All", "Socket", "UART", "Ethernet",
        "User", "Tester_UART"};
    int i = 0;
#ifdef BOOTS_VERBOSE_MSG
    BPRINT_D("%s: argc = %d", __func__, argc);
    for (i = 0; i < argc; i++)
        BPRINT_D("argv[%d]:%s", i, argv[i]);
    i = 0;
#endif

#if 0
    if (*argv[0] == '-') {
        if (memcmp(argv[0], "-relay", strlen("-relay"))) {
            boots_if.clif = BOOTS_CLIF_USER;
            boots_if.csif = BOOTS_CSIF_SKT;
        } else if (argc >= 2) {
            // boots_srv as relayer in PC/NB
            boots_if.csif = BOOTS_CSIF_UART;
            if (argc == 2)
                snprintf(boots_if.cs, sizeof(boots_if.cs), "%s", argv[1]);
            else if (argc >= 3)
                snprintf(boots_if.cs, sizeof(boots_if.cs), "%s %s", argv[1], argv[2]);
            goto exit;
        }
    } else if (!memcmp(argv[0], "tty", strlen("tty"))) {
        boots_if.csif = BOOTS_CSIF_UART;
        snprintf(boots_if.cs, sizeof(boots_if.cs), "/dev/%s", argv[0]);
    } else if (!memcmp(argv[0], "eth", strlen("eth"))) {
        boots_if.csif = BOOTS_CSIF_ETH;
        memcpy(boots_if.cs, argv[1], strlen(argv[1]));
    }

    if (argc > 2 && !memcmp(argv[1], "-relay", strlen("-relay"))) {
        boots_if.clif = BOOTS_CLIF_UART;
        snprintf(boots_if.cli, sizeof(boots_if.cli), "/dev/%s", argv[2]);
    } else {
        boots_if.clif = BOOTS_CLIF_USER;
    }
#else
    while (i < argc) {
        // check interface for BT side
        if (!memcmp(argv[i], "tty", strlen("tty"))) {
            if (i + 1 < argc) {
                boots_if.csif = BOOTS_CSIF_UART;
                if (memcmp(argv[i], "/dev/", strlen("/dev/")))
                    snprintf(boots_if.cs, sizeof(boots_if.cs), "/dev/%s", argv[i]);
                boots_if.cs_speed = strtol(argv[i + 1], NULL, 10);
                BPRINT_D("cs_speed: %d", boots_if.cs_speed);
                i += 2;
            } else {
                BPRINT_E("Lack a parameter for %s", argv[i]);
                return;
            }
        } else if (!memcmp(argv[i], "eth", strlen("eth"))) {
            boots_if.csif = BOOTS_CSIF_ETH;
            long cp_len = strlen(argv[i + 1]) > sizeof(boots_if.cs) ? sizeof(boots_if.cs) : strlen(argv[i + 1]);
            memcpy(boots_if.cs, argv[i + 1], cp_len);
            i += 2;
        } else if (!memcmp(argv[i], "-relay", strlen("-relay"))) {
            if (boots_if.csif == BOOTS_IF_NONE) {
                // boots_srv as relayer in PC/NB
                if (i + 2 < argc && strtol(argv[i + 2], NULL, 10) != 0) {
                    snprintf(boots_if.cs, sizeof(boots_if.cs), "%s %s", argv[i + 1], argv[i + 2]);
                    i += 2;
                } else if (i + 1 < argc) {
                    snprintf(boots_if.cs, sizeof(boots_if.cs), "%s", argv[i + 1]);
                    i++;
                } else {
                    BPRINT_E("Lack a parameter for %s", argv[i]);
                    return;
                }
                boots_if.csif = BOOTS_CSIF_UART;
            } else {
                // boots CLI
                if (i + 2 < argc && !memcmp(argv[i + 1], "tty", strlen("tty"))
                        && strtol(argv[i + 2], NULL, 10) != 0) {
                    snprintf(boots_if.cli, sizeof(boots_if.cli), "/dev/%s", argv[i + 1]);
                    boots_if.cli_speed = strtol(argv[i + 2], NULL, 10);
                    boots_if.clif = BOOTS_CLIF_UART;
                    i += 2;
                } else {
                    BPRINT_E("Incorrect argument for %s", argv[i]);
                    return;
                }
            }
        } else if (*argv[i] == '-') {
            boots_if.clif = BOOTS_CLIF_USER;
            i++;
        } else {
            i++;
        }
    }

    if (boots_if.csif == BOOTS_IF_NONE) {
        boots_if.csif = BOOTS_CSIF_SKT;
        boots_if.clif = BOOTS_CLIF_USER;
    }
#endif
    BPRINT_D("cs: %s, cs_speed: %d", boots_if.cs, boots_if.cs_speed);
    BPRINT_I("%s(%d) <-> %s(%d) <-> server", c_inf[boots_if.clif],
            boots_if.clif, c_inf[boots_if.csif], boots_if.csif);
}

//---------------------------------------------------------------------------
static char *boots_chk_btif(char *interface)
{
#define BOOTS_SRV_LOCAL "./boots_srv "
#define BOOTS_SRV       "boots_srv "
#define BOOTS_AND       " &"

#define BTPROTO_HCI 1

    int i = 0;
    static char server[64] = {0};

    if (*interface == '-') {    // no assign btif or clif
        // do auto detect
        BPRINT_D("%s: DO auto detect", __func__);
        if (access(boots_btif[0].p, R_OK) == 0) {
            // STPBT
            i = 0;
            boots_if.btif = BOOTS_BTIF_STPBT;
        } else if ((i = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) != -1) {
            // HCI
            BPRINT_D("%s: HCI: %d", __func__, i);
            close(i);
            i = 1;
            boots_if.btif = BOOTS_BTIF_HCI;
        } else {
            BPRINT_E("%s: stpbt/HCI are all not exist", __func__);
            return NULL;
        }
    } else {
        BPRINT_D("%s: btif assigned %s", __func__, interface);
        while (boots_btif[i].inf) {
            if (!strcmp(interface, boots_btif[i].n))
                break;
            i++;
        }
        boots_if.btif = boots_btif[i].inf;
        if (i + 1 == ARRAY_SIZE(boots_btif)) {
            BPRINT_E("%s: boots doesn't support \"%s\"", __func__, interface);
            return NULL;
        }
    }

    if (boots_if.clif == BOOTS_IF_NONE && boots_if.csif == BOOTS_CSIF_UART && strlen(boots_if.cs)) {
        snprintf(server, sizeof(server), boots_local_exe ? BOOTS_SRV_LOCAL "%s %s &" : BOOTS_SRV "%s %s &", boots_if.cs, boots_btif[i].n);
    } else {
        snprintf(server, sizeof(server), boots_local_exe ? BOOTS_SRV_LOCAL "%s &" : BOOTS_SRV "%s &", boots_btif[i].n);
    }
    BPRINT_D("%s: server: %s", __func__, server);
    return server;
}

//---------------------------------------------------------------------------
static int boots_invoke_boots_srv(char *path)
{
    if (path == NULL) return 0;

    // check boots_srv is running or not
    if (osi_system("pidof boots_srv > /dev/null") != 0) {
        if (osi_system(path) == 0) {
            BPRINT_I("Server(%s) executing and wait boots_srv...", path);
            sleep(3);   // waiting boots_srv
        } else {
            BPRINT_E("***** WARNING: Please input # %s, if boots_srv isn't running *****",
                    path);
        }
    }
    return 1;
}

//---------------------------------------------------------------------------
static int boots_killall_boots_srv(void)
{
    int ret = -1;

    ret = osi_system("killall boots_srv > /dev/null");
    if (ret) {
        BPRINT_W("*** Please input #killall boots_srv ***");
        BPRINT_D("%s: kill boots_srv(%d)", __func__, ret);
    }

    return ret;
}

//---------------------------------------------------------------------------
static void boots_sig_handler(int signum)
{
    UNUSED(signum);
    if (BOOTS_STRESS_MEASURE_IN_BOOTS)
        boots_stress_deinit();
    exit(0);
}

//---------------------------------------------------------------------------
static void boots_tell_socket_end(int fd, struct sockaddr_un *addr)
{
    char skend[] = {'s', 'e', 'n', 'd'};

    boots_sk_send(fd, (void *)skend, sizeof(skend), addr, BOOTS_SRVSK_NAME);
}

static void boots_tell_ethernet_end(int fd, struct sockaddr_in *addr)
{
    char skend[] = {'s', 'e', 'n', 'd'};
    addr->sin_family = AF_INET;
    addr->sin_port = ETH_UDP_SRV_PORT;
    addr->sin_addr.s_addr = inet_addr(boots_if.cs);
    boots_eth_send(fd, (void *)skend, sizeof(skend), addr);
}

//---------------------------------------------------------------------------
#define USAGE_DETAILS      (1 << 0)
#define USAGE_DETAILS_MTK  (1 << 1)
static void boots_cmd_usage(uint8_t detail)
{
    int i = 0;
    extern boots_cmds_s commands[];

    printf("Boots - MTK Bluetooth Test Suite ver:%s(rev:%s)\n", VERSION, REV);
    printf("Modular Commands:\n");
    for (i = 0; commands[i].cmd; i++) {
        if (commands[i].hidden == false)
            printf("  "BLUE"%s"NONE" - %s\n", commands[i].cmd, commands[i].comment);
        else if (detail & USAGE_DETAILS_MTK)
            printf("  "BLUE"%s"NONE" - %s\n", commands[i].cmd, commands[i].comment);

        if (detail & USAGE_DETAILS && commands[i].details) {
            if (commands[i].hidden == false)
                printf("%s\n", commands[i].details);
            else if (detail & USAGE_DETAILS_MTK)
                printf("%s\n", commands[i].details);
        }
    }
    printf("For specfic command details use: ./boots -c <CMD> -h\n");
}

//---------------------------------------------------------------------------
static void boots_country_cmd_usage(uint8_t detail)
{
    int i = 0;
    extern boots_country_cmds_s country_commands[];

    printf("Boots - MTK Bluetooth Test Suite ver:%s(rev:%s)\n", VERSION, REV);
    printf("Power Limit Commands:\n");
    for (i = 0; country_commands[i].cmd; i++) {
        if (country_commands[i].hidden == false)
            printf("  "BLUE"%s"NONE" - %s\n", country_commands[i].cmd, country_commands[i].comment);
        else if (detail & USAGE_DETAILS_MTK)
            printf("  "BLUE"%s"NONE" - %s\n", country_commands[i].cmd, country_commands[i].comment);

        if (detail & USAGE_DETAILS && country_commands[i].details) {
            if (country_commands[i].hidden == false)
                printf("%s\n", country_commands[i].details);
            else if (detail & USAGE_DETAILS_MTK)
                printf("%s\n", country_commands[i].details);
        }
    }
    printf("For specfic command details use: ./boots -c <CMD> -h\n");
}

//---------------------------------------------------------------------------
static void boots_help(void)
{
    printf("Boots - MTK Bluetooth Test Suite ver:%s(rev:%s)\n", VERSION, REV);
    printf("Usage:\n");
    printf(BLUE"    boots [BT Interface] <InputMethod/RelayMode> [parameters]\n"NONE);
    printf("      NOTE: DO NOTE reorder\n");
    printf("    BT Interface:\n");
    printf("\tPlease ignore this if not through UART/Ethernet to connect platform(BT)\n");
    printf("\tttyX      Through UART send to BT\n");
    printf("\t              ex: ./boots ttyACM0 115200 ...\n");
    printf("\tethX      Through Ethernet send to BT, need server IP\n");
    printf("\t              ex: ./boots eth0 10.1.1.1 ...\n");
    printf("    InputMethod: [parameters]\n");
    printf("\t-f        File, ex: ./boots -f test.boots\n");
    printf("\t-r        Raw data, ex: ./boots -r CMD 03 0C 00\n");
    printf("\t-c        Command, for more command information on the usage of everyone command use:\n");
    printf("\t              ./boots -c [detail]\n");
    printf("\t-relay    Through serial port to input\n");
    printf("\t              ex: ./boots ttyACM0 115200 -relay ttyUSB0 115200\n");
    printf("\t              ex: ./boots -relay ttyGS2\n");
    printf("\t              ex: ./boots -relay eth0 10.1.1.1\n");
    printf("    Others:\n");
    printf("\tstop      Stop service since boots_srv is running in background\n");
}

//---------------------------------------------------------------------------
static ssize_t boots_read(int fd, void *buf, size_t buf_size, int inf)
{
    ssize_t ret = 0;

    if (!buf || !buf_size || !inf) {
        BPRINT_E("%s: Invalid argument(buf: %p, buf size: %d, inf: %d)",
                __func__, buf, (int)buf_size, inf);
        return -EINVAL;
    } else if (fd < 0) {
        BPRINT_E("%s: Bad file descriptor(%d)", __func__, fd);
        return -EBADFD;
    }

    if (inf == BOOTS_CSIF_SKT) {
        ret = boots_sk_recv(fd, buf, buf_size, 0, NULL, NULL);
    } else if (inf == BOOTS_CSIF_UART || inf == BOOTS_CLIF_UART) {
        ret = boots_uart_read(fd, buf, buf_size);
    } else if (inf == BOOTS_CSIF_ETH) {
        ret = boots_eth_recv(fd, buf, buf_size, 0);
    } else {
        BPRINT_E("%s: Incorrect interface(%d)", __func__, inf);
    }
    return ret;
}

//---------------------------------------------------------------------------
static ssize_t boots_write(int fd, const void *buf, size_t len, int inf)
{
    ssize_t ret = 0;

    if (!buf || !len || !inf) {
        BPRINT_E("%s: Invalid argument(buf: %p, len: %d, inf: %d)", __func__, buf, (int)len, inf);
        return -EINVAL;
    } else if (fd < 0) {
        BPRINT_E("%s: Bad file descriptor(%d)", __func__, fd);
        return -EBADFD;
    }

    if (inf == BOOTS_CSIF_SKT) {
        ret = boots_sk_send(fd, buf, len, NULL, BOOTS_SRVSK_NAME);
    } else if (inf == BOOTS_CSIF_UART || inf == BOOTS_CLIF_UART) {
        ret = boots_uart_write(fd, buf, len);
    } else if (inf == BOOTS_CSIF_ETH) {
        struct sockaddr_in sockaddr;
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = ETH_UDP_SRV_PORT;
        sockaddr.sin_addr.s_addr = inet_addr(boots_if.cs);
        ret = boots_eth_send(fd, buf, len, &sockaddr);
    } else {
        BPRINT_E("%s: Incorrect interface(%d)", __func__, inf);
    }
    return ret;
}

//---------------------------------------------------------------------------
void boots_chk_local_exe(char *argv)
{
    if (argv == NULL) {
        BPRINT_E("%s: argv should not NULL", __func__);
        return;
    }

    if (*argv == '.' && *(argv + 1) == '/') {
        boots_local_exe = 1;
    } else {
        boots_local_exe = 0;
    }
}

//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    static pkt_list_s *pkt = NULL;
    static script_set_s sfile = {NULL, 0, 0, 0, 0};
    static uint8_t buf[HCI_BUF_SIZE] = {0};
    static int cont = 0;
    size_t len = 0;
    ssize_t read_len = 0;
    int csw_fd = -1;
    int csr_fd = -1;
    int cli_uartfd = -1;
    int type = 0;
    struct sockaddr_un skaddr_cli;
    struct sockaddr_un skaddr_srv;
    socklen_t sklen_cli = {0};
    socklen_t sklen_srv = {0};
    struct sockaddr_in ethaddr_cli;
    struct sockaddr_in ethaddr_srv;
    fd_set readfs;
    struct sigaction sigact;
    struct timeval time_start;
    struct timeval time_end;
    uint32_t diff_time;

    // Register signal handler
    sigact.sa_handler = boots_sig_handler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGQUIT, &sigact, NULL);
    sigaction(SIGKILL, &sigact, NULL);

    // For root permission
    CHECK_USERID();

    // Local execute or not
    boots_chk_local_exe(argv[0]);

    if (argc == 2 && !memcmp("-c", argv[1], strlen("-c"))) {
        // Print commands
        boots_cmd_usage(0);
        exit(0);
    } else if (argc == 2 && !memcmp("-o", argv[1], strlen("-o"))) {
        // Print commands
        boots_country_cmd_usage(0);
        exit(0);
    } else if (argc == 3 && !memcmp("-c", argv[1], strlen("-c"))) {
        if (!memcmp("detail", argv[2], strlen("detail"))) {
            boots_cmd_usage(1);
            exit(0);
        } else if (!memcmp("mtk", argv[2], strlen("mtk"))) {
            boots_cmd_usage(3);
            exit(0);
        } else {
            // do nothing for command without any parameters.
        }
    } else if (argc == 2 && !memcmp("stop", argv[1], strlen("stop"))) {
        boots_killall_boots_srv();
        return 0;
    } else if (argc == 3 && !memcmp("-o", argv[1], strlen("-o"))) {
        if (!memcmp("detail", argv[2], strlen("detail"))) {
            boots_country_cmd_usage(1);
            exit(0);
        } else if (!memcmp("mtk", argv[2], strlen("mtk"))) {
            boots_country_cmd_usage(3);
            exit(0);
        } else {
            // do nothing for command without any parameters.
        }
    } else if (argc < 3) {
        // Print help
        boots_help();
        exit(0);
    }

    /** country command set, because it's not related controller operation
     *    Only access file
     */
    if (!memcmp("-o", argv[1], strlen("-o"))) {
        boots_country_set_handler(argv + 2, argc - 2);
        goto exit;
    }

    // Confirm interface between client/server, upper layer
    boots_chk_if(argc - 1, &argv[1]);

    /** Communication Interface with boots_srv */
    if (boots_if.csif == BOOTS_CSIF_SKT) {
        // Check interface is support or not
        if (!boots_invoke_boots_srv(boots_chk_btif(argv[1])))
            exit(1);

        // create socket
        if (boots_sk_create(&csr_fd, &skaddr_cli, &sklen_cli, BOOTS_CLISK_NAME, 0)) {
            BPRINT_E("Create socket failed");
            exit(1);
        }
        // connect socket
        if (boots_sk_create(&csw_fd, &skaddr_srv, &sklen_srv, BOOTS_SRVSK_NAME, 1)) {
            BPRINT_E("Connect socket failed");
            boots_sk_close(&csr_fd);
            exit(1);
        }

    } else if (boots_if.csif == BOOTS_CSIF_UART) {
        if (boots_if.clif == BOOTS_IF_NONE) {
            // Maybe last time user use command ask BT into DUT mode
            boots_killall_boots_srv();
            sleep(1);
            // means boots_srv as relayer in PC/NB
            boots_invoke_boots_srv(boots_chk_btif(argv[1]));
            exit(1);

        } else {
            // client/server communication interface
            csw_fd = boots_uart_init(boots_if.cs, boots_if.cs_speed);
            if (csw_fd < 0) {
                BPRINT_E("%s: Open serial port:%s failed(%d)!", __func__, boots_if.cs, csw_fd);
                exit(1);
            }
            csr_fd = csw_fd;
        }

    } else if (boots_if.csif == BOOTS_CSIF_ETH) {
        // create socket
        if (boots_eth_create(&csr_fd, &ethaddr_cli, 0)) {
            BPRINT_E("Create socket failed");
            exit(1);
        }
        // connect socket
        if (boots_eth_create(&csw_fd, &ethaddr_srv, 1)) {
            BPRINT_E("Connect socket failed");
            boots_eth_close(&csr_fd);
            exit(1);
        }
    } else {
        BPRINT_E("Unknown communication interface");
        exit(1);
    }

    /** Input Interface for boots */
    if (boots_if.clif == BOOTS_CLIF_UART) {
        cli_uartfd = boots_uart_init(boots_if.cli, boots_if.cli_speed);
        if (cli_uartfd < 0) {
            BPRINT_E("Open serial port:%s failed(%d)!!", boots_if.cli, cli_uartfd);
            goto exit;
        }
        cont = 1;

    } else {
        if ((type = getopt(argc, argv, "frco")) != -1) {
            BPRINT_D("optopt: %d, opterr: %d, optind: %d", optopt, opterr, optind);
            switch (type) {
            case 'f':   /** script file */
                sfile.script = boots_script_open(*(argv + optind));
                if (!sfile.script) {
                    goto exit;
                }
                break;
            case 'r':   /** raw data */
                pkt = boots_raw_cmd_handler(argv + optind, argc - optind);
                break;
            case 'c':   /** command set */
                pkt = boots_cmd_set_handler(argv + optind, argc - optind);
                break;
        #if 0 /** process this in above */
            case 'o':
                pkt = boots_country_set_handler(argv + optind, argc - optind);
                break;
        #endif
            default:
                BPRINT_W("Unknown type: %c", type);
                break;
            };
            if (!pkt && !sfile.script) goto exit;
        }
    }

    /** Process handler */
    do {
        int ret = -1;
        struct timeval timo;

        if (sfile.script) {
            pkt = boots_script_get(sfile.script);
            if (pkt == NULL)
                break;  // Could end of file
        }

        // default timeout
        if (sfile.timo) {
            timo.tv_sec = sfile.timo / 1000;
            timo.tv_usec = (sfile.timo % 1000) * 1000;
        } else {
            timo.tv_sec = 3;
            timo.tv_usec = 0;
        }
        if (pkt) {
            BPRINT_D("s_type: %d", pkt->s_type);
            switch (pkt->s_type) {
            case SCRIPT_NONE:       // modular commands
            case SCRIPT_CMD:        // hci_cmd script
            case SCRIPT_STRESS:     // stress test script
            case SCRIPT_LOOPBACK:   // loopback test script
            case SCRIPT_LPTIMER:    // loopback test script with timer
                pkt = boots_pkt_node_pop(pkt, buf, &len);
                break;
            case SCRIPT_TX:         // combo tool script
                pkt = boots_pkt_node_pop(pkt, buf, &len);
                if (len) {
                    // Send command, if input by user or script
                    boots_write(csw_fd, (void *)buf, len, boots_if.csif);
                    boots_pkt_handler(buf, len, NULL);
                    memset(buf, 0, sizeof(buf));
                    len = 0;
                }
                continue;
            case SCRIPT_RX:
            case SCRIPT_WAITRX:
                FD_ZERO(&readfs);
                if (csr_fd >= 0)
                    FD_SET(csr_fd, &readfs);
                ret = select(csr_fd + 1, &readfs, NULL, NULL, &timo);
                if (ret > 0) {
                    read_len = boots_read(csr_fd, buf, sizeof(buf), boots_if.csif);
                    if (read_len > 0) {
                        cont = boots_pkt_handler(buf, (size_t)read_len, pkt ? &pkt : NULL);
                        memset(buf, 0, sizeof(buf));
                        read_len = 0;
                    }
                } else if (ret == 0) {
                    goto exit;
                }
                continue;
            case SCRIPT_TITLE:
            case SCRIPT_PROC:
                BPRINT_I("%s", pkt->u_cnt.msg);     // Just print msg
                pkt = boots_pkt_node_pop(pkt, NULL, NULL);
                continue;
            case SCRIPT_TIMEOUT:
                if (pkt->u_cnt.timo) {
                    sfile.timo = pkt->u_cnt.timo;
                }
                pkt = boots_pkt_node_pop(pkt, NULL, NULL);
                continue;
            case SCRIPT_WAIT:
                if (pkt->u_cnt.wait) {
                    (void)usleep(pkt->u_cnt.wait * 1000);
                }
                pkt = boots_pkt_node_pop(pkt, NULL, NULL);
                continue;
            case SCRIPT_USBALT:
                // TODO
                continue;
            case SCRIPT_LOOP:
                sfile.loop = pkt->u_cnt.loop;
                if (sfile.script)
                    sfile.loop_pos = ftell(sfile.script);
                pkt = boots_pkt_node_pop(pkt, NULL, NULL);
                BPRINT_D("Looping");
                continue;
            case SCRIPT_LOOPEND:
                if (!sfile.loop) {
                    sfile.loop_pos = 0;
                    BPRINT_D("Loop End");
                } else {
                    if ((--sfile.loop) && sfile.script && sfile.loop_pos >= 0)
                        boots_script_loop(sfile.script, sfile.loop_pos);
                }
                pkt = boots_pkt_node_pop(pkt, NULL, NULL);
                continue;
            case SCRIPT_RSSI:    // Background RSSI scan
                if (pkt->u_cnt.rssi_s->stop)
                    boots_pkt_cleanup_report_rssi(1);
                else {
                    boots_pkt_cleanup_report_rssi(0);
                    memcpy(&rssi_setting, pkt->u_cnt.rssi_s, sizeof(rssi_set_s));
                }
                buf[0] = SCRIPT_RSSI;
                memcpy(&buf[1], pkt->u_cnt.rssi_s, sizeof(rssi_set_s));
                len = sizeof(rssi_set_s) + 1;
                boots_write(csw_fd, (void *)buf, len, boots_if.csif);
                memset(buf, 0, sizeof(buf));
                len = 0;
                pkt = boots_pkt_node_pop(pkt, NULL, NULL);
                continue;
            case SCRIPT_END:
                BPRINT_I("Script End");
                pkt = boots_pkt_node_pop(pkt, NULL, NULL);
                boots_script_close(sfile.script);
                sfile.script = NULL;
                goto exit;
            default:
                break;
            }
        }

        if (len) {
            if (pkt && (pkt->s_type == SCRIPT_STRESS || pkt->s_type == SCRIPT_LOOPBACK || pkt->s_type == SCRIPT_LPTIMER)) {
                if (BOOTS_STRESS_MEASURE_IN_BOOTS)
                    boots_stress_record_timestamp(BOOTS_STRESS_TIMESTAMP_SEND_CMD_START);
            }

            // Send command, if input by user or script
            boots_write(csw_fd, (void *)buf, len, boots_if.csif);

            if (pkt && (pkt->s_type == SCRIPT_STRESS || pkt->s_type == SCRIPT_LOOPBACK || pkt->s_type == SCRIPT_LPTIMER)) {
                if (BOOTS_STRESS_MEASURE_IN_BOOTS)
                    boots_stress_record_timestamp(BOOTS_STRESS_TIMESTAMP_SEND_CMD_FINISH);
                if (BOOTS_STRESS_SHOW_ALL_CMD)
                    boots_pkt_handler(buf, len, NULL);
                if (pkt->s_type == SCRIPT_LPTIMER)
                    gettimeofday(&time_start, NULL);
            } else
                boots_pkt_handler(buf, len, NULL);
            memset(buf, 0, sizeof(buf));
            len = 0;
        }

        FD_ZERO(&readfs);

        if (cli_uartfd >= 0)
            FD_SET(cli_uartfd, &readfs);
        if (csr_fd >= 0)
            FD_SET(csr_fd, &readfs);

        ret = select(MAX(cli_uartfd, csr_fd) + 1, &readfs, NULL, NULL, &timo);
        if (ret > 0) {
            if (cli_uartfd >= 0 && FD_ISSET(cli_uartfd, &readfs)) {
                // read from UART(CBT)
                read_len = boots_read(cli_uartfd, buf, sizeof(buf), boots_if.clif);
                if (read_len > 0) {
                    // write to platform
                    boots_write(csw_fd, (void *)buf, (size_t)read_len, boots_if.csif);
                    boots_pkt_handler(buf, (size_t)read_len, NULL);
                    memset(buf, 0, sizeof(buf));
                    read_len = 0;
                }
            }
            if (FD_ISSET(csr_fd, &readfs)) {
                // read from platform
                read_len = boots_read(csr_fd, buf, sizeof(buf), boots_if.csif);

                if (read_len > 0 && boots_if.clif == BOOTS_CLIF_USER) {
                    cont = boots_pkt_handler(buf, read_len, pkt ? &pkt : NULL);
                } else if (read_len > 0 && boots_if.clif == BOOTS_CLIF_UART) {
                    // write to UART(CBT)
                    boots_write(cli_uartfd, (void *)buf, read_len, boots_if.clif);
                    boots_pkt_handler(buf, read_len, NULL);
                }

                if (pkt && pkt->s_type == SCRIPT_STRESS) {
                    if (BOOTS_STRESS_MEASURE_IN_BOOTS)
                        boots_stress_record_timestamp(BOOTS_STRESS_TIMESTAMP_RECEIVE_EVENT_FINISH);
                } else if (pkt && (pkt->s_type == SCRIPT_LOOPBACK || pkt->s_type == SCRIPT_LPTIMER)) {
                    if (BOOTS_STRESS_MEASURE_IN_BOOTS && !BOOTS_STRESS_MEASURE_LBT_TOTAL_LATENCY)
                        boots_stress_record_timestamp(BOOTS_STRESS_TIMESTAMP_RECEIVE_EVENT_FINISH);
                    if (buf[0] == HCI_EVENT_PKT && read_len == 8) {
                        // should read again (Titan:Why read again but not use it?)
                        boots_read(csr_fd, buf, sizeof(buf), boots_if.csif);
                    } else if (buf[0] == HCI_EVENT_PKT && buf[8] == HCI_ACL_PKT && ((size_t)read_len) == (8 + pkt->len)) {
                        // current loopback test iteration is finished
                    } else if (buf[0] == HCI_ACL_PKT) {
                        // current loopback test iteration is finished
                    } else {
                        BPRINT_E("Receive unknonw type %02X, len=%d(EVENT/ACL is expected)", buf[0], (int)read_len);
                        exit(0);
                    }
                    if (BOOTS_STRESS_MEASURE_IN_BOOTS && BOOTS_STRESS_MEASURE_LBT_TOTAL_LATENCY)
                        boots_stress_record_timestamp(BOOTS_STRESS_TIMESTAMP_RECEIVE_EVENT_FINISH);
                    if (pkt->s_type == SCRIPT_LPTIMER) {
                        gettimeofday(&time_end, NULL);
                        diff_time = (time_end.tv_usec + time_end.tv_sec * 1000000) - (time_start.tv_usec + time_start.tv_sec * 1000000);
                        if (boots_loop_timer > 0 && (unsigned int)boots_loop_timer > (diff_time / 1000))
                            usleep(boots_loop_timer*1000 - diff_time);
                    }
                }
                memset(buf, 0, sizeof(buf));
                read_len = 0;
            }
        } else if (ret == 0) {
            // timeout
            if (boots_if.clif == BOOTS_CLIF_UART)
                cont = 1;
            else
                cont = 0;
        } else {
            BPRINT_D("Unexpect return: %d", ret);
        }
    } while (cont || sfile.script);

exit:
    if (boots_if.csif == BOOTS_CSIF_SKT) {
        if (csw_fd >= 0)
            boots_tell_socket_end(csw_fd, &skaddr_srv);
        (void)usleep(1000);
        if (csw_fd >= 0)
            boots_sk_close(&csw_fd);
        if (csr_fd >= 0)
            boots_sk_close(&csr_fd);
    } else if (boots_if.csif == BOOTS_CSIF_UART) {
        if (csw_fd >= 0)
            close(csw_fd);
    } else if (boots_if.csif == BOOTS_CSIF_ETH) {
        if (csw_fd >= 0)
            boots_tell_ethernet_end(csw_fd, &ethaddr_srv);
        (void)usleep(1000);
        if (csw_fd >= 0)
            boots_eth_close(&csw_fd);
        if (csr_fd >= 0)
            boots_eth_close(&csr_fd);
    }
    if (boots_if.clif == BOOTS_CLIF_UART) {
        if (cli_uartfd >= 0)
            close(cli_uartfd);
    }
    boots_pkt_list_destroy(pkt);
    pkt = NULL;
    boots_script_close(sfile.script);
    BPRINT_D("Input End");
    return 0;
}

//---------------------------------------------------------------------------
