/*! \file ccu_control_extif.h
    \brief External interfaces for CCU Control
*/
#ifndef __CCU_CTRL_INTERFACE__
#define __CCU_CTRL_INTERFACE__

#include "ccu_ext_interface/ccu_types.h"

//--fpgaep
extern MBOOL sec_vsync_pushed;

/******************************************************************************
* Task definition
******************************************************************************/
/**
* Enum for function NSCcuIf::ICcuCtrl::ccuControl()
*/
typedef enum
{
    /*Receive by CCU*/
    //CCU Task
    MSG_TO_CCU_IDLE = 0x00000000,
    MSG_TO_CCU_SENSOR_INIT, /**< Request CCU do sensor initialization \param[in] inDataPtr #SENSOR_INFO_IN_T \param[out] outDataPtr #SENSOR_INFO_OUT_T*/
    MSG_TO_CCU_AE_INIT, /**< Request CCU do AE initialization \param[in] inDataPtr #CCU_AE_INITI_PARAM_T */
    MSG_TO_CCU_AE_START, /**< Request CCU to start AE processing \n receving AE related ISP interrupt, process AE algo and do sensor/ISP settings \n No params */
    MSG_TO_CCU_AE_STOP, /**< Request CCU to stop AE processing \n receving AE related ISP interrupt, process AE algo and do sensor/ISP settings, \n No params */
    MSG_TO_CCU_SET_AP_AE_FRAME_SYNC_DATA, /**< Send per-frame AE informaton to CCU \param[in] inDataPtr #ccu_ae_frame_sync_data */
    MSG_TO_CCU_SET_AP_AE_ONCHANGE_DATA, /**< Send AE information that merely changes \param[in] inDataPtr #ccu_ae_onchange_data */
    MSG_TO_CCU_GET_CCU_OUTPUT, /**< Get CCU AE algo output \n Includes AE index, exposure settings.. \param[out] outDataPtr #AE_CORE_MAIN_OUT* */
    MSG_TO_CCU_SET_MAX_FRAMERATE, /**< Set current max fps to CCU \n CCU will set max fps to sensor to achive framerate control \param[in] inDataPtr #ccu_max_framerate_data */
    MSG_TO_CCU_GET_AE_DEBUG_INFO, /**< Request CCU to fill AE EXIF information \n Includes AE index, exposure settings \param[out] outDataPtr #CCU_AE_DEBUG_INFO_T* */
    MSG_TO_CCU_SET_3A_SYNC_STATE, /**< Tell CCU 3A sync control mode\param[in] inDataPtr #NSCcuIf::Ccu3ASyncState */
    MSG_TO_CCU_SYNC_AE_SETTING, /**< Request CCU do 1 time AE expsure sync \n CCU will sync AE expsure from master sensor to slave sensor \n No params */
    MSG_TO_CCU_START_FRAME_SYNC,
    MSG_TO_CCU_STOP_FRAME_SYNC,
    MSG_TO_CCU_QUERY_FRAME_SYNC_DONE,
    MSG_TO_CCU_SUSPEND, /**< Request CCU to suspend on corresponding TG \n No params */
    MSG_TO_CCU_RESUME, /**< Request CCU to resume on corresponding TG \n No params */
    MSG_TO_CCU_SHUTDOWN, /**< Request CCU to shutdown \n No params */

#ifdef CCU_AF_ENABLE
    MSG_TO_CCU_AF_INIT,
    MSG_TO_CCU_AF_START,
    MSG_TO_CCU_AF_ALGO_DONE,
    MSG_TO_CCU_AF_ACQUIRE_AFO_BUFFER,
    MSG_TO_CCU_AF_SET_HW_REG,
    MSG_TO_CCU_AF_STOP,
    MSG_TO_CCU_AF_ABORT,
#endif //CCU_AF_ENABLE
    /*To identify ccu control msg count*/
    DELIMITER_CCU_CONTROL_CNT_1,
    /*CCU internal task*/
#ifdef CCU_AF_ENABLE
    MSG_TO_CCU_AF_CQ0,
    MSG_TO_CCU_AF_AFO,
    MSG_TO_CCU_AF_CQ0B,
    MSG_TO_CCU_AF_AFOB,
#endif //CCU_AF_ENABLE
    MSG_CCU_INTERNAL_BANK_AE_ALGO,
    MSG_CCU_INTERNAL_FRAME_AE_ALGO,
    MSG_CCU_INTERNAL_VSYNC_AE_SYNC,
    MSG_CCU_INTERNAL_FRAME_SYNC,
    MSG_CCU_INTERNAL_PR_LOG,
    /*To identify ccu control msg count*/
    DELIMITER_CCU_CONTROL_CNT_2,

    /*Send to APMCU*/
    //CCU Ack/Done
    MSG_CCU_DONE_SENSOR_INIT, /**< Ack cmd for #MSG_TO_CCU_SENSOR_INIT */
    MSG_CCU_DONE_AE_INIT, /**< Ack cmd for #MSG_TO_CCU_AE_INIT */
    MSG_CCU_ACK_AE_START, /**< Ack cmd for #MSG_TO_CCU_AE_START */
    MSG_CCU_ACK_AE_STOP, /**< Ack cmd for #MSG_TO_CCU_AE_STOP */
    MSG_CCU_ACK_SET_AP_AE_FRAME_SYNC_DATA, /**< Ack cmd for #MSG_TO_CCU_SET_AP_AE_FRAME_SYNC_DATA */
    MSG_CCU_ACK_SET_AP_AE_ONCHANGE_DATA, /**< Ack cmd for #MSG_TO_CCU_SET_AP_AE_ONCHANGE_DATA */
    MSG_CCU_ACK_GET_CCU_OUTPUT, /**< Ack cmd for #MSG_TO_CCU_GET_CCU_OUTPUT */
    MSG_CCU_ACK_SET_MAX_FRAMERATE, /**< Ack cmd for #MSG_TO_CCU_SET_MAX_FRAMERATE */
    MSG_CCU_ACK_GET_AE_DEBUG_INFO, /**< Ack cmd for #MSG_TO_CCU_GET_AE_DEBUG_INFO */
    MSG_CCU_ACK_SET_3A_SYNC_STATE, /**< Ack cmd for #MSG_TO_CCU_SET_3A_SYNC_STATE */
    MSG_CCU_ACK_SYNC_AE_SETTING, /**< Ack cmd for #MSG_TO_CCU_SYNC_AE_SETTING */
    MSG_CCU_ACK_START_FRAME_SYNC,
    MSG_CCU_ACK_STOP_FRAME_SYNC,
    MSG_CCU_ACK_QUERY_FRAME_SYNC_DONE,
    MSG_CCU_ACK_SUSPEND, /**< Ack cmd for #MSG_TO_CCU_SUSPEND */
    MSG_CCU_ACK_RESUME, /**< Ack cmd for #MSG_TO_CCU_RESUME */
    MSG_CCU_DONE_SHUTDOWN, /**< Ack cmd for #MSG_TO_CCU_SHUTDOWN */

#ifdef CCU_AF_ENABLE
    MSG_CCU_ACK_AF_INIT,
    MSG_CCU_ACK_AF_START,
    MSG_CCU_ACK_AF_ALGO_DONE,
    MSG_CCU_ACK_AF_ACQUIRE_AFO_BUFFER,
    MSG_CCU_ACK_AF_SET_HW_REG,
    MSG_CCU_ACK_AF_STOP,
    MSG_CCU_ACK_AF_ABORT,
#endif //CCU_AF_ENABLE
    //APMCU Task
    MSG_TO_APMCU_FLUSH_LOG, /**< CCU Request APMCU to print out CCU logs. \param[in] inDataPtr (MUINT32) indicates log buffer index */
    MSG_TO_APMCU_CCU_ASSERT, /**< CCU inform APMCU that CCU ASSERT occurs \param[in] inDataPtr (MUINT32) indicates error# */
    MSG_TO_APMCU_CCU_WARNING /**< CCU inform APMCU that CCU WARNING occurs \param[in] inDataPtr (MUINT32) indicates error# */
#ifdef CCU_AF_ENABLE
    ,MSG_TO_APMCU_CAM_A_AFO_i,
    MSG_TO_APMCU_CAM_B_AFO_i
#endif //CCU_AF_ENABLE
} ccu_msg_id;

enum ccu_tg_info
{
    CCU_CAM_TG_NONE = 0x0,
    CCU_CAM_TG_MIN  = 0x1,
    CCU_CAM_TG_1    = 0x1,
    CCU_CAM_TG_2    = 0x2,
    CCU_CAM_TG_MAX
};
#define CCU_TG2IDX(TG) (TG-1)
#define CCU_CAM_TG_MIN_IDX (CCU_CAM_TG_MIN - 1)
#define CCU_CAM_TG_MAX_IDX (CCU_CAM_TG_MAX - 1)
#define CCU_CAM_TG_CNT (CCU_CAM_TG_MAX - CCU_CAM_TG_MIN)

struct ccu_msg { /*16bytes*/
    volatile ccu_msg_id msg_id;
    volatile MUINT32 in_data_ptr;
    volatile MUINT32 out_data_ptr;
    volatile enum ccu_tg_info tg_info;
};

struct ccu2ap_msg //12bytes
{
    volatile ccu_msg_id msg_id;
    volatile MUINT32 in_data_ptr;
    volatile MUINT32 out_data_ptr;
};

struct ap2ccu_ipc_t //20bytes
{
    volatile MUINT32 write_cnt;
    volatile MUINT32 read_cnt;
    struct ccu_msg msg;
    volatile MBOOL ack;
};

struct exif_data_addrs_s
{
    MUINT32 ae_algo_data_addr;
    MUINT32 ae_init_data_addr;
    MUINT32 ae_vsync_info_addr;
    MUINT32 n3d_ae_addr;
    MUINT32 n3d_output_addr;
    MUINT32 n3d_init_addr;
};

struct shared_buf_map
{
    MUINT32 ipc_in_data_addr_ccu;
    MUINT32 ipc_out_data_addr_ccu;
    MUINT32 ipc_in_data_base_offset;
    MUINT32 ipc_out_data_base_offset;
    MUINT32 ipc_base_offset;
    struct exif_data_addrs_s exif_data_addrs[CCU_CAM_TG_CNT];
};

enum ccu_feature_type
{
    CCU_FEATURE_UNDEF = 0x0,
    CCU_FEATURE_MIN = 0x1,
    CCU_FEATURE_AE = 0x1,
    CCU_FEATURE_AF,
    CCU_FEATURE_3ASYNC,
    CCU_FEATURE_MAX,
};
#define CCU_FEATURE_COUNT (CCU_FEATURE_MAX - CCU_FEATURE_MIN)

/******************************************************************************
* Special isr task (execute in isr)
******************************************************************************/
#define ISR_SP_TASK_SHUTDOWN 0x000000FF
#define ISR_SP_TASK_TRG_I2C_DONE 0x000000C1
#define ISR_SP_TASK_RST_I2C_DONE 0x000000B9

/******************************************************************************
* Special ap isr task (execute in apmcu_isr)
******************************************************************************/
#define APISR_SP_TASK_TRIGGER_I2C 0x000000A1
#define APISR_SP_TASK_RESET_I2C   0x000000A2

/******************************************************************************
* Error code definition
******************************************************************************/
#define CCU_ERROR_NO               (0)
#define CCU_ERROR_QUEUE_FULL       (1)

/******************************************************************************
* Status definition
******************************************************************************/
#define CCU_STATUS_INIT_DONE              0xffff0000
#define CCU_STATUS_INIT_DONE_2            0xffff00a5

/******************************************************************************
* Direct-link source definition
******************************************************************************/
#define AE_STREAM_DL_SRC_AAO  0x00000000
#define AE_STREAM_DL_SRC_PSO  0x00000001

#endif
