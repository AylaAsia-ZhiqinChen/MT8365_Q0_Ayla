/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

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
#define LOG_TAG "DSI"

#define ENABLE_DSI_INTERRUPT 0
#include <platform/ddp_info.h>


#include <platform/mt_typedefs.h>
#include <platform/sync_write.h>

#include <platform/disp_drv_platform.h>
#include <platform/disp_drv_log.h>
/* #include <debug.h> */
/* #include <platform/ddp_path.h> */

#include <platform/ddp_manager.h>
/* #include <platform/ddp_dump.h> */



#include <platform/ddp_reg.h>
#include <platform/ddp_dsi.h>

#include <debug.h>

/*...below is new dsi driver...*/
#define IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII 0
static int dsi_reg_op_debug;
static int mipi_reg_op_debug = 0;

#define DSI_OUTREG32(cmdq, addr, val) \
	{\
		if (dsi_reg_op_debug) \
			DISPMSG("[dsi/reg]0x%08x=0x%08x, cmdq:0x%08x\n", (unsigned int)addr, val, (unsigned int)cmdq);\
		if (cmdq) \
			{} \
		else \
			mt_reg_sync_writel(val, addr); }


#define BIT_TO_VALUE(TYPE, bit)  \
		do {    TYPE r;\
			*(unsigned int *)(&r) = ((unsigned int)0x00000000);	  \
			r.bit = ~(r.bit);\
			r;\
		    } while (0);\

#define DSI_MASKREG32(cmdq, REG, MASK, VALUE)  \
	{\
		if (cmdq)	\
			{} \
		else\
			DSI_OUTREG32(cmdq, REG, (INREG32(REG)&~(MASK))|(VALUE));\
	}

#define DSI_OUTREGBIT(cmdq, TYPE, REG, bit, value)  \
	{\
	if (cmdq)\
	{do {\
	} while (0); } \
	else\
	{\
		do {	\
			TYPE r = *((TYPE *)&INREG32(&REG));	  \
			r.bit = value;	  \
			DSI_OUTREG32(cmdq, &REG, AS_UINT32(&r));	  \
			} while (0);\
	} }

#ifdef MACH_FPGA
#define MIPITX_I2C_BASE 0x1401C000
#define MIPITX_Write60384(slave_addr, write_addr, write_data)			\
{	\
	printf("MIPITX_Write60384:0x%x,0x%x,0x%x\n",slave_addr,write_addr,write_data);		\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x50);		\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x38);		\
	mt_reg_sync_writel(0xFF,MIPITX_I2C_BASE+0xC);		\
	mt_reg_sync_writel(0x2,MIPITX_I2C_BASE+0x14);		\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x18);		\
	mt_reg_sync_writel(((unsigned int)slave_addr << 0x1),MIPITX_I2C_BASE+0x04);		\
	mt_reg_sync_writel(write_addr,MIPITX_I2C_BASE+0x0);		\
	mt_reg_sync_writel(write_data,MIPITX_I2C_BASE+0x0);		\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x24);		\
	mt_reg_sync_writel(0xFF,MIPITX_I2C_BASE+0xC);		\
	while((INREG32(MIPITX_I2C_BASE+0xC) & 0x01) != 0x01); \
	mt_reg_sync_writel(0xFF,MIPITX_I2C_BASE+0xC);		\
	\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x14);		\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x18);		\
	mt_reg_sync_writel(((unsigned int)slave_addr << 0x1),MIPITX_I2C_BASE+0x04);		\
	mt_reg_sync_writel(write_addr,MIPITX_I2C_BASE+0x0);		\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x24);		\
	mt_reg_sync_writel(0xFF,MIPITX_I2C_BASE+0xC);		\
	while((INREG32(MIPITX_I2C_BASE+0xC) & 0x01) != 0x01); \
	mt_reg_sync_writel(0xFF,MIPITX_I2C_BASE+0xC);		\
	\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x14);		\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x18);		\
	mt_reg_sync_writel(((unsigned int)slave_addr << 0x1)+1,MIPITX_I2C_BASE+0x04);		\
	mt_reg_sync_writel(0x1,MIPITX_I2C_BASE+0x24);		\
	mt_reg_sync_writel(0xFF,MIPITX_I2C_BASE+0xC);		\
	while((INREG32(MIPITX_I2C_BASE+0xC) & 0x01) != 0x01); \
	mt_reg_sync_writel(0xFF,MIPITX_I2C_BASE+0xC);		\
	\
	if(INREG32(MIPITX_I2C_BASE) == write_data)printf("MIPI write success\n");		\
	else printf("MIPI write fail\n");		\
}

#define MIPITX_INREG32(addr)								\
	{														\
		unsigned int val = 0;										\
		if(mipi_reg_op_debug) 								\
		{													\
			DISPMSG("[mipitx/inreg]0x%08x=0x%08x\n", addr, val);	\
		}													\
		val;													\
	}

#define MIPITX_OUTREG32(addr, val) \
	{\
		if (mipi_reg_op_debug) \
		{	DISPMSG("[mipitx/reg]0x%08x=0x%08x\n", (unsigned int)addr, val); } \
	}
#define MIPITX_OUTREGBIT(TYPE, REG, bit, value)  \
	{\
		do {	\
			TYPE r;\
			*(unsigned int *)(&r) = ((unsigned int)0x00000000);	  \
			r.bit = value;	  \
			MIPITX_OUTREG32(&REG, AS_UINT32(&r));	  \
			} while (0);\
	}
#define MIPITX_MASKREG32(x, y, z)  MIPITX_OUTREG32(x, (MIPITX_INREG32(x)&~(y))|(z))
#else
#define MIPITX_INREG32(addr)								\
	{														\
		unsigned int val = 0;										\
		val = INREG32(addr);								\
		if(mipi_reg_op_debug) 								\
		{													\
			DISPMSG("[mipitx/inreg]0x%08x=0x%08x\n", addr, val);	\
		}													\
		val;													\
	}

#define MIPITX_OUTREG32(addr, val) \
	{\
		if (mipi_reg_op_debug) \
		{	\
			DISPMSG("[mipitx/reg]0x%08x=0x%08x\n", (unsigned int)addr, val);\
		} \
		mt_reg_sync_writel(val, addr);\
	}

#define MIPITX_OUTREGBIT(TYPE, REG, bit, value)  \
	{\
		do {	\
			TYPE r;\
			r = *((TYPE *)&INREG32(&REG));	  \
			r.bit = value;	  \
			MIPITX_OUTREG32(&REG, AS_UINT32(&r));	  \
			} while (0);\
	}

#define MIPITX_MASKREG32(x, y, z)  MIPITX_OUTREG32(x, (MIPITX_INREG32(x)&~(y))|(z))
#endif

#define DSI_POLLREG32(cmdq, addr, mask, value)  \
	do {\
		{} \
	} while (0);

#define DSI_INREG32(type, addr)												 \
		({																 \
		    unsigned int var = 0;                                            \
			union p_regs													 \
			{																 \
				type p_reg;													 \
				unsigned int *p_uint;										 \
			} p_temp1;														 \
			p_temp1.p_reg  = (type)(addr);									 \
			var = INREG32(p_temp1.p_uint);									 \
			var;															 \
		})

#define DSI_READREG32(type, dst, src)	                             \
			{																	 \
				union p_regs													 \
				{																 \
					type p_reg;												 \
					unsigned int *p_uint;										 \
				} p_temp1, p_temp2;												 \
				p_temp1.p_reg  = (type)(dst);								 \
				p_temp2.p_reg  = (type)(src);								 \
				OUTREG32(p_temp1.p_uint, INREG32(p_temp2.p_uint)); }


typedef struct {
	void *handle;
	bool enable;
	volatile struct DSI_REGS regBackup;
	unsigned int cmdq_size;
	LCM_DSI_PARAMS dsi_params;
} t_dsi_context;

t_dsi_context _dsi_context[DSI_INTERFACE_NUM];
#define DSI_MODULE_BEGIN(x)	(0)
#define DSI_MODULE_END(x)		(0)
#define DSI_MODULE_to_ID(x)	(0)
#define DIFF_CLK_LANE_LP 0X10

struct DSI_REGS *DSI_REG[2];
struct DSI_PHY_REGS *DSI_PHY_REG[2];
struct DSI_CMDQ_REGS *DSI_CMDQ_REG[2];
struct DSI_VM_CMDQ_REGS *DSI_VM_CMD_REG[2];

void DSI_PHY_clk_setting(DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params);
DSI_STATUS DSI_DumpRegisters(DISP_MODULE_ENUM module, void *cmdq, int level);


static void _DSI_INTERNAL_IRQ_Handler(DISP_MODULE_ENUM module, unsigned int param)
{
}


static DSI_STATUS DSI_Reset(DISP_MODULE_ENUM module, void *cmdq)
{
	int i = 0;

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG, DSI_REG[i]->DSI_COM_CTRL, DSI_RESET, 1);
		DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG, DSI_REG[i]->DSI_COM_CTRL, DSI_RESET, 0);
	}

	return DSI_STATUS_OK;
}

static int _dsi_is_video_mode(DISP_MODULE_ENUM module)
{
	if (DSI_REG[0]->DSI_MODE_CTRL.MODE == CMD_MODE)
		return 0;
	else
		return 1;
}

static DSI_STATUS DSI_SetMode(DISP_MODULE_ENUM module, void *cmdq, unsigned int mode)
{
	int i = 0;

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++)
		DSI_OUTREGBIT(cmdq, struct DSI_MODE_CTRL_REG, DSI_REG[i]->DSI_MODE_CTRL, MODE, mode);

	return DSI_STATUS_OK;
}

static void DSI_WaitForNotBusy(DISP_MODULE_ENUM module, void *cmdq)
{
    int timeOut = 500000;

	if(DSI_REG[0]->DSI_MODE_CTRL.MODE != CMD_MODE)
	{
		return;
	}

	while(ddp_dsi_is_busy(module))
	{
		udelay(100);
		if (--timeOut < 0) {
			DISPCHECK(" Wait for DSI engine not busy timeout!!!\n");
			DSI_DumpRegisters(module, NULL, 1);
			DSI_Reset(module, cmdq);
			break;
		}
	}
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_INTSTA, 0x0);

}

void DSI_lane0_ULP_mode(DISP_MODULE_ENUM module, void *cmdq, bool enter)
{
	int i = 0;

	ASSERT(cmdq == NULL);

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		if (enter) {
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG, DSI_REG[i]->DSI_PHY_LD0CON,
				      L0_HS_TX_EN, 0);
			mdelay(1);
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG, DSI_REG[i]->DSI_PHY_LD0CON,
				      L0_ULPM_EN, 0);
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG, DSI_REG[i]->DSI_PHY_LD0CON,
				      L0_ULPM_EN, 1);
			mdelay(1);
		} else {
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG, DSI_REG[i]->DSI_PHY_LD0CON,
				      L0_ULPM_EN, 0);
			mdelay(1);
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG, DSI_REG[i]->DSI_PHY_LD0CON,
				      L0_WAKEUP_EN, 1);
			mdelay(1);
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LD0CON_REG, DSI_REG[i]->DSI_PHY_LD0CON,
				      L0_WAKEUP_EN, 0);
			mdelay(1);
		}
	}
}


void DSI_clk_ULP_mode(DISP_MODULE_ENUM module, void *cmdq, bool enter)
{
	int i = 0;

	ASSERT(cmdq == NULL);

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		if (enter) {
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG, DSI_REG[i]->DSI_PHY_LCCON,
				      LC_ULPM_EN, 0);
			mdelay(1);
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG, DSI_REG[i]->DSI_PHY_LCCON,
				      LC_ULPM_EN, 1);
			mdelay(1);
		} else {
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG, DSI_REG[i]->DSI_PHY_LCCON,
				      LC_ULPM_EN, 0);
			mdelay(1);
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG, DSI_REG[i]->DSI_PHY_LCCON,
				      LC_WAKEUP_EN, 1);
			mdelay(1);
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG, DSI_REG[i]->DSI_PHY_LCCON,
				      LC_WAKEUP_EN, 0);
			mdelay(1);
		}
	}
}

bool DSI_clk_HS_state(DISP_MODULE_ENUM module, void *cmdq)
{
	struct DSI_PHY_LCCON_REG tmpreg;
	DSI_READREG32(PDSI_PHY_LCCON_REG, &tmpreg, &DSI_REG[0]->DSI_PHY_LCCON);
	return tmpreg.LC_HS_TX_EN ? TRUE : FALSE;
}

void DSI_clk_HS_mode(DISP_MODULE_ENUM module, void *cmdq, bool enter)
{
	int i = 0;

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		if (enter) {
			/* && !DSI_clk_HS_state(i, cmdq)) */
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG, DSI_REG[i]->DSI_PHY_LCCON,
				      LC_HS_TX_EN, 1);
		} else if (!enter) {
			DSI_OUTREGBIT(cmdq, struct DSI_PHY_LCCON_REG, DSI_REG[i]->DSI_PHY_LCCON,
				      LC_HS_TX_EN, 0);
		}
	}
}

const char *_dsi_cmd_mode_parse_state(unsigned int state)
{
	switch (state) {
	case 0x0001:
		return "idle";
	case 0x0002:
		return "Reading command queue for header";
	case 0x0004:
		return "Sending type-0 command";
	case 0x0008:
		return "Waiting frame data from RDMA for type-1 command";
	case 0x0010:
		return "Sending type-1 command";
	case 0x0020:
		return "Sending type-2 command";
	case 0x0040:
		return "Reading command queue for data";
	case 0x0080:
		return "Sending type-3 command";
	case 0x0100:
		return "Sending BTA";
	case 0x0200:
		return "Waiting RX-read data ";
	case 0x0400:
		return "Waiting SW RACK for RX-read data";
	case 0x0800:
		return "Waiting TE";
	case 0x1000:
		return "Get TE ";
	case 0x2000:
		return "Waiting external TE";
	case 0x4000:
		return "Waiting SW RACK for TE";
	default:
		return "unknown";
	}
}

DSI_STATUS DSI_DumpRegisters(DISP_MODULE_ENUM module, void *cmdq, int level)
{
	uint32_t i;
	/*
	   description of dsi status
	   Bit Value   Description
	   [0] 0x0001  Idle (wait for command)
	   [1] 0x0002  Reading command queue for header
	   [2] 0x0004  Sending type-0 command
	   [3] 0x0008  Waiting frame data from RDMA for type-1 command
	   [4] 0x0010  Sending type-1 command
	   [5] 0x0020  Sending type-2 command
	   [6] 0x0040  Reading command queue for data
	   [7] 0x0080  Sending type-3 command
	   [8] 0x0100  Sending BTA
	   [9] 0x0200  Waiting RX-read data
	   [10]    0x0400  Waiting SW RACK for RX-read data
	   [11]    0x0800  Waiting TE
	   [12]    0x1000  Get TE
	   [13]    0x2000  Waiting external TE
	   [14]    0x4000  Waiting SW RACK for TE

	 */
	static const char *const DSI_DBG_STATUS_DESCRIPTION[] = {
		"null",
		"Idle (wait for command)",
		"Reading command queue for header",
		"Sending type-0 command",
		"Waiting frame data from RDMA for type-1 command",
		"Sending type-1 command",
		"Sending type-2 command",
		"Reading command queue for data",
		"Sending type-3 command",
		"Sending BTA",
		"Waiting RX-read data ",
		"Waiting SW RACK for RX-read data",
		"Waiting TE",
		"Get TE ",
		"Waiting external TE",
		"Waiting SW RACK for TE",
	};
	unsigned int DSI_DBG6_Status = (DISP_REG_GET(DSI_REG[0] + 0x160)) & 0xffff;
	/* unsigned int DSI_DBG6_Status_bak = DSI_DBG6_Status; */
	int count = 0;

	while (DSI_DBG6_Status) {
		DSI_DBG6_Status >>= 1;
		count++;
	}
	/* while((1<<count) != DSI_DBG6_Status) count++; */
	/* count++; */
	DDPMSG("---------- Start dump DSI registers ----------\n");
	DDPMSG("DSI_STATE_DBG6=0x%08x, count=%d, means: [%s]\n",
		       DSI_DBG6_Status, count, DSI_DBG_STATUS_DESCRIPTION[count]);

	DDPMSG("---------- Start dump DSI registers ----------\n");

	for (i = 0; i < sizeof(struct DSI_REGS); i += 16) {
		DDPMSG("DSI+%04x : 0x%08x  0x%08x  0x%08x  0x%08x\n", i,
			       DISP_REG_GET(DSI_REG[0] + i), DISP_REG_GET(DSI_REG[0] + i + 0x4),
			       DISP_REG_GET(DSI_REG[0] + i + 0x8),
			       DISP_REG_GET(DSI_REG[0] + i + 0xc));
	}

	for (i = 0; i < sizeof(struct DSI_CMDQ_REGS); i += 16) {
		DDPMSG("DSI_CMD+%04x : 0x%08x  0x%08x  0x%08x  0x%08x\n", i,
			       DISP_REG_GET((DSI_REG[0] + 0x180 + i)),
			       DISP_REG_GET((DSI_REG[0] + 0x180 + i + 0x4)),
			       DISP_REG_GET((DSI_REG[0] + 0x180 + i + 0x8)),
			       DISP_REG_GET((DSI_REG[0] + 0x180 + i + 0xc)));
	}

	for (i = 0; i < sizeof(struct DSI_PHY_REGS); i += 16) {
		DDPMSG("DSI_PHY+%04x : 0x%08x    0x%08x  0x%08x  0x%08x\n", i,
			       DISP_REG_GET((DSI_PHY_REG[0] + i)),
			       DISP_REG_GET((DSI_PHY_REG[0] + i + 0x4)),
			       DISP_REG_GET((DSI_PHY_REG[0] + i + 0x8)),
			       DISP_REG_GET((DSI_PHY_REG[0] + i + 0xc)));
	}
	return DSI_STATUS_OK;
}

DSI_STATUS DSI_SleepOut(DISP_MODULE_ENUM module, void *cmdq)
{
    DSI_OUTREGBIT(cmdq, struct DSI_MODE_CTRL_REG,DSI_REG[0]->DSI_MODE_CTRL,SLEEP_MODE,1);
    DSI_OUTREGBIT(cmdq, struct DSI_PHY_TIMCON4_REG,DSI_REG[0]->DSI_PHY_TIMECON4,ULPS_WAKEUP,0x22E09);  // cycle to 1ms for 520MHz

    return DSI_STATUS_OK;
}


DSI_STATUS DSI_Wakeup(DISP_MODULE_ENUM module, void *cmdq)
{
    DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[0]->DSI_START,SLEEPOUT_START,0);
    DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[0]->DSI_START,SLEEPOUT_START,1);
    mdelay(1);

    DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG[0]->DSI_START,SLEEPOUT_START,0);
    DSI_OUTREGBIT(cmdq, struct DSI_MODE_CTRL_REG,DSI_REG[0]->DSI_MODE_CTRL,SLEEP_MODE,0);

    return DSI_STATUS_OK;
}

DSI_STATUS DSI_BackupRegisters(DISP_MODULE_ENUM module, void *cmdq)
{
	volatile struct DSI_REGS *regs = NULL;

	regs = &(_dsi_context[0].regBackup);

	/* memcpy((void*)&(_dsiContext.regBackup),
	   (void*)DSI_REG[0], sizeof(struct DSI_REGS)); */

	DSI_OUTREG32(cmdq, &regs->DSI_INTEN, AS_UINT32(&DSI_REG[0]->DSI_INTEN));
	DSI_OUTREG32(cmdq, &regs->DSI_MODE_CTRL, AS_UINT32(&DSI_REG[0]->DSI_MODE_CTRL));
	DSI_OUTREG32(cmdq, &regs->DSI_TXRX_CTRL, AS_UINT32(&DSI_REG[0]->DSI_TXRX_CTRL));
	DSI_OUTREG32(cmdq, &regs->DSI_PSCTRL, AS_UINT32(&DSI_REG[0]->DSI_PSCTRL));

	DSI_OUTREG32(cmdq, &regs->DSI_VSA_NL, AS_UINT32(&DSI_REG[0]->DSI_VSA_NL));
	DSI_OUTREG32(cmdq, &regs->DSI_VBP_NL, AS_UINT32(&DSI_REG[0]->DSI_VBP_NL));
	DSI_OUTREG32(cmdq, &regs->DSI_VFP_NL, AS_UINT32(&DSI_REG[0]->DSI_VFP_NL));
	DSI_OUTREG32(cmdq, &regs->DSI_VACT_NL, AS_UINT32(&DSI_REG[0]->DSI_VACT_NL));

	DSI_OUTREG32(cmdq, &regs->DSI_HSA_WC, AS_UINT32(&DSI_REG[0]->DSI_HSA_WC));
	DSI_OUTREG32(cmdq, &regs->DSI_HBP_WC, AS_UINT32(&DSI_REG[0]->DSI_HBP_WC));
	DSI_OUTREG32(cmdq, &regs->DSI_HFP_WC, AS_UINT32(&DSI_REG[0]->DSI_HFP_WC));
	DSI_OUTREG32(cmdq, &regs->DSI_BLLP_WC, AS_UINT32(&DSI_REG[0]->DSI_BLLP_WC));

	DSI_OUTREG32(cmdq, &regs->DSI_HSTX_CKL_WC, AS_UINT32(&DSI_REG[0]->DSI_HSTX_CKL_WC));
	DSI_OUTREG32(cmdq, &regs->DSI_MEM_CONTI, AS_UINT32(&DSI_REG[0]->DSI_MEM_CONTI));

	DSI_OUTREG32(cmdq, &regs->DSI_PHY_TIMECON0, AS_UINT32(&DSI_REG[0]->DSI_PHY_TIMECON0));
	DSI_OUTREG32(cmdq, &regs->DSI_PHY_TIMECON1, AS_UINT32(&DSI_REG[0]->DSI_PHY_TIMECON1));
	DSI_OUTREG32(cmdq, &regs->DSI_PHY_TIMECON2, AS_UINT32(&DSI_REG[0]->DSI_PHY_TIMECON2));
	DSI_OUTREG32(cmdq, &regs->DSI_PHY_TIMECON3, AS_UINT32(&DSI_REG[0]->DSI_PHY_TIMECON3));
	DSI_OUTREG32(cmdq, &regs->DSI_VM_CMD_CON, AS_UINT32(&DSI_REG[0]->DSI_VM_CMD_CON));
	return DSI_STATUS_OK;
}

DSI_STATUS DSI_RestoreRegisters(DISP_MODULE_ENUM module, void *cmdq)
{
	volatile struct DSI_REGS *regs = NULL;

	regs = &(_dsi_context[0].regBackup);

	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_INTEN, AS_UINT32(&regs->DSI_INTEN));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_MODE_CTRL, AS_UINT32(&regs->DSI_MODE_CTRL));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_TXRX_CTRL, AS_UINT32(&regs->DSI_TXRX_CTRL));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_PSCTRL, AS_UINT32(&regs->DSI_PSCTRL));

	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_VSA_NL, AS_UINT32(&regs->DSI_VSA_NL));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_VBP_NL, AS_UINT32(&regs->DSI_VBP_NL));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_VFP_NL, AS_UINT32(&regs->DSI_VFP_NL));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_VACT_NL, AS_UINT32(&regs->DSI_VACT_NL));

	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_HSA_WC, AS_UINT32(&regs->DSI_HSA_WC));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_HBP_WC, AS_UINT32(&regs->DSI_HBP_WC));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_HFP_WC, AS_UINT32(&regs->DSI_HFP_WC));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_BLLP_WC, AS_UINT32(&regs->DSI_BLLP_WC));

	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_HSTX_CKL_WC, AS_UINT32(&regs->DSI_HSTX_CKL_WC));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_MEM_CONTI, AS_UINT32(&regs->DSI_MEM_CONTI));

	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_PHY_TIMECON0, AS_UINT32(&regs->DSI_PHY_TIMECON0));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_PHY_TIMECON1, AS_UINT32(&regs->DSI_PHY_TIMECON1));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_PHY_TIMECON2, AS_UINT32(&regs->DSI_PHY_TIMECON2));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_PHY_TIMECON3, AS_UINT32(&regs->DSI_PHY_TIMECON3));
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_VM_CMD_CON, AS_UINT32(&regs->DSI_VM_CMD_CON));
	return DSI_STATUS_OK;
}


void DSI_PHY_clk_switch(DISP_MODULE_ENUM module, void *cmdq, int on)
{
	int i = 0;

	if (on) {
		DSI_PHY_clk_setting(module, cmdq, &(_dsi_context[i].dsi_params));
	} else {
		for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
			/* pre_oe/oe = 1 */
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON0,
				  SW_LNTC_LPTX_PRE_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON0,
				  SW_LNTC_LPTX_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON1,
				  SW_LNT0_LPTX_PRE_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON1,
				  SW_LNT0_LPTX_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON1,
				  SW_LNT1_LPTX_PRE_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON1,
				  SW_LNT1_LPTX_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON1,
				  SW_LNT2_LPTX_PRE_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON1,
				  SW_LNT2_LPTX_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON1,
				  SW_LNT3_LPTX_PRE_OE, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL_CON1,
				  SW_LNT3_LPTX_OE, 1);

			/* switch to mipi tx sw mode */
			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_REG, DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL, SW_CTRL_EN, 1);

			/* disable mipi clock */
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0,
				  RG_DSI0_MPPLL_PLL_EN, 0);
			mdelay(1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_TOP_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_TOP,
				  RG_MPPLL_PRESERVE_L, 0);

			MIPITX_OUTREGBIT(struct MIPITX_DSI_TOP_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_TOP_CON,
				  RG_DSI_PAD_TIE_LOW_EN, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_CLOCK_LANE_REG, DSI_PHY_REG[i]->MIPITX_DSI0_CLOCK_LANE,
				  RG_DSI0_LNTC_LDOOUT_EN, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE0_REG, DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE0,
				  RG_DSI0_LNT0_LDOOUT_EN, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE1_REG, DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE1,
				  RG_DSI0_LNT1_LDOOUT_EN, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE2_REG, DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE2,
				  RG_DSI0_LNT2_LDOOUT_EN, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE3_REG, DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE3,
				  RG_DSI0_LNT3_LDOOUT_EN, 0);
			mdelay(1);

			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_PWR_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_PWR,
				  DA_DSI0_MPPLL_SDM_ISO_EN, 1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_PWR_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_PWR,
				  DA_DSI0_MPPLL_SDM_PWR_ON, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_TOP_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_TOP_CON,
				  RG_DSI_LNT_HS_BIAS_EN, 0);

			MIPITX_OUTREGBIT(struct MIPITX_DSI_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_TOP_CON,
				RG_DSI0_CKG_LDOOUT_EN, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_TOP_CON,
				RG_DSI0_LDOCORE_EN, 0);

			MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON, RG_DSI_BG_CKEN, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON, RG_DSI_BG_CORE_EN,
				  0);

			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0,
				  RG_DSI0_MPPLL_PREDIV, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0,
				  RG_DSI0_MPPLL_TXDIV0, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0,
				  RG_DSI0_MPPLL_TXDIV1, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0,
				  RG_DSI0_MPPLL_POSDIV, 0);


			MIPITX_OUTREG32(&DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON1, 0x00000000);
			MIPITX_OUTREG32(&DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2, 0x50000000);

			MIPITX_OUTREGBIT(struct MIPITX_DSI_SW_CTRL_REG,
				DSI_PHY_REG[i]->MIPITX_DSI_SW_CTRL, SW_CTRL_EN, 0);
			mdelay(1);
		}
	}
}

DSI_STATUS DSI_BIST_Pattern_Test(DISP_MODULE_ENUM module, void *cmdq, bool enable,
				 unsigned int color)
{
	int i = 0;

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		if (enable) {
			DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_BIST_PATTERN, color);
			/* DSI_OUTREG32(&DSI_REG->DSI_BIST_CON, AS_UINT32(&temp_reg)); */
			/* DSI_OUTREGBIT(DSI_BIST_CON_REG, DSI_REG->DSI_BIST_CON, SELF_PAT_MODE, 1); */
			DSI_OUTREGBIT(cmdq, struct DSI_BIST_CON_REG, DSI_REG[i]->DSI_BIST_CON,
				      SELF_PAT_MODE, 1);

			if (!_dsi_is_video_mode(module)) {
				DSI_T0_INS t0;

				t0.CONFG = 0x09;
				t0.Data_ID = 0x39;
				t0.Data0 = 0x2c;
				t0.Data1 = 0;

				DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[i]->data[0], AS_UINT32(&t0));
				DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_CMDQ_SIZE, 1);

				/* DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG->DSI_START,DSI_START,0); */
				DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_START, 0);
				DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_START, 1);
				/* DSI_OUTREGBIT(cmdq, struct DSI_START_REG,DSI_REG->DSI_START,DSI_START,1); */
			}
		} else {
			/* if disable dsi pattern, need enable mutex, can't just start dsi */
			/* so we just disable pattern bit, do not start dsi here */
			/* DSI_WaitForNotBusy(module,cmdq); */
			/* DSI_OUTREGBIT(cmdq, DSI_BIST_CON_REG, DSI_REG[i]->DSI_BIST_CON, SELF_PAT_MODE, 0); */
			DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_BIST_CON, 0x00);
		}

	}
	return DSI_STATUS_OK;
}

void DSI_Config_VDO_Timing(DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
	int i = 0;
	unsigned int line_byte;
	unsigned int horizontal_sync_active_byte;
	unsigned int horizontal_backporch_byte;
	unsigned int horizontal_frontporch_byte;
	unsigned int horizontal_bllp_byte;
	unsigned int dsiTmpBufBpp;

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		if (dsi_params->data_format.format == LCM_DSI_FORMAT_RGB565) {
			dsiTmpBufBpp = 2;
		} else {
			dsiTmpBufBpp = 3;
		}

		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_VSA_NL, dsi_params->vertical_sync_active);
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_VBP_NL, dsi_params->vertical_backporch);
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_VFP_NL, dsi_params->vertical_frontporch);
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_VACT_NL, dsi_params->vertical_active_line);

		line_byte =
		    (dsi_params->horizontal_sync_active + dsi_params->horizontal_backporch +
		     dsi_params->horizontal_frontporch +
		     dsi_params->horizontal_active_pixel) * dsiTmpBufBpp;
		horizontal_sync_active_byte =
		    (dsi_params->horizontal_sync_active * dsiTmpBufBpp - 4);

		if (dsi_params->mode == SYNC_EVENT_VDO_MODE || dsi_params->mode == BURST_VDO_MODE
		    || dsi_params->switch_mode == SYNC_EVENT_VDO_MODE
		    || dsi_params->switch_mode == BURST_VDO_MODE) {
			ASSERT((dsi_params->horizontal_backporch +
				dsi_params->horizontal_sync_active) * dsiTmpBufBpp > 9);
			horizontal_backporch_byte =
			    ((dsi_params->horizontal_backporch +
			      dsi_params->horizontal_sync_active) * dsiTmpBufBpp - 10);
		} else {
			ASSERT(dsi_params->horizontal_sync_active * dsiTmpBufBpp > 9);
			horizontal_sync_active_byte =
			    (dsi_params->horizontal_sync_active * dsiTmpBufBpp - 10);

			ASSERT(dsi_params->horizontal_backporch * dsiTmpBufBpp > 9);
			horizontal_backporch_byte =
			    (dsi_params->horizontal_backporch * dsiTmpBufBpp - 10);
		}

		ASSERT(dsi_params->horizontal_frontporch * dsiTmpBufBpp > 11);
		horizontal_frontporch_byte =
		    (dsi_params->horizontal_frontporch * dsiTmpBufBpp - 12);
		horizontal_bllp_byte = (dsi_params->horizontal_bllp * dsiTmpBufBpp);

		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HSA_WC,
			     ALIGN_TO((horizontal_sync_active_byte), 4));
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HBP_WC,
			     ALIGN_TO((horizontal_backporch_byte), 4));
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HFP_WC,
			     ALIGN_TO((horizontal_frontporch_byte), 4));
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_BLLP_WC, ALIGN_TO((horizontal_bllp_byte), 4));
	}
}

void DSI_PHY_CLK_LP_PerLine_config(DISP_MODULE_ENUM module, cmdqRecHandle cmdq,
				   LCM_DSI_PARAMS *dsi_params)
{
	int i;
	struct DSI_PHY_TIMCON0_REG timcon0 = { 0 }; /* LPX */
	struct DSI_PHY_TIMCON2_REG timcon2 = { 0 }; /* CLK_HS_TRAIL, CLK_HS_ZERO */
	struct DSI_PHY_TIMCON3_REG timcon3 = { 0 }; /* CLK_HS_EXIT, CLK_HS_POST, CLK_HS_PREP */
	struct DSI_HSA_WC_REG hsa = { 0 };
	struct DSI_HBP_WC_REG hbp = { 0 };
	struct DSI_HFP_WC_REG hfp = { 0 }, new_hfp = { 0 };
	struct DSI_BLLP_WC_REG bllp = { 0 };
	struct DSI_PSCTRL_REG ps = { 0 };
	uint32_t hstx_ckl_wc = 0, new_hstx_ckl_wc = 0;
	uint32_t v_a, v_b, v_c, lane_num;
	LCM_DSI_MODE_CON dsi_mode;

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		lane_num = dsi_params->LANE_NUM;
		dsi_mode = dsi_params->mode;

		if (dsi_mode == CMD_MODE)
			continue;

		/* vdo mode */
		DSI_OUTREG32(cmdq, &hsa, AS_UINT32(&DSI_REG[i]->DSI_HSA_WC));
		DSI_OUTREG32(cmdq, &hbp, AS_UINT32(&DSI_REG[i]->DSI_HBP_WC));
		DSI_OUTREG32(cmdq, &hfp, AS_UINT32(&DSI_REG[i]->DSI_HFP_WC));
		DSI_OUTREG32(cmdq, &bllp, AS_UINT32(&DSI_REG[i]->DSI_BLLP_WC));
		DSI_OUTREG32(cmdq, &ps, AS_UINT32(&DSI_REG[i]->DSI_PSCTRL));
		DSI_OUTREG32(cmdq, &hstx_ckl_wc, AS_UINT32(&DSI_REG[i]->DSI_HSTX_CKL_WC));
		DSI_OUTREG32(cmdq, &timcon0, AS_UINT32(&DSI_REG[i]->DSI_PHY_TIMECON0));
		DSI_OUTREG32(cmdq, &timcon2, AS_UINT32(&DSI_REG[i]->DSI_PHY_TIMECON2));
		DSI_OUTREG32(cmdq, &timcon3, AS_UINT32(&DSI_REG[i]->DSI_PHY_TIMECON3));

		/* 1. sync_pulse_mode */
		/* Total	WC(A) = HSA_WC + HBP_WC + HFP_WC + PS_WC + 32 */
		/* CLK init WC(B) = (CLK_HS_EXIT + LPX + CLK_HS_PREP + CLK_HS_ZERO)*lane_num */
		/* CLK end	WC(C) = (CLK_HS_POST + CLK_HS_TRAIL)*lane_num */
		/* HSTX_CKLP_WC = A - B */
		/* Limitation: B + C < HFP_WC */
		if (dsi_mode == SYNC_PULSE_VDO_MODE) {
			v_a = hsa.HSA_WC + hbp.HBP_WC + hfp.HFP_WC + ps.DSI_PS_WC + 32;
			v_b =
				(timcon3.CLK_HS_EXIT + timcon0.LPX + timcon3.CLK_HS_PRPR +
				 timcon2.CLK_ZERO) * lane_num;
			v_c = (timcon3.CLK_HS_POST + timcon2.CLK_TRAIL) * lane_num;

			DISPDBG("===>v_a-v_b=0x%x,HSTX_CKLP_WC=0x%x\n", (v_a - v_b), hstx_ckl_wc);
/* DISPCHECK("===>v_b+v_c=0x%x,HFP_WC=0x%x\n",(v_b+v_c),hfp); */
			DISPDBG("===>Will Reconfig in order to fulfill LP clock lane per line\n");

			DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HFP_WC, (v_b + v_c + DIFF_CLK_LANE_LP));
			DSI_OUTREG32(cmdq, &new_hfp, AS_UINT32(&DSI_REG[i]->DSI_HFP_WC));
			v_a = hsa.HSA_WC + hbp.HBP_WC + new_hfp.HFP_WC + ps.DSI_PS_WC + 32;
			DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HSTX_CKL_WC, (v_a - v_b));
			DSI_OUTREG32(cmdq, &new_hstx_ckl_wc,
					 AS_UINT32(&DSI_REG[i]->DSI_HSTX_CKL_WC));
			DISPDBG("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc,
				  new_hfp.HFP_WC);
		}
		/* 2. sync_event_mode */
		/* Total	WC(A) = HBP_WC + HFP_WC + PS_WC + 26 */
		/* CLK init WC(B) = (CLK_HS_EXIT + LPX + CLK_HS_PREP + CLK_HS_ZERO)*lane_num */
		/* CLK end	WC(C) = (CLK_HS_POST + CLK_HS_TRAIL)*lane_num */
		/* HSTX_CKLP_WC = A - B */
		/* Limitation: B + C < HFP_WC */
		else if (dsi_mode == SYNC_EVENT_VDO_MODE) {
			v_a = hbp.HBP_WC + hfp.HFP_WC + ps.DSI_PS_WC + 26;
			v_b =
				(timcon3.CLK_HS_EXIT + timcon0.LPX + timcon3.CLK_HS_PRPR +
				 timcon2.CLK_ZERO) * lane_num;
			v_c = (timcon3.CLK_HS_POST + timcon2.CLK_TRAIL) * lane_num;

			DISPDBG("===>v_a-v_b=0x%x,HSTX_CKLP_WC=0x%x\n", (v_a - v_b), hstx_ckl_wc);
/* DISPCHECK("===>v_b+v_c=0x%x,HFP_WC=0x%x\n",(v_b+v_c),hfp); */
			DISPDBG("===>Will Reconfig in order to fulfill LP clock lane per line\n");

			DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HFP_WC, (v_b + v_c + DIFF_CLK_LANE_LP));
			DSI_OUTREG32(cmdq, &new_hfp, AS_UINT32(&DSI_REG[i]->DSI_HFP_WC));
			v_a = hbp.HBP_WC + new_hfp.HFP_WC + ps.DSI_PS_WC + 26;
			DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HSTX_CKL_WC, (v_a - v_b));
			DSI_OUTREG32(cmdq, &new_hstx_ckl_wc,
					 AS_UINT32(&DSI_REG[i]->DSI_HSTX_CKL_WC));
			DISPDBG("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc,
				  new_hfp.HFP_WC);

		}
		/* 3. burst_mode */
		/* Total	WC(A) = HBP_WC + HFP_WC + PS_WC + BLLP_WC + 32 */
		/* CLK init WC(B) = (CLK_HS_EXIT + LPX + CLK_HS_PREP + CLK_HS_ZERO)*lane_num */
		/* CLK end	WC(C) = (CLK_HS_POST + CLK_HS_TRAIL)*lane_num */
		/* HSTX_CKLP_WC = A - B */
		/* Limitation: B + C < HFP_WC */
		else if (dsi_mode == BURST_VDO_MODE) {
			v_a = hbp.HBP_WC + hfp.HFP_WC + ps.DSI_PS_WC + bllp.BLLP_WC + 32;
			v_b =
				(timcon3.CLK_HS_EXIT + timcon0.LPX + timcon3.CLK_HS_PRPR +
				 timcon2.CLK_ZERO) * lane_num;
			v_c = (timcon3.CLK_HS_POST + timcon2.CLK_TRAIL) * lane_num;

			DISPDBG("===>v_a-v_b=0x%x,HSTX_CKLP_WC=0x%x\n", (v_a - v_b), hstx_ckl_wc);
			/* DISPCHECK("===>v_b+v_c=0x%x,HFP_WC=0x%x\n",(v_b+v_c),hfp); */
			DISPDBG("===>Will Reconfig in order to fulfill LP clock lane per line\n");

			DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HFP_WC, (v_b + v_c + DIFF_CLK_LANE_LP));
			DSI_OUTREG32(cmdq, &new_hfp, AS_UINT32(&DSI_REG[i]->DSI_HFP_WC));
			v_a = hbp.HBP_WC + new_hfp.HFP_WC + ps.DSI_PS_WC + bllp.BLLP_WC + 32;
			DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_HSTX_CKL_WC, (v_a - v_b));
			DSI_OUTREG32(cmdq, &new_hstx_ckl_wc,
					 AS_UINT32(&DSI_REG[i]->DSI_HSTX_CKL_WC));
			DISPDBG("===>new HSTX_CKL_WC=0x%x, HFP_WC=0x%x\n", new_hstx_ckl_wc,
				  new_hfp.HFP_WC);
		}
	}
}


int _dsi_ps_type_to_bpp(LCM_PS_TYPE ps)
{
	switch (ps) {
	case LCM_PACKED_PS_16BIT_RGB565:
		return 2;
	case LCM_LOOSELY_PS_18BIT_RGB666:
		return 3;
	case LCM_PACKED_PS_24BIT_RGB888:
		return 3;
	case LCM_PACKED_PS_18BIT_RGB666:
		return 3;
	}
}

DSI_STATUS DSI_PS_Control(DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params, int w,
			  int h)
{
 	int i = 0;
	unsigned int ps_sel_bitvalue = 0;
	/* /TODO: parameter checking */
	ASSERT(dsi_params->PS <= PACKED_PS_18BIT_RGB666);

	if (dsi_params->PS > LOOSELY_PS_18BIT_RGB666) {
		ps_sel_bitvalue = (5 - dsi_params->PS);
	} else {
		ps_sel_bitvalue = dsi_params->PS;
	}
#if 0
	if (module == DISP_MODULE_DSIDUAL) {
		w = w / 2;
	}
#endif
	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		DSI_OUTREGBIT(cmdq, struct DSI_VACT_NL_REG, DSI_REG[i]->DSI_VACT_NL, VACT_NL, h);
		if (dsi_params->ufoe_enable && dsi_params->ufoe_params.lr_mode_en != 1) {
			if (dsi_params->ufoe_params.compress_ratio == 3) {	/* 1/3 */
				unsigned int ufoe_internal_width = w + w % 4;
				if (ufoe_internal_width % 3 == 0) {
					DSI_OUTREGBIT(cmdq, struct DSI_PSCTRL_REG, DSI_REG[i]->DSI_PSCTRL,
						      DSI_PS_WC,
						      (ufoe_internal_width / 3) *
						      _dsi_ps_type_to_bpp(dsi_params->PS));
				} else {
					unsigned int temp_w = ufoe_internal_width / 3 + 1;
					temp_w = ((temp_w % 2) == 1) ? (temp_w + 1) : temp_w;
					DSI_OUTREGBIT(cmdq, struct DSI_PSCTRL_REG, DSI_REG[i]->DSI_PSCTRL,
						      DSI_PS_WC,
						      temp_w * _dsi_ps_type_to_bpp(dsi_params->PS));
				}
			} else	/* 1/2 */
				DSI_OUTREGBIT(cmdq, struct DSI_PSCTRL_REG, DSI_REG[i]->DSI_PSCTRL,
					      DSI_PS_WC,
					      (w +
					       w % 4) / 2 * _dsi_ps_type_to_bpp(dsi_params->PS));
		} else {
			DSI_OUTREGBIT(cmdq, struct DSI_PSCTRL_REG, DSI_REG[i]->DSI_PSCTRL, DSI_PS_WC,
				      w * _dsi_ps_type_to_bpp(dsi_params->PS));
		}


		DSI_OUTREGBIT(cmdq, struct DSI_PSCTRL_REG, DSI_REG[i]->DSI_PSCTRL, DSI_PS_SEL,
			      ps_sel_bitvalue);
	}
	return DSI_STATUS_OK;
}

DSI_STATUS DSI_TXRX_Control(DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
	int i = 0;
	unsigned int lane_num_bitvalue = 0;
	/*bool cksm_en = true; */
	/*bool ecc_en = true; */
	int lane_num = dsi_params->LANE_NUM;
	int vc_num = 0;
	bool null_packet_en = dsi_params->null_packet_en;
	/*bool err_correction_en = false; */
	bool dis_eotp_en = false;
	bool hstx_cklp_en = dsi_params->noncont_clock;
	int max_return_size = 0;

	switch (lane_num) {
	case LCM_ONE_LANE:
		lane_num_bitvalue = 0x1;
		break;
	case LCM_TWO_LANE:
		lane_num_bitvalue = 0x3;
		break;
	case LCM_THREE_LANE:
		lane_num_bitvalue = 0x7;
		break;
	case LCM_FOUR_LANE:
		lane_num_bitvalue = 0xF;
		break;
	}

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL, VC_NUM, vc_num);
		DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL, DIS_EOT,
			      dis_eotp_en);
		DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL, NULL_EN,
			      null_packet_en);
		DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL, MAX_RTN_SIZE,
			      max_return_size);
		DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL, HSTX_CKLP_EN,
			      hstx_cklp_en);
		DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL, LANE_NUM,
			      lane_num_bitvalue);
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_MEM_CONTI, DSI_WMEM_CONTI);
		if (CMD_MODE == dsi_params->mode) {
			if (dsi_params->ext_te_edge == LCM_POLARITY_FALLING) {
				/*use ext te falling edge */
				DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL,
					      EXT_TE_EDGE, 1);
			}
			DSI_OUTREGBIT(cmdq, struct DSI_TXRX_CTRL_REG, DSI_REG[i]->DSI_TXRX_CTRL, EXT_TE_EN,
				      1);
		}
	}
	return DSI_STATUS_OK;
}
#define CONFIG_MTK_FPGA 1

void DSI_PHY_clk_setting(DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
#ifdef MACH_FPGA
#if 1
	MIPITX_OUTREG32(0x14000108, 0xFFFFFFFF);
	MIPITX_OUTREG32(0x14000118, 0xFFFFFFFF);

	DISPMSG("CG_CON0=0x%x CLR0=0x%x\n",DISP_REG_GET(0x14000100),DISP_REG_GET(0x14000108));
	DISPMSG("CG_CON1=0x%x CLR1=0x%x\n",DISP_REG_GET(0x14000110),DISP_REG_GET(0x14000118));

	MIPITX_Write60384(0x18,0x00,0x10);
	MIPITX_Write60384(0x20,0x42,0x01);
	MIPITX_Write60384(0x20,0x43,0x01);
	MIPITX_Write60384(0x20,0x05,0x01);
	MIPITX_Write60384(0x20,0x22,0x01);
	MIPITX_Write60384(0x30,0x44,0x83);
	MIPITX_Write60384(0x30,0x40,0x82);
	MIPITX_Write60384(0x30,0x00,0x03);
	MIPITX_Write60384(0x30,0x68,0x03);
	MIPITX_Write60384(0x30,0x68,0x01);
	MIPITX_Write60384(0x30,0x50,0x80);
	MIPITX_Write60384(0x30,0x51,0x01);
	MIPITX_Write60384(0x30,0x54,0x01);
	MIPITX_Write60384(0x30,0x58,0x00);
	MIPITX_Write60384(0x30,0x59,0x00);
	MIPITX_Write60384(0x30,0x5a,0x00);
	//MIPITX_Write60384(0x30,0x5b,(dsi_params->fbk_div)<< 2);
	MIPITX_Write60384(0x30,0x5b,0x24);
	MIPITX_Write60384(0x30,0x04,0x11);
	MIPITX_Write60384(0x30,0x08,0x01);
	MIPITX_Write60384(0x30,0x0C,0x01);
	MIPITX_Write60384(0x30,0x10,0x01);
	MIPITX_Write60384(0x30,0x14,0x01);
	MIPITX_Write60384(0x30,0x64,0x20);
	MIPITX_Write60384(0x30,0x50,0x81);
	MIPITX_Write60384(0x30,0x28,0x00);

	//added 20160422; 8163 does not have it
	MIPITX_Write60384(0x18, 0x27, 0x70);
	mdelay(500);
	DDPMSG("PLL setting finish!!\n");

	DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_LCM_RST_B, 0);
	DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_LCM_RST_B, 1);
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_COM_CTRL, 0x1);
	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_COM_CTRL, 0x0);
#endif
#else
#if 0
	MIPITX_OUTREG32(0x10215044, 0x88492483);
	MIPITX_OUTREG32(0x10215040, 0x00000002);
	mdelay(10);
	MIPITX_OUTREG32(0x10215000, 0x00000403);
	MIPITX_OUTREG32(0x10215068, 0x00000003);
	MIPITX_OUTREG32(0x10215068, 0x00000001);

	mdelay(10);
	MIPITX_OUTREG32(0x10215050, 0x00000000);
	mdelay(10);
	MIPITX_OUTREG32(0x10215054, 0x00000003);
	MIPITX_OUTREG32(0x10215058, 0x60000000);
	MIPITX_OUTREG32(0x1021505c, 0x00000000);

	MIPITX_OUTREG32(0x10215004, 0x00000803);
	MIPITX_OUTREG32(0x10215008, 0x00000801);
	MIPITX_OUTREG32(0x1021500c, 0x00000801);
	MIPITX_OUTREG32(0x10215010, 0x00000801);
	MIPITX_OUTREG32(0x10215014, 0x00000801);

	MIPITX_OUTREG32(0x10215050, 0x00000001);

	mdelay(10);


	MIPITX_OUTREG32(0x10215064, 0x00000020);
	return 0;
#endif

	int i = 0;
	unsigned int data_Rate = dsi_params->PLL_CLOCK * 2;
	unsigned int txdiv = 0;
	unsigned int txdiv0 = 0;
	unsigned int txdiv1 = 0;
	unsigned int pcw = 0;
	/* unsigned int fmod = 30; */	/*Fmod = 30KHz by default */
	unsigned int delta1 = 5;	/* Delta1 is SSC range, default is 0%~-5% */
	unsigned int pdelta1 = 0;

	/* temp1~5 is used for impedence calibration, not enable now */
#if 0
	u32 m_hw_res3 = 0;
	u32 temp1 = 0;
	u32 temp2 = 0;
	u32 temp3 = 0;
	u32 temp4 = 0;
	u32 temp5 = 0;
	u32 lnt = 0;

	/* temp1~5 is used for impedence calibration, not enable now */
	m_hw_res3 = INREG32(0xF0206180);
	temp1 = (m_hw_res3 >> 28) & 0xF;
	temp2 = (m_hw_res3 >> 24) & 0xF;
	temp3 = (m_hw_res3 >> 20) & 0xF;
	temp4 = (m_hw_res3 >> 16) & 0xF;
	temp5 = (m_hw_res3 >> 12) & 0xF;
#endif

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		/* step 4 */
		MIPITX_OUTREGBIT(struct MIPITX_DSI_TOP_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_TOP_CON,
				 RG_DSI_LNT_IMP_CAL_CODE, 8);

		MIPITX_OUTREGBIT(struct MIPITX_DSI_TOP_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_TOP_CON,
				 RG_DSI_LNT_HS_BIAS_EN, 1);

		/* step 2 */
		MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON,
				 RG_DSI_V032_SEL, 4);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON,
				 RG_DSI_V04_SEL, 4);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON,
				 RG_DSI_V072_SEL, 4);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON,
				 RG_DSI_V10_SEL, 4);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON,
				 RG_DSI_V12_SEL, 4);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON,
				 RG_DSI_BG_CORE_EN, 1);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_BG_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_BG_CON,
				 RG_DSI_BG_CKEN, 1);
		/* step 3 */
		mdelay(10);

		/* step 5 */
		MIPITX_OUTREGBIT(struct MIPITX_DSI_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI0_CON,
				 RG_DSI0_CKG_LDOOUT_EN, 1);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI0_CON,
				 RG_DSI0_LDOCORE_EN, 1);
		/* step 6 */
		MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_PWR_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_PWR,
				 DA_DSI0_MPPLL_SDM_PWR_ON, 1);

		/* step 7 */
		MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_PWR_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_PWR,
				 DA_DSI0_MPPLL_SDM_ISO_EN, 1);
		mdelay(10);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_PWR_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_PWR,
				 DA_DSI0_MPPLL_SDM_ISO_EN, 0);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG,
			DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0, RG_DSI0_MPPLL_PREDIV, 0);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG,
			DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0, RG_DSI0_MPPLL_POSDIV, 0);

		if (0 != data_Rate) {
			if (data_Rate > 1250) {
				DISPCHECK("mipitx Data Rate exceed limitation(%d)\n", data_Rate);
				ASSERT(0);
			} else if (data_Rate >= 500) {
				txdiv = 1;
				txdiv0 = 0;
				txdiv1 = 0;
			} else if (data_Rate >= 250) {
				txdiv = 2;
				txdiv0 = 1;
				txdiv1 = 0;
			} else if (data_Rate >= 125) {
				txdiv = 4;
				txdiv0 = 2;
				txdiv1 = 0;
			}else if (data_Rate > 62) {
				txdiv = 8;
				txdiv0 = 2;
				txdiv1 = 1;
			} else if (data_Rate >= 50) {
				txdiv = 16;
				txdiv0 = 2;
				txdiv1 = 2;
			}  else {
				DISPCHECK("dataRate is too low(%d)\n", data_Rate);
				ASSERT(0);
			}

			/* step 8 */
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0, RG_DSI0_MPPLL_TXDIV0,
					 txdiv0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0, RG_DSI0_MPPLL_TXDIV1,
					 txdiv1);

			/* step 10 */
			/* PLL PCW config */
			/*
			   PCW bit 24~30 = floor(pcw)
			   PCW bit 16~23 = (pcw - floor(pcw))*256
			   PCW bit 8~15 = (pcw*256 - floor(pcw)*256)*256
			   PCW bit 8~15 = (pcw*256*256 - floor(pcw)*256*256)*256
			 */
			/* pcw = data_Rate*4*txdiv/(26*2);//Post DIV =4, so need data_Rate*4 */
			pcw = data_Rate * txdiv / 13;

			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON2_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2,
					 RG_DSI0_MPPLL_SDM_PCW_H, (pcw & 0x7F));
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON2_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2,
					 RG_DSI0_MPPLL_SDM_PCW_16_23,
					 ((256 * (data_Rate * txdiv % 13) / 13) & 0xFF));
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON2_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2,
					 RG_DSI0_MPPLL_SDM_PCW_8_15,
					 ((256 * (256 * (data_Rate * txdiv % 13) % 13) /
					   13) & 0xFF));
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON2_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2,
					 RG_DSI0_MPPLL_SDM_PCW_0_7,
					 ((256 *
					   (256 * (256 * (data_Rate * txdiv % 13) % 13) % 13) /
					   13) & 0xFF));

			if (1 != dsi_params->ssc_disable) {
				MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON1_REG,
						 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON1,
						 RG_DSI0_MPPLL_SDM_SSC_PH_INIT, 1);
				MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON1_REG,
						 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON1,
						 RG_DSI0_MPPLL_SDM_SSC_PRD, 0x1B1);
				if (0 != dsi_params->ssc_range)
					delta1 = dsi_params->ssc_range;

				ASSERT(delta1 <= 8);
				pdelta1 = (delta1 * data_Rate * txdiv * 262144 + 281664) / 563329;
				MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON3_REG,
						 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON3,
						 RG_DSI0_MPPLL_SDM_SSC_DELTA, pdelta1);
				MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON3_REG,
						 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON3,
						 RG_DSI0_MPPLL_SDM_SSC_DELTA1, pdelta1);
				/* DSI_OUTREGBIT(struct MIPITX_DSI_PLL_CON1_REG,DSI_PHY_REG->MIPITX_DSI_PLL_CON1,
					RG_DSI0_MPPLL_SDM_FRA_EN,1); */
				DISPMSG
					("[dsi_drv.c] PLL config:data_rate=%d,txdiv=%d,pcw=%d,delta1=%d,pdelta1=0x%x\n",
						data_Rate, txdiv, DSI_INREG32(PMIPITX_DSI_PLL_CON2_REG,
						&DSI_PHY_REG[i]-> MIPITX_DSI_PLL_CON2), delta1, pdelta1);
			}
		} else {
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0,
				  RG_DSI0_MPPLL_TXDIV0, dsi_params->pll_div1);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0,
				  RG_DSI0_MPPLL_TXDIV1, dsi_params->pll_div2);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON2_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2,
				  RG_DSI0_MPPLL_SDM_PCW_H, ((dsi_params->fbk_div) << 2));
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON2_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2,
				  RG_DSI0_MPPLL_SDM_PCW_16_23, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON2_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2,
				  RG_DSI0_MPPLL_SDM_PCW_8_15, 0);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON2_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON2,
				  RG_DSI0_MPPLL_SDM_PCW_0_7, 0);
			DISPERR("[dsi_dsi.c] PLL clock should not be 0!!!\n");
			ASSERT(0);
		}

		MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON1_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON1,
				 RG_DSI0_MPPLL_SDM_FRA_EN, 1);

		/* step 11 */
		MIPITX_OUTREGBIT(struct MIPITX_DSI_CLOCK_LANE_REG, DSI_PHY_REG[i]->MIPITX_DSI0_CLOCK_LANE,
			RG_DSI0_LNTC_RT_CODE, 0x8);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_CLOCK_LANE_REG, DSI_PHY_REG[i]->MIPITX_DSI0_CLOCK_LANE,
			RG_DSI0_LNTC_PHI_SEL, 0x1);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_CLOCK_LANE_REG, DSI_PHY_REG[i]->MIPITX_DSI0_CLOCK_LANE,
			RG_DSI0_LNTC_LDOOUT_EN, 1);

		/* step 12 */
		if (dsi_params->LANE_NUM > 0) {
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE0_REG,
				DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE0,
				RG_DSI0_LNT0_RT_CODE, 0x8);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE0_REG,
				DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE0,
				RG_DSI0_LNT0_LDOOUT_EN, 1);
		}

		/* step 13 */
		if (dsi_params->LANE_NUM > 1) {
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE1_REG,
				DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE1,
				RG_DSI0_LNT1_RT_CODE, 0x8);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE1_REG,
				DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE1,
				RG_DSI0_LNT1_LDOOUT_EN, 1);
		}

		/* step 14 */
		if (dsi_params->LANE_NUM > 2) {
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE2_REG,
				DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE2,
				RG_DSI0_LNT2_RT_CODE, 0x8);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE2_REG,
				DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE2,
				RG_DSI0_LNT2_LDOOUT_EN, 1);
		}

		/* step 15 */
		if (dsi_params->LANE_NUM > 3) {
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE3_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE3,
					 RG_DSI0_LNT3_RT_CODE, 0x8);
			MIPITX_OUTREGBIT(struct MIPITX_DSI_DATA_LANE3_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI0_DATA_LANE3,
					 RG_DSI0_LNT3_LDOOUT_EN, 1);
		}

		/* step 16 */
		MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON0_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON0,
				 RG_DSI0_MPPLL_PLL_EN, 1);

		/* step 17 */
		mdelay(1);

		if ((0 != data_Rate) && (1 != dsi_params->ssc_disable)) {
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON1_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON1,
					 RG_DSI0_MPPLL_SDM_SSC_EN, 1);
		} else {
			MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_CON1_REG,
					 DSI_PHY_REG[i]->MIPITX_DSI_PLL_CON1,
					 RG_DSI0_MPPLL_SDM_SSC_EN, 0);
		}

		/* step 18 */
		MIPITX_OUTREGBIT(struct MIPITX_DSI_PLL_TOP_REG, DSI_PHY_REG[i]->MIPITX_DSI_PLL_TOP,
				RG_MPPLL_PRESERVE_L, 3);
		MIPITX_OUTREGBIT(struct MIPITX_DSI_TOP_CON_REG, DSI_PHY_REG[i]->MIPITX_DSI_TOP_CON,
				RG_DSI_PAD_TIE_LOW_EN, 0);
		mdelay(1);
	}
#endif
}


void DSI_PHY_TIMCONFIG(DISP_MODULE_ENUM module, void *cmdq, LCM_DSI_PARAMS *dsi_params)
{
	struct DSI_PHY_TIMCON0_REG timcon0;
	struct DSI_PHY_TIMCON1_REG timcon1;
	struct DSI_PHY_TIMCON2_REG timcon2;
	struct DSI_PHY_TIMCON3_REG timcon3;
	unsigned int div1 = 0;
	unsigned int div2 = 0;
#if 0  /* Not use now */
	unsigned int pre_div = 0;
	unsigned int post_div = 0;
	unsigned int fbk_sel = 0;
#endif
	unsigned int fbk_div = 0;
	unsigned int lane_no = dsi_params->LANE_NUM;

	/* unsigned int div2_real; */
	unsigned int cycle_time;
	unsigned int ui;
	unsigned int hs_trail_m, hs_trail_n;

	if (0 != dsi_params->PLL_CLOCK) {
		ui = 1000 / (dsi_params->PLL_CLOCK * 2) + 0x01;
		cycle_time = 8000 / (dsi_params->PLL_CLOCK * 2) + 0x01;
		DDPMSG("DSI_PHY, Cycle Time: %d(ns), Unit Interval: %d(ns). , lane#: %d\n",
				   cycle_time, ui, lane_no);
	} else {
		div1 = dsi_params->pll_div1;
		div2 = dsi_params->pll_div2;
		fbk_div = dsi_params->fbk_div;
		switch (div1) {
		case 0:
			div1 = 1;
			break;

		case 1:
			div1 = 2;
			break;

		case 2:
		case 3:
			div1 = 4;
			break;

		default:
			DISPCHECK("div1 should be less than 4!!\n");
			div1 = 4;
			break;
		}

		switch (div2) {
		case 0:
			div2 = 1;
			break;
		case 1:
			div2 = 2;
			break;
		case 2:
		case 3:
			div2 = 4;
			break;
		default:
			DISPCHECK("div2 should be less than 4!!\n");
			div2 = 4;
			break;
		}

#if 0  /* Not use now */
		switch (pre_div) {
		case 0:
			pre_div = 1;
			break;

		case 1:
			pre_div = 2;
			break;

		case 2:
		case 3:
			pre_div = 4;
			break;

		default:
			DISPCHECK("pre_div should be less than 4!!\n");
			pre_div = 4;
			break;
		}

		switch (post_div) {
		case 0:
			post_div = 1;
			break;

		case 1:
			post_div = 2;
			break;

		case 2:
		case 3:
			post_div = 4;
			break;

		default:
			DISPCHECK("post_div should be less than 4!!\n");
			post_div = 4;
			break;
		}

		switch (fbk_sel) {
		case 0:
			fbk_sel = 1;
			break;

		case 1:
			fbk_sel = 2;
			break;

		case 2:
		case 3:
			fbk_sel = 4;
			break;

		default:
			DDPMSG("fbk_sel should be less than 4!!\n");
			fbk_sel = 4;
			break;
		}
#endif
		cycle_time = (1000 * 4 * div2 * div1) / (fbk_div * 26) + 0x01;

		ui = (1000 * div2 * div1) / (fbk_div * 26 * 0x2) + 0x01;
		DDPMSG("DSI_PHY, Cycle Time: %d(ns), Unit Interval: %d(ns). div1: %d, div2: %d, fbk_div: %d, lane#: %d\n",
				   cycle_time, ui, div1, div2, fbk_div, lane_no);
	}

	/* div2_real=div2 ? div2*0x02 : 0x1; */
	/* cycle_time = (1000 * div2 * div1 * pre_div * post_div)/ (fbk_sel * (fbk_div+0x01) * 26) + 1; */
	/* ui = (1000 * div2 * div1 * pre_div * post_div)/ (fbk_sel * (fbk_div+0x01) * 26 * 2) + 1; */
#define NS_TO_CYCLE(n, c)   ((n) / (c))

	hs_trail_m = 1;
	hs_trail_n =
		(dsi_params->HS_TRAIL == 0) ? NS_TO_CYCLE(((hs_trail_m * 0x4) + 0x60),
							  cycle_time) : dsi_params->HS_TRAIL;
	/* +3 is recommended from designer becauase of HW latency */
	timcon0.HS_TRAIL = ((hs_trail_m > hs_trail_n) ? hs_trail_m : hs_trail_n) + 0x0a;

	timcon0.HS_PRPR =
		(dsi_params->HS_PRPR == 0) ? NS_TO_CYCLE((0x40 + 0x5 * ui),
							 cycle_time) : dsi_params->HS_PRPR;
	/* HS_PRPR can't be 1. */
	if (timcon0.HS_PRPR == 0)
		timcon0.HS_PRPR = 1;

	timcon0.HS_ZERO =
		(dsi_params->HS_ZERO == 0) ? NS_TO_CYCLE((0xC8 + 0x0a * ui),
							 cycle_time) : dsi_params->HS_ZERO;
	if (timcon0.HS_ZERO > timcon0.HS_PRPR)
		timcon0.HS_ZERO -= timcon0.HS_PRPR;

	timcon0.LPX =
		(dsi_params->LPX == 0) ? NS_TO_CYCLE(0x50, cycle_time) : dsi_params->LPX;
	if (timcon0.LPX == 0)
		timcon0.LPX = 1;

	/* timcon1.TA_SACK	   = (dsi_params->TA_SACK == 0) ? 1 : dsi_params->TA_SACK; */
	timcon1.TA_GET =
		(dsi_params->TA_GET == 0) ? (0x5 * timcon0.LPX) : dsi_params->TA_GET;
	timcon1.TA_SURE =
		(dsi_params->TA_SURE == 0) ? (0x3 * timcon0.LPX / 0x2) : dsi_params->TA_SURE;
	timcon1.TA_GO = (dsi_params->TA_GO == 0) ? (0x4 * timcon0.LPX) : dsi_params->TA_GO;
	/* -------------------------------------------------------------- */
	/* NT35510 need fine tune timing */
	/* Data_hs_exit = 60 ns + 128UI */
	/* Clk_post = 60 ns + 128 UI. */
	/* -------------------------------------------------------------- */
	timcon1.DA_HS_EXIT =
		(dsi_params->DA_HS_EXIT == 0) ? NS_TO_CYCLE((0x3c + 0x80 * ui),
								cycle_time) : dsi_params->DA_HS_EXIT;

	timcon2.CLK_TRAIL =
		((dsi_params->CLK_TRAIL == 0) ? NS_TO_CYCLE(0x64,
				cycle_time) : dsi_params->CLK_TRAIL) + 0x0a;
	/* CLK_TRAIL can't be 1. */
	if (timcon2.CLK_TRAIL < 2)
		timcon2.CLK_TRAIL = 2;

	/* timcon2.LPX_WAIT    = (dsi_params->LPX_WAIT == 0) ? 1 : dsi_params->LPX_WAIT; */
	timcon2.CONT_DET = dsi_params->CONT_DET;
	timcon2.CLK_ZERO =
		(dsi_params->CLK_ZERO == 0) ? NS_TO_CYCLE(0x190,
							  cycle_time) : dsi_params->CLK_ZERO;

	timcon3.CLK_HS_PRPR =
		(dsi_params->CLK_HS_PRPR == 0) ? NS_TO_CYCLE(0x40,
								 cycle_time) : dsi_params->CLK_HS_PRPR;
	if (timcon3.CLK_HS_PRPR == 0)
		timcon3.CLK_HS_PRPR = 1;
	timcon3.CLK_HS_EXIT =
		(dsi_params->CLK_HS_EXIT == 0) ? (2 * timcon0.LPX) : dsi_params->CLK_HS_EXIT;
	timcon3.CLK_HS_POST =
		(dsi_params->CLK_HS_POST == 0) ? NS_TO_CYCLE((0x3c + 0x80 * ui),
								 cycle_time) : dsi_params->CLK_HS_POST;

	DDPMSG("DSI_PHY, HS_TRAIL:%d, HS_ZERO:%d, HS_PRPR:%d, LPX:%d, TA_GET:%d, TA_SURE:%d\n",
			   timcon0.HS_TRAIL, timcon0.HS_ZERO, timcon0.HS_PRPR, timcon0.LPX,
			   timcon1.TA_GET, timcon1.TA_SURE);
	DDPMSG("DSI_PHY, TA_GO:%d, CLK_TRAIL:%d, CLK_ZERO:%d, CLK_HS_PRPR:%d\n",
			   timcon1.TA_GO, timcon2.CLK_TRAIL,
			   timcon2.CLK_ZERO, timcon3.CLK_HS_PRPR);

	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON0_REG, DSI_REG[0]->DSI_PHY_TIMECON0, LPX, timcon0.LPX);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON0_REG, DSI_REG[0]->DSI_PHY_TIMECON0, HS_PRPR, timcon0.HS_PRPR);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON0_REG, DSI_REG[0]->DSI_PHY_TIMECON0, HS_ZERO, timcon0.HS_ZERO);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON0_REG, DSI_REG[0]->DSI_PHY_TIMECON0, HS_TRAIL, timcon0.HS_TRAIL);

	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON1_REG, DSI_REG[0]->DSI_PHY_TIMECON1, TA_GO, timcon1.TA_GO);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON1_REG, DSI_REG[0]->DSI_PHY_TIMECON1, TA_SURE, timcon1.TA_SURE);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON1_REG, DSI_REG[0]->DSI_PHY_TIMECON1, TA_GET, timcon1.TA_GET);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON1_REG, DSI_REG[0]->DSI_PHY_TIMECON1, DA_HS_EXIT, timcon1.DA_HS_EXIT);

	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON2_REG, DSI_REG[0]->DSI_PHY_TIMECON2, CONT_DET, timcon2.CONT_DET);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON2_REG, DSI_REG[0]->DSI_PHY_TIMECON2, CLK_ZERO, timcon2.CLK_ZERO);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON2_REG, DSI_REG[0]->DSI_PHY_TIMECON2, CLK_TRAIL, timcon2.CLK_TRAIL);

	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON3_REG, DSI_REG[0]->DSI_PHY_TIMECON3, CLK_HS_PRPR, timcon3.CLK_HS_PRPR);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON3_REG, DSI_REG[0]->DSI_PHY_TIMECON3, CLK_HS_POST, timcon3.CLK_HS_POST);
	DSI_OUTREGBIT(NULL,struct DSI_PHY_TIMCON3_REG, DSI_REG[0]->DSI_PHY_TIMECON3, CLK_HS_EXIT, timcon3.CLK_HS_EXIT);
	DDPMSG("%s, 0x%08x,0x%08x,0x%08x,0x%08x\n", __func__,
		DISP_REG_GET(DSI_REG[0] + 0x110), DISP_REG_GET(DSI_REG[0] + 0x114),
		DISP_REG_GET(DSI_REG[0] + 0x118), DISP_REG_GET(DSI_REG[0] + 0x11c));
}

DSI_STATUS DSI_EnableClk(DISP_MODULE_ENUM module, void *cmdq)
{
	DSI_OUTREGBIT(cmdq, struct DSI_COM_CTRL_REG, DSI_REG[0]->DSI_COM_CTRL, DSI_EN, 1);

	return DSI_STATUS_OK;
}

DSI_STATUS DSI_Start(DISP_MODULE_ENUM module, void *cmdq)
{
	DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[0]->DSI_START, DSI_START, 0);
	DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[0]->DSI_START, DSI_START, 1);

	return DSI_STATUS_OK;
}

DSI_STATUS DSI_EnableVM_CMD(DISP_MODULE_ENUM module, void *cmdq)
{
	int i = 0;
	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[i]->DSI_START, VM_CMD_START, 0);
		DSI_OUTREGBIT(cmdq, struct DSI_START_REG, DSI_REG[i]->DSI_START, VM_CMD_START, 1);
	}
	return DSI_STATUS_OK;
}

/* return value: the data length we got */
uint32_t DSI_dcs_read_lcm_reg_v2(DISP_MODULE_ENUM module, void *cmdq, uint8_t cmd,
	uint8_t *buffer, uint8_t buffer_size)
{
	uint32_t max_try_count = 5;
	uint32_t recv_data_cnt;
	unsigned int read_timeout_ms;
	unsigned char packet_type;
	struct DSI_RX_DATA_REG read_data0;
	struct DSI_RX_DATA_REG read_data1;
	struct DSI_RX_DATA_REG read_data2;
	struct DSI_RX_DATA_REG read_data3;
	int i =0;
	DSI_T0_INS t0;

#if ENABLE_DSI_INTERRUPT
	static const long WAIT_TIMEOUT = HZ / 2;
	long ret;
#endif

	if (DSI_REG[i]->DSI_MODE_CTRL.MODE)
		return 0;

	if (buffer == NULL || buffer_size == 0)
		return 0;

	do {
		if (max_try_count == 0)
			return 0;
		max_try_count--;
		recv_data_cnt = 0;
		read_timeout_ms = 20;
		DSI_WaitForNotBusy(module, cmdq);

		t0.CONFG = 0x04;	/* BTA */
		t0.Data0 = cmd;
		if (buffer_size < 0x3)
			t0.Data_ID = DSI_DCS_READ_PACKET_ID;
		else
			t0.Data_ID = DSI_GERNERIC_READ_LONG_PACKET_ID;
		t0.Data1 = 0;

		DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[i]->data[0], AS_UINT32(&t0));
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_CMDQ_SIZE, 1);

		DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG, DSI_REG[i]->DSI_RACK, DSI_RACK, 1);
		DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG, DSI_REG[i]->DSI_INTSTA, RD_RDY, 1);
		DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG, DSI_REG[i]->DSI_INTSTA, CMD_DONE, 1);
		DSI_OUTREGBIT(cmdq, struct DSI_INT_ENABLE_REG, DSI_REG[i]->DSI_INTEN, RD_RDY, 1);
		DSI_OUTREGBIT(cmdq, struct DSI_INT_ENABLE_REG, DSI_REG[i]->DSI_INTEN, CMD_DONE, 1);

		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_START, 0);
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_START, 1);

#if ENABLE_DSI_INTERRUPT
		ret = wait_event_interruptible_timeout(_dsi_dcs_read_wait_queue[i],
						       !ddp_dsi_is_busy(module), WAIT_TIMEOUT);
		if (0 == ret) {
			DDPMSG(" Wait for DSI engine read ready timeout!!!\n");

			DSI_DumpRegisters(module, NULL, 1);
			DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG, DSI_REG[i]->DSI_RACK, DSI_RACK, 1);
			DSI_Reset(module, NULL);

			return 0;
		}
#else
		DDPMSG(" Start polling DSI read ready!!!\n");

		while (DSI_REG[i]->DSI_INTSTA.RD_RDY == 0) {
			mdelay(1);
			read_timeout_ms--;

			if (read_timeout_ms == 0) {
				DISPCHECK(" Polling DSI read ready timeout!!!\n");
				DSI_DumpRegisters(module, NULL, 1);
				DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG, DSI_REG[i]->DSI_RACK, DSI_RACK, 1);
				DSI_Reset(module, NULL);

				return 0;
			}
		}

		DDPMSG(" End polling DSI read ready!!!\n");

		DSI_OUTREGBIT(cmdq, struct DSI_RACK_REG, DSI_REG[i]->DSI_RACK, DSI_RACK, 1);
		DSI_OUTREGBIT(cmdq, struct DSI_INT_STATUS_REG, DSI_REG[i]->DSI_INTSTA, RD_RDY, 0);
#endif

		DSI_OUTREG32(cmdq, &read_data0, AS_UINT32(&DSI_REG[i]->DSI_RX_DATA0));
		DSI_OUTREG32(cmdq, &read_data1, AS_UINT32(&DSI_REG[i]->DSI_RX_DATA1));
		DSI_OUTREG32(cmdq, &read_data2, AS_UINT32(&DSI_REG[i]->DSI_RX_DATA2));
		DSI_OUTREG32(cmdq, &read_data3, AS_UINT32(&DSI_REG[i]->DSI_RX_DATA3));

		DDPMSG(" DSI_CMDQ_SIZE : 0x%x\n", DSI_REG[i]->DSI_CMDQ_SIZE.CMDQ_SIZE);
		DDPMSG(" DSI_CMDQ_DATA0 : 0x%x\n", DSI_CMDQ_REG[i]->data[0].byte0);
		DDPMSG(" DSI_CMDQ_DATA1 : 0x%x\n", DSI_CMDQ_REG[i]->data[0].byte1);
		DDPMSG(" DSI_CMDQ_DATA2 : 0x%x\n", DSI_CMDQ_REG[i]->data[0].byte2);
		DDPMSG(" DSI_CMDQ_DATA3 : 0x%x\n", DSI_CMDQ_REG[i]->data[0].byte3);
		DDPMSG(" DSI_RX_DATA0 : 0x%x\n", *((uint32_t *)(&DSI_REG[i]->DSI_RX_DATA0)));
		DDPMSG(" DSI_RX_DATA1 : 0x%x\n", *((uint32_t *)(&DSI_REG[i]->DSI_RX_DATA1)));
		DDPMSG(" DSI_RX_DATA2 : 0x%x\n", *((uint32_t *)(&DSI_REG[i]->DSI_RX_DATA2)));
		DDPMSG(" DSI_RX_DATA3 : 0x%x\n", *((uint32_t *)(&DSI_REG[i]->DSI_RX_DATA3)));

		DDPMSG("read_data0, %x,%x,%x,%x\n", read_data0.byte0, read_data0.byte1,
			read_data0.byte2, read_data0.byte3);
		DDPMSG("read_data1, %x,%x,%x,%x\n", read_data1.byte0, read_data1.byte1,
			read_data1.byte2, read_data1.byte3);
		DDPMSG("read_data2, %x,%x,%x,%x\n", read_data2.byte0, read_data2.byte1,
			read_data2.byte2, read_data2.byte3);
		DDPMSG("read_data3, %x,%x,%x,%x\n", read_data3.byte0, read_data3.byte1,
			read_data3.byte2, read_data3.byte3);

		packet_type = read_data0.byte0;

		DISPCHECK(" DSI read packet_type is 0x%x\n", packet_type);

		if (packet_type == 0x1A || packet_type == 0x1C) {
			recv_data_cnt = read_data0.byte1 + read_data0.byte2 * 16;
			if (recv_data_cnt > 10) {
				recv_data_cnt = 10;
			}

			if (recv_data_cnt > buffer_size) {
				recv_data_cnt = buffer_size;
			}

			DDPMSG(" DSI read long packet size: %d\n", recv_data_cnt);
			if (recv_data_cnt <= 4) {
			memcpy((void *)buffer, (void *)&read_data1, recv_data_cnt);
			} else if (recv_data_cnt <= 8) {
				memcpy((void *)buffer, (void *)&read_data1, 4);
				memcpy((void *)buffer + 4, (void *)&read_data2,
					   recv_data_cnt - 4);
			} else {
				memcpy((void *)buffer, (void *)&read_data1, 4);
				memcpy((void *)buffer + 4, (void *)&read_data2, 4);
				memcpy((void *)buffer + 8, (void *)&read_data2,
					   recv_data_cnt - 8);
			}
		} else {
			/* short  packet */
			recv_data_cnt = 2;
			if (recv_data_cnt > buffer_size) {
				DDPMSG(" DSI read short packet data exceeds buffer size: %d\n", buffer_size);
				recv_data_cnt = buffer_size;
			}
			memcpy((void *)buffer, (void *)&read_data0.byte1, 2);
		}
	} while (packet_type != 0x1C && packet_type != 0x21 && packet_type != 0x22
		 && packet_type != 0x1A);

	return recv_data_cnt;
}

void DSI_set_null(DISP_MODULE_ENUM module, void *cmdq, unsigned cmd, unsigned char count,
		  unsigned char *para_list, unsigned char force_update)
{
	UINT32 i = 0;
	int d = 0;
	UINT32 goto_addr, mask_para, set_para;
	DSI_T0_INS t0 = {0};
	DSI_T2_INS t2 = {0};

	/* DISPFUNC(); */
	for (d = DSI_MODULE_BEGIN(module); d <= DSI_MODULE_END(module); d++) {
		if (0 != DSI_REG[d]->DSI_MODE_CTRL.MODE) {	/* not in cmd mode */
		} else {
			DSI_WaitForNotBusy(module, cmdq);

			/* null packet */
			t2.CONFG = 2;
			t2.Data_ID = DSI_NULL_PACKET_ID;
			t2.WC16 = count;

			DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[d]->data[0], AS_UINT32(&t2));
			DISPMSG("[DSI] start: 0x%08x\n", AS_UINT32(&DSI_CMDQ_REG[d]->data[0]));

			for (i = 0; i < count; i++) {
				goto_addr = (UINT32) (&DSI_CMDQ_REG[d]->data[1].byte0) + i;
				mask_para = (0xFF << ((goto_addr & 0x3) * 8));
				set_para = (para_list[i] << ((goto_addr & 0x3) * 8));
				DSI_MASKREG32(cmdq, goto_addr & (~0x3), mask_para, set_para);

				if ((i & 0x3) == 0x3)
					DISPMSG("[DSI] cmd: 0x%08x\n",
						AS_UINT32(&DSI_CMDQ_REG[d]->data[1 + (i / 4)]));
			}

			DSI_OUTREG32(cmdq, &DSI_REG[d]->DSI_CMDQ_SIZE, 1 + (count) / 4);
			DISPMSG("[DSI] size: 0x%08x\n", AS_UINT32(&DSI_REG[d]->DSI_CMDQ_SIZE));

			if (force_update) {
				DSI_Start(module, cmdq);
				DSI_WaitForNotBusy(module, cmdq);
			}
		}
	}
}

void DSI_set_cmdq_V2(DISP_MODULE_ENUM module, void *cmdq, unsigned cmd,
	unsigned char count, unsigned char *para_list, unsigned char force_update)
{
	uint32_t i;
	uint32_t goto_addr, mask_para, set_para;
	/* uint32_t fbPhysAddr, fbVirAddr; */
	DSI_T0_INS t0 = { 0 };
	DSI_T2_INS t2 = { 0 };
	DSI_WaitForNotBusy(module, cmdq);
	if (cmd < 0xB0) {
		if (count > 1) {
			t2.CONFG = 2;
			t2.Data_ID = DSI_DCS_LONG_PACKET_ID;
			t2.WC16 = count + 1;

			DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[0]->data[0], AS_UINT32(&t2));

			goto_addr = (unsigned long) (&DSI_CMDQ_REG[0]->data[1].byte0);
			mask_para = (0xFF << ((goto_addr & 0x3) * 8));
			set_para = (cmd << ((goto_addr & 0x3) * 8));
			DSI_MASKREG32(cmdq, goto_addr & (~0x3), mask_para, set_para);

			for (i = 0; i < count; i++) {
				goto_addr = (unsigned long) (&DSI_CMDQ_REG[0]->data[1].byte1) + i;
				mask_para = (0xFF << ((goto_addr & 0x3) * 8));
				set_para = (para_list[i] << ((goto_addr & 0x3) * 8));
				DSI_MASKREG32(cmdq, goto_addr & (~0x3), mask_para, set_para);
			}

			DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_CMDQ_SIZE, 2 + (count) / 4);
		} else {
			t0.CONFG = 0;
			t0.Data0 = cmd;
			if (count) {
				t0.Data_ID = DSI_DCS_SHORT_PACKET_ID_1;
				t0.Data1 = para_list[0];
			} else {
				t0.Data_ID = DSI_DCS_SHORT_PACKET_ID_0;
				t0.Data1 = 0;
			}
			DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[0]->data[0], AS_UINT32(&t0));
			DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_CMDQ_SIZE, 1);
		}
	} else {
		if (count > 1) {
			t2.CONFG = 2;
			t2.Data_ID = DSI_GERNERIC_LONG_PACKET_ID;
			t2.WC16 = count + 1;

			DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[0]->data[0], AS_UINT32(&t2));

			goto_addr = (unsigned long) (&DSI_CMDQ_REG[0]->data[1].byte0);
			mask_para = (0xFF << ((goto_addr & 0x3) * 8));
			set_para = (cmd << ((goto_addr & 0x3) * 8));
			DSI_MASKREG32(cmdq, goto_addr & (~0x3), mask_para, set_para);

			for (i = 0; i < count; i++) {
				goto_addr = (unsigned long) (&DSI_CMDQ_REG[0]->data[1].byte1) + i;
				mask_para = (0xFF << ((goto_addr & 0x3) * 8));
				set_para = (para_list[i] << ((goto_addr & 0x3) * 8));
				DSI_MASKREG32(cmdq, goto_addr & (~0x3), mask_para, set_para);
			}

			DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_CMDQ_SIZE, 2 + (count) / 4);

		} else {
			t0.CONFG = 0;
			t0.Data0 = cmd;
			if (count) {
				t0.Data_ID = DSI_GERNERIC_SHORT_PACKET_ID_2;
				t0.Data1 = para_list[0];
			} else {
				t0.Data_ID = DSI_GERNERIC_SHORT_PACKET_ID_1;
				t0.Data1 = 0;
			}
			DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[0]->data[0], AS_UINT32(&t0));
			DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_CMDQ_SIZE, 1);
		}
	}

	/* for (i = 0; i < AS_UINT32(&DSI_REG[0]->DSI_CMDQ_SIZE); i++) */
	/*ISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI",
	   "DSI_set_cmdq_V2. DSI_CMDQ+%04x : 0x%08x\n", i*4, DISP_REG_GET(DSI_REG[0] + 0x180 + i*4)); */

	if (force_update) {
		DSI_Start(module, cmdq);
		DSI_WaitForNotBusy(module, cmdq);
	}
}

void DSI_set_cmdq_V3(DISP_MODULE_ENUM module, void *cmdq,
	LCM_setting_table_V3 *para_tbl, unsigned int size, unsigned char force_update)
{
	uint32_t i;
	unsigned long goto_addr, mask_para, set_para;
	/* uint32_t fbPhysAddr, fbVirAddr; */
	DSI_T0_INS t0;
	/* DSI_T1_INS t1; */
	DSI_T2_INS t2;

	uint32_t index = 0;

	unsigned char data_id, cmd, count;
	unsigned char *para_list;

	do {
		data_id = para_tbl[index].id;
		cmd = para_tbl[index].cmd;
		count = para_tbl[index].count;
		para_list = para_tbl[index].para_list;

		if (data_id == REGFLAG_ESCAPE_ID && cmd == REGFLAG_DELAY_MS_V3)
		{
			udelay(1000*count);
			DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "DSI_set_cmdq_V3[%d]. Delay %d (ms) \n", index, count);

			continue;
		}

		DSI_WaitForNotBusy(module, cmdq);
		/* for(i = 0; i < sizeof(DSI_CMDQ_REG->data0) / sizeof(DSI_CMDQ); i++) */
		/* OUTREG32(&DSI_CMDQ_REG->data0[i], 0); */
		/* memset(&DSI_CMDQ_REG->data[0], 0, sizeof(DSI_CMDQ_REG->data[0])); */
		DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[0]->data[0], 0);

		if (count > 1) {
			t2.CONFG = 2;
			t2.Data_ID = data_id;
			t2.WC16 = count + 1;

			DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[0]->data[0].byte0, AS_UINT32(&t2));

			goto_addr = (unsigned long) (&DSI_CMDQ_REG[0]->data[1].byte0);
			mask_para = (0xFF << ((goto_addr & 0x3) * 8));
			set_para = (cmd << ((goto_addr & 0x3) * 8));
			DSI_MASKREG32(cmdq, goto_addr & (~0x3), mask_para, set_para);

			for (i = 0; i < count; i++) {
				goto_addr =
				    (unsigned long) (&DSI_CMDQ_REG[0]->data[1].byte1) + i;
				mask_para = (0xFF << ((goto_addr & 0x3) * 8));
				set_para =
				    (para_list[i] << ((goto_addr & 0x3) * 8));
				DSI_MASKREG32(cmdq, goto_addr & (~0x3), mask_para, set_para);
			}

			DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_CMDQ_SIZE, 2 + (count) / 4);
		} else {
			t0.CONFG = 0;
			t0.Data0 = cmd;
			if (count) {
				t0.Data_ID = data_id;
				t0.Data1 = para_list[0];
			} else {
				t0.Data_ID = data_id;
				t0.Data1 = 0;
			}
			DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[0]->data[0], AS_UINT32(&t0));
			DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_CMDQ_SIZE, 1);
		}

		for (i = 0; i < AS_UINT32(&DSI_REG[0]->DSI_CMDQ_SIZE); i++)
			DDPMSG("DSI_set_cmdq_V3[%d]. DSI_CMDQ+%04x : 0x%08x\n",
				       index, i * 4,
				       DISP_REG_GET(DSI_PHY_REG[0] + 0x180 + i * 4));

		if (force_update) {
			DSI_Start(module, cmdq);
			for (i = 0; i < 10; i++)
				;
			DSI_WaitForNotBusy(module, cmdq);
		}
	} while (++index < size);
}

void DSI_set_cmdq(DISP_MODULE_ENUM module, void *cmdq,
	unsigned int *pdata, unsigned int queue_size, unsigned char force_update)
{
	UINT32 i;

	ASSERT(queue_size<=16);

    DSI_WaitForNotBusy(module, cmdq);

	for(i=0; i<queue_size; i++)
	{
		DSI_OUTREG32(cmdq, &DSI_CMDQ_REG[0]->data[i], AS_UINT32((pdata+i)));
	}

	DSI_OUTREG32(cmdq, &DSI_REG[0]->DSI_CMDQ_SIZE, queue_size);

	if (0 != DSI_REG[0]->DSI_MODE_CTRL.MODE) {
	} else {
		if(force_update)
		{
			DISPMSG("DSI_force_update\n");
			DSI_Start(module, cmdq);
			DSI_WaitForNotBusy(module, cmdq);
		}
	}
}


void DSI_set_rar(DISP_MODULE_ENUM module, void *cmdq)
{
	int i = 0;
	char *module_name = ddp_get_module_name(module);
	struct DSI_PHY_LD0CON_REG phy_ld0con;
	memset(&phy_ld0con, 0, sizeof(struct DSI_PHY_LD0CON_REG));

	DISPMSG("DSI_set_rar, module=%s, cmdq=0x%08x\n", module_name, (unsigned int)cmdq);

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		DSI_READREG32(PDSI_PHY_LD0CON_REG, &phy_ld0con, &DSI_REG[i]->DSI_PHY_LD0CON);
		phy_ld0con.L0_HS_TX_EN/*L0_RM_TRIG_EN*/ = 1;
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_PHY_LD0CON, AS_UINT32(&phy_ld0con));
		mdelay(1);
		phy_ld0con.L0_HS_TX_EN = 0;
		DSI_OUTREG32(cmdq, &DSI_REG[i]->DSI_PHY_LD0CON, AS_UINT32(&phy_ld0con));
		mdelay(1);
	}
}

void _copy_dsi_params(LCM_DSI_PARAMS *src, LCM_DSI_PARAMS *dst)
{
	memcpy((LCM_DSI_PARAMS *) dst, (LCM_DSI_PARAMS *) src, sizeof(LCM_DSI_PARAMS));
}

int ddp_dsi_init(DISP_MODULE_ENUM module, void *cmdq)
{
	DSI_STATUS ret = DSI_STATUS_OK;
	int i = 0;
	DISPFUNC();

	DSI_MASKREG32(cmdq, 0x10000050, 0x10, 0x1);
	ddp_enable_module_clock(module);

	memset(&_dsi_context, 0, sizeof(_dsi_context));
	DSI_REG[0] = (struct DSI_REGS *) DSI0_BASE;
	DSI_PHY_REG[0] = (struct DSI_PHY_REGS *) MIPI_TX0_BASE;
	DSI_CMDQ_REG[0] = (struct DSI_CMDQ_REGS *) (DSI0_BASE + 0x180);
	DSI_VM_CMD_REG[0] = (struct DSI_VM_CMDQ_REGS *) (DSI0_BASE + 0x134);

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		DISPCHECK("dsi%d init finished\n", i);
	}

	return DSI_STATUS_OK;
}

int ddp_dsi_deinit(DISP_MODULE_ENUM module, void *cmdq)
{
	int i = 0;

	memset(&_dsi_context, 0, sizeof(_dsi_context));
	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		DISPCHECK("dsi%d init finished\n", i);
	}
	DSI_SetMode(module, NULL, CMD_MODE);
	DSI_clk_HS_mode(module, NULL, FALSE);
	ddp_disable_module_clock(module);

	DSI_PHY_clk_switch(module, NULL, false);

	return 0;
}

void _dump_dsi_params(LCM_DSI_PARAMS *dsi_config)
{
	int i = 0;

	if (dsi_config) {
		switch (dsi_config->mode) {
		case CMD_MODE:
			DISPCHECK("[DDPDSI] DSI Mode: CMD_MODE\n");
			break;
		case SYNC_PULSE_VDO_MODE:
			DISPCHECK("[DDPDSI] DSI Mode: SYNC_PULSE_VDO_MODE\n");
			break;
		case SYNC_EVENT_VDO_MODE:
			DISPCHECK("[DDPDSI] DSI Mode: SYNC_EVENT_VDO_MODE\n");
			break;
		case BURST_VDO_MODE:
			DISPCHECK("[DDPDSI] DSI Mode: BURST_VDO_MODE\n");
			break;
		default:
			DISPCHECK("[DDPDSI] DSI Mode: Unknown\n");
			break;
		}

		DISPCHECK("[DDPDSI] LANE_NUM: %d,data_format: %d,vertical_sync_active: %d\n",
			  dsi_config->LANE_NUM, dsi_config->data_format.format, dsi_config->vertical_sync_active);
		DISPCHECK
		    ("[DDPDSI] vact: %d, vbp: %d, vfp: %d, vact_line: %d, hact: %d, hbp: %d, hfp: %d, hblank: %d\n",
		     dsi_config->vertical_sync_active, dsi_config->vertical_backporch,
		     dsi_config->vertical_frontporch, dsi_config->vertical_active_line,
		     dsi_config->horizontal_sync_active, dsi_config->horizontal_backporch,
		     dsi_config->horizontal_frontporch, dsi_config->horizontal_blanking_pixel);
		DISPCHECK
		    ("[DDPDSI] pll_select: %d, pll_div1: %d, pll_div2: %d, fbk_div: %d,fbk_sel: %d, rg_bir: %d\n",
		     dsi_config->pll_select, dsi_config->pll_div1, dsi_config->pll_div2,
		     dsi_config->fbk_div, dsi_config->fbk_sel, dsi_config->rg_bir);
		DISPCHECK
		    ("[DDPDSI] rg_bic: %d, rg_bp: %d, PLL_CLOCK: %d, dsi_clock: %d, ssc_range: %d,	ssc_disable: %d, compatibility_for_nvk: %d, cont_clock: %d\n",
		     dsi_config->rg_bic, dsi_config->rg_bp, dsi_config->PLL_CLOCK,
		     dsi_config->dsi_clock, dsi_config->ssc_range, dsi_config->ssc_disable,
		     dsi_config->compatibility_for_nvk, dsi_config->cont_clock);
		DISPCHECK
		    ("[DDPDSI] lcm_ext_te_enable: %d, noncont_clock: %d, noncont_clock_period: %d\n",
		     dsi_config->lcm_ext_te_enable, dsi_config->noncont_clock,
		     dsi_config->noncont_clock_period);
	}

	return;
}

void DSI_Set_VM_CMD(DISP_MODULE_ENUM module, cmdqRecHandle cmdq)
{

	DSI_OUTREGBIT(cmdq, struct DSI_VM_CMD_CON_REG, DSI_REG[0]->DSI_VM_CMD_CON,
		      TS_VFP_EN, 1);
	DSI_OUTREGBIT(cmdq, struct DSI_VM_CMD_CON_REG, DSI_REG[0]->DSI_VM_CMD_CON,
		      VM_CMD_EN, 1);
	return;
}

int ddp_dsi_config(DISP_MODULE_ENUM module, disp_ddp_path_config *config, void *cmdq_handle)
{
	int i = 0;
	DISPFUNC();

	if (!config->dst_dirty)
		return 0;

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		_copy_dsi_params(&(config->dsi_config), &(_dsi_context[i].dsi_params));
		_dump_dsi_params(&(_dsi_context[i].dsi_params));
	}

	DSI_PHY_clk_setting(module, NULL, &(config->dsi_config));
	DSI_TXRX_Control(module, NULL, &(config->dsi_config));
	DSI_PS_Control(module, NULL, &(config->dsi_config), config->dst_w, config->dst_h);
	DSI_PHY_TIMCONFIG(module, NULL, &(config->dsi_config));
	DSI_EnableClk(module, NULL);

/* if(config->dsi_config.mode != CMD_MODE) */
	if (config->dsi_config.mode != CMD_MODE || ((config->dsi_config.switch_mode_enable == 1)
						    && (config->dsi_config.switch_mode !=
							CMD_MODE))) {
		DSI_Config_VDO_Timing(module, NULL, &(config->dsi_config));
		DSI_Set_VM_CMD(module, cmdq_handle);
	}
#if 0
	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		DSI_OUTREGBIT(cmdq_handle, DSI_INT_ENABLE_REG, DSI_REG[i]->DSI_INTEN, CMD_DONE, 1);
		DSI_OUTREGBIT(cmdq_handle, DSI_INT_ENABLE_REG, DSI_REG[i]->DSI_INTEN, RD_RDY, 1);
		DSI_OUTREGBIT(cmdq_handle, DSI_INT_ENABLE_REG, DSI_REG[i]->DSI_INTEN, TE_RDY, 1);
/* DSI_OUTREGBIT(cmdq_handle, DSI_INT_ENABLE_REG,DSI_REG[i]->DSI_INTEN,EXT_TE,1); */
		DSI_OUTREGBIT(cmdq_handle, DSI_INT_ENABLE_REG, DSI_REG[i]->DSI_INTEN, VM_DONE, 1);
	}
#endif

	/* Enable clk low power per Line ; */
	if (config->dsi_config.clk_lp_per_line_enable) {
		DSI_PHY_CLK_LP_PerLine_config(module, NULL, &(config->dsi_config));
	}

done:
	DSI_BackupRegisters(module, cmdq_handle);
	/* DSI_BIST_Pattern_Test(FALSE, 0x00ffff00); */
	return 0;
}

int ddp_dsi_stop(DISP_MODULE_ENUM module, struct disp_path_config_struct_ex *config,
		 void *cmdq_handle)
{
	/* ths caller should call wait_event_or_idle for frame stop event then. */
	if (_dsi_is_video_mode(module)) {
		DSI_SetMode(module, cmdq_handle, CMD_MODE);
	}

	return 0;
}

int ddp_dsi_reset(DISP_MODULE_ENUM module, void *cmdq_handle)
{
	DSI_Reset(module, cmdq_handle);

	return 0;
}

static int s_isDsiPowerOn;

int ddp_dsi_power_on(DISP_MODULE_ENUM module, void *cmdq_handle)
{
	int i = 0;
	int ret = 0;

	if (!s_isDsiPowerOn) {
		if (module == DISP_MODULE_DSI0 || module == DISP_MODULE_DSI1) {
			ddp_enable_module_clock(module);

			if (ret > 0) {
				DISP_LOG_PRINT(ANDROID_LOG_WARN, "DSI",
					       "DSI0 power manager API return FALSE\n");
			}
		}

		s_isDsiPowerOn = TRUE;
	}

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		if (_dsi_context[i].dsi_params.mode == CMD_MODE) {
			DSI_PHY_clk_switch(module, NULL, true);

			/* restore dsi register */
			DSI_RestoreRegisters(module, NULL);

			/* enable sleep-out mode */
			DSI_SleepOut(module, NULL);

			/* enter wakeup */
			DSI_Wakeup(module, NULL);

			DSI_Reset(module, NULL);
		} else {
			/* initialize clock setting */
			DSI_PHY_clk_switch(module, NULL, true);

			/* restore dsi register */
			DSI_RestoreRegisters(module, NULL);

			/* enable sleep-out mode */
			DSI_SleepOut(module, NULL);

			/* enter wakeup */
			DSI_Wakeup(module, NULL);
			DSI_clk_HS_mode(module, NULL, false);

			DSI_Reset(module, NULL);
		}
	}

	return DSI_STATUS_OK;
}


int ddp_dsi_power_off(DISP_MODULE_ENUM module, void *cmdq_handle)
{
	int i = 0;
	int ret = 0;

	if (!s_isDsiPowerOn) {
		if (module == DISP_MODULE_DSI0 || module == DISP_MODULE_DSI1) {
			ddp_disable_module_clock(module);

			if (ret > 0) {
				DISP_LOG_PRINT(ANDROID_LOG_WARN, "DSI0",
					       "DSI0 power manager API return FALSE\n");
			}
		}
		s_isDsiPowerOn = TRUE;
	}

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		if (_dsi_context[i].dsi_params.mode == CMD_MODE) {
			/* no need this, we will make dsi is in idle when ddp_dsi_stop() returns */
			/* DSI_CHECK_RET(DSI_WaitForNotBusy(module, NULL)); */
			DSI_CHECK_RET(DSI_BackupRegisters(module, NULL));

			/* disable HS mode */
			DSI_clk_HS_mode(module, NULL, false);
			/* enter ULPS mode */
			DSI_lane0_ULP_mode(module, NULL, 1);
			DSI_clk_ULP_mode(module, NULL, 1);
			/* disable mipi pll */
			DSI_PHY_clk_switch(module, NULL, false);
		} else {
			/* backup dsi register */
			/* no need this, we will make dsi is in idle when ddp_dsi_stop() returns */
			/* DSI_CHECK_RET(DSI_WaitForNotBusy()); */
			DSI_BackupRegisters(module, NULL);

			/* disable HS mode */
			DSI_clk_HS_mode(module, NULL, false);
			/* enter ULPS mode */
			DSI_lane0_ULP_mode(module, NULL, 1);
			DSI_clk_ULP_mode(module, NULL, 1);

			/* disable mipi pll */
			DSI_PHY_clk_switch(module, NULL, false);
		}
	}

	return DSI_STATUS_OK;
}
static int busy_log_cnt = 0;
int ddp_dsi_is_busy(DISP_MODULE_ENUM module)
{
	int i = 0;
	int busy = 0;
	struct DSI_INT_STATUS_REG status;
	/* DISPFUNC(); */

	for (i = DSI_MODULE_BEGIN(module); i <= DSI_MODULE_END(module); i++) {
		status = DSI_REG[i]->DSI_INTSTA;

		if (status.BUSY)
			busy++;
	}

	if(busy_log_cnt == 0)
		DISPDBG("%s is %d %s\n", ddp_get_module_name(module), busy, busy ? "busy" : "idle");
	busy_log_cnt = (busy_log_cnt + 1) % 20;

	return busy;
}

int ddp_dsi_is_idle(DISP_MODULE_ENUM module)
{
	return !ddp_dsi_is_busy(module);
}

int ddp_dsi_dump(DISP_MODULE_ENUM module, int level)
{
	DSI_DumpRegisters(module, NULL, level);
	return 0;
}

int ddp_dsi_start(DISP_MODULE_ENUM module, void *cmdq)
{
	int i = 0;
#if 0
	if (module == DISP_MODULE_DSIDUAL) {
		DSI_OUTREGBIT(cmdq, DSI_START_REG, DSI_REG[0]->DSI_START, DSI_START, 0);
		DSI_OUTREGBIT(cmdq, DSI_START_REG, DSI_REG[1]->DSI_START, DSI_START, 0);
		DSI_OUTREGBIT(cmdq, DSI_COM_CTRL_REG, DSI_REG[0]->DSI_COM_CTRL, DSI_DUAL_EN, 1);
		DSI_OUTREGBIT(cmdq, DSI_COM_CTRL_REG, DSI_REG[1]->DSI_COM_CTRL, DSI_DUAL_EN, 1);
		DSI_SetMode(module, cmdq, _dsi_context[i].dsi_params.mode);
		DSI_clk_HS_mode(module, cmdq, TRUE);
	} else
#endif
	{
		DSI_SetMode(module, cmdq, _dsi_context[i].dsi_params.mode);
		DSI_clk_HS_mode(module, cmdq, TRUE);
	}

	return 0;
}

int ddp_dsi_trigger(DISP_MODULE_ENUM module, void *cmdq)
{
	int i = 0;
#if 0
	static int j;
	DSI_OUTREG32(NULL, 0x14012178, 0x00000000 | (0xFF << (j * 8)));
	DSI_OUTREG32(NULL, 0x1401217C, 0x00000040);
	j++;
#endif
	DSI_Start(module, cmdq);

	return 0;
}

static void lcm_set_reset_pin(UINT32 value)
{
	OUTREG32(MMSYS_CONFIG_BASE + 0x150, value);
}

static void lcm_udelay(UINT32 us)
{
	udelay(us);
}

static void lcm_mdelay(UINT32 ms)
{
	mdelay(ms);
}

static void lcm_rar(UINT32 ms)
{
	DSI_set_rar(DISP_MODULE_DSI0, NULL);
	mdelay(ms);
}

void DSI_set_null_Wrapper_DSI0(unsigned cmd, unsigned char count, unsigned char *para_list,
			       unsigned char force_update)
{
	DSI_set_null(DISP_MODULE_DSI0, NULL, cmd, count, para_list, force_update);
}

void DSI_set_cmdq_V2_Wrapper_DSI0(unsigned cmd, unsigned char count, unsigned char *para_list,
				  unsigned char force_update)
{
	DSI_set_cmdq_V2(DISP_MODULE_DSI0, NULL, cmd, count, para_list, force_update);
}

void DSI_set_cmdq_V3_Wrapper_DSI0(LCM_setting_table_V3 *para_tbl, unsigned int size,
				  unsigned char force_update)
{
	DSI_set_cmdq_V3(DISP_MODULE_DSI0, NULL, para_tbl, size, force_update);
}

void DSI_set_cmdq_wrapper_DSI0(unsigned int *pdata, unsigned int queue_size,
			       unsigned char force_update)
{
	DSI_set_cmdq(DISP_MODULE_DSI0, NULL, pdata, queue_size, force_update);
}

unsigned int DSI_dcs_read_lcm_reg_v2_wrapper_DSI0(UINT8 cmd, UINT8 *buffer, UINT8 buffer_size)
{
	return DSI_dcs_read_lcm_reg_v2(DISP_MODULE_DSI0, NULL, cmd, buffer, buffer_size);
}

static const LCM_UTIL_FUNCS lcm_utils_dsi0;

int ddp_dsi_set_lcm_utils(DISP_MODULE_ENUM module, LCM_DRIVER *lcm_drv)
{
	LCM_UTIL_FUNCS *utils = NULL;
	if (lcm_drv == NULL) {
		DISPERR("lcm_drv is null\n");
		return -1;
	}

	if (module == DISP_MODULE_DSI0) {
		utils = &lcm_utils_dsi0;
	}
	else {
		DISPERR("wrong module: %d\n", module);
		return -1;
	}

	utils->set_reset_pin = lcm_set_reset_pin;
	utils->udelay = lcm_udelay;
	utils->mdelay = lcm_mdelay;
	utils->rar = lcm_rar;

	if (module == DISP_MODULE_DSI0) {
		utils->dsi_set_cmdq = DSI_set_cmdq_wrapper_DSI0;
		utils->dsi_set_cmdq_V2 = DSI_set_cmdq_V2_Wrapper_DSI0;
		utils->dsi_set_null = DSI_set_null_Wrapper_DSI0;
		utils->dsi_dcs_read_lcm_reg_v2 = DSI_dcs_read_lcm_reg_v2_wrapper_DSI0;
	}
	utils->set_gpio_out = mt_set_gpio_out;
	utils->set_gpio_mode = mt_set_gpio_mode;
	utils->set_gpio_dir = mt_set_gpio_dir;
	utils->set_gpio_pull_enable = (int (*)(unsigned int, unsigned char))mt_set_gpio_pull_enable;
	lcm_drv->set_util_funcs(utils);

	return 0;
}

static int dsi0_te_enable;
static int dsi1_te_enable;
static int dsidual_te_enable;

static int ddp_dsi_polling_irq(DISP_MODULE_ENUM module, int bit, int timeout)
{
	int i = 0;
	unsigned int cnt = 0;
	unsigned int irq_reg_base = 0;
	unsigned int reg_val = 0;

	if (module == DISP_MODULE_DSI0 /* || module == DISP_MODULE_DSIDUAL */)
		irq_reg_base = DISP_REG_DSI_INSTA;
#if 0
	else
		irq_reg_base = 0x1401C00C;
#endif
	/* DISPCHECK("dsi polling irq, module=%d, bit=0x%08x, timeout=%d, irq_regbase=0x%08x\n", module, bit, timeout, irq_reg_base); */

	if (timeout <= 0) {
		while ((DISP_REG_GET(irq_reg_base) & bit) == 0);
		cnt = 1;
	} else {
		/* time need to update */
		cnt = timeout * 1000 / 100;
		while (cnt > 0) {
			cnt--;
			reg_val = DISP_REG_GET(irq_reg_base);
			/* DISPMSG("reg_val=0x%08x\n", reg_val); */
			if (reg_val & bit) {
				DSI_OUTREG32(NULL, irq_reg_base, ~reg_val);
				break;
			}
			udelay(100);
		}
	}

	DISPMSG("DSI polling interrupt ret =%d\n", cnt);

	if (cnt == 0)
		DSI_DumpRegisters(module, NULL, 2);

	return cnt;
}

DDP_MODULE_DRIVER ddp_driver_dsi0 = {
	.module = DISP_MODULE_DSI0,
	.init = ddp_dsi_init,
	.deinit = ddp_dsi_deinit,
	.config = ddp_dsi_config,
	.trigger = ddp_dsi_trigger,
	.start = ddp_dsi_start,
	.stop = ddp_dsi_stop,
	.reset = ddp_dsi_reset,
	.power_on = ddp_dsi_power_on,
	.power_off = ddp_dsi_power_off,
	.is_idle = ddp_dsi_is_idle,
	.is_busy = ddp_dsi_is_busy,
	.dump_info = ddp_dsi_dump,
	.set_lcm_utils = ddp_dsi_set_lcm_utils,
	.polling_irq = ddp_dsi_polling_irq
};
