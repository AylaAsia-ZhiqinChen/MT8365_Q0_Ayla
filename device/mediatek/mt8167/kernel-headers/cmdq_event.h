/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_RDMA0_SOF, 0) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_RSZ0_SOF, 1) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_RSZ1_SOF, 2) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_TDSHP_SOF, 3) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_WDMA_SOF, 4) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_WROT_SOF, 5) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_OVL0_SOF, 6) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_OVL1_SOF, 7) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_RDMA0_SOF, 8) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_RDMA1_SOF, 9) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_WDMA0_SOF, 10) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_COLOR_SOF, 11) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_CCORR_SOF, 12) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_AAL_SOF, 13) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_GAMMA_SOF, 14) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_DITHER_SOF, 15) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_UFOE_SOF, 16) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_PWM0_SOF, 17) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_WDMA1_SOF, 18) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA0_L0_SOF, 19) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA0_L1_SOF, 20) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA0_L2_SOF, 21) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA0_L3_SOF, 22) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA1_L0_SOF, 23) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA1_L1_SOF, 24) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA1_L2_SOF, 25) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA1_L3_SOF, 26) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_DSC_SOF, 27) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_RDMA0_EOF, 28) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_RSZ0_EOF, 29) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_RSZ1_EOF, 30) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_TDSHP_EOF, 31) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_WDMA_EOF, 32) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_WROT_WRITE_EOF, 33) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MDP_WROT_READ_EOF, 34) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_OVL0_EOF, 35) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_OVL1_EOF, 36) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_RDMA0_EOF, 37) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_RDMA1_EOF, 38) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_WDMA0_EOF, 39) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_COLOR_EOF, 40) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_CCORR_EOF, 41) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_AAL_EOF, 42) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_GAMMA_EOF, 43) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_DITHER_EOF, 44) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_UFOE_EOF, 45) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_DPI0_EOF, 46) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_WDMA1_EOF, 47) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA0_L0_EOF, 48) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA0_L1_EOF, 49) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA0_L2_EOF, 50) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA0_L3_EOF, 51) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA1_L0_EOF, 52) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA1_L1_EOF, 53) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA1_L2_EOF, 54) DECLARE_CMDQ_EVENT(CMDQ_EVENT_UFOD_RAMA1_L3_EOF, 55) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_DPI1_EOF, 56) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_DSC_EOF, 57) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX0_STREAM_EOF, 58) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX1_STREAM_EOF, 59) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX2_STREAM_EOF, 60) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX3_STREAM_EOF, 61) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX4_STREAM_EOF, 62) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX5_STREAM_EOF, 63) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX6_STREAM_EOF, 64) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX7_STREAM_EOF, 65) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX8_STREAM_EOF, 66) DECLARE_CMDQ_EVENT(CMDQ_EVENT_MUTEX9_STREAM_EOF, 67) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_RDMA0_UNDERRUN, 68) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DISP_RDMA1_UNDERRUN, 69) DECLARE_CMDQ_EVENT(CMDQ_EVENT_DSI_TE, 70) DECLARE_CMDQ_EVENT(CMDQ_EVENT_ISP_2_EOF, 130) DECLARE_CMDQ_EVENT(CMDQ_EVENT_ISP_1_EOF, 131) DECLARE_CMDQ_EVENT(CMDQ_EVENT_ISP_SENINF1_FULL, 135) DECLARE_CMDQ_EVENT(CMDQ_EVENT_VENC_EOF, 257) DECLARE_CMDQ_EVENT(CMDQ_EVENT_JPEG_ENC_EOF, 258) DECLARE_CMDQ_EVENT(CMDQ_EVENT_JPEG_DEC_EOF, 259) DECLARE_CMDQ_EVENT(CMDQ_EVENT_VENC_MB_DONE, 260) DECLARE_CMDQ_EVENT(CMDQ_EVENT_VENC_128BYTE_CNT_DONE, 261) DECLARE_CMDQ_EVENT(CMDQ_MAX_HW_EVENT_COUNT, 270) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_CONFIG_DIRTY, 271) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_STREAM_EOF, 272) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_ESD_EOF, 273) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_CABC_EOF, 274) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_FREEZE_EOF, 275) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_VENC_INPUT_READY, 280) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_VENC_EOF, 281) DECLARE_CMDQ_EVENT(CMDQ_SYNC_SECURE_THR_EOF, 299) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_USER_0, 300) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_USER_1, 301) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_GPR_SET_0, 400) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_GPR_SET_1, 401) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_GPR_SET_2, 402) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_GPR_SET_3, 403) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_GPR_SET_4, 404) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_MAX, (0x1FF)) DECLARE_CMDQ_EVENT(CMDQ_SYNC_TOKEN_INVALID, (- 1))
