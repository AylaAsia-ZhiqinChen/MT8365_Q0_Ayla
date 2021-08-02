/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <cutils/log.h>
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/gatekeeper.h>

#include <UniquePtr.h>
#include <uree/system.h>
#include <uree/mem.h>
#include <tz_cross/ta_mem.h>
#include <tz_cross/trustzone.h>
#include <tz_cross/ta_gatekeeper.h>

// For debugging
// #define LOG_NDEBUG 0

#define LOG_TAG "MTKGateKeeper"
#include <cutils/log.h>

typedef gatekeeper::UniquePtr<gatekeeper_device_t> Unique_gatekeeper_device_t;

/* Close an mtk gatekeeper instance */
static int mtk_gatekeeper_close(hw_device_t* dev) {
    delete dev;
    return 0;
}

#include <time.h>
unsigned long long GetMillisecondsSinceBoot() {
    struct timespec time;
    int res = clock_gettime(CLOCK_BOOTTIME, &time);
    if (res < 0) return 0;
    return ((unsigned long long)time.tv_sec * 1000) + (time.tv_nsec / 1000 / 1000);
}


int uree_mtk_enroll(const struct gatekeeper_device *dev, uint32_t uid,
            const uint8_t *current_password_handle, uint32_t current_password_handle_length,
            const uint8_t *current_password, uint32_t current_password_length,
            const uint8_t *desired_password, uint32_t desired_password_length,
            uint8_t **enrolled_password_handle, uint32_t *enrolled_password_handle_length)
{
	TZ_RESULT ret = TZ_RESULT_SUCCESS;
	UREE_SESSION_HANDLE mem_session;
	UREE_SESSION_HANDLE gatekeeper_session;
	MTEEC_PARAM param[4];

	unsigned long long mssinceboot = GetMillisecondsSinceBoot();
	struct packed_gatekeeper_enroll_param enroll_param = {
		.uid = uid,
		.current_password_handle_length = current_password_handle_length,
		.current_password_length = current_password_length,
		.desired_password_length = desired_password_length,
		.mssinceboot = mssinceboot,
		.current_password_handle = (uint8_t *)current_password_handle,
		.current_password = (uint8_t *)current_password,
		.desired_password = (uint8_t *)desired_password,
		};

	unsigned int enroll_param_buffer_size = 0;
	unsigned char* enroll_param_buffer=serialize_enroll_param(&enroll_param,&enroll_param_buffer_size);

	UREE_SHAREDMEM_PARAM shm_param;
	UREE_SHAREDMEM_HANDLE input_shm_handle = 0;
	UREE_SHAREDMEM_HANDLE output_shm_handle = 0;

	ret = UREE_CreateSession(TZ_TA_GATEKEEPER_UUID, &gatekeeper_session);
	if (ret != TZ_RESULT_SUCCESS) 
	{        
		ALOGE("CreateSession Gatekeeper Session Error: %s\n", TZ_GetErrorString(ret));        
		goto fail_5;
	}

	ret = UREE_CreateSession(TZ_TA_MEM_UUID, &mem_session);    
	if (ret != TZ_RESULT_SUCCESS) 
	{
		ALOGE("CreateSession Memory Session Error: %s\n", TZ_GetErrorString(ret));        
		goto fail_4;
	}
	
	shm_param.buffer = enroll_param_buffer;
	shm_param.size = enroll_param_buffer_size;
	
	ret = UREE_RegisterSharedmemWithTag(mem_session, &input_shm_handle, &shm_param,"gatekeeper.enroll_param");
	if (ret != TZ_RESULT_SUCCESS) 
	{
		ALOGE("RegisterInputSharedMemory Error: %s\n", TZ_GetErrorString(ret));        
		goto fail_3;
	}

	//TODO enrolled_password_handle is prealloc 128K,maybe not enough?
	#define OUTBUFFER_SIZE (128*1024)
	*enrolled_password_handle = (uint8_t *)malloc(OUTBUFFER_SIZE);
	
	shm_param.buffer = *enrolled_password_handle;
	shm_param.size = OUTBUFFER_SIZE;

	ret = UREE_RegisterSharedmemWithTag(mem_session, &output_shm_handle, &shm_param,"gatekeeper.enrolled_password");
	if (ret != TZ_RESULT_SUCCESS) 
	{
		ALOGE("RegisterOutputSharedMemory Error: %s\n", TZ_GetErrorString(ret));        
		goto fail_2;
	}

	param[0].memref.handle = input_shm_handle;
	param[0].memref.offset = 0;
	param[0].memref.size = enroll_param_buffer_size;
	param[1].memref.handle = output_shm_handle;
	param[1].memref.offset = 0;
	param[1].memref.size = OUTBUFFER_SIZE;
	param[2].value.a = 0;
	param[2].value.b = TZ_RESULT_ERROR_GENERIC; //error code
	ret = UREE_TeeServiceCall(gatekeeper_session, TZCMD_GATEKEEPER_ENROLL,TZ_ParamTypes3(TZPT_MEMREF_INPUT,TZPT_MEMREF_OUTPUT,TZPT_VALUE_INOUT), param);    
	if (ret != TZ_RESULT_SUCCESS) 
	{
		ALOGE("MTKGATEKEEPER Enroll Error: %s\n", TZ_GetErrorString(ret)); 
		free(*enrolled_password_handle);
		*enrolled_password_handle = NULL;
		*enrolled_password_handle_length = 0;
	}
	else
	{
		*enrolled_password_handle_length = param[2].value.a;
	}
	ret = param[2].value.b;
fail_1:
	UREE_UnregisterSharedmem(mem_session, output_shm_handle);    
fail_2:
	UREE_UnregisterSharedmem(mem_session, input_shm_handle);    
fail_3:
	UREE_CloseSession(mem_session);    
fail_4:
	UREE_CloseSession(gatekeeper_session);    
fail_5:
	free(enroll_param_buffer);
	return ret;
}

int uree_mtk_verify(const struct gatekeeper_device *dev, uint32_t uid, uint64_t challenge,
            const uint8_t *enrolled_password_handle, uint32_t enrolled_password_handle_length,
            const uint8_t *provided_password, uint32_t provided_password_length,
            uint8_t **auth_token, uint32_t *auth_token_length, bool *request_reenroll)
{
	TZ_RESULT ret = TZ_RESULT_SUCCESS;
	UREE_SESSION_HANDLE mem_session;
	UREE_SESSION_HANDLE gatekeeper_session;
	MTEEC_PARAM param[4];

	unsigned long long mssinceboot = GetMillisecondsSinceBoot();
	struct packed_gatekeeper_verify_param verify_param = {
		.uid = uid,
		.challenge = challenge,
		.enrolled_password_handle_length = enrolled_password_handle_length,
		.provided_password_length = provided_password_length,
		.mssinceboot = mssinceboot,
		.enrolled_password_handle = (uint8_t *)enrolled_password_handle,
		.provided_password = (uint8_t *)provided_password,
		};

	unsigned int verify_param_buffer_size = 0;
	unsigned char* verify_param_buffer=serialize_verify_param(&verify_param,&verify_param_buffer_size);

	UREE_SHAREDMEM_PARAM shm_param;
	UREE_SHAREDMEM_HANDLE input_shm_handle = 0;
	UREE_SHAREDMEM_HANDLE output_shm_handle = 0;

	ret = UREE_CreateSession(TZ_TA_GATEKEEPER_UUID, &gatekeeper_session);
	if (ret != TZ_RESULT_SUCCESS) 
	{        
		ALOGE("CreateSession Gatekeeper Session Error: %s\n", TZ_GetErrorString(ret));        
		goto fail_5;
	}

	ret = UREE_CreateSession(TZ_TA_MEM_UUID, &mem_session);    
	if (ret != TZ_RESULT_SUCCESS) 
	{
		ALOGE("CreateSession Memory Session Error: %s\n", TZ_GetErrorString(ret));        
		goto fail_4;
	}
	
	shm_param.buffer = verify_param_buffer;
	shm_param.size = verify_param_buffer_size;
	
	ret = UREE_RegisterSharedmemWithTag(mem_session, &input_shm_handle, &shm_param,"gatekeeper.verify_param");
	if (ret != TZ_RESULT_SUCCESS) 
	{
		ALOGE("RegisterInputSharedMemory Error: %s\n", TZ_GetErrorString(ret));        
		goto fail_3;
	}

	//TODO enrolled_password_handle is prealloc 128K,maybe not enough?
	#define OUTBUFFER_SIZE (128*1024)
	*auth_token = (uint8_t *)malloc(OUTBUFFER_SIZE);
	
	shm_param.buffer = *auth_token;
	shm_param.size = OUTBUFFER_SIZE;

	ret = UREE_RegisterSharedmemWithTag(mem_session, &output_shm_handle, &shm_param,"gatekeeper.auth_token");
	if (ret != TZ_RESULT_SUCCESS) 
	{
		ALOGE("RegisterOutputSharedMemory Error: %s\n", TZ_GetErrorString(ret));        
		goto fail_2;
	}

	param[0].memref.handle = input_shm_handle;
	param[0].memref.offset = 0;
	param[0].memref.size = verify_param_buffer_size;
	param[1].memref.handle = output_shm_handle;
	param[1].memref.offset = 0;
	param[1].memref.size = OUTBUFFER_SIZE;
	param[2].value.a = 0;
	param[2].value.b = 0;
	param[3].value.a = TZ_RESULT_ERROR_GENERIC; //error code
	ret = UREE_TeeServiceCall(gatekeeper_session, TZCMD_GATEKEEPER_VERIFY,TZ_ParamTypes4(TZPT_MEMREF_INPUT,TZPT_MEMREF_OUTPUT,TZPT_VALUE_OUTPUT,TZPT_VALUE_INOUT), param);    
	if (ret != TZ_RESULT_SUCCESS) 
	{
		ALOGE("MTKGATEKEEPER Verify Error: %s\n", TZ_GetErrorString(ret)); 
		free(*auth_token);
		*auth_token = NULL;
		*auth_token_length = 0;
	}
	else
	{
		*auth_token_length = param[2].value.a;
	}
	*request_reenroll=param[2].value.b;
	ret = param[3].value.a;
fail_1:
	UREE_UnregisterSharedmem(mem_session, output_shm_handle);    
fail_2:
	UREE_UnregisterSharedmem(mem_session, input_shm_handle);    
fail_3:
	UREE_CloseSession(mem_session);    
fail_4:
	UREE_CloseSession(gatekeeper_session);    
fail_5:
	free(verify_param_buffer);
	return ret;
}

int mtk_enroll(const struct gatekeeper_device *dev, uint32_t uid,
            const uint8_t *current_password_handle, uint32_t current_password_handle_length,
            const uint8_t *current_password, uint32_t current_password_length,
            const uint8_t *desired_password, uint32_t desired_password_length,
            uint8_t **enrolled_password_handle, uint32_t *enrolled_password_handle_length)
{
	TZ_RESULT ret = TZ_RESULT_SUCCESS;
    if (enrolled_password_handle == NULL || enrolled_password_handle_length == NULL ||
            desired_password == NULL || desired_password_length == 0)
        return -EINVAL;
        
    // Current password and current password handle go together
    if (current_password_handle == NULL || current_password_handle_length == 0 ||
            current_password == NULL || current_password_length == 0) {
        current_password_handle = NULL; 
        current_password_handle_length = 0;
        current_password = NULL;
        current_password_length = 0;
    }
    
    ret = uree_mtk_enroll(dev,uid,
            current_password_handle, current_password_handle_length,
            current_password, current_password_length,
            desired_password, desired_password_length,
            enrolled_password_handle, enrolled_password_handle_length);
	
    return ret;
}
            

int mtk_verify(const struct gatekeeper_device *dev, uint32_t uid, uint64_t challenge,
            const uint8_t *enrolled_password_handle, uint32_t enrolled_password_handle_length,
            const uint8_t *provided_password, uint32_t provided_password_length,
            uint8_t **auth_token, uint32_t *auth_token_length, bool *request_reenroll)
{
    TZ_RESULT ret = TZ_RESULT_SUCCESS;
	if (enrolled_password_handle == NULL ||
            provided_password == NULL) {
        return -EINVAL;
    }

	ret = uree_mtk_verify(dev,uid,challenge,enrolled_password_handle,enrolled_password_handle_length,
						provided_password,provided_password_length,auth_token,auth_token_length,request_reenroll);
	return ret;
}

/*
 * Generic device handling
 */
__attribute__((visibility("default"))) int mtk_gatekeeper_open(const hw_module_t* module, const char* name,
                                                        hw_device_t** device) {
    if (strcmp(name, HARDWARE_GATEKEEPER) != 0)
        return -EINVAL;

    Unique_gatekeeper_device_t dev(new gatekeeper_device_t);
    if (dev.get() == NULL)
        return -ENOMEM;

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 1;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = mtk_gatekeeper_close;

    dev->enroll = mtk_enroll;
    dev->verify = mtk_verify;
    dev->delete_user = NULL;
    dev->delete_all_users = NULL;
    
    *device = reinterpret_cast<hw_device_t*>(dev.release());

    return 0;
}

static struct hw_module_methods_t gatekeeper_module_methods = {
    .open =mtk_gatekeeper_open,
};

struct gatekeeper_module mtkgatekeeper_module __attribute__((visibility("default"))) = {
    .common =
        {
         .tag = HARDWARE_MODULE_TAG,
         .module_api_version = GATEKEEPER_MODULE_API_VERSION_0_1,
         .hal_api_version = HARDWARE_HAL_API_VERSION,
         .id = GATEKEEPER_HARDWARE_MODULE_ID,
         .name = "MTK HAREWARE GateKeeper HAL",
         .author = "Mediatek",
         .methods = &gatekeeper_module_methods,
         .dso = 0,
         .reserved = {},
        },
};
