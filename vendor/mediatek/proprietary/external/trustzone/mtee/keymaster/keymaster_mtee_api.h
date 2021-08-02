
#ifndef _KEYMASTER_TZ_WRAPPER_H_
#define _KEYMASTER_TZ_WRAPPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <hardware/keymaster_defs.h>
#include <tz_cross/ta_keymaster.h>
#include <android/log.h>

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,##__VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,##__VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,##__VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,##__VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,##__VA_ARGS__)

keymaster_error_t km_customized_api_call(int cmd, uint8_t *input, size_t input_size);

keymaster_error_t km_mtee_call(int cmd,
                               uint8_t * input,size_t input_size,
                               uint8_t ** output,size_t * output_size);

template <typename T, typename N>
void km_mtee_request(int cmd, const T &request, N* response){
    keymaster_error_t ret = KM_ERROR_OK;
    bool result = false;
    uint8_t *input = nullptr;
    size_t input_size = 0;
    uint8_t *output = nullptr;
    uint8_t *result_data = nullptr;
    size_t output_size = 0;

    if (response == NULL)
        return;

    response->error = KM_ERROR_OK;

    input_size = request.SerializedSize();

    if(input_size) {
        input = (uint8_t*)malloc(input_size);
        if(!input) {
            response->error = KM_ERROR_MEMORY_ALLOCATION_FAILED;
            return;
        }
        request.Serialize(input, (const uint8_t*)(input + input_size));
    }

    ret = km_mtee_call(cmd, input, input_size, &output, &output_size);

    if(ret != KM_ERROR_OK) {
        response->error = ret;
        return;
    }

    result_data= output;
    result = response->Deserialize((const uint8_t * *)&result_data,
                                   (const uint8_t *)(result_data + output_size));
    if(!result) {
        response->error = KM_ERROR_INSUFFICIENT_BUFFER_SPACE;
        return ;
    }

    free((void *)input);
    free((void *)output);
}
#endif
