// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 MediaTek Inc.
 * Author: Yong Wu <yong.wu@mediatek.com>
 */
#include "pseudo_m4u.h"
#include <dt-bindings/memory/mt8167-larb-port.h>

/*
 * Get the local arbiter ID and the portid within the larb arbiter
 * from mtk_m4u_id which is defined by MTK_M4U_ID.
 */
#define MTK_M4U_TO_LARB(id)		(((id) >> 5) & 0xf)
#define MTK_M4U_TO_PORT(id)		((id) & 0x1f)
bool m4u_portid_valid(const int portID)
{
	unsigned int larb = MTK_M4U_TO_LARB(portID);

	return !!(larb <= 2);
}

/* portID is from dt-bindings/memory/mtxx-larb-port.h */
/* Only print the port name for debug. */
const char *m4u_get_port_name(const int portID)
{
	const char *p;

	switch (portID) {
	case M4U_PORT_DISP_OVL0:
		p = "M4U_PORT_DISP_OVL0";
		break;
	case M4U_PORT_DISP_RDMA0:
		p = "M4U_PORT_DISP_RDMA0";
		break;
	case M4U_PORT_DISP_WDMA0:
		p = "M4U_PORT_DISP_WDMA0";
		break;
	case M4U_PORT_DISP_RDMA1:
		p = "M4U_PORT_DISP_RDMA1";
		break;
	case M4U_PORT_MDP_RDMA:
		p = "M4U_PORT_MDP_RDMA";
		break;
	case M4U_PORT_MDP_WDMA:
		p = "M4U_PORT_MDP_WDMA";
		break;
	case M4U_PORT_MDP_WROT:
		p = "M4U_PORT_MDP_WROT";
		break;
	case M4U_PORT_DISP_FAKE:
		p = "M4U_PORT_DISP_FAKE";
		break;

	/* larb1 */
	case M4U_PORT_CAM_IMGO:
		p = "M4U_PORT_CAM_IMGO";
		break;
	case M4U_PORT_CAM_IMG2O:
		p = "M4U_PORT_CAM_IMG2O";
		break;
	case M4U_PORT_CAM_LSCI:
		p = "M4U_PORT_CAM_LSCI";
		break;
	case M4U_PORT_CAM_ESFKO:
		p = "M4U_PORT_CAM_ESFKO";
		break;
	case M4U_PORT_CAM_AAO:
		p = "M4U_PORT_CAM_AAO";
		break;
	case M4U_PORT_VENC_REC:
		p = "M4U_PORT_VENC_REC";
		break;
	case M4U_PORT_VENC_BSDMA:
		p = "M4U_PORT_VENC_BSDMA";
		break;
	case M4U_PORT_VENC_RD_COMV:
		p = "M4U_PORT_VENC_RD_COMV";
		break;
	case M4U_PORT_CAM_IMGI:
		p = "M4U_PORT_CAM_IMGI";
		break;
	case M4U_PORT_VENC_CUR_LUMA:
		p = "M4U_PORT_VENC_CUR_LUMA";
		break;
	case M4U_PORT_VENC_CUR_CHROMA:
		p = "M4U_PORT_VENC_CUR_CHROMA";
		break;
	case M4U_PORT_VENC_REF_LUMA:
		p = "M4U_PORT_VENC_REF_LUMA";
		break;
	case M4U_PORT_VENC_REF_CHROMA:
		p = "M4U_PORT_VENC_REF_CHROMA";
		break;

	/* larb2 */
	case M4U_PORT_HW_VDEC_MC_EXT:
		p = "M4U_PORT_HW_VDEC_MC_EXT";
		break;
	case M4U_PORT_HW_VDEC_PP_EXT:
		p = "M4U_PORT_HW_VDEC_PP_EXT";
		break;
	case M4U_PORT_HW_VDEC_VLD_EXT:
		p = "M4U_PORT_HW_VDEC_VLD_EXT";
		break;
	case M4U_PORT_HW_VDEC_AVC_MV_EXT:
		p = "M4U_PORT_HW_VDEC_AVC_MV_EXT";
		break;
	case M4U_PORT_HW_VDEC_PRED_RD_EXT:
		p = "M4U_PORT_HW_VDEC_PRED_RD_EXT";
		break;
	case M4U_PORT_HW_VDEC_PRED_WR_EXT:
		p = "M4U_PORT_HW_VDEC_PRED_WR_EXT";
		break;
	case M4U_PORT_HW_VDEC_PPWRAP_EXT:
		p = "M4U_PORT_HW_VDEC_PPWRAP_EXT";
		break;

	default:
		p = "UNKNOWN PORT";
		break;
	}

	return p;
}
