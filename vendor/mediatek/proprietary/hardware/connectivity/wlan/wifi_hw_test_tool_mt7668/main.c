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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <linux/types.h>
#include <limits.h>
#include <getopt.h>
#include <errno.h>
typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
#define PROP_VALUE_MAX  92
#define RF_TEST_MODE_SWITCH_IDLE 1000000
#include "libwifitest.h"
#include "lib.h"
#include <sys/types.h>
#include <regex.h>
//#include <cutils/properties.h>
//#include <sys/system_properties.h>
#include <errno.h>
#include "libwifitest.h"
#define PACKAGE     "WifiHwTestTool"
//#define EEPROM_PATH		"/lib/firmware/EEPROM_MT7668.bin"

#ifdef CONFIG_YOCTO_EEPROM_PATH
char EEPROM_PATH[64] = "/data/misc/connectivity/EEPROM_MT7668.bin";
char EEPROM_PATH_TMP[64] = "/data/misc/connectivity/EEPROM_MT7668.bin"; // tmp buffer
char EEPROM_PATH_WRITE[64] = "/data/misc/connectivity/EEPROM_MT7668.bin";
char EEPROM_PATH_7663[64] = "/data/misc/connectivity/EEPROM_MT7663.bin";
char EEPROM_PATH_TMP_7663[64] = "/data/misc/connectivity/EEPROM_MT7663.bin"; // tmp buffer
char EEPROM_PATH_WRITE_7663[64] = "/data/misc/connectivity/EEPROM_MT7663.bin";

#else
char EEPROM_PATH[64] = "/vendor/firmware/EEPROM_MT7668.bin";
char EEPROM_PATH_TMP[64] = "/data/EEPROM_MT7668.bin"; // tmp buffer
char EEPROM_PATH_WRITE[64] = "/vendor/firmware/EEPROM_MT7668.bin";
char EEPROM_PATH_7663[64] = "/vendor/firmware/EEPROM_MT7663.bin";
char EEPROM_PATH_TMP_7663[64] = "/data/EEPROM_MT7663.bin"; // tmp buffer
char EEPROM_PATH_WRITE_7663[64] = "/vendor/firmware/EEPROM_MT7663.bin";

#endif
char proc_name[256];
const int SW_VERSION[] = {1, 8, 6};

/**************************************************
  COMPILER FLAG
 **************************************************/
/* Tone Argument */
#define TONE_ARG 6
#define SCANF_ARG6 6

typedef enum {
	WLAN_MODE_OFF,
	NORMAL_MODE_ON,
	TEST_MODE_ON
}WlanStatus;

typedef enum {
	OPER_NONE = 0,
	TEST_TX,
	TEST_RX,
	READ_EFUSE,
	READ_EFUSE_FREE_BLOCK,
	WRITE_EFUSE,
	WRITE_EFUSE_MAC_WIFI,
	WRITE_EFUSE_MAC_BT,
	READ_MCR,
	WRITE_MCR,
	TEST_STOP,
	QUERY_RESULT,
	SET_NSS,
	READ_EEPROM,
	WRITE_EEPROM,
	I_CAL,
	WRITE_EEPROM_TO_EFUSE,
	SET_RX_PATH,
	SET_FREQ_OFFSET,
	SET_TX_POWER_COMPENSATION,
	SET_TX_PATH,
	DUMP_EFUSE_ALL,
	SINGLE_TONE,
	CONTINUOUS_WAVE = 23,
	SET_TSSI = 24,
	SET_SINGLESKU = 25,
	SET_DPD = 26,
	GET_FWversion = 27,
	DUMP_EEPROM_ALL = 28,
	/*recal part*/
	RECAL = 29,
	INTERNAL_CAPTURE = 30,
#if CONFIG_SUPPORT_IWPRIV
	PIRV_CMD = 31,
#endif
	QUERY_TEMPERATURE = 32,
    	MPS_SET = 33,
	MPS_ADD = 34,
	MPS_START = 35
}Oper_Mode;

typedef enum _ENUM_RX_MATCH_RULE_T {
	RX_MATCH_RULE_DISABLE,
	RX_MATCH_RULE_RA,           /* RA only */
	RX_MATCH_RULE_TA,           /* TA only */
	RX_MATCH_RULE_RA_TA,        /* Both RA and TA */
	RX_MATCH_RULE_NUM
} ENUM_RX_MATCH_RULE_T, *P_ENUM_RX_MATCH_RULE_T;

typedef enum _ENUM_BUFFER_MODE_SOURCE {
	SOURCE_AUTO = 0,
	SOURCE_EEPROMBIN,
	SOURCE_EFUSE,
} ENUM_BUFFER_MODE_SOURCE;

char *bg_rate[] = {
	"RATE_AUTO",
	"RATE_1MBPS",
	"RATE_2MBPS",
	"RATE_5_5MBPS",
	"RATE_6MBPS",
	"RATE_9MBPS",
	"RATE_11MBPS",
	"RATE_12MBPS",
	"RATE_18MBPS",
	"RATE_24MBPS",
	"RATE_36MBPS",
	"RATE_48MBPS",
	"RATE_54MBPS",
};
char *preamble[] = {
	"LONG",
	"SHORT",
};
char *bandwidth[] = {
	"BW20",
	"BW40",
	"BW20U",
	"BW20L",
	"BW80",
	"BW160"
};

char *bandwidthV2[] = {
	"BW20",
	"BW40",
	"BW80",
	"BW160"
};

static bool check_string(signed char *optarg);
static void wifi_single_tone();
static void wifi_sensitivity(int, int);
static void wifi_tx();
static WlanStatus wifiStatus();
void wifiTestStop();
void wifiGetResult();
int open_eeprom_file();
bool WIFI_TEST_EEPROM_Write(unsigned int offset, unsigned int val);
bool WIFI_TEST_EEPROM_Read(unsigned int offset, unsigned int *val);
//void replaceEEPROM();
void saveEEPROM();

#if CONFIG_SUPPORT_FFT
void wifi_icap(void);
#endif

void signal_handler(int sig)
{
	bool retval = false;
    	retval = WIFI_TEST_TxStop();
    	retval = WIFI_TEST_SetRateOffset();//restore rate offset
   	printf("\n(%d) aborted TX .., sig=%d\n", retval, sig);
	retval = WIFI_TEST_RxStop();
	//retval = WIFI_TEST_CloseDUT();
	printf("\n(%d) aborted RX.., sig=%d\n", retval, sig);

	signal(SIGINT, SIG_DFL);
	exit(0);
}

//  "E:e:M:g:G:I:B:R:N:T:m:i:s:p:b:t:hVw:v:l:f    :c:rn:"


void print_help(int exval)
{
	printf("Usage: %s [options]\n", proc_name);
	printf("\n");

	printf("<MCR read/write>\n");
	printf("    %s [--version]              wifitest version\n", proc_name);
	printf("\n");

	printf("<Test mode control>\n");
	printf("    %s -O                       Start Wi-Fi test mode\n", proc_name);
	printf("    %s -C                       Open Default buffer bin\n", proc_name);
    	printf("    %s --version                WiFi Test Tool version\n", proc_name);
	printf("    %s --stoplimit [1/0]        Stop singleSKU in test mode <1: stop, 0: enalbe>\n", proc_name);
    	printf("    %s --setdpd [1/0]           Set DPD in test mode <1: disable, 0: enalbe>\n", proc_name);
    	printf("    %s --isodetect              Isolation Detection flow, must used with Rx flow\n", proc_name);
	printf("    %s --ackmac [aa:bb:cc:dd:ee:ff]  Set MAC address for auto ACK if receiver's address match with this address\n", proc_name);
	printf("    %s --hwack [1/0]            Enable Auto ACK <1: enable, 0: disable>\n", proc_name);
	printf("\n");

	printf("<MCR read/write>\n");
	printf("    %s [-M addr]                Read value from CR address\n", proc_name);
	printf("    %s [-w addr] [-v value]     Write value to CR address\n", proc_name);
	printf("\n");

	printf("<EFUSE read/write>\n");
	printf("    %s [-E offset]              Read value from EFUSE at offset\n", proc_name);
	printf("    %s [-e offset] [-v value]   Write value to EFUSE at offset\n", proc_name);
	printf("\n");

	printf("<EEPROM read/write>\n");
	printf("    %s [-U offset]              Read value from EEPROM at offset\n", proc_name);
	printf("    %s [-u offset] [-v value]   Write value to EEPROM at offset\n", proc_name);
	printf("\n");
#if CONFIG_SUPPORT_FFT
	printf("<FFT function>\n");
	printf("    %s [--fft] [-c channel] [-Q Rx path] [-b RF bandwidth] [-Y data bandwidth]\n", proc_name);
	printf("\n");
#endif
#if CONFIG_SUPPORT_IWPRIV
	printf("<priv function>\n");
	printf("    %s --priv [command]         Suuport iwpriv command via wifitestTool\n", proc_name);
	printf("\n");
#endif
	printf("<Temperature read>\n");
	printf("    %s --temp                   Read current temperature value\n", proc_name);
	printf("\n");

	printf("<Tx test>\n");
	printf("A/B/G Mode:\n");
	printf("    %s [-t 0] [-R legacy rate] [-s preamble] [options]\n", proc_name);

	printf("N Mode:\n");
	printf("    %s [-t 1] [-N MCS rate] [-g greenfield] [-G SGI] [options]\n", proc_name);

	printf("AC Mode:\n");
	printf("    %s [-t 2] [-N MCS rate] [-G SGI] [options]\n", proc_name);
	printf("\n");

	printf("<Rx test>\n");
	printf("    %s [-r] [-n time] [options]\n", proc_name);
	printf("\n");

	printf("Common for Tx/Rx:\n");
	printf("    -c #           Central channel number\n");
	printf("    -b [0~3]       RF bandwidth <0:20/1:40/2:80/3:160>Mhz <default 20Mhz>\n");
	printf("    -P [0~7]       Primary channel setting in unit of 20Mhz <default 0>\n");
	printf("    -B [0~3]       Bandwidth <0:20/1:40/2:20U/3:20L>Mhz (Legacy commaand, *deprecated)\n");
	printf("    -j [0~2]       J mode setting <0:disable/1:5Mhz/2:10Mhz>\n");
	printf("    -d [0/1]       Set Rx default antenna <0:main/1:AUX>\n");

	printf("    -S #           Test mode <0:non-blocking/others:blocking mode timeout in seconds>\n");
	printf("    -T             Test terminate command for non-blocking test\n");
	printf("    -a #           Blocking mode test result query interval in seconds\n");
	printf("    -o #           Max Tx/Rx packet count in blocking mode test\n");

	printf("    -q             Query test result\n");

	printf("    -D             Enable debug mode(dump AT command sequence)\n");

	printf("    -f             RX Filter type <0:default,Disalbe,1:filter RA>\n");
	printf("    -A             Set RA address on enabling RX Filter. ex:-A 123456789ABC is set mac 12:34:56:78:9A:BC to RA address\n");
	printf("\n");

	printf("Rx specific:\n");
	printf("    -n #           Test time in seconds.\n");
	printf("    -Q [1/2/3]     Set Rx Path <1:Rx0 1/2:Rx1/3:Rx0+Rx1> \n");
	printf("\n");

	printf("Tx specific:\n");

	printf("    -n #           TX Packet number, 0 is meaning that TX Packet number = unlimited\n");

	printf("    -t [0/1/2]     Tx mode <0:11abg/1:11n/2:11ac>\n");
	printf("    -x [0~3]       Tx bandwidth <0:20/1:40/2:80/3:160>Mhz <default follow RF BW>\n");
	printf("    -p #           Tx gain in dBm\n");

	printf("    -n #           Frame count\n");
	printf("    -l #           Frame length in bytes\n");
	printf("    -i #           Frame burst interval in TU\n");

	printf("    -R [1~12]      Legacy rate code\n");
	printf("                   <1M/2M/5.5M/6M/9M/11M/12M/18M/24M/36M/48M/54M>\n");
	printf("    -s [0/1]       <0:short/1:long> preamble\n");

	printf("    -N [0~15/32]   MCS rate index\n");
	printf("    -g [0/1]       <0:mixed mode/1:greenfield> \n");
	printf("    -G [0/1]       <0:normal/1:short> guard interval\n");
	printf("    -L             Enable LDPC <default BCC>\n");

	printf("    -m [0/3]       <0:disable/3:enable> continuous waveform mode\n");
	printf("    -# [1/2]       <1:NSS 1/2:NSS 2> \n");
	printf("    -y [1/2/3]     Set Tx Path <1:Tx0 1/2:Tx1/3:Tx0+Tx1> \n");
	printf("\n");

	printf("    -V #     Set Freq Offset \n");
	printf("    -J 1     I_Cal mode \n");
	printf("    -K 1     Write EEPROM bin file to efuse \n");
	printf("    -F [path for EEPROM_MT7668.bin]     change EEPROM bin file folder path, default is /lib/firmware/EEPROM_MT7668.bin\n");
	printf("    -X 1     Save bin file to EEPROM path\n");

	exit(exval);
}

static int channel = 1;
static int times = 10;
//static int numBurst = 0;
static int txMode = 0;
static unsigned char macAddr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static float txGain = 0;
static int payloadLength = 1024;
static int SIFS = 20;
static int g_rate = 6;
static ENUM_WIFI_TEST_MCS_RATE gMCSrate = WIFI_TEST_MCS_RATE_0;
static int g_bandwidth = WIFI_TEST_BW_20MHZ;
static ENUM_WIFI_TEST_PREAMBLE_TYPE gMode = WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE;
static ENUM_WIFI_TEST_GI_TYPE giType = WIFI_TEST_GI_TYPE_NORMAL_GI;
extern char WIFI_IF_NAME[256];
static WIFI_PreambleType_t pType = WIFI_TEST_PREAMBLE_SHORT;
static unsigned int mcr_addr = 0;
static unsigned int mcr_value = -10000;//impossible value
static unsigned int efuse_addr = 0;
static unsigned int eeprom_addr = 0;
static unsigned int rx_path = 0x1;
static unsigned int freq_offset = 0;
static unsigned int tx_power_compensation = 0;
/* we use bit data to tranfser,
   In detail => [31:24]:Reserved
   [23:20]: Tx0/Tx1
   [19:18]: 2G/5G
   [17:8]: Channel#
   [7:0]: channelpower compensataion */
static unsigned int tx_power_compensation_data = 0;
static int cw_mode = -1;
static int sleep_time = 10;
static bool sleepModeSet = false;
static int NSS = 1;

static int priSetting = 0;
static bool isNewBwSet = false;
static ENUM_WIFI_CHANNEL_BANDWIDTH rfBw = WIFI_TEST_CH_BW_20MHZ;
static bool isTxBwSet = false;
static ENUM_WIFI_CHANNEL_BANDWIDTH txBw = WIFI_TEST_CH_BW_20MHZ;
static int coding = 0; /* BCC */
static int rxDefaultAnt = 0;
static int jModeSetting = 0;
static int printInterval = 1;
static uint32_t maxPktCount = 0;
static int user_expect = 0;
static int current_mode = 0;
static int buffer_mode_content_source = 0;
static ENUM_RX_MATCH_RULE_T eRxOkMatchRule = RX_MATCH_RULE_DISABLE;
static bool bRxFilterMacAddrLegalFg = false;
static unsigned char aucRxFilterMacAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static uint32_t u4DefaultRxPath = 0x00030000;
static uint32_t u4DefaultTxPath = 0x1;
static uint32_t u4Dbdc = 0x0;
static uint32_t u4DbdcBandIndex = 0x0;
static uint32_t u4Cbw = 0x0;
//static uint32_t u4Dbw = 0x0;
static uint32_t u4PriCh = 0x0;
static uint32_t u4Band = 0;
static uint32_t u4MacHeader = 0x1;
static uint32_t u4Payload = 65706;
static uint32_t u4Ta = 0x0;
static uint32_t u4Stbc = 0x0;
static uint32_t u4Ibf = 0x0;
static uint32_t u4Ebf = 0x0;

static uint8_t efuse_write_mac_bytes[6] = {0};
static uint32_t u4DefaultCalFreqOffset = 0x0;

/* Auto Isolation Feature */
static unsigned int Isolation_val = 0;
static float BTTxPower = 0;
static bool SetBTTxFlag = false;
static bool SetIsoFlag = false;
static bool abspowerFlag = false;


extern bool default_channel_power_Flag;
/* Tone */
static uint32_t u4ToneFreq = 0;
static uint32_t u4ToneType = 0;
static uint32_t u4ToneRfGain = 0;
static uint32_t u4ToneDigitalGain = 0;
static uint32_t u4ToneDcOffsetI = 0;
static uint32_t u4ToneDcOffsetQ = 0;

/*Long Arg Parameter*/
static uint32_t u4DPDMode = 0;
static uint32_t u4SingleSkuMode = 0;
uint32_t u4HwAckEnable = 0;
unsigned char AckMac[6] = {0};

/* Use for Operation multiple parameters */
#define WLAN_CFG_ARGV_MAX 6
//static uint32_t u4ArgNum = 2;
static signed char *apcArgv[WLAN_CFG_ARGV_MAX] = {NULL};

#define WIFI_TEST_BW_MAX 5
#define CHIP_7668 30312
#define CHIP_7663 30307

#define CHIPID_CR 0x80000008
extern bool fgDebugMode;
unsigned char uacEEPROMImage[MAX_EEPROM_BUFFER_SIZE];

/*recal type*/
extern char *RecalType[];
/*Mapping calID to recal type*/
extern int CalID[];

/* short option */
const char *const short_option = \
	"*:A:f:E:e:U:u:M:g:G:F:J:K:Q:V:W:X:#:$:y:Y:I:B:R:N:Tm:i:S:s:p:b:t:hVw:v:k:l:f:c:rOCn:DP:x:Ld:j:q:a:oz:ZH:";

int long_flag;
bool g_MPS_flag = false;

/* LONG option flag*/
typedef enum _ENUM_LONG_OPTION {
	LONG_OP_TOOL_VERSION  = 0xF0000000,
	LONG_OP_PWR_LIMIT,
	LONG_OP_SET_DPD,
	LONG_OP_ISO_DETECT,
	LONG_OP_SET_ABSPOWER,
	LONG_OP_FW_VERSION,
	LONG_OP_DPD_CAL_LINK,
	LONG_OP_CAL_COUNT,
	LONG_OP_CAL_DUMP,
	LONG_OP_FFT_FUNCTION,
#if CONFIG_SUPPORT_IWPRIV
	LONG_OP_PRIV_FUNCTION,
#endif
	LONG_OP_TEMPERATURE,
	LONG_OP_MPS_SET,
	LONG_OP_MPS_ADD,
	LONG_OP_MPS_START,
	LONG_OP_MPS_NUMBER,
	LONG_OP_HW_ACK,
	LONG_OP_ACK_MAC,
	LONG_OP_RECAL,
	LONG_OP_INI,
	LONG_OP_DBDC
} ENUM_LONG_OPTION, *P_ENUM_LONG_OPTION;

/* LONG option */
const struct option long_options[] = {
	{"version", no_argument, &long_flag, LONG_OP_TOOL_VERSION},
	{"stoplimit", required_argument, &long_flag, LONG_OP_PWR_LIMIT},
	{"setdpd", required_argument, &long_flag, LONG_OP_SET_DPD},
	{"isodetect", no_argument, &long_flag, LONG_OP_ISO_DETECT},
        {"abspower", required_argument, &long_flag, LONG_OP_SET_ABSPOWER},
        {"FWversion", no_argument, &long_flag, LONG_OP_FW_VERSION},
        {"cal_tx_dpd_link", no_argument, &long_flag, LONG_OP_DPD_CAL_LINK},
        {"calcount", required_argument, &long_flag, LONG_OP_CAL_COUNT},
        {"caldump", required_argument, &long_flag, LONG_OP_CAL_DUMP},
        {"fft", no_argument, &long_flag, LONG_OP_FFT_FUNCTION},
#if CONFIG_SUPPORT_IWPRIV
        {"priv", required_argument, &long_flag, LONG_OP_PRIV_FUNCTION},
#endif
        {"temp", no_argument, &long_flag, LONG_OP_TEMPERATURE},
        {"MPS_set", no_argument, &long_flag, LONG_OP_MPS_SET},
        {"MPS_add", no_argument, &long_flag, LONG_OP_MPS_ADD},
        {"MPS_start", no_argument, &long_flag, LONG_OP_MPS_START},
        {"MPS_number", required_argument, &long_flag, LONG_OP_MPS_NUMBER},
	{"hwack", required_argument, &long_flag, LONG_OP_HW_ACK},
	{"ackmac", required_argument, &long_flag, LONG_OP_ACK_MAC},
    	{"recal", required_argument, &long_flag, LONG_OP_RECAL},
    {"INI", required_argument, &long_flag, LONG_OP_INI},
    {"dbdc", required_argument, &long_flag, LONG_OP_DBDC},
	{NULL, 0, 0, 'h'}
};


int main(int argc, char *argv[])
{
	int opt = 0;
	int result = 0;
	int index = 0, option_index = 0;
    	uint32_t u4CalMode = 128;
    	int CalCount = 1;
   	int CalDump = 1;
    	int CalIDindex = 0;
    	int tempDbdcBandIndex = 0;
	Oper_Mode operation = OPER_NONE;

	bool t_flag = false;/*to detect tX, stop TX/RX setting or not*/
        //bool C_flag = false;  /*take off -C due to not used from 1.8.2*/
        bool O_flag = false;/* detect if -O is setting or not*/
        bool Tool_version_flag = false, X_flag = false, F_flag = false, Ini_interface_flag = false;/*detect version operation*/
	WlanStatus wlan_status = 0;
	int band_width[] = {WIFI_TEST_BW_20MHZ, WIFI_TEST_BW_40MHZ,
		WIFI_TEST_BW_U20MHZ, WIFI_TEST_BW_D20MHZ, WIFI_TEST_BW_80MHZ,
		WIFI_TEST_BW_160MHZ};
    	int MpsNum = 0;
#if CONFIG_SUPPORT_IWPRIV
	char privcmdBuf[PRIV_CMD_SIZE] = {0};
	int privCmdLen = 0;
#endif
	strncpy(proc_name, argv[0], 255);
	proc_name[255] = '\0';

	if (argc == 1){
		//if (count == 1){
		fprintf(stderr, "Needs arguments....\n\n");
		print_help(1);
	}

   	WIFI_TEST_init();
   	unsigned ChipID;
   	bool retval = false;
   	retval = WIFI_TEST_GetChipID(&ChipID);
   	if(!retval) {
   	    printf("!!Get CHIP ID fail!! \n");
   	    exit(1);
   	}

    	if(ChipID == CHIP_7663) {
   	    strcpy(EEPROM_PATH,EEPROM_PATH_7663);
   	    strcpy(EEPROM_PATH_TMP,EEPROM_PATH_TMP_7663);
   	    strcpy(EEPROM_PATH_WRITE,EEPROM_PATH_WRITE_7663);
   	}

	// set up the Ctrl + C handler
	signal(SIGINT, signal_handler);
	while ((opt = getopt_long(argc, argv, short_option, long_options, &option_index)) != -1) {
		switch(opt) {
			case 0:
				/* long option */
				switch(long_flag) {
					case LONG_OP_TOOL_VERSION:
        			                Tool_version_flag = true;
						printf("Tool version %d.%d.%d\n", \
						SW_VERSION[0], SW_VERSION[1], SW_VERSION[2]);
						break;

					case LONG_OP_PWR_LIMIT:
						if (operation == OPER_NONE) {
							operation = SET_SINGLESKU;
							u4SingleSkuMode = atoi(optarg);
						}
                				else {
                   					printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    					return -1;
                				}
						break;
					case LONG_OP_SET_DPD:
						if (operation == OPER_NONE) {
							operation = SET_DPD;
							u4DPDMode = atoi(optarg);
						}
                				else {
                   					printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    					return -1;
                				}
						break;
					case LONG_OP_ISO_DETECT:
						SetIsoFlag = true;
						break;
        		                case LONG_OP_SET_ABSPOWER:
                                        	default_channel_power_Flag = false;
        		             	        abspowerFlag = true;
        		                        txGain = atof(optarg);
        		                        break;
                   			case LONG_OP_FW_VERSION:
                   			    	 if (operation == OPER_NONE) {
		   			    	 	operation = GET_FWversion;
		   			    	 }
                   			    	 else {
                   			    	    	printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                   			    	    	return -1;
                   			    	 }
                   			    	 break;
                    			case LONG_OP_DPD_CAL_LINK:
                        			if (operation == OPER_NONE) {
							operation = RECAL;
							u4CalMode = 128;
                            				CalIDindex = 7;
						}
                				else {
                   					printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    					return -1;
                				}
                   			        break;
                   			 case LONG_OP_CAL_COUNT:
                   				     CalCount = atoi(optarg);
                   				 break;

                   				 case LONG_OP_CAL_DUMP:
                   				     CalDump = atoi(optarg);
                   				 break;
#if CONFIG_SUPPORT_FFT
					/* Internal Capture */
					case LONG_OP_FFT_FUNCTION:
						if (operation == OPER_NONE) {
							operation = INTERNAL_CAPTURE;
						}
						else {
							printf("\n!! Wrong setting!!Conflict command is detected!!\n");
							return -1;
						}
						break;
#endif
#if CONFIG_SUPPORT_IWPRIV
					case LONG_OP_PRIV_FUNCTION:
						if (operation == OPER_NONE) {
							operation = PIRV_CMD;
							if(strlen(optarg) <  PRIV_CMD_SIZE){
								strcpy(privcmdBuf,optarg);
								privCmdLen = strlen(optarg);
							}
						}
						else {
							printf("\n!! Wrong setting!!Conflict command is detected!!\n");
							return -1;
						}
						break;
#endif
					case LONG_OP_TEMPERATURE:
						if (operation == OPER_NONE) {
							operation = QUERY_TEMPERATURE;
						}
						else {
							printf("\n!! Wrong setting!!Conflict command is detected!!\n");
							return -1;
						}
						break;
                   			 case LONG_OP_MPS_SET:
                   			       if (operation == OPER_NONE) {
                   			            operation = MPS_SET;
                   			       }
                   			       else {
                   			            printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                   			            return -1;
                   			       }
                   			 break;
                   			 case LONG_OP_MPS_ADD:
                   			       if (operation == OPER_NONE) {
                   			            operation = MPS_ADD;
                   			       }
                   			       else {
                   			            printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                   			            return -1;
                   			       }
                   			 break;
                   			 case LONG_OP_MPS_NUMBER:
                   			        MpsNum = atoi(optarg);
                   			 break;
                   			 case LONG_OP_MPS_START:
                   			     if (operation == OPER_NONE) {
                   			          operation = MPS_START;
                   			          g_MPS_flag = true;
                   			     }
                   			     else {
                   			          printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                   			          return -1;
                   			     }
                   			 break;
					case LONG_OP_ACK_MAC:{
						unsigned int MacAddress[6] = {0};

						result = sscanf(optarg, "%02x:%02x:%02x:%02x:%02x:%02x",
												&MacAddress[0], &MacAddress[1], &MacAddress[2],
												&MacAddress[3], &MacAddress[4], &MacAddress[5]);
						if (result != SCANF_ARG6) {
							printf("\n!! Wrong setting!! mac address format aa:bb:cc:dd:ee:ff !!\n");
							return -1;
						}
						AckMac[0] = (unsigned char)MacAddress[0];
						AckMac[1] = (unsigned char)MacAddress[1];
						AckMac[2] = (unsigned char)MacAddress[2];
						AckMac[3] = (unsigned char)MacAddress[3];
						AckMac[4] = (unsigned char)MacAddress[4];
						AckMac[5] = (unsigned char)MacAddress[5];
						break;
					 }
					case LONG_OP_HW_ACK:
						u4HwAckEnable = atoi(optarg);
						break;
                   			case LONG_OP_RECAL:
                   			   	 CalIDindex = searchCalID(22, optarg);
                   			   	 u4CalMode = CalID[CalIDindex];
                   			   	 //u4CalMode = optarg;
                   			   	 printf("calmode:%d\n\n", u4CalMode);
                   			   	 if (operation == OPER_NONE) {
                   			   	    		 operation = RECAL;
                   			   	    	}
                   			   	    	else {
                   			   	    		  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                   			   	    		  return -1;
                   			   	    	}
                   			   	 break;
                    			 case LONG_OP_INI: {
                    			        Ini_interface_flag = true;
                    			        char buff[200];
                    			        char ini_name[100];
                    			        char tmp_path[64];
                    			        FILE *fh;
                    			        strcpy(ini_name, optarg);
                    			       	fh = fopen(ini_name, "rb");
                    			        char tmp[200];
                    			    	if(fh != NULL) {
                    			    		while(fgets(buff, sizeof(buff), fh)) {
                    			                sscanf(buff, "%[^ ]", tmp);
                    			                if(strcmp(tmp, "Interface") == 0) {
                    			                    sscanf(buff, "%*s%s", tmp);
                    			                    strcpy(WIFI_IF_NAME, tmp);
                    			                }
		    			    	                    else if(strcmp(tmp, "EEPROM_version") == 0) {
                    			                    sscanf(buff, "%*s%s", tmp);
                    			                    sscanf(EEPROM_PATH, "%[^EEPROM]", tmp_path);
                    			                    strcat(tmp_path, tmp);
                    			                    strcpy(EEPROM_PATH, tmp_path);
                    			                    sscanf(EEPROM_PATH_TMP, "%[^EEPROM]", tmp_path);
                    			                    strcat(tmp_path, tmp);
                    			                    strcpy(EEPROM_PATH_TMP, tmp_path);
                    			                    sscanf(EEPROM_PATH_WRITE, "%[^EEPROM]", tmp_path);
                    			                    strcat(tmp_path, tmp);
                    			                    strcpy(EEPROM_PATH_WRITE, tmp_path);
		    			    	                    }
                    			            }
                    			        }
                    			        fclose(fh);
                    			    break;
                    			   }
                    			 case LONG_OP_DBDC:
                    			         tempDbdcBandIndex = atoi(optarg);
                    			         if (tempDbdcBandIndex < 2){
                    			             u4DbdcBandIndex = tempDbdcBandIndex;
                    			             u4Dbdc = true;
                    			             u4DefaultTxPath = 1;
                    			             //u4CalMode = optarg;
                    			             printf("DBDC band:%d enable\n\n", u4DbdcBandIndex);
                    			          }
                   				 break;
					default:
						/* debug long index*/
						printf("Not match long option_index = %d\n", option_index);
						break;
				}
				break;

			case 'e':
				if (operation == OPER_NONE) {
					operation = WRITE_EFUSE;
					xtoi(optarg, &efuse_addr);
					printf("efuse_addr = %x\n", efuse_addr);
				}
                		else {
                   			printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			return -1;
                		}
				break;
			case 'E':
				if (operation == OPER_NONE) {
					operation = READ_EFUSE;
					xtoi(optarg, &efuse_addr);
				}
                		else {
                    			printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			return -1;
               			}
				break;
			case 'w':
				if (operation == OPER_NONE) {
					operation = WRITE_MCR;
					xtoi(optarg, &mcr_addr);
				}
                		else {
                    			printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			return -1;
                		}
				break;
			case 'M':
				if (operation == OPER_NONE) {
					operation = READ_MCR;
					xtoi(optarg, &mcr_addr);
				}
                		else {
                    			printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			return -1;
                		}
				break;
			case 'r':
				if (operation == OPER_NONE) {
					operation = TEST_RX;
				}
                		else {
                    			printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			return -1;
                		}
				break;
			case 't':
               			 if(operation == MPS_ADD) {
               			     txMode = atoi(optarg);
               			 }
				else if (operation == OPER_NONE ) {
                    			t_flag = true;
					operation = TEST_TX;
					txMode = atoi(optarg);
				}
                		else {
                    			printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			return -1;
                		}
				break;
				/* query operation mode */
			case 'q': {
					  char *pToken = NULL;
					  char *pDelimiter = ",";
					  char *pSave = NULL;
					  int index = 0;

					  pToken = strtok_r(optarg, pDelimiter, &pSave);

					  while(pToken && (index < WLAN_CFG_ARGV_MAX)) {
						  apcArgv[index] = (signed char *) pToken;
						  pToken = strtok_r(NULL, pDelimiter, &pSave);
						  index++;
					  }
					  /*if (operation == OPER_NONE)
					    operation = QUERY_RESULT;*/
					  if (operation == OPER_NONE) {
						  if(check_string((signed char *)apcArgv[0])) {
							  operation = (Oper_Mode) atoi((const char *)apcArgv[0]);
						  }
					  }
					  printf("Set to Operation = %d\n", operation);
					  break;
				  }
			case 'u':
				  if (operation == OPER_NONE){
					  operation = WRITE_EEPROM;
					  xtoi(optarg, &eeprom_addr);
				  }
                  		  else {
                    			  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			  return -1;
                  		  }
				  break;
			case 'U':
				  if (operation == OPER_NONE){
					  operation = READ_EEPROM;
					  xtoi(optarg, &eeprom_addr);
				  }
                  		  else {
                  		 	  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			  return -1;
                  		  }
				  break;
			case 'g':
                  		  //g_flag = true;//for modulation checking in future wifi test tool2.0
				  gMode = !atoi(optarg) ? WIFI_TEST_PREAMBLE_TYPE_MIXED_MODE:WIFI_TEST_PREAMBLE_TYPE_GREENFIELD;
				  break;
			case 'G':
				  giType = !atoi(optarg) ? WIFI_TEST_GI_TYPE_NORMAL_GI:WIFI_TEST_GI_TYPE_SHORT_GI;
				  break;
			case 'I':
				  strcpy(WIFI_IF_NAME, optarg);
				  break;
			case 'F':
                  		  F_flag = true;
				  memset(EEPROM_PATH_WRITE, 0, sizeof(EEPROM_PATH_WRITE));
				  //printf("path name is 11 %s\n", EEPROM_PATH);
				  strcpy(EEPROM_PATH_WRITE, optarg);
				  //printf("path name is 22 %s\n", EEPROM_PATH);
				  //open_eeprom_file();
				  break;
			case 'J':
				  printf("i-cal mode \n");
				  if (operation == OPER_NONE){
					  operation = I_CAL;
					  buffer_mode_content_source = atoi(optarg);
				  }
                  		  else {
                    			  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			  return -1;
                  		  }
				  break;
			case 'K':
				  if (operation == OPER_NONE) {
					  operation = WRITE_EEPROM_TO_EFUSE;
				  }
                  		  else {
                    		  	  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			  return -1;
                  		  }
				  break;
			case 'W':
				  if (operation == OPER_NONE) {
					  operation = SET_TX_POWER_COMPENSATION;
					  xtoi(optarg, &tx_power_compensation);
				  }
                  		  else {
                    			  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			  return -1;
                  		  }
				  break;
			case 'Q':
				  //if (operation == OPER_NONE){
				  //operation = SET_RX_PATH;
				  xtoi(optarg, &rx_path);
				  //}
				  break;
			case 'y':
				  //if (operation == OPER_NONE){
				  //operation = SET_TX_PATH;
				  xtoi(optarg, &u4DefaultTxPath);
				  //}
				  break;
			case 'Y':
				  u4Cbw = atoi(optarg);
				  break;
			case 'X':
                                 X_flag = true;
				  open_eeprom_file(); //read from tmp buffer
				  saveEEPROM();
				  break;
			case 'B':
				  {
				  	int index = atoi(optarg);
					if (index > WIFI_TEST_BW_MAX){
						printf("not support this band");
						return -1;
					}
					g_bandwidth = band_width[index];
					break;
				  }
			case 'N':
				  gMCSrate = atoi(optarg);
				  break;
			case 'R':
				  g_rate = atoi(optarg);
				  break;
			case 'i':
				  SIFS = atoi(optarg);
				  break;
			case 'p':
				  default_channel_power_Flag = false;
                                  abspowerFlag = false;
				  txGain = atof(optarg);
				  SetBTTxFlag = true;
				  BTTxPower = atof(optarg);
				  break;
			case 'l':
				  payloadLength = atoi(optarg);
				  break;
			case 'b':
				  rfBw = atoi(optarg);
				  isNewBwSet = true;
				  break;
			case 'j':
				  jModeSetting = atoi(optarg);
				  break;
			case 'P':
				  priSetting = atoi(optarg);
				  break;
			case 'x':
				  txBw = atoi(optarg);
				  isTxBwSet = true;
				  break;
			case 'L':
				  coding = 1;
				  break;
			case 'd':
				  rxDefaultAnt = atoi(optarg);
				  break;
			case 'a':
				  printInterval = atoi(optarg);
				  break;
			case 'o':
				  maxPktCount = atoi(optarg);
				  break;
			case 'h':
			case ':':
				  print_help(0);
				  break;
			case 'n':
				  times = atoi(optarg);
				  break;
			case 'c':
				  channel = atoi(optarg);
				  break;
			case 'V':
				  if (operation == OPER_NONE){
					  operation = SET_FREQ_OFFSET;
					  xtoi(optarg, &freq_offset);
				  }
                  		  else {
                    			  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                    			  return -1;
                  		  }
				  break;
			case 'v':
				  xtoi(optarg, &mcr_value);
				  break;
			case 's':
                  //s_flag = true; //for modulation checking in future wifi test tool 2.0
				  pType = !atoi(optarg) ? WIFI_TEST_PREAMBLE_SHORT:WIFI_TEST_PREAMBLE_LONG;
				  break;
			case 'm':
				  cw_mode = atoi(optarg);
				  break;
			case '?':
				  fprintf(stderr, "%s: Error - No such option: `%c`\r", proc_name, optopt);
				  print_help(1);
				  break;
			case 'S':
				  sleep_time = atoi(optarg);
				  sleepModeSet = true;
				  break;
			case 'O':
                  		  O_flag = true;
				  user_expect = 1;
				  break;
			case 'C':
                  		  //C_flag = true;
				  //open_eeprom_file();
				  break;
			case 'T':
				  if (operation == OPER_NONE){
				          operation = TEST_STOP;
                  		  }
                  		  else{
                     	 	          printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                      			  return -1;
                  		  }
				  break;
			case 'D':
				  fgDebugMode = true;
				  break;
			case 'f':
				  eRxOkMatchRule = atoi(optarg);
				  break;
			case '#':
				  //if (operation == OPER_NONE){
				  //    operation = SET_NSS;  // add one more operaton?
				  //xtoi(optarg, &NSS);
				  NSS = atoi(optarg);
				  printf("NSS = %d, %x\n", NSS, NSS);
				  //}
				  break;

			case 'A':
				  result = xtoAddrptr(optarg, aucRxFilterMacAddr);
				  if (!result) {
					  printf("Address format doesn't support\n");
					  return -1;
				  }
				  else {
					  bRxFilterMacAddrLegalFg = true;
				  }
				  break;
			case 'k':
				  result = xtoAddrptr(optarg, macAddr);
				  if (!result) {
					  printf("Address format doesn't support\n");
					  return -1;
				  }
				  break;
			case 'z':
				  if (operation == OPER_NONE){
					  int values[6];
					  int idx;
					  char mac_type;

					  if (strlen(optarg)!=sizeof("B 00:11:22:33:44:55")-1) {
						  printf("Invalid input format (%s)\n", optarg);
						  return -1;
					  }

					  mac_type = optarg[0];
					  if (mac_type != 'W' && mac_type != 'B') {
						  printf("Invalid target type (%c)\n", mac_type);
						  return -1;
					  }

					  if (mac_type == 'W') {
						  operation = WRITE_EFUSE_MAC_WIFI;
					  }
					  if (mac_type == 'B') {
						  operation = WRITE_EFUSE_MAC_BT;
					  }

					  /* parse MAC address string */
					  if( 6 != sscanf( optarg+2, "%x:%x:%x:%x:%x:%x",
								  &values[0], &values[1], &values[2],
								  &values[3], &values[4], &values[5] ) ) {
						  printf("Cannot parse mac address (%s)\n", optarg+2);
						  return -1;
					  }

					  /* convert to uint8_t */
					  for(idx = 0; idx < 6; ++idx) {
						  efuse_write_mac_bytes[idx] = (uint8_t) values[idx];
					  }
				  }
                  		  else{
                      			  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                      			  return -1;
                 	 	  }
				  break;
			case 'Z':
				  if (operation == OPER_NONE){
    				  	  operation = READ_EFUSE_FREE_BLOCK;
                  		  }
                  		  else{
                      			  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                      			  return -1;
                  		  }
				  break;
			case 'H':
				  if (operation == OPER_NONE){
					  //operation = DUMP_EFUSE_ALL;
					  current_mode = atoi(optarg);
					  printf("current_mode is %d\n", current_mode);

					  WIFI_TEST_init();

					  if (current_mode == 1) { //dump from efuse
						  operation = DUMP_EFUSE_ALL;
						  break;
					  }
					  else if (current_mode == 2) {//dump from tmp eeprom
					      operation = DUMP_EEPROM_ALL;
                          break;
					  }
					  else {
						  printf("Not Support this mode!!\n");
						  break;
					  }
				  }
                  		  else{
                      			  printf("\n!! Wrong setting!!Conflict command is detected!!\n");
                      			  return -1;
                  		  }
				  break;
			case '$':
				  u4DefaultCalFreqOffset = atoi(optarg);
				  break;
			/* Single Tone */
			case '*':
				  printf("-* %s\n", optarg);
				  int ret = 0;
				  ret = sscanf(optarg, "%d-%d-%d-%d-%d-%d",
						  &u4ToneFreq, &u4ToneType, &u4ToneRfGain,
						  &u4ToneDigitalGain, &u4ToneDcOffsetI, &u4ToneDcOffsetQ);

				  if (ret != TONE_ARG) {
					  return -1;
				  }
				  break;
			default:
				  printf("No case meet !!!\n");
				  break;
		}
	}

   	if (operation==OPER_NONE && Ini_interface_flag==false && O_flag==false && Tool_version_flag==false && F_flag==false && X_flag == false) { /*for wrong cmd setting report*/
        	printf("\n!!wrong CMD setting!!\n");
        	print_help(1);
        	return -1;
        }
   	WIFI_TEST_init();

   	retval = WIFI_TEST_GetChipID(&ChipID);
   	if(!retval) {
   	    printf("!!Get CHIP ID fail!! \n");
   	    exit(1);
   	}

    	if(ChipID == CHIP_7663) {
   	    strcpy(EEPROM_PATH,EEPROM_PATH_7663);
   	    strcpy(EEPROM_PATH_TMP,EEPROM_PATH_TMP_7663);
   	    strcpy(EEPROM_PATH_WRITE,EEPROM_PATH_WRITE_7663);
   	}


#if 0   /*for modulation checking in the future wifi test tool 2.0*/
        if (t_flag) { /*check preamble setting*/
            if (txMode==0) {
                if (g_flag || !s_flag) {
                    printf("\n!!wrong preamble setting!!\n");
                    return -1;
                }
                if (pType < 0) {
                    return -1;
                }
            }
            else if(txMode==1) {
                if (s_flag || !g_flag) {
                    printf("\n!!wrong preamble setting!!\n");
                    return -1;
                }
            }
            else {
                /*ac is nothing to check*/
            }
        }
#endif
    /*DBDC part setting*/
    if (u4Dbdc) {
        if (u4DefaultTxPath != 1) {
            printf("!!DBDC mode TX must be WF0, tool will change your path setting to WF0!!\n");
            u4DefaultTxPath = 1;
        }

        if (u4DbdcBandIndex == 0 && channel < 36 && operation != TEST_STOP){
            printf("!!DBDC 0 must be 5G channel, please key in again!!\n");
            exit(1);
        }
        else if (u4DbdcBandIndex == 1 && channel > 14 && operation != TEST_STOP){
            printf("!!DBDC 1 must be 2G channel, please key in again!!\n");
            exit(1);
        }
        else{
            //do nothing
        }
    }

	/* Decide RF/Tx bandwidth */
	if(isNewBwSet) {
		if(!isTxBwSet) {
			txBw = rfBw;
		}
		else if(txBw > rfBw) {
			txBw = rfBw;
		}
	}
	else if(isTxBwSet) {
		isNewBwSet = true;
		rfBw = txBw;
	}

	/* BW coding check */
	if((txBw >= WIFI_TEST_CH_BW_NUM) || (rfBw >= WIFI_TEST_CH_BW_NUM)) {
		printf("Invalid bandwidth setting RF[%u] Tx[%u]", rfBw, txBw);
		return -1;
	}

    /* BW spec coding check */
    	if (t_flag) {
       	        switch (txMode){
       	            case 0: /* a/b/g BW > 20 */
       	                if (g_bandwidth > WIFI_TEST_CH_BW_20MHZ) {
       	                    printf("\n!!Invalid bandwidth setting RF[%u]!!\n", g_bandwidth);
       	        	            return -1;
       	                }
       	                break;
       	            case 1: /* n  BW > 40 */
       	                if (g_bandwidth > WIFI_TEST_CH_BW_40MHZ) {
       	                    printf("\n!!Invalid bandwidth setting RF[%u]!!\n", g_bandwidth);
       	        	            return -1;
       	                }
       	                break;
       	            case 2:
       	                   /* ac */
       	                break;
       	            default:
       	                printf("\n Invalid txMode setting \n");
       	                return -1;
       	        }
         }

    /* channel spec coding check*/
   	if (channel >= 73 && channel <= 94) {
   	    printf("\n\n!! illegal channel:%d setting of Group3, please use legal channel !!\n\n", channel);
   	    return -1;
   	}
   	if (channel > 202) {
   	    printf("\n\n!! illegal channel:%d setting, please use legal channel !!\n\n", channel);
            return -1;
   	}

	wlan_status = wifiStatus();
	switch (wlan_status) {
		case WLAN_MODE_OFF:
			if ((user_expect & 0x1) == 1) {
				bool ret = false;
				if ((user_expect & 0x2)==0x2 && operation == OPER_NONE) {
					return 0;
				}
				ret = WIFI_TEST_OpenDUT();
				printf("[%s] Enable Wi-Fi test mode %s\n", WIFI_IF_NAME, ret==true ? "success":"fail");
				if (ret == true) {
					wlan_status = TEST_MODE_ON;
					break;
				}
			}
			printf("[%s] Not in test mode, use -O to enable.\n", WIFI_IF_NAME);
			return 0;
		case TEST_MODE_ON:
			if ((user_expect & 0x1) == 1) {
				unsigned int free_block;
				bool retval = false;
				unsigned char counter = 10;
                       		unsigned ChipID;
                		int FOR_RX_query;/*clear MIB count*/
				do {
					if (WIFI_TEST_OpenDUT() == false) {
						usleep(100*1000);
						continue;
					}

					retval = WIFI_TEST_GetFreeEfuseBlock(&free_block);
					if (free_block==0 || !retval) { // result is something wrong. Retry again
						usleep(100*1000);
						continue;
					}
					// Step here means init is fully ready. Stop retry process
					break;
				} while (counter--);
				if (counter<=0 || !retval) {
					printf("[%s] !!Test mode init fail!! \n Please using -I to correct wlan interface\n\n", WIFI_IF_NAME);
					exit(1);
				}

		                retval = WIFI_TEST_GetChipID(&ChipID);
                       		if(!retval) {
                       		    printf("!!Get CHIP ID fail!! \n");
                       		    exit(1);
                       		}
		                if(ChipID == CHIP_7663) {
		                    strcpy(EEPROM_PATH,EEPROM_PATH_7663);
		                    strcpy(EEPROM_PATH_TMP,EEPROM_PATH_TMP_7663);
		                    strcpy(EEPROM_PATH_WRITE,EEPROM_PATH_WRITE_7663);
		                }
		                if (access(EEPROM_PATH, R_OK)==0)
				{
				        int source = open(EEPROM_PATH, O_RDONLY, 0);
				        int dest = open(EEPROM_PATH_TMP, O_RDWR | O_CREAT, 0777);
				        struct stat stat_source;
				        fstat(source, &stat_source);

				        sendfile(dest, source, 0, stat_source.st_size);

				        close(source);
				        close(dest);
				        fprintf(stderr, "copy from %s to %s\n", EEPROM_PATH, EEPROM_PATH_TMP);
				        if (access(EEPROM_PATH_TMP, R_OK|W_OK)!=0) {
				      	  fprintf(stderr, "%s is not ready\n", EEPROM_PATH_TMP);
				      	  exit(1);
				        }
				        fprintf(stderr, "%s is ready\n", EEPROM_PATH_TMP);
				}
				else {
				        fprintf(stderr, "%s is not valid\n", EEPROM_PATH);
					exit(1);
				}
                		WIFI_TEST_FRGood((int*)&FOR_RX_query);/*clear MIB count in test mode*/
				//printf("[%s] Already in test mode\n", WIFI_IF_NAME);
			}
			break;

		case NORMAL_MODE_ON:
			printf("Please turn off normal mode wlan first!\n");
			return 0;

		default:
			printf("No case meet !!!\n");
			break;
	}

	/* J mode setting */
	if(jModeSetting) {
		bool retval = false;
		retval = WIFI_TEST_SetJMode(jModeSetting);
		printf("(%s) Set J mode to %d\n", retval ? "success":"fail", jModeSetting);
		if (retval == 0) return -1;
	}

	switch (operation) {
		case READ_EFUSE_FREE_BLOCK:
			{
				unsigned int free_block;
				bool retval = WIFI_TEST_GetFreeEfuseBlock(&free_block);
				printf("(%s) Read EFUSE Free block 0x%02X\n", retval ? "success":"fail", free_block);
				break;
			}
		case WRITE_EFUSE_MAC_WIFI:
			{
				bool retval = WIFI_TEST_MAC_Write(0x4, efuse_write_mac_bytes, sizeof(efuse_write_mac_bytes));
				printf("(%s) Write EFUSE WIFI MAC\n", retval ? "success":"fail");
				break;
			}
		case WRITE_EFUSE_MAC_BT:
			{
                bool retval;

                //TODO: replace BT address efuse offset by API to FW which may be different per project
                if(ChipID == CHIP_7663){
                    retval = WIFI_TEST_MAC_Write(0x131, efuse_write_mac_bytes, sizeof(efuse_write_mac_bytes));
                }
                else if (ChipID == CHIP_7668) {
				    retval = WIFI_TEST_MAC_Write(0x384, efuse_write_mac_bytes, sizeof(efuse_write_mac_bytes));
                }
                else{
                    printf("chip ID not support 0x%08x\n", ChipID);
                }

				printf("(%s) Write EFUSE BT MAC\n", retval ? "success":"fail");
				break;
			}
		case WRITE_EFUSE:
			{
                		if(mcr_value == -10000) {
                    			printf("!!Wrong setting!! Please use -v to key in value \n");
                    			return -1;
                		}
				bool retval = WIFI_TEST_EFUSE_Write(efuse_addr, mcr_value);
				printf("(%s) Wirte EFUSE addr 0x%x value 0x%x\n", retval ? "success":"fail", efuse_addr, mcr_value);
				break;
			}
		case READ_EFUSE:
			{
				unsigned int val = 0;
				bool retval = WIFI_TEST_EFUSE_Read(efuse_addr, &val);
				printf("(%s) EFUSE addr 0x%x value 0x%02x\n", retval ? "success":"fail", efuse_addr, val);
				break;
			}
		case WRITE_EEPROM:
			{
                		if(mcr_value == -10000) {
                   			printf("!!Wrong setting!! Please use -v to key in value \n");
                    			return -1;
                		}
				open_eeprom_file();
				bool retval = WIFI_TEST_EEPROM_Write(eeprom_addr, mcr_value);
				printf("(%s) Wirte EEPROM addr 0x%x value 0x%02x\n", retval ? "success":"fail", eeprom_addr, mcr_value);
				break;
			}
		case READ_EEPROM:
			{
				unsigned int val = 0;
				open_eeprom_file();
				bool retval = WIFI_TEST_EEPROM_Read(eeprom_addr, &val);
				printf("(%s) EEPROM addr 0x%x value 0x%02x\n", retval ? "success":"fail", eeprom_addr, val);
				break;
			}
		case I_CAL:
			{
                		/*if (C_flag == false && buffer_mode_content_source == SOURCE_EEPROMBIN) {
                    			printf("\n!!wrong setting without -C !!\n");
                    			return -1;
                		}*/
				unsigned int val = 0;
   	            		int delay_counter = 0;
				printf("buffer_mode_content_source = %d\n", buffer_mode_content_source);
				if (buffer_mode_content_source == SOURCE_AUTO) {
					/* Read address 0x00 to check it is a re-cal ic or not */
					bool retval = WIFI_TEST_EFUSE_Read(0x00, &val);
					if (retval) {
						printf("0x00 is %x\n", val);

						if(val != 0x68)  // non-cal I-cal IC
							buffer_mode_content_source = SOURCE_EEPROMBIN;
						else  //re-cal IC
							buffer_mode_content_source = SOURCE_EFUSE;
					}
				}

				if (buffer_mode_content_source == SOURCE_EEPROMBIN) {
					printf("Source from eeprom bin\n");
					open_eeprom_file();

					// 2017.03.16 We don't need to merget the i-cal fields anymore, due to we use patch to protect them.
					//replaceEEPROM();

					/* 1. Set to eeprom bin mode */
					WIFI_TEST_set_Eeprom_Mode(BUFFER_BIN_MODE);

					/* 2. Update the contents in driver's eeprom table from EEPROM path */
					/* 1024/16 = 64*/
					for (index=0; index<(MAX_EEPROM_BUFFER_SIZE/EFUSE_BYTES_PER_LINE); index++) {
						WIFI_TEST_set_HQA(index*EFUSE_BYTES_PER_LINE, index, uacEEPROMImage+index*EFUSE_BYTES_PER_LINE);
					}

					/* 3. Send buffer mode CMD through driver to FW */
					WIFI_TEST_set_Efuse_Buffer_Mode(1);  //1 // 1: buffer mode CMD source from eeprom.bin
					usleep(15 * 1000);
                   			WIFI_TEST_MCR_Read(CHIPID_CR, &val);
                   			while(val == 0) {
                   			    if(delay_counter > 5000)
                   			    {
                   			        printf("!!TX initialize fail!!\n");
                   			        exit(0);
                   			    }
                   			    delay_counter++;
                   			    WIFI_TEST_MCR_Read(CHIPID_CR, &val);
                   			    usleep(1000);
                   			}
                   			val = 0;
				}
				else if (buffer_mode_content_source == SOURCE_EFUSE) {
					printf("Source from efuse\n");
					WIFI_TEST_set_Efuse_Buffer_Mode(0);  //1 // 0: buffer mode CMD source from EFUSE
				}
				else {
					printf("Not Support!!!!\n");
					break;
				}

				break;
			}
		case WRITE_EEPROM_TO_EFUSE:
			{
                		/*if (C_flag == false) {
                    			printf("\n!!wrong setting without -C !!\n");
                    			return -1;
                		}*/
                		open_eeprom_file();
				/* Set to efuse mode */
				WIFI_TEST_set_Eeprom_Mode(EFUSE_MODE);
                		unsigned int free_block;
				for (index=0; index<(MAX_EEPROM_BUFFER_SIZE/EFUSE_BYTES_PER_LINE); index++) {  /* 1024/16 = 64*/
                    			free_block = 0;

    					WIFI_TEST_GetFreeEfuseBlock(&free_block);
                   			if (free_block==0) {//no free block
                   			    break;
                   			}

					WIFI_TEST_set_HQA(index*EFUSE_BYTES_PER_LINE, index, (unsigned char *)(uacEEPROMImage+index*EFUSE_BYTES_PER_LINE)); 
				}

               			if (free_block) {
               			    printf("!!Write Efuse success!!\n");
               			}
               			else {
               			    printf("!!Write Efuse fail!!\n");
               			}
				break;
			}
		case SET_RX_PATH:
			{
				if (rx_path == 0x1) { /*WF0*/
					u4DefaultRxPath = 0x00010000;
				}
				else if (rx_path == 0x2) {/*WF1*/
					u4DefaultRxPath = 0x00020000;
				}
				else if (rx_path == 0x3) {/*WF0 + WF1*/
					u4DefaultRxPath = 0x00030000;
				}
				else {
					printf("RX path not support!!!\n");
					break;
				}

				bool retval = WIFI_TEST_SetRxPath(u4DefaultRxPath);
				printf("(%s) RX path is set to value 0x%x\n", retval ? "success":"fail", rx_path);
				break;
			}
		case SET_TX_PATH:
			{
				bool retval = WIFI_TEST_SetTxPath(u4DefaultTxPath);
				printf("(%s) TX path is set to value 0x%x\n", retval ? "success":"fail", u4DefaultTxPath);
				break;
			}

		case SET_FREQ_OFFSET:
			{
				bool retval = WIFI_TEST_SetFreqOffset(freq_offset);
				printf("(%s) Freq Offset is set to value 0x%x\n", retval ? "success":"fail", freq_offset);

				if(u4DefaultCalFreqOffset == 0) {
					/* Do Nothing */
				}
				else if(u4DefaultCalFreqOffset == 1) {
					/* Code Compenstaion Enable bit */
					if(freq_offset & BIT(7)) {
						uint32_t offset_value, default_offset_value;
						unsigned int compare_addr = eeprom_addr;
						/* Get the Freq. Offset Default Absolute value */
						open_eeprom_file();
						retval = WIFI_TEST_GetCalFreqOffsetEepromAddr(0x0, &eeprom_addr);
						printf("(%s) Get Freq Offset EEPROM addr 0x%x \n", retval ? "success":"fail", eeprom_addr);
						retval = (compare_addr == eeprom_addr) ? 1 : 0;
						if(retval) {
							printf("(%s) Not compatible with firmware, Please update the newset firmware!!\n", retval?"fail":"success");
							break;
						}
						retval = WIFI_TEST_EEPROM_Read(eeprom_addr, &default_offset_value);
						printf("(%s) Get Freq Offset 0x%x value 0x%02x\n", retval ? "success":"fail", eeprom_addr, default_offset_value);

						/* Set the Freq. Offset Compenstation value */
						offset_value = freq_offset & BITS(0,6);
						default_offset_value &= BITS(0,6);

						if(default_offset_value >= offset_value) {
							/* - compensation */
							freq_offset = default_offset_value - offset_value;
							if(freq_offset > 0x3F) {
								printf("(fail) Invalid Compensation value\n");
								break;
							}
							freq_offset |= BIT(6);
						}
						else {
							/* + compensation */
							freq_offset = offset_value - default_offset_value;
							if(freq_offset > 0x3F) {
								printf("(fail) Invalid Compensation value\n");
								break;
							}
							freq_offset &= ~BIT(6);
						}

						/* Set default freq_offset enable bit */
						freq_offset |= BIT(7);
					}
					else {
						printf("(fail) Invalid Freq Offset value 0x%02x\n", freq_offset);
						break;
					}
				}
				else {
					printf("(fail) Set Freq Offset not support!!!\n");
					break;
				}

				unsigned int cal_freq_addr = 0;
				unsigned int last_cal_freq_addr = cal_freq_addr;
				/* Get eeprom value */
				retval = WIFI_TEST_GetCalFreqOffsetEepromAddr(u4DefaultCalFreqOffset ,&cal_freq_addr);
				printf("(%s) Freq Offset Calibration get eeprom address 0x%x\n", retval ? "success":"fail", cal_freq_addr);
				retval = (cal_freq_addr == last_cal_freq_addr) ? 1 : 0;
				if(retval) {
					printf("(%s) Not compatible with firmware, Please update the newset firmware!!\n", retval?"fail":"success");
					break;
				}
				/* Write eeprom value */
				retval = WIFI_TEST_EEPROM_Write(cal_freq_addr, freq_offset);
				printf("(%s) Freq Offset Calibration Write EEPROM addr 0x%x value 0x%x\n", retval? "success":"fail", cal_freq_addr, freq_offset);
				break;
			}

		case SET_NSS:
			{
				bool retval = WIFI_TEST_SetNss(NSS);
				printf("(%s) Nss is set to value 0x%x\n", retval ? "success":"fail", NSS);
				break;
			}

		case SET_TX_POWER_COMPENSATION:
			{
				/* we use bit data to tranfser,
				   [31:24]:Reserved
				   [23:20]: Tx0/Tx1
				   [19:18]: 2G/5G
				   [17:8]: Channel#
				   [7:0]: channelpower compensataion */

				/* For Get EEPROM address subtype*/
				tx_power_compensation_data |= BIT(24);
				tx_power_compensation_data |= tx_power_compensation;
				tx_power_compensation_data |= channel << 8;

				if (channel > 14) //5G
					tx_power_compensation_data |= BIT(18);


				bool retval = 0;

				if (u4DefaultTxPath == 0x3) { // Tx0 + Tx1

					/* Tx0 */
					/* Set Tx0 Channel Compensation */
					retval = WIFI_TEST_SetTxPowerCompensation(tx_power_compensation_data);
					printf("(%s) Tx0 power compensation is set to value 0x%x\n", retval ? "success":"fail", tx_power_compensation_data);

					/* Tx1 */
					/* Set Tx1 Channel Compensation */
					tx_power_compensation_data |= BIT(20);
					retval = WIFI_TEST_SetTxPowerCompensation(tx_power_compensation_data);
					printf("(%s) Tx1 power compensation is set to value 0x%x\n", retval ? "success":"fail", tx_power_compensation_data);
				}
				else {
					if (u4DefaultTxPath == 0x2) //Tx1
						tx_power_compensation_data |= BIT(20);
					else if (u4DefaultTxPath == 0x1) //Tx0
						tx_power_compensation_data &= ~BIT(20);

					/* Set Tx Channel Compensation */
					retval = WIFI_TEST_SetTxPowerCompensation(tx_power_compensation_data);
					printf("(%s) Tx power compensation is set to value 0x%x\n", retval ? "success":"fail", tx_power_compensation_data);
				}


				unsigned int val = 0;
				bool retval3=0, retval2=0;

				if (u4DefaultTxPath == 0x3) { // Tx0 + Tx1

					/* Tx0 */
					/* Get eeprom address */
					tx_power_compensation_data &= ~BIT(20);
					retval2 = WIFI_TEST_GetTxPowerCompensationEepromAddr(tx_power_compensation_data ,&val);
					printf("(%s) Tx0 power get eeprom address 0x%x\n", retval2 ? "success":"fail", val);

					/* Write eeprom valuse */
					retval3 = WIFI_TEST_EEPROM_Write(val, tx_power_compensation);
					printf("(%s) Tx0 Wirte EEPROM addr 0x%x value 0x%x\n", retval3 ? "success":"fail", val, tx_power_compensation);

					/* Tx1 */
					/* Get eeprom address */
					tx_power_compensation_data |= BIT(20);
					retval2 = WIFI_TEST_GetTxPowerCompensationEepromAddr(tx_power_compensation_data ,&val);
					printf("(%s) Tx1 power get eeprom address 0x%x\n", retval2 ? "success":"fail", val);

					/* Write eeprom valuse */
					retval3 = WIFI_TEST_EEPROM_Write(val, tx_power_compensation);
					printf("(%s) Tx1 Wirte EEPROM addr 0x%x value 0x%x\n", retval3 ? "success":"fail", val, tx_power_compensation);
				}
				else {
					if (u4DefaultTxPath == 0x2) //Tx1
						tx_power_compensation_data |= BIT(20);
					else if (u4DefaultTxPath == 0x1) //Tx0
						tx_power_compensation_data &= ~BIT(20);

					/* Get eeprom address */
					retval2 = WIFI_TEST_GetTxPowerCompensationEepromAddr(tx_power_compensation_data ,&val);
					printf("(%s) power get eeprom address 0x%x\n", retval2 ? "success":"fail", val);

					/* Write eeprom valuse */
					retval3 = WIFI_TEST_EEPROM_Write(val, tx_power_compensation);
					printf("(%s) Wirte EEPROM addr 0x%x value 0x%x\n", retval3 ? "success":"fail", val, tx_power_compensation);
				}
				break;
			}
		case WRITE_MCR:
			{
                		if(mcr_value == -10000) {
                    			printf("!!Wrong setting!! Please use -v to key in value \n");
                    			return -1;
                		}
				bool retval = WIFI_TEST_MCR_Write(mcr_addr, mcr_value);
				printf("(%s) MCR addr 0x%x is set to value 0x%x\n", retval ? "success":"fail", mcr_addr, mcr_value);
				break;
			}
		case READ_MCR:
			{
				unsigned int val = 0;
				bool retval = WIFI_TEST_MCR_Read(mcr_addr, &val);
				printf("(%s) MCR addr 0x%x value 0x%x\n", retval ? "success":"fail", mcr_addr, val);
				break;
			}
		case TEST_RX:
			{
				int testDuration;

				if(sleepModeSet) {
					testDuration = sleep_time;
				}
				else {
					testDuration = times;
				}

				wifi_sensitivity(testDuration, channel);
				break;
			}
		case TEST_TX:
			wifi_tx();
			break;
		case TEST_STOP:
			wifiTestStop();
            		WIFI_TEST_SetRateOffset();//restore abspower
			break;

		case QUERY_RESULT:
			wifiGetResult();
			break;

		case DUMP_EFUSE_ALL:
			{
				int index;
				/* 1. Set to eeprom efuse mode */
				WIFI_TEST_set_Eeprom_Mode(EFUSE_MODE);

				/* 2. Read back all the efuse contents */
				/* 1024/16 = 64*/
				for (index=0; index<64 ; index++) {
					WIFI_TEST_get_HQA(index*EFUSE_BYTES_PER_LINE, index, uacEEPROMImage+index*EFUSE_BYTES_PER_LINE);
				}

				printf("       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n");
				for (index=0; index<MAX_EEPROM_BUFFER_SIZE; index+=16) {
					printf("%3x    %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x\n", index,
							uacEEPROMImage[index]  ,  uacEEPROMImage[index+1],  uacEEPROMImage[index+2],   uacEEPROMImage[index+3],
							uacEEPROMImage[index+4],  uacEEPROMImage[index+5],  uacEEPROMImage[index+6],   uacEEPROMImage[index+7],
							uacEEPROMImage[index+8],  uacEEPROMImage[index+9],  uacEEPROMImage[index+10], uacEEPROMImage[index+11],
							uacEEPROMImage[index+12], uacEEPROMImage[index+13],  uacEEPROMImage[index+14], uacEEPROMImage[index+15]);
				}
                break;
			}
        	case DUMP_EEPROM_ALL:
        	   	 {
        	   	    	 open_eeprom_file();
		   	    	 	    printf("Dump from eeprom:\n");
        	   	    	 printf("       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n");
    		   	    	     for (index=0; index<MAX_EEPROM_BUFFER_SIZE; index+=16) {
    		   	    	 		 printf("%3x    %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x\n", index,
    		   	    	 	     		uacEEPROMImage[index]  ,  uacEEPROMImage[index+1],  uacEEPROMImage[index+2],   uacEEPROMImage[index+3],
    		   	    	 			    uacEEPROMImage[index+4],  uacEEPROMImage[index+5],  uacEEPROMImage[index+6],   uacEEPROMImage[index+7],
    		   	    	 			    uacEEPROMImage[index+8],  uacEEPROMImage[index+9],  uacEEPROMImage[index+10], uacEEPROMImage[index+11],
    		   	    	 			    uacEEPROMImage[index+12], uacEEPROMImage[index+13],  uacEEPROMImage[index+14], uacEEPROMImage[index+15]);
    		   	    	     }
        	   	    	 break;
        	   	 }
		case SINGLE_TONE :
			{
				wifi_single_tone();
				break;
			}

		case CONTINUOUS_WAVE :
			{
				break;
			}

		case SET_TSSI:
			{
				if (check_string(apcArgv[1])) {
					bool retval = false;
					int TssiEnable = atoi((const char *)apcArgv[1]);
					/* Set TSSI mode */
					if (TssiEnable == 1 || TssiEnable == 0) {
						retval = WIFI_TEST_SET_TSSI(TssiEnable);
						printf("(%s) Set TSSI mode to %d\n", retval ? "success":"fail", TssiEnable);
						if (retval == 0) return 0;
					}
					else {
						printf("No TSSI Arguments! Please Check the input value\n");
					}
				}
				else {
					printf("No TSSI Arguments! Please Check the input value\n");
				}
				break;
			}
		case SET_SINGLESKU:
			{
				WIFI_TEST_DisSingleSku(u4SingleSkuMode);
				break;
			}
		case SET_DPD:
			{
				WIFI_TEST_SET_DPD(u4DPDMode);
				break;
			}
       		case GET_FWversion:
       			{
   	    		        int FW_version, first, second, third;
   	    		        WIFI_TEST_GetFWVersion(&FW_version);
   	    		        third = FW_version%100;
   	    		        FW_version/=100;
   	    		        second = FW_version%100;
   	    		        FW_version/=100;
   	    		        first = FW_version;
   	    		        printf("FW version %d.%d.%d \n",first, second, third);
  				break;
       		        }
           	case RECAL:
           	    {
           	       	    int loop = 0;
           	       	    unsigned int val = 0;
       	   	    	    bool retval = WIFI_TEST_SetTxPath(u4DefaultTxPath);
       	   	    	    printf("(%s) TX path is set to value 0x%x\n", retval ? "success":"fail", u4DefaultTxPath);
       	   	    	    retval = WIFI_TEST_Channel(channel);
       	   	    	    printf("(%s) Set central channel number to %d\n", retval ? "success":"fail", channel);
       	   	    	    retval = WIFI_TEST_SetBandwidthV2(rfBw);
       	   	    	    printf("(%s) Set RF bandwidth to %s\n", retval ? "success":"fail",bandwidthV2[rfBw]);
       	   	    	    retval = WIFI_TEST_SetDBDCBand(u4DbdcBandIndex);/* always 0 */
       	   	    	    retval = WIFI_TEST_SetRECAL_Dump_Control(CalDump);
       	   	    	    //printf("(%s) u4DbdcBandIndex is set to value 0x%d\n", retval ? "success":"fail", u4DbdcBandIndex);

       	   	    	    for(loop = 0; loop < CalCount; ++loop) {
       	   	    	        retval = WIFI_TEST_SetRECAL(u4CalMode);
       	   	    	        printf("(%s) Calibration mode is set to %s, caltime:%d  \n", retval ? "success":"fail",RecalType[CalIDindex], loop+1);
           	           	usleep(15*1000);
           	           	while(val == 0) {
                                    WIFI_TEST_MCR_Read(CHIPID_CR, &val);
           	            	}
           	            	val = 0;
       	   	    	    }
       	   	    	    CalDump = 1;
       	   	    	    retval = WIFI_TEST_SetRECAL_Dump_Control(CalDump);
       	   	    	    break;
           	    }
#if CONFIG_SUPPORT_FFT
		case INTERNAL_CAPTURE :
			{
				wifi_icap();
				break;
			}
#endif
#if CONFIG_SUPPORT_IWPRIV
		case PIRV_CMD :
			{
				int result ;
				result = WIFI_Driver_priv(privcmdBuf,privCmdLen);
				if(result > 0){
					printf("%s\n",privcmdBuf);
				}else{
					printf("Input format error\n");
				}
				break;
			}
#endif

		case QUERY_TEMPERATURE:
			{
				int val = 0;
				bool retval = WIFI_TEST_Temperature(&val);
				printf("(%s) Temperature value %d\n", retval ? "success":"fail", val);
				break;
			}
	        case MPS_SET:
	            {
	                bool retval = WIFI_TEST_SetTxPath(u4DefaultTxPath);
	                printf("(%s) TX path is set to value 0x%x\n", retval ? "success":"fail", u4DefaultTxPath);
	                retval = WIFI_TEST_Channel(channel);
	                printf("(%s) Set central channel number to %d\n", retval ? "success":"fail", channel);
	                retval = WIFI_TEST_SetBandwidthV2(rfBw);
	                printf("(%s) Set RF bandwidth to %s\n", retval ? "success":"fail",bandwidthV2[rfBw]);
	                retval = WIFI_TEST_SetDBDCBand(u4DbdcBandIndex);/* default 0 */
	                retval = WIFI_TEST_SetPriCh(u4PriCh);/* default 0*/
	                retval = WIFI_TEST_MPSChannel();
                    	printf("(%s) Set MPS channel \n", retval ? "success":"fail");
	                retval = WIFI_TEST_MPSNum(MpsNum);
                    	printf("(%s) Set MPS num:%d \n", retval ? "success":"fail", MpsNum);
	                break;
	            }
	        case MPS_ADD:
	            {
	                uint32_t u4Rate;
	                uint32_t gain;
	                bool retval;
	                u4DefaultTxPath = 0x0;
	                switch(txMode) {
	                    case 0: /* A/B/G mode */
	                        retval = WIFI_TEST_MPSSeqData((pType << 24) | (u4DefaultTxPath << 8) | (g_rate-1));
	                        printf("(%s) Set MPS data \n", retval ? "success":"fail");
	                        //retval = WIFI_TEST_MPSSystemBW(g_bandwidth);
	                        //printf("(%s) Set MPS data bandwidth:%d \n", retval ? "success":"fail", rfBw);
	                        break;
	                    case 1: /* N mode */
	                        u4Rate = RF_AT_PARAM_RATE_MCS_MASK | (uint32_t)(gMCSrate);
	                        retval = WIFI_TEST_MPSSeqData(((gMode + RF_AT_PREAMBLE_11N_MM) << 24) | (u4DefaultTxPath<<8) | u4Rate);
	                        printf("(%s) Set MPS data \n", retval ? "success":"fail");
	                        //retval = WIFI_TEST_MPSSystemBW(g_bandwidth);
	                        //printf("(%s) Set MPS data bandwidth:%d \n", retval ? "success":"fail", rfBw);
	                        break;
	                    case 2: /* AC mode */
	                        u4Rate = RF_AT_PARAM_RATE_MCS_MASK | (uint32_t)(gMCSrate);
	                        retval = WIFI_TEST_MPSSeqData((RF_AT_PREAMBLE_11AC << 24) | (u4DefaultTxPath << 8) | u4Rate);
	                        printf("(%s) Set MPS data \n", retval ? "success":"fail");
	                        //retval = WIFI_TEST_MPSSystemBW(rfBw);
	                        //printf("(%s) Set MPS data bandwidth:%d \n", retval ? "success":"fail", rfBw);
	                        break;
	                    default:
	                        printf("Unsupported Tx mode[%u]!\n", txMode);
	                        break;
	                }
	                retval = WIFI_TEST_MPSPayloadLength(payloadLength);
	                printf("(%s) Set MPS payload length:%d \n", retval ? "success":"fail", payloadLength);
	                retval = WIFI_TEST_MPSPktCount(times);
	                printf("(%s) Set MPS packet count:%d \n", retval ? "success":"fail", times);
                    	gain = txGain * 2;
	                if (abspowerFlag) {/*MPS abspower bit 31*/
	                    gain |= (1<<31);
	                }
                   	if (!default_channel_power_Flag) { /*decide to set power or use power from efuse/EEPROME table*/
    	           	    retval = WIFI_TEST_MPSPower(gain);
    	           	    printf("(%s) Set MPS power:%f \n", retval ? "success":"fail", txGain);
                   	}
	                retval = WIFI_TEST_MPSStream(NSS);
	                printf("(%s) Set MPS stream:%d \n", retval ? "success":"fail", NSS);
	                retval = WIFI_TEST_MPSPackageBW(u4Cbw);
	                printf("(%s) Set MPS data bandwidth:%d \n", retval ? "success":"fail", u4Cbw);
	                break;
	            }
	        case MPS_START:
	            {
	                wifi_tx();
	                break;
	            }
		default:
		case OPER_NONE:
			//printf("not give any operation\n");
			break;
	}
	WIFI_TEST_deinit();

	if ((user_expect & 0x2) == 0x2 && wlan_status == TEST_MODE_ON){
		int ret = WIFI_TEST_CloseDUT();
		printf("[%s] Disable Wi-Fi test mode %s\n", WIFI_IF_NAME,
				ret==true ? "success":"fail");
	}

	return 0;
}

bool check_string(signed char *optarg) {
	bool ret = true;
	signed char * endptr;
	errno = 0;
	if (optarg == NULL)
		return false;

	long val = strtol((const char*)optarg,(char **) &endptr, 0);

	/* Check for various possible errors */
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		fprintf(stderr, "Invalid Argument\n");
		//perror("strtol");
		//exit(EXIT_FAILURE);
		ret = false;
	}

	if (endptr == optarg) {
		fprintf(stderr, "No digits were found\n");
		//exit(EXIT_FAILURE);
		ret = false;
	}
	return ret;
}

void wifi_single_tone() {
	bool retval = false;

	/* Set Tone Type */
	retval = WIFI_TEST_Single_Tone_Type(u4ToneType);
	printf("(%s) Set Single Tone Type to %d\n", retval ? "success":"fail", u4ToneType);
	if (retval == 0) return;

	/* Set Tone Freq */
	retval = WIFI_TEST_Single_Tone_Freq(u4ToneFreq);
	printf("(%s) Set Single Tone Frequency to %d\n", retval ? "success":"fail", u4ToneFreq);
	if (retval == 0) return;

	/* Set Tone RF Gain */
	retval = WIFI_TEST_Single_Tone_RF_Gain(u4ToneRfGain);
	printf("(%s) Set Single Tone RF Gain to %d\n", retval ? "success":"fail", u4ToneRfGain);
	if (retval == 0) return;

	/* Set Tone Digital Gain */
	retval = WIFI_TEST_Single_Tone_Digital_Gain(u4ToneDigitalGain);
	printf("(%s) Set Single Tone Digital Gain to %d\n", retval ? "success":"fail", u4ToneDigitalGain);
	if (retval == 0) return;

	/* Set Tone DC Offset */
	retval = WIFI_TEST_Single_Tone_DC_Offset(u4ToneDcOffsetI, u4ToneDcOffsetQ);
	printf("(%s) Set Single Tone DC Offset to %d %d\n", retval ? "success":"fail", u4ToneDcOffsetI, u4ToneDcOffsetQ);
	if (retval == 0) return;

	/* Start Tone Tx*/
	retval = WIFI_TEST_Single_Tone();
	printf("(%s) Call Single Tone function.\n", retval ? "success":"fail");
	if (retval == 0) return;
}

void wifi_sensitivity(int times, int channel)
{
	int i, nextInterval;
	int rxOk, rxErr, RXDATA = 0, RxTotal_OKCount = 0, RXTotal_ERRCount = 0;
	int rxRssisFinal;
   	int function_data_15_0 = 0, function_data_15_0_carry = 0, function_data_31_16_carry = 0;
   	int pre_function_data_15_0 = 0, pre_function_data_31_16 = 0;//store pre step function data
   	bool retval;
	bool finalResult = false;
	bool ret[3];
   	unsigned int val = 0;
   	int delay_counter = 0;
	retval = WIFI_TEST_Channel(channel);
	printf("(%s) Set central channel number to %d\n", retval ? "success":"fail",
			channel);
	if (retval == 0) return;

#if 0
	retval = WIFI_TEST_SetRxDefaultAnt(rxDefaultAnt);
	printf("(%s) Set Rx default antenna to %s\n", retval ? "success":"fail",
			rxDefaultAnt?"AUX":"main");
	if (retval == 0) return;
#endif

	if (isNewBwSet) {
		retval = WIFI_TEST_SetBandwidthV2(rfBw);
		printf("(%s) Set RF bandwidth to %s\n", retval ? "success":"fail",
				bandwidthV2[rfBw]);
		if (retval == 0) return;

		retval = WIFI_TEST_SetPriChannelSetting(priSetting);
		printf("(%s) Set primary channel index to %u\n",
				retval ? "success":"fail", priSetting);
		if (retval == 0) return;
	}
	else {
		retval = WIFI_TEST_SetBandwidth(g_bandwidth);
		printf("(%s) Set bandwidth to %s\n", retval ? "success":"fail",
				bandwidth[g_bandwidth]);
		if (retval == 0) return;
	}

	if (u4HwAckEnable) {
		WIFI_TEST_Set_Auto_Ack((char *)AckMac);
	}
	else {
	if (eRxOkMatchRule == RX_MATCH_RULE_DISABLE) {
		retval = WIFI_TEST_SetRX(false, NULL, NULL);
		printf("(%s) Disable RX filter\n", retval ? "success":"fail");
	}
	else {
		if (bRxFilterMacAddrLegalFg) {
			retval = WIFI_TEST_SetRX(true, NULL, (char *)aucRxFilterMacAddr);
			printf("(%s) Enable RX filter, Set RA Address to %02x:%02x:%02x:%02x:%02x:%02x\n", retval ? "success":"fail",
					aucRxFilterMacAddr[0],
					aucRxFilterMacAddr[1],
					aucRxFilterMacAddr[2],
					aucRxFilterMacAddr[3],
					aucRxFilterMacAddr[4],
					aucRxFilterMacAddr[5]
			      );
			if (retval == 0) return;
		}
		else {
			printf("Enalbe RX filter, need to set RA address\n");
			return;
			}
		}
	}

	/* 71 */
	retval = WIFI_TEST_SetCbw(u4Cbw);
	printf("(%s) Set Channel Bandwidth to value 0x%d\n", retval ? "success":"fail", u4Cbw);

	/* todo: not support yet, temp code.
	 * Ted201213: force RX patch WF0+WF1 band => 0x00030000
	 * bit[15:0] band index, bit[31:16] mask of RX path.
	 */
	if (rx_path == 0x1) { /*WF0*/
		u4DefaultRxPath = 0x00010000;
	}
	else if (rx_path == 0x2) { /*WF1*/
		u4DefaultRxPath = 0x00020000;
	}
	else if (rx_path == 0x3) {/*WF0 + WF1*/
		u4DefaultRxPath = 0x00030000;
	}
	else
	{
		printf("RX path not support!!!\n");

	}

	WIFI_TEST_SetRxPath(u4DefaultRxPath);

	retval = WIFI_TEST_RxStart();
	printf("(%s) RX test started\n", retval ? "success":"fail");
	if (retval == 0) return;

	if(SetIsoFlag) {
		retval = WIFI_TEST_AutoIso();
		/* Auto Isolation Value */
		uint32_t defaultBTTxPower, SetBTTxPower, PowerOffset = 0;
		retval = WIFI_TEST_GetCalIsolationValue(&Isolation_val);
		defaultBTTxPower = Isolation_val >> 24;
		defaultBTTxPower |= ((defaultBTTxPower >> 7) & 1)? 0xffffff00 : 0x00000000;
		SetBTTxPower = BTTxPower*2;
		Isolation_val = ((Isolation_val << 8) >> 8);
		if (SetBTTxFlag){
			PowerOffset = SetBTTxPower - defaultBTTxPower*2;
			printf("BT Tx Power : %.1f dbm\n", BTTxPower);
		}
		else {
			printf("BT Tx Power : %d.0 dBm\n", defaultBTTxPower);
		}
		Isolation_val += PowerOffset;
		printf("(%s)Auto Isolation Value %.1f \n", retval ? "success":"fail",(float)Isolation_val/2);
		SetIsoFlag = false;
	}

   	if (times == 0) {
   	    usleep(15 * 1000);
   	    WIFI_TEST_MCR_Read(CHIPID_CR, &val);
   	    while(val == 0) {
   	        if(delay_counter > 5000)
   	        {
   	            printf("!!RX initialize fail!!\n");
   	            exit(0);
   	        }
   	        delay_counter++;
   	        WIFI_TEST_MCR_Read(CHIPID_CR, &val);
   	        usleep(1000);
   	    }
   	    val = 0;
   	}

	nextInterval = printInterval;

	for(i = 0; (i < times) || !finalResult; i += nextInterval) {

		if(i >= times) {
			finalResult = true;
		}
       		if(feature_support(SW_VERSION, 10705)) { /*10705 is tool/FW version that support T/RX data sync*/
       		    ret[0] = WIFI_TEST_RXDATA(&RXDATA);
       		    ret[1] = ret[0];
       		    ret[2] = WIFI_TEST_RSSI(&rxRssisFinal);
       		    pre_function_data_31_16 = 0;
       		    pre_function_data_15_0 = 0;
       		    Overflow_bit15_0(&RXDATA, &function_data_15_0, &pre_function_data_15_0, &rxErr, &function_data_15_0_carry);
       		    Overflow_bit31_16(&RXDATA, &pre_function_data_31_16, &rxOk, &function_data_31_16_carry);
       		    RXTotal_ERRCount += rxErr;/*Rx err is not accumulative in new FW*/
       		}
       		else {
       			ret[0] = WIFI_TEST_FRGood(&rxOk);
       			ret[1] = WIFI_TEST_FRError(&rxErr);
       			ret[2] = WIFI_TEST_RSSI(&rxRssisFinal);
       		    RXTotal_ERRCount = rxErr;/*Rx err is accumulative in old FW*/
       		}

       		RxTotal_OKCount += rxOk;/*Rx ok is not accumulative in FW*/

		if ((RxTotal_OKCount + RXTotal_ERRCount) == 0) {
			fprintf(stdout, "[%3d] (%d)RX OK: %4d / (%d)RX ERR: %4d\n",
					i, ret[0], RxTotal_OKCount, ret[0], RXTotal_ERRCount);
		}
		else {
			fprintf(stdout, "[%3d] (%d)RX Total OK Count: %4d  /(%d)RX Tolal ERR Count: %4d / PER: %2d .. / Rx Total Count: %4d"
					" (%d)RSSI0: %i / RSSI1: %i \r\n", i, ret[0], RxTotal_OKCount, ret[1], RXTotal_ERRCount,
					(100 * RXTotal_ERRCount)/(RxTotal_OKCount + RXTotal_ERRCount), (RxTotal_OKCount + RXTotal_ERRCount), ret[2], (signed char)(rxRssisFinal & BITS(0,7)), (signed char)(rxRssisFinal >> 8));
		}
		fflush(stdout);

		if((times - i) < printInterval) {
			nextInterval = times - i;
		}

		if((rxOk + rxErr >= (int)maxPktCount) && maxPktCount) {
			printf("Rx packet count[%u] >= max count[%u], break!\n",
					rxOk + rxErr, maxPktCount);
			break;
		}

		sleep(nextInterval);
	}

	if (times == 0) {
		printf("Rx test is running! use -T to stop Rx test...\n");
	}
	else {
		retval = WIFI_TEST_RxStop();
	}
}

void wifi_tx(void)
{
	bool retval;
	bool finalResult = false;
    	unsigned int val = 0;
   	int delay_counter = 0;
	WIFI_TEST_TxDestAddress(macAddr);
   	if(!g_MPS_flag) { /* to avoid set channel again*/
   	    	retval = WIFI_TEST_Channel(channel);
   	    	printf("(%s) Set central channel number to %d\n", retval ? "success":"fail", channel);
   	}
	if (retval == 0) return;

#if 0
	retval = WIFI_TEST_SetRxDefaultAnt(rxDefaultAnt);
	printf("(%s) Set Rx default antenna to %s\n", retval ? "success":"fail",
			rxDefaultAnt?"AUX":"main");
	if (retval == 0) return;
#endif
   	 if(!g_MPS_flag) { /* cannot set CBW again to avoid set channel again*/
	if (isNewBwSet) {
		retval = WIFI_TEST_SetBandwidthV2(rfBw);
		printf("(%s) Set RF bandwidth to %s\n", retval ? "success":"fail",
				bandwidthV2[rfBw]);
		if (retval == 0) return;

		retval = WIFI_TEST_SetTxBandwidth(txBw);
		printf("(%s) Set Tx bandwidth to %s\n", retval ? "success":"fail",
				bandwidthV2[txBw]);
		if (retval == 0) return;

		retval = WIFI_TEST_SetPriChannelSetting(priSetting);
		printf("(%s) Set primary channel index to %u\n",
				retval ? "success":"fail", priSetting);
		if (retval == 0) return;
	}
	else {
		retval = WIFI_TEST_SetBandwidth(g_bandwidth);
		printf("(%s) Set bandwidth to %s\n", retval ? "success":"fail",
				bandwidth[g_bandwidth]);
		if (retval == 0) return;
	}
   	 }
	if (!default_channel_power_Flag) {
        if (abspowerFlag) {
            	retval = WIFI_TEST_ABSTxGain(txGain);
            	printf("(%s) Set absolute Tx power gain to %.1f dBm\n", retval ? "success":"fail", txGain);
            	if (retval == 0) return;
        }
        else {
                retval = WIFI_TEST_TxGain(txGain);
                printf("(%s) Set Tx power gain to %.1f dBm\n", retval ? "success":"fail", txGain);
                if (retval == 0) return;
        }
	}
	retval = WIFI_TEST_TxPayloadLength(payloadLength);
	printf("(%s) Set Tx payload to %d bytes..\n", retval ? "success":"fail", payloadLength);
	if (retval == 0) return;

	retval = WIFI_TEST_TxBurstInterval(SIFS);
	printf("(%s) Set frame interval to %d TU\n", retval ? "success":"fail", SIFS);
	if (retval == 0) return;

	retval = WIFI_TEST_TxBurstFrames(times);
	printf("(%s) Set frame count to %d \n", retval ? "success":"fail", times);
	if (retval == 0) return;

	switch(txMode) {
		case 0: /* A/B/G mode */
			retval = WIFI_TEST_SetPreamble(pType);
			printf("(%s) Set %s preamble\n", retval ? "success":"fail", preamble[pType]);
			if (retval == 0) return;

			retval = WIFI_TEST_TxDataRate(g_rate);
			printf("(%s) Set Tx mode to 11a/b/g, tx rate %s\n", retval ? "success":"fail", bg_rate[g_rate]);
			if (retval == 0) return;
			break;

		case 1: /* N mode */
			retval = WIFI_TEST_TxDataRate11n(gMCSrate, gMode, giType);
			printf("(%s) Set Tx mode to 11n, MCS%u, %s, %s GI, %s\n", retval ? "success":"fail",
					gMCSrate, gMode?"greenfield":"mixed-mode", giType?"Short":"Normal",
					coding?"LDPC":"BCC");
			if (retval == 0) return;

			retval = WIFI_TEST_SetTxCodingMode(coding);
			if (retval == 0) return;
			break;

		case 2: /* AC mode */
			retval = WIFI_TEST_TxDataRate11ac(gMCSrate, giType);
			printf("(%s) Set Tx mode to 11ac MCS%u, %s GI, %s\n", retval ? "success":"fail",
					gMCSrate, giType?"Short":"Normal", coding?"LDPC":"BCC");
			if (retval == 0) return;

			retval = WIFI_TEST_SetTxCodingMode(coding);
			if (retval == 0) return;
			break;
		default:
			printf("Unsupported Tx mode[%u]!\n", txMode);
			return;
	}
   	if(!g_MPS_flag) {/*to avoid set channel again*/
   	        retval = WIFI_TEST_SetTxPath(u4DefaultTxPath);
   	        printf("(%s) TX path is set to value 0x%x\n", retval ? "success":"fail", u4DefaultTxPath);
   	}
	retval = WIFI_TEST_SetBand(u4Band);
	printf("(%s) Band is set to value 0x%d\n", retval ? "success":"fail", u4Band);

	/* 110 */
	retval = WIFI_TEST_SetDBDC(u4Dbdc);
	printf("(%s) u4Dbdc is set to value 0x%d\n", retval ? "success":"fail", u4Dbdc);

	/* 104 */
	retval = WIFI_TEST_SetDBDCBand(u4DbdcBandIndex);
	printf("(%s) u4DbdcBandIndex is set to value 0x%d\n", retval ? "success":"fail", u4DbdcBandIndex);


	/* 71 */
   	if(!g_MPS_flag) {/*to avoid set channel again*/
   	        retval = WIFI_TEST_SetCbw(u4Cbw);
   	        printf("(%s) Channel Bandwidth is set to value 0x%d\n", retval ? "success":"fail", u4Cbw);
   	}
	/* 72 */
	//retval = WIFI_TEST_SetDbw(u4Dbw);
	//printf("(%s) Band is set to value 0x%d\n", retval ? "success":"fail", u4Dbw);

	retval = WIFI_TEST_SetDbw(u4Cbw);
	printf("(%s) Data Bandwidth is set to value 0x%d\n", retval ? "success":"fail", u4Cbw);

	/* 73 */
   	if(!g_MPS_flag) {/*to avoid set channel again*/
   	        retval = WIFI_TEST_SetPriCh(u4PriCh);
   	        printf("(%s) Primary channel is set to value 0x%d\n", retval ? "success":"fail", u4PriCh);
   	}

	/* 101 */
	retval = WIFI_TEST_SetMacHeader(u4MacHeader);
	printf("(%s) u4MacHeader is set to value 0x%d\n", retval ? "success":"fail", u4MacHeader);

	/* 103 */
	retval = WIFI_TEST_SetPayLoad(u4Payload);
	printf("(%s) u4Payload is set to value 0x%d\n", retval ? "success":"fail", u4Payload);

	/* 69 */
	retval = WIFI_TEST_SetTA(u4Ta);
	printf("(%s) u4Ta is set to value 0x%d\n", retval ? "success":"fail", u4Ta);

	/* 17 */
	retval = WIFI_TEST_SetStbc(u4Stbc);
	printf("(%s) u4Stbc is set to value 0x%d\n", retval ? "success":"fail", u4Stbc);

	/* 126*/
	retval = WIFI_TEST_SetIbf(u4Ibf);
	printf("(%s) u4Ibf is set to value 0x%d\n", retval ? "success":"fail", u4Ibf);

	/* 127*/
	retval = WIFI_TEST_SetEbf(u4Ebf);
	printf("(%s) u4Ebf is set to value 0x%d\n", retval ? "success":"fail", u4Ebf);

	/* 114*/
	retval = WIFI_TEST_SetNss(NSS);
	printf("(%s) Nss is set to value 0x%x\n", retval ? "success":"fail", NSS);

	//for CW mode
	if (-1 != cw_mode) {

     		retval = WIFI_TEST_CW_SetTxPath(u4DefaultTxPath);
                printf("(%s) CW TX path is set to value 0x%x\n", retval ? "success":"fail", u4DefaultTxPath);
                if (retval == 0) return;

		retval = WIFI_TEST_CW_MODE(cw_mode);
		printf("(%s) cw mode set to %d\n", retval ? "success":"fail", cw_mode);
		if (retval == 0) return;

		retval = WIFI_TEST_CW_MODE_START(abspowerFlag, txGain);
		printf("(%s) cw mode start.\n", retval ? "success":"fail");
		if (retval == 0) return;
	}
	else {
		printf("no cw mode configuration.\n");
		retval = WIFI_TEST_TxStart();
		printf("(%s) TX test started..\n", retval ? "success":"fail");
		if (retval == 0) return;
	}

  	usleep(15 * 1000);
  	WIFI_TEST_MCR_Read(CHIPID_CR, &val);
  	while(val == 0) {
       		 if(delay_counter > 5000)
       		 {
       		     printf("!!TX initialize fail!!\n");
       		     exit(0);
       		 }
       		 delay_counter++;
       		 WIFI_TEST_MCR_Read(CHIPID_CR, &val);
       		 usleep(1000);
    	}
    	val = 0;

   	if(!g_MPS_flag) {
	if (sleep_time == 0) {
		printf("Tx test is running! use -T to stop Tx test...\n");
		return;
	}
	else {
		int i, nextInterval;
		uint32_t u4TxOk = 0, u4Tx = 0;
        int TX_DATA = 0;// ok/air count, report from FW
        int function_data_15_0 = 0;
        int pre_function_data_15_0 = 0, pre_function_data_31_16 = 0;//save previous function data to detect overflow
        int function_data_15_0_carry = 0, function_data_31_16_carry = 0;//overflow bit for function data[15:0] and [31:16]
		nextInterval = printInterval;

		printf("Tx test is running! wait for %us...\n", sleep_time);


		for(i = 0; (i < sleep_time) || !finalResult; i += nextInterval) {
			if(i >= sleep_time) {
				finalResult = true;
			}

            if(feature_support(SW_VERSION, 10705)) {
			    retval = WIFI_TEST_TXDATA(&TX_DATA);
                Overflow_bit15_0(&TX_DATA, &function_data_15_0, &pre_function_data_15_0, (int*)&u4Tx, &function_data_15_0_carry);//tx [15:0]air count
		        Overflow_bit31_16(&TX_DATA, &pre_function_data_31_16, (int*)&u4TxOk, &function_data_31_16_carry);     //tx[31:16]OK count
            }
            else {
                retval = WIFI_TEST_TxCount(&u4Tx);
    			retval = WIFI_TEST_TxGoodCount(&u4TxOk);  /*mark for sync ok count and aircount*/
            }

			if(retval == 0) {
				printf("(%s) Cannot get test result!\n", retval ?"success":"fail");
			}
			else {
				printf("[%u] Tx total/good count: %u/%u\n", i, u4Tx, u4TxOk);
			}

			if(i >= sleep_time) {
				break;
			}

			if((u4Tx >= maxPktCount) && maxPktCount) {
				printf("Tx packet count[%u] >= max count[%u], break!\n",
						u4Tx, maxPktCount);
				break;
			}

			if((sleep_time - i) < printInterval) {
				nextInterval = (sleep_time - i);
			}

			sleep(nextInterval);
		}

		printf("Stop Tx test!\n");
	}

	retval = WIFI_TEST_TxStop();
    	        retval = WIFI_TEST_SetRateOffset();//restore rateoffset
   	}
}

void wifiGetResult(void) {
	uint32_t u4RxOk, u4RxFailed, u4Rssi, RX_DATA = 0;
	uint32_t u4TxOk, u4Tx;
   	int TX_DATA = 0;
        int function_data_15_0 = 0;
        int pre_function_data_15_0 = 0, pre_function_data_31_16 = 0;//save previous function data to detect overflow
        int function_data_15_0_carry = 0, function_data_31_16_carry = 0;//overflow bit for function data[15:0] and [31:16]
	bool retval;

	do {
       		if(feature_support(SW_VERSION, 10705)) {
       		   	 retval = WIFI_TEST_RXDATA((int*)&RX_DATA);
       		   	 if (retval == 0) break;

       		   	 retval = WIFI_TEST_TXDATA((int*)&TX_DATA);
       		   	 if (retval == 0) break;

       		   	 Overflow_bit15_0(&TX_DATA, &function_data_15_0, &pre_function_data_15_0, (int*)&u4Tx, &function_data_15_0_carry);//tx [15:0]air count
       		   	 Overflow_bit31_16(&TX_DATA, &pre_function_data_31_16, (int*)&u4TxOk, &function_data_31_16_carry);     //tx[31:16]OK count

       		   	 pre_function_data_15_0 = 0;/*Rx err/ok are not accumulative in new FW*/
       		   	 pre_function_data_31_16 = 0;
       		   	 function_data_15_0 = 0;
       		   	 function_data_31_16_carry = 0;
       		   	 function_data_15_0_carry = 0;

       			 Overflow_bit15_0((int*)&RX_DATA, &function_data_15_0, &pre_function_data_15_0, (int*)&u4RxFailed, &function_data_15_0_carry);//rx [15:0]ok count
       			 Overflow_bit31_16((int*)&RX_DATA, &pre_function_data_31_16, (int*)&u4RxOk, &function_data_31_16_carry);     //rx[31:16]err count
       		}
       		else {
       			retval = WIFI_TEST_FRGood((int*)&u4RxOk);
       			if (retval == 0) break;

       			retval = WIFI_TEST_FRError((int*)&u4RxFailed);
       			if (retval == 0) break;

       			retval = WIFI_TEST_TxCount(&u4Tx);
       			if (retval == 0) break;

       			retval = WIFI_TEST_TxGoodCount(&u4TxOk);
       			if (retval == 0) break;
       		}
		retval = WIFI_TEST_RSSI((int*)&u4Rssi);
		if (retval == 0) break;

	} while(false);

	if(retval == 0) {
		printf("(%s) Cannot get test result!\n", retval ?"success":"fail");
	}
	else {
		printf("Tx total/good count: %u/%u\n", u4Tx, u4TxOk);
        if(u4RxOk==0 && u4RxFailed==0) {
            printf("Rx good/err count: %u/%u PER: 100 RSSI:-999 \n", u4RxOk, u4RxFailed);
        }
        else {
       			    printf("Rx good/err count: %u/%u PER: %u RSSI0: %i / RSSI1: %i\n", u4RxOk, u4RxFailed,
       					    (100 * u4RxFailed)/(u4RxOk + u4RxFailed), (signed char)(u4Rssi & BITS(0,7)), (signed char)((u4Rssi & BITS(8,15)) >> 8));
       		}
	}
}

void wifiTestStop(void){
	bool retval = WIFI_TEST_TxStop();
	printf("(%s) stop Tx\n", retval ? "success":"fail");

	retval = WIFI_TEST_RxStop();
	printf("(%s) stop Rx\n", retval ? "success":"fail");
}

/* if wlan.driver.status is ok, then wlan normal mode is on
   if /sys/class/net/wlan0 is not exist, then wlan is off
   otherwise, we think the wlan may be turned on by us */
static WlanStatus wifiStatus(void){

	//char driver_status[PROP_VALUE_MAX];
	bool normal_mode_on = false;
	char netdevPath[256];
	//static bool isTestMode = false;

	struct stat buf;
	//property_get("wlan.driver.status", driver_status, "unloaded");
	//if (strncmp(driver_status, "ok", 2) == 0){
	//	normal_mode_on = true;
	//	}


	return TEST_MODE_ON;
	snprintf(netdevPath, 255, "/sys/class/net/%s", WIFI_IF_NAME);
	if (stat(netdevPath, &buf) < 0 && errno==ENOENT)
		return WLAN_MODE_OFF;
	return normal_mode_on ? NORMAL_MODE_ON:TEST_MODE_ON;
}


/*----------------------------------------------------------------------------*/
/*!
 * @brief This function is to open the eeprom.bin file in /lib/firmware
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
int
open_eeprom_file(
		void
		)
{
	FILE *fh;
	int retval = -1;

	fh = fopen(EEPROM_PATH_TMP, "rb");

	if(fh != NULL) {
		/* Success : use eeprom.bin file from /lib/firmware */
		/*fgets(buff, sizeof(buff), fh);*/
		/*fgets(buff, sizeof(buff), fh);*/

		/* Read each device line */
		/*while(fgets(buff, sizeof(buff), fh)) */

		while(!feof(fh)) {
			fread(uacEEPROMImage, 1, MAX_EEPROM_BUFFER_SIZE, fh);
		}
		fclose(fh);
	}
	else {
		printf("Open file error!!! Please modify the file path for reading source\n");
	}

#if 0
	for (index=0; index<MAX_EEPROM_BUFFER_SIZE; index+=16)
	{
		printf("offset=%2x, %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x", index,
				uacEEPROMImage[index]  ,  uacEEPROMImage[index+1],  uacEEPROMImage[index+2],   uacEEPROMImage[index+3],
				uacEEPROMImage[index+4],  uacEEPROMImage[index+5],  uacEEPROMImage[index+6],   uacEEPROMImage[index+7],
				uacEEPROMImage[index+8],  uacEEPROMImage[index+9],  uacEEPROMImage[index+10], uacEEPROMImage[index+11],
				uacEEPROMImage[index+12], uacEEPROMImage[index+13],  uacEEPROMImage[index+14], uacEEPROMImage[index+15]);


		printf("\n");
	}
#endif

	return retval;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for EEPROM query commands
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_EEPROM_Read(unsigned int offset, unsigned int *val)
{
	if (!val) {
		return false;
	}

	*val = uacEEPROMImage[offset];

	return true;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for EEPROM write commands
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/

bool WIFI_TEST_EEPROM_Write(unsigned int offset, unsigned int val)
{
	FILE *fh;
	unsigned char test;
	uacEEPROMImage[offset] = val;
	test = uacEEPROMImage[offset];

#if 1
	fh = fopen(EEPROM_PATH_TMP, "rb+");
	if(fh != NULL) {

		//printf("111  of WIFI_TEST_EEPROM_Write\n");

#if 0
		while(!feof(fh)) {
			printf("1.5  of WIFI_TEST_EEPROM_Write\n");
			fwrite(uacEEPROMImage, sizeof(uacEEPROMImage), 1200, fh);
			printf("1.6  of WIFI_TEST_EEPROM_Write\n");
		}
#endif
		fseek(fh, offset, SEEK_SET);
		fwrite(&test, sizeof(unsigned char), 1, fh);
		//printf("222  of WIFI_TEST_EEPROM_Write\n");

		fclose(fh);
	}
#endif

	//printf("end of WIFI_TEST_EEPROM_Write\n");

	return true;
}

#if 0
/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for i-cal mode (intelligent calibration)
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
void replaceEEPROM()
{
	int i;
	int retval = -1;
	unsigned int val = 0;
	unsigned int replace_positions[] =
	{0x53, 0x54, 0x55, 0xF1, 0xF4, 0xF7,0x178,0x179,0x17A,
		0x17B,0x1B3,0x1B4,0x1B5,0x1B6,0x1B7,0x1E2,0x1E3,0x300,0x305,
		0x306,0x307,0x308,0x309,0x366,0x367,0x368,0x369,0x36A,0x36B,
		0x36C,0x38C,0x38D,0x38E};

	printf("Inside Replace: Checking Address\n");
#if 0
	for(i=0;i<80;i++)
	{
#endif
		// Replace all the contents in the FT fields
		for(i=0;i< sizeof(replace_positions)/sizeof(unsigned int) ;i++)  //34
		{
			retval = WIFI_TEST_EFUSE_Read(replace_positions[i], &val);

			//printf("(%s) EFUSE addr 0x%x value 0x%x\n", retval ? "success":"fail", replace_positions[i], val);

			uacEEPROMImage[replace_positions[i]] = val;
		}
		return retval;
#if 0
	}
#endif
#if 0
	printf("After merge\n");

	for (index=0; index<MAX_EEPROM_BUFFER_SIZE; index+=16)
	{
		printf("offset=%2x, %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x", index,
				uacEEPROMImage[index]  ,  uacEEPROMImage[index+1],  uacEEPROMImage[index+2],   uacEEPROMImage[index+3],
				uacEEPROMImage[index+4],  uacEEPROMImage[index+5],  uacEEPROMImage[index+6],   uacEEPROMImage[index+7],
				uacEEPROMImage[index+8],  uacEEPROMImage[index+9],  uacEEPROMImage[index+10], uacEEPROMImage[index+11],
				uacEEPROMImage[index+12], uacEEPROMImage[index+13],  uacEEPROMImage[index+14], uacEEPROMImage[index+15]);


		printf("\n");
	}
#endif
}
#endif

/*----------------------------------------------------------------------------*/
/*!
 * @brief This API provided a service for saving array to bin file
 *
 * @param
 *
 * @return
 */
/*----------------------------------------------------------------------------*/
void saveEEPROM()
{
	FILE *file=fopen(EEPROM_PATH_WRITE,"wb");

	fwrite(uacEEPROMImage,sizeof(unsigned char),MAX_EEPROM_BUFFER_SIZE,file);

	fclose(file);
}
#if CONFIG_SUPPORT_FFT
void wifi_icap(void) {
    bool retval = false;
    uint16_t u2Status;
    uint32_t u4BwMhz;
    ICAP_CTRL_T prIcapCtrl;
    uint16_t u2WaitTime = 0;

    // Start Rx first (refer to wifi_sensitivity)

    retval = WIFI_TEST_Channel(channel);
    if (retval == 0) return;

    if (isNewBwSet) {
        retval = WIFI_TEST_SetBandwidthV2(rfBw);
        if (retval == 0) return;

        retval = WIFI_TEST_SetPriChannelSetting(priSetting);
        if (retval == 0) return;
    }
    else {
        retval = WIFI_TEST_SetBandwidth(g_bandwidth);
        if (retval == 0) return;
    }

    if (eRxOkMatchRule == RX_MATCH_RULE_DISABLE) {
        retval = WIFI_TEST_SetRX(false, NULL, NULL);
    }
    else {
        if (bRxFilterMacAddrLegalFg) {
            retval = WIFI_TEST_SetRX(true, NULL, (char *)aucRxFilterMacAddr);
            if (retval == 0) return;
        }
        else {
            return;
        }
    }

    /* 71 */
    retval = WIFI_TEST_SetCbw(u4Cbw);

	/* todo: not support yet, temp code.
	 * Ted201213: force RX patch WF0+WF1 band => 0x00030000
	 * bit[15:0] band index, bit[31:16] mask of RX path.
	 */
    if (rx_path == 0x1) { /*WF0*/
        u4DefaultRxPath = 0x00010000;
    }
    else if (rx_path == 0x2) { /*WF1*/
        u4DefaultRxPath = 0x00020000;
    }
    else if (rx_path == 0x3) {/*WF0 + WF1*/
        u4DefaultRxPath = 0x00030000;
    }

    WIFI_TEST_SetRxPath(u4DefaultRxPath);

    retval = WIFI_TEST_RxStart();
    if (retval == 0) return;

    if (SetIsoFlag) {
        retval = WIFI_TEST_AutoIso();
        /* Auto Isolation Value */
        uint32_t defaultBTTxPower, SetBTTxPower, PowerOffset = 0;
        retval = WIFI_TEST_GetCalIsolationValue(&Isolation_val);
        defaultBTTxPower = Isolation_val >> 24;
        defaultBTTxPower |= ((defaultBTTxPower >> 7) & 1)? 0xffffff00 : 0x00000000;
        SetBTTxPower = BTTxPower*2;
        Isolation_val = ((Isolation_val << 8) >> 8);
        if (SetBTTxFlag) {
            PowerOffset = SetBTTxPower - defaultBTTxPower*2;
        }
        Isolation_val += PowerOffset;
        SetIsoFlag = false;
    }


    // Run Internal Capture

    if (u4Cbw == WIFI_TEST_CH_BW_20MHZ) {
        u4BwMhz = 20;
    }
    else if (u4Cbw == WIFI_TEST_CH_BW_40MHZ) {
        u4BwMhz = 40;
    }
    else if (u4Cbw == WIFI_TEST_CH_BW_80MHZ) {
        u4BwMhz = 80;
    }
    else {
        return; /* invalid channel bandwidth */
    }

    prIcapCtrl.i4Channel = channel;
    prIcapCtrl.u4Cbw = u4Cbw;
    prIcapCtrl.u4BwMhz = u4BwMhz;
    prIcapCtrl.u4RxPath = rx_path;

    retval = WIFI_TEST_SetIcapStart(&prIcapCtrl);
    printf("(%s) Start internal capture\n", retval ? "success":"fail");
    if (retval == 0) return;

    while (1) {
        retval = WIFI_TEST_GetIcapStatus(&u2Status);
        if (retval == 0) return;

        if (!u2Status) {  // ICAP done
            WIFI_TEST_RxStop();
            break;
        }

        if (u2WaitTime++ < ICAP_TIMEOUT) {
            sleep(1);  // 1 sec
        } else {
            printf("timeout\n");
            return;
        }
    }

    retval = WIFI_TEST_GetIcapData(&prIcapCtrl);
    printf("(%s) Get internal capture data\n", retval ? "success":"fail");
    if (retval == 0) return;

    retval = WIFI_TEST_RunFftFunction(&prIcapCtrl);
    printf("(%s) Run FFT function\n", retval ? "success":"fail");
    if (retval == 0) return;

    retval = WIFI_TEST_DumpIcapResult(&prIcapCtrl, false);
    if (retval == 0) return;
}
#endif
