/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2011
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

#include <platform/sec_status.h>
#include <platform/mt_typedefs.h>
#include <platform/sec_export.h>
#include <debug.h>
#include <part_status.h>
#include <profiling.h>
#include "sec_logo_auth.h"

#define MOD                             "SBC"

extern void mtk_wdt_restart(void);

int img_auth_stor(char *part_name, char *img_name, uint32_t feature_mask)
{

	int ret = B_OK;
	unsigned int policy_entry_idx = 0;
	unsigned int img_auth_required = 0;

	dprintf(CRITICAL,"[%s] Enter img auth check \n", MOD);

	/* partition does not exist, so we don't do verification */
	if (PART_OK != partition_exists(part_name)) {
		dprintf(CRITICAL,"[%s] %s does not exist, bypass\n", MOD, part_name);
		return B_OK;
	}

	seclib_image_buf_init();

	PROFILING_START("img_auth_stor");

	policy_entry_idx = get_policy_entry_idx(part_name);
	img_auth_required = get_vfy_policy(policy_entry_idx);

	if (img_auth_required) {
		mtk_wdt_restart();
		ret = sec_img_auth_init(part_name, img_name, 0);
		if (B_OK != ret) {
			dprintf(CRITICAL,"Verified boot is enabled.\n");
			dprintf(CRITICAL,"Please download %s image with corret signature or disable verified boot.\n", img_name);
			goto fail;
		}

		mtk_wdt_restart();
		ret = sec_img_auth_stor(part_name, img_name);
		if (B_OK != ret)
			goto fail;
	}

	PROFILING_END();

	seclib_image_buf_free();

	return B_OK;

fail:
	dprintf(CRITICAL,"[%s] Fail (0x%x)\n",MOD,ret);
	return ret;
}
