/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

/*
 * This is sample code for WLAN test library implementation
 */
#include <stdio.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <net/if.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
//#include <cutils/log.h>
//#include <cutils/misc.h> /* load_file() */
//#include <libnvram.h>
//#include <Custom_NvRam_LID.h>
//#include <CFG_Wifi_File.h>

#include "iwlib.h"
#include "libwifitest.h"

#if CONFIG_SUPPORT_FFT
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <regex.h>

#endif
#define DBG 1
#define ALOGD
#define ALOGE

extern int init_module(void *, unsigned long, const char *);
extern unsigned char uacEEPROMImage[MAX_EEPROM_BUFFER_SIZE];
bool default_channel_power_Flag = true;

#if CONFIG_SUPPORT_FFT
IQ_FORMAT_T rRawData[MAX_ANTENNA_NUM][IQ_NUM][ICAP_SIZE];
double rFreqPwr[MAX_ANTENNA_NUM][FFT_SIZE][FFT_OUT_NUM];

#ifdef CONFIG_YOCTO_EEPROM_PATH
char FFT_RESULT_PATH[] = "/data/misc/wifi/fft";

#else
char FFT_RESULT_PATH[] = "./fft";

#endif
/*recalibration*/
char *RecalType[] = {
    /*Recal*/
	"RC_CAL",               //0x00000001
	"RX_RSSI_DCOC_CAL",     //0x00000002
	"RX_DCOC_CAL",          //0x00000004
	"TX_DPD_RX_FI_FD_MPM",  //0x00000008
	"TX_DPD_SCAN_HPM",  //0x00000010
	"RX_FIIQ_CAL",    //0x00000020
	"RX_FDIQ_CAL",    //0x00000040
	"TX_DPD_LINK",    //0x00000080
	"TSSI_DCOC_CAL",  //0x10000000 //Included on RX DCOC Cal
	"RX_DPD_FI_CAL",  //0x20000000   //Included on TX DPD Cal
	"TX_LPFG",       //0x00000100
	"TX_DCIQC",      //0x00000200
	"TX_IQM",        //0x00000400
	"TX_PGA",        //0x00000800
	"TX_LC_TANK",    //0x00001000
	"RX_FD_FI",      //0x00004000
	"IBF",           //0x00008000
	"TOAE",          //0x00010000
	"TX_FDIQ_CAL",   //0x00020000
	"RX_FIIQ_GAIN",  //0x00040000
	"SX",            //0x00080000
	"POR_CAL_DUMP"  //0x40000000
};

/*Mapping calID to recal type*/
int CalID[] = {
    0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,
	0x00000010,
	0x00000020,
	0x00000040,
	0x00000080,
	0x10000000,//Included on RX DCOC Cal
	0x20000000, //Included on TX DPD Cal
	0x00000100,
	0x00000200,
	0x00000400,
	0x00000800,
	0x00001000,
	0x00004000,
	0x00008000,
	0x00010000,
	0x00020000,
	0x00040000,
	0x00080000,
	0x40000000
};


double rwindow[512] = {
    3.0616e-017, 0.0030859,  0.0062077,  0.0093653,  0.012558,   0.015787,   0.019052,   0.022351,
    0.025686,    0.029056,   0.032461,   0.0359,     0.039375,   0.042884,   0.046427,   0.050004,
    0.053616,    0.057262,   0.060941,   0.064653,   0.0684,     0.072179,   0.075991,   0.079836,
    0.083713,    0.087623,   0.091565,   0.095539,   0.099545,   0.10358,    0.10765,    0.11175,
    0.11588,     0.12004,    0.12423,    0.12845,    0.1327,     0.13698,    0.14129,    0.14563,
    0.14999,     0.15439,    0.15881,    0.16326,    0.16774,    0.17224,    0.17677,    0.18133,
    0.18591,     0.19052,    0.19516,    0.19982,    0.2045,     0.20921,    0.21394,    0.2187,
    0.22348,     0.22828,    0.2331,     0.23795,    0.24282,    0.24771,    0.25262,    0.25755,
    0.2625,      0.26748,    0.27247,    0.27748,    0.2825,     0.28755,    0.29261,    0.29769,
    0.30279,     0.30791,    0.31304,    0.31818,    0.32334,    0.32852,    0.33371,    0.33891,
    0.34413,     0.34935,    0.35459,    0.35985,    0.36511,    0.37039,    0.37567,    0.38097,
    0.38627,     0.39158,    0.39691,    0.40224,    0.40757,    0.41292,    0.41827,    0.42362,
    0.42898,     0.43435,    0.43972,    0.44509,    0.45047,    0.45585,    0.46123,    0.46662,
    0.472,       0.47739,    0.48278,    0.48816,    0.49355,    0.49893,    0.50431,    0.50969,
    0.51506,     0.52044,    0.5258,     0.53117,    0.53653,    0.54188,    0.54722,    0.55256,
    0.55789,     0.56322,    0.56853,    0.57384,    0.57913,    0.58442,    0.5897,     0.59496,
    0.60021,     0.60545,    0.61068,    0.61589,    0.62109,    0.62627,    0.63144,    0.6366,
    0.64173,     0.64685,    0.65196,    0.65704,    0.66211,    0.66716,    0.67219,    0.6772,
    0.68218,     0.68715,    0.69209,    0.69702,    0.70192,    0.70679,    0.71165,    0.71648,
    0.72128,     0.72606,    0.73081,    0.73554,    0.74023,    0.74491,    0.74955,    0.75416,
    0.75875,     0.7633,     0.76783,    0.77233,    0.77679,    0.78122,    0.78562,    0.78999,
    0.79433,     0.79863,    0.8029,     0.80713,    0.81133,    0.81549,    0.81962,    0.82371,
    0.82776,     0.83178,    0.83575,    0.83969,    0.8436,     0.84746,    0.85128,    0.85506,
    0.8588,      0.86251,    0.86617,    0.86978,    0.87336,    0.87689,    0.88039,    0.88383,
    0.88724,     0.8906,     0.89391,    0.89718,    0.90041,    0.90358,    0.90672,    0.9098,
    0.91284,     0.91584,    0.91878,    0.92168,    0.92453,    0.92733,    0.93008,    0.93279,
    0.93544,     0.93804,    0.9406,     0.9431,     0.94555,    0.94796,    0.95031,    0.95261,
    0.95485,     0.95705,    0.95919,    0.96128,    0.96332,    0.96531,    0.96724,    0.96912,
    0.97094,     0.97271,    0.97443,    0.97609,    0.9777,     0.97925,    0.98075,    0.98219,
    0.98358,     0.98491,    0.98619,    0.98741,    0.98858,    0.98969,    0.99074,    0.99174,
    0.99268,     0.99356,    0.99439,    0.99516,    0.99588,    0.99654,    0.99714,    0.99768,
    0.99817,     0.9986,     0.99897,    0.99928,    0.99954,    0.99974,    0.99989,    0.99997,
    1,           0.99997,    0.99989,    0.99974,    0.99954,    0.99928,    0.99897,    0.9986,
    0.99817,     0.99768,    0.99714,    0.99654,    0.99588,    0.99516,    0.99439,    0.99356,
    0.99268,     0.99174,    0.99074,    0.98969,    0.98858,    0.98741,    0.98619,    0.98491,
    0.98358,     0.98219,    0.98075,    0.97925,    0.9777,     0.97609,    0.97443,    0.97271,
    0.97094,     0.96912,    0.96724,    0.96531,    0.96332,    0.96128,    0.95919,    0.95705,
    0.95485,     0.95261,    0.95031,    0.94796,    0.94555,    0.9431,     0.9406,     0.93804,
    0.93544,     0.93279,    0.93008,    0.92733,    0.92453,    0.92168,    0.91878,    0.91584,
    0.91284,     0.9098,     0.90672,    0.90358,    0.90041,    0.89718,    0.89391,    0.8906,
    0.88724,     0.88383,    0.88039,    0.87689,    0.87336,    0.86978,    0.86617,    0.86251,
    0.8588,      0.85506,    0.85128,    0.84746,    0.8436,     0.83969,    0.83575,    0.83178,
    0.82776,     0.82371,    0.81962,    0.81549,    0.81133,    0.80713,    0.8029,     0.79863,
    0.79433,     0.78999,    0.78562,    0.78122,    0.77679,    0.77233,    0.76783,    0.7633,
    0.75875,     0.75416,    0.74955,    0.74491,    0.74023,    0.73554,    0.73081,    0.72606,
    0.72128,     0.71648,    0.71165,    0.70679,    0.70192,    0.69702,    0.69209,    0.68715,
    0.68218,     0.6772,     0.67219,    0.66716,    0.66211,    0.65704,    0.65196,    0.64685,
    0.64173,     0.6366,     0.63144,    0.62627,    0.62109,    0.61589,    0.61068,    0.60545,
    0.60021,     0.59496,    0.5897,     0.58442,    0.57913,    0.57384,    0.56853,    0.56322,
    0.55789,     0.55256,    0.54722,    0.54188,    0.53653,    0.53117,    0.5258,     0.52044,
    0.51506,     0.50969,    0.50431,    0.49893,    0.49355,    0.48816,    0.48278,    0.47739,
    0.472,       0.46662,    0.46123,    0.45585,    0.45047,    0.44509,    0.43972,    0.43435,
    0.42898,     0.42362,    0.41827,    0.41292,    0.40757,    0.40224,    0.39691,    0.39158,
    0.38627,     0.38097,    0.37567,    0.37039,    0.36511,    0.35985,    0.35459,    0.34935,
    0.34413,     0.33891,    0.33371,    0.32852,    0.32334,    0.31818,    0.31304,    0.30791,
    0.30279,     0.29769,    0.29261,    0.28755,    0.2825,     0.27748,    0.27247,    0.26748,
    0.2625,      0.25755,    0.25262,    0.24771,    0.24282,    0.23795,    0.2331,     0.22828,
    0.22348,     0.2187,     0.21394,    0.20921,    0.2045,     0.19982,    0.19516,    0.19052,
    0.18591,     0.18133,    0.17677,    0.17224,    0.16774,    0.16326,    0.15881,    0.15439,
    0.14999,     0.14563,    0.14129,    0.13698,    0.1327,     0.12845,    0.12423,    0.12004,
    0.11588,     0.11175,    0.10765,    0.10358,    0.099545,   0.095539,   0.091565,   0.087623,
    0.083713,    0.079836,   0.075991,   0.072179,   0.0684,     0.064653,   0.060941,   0.057262,
    0.053616,    0.050004,   0.046427,   0.042884,   0.039375,   0.0359,     0.032461,   0.029056,
    0.025686,    0.022351,   0.019052,   0.015787,   0.012558,   0.0093653,  0.0062077,  0.0030859
};
#endif

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */

#ifndef BIT
#define BIT(n)          ((uint32_t) 1 << (n))
#define BITS(m,n)       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

#define MAC_ADDR_LEN                            6

#define IS_BMCAST_MAC_ADDR(_pucDestAddr)            \
        ((bool) ( ((uint8_t *)(_pucDestAddr))[0] & BIT(0) ))

#define EQUAL_MAC_ADDR(_pucDestAddr, _pucSrcAddr)   \
        (!memcmp(_pucDestAddr, _pucSrcAddr, MAC_ADDR_LEN))

/* Debug print format string for the MAC Address */
#define MACSTR      "%02x:%02x:%02x:%02x:%02x:%02x"

/* Debug print argument for the MAC Address */
#define MAC2STR(a)  ((uint8_t *)a)[0], ((uint8_t *)a)[1], ((uint8_t *)a)[2], \
    ((uint8_t *)a)[3], ((uint8_t *)a)[4], ((uint8_t *)a)[5]

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */

#ifndef WIFI_DRIVER_MODULE_PATH
#define WIFI_DRIVER_MODULE_PATH     "/system/lib/modules/wlan.ko"
#endif
#ifndef WIFI_DRIVER_MODULE_NAME
#define WIFI_DRIVER_MODULE_NAME     "wlan"
#endif
#ifndef WIFI_DRIVER_MODULE_ARG
#define WIFI_DRIVER_MODULE_ARG      ""
#endif

#if 0
#ifndef WIFI_IF_NAME
#define WIFI_IF_NAME                "wlan0"
#endif
#endif

char WIFI_IF_NAME[256] = "wlan0";
bool fgDebugMode = false;

static const char DRIVER_PROP_NAME[]    = "wlan.driver.status";
static const char DRIVER_MODULE_NAME[]  = WIFI_DRIVER_MODULE_NAME;
//static const char DRIVER_MODULE_TAG[]   = WIFI_DRIVER_MODULE_NAME " ";
static const char DRIVER_MODULE_TAG[]   = WIFI_DRIVER_MODULE_NAME;
static const char DRIVER_MODULE_PATH[]  = WIFI_DRIVER_MODULE_PATH;
static const char DRIVER_MODULE_ARG[]   = WIFI_DRIVER_MODULE_ARG;
static const char MODULE_FILE[]         = "/proc/modules";

#define IOCTL_SET_STRUCT                (SIOCIWFIRSTPRIV + 8)
#define IOCTL_GET_STRUCT                (SIOCIWFIRSTPRIV + 9)
#define IOCTL_SET_STRUCT_FOR_EM         (SIOCIWFIRSTPRIV + 11)
#define IOCTL_QA_TOOL_DAEMON			(SIOCIWFIRSTPRIV + 16)

#define PRIV_CMD_OID                    15
#if CONFIG_SUPPORT_IWPRIV
#define IOCTL_PRIV_CMD (SIOCDEVPRIVATE + 1)
#endif
/* RF Test specific OIDs */
#define OID_CUSTOM_TEST_MODE                            0xFFA0C901
#define OID_CUSTOM_ABORT_TEST_MODE                      0xFFA0C906
#define OID_CUSTOM_MTK_WIFI_TEST_                       0xFFA0C911
#define OID_CUSTOM_MCR_RW                               0xFFA0c801

/* command mask */
#define TEST_FUNC_IDX_MASK              BITS(0,7)
#define TEST_SET_CMD_OFFSET_MASK        BITS(16,31)
#define TEST_SET_CMD_OFFSET             16

/* RF Test Properties */
#define RF_AT_PARAM_RATE_MCS_MASK   BIT(31)
#define RF_AT_PARAM_RATE_MASK       BITS(0,7)
#define RF_AT_PARAM_RATE_1M         0
#define RF_AT_PARAM_RATE_2M         1
#define RF_AT_PARAM_RATE_5_5M       2
#define RF_AT_PARAM_RATE_11M        3
#define RF_AT_PARAM_RATE_6M         4
#define RF_AT_PARAM_RATE_9M         5
#define RF_AT_PARAM_RATE_12M        6
#define RF_AT_PARAM_RATE_18M        7
#define RF_AT_PARAM_RATE_24M        8
#define RF_AT_PARAM_RATE_36M        9
#define RF_AT_PARAM_RATE_48M        10
#define RF_AT_PARAM_RATE_54M        11

#define NULL_MAC_ADDR               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */
typedef struct _NDIS_TRANSPORT_STRUCT {
    uint32_t    ndisOidCmd;
    uint32_t    inNdisOidlength;
    uint32_t    outNdisOidLength;
    uint8_t     ndisOidContent[16];
} NDIS_TRANSPORT_STRUCT, *P_NDIS_TRANSPORT_STRUCT;

typedef struct _PARAM_MTK_WIFI_TEST_STRUC_T {
    uint32_t u4FuncIndex;
    uint32_t u4FuncData;
} PARAM_MTK_WIFI_TEST_STRUC_T, *P_PARAM_MTK_WIFI_TEST_STRUC_T;

typedef struct _PARAM_CUSTOM_MCR_RW_STRUC_T {
    uint32_t    u4McrOffset;
    uint32_t    u4McrData;
} PARAM_CUSTOM_MCR_RW_STRUC_T, *P_PARAM_CUSTOM_MCR_RW_STRUC_T;

typedef enum _ENUM_RF_AT_BW_T {

    RF_AT_BW_20 = 0,
    RF_AT_BW_40,
    RF_AT_BW_U20,
    RF_AT_BW_L20,
    RF_AT_BW_NUM

} ENUM_RF_AT_BW_T, *P_ENUM_RF_AT_BW_T;

typedef enum _ENUM_RF_AT_RXOK_MATCH_RULE_T {

    RF_AT_RXOK_DISABLED = 0,
    RF_AT_RXOK_MATCH_RA_ONLY,
    RF_AT_RXOK_MATCH_TA_ONLY,
    RF_AT_RXOK_MATCH_RA_TA,
    RF_AT_RXOK_NUM

} ENUM_RF_AT_RXOK_MATCH_RULE_T, *P_ENUM_RF_AT_RXOK_MATCH_RULE_T;

typedef struct _HQA_CMD_FRAME {
	uint32_t MagicNo;
	uint16_t Type;
	uint16_t Id;
	uint16_t Length;
	uint16_t Sequence;
	unsigned char Data[2048];
} __attribute__((packed)) HQA_CMD_FRAME;


/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */
//static int wifi_rfkill_id = -1;
//static char *wifi_rfkill_state_path = NULL;
static int skfd = -1;
static bool fgIsTesting = false;

/*----------------------------------------------------------------------------*/
/*!
 * @brief This function is called to initial rfkill variables
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
#if 0
static int
wifi_init_rfkill(
    void
    )
{
    char path[64];
    char buf[16];
    int fd;
    int sz;
    int id;

    for (id = 0; ; id++) {
        snprintf(path, sizeof(path), "/sys/class/rfkill/rfkill%d/type", id);
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            ALOGW("[%s] open(%s) failed: %s (%d)\n", __func__, path, strerror(errno), errno);
            return -1;
        }
        sz = read(fd, &buf, sizeof(buf));
        close(fd);
        if (sz >= 4 && memcmp(buf, "wlan", 4) == 0) {
            wifi_rfkill_id = id;
            break;
        }
    }

    asprintf(&wifi_rfkill_state_path, "/sys/class/rfkill/rfkill%d/state",
            wifi_rfkill_id);
    return 0;
}
#endif

/*----------------------------------------------------------------------------*/
/*!
 * @brief This function is called to turn on/off Wi-Fi interface
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
#if 0
static int
wifi_set_power(
    int on
    )
{
    int sz;
    int fd = -1;
    int ret = -1;
    const char buffer = (on ? '1' : '0');

    ALOGD("[%s] %d", __func__, on);

    if (wifi_rfkill_id == -1) {
        if (wifi_init_rfkill()) {
            goto out;
        }
    }

    fd = open(wifi_rfkill_state_path, O_WRONLY);
    ALOGD("[%s] %s", __func__, wifi_rfkill_state_path);
    if (fd < 0) {
        ALOGE("open(%s) for write failed: %s (%d)",
                wifi_rfkill_state_path,
                strerror(errno),
                errno);
        goto out;
    }
    sz = write(fd, &buffer, 1);
    if (sz < 0) {
        ALOGE("write(%s) failed: %s (%d)",
                wifi_rfkill_state_path,
                strerror(errno),
                errno);
        goto out;
    }
    ret = 0;

out:
    if (fd >= 0) {
        close(fd);
    }
    return ret;
}
#else
#define WIFI_POWER_PATH                 "/dev/wmtWifi"

int wifi_set_power(int enable) {
    int sz;
    int fd = -1;
    const char buffer = (enable ? '1' : '0');

    fd = open(WIFI_POWER_PATH, O_WRONLY);
    if (fd < 0) {
	//fprintf(stderr, "[%s][%s] Open \"%s\" failed\n", __func__, strerror(errno), WIFI_POWER_PATH);
        goto out;
    }
    sz = write(fd, &buffer, 1);
    if (sz < 0) {
	//fprintf(stderr, "[%s][%s] %s [%c] failed\n", __func__, strerror(errno), WIFI_POWER_PATH, buffer);
        goto out;
    }

out:
    if (fd >= 0) close(fd);
	errno = 0;
    return 0;
}

#endif

/*----------------------------------------------------------------------------*/
/*!
 * @brief This function is to insert module
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
#if 0
static int
insmod(
    const char *filename,
    const char *args
    )
{
    void *module;
    unsigned int size;
    int ret;

    module = load_file(filename, &size);
    if (!module)
        return -1;

    ret = init_module(module, size, args);

    free(module);

    return ret;
}
#endif

#if 0
/*----------------------------------------------------------------------------*/
/*!
 * @brief This function is to check if driver is loaded or not
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
static int
check_driver_loaded(
    void
    )
{
#if 1
	return 1;
#else
    FILE *proc;
    char line[sizeof(DRIVER_MODULE_TAG)+10];

    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        ALOGW("Could not open %s: %s", MODULE_FILE, strerror(errno));
        return 0;
    }

    while ((fgets(line, sizeof(line), proc)) != NULL) {
        if (strncmp(line, DRIVER_MODULE_TAG, strlen(DRIVER_MODULE_TAG)) == 0) {
            fclose(proc);
            return 1;
        }
    }

    fclose(proc);
    return 0;
#endif
}
#endif

/*----------------------------------------------------------------------------*/
/*!
 * @brief This function is to extract interface name
 *
 * @param   name    pointer to name buffer
 *          nsize   size of name buffer
 *          buf     current position in buffer
 * @return
 */
/*----------------------------------------------------------------------------*/
static inline char *
iw_get_ifname(
    char *name,
    int	  nsize,
    char *buf
    )
{
    char *end;

    /* Skip leading spaces */
    while(isspace(*buf))
        buf++;

    end = strrchr(buf, ':');

    /* Not found ??? To big ??? */
    if((end == NULL) || (((end - buf) + 1) > nsize))
        return(NULL);

    /* Copy */
    memcpy(name, buf, (end - buf));
    name[end - buf] = '\0';

    /* Return value currently unused, just make sure it's non-NULL */
    return(end);
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This function is to check whether wlan0 has been spawn or not
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
static int
find_wifi_device(
    void
    )
{
    FILE *fh;
    char  buff[1024];
    int   ret = -1;

    fh = fopen(PROC_NET_DEV, "r");

    if(fh != NULL) {
        /* Success : use data from /proc/net/wireless */
        /* Eat 2 lines of header */
        fgets(buff, sizeof(buff), fh);
        fgets(buff, sizeof(buff), fh);

        /* Read each device line */
        while(fgets(buff, sizeof(buff), fh)) {
            char  name[IFNAMSIZ + 1];
            char *s;

            /* Skip empty or almost empty lines. It seems that in some
             * *        * cases fgets return a line with only a newline. */
            if ((buff[0] == '\0') || (buff[1] == '\0'))
                continue;
            /* Extract interface name */
            s = iw_get_ifname(name, sizeof(name), buff);

            if(s) {
		//fprintf(stderr, "[%s][%s] \n", __func__, strerror(errno));
                if (strcmp(name, WIFI_IF_NAME) == 0 ){
                    ret = 0;
                    break;
                }
            }
        }

        fclose(fh);
    }

    return ret;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief   Wrapper to push some Wireless Parameter in the driver
 *
 * @param   request Wireless extension identifier
 *          pwrq    Pointer to wireless extension request
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
static inline int
ioctl_iw_ext(
    int             request,
    struct iwreq   *pwrq
    )
{
    if(skfd > 0) {
        /* Set device name */
        strncpy(pwrq->ifr_name, WIFI_IF_NAME, (IFNAMSIZ - 1));

        /* Do the request */
        return(ioctl(skfd, request, pwrq));
    }
    else {
        return -1;
    }
}

static int
wifi_device_is_up(
	int sockfd,
	char* if_name
	)
{
	struct ifreq ifr;
	bool ret = false;

	memset((void*)(&ifr), 0, sizeof(ifr));
	strcpy(ifr.ifr_name, if_name);

	ioctl(sockfd, SIOCGIFFLAGS, &ifr);

	if (ifr.ifr_flags & IFF_UP) {
		ret = true;
	}
	return ret;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API is to ask underlying software to enter/leave RF test mode
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
static int
wifi_switch_test_mode(
    int on
    )
{
    int retval;
    struct iwreq wrq;
    NDIS_TRANSPORT_STRUCT rNdisStruct;

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    /* configure NDIS_TRANSPORT_STRUC */
    if(on == 1) {
        rNdisStruct.ndisOidCmd = OID_CUSTOM_TEST_MODE;
    }
    else if(on == 0) {
        rNdisStruct.ndisOidCmd = OID_CUSTOM_ABORT_TEST_MODE;
    }
    else {
        return -1;
    }

    rNdisStruct.inNdisOidlength = 0;
    rNdisStruct.outNdisOidLength = 0;

    /* configure struct iwreq */
    wrq.u.data.pointer = &rNdisStruct;
    wrq.u.data.length = sizeof(NDIS_TRANSPORT_STRUCT);
    wrq.u.data.flags = PRIV_CMD_OID;

    retval = ioctl_iw_ext(IOCTL_SET_STRUCT, &wrq);

    if(retval == 0) {
        if(on == 1) {
            fgIsTesting = true;
        }
        else {
            fgIsTesting = false;
        }
    }

    return retval;
}
#if CONFIG_SUPPORT_IWPRIV
int
WIFI_Driver_priv(
        char *cmdBuff,
        int cmdLen
    )
{
    int result;
    struct iwreq wrq;

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    wrq.u.data.pointer = cmdBuff;
    wrq.u.data.length = cmdLen;

    result = ioctl_iw_ext(IOCTL_PRIV_CMD, &wrq);

    if(result == 0) {
        result = wrq.u.data.length;
        DBGLOG("<ok> Send CMD success return len %d \n",
            result);
    }
    else {
        DBGLOG("<failed> Set CMD fail\n");
    }

    return result;
}

#endif
/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a generic service for RF test set commands
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
int
WIFI_TEST_set(
    uint32_t  u4FuncIndex,
    uint32_t  u4FuncData,
    uint32_t *pu4FuncIndex,
    uint32_t *pu4FuncData
    )
{
    int retval;
    struct iwreq wrq;
    NDIS_TRANSPORT_STRUCT rNdisStruct;
    P_PARAM_MTK_WIFI_TEST_STRUC_T prTestStruct;

    prTestStruct = (P_PARAM_MTK_WIFI_TEST_STRUC_T)rNdisStruct.ndisOidContent;

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    /* configure TEST_STRUCT */
    prTestStruct->u4FuncIndex = u4FuncIndex;
    prTestStruct->u4FuncData = u4FuncData;

    /* configure NDIS_TRANSPORT_STRUC */
    rNdisStruct.ndisOidCmd = OID_CUSTOM_MTK_WIFI_TEST_;
    rNdisStruct.inNdisOidlength = sizeof(PARAM_MTK_WIFI_TEST_STRUC_T);
    rNdisStruct.outNdisOidLength = sizeof(PARAM_MTK_WIFI_TEST_STRUC_T);

    /* configure struct iwreq */
    wrq.u.data.pointer = &rNdisStruct;
    wrq.u.data.length = sizeof(NDIS_TRANSPORT_STRUCT);
    wrq.u.data.flags = PRIV_CMD_OID;

    retval = ioctl_iw_ext(IOCTL_SET_STRUCT, &wrq);

    if(retval == 0) {
        if(pu4FuncIndex) {
            *pu4FuncIndex = prTestStruct->u4FuncIndex;
        }

        if(pu4FuncData) {
            *pu4FuncData = prTestStruct->u4FuncData;
        }

        DBGLOG("<ok> Set CMD[%03u] DATA[0x%08x]\n",
            (u4FuncIndex & TEST_FUNC_IDX_MASK), u4FuncData);
    }
    else {
        DBGLOG("<failed> Set CMD[%03u] DATA[0x%08x]\n",
            (u4FuncIndex & TEST_FUNC_IDX_MASK), u4FuncData);
    }

    return retval;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a generic service for RF test query commands
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
static int
WIFI_TEST_get(
    uint32_t  u4FuncIndex,
    uint32_t  u4FuncData,
    uint32_t *pu4FuncIndex,
    uint32_t *pu4FuncData
    )
{
    int retval;
    struct iwreq wrq;
    NDIS_TRANSPORT_STRUCT rNdisStruct;
    P_PARAM_MTK_WIFI_TEST_STRUC_T prTestStruct;

    prTestStruct = (P_PARAM_MTK_WIFI_TEST_STRUC_T)rNdisStruct.ndisOidContent;

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    /* configure TEST_STRUCT */
    prTestStruct->u4FuncIndex = u4FuncIndex;
    prTestStruct->u4FuncData = u4FuncData;

    /* configure NDIS_TRANSPORT_STRUC */
    rNdisStruct.ndisOidCmd = OID_CUSTOM_MTK_WIFI_TEST_;
    rNdisStruct.inNdisOidlength = sizeof(PARAM_MTK_WIFI_TEST_STRUC_T);
    rNdisStruct.outNdisOidLength = sizeof(PARAM_MTK_WIFI_TEST_STRUC_T);

    /* configure struct iwreq */
    wrq.u.data.pointer = &rNdisStruct;
    wrq.u.data.length = sizeof(NDIS_TRANSPORT_STRUCT);
    wrq.u.data.flags = PRIV_CMD_OID;

    retval = ioctl_iw_ext(IOCTL_GET_STRUCT, &wrq);

    if(retval == 0) {
        if(pu4FuncIndex) {
            *pu4FuncIndex = prTestStruct->u4FuncIndex;
        }

        if(pu4FuncData) {
            *pu4FuncData = prTestStruct->u4FuncData;
        }

        DBGLOG("<ok> Get CMD[%03u] DATA[0x%08x] RESULT[0x%08x]\n",
            (u4FuncIndex & TEST_FUNC_IDX_MASK), u4FuncData, *pu4FuncData);
    }
    else {
        DBGLOG("<failed> Get CMD[%03u] DATA[0x%08x]\n",
            (u4FuncIndex & TEST_FUNC_IDX_MASK), u4FuncData);
    }
    return retval;
}

#define HQA_CMD_MAGIC_NO 0x18142880


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service for efuse write per block line
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
int
WIFI_TEST_set_HQA(
	uint16_t offset,
	uint16_t sequence,
	unsigned char *data
	)
{
	int retval;
	int index;
	uint16_t offset_transfer;
	struct iwreq wrq;
	unsigned char temp=0;

	HQA_CMD_FRAME HqaCmdFrame;

	memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));

	/* zeroize */
	memset(&wrq, 0, sizeof(struct iwreq));

	HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
	HqaCmdFrame.Type = htons(0x0008);
	HqaCmdFrame.Id = htons(0x1308);
	HqaCmdFrame.Length =   htons(20);  //(0x1308)   //20       //2 + 16;
	//HqaCmdFrame.Sequence = htons(sequence);

	for (index=0; index<EFUSE_BYTES_PER_LINE; index++)
		HqaCmdFrame.Data[index+4] = data[index];

	for (index=0; index<EFUSE_BYTES_PER_LINE ; index+=2)
	{
		temp = HqaCmdFrame.Data[index+4];
		HqaCmdFrame.Data[index+4] = HqaCmdFrame.Data[index+4+1];
		HqaCmdFrame.Data[index+4+1] = temp;
	}

	offset_transfer = htons(offset);
	HqaCmdFrame.Data[3] = EFUSE_BYTES_PER_LINE;
	memcpy(HqaCmdFrame.Data , &offset_transfer, 2);

#if 0
    {
        printf("HQA:  offset=%4x, %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %x\n", offset,
         HqaCmdFrame.Data[0]  ,  HqaCmdFrame.Data[1],  HqaCmdFrame.Data[2],   HqaCmdFrame.Data[3],
         HqaCmdFrame.Data[4],  HqaCmdFrame.Data[5],  HqaCmdFrame.Data[6],   HqaCmdFrame.Data[7],
         HqaCmdFrame.Data[8],  HqaCmdFrame.Data[9],  HqaCmdFrame.Data[10], HqaCmdFrame.Data[11],
         HqaCmdFrame.Data[12], HqaCmdFrame.Data[13],  HqaCmdFrame.Data[14], HqaCmdFrame.Data[15], HqaCmdFrame.Data[16]);

        printf("Data: offset=%4x, %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x\n", offset,
         uacEEPROMImage[offset]  ,  uacEEPROMImage[offset+1],  uacEEPROMImage[offset+2],  uacEEPROMImage[offset+3],
         uacEEPROMImage[offset+4],  uacEEPROMImage[offset+5],  uacEEPROMImage[offset+6],  uacEEPROMImage[offset+7],
         uacEEPROMImage[offset+8],  uacEEPROMImage[offset+9],  uacEEPROMImage[offset+10], uacEEPROMImage[offset+11],
         uacEEPROMImage[offset+12], uacEEPROMImage[offset+13], uacEEPROMImage[offset+14], uacEEPROMImage[offset+15]);

    }
#endif
	/* configure struct iwreq */
	//wrq.u.data.pointer = HqaCmdFrame;
	wrq.u.data.pointer = &HqaCmdFrame;
	wrq.u.data.length = sizeof(HQA_CMD_FRAME); //0x20;
	wrq.u.data.flags = 0;//0;


	retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);

	return retval;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service for efuse read per block line
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
int
WIFI_TEST_get_HQA(
	uint16_t offset,
	uint16_t sequence,
	unsigned char *data
	)
{
	int retval;
	int index;
	uint16_t offset_transfer;
	struct iwreq wrq;

	HQA_CMD_FRAME HqaCmdFrame;

	memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));


	/* zeroize */
	memset(&wrq, 0, sizeof(struct iwreq));

	HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
	HqaCmdFrame.Type = htons(0x0008);
	HqaCmdFrame.Id = htons(0x1307);		// HQA_ReadBulkEEPROM
	HqaCmdFrame.Length =   htons(20);  //(0x1308)   //20       //2 + 16;
	//HqaCmdFrame.Sequence = htons(sequence);

	offset_transfer = htons(offset);
	HqaCmdFrame.Data[3] = EFUSE_BYTES_PER_LINE;
	memcpy(HqaCmdFrame.Data , &offset_transfer, 2);

	/* configure struct iwreq */
	wrq.u.data.pointer = &HqaCmdFrame;
	wrq.u.data.length = sizeof(HQA_CMD_FRAME); //0x20;
	wrq.u.data.flags = 0;//0;

	retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);

	memcpy(data, HqaCmdFrame.Data+2, EFUSE_BYTES_PER_LINE);

	for (index=0; index<EFUSE_BYTES_PER_LINE ; index+=2)
	{
		/* XOR swap algo.*/
		data[index]   = data[index]   ^ data[index+1];
		data[index+1] = data[index]   ^ data[index+1];
		data[index]   = data[index+1] ^ data[index];
	}

	return retval;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service for set EEPROM/efuse mode
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
int
WIFI_TEST_set_Eeprom_Mode(
      unsigned char data
    )
{
    int retval;
    struct iwreq wrq;

    HQA_CMD_FRAME HqaCmdFrame;

    memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
    HqaCmdFrame.Type = htons(0x0008);
    HqaCmdFrame.Id = htons(0x1316);
    HqaCmdFrame.Length =   htons(20);  //(0x1308)   //20       //2 + 16;
    //HqaCmdFrame.Sequence = htons(sequence);


    HqaCmdFrame.Data[3] = data;

    /* configure struct iwreq */
    //wrq.u.data.pointer = HqaCmdFrame;
    wrq.u.data.pointer = &HqaCmdFrame;
    wrq.u.data.length = sizeof(HQA_CMD_FRAME); //0x20;
    wrq.u.data.flags = 0;//0;

    retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);

    return retval;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service for sending buffer mode CMD
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
int
WIFI_TEST_set_Efuse_Buffer_Mode(
      unsigned char data
    )
{
    int retval;
    struct iwreq wrq;

    HQA_CMD_FRAME HqaCmdFrame;

    memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
    HqaCmdFrame.Type = htons(0x0008);
    HqaCmdFrame.Id = htons(0x1511);
    HqaCmdFrame.Length =   htons(20);  //(0x1308)   //20       //2 + 16;
    //HqaCmdFrame.Sequence = htons(sequence);

    HqaCmdFrame.Data[3] = data;

    /* configure struct iwreq */
    //wrq.u.data.pointer = HqaCmdFrame;
    wrq.u.data.pointer = &HqaCmdFrame;
    wrq.u.data.length = sizeof(HQA_CMD_FRAME); //0x20;
    wrq.u.data.flags = 0;//0;

    retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);

    return retval;
}


/* API
 * ========================================================================== */
#if 1
bool WIFI_TEST_OpenDUT(void)
{
    int count = 60;
    bool retval = false;

	//fprintf(stderr, "[%s][%s] entry\n", __func__, strerror(errno));

    wifi_set_power(1);

    sched_yield();

    while(count -- > 0) {
        if(find_wifi_device()==0) {
            retval = true;
            	//ALOGD("[%s] find wifi device\n", __func__);
            	//fprintf(stderr, "[%s][%s] find wifi device\n", __func__, strerror(errno));
		break;
        }
        usleep(100*1000);
    }

    if (retval == false) {
    	return false;
    }
    else {
        /* initialize skfd */
        if (skfd < 0 && (skfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "[%s][%s] failed to open net socket\n", __func__, strerror(errno));
		errno = 0;
		return false;
        }
    }

    if (wifi_device_is_up(skfd, WIFI_IF_NAME) == false) {
		fprintf(stderr, "[%s][%s] %s is down\n", __func__, strerror(errno), WIFI_IF_NAME);
		errno = 0;
		return false;
    }

    /* switch into test mode */
    if(wifi_switch_test_mode(1) != 0) {
        goto error;
    }

    return true;

error:
	fprintf(stderr, "[%s][%s] failure\n", __func__, strerror(errno));
	errno = 0;
	WIFI_TEST_CloseDUT();

	return false;
}
#endif
bool WIFI_TEST_CloseDUT(void)
{
    /* turn off test mode */
    wifi_switch_test_mode(0);

    /* close socket if necessary */
    if(skfd > 0) {
        close(skfd);
        skfd = -1;
    }

    /* no need to remove module, just turn off host power via rfkill */
    wifi_set_power(0);

    return true;
}

bool WIFI_TEST_TxDataRate(int TxDataRate)
{
    int retval;

	switch (TxDataRate) {
	case WIFI_TEST_RATE_AUTO:
        return false;   //@FIXME

	case WIFI_TEST_RATE_1MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_1M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_2MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_2M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_5_5MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_5_5M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_6MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_6M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_9MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_9M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_11MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_11M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_12MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_12M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_18MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_18M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_24MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_24M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_36MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_36M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_48MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_48M,
                NULL,
                NULL);
        break;

	case WIFI_TEST_RATE_54MBPS:
        retval = WIFI_TEST_set(RF_AT_FUNCID_RATE,
                RF_AT_PARAM_RATE_54M,
                NULL,
                NULL);
        break;

	default:
        return false;

	}

    /* return value checking */
    if(retval == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetPreamble(WIFI_PreambleType_t PreambleType)
{
	int retval;

	switch (PreambleType) {
	case WIFI_TEST_PREAMBLE_LONG:
        retval = WIFI_TEST_set(RF_AT_FUNCID_PREAMBLE,
                RF_AT_PREAMBLE_NORMAL,
                NULL,
                NULL);
		break;

	case WIFI_TEST_PREAMBLE_SHORT:
        retval = WIFI_TEST_set(RF_AT_FUNCID_PREAMBLE,
                RF_AT_PREAMBLE_CCK_SHORT,
                NULL,
                NULL);
		break;

	default:
        return false;
	}

    /* return value checking */
    if(retval == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_Channel(int ChannelNo)
{
    uint32_t u4Freq;

    if(ChannelNo < 0) {
        return false; /* invalid channel number */
    }
    /* 2.4GHz band */
    else if(ChannelNo <= 13) {
        u4Freq = 2412000 + (ChannelNo - 1) * 5000;
    }
    else if(ChannelNo == 14) {
        u4Freq = 2484000;
    }
    /* 5GHz band */
    else if(ChannelNo >= 36) {
        u4Freq = 5180000 + (ChannelNo - 36) * 5000;
    }
    else {
        return false; /* invalid channel number */
    }

    if(WIFI_TEST_set(RF_AT_FUNCID_CHNL_FREQ,
            u4Freq,
            NULL,
            NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_TxGain(float TxGain)
{
    int iTxGain = TxGain * 2;
    /* assign TX power gain */
    if (WIFI_TEST_set(RF_AT_FUNCID_POWER,
            iTxGain, // in unit of 0.5dBm
            NULL,
            NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_ABSTxGain(float TxGain)
{
    int iTxGain = TxGain * 2;
    /* assign TX power gain */
    if (WIFI_TEST_set(RF_AT_FUNCID_SET_ABSPOWER,
            iTxGain, // in unit of 0.5dBm
            NULL,
            NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_TxBurstInterval(int SIFS)
{
	if (SIFS < 20 || SIFS > 1000)
		return false;

    /* specify packet interval */
    if(WIFI_TEST_set(RF_AT_FUNCID_PKTINTERVAL,
            SIFS,
            NULL,
            NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_TxPayloadLength(int TxPayLength)
{
	if (TxPayLength <= 0)
		return false;

    /* specify packet length */
    if(WIFI_TEST_set(RF_AT_FUNCID_PKTLEN,
            TxPayLength,
            NULL,
            NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_TxBurstFrames(int Frames)
{
	if (Frames < 0)
		return false;

    /* specify packet count */
    if(WIFI_TEST_set(RF_AT_FUNCID_PKTCNT,
                Frames,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_TxDestAddress(unsigned char *addr)
{
    uint8_t aucMacAddr[4];
    uint32_t u4MacAddr;

    /* specify MAC address[0:3] */
    memcpy(aucMacAddr, addr, sizeof(uint8_t) * 4);
    u4MacAddr = *(uint32_t *)(&(aucMacAddr[0]));
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_DST_ADDRESS,
                u4MacAddr,
                NULL,
                NULL) != 0) {
        return false;
    }

    /* specify MAC address[4:5] */
    memset(aucMacAddr, 0, sizeof(uint8_t) * 4);
    memcpy(aucMacAddr, addr + 4, sizeof(uint8_t) * 2);
    u4MacAddr = *(uint32_t *)(&(aucMacAddr[0]));
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_DST_ADDRESS | (4 << TEST_SET_CMD_OFFSET),
                u4MacAddr,
                NULL,
                NULL) != 0) {
        return false;
    }

    return true;
}

bool WIFI_TEST_TxStart(void)
{
	/* tx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_STARTTX,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_TxStop(void)
{
    /* tx stop */
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_STOPTEST,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}


bool WIFI_TEST_SetRxPath(uint32_t u4Path)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_RX_PATH,
                u4Path,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetTxPath(uint32_t u4Path)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_TX_PATH,
                u4Path,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetDBDC(uint32_t u4Dbdc)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_DBDC_ENABLE,
                u4Dbdc,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}


bool WIFI_TEST_SetDBDCBand(uint32_t u4Dbdcband)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_DBDC_BAND_IDX,
                u4Dbdcband,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetRECAL(uint32_t u4CalMode)
{
    if (WIFI_TEST_set(RF_AT_FUNCID_SET_RECAL_CAL_STEP,
                   u4CalMode,
                   NULL,
                   NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetRECAL_Dump_Control(uint32_t u4CalDump)
{
    if (WIFI_TEST_set(RF_AT_FUNCID_SET_CAL_DUMP_CONTROL,
                   u4CalDump,
                   NULL,
                   NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}


bool WIFI_TEST_SetCBW(uint32_t u4Cbw)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_CHANNEL_BANDWIDTH,
                u4Cbw,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetDBW(uint32_t u4Dbw)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_DATA_BANDWIDTH,
                u4Dbw,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetPrimaryCh(uint32_t u4Pch)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_PRI_SETTING,
                u4Pch,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}


bool WIFI_TEST_SetBand(uint32_t u4Band)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_BAND,
                u4Band,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetCbw(uint32_t u4Cbw)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_CHANNEL_BANDWIDTH,
                u4Cbw,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetDbw(uint32_t u4Dbw)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_DATA_BANDWIDTH,
                u4Dbw,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetPriCh(uint32_t u4PriCh)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_PRI_SETTING,
                u4PriCh,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}




bool WIFI_TEST_SetMacHeader(uint32_t u4MacHeader)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_HEADER,
                u4MacHeader,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetPayLoad(uint32_t u4Payload)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_PAYLOAD,
                u4Payload,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}


bool WIFI_TEST_SetTA(uint32_t u4TA)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_SRC_ADDRESS,
                u4TA,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetStbc(uint32_t u4Stbc)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_STBC,
                u4Stbc,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetIbf(uint32_t u4Ibf)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_IBF_ENABLE,
                u4Ibf,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetEbf(uint32_t u4Ebf)
{
    /* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_EBF_ENABLE,
                u4Ebf,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}




bool WIFI_TEST_SetNss(uint32_t u4Nss)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_NSS,
                u4Nss,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}


bool WIFI_TEST_SetFreqOffset(uint32_t u4FreqOffset)
{
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_FRWQ_OFFSET,
                u4FreqOffset,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetRateOffset()
{
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_RATEOFFSET,
                0,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}


bool WIFI_TEST_SetTxPowerCompensation(uint32_t u4TxPowerCompensation)
{
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_CH_TX_PWR_OFFSET,
                u4TxPowerCompensation,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}


bool WIFI_TEST_GetTxPowerCompensationEepromAddr(unsigned int offset, unsigned int *val)
{
    if (!val) {
        return false;
    }

    if (WIFI_TEST_get(RF_AT_FUNCID_GET_EEPROM_ADDR,
                offset,
                NULL,
                (uint32_t *)val) != 0) {
        return false;
    }
    return true;
}

bool WIFI_TEST_GetCalFreqOffsetEepromAddr(unsigned int offset, unsigned int *val)
{
    if (!val) {
        return false;
    }

    if (WIFI_TEST_get(RF_AT_FUNCID_GET_FREQ_OFFSET_EEPROM_ADDR,
                offset,
                NULL,
                (uint32_t *)val) != 0) {
        return false;
    }
    return true;
}
bool WIFI_TEST_GetCalIsolationValue(uint32_t *val)
{
    if (!val) {
            return false;
    }
    if (WIFI_TEST_get(RF_AT_FUNCID_GET_AUTO_ISOLATION_VALUE,
                0,
                NULL,
                (uint32_t *)val) != 0) {
        return false;
    }
    return true;
}

bool WIFI_TEST_RxStart(void)
{
	/* rx start: without ack, async mode */
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_STARTRX,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_AutoIso(void)
{
	/* AutoIso Start */
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_AUTOISO,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_RxStop(void)
{
	/* rx stop */
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_STOPTEST,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_FRError(int *FError)
{
	if (!FError)
		return false;

    if(WIFI_TEST_get(RF_AT_FUNCID_RXERROR_COUNT,
                0,
                NULL,
                (uint32_t *)FError) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_FRGood(int *FRGood)
{
	if (!FRGood)
		return false;

    if(WIFI_TEST_get(RF_AT_FUNCID_RXOK_COUNT,
                0,
                NULL,
                (uint32_t *)FRGood) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_TxCount(uint32_t *TxCount)
{
	if (!TxCount)
		return false;

    if(WIFI_TEST_get(RF_AT_FUNCID_TXED_COUNT,
                0,
                NULL,
                (uint32_t *)TxCount) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_TxGoodCount(uint32_t *TxGoodCount)
{
	if (!TxGoodCount)
		return false;

    if(WIFI_TEST_get(RF_AT_FUNCID_TXOK_COUNT,
                0,
                NULL,
                (uint32_t *)TxGoodCount) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_RSSI(int *RSSI)
{
    uint32_t u4Result;

	if (!RSSI)
		return false;

    if(WIFI_TEST_get(RF_AT_FUNCID_RX_RSSI,
                0,
                NULL,
                &u4Result) == 0) {
        *RSSI = (int)u4Result;
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_IsRunning(void)
{
	return fgIsTesting;
}

bool WIFI_TEST_IsUp(void)
{
    struct ifreq ifr;
    int sk, up;

    /* NULL ptr checking */
    sk = socket(PF_INET, SOCK_STREAM, 0);
    if (sk == -1) {
        return false;
    }

    strncpy(ifr.ifr_name, WIFI_IF_NAME, sizeof(ifr.ifr_name)-1);
    ifr.ifr_name[sizeof(ifr.ifr_name)-1]='\0';

    /* retrieve hardware address */
    if (ioctl(sk, SIOCGIFFLAGS, &ifr) == -1) {
        close(sk);
        return false;
    }

    up = ifr.ifr_flags & IFF_UP;

    close(sk);

    if(up) {
        return true;
    }
    else {
        return false;
    }
}

/* for 11n test */
// FrameFormat        : 1 (Mixed Mode), 2 (Green field mode)
// GI (Gard interval) : 1 (Long GI), 2 (Short GI)
bool WIFI_TEST_TxDataRate11n(
    ENUM_WIFI_TEST_MCS_RATE eTxDataRate11n,
    ENUM_WIFI_TEST_PREAMBLE_TYPE ePreambleType,
    ENUM_WIFI_TEST_GI_TYPE eGIType
    )
{
    uint32_t u4Rate, u4Preamble;

	if( (eTxDataRate11n > WIFI_TEST_MCS_RATE_15) && (eTxDataRate11n != WIFI_TEST_MCS_RATE_32) )
	{
        DBGLOG("<failed> Unsupported MCS rate index[%u]\n", eTxDataRate11n);
		return false;
	}

	if( (ePreambleType != 0) && (ePreambleType != 1) )
	{
	    DBGLOG("<failed> Unsupported N mode preamable type[%u]\n", ePreambleType);
		return false;
	}

	if( (eGIType != 0) && (eGIType != 1) )
	{
		return false;
	}

    /* specify 11n rate */
    u4Rate = RF_AT_PARAM_RATE_MCS_MASK | (uint32_t)(eTxDataRate11n);
    if(WIFI_TEST_set(RF_AT_FUNCID_RATE,
                u4Rate,
                NULL,
                NULL) != 0) {
        return false;
    }

    /* specify preamble type */
    switch(ePreambleType) {
    case WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE:
        u4Preamble = RF_AT_PREAMBLE_11N_MM;
        break;

    case WIFI_TEST_PREAMBLE_TYPE_GREENFIELD:
        u4Preamble = RF_AT_PREAMBLE_11N_GF;
        break;

    default:
        return false;
    }

    if(WIFI_TEST_set(RF_AT_FUNCID_PREAMBLE,
                u4Preamble,
                NULL,
                NULL) != 0) {
        return false;
    }

    /* specify Guard Interval type */
    if(WIFI_TEST_set(RF_AT_FUNCID_GI,
                eGIType,
                NULL,
                NULL) != 0) {
        return false;
    }

	return true;
}


/* for 11ac test */
// GI (Gard interval) : 1 (Long GI), 2 (Short GI)
bool WIFI_TEST_TxDataRate11ac(
    ENUM_WIFI_TEST_MCS_RATE eTxMcsRate,
    ENUM_WIFI_TEST_GI_TYPE eGIType
    )
{
    uint32_t u4Rate;

	//if(eTxMcsRate > WIFI_TEST_MCS_RATE_9)
	if( (eTxMcsRate > WIFI_TEST_MCS_RATE_15) && (eTxMcsRate != WIFI_TEST_MCS_RATE_32) )
	{
		return false;
	}

	if( (eGIType != 0) && (eGIType != 1) )
	{
		return false;
	}

    /* specify MCS rate */
    u4Rate = RF_AT_PARAM_RATE_MCS_MASK | (uint32_t)(eTxMcsRate);
    if(WIFI_TEST_set(RF_AT_FUNCID_RATE,
                u4Rate,
                NULL,
                NULL) != 0) {
        return false;
    }

    if(WIFI_TEST_set(RF_AT_FUNCID_PREAMBLE,
                RF_AT_PREAMBLE_11AC,
                NULL,
                NULL) != 0) {
        return false;
    }

    /* specify Guard Interval type */
    if(WIFI_TEST_set(RF_AT_FUNCID_GI,
                eGIType,
                NULL,
                NULL) != 0) {
        return false;
    }

	return true;
}

/* for 11n test */
bool WIFI_TEST_FrequencyAccuracy(int ChannelNo)
{
    /* set channel */
    if(WIFI_TEST_Channel(ChannelNo) == false) {
        return false;
    }

    /* start carrier tone */
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_LO_LEAKAGE,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

/* for 11n test */
bool WIFI_TEST_FrequencyAccuracy_Stop()
{

	/* stop */
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_STOPTEST,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

/* Single Tone , Add by Jared.Huang 2017/09/28 */
bool WIFI_TEST_Single_Tone() {

    /* Start Single Tone */
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_SINGLE_TONE,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

/* Set Single Tone Type */
bool WIFI_TEST_Single_Tone_Type(uint32_t u4ToneType) {

    if(WIFI_TEST_set(RF_AT_FUNCID_SET_TONE_TYPE,
                    u4ToneType,
                    NULL,
                    NULL) == 0) {
            return true;
    }
    else {
           return false;
    }
}

/* Set Single Tone RF Gain */
bool WIFI_TEST_Single_Tone_RF_Gain(uint32_t u4ToneRfGain) {

    if(WIFI_TEST_set(RF_AT_FUNCID_SET_TONE_RF_GAIN,
                     u4ToneRfGain,
                     NULL,
                     NULL) == 0) {
            return true;
    }
    else {
            return false;
    }

}

/* Set Single Tone RF Gain */
bool WIFI_TEST_Single_Tone_Digital_Gain(uint32_t u4ToneDigitalGain) {

    if(WIFI_TEST_set(RF_AT_FUNCID_SET_TONE_DIGITAL_GAIN,
                     u4ToneDigitalGain,
                     NULL,
                     NULL) == 0) {
            return true;
    }
    else {
            return false;
    }

}

/* Set Single Tone Frequence */
bool WIFI_TEST_Single_Tone_Freq(uint32_t u4ToneFreq) {

    if(WIFI_TEST_set(RF_AT_FUNCID_SET_TONE_BW,
                    u4ToneFreq,
                    NULL,
                    NULL) == 0) {
            return true;
    }
    else {
           return false;
    }
}

/* Set Single Tone DC Offset */
bool WIFI_TEST_Single_Tone_DC_Offset(uint16_t u2ToneDcOffsetI, uint16_t u2ToneDcOffsetQ) {

	uint32_t u4FuncData = 0;
	u4FuncData = u2ToneDcOffsetI | (u2ToneDcOffsetQ << 16);

	if(WIFI_TEST_set(RF_AT_FUNCID_SET_TONE_DC_OFFSET, u4FuncData, NULL, NULL) == 0) {
		return true;
	}
	else {
		return false;
	}
}

/* Set TSSI mode */
bool WIFI_TEST_SET_TSSI(uint32_t u4TssiEnable) {

	if(WIFI_TEST_set(RF_AT_FUNCID_SET_TSSI, u4TssiEnable, NULL, NULL) == 0) {
		return true;
	}
	else {
		return false;
	}
}

/* Set DPD mode */
bool WIFI_TEST_SET_DPD(uint32_t u4DPDEnable) {

	if(WIFI_TEST_set(RF_AT_FUNCID_DPD_MODE, u4DPDEnable, NULL, NULL) == 0) {
		printf("(%s) DPD Mode\n", u4DPDEnable ? "Enable" : "Disable");
		return true;
	}
	else {
		printf("Fail to Set DPD Mode\n");
		return false;
	}
}

/*******************************************************************************
 * STYLE #B Control API
 *******************************************************************************
 */
/* static variables for Style #B Control API */
static ENUM_WIFI_TEST_MODE eWifiTestMode = WIFI_TEST_MODE_BY_API_CONTROL;
static ENUM_WIFI_BANDWIDTH eWifiBw = WIFI_TEST_BW_20MHZ;
static uint32_t u4NumGoodFrames = 0;
static uint32_t u4NumBadFrames = 0;

bool WIFI_TEST_SetMode(ENUM_WIFI_TEST_MODE eMode)
{
    if(eMode >= WIFI_TEST_MODE_NUM) {
        return false;
    }
    else {
        eWifiTestMode = eMode;
        return true;
    }
}


bool WIFI_TEST_SetBandwidth(ENUM_WIFI_BANDWIDTH eBandwidth)
{
    int result = 0;
    ENUM_WIFI_CHANNEL_BANDWIDTH eChBw;

    if(eBandwidth >= WIFI_TEST_BW_NUM) {
        return false;
    }

    /* Legacy/11n BW */
    if(eBandwidth < WIFI_TEST_BW_80MHZ) {
        result = WIFI_TEST_set(RF_AT_FUNCID_BANDWIDTH, (uint32_t)eBandwidth,
                        NULL, NULL);
    } else {
        switch(eBandwidth) {
        case WIFI_TEST_BW_80MHZ:
            eChBw = WIFI_TEST_CH_BW_80MHZ;
            break;

        case WIFI_TEST_BW_160MHZ:
            eChBw = WIFI_TEST_CH_BW_160MHZ;
            break;

        default:
            return false;
        }

        result = WIFI_TEST_set(RF_AT_FUNCID_SET_CHANNEL_BANDWIDTH,
                (uint32_t)eChBw, NULL, NULL);
    }

    if(result == 0) {
        eWifiBw = eBandwidth;
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetBandwidthV2(ENUM_WIFI_CHANNEL_BANDWIDTH eBandwidth)
{
    int result = 0;

    if(eBandwidth >= WIFI_TEST_CH_BW_NUM) {
        return false;
    }

    result = WIFI_TEST_set(RF_AT_FUNCID_SET_CHANNEL_BANDWIDTH,
            (uint32_t)eBandwidth, NULL, NULL);

    return (result == 0)? true:false;
}

bool WIFI_TEST_SetPriChannelSetting(uint8_t ucPrimaryChSetting)
{
    int result = 0;

    if(ucPrimaryChSetting >= WIFI_TEST_PRI_CH_SETTING_NUM) {
        return false;
    }

    result = WIFI_TEST_set(RF_AT_FUNCID_SET_PRI_SETTING,
            (uint32_t)ucPrimaryChSetting, NULL, NULL);

    return (result == 0)? true:false;
}

bool WIFI_TEST_SetTxBandwidth(ENUM_WIFI_CHANNEL_BANDWIDTH eBandwidth)
{
    int result = 0;

    if(eBandwidth >= WIFI_TEST_CH_BW_NUM) {
        return false;
    }

    result = WIFI_TEST_set(RF_AT_FUNCID_SET_DATA_BANDWIDTH,
            (uint32_t)eBandwidth, NULL, NULL);

    return (result == 0)? true:false;
}

bool WIFI_TEST_SetFrequency(uint32_t u4FreqInMHz, uint32_t u4OffsetInKHz)
{
    if(u4OffsetInKHz != 0) {
        /* not supporting offset frequency */
        return false;
    }
    else {
        switch(eWifiTestMode) {
        case WIFI_TEST_MODE_80211B_ONLY:
        case WIFI_TEST_MODE_80211G_ONLY:
            /* check for channel #1 ~ channel #14 */
            switch(u4FreqInMHz) {
            case 2412:
                return WIFI_TEST_Channel(1);
            case 2417:
                return WIFI_TEST_Channel(2);
            case 2422:
                return WIFI_TEST_Channel(3);
            case 2427:
                return WIFI_TEST_Channel(4);
            case 2432:
                return WIFI_TEST_Channel(5);
            case 2437:
                return WIFI_TEST_Channel(6);
            case 2442:
                return WIFI_TEST_Channel(7);
            case 2447:
                return WIFI_TEST_Channel(8);
            case 2452:
                return WIFI_TEST_Channel(9);
            case 2457:
                return WIFI_TEST_Channel(10);
            case 2462:
                return WIFI_TEST_Channel(11);
            case 2467:
                return WIFI_TEST_Channel(12);
            case 2472:
                return WIFI_TEST_Channel(13);
            case 2484:
                return WIFI_TEST_Channel(14);
            default:
                break;
            }
            break;

        case WIFI_TEST_MODE_80211A_ONLY:
            switch(u4FreqInMHz) {
            case 4915:
                return WIFI_TEST_Channel(183);
            case 4920:
                return WIFI_TEST_Channel(184);
            case 4925:
                return WIFI_TEST_Channel(185);
            case 4935:
                return WIFI_TEST_Channel(187);
            case 4940:
                return WIFI_TEST_Channel(188);
            case 4945:
                return WIFI_TEST_Channel(189);
            case 4960:
                return WIFI_TEST_Channel(192);
            case 4980:
                return WIFI_TEST_Channel(196);
            case 5170:
                return WIFI_TEST_Channel(34);
            case 5180:
                return WIFI_TEST_Channel(36);
            case 5190:
                return WIFI_TEST_Channel(38);
            case 5200:
                return WIFI_TEST_Channel(40);
            case 5210:
                return WIFI_TEST_Channel(42);
            case 5220:
                return WIFI_TEST_Channel(44);
            case 5230:
                return WIFI_TEST_Channel(46);
            case 5240:
                return WIFI_TEST_Channel(48);
            case 5260:
                return WIFI_TEST_Channel(52);
            case 5280:
                return WIFI_TEST_Channel(56);
            case 5300:
                return WIFI_TEST_Channel(60);
            case 5320:
                return WIFI_TEST_Channel(64);
            case 5500:
                return WIFI_TEST_Channel(100);
            case 5520:
                return WIFI_TEST_Channel(104);
            case 5540:
                return WIFI_TEST_Channel(108);
            case 5560:
                return WIFI_TEST_Channel(112);
            case 5580:
                return WIFI_TEST_Channel(116);
            case 5600:
                return WIFI_TEST_Channel(120);
            case 5620:
                return WIFI_TEST_Channel(124);
            case 5640:
                return WIFI_TEST_Channel(128);
            case 5660:
                return WIFI_TEST_Channel(132);
            case 5680:
                return WIFI_TEST_Channel(136);
            case 5700:
                return WIFI_TEST_Channel(140);
            case 5745:
                return WIFI_TEST_Channel(149);
            case 5765:
                return WIFI_TEST_Channel(153);
            case 5785:
                return WIFI_TEST_Channel(157);
            case 5805:
                return WIFI_TEST_Channel(161);
            case 5825:
                return WIFI_TEST_Channel(165);
            default:
                break;
            }
            break;

        case WIFI_TEST_MODE_80211N_ONLY:
        case WIFI_TEST_MODE_80211AC_ONLY:
            switch(u4FreqInMHz) {
            case 2412:
                return WIFI_TEST_Channel(1);
            case 2417:
                return WIFI_TEST_Channel(2);
            case 2422:
                return WIFI_TEST_Channel(3);
            case 2427:
                return WIFI_TEST_Channel(4);
            case 2432:
                return WIFI_TEST_Channel(5);
            case 2437:
                return WIFI_TEST_Channel(6);
            case 2442:
                return WIFI_TEST_Channel(7);
            case 2447:
                return WIFI_TEST_Channel(8);
            case 2452:
                return WIFI_TEST_Channel(9);
            case 2457:
                return WIFI_TEST_Channel(10);
            case 2462:
                return WIFI_TEST_Channel(11);
            case 2467:
                return WIFI_TEST_Channel(12);
            case 2472:
                return WIFI_TEST_Channel(13);
            case 2484:
                return WIFI_TEST_Channel(14);
            case 4915:
                return WIFI_TEST_Channel(183);
            case 4920:
                return WIFI_TEST_Channel(184);
            case 4925:
                return WIFI_TEST_Channel(185);
            case 4935:
                return WIFI_TEST_Channel(187);
            case 4940:
                return WIFI_TEST_Channel(188);
            case 4945:
                return WIFI_TEST_Channel(189);
            case 4960:
                return WIFI_TEST_Channel(192);
            case 4980:
                return WIFI_TEST_Channel(196);
            case 5170:
                return WIFI_TEST_Channel(34);
            case 5180:
                return WIFI_TEST_Channel(36);
            case 5190:
                return WIFI_TEST_Channel(38);
            case 5200:
                return WIFI_TEST_Channel(40);
            case 5210:
                return WIFI_TEST_Channel(42);
            case 5220:
                return WIFI_TEST_Channel(44);
            case 5230:
                return WIFI_TEST_Channel(46);
            case 5240:
                return WIFI_TEST_Channel(48);
            case 5260:
                return WIFI_TEST_Channel(52);
            case 5280:
                return WIFI_TEST_Channel(56);
            case 5300:
                return WIFI_TEST_Channel(60);
            case 5320:
                return WIFI_TEST_Channel(64);
            case 5500:
                return WIFI_TEST_Channel(100);
            case 5520:
                return WIFI_TEST_Channel(104);
            case 5540:
                return WIFI_TEST_Channel(108);
            case 5560:
                return WIFI_TEST_Channel(112);
            case 5580:
                return WIFI_TEST_Channel(116);
            case 5600:
                return WIFI_TEST_Channel(120);
            case 5620:
                return WIFI_TEST_Channel(124);
            case 5640:
                return WIFI_TEST_Channel(128);
            case 5660:
                return WIFI_TEST_Channel(132);
            case 5680:
                return WIFI_TEST_Channel(136);
            case 5700:
                return WIFI_TEST_Channel(140);
            case 5745:
                return WIFI_TEST_Channel(149);
            case 5765:
                return WIFI_TEST_Channel(153);
            case 5785:
                return WIFI_TEST_Channel(157);
            case 5805:
                return WIFI_TEST_Channel(161);
            case 5825:
                return WIFI_TEST_Channel(165);
            default:
                break;
            }
            break;

        default:
            break;
        }

        return false;
    }
}

bool WIFI_TEST_SetRate(uint32_t u4Rate)
{
    ENUM_WIFI_TEST_GI_TYPE eGIType;
    uint32_t u4RateCode;
	//fprintf(stderr, "[%s][%s] Rate = %d\n", __func__, strerror(errno), u4Rate);


    /* validate rate by mode/bandwidth settings */
    if(eWifiTestMode == WIFI_TEST_MODE_80211B_ONLY) {
        switch(u4Rate) {
        case 100:
            u4RateCode = RF_AT_PARAM_RATE_1M;
            break;

        case 200:
            u4RateCode = RF_AT_PARAM_RATE_2M;
            break;

        case 550:
            u4RateCode = RF_AT_PARAM_RATE_5_5M;
            break;

        case 1100:
            u4RateCode = RF_AT_PARAM_RATE_11M;
            break;

        default:
            return false;
        }
    }
    else if(eWifiTestMode == WIFI_TEST_MODE_80211G_ONLY || eWifiTestMode == WIFI_TEST_MODE_80211A_ONLY) {
        switch(u4Rate) {
        case 600:
            u4RateCode = RF_AT_PARAM_RATE_6M;
            break;

        case 900:
            u4RateCode = RF_AT_PARAM_RATE_9M;
            break;

        case 1200:
            u4RateCode = RF_AT_PARAM_RATE_12M;
            break;

        case 1800:
            u4RateCode = RF_AT_PARAM_RATE_18M;
            break;

        case 2400:
            u4RateCode = RF_AT_PARAM_RATE_24M;
            break;

        case 3600:
            u4RateCode = RF_AT_PARAM_RATE_36M;
            break;

        case 4800:
            u4RateCode = RF_AT_PARAM_RATE_48M;
            break;

        case 5400:
            u4RateCode = RF_AT_PARAM_RATE_54M;
            break;

        default:
            return false;
        }
    }
    else if(eWifiTestMode == WIFI_TEST_MODE_80211N_ONLY && eWifiBw == WIFI_TEST_BW_20MHZ) {
        switch(u4Rate) {
        case 650:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_0;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 720:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_0;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 1300:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_1;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 1440:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_1;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 1950:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_2;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 2170:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_2;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 2600:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_3;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 2890:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_3;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 3900:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_4;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 4330:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_4;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 5200:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_5;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 5780:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_5;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 5850:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_6;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 6500:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_7;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 7200:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_7;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        default:
            return false;
        }
    }
    else if(eWifiTestMode == WIFI_TEST_MODE_80211N_ONLY && eWifiBw == WIFI_TEST_BW_40MHZ) {
        switch(u4Rate) {
        case 1350:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_0;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 1500:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_0;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 2700:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_1;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 3000:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_1;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 4050:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_2;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 4500:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_2;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 5400:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_3;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 6000:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_3;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 8100:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_4;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 9000:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_4;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 10800:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_5;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 12000:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_5;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        case 12150:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_6;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 13500:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_7;
            eGIType = WIFI_TEST_GI_TYPE_NORMAL_GI;
            break;
        case 15000:
            u4RateCode = RF_AT_PARAM_RATE_MCS_MASK | WIFI_TEST_MCS_RATE_7;
            eGIType = WIFI_TEST_GI_TYPE_SHORT_GI;
            break;
        default:
            return false;
        }
    }
    else {
        return false;
    }

    /* set GI type */
    if(eWifiTestMode == WIFI_TEST_MODE_80211N_ONLY &&
            WIFI_TEST_set(RF_AT_FUNCID_GI,
                eGIType,
                NULL,
                NULL) != 0) {
        return false;
    }

    /* set rate code */
    if(WIFI_TEST_set(RF_AT_FUNCID_RATE,
                u4RateCode,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_SetTXPower(uint32_t u4Gain)
{
    if(u4Gain > 6350) {
        return false;
    }
    else if(WIFI_TEST_set(RF_AT_FUNCID_POWER,
            u4Gain / 50, // convert to unit of 0.5dBm
            NULL,
            NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}
bool WIFI_TEST_SetTX(bool fgEnable)
{
    if(fgEnable == true) {
        return WIFI_TEST_TxStart();
    }
    else {
        return WIFI_TEST_TxStop();
    }
}

bool WIFI_TEST_Set_Auto_Ack(char *aucAddr)
{
    uint8_t aucMacAddr[4];
    uint32_t u4MacAddr;
    ENUM_RF_AT_RXOK_MATCH_RULE_T eRxRule;

    eRxRule = RF_AT_RXOK_DISABLED;

    /* specify policy */
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_RXOK_MATCH_RULE,
                eRxRule,
                NULL,
                NULL) != 0) {
        return false;
    }

    if(aucAddr) {
        /* Must reset MAC_DST_ADDRESS in Auto ack*/
        /* specify MAC address[0:3] */
        u4MacAddr = 0;
        if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_DST_ADDRESS,
                    u4MacAddr,
                    NULL,
                    NULL) != 0) {
            return false;
        }

        /* specify MAC address[4:5] */
        u4MacAddr = 0;
        if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_DST_ADDRESS | (4 << TEST_SET_CMD_OFFSET),
                    u4MacAddr,
                    NULL,
                    NULL) != 0) {
            return false;
        }

        /* Match this address will auto-reply ack by hardware */
        /* specify MAC address[0:3] */
        memcpy(aucMacAddr, aucAddr, sizeof(uint8_t) * 4);
        u4MacAddr = *(uint32_t *)(&(aucMacAddr[0]));

        if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_SRC_ADDRESS,
                    u4MacAddr,
                    NULL,
                    NULL) != 0) {
            return false;
        }

        /* specify MAC address[4:5] */
        memset(aucMacAddr, 0, sizeof(uint8_t) * 4);
        memcpy(aucMacAddr, &(aucAddr[4]), sizeof(uint8_t) * 2);
        u4MacAddr = *(uint32_t *)(&(aucMacAddr[0]));

        if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_SRC_ADDRESS | (4 << TEST_SET_CMD_OFFSET),
                    u4MacAddr,
                    NULL,
                    NULL) != 0) {
            return false;
        }
    }
    return true;
}

bool WIFI_TEST_SetRX(bool fgEnable, char *aucSrcAddr, char *aucDstAddr)
{
	uint8_t aucMacAddr[4];
	uint32_t u4MacAddr;
	ENUM_RF_AT_RXOK_MATCH_RULE_T eRxRule;

	if(fgEnable == true) {
		if(aucSrcAddr && aucDstAddr) {
		    eRxRule = RF_AT_RXOK_DISABLED;
		}
		else if(aucDstAddr) {
		    eRxRule = RF_AT_RXOK_MATCH_RA_ONLY;
		}
		else if(aucSrcAddr) {
		    eRxRule = RF_AT_RXOK_MATCH_TA_ONLY;
		}
		else {
		    eRxRule = RF_AT_RXOK_DISABLED;
		}

		/* specify policy */
		if(WIFI_TEST_set(RF_AT_FUNCID_SET_RXOK_MATCH_RULE,
		            eRxRule,
		            NULL,
		            NULL) != 0) {
		    return false;
		}

		if(aucDstAddr) {
		    /* specify MAC address[0:3] */
		    memcpy(aucMacAddr, aucDstAddr, sizeof(uint8_t) * 4);
		    u4MacAddr = *(uint32_t *)(&(aucMacAddr[0]));
		    if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_DST_ADDRESS,
		                u4MacAddr,
		                NULL,
		                NULL) != 0) {
		        return false;
		    }

		    /* specify MAC address[4:5] */
		    memset(aucMacAddr, 0, sizeof(uint8_t) * 4);
		    memcpy(aucMacAddr, &(aucDstAddr[4]), sizeof(uint8_t) * 2);
		    u4MacAddr = *(uint32_t *)(&(aucMacAddr[0]));
		    if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_DST_ADDRESS | (4 << TEST_SET_CMD_OFFSET),
		                u4MacAddr,
		                NULL,
		                NULL) != 0) {
		        return false;
		    }
		}

		if(aucSrcAddr) {
		    /* specify MAC address[0:3] */
		    memcpy(aucMacAddr, aucSrcAddr, sizeof(uint8_t) * 4);
		    u4MacAddr = *(uint32_t *)(&(aucMacAddr[0]));
		    if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_SRC_ADDRESS,
		                u4MacAddr,
		                NULL,
		                NULL) != 0) {
		        return false;
		    }

		    /* specify MAC address[4:5] */
		    memset(aucMacAddr, 0, sizeof(uint8_t) * 4);
		    memcpy(aucMacAddr, &(aucSrcAddr[4]), sizeof(uint8_t) * 2);
		    u4MacAddr = *(uint32_t *)(&(aucMacAddr[0]));
		    if(WIFI_TEST_set(RF_AT_FUNCID_SET_MAC_SRC_ADDRESS | (4 << TEST_SET_CMD_OFFSET),
		                u4MacAddr,
		                NULL,
		                NULL) != 0) {
		        return false;
		    }
		}

		/* Start RX */
		//return WIFI_TEST_RxStart();
		return true;
	}
	else {
		if(aucSrcAddr && aucDstAddr) {
			eRxRule = RF_AT_RXOK_DISABLED;
		}
		else if(aucDstAddr) {
			eRxRule = RF_AT_RXOK_MATCH_RA_ONLY;
		}
		else if(aucSrcAddr) {
			eRxRule = RF_AT_RXOK_MATCH_TA_ONLY;
		}
		else {
			eRxRule = RF_AT_RXOK_DISABLED;
		}
		/* specify policy */
		if(WIFI_TEST_set(RF_AT_FUNCID_SET_RXOK_MATCH_RULE,
		        eRxRule,
		        NULL,
		        NULL) != 0) {
			return false;
		}
		//return WIFI_TEST_RxStop();
		return true;
	}
}

bool WIFI_TEST_ClearResult(void)
{
    if(WIFI_TEST_get(RF_AT_FUNCID_RXOK_COUNT,
                0,
                NULL,
                &u4NumGoodFrames) == 0 &&
            WIFI_TEST_get(RF_AT_FUNCID_RXERROR_COUNT,
                0,
                NULL,
                &u4NumBadFrames) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_GetResult(uint32_t *pu4GoodFrameCount, uint32_t *pu4BadFrameCount)
{
    uint32_t u4GoodCnt, u4BadCnt;

    if(WIFI_TEST_get(RF_AT_FUNCID_RXOK_COUNT,
                0,
                NULL,
                &u4GoodCnt) == 0 &&
            WIFI_TEST_get(RF_AT_FUNCID_RXERROR_COUNT,
                0,
                NULL,
                &u4BadCnt) == 0) {

        if(pu4GoodFrameCount) {
            *pu4GoodFrameCount = u4GoodCnt - u4NumGoodFrames;
        }
        if(pu4BadFrameCount) {
            *pu4BadFrameCount = u4BadCnt - u4NumBadFrames;
        }

        return true;
    }
    else {
        return false;
    }
}


/* Set Rx default antenna for Rx diversity */
/* 0: main antenna */
/* 1: AUX antenna */
bool WIFI_TEST_SetRxDefaultAnt(uint8_t ucUseAuxAnt)
{
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_RX_DEF_ANT,
                ucUseAuxAnt,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

/* Set Tx encoding mode */
/* 0: BCC */
/* 1: LDPC */
bool WIFI_TEST_SetTxCodingMode(uint8_t ucLdpc)
{
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_TX_ENCODE_MODE,
                ucLdpc,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

/* Set J mode */
/* 0: disable */
/* 1: 5M single tone */
/* 2: 10M single tone */
bool WIFI_TEST_SetJMode(ENUM_WIFI_J_MODE eJModeSetting)
{
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_J_MODE_SETTING,
                eJModeSetting,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

//for cw mode
bool WIFI_TEST_TxTraining(void)
{
    bool result = false;

    result = WIFI_TEST_TxBurstInterval(20);
    if(!result)
        return result;

    result = WIFI_TEST_TxBurstFrames(50);
    if(!result)
        return result;

    result = WIFI_TEST_TxPayloadLength(500);
    if(!result)
        return result;

    result = WIFI_TEST_TxStart();
    if(!result)
        return result;
    sleep(1);
    //usleep(200000);

    result = WIFI_TEST_TxStop();
    if(!result)
        return result;

    return true;
}

bool WIFI_TEST_CW_SetTxPath(uint32_t u4DefaultTxPath)
{
    if (WIFI_TEST_set(RF_AT_FUNCID_SET_ANTMASK,
                u4DefaultTxPath,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_CW_MODE(int mode)
{
    if(WIFI_TEST_set(RF_AT_FUNCID_SET_CW_MODE,
                (uint32_t)mode,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool WIFI_TEST_CW_MODE_START(bool abspowerFlag, float txGain)
{
	/* cw mode start */
    if(WIFI_TEST_TxTraining() == false) {
        return false;
    }
    if (!default_channel_power_Flag && abspowerFlag) {
        WIFI_TEST_ABSTxGain(txGain);
    }
    if(WIFI_TEST_set(RF_AT_FUNCID_COMMAND,
                RF_AT_COMMAND_CW,
                NULL,
                NULL) == 0) {
        return true;
    }
    else {
        return false;
    }


}

bool WIFI_TEST_init(void) {
	/* initialize skfd */
    if (skfd < 0 && (skfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        //ALOGE("[%s] failed to open net socket\n", __func__);
	fprintf(stderr, "[%s] : failed to opern net socket in %s\n", strerror(errno), __func__);
	errno = 0;
        return false;
    }
	return true;
}

void WIFI_TEST_deinit(void) {
	/* close socket if necessary */
    if(skfd > 0) {
        close(skfd);
        skfd = -1;
    }
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service for getting RX err/ok count at the same time
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_RXDATA(int *RXDATA) {

    if (WIFI_TEST_get(RF_AT_FUNCID_GET_RX_DATA,
                0,
                NULL,
                (uint32_t *)RXDATA) == 0) {
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service getting TX total/ok count at the same time
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_TXDATA(int *TXDATA) {

    if (WIFI_TEST_get(RF_AT_FUNCID_GET_TX_DATA,
            0,
            NULL,
            (uint32_t *)TXDATA) == 0) {
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service for checking FW version link to wifi test tool
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_GetFWVersion(int* FW_version) {

    if (WIFI_TEST_get(RF_AT_FUNCID_GET_VERSION,
                0,
                NULL,
                (uint32_t *)FW_version) == 0) {
        return true;
    }
    else {
        return false;
    }

}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for EFUSE query commands
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_EFUSE_Read(unsigned int offset, unsigned int *val)
{
    if (!val) {
        return false;
    }

    if (WIFI_TEST_get(RF_AT_FUNCID_READ_EFUSE,
                offset,
                NULL,
                (uint32_t *)val) != 0) {
        return false;
    }
    return true;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for EFUSE write commands
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_EFUSE_Write(unsigned int offset, unsigned int val)
{
    if (WIFI_TEST_set(RF_AT_FUNCID_WRITE_EFUSE | (offset << TEST_SET_CMD_OFFSET),
                val,
                NULL,
                NULL) != 0) {
        return false;
    }
    return true;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for MCR query commands
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MCR_Read(unsigned int addr, unsigned int *val)
{
    int retval = -1;
    struct iwreq wrq;
    NDIS_TRANSPORT_STRUCT rNdisStruct;
    P_PARAM_CUSTOM_MCR_RW_STRUC_T prMCRStruct;

    prMCRStruct = (P_PARAM_CUSTOM_MCR_RW_STRUC_T)rNdisStruct.ndisOidContent;
    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    prMCRStruct->u4McrOffset = addr;
    /* zeroize */
    rNdisStruct.ndisOidCmd = OID_CUSTOM_MCR_RW;
    rNdisStruct.inNdisOidlength = sizeof(PARAM_CUSTOM_MCR_RW_STRUC_T);
    rNdisStruct.outNdisOidLength = sizeof(PARAM_CUSTOM_MCR_RW_STRUC_T);

    wrq.u.data.pointer = &rNdisStruct;
    wrq.u.data.length = sizeof(NDIS_TRANSPORT_STRUCT);
    wrq.u.data.flags = PRIV_CMD_OID;

    retval = ioctl_iw_ext(IOCTL_GET_STRUCT, &wrq);

    if (retval == 0) {
        *val = prMCRStruct->u4McrData;
    }

	return !retval;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service for determine chip ID
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
int
WIFI_TEST_GetChipID(
      unsigned int *data
    )
{
    int retval;
    struct iwreq wrq;

    HQA_CMD_FRAME HqaCmdFrame;

    memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));


    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
    HqaCmdFrame.Type = htons(0x0008);
    HqaCmdFrame.Id = htons(0x1514);		// HQA_GetFreeEfuseBlock
    HqaCmdFrame.Length =   htons(20);  //(0x1308)   //20       //2 + 16;
    HqaCmdFrame.Sequence = htons(0);


    /* configure struct iwreq */
    wrq.u.data.pointer = &HqaCmdFrame;
    wrq.u.data.length = sizeof(HQA_CMD_FRAME); //0x20;
    wrq.u.data.flags = 0;//0;

    retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);

    memcpy(data, HqaCmdFrame.Data+2, 4);
    *data = htonl(*data);

    return !retval;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for MCR set commands
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MCR_Write(unsigned int addr, unsigned int val)
{
    int retval = -1;
    struct iwreq wrq;
    NDIS_TRANSPORT_STRUCT rNdisStruct;
    P_PARAM_CUSTOM_MCR_RW_STRUC_T prMCRStruct;

    prMCRStruct = (P_PARAM_CUSTOM_MCR_RW_STRUC_T)rNdisStruct.ndisOidContent;
    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    prMCRStruct->u4McrOffset = addr;
    prMCRStruct->u4McrData = val;
    /* zeroize */
    rNdisStruct.ndisOidCmd = OID_CUSTOM_MCR_RW;
    rNdisStruct.inNdisOidlength = sizeof(PARAM_CUSTOM_MCR_RW_STRUC_T);
    rNdisStruct.outNdisOidLength = sizeof(PARAM_CUSTOM_MCR_RW_STRUC_T);

    wrq.u.data.pointer = &rNdisStruct;
    wrq.u.data.length = sizeof(NDIS_TRANSPORT_STRUCT);
    wrq.u.data.flags = PRIV_CMD_OID;

    retval = ioctl_iw_ext(IOCTL_SET_STRUCT, &wrq);

    return !retval;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for MAC address efuse write
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MAC_Write(
unsigned int addr,
unsigned char* mac_bytes,
unsigned char mac_bytes_len)
{

    int retval = -1;
	char efuse_buf[EFUSE_BYTES_PER_LINE] = {};
	const uint32_t address = addr; /* MAC start address */
	const uint16_t address_in_block = (address % EFUSE_BYTES_PER_LINE);
	const uint16_t block_offset = (address / EFUSE_BYTES_PER_LINE) * EFUSE_BYTES_PER_LINE;
	int index;

	/* read efuse block memory */
	WIFI_TEST_get_HQA(block_offset, block_offset, (unsigned char *)efuse_buf);

	/* copy mac address to efuse block memory */
	memcpy(efuse_buf+address_in_block, mac_bytes, mac_bytes_len);

    /* Set mode to efuse mode to notify driver for writing efuse */
    WIFI_TEST_set_Eeprom_Mode(EFUSE_MODE);

	/* write efuse block memory */
	WIFI_TEST_set_HQA(block_offset, block_offset, (unsigned char *)efuse_buf);

	/* read efuse again to check if efuse data is correct */
	WIFI_TEST_get_HQA(block_offset, block_offset, (unsigned char *)efuse_buf);
	if (memcmp(efuse_buf+address_in_block, mac_bytes, mac_bytes_len)==0) {
		for (index=0; index<6; index++) {
			printf("EFUSE[0x%X] = 0x%02X\n", index+address, mac_bytes[index]);
		}
		retval = 0;
	}


    return !retval;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided service for efuse read per block line
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
int
WIFI_TEST_GetFreeEfuseBlock(
      unsigned int *data
    )
{
    int retval;
    struct iwreq wrq;

    HQA_CMD_FRAME HqaCmdFrame;

	memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));


    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
    HqaCmdFrame.Type = htons(0x0008);
    HqaCmdFrame.Id = htons(0x130A);		// HQA_GetFreeEfuseBlock
    HqaCmdFrame.Length =   htons(20);  //(0x1308)   //20       //2 + 16;
    HqaCmdFrame.Sequence = htons(0);


    /* configure struct iwreq */
    wrq.u.data.pointer = &HqaCmdFrame;
    wrq.u.data.length = sizeof(HQA_CMD_FRAME); //0x20;
    wrq.u.data.flags = 0;//0;

    retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);

	memcpy(data, HqaCmdFrame.Data+2, 4);
	*data = htonl(*data);

    return !retval;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API enable/disable singleSKU in RF test mode
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_DisSingleSku(uint32_t val)
{
	int retval;
	if (val) {
		retval = WIFI_TEST_set(RF_AT_FUNCID_SET_SINGLE_SKU, 1, NULL, NULL);
		printf("Stop singleSKU TX power limitation\n");
	} else {
		retval = WIFI_TEST_set(RF_AT_FUNCID_SET_SINGLE_SKU, 0, NULL, NULL);
		printf("Enable singleSKU TX power limitation\n");
	}

    	return retval;
}

#if CONFIG_SUPPORT_FFT
bool WIFI_TEST_SetIcapStart(P_ICAP_CTRL_T prIcapCtrl) {
    int retval;
    struct iwreq wrq;
    uint32_t u4Control;
    uint32_t u4Trigger;
    uint32_t u4RingCapEn;
    uint32_t u4TriggerEvent;
    uint32_t u4CaptureNode;
    uint32_t u4CaptureLen;
    uint32_t u4CapStopCycle;
    uint32_t u4BW;
    HQA_CMD_FRAME HqaCmdFrame;

    memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
    HqaCmdFrame.Type = htons(0x0008);
    HqaCmdFrame.Id = htons(0x1580);     // HQA_CapWiFiSpectrum
    HqaCmdFrame.Length = htons(4 * 8);
    HqaCmdFrame.Sequence = htons(0);

    u4Control = htonl(1);
    memcpy(HqaCmdFrame.Data + 4 * 0, &u4Control, 4);
    u4Trigger = htonl(1);
    memcpy(HqaCmdFrame.Data + 4 * 1, &u4Trigger, 4);
    u4RingCapEn = htonl(0);
    memcpy(HqaCmdFrame.Data + 4 * 2, &u4RingCapEn, 4);
    u4TriggerEvent = htonl(0);
    memcpy(HqaCmdFrame.Data + 4 * 3, &u4TriggerEvent, 4);
    u4CaptureNode = htonl(8);
    memcpy(HqaCmdFrame.Data + 4 * 4, &u4CaptureNode, 4);
    u4CaptureLen = htonl(400);
    memcpy(HqaCmdFrame.Data + 4 * 5, &u4CaptureLen, 4);
    u4CapStopCycle = htonl(400);
    memcpy(HqaCmdFrame.Data + 4 * 6, &u4CapStopCycle, 4);
    u4BW = htonl(prIcapCtrl->u4Cbw);
    memcpy(HqaCmdFrame.Data + 4 * 7, &u4BW, 4);

    /* configure struct iwreq */
    wrq.u.data.pointer = &HqaCmdFrame;
    wrq.u.data.length = sizeof(HQA_CMD_FRAME);
    wrq.u.data.flags = 0;

    retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);
    if (retval != 0) return false;

    return true;
}

bool WIFI_TEST_GetIcapStatus(uint16_t *pu2Status) {
    int retval;
    struct iwreq wrq;
    uint32_t u4Control = 0;
    HQA_CMD_FRAME HqaCmdFrame;

    memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
    HqaCmdFrame.Type = htons(0x0008);
    HqaCmdFrame.Id = htons(0x1580);     // HQA_CapWiFiSpectrum
    HqaCmdFrame.Length = htons(4 * 1);
    HqaCmdFrame.Sequence = htons(0);

    u4Control = htonl(2);
    memcpy(HqaCmdFrame.Data + 4 * 0, &u4Control, 4);

    /* configure struct iwreq */
    wrq.u.data.pointer = &HqaCmdFrame;
    wrq.u.data.length = sizeof(HQA_CMD_FRAME);
    wrq.u.data.flags = 0;

    retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);
    if (retval != 0) return false;

    memcpy(pu2Status, HqaCmdFrame.Data, 2);

    *pu2Status = htons(*pu2Status);

    return true;
}

bool WIFI_TEST_GetIcapDataFragment(uint32_t u4WF, uint32_t u4IQ, uint32_t *pu4Len, uint32_t *pu4Buf) {
    int retval;
    struct iwreq wrq;
    uint32_t u4Control;
    uint32_t u4DataLen;
    uint32_t u4WFNum;
    uint32_t u4IQNum;
    HQA_CMD_FRAME HqaCmdFrame;
    int i;

    memset(&HqaCmdFrame, 0, sizeof(HQA_CMD_FRAME));

    /* zeroize */
    memset(&wrq, 0, sizeof(struct iwreq));

    HqaCmdFrame.MagicNo = htonl(HQA_CMD_MAGIC_NO);
    HqaCmdFrame.Type = htons(0x0008);
    HqaCmdFrame.Id = htons(0x1580);     // HQA_CapWiFiSpectrum
    HqaCmdFrame.Length = htons(4 * 3);
    HqaCmdFrame.Sequence = htons(0);

    u4Control = htonl(3);
    memcpy(HqaCmdFrame.Data + 4 * 0, &u4Control, 4);
    u4WFNum = htonl(u4WF);
    memcpy(HqaCmdFrame.Data + 4 * 1, &u4WFNum, 4);
    u4IQNum = htonl(u4IQ);
    memcpy(HqaCmdFrame.Data + 4 * 2, &u4IQNum, 4);

    /* configure struct iwreq */
    wrq.u.data.pointer = &HqaCmdFrame;
    wrq.u.data.length = sizeof(HQA_CMD_FRAME);
    wrq.u.data.flags = 0;

    retval = ioctl_iw_ext(IOCTL_QA_TOOL_DAEMON, &wrq);
    if (retval != 0) return false;

    memcpy(&u4DataLen, HqaCmdFrame.Data + 2 + 4 * 3, sizeof(u4DataLen));
    u4DataLen = htonl(u4DataLen);

    memcpy(pu4Buf, HqaCmdFrame.Data + 2 + 4 * 4, u4DataLen * sizeof(uint32_t));

    for (i = 0; i < u4DataLen; i++)
        pu4Buf[i] = ntohl(pu4Buf[i]);

    *pu4Len = u4DataLen;

    return true;
}

bool WIFI_TEST_GetIcapData(P_ICAP_CTRL_T prIcapCtrl) {
    bool retval;
    uint32_t u4WFNum;
    uint32_t u4Loop;
    uint32_t u4IQ;
    uint32_t IQNumberCount;
    uint32_t IQNumberTotalCount;
    uint32_t *prIcapData;
    int i;

    prIcapData = malloc(ICAP_SIZE * sizeof(uint32_t));
    if (prIcapData == NULL) {
        return false;
    }

    memset(rRawData, 0, sizeof(rRawData));

    for (u4Loop = 0; u4Loop < MAX_ANTENNA_NUM; u4Loop++) {
        if ( (prIcapCtrl->u4RxPath != 0x3) && (u4Loop > ANTENNA_WF0) ) {  // WF0 or WF1 only
            break;
        }

        if (prIcapCtrl->u4RxPath == 0x2) {
            u4WFNum = ANTENNA_WF1;
        } else {
            u4WFNum = u4Loop;
        }

        for (u4IQ = 0; u4IQ < IQ_NUM; u4IQ++) {  // I, Q
            memset(prIcapData, 0, ICAP_SIZE);
            IQNumberTotalCount = 0;

            do {
                retval = WIFI_TEST_GetIcapDataFragment(u4Loop, u4IQ, &IQNumberCount, prIcapData + IQNumberTotalCount);
                if (retval == 0) {
                    free(prIcapData);
                    return false;
                }

                IQNumberTotalCount += IQNumberCount;
            } while (IQNumberCount > 0);

            for (i = 0; i < IQNumberTotalCount; i++) {
                rRawData[u4WFNum][u4IQ][i].Value16Bit = prIcapData[i];
            }
        }
    }

    free(prIcapData);

#if 0  // for debug
    u4WFNum = (prIcapCtrl->u4RxPath == 0x2)? ANTENNA_WF1 : ANTENNA_WF0;

    printf("%s Tool Data (WF%d)\n", __FUNCTION__, u4WFNum);

    for (i = 0; i < 128; i++) {
        printf("%s Tool Data[%3d]: %8d %8d\n", __FUNCTION__, i,
            rRawData[u4WFNum][IQ_0][i].Value14Bit.Value,
            rRawData[u4WFNum][IQ_1][i].Value14Bit.Value);
    }

    if (prIcapCtrl->u4RxPath == 0x3) {
        printf("%s Tool Data (WF1)\n", __FUNCTION__);

        for (i = 0; i < 128; i++) {
            printf("%s Tool Data[%3d]: %8d %8d\n", __FUNCTION__, i,
                rRawData[ANTENNA_WF1][IQ_0][i].Value14Bit.Value,
                rRawData[ANTENNA_WF1][IQ_1][i].Value14Bit.Value);
        }
    }
#endif

    return true;
}

/* FFT recursion */
void fft_rec(int N, int offset, int delta, double (*x)[2], double (*X)[2], double (*XX)[2]) {
    int N2 = N / 2;          /* half the number of points in FFT */
    int k;                   /* generic index */
    double cs, sn;           /* cosine and sine */
    int k00, k01, k10, k11;  /* indices for butterflies */
    double tmp0, tmp1;       /* temporary storage */

    if (N != 2) {  /* Perform recursive step. */
        /* Calculate two (N/2)-point DFT's. */
        fft_rec(N2, offset, 2 * delta, x, XX, X);
        fft_rec(N2, offset + delta, 2 * delta, x, XX, X);

        /* Combine the two (N/2)-point DFT's into one N-point DFT. */
        for (k = 0; k < N2; k++) {
            k00 = offset + k * delta;
            k01 = k00 + N2 * delta;
            k10 = offset + 2 * k * delta;
            k11 = k10 + delta;
            cs = cos(TWO_PI * k / (double)N);
            sn = sin(TWO_PI * k / (double)N);
            tmp0 = cs * XX[k11][0] + sn * XX[k11][1];
            tmp1 = cs * XX[k11][1] - sn * XX[k11][0];
            X[k01][0] = XX[k10][0] - tmp0;
            X[k01][1] = XX[k10][1] - tmp1;
            X[k00][0] = XX[k10][0] + tmp0;
            X[k00][1] = XX[k10][1] + tmp1;
        }
    } else {  /* Perform 2-point DFT. */
        k00 = offset;
        k01 = k00 + delta;
        X[k01][0] = x[k00][0] - x[k01][0];
        X[k01][1] = x[k00][1] - x[k01][1];
        X[k00][0] = x[k00][0] + x[k01][0];
        X[k00][1] = x[k00][1] + x[k01][1];
    }
}

/* FFT */
void fft(int N, double (*x)[2], double (*X)[2]) {
    /* Declare a pointer to scratch space. */
    double (*XX)[2] = (double(*)[2]) malloc(2 * N * sizeof(double));

    /* Calculate FFT by a recursion. */
    fft_rec(N, 0, 1, x, X, XX);

    /* Free memory. */
    free(XX);
}

void CalFrequency_N(int WfNumber, int N_fft, int Bandwidth, double * Buffer) {
    int N = 0, Nlimit = 0, ii = 0, i = 0;                    // number of points in FFT
    double fpower = 0;
    double FFT_IN[FFT_SIZE][2], FFT_OUT[FFT_SIZE][2], temp[FFT_SIZE][2];
    double XYaxis_FrequencyDomain[FFT_SIZE][2];
    double AfterIQ_Flip[FFT_SIZE];
    int nFFTSize = 512;     //
    double win_v;
    int N_512;  // number of multiple of 512 of N_fft
    int start_idx;

    Nlimit = (int)(FFT_SIZE_MAX / N_fft);  // average FFT
    N = N_fft;
    memset(FFT_IN, 0, sizeof(double) * FFT_SIZE * 2);
    memset(FFT_OUT, 0, sizeof(double) * FFT_SIZE * 2);
    memset(temp, 0, sizeof(double) * FFT_SIZE * 2);
    memset(XYaxis_FrequencyDomain, 0, sizeof(double) * FFT_SIZE * 2);
    memset(AfterIQ_Flip, 0, sizeof(double) * FFT_SIZE);
    memset(Buffer, 0, sizeof(double) * N_fft * 2);

    for (ii = 0; ii < Nlimit; ii++) {
        for (i = 0; i < N; i++) {
            FFT_IN[i][0] = (double)rRawData[WfNumber][IQ_0][ii * N + i].Value14Bit.Value;
            FFT_IN[i][1] = (double)rRawData[WfNumber][IQ_1][ii * N + i].Value14Bit.Value;
        }

        // linear interpolator
        N_512 = N / nFFTSize;  // number of multiple of 512 of N_fft

        for (i = 0; (int)i < N; i++) {
            start_idx = (int)(i / N_512);
            win_v = (double)(rwindow[start_idx] +
                (rwindow[(start_idx+1) % nFFTSize] - rwindow[start_idx]) / N_512 * (i-start_idx * N_512));
            FFT_IN[i][0] = FFT_IN[i][0] * win_v;
            FFT_IN[i][1] = FFT_IN[i][1] * win_v;
        }

        // Calculate FFT.
        fft(N, FFT_IN, FFT_OUT);

        for (i = 0; (int)i < N; i++) {
            if (ii == 0) {
                temp[i][0] = FFT_OUT[i][0] * FFT_OUT[i][0];
                temp[i][1] = FFT_OUT[i][1] * FFT_OUT[i][1];
            } else {
                temp[i][0] = temp[i][0] + FFT_OUT[i][0] * FFT_OUT[i][0];
                temp[i][1] = temp[i][1] + FFT_OUT[i][1] * FFT_OUT[i][1];
            }
        }
    }  // end for ii

    for (i = 0; (int)i < N; i++) {
        temp[i][0] = temp[i][0] / Nlimit;
        temp[i][1] = temp[i][1] / Nlimit;
        // calcuate power excel tail to compare with golden
        if (i < N - (N / nFFTSize * 27) || i > N - (N / nFFTSize * 24))
            fpower = fpower + 10 * log10((temp[i][0] + temp[i][1])) - 27.1;
    }

    // write into array
    for (i = 0; i <= (N / 2) - 1; i++) {  // 0~N/2-1 x
        XYaxis_FrequencyDomain[i][0] = (float)(-(N / 2 - (int)i)) * Bandwidth / (N / 2);
        AfterIQ_Flip[i] = 10 * log10((temp[(N / 2) - i][0] + temp[(N / 2) - i][1])) - 28;
    }

    XYaxis_FrequencyDomain[i][0] = (float)0;
    AfterIQ_Flip[i] = 10 * log10(temp[0][0] + temp[0][1]) - 28;

    for (i = 0; i < (N / 2) - 1; i++) {  // N/2+1~N-1  x
        XYaxis_FrequencyDomain[i + (N / 2) + 1][0] = (float)(i + 1) * Bandwidth / (N / 2);
        AfterIQ_Flip[i + (N / 2) + 1] = 10 * log10(temp[N - 1 - i][0] + temp[N - 1 - i][1]) - 28;
    }

    //================================================================================//
    // For IQ flip
    // re-order the capture data
    for (i = 0; i <= N - 1; i++) {  // 0~N-1 y
        XYaxis_FrequencyDomain[i][1] = AfterIQ_Flip[N - 1 - i];
    }
    //================================================================================//

    memcpy(Buffer, XYaxis_FrequencyDomain, sizeof(double) * N_fft * 2);

}

bool WIFI_TEST_RunFftFunction(P_ICAP_CTRL_T prIcapCtrl) {
    if (prIcapCtrl->u4RxPath & 0x1) {
        CalFrequency_N(ANTENNA_WF0, FFT_SIZE, prIcapCtrl->u4BwMhz, &rFreqPwr[ANTENNA_WF0][0][0]);
    }

    if (prIcapCtrl->u4RxPath & 0x2) {
        CalFrequency_N(ANTENNA_WF1, FFT_SIZE, prIcapCtrl->u4BwMhz, &rFreqPwr[ANTENNA_WF1][0][0]);
    }

    return true;
}

bool WIFI_TEST_DumpIcapResult(P_ICAP_CTRL_T prIcapCtrl, bool bIQData) {
    FILE *streamOpenFile;
    char aucFileName[255];
    int i, k;
    int i4FreqMhz;

    if (prIcapCtrl->i4Channel < 0) {
        return false; /* invalid channel number */
    }
    /* 2.4GHz band */
    else if (prIcapCtrl->i4Channel <= 13) {
        i4FreqMhz = 2412 + (prIcapCtrl->i4Channel - 1) * 5;
    }
    else if (prIcapCtrl->i4Channel == 14) {
        i4FreqMhz = 2484;
    }
    /* 5GHz band */
    else if (prIcapCtrl->i4Channel >= 36) {
        i4FreqMhz = 5180 + (prIcapCtrl->i4Channel - 36) * 5;
    }
    else {
        return false; /* invalid channel number */
    }

    for (k = 0; k < MAX_ANTENNA_NUM; k++) {
        if ((prIcapCtrl->u4RxPath == 0x1) && (k != ANTENNA_WF0)) {  // WF0 only
            break;
        }

        if ((prIcapCtrl->u4RxPath == 0x2) && (k != ANTENNA_WF1)) {  // WF1 only
            continue;
        }

        if (access(FFT_RESULT_PATH, F_OK) != 0) {
            if (mkdir(FFT_RESULT_PATH, 0770) == -1) {
                printf("==> make directory error \n");
                return false;
            }
        }

        memset(aucFileName, 0, sizeof(aucFileName));

        if (bIQData) {
            sprintf(aucFileName, "%s/raw_data_ch%d_bw%d_wf%d.adc",
                FFT_RESULT_PATH, prIcapCtrl->i4Channel, prIcapCtrl->u4BwMhz, k);
        } else {
            sprintf(aucFileName, "%s/freq_pwr_ch%d_bw%d_wf%d.csv",
                FFT_RESULT_PATH, prIcapCtrl->i4Channel, prIcapCtrl->u4BwMhz, k);
        }

        if ( (streamOpenFile = fopen(aucFileName, "w+")) != NULL ) {
            if (bIQData)
                fprintf(streamOpenFile, "%%(I%d,Q%d)\n", k, k);
            else
                fprintf(streamOpenFile, "Freq,Power\n");
        } else {  // open file error
            printf("==> open file error \n");

            return false;
        }

        printf("==> %s \n", aucFileName);

        if (bIQData) {
            for (i = 0; i < ICAP_SIZE; i++) {
                fprintf(streamOpenFile, "%+04d\t%+04d\t\n",
                    rRawData[k][IQ_0][i].Value14Bit.Value, rRawData[k][IQ_1][i].Value14Bit.Value);
            }
        } else {
            for (i = 0; i < FFT_SIZE; i++) {
                fprintf(streamOpenFile, "%8.3f,%9.5f\n",
                    (double)(i4FreqMhz + rFreqPwr[k][i][0]), rFreqPwr[k][i][1]);
            }
        }

        fclose(streamOpenFile);
    }

    return true;
}
#endif

bool WIFI_TEST_Temperature(int *pi4Temp)
{
    uint32_t u4Value;
    uint32_t u4Temp;

    if (!pi4Temp)
        return false;

    if (WIFI_TEST_get(RF_AT_FUNCID_TEMP_SENSOR,
                0,
                NULL,
                (uint32_t *)&u4Value) != 0) {
        return false;
    }

    u4Temp = u4Value >> 16;
    *pi4Temp = (u4Temp & BIT(15))? (u4Temp - 65536) : u4Temp;

    return true;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API enable set channel
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSChannel()
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_CHANNEL, 0, NULL, NULL);

	return (retval==0)?true:false;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This API set tx number in mps feature
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSNum(uint32_t val)
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_MPS_SIZE, val, NULL, NULL);
	printf("MPS TX number:%d\n", val);
	return (retval==0)?true:false;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API set seq data in mps feature
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSSeqData(uint32_t val)
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_MPS_SEQ_DATA, val, NULL, NULL);
	printf("MPS Seq Data:%d\n", val);
	return (retval==0)?true:false;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API set payload length in mps feature
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSPayloadLength(uint32_t val)
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_MPS_PAYLOAD_LEN, val, NULL, NULL);
	printf("MPS Seq Data:%d\n", val);
	return (retval==0)?true:false;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API set Packets Count in mps feature
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSPktCount(uint32_t val)
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_MPS_PKT_CNT, val, NULL, NULL);
	printf("MPS Packet Count:%d\n", val);
	return (retval==0)?true:false;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API set PWR in mps feature
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSPower(uint32_t val)
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_MPS_PWR_GAIN, val, NULL, NULL);
	printf("MPS Power:%d\n", val);
	return (retval==0)?true:false;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API set stream number in mps feature
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSStream(uint32_t val)
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_MPS_NSS, val, NULL, NULL);
	printf("MPS stream:%d\n", val);
	return (retval==0)?true:false;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API set data BW in mps feature
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSPackageBW(uint32_t val)
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_MPS_PACKAGE_BW, val, NULL, NULL);
	printf("PKG BW stream:%d\n", val);
	return (retval==0)?true:false;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API set System BW in mps feature
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_MPSSystemBW(uint32_t val)
{
	int retval;
	retval = WIFI_TEST_set(RF_AT_FUNCID_SET_MPS_SYSTEM_BW, val, NULL, NULL);
	printf("System BW stream:%d\n", val);
	return (retval==0)?true:false;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provide for counting function data 15~0 bit overflow, get function data[15:0]
 *
          and make 16 bit register show more than 16 bit

          register can show when overflow
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

void Overflow_bit15_0(int* function_data, int* function_data_15_0,
                     int* pre_function_data_15_0,
                     int* result_15_0,
                     int* function_data_15_0_carry)
{
    *function_data_15_0 =  (*function_data << 16) >> 16; //clear bit [31:16]

    if (*function_data_15_0 < *pre_function_data_15_0) {//count overflow times, per overflow means 64000 packets
        (*function_data_15_0_carry)++;
    }

    *pre_function_data_15_0 = *function_data_15_0;

    *result_15_0 = *function_data_15_0 + (*function_data_15_0_carry) * 65536;//add 16 bit overflow count
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provide for counting function data 31~16 bit overflow, get function data[31:16]
 *
          and make 16 bit register show more than 16 bit

          register can show when overflow
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/


void Overflow_bit31_16(int* function_data,
                     int* pre_funcion_data_31_16,
                     int* result_31_16,
                     int* function_data_31_16_carry)
{
	*function_data = (*function_data >> 16); //clear bit [15:0]

    if (*function_data < *pre_funcion_data_31_16) {
        (*function_data_31_16_carry)++;
    }

    *pre_funcion_data_31_16 = *function_data;

    *result_31_16 = *function_data + (*function_data_31_16_carry) * 65536;//add 16 bit overflow count
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provide feature support or not of FW and tool version dependency
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool feature_support(const int SW_VERSION[], int feature_version) {
    int FW_version = 0, toolversion;

    WIFI_TEST_GetFWVersion(&FW_version);
    if(!FW_version) {
        return false;
    }
    toolversion = SW_VERSION[0] * 10000 + SW_VERSION[1] * 100 + SW_VERSION[2];

    return ((toolversion >= feature_version) && (FW_version>=feature_version))?true:false;
}

int searchCalID(int CalTotalNum, char *InCalType) {
    int i;
    for(i = 0; i < CalTotalNum; ++i) {
        if(!strcmp(RecalType[i],InCalType)) {
            printf("return caltype:%s", InCalType );
            return i;
        }
    }
    return 0;
}
