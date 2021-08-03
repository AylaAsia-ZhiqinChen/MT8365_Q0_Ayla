/*
 * Copyright (c) 2015-2016 MediaTek Inc.
 * Author: Honghui Zhang <honghui.zhang@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef _DTS_IOMMU_PORT_MT8167_H_
#define _DTS_IOMMU_PORT_MT8167_H_


#define MTK_M4U_ID(larb, port)		(((larb) << 5) | (port))

#define M4U_LARB0_ID			0
#define M4U_LARB1_ID			1
#define M4U_LARB2_ID			2

/* larb0 */
#define M4U_PORT_DISP_OVL0		MTK_M4U_ID(M4U_LARB0_ID, 0)
#define M4U_PORT_DISP_RDMA0		MTK_M4U_ID(M4U_LARB0_ID, 1)
#define M4U_PORT_DISP_WDMA0		MTK_M4U_ID(M4U_LARB0_ID, 2)
#define M4U_PORT_DISP_RDMA1		MTK_M4U_ID(M4U_LARB0_ID, 3)
#define M4U_PORT_MDP_RDMA		MTK_M4U_ID(M4U_LARB0_ID, 4)
#define M4U_PORT_MDP_WDMA		MTK_M4U_ID(M4U_LARB0_ID, 5)
#define M4U_PORT_MDP_WROT		MTK_M4U_ID(M4U_LARB0_ID, 6)
#define M4U_PORT_DISP_FAKE		MTK_M4U_ID(M4U_LARB0_ID, 7)

/* IMG larb1*/
#define M4U_PORT_CAM_IMGO		MTK_M4U_ID(M4U_LARB1_ID, 0)
#define M4U_PORT_CAM_IMG2O		MTK_M4U_ID(M4U_LARB1_ID, 1)
#define M4U_PORT_CAM_LSCI		MTK_M4U_ID(M4U_LARB1_ID, 2)
#define M4U_PORT_CAM_ESFKO		MTK_M4U_ID(M4U_LARB1_ID, 3)
#define M4U_PORT_CAM_AAO		MTK_M4U_ID(M4U_LARB1_ID, 4)
#define M4U_PORT_VENC_REC		MTK_M4U_ID(M4U_LARB1_ID, 5)
#define M4U_PORT_VENC_BSDMA		MTK_M4U_ID(M4U_LARB1_ID, 6)
#define M4U_PORT_VENC_RD_COMV		MTK_M4U_ID(M4U_LARB1_ID, 7)
#define M4U_PORT_CAM_IMGI		MTK_M4U_ID(M4U_LARB1_ID, 8)
#define M4U_PORT_VENC_CUR_LUMA		MTK_M4U_ID(M4U_LARB1_ID, 9)
#define M4U_PORT_VENC_CUR_CHROMA	MTK_M4U_ID(M4U_LARB1_ID, 10)
#define M4U_PORT_VENC_REF_LUMA		MTK_M4U_ID(M4U_LARB1_ID, 11)
#define M4U_PORT_VENC_REF_CHROMA	MTK_M4U_ID(M4U_LARB1_ID, 12)

/* VDEC larb2*/
#define M4U_PORT_HW_VDEC_MC_EXT	MTK_M4U_ID(M4U_LARB2_ID, 0)
#define M4U_PORT_HW_VDEC_PP_EXT	MTK_M4U_ID(M4U_LARB2_ID, 1)
#define M4U_PORT_HW_VDEC_VLD_EXT	MTK_M4U_ID(M4U_LARB2_ID, 2)
#define M4U_PORT_HW_VDEC_AVC_MV_EXT	MTK_M4U_ID(M4U_LARB2_ID, 3)
#define M4U_PORT_HW_VDEC_PRED_RD_EXT	MTK_M4U_ID(M4U_LARB2_ID, 4)
#define M4U_PORT_HW_VDEC_PRED_WR_EXT	MTK_M4U_ID(M4U_LARB2_ID, 5)
#define M4U_PORT_HW_VDEC_PPWRAP_EXT	MTK_M4U_ID(M4U_LARB2_ID, 6)

#endif
