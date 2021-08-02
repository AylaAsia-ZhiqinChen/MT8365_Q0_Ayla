#ifndef _CLTRACE_H_
#define _CLTRACE_H_

#include "icd.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <utils/RefBase.h>
#include <cutils/log.h>
#include <utils/Trace.h>

#define ATRACE_TAG ATRACE_TAG_GRAPHICS

using namespace android;

// If ATRACE_NAME() under the if scope, it will be destroyed when leave the if condition
// Use class to implement ATRACE_BEGIN and ATRACE_END manually
#define CL_TRACE_CALL(_funcName, ...)  \
    if(sCLTraceLevel) \
        TraceCL(#_funcName, __VA_ARGS__); \
    CL_Systrace_Object cl_systrace_object(#_funcName, sCLSystraceEnabled);

class CL_Systrace_Object {

private:
    cl_bool l_sCLSystraceEnabled;

public:
    CL_Systrace_Object(const char* funcName, cl_bool sCLSystraceEnabled) {
        l_sCLSystraceEnabled = sCLSystraceEnabled;

        if(l_sCLSystraceEnabled)
            ATRACE_BEGIN(funcName);
    };

    ~CL_Systrace_Object() {
        if(l_sCLSystraceEnabled)
            ATRACE_END();
    };
};

class StringBuilder {
    static const int lineSize = 500;
    char line[lineSize];
    int line_index;
public:
    StringBuilder() {
        line_index = 0;
        line[0] = '\0';
    }
    void append(const char* fmt, ...) {

        va_list argp;
        va_start(argp, fmt);
        line_index += vsnprintf(line + line_index, lineSize-line_index, fmt, argp);
        va_end(argp);
    }
    const char* getString() {
        line_index = 0;
        line[lineSize-1] = '\0';
        return line;
    }
};

void TraceCL(const char* name, int numArgs, ...) {

    va_list argp;
    va_start(argp, numArgs);

    StringBuilder builder;
    builder.append("%s( ", name);

    for(int i=0; i<numArgs; i++) {
        if ( i > 0 ) {
            builder.append(", ");
        }
        const char* type = va_arg(argp, const char*);
        bool isPtr = false;
        if ( (type[strlen(type)-1] == '*')          ||
             (strcmp(type,"cl_platform_id") == 0)   ||
             (strcmp(type,"cl_device_id") == 0)     ||
             (strcmp(type,"cl_context") == 0)       ||
             (strcmp(type,"cl_command_queue") == 0) ||
             (strcmp(type,"cl_program") == 0)       ||
             (strcmp(type,"cl_mem") == 0)           ||
             (strcmp(type,"cl_kernel") == 0)        ||
             (strcmp(type,"cl_event") == 0)         ||
             (strcmp(type,"cl_sampler") == 0)       ||
             (strcmp(type,"CL_CALLBACK") == 0) ) {
            isPtr = true;
        }

        if (isPtr) {
            const void* arg = va_arg(argp, const void*);
            builder.append("(%s) 0x%zx", type, (size_t) arg);
        } else if ( (strcmp(type, "cl_uint") == 0 )                     ||
                    (strcmp(type, "cl_bool") == 0 )                     ||
                    (strcmp(type, "cl_platform_info") == 0 )            ||
                    (strcmp(type, "cl_device_info") == 0 )              ||
                    (strcmp(type, "cl_device_mem_cache_type") == 0 )    ||
                    (strcmp(type, "cl_device_local_mem_type") == 0 )    ||
                    (strcmp(type, "cl_context_info") == 0 )             ||
                    (strcmp(type, "cl_command_queue_info") == 0 )       ||
                    (strcmp(type, "cl_channel_order") == 0 )            ||
                    (strcmp(type, "cl_channel_type") == 0 )             ||
                    (strcmp(type, "cl_mem_object_type") == 0 )          ||
                    (strcmp(type, "cl_mem_info") == 0 )                 ||
                    (strcmp(type, "cl_image_info") == 0 )               ||
                    (strcmp(type, "cl_buffer_create_type") == 0 )       ||
                    (strcmp(type, "cl_addressing_mode") == 0 )          ||
                    (strcmp(type, "cl_filter_mode") == 0 )              ||
                    (strcmp(type, "cl_sampler_info") == 0 )             ||
                    (strcmp(type, "cl_program_info") == 0 )             ||
                    (strcmp(type, "cl_program_build_info") == 0 )       ||
                    (strcmp(type, "cl_program_binary_type") == 0 )      ||
                    (strcmp(type, "cl_kernel_info") == 0 )              ||
                    (strcmp(type, "cl_kernel_arg_info") == 0 )          ||
                    (strcmp(type, "cl_kernel_arg_address_qualifier") == 0 ) ||
                    (strcmp(type, "cl_kernel_arg_access_qualifier") == 0 )  ||
                    (strcmp(type, "cl_kernel_work_group_info") == 0 )   ||
                    (strcmp(type, "cl_event_info") == 0 )               ||
                    (strcmp(type, "cl_command_type") == 0 )             ||
                    (strcmp(type, "cl_profiling_info") == 0 )           ||
                    (strcmp(type, "cl_pipe_info") == 0 )                ||
                    (strcmp(type, "cl_kernel_sub_group_info") == 0 )    ||
                    (strcmp(type, "cl_kernel_exec_info") == 0 )) {
            int arg = va_arg(argp, unsigned int);
            builder.append("(%s) %u",type, arg);
        } else if(  (strcmp(type, "cl_ulong") == 0 )                    ||
                    (strcmp(type, "cl_bitfield") == 0 )                 ||
                    (strcmp(type, "cl_device_type") == 0 )              ||
                    (strcmp(type, "cl_device_fp_config") == 0 )         ||
                    (strcmp(type, "cl_device_exec_capabilities") == 0 ) ||
                    (strcmp(type, "cl_command_queue_properties") == 0 ) ||
                    (strcmp(type, "cl_device_affinity_domain") == 0 )   ||
                    (strcmp(type, "cl_mem_flags") == 0 )                ||
                    (strcmp(type, "cl_mem_migration_flags") == 0 )      ||
                    (strcmp(type, "cl_map_flags") == 0 )                ||
                    (strcmp(type, "cl_kernel_arg_type_qualifier") == 0 )||
                    (strcmp(type, "cl_device_svm_capabilities") == 0 )  ||
                    (strcmp(type, "cl_queue_properties") == 0 )         ||
                    (strcmp(type, "cl_svm_mem_flags") == 0 )            ||
                    (strcmp(type, "cl_sampler_properties") == 0 )) {
            long arg = va_arg(argp, unsigned long);
            builder.append("(%s) %lu",type, arg);
        } else if(  (strcmp(type, "cl_int")) == 0 ||
                    (strcmp(type, "cl_build_status")) == 0 ) {
            int arg = va_arg(argp, int);
            builder.append("(%s) %d",type, arg);
        } else if( (strcmp(type, "size_t")) == 0 ) {
            int arg = va_arg(argp, size_t);
            builder.append("(%s) %d", type, arg);
        } else {
            builder.append("/* ??? %s */", type);
            break;
        }
    }

    builder.append(" );");
    ALOGD("%s", builder.getString());
    va_end(argp);
}

#endif // _CLTRACE_H_