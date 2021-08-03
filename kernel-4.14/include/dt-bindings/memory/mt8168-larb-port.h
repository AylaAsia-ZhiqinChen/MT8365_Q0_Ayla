/*
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Yong Wu <yong.wu@mediatek.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#ifndef _DTS_IOMMU_PORT_MT8168_H_
#define _DTS_IOMMU_PORT_MT8168_H_

#define MTK_M4U_ID(larb, port)	(((larb) << 5) | (port))

/* larb0 */
#define M4U_PORT_DISP_OVL0		MTK_M4U_ID(0, 0)
#define M4U_PORT_DISP_OVL0_2L		MTK_M4U_ID(0, 1)
#define M4U_PORT_DISP_RDMA0		MTK_M4U_ID(0, 2)
#define M4U_PORT_DISP_WDMA0		MTK_M4U_ID(0, 3)
#define M4U_PORT_DISP_RDMA1		MTK_M4U_ID(0, 4)
#define M4U_PORT_MDP_RDMA0		MTK_M4U_ID(0, 5)
#define M4U_PORT_MDP_WROT1		MTK_M4U_ID(0, 6)
#define M4U_PORT_MDP_WROT0		MTK_M4U_ID(0, 7)
#define M4U_PORT_MDP_RDMA1		MTK_M4U_ID(0, 8)
#define M4U_PORT_DISP_FAKE0		MTK_M4U_ID(0, 9)

/* larb1 */
#define M4U_PORT_VENC_RCPU		MTK_M4U_ID(1, 0)
#define M4U_PORT_VENC_REC		MTK_M4U_ID(1, 1)
#define M4U_PORT_VENC_BSDMA		MTK_M4U_ID(1, 2)
#define M4U_PORT_VENC_SV_COMV		MTK_M4U_ID(1, 3)
#define M4U_PORT_VENC_RD_COMV		MTK_M4U_ID(1, 4)
#define M4U_PORT_VENC_NBM_RDMA		MTK_M4U_ID(1, 5)
#define M4U_PORT_VENC_NBM_RDMA_LITE	MTK_M4U_ID(1, 6)
#define M4U_PORT_JPGENC_Y_RDMA		MTK_M4U_ID(1, 7)
#define M4U_PORT_JPGENC_C_RDMA		MTK_M4U_ID(1, 8)
#define M4U_PORT_JPGENC_Q_TABLE		MTK_M4U_ID(1, 9)
#define M4U_PORT_JPGENC_BSDMA		MTK_M4U_ID(1, 10)
#define M4U_PORT_JPGDEC_WDMA		MTK_M4U_ID(1, 11)
#define M4U_PORT_JPGDEC_BSDMA		MTK_M4U_ID(1, 12)
#define M4U_PORT_VENC_NBM_WDMA		MTK_M4U_ID(1, 13)
#define M4U_PORT_VENC_NBM_WDMA_LITE	MTK_M4U_ID(1, 14)
#define M4U_PORT_VENC_CUR_LUMA		MTK_M4U_ID(1, 15)
#define M4U_PORT_VENC_CUR_CHROMA	MTK_M4U_ID(1, 16)
#define M4U_PORT_VENC_REF_LUMA		MTK_M4U_ID(1, 17)
#define M4U_PORT_VENC_REF_CHROMA	MTK_M4U_ID(1, 18)

/* larb2 */
#define M4U_PORT_CAM_IMGO		MTK_M4U_ID(2, 0)
#define M4U_PORT_CAM_RRZO		MTK_M4U_ID(2, 1)
#define M4U_PORT_CAM_AAO		MTK_M4U_ID(2, 2)
#define M4U_PORT_CAM_LCS		MTK_M4U_ID(2, 3)
#define M4U_PORT_CAM_ESFKO		MTK_M4U_ID(2, 4)
#define M4U_PORT_CAM_CAM_SV0		MTK_M4U_ID(2, 5)
#define M4U_PORT_CAM_CAM_SV1		MTK_M4U_ID(2, 6)
#define M4U_PORT_CAM_LSCI		MTK_M4U_ID(2, 7)
#define M4U_PORT_CAM_LSCI_D		MTK_M4U_ID(2, 8)
#define M4U_PORT_CAM_AFO		MTK_M4U_ID(2, 9)
#define M4U_PORT_CAM_SPARE		MTK_M4U_ID(2, 10)
#define M4U_PORT_CAM_BPCI		MTK_M4U_ID(2, 11)
#define M4U_PORT_CAM_BPCI_D		MTK_M4U_ID(2, 12)
#define M4U_PORT_CAM_UFDI		MTK_M4U_ID(2, 13)
#define M4U_PORT_CAM_IMGI		MTK_M4U_ID(2, 14)
#define M4U_PORT_CAM_IMG2O		MTK_M4U_ID(2, 15)
#define M4U_PORT_CAM_IMG3O		MTK_M4U_ID(2, 16)
#define M4U_PORT_CAM_WPE0_I		MTK_M4U_ID(2, 17)
#define M4U_PORT_CAM_WPE1_I		MTK_M4U_ID(2, 18)
#define M4U_PORT_CAM_WPE_O		MTK_M4U_ID(2, 19)
#define M4U_PORT_CAM_FD0_I		MTK_M4U_ID(2, 20)
#define M4U_PORT_CAM_FD1_I		MTK_M4U_ID(2, 21)
#define M4U_PORT_CAM_FD0_O		MTK_M4U_ID(2, 22)
#define M4U_PORT_CAM_FD1_O		MTK_M4U_ID(2, 23)

/* larb3 */
#define M4U_PORT_HW_VDEC_MC_EXT		MTK_M4U_ID(3, 0)
#define M4U_PORT_HW_VDEC_UFO_EXT	MTK_M4U_ID(3, 1)
#define M4U_PORT_HW_VDEC_PP_EXT		MTK_M4U_ID(3, 2)
#define M4U_PORT_HW_VDEC_PRED_RD_EXT	MTK_M4U_ID(3, 3)
#define M4U_PORT_HW_VDEC_PRED_WR_EXT	MTK_M4U_ID(3, 4)
#define M4U_PORT_HW_VDEC_PPWRAP_EXT	MTK_M4U_ID(3, 5)
#define M4U_PORT_HW_VDEC_TILE_EXT	MTK_M4U_ID(3, 6)
#define M4U_PORT_HW_VDEC_VLD_EXT	MTK_M4U_ID(3, 7)
#define M4U_PORT_HW_VDEC_VLD2_EXT	MTK_M4U_ID(3, 8)
#define M4U_PORT_HW_VDEC_AVC_MV_EXT	MTK_M4U_ID(3, 9)
#define M4U_PORT_HW_VDEC_RG_CTRL_DMA_EXT MTK_M4U_ID(3, 10)

/* larb4 */
#define M4U_PORT_APU_READ		MTK_M4U_ID(0, 0)
#define M4U_PORT_APU_WRITE		MTK_M4U_ID(0, 1)

#endif
