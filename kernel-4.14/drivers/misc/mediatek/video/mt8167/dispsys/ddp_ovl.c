/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#define LOG_TAG "OVL"
#include "ddp_log.h"

#ifdef CONFIG_MTK_CLKMGR
#include <mach/mt_clkmgr.h>
#endif

#ifdef CONFIG_MTK_M4U
#include "m4u.h"
#endif
#include <linux/delay.h>

#include "disp_drv_platform.h"
#include "ddp_info.h"
#include "ddp_hal.h"
#include "ddp_reg.h"
#include "ddp_ovl.h"
#include "ddp_drv.h"
#include "primary_display.h"
#include "ddp_debug.h"
#include "disp_assert_layer.h"
#include "ddp_irq.h"

#define OVL_NUM			(2)
#define OVL_REG_BACK_MAX	(40)
#define OVL_LAYER_OFFSET	(0x20)
#define OVL_RDMA_DEBUG_OFFSET	(0x4)

enum OVL_COLOR_SPACE {
	OVL_COLOR_SPACE_RGB = 0,
	OVL_COLOR_SPACE_YUV,
};

enum OVL_INPUT_FORMAT {
	OVL_INPUT_FORMAT_BGR565 = 0,
	OVL_INPUT_FORMAT_RGB888 = 1,
	OVL_INPUT_FORMAT_RGBA8888 = 2,
	OVL_INPUT_FORMAT_ARGB8888 = 3,
	OVL_INPUT_FORMAT_VYUY = 4,
	OVL_INPUT_FORMAT_YVYU = 5,

	OVL_INPUT_FORMAT_RGB565 = 6,
	OVL_INPUT_FORMAT_BGR888 = 7,
	OVL_INPUT_FORMAT_BGRA8888 = 8,
	OVL_INPUT_FORMAT_ABGR8888 = 9,
	OVL_INPUT_FORMAT_UYVY = 10,
	OVL_INPUT_FORMAT_YUYV = 11,

	OVL_INPUT_FORMAT_UNKNOWN = 32,
};

struct OVL_REG {
	unsigned long address;
	unsigned int value;
};

enum DISP_OVL1_STATUS ovl1_status = DDP_OVL1_STATUS_IDLE;
static int reg_back_cnt[OVL_NUM];
static struct OVL_REG reg_back[OVL_NUM][OVL_REG_BACK_MAX];

static char *ovl_get_status_name(enum DISP_OVL1_STATUS status)
{
	switch (status) {
	case DDP_OVL1_STATUS_IDLE:
		return "idle";
	case DDP_OVL1_STATUS_PRIMARY:
		return "primary";
	case DDP_OVL1_STATUS_SUB:
		return "sub";
	case DDP_OVL1_STATUS_SUB_REQUESTING:
		return "sub_requesting";
	case DDP_OVL1_STATUS_PRIMARY_RELEASED:
		return "primary_released";
	case DDP_OVL1_STATUS_PRIMARY_RELEASING:
		return "primary_releasing";
	case DDP_OVL1_STATUS_PRIMARY_DISABLE:
		return "primary_disabled";
	default:
		return "unknown";
	}
}

/* get ovl1 status */
enum DISP_OVL1_STATUS ovl_get_status(void)
{
	return ovl1_status;
}

/* set ovl1 status */
void ovl_set_status(enum DISP_OVL1_STATUS status)
{
	DDPMSG("cascade, set_ovl1 from %s to %s!\n",
			ovl_get_status_name(ovl1_status),
	       ovl_get_status_name(status));
	mmprofile_log_ex(ddp_mmp_get_events()->ovl1_status,
			MMPROFILE_FLAG_PULSE, ovl1_status, status);
	ovl1_status = status;	/* atomic operation */
}

static enum OVL_INPUT_FORMAT ovl_input_fmt_convert(enum DP_COLOR_ENUM fmt)
{
	enum OVL_INPUT_FORMAT ovl_fmt = OVL_INPUT_FORMAT_UNKNOWN;

	switch (fmt) {
	case eBGR565:
		ovl_fmt = OVL_INPUT_FORMAT_BGR565;
		break;
	case eRGB565:
		ovl_fmt = OVL_INPUT_FORMAT_RGB565;
		break;
	case eRGB888:
		ovl_fmt = OVL_INPUT_FORMAT_RGB888;
		break;
	case eBGR888:
		ovl_fmt = OVL_INPUT_FORMAT_BGR888;
		break;
	case eRGBA8888:
		ovl_fmt = OVL_INPUT_FORMAT_RGBA8888;
		break;
	case eBGRA8888:
		ovl_fmt = OVL_INPUT_FORMAT_BGRA8888;
		break;
	case eARGB8888:
		ovl_fmt = OVL_INPUT_FORMAT_ARGB8888;
		break;
	case eABGR8888:
		ovl_fmt = OVL_INPUT_FORMAT_ABGR8888;
		break;
	case eVYUY:
		ovl_fmt = OVL_INPUT_FORMAT_VYUY;
		break;
	case eYVYU:
		ovl_fmt = OVL_INPUT_FORMAT_YVYU;
		break;
	case eUYVY:
		ovl_fmt = OVL_INPUT_FORMAT_UYVY;
		break;
	case eYUY2:
		ovl_fmt = OVL_INPUT_FORMAT_YUYV;
		break;
	default:
		DDPERR("ovl_fmt_convert fmt=%d, ovl_fmt=%d\n", fmt, ovl_fmt);
		break;
	}
	return ovl_fmt;
}

static enum DP_COLOR_ENUM ovl_input_fmt(enum OVL_INPUT_FORMAT fmt, int swap)
{
	switch (fmt) {
	case OVL_INPUT_FORMAT_BGR565:
		return swap ? eBGR565 : eRGB565;
	case OVL_INPUT_FORMAT_RGB888:
		return swap ? eRGB888 : eBGR888;
	case OVL_INPUT_FORMAT_RGBA8888:
		return swap ? eRGBA8888 : eBGRA8888;
	case OVL_INPUT_FORMAT_ARGB8888:
		return swap ? eARGB8888 : eABGR8888;
	case OVL_INPUT_FORMAT_VYUY:
		return swap ? eVYUY : eUYVY;
	case OVL_INPUT_FORMAT_YVYU:
		return swap ? eYVYU : eYUY2;
	default:
		DDPERR("%s fmt=%d, swap=%d\n", __func__, fmt, swap);
		break;
	}
	return eRGB888;
}

static unsigned int ovl_input_fmt_byte_swap(enum OVL_INPUT_FORMAT fmt)
{
	int input_swap = 0;

	switch (fmt) {
	case OVL_INPUT_FORMAT_BGR565:
	case OVL_INPUT_FORMAT_RGB888:
	case OVL_INPUT_FORMAT_RGBA8888:
	case OVL_INPUT_FORMAT_ARGB8888:
	case OVL_INPUT_FORMAT_VYUY:
	case OVL_INPUT_FORMAT_YVYU:
		input_swap = 1;
		break;
	case OVL_INPUT_FORMAT_RGB565:
	case OVL_INPUT_FORMAT_BGR888:
	case OVL_INPUT_FORMAT_BGRA8888:
	case OVL_INPUT_FORMAT_ABGR8888:
	case OVL_INPUT_FORMAT_UYVY:
	case OVL_INPUT_FORMAT_YUYV:
		input_swap = 0;
		break;
	default:
		DDPERR("unknown input ovl format is %d\n", fmt);
		ASSERT(0);
	}
	return input_swap;
}

static unsigned int ovl_input_fmt_bpp(enum OVL_INPUT_FORMAT fmt)
{
	int bpp = 0;

	switch (fmt) {
	case OVL_INPUT_FORMAT_BGR565:
	case OVL_INPUT_FORMAT_RGB565:
	case OVL_INPUT_FORMAT_VYUY:
	case OVL_INPUT_FORMAT_UYVY:
	case OVL_INPUT_FORMAT_YVYU:
	case OVL_INPUT_FORMAT_YUYV:
		bpp = 2;
		break;
	case OVL_INPUT_FORMAT_RGB888:
	case OVL_INPUT_FORMAT_BGR888:
		bpp = 3;
		break;
	case OVL_INPUT_FORMAT_RGBA8888:
	case OVL_INPUT_FORMAT_BGRA8888:
	case OVL_INPUT_FORMAT_ARGB8888:
	case OVL_INPUT_FORMAT_ABGR8888:
		bpp = 4;
		break;
	default:
		DDPERR("unknown ovl input format = %d\n", fmt);
		ASSERT(0);
	}
	return bpp;
}

static enum OVL_COLOR_SPACE ovl_input_fmt_color_space(enum OVL_INPUT_FORMAT fmt)
{
	enum OVL_COLOR_SPACE space = OVL_COLOR_SPACE_RGB;

	switch (fmt) {
	case OVL_INPUT_FORMAT_BGR565:
	case OVL_INPUT_FORMAT_RGB565:
	case OVL_INPUT_FORMAT_RGB888:
	case OVL_INPUT_FORMAT_BGR888:
	case OVL_INPUT_FORMAT_RGBA8888:
	case OVL_INPUT_FORMAT_BGRA8888:
	case OVL_INPUT_FORMAT_ARGB8888:
	case OVL_INPUT_FORMAT_ABGR8888:
		space = OVL_COLOR_SPACE_RGB;
		break;
	case OVL_INPUT_FORMAT_VYUY:
	case OVL_INPUT_FORMAT_UYVY:
	case OVL_INPUT_FORMAT_YVYU:
	case OVL_INPUT_FORMAT_YUYV:
		space = OVL_COLOR_SPACE_YUV;
		break;
	default:
		DDPERR("unknown ovl input format = %d\n", fmt);
		ASSERT(0);
	}
	return space;
}

static unsigned int ovl_input_fmt_reg_value(enum OVL_INPUT_FORMAT fmt)
{
	int reg_value = 0;

	switch (fmt) {
	case OVL_INPUT_FORMAT_BGR565:
	case OVL_INPUT_FORMAT_RGB565:
		reg_value = 0x0;
		break;
	case OVL_INPUT_FORMAT_RGB888:
	case OVL_INPUT_FORMAT_BGR888:
		reg_value = 0x1;
		break;
	case OVL_INPUT_FORMAT_RGBA8888:
	case OVL_INPUT_FORMAT_BGRA8888:
		reg_value = 0x2;
		break;
	case OVL_INPUT_FORMAT_ARGB8888:
	case OVL_INPUT_FORMAT_ABGR8888:
		reg_value = 0x3;
		break;
	case OVL_INPUT_FORMAT_VYUY:
	case OVL_INPUT_FORMAT_UYVY:
		reg_value = 0x4;
		break;
	case OVL_INPUT_FORMAT_YVYU:
	case OVL_INPUT_FORMAT_YUYV:
		reg_value = 0x5;
		break;
	default:
		DDPERR("unknown ovl input format is %d\n", fmt);
		ASSERT(0);
	}
	return reg_value;
}

static char *ovl_intput_format_name(enum OVL_INPUT_FORMAT fmt, int swap)
{
	switch (fmt) {
	case OVL_INPUT_FORMAT_BGR565:
		return swap ? "eBGR565" : "eRGB565";
	case OVL_INPUT_FORMAT_RGB565:
		return "eRGB565";
	case OVL_INPUT_FORMAT_RGB888:
		return swap ? "eRGB888" : "eBGR888";
	case OVL_INPUT_FORMAT_BGR888:
		return "eBGR888";
	case OVL_INPUT_FORMAT_RGBA8888:
		return swap ? "eRGBA8888" : "eBGRA8888";
	case OVL_INPUT_FORMAT_BGRA8888:
		return "eBGRA8888";
	case OVL_INPUT_FORMAT_ARGB8888:
		return swap ? "eARGB8888" : "eABGR8888";
	case OVL_INPUT_FORMAT_ABGR8888:
		return "eABGR8888";
	case OVL_INPUT_FORMAT_VYUY:
		return swap ? "eVYUY" : "eUYVY";
	case OVL_INPUT_FORMAT_UYVY:
		return "eUYVY";
	case OVL_INPUT_FORMAT_YVYU:
		return swap ? "eYVYU" : "eYUY2";
	case OVL_INPUT_FORMAT_YUYV:
		return "eYUY2";
	default:
		DDPERR("ovl_intput_fmt unknown fmt=%d, swap=%d\n", fmt, swap);
		break;
	}
	return "unknown";
}

static unsigned int ovl_index(enum DISP_MODULE_ENUM module)
{
	int idx = 0;

	switch (module) {
	case DISP_MODULE_OVL0:
		idx = 0;
		break;
	case DISP_MODULE_OVL1:
		idx = 1;
		break;
	default:
		DDPERR("invalid module=%d\n", module);
		ASSERT(0);
	}
	return idx;
}

int ovl_start(enum DISP_MODULE_ENUM module, void *handle)
{
	int idx = ovl_index(module);
	int idx_offset = idx * DISP_OVL_INDEX_OFFSET;
	int enable_ovl_irq = 1;

	DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_EN, 0x01);

#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT) && \
	defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
	enable_ovl_irq = 1;
#else
	if (gEnableIRQ == 1)
		enable_ovl_irq = 1;
	else
		enable_ovl_irq = 0;
#endif

	if (enable_ovl_irq)
		DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_INTEN, 0x1e2);
	else
		DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_INTEN, 0x1e0);

	DISP_REG_SET_FIELD(handle, DATAPATH_CON_FLD_LAYER_SMI_ID_EN,
			   idx_offset + DISP_REG_OVL_DATAPATH_CON, 0x1);
	return 0;
}

int ovl_stop(enum DISP_MODULE_ENUM module, void *handle)
{
	int idx = ovl_index(module);
	int idx_offset = idx * DISP_OVL_INDEX_OFFSET;

	DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_INTEN, 0x00);
	DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_EN, 0x00);
	DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_INTSTA, 0x00);

	return 0;
}

int ovl_is_idle(enum DISP_MODULE_ENUM module)
{
	int idx = ovl_index(module);
	int idx_offset = idx * DISP_OVL_INDEX_OFFSET;

	if (((DISP_REG_GET(idx_offset + DISP_REG_OVL_FLOW_CTRL_DBG
						) & 0x3ff) != 0x1) &&
	    ((DISP_REG_GET(idx_offset + DISP_REG_OVL_FLOW_CTRL_DBG
			   ) & 0x3ff) != 0x2))
		return 0;
	else
		return 1;
}

int ovl_reset(enum DISP_MODULE_ENUM module, void *handle)
{
#define OVL_IDLE (0x3)
	int ret = 0;
	unsigned int delay_cnt = 0;
	int idx = ovl_index(module);
	int idx_offset = idx * DISP_OVL_INDEX_OFFSET;

	DISP_CPU_REG_SET(idx_offset + DISP_REG_OVL_RST, 0x1);
	DISP_CPU_REG_SET(idx_offset + DISP_REG_OVL_RST, 0x0);
	/*only wait if not cmdq */
	if (handle == NULL) {
		while (!(DISP_REG_GET(idx_offset + DISP_REG_OVL_FLOW_CTRL_DBG
						) & OVL_IDLE)) {
			delay_cnt++;
			udelay(10);
			if (delay_cnt > 2000) {
				DDPERR("ovl%d_reset timeout!\n", idx);
				ret = -1;
				break;
			}
		}
	}
	return ret;
}

int ovl_roi(enum DISP_MODULE_ENUM module,
	    unsigned int bg_w, unsigned int bg_h, unsigned int bg_color,
	    void *handle)
{
	int idx = ovl_index(module);
	int idx_offset = idx * DISP_OVL_INDEX_OFFSET;

	if ((bg_w > OVL_MAX_WIDTH) || (bg_h > OVL_MAX_HEIGHT)) {
		DDPERR("%s,exceed OVL max size, w=%d, h=%d\n",
				__func__, bg_w, bg_h);
		ASSERT(0);
	}

	DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_ROI_SIZE,
			bg_h << 16 | bg_w);
	DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_ROI_BGCLR, bg_color);
	DDPMSG("update ROI(%dx%d)\n", bg_w, bg_h);

	return 0;
}

int ovl_layer_switch(enum DISP_MODULE_ENUM module, unsigned int layer,
		unsigned int en, void *handle)
{
	int idx = ovl_index(module);
	int idx_offset = idx * DISP_OVL_INDEX_OFFSET;

	ASSERT(layer <= 3);
	/* DDPDBG("ovl%d,layer %d,enable %d\n", idx, layer, en); */

	switch (layer) {
	case 0:
		DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_RDMA0_CTRL, en);
		break;
	case 1:
		DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_RDMA1_CTRL, en);
		break;
	case 2:
		DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_RDMA2_CTRL, en);
		break;
	case 3:
		DISP_REG_SET(handle, idx_offset + DISP_REG_OVL_RDMA3_CTRL, en);
		break;
	default:
		DDPERR("invalid layer=%d\n", layer);
		ASSERT(0);
	}

	return 0;
}

int ovl_layer_config(enum DISP_MODULE_ENUM module,
			unsigned int layer,
			unsigned int source,
			enum DP_COLOR_ENUM format,
			unsigned long addr,
			unsigned int src_x,	/* ROI x offset */
			unsigned int src_y,	/* ROI y offset */
			unsigned int src_pitch,
			unsigned int dst_x,	/* ROI x offset */
			unsigned int dst_y,	/* ROI y offset */
			unsigned int dst_w,	/* ROT width */
			unsigned int dst_h,	/* ROI height */
			unsigned int key_en,
			unsigned int key,	/*color key */
			unsigned int aen,	/* alpha enable */
			unsigned char alpha,
			unsigned int sur_aen,
			unsigned int src_alpha,
			unsigned int dst_alpha,
			unsigned int constant_color,
			unsigned int yuv_range,
			enum DISP_BUFFER_TYPE sec, unsigned int is_engine_sec,
			void *handle)
{
	int idx = ovl_index(module);
	unsigned int value = 0;
	enum OVL_INPUT_FORMAT fmt = ovl_input_fmt_convert(format);
	unsigned int bpp = ovl_input_fmt_bpp(fmt);
	unsigned int input_swap = ovl_input_fmt_byte_swap(fmt);
	unsigned int input_fmt = ovl_input_fmt_reg_value(fmt);
	enum OVL_COLOR_SPACE space = ovl_input_fmt_color_space(fmt);
	unsigned int offset = 0;
	/*0100 MTX_JPEG_TO_RGB (YUV FUll TO RGB) */
	int color_matrix = 0x4;

	unsigned int idx_offset = idx * DISP_OVL_INDEX_OFFSET;
	unsigned int layer_offset = idx_offset + layer * OVL_LAYER_OFFSET;

#ifdef CONFIG_MTK_LCM_PHYSICAL_ROTATION_HW
	unsigned int bg_w, bg_h;
#endif

	if (format == eVYUY || format == eUYVY ||
		format == eYVYU || format == eYUY2) {
		unsigned int regval = 0;

		if (src_x % 2) {
			src_x -= 1; /* make src_x to even */
			dst_w += 1;
			regval |= REG_FLD_VAL(OVL_L_CLIP_FLD_LEFT, 1);
		}

		if ((src_x + dst_w) % 2) {
			dst_w += 1; /* make right boundary even */
			regval |= REG_FLD_VAL(OVL_L_CLIP_FLD_RIGHT, 1);
		}

		if (dst_x % 2)
			dst_x -= 1;

		DISP_REG_SET(handle,
				DISP_REG_OVL_L0_CLIP + layer_offset, regval);
	} else {
		DISP_REG_SET(handle, DISP_REG_OVL_L0_CLIP + layer_offset, 0);
	}

	switch (yuv_range) {
	case 0:
		color_matrix = 4;
		break;		/* BT601_full */
	case 1:
		color_matrix = 6;
		break;		/* BT601 */
	case 2:
		color_matrix = 7;
		break;		/* BT709 */
	default:
		DDPMSG("use default yuv_range=%d!\n", yuv_range);
		color_matrix = 4;
	}
	/* DDPMSG("color matrix=%d.\n", color_matrix); */

	ASSERT((dst_w <= OVL_MAX_WIDTH) && (dst_h <= OVL_MAX_HEIGHT) &&
			(layer <= 3));

	if (addr == 0) {
		DDPERR("source from memory, but addr is 0!\n");
		ASSERT(0);
	}

	DDPMSG("ovl%d,layer=%d source=%s off(x=%d, y=%d),dst(%d, %d, %d, %d)\n",
			idx, layer, (source == 0) ? "memory" : "dim",
			src_x, src_y, dst_x, dst_y, dst_w, dst_h);
	DDPMSG("                pitch=%d, fmt=%s, addr=0x%lx, sec = %d\n",
			src_pitch, ovl_intput_format_name(fmt, input_swap),
			addr, sec);

	/*
	 *DDPPRINT(
	 *"ovl%d, layer=%d, source=%s, off(x=%d, y=%d), dst(%d, %d, %d, %d),
	 *pitch=%d, fmt=%s,
	 *	addr=%lx, keyEn=%d, key=%d, aen=%d, alpha=%d,
	 *	sur_aen=%d,sur_alpha=0x%x, constant_color=0x%x, yuv_range=%d\n",
	 *       idx, layer, (source == 0) ? "memory" : "dim", src_x, src_y,
	 *       dst_x, dst_y,
	 *     dst_w, dst_h, src_pitch, ovl_intput_format_name(fmt, input_swap),
	 *       addr, key_en, key, aen, alpha, sur_aen,
	 *       dst_alpha << 2 | src_alpha, constant_color, yuv_range);
	 */

	if (source == OVL_LAYER_SOURCE_RESERVED) {
		if (aen == 0)
			DDPERR("dim layer ahpha enable should be 1!\n");

		if (fmt != OVL_INPUT_FORMAT_RGB565 &&
				fmt != OVL_INPUT_FORMAT_RGB888) {
			/* DDPERR("dim layer format should be RGB565"); */
			fmt = OVL_INPUT_FORMAT_RGB888;
			input_fmt = ovl_input_fmt_reg_value(fmt);
		}
		DISP_REG_SET(handle, DISP_REG_OVL_RDMA0_CTRL + layer_offset,
				0x0);
	} else {
		DISP_REG_SET(handle, DISP_REG_OVL_RDMA0_CTRL + layer_offset,
				0x1);
	}
	value = (REG_FLD_VAL((L_CON_FLD_LARC), (source)) |
		 REG_FLD_VAL((L_CON_FLD_CFMT), (input_fmt)) |
		 REG_FLD_VAL((L_CON_FLD_AEN), (aen)) |
		 REG_FLD_VAL((L_CON_FLD_APHA), (alpha)) |
		 REG_FLD_VAL((L_CON_FLD_SKEN), (key_en)) |
		 REG_FLD_VAL((L_CON_FLD_BTSW), (input_swap)));

#ifdef CONFIG_MTK_LCM_PHYSICAL_ROTATION_HW
	value = value | (REG_FLD_VAL((L_CON_FLD_VIRTICAL_FLIP), 1) |
			 REG_FLD_VAL((L_CON_FLD_HORI_FLIP), 1) |
			 REG_FLD_VAL((L_CON_FLD_BTSW), 1));
#endif

	if (space == OVL_COLOR_SPACE_YUV)
		value = value | REG_FLD_VAL((L_CON_FLD_MTX), (color_matrix));

	DISP_REG_SET(handle, DISP_REG_OVL_L0_CON + layer_offset, value);

	DISP_REG_SET(handle, DISP_REG_OVL_L0_CLR + idx_offset + layer * 4,
			constant_color);

	DISP_REG_SET(handle, DISP_REG_OVL_L0_SRC_SIZE + layer_offset,
			dst_h << 16 | dst_w);

#ifdef CONFIG_MTK_LCM_PHYSICAL_ROTATION_HW
	bg_h = DISP_REG_GET(idx_offset + DISP_REG_OVL_ROI_SIZE);
	bg_w = bg_h & 0xFFFF;
	bg_h = bg_h >> 16;
	DISP_REG_SET(handle, DISP_REG_OVL_L0_OFFSET + layer_offset,
		     (bg_h - dst_h - dst_y) << 16 | (bg_w - dst_w - dst_x));
	offset =
		(src_x + dst_w + 1) * bpp + (src_y + dst_h - 1) * src_pitch - 1;
#else

	DISP_REG_SET(handle, DISP_REG_OVL_L0_OFFSET + layer_offset,
			dst_y << 16 | dst_x);
	offset = src_x * bpp + src_y * src_pitch;
#endif

	if (!is_engine_sec) {
		DISP_REG_SET(handle, DISP_REG_OVL_L0_ADDR + layer_offset,
				addr + offset);
	} else {
		unsigned int size;
		int m4u_port;

		size = (dst_h - 1) * src_pitch + dst_w * bpp;
		m4u_port = M4U_PORT_DISP_OVL0;
		if (sec != DISP_SECURE_BUFFER) {
			/* ovl is sec but this layer is non-sec */
			/* we need to tell cmdq to help
			 * map non-sec mva to sec mva
			 */
			cmdqRecWriteSecure(handle,
					   disp_addr_convert(
						   DISP_REG_OVL_L0_ADDR +
						   layer_offset),
					   CMDQ_SAM_NMVA_2_MVA, addr + offset,
					   0, size, m4u_port);

		} else {
			/* for sec layer, addr variable stores sec handle */
			/* we need to pass this handle and offset
			 * to cmdq driver
			 */
			/* cmdq sec driver will help to convert handle
			 * to correct address
			 */
			offset = src_x * bpp + src_y * src_pitch;
			cmdqRecWriteSecure(handle,
					   disp_addr_convert(
						   DISP_REG_OVL_L0_ADDR +
						   layer_offset),
					   CMDQ_SAM_H_2_MVA, addr, offset,
					   size, m4u_port);
		}
	}
	DISP_REG_SET(handle, DISP_REG_OVL_L0_SRCKEY + layer_offset, key);

	value = (((sur_aen & 0x1) << 15) |
		 ((dst_alpha & 0x3) << 6) | ((dst_alpha & 0x3) << 4) |
		 ((src_alpha & 0x3) << 2) | (src_alpha & 0x3));
	value = (REG_FLD_VAL((L_PITCH_FLD_SUR_ALFA), (value)) |
		 REG_FLD_VAL((L_PITCH_FLD_LSP), (src_pitch)));

	DISP_REG_SET(handle, DISP_REG_OVL_L0_PITCH + layer_offset, value);

	DISP_REG_SET(handle, DISP_REG_OVL_RDMA0_MEM_GMC_SETTING+layer_offset,
			0x6070);

	return 0;
}

static void ovl_store_regs(enum DISP_MODULE_ENUM module)
{
	int i = 0;
	int idx = ovl_index(module);
	unsigned int idx_offset = idx * DISP_OVL_INDEX_OFFSET;
#if 0
	static const unsigned long regs[] = {
		DISP_REG_OVL_ROI_SIZE, DISP_REG_OVL_ROI_BGCLR,
	};
#else
	static unsigned long regs[3];

	regs[0] = DISP_REG_OVL_ROI_SIZE + idx_offset;
	regs[1] = DISP_REG_OVL_ROI_BGCLR + idx_offset;
	regs[2] = DISP_REG_OVL_DATAPATH_CON + idx_offset;
#endif

	reg_back_cnt[idx] = sizeof(regs) / sizeof(unsigned long);
	ASSERT(reg_back_cnt[idx] <= OVL_REG_BACK_MAX);


	for (i = 0; i < reg_back_cnt[idx]; i++) {
		reg_back[idx][i].address = regs[i];
		reg_back[idx][i].value = DISP_REG_GET(regs[i]);
	}
	DDPMSG("store %d cnt registers on ovl %d\n", reg_back_cnt[idx], idx);

}

static void ovl_restore_regs(enum DISP_MODULE_ENUM module, void *handle)
{
	int idx = ovl_index(module);
	int i = reg_back_cnt[idx];

	while (i > 0) {
		i--;
		DISP_REG_SET(handle, reg_back[idx][i].address,
				reg_back[idx][i].value);
	}
	DDPMSG("restore %d cnt registers on ovl %d\n", reg_back_cnt[idx], idx);
	reg_back_cnt[idx] = 0;
}

int ovl_clock_on(enum DISP_MODULE_ENUM module, void *handle)
{
	return 0;
}

int ovl_clock_off(enum DISP_MODULE_ENUM module, void *handle)
{
	return 0;
}

int ovl_resume(enum DISP_MODULE_ENUM module, void *handle)
{
	int idx = ovl_index(module);

	DDPMSG("ovl%d_resume\n", idx);
	ovl_restore_regs(module, handle);
	return 0;
}

int ovl_suspend(enum DISP_MODULE_ENUM module, void *handle)
{
	int idx = ovl_index(module);

	DDPMSG("ovl%d_suspend\n", idx);
	ovl_store_regs(module);
	return 0;
}

int ovl_init(enum DISP_MODULE_ENUM module, void *handle)
{
	ovl_clock_on(module, handle);
	return 0;
}

int ovl_deinit(enum DISP_MODULE_ENUM module, void *handle)
{
	ovl_clock_off(module, handle);
	return 0;
}

unsigned int ddp_ovl_get_cur_addr(bool rdma_mode, int layerid)
{
	if (rdma_mode)
		return DISP_REG_GET(DISP_REG_RDMA_MEM_START_ADDR);

#ifdef OVL_CASCADE_SUPPORT
	if (layerid < 4 && (ovl_get_status() == DDP_OVL1_STATUS_PRIMARY ||
			    ovl_get_status() ==
			    DDP_OVL1_STATUS_PRIMARY_RELEASING ||
			    ovl_get_status() ==
			    DDP_OVL1_STATUS_SUB_REQUESTING)) { /* OVL 1 */
		if (DISP_REG_GET(DISP_REG_OVL_RDMA0_CTRL +
					DISP_OVL_INDEX_OFFSET +
					layerid * 0x20) & 0x1)
			return DISP_REG_GET(DISP_REG_OVL_L0_ADDR +
					DISP_OVL_INDEX_OFFSET + layerid * 0x20);
		else
			return 0;
	} else { /* OVL 0 */
		if (DISP_REG_GET(DISP_REG_OVL_RDMA0_CTRL + (layerid % 4) *
					0x20) & 0x1)
			return DISP_REG_GET(DISP_REG_OVL_L0_ADDR +
					(layerid % 4) * 0x20);
		else
			return 0;
	}
#else
	if (DISP_REG_GET(DISP_REG_OVL_RDMA0_CTRL + layerid * 0x20) & 0x1)
		return DISP_REG_GET(DISP_REG_OVL_L0_ADDR + layerid * 0x20);
	else
		return 0;
#endif
}

void ovl_get_address(enum DISP_MODULE_ENUM module, unsigned long *add)
{
	int i = 0;
	int idx = ovl_index(module);
	unsigned int idx_offset = idx * DISP_OVL_INDEX_OFFSET;
	unsigned int layer_off = 0;
	unsigned int src_on = DISP_REG_GET(DISP_REG_OVL_SRC_CON + idx_offset);

	for (i = 0; i < 4; i++) {
		layer_off = i * OVL_LAYER_OFFSET + idx_offset;
		if (src_on & (0x1 << i))
			add[i] = DISP_REG_GET(layer_off + DISP_REG_OVL_L0_ADDR);
		else
			add[i] = 0;

	}
}

void ovl_get_info(int idx, void *data)
{
	int i = 0;
	struct OVL_BASIC_STRUCT *pdata = (struct OVL_BASIC_STRUCT *) data;
	unsigned int idx_offset = idx * DISP_OVL_INDEX_OFFSET;
	unsigned int layer_off = 0;
	unsigned int src_on = DISP_REG_GET(DISP_REG_OVL_SRC_CON + idx_offset);
	struct OVL_BASIC_STRUCT *p = NULL;

	memset(pdata, 0, sizeof(struct OVL_BASIC_STRUCT) * OVL_LAYER_NUM);

	if (ovl_get_status() == DDP_OVL1_STATUS_SUB ||
			ovl_get_status() == DDP_OVL1_STATUS_IDLE) {
		/* 4+4 mode */
		/* idx=0; */
		idx_offset = idx * DISP_OVL_INDEX_OFFSET;
		src_on = DISP_REG_GET(DISP_REG_OVL_SRC_CON + idx_offset);
		for (i = 0; i < OVL_LAYER_NUM_PER_OVL; i++) {
			layer_off = (i % 4) * OVL_LAYER_OFFSET + idx_offset;
			p = &pdata[i];
			p->layer = i;
			p->layer_en = src_on & (0x1 << i);
			if (p->layer_en) {
				p->fmt = (unsigned int)ovl_input_fmt(
						DISP_REG_GET_FIELD(
							L_CON_FLD_CFMT,
							layer_off +
							DISP_REG_OVL_L0_CON),
						DISP_REG_GET_FIELD(
							L_CON_FLD_BTSW,
							layer_off +
							DISP_REG_OVL_L0_CON));
				p->addr = DISP_REG_GET(layer_off +
						DISP_REG_OVL_L0_ADDR);
				p->src_w = DISP_REG_GET(layer_off +
						DISP_REG_OVL_L0_SRC_SIZE
						) & 0xfff;
				p->src_h =
					(DISP_REG_GET(layer_off +
						      DISP_REG_OVL_L0_SRC_SIZE
						      ) >> 16) & 0xfff;
				p->src_pitch =
					DISP_REG_GET(layer_off +
							DISP_REG_OVL_L0_PITCH
							) & 0xffff;
				p->bpp = ovl_input_fmt_bpp(
						DISP_REG_GET_FIELD(
							L_CON_FLD_CFMT,
							layer_off +
							DISP_REG_OVL_L0_CON));
			}
			DDPMSG("%s:layer%d,en %d,w %d,h %d,bpp %d,addr %lu\n",
					__func__, i, p->layer_en, p->src_w,
					p->src_h, p->bpp, p->addr);
		}
	} else {

		idx = 0;
		idx_offset = idx * DISP_OVL_INDEX_OFFSET;
		src_on = DISP_REG_GET(DISP_REG_OVL_SRC_CON + idx_offset);
		for (i = OVL_LAYER_NUM_PER_OVL; i < OVL_LAYER_NUM; i++) {
			layer_off = (i % 4) * OVL_LAYER_OFFSET + idx_offset;
			p = &pdata[i];
			p->layer = i;
			p->layer_en = src_on & (0x1 << (i % 4));
			if (p->layer_en) {
				p->fmt = (unsigned int)ovl_input_fmt(
						DISP_REG_GET_FIELD(
							L_CON_FLD_CFMT,
							layer_off +
							DISP_REG_OVL_L0_CON),
						DISP_REG_GET_FIELD(
							L_CON_FLD_BTSW,
							layer_off +
							DISP_REG_OVL_L0_CON));
				p->addr =
					DISP_REG_GET(layer_off +
							DISP_REG_OVL_L0_ADDR);
				p->src_w =
					DISP_REG_GET(layer_off +
						DISP_REG_OVL_L0_SRC_SIZE
						) & 0xfff;
				p->src_h = (DISP_REG_GET(layer_off +
						DISP_REG_OVL_L0_SRC_SIZE) >>
						16) & 0xfff;
				p->src_pitch =
					DISP_REG_GET(layer_off +
							DISP_REG_OVL_L0_PITCH
							) & 0xffff;
				p->bpp = ovl_input_fmt_bpp(DISP_REG_GET_FIELD(
							L_CON_FLD_CFMT,
							layer_off +
							DISP_REG_OVL_L0_CON));
			}
			DDPMSG("%s:layer%d,en %d,w %d,h %d,bpp %d,addr %lu\n",
					__func__, i, p->layer_en, p->src_w,
					p->src_h, p->bpp, p->addr);
		}


		idx = 1;
		idx_offset = idx * DISP_OVL_INDEX_OFFSET;
		src_on = DISP_REG_GET(DISP_REG_OVL_SRC_CON + idx_offset);
		for (i = 0; i < OVL_LAYER_NUM_PER_OVL; i++) {
			layer_off = (i % 4) * OVL_LAYER_OFFSET + idx_offset;
			p = &pdata[i];
			p->layer = i;
			p->layer_en = src_on & (0x1 << (i % 4));
			if (p->layer_en) {
				p->fmt = (unsigned int)ovl_input_fmt(
						DISP_REG_GET_FIELD(
							L_CON_FLD_CFMT,
							layer_off +
							DISP_REG_OVL_L0_CON),
						DISP_REG_GET_FIELD(
							L_CON_FLD_BTSW,
							layer_off +
							DISP_REG_OVL_L0_CON));
				p->addr = DISP_REG_GET(layer_off +
						DISP_REG_OVL_L0_ADDR);
				p->src_w = DISP_REG_GET(layer_off +
						DISP_REG_OVL_L0_SRC_SIZE
						) & 0xfff;
				p->src_h = (DISP_REG_GET(layer_off +
							DISP_REG_OVL_L0_SRC_SIZE
							) >> 16) & 0xfff;
				p->src_pitch = DISP_REG_GET(layer_off +
						DISP_REG_OVL_L0_PITCH) & 0xffff;
				p->bpp = ovl_input_fmt_bpp(DISP_REG_GET_FIELD(
							L_CON_FLD_CFMT,
							layer_off +
							DISP_REG_OVL_L0_CON));
			}
			DDPMSG("%s:layer%d,en %d,w %d,h %d,bpp %d,addr %lu\n",
					__func__, i, p->layer_en, p->src_w,
					p->src_h, p->bpp, p->addr);
		}
	}
}

static int ovl_check_input_param(struct OVL_CONFIG_STRUCT *config)
{
	if ((config->addr == 0 && config->source == 0) || config->dst_w == 0 ||
			config->dst_h == 0) {
		DDPERR("ovl parameter invalidate, addr=%lx, w=%d, h=%d\n",
		       config->addr, config->dst_w, config->dst_h);
		ASSERT(0);
		return -1;
	}

	return 0;
}

void ovl_reset_by_cmdq(void *handle, enum DISP_MODULE_ENUM module)
{
	/* warm reset ovl every time we use it */
	if (handle) {
		if ((primary_display_is_decouple_mode() == 1) ||
		    (primary_display_is_video_mode() == 0)) {
			unsigned int offset;

			offset = ovl_index(module) * DISP_OVL_INDEX_OFFSET;
			DISP_REG_SET(handle, DISP_REG_OVL_RST + offset, 0x1);
			DISP_REG_SET(handle, DISP_REG_OVL_RST + offset, 0x0);
			cmdqRecPoll(handle, disp_addr_convert(DISP_REG_OVL_STA +
						offset), 0, 0x1);
		}
	}
}

#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT
static int ovl_get_sec_mode(enum DISP_MODULE_ENUM module)
{
	enum CMDQ_DISP_MODE mode = CMDQ_DISP_NON_SUPPORTED_MODE;

	if (primary_display_is_decouple_mode())
		mode = CMDQ_DISP_SINGLE_MODE;
	else
		mode = CMDQ_DISP_VIDEO_MODE;

	return mode;
}
#endif

static int ovl_is_sec[2];
static int ovl_config_l(enum DISP_MODULE_ENUM module,
		struct disp_ddp_path_config *pConfig, void *handle)
{
	int i = 0;
	int ovl_idx = ovl_index(module);
	int enabled_layers = 0;
	unsigned int offset;
	int has_sec_layer = 0;
	enum CMDQ_ENG_ENUM cmdq_engine;
#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT
	enum CMDQ_DISP_MODE mode = ovl_get_sec_mode(module);
#endif

	if (pConfig->dst_dirty)
		ovl_roi(module, pConfig->dst_w, pConfig->dst_h,
				gOVLBackground, handle);

	if (!pConfig->ovl_dirty)
		return 0;

	/* check if we has sec layer */
	for (i = 0; i < OVL_LAYER_NUM; i++) {
		if (pConfig->ovl_config[i].layer_en &&
		    (pConfig->ovl_config[i].security == DISP_SECURE_BUFFER))
			has_sec_layer = 1;
	}

	if (force_ovl_sec)
		has_sec_layer = 1;

	/* warm reset ovl every time we use it */
	if (handle
	    && ((primary_display_is_decouple_mode() == 1)
		|| (primary_display_is_video_mode() == 0))) {
		offset = ovl_index(module) * DISP_OVL_INDEX_OFFSET;
		DISP_REG_SET(handle, DISP_REG_OVL_RST + offset, 0x1);
		DISP_REG_SET(handle, DISP_REG_OVL_RST + offset, 0x0);
		cmdqRecPoll(handle,
				disp_addr_convert(DISP_REG_OVL_STA + offset),
				0, 0x1);
		DDPMSG("ovl%d warm reset done\n", ovl_idx);
	}

	cmdq_engine = CMDQ_ENG_DISP_OVL0;

	if (has_sec_layer) {
		cmdqRecSetSecure(handle, 1);
		/* set engine as sec */
		cmdqRecSecureEnablePortSecurity(handle, (1LL << cmdq_engine));
#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT
		cmdqRecSetSecureMode(handle, mode);
#endif
		/* cmdqRecSecureEnableDAPC(handle, (1LL << cmdq_engine)); */
		if (ovl_is_sec[ovl_idx] == 0) {
			pr_info("[SVP] switch ovl%d to sec\n", ovl_idx);
			ovl_is_sec[ovl_idx] = 1;
		}
	} else {
		if (ovl_is_sec[ovl_idx] == 1) {
			/* ovl is in sec stat, we need to switch it to nonsec */
			struct cmdqRecStruct *nonsec_switch_handle;
			int ret;

			/* ovl->wdma use CMDQ_SCENARIO_PRIMARY_MEMOUT, */
			/* the secure hw thread will be
			 * CMDQ_THREAD_SEC_SUB_DISP,
			 */
			/* disable secure path should use the same hw thread. */
			if (primary_display_is_decouple_mode())
				ret = cmdqRecCreate(
				CMDQ_SCENARIO_DISP_SUB_DISABLE_SECURE_PATH,
					&(nonsec_switch_handle));
			else
				ret = cmdqRecCreate(
				CMDQ_SCENARIO_DISP_PRIMARY_DISABLE_SECURE_PATH,
					&(nonsec_switch_handle));
			if (ret)
				DDPAEE("[SVP]err2create handle %s ret=%d\n",
				       __func__, ret);

			cmdqRecReset(nonsec_switch_handle);
			if (primary_display_is_decouple_mode())
				cmdqRecWaitNoClear(nonsec_switch_handle,
						CMDQ_EVENT_DISP_WDMA0_EOF);
			else
				_cmdq_insert_wait_frame_done_token_mira(
						nonsec_switch_handle);
			cmdqRecSetSecure(nonsec_switch_handle, 1);
#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT
			cmdqRecSetSecureMode(nonsec_switch_handle, mode);
#endif

			/* we should disable ovl before new (nonsec)
			 * setting take effect
			 */
			 /* or translation fault may happen,*/
			 /* if we switch ovl to nonsec BUT
			  * its setting is still sec
			  */
			/*to fix m4u translation fault*/
			for (i = 0; i < OVL_LAYER_NUM; i++) {
				if (pConfig->ovl_config[i].layer_en != 0) {
					if (ovl_check_input_param(
						&pConfig->ovl_config[i]))
						continue;

				ovl_layer_config(module, i,
						pConfig->ovl_config[i].source,
					pConfig->ovl_config[i].fmt,
					pConfig->ovl_config[i].addr,
					pConfig->ovl_config[i].src_x,
					pConfig->ovl_config[i].src_y,
					pConfig->ovl_config[i].src_pitch,
					pConfig->ovl_config[i].dst_x,
					pConfig->ovl_config[i].dst_y,
					pConfig->ovl_config[i].dst_w,
					pConfig->ovl_config[i].dst_h,
					pConfig->ovl_config[i].keyEn,
					pConfig->ovl_config[i].key,
					pConfig->ovl_config[i].aen,
					pConfig->ovl_config[i].alpha,
					pConfig->ovl_config[i].sur_aen,
					pConfig->ovl_config[i].src_alpha,
					pConfig->ovl_config[i].dst_alpha,
					0xff000000,	/* constant_color */
					pConfig->ovl_config[i].yuv_range,
					pConfig->ovl_config[i].security, 1,
					nonsec_switch_handle);
				}
				ovl_layer_switch(module, i,
						pConfig->ovl_config[i].layer_en,
						nonsec_switch_handle);

				if (pConfig->ovl_config[i].layer_en == 1)
					enabled_layers |= (1 << i);
			}
			DISP_REG_SET(nonsec_switch_handle,
		     DISP_REG_OVL_SRC_CON + (module - DISP_MODULE_OVL0) *
		     DISP_OVL_INDEX_OFFSET,
		     enabled_layers);
			enabled_layers = 0;
			/*in fact, dapc/port_sec will be disabled by cmdq */
			cmdqRecSecureEnablePortSecurity(nonsec_switch_handle,
					(1LL << cmdq_engine));
			cmdqRecFlushAsync(nonsec_switch_handle);
			cmdqRecDestroy(nonsec_switch_handle);
			pr_info("[SVP] switch ovl%d to nonsec\n",
					ovl_idx);
			ovl_is_sec[ovl_idx] = 0;
		}
	}

	for (i = 0; i < OVL_LAYER_NUM; i++) {
		if (pConfig->ovl_config[i].layer_en != 0) {
			if (ovl_check_input_param(&pConfig->ovl_config[i]))
				continue;

			ovl_layer_config(module,
					i,
					pConfig->ovl_config[i].source,
					pConfig->ovl_config[i].fmt,
					pConfig->ovl_config[i].addr,
					pConfig->ovl_config[i].src_x,
					pConfig->ovl_config[i].src_y,
					pConfig->ovl_config[i].src_pitch,
					pConfig->ovl_config[i].dst_x,
					pConfig->ovl_config[i].dst_y,
					pConfig->ovl_config[i].dst_w,
					pConfig->ovl_config[i].dst_h,
					pConfig->ovl_config[i].keyEn,
					pConfig->ovl_config[i].key,
					pConfig->ovl_config[i].aen,
					pConfig->ovl_config[i].alpha,
					pConfig->ovl_config[i].sur_aen,
					pConfig->ovl_config[i].src_alpha,
					pConfig->ovl_config[i].dst_alpha,
					0xff000000,
					pConfig->ovl_config[i].yuv_range,
					pConfig->ovl_config[i].security,
					has_sec_layer, handle);

		}
		if (pConfig->ovl_config[i].layer_en)
			enabled_layers |= (1 << i);
	}

	DISP_REG_SET(handle, ovl_idx * DISP_OVL_INDEX_OFFSET +
			DISP_REG_OVL_SRC_CON,
		     enabled_layers);
	return 0;
}

int ovl_build_cmdq(enum DISP_MODULE_ENUM module, void *cmdq_trigger_handle,
		enum CMDQ_STATE state)
{
	int ret = 0;

	if (cmdq_trigger_handle == NULL) {
		DDPERR("cmdq_trigger_handle is NULL\n");
		return -1;
	}

	if (primary_display_is_video_mode() == 1 &&
			primary_display_is_decouple_mode() == 0) {
		if (state == CMDQ_CHECK_IDLE_AFTER_STREAM_EOF) {
			if (module == DISP_MODULE_OVL0) {
				ret = cmdqRecPoll(cmdq_trigger_handle,
						DISP_REG_OVL0_STATE_PA,
						2, 0x3ff);
			} else {
				DDPERR("wrong module: %s\n",
						ddp_get_module_name(module));
				return -1;
			}
		}
	}

	return 0;
}


/***************** ovl debug info ************/

void ovl_dump_reg(enum DISP_MODULE_ENUM module)
{
	int idx = ovl_index(module);
	unsigned int offset = idx * DISP_OVL_INDEX_OFFSET;
	unsigned int src_on = DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset);

	DDPDUMP("== DISP OVL%d REGS ==\n", idx);
	DDPDUMP("OVL:0x000=0x%08x,0x004=0x%08x,0x008=0x%08x,0x00c=0x%08x\n",
		DISP_REG_GET(DISP_REG_OVL_STA + offset),
		DISP_REG_GET(DISP_REG_OVL_INTEN + offset),
		DISP_REG_GET(DISP_REG_OVL_INTSTA + offset),
		DISP_REG_GET(DISP_REG_OVL_EN + offset));
	DDPDUMP("OVL:0x010=0x%08x,0x014=0x%08x,0x020=0x%08x,0x024=0x%08x\n",
		DISP_REG_GET(DISP_REG_OVL_TRIG + offset),
		DISP_REG_GET(DISP_REG_OVL_RST + offset),
		DISP_REG_GET(DISP_REG_OVL_ROI_SIZE + offset),
		DISP_REG_GET(DISP_REG_OVL_DATAPATH_CON + offset));
	DDPDUMP("OVL:0x028=0x%08x,0x02c=0x%08x\n",
		DISP_REG_GET(DISP_REG_OVL_ROI_BGCLR + offset),
		DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset));

	if (src_on & 0x1) {
		DDPDUMP("OVL:0x30=0x%08x,0x34=0x%08x,0x38=0x%08x,0x3c=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_L0_CON + offset),
			DISP_REG_GET(DISP_REG_OVL_L0_SRCKEY + offset),
			DISP_REG_GET(DISP_REG_OVL_L0_SRC_SIZE + offset),
			DISP_REG_GET(DISP_REG_OVL_L0_OFFSET + offset));

		DDPDUMP("OVL:0xf40=0x%08x,0x44=0x%08x,0xc0=0x%08x,",
			DISP_REG_GET(DISP_REG_OVL_L0_ADDR + offset),
			DISP_REG_GET(DISP_REG_OVL_L0_PITCH + offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA0_CTRL + offset));
		DDPDUMP("0xc8=0x%08x,0xd0=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING +
				offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA0_FIFO_CTRL + offset));

		DDPDUMP("OVL:0x1e0=0x%08x,0x24c=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_GMC_S2 + offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA0_DBG + offset));
	}
	if (src_on & 0x2) {
		DDPDUMP("OVL:0x50=0x%08x,0x54=0x%08x,0x58=0x%08x,0x5c=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_L1_CON + offset),
			DISP_REG_GET(DISP_REG_OVL_L1_SRCKEY + offset),
			DISP_REG_GET(DISP_REG_OVL_L1_SRC_SIZE + offset),
			DISP_REG_GET(DISP_REG_OVL_L1_OFFSET + offset));

		DDPDUMP("OVL:0xf60=0x%08x,0x064=0x%08x,0x0e0=0x%08x,",
			DISP_REG_GET(DISP_REG_OVL_L1_ADDR + offset),
			DISP_REG_GET(DISP_REG_OVL_L1_PITCH + offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA1_CTRL + offset));
		DDPDUMP("0x0e8=0x%08x,0x0f0=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING +
				offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA1_FIFO_CTRL + offset));

		DDPDUMP("OVL:0x1e4=0x%08x,0x250=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_GMC_S2 + offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA1_DBG + offset));
	}
	if (src_on & 0x4) {
		DDPDUMP("OVL:0x70=0x%08x,0x74=0x%08x,0x78=0x%08x,0x7c=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_L2_CON + offset),
			DISP_REG_GET(DISP_REG_OVL_L2_SRCKEY + offset),
			DISP_REG_GET(DISP_REG_OVL_L2_SRC_SIZE + offset),
			DISP_REG_GET(DISP_REG_OVL_L2_OFFSET + offset));

		DDPDUMP("OVL:0xf80=0x%08x,0x084=0x%08x,0x100=0x%08x,",
			DISP_REG_GET(DISP_REG_OVL_L2_ADDR + offset),
			DISP_REG_GET(DISP_REG_OVL_L2_PITCH + offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA2_CTRL + offset));
		DDPDUMP("0x110=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_RDMA2_FIFO_CTRL + offset));

		DDPDUMP("OVL:0x1e8=0x%08x,0x254=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_RDMA2_MEM_GMC_S2 + offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA2_DBG + offset));
	}
	if (src_on & 0x8) {
		DDPDUMP("OVL:0x90=0x%08x,0x94=0x%08x,0x98=0x%08x,0x9c=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_L3_CON + offset),
			DISP_REG_GET(DISP_REG_OVL_L3_SRCKEY + offset),
			DISP_REG_GET(DISP_REG_OVL_L3_SRC_SIZE + offset),
			DISP_REG_GET(DISP_REG_OVL_L3_OFFSET + offset));

		DDPDUMP("OVL:0xfa0=0x%08x,0x0a4=0x%08x,0x120=0x%08x,",
			DISP_REG_GET(DISP_REG_OVL_L3_ADDR + offset),
			DISP_REG_GET(DISP_REG_OVL_L3_PITCH + offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA3_CTRL + offset));
		DDPDUMP("0x130=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_RDMA3_FIFO_CTRL + offset));

		DDPDUMP("OVL:0x1ec=0x%08x,0x258=0x%08x\n",
			DISP_REG_GET(DISP_REG_OVL_RDMA3_MEM_GMC_S2 + offset),
			DISP_REG_GET(DISP_REG_OVL_RDMA3_DBG + offset));
	}
	DDPDUMP("OVL:0x1d4=0x%08x,0x200=0x%08x,0x240=0x%08x,0x244=0x%08x\n",
		DISP_REG_GET(DISP_REG_OVL_DEBUG_MON_SEL + offset),
		DISP_REG_GET(DISP_REG_OVL_DUMMY_REG + offset),
		DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG + offset),
		DISP_REG_GET(DISP_REG_OVL_ADDCON_DBG + offset));
	DDPDUMP("OVL:0x25c=0x%08x,0x260=0x%08x,0x264=0x%08x,0x268=0x%08x\n",
		DISP_REG_GET(DISP_REG_OVL_L0_CLR + offset),
		DISP_REG_GET(DISP_REG_OVL_L1_CLR + offset),
		DISP_REG_GET(DISP_REG_OVL_L2_CLR + offset),
		DISP_REG_GET(DISP_REG_OVL_L3_CLR + offset));
}

static char *ovl_get_state_name(unsigned int status)
{
	switch (status & 0x3ff) {
	case 0x1:
		return "idle";
	case 0x2:
		return "wait_SOF";
	case 0x4:
		return "prepare";
	case 0x8:
		return "reg_update";
	case 0x10:
		return "eng_clr(internal reset)";
	case 0x20:
		return "eng_act(processing)";
	case 0x40:
		return "h_wait_w_rst";
	case 0x80:
		return "s_wait_w_rst";
	case 0x100:
		return "h_w_rst";
	case 0x200:
		return "s_w_rst";
	default:
		return "ovl_fsm_unknown";
	}
}

static void ovl_printf_status(int idx, unsigned int status)
{
#if 0
	DDPDUMP("=OVL%d_FLOW_CONTROL_DEBUG=:\n", idx);
	DDPDUMP("addcon_idle:%d,blend_idle:%d,out_valid:%d,out_ready:%d,",
		(status >> 10) & (0x1),
		(status >> 11) & (0x1),
		(status >> 12) & (0x1),
		(status >> 13) & (0x1));
	DDPDUMP("out_idle:%d\n", (status >> 15) & (0x1));
	DDPDUMP("rdma3_idle:%d,rdma2_idle:%d,rdma1_idle:%d,",
		(status >> 16) & (0x1),
		(status >> 17) & (0x1),
		(status >> 18) & (0x1));
	DDPDUMP(" rdma0_idle:%d,rst:%d\n",
		(status >> 19) & (0x1), (status >> 20) & (0x1));
	DDPDUMP("trig:%d,frame_hwrst_done:%d,frame_swrst_done:%d,",
		(status >> 21) & (0x1),
		(status >> 23) & (0x1),
		(status >> 24) & (0x1));
	DDPDUMP("frame_underrun:%d,frame_done:%d\n",
		(status >> 25) & (0x1), (status >> 26) & (0x1));
	DDPDUMP("ovl_running:%d,ovl_start:%d,ovl_clr:%d,",
		(status >> 27) & (0x1),
		(status >> 28) & (0x1),
		(status >> 29) & (0x1));
	DDPDUMP("reg_update:%d,ovl_upd_reg:%d\n",
		(status >> 30) & (0x1), (status >> 31) & (0x1));
#endif

	DDPDUMP("ovl%d, state=0x%x, fms_state:%s\n",
			idx, status, ovl_get_state_name(status));
}

void ovl_dump_analysis(enum DISP_MODULE_ENUM module)
{
	int i = 0;
	unsigned int layer_offset = 0;
	unsigned int rdma_offset = 0;
	int index = ovl_index(module);
	unsigned int offset = index * DISP_OVL_INDEX_OFFSET;
	unsigned int src_on = DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset);

	DDPDUMP("==DISP OVL%d ANALYSIS==\n", index);
	DDPDUMP("ovl_en=%d,lay_en(%d,%d,%d,%d),bg(w=%d,h=%d),pos(x=%d,y=%d)\n",
		DISP_REG_GET(DISP_REG_OVL_EN + offset),
		DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset) & 0x1,
		(DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset) >> 1) & 0x1,
		(DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset) >> 2) & 0x1,
		(DISP_REG_GET(DISP_REG_OVL_SRC_CON + offset) >> 3) & 0x1,
		DISP_REG_GET(DISP_REG_OVL_ROI_SIZE + offset) & 0xfff,
		(DISP_REG_GET(DISP_REG_OVL_ROI_SIZE + offset) >> 16) & 0xfff,
		DISP_REG_GET_FIELD(ADDCON_DBG_FLD_ROI_X,
			DISP_REG_OVL_ADDCON_DBG + offset),
		DISP_REG_GET_FIELD(ADDCON_DBG_FLD_ROI_Y,
			DISP_REG_OVL_ADDCON_DBG + offset));
	DDPDUMP("layer_hit(%d,%d,%d,%d), ovl_complete_cnt=(%d, %d)\n",
		DISP_REG_GET_FIELD(ADDCON_DBG_FLD_L0_WIN_HIT,
			DISP_REG_OVL_ADDCON_DBG + offset),
		DISP_REG_GET_FIELD(ADDCON_DBG_FLD_L1_WIN_HIT,
			DISP_REG_OVL_ADDCON_DBG + offset),
		DISP_REG_GET_FIELD(ADDCON_DBG_FLD_L2_WIN_HIT,
			DISP_REG_OVL_ADDCON_DBG + offset),
		DISP_REG_GET_FIELD(ADDCON_DBG_FLD_L3_WIN_HIT,
			DISP_REG_OVL_ADDCON_DBG + offset),
		ovl_complete_irq_cnt[0], ovl_complete_irq_cnt[1]);

	for (i = 0; i < 4; i++) {
		layer_offset = i * OVL_LAYER_OFFSET + offset;
		rdma_offset = i * OVL_RDMA_DEBUG_OFFSET + offset;
		if (src_on & (0x1 << i)) {
			DDPDUMP("layer%d: w=%d,h=%d,off(x=%d,y=%d),pitch=%d,",
			i,
			DISP_REG_GET(layer_offset +
				DISP_REG_OVL_L0_SRC_SIZE) & 0xfff,
			(DISP_REG_GET(layer_offset +
				      DISP_REG_OVL_L0_SRC_SIZE) >> 16) & 0xfff,
			DISP_REG_GET(layer_offset +
				DISP_REG_OVL_L0_OFFSET) & 0xfff,
			(DISP_REG_GET(layer_offset +
				      DISP_REG_OVL_L0_OFFSET) >> 16) & 0xfff,
			DISP_REG_GET(layer_offset +
				DISP_REG_OVL_L0_PITCH) & 0xffff);
			DDPDUMP("addr=0x%x,fmt=%s,source=%s,\n",
			DISP_REG_GET(layer_offset + DISP_REG_OVL_L0_ADDR),
			ovl_intput_format_name(DISP_REG_GET_FIELD(
				L_CON_FLD_CFMT, DISP_REG_OVL_L0_CON +
				layer_offset),
				DISP_REG_GET_FIELD(L_CON_FLD_BTSW,
					DISP_REG_OVL_L0_CON + layer_offset)),
			(DISP_REG_GET_FIELD(L_CON_FLD_LARC,
					    DISP_REG_OVL_L0_CON +
					    layer_offset) == 0) ?
			"memory" : "constant_color");
			DDPDUMP("aen=%d,alpha=%d\n",
				DISP_REG_GET_FIELD(L_CON_FLD_AEN,
					DISP_REG_OVL_L0_CON + layer_offset),
				DISP_REG_GET_FIELD(L_CON_FLD_APHA,
					DISP_REG_OVL_L0_CON + layer_offset));

			DDPDUMP("ovl rdma%d status:(en %d)\n", i,
					DISP_REG_GET(layer_offset +
						DISP_REG_OVL_RDMA0_CTRL));

		}
	}
	ovl_printf_status(index,
			DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG + offset));
}

int ovl_dump(enum DISP_MODULE_ENUM module, int level)
{
	ovl_dump_analysis(module);
	ovl_dump_reg(module);

	return 0;
}

static int ovl_io(enum DISP_MODULE_ENUM module, unsigned int msg,
		unsigned long arg, void *cmdq)
{
	int ret = 0;

	if (module != DISP_MODULE_OVL0 && module != DISP_MODULE_OVL1) {
		DDPDUMP("error, %s unknown module=%d\n", __func__, module);
		ret = -1;
		return ret;
	}

	switch (msg) {
	case DISP_IOCTL_OVL_ENABLE_CASCADE:
		DDPDUMP("enable cascade ovl1\n");
		DISP_REG_SET_FIELD(cmdq, DATAPATH_CON_FLD_BGCLR_IN_SEL,
				   DISP_REG_OVL_DATAPATH_CON, 1);

		/* when add ovl1 to path, dst_dirty may be not 1,
		 * so we have to call ovl_roi
		 */
		ovl_roi(DISP_MODULE_OVL1,
				DISP_REG_GET(DISP_REG_OVL_ROI_SIZE) & 0xfff,
			(DISP_REG_GET(DISP_REG_OVL_ROI_SIZE) >> 16) & 0xfff,
			gOVLBackground, cmdq);
		/* ovl1_status = DDP_OVL1_STATUS_PRIMARY; */
		break;
	case DISP_IOCTL_OVL_DISABLE_CASCADE:
		DDPDUMP("disable cascade ovl1\n");
		DISP_REG_SET_FIELD(cmdq, DATAPATH_CON_FLD_BGCLR_IN_SEL,
				DISP_REG_OVL_DATAPATH_CON, 0);
		/* ovl1_status = DDP_OVL1_STATUS_IDLE; */
		break;
	default:
		DDPDUMP("error: unknown cmd=%d\n", msg);
	}

	return ret;
}

/***************** driver************/
struct DDP_MODULE_DRIVER ddp_driver_ovl = {
	.init = ovl_init,
	.deinit = ovl_deinit,
	.config = ovl_config_l,
	.start = ovl_start,
	.trigger = NULL,
	.stop = ovl_stop,
	.reset = ovl_reset,
	.power_on = ovl_clock_on,
	.power_off = ovl_clock_off,
	.suspend = ovl_suspend,
	.resume = ovl_resume,
	.is_idle = NULL,
	.is_busy = NULL,
	.dump_info = ovl_dump,
	.bypass = NULL,
	.build_cmdq = NULL,
	.set_lcm_utils = NULL,
	.cmd = ovl_io,
};
