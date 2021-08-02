#ifndef __TRUSTZONE_TA_GATEKEEPER__
#define __TRUSTZONE_TA_GATEKEEPER__

#define TZ_TA_GATEKEEPER_UUID   "88d5fce8-5467-11e5-885d-feff819cdc9f"

/* Data Structure for GATEKEEPER TA */
/* You should define data structure used both in REE/TEE here
   N/A for Test TA */

/* Command for GATEKEEPER TA */
#define TZCMD_GATEKEEPER_ENROLL            0
#define TZCMD_GATEKEEPER_VERIFY            1


#include <string.h>
struct packed_gatekeeper_enroll_param
{
    unsigned int uid;
    unsigned int current_password_handle_length;
    unsigned int current_password_length;
    unsigned int desired_password_length;
    unsigned long long mssinceboot;
    unsigned char *current_password_handle;
    unsigned char *current_password;
    unsigned char *desired_password;
};

static unsigned char* serialize_enroll_param(struct packed_gatekeeper_enroll_param* param,unsigned int * size)
{
    unsigned int serialize_buffer_size = sizeof(unsigned int)*4+sizeof(unsigned long long)+param->current_password_handle_length+param->current_password_length+param->desired_password_length;
    unsigned char* serialize_buffer = (unsigned char*)malloc(serialize_buffer_size);
    unsigned int offset = 0;
    memcpy(serialize_buffer+offset,&param->uid,sizeof(unsigned int));
    offset += sizeof(unsigned int);
    memcpy(serialize_buffer+offset,&param->current_password_handle_length,sizeof(unsigned int));
    offset += sizeof(unsigned int);
    memcpy(serialize_buffer+offset,&param->current_password_length,sizeof(unsigned int));
    offset += sizeof(unsigned int);
    memcpy(serialize_buffer+offset,&param->desired_password_length,sizeof(unsigned int));
    offset += sizeof(unsigned int);
    memcpy(serialize_buffer+offset,&param->mssinceboot,sizeof(unsigned long long));
    offset += sizeof(unsigned long long);
    if(param->current_password_handle_length)
        memcpy(serialize_buffer+offset,param->current_password_handle,param->current_password_handle_length);
    offset += param->current_password_handle_length;
    if(param->current_password_length)
        memcpy(serialize_buffer+offset,param->current_password,param->current_password_length);
    offset += param->current_password_length;
    if(param->desired_password_length)
        memcpy(serialize_buffer+offset,param->desired_password,param->desired_password_length);
    offset += param->desired_password_length;
    *size = serialize_buffer_size;
    return serialize_buffer;
}

static struct packed_gatekeeper_enroll_param* deserialize_enroll_param(unsigned char* param_buffer)
{
    struct packed_gatekeeper_enroll_param* param = (struct packed_gatekeeper_enroll_param*)malloc(sizeof(struct packed_gatekeeper_enroll_param));
    unsigned int offset = 0;
    memset(param,0,sizeof(struct packed_gatekeeper_enroll_param));
    param->uid = *(unsigned int*)(param_buffer+offset);
    offset+=sizeof(unsigned int);
    param->current_password_handle_length = *(unsigned int*)(param_buffer+offset);
    offset+=sizeof(unsigned int);
    param->current_password_length = *(unsigned int*)(param_buffer+offset);
    offset+=sizeof(unsigned int);
    param->desired_password_length = *(unsigned int*)(param_buffer+offset);
    offset+=sizeof(unsigned int);
    param->mssinceboot = *(unsigned long long*)(param_buffer+offset);
    offset+=sizeof(unsigned long long);
    if(param->current_password_handle_length)
    {
        param->current_password_handle = (unsigned char*)malloc(param->current_password_handle_length);
        memcpy(param->current_password_handle,param_buffer+offset,param->current_password_handle_length);
    }
    offset+=param->current_password_handle_length;
    if(param->current_password_length)
    {
        param->current_password = (unsigned char*)malloc(param->current_password_length);
        memcpy(param->current_password,param_buffer+offset,param->current_password_length);
    }
    offset+=param->current_password_length;
    if(param->desired_password_length)
    {
        param->desired_password = (unsigned char*)malloc(param->desired_password_length);
        memcpy(param->desired_password,param_buffer+offset,param->desired_password_length);
    }
    offset+=param->desired_password_length;
    return param;
}

static void free_enroll_param(struct packed_gatekeeper_enroll_param* param)
{
    if(param->current_password_handle_length)
    {
        free(param->current_password_handle);
    }
    if(param->current_password_length)
    {
        free(param->current_password);
    }
    if(param->desired_password_length)
    {
        free(param->desired_password);
    }
    free(param);
}

struct packed_gatekeeper_verify_param
{
    unsigned int uid;
    unsigned long long challenge;
    unsigned int enrolled_password_handle_length;
    unsigned int provided_password_length;
    unsigned long long mssinceboot;
    unsigned char * enrolled_password_handle;
    unsigned char * provided_password;
};

static unsigned char* serialize_verify_param(struct packed_gatekeeper_verify_param* param,unsigned int * size)
{
    unsigned int serialize_buffer_size = sizeof(unsigned int)*3 + sizeof(unsigned long long)*2 + param->enrolled_password_handle_length + param->provided_password_length;
    unsigned char* serialize_buffer = (unsigned char*)malloc(serialize_buffer_size);
    unsigned int offset = 0;
    memcpy(serialize_buffer+offset,&param->uid,sizeof(unsigned int));
    offset += sizeof(unsigned int);
    memcpy(serialize_buffer+offset,&param->challenge,sizeof(unsigned long long));
    offset += sizeof(unsigned long long);
    memcpy(serialize_buffer+offset,&param->enrolled_password_handle_length,sizeof(unsigned int));
    offset += sizeof(unsigned int);
    memcpy(serialize_buffer+offset,&param->provided_password_length,sizeof(unsigned int));
    offset += sizeof(unsigned int);
    memcpy(serialize_buffer+offset,&param->mssinceboot,sizeof(unsigned long long));
    offset += sizeof(unsigned long long);
    if(param->enrolled_password_handle_length)
        memcpy(serialize_buffer+offset,param->enrolled_password_handle,param->enrolled_password_handle_length);
    offset += param->enrolled_password_handle_length;
    if(param->provided_password_length)
        memcpy(serialize_buffer+offset,param->provided_password,param->provided_password_length);
    offset += param->provided_password_length;
    *size = serialize_buffer_size;
    return serialize_buffer;
}

static struct packed_gatekeeper_verify_param* deserialize_verify_param(unsigned char* param_buffer)
{
    struct packed_gatekeeper_verify_param* param = (struct packed_gatekeeper_verify_param*)malloc(sizeof(struct packed_gatekeeper_verify_param));
    unsigned int offset = 0;
    param->uid = *(unsigned int*)(param_buffer+offset);
    offset+=sizeof(unsigned int);
    param->challenge = *(unsigned long long*)(param_buffer+offset);
    offset+=sizeof(unsigned long long);
    param->enrolled_password_handle_length = *(unsigned int*)(param_buffer+offset);
    offset+=sizeof(unsigned int);
    param->provided_password_length = *(unsigned int*)(param_buffer+offset);
    offset+=sizeof(unsigned int);
    param->mssinceboot = *(unsigned long long*)(param_buffer+offset);
    offset+=sizeof(unsigned long long);
    if(param->enrolled_password_handle_length)
    {
        param->enrolled_password_handle = (unsigned char*)malloc(param->enrolled_password_handle_length);
        memcpy(param->enrolled_password_handle,param_buffer+offset,param->enrolled_password_handle_length);
    }
    offset+=param->enrolled_password_handle_length;
    if(param->provided_password_length)
    {
        param->provided_password = (unsigned char*)malloc(param->provided_password_length);
        memcpy(param->provided_password,param_buffer+offset,param->provided_password_length);
    }
    offset+=param->provided_password_length;
    return param;
}

static void free_verify_param(struct packed_gatekeeper_verify_param* param)
{
    if(param->enrolled_password_handle_length)
    {
        free(param->enrolled_password_handle);
    }
    if(param->provided_password_length)
    {
        free(param->provided_password);
    }
    free(param);
}

#endif /* __TRUSTZONE_TA_GATEKEEPER__ */

