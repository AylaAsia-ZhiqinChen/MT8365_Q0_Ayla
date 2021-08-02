#ifndef __CCU_SENSOR_INTERFACE__
#define __CCU_SENSOR_INTERFACE__

#include "ccu_ext_interface/ccu_types.h"

/******************************************************************************
* To save code size, set WHICH_SENSOR_MAIN and WHICH_SENSOR_SUB to 0 and 1
* respectively, and remove the switch-case block that converts them to
* SENSOR_IDX_MAIN and SENSOR_IDX_SUB.
******************************************************************************/
typedef enum WHICH_SENSOR
{
    WHICH_SENSOR_NONE       = 0x00,
    WHICH_SENSOR_MAIN       = 0x01,     // Main sensor: rear camera
    WHICH_SENSOR_MAIN_2     = 0x02,     // Not used for now
    WHICH_SENSOR_MAIN_3     = 0x03,     // Not used for now
    WHICH_SENSOR_SUB        = 0x10,     // Sub sensor: front camera
    WHICH_SENSOR_SUB_2      = 0x20,     // Sub sensor2
    WHICH_SENSOR_MAX        = 0xFF
} WHICH_SENSOR_T;


typedef struct SENSOR_INFO_IN
{
    WHICH_SENSOR_T eWhichSensor;    // Main/Sub sensor
    U16 u16FPS;                     // Current FPS used by set_mode()
    U32 eScenario;    // Preview, Video, HS Video, and etc.
    U32 sensorI2cSlaveAddr;
    U32 dma_buf_va_h;
    U32 dma_buf_va_l;
    U32 u32I2CId;
    U32 *dma_buf_mva;             // virtual address of I2C DMA buffer
    U32 sensor_idx;
    bool isSpSensor;
} SENSOR_INFO_IN_T;

typedef struct SENSOR_INFO_OUT
{
    U8 u8SupportedByCCU;            // 0: not supported, 1: supported
    bool isSpSensor;
} SENSOR_INFO_OUT_T;

#endif
