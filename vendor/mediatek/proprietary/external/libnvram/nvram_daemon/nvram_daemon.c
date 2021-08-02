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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <cutils/properties.h>
#include "libnvram.h"
#include "libnvram_log.h"
#include "nvram_battery.h"
#include "nvram_bt.h"
#include "nvram_wifi.h"
//#include "nvram_touchpanel.h"
#include "nvram_acc.h"
#include "nvram_gyro.h"
#include "nvram_ps.h"
#include "nvram_productinfo.h"
#include "libfile_op.h"
enum NVRAM_USER {
	NU_BATTERY,
	NU_WIFI,
	NU_BT,
	NU_FM,
	//NU_TOUCHPANEL,
	NU_HWMON,
	NU_HWMON_PS,
	NU_PRODUCT_INFO,
	NU_MAX
};

typedef struct _NVRAM_USER_DATA {
	const char*	pcName;
	pthread_t	kThread;
	void* (*start_routine)(void*);
} NVRAM_USER_DATA;

static NVRAM_USER_DATA g_akNUData[NU_MAX] = {
	{"Battery", 0, NVRAM_BATTERY},
	{"Bluetooth", 0, NVRAM_BT},
	{"WIFI", 0, NVRAM_WIFI},
	//{"Touchpanel", 0, NVRAM_TOUCHPANEL},
#ifndef MTK_BASIC_PACKAGE
	{"HWMON_ACC", 0, NVRAM_ACC},
	{"HWMON_GYRO", 0, NVRAM_GYRO},
	{"HWMON_PS", 0, NVRAM_PS},
#endif
	{"PROUDCT_INFO", 0, NVRAM_PRODUCTINFO},
	{"None", 0, NULL}
};
extern int iCustomBeginLID;
static bool bCleanBoot = true;
extern pfCallbackForDaemon callback_for_nvram_daemon;

bool Init_AllThreads_In_Nvram(void) {
	int i, ret;
	void* thread_result;
	for (i = 0; i < NU_MAX; ++i) {
		if (g_akNUData[i].start_routine) {
			ret = pthread_create(&g_akNUData[i].kThread, NULL, g_akNUData[i].start_routine,
			                     NULL);
			if (ret != 0) {
				//NVRAM_LOG("%s Thread creation failed\n", g_akNUData[i].pcName);
				return false;
			}
		}
	}
	//NVRAM_LOG("Finish thread create\n");

	for (i = NU_MAX - 1; i >= 0 ; --i) {
		if (g_akNUData[i].start_routine) {
			ret = pthread_join(g_akNUData[i].kThread, &thread_result);
			if (ret != 0) {
				//NVRAM_LOG("%s Thread join failed\n", g_akNUData[i].pcName);
				return false;
			}
		}
	}
	//NVRAM_LOG("Finish thread joint\n");
	return true;
}
bool Init_Productinfo_In_Nvram(void) {
	int i, ret;
	void* thread_result;
	for (i = 0; i < NU_MAX; ++i) {
		if (strcmp(g_akNUData[i].pcName, "PROUDCT_INFO") == 0) {
			if (g_akNUData[i].start_routine) {
				ret = pthread_create(&g_akNUData[i].kThread, NULL, g_akNUData[i].start_routine,
				                     NULL);
				if (ret != 0) {
					//NVRAM_LOG("%s Thread creation failed\n", g_akNUData[i].pcName);
					return false;
				}
			}
			if (g_akNUData[i].start_routine) {
				ret = pthread_join(g_akNUData[i].kThread, &thread_result);
				if (ret != 0) {
					//NVRAM_LOG("%s Thread join failed\n", g_akNUData[i].pcName);
					return false;
				}
			}
			return true;
		}
	}
	NVRAM_LOG("PROUDCT_INFO thread is not in init list\n");
	return false;
}

#define FSTAB_PREFIX "fstab."

void find_fstab_filename(unsigned char *fstab_filename, unsigned int size)
{
	char propbuf[PROPERTY_VALUE_MAX];

	property_get("ro.hardware", propbuf, "");
	snprintf(fstab_filename, size, FSTAB_PREFIX"%s", propbuf);

}

#define BUF_SIZE 128
int copy_file(unsigned char *src, unsigned char *dst)
{

	int frd, fwd;
	int read_len = 0, write_len = 0;
	unsigned char buf[BUF_SIZE], *p_buf;
	int ret = 0;
	frd = open(src, O_RDONLY);

	if (frd == -1) {
		NVRAM_LOG("Can not open file(%s): %d\n",src, __LINE__);
		ret = -1;
		goto error3;
	}

	fwd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fwd == -1) {
		NVRAM_LOG("Can not creat file(%s): %d\n", dst, __LINE__);
		ret = -1;
		goto error2;
	}
	chmod(dst, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	while ((read_len = read(frd, buf, BUF_SIZE))) {
		if (-1 == read_len) {
			NVRAM_LOG("Read error: %d\n", __LINE__);
			ret = -1;
			goto error1;
		} else if (read_len > 0) {
			p_buf = buf;
			while ((write_len = write(fwd, p_buf, read_len))) {
				if(write_len == read_len) {
					break;
				} else if (write_len > 0) {
					p_buf += write_len;
					read_len -= write_len;
				} else if(-1 == write_len) {
					NVRAM_LOG("Write error: %d\n", __LINE__);
					ret = -1;
					goto error1;
				}
			}
			if (-1 == write_len) {
				ret = -1;
				break;
			}
		}
	}

error1:
	close(fwd);
error2:
	close(frd);
error3:
	return ret;
}

int main(void) {
	int iMaxLidNum = 0;
	int ret_callback = -1;
	char cypto_state_value[100] = {0};
	char nvram_init_value[100] = {0};
	char decypt_state_value[100] = {0};
	unsigned char fstab_filename[256];
	unsigned char src1[256 + 1]; //"/odm/etc/fstab*"
	unsigned char src2[256 + 1]; //"vendor/etc/fstab*"
	unsigned char src3[256 + 1]; //"/fstab*"
	unsigned char dst[256 + 12]; // "/mnt/vendor/nvdata"

	/* copy /fstab.mt*** to /nvdata/fstab.mtxxx */
	find_fstab_filename(fstab_filename, 256);
	snprintf(src1, 256 + 1, "/odm/etc/""%s", fstab_filename);
	snprintf(src2, 256 + 1, "/vendor/etc/""%s", fstab_filename);
	snprintf(src3, 256 + 1, "/""%s", fstab_filename);
	snprintf(dst, 256 + 12, "/mnt/vendor/nvdata/""%s", fstab_filename);
	if (access(src1, F_OK) == 0) {
	    if (copy_file(src1, dst)) {
		    NVRAM_LOG("can't copy fstab file,src1(%s),dst(%s)!!!\n",src1,dst);
		    exit(EXIT_FAILURE);
            }
	}
	else if(access(src2, F_OK) == 0) {
	    if (copy_file(src2, dst)) {
		    NVRAM_LOG("can't copy fstab file,src2(%s),dst(%s)!!!\n",src2,dst);
		    exit(EXIT_FAILURE);
            }
	}
	else if(access(src3, F_OK) == 0) {
	    if (copy_file(src3, dst)) {
		    NVRAM_LOG("can't copy fstab file,src3(%s),dst(%s)!!!\n",src3,dst);
		    exit(EXIT_FAILURE);
            }
	}
	else {
		NVRAM_LOG("can't find fstab file!!!\n");
		exit(EXIT_FAILURE);
        }

	iMaxLidNum = NVM_Init();
	NVRAM_LOG("Total nvram Lid number is %d\n", iMaxLidNum);
	umask(000);
	if (iMaxLidNum <= 0) {
		NVRAM_LOG("Total nvram Lid number is error !!!\n");
		exit(EXIT_FAILURE);
	}

	if (nvram_emmc_support() || nvram_ufs_support()) {
		if (!Check_UpdateStatus()) {
			NVRAM_LOG("fail to check upgrade status in first boot\n");
		}
	}
#ifdef MTK_NVRAM_AUTO_BACKUP
	if (!FileOp_CmpBackupFileNum()) {
		FileOp_BackupToBinRegion_All();
		FileOp_SetCleanBootFlag(true);
	} else {
#endif
		if (!FileOp_RecoveryData())
			//if(!FileOp_RestoreFromBinRegionForDM())
		{
			NVRAM_LOG("Bin Region Restore to NvRam Fail\n");
			//exit(EXIT_FAILURE);
		}
#ifdef MTK_NVRAM_AUTO_BACKUP
	}
#endif

	if (!Check_FileVerinFirstBoot()) {
		NVRAM_LOG("Check FILE_VER in first boot, and fail to generate FILE_VER\n");
	}

	//Add for product info feature
	if (nvram_new_partition_support()) {
		NVRAM_LOG("Begin excute customer callback!!!\n");
		init_callback();
		if (callback_for_nvram_daemon != NULL)
			ret_callback = callback_for_nvram_daemon();
		NVRAM_LOG("Callback return %d\n", ret_callback);
	}
	//end for product info feature

	if (!Init_AllThreads_In_Nvram()) {
		NVRAM_LOG("init all threads fail\n");
	} else {
		if (0 == property_set("vendor.service.nvram_init", "Ready")) {
			NVRAM_LOG("Set Ready property success\n");
		} else {
			NVRAM_LOG("Set property failed, %s\n", (char*)strerror(errno));
		}
	}

	NVRAM_LOG("NVRAM daemon sleep !\n");
	sleep(10);
	NVRAM_LOG("NVRAM daemon sync start !\n");
	sync();
	NVRAM_LOG("NVRAM daemon sync end !\n");
	NVRAM_LOG("NVRAM daemon exits !\n");
	exit(EXIT_SUCCESS);
}
