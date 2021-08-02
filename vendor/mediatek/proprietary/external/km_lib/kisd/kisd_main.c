/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/

#define LOG_TAG "kisd"

#include <cutils/log.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <uree/system.h>
#include <uree/mem.h>
#include <tz_cross/ta_drmkey.h>
#include <tz_cross/ta_mem.h>
#include <tz_cross/keyblock.h>
#include <keyblock_protect.h>

#include <cutils/sockets.h>
#include "kisd.h"

#include <linux/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <tz_cross/keyblock.h>
#include <keyblock_protect.h>
#include <mtd/mtd-abi.h>
#include <unistd.h>


#undef LOGE
#undef LOGV
#undef LOGD

//#define LOGE printf

//#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"kisd",__VA_ARGS__);
//#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"kisd",__VA_ARGS__);
#define LOGV printf

extern void factory_provision_str(char* str,...);
#define LOGE factory_provision_str
#define LOGD LOGE


#if 0
struct erase_info_user {
	__u32 start;
	__u32 length;
};

struct mtd_info_user {
	__u8 type;
	__u32 flags;
	__u32 size;	 // Total size of the MTD
	__u32 erasesize;
	__u32 writesize;
	__u32 oobsize;   // Amount of OOB data per block (e.g. 16)
	/* The below two fields are obsolete and broken, do not use them
	 * (TODO: remove at some point) */
	__u32 ecctype;
	__u32 eccsize;
};

#define MEMERASE		_IOW('M', 2, struct erase_info_user)
#define MEMGETINFO		_IOR('M', 1, struct mtd_info_user)
#endif

#define SERVER_NAME "kisd"
#define KEYBLOCK_SIZE 0x100000

int server_fd = -1;
int client_fd = -1;

int server_init()
{
	server_fd = socket_local_server(SERVER_NAME,ANDROID_SOCKET_NAMESPACE_ABSTRACT,SOCK_STREAM);
	if(server_fd < 0 ) return -1;
	listen(server_fd,4);
	return 0;
}

extern int (*earse_keyblock)(char*);
extern int keyblock_signature_add(unsigned char* kb,unsigned char* sig);
extern int keyblock_signature_check(unsigned char* kb);
extern int drmkey_operation_log(char* buff);
int store_keyblock_to_nvram_impl_kisd(char* KBPath,unsigned char* kb,int len)
{
	int fd = 0, n=0,of=0;
	unsigned char* signed_kb = NULL;
	int signed_kb_len = 0;

	if(len>KEYBLOCK_SIZE)
	{
		LOGE("Keyblock is larger than 1MB,pls adjust partition size!\n");
		return -1;
	}

	signed_kb_len = len+KB_SIGNATURE_SIZE;
	signed_kb = (unsigned char*)malloc(signed_kb_len);

	if(signed_kb == NULL)
	{
		LOGE("alloc buffer for signed_kb fail!\n");
		return -2;
	}
	memset(signed_kb,0,signed_kb_len);

	factory_provision_str("Preparing for writing keyblock %s.......\n",KBPath);

	if(memcmp(signed_kb,kb,len)==0)
	{
		//all zero case
		//do nothing
		factory_provision_str("Earsing keyblock case,keyblock %s will be easred!\n",KBPath);
	}
	else
	{
		memcpy(signed_kb,kb,len);
		keyblock_signature_add(signed_kb,signed_kb+len);
	}

	if( earse_keyblock(KBPath) != 0)
	{
		LOGE("Earse keyblock partition fail:%s!\n",KBPath);
		free(signed_kb);
		return -2;
	}
//write
	factory_provision_str("Writing keyblock....\n");
	fd = open(KBPath, O_RDWR);
	if(fd <0)
	{
		LOGE("ERROR open fail %d\n",fd);
		free(signed_kb);
		return -3;
	}
	of=lseek(fd,0,SEEK_SET);
	n=write(fd,signed_kb,signed_kb_len); //32 is kb signature

	if(n!=signed_kb_len && n!=len){
		LOGE("ERROR write fail");
		free(signed_kb);
		close(fd);
		return -4;
	}
	free(signed_kb);
	close(fd);
	factory_provision_str("Write keyblock successfully\n");
	return 0;
}

int get_keyblock_from_nvram_impl_kisd(char* KBPath,unsigned char* buff,int len)
{
	FILE *a;
    //unsigned char* buff = NULL;
    //struct stat f_info;
    a = fopen ( KBPath, "rb" );

    if ( a == NULL ) {
        LOGE ( "open %s fail\n",KBPath );
        return -1;
    }

    //stat ( KEYBLOCK_PATH, &f_info );
    //buff = ( unsigned char * ) malloc ( f_info.st_size );
    //buff = ( unsigned char * ) malloc ( len );

    if ( buff == NULL ) {
		//ALOGE("%d bytes memory allcation fail at %s:%d\n",KEYBLOCK_SIZE,__FILE__,__LINE__);
		LOGE("%d bytes memory allcation fail at %s:%d\n",KEYBLOCK_SIZE,__FILE__,__LINE__);
        fclose(a);
        return -2;
    }

    //fread ( buff, f_info.st_size, 1, a );

    int count = fread ( buff, len, 1, a );
    if(count != 1)
    {
        //read might be failed,but just warning
        LOGE ( "read keyblock from kisd fail.count = %d.\n",count );
    }

    fclose(a);
    if(memcmp(buff,"KBP",3)!= 0)
    {
        //ALOGE("keyblock header is not vailded,maybe blank\n");
        return 0;
    }

	if(keyblock_signature_check(buff)!=0)
	{
		memset(buff,0,len);
	    LOGE("keyblock %s signature is not vailded\n",KBPath);
		factory_provision_str("Keyblock signature verify fail!");
		return 0;
	}
    return 0;
}


int flash_read_handler(int l_client_fd,READ_WRITE_COMMAND_T* command)
{
    READ_WRITE_COMMAND_RESULT_T* result;
    result = (READ_WRITE_COMMAND_RESULT_T*)malloc(sizeof(READ_WRITE_COMMAND_RESULT_T)+command->data_len);
    if(!result)
    {
        LOGE("flash read for handler:allocate memory failed.\n");
        return -1;
    }
    FUNC_WRAPPER(get_keyblock_from_nvram_impl_kisd(command->path,result->buff,command->data_len),0);
    result->header.result=RESULT_OK;
    result->data_len = command->data_len;
    result->header.total_len = sizeof(READ_WRITE_COMMAND_RESULT_T)+command->data_len;
    LOGV("Need read 0x%x length buffer!\n",result->data_len);
    LOGV("write result to client 0x%x!\n",(unsigned int)full_write(l_client_fd,result,result->header.total_len) );
    free(result);
    return 0;
}

int flash_write_handler(int l_client_fd,READ_WRITE_COMMAND_T* command)
{
	READ_WRITE_COMMAND_RESULT_T result;
	LOGE("write command path:%s len:0x%x\n",command->path,command->data_len);
	FUNC_WRAPPER(store_keyblock_to_nvram_impl_kisd(command->path,command->buff,command->data_len),0);
	result.header.result = RESULT_OK;
	result.data_len = command->data_len;
	result.header.total_len = sizeof(READ_WRITE_COMMAND_RESULT_T);
	LOGE("write result to client 0x%x!\n",(unsigned int)full_write(l_client_fd,&result,result.header.total_len));
	return 0;
}

int merge_install_kb_handler(int l_client_fd,MERGE_INSTALL_KB_COMMAND_T* command)
{
	MERGE_INSTALL_KB_COMMAND_RESULT_T result;
	FUNC_WRAPPER(merge_install_kb(command->reEncKB,command->reEncKB_len,command->temp_file_path,command->target_path),0);
	result.header.result = RESULT_OK;
	result.header.total_len = sizeof(MERGE_INSTALL_KB_COMMAND_RESULT_T);
	FUNC_WRAPPER(full_write(l_client_fd,&result,result.header.total_len),result.header.total_len);
	return 0;
}


extern unsigned char Input_Ekkb_pub[];
extern unsigned char InputPkb[];

int init_drmkey_env(void)
{
    DRMKEY_ENV_T* env;
    TZ_RESULT ret;
    UREE_SESSION_HANDLE session;
    MTEEC_PARAM param[4];
    ret = UREE_CreateSession(TZ_TA_DRMKEY_UUID, &session);
    if (ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CreateSession Error: %s\n", TZ_GetErrorString(ret));
        return -1;
    }

    env=(DRMKEY_ENV_T*)malloc(sizeof(DRMKEY_ENV_T));
    if(!env)
    {
        LOGE("init drmkey: memory allocate failed.\n");
        return -1;
    }

    memcpy(env->_Ekkb_pub_,Input_Ekkb_pub,256);
    memcpy(env->_Pkb_,InputPkb,129);

    param[0].mem.buffer = env;
    param[0].mem.size = sizeof(DRMKEY_ENV_T);

    ret = UREE_TeeServiceCall(session, TZCMD_DRMKEY_INIT_ENV,
        TZ_ParamTypes1(TZPT_MEM_INPUT), param);
    if (ret != TZ_RESULT_SUCCESS)
    {
        LOGE("TZCMD_DRMKEY_INIT_ENV Error: %s\n", TZ_GetErrorString(ret));
        return -3;
    }

    ret = UREE_CloseSession(session);
    if (ret != TZ_RESULT_SUCCESS)
    {
        LOGE("CloseSeesion session Error: %d\n", ret);
    }

    return 0;
}

extern int access(const char* pathname,int mode);

int file_exist(const char* pathname)
{
	return (access(pathname,0)==0);
}

#define KB_TEMP_DIR 		"/data/vendor/key_provisioning/"
#define KBO_TEMP_FILE_PATH 	KB_TEMP_DIR"/KBO_BIN"
#define KBF_TEMP_FILE_PATH 	KB_TEMP_DIR"/KBF_BIN"
#define KB_EKKB_PATH 		KB_TEMP_DIR"/KB_EKKB"
#define FORCE_KB_EKKB_PATH 	KB_TEMP_DIR"/FORCE_KB_EKKB"
#define KB_PM_PATH 			KB_TEMP_DIR"/KB_PM"
#define FORCE_KB_PM_PATH 	KB_TEMP_DIR"/FORCE_KB_PM"
#define KB_PD_PATH 			KB_TEMP_DIR"/KB_PD"
#define KB_MIX_PATH 		KB_TEMP_DIR"/KB_MIX"
#define KB_OTA_PATH 		KB_TEMP_DIR"/KB_OTA"


#include <cutils/properties.h>

#define KEYBLOCK_F_PATH "/dev/kb"
#define KEYBLOCK_O_PATH "/dev/dkb"

void wait_for_env_ready(void)
{
	while(1)
	{
		int fd = -1;
		TZ_RESULT ret;
		UREE_SESSION_HANDLE session;
		LOGE("Waiting for env ready...\n");
		fd = open(KEYBLOCK_F_PATH, O_RDWR);
		if(fd < 0)
		{
			LOGE("kisd open %s fail\n", KEYBLOCK_F_PATH);
			sleep(1);
			continue;
		}
		close(fd);
		fd = -1;
		fd = open(KEYBLOCK_O_PATH, O_RDWR);
		if(fd < 0)
		{
			LOGE("kisd open dkb fail\n");
			sleep(1);
			continue;
		}
		close(fd);
		ret = UREE_CreateSession(TZ_TA_DRMKEY_UUID, &session);
		if (ret != TZ_RESULT_SUCCESS)
		{
			LOGE("kisd create session {TZ_TA_DRMKEY_UUID} failed\n");
			sleep(1);
			continue;
		}
		UREE_CloseSession(session);
		break;
	}
	LOGE("Env is ready...\n");
}

static int check_header_parameters(KISD_RPC_COMMAND_HEADER* header)
{
    unsigned int merge_size = sizeof(MERGE_INSTALL_KB_COMMAND_T);
    unsigned int rw_size = sizeof(READ_WRITE_COMMAND_T);
    if(header->command != FLASH_READ && header->command != FLASH_WRITE && header->command != MERGE_INSTALL_KB)
    {
        LOGE("Invalid input command:%d.\n",header->command);
        return -1;
    }

    //for MERGE_INSTALL_KB,check length
    if(header->command == MERGE_INSTALL_KB){
        if((header->total_len > (merge_size + KEYBLOCK_SIZE)) || (header->total_len < merge_size))
        {
            LOGE("Invalid input total_len:%u.\n",header->total_len);
            return -2;
        }
        return 0;
    }

    //for flash read/write
    if((header->total_len < rw_size) || (header->total_len > (rw_size + KEYBLOCK_SIZE)))
    {
        LOGE("Invalid input total_len:%u.\n",header->total_len);
        return -3;
    }

    return 0;
}

static int return_fail_result(int l_client_fd)
{
    READ_WRITE_COMMAND_RESULT_T result;
    LOGE("parameter check fail!\n");
    result.header.result = RESULT_FAIL;
    result.data_len = 0;
    result.header.total_len = sizeof(READ_WRITE_COMMAND_RESULT_T);
    LOGE("write result to client 0x%x!\n",(unsigned int)full_write(l_client_fd,&result,result.header.total_len));
    return 0;
}
static int check_falsh_operation_parameters(READ_WRITE_COMMAND_T* buff)
{
    if((strcmp(KEYBLOCK_F_PATH, buff->path)!=0) && (strcmp(KEYBLOCK_O_PATH, buff->path)!=0))
    {
        LOGE("Invalid input path.\n");
        return -1;
    }

    //data_len is changed to unsigned int type
    if((buff->data_len > KEYBLOCK_SIZE) || (0 == buff->data_len))
    {
        LOGE("Invalid input data_len:%u.\n",buff->data_len);
        return -2;
    }

    //for flash write cmd,data len should not exceed total len
    if(buff->header.command == FLASH_WRITE){
        if((buff->data_len + sizeof(READ_WRITE_COMMAND_T)) > buff->header.total_len){
            LOGE("Invalid input for flash write cmd.data_len=%d,total_len = %d.\n",buff->data_len,buff->header.total_len);
            return -2;
        }
    }

    return 0;
}

static int check_MERGE_INSTALL_KB_parameters(MERGE_INSTALL_KB_COMMAND_T* buff)
{
    if((buff->reEncKB_len > KEYBLOCK_SIZE) || (0 == buff->reEncKB_len))
    {
        LOGE("Invalid input reEncKB_len:%u.\n",buff->reEncKB_len);
        return -1;
    }

    if(strcmp(KBO_TEMP_FILE_PATH, buff->temp_file_path)!=0
         && strcmp(KBF_TEMP_FILE_PATH, buff->temp_file_path)!=0)
    {
        LOGE("Invalid input temp file path.\n");
        return -2;
    }
    if(strcmp(KEYBLOCK_F_PATH, buff->target_path)!=0
         && strcmp(KEYBLOCK_O_PATH, buff->target_path)!=0)
    {
        LOGE("Invalid input target file path.\n");
        return -3;
    }
    //write buffer should not exceed total length
    if((buff->reEncKB_len + sizeof(MERGE_INSTALL_KB_COMMAND_T)) > buff->header.total_len){
        LOGE("Invalid input for merge cmd.EncKB_len=%d,total_len = %d.\n",buff->reEncKB_len,buff->header.total_len);
        return -4;
    }

    return 0;
}

extern void factory_provision_enable_kisd_mode(void);

int kisd_main(int argc,char* argv[])
{
	wait_for_env_ready();
	factory_provision_enable_kisd_mode();
	FUNC_WRAPPER(init_drmkey_env(),0);
	if(argc < 2)
	{
		switch_flash_operaion_mode(FLASH_OPERATION_LOCAL);

		if(file_exist(KBF_TEMP_FILE_PATH))
		{
			FUNC_WRAPPER(restore_kbf_for_KISD(),0);
		}
		if(file_exist(KBO_TEMP_FILE_PATH))
		{
			FUNC_WRAPPER(restore_kbo_for_KISD(),0);
		}
		if(file_exist(KB_EKKB_PATH) || file_exist(FORCE_KB_EKKB_PATH))
		{
			FUNC_WRAPPER(install_KB_EKKB_for_KISD(),0);
		}
		if(file_exist(KB_PM_PATH) || file_exist(FORCE_KB_PM_PATH))
		{
			FUNC_WRAPPER(install_KB_PM_for_KISD(),0);
		}
		if(file_exist(KB_PD_PATH))
		{
			FUNC_WRAPPER(install_KB_PD_for_KISD(),0);
		}
		if(file_exist(KB_MIX_PATH))
		{
			FUNC_WRAPPER(install_KB_MIX_for_KISD(),0);
		}
		if(file_exist(KB_OTA_PATH))
		{
			FUNC_WRAPPER(install_KB_OTA_for_KISD(),0);
		}

        //query drm key status.
        FUNC_WRAPPER(query_drmkey_status(),0);

        FUNC_WRAPPER(server_init(),0);
        while((client_fd = accept(server_fd,NULL,NULL)) > 0)
        {
            KISD_RPC_COMMAND_HEADER command;
            memset(&command, 0, sizeof(KISD_RPC_COMMAND_HEADER));
            unsigned char * buff = NULL;
            int left_bytes = 0;
            LOGV("connected!\n");
            if ( -1 == full_read(client_fd, &command, sizeof(KISD_RPC_COMMAND_HEADER)))
            {
                LOGE("full_read failed, and break!\n");
                close(client_fd);
                continue;
            }

            if(check_header_parameters(&command) != 0)
            {
                LOGE("check_header_parameters failed, and continue!\n");
                return_fail_result(client_fd);
                close(client_fd);
                continue;
            }
            buff = (unsigned char*)calloc(command.total_len,sizeof(char));
            if (buff == NULL)
            {
                LOGE("calloc failed, and continue!\n");
                close(client_fd);
                continue;
            }
            memcpy(buff,&command,sizeof(KISD_RPC_COMMAND_HEADER));
            left_bytes = command.total_len-sizeof(KISD_RPC_COMMAND_HEADER);
            if ( -1 == full_read(client_fd, buff + sizeof(KISD_RPC_COMMAND_HEADER), left_bytes))
            {
                LOGE("full_read failed, and continue!\n");
                close(client_fd);
                if (buff != NULL) free(buff);
                continue;
            }
            switch(command.command)
            {
                case FLASH_READ:
                    LOGV("FLASH_READ!\n");
                    if(check_falsh_operation_parameters((READ_WRITE_COMMAND_T *)buff)!=0)
                    {
                        return_fail_result(client_fd);
                        close(client_fd);
                        if (buff != NULL) free(buff);
                        continue;
                    }
                    flash_read_handler(client_fd,(READ_WRITE_COMMAND_T*)buff);
                    LOGV("FLASH_READ finished!\n");
                    break;

                case FLASH_WRITE:
                    LOGV("FLASH_WRITE!\n");
                    if(check_falsh_operation_parameters((READ_WRITE_COMMAND_T *)buff)!=0)
                    {
                        return_fail_result(client_fd);
                        close(client_fd);
                        if (buff != NULL) free(buff);
                        continue;
                    }
                    flash_write_handler(client_fd,(READ_WRITE_COMMAND_T*)buff);
                    LOGV("FLASH_WRITE finished!\n");
                    break;

                case MERGE_INSTALL_KB:
                    LOGV("MERGE_INSTALL_KB!\n");
                    if(check_MERGE_INSTALL_KB_parameters((MERGE_INSTALL_KB_COMMAND_T *)buff)!=0)
                    {
                        LOGE("check_MERGE_INSTALL_KB_parameters failed!\n");
                        return_fail_result(client_fd);
                        close(client_fd);
                        if (buff != NULL) free(buff);
                        continue;
                    }
                    merge_install_kb_handler(client_fd,(MERGE_INSTALL_KB_COMMAND_T*)buff);
                    LOGV("MERGE_INSTALL_KB finished!\n");
                    break;

                default:
                    break;
            }
            if (buff != NULL) free(buff);
            close(client_fd);

        }
    }
    return 0;
}
