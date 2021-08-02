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
#ifndef _MT_ISP_FRMB_H
#define _MT_ISP_FRMB_H
#include "camera_isp.h"
#define ISR_LOG_ON
#define T_STAMP_2_0
#define _rtbc_buf_que_2_0_
typedef enum {
  ISP_IRQ_CLEAR_NONE_FRMB,
  ISP_IRQ_CLEAR_WAIT_FRMB,
  ISP_IRQ_CLEAR_STATUS_FRMB,
  ISP_IRQ_CLEAR_ALL_FRMB
} ISP_IRQ_CLEAR_ENUM_FRMB;
typedef enum {
  ISP_IRQ_TYPE_INT_FRMB,
  ISP_IRQ_TYPE_DMA_FRMB,
  ISP_IRQ_TYPE_INTB_FRMB,
  ISP_IRQ_TYPE_DMAB_FRMB,
  ISP_IRQ_TYPE_INTC_FRMB,
  ISP_IRQ_TYPE_DMAC_FRMB,
  ISP_IRQ_TYPE_INTX_FRMB,
  ISP_IRQ_TYPE_DMAX_FRMB,
  ISP_IRQ_TYPE_AMOUNT_FRMB
} ISP_IRQ_TYPE_ENUM_FRMB;
typedef enum {
  ISP_IRQ_WAITIRQ_SPEUSER_NONE = 0,
  ISP_IRQ_WAITIRQ_SPEUSER_EIS = 1,
  ISP_IRQ_WAITIRQ_SPEUSER_NUM
} ISP_IRQ_WAITIRQ_SPEUSER_ENUM;
typedef struct {
  ISP_IRQ_TYPE_ENUM_FRMB Type;
  unsigned int Status;
  int UserKey;
} ISP_IRQ_USER_STRUCT_FRMB;
typedef struct {
  unsigned int tLastSig_sec;
  unsigned int tLastSig_usec;
  unsigned int tMark2WaitSig_sec;
  unsigned int tMark2WaitSig_usec;
  unsigned int tLastSig2GetSig_sec;
  unsigned int tLastSig2GetSig_usec;
  int passedbySigcnt;
} ISP_IRQ_TIME_STRUCT_FRMB;
typedef struct {
  unsigned int tLastSOF2P1done_sec;
  unsigned int tLastSOF2P1done_usec;
} ISP_EIS_META_STRUCT;
typedef struct {
  ISP_IRQ_CLEAR_ENUM_FRMB Clear;
  ISP_IRQ_USER_STRUCT_FRMB UserInfo;
  ISP_IRQ_TIME_STRUCT_FRMB TimeInfo;
  ISP_EIS_META_STRUCT EisMeta;
  ISP_IRQ_WAITIRQ_SPEUSER_ENUM SpecUser;
  unsigned int Timeout;
  unsigned int bDumpReg;
} ISP_WAIT_IRQ_STRUCT_FRMB;
typedef struct {
  int userKey;
  char * userName;
} ISP_REGISTER_USERKEY_STRUCT_FRMB;
typedef struct {
  ISP_IRQ_TYPE_ENUM Type;
  unsigned int Status;
} ISP_READ_IRQ_STRUCT_FRMB;
typedef struct {
  ISP_IRQ_TYPE_ENUM Type;
  unsigned int Status;
} ISP_CLEAR_IRQ_STRUCT_FRMB;
typedef enum {
  ISP_HOLD_TIME_VD_FRMB,
  ISP_HOLD_TIME_EXPDONE_FRMB
} ISP_HOLD_TIME_ENUM_FRMB;
typedef struct {
  unsigned int Addr_FrmB;
  unsigned int Val_FrmB;
} ISP_REG_STRUCT_FRMB;
typedef struct {
  ISP_REG_STRUCT_FRMB * pData_FrmB;
  unsigned int Count_FrmB;
} ISP_REG_IO_STRUCT_FRMB;
typedef enum {
  ISP_CALLBACK_WORKQUEUE_VD_FRMB,
  ISP_CALLBACK_WORKQUEUE_EXPDONE_FRMB,
  ISP_CALLBACK_TASKLET_VD_FRMB,
  ISP_CALLBACK_TASKLET_EXPDONE_FRMB,
  ISP_CALLBACK_AMOUNT_FRMB
} ISP_CALLBACK_ENUM_FRMB;
typedef struct {
  ISP_CALLBACK_ENUM Type_FrmB;
  pIspCallback Func_FrmB;
} ISP_CALLBACK_STRUCT_FRMB;
#define P1_DEQUE_CNT 1
#define ISP_REG_P1_CFG_IDX 0x4090
typedef enum {
  _cam_tg_ = 0,
  _cam_tg2_,
  _camsv_tg_,
  _camsv2_tg_,
  _cam_tg_max_
} _isp_tg_enum_;
typedef struct {
  unsigned int w;
  unsigned int h;
  unsigned int xsize;
  unsigned int stride;
  unsigned int fmt;
  unsigned int pxl_id;
  unsigned int wbn;
  unsigned int ob;
  unsigned int lsc;
  unsigned int rpg;
  unsigned int m_num_0;
  unsigned int frm_cnt;
} ISP_RT_IMAGE_INFO_STRUCT;
typedef struct {
  unsigned int srcX;
  unsigned int srcY;
  unsigned int srcW;
  unsigned int srcH;
  unsigned int dstW;
  unsigned int dstH;
} ISP_RT_HRZ_INFO_STRUCT;
typedef struct {
  unsigned int x;
  unsigned int y;
  unsigned int w;
  unsigned int h;
} ISP_RT_DMAO_CROPPING_STRUCT;
typedef struct {
  unsigned int memID;
  unsigned int size;
  long long base_vAddr;
  unsigned int base_pAddr;
  unsigned int timeStampS;
  unsigned int timeStampUs;
  unsigned int bFilled;
  ISP_RT_IMAGE_INFO_STRUCT image;
  ISP_RT_HRZ_INFO_STRUCT HrzInfo;
  ISP_RT_DMAO_CROPPING_STRUCT dmaoCrop;
  unsigned int bDequeued;
  signed int bufIdx;
} ISP_RT_BUF_INFO_STRUCT_FRMB;
typedef struct {
  unsigned int count;
  unsigned int sof_cnt;
  unsigned int img_cnt;
  ISP_RT_BUF_INFO_STRUCT_FRMB data[P1_DEQUE_CNT];
} ISP_DEQUE_BUF_INFO_STRUCT_FRMB;
typedef struct {
  unsigned int start;
  unsigned int total_count;
  unsigned int empty_count;
  unsigned int pre_empty_count;
  unsigned int active;
  unsigned int read_idx;
  unsigned int img_cnt;
  ISP_RT_BUF_INFO_STRUCT_FRMB data[ISP_RT_BUF_SIZE];
} ISP_RT_RING_BUF_INFO_STRUCT_FRMB;
typedef enum {
  ISP_RT_BUF_CTRL_ENQUE_FRMB,
#ifdef _rtbc_buf_que_2_0_
  ISP_RT_BUF_CTRL_ENQUE_IMD_FRMB,
#else
  ISP_RT_BUF_CTRL_ENQUE_IMD_FRMB = ISP_RT_BUF_CTRL_ENQUE_FRMB,
#endif
  ISP_RT_BUF_CTRL_EXCHANGE_ENQUE_FRMB,
  ISP_RT_BUF_CTRL_DEQUE_FRMB,
  ISP_RT_BUF_CTRL_IS_RDY_FRMB,
#ifdef _rtbc_buf_que_2_0_
  ISP_RT_BUF_CTRL_DMA_EN_FRMB,
#endif
  ISP_RT_BUF_CTRL_GET_SIZE_FRMB,
  ISP_RT_BUF_CTRL_CLEAR_FRMB,
  ISP_RT_BUF_CTRL_CUR_STATUS_FRMB,
  ISP_RT_BUF_CTRL_MAX_FRMB
} ISP_RT_BUF_CTRL_ENUM_FRMB;
typedef struct {
  ISP_RTBC_STATE_ENUM state;
  unsigned long dropCnt;
  ISP_RT_RING_BUF_INFO_STRUCT_FRMB ring_buf[_rt_dma_max_];
} ISP_RT_BUF_STRUCT_FRMB;
typedef struct {
  ISP_RT_BUF_CTRL_ENUM_FRMB ctrl;
  _isp_dma_enum_ buf_id;
  ISP_RT_BUF_INFO_STRUCT_FRMB * data_ptr;
  ISP_RT_BUF_INFO_STRUCT_FRMB * ex_data_ptr;
  unsigned char * pExtend;
} ISP_BUFFER_CTRL_STRUCT_FRMB;
#define _use_kernel_ref_cnt_
typedef enum {
  ISP_REF_CNT_GET_FRMB,
  ISP_REF_CNT_INC_FRMB,
  ISP_REF_CNT_DEC_FRMB,
  ISP_REF_CNT_DEC_AND_RESET_P1_P2_IF_LAST_ONE_FRMB,
  ISP_REF_CNT_DEC_AND_RESET_P1_IF_LAST_ONE_FRMB,
  ISP_REF_CNT_DEC_AND_RESET_P2_IF_LAST_ONE_FRMB,
  ISP_REF_CNT_MAX_FRMB
} ISP_REF_CNT_CTRL_ENUM_FRMB;
typedef enum {
  ISP_REF_CNT_ID_IMEM_FRMB,
  ISP_REF_CNT_ID_ISP_FUNC_FRMB,
  ISP_REF_CNT_ID_GLOBAL_PIPE_FRMB,
  ISP_REF_CNT_ID_P1_PIPE_FRMB,
  ISP_REF_CNT_ID_P2_PIPE_FRMB,
  ISP_REF_CNT_ID_MAX_FRMB,
} ISP_REF_CNT_ID_ENUM_FRMB;
typedef struct {
  ISP_REF_CNT_CTRL_ENUM_FRMB ctrl;
  ISP_REF_CNT_ID_ENUM_FRMB id;
  signed int * data_ptr;
} ISP_REF_CNT_CTRL_STRUCT_FRMB;
typedef enum {
  ISP_ED_BUFQUE_CTRL_ENQUE_FRAME = 0,
  ISP_ED_BUFQUE_CTRL_WAIT_DEQUE,
  ISP_ED_BUFQUE_CTRL_DEQUE_SUCCESS,
  ISP_ED_BUFQUE_CTRL_DEQUE_FAIL,
  ISP_ED_BUFQUE_CTRL_WAIT_FRAME,
  ISP_ED_BUFQUE_CTRL_WAKE_WAITFRAME,
  ISP_ED_BUFQUE_CTRL_CLAER_ALL,
  ISP_ED_BUFQUE_CTRL_MAX
} ISP_ED_BUFQUE_CTRL_ENUM;
typedef struct {
  ISP_ED_BUFQUE_CTRL_ENUM ctrl;
  unsigned int processID;
  unsigned int callerID;
  int p2burstQIdx;
  int p2dupCQIdx;
  unsigned int timeoutUs;
} ISP_ED_BUFQUE_STRUCT_FRMB;
typedef enum {
  ISP_ED_BUF_STATE_NONE = - 1,
  ISP_ED_BUF_STATE_ENQUE = 0,
  ISP_ED_BUF_STATE_RUNNING,
  ISP_ED_BUF_STATE_WAIT_DEQUE_FAIL,
  ISP_ED_BUF_STATE_DEQUE_SUCCESS,
  ISP_ED_BUF_STATE_DEQUE_FAIL
} ISP_ED_BUF_STATE_ENUM;
#define _MAGIC_NUM_ERR_HANDLING_
#ifdef _rtbc_use_cq0c_
typedef struct _cq_info_rtbc_st_frmb_ {
  CQ_CMD_ST imgo_frmb;
  CQ_CMD_ST img2o_frmb;
  CQ_CMD_ST next_cq0ci_frmb;
  CQ_CMD_ST end_frmb;
  unsigned long imgo_base_pAddr_frmb;
  unsigned long img2o_base_pAddr_frmb;
  signed int imgo_buf_idx_frmb;
  signed int img2o_buf_idx_frmb;
} CQ_INFO_RTBC_ST_FRMB;
typedef struct _cq_ring_cmd_st_frmb_ {
  CQ_INFO_RTBC_ST_FRMB cq_rtbc_frmb;
  unsigned long next_pa_frmb;
  struct _cq_ring_cmd_st_frmb_ * pNext_frmb;
} CQ_RING_CMD_ST_FRMB;
typedef struct _cq_rtbc_ring_st_frmb_ {
  CQ_RING_CMD_ST_FRMB rtbc_ring_frmb[ISP_RT_CQ0C_BUF_SIZE];
  unsigned long imgo_ring_size_frmb;
  unsigned long img2o_ring_size_frmb;
} CQ_RTBC_RING_ST_FRMB;
#endif
typedef struct _cq0b_info_rtbc_st_frmb_ {
  CQ_CMD_ST ob_frmb;
  CQ_CMD_ST end_frmb;
} CQ0B_INFO_RTBC_ST_FRMB;
typedef struct _cq0b_ring_cmd_st_frmb_ {
  CQ0B_INFO_RTBC_ST_FRMB cq0b_rtbc_frmb;
  unsigned long next_pa_frmb;
  struct _cq0b_ring_cmd_st_frmb_ * pNext_frmb;
} CQ0B_RING_CMD_ST_FRMB;
typedef struct _cq0b_rtbc_ring_st_frmb_ {
  CQ0B_RING_CMD_ST_FRMB rtbc_ring_frmb;
} CQ0B_RTBC_RING_ST_FRMB;
typedef enum {
  ISP_CMD_REGISTER_IRQ_FRMB = ISP_CMD_SENSOR_FREQ_CTRL + 1,
  ISP_CMD_DEBUG_FLAG_FRMB,
  ISP_CMD_UNREGISTER_IRQ_FRMB,
  ISP_CMD_WAIT_IRQ_FRMB,
  ISP_CMD_ED_QUEBUF_CTRL_FRMB,
  ISP_CMD_UPDATE_REGSCEN_FRMB,
  ISP_CMD_QUERY_REGSCEN_FRMB,
  ISP_CMD_UPDATE_BURSTQNUM_FRMB,
  ISP_CMD_QUERY_BURSTQNUM_FRMB,
  ISP_CMD_DUMP_ISR_LOG_FRMB,
  ISP_CMD_GET_CUR_SOF_FRMB,
  ISP_CMD_GET_DMA_ERR_FRMB,
  ISP_CMD_GET_INT_ERR_FRMB,
#ifdef T_STAMP_2_0
  ISP_CMD_SET_FPS_FRMB,
#endif
  ISP_CMD_REGISTER_IRQ_USER_KEY,
  ISP_CMD_MARK_IRQ_REQUEST,
  ISP_CMD_GET_MARK2QUERY_TIME,
  ISP_CMD_FLUSH_IRQ_REQUEST,
  ISP_CMD_SET_CAM_VERSION,
  ISP_CMD_GET_DROP_FRAME_FRMB,
} ISP_CMD_ENUM_FRMB;
#define ISP_DEBUG_FLAG_FRMB _IOW(ISP_MAGIC, ISP_CMD_DEBUG_FLAG_FRMB, unsigned long)
#define ISP_REGISTER_IRQ_FRMB _IOW(ISP_MAGIC, ISP_CMD_REGISTER_IRQ_FRMB, ISP_WAIT_IRQ_STRUCT_FRMB)
#define ISP_UNREGISTER_IRQ_FRMB _IOW(ISP_MAGIC, ISP_CMD_UNREGISTER_IRQ_FRMB, ISP_WAIT_IRQ_STRUCT_FRMB)
#define ISP_WAIT_IRQ_FRMB _IOW(ISP_MAGIC, ISP_CMD_WAIT_IRQ_FRMB, ISP_WAIT_IRQ_STRUCT_FRMB)
#define ISP_ED_QUEBUF_CTRL_FRMB _IOWR(ISP_MAGIC, ISP_CMD_ED_QUEBUF_CTRL_FRMB, ISP_ED_BUFQUE_STRUCT_FRMB)
#define ISP_UPDATE_REGSCEN_FRMB _IOWR(ISP_MAGIC, ISP_CMD_UPDATE_REGSCEN_FRMB, unsigned int)
#define ISP_QUERY_REGSCEN_FRMB _IOR(ISP_MAGIC, ISP_CMD_QUERY_REGSCEN_FRMB, unsigned int)
#define ISP_UPDATE_BURSTQNUM_FRMB _IOW(ISP_MAGIC, ISP_CMD_UPDATE_BURSTQNUM_FRMB, int)
#define ISP_QUERY_BURSTQNUM_FRMB _IOR(ISP_MAGIC, ISP_CMD_QUERY_BURSTQNUM_FRMB, int)
#define ISP_DUMP_ISR_LOG_FRMB _IO(ISP_MAGIC, ISP_CMD_DUMP_ISR_LOG_FRMB)
#define ISP_GET_CUR_SOF_FRMB _IOWR(ISP_MAGIC, ISP_CMD_GET_CUR_SOF_FRMB, unsigned long)
#define ISP_GET_DMA_ERR_FRMB _IOWR(ISP_MAGIC, ISP_CMD_GET_DMA_ERR_FRMB, unsigned int)
#define ISP_GET_INT_ERR_FRMB _IOR(ISP_MAGIC, ISP_CMD_GET_INT_ERR_FRMB, unsigned long)
#ifdef T_STAMP_2_0
#define ISP_SET_FPS_FRMB _IOW(ISP_MAGIC, ISP_CMD_SET_FPS_FRMB, unsigned int)
#endif
#define ISP_GET_DROP_FRAME_FRMB _IOWR(ISP_MAGIC, ISP_CMD_GET_DROP_FRAME_FRMB, unsigned int)
#define ISP_REGISTER_IRQ_USER_KEY _IOWR(ISP_MAGIC, ISP_CMD_REGISTER_IRQ_USER_KEY, ISP_REGISTER_USERKEY_STRUCT_FRMB)
#define ISP_MARK_IRQ_REQUEST _IOWR(ISP_MAGIC, ISP_CMD_MARK_IRQ_REQUEST, ISP_WAIT_IRQ_STRUCT_FRMB)
#define ISP_GET_MARK2QUERY_TIME _IOWR(ISP_MAGIC, ISP_CMD_GET_MARK2QUERY_TIME, ISP_WAIT_IRQ_STRUCT_FRMB)
#define ISP_FLUSH_IRQ_REQUEST _IOW(ISP_MAGIC, ISP_CMD_FLUSH_IRQ_REQUEST, ISP_WAIT_IRQ_STRUCT_FRMB)
#define ISP_SET_CAM_VERSION _IOW(ISP_MAGIC, ISP_CMD_SET_CAM_VERSION, bool)
typedef enum _eISPIrq {
  _IRQ = 0,
  _IRQ_D = 1,
  _CAMSV_IRQ = 2,
  _CAMSV_D_IRQ = 3,
  _IRQ_MAX = 4,
} eISPIrq;
#endif
