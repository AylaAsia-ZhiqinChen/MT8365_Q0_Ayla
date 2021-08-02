#ifndef _KD_IMGSENSOR_CA_TA_CMD_H_
#define _KD_IMGSENSOR_CA_TA_CMD_H_

enum IMGSENSOR_TEE_CMD {
	IMGSENSOR_TEE_CMD_OPEN = 0x0,
	IMGSENSOR_TEE_CMD_GET_INFO,
	IMGSENSOR_TEE_CMD_GET_RESOLUTION,
	IMGSENSOR_TEE_CMD_FEATURE_CONTROL,
	IMGSENSOR_TEE_CMD_CONTROL,
	IMGSENSOR_TEE_CMD_CLOSE,
	IMGSENSOR_TEE_CMD_DUMP_REG,
	IMGSENSOR_TEE_CMD_GET_I2C_BUS,
	IMGSENSOR_TEE_CMD_SET_SENSOR,
};

typedef void* seninf_ca_open_session_t(void);
typedef int seninf_ca_close_session_t(void*);
typedef int seninf_ca_sync_to_pa_t(void*, void *);
typedef int seninf_ca_sync_to_va_t(void*, void *);
typedef int seninf_ca_checkpipe_t(void*);
typedef int seninf_ca_free_t(void*);

#define SENINF_CA_LIB "libimgsensorca.so"
#define SENINF_CA_OPEN_SESSION "seninf_ca_open_session"
#define SENINF_CA_CLOSE_SESSION "seninf_ca_close_session"
#define SENINF_CA_SYNC_TO_PA "seninf_ca_sync_to_pa"
#define SENINF_CA_SYNC_TO_VA "seninf_ca_sync_to_va"
#define SENINF_CA_CHECKPIPE "seninf_ca_checkpipe"
#define SENINF_CA_FREE "seninf_ca_free"

#endif