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
#ifndef __KEY_BLOCK_PROTECT_H__
#define __KEY_BLOCK_PROTECT_H__

#ifdef __cplusplus
extern "C" {
#endif

//begin of internal using

//for SignKB
#define OFFSET_ENCKB_HEADER 0
#define SZ_ENCKB_HEADER 4
#define OFFSET_ENCKB_HEADER_RESERVED (OFFSET_ENCKB_HEADER+SZ_ENCKB_HEADER)
#define SZ_ENCKB_HEADER_RESERVED 64
#define OFFSET_ENCKB_HEADER_SIZE (OFFSET_ENCKB_HEADER_RESERVED+SZ_ENCKB_HEADER_RESERVED)
#define SZ_ENCKB_HEADER_SIZE 4
#define OFFSET_ENCKB_HEADER_KEYCOUNT (OFFSET_ENCKB_HEADER_SIZE+SZ_ENCKB_HEADER_SIZE)
#define SZ_ENCKB_HEADER_KEYCOUNT 4
#define OFFSET_ENCKB_HEADER_HASEKKB (OFFSET_ENCKB_HEADER_KEYCOUNT+SZ_ENCKB_HEADER_KEYCOUNT)
#define SZ_ENCKB_HEADER_HASEKKB 4
#define OFFSET_ENCKB_HEADER_EKKB (OFFSET_ENCKB_HEADER_HASEKKB+SZ_ENCKB_HEADER_HASEKKB)
#define SZ_ENCKB_HEADER_EKKB 256
#define SZ_ENCKB_TOTAL_HEADER (OFFSET_ENCKB_HEADER_EKKB+SZ_ENCKB_HEADER_EKKB)
#define SZ_ENCKB_SIG 256


#define OFFSET_REENCKB_HEADER 0
#define SZ_REENCKB_HEADER 4
#define OFFSET_REENCKB_HEADER_RESERVED (OFFSET_REENCKB_HEADER+SZ_REENCKB_HEADER)
#define SZ_REENCKB_HEADER_RESERVED 64
#define OFFSET_REENCKB_HEADER_KEYCOUNT (OFFSET_REENCKB_HEADER_RESERVED+SZ_REENCKB_HEADER_RESERVED)
#define SZ_REENCKB_HEADER_KEYCOUNT 4
#define SZ_REENCKB_TOTAL_HEADER (OFFSET_REENCKB_HEADER_KEYCOUNT+SZ_REENCKB_HEADER_KEYCOUNT)

typedef enum
{
    KB_TYPE_MIX = 0,
    KB_TYPE_PM,
    KB_TYPE_PD,
    KB_TYPE_MAX,

    KB_TYPE_SPECIAL_DEV_GEN = 0x100,
    KB_TYPE_SPECIAL_OTA = 0x200,
    KB_TYPE_SPECIAL_PLAIN_CONTENT=0x300,
    KB_TYPE_SPECIAL_MAX
}
DRMKeyType;

#define KB_SIGNATURE_SIZE 16
typedef enum
{
    SigOperaionType_ADD = 1,
    SigOperaionType_VERIFY
}
KB_SigOperaionType;

typedef struct {
    DRMKeyID drmKeyID;
    DRMKeyType drmKeyType;
    unsigned int drmKeySize;
    unsigned int encDrmKeySize;
    unsigned char clearIV[16];
    unsigned char reserved[48];
    unsigned char * drmKey;
    unsigned char * drmKeySignature; //16 bytes
} ENCRYPT_DRM_KEY_T;

typedef struct {
    unsigned char BMagicNumber[4]; // now will use "EKKB"
    unsigned int totalSize;
    unsigned int keyCount;
    unsigned char EKkb[32];
    unsigned char SIG[16];
    unsigned char EMagicNumber[4]; // now will use "EKKB"
} KB_EKKB_T;

typedef struct {
    DRMKeyID drmKeyID;
    DRMKeyType drmKeyType;
    unsigned int drmKeySize;
    unsigned int encDrmKeySize;
    unsigned char reserved[64];
    unsigned char drmKey[32];
    unsigned char drmKeySignature[16]; //16 bytes
} ENCRYPT_DRM_EKKB_T;

typedef struct
{
    unsigned char _Ekkb_pub_[256];
    unsigned char _Pkb_[129];
}
DRMKEY_ENV_T;

#define FUNC_WRAPPER(FUNC_CALL,EXCEPTED) \
    ({ \
    int ret = FUNC_CALL; \
    if((ret)!=(EXCEPTED)) \
        LOGE("Return value error at %s:%d,result is 0x%x,excepted is 0x%x\n",__FILE__,__LINE__,ret,(EXCEPTED)); \
    ret; \
    })

extern int process_encrypt_key_block ( unsigned char *encryptKbk, unsigned int inLength,
                                unsigned char **reEncryptKbk, unsigned int *outLength );

extern int free_encrypt_key_block ( unsigned char *reEncryptKbk );

/*
[in] encDrmKeyBlock 	encrypt DRMBlock get from get_encrypt_drmkey
[in] inLength			encrypt DRMBlock length get from get_encrypt_drmkey
[out] DrmKey			clear DRMkey
[out] outLength 		clear DRMkey length

return	 0: OK,  others: FAIL
*/

int generateEkkb_pub(unsigned char* Pkb,unsigned int PkbLen,
                     unsigned char* Kkb_pub,unsigned int Kkb_pubLen,
                     unsigned char** EKkb_pub,unsigned int *EKkb_pubLen);


int generateEKkb_EKc(unsigned char* Kkb,unsigned int KkbLen,
                     unsigned char* Ck,unsigned int CkLen,
                     unsigned char** KB_EKkb,unsigned int *KB_EKkbLen,
                     unsigned char** ECk,unsigned int *ECkLen);

int process_encrypt_kb_ekkb(unsigned char *encryptKB_Ekkb, unsigned int inLength,
                            unsigned char **reEncryptDrmEkkb, unsigned int *outLength);
//end of internal using

// begin only for kisd, please do not use these functions
typedef enum
{
    FLASH_OPERATION_LOCAL = 0,
    FLASH_OPERATION_RPC = 1
}FLASH_OPERATION_MODE;

extern unsigned char* (*get_keyblock_from_nvram_impl)(char*);
extern int (*store_keyblock_to_nvram_impl)(char* ,unsigned char*,int);
extern int (*merge_install_kb)(unsigned char*,unsigned int,char*,char*);
int flash_write_rpc_agent(char* path,unsigned char* buff,unsigned int len);
int flash_read_rpc_agent(char* path,unsigned char* buff,unsigned int len);

void switch_flash_operaion_mode(FLASH_OPERATION_MODE mode);

int restore_kbf_for_KISD(void);

int restore_kbo_for_KISD(void);

int install_KB_EKKB_for_KISD(void);

int install_KB_EKKB_for_Debug(char *path);

int install_KB_PM_for_KISD(void);

int install_KB_PM_for_Debug(char *path);

int install_KB_PD_for_KISD(void);

int install_KB_MIX_for_KISD(void);

int install_KB_OTA_for_KISD(void);

int query_drmkey_status(void);

//end for kisd using

#ifdef __cplusplus
}
#endif

#endif /* __KEY_BLOCK_PROTECT_H__ */
