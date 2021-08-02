#ifndef CMDQ_EVENT_COMMON
#define CMDQ_EVENT_COMMON

/* Define CMDQ events
 *
 * For hardware event must define in device tree.
 * For SW event assign event ID here directly.
 *
 * Note: event name must sync to cmdq_events table in cmdq_event_common.c
 */
enum CMDQ_EVENT_ENUM {
	/* MDP start frame */
	CMDQ_EVENT_MDP_RDMA0_SOF = 0,
	CMDQ_EVENT_MDP_RDMA1_SOF,		/* 1 */
	CMDQ_EVENT_MDP_DSI0_TE_SOF,		/* 2 */
	CMDQ_EVENT_MDP_DSI1_TE_SOF,		/* 3 */
	CMDQ_EVENT_MDP_MVW_SOF,			/* 4 */
	CMDQ_EVENT_MDP_TDSHP0_SOF,		/* 5 */
	CMDQ_EVENT_MDP_TDSHP1_SOF,		/* 6 */
	CMDQ_EVENT_MDP_WDMA_SOF,		/* 7 */
	CMDQ_EVENT_MDP_WROT0_SOF,		/* 8 */
	CMDQ_EVENT_MDP_WROT1_SOF,		/* 9 */
	CMDQ_EVENT_MDP_CROP_SOF,		/* 10 */
	/* Display start frame */
	CMDQ_EVENT_DISP_OVL0_SOF,		/* 11 */
	CMDQ_EVENT_DISP_OVL1_SOF,		/* 12 */
	CMDQ_EVENT_DISP_RDMA0_SOF,		/* 13 */
	CMDQ_EVENT_DISP_RDMA1_SOF,		/* 14 */
	CMDQ_EVENT_DISP_RDMA2_SOF,		/* 15 */
	CMDQ_EVENT_DISP_WDMA0_SOF,		/* 16 */
	CMDQ_EVENT_DISP_WDMA1_SOF,		/* 17 */
	CMDQ_EVENT_DISP_COLOR0_SOF,		/* 18 */
	CMDQ_EVENT_DISP_COLOR1_SOF,		/* 19 */
	CMDQ_EVENT_DISP_AAL_SOF,		/* 20 */
	CMDQ_EVENT_DISP_GAMMA_SOF,		/* 21 */
	CMDQ_EVENT_DISP_UFOE_SOF,		/* 22 */
	CMDQ_EVENT_DISP_PWM0_SOF,		/* 23 */
	CMDQ_EVENT_DISP_PWM1_SOF,		/* 24 */
	CMDQ_EVENT_DISP_OD_SOF,			/* 25 */
	/* MDP frame done */
	CMDQ_EVENT_MDP_RDMA0_EOF,		/* 26 */
	CMDQ_EVENT_MDP_RDMA1_EOF,		/* 27 */
	CMDQ_EVENT_MDP_RSZ0_EOF,		/* 28 */
	CMDQ_EVENT_MDP_RSZ1_EOF,		/* 29 */
	CMDQ_EVENT_MDP_RSZ2_EOF,		/* 30 */
	CMDQ_EVENT_MDP_TDSHP0_EOF,		/* 31 */
	CMDQ_EVENT_MDP_TDSHP1_EOF,		/* 32 */
	CMDQ_EVENT_MDP_WDMA_EOF,		/* 33 */
	CMDQ_EVENT_MDP_WROT0_W_EOF,		/* 34 */
	CMDQ_EVENT_MDP_WROT0_R_EOF,		/* 35 */
	CMDQ_EVENT_MDP_WROT1_W_EOF,		/* 36 */
	CMDQ_EVENT_MDP_WROT1_R_EOF,		/* 37 */
	CMDQ_EVENT_MDP_CROP_EOF,		/* 38 */
	/* Display frame done */
	CMDQ_EVENT_DISP_OVL0_EOF,		/* 39 */
	CMDQ_EVENT_DISP_OVL1_EOF,		/* 40 */
	CMDQ_EVENT_DISP_RDMA0_EOF,		/* 41 */
	CMDQ_EVENT_DISP_RDMA1_EOF,		/* 42 */
	CMDQ_EVENT_DISP_RDMA2_EOF,		/* 43 */
	CMDQ_EVENT_DISP_WDMA0_EOF,		/* 44 */
	CMDQ_EVENT_DISP_WDMA1_EOF,		/* 45 */
	CMDQ_EVENT_DISP_COLOR0_EOF,		/* 46 */
	CMDQ_EVENT_DISP_COLOR1_EOF,		/* 47 */
	CMDQ_EVENT_DISP_AAL_EOF,		/* 48 */
	CMDQ_EVENT_DISP_GAMMA_EOF,		/* 49 */
	CMDQ_EVENT_DISP_UFOE_EOF,		/* 50 */
	CMDQ_EVENT_DISP_DPI0_EOF,		/* 51 */
	/* Mutex frame done */
	CMDQ_EVENT_MUTEX0_STREAM_EOF = 53,
	CMDQ_EVENT_MUTEX1_STREAM_EOF,		/* 54 */
	CMDQ_EVENT_MUTEX2_STREAM_EOF,		/* 55 */
	CMDQ_EVENT_MUTEX3_STREAM_EOF,		/* 56 */
	CMDQ_EVENT_MUTEX4_STREAM_EOF,		/* 57 */
	CMDQ_EVENT_MUTEX5_STREAM_EOF,		/* 58 */
	CMDQ_EVENT_MUTEX6_STREAM_EOF,		/* 59 */
	CMDQ_EVENT_MUTEX7_STREAM_EOF,		/* 60 */
	CMDQ_EVENT_MUTEX8_STREAM_EOF,		/* 61 */
	CMDQ_EVENT_MUTEX9_STREAM_EOF,		/* 62 */
	/* DpFramework via CMDQ_IOCTL_LOCK_MUTEX */
	/* Display underrun */
	CMDQ_EVENT_DISP_RDMA0_UNDERRUN,		/* 63 */
	CMDQ_EVENT_DISP_RDMA1_UNDERRUN,		/* 64 */
	CMDQ_EVENT_DISP_RDMA2_UNDERRUN,		/* 65 */
	/* ISP frame done */
	CMDQ_EVENT_ISP_PASS2_2_EOF = 129,
	CMDQ_EVENT_ISP_PASS2_1_EOF,		/* 130 */
	CMDQ_EVENT_ISP_PASS2_0_EOF,		/* 131 */
	CMDQ_EVENT_ISP_PASS1_1_EOF,		/* 132 */
	CMDQ_EVENT_ISP_PASS1_0_EOF,		/* 133 */
	/* Engine events */
	CMDQ_EVENT_ISP_CAMSV_2_PASS1_DONE,	/* 134 */
	CMDQ_EVENT_ISP_CAMSV_1_PASS1_DONE,	/* 135 */
	CMDQ_EVENT_ISP_SENINF_CAM1_2_3_FULL,	/* 136 */
	CMDQ_EVENT_ISP_SENINF_CAM0_FULL,	/* 137 */
	/* JPEG frame done */
	CMDQ_EVENT_JPEG_ENC_PASS2_EOF = 257,
	CMDQ_EVENT_JPEG_ENC_PASS1_EOF,		/* 258 */
	CMDQ_EVENT_JPEG_DEC_EOF,		/* 259 */

	CMDQ_MAX_HW_EVENT_COUNT,		/* 260 */

	/* Keep this at the end of HW events */
	/* SW Sync Tokens (Pre-defined) */
	CMDQ_SYNC_TOKEN_CONFIG_DIRTY,		/* 261 */
	/* Config thread notify trigger thread */
	CMDQ_SYNC_TOKEN_STREAM_EOF,		/* 262 */
	/* Trigger thread notify config thread */
	/* ESD check state. Trigger thread will be blocked until the check finishes. */
	CMDQ_SYNC_TOKEN_ESD_EOF,		/* 263 */
	CMDQ_SYNC_TOKEN_CABC_EOF,		/* 264 */
	/* check CABC setup finish */
	/* Block Trigger thread until the path freeze finishes */
	CMDQ_SYNC_TOKEN_FREEZE_EOF,		/* 265 */
	/* Pass-2 notifies VENC frame is ready to be encoded */
	CMDQ_SYNC_TOKEN_VENC_INPUT_READY = 270,
	/* VENC notifies Pass-2 encoding completion so next frame may start */
	CMDQ_SYNC_TOKEN_VENC_EOF,		/* 271 */
	/* Notify normal CMDQ there are some secure task done */
	CMDQ_SYNC_SECURE_THR_EOF = 299,
	/* SW Sync Tokens (User-defined) */
	CMDQ_SYNC_TOKEN_USER_0,			/* 300 */
	/* SW-defined sync token */
	CMDQ_SYNC_TOKEN_USER_1,			/* 301 */
	/* GPR access tokens (for HW register backup) */
	/* There are 15 32-bit GPR, 3 GPR form a set (64-bit for address, 32-bit for value) */
	CMDQ_SYNC_TOKEN_GPR_SET_0 = 400,
	CMDQ_SYNC_TOKEN_GPR_SET_1 = 401,
	CMDQ_SYNC_TOKEN_GPR_SET_2 = 402,
	CMDQ_SYNC_TOKEN_GPR_SET_3 = 403,
	CMDQ_SYNC_TOKEN_GPR_SET_4 = 404,


	CMDQ_SYNC_TOKEN_MAX = 0x1FF,
	CMDQ_SYNC_TOKEN_INVALID = -1,
};

#endif