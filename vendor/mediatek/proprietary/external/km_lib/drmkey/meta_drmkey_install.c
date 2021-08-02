/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cutils/log.h>
#include <tz_cross/keyblock.h>
#include <keyblock_protect.h>

//#include "FT_Public.h"
//#include "meta_common.h"
#include "MetaPub.h"
#include "meta_drmkey_install_para.h"

unsigned char *encKeyBlock = NULL;
unsigned char *p = NULL;
unsigned int encKeyBlockLen = 0;

void META_DRMKEY_INSTALL_OP( FT_DRMKEY_INSTALL_REQ *req,  FT_DRMKEY_INSTALL_CNF *drmkey_cnf,
                             char *peer_buff, unsigned short peer_len )
{
    int ret = 0;

    memset(drmkey_cnf, 0, sizeof(FT_DRMKEY_INSTALL_CNF));
    drmkey_cnf->header.id = req->header.id+1; // FT_DRM_KEY_INSTALL_CNF_ID
    drmkey_cnf->header.token = req->header.token;
    drmkey_cnf->op = req->op;

    ALOGE("req->op:%d\n", req->op);

    switch(req->op){
    case FT_DRMKEY_INSTALL_SET:

        drmkey_cnf->status = DRMKEY_INSTALL_OK;

        ALOGE("FT_DRMKEY_INSTALL_SET receive block stage %x, file size %d!", req->cmd.set_req.stage, req->cmd.set_req.file_size);
        if(req->cmd.set_req.stage & KEY_BLK_CREATE)
        {
                encKeyBlock = (unsigned char *) malloc (req->cmd.set_req.file_size);
                if (encKeyBlock == NULL)
                {
                    drmkey_cnf->status = DRMKEY_INSTALL_FAIL;
                    break;
                }

                p = encKeyBlock;
        }

        if(encKeyBlock != NULL)
        {
            memcpy(p, peer_buff, peer_len);
            p += peer_len;
            encKeyBlockLen += peer_len;

            if(req->cmd.set_req.stage & KEY_BLK_EOF)
            {
                if (encKeyBlockLen == req->cmd.set_req.file_size)
                {
                    extern int install_KB_MIX_OR_PD_API(unsigned char* buff,unsigned int len);
                    ALOGE("FT_DRMKEY_INSTALL_SET end of file, start to process encrypt key block, size = %d\n", encKeyBlockLen);
                    ret = install_KB_MIX_OR_PD_API(encKeyBlock,encKeyBlockLen);
                    if (ret == 0)
                    {
                        // 2. store the result to NVRAM or Raw partition, if fail then drmkey_cnf.status = DRMKEY_INSTALL_FAIL
                        /////////////// now store to file directly ////////////////
                        drmkey_cnf->status = DRMKEY_INSTALL_OK;
                        ///////////////////////////////////////////////////////////
                    }
                    else
                    {
                        drmkey_cnf->status = DRMKEY_INSTALL_FAIL;
                    }

                    free(encKeyBlock);
                    encKeyBlock = NULL;
                    p = encKeyBlock;
                    encKeyBlockLen = 0;
                }
            }
        }

        break;

    case FT_DRMKEY_INSTALL_QUERY:
        drmkey_cnf->status = DRMKEY_INSTALL_OK;

        ret = query_drmkey(&drmkey_cnf->result.keyresult.keycount, drmkey_cnf->result.keyresult.keytype);
        if (ret == 0)
        {
            printf("keycount = %d\n", drmkey_cnf->result.keyresult.keycount);
        }
        else
        {
            drmkey_cnf->status = DRMKEY_INSTALL_FAIL;
        }

        break;

    default:
        ALOGE("DRM KEY INSTALL Not support OPCODE:req->op [%d]\n", req->op);

        drmkey_cnf->header.id = req->header.id+1; // FT_DRM_KEY_INSTALL_CNF_ID
        drmkey_cnf->op = req->op;
        drmkey_cnf->status = DRMKEY_INSTALL_FAIL;

        break;
    }

    return;
}

