#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <uree/system.h>
#include <uree/mem.h>
#include <tz_cross/ta_drmkey.h>
#include <tz_cross/ta_mem.h>
#include <tz_cross/keyblock.h>
#include <keyblock_protect.h>

#include <cutils/log.h>

#include <cutils/sockets.h>

#include <linux/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

//#include "FT_Public.h"
//#include "meta_common.h"
#include "MetaPub.h"
#include "meta_drmkey_install_para.h"
#include <sys/stat.h>
#include <sys/types.h>


//#define printf LOGE
#undef LOGE
#define LOGE factory_provision_str
#define DRMKEY_FLOW_TEST 0

#define MEM_TAG "KM for test memory"

#include <time.h>

void GetNowTimeStr(char* strDest)
{
    time_t timer = time(NULL);
    strftime(strDest,256,"%Y-%m-%d %H:%M:%S",localtime(&timer));
}

#define KEYBLOCK_SIZE 0x100000

extern int store_keyblock_ota_to_nvram(unsigned char* kb,int len);
extern int merge_drm_buffs(char* kb_magic,unsigned char* kb_buff[],unsigned int kb_length[],unsigned int kb_num,DRMKeyID id_filter[],unsigned int filter_num,unsigned char** out_kb,unsigned int* out_length);

#define factory_provision_kisd_mode_log_path "/persistbackup/drmkey_operation.log"
static int factory_provision_mode_enable = 0;
static int factory_provision_kisd_mode_enable = 0;
static char factory_provision_log_path[256] = {0};
void factory_provision_str(char* str,...);

//---modify for all porject start
#include <cutils/properties.h>

#define KEYBLOCK_F_PATH "/dev/kb"
#define KEYBLOCK_O_PATH "/dev/dkb"

static int drmkey_remove_file(char *filename)
{
    if (remove(filename) != 0)
    {
        LOGE("remove file faield: %s\n", filename);
        return -1;
    }

    return 0;
}

static int drmkey_make_directory(const char *path, mode_t mode)
{
    if (mkdir(path, 0755) != 0)
    {
        LOGE("mkdir faield: %s\n", path);
        return -1;
    }
    return 0;
}
//---modify for all porject end


int write_buff_to_file(unsigned char* buff,unsigned int length,char* path)
{
#if 1
    FILE* file = fopen(path,"wb");
    int written = 0;
    if(file == NULL)
    {
         LOGE("11 Open file Error: %s\n", path);
         return -1;
    }
    written = fwrite(buff,1,length,file);
    fflush(file);
    if(written != (int)length)
    {
         LOGE("Write file Error: %s\n", path);
         fclose(file);
         return -2;
    }
    fclose(file);
    return 0;
#endif

#if 0
    int fd = open(path,O_RDWR|O_CREAT,0777);
    if(fd >0)
    {
        write(fd,buff,length);
        close(fd);
        return 0;
    }
    return -1;
#endif
}

int read_file_to_buff(unsigned char** buff,unsigned int* length,char* path)
{
    struct stat f_info;
    FILE* file;
    unsigned int readLen = 0;
    file = fopen(path,"rb");
    if(file == NULL)
    {
        LOGE("22 Open file Error: %s\n", path);
        return -1;
    }
    if(stat(path,&f_info) != 0)
    {
        LOGE("stat file_info Error: %s\n", path);
        fclose(file);
        return -2;
    }
    *buff= (unsigned char*)malloc(f_info.st_size);
    if(*buff == NULL)
    {
        LOGE("read file to buffer:Allocate memory failed.\n");
        fclose(file);
        return -3;
    }
    *length = f_info.st_size;
    readLen = fread(*buff,1,*length,file);
    if(readLen != *length)
    {
        LOGE("failed to read file,read len = %d.\n",readLen);
        fclose(file);
        return -3;
    }
    fclose(file);
    return 0;
}


static long get_file_size(char* filename)
{
    struct stat f_stat;

    if(stat(filename,&f_stat) == -1)
    {
        return -1;
    }

    return (long)f_stat.st_size;
}

#define PROVISION_LOG_SIZE_LIMITED (6*1024*1024)
#define PROVISION_LOG_SIZE_STRIPED (5*1024*1024)
void check_and_strip_provision_file(char* path)
{
    long file_size = get_file_size(path);
    //if the size of provision file is larger than 6MB,strip it to 5MB.
    if(file_size >=  PROVISION_LOG_SIZE_LIMITED)
    {
        unsigned char* tmp_buff = NULL;
        long strip_offset = file_size-PROVISION_LOG_SIZE_STRIPED;
        int ret = read_file_to_buff(&tmp_buff,(unsigned int *)&file_size,path);
        if(ret == 0 && tmp_buff)
        {
            ret = write_buff_to_file(tmp_buff+strip_offset,PROVISION_LOG_SIZE_STRIPED,path);
            if(ret != 0)
            {
                ALOGE("Failed to write tmp_buff!\n");
            }
        }else
        {
            ALOGE("Failed to read tmp_buff!\n");
        }
        if(tmp_buff)
        {
            free(tmp_buff);
        }
    }
}
int write_buff_to_provision_file(unsigned char* buff,unsigned int length,char* path)
{
    check_and_strip_provision_file(path);
    int fsLen = 0;
    FILE* file = fopen(path,"a+");

    //delete dummy string
    int i = 0;
    for(i=length-1;i>=1;i--)
    {
        if(buff[i] == 0 && buff[i-1] == 0)
            continue;
        break;
    }

    if(file == NULL)
    {
         //ALOGE("33 Open file Error: %s\n", path);
         return -1;
    }
    char timestr[256] = {0};
    GetNowTimeStr(timestr);
    fsLen += fwrite(timestr,1,strlen(timestr)+1,file);
    fsLen += fwrite("    ",1,1,file);
    fsLen += fwrite(buff,1,i,file);
    int total_len = strlen(timestr)+1+1+i;
    if(fsLen != total_len){
        //just warning
        ALOGE("Total write len: %d, total len: %d.\n", fsLen,total_len);
    }
    fflush(file);
    fclose(file);
    return 0;
}

void factory_provision_mode(char* log_path)
{
    factory_provision_mode_enable = 1;
    strncpy(factory_provision_log_path,log_path,255);
}

void factory_provision_enable_kisd_mode(void)
{
    factory_provision_kisd_mode_enable = 1;
}


void factory_provision_str(char* str,...)
{
    char log_path[256]={0};
    char log_buff[256]={0};
    va_list args;
    va_start(args,str);
    vsnprintf(log_buff,255,str,args);
    va_end(args);
    snprintf(log_path,256,"%s/provision_log",factory_provision_log_path);
    if(factory_provision_mode_enable)
    {
        //write_buff_to_provision_file((unsigned char*)log_buff,strlen(log_buff)+1,log_path);
    }
    if(factory_provision_kisd_mode_enable)
    {
        //write_buff_to_provision_file((unsigned char*)log_buff,strlen(log_buff)+1,factory_provision_kisd_mode_log_path);
    }
    ALOGE("%s",log_buff);
}

void factory_provision_info(unsigned char* buff,int len)
{
    char log_path[256]={0};
    snprintf(log_path,256,"%s/provision_log",factory_provision_log_path);
    if(factory_provision_mode_enable)
    {
        write_buff_to_provision_file(buff,len,log_path);
    }
    if(factory_provision_kisd_mode_enable)
    {
        write_buff_to_provision_file(buff,len,factory_provision_kisd_mode_log_path);
    }
}

void factory_provision_finish(void)
{
    //dummy funcion,provision_ok is not needed from new quest
}


int HexDump(unsigned char* buff,int len)
{
    int i=0;
    //LOGE("\n===Dump buffer 0x%x len:0x%x\n",(unsigned int)buff,len);
    for(i=0;i<len;i++)
    {
        LOGE("0x%x,",buff[i]);
        if(i%16 == 0) LOGE("\n");
    }
    return 0;
}

int drmkey_operation_log(char* buff)
{
    char* path = "/data/vendor/key_provisioning/drmkey_operation.log";
    char timestr[256] = {0};
    int fsLen = 0;
    FILE* file = fopen(path,"a+");
    if(file == NULL)
    {
         //LOGE("44 Open file Error: %s\n", path);
         return -1;
    }
    fsLen += fwrite(buff,1,strlen(buff)+1,file);
    GetNowTimeStr(timestr);
    fsLen += fwrite(timestr,1,strlen(timestr)+1,file);
    fsLen += fwrite("\n",1,1,file);
    int total_len = strlen(buff)+1+strlen(timestr)+1+1;
    if(fsLen != total_len){
        //just warning
        ALOGE("Total write len: %d, total len: %d.\n", fsLen,total_len);
    }
    fflush(file);
    fclose(file);
    return 0;
}

static int socket_local_client_retry(const char* name,int namespaceId,int type)
{
    int retry_times = 0;
    int client_fd = -1;
retry:
    client_fd = socket_local_client(name,namespaceId,type);
    if(client_fd < 0)
    {
        if(retry_times < 5)
        {
            ALOGE("Failed to connect kisd,retry after 2 seconds...\n");
            sleep(2);
            retry_times++;
            goto retry;
        }
        else
        {
            ALOGE("Failed to connect kisd!\n");
            return client_fd;
        }
    }
    return client_fd;
}

int process_encrypt_key_block ( unsigned char *encryptKbk, unsigned int inLength,
                                          unsigned char **reEncryptKbk, unsigned int *outLength )
{
    TZ_RESULT tz_ret = 0;
    UREE_SESSION_HANDLE mem_session;
    UREE_SESSION_HANDLE drm_session;
    UREE_SHAREDMEM_PARAM shm_param;
    MTEEC_PARAM param[4];

    tz_ret = UREE_CreateSession(TZ_TA_MEM_UUID, &mem_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CreateMemSession Error: %d\n", tz_ret);
        return -1;
    }

    shm_param.buffer = encryptKbk;
    shm_param.size = inLength;
    tz_ret = UREE_RegisterSharedmemWithTag(mem_session, &param[0].memref.handle, &shm_param, MEM_TAG);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("Register encryptKbk Error: %d\n", tz_ret);
        return -1;
    }
    param[0].memref.offset = 0;
    param[0].memref.size = inLength;

    param[1].value.a = inLength;
    param[1].value.b = 0;

    //reserved 1MB to record keyblock install log
    *reEncryptKbk = ( unsigned char * ) malloc (KEYBLOCK_SIZE * 2); //MALLOC

    shm_param.buffer = *reEncryptKbk;
    shm_param.size = KEYBLOCK_SIZE * 2;
    tz_ret = UREE_RegisterSharedmemWithTag(mem_session, &param[2].memref.handle, &shm_param, MEM_TAG);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("Register reEncryptKbk Error: %d\n", tz_ret);
        return -1;
    }
    param[2].memref.offset = 0;
    param[2].memref.size = KEYBLOCK_SIZE * 2;

    param[3].value.a = 0;
    param[3].value.b = 0;

    tz_ret = UREE_CreateSession(TZ_TA_DRMKEY_UUID, &drm_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CreateSession Error: %d\n", tz_ret);
        return -1;
    }

    LOGE("Enter UREE_TeeServiceCall\n");
    tz_ret = UREE_TeeServiceCall(drm_session, TZCMD_DRMKEY_INSTALL,
                                 TZ_ParamTypes4(TZPT_MEMREF_INPUT, TZPT_VALUE_INPUT, TZPT_MEMREF_OUTPUT,TZPT_VALUE_OUTPUT), param);
    LOGE("Leave UREE_TeeServiceCall\n");

    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("TZCMD_DRMKEY_INSTALL Error: %d\n", tz_ret);
        return -2;
    }

    *outLength = param[3].value.a;

    tz_ret = UREE_CloseSession(drm_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CloseSession Error: %d\n", tz_ret);
        return -3;
    }

    tz_ret = UREE_UnregisterSharedmem(mem_session, param[2].memref.handle);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("UnRegister reEncryptKbk Error: %d\n", tz_ret);
        return -1;
    }

    tz_ret = UREE_UnregisterSharedmem(mem_session, param[0].memref.handle);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("UnRegister encryptKbk Error: %d\n", tz_ret);
        return -1;
    }

    tz_ret = UREE_CloseSession(mem_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CloseSession Error: %d\n", tz_ret);
        return -3;
    }
    return 0;
}

int free_keyblock_impl(unsigned char* kb)
{
     if ( kb != NULL ) {
        free ( kb );
    }

    return 0;
}

#define free_keyblock(kb) \
    ({ \
    int ret = free_keyblock_impl(kb); \
    kb = NULL; \
    ret; \
    })

#define SERVER_NAME "kisd"

#include "../kisd/kisd.h"
ssize_t full_read(int fd, void *buf, size_t count)
{
    int n_ret = 0;
    unsigned int n_read = 0;
    if((buf == NULL) || (count >= (2*KEYBLOCK_SIZE))){
        LOGE("invalid input for full_read!\n");
        return -1;
    }

    do
    {
        n_ret = read(fd,((unsigned char*)buf)+n_read,count-n_read);
        if(n_ret == -1)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }

        }
        n_read += n_ret;
        n_ret = 0;
    }while(n_read < count);
    return n_read;
}

ssize_t full_write(int fd, void *buf, size_t count)
{
    int n_ret = 0;
    unsigned int n_write = 0;

    if((buf == NULL) || (count >= (2*KEYBLOCK_SIZE))){
        LOGE("invalid input for full_write!\n");
        return -1;
    }

    do
    {
        n_ret = write(fd,((unsigned char*)buf)+n_write,count-n_write);
        if(n_ret == -1)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }

        }
        n_write += n_ret;
        n_ret = 0;
    }while(n_write < count);
    return n_write;
}

int flash_read_rpc_agent(char* path,unsigned char* buff,unsigned int len)
{
    READ_WRITE_COMMAND_T* command = NULL;
    READ_WRITE_COMMAND_RESULT_T result;
    memset(&result,0,sizeof(READ_WRITE_COMMAND_RESULT_T));
    int client_fd = -1;
    int ret = 0;
    if(path == NULL){
        LOGE("invalid input path for flash_read!\n");
        return -1;
    }
    if((buff == NULL) || (len == 0)){
        LOGE("invalid input buffer for flash_read!\n");
        return -1;
    }

    command = (READ_WRITE_COMMAND_T*)calloc(sizeof(READ_WRITE_COMMAND_T),sizeof(char));
    if(!command)
    {
        LOGE("Allocate memory failed for command!\n");
        return -1;
    }
    if(strlen(path)>=64)
    {
        LOGE("Path is too long!\n");
        free(command);
        return -1;
    }
    strncpy(command->path,path,63);
    command->data_len = len;
    command->header.command = FLASH_READ;
    command->header.total_len = sizeof(READ_WRITE_COMMAND_T);

    client_fd = socket_local_client_retry(SERVER_NAME,ANDROID_SOCKET_NAMESPACE_ABSTRACT,SOCK_STREAM);
    if(client_fd < 0)
    {
        FUNC_WRAPPER(1,0);
        free(command);
        return -2;
    }
    ret = full_write(client_fd,command,command->header.total_len);
    if(ret < 0)
    {
        FUNC_WRAPPER(1,0);
        close(client_fd);
        free(command);
        return -3;
    }
    ret = full_read(client_fd,&result,sizeof(READ_WRITE_COMMAND_RESULT_T));
    if(ret < 0)
    {
        FUNC_WRAPPER(1,0);
        close(client_fd);
        free(command);
        return -4;
    }
    if(result.header.result != RESULT_OK || result.data_len != len)
    {
        LOGE("flash_read_rpc_agent fail!\n");
        close(client_fd);
        free(command);
        return -5;
    }
    if(full_read(client_fd,buff,result.data_len)!=result.data_len)
    {
        LOGE("flash_read_rpc_agent read length is fail!\n");
        close(client_fd);
        free(command);
        return -6;
    }
    close(client_fd);
    free(command);
    return 0;
}

int flash_write_rpc_agent(char* path,unsigned char* buff,unsigned int len)
{
    READ_WRITE_COMMAND_T* command = NULL;
    READ_WRITE_COMMAND_RESULT_T result;
    memset(&result,0,sizeof(READ_WRITE_COMMAND_RESULT_T));
    int client_fd = -1;
    int ret = 0;
    if(path == NULL){
        LOGE("invalid input path for flash_write!\n");
        return -1;
    }
    if((buff == NULL) || (len == 0)){
        LOGE("invalid input buffer for flash_write!\n");
        return -1;
    }

    command = (READ_WRITE_COMMAND_T*)calloc(sizeof(READ_WRITE_COMMAND_T)+len,sizeof(char));
    if(!command)
    {
        LOGE("Allocate memory failed for command!\n");
        return -1;
    }
    if(strlen(path)>=64)
    {
        LOGE("Path is too long!\n");
        free(command);
        return -1;
    }
    strncpy(command->path,path,63);
    command->data_len = len;
    command->header.command = FLASH_WRITE;
    command->header.total_len = sizeof(READ_WRITE_COMMAND_T)+len;
    memcpy((unsigned char*)(command+1),buff,len);
    client_fd = socket_local_client_retry(SERVER_NAME,ANDROID_SOCKET_NAMESPACE_ABSTRACT,SOCK_STREAM);
    if(client_fd < 0)
    {
        FUNC_WRAPPER(1,0);
        free(command);
        return -2;
    }
    ret = full_write(client_fd,command,command->header.total_len);
    if(ret < 0)
    {
        FUNC_WRAPPER(1,0);
        close(client_fd);
        free(command);
        return -3;
    }
    ret = full_read(client_fd,&result,sizeof(READ_WRITE_COMMAND_RESULT_T));
    if(ret < 0)
    {
        FUNC_WRAPPER(1,0);
        close(client_fd);
        free(command);
        return -4;
    }
    if(result.header.result != RESULT_OK || result.data_len != len)
    {
        LOGE("flash_write_rpc_agent fail!\n");
        close(client_fd);
        free(command);
        return -2;
    }
    close(client_fd);
    free(command);
    return 0;
}

int earse_keyblock_local_emmc(char* KBPath)
{
    unsigned char* dummy_kb=(unsigned char*)malloc(KEYBLOCK_SIZE);
    int len = KEYBLOCK_SIZE;
    int fd = 0, n=0,of=0;

    if(dummy_kb == NULL)
    {
        LOGE("allocate dummy_kb fail.");
        return -1;
    }
    memset(dummy_kb,0,len);

//write
    fd = open(KBPath, O_RDWR);
    if(fd <0)
    {
        LOGE("ERROR open fail %d\n",fd);
        free(dummy_kb);
        dummy_kb = NULL;
        return -3;
    }
    of=lseek(fd,0,SEEK_SET);
    n=write(fd,dummy_kb,len);
    if(n!=len){
        LOGE("ERROR write fail");
        close(fd);
        free(dummy_kb);
        dummy_kb = NULL;
        return -4;
    }
    close(fd);

    if (dummy_kb != NULL)
    {
        LOGE("free dummy_kb");
        free(dummy_kb);
        dummy_kb = NULL;
    }

    return 0;
}

#include <fcntl.h>
#include <mtd/mtd-user.h>
#include <sys/ioctl.h>
int earse_keyblock_local_nand(char* KBPath)
{
/*
    int dev_fd;
    erase_info_t erase;
    mtd_info_t mtd;
    dev_fd = open(KBPath,O_SYNC|O_RDWR);
    if(dev_fd < 0)
    {
        LOGE("open %s failed\n",KBPath);
        return -1;
    }
    if(ioctl(dev_fd,MEMGETINFO,&mtd)<0)
    {
        LOGE("%s:MTD getinfo failed\n",KBPath);
        close(dev_fd);
        return -2;
    }
    erase.start = 0;
    erase.length = mtd.size;

    if(ioctl(dev_fd,MEMERASE,&erase)<0)
    {
        LOGE("%s:MTD erase failed\n",KBPath);
        close(dev_fd);
        return -3;
    }

    if (dev_fd >= 0)
    {
        close(dev_fd);
    }
*/
    unsigned char* dummy_kb=(unsigned char*)malloc(KEYBLOCK_SIZE);
    int len = KEYBLOCK_SIZE;
    int fd = 0, n=0,of=0;

    if(dummy_kb == NULL)
    {
        LOGE("allocate dummy_kb fail.");
        return -1;
    }
    memset(dummy_kb,0,len);

//write
    fd = open(KBPath, O_RDWR);
    if(fd <0)
    {
        LOGE("ERROR open nand fail %d\n",fd);
        free(dummy_kb);
        return -3;
    }
    of=lseek(fd,0,SEEK_SET);
    n=write(fd,dummy_kb,len);
    if(n!=len){
        LOGE("ERROR write nand fail");
        close(fd);
        free(dummy_kb);
        return -4;
    }
    close(fd);

    if (dummy_kb != NULL)
    {
        LOGE("free dummy_kb fail");
        free(dummy_kb);
    }

    return 0;
}

int earse_keyblock_rpc(char* KBPath)
{
    unsigned char* dummy_kb=(unsigned char*)malloc(KEYBLOCK_SIZE);
    int len = KEYBLOCK_SIZE;
    int ret = 0;

    if(dummy_kb == NULL)
    {
        LOGE("allocate dummy_kb fail.");
        return -1;
    }
    memset(dummy_kb,0,len);

    ret= FUNC_WRAPPER(
        flash_write_rpc_agent(KBPath,dummy_kb,len),
        0);

    free(dummy_kb);
    return ret;
}

int (*earse_keyblock)(char*) = earse_keyblock_rpc;

unsigned char* get_keyblock_from_nvram_impl_rpc(char* KBPath)
{
    unsigned char* buff = NULL;
    buff = ( unsigned char * ) malloc ( KEYBLOCK_SIZE );
    if(buff == NULL)
    {
        ALOGE("%d bytes memory allcation fail at %s:%d\n",KEYBLOCK_SIZE,__FILE__,__LINE__);
        return NULL;
    }
    memset(buff,0,KEYBLOCK_SIZE);
    FUNC_WRAPPER(
        flash_read_rpc_agent(KBPath,buff,KEYBLOCK_SIZE),
        0);
    if(memcmp(buff,"KBP",3)!= 0)
    {
        free(buff);
        return NULL;
    }
    return buff;
}

int store_keyblock_to_nvram_impl_rpc(char* KBPath,unsigned char* kb,int len)
{
    int ret = 0;
    if((len>KEYBLOCK_SIZE) || (len <= 0))
    {
        LOGE("Keyblock is larger than 1MB,pls adjust partition size!\n");
        return -1;
    }
    ret= FUNC_WRAPPER(
        flash_write_rpc_agent(KBPath,kb,len),
        0);
    return ret;
}

int keyblock_signature_operation(unsigned char* kb,int kb_size,unsigned char* sig,KB_SigOperaionType op)
{
    TZ_RESULT tz_ret = 0;
    UREE_SESSION_HANDLE mem_session;
    UREE_SESSION_HANDLE drm_session;
    UREE_SHAREDMEM_PARAM shm_param;
    MTEEC_PARAM param[4];

    tz_ret = UREE_CreateSession(TZ_TA_MEM_UUID, &mem_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CreateMemSession Error: %d\n", tz_ret);
        return -1;
    }

    shm_param.buffer = kb;
    shm_param.size = kb_size;
    tz_ret = UREE_RegisterSharedmemWithTag(mem_session, &param[0].memref.handle, &shm_param, MEM_TAG);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("Register kb buffer Error: %d\n", tz_ret);
        return -1;
    }
    param[0].memref.offset = 0;
    param[0].memref.size = kb_size;

    param[1].value.a = kb_size;
    param[1].value.b = op;

    param[2].mem.buffer = sig;
    param[2].mem.size = KB_SIGNATURE_SIZE;

    param[3].value.a = 0;
    param[3].value.b = 0;

    tz_ret = UREE_CreateSession(TZ_TA_DRMKEY_UUID, &drm_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CreateSession Error: %d\n", tz_ret);
        return -1;
    }

    //LOGE("Enter UREE_TeeServiceCall\n");
    if(op == SigOperaionType_ADD)
    {
        tz_ret = UREE_TeeServiceCall(drm_session, TZCMD_DRMKEY_SIGNATURE_OP,
                TZ_ParamTypes4(TZPT_MEMREF_INPUT, TZPT_VALUE_INPUT, TZPT_MEM_OUTPUT,TZPT_VALUE_OUTPUT), param);
    }
    else
    {
        tz_ret = UREE_TeeServiceCall(drm_session, TZCMD_DRMKEY_SIGNATURE_OP,
                TZ_ParamTypes4(TZPT_MEMREF_INPUT, TZPT_VALUE_INPUT, TZPT_MEM_INPUT,TZPT_VALUE_OUTPUT), param);
    }
    //LOGE("Leave UREE_TeeServiceCall\n");

    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("TZCMD_DRMKEY_SIGNATURE_OP Error: %d  OP: %d\n", tz_ret,op);
        return -2;
    }

    tz_ret = UREE_CloseSession(drm_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CloseSession Error: %d\n", tz_ret);
        return -3;
    }

    tz_ret = UREE_UnregisterSharedmem(mem_session, param[0].memref.handle);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("UnRegister kb buffer Error: %d\n", tz_ret);
        return -1;
    }

    tz_ret = UREE_CloseSession(mem_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CloseSession Error: %d\n", tz_ret);
        return -3;
    }

    //param[3].value.a == 0 -->OK, param[3].value.a !=0 -->FAIL
    return param[3].value.a;

}

int keyblock_signature_add(unsigned char* kb,unsigned char* sig)
{
    int kb_size = *(int*)(kb+OFFSET_REENCKB_HEADER_RESERVED);
    //update backup size
    *(int*)(kb+OFFSET_REENCKB_HEADER_RESERVED+sizeof(kb_size)) = kb_size + KB_SIGNATURE_SIZE;
    return keyblock_signature_operation(kb,kb_size,sig,SigOperaionType_ADD);
}

int keyblock_signature_check(unsigned char* kb)
{
    int kb_size = *(int*)(kb+OFFSET_REENCKB_HEADER_RESERVED);
    unsigned char* kb_sig=(unsigned char*)(kb+kb_size);

    return keyblock_signature_operation(kb,kb_size,kb_sig,SigOperaionType_VERIFY);
}

unsigned char* get_keyblock_from_nvram_impl_local(char* KBPath)
{
    FILE *a;
    unsigned char* buff = NULL;
    int readLen = 0;
    //struct stat f_info;
    a = fopen(KBPath, "rb");

    if (a == NULL) {
        LOGE("open %s fail\n",KBPath);
        return NULL;
    }

    //stat ( KEYBLOCK_PATH, &f_info );
    //buff = ( unsigned char * ) malloc ( f_info.st_size );
    buff = (unsigned char *) malloc (KEYBLOCK_SIZE);
    if (buff == NULL) {
        ALOGE("%d bytes memory allcation fail at %s:%d\n",KEYBLOCK_SIZE,__FILE__,__LINE__);
        fclose(a);
        return NULL;
    }

    //fread ( buff, f_info.st_size, 1, a );
    readLen = fread(buff, KEYBLOCK_SIZE, 1, a);
    if(readLen != 1)
    {
        //just warning
        LOGE("failed to read file,read len = %d.\n",readLen);
    }

    fclose(a);

    if(memcmp(buff,"KBP",3)!= 0 )
    {
        //ALOGE("keyblock header is not vailded,maybe blank\n");
        free(buff);
        return NULL;
    }

    if(keyblock_signature_check(buff)!=0)
    {
        memset(buff,0,KEYBLOCK_SIZE);
        LOGE("keyblock %s signature is not vailded\n",KBPath);
        factory_provision_str("Keyblock signature verify fail!");
    }
    return buff;
}


int store_keyblock_to_nvram_impl_local(char* KBPath,unsigned char* kb,int len)
{
    int fd = 0, n=0,of=0;
    unsigned char* signed_kb = NULL;
    int signed_kb_len = 0;

    if(len>KEYBLOCK_SIZE)
    {
        LOGE("Keyblock is larger than 1MB,pls adjust partition size!\n");
        return -1;
    }

    signed_kb_len = len+KB_SIGNATURE_SIZE;
    signed_kb = (unsigned char*)malloc(KEYBLOCK_SIZE);

    if(signed_kb == NULL)
    {
        LOGE("alloc buffer for signed_kb fail!\n");
        return -2;
    }
    memset(signed_kb,0,KEYBLOCK_SIZE);

    factory_provision_str("Preparing for wrting keyblock %s.......\n",KBPath);

    if(memcmp(signed_kb,kb,len)==0)
    {
        //all zero case
        //do nothing
        factory_provision_str("Earsing keyblock case,keyblock %s will be easred!\n",KBPath);
    }
    else
    {
        memcpy(signed_kb,kb,len);
        keyblock_signature_add(signed_kb,signed_kb+len);
    }

    if( earse_keyblock(KBPath) != 0)
    {
        LOGE("Earse keyblock partition fail:%s!\n",KBPath);

        if (signed_kb != NULL)
        {
            free(signed_kb);
            signed_kb = NULL;
        }

        return -2;
    }
//write
    factory_provision_str("Writing keyblock....\n");
    fd = open(KBPath, O_RDWR);
    if(fd <0)
    {
        LOGE("ERROR open fail %d\n",fd);

        if (signed_kb != NULL)
        {
            free(signed_kb);
            signed_kb = NULL;
        }

        return -3;
    }
    of=lseek(fd,0,SEEK_SET);
    n=write(fd,signed_kb,KEYBLOCK_SIZE); //32 is kb signature

    if(n!=KEYBLOCK_SIZE){
        LOGE("ERROR write fail");
        free(signed_kb);
        close(fd);
        return -4;
    }
    free(signed_kb);
    close(fd);
    factory_provision_str("Write keyblock successfully\n");
    return 0;
}


unsigned char* (*get_keyblock_from_nvram_impl)(char*) = get_keyblock_from_nvram_impl_rpc;

int (*store_keyblock_to_nvram_impl)(char* ,unsigned char*,int) = store_keyblock_to_nvram_impl_rpc;

unsigned char* get_keyblock_from_nvram(void)
{
    return get_keyblock_from_nvram_impl(KEYBLOCK_F_PATH);
}

unsigned char* get_keyblock_ota_from_nvram(void)
{
    return get_keyblock_from_nvram_impl(KEYBLOCK_O_PATH);
}

int store_keyblock_to_nvram(unsigned char* kb,int len)
{
    return store_keyblock_to_nvram_impl(KEYBLOCK_F_PATH,kb,len);
}

int store_keyblock_ota_to_nvram(unsigned char* kb,int len)
{
    return store_keyblock_to_nvram_impl(KEYBLOCK_O_PATH,kb,len);
}

int get_clearDrmkey_size( unsigned int keyID, unsigned int *leng)
{
    ENCRYPT_DRM_KEY_T encryptDrmHeader;
    int keycount;
    int keyindex = -1;
    int i=0,j=0;
    unsigned char *p=NULL,*kb_buff=NULL;
    int keyblockLeng;

#if 0
    char KBPaths[2][64] = {KEYBLOCK_O_PATH,KEYBLOCK_F_PATH};
#else
    char KBPaths[2][128];
    memset(KBPaths, 0, sizeof(KBPaths));

    strncpy(KBPaths[0], KEYBLOCK_O_PATH, strlen(KEYBLOCK_O_PATH));
    strncpy(KBPaths[1], KEYBLOCK_F_PATH, strlen(KEYBLOCK_F_PATH));

    //LOGE("[get_clearDrmkey_size]KBPaths[0] = %s\n", KBPaths[0]);
    //LOGE("[get_clearDrmkey_size]KBPaths[1] = %s\n", KBPaths[1]);
#endif
    // test, get re-encrypt key block from memory.
    //////////////////////////////

    for(j=0;j<2;j++)
    {
        kb_buff = p = get_keyblock_from_nvram_impl(KBPaths[j]);
        if(kb_buff == NULL)
            continue;
        p+= OFFSET_REENCKB_HEADER_KEYCOUNT;
        keycount = * ( unsigned int * ) p;
        p += SZ_REENCKB_HEADER_KEYCOUNT; // drm key count
        //////////////////////////////

        for ( i = 0; i < keycount; i++ ) {
            memcpy ( &encryptDrmHeader, p, sizeof ( ENCRYPT_DRM_KEY_T ) );
            // drmkey type id(4) + drmkey size(4) + encrypt drmkey size(4) + reserved(64) + encrypt drmkey + plain drmkey signature
            keyblockLeng = SZ_DRMKEY_HEADER_SIZE + encryptDrmHeader.encDrmKeySize + SZ_DRMKEY_SIG;

            if ( encryptDrmHeader.drmKeyID == keyID ) {
                keyindex = i;
                break;
            }

            p += keyblockLeng;
        }

        if ( keyindex == -1 )
        { // can not find requested DRM key
            if(j==0)
            {
                free_keyblock(kb_buff);
            }
            continue;
        }

        // we find the requested DRM key,
        // p is the start address, keyblockLeng is the specific total drm key block size

        *leng = *(unsigned int *)(p+8);
        break;
    }
    free_keyblock (kb_buff);
    if(keyindex == -1)
    {
        *leng = 0;
        return -1;
    }
    else
    {
        return 0;
    }
}

// Key manager get encrypted key
int get_encrypt_drmkey ( unsigned int keyID, unsigned char **oneDrmkeyBlock, unsigned int *blockLeng )
{
    ENCRYPT_DRM_KEY_T encryptDrmHeader;
    int keycount;
    int keyindex = -1;
    int i=0,j=0;
    unsigned char *p=NULL,*kb_buff=NULL;
    int keyblockLeng;

#if 0
    char KBPaths[2][64] = {KEYBLOCK_O_PATH,KEYBLOCK_F_PATH};
#else
    char KBPaths[2][128];
    memset(KBPaths, 0, sizeof(KBPaths));

    strncpy(KBPaths[0], KEYBLOCK_O_PATH, strlen(KEYBLOCK_O_PATH));
    strncpy(KBPaths[1], KEYBLOCK_F_PATH, strlen(KEYBLOCK_F_PATH));

    //LOGE("[get_encrypt_drmkey]KBPaths[0] = %s\n", KBPaths[0]);
    //LOGE("[get_encrypt_drmkey]KBPaths[1] = %s\n", KBPaths[1]);
#endif
    // test, get re-encrypt key block from memory.
    //////////////////////////////

    for(j=0;j<2;j++)
    {
        kb_buff = p = get_keyblock_from_nvram_impl(KBPaths[j]);
        if(kb_buff == NULL)
            continue;
        p+= OFFSET_REENCKB_HEADER_KEYCOUNT;
        keycount = * ( unsigned int * ) p;
        p += SZ_REENCKB_HEADER_KEYCOUNT; // drm key count
        //////////////////////////////

        for ( i = 0; i < keycount; i++ ) {
            memcpy ( &encryptDrmHeader, p, sizeof ( ENCRYPT_DRM_KEY_T ) );
            // drmkey type id(4) + drmkey size(4) + encrypt drmkey size(4) + reserved(64) + encrypt drmkey + plain drmkey signature
            keyblockLeng = SZ_DRMKEY_HEADER_SIZE + encryptDrmHeader.encDrmKeySize + SZ_DRMKEY_SIG;

            if ( encryptDrmHeader.drmKeyID == keyID ) {
                keyindex = i;
                break;
            }

            p += keyblockLeng;
        }

        if ( keyindex == -1 )
        { // can not find requested DRM key
            if(j==0)
            {
                free_keyblock(kb_buff);
            }
            continue;
        }

        // we find the requested DRM key,
        // p is the start address, keyblockLeng is the specific total drm key block size
        *oneDrmkeyBlock = ( unsigned char * ) malloc ( keyblockLeng );
        if(*oneDrmkeyBlock)
        {
            memcpy ( *oneDrmkeyBlock, p, keyblockLeng );
            *blockLeng = keyblockLeng;
            break;
        }else{
            free_keyblock (kb_buff);
            LOGE("Allocate memory failed!\n");
            return -1;
        }
    }
    free_keyblock (kb_buff);
    if(keyindex == -1)
    {
        *oneDrmkeyBlock = NULL;
        *blockLeng = 0;
        return -1;
    }
    else
    {
        return 0;
    }
}

// Key manager FREE encrypted key
int free_encrypt_drmkey ( unsigned char *oneEncDrmkeyBlock )
{
    if ( oneEncDrmkeyBlock != NULL ) {
        free ( oneEncDrmkeyBlock );
    }

    return 0;
}

int free_encrypt_key_block ( unsigned char *reEncryptKbk )
{
    if ( reEncryptKbk != NULL ) {
        free ( reEncryptKbk );
    }

    return 0;
}

int decrypt_drmkey ( unsigned char *encDrmKeyBlock, unsigned int inLength,
                     unsigned char **DrmKey, unsigned int *outLength )
{
    LOGE("Error! TEE DRM is enabled,can not decrypt drm key in normal !\n");
    return -1;
}

int free_drmkey ( unsigned char *drmkey )
{
    LOGE("Error! TEE DRM is enabled,can not free drm key in normal !\n");
    return -1;
}

#define KEYTYPE_ARRAY_SIZE 512*sizeof(unsigned int)
int query_drmkey ( unsigned int *count, unsigned int *keytype )
{
    unsigned char *kb_buff;
    unsigned int kb_len = 0;
    TZ_RESULT tz_ret = 0;
    UREE_SESSION_HANDLE mem_session;
    UREE_SESSION_HANDLE drm_session;
    UREE_SHAREDMEM_PARAM shm_param;
    MTEEC_PARAM param[4];

    tz_ret = UREE_CreateSession(TZ_TA_MEM_UUID, &mem_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CreateMemSession Error: %d\n", tz_ret);
        return -1;
    }
    // test, get re-encrypt key block from memory.
    //////////////////////////////
    {
        unsigned char* tmp_kb_buffs[2];
        unsigned int tmp_kb_lengths[2];
        tmp_kb_buffs[0]=get_keyblock_ota_from_nvram();
        tmp_kb_lengths[0] = KEYBLOCK_SIZE;
        tmp_kb_buffs[1]=get_keyblock_from_nvram();
        tmp_kb_lengths[1] = KEYBLOCK_SIZE;
        FUNC_WRAPPER(
            merge_drm_buffs("KBPM",tmp_kb_buffs,tmp_kb_lengths,2,NULL,0,&kb_buff,&kb_len),
            0);
        free_keyblock(tmp_kb_buffs[0]);
        free_keyblock(tmp_kb_buffs[1]);
    }

    shm_param.buffer = kb_buff;
    shm_param.size = KEYBLOCK_SIZE;
    tz_ret = UREE_RegisterSharedmemWithTag(mem_session, &param[0].memref.handle, &shm_param, MEM_TAG);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        if(kb_buff)
            free_keyblock(kb_buff);
        LOGE("Register kb_buff Error: %d\n", tz_ret);
        return -1;
    }
    param[0].memref.offset = 0;
    param[0].memref.size = KEYBLOCK_SIZE;

    param[1].value.a = 0;
    param[1].value.b = 0;

    shm_param.buffer = keytype;
    shm_param.size = KEYTYPE_ARRAY_SIZE;
    tz_ret = UREE_RegisterSharedmemWithTag(mem_session, &param[2].memref.handle, &shm_param, MEM_TAG);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("Register keytype Error: %d\n", tz_ret);
        return -1;
    }
    param[2].memref.offset = 0;
    param[2].memref.size = KEYTYPE_ARRAY_SIZE;

    tz_ret = UREE_CreateSession(TZ_TA_DRMKEY_UUID, &drm_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CreateSession Error: %d\n", tz_ret);
        return -1;
    }

    tz_ret = UREE_TeeServiceCall(drm_session, TZCMD_DRMKEY_QUERY,
            TZ_ParamTypes3(TZPT_MEMREF_INPUT, TZPT_VALUE_OUTPUT, TZPT_MEMREF_OUTPUT), param);

    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("TZCMD_DRMKEY_QUERY Error: %d\n", tz_ret);
        return -2;
    }

    *count = param[1].value.a;

    free_keyblock(kb_buff);

    tz_ret = UREE_CloseSession(drm_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CloseSession Error: %d\n", tz_ret);
        return -3;
    }

    tz_ret = UREE_UnregisterSharedmem(mem_session, param[2].memref.handle);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("UnRegister keytype Error: %d\n", tz_ret);
        return -1;
    }

    tz_ret = UREE_UnregisterSharedmem(mem_session, param[0].memref.handle);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("UnRegister kb_buff Error: %d\n", tz_ret);
        return -1;
    }

    tz_ret = UREE_CloseSession(mem_session);
    if (tz_ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CloseSession Error: %d\n", tz_ret);
        return -3;
    }

    #if DRMKEY_FLOW_TEST
    {
        unsigned char* drmkey;
        unsigned int len = 0;
        static int test_mode = 0;
        extern int write_kbo_drmkey(DRMKeyID id,unsigned char* enckey,unsigned int length);
        extern int delete_kbo_drmkey(DRMKeyID id);
        if(test_mode %2 == 0)
        {
            FUNC_WRAPPER( get_encrypt_drmkey(WIDEVINE_ID,&drmkey,&len), 0);
            if(drmkey != NULL)
            {
                FUNC_WRAPPER(
                    write_kbo_drmkey(WIDEVINE_ID,drmkey,len),
                    0);
            }
        }
        else
        {
            FUNC_WRAPPER(
                delete_kbo_drmkey(WIDEVINE_ID),
                0);
        }
        test_mode ++;
    }
    #endif
    return 0;
}

/*
[in] id            DRMKeyID

return   0: OK,  others: FAIL
*/
int query_drmkey_exist(DRMKeyID id)
{
    unsigned int  keycount = 0;
    unsigned int  keytype[512] = {0};
    int ret = 0;
    unsigned int i = 0;
    if((ret=query_drmkey(&keycount,keytype))!=0)
    {
        LOGE("Query drmkey type fail: %d\n", ret);
        return -1;
    }
    for(i=0;i<keycount;i++)
    {
        if(keytype[i]==id)
        {
            return 0;
        }
    }
    return -1;
}

typedef struct
{
    unsigned char magic[4];
    unsigned char Reserved[64];
    unsigned int keycount;
}KBHeader_T;

//magic must be "KBPF" or "KBPO"
#define KB_TEMP_DIR         "/data/vendor/key_provisioning/"
#define KBO_TEMP_FILE_PATH     KB_TEMP_DIR"/KBO_BIN"
#define KBF_TEMP_FILE_PATH     KB_TEMP_DIR"/KBF_BIN"
#define KB_EKKB_PATH         KB_TEMP_DIR"/KB_EKKB"
#define FORCE_KB_EKKB_PATH     KB_TEMP_DIR"/FORCE_KB_EKKB"
#define KB_PM_PATH             KB_TEMP_DIR"/KB_PM"
#define FORCE_KB_PM_PATH     KB_TEMP_DIR"/FORCE_KB_PM"
#define KB_PD_PATH             KB_TEMP_DIR"/KB_PD"
#define KB_MIX_PATH         KB_TEMP_DIR"/KB_MIX"
#define KB_OTA_PATH         KB_TEMP_DIR"/KB_OTA"

int resotre_kb_from_file(char* temp_file_path,char* target_path)
{
    unsigned char* merged_kb=NULL;
    unsigned int len = 0;
    int ret = 0;

    ret = FUNC_WRAPPER(
        read_file_to_buff(&merged_kb,&len,temp_file_path),
        0);
    if(ret != 0) return -1;

    ret = FUNC_WRAPPER(
        store_keyblock_to_nvram_impl(target_path,merged_kb,len),
        0);
    if(ret != 0) return -2;

    free_keyblock(merged_kb);

    return 0;
}
int restore_kbo_from_file(void)
{
    return resotre_kb_from_file(KBO_TEMP_FILE_PATH,KEYBLOCK_O_PATH);
}

int restore_kbf_from_file(void)
{
    return resotre_kb_from_file(KBF_TEMP_FILE_PATH,KEYBLOCK_F_PATH);
}

int merge_drm_buffs(char* kb_magic,unsigned char* kb_buff[],unsigned int kb_length[],unsigned int kb_num,DRMKeyID id_filter[],unsigned int filter_num,unsigned char** out_kb,unsigned int* out_length)
{
    unsigned int i=0,total_length = 0;
    unsigned char* pout_kb = NULL;
    for(i=0;i<kb_num;i++)
    {
        total_length+=kb_length[i];
    }
    total_length += SZ_REENCKB_TOTAL_HEADER;
    *out_kb = (unsigned char*)malloc(total_length);
    if(*out_kb == NULL)
    {
        LOGE("Alloc memory fail: %d\n", total_length);
        return -1;
    }
    memset(*out_kb,0,total_length);
    pout_kb = *out_kb + sizeof(KBHeader_T);
    memcpy(*out_kb,kb_magic,SZ_REENCKB_HEADER);
    for(i=0;i<kb_num;i++)
    {
        unsigned char* p = kb_buff[i];
        if(p == NULL)
        {
            continue;
        }
        if(memcmp(p,"KBP",3)==0)
        {
            //kb_buff is keyblock
            //cannot copy all kb_buff directly.because keyblock read from Partition is always 1MB
            KBHeader_T* header = (KBHeader_T*)p;
            p+=sizeof(KBHeader_T);
            unsigned int j = 0;
            for(j=0;j<header->keycount;j++)
            {
                ENCRYPT_DRM_KEY_T* drmkey = (ENCRYPT_DRM_KEY_T*)p;
                unsigned int size = SZ_DRMKEY_HEADER_SIZE+drmkey->encDrmKeySize+SZ_DRMKEY_SIG;
                unsigned int k = 0;
                for(k=0;k<filter_num;k++)
                {
                    if(drmkey->drmKeyID == id_filter[k])
                    {
                        break;
                    }
                }
                if( k == filter_num)
                {
                    memcpy(pout_kb,p,size);
                    pout_kb += size;
                    ((KBHeader_T*)(*out_kb))->keycount++;
                }
                p += size;
            }
        }
        else
        {
            ENCRYPT_DRM_KEY_T* drmkey = (ENCRYPT_DRM_KEY_T*)p;
            unsigned int size = SZ_DRMKEY_HEADER_SIZE+drmkey->encDrmKeySize+SZ_DRMKEY_SIG;
            unsigned int k = 0;
            for(k=0;k<filter_num;k++)
            {
                if(drmkey->drmKeyID == id_filter[k])
                {
                    break;
                }
            }
            if( k == filter_num)
            {
                memcpy(pout_kb,p,size);
                pout_kb += size;
                ((KBHeader_T*)(*out_kb))->keycount++;
            }
        }
    }
    *out_length = pout_kb - *out_kb;
    *(unsigned int*)(((KBHeader_T*)(*out_kb))->Reserved)=*out_length;
    return 0;
}

int write_kbo_drmkey(DRMKeyID id,unsigned char* enckey,unsigned int length)
{
    int ret = 0;
    unsigned char* buffs[1]={NULL};
    unsigned int   lens[1];
    unsigned char* merged_kb=NULL;
    unsigned int   len = 0;

    buffs[0] = enckey;
    lens[0] = length;
    //fliter the key which will be installed
    merge_drm_buffs("KBPO",buffs,lens,1,NULL,0,&merged_kb,&len);

    ret = FUNC_WRAPPER(merge_install_kb(merged_kb,len,KBO_TEMP_FILE_PATH,KEYBLOCK_O_PATH),0);
    free_keyblock(merged_kb);
    if( ret != 0 ) return -1;

    return 0;
}

int delete_kbo_drmkey(DRMKeyID id)
{
    unsigned char* kbbuff=NULL;
    unsigned int len = 0;
    unsigned char* buffs[1]={NULL};
    unsigned int   lens[1];
    unsigned char* merged_kb=NULL;
    DRMKeyID id_filter[1];

        if (drmkey_make_directory((const char *)KB_TEMP_DIR,  0755) != 0)
            LOGE("[delete_kbo_drmkey] drmkey_make_directory() faield\n");

    kbbuff=get_keyblock_ota_from_nvram();
    buffs[0]=kbbuff;
    lens[0]=KEYBLOCK_SIZE;
    id_filter[0]=id;
    merge_drm_buffs("KBPO",buffs,lens,1,id_filter,1,&merged_kb,&len);
    FUNC_WRAPPER(
        write_buff_to_file(merged_kb,len,KBO_TEMP_FILE_PATH),
        0);
    free_keyblock(merged_kb);
    free_keyblock(kbbuff);
    restore_kbo_from_file();

        if (drmkey_remove_file(KBO_TEMP_FILE_PATH) != 0)
            LOGE("[delete_kbo_drmkey] drmkey_remove_file() faield %s\n", KBO_TEMP_FILE_PATH);

        return 0;

}

int write_kbf_drmkey(DRMKeyID id,unsigned char* enckey,unsigned int length)
{
    int ret = 0;
    unsigned char* buffs[1]={NULL};
    unsigned int   lens[1];
    unsigned char* merged_kb=NULL;
    unsigned int   len = 0;

    buffs[0] = enckey;
    lens[0] = length;
    //fliter the key which will be installed
    merge_drm_buffs("KBPF",buffs,lens,1,NULL,0,&merged_kb,&len);

    ret = FUNC_WRAPPER(merge_install_kb(merged_kb,len,KBF_TEMP_FILE_PATH,KEYBLOCK_F_PATH),0);
    free_keyblock(merged_kb);
    if( ret != 0 ) return -1;

    return 0;
}

int delete_kbf_drmkey(DRMKeyID id)
{
    unsigned char* kbbuff=NULL;
    unsigned int len = 0;
    unsigned char* buffs[1]={NULL};
    unsigned int   lens[1];
    unsigned char* merged_kb=NULL;
    DRMKeyID id_filter[1];

        if (drmkey_make_directory((const char *)KB_TEMP_DIR,  0755) != 0)
            LOGE("[delete_kbf_drmkey] drmkey_make_directory() faield\n");

    kbbuff=get_keyblock_from_nvram();
    buffs[0]=kbbuff;
    lens[0]=KEYBLOCK_SIZE;
    id_filter[0]=id;
    merge_drm_buffs("KBPF",buffs,lens,1,id_filter,1,&merged_kb,&len);
    FUNC_WRAPPER(
        write_buff_to_file(merged_kb,len,KBF_TEMP_FILE_PATH),
        0);
    free_keyblock(merged_kb);
    free_keyblock(kbbuff);
    restore_kbf_from_file();

        if (drmkey_remove_file(KBF_TEMP_FILE_PATH) != 0)
            LOGE("[delete_kbf_drmkey] drmkey_remove_file faield %s\n", KBF_TEMP_FILE_PATH);

        return 0;

}

int process_encrypt_kb_ekkb(unsigned char *encryptKB_Ekkb, unsigned int inLength,
                            unsigned char **reEncryptDrmEkkb, unsigned int *outLength)
{
    TZ_RESULT ret;
    UREE_SESSION_HANDLE session;
    MTEEC_PARAM param[4];
    ret = UREE_CreateSession(TZ_TA_DRMKEY_UUID, &session);
    if (ret != TZ_RESULT_SUCCESS)
    {
        // Should provide strerror style error string in UREE.
        LOGE("CreateSession Error: %s\n", TZ_GetErrorString(ret));
        return -1;
    }

    if(inLength != sizeof(KB_EKKB_T)) return -2;

    *outLength = sizeof(ENCRYPT_DRM_EKKB_T);

    *reEncryptDrmEkkb=(unsigned char*)malloc(*outLength+0x400); //reserved 1KB for log

    param[0].mem.buffer = encryptKB_Ekkb;
    param[0].mem.size = inLength;
    param[1].mem.buffer = *reEncryptDrmEkkb;
    param[1].mem.size = *outLength+0x400;

    ret = UREE_TeeServiceCall(session, TZCMD_DRMKEY_GEN_REENC_EKKB,
        TZ_ParamTypes2(TZPT_MEM_INPUT, TZPT_MEM_OUTPUT), param);
    if (ret != TZ_RESULT_SUCCESS)
    {
        LOGE("TZCMD_DRMKEY_GEN_REENC_EKKB Error: %s\n", TZ_GetErrorString(ret));
        return -3;
    }

    ret = UREE_CloseSession(session);
    if (ret != TZ_RESULT_SUCCESS)
    {
        LOGE("CloseSeesion session Error: %d\n", ret);
    }

    return 0;
}

int restruct_for_noEkkb_keyblock(unsigned char *encryptKbk, unsigned int inLength)
{
    unsigned int hasEkkb = *(unsigned int*)(encryptKbk+OFFSET_ENCKB_HEADER_HASEKKB);
    unsigned char*    encDrmEkkb = NULL;
    unsigned int    encDrmEkkbLen = 0;
    int ret = 0;

    if(hasEkkb == 1)
    {
        LOGE("This keyblock has EKKB!!!!\n");
        return -1;
    }

    ret = FUNC_WRAPPER(get_encrypt_drmkey(DRM_SP_EKKB,&encDrmEkkb,&encDrmEkkbLen),0);
    if(ret != 0)
    {
        LOGE("EKKB is not installed!!!!\n");
        return -2;
    }

    memcpy(encryptKbk+OFFSET_ENCKB_HEADER_EKKB,encDrmEkkb,encDrmEkkbLen);

    free_encrypt_drmkey(encDrmEkkb);

    return 0;
}

int install_kb_ekkb(unsigned char* kb_ekkb,unsigned int kb_ekkb_len)
{
    unsigned char* reEncKkb = NULL;
    unsigned int   reEncKkb_len = 0;
    unsigned char*  kb_buffs[1] = {NULL};
    unsigned int   kb_lens[1] = {0};
    unsigned char* merged_kb = NULL;
    unsigned int merged_kb_len = 0;
    int ret = 0;
    factory_provision_str("Start to process KB_EKKB...\n");
    ret = FUNC_WRAPPER(process_encrypt_kb_ekkb(kb_ekkb,kb_ekkb_len,&reEncKkb,&reEncKkb_len),0);
    factory_provision_info(reEncKkb+reEncKkb_len,0x400);
    if(ret != 0)
    {
        factory_provision_str("KB_EKKB process fail...\n");
        return -1;
    }
    kb_buffs[0]=reEncKkb;
    kb_lens[0]=reEncKkb_len;
    FUNC_WRAPPER(merge_drm_buffs("KBPF",kb_buffs,kb_lens,1,NULL,0,&merged_kb,&merged_kb_len),0);

    ret = FUNC_WRAPPER(write_buff_to_file(merged_kb,merged_kb_len,KBF_TEMP_FILE_PATH),0);
    if(ret != 0)
    {
        free_encrypt_drmkey(reEncKkb);
        free_encrypt_key_block(merged_kb);
        return -2;
    }

    free_encrypt_drmkey(reEncKkb);
    free_encrypt_key_block(merged_kb);

    factory_provision_str("KB_EKKB process ok,write it to keyblock...\n");
    ret = FUNC_WRAPPER(restore_kbf_from_file(),0);
    if(ret != 0 )
    {
        factory_provision_str("KB_EKKB can not write to keyblock...\n");
        return -3;
    }

    factory_provision_str("KB_EKKB write to keyblock successfully...\n");

        if (drmkey_remove_file(KBF_TEMP_FILE_PATH) != 0)
            LOGE("[delete_kbo_drmkey] drmkey_remove_file() faield %s\n", KBF_TEMP_FILE_PATH);

        return 0;
}

int query_drmkey_from_encKB(unsigned char* kb_buff,unsigned int *count, DRMKeyID *keytype)
{
    KBHeader_T* header = (KBHeader_T*)kb_buff;
    unsigned int j = 0;
    kb_buff+=sizeof(KBHeader_T);
    *count = 0;
    for(j=0;j<header->keycount;j++)
    {
        ENCRYPT_DRM_KEY_T* drmkey = (ENCRYPT_DRM_KEY_T*)kb_buff;
        unsigned int size = SZ_DRMKEY_HEADER_SIZE+drmkey->encDrmKeySize+SZ_DRMKEY_SIG;
        keytype[(*count)]=drmkey->drmKeyID;
        *count+=1;
        kb_buff += size;
    }
    return 0;
}

int merge_install_kb_rpc(unsigned char* reEncKB,unsigned int reEncKB_len,char* temp_file_path,char* target_path)
{
    MERGE_INSTALL_KB_COMMAND_T* command = NULL;
    MERGE_INSTALL_KB_COMMAND_RESULT_T result;
    int client_fd = -1;
    int ret = 0;
    if((reEncKB == NULL) || (reEncKB_len == 0)){
        LOGE("invalid input recBuffer for merge_install!\n");
        return -1;
    }
    if((temp_file_path == NULL) || (target_path == NULL)){
        LOGE("invalid input path for merge_install!\n");
        return -1;
    }

    command = (MERGE_INSTALL_KB_COMMAND_T*)calloc(sizeof(MERGE_INSTALL_KB_COMMAND_T)+reEncKB_len,sizeof(char));
    if(!command)
    {
        LOGE("merge_install: allocate memory failed!\n");
        return -1;
    }

    if(strlen(temp_file_path)>=64)
    {
        free(command);
        LOGE("temp_file_path is too long!\n");
        return -1;
    }

    if(strlen(target_path)>=64)
    {
        free(command);
        LOGE("target_path is too long!\n");
        return -2;
    }

    strncpy(command->target_path,target_path,63);
    strncpy(command->temp_file_path,temp_file_path,63);
    command->reEncKB_len = reEncKB_len;
    memcpy(command->reEncKB,reEncKB,reEncKB_len);

    command->header.command = MERGE_INSTALL_KB;
    command->header.total_len = sizeof(MERGE_INSTALL_KB_COMMAND_T)+reEncKB_len;

    client_fd = socket_local_client_retry(SERVER_NAME,ANDROID_SOCKET_NAMESPACE_ABSTRACT,SOCK_STREAM);
    if(client_fd < 0)
    {
        FUNC_WRAPPER(1,0);
        free(command);
        return -2;
    }
    ret = full_write(client_fd,command,command->header.total_len);
    if(ret < 0)
    {
        FUNC_WRAPPER(1,0);
        close(client_fd);
        free(command);
        return -3;
    }
    ret = full_read(client_fd,&result,sizeof(MERGE_INSTALL_KB_COMMAND_RESULT_T));
    if(ret < 0)
    {
        FUNC_WRAPPER(1,0);
        close(client_fd);
        free(command);
        return -4;
    }
    if(result.header.result != RESULT_OK)
    {
        LOGE("merge_install_kb_rpc fail!\n");
        close(client_fd);
        free(command);
        return -5;
    }
    close(client_fd);
    free(command);
    return 0;
}

int merge_install_kb_local(unsigned char* reEncKB,unsigned int reEncKB_len,char* temp_file_path,char* target_path)
{
    int ret = 0;
    unsigned char* oriKBbuff = NULL;
    unsigned int   oriKBbuff_len = 0;
    unsigned char* merged_kb_fliter = NULL;
    unsigned int   merged_kb_fliter_len = 0;
    unsigned char* merged_kb = NULL;
    unsigned int   merged_kb_len = 0;
    DRMKeyID       key_filters[200];
    unsigned int   key_count = 0;
    unsigned char* kb_buffs[2];
    unsigned int   kb_lens[2];
    char  kb_magic[4];
    int hasKBexist = 0;

    if((reEncKB == NULL) || (reEncKB_len == 0)){
        LOGE("Invalid input for merge_install_kb!\n");
        return -1;
    }

    if((temp_file_path == NULL) || (target_path == NULL)){
        LOGE("Invalid input path for merge_install_kb!\n");
        return -2;
    }

    oriKBbuff=get_keyblock_from_nvram_impl(target_path);
    oriKBbuff_len = KEYBLOCK_SIZE;

    //for excluding empty keyblock case
    //if(oriKBbuff != NULL)
    if ((oriKBbuff != NULL) && (memcmp("KBP", oriKBbuff, 3) == 0))
    {
        hasKBexist = 1;
        query_drmkey_from_encKB(reEncKB,&key_count,key_filters);

        kb_buffs[0]=oriKBbuff;
        kb_lens[0]=oriKBbuff_len;

        memcpy(kb_magic,oriKBbuff,4);

        FUNC_WRAPPER(merge_drm_buffs(kb_magic,kb_buffs,kb_lens,1,key_filters,key_count,&merged_kb_fliter,&merged_kb_fliter_len),0);

        kb_buffs[0]=merged_kb_fliter;
        kb_lens[0]=merged_kb_fliter_len;
        kb_buffs[1]=reEncKB;
        kb_lens[1]=reEncKB_len;
        FUNC_WRAPPER(merge_drm_buffs(kb_magic,kb_buffs,kb_lens,2,NULL,0,&merged_kb,&merged_kb_len),0);
    }
    else
    {
        hasKBexist = 0;
        merged_kb = reEncKB;
        merged_kb_len = reEncKB_len;
        if(memcmp(target_path,KEYBLOCK_F_PATH,strlen(KEYBLOCK_F_PATH))==0)
        {
            memcpy(merged_kb,"KBPF",4);
        }
        if(memcmp(target_path,KEYBLOCK_O_PATH,strlen(KEYBLOCK_O_PATH))==0)
        {
            memcpy(merged_kb,"KBPO",4);
        }
        reEncKB = NULL;
    }

    ret = FUNC_WRAPPER(write_buff_to_file(merged_kb,merged_kb_len,temp_file_path),0);
    if(hasKBexist == 1)
    {
        free_encrypt_key_block(merged_kb);
    }
    free_encrypt_key_block(merged_kb_fliter);
    free_encrypt_key_block(oriKBbuff);
    //free memory first and then check return value
    if(ret != 0) return -4;

    ret = FUNC_WRAPPER(resotre_kb_from_file(temp_file_path,target_path),0);
    if(ret != 0) return -5;

        LOGE("temp_file_path is %s!!!!\n",temp_file_path);

        if (drmkey_remove_file(temp_file_path) != 0)
            LOGE("[merge_install_kb_local] drmkey_remove_file() faield %s\n", temp_file_path);

        return 0;
}

int (*merge_install_kb)(unsigned char*,unsigned int,char*,char*) = merge_install_kb_rpc;

static int file_exist(const char* pathname)
{
    return (access(pathname,0)==0);
}

static int is_mtd_device(void)
{
    int is_mtd = file_exist("/dev/mtd");
    LOGE("%s",is_mtd?"MTD DEVICE":"EMMC DEVICE");
    return is_mtd;
}

void switch_flash_operaion_mode_ext(int mode)
{
        if(mode == 0)
        {
                get_keyblock_from_nvram_impl=get_keyblock_from_nvram_impl_local;
                store_keyblock_to_nvram_impl = store_keyblock_to_nvram_impl_local;
                merge_install_kb = merge_install_kb_local;
                if(is_mtd_device())
                {
                        earse_keyblock = earse_keyblock_local_nand;
                }
                else
                {
                        earse_keyblock = earse_keyblock_local_emmc;
                }

        }
        if(mode == 1)
        {
                get_keyblock_from_nvram_impl=get_keyblock_from_nvram_impl_rpc;
                store_keyblock_to_nvram_impl = store_keyblock_to_nvram_impl_rpc;
                merge_install_kb = merge_install_kb_rpc;
                earse_keyblock = earse_keyblock_rpc;
        }
}

void switch_flash_operaion_mode(FLASH_OPERATION_MODE mode)
{
    if(mode == FLASH_OPERATION_LOCAL)
    {
        get_keyblock_from_nvram_impl=get_keyblock_from_nvram_impl_local;
        store_keyblock_to_nvram_impl = store_keyblock_to_nvram_impl_local;
        merge_install_kb = merge_install_kb_local;
        if(is_mtd_device())
        {
            earse_keyblock = earse_keyblock_local_nand;
        }
        else
        {
            earse_keyblock = earse_keyblock_local_emmc;
        }
    }
    if(mode == FLASH_OPERATION_RPC)
    {
        get_keyblock_from_nvram_impl=get_keyblock_from_nvram_impl_rpc;
        store_keyblock_to_nvram_impl = store_keyblock_to_nvram_impl_rpc;
        merge_install_kb = merge_install_kb_rpc;
        earse_keyblock = earse_keyblock_rpc;
    }
}


int install_normal_kb(unsigned char* kb_buff,unsigned int kb_len,char* temp_file_path,char* target_path)
{
    int ret = 0;
    unsigned char* reEncKB = NULL;
    unsigned int   reEncKB_len = 0;
    unsigned int   hasEkkb = 0;

    hasEkkb = *(unsigned int*)(kb_buff+OFFSET_ENCKB_HEADER_HASEKKB);
    if(hasEkkb == 0)
    {
        //for Ekkb not embeded
        factory_provision_str("Keyblock file do not has ekkb,try to find ekkb in keyblock...\n");
        ret = query_drmkey_exist(DRM_SP_EKKB);

        if(ret != 0)
        {
            factory_provision_str("Can not find ekkb in keyblock,Keyblock file install fail!\n");
            LOGE("Fatal error!Ekkb is not installed!!!!\n");
            return -1;
        }

        factory_provision_str("ekkb is found in keyblock!\n");
        ret = FUNC_WRAPPER(restruct_for_noEkkb_keyblock(kb_buff,kb_len),0);
        if(ret != 0)
        {
            LOGE("restruct_for_noEkkb_keyblock fail!!!!\n");
            return -2;
        }
    }

    ret = FUNC_WRAPPER(process_encrypt_key_block(kb_buff,kb_len,&reEncKB,&reEncKB_len),0);

    factory_provision_info(reEncKB+KEYBLOCK_SIZE,KEYBLOCK_SIZE);

    if(ret != 0)
    {
        free_encrypt_key_block(reEncKB);

        return -3;
    }

    factory_provision_str("Keyblock process ok,write it to keyblock...\n");

    ret = FUNC_WRAPPER(merge_install_kb(reEncKB,reEncKB_len,temp_file_path,target_path),0);
    if(ret != 0)
    {
        free_encrypt_key_block(reEncKB);

        return -4;
    }

    free_encrypt_key_block(reEncKB);

    return 0;
}

int restore_kbf_for_KISD(void)
{
    int ret = 0;
    ret = restore_kbf_from_file();
    factory_provision_str("restore kbf partition\n");

    if (drmkey_remove_file(KBF_TEMP_FILE_PATH) != 0)
        LOGE("[restore_kbf_for_KISD] drmkey_remove_file() faield %s\n", KBF_TEMP_FILE_PATH);

    return ret;
}

int restore_kbo_for_KISD(void)
{
    int ret = 0;
    ret = restore_kbo_from_file();
    factory_provision_str("restore kbo partition\n");

    if (drmkey_remove_file(KBO_TEMP_FILE_PATH) != 0)
        LOGE("[restore_kbo_for_KISD] drmkey_remove_file() faield %s\n", KBO_TEMP_FILE_PATH);

    return ret;
}

int install_KB_EKKB_for_KISD(void)
{
    int ret = 0;
    unsigned char* kb_ekkb = NULL;
    unsigned int   kb_ekkb_len = 0;
    unsigned int   install_force = 0;

    factory_provision_str("Try to install KB_EKKB\n");

    factory_provision_str("Try to find FORCE_KB_EKKB......\n");
    ret = FUNC_WRAPPER(read_file_to_buff(&kb_ekkb,&kb_ekkb_len,FORCE_KB_EKKB_PATH),0);
    if(ret == 0)
    {
        factory_provision_str("FORCE_KB_EKKB is found,try to install FORCE_KB_EKKB!\n");
        factory_provision_str("install "FORCE_KB_EKKB_PATH"\n");
        install_force = 1;
    }
    else
    {
        factory_provision_str("FORCE_KB_EKKB is not found,try to install KB_EKKB......\n");
        if(query_drmkey_exist(DRM_SP_EKKB) == 0)
        {
            factory_provision_str("KB_EKKB is already exist in keyblock,give up to find KB_EKKB\n");
            factory_provision_str("KB_EKKB/FORCE_KB_EKKB installation exit...\n");
            return 0;
        }
        else
        {
            factory_provision_str("Finding KB_EKKB from /data/vendor/key_provisioning/......\n");
            ret = FUNC_WRAPPER(read_file_to_buff(&kb_ekkb,&kb_ekkb_len,KB_EKKB_PATH),0);
            if(ret != 0)
            {
                factory_provision_str("KB_EKKB is not found,KB_EKKB install fail!\n");
                factory_provision_str("KB_EKKB/FORCE_KB_EKKB installation exit...\n");
                return 0;
            }
            factory_provision_str("KB_EKKB is found,try to install KB_EKKB!\n");
            factory_provision_str("install "KB_EKKB_PATH"\n");
            install_force = 0;
        }
    }

    ret =  FUNC_WRAPPER(install_kb_ekkb(kb_ekkb,kb_ekkb_len),0);
    if(ret != 0)
    {
        if(install_force)
        {
            factory_provision_str("FORCE_KB_EKKB install fail!\n");
        }
        else
        {
            factory_provision_str("KB_EKKB install fail!\n");
        }
        LOGE("install_kb_ekkb ret = %d\n", ret);
        return -1;
    }
    if(install_force)
    {
        factory_provision_str("FORCE_KB_EKKB install successfully!\n");
    }
    else
    {
        factory_provision_str("KB_EKKB install successfully!\n");
    }
    free_keyblock(kb_ekkb);
    if(install_force)
    {
            if (drmkey_remove_file(FORCE_KB_EKKB_PATH) != 0)
                LOGE("[install_KB_EKKB_for_KISD] drmkey_remove_file() faield %s\n", FORCE_KB_EKKB_PATH);

            factory_provision_str("remove "FORCE_KB_EKKB_PATH"\n");
        }
        else
        {
            if (drmkey_remove_file(KB_EKKB_PATH) != 0)
                LOGE("[install_KB_EKKB_for_KISD] drmkey_remove_file() faield %s\n", KB_EKKB_PATH);

            factory_provision_str("remove "KB_EKKB_PATH"\n");
        }

        return 0;
}

int install_KB_EKKB_for_Debug(char *path)
{
    int ret = 0;
    unsigned char* kb_ekkb = NULL;
    unsigned int   kb_ekkb_len = 0;

    if(query_drmkey_exist(DRM_SP_EKKB) == 0)
    {
        ret = FUNC_WRAPPER(read_file_to_buff(&kb_ekkb,&kb_ekkb_len,path),0);
        if(ret != 0)
        {
            return 0;
        }
        factory_provision_str("install "FORCE_KB_EKKB_PATH"\n");
    }
    else
    {
        ret = FUNC_WRAPPER(read_file_to_buff(&kb_ekkb,&kb_ekkb_len,path),0);
        if(ret != 0)
        {
            return 0;
        }
        factory_provision_str("install "KB_EKKB_PATH"\n");
    }
    ret =  FUNC_WRAPPER(install_kb_ekkb(kb_ekkb,kb_ekkb_len),0);
    if(ret != 0)
    {
        LOGE("install_kb_ekkb ret = %d\n", ret);
        return -1;
    }
    free_keyblock(kb_ekkb);

        if (drmkey_remove_file(KB_EKKB_PATH) != 0)
            LOGE("[install_KB_EKKB_for_Debug] drmkey_remove_file() faield %s\n", KB_EKKB_PATH);

    factory_provision_str("remove "KB_EKKB_PATH"\n");

        if (drmkey_remove_file(FORCE_KB_EKKB_PATH) != 0)
            LOGE("[install_KB_EKKB_for_Debug] drmkey_remove_file() faield %s\n", FORCE_KB_EKKB_PATH);

    factory_provision_str("remove "FORCE_KB_EKKB_PATH"\n");
    return 0;
}

int query_pm_drmkey_exist(void)
{
    unsigned char* oriKBbuff = NULL;
    unsigned char* p_buff = NULL;
    KBHeader_T* header = NULL;
    unsigned int j = 0;
    oriKBbuff=get_keyblock_from_nvram_impl(KEYBLOCK_F_PATH);
    if(oriKBbuff == NULL)
    {
        return -1;
    }
    p_buff = oriKBbuff;
    header = (KBHeader_T*)oriKBbuff;
    p_buff += sizeof(KBHeader_T);
    for(j=0;j<header->keycount;j++)
    {
        ENCRYPT_DRM_KEY_T* drmkey = (ENCRYPT_DRM_KEY_T*)p_buff;
        unsigned int size = SZ_DRMKEY_HEADER_SIZE+drmkey->encDrmKeySize+SZ_DRMKEY_SIG;
        if(drmkey->drmKeyType == KB_TYPE_PM)
        {
            free_keyblock(oriKBbuff);
            return 0;
        }
        p_buff += size;
    }
    free_keyblock(oriKBbuff);
    return -1;
}


int install_KB_PM_for_KISD(void)
{
    int ret = 0;
    unsigned char* kb_pm = NULL;
    unsigned int   kb_pm_len = 0;

    factory_provision_str("Try to install KB_PM\n");
    if(query_pm_drmkey_exist() == 0)
    {
        factory_provision_str("KB_PM is already exist in keyblock,try to install FORCE_KB_PM\n");
        ret = FUNC_WRAPPER(read_file_to_buff(&kb_pm,&kb_pm_len,FORCE_KB_PM_PATH),0);
        if(ret != 0)
        {
            factory_provision_str("FORCE_KB_PM is not exist, KB_PM/FORCE_KB_PM install fail!\n");
            return 0;
        }
        factory_provision_str("FORCE_KB_PM is found,try to install FORCE_KB_PM......\n");
        factory_provision_str("install "FORCE_KB_PM_PATH"\n");
    }
    else
    {
        factory_provision_str("Reading KB_PM from /data/vendor/key_provisioning/......\n");
        ret = FUNC_WRAPPER(read_file_to_buff(&kb_pm,&kb_pm_len,KB_PM_PATH),0);
        if(ret != 0)
        {
            factory_provision_str("KB_PM is not found,KB_PM install fail!\n");
            return 0;
        }
        factory_provision_str("KB_PM is found,try to install KB_PM!\n");
        factory_provision_str("install "KB_PM_PATH"\n");
    }
    ret =  FUNC_WRAPPER(install_normal_kb(kb_pm,kb_pm_len,KBF_TEMP_FILE_PATH,KEYBLOCK_F_PATH),0);
    if(ret != 0)
    {
        factory_provision_str("KB_PM install fail!\n");
        return -1;
    }
    factory_provision_str("KB_PM install successfully!\n");
    free_keyblock(kb_pm);

    if (drmkey_remove_file(KB_PM_PATH) != 0)
        LOGE("[install_KB_PM_for_KISD] drmkey_remove_file() faield %s\n", KB_PM_PATH);

    factory_provision_str("remove "KB_PM_PATH"\n");

    if (drmkey_remove_file(FORCE_KB_PM_PATH) != 0)
        LOGE("[install_KB_PM_for_KISD] drmkey_remove_file() faield %s\n", FORCE_KB_PM_PATH);

    factory_provision_str("remove "FORCE_KB_PM_PATH"\n");
    return 0;
}

int install_KB_PM_for_Debug(char *path)
{
    int ret = 0;
    unsigned char* kb_pm = NULL;
    unsigned int   kb_pm_len = 0;
    if(query_pm_drmkey_exist() == 0)
    {
        ret = FUNC_WRAPPER(read_file_to_buff(&kb_pm,&kb_pm_len,path),0);
        if(ret != 0)
        {
            return 0;
        }
        factory_provision_str("install "FORCE_KB_PM_PATH"\n");
    }
    else
    {
        ret = FUNC_WRAPPER(read_file_to_buff(&kb_pm,&kb_pm_len,path),0);
        if(ret != 0)
        {
            return 0;
        }
        factory_provision_str("install "KB_PM_PATH"\n");
    }
    ret =  FUNC_WRAPPER(install_normal_kb(kb_pm,kb_pm_len,KBF_TEMP_FILE_PATH,KEYBLOCK_F_PATH),0);
    if(ret != 0)
    {
        return -1;
    }
    free_keyblock(kb_pm);

        if (drmkey_remove_file(KB_PM_PATH) != 0)
            LOGE("[install_KB_PM_for_Debug] drmkey_remove_file() faield %s\n", KB_PM_PATH);

    factory_provision_str("remove "KB_PM_PATH"\n");

        if (drmkey_remove_file(FORCE_KB_PM_PATH) != 0)
            LOGE("[install_KB_PM_for_Debug] drmkey_remove_file() faield %s\n", FORCE_KB_PM_PATH);

    factory_provision_str("remove "FORCE_KB_PM_PATH"\n");
    return 0;
}

int install_KB_PD_for_KISD(void)
{
    int ret = 0;
    unsigned char* kb_pd = NULL;
    unsigned int   kb_pd_len = 0;
    ret = FUNC_WRAPPER(read_file_to_buff(&kb_pd,&kb_pd_len,KB_PD_PATH),0);
    if(ret != 0)
    {
        return 0;
    }
    ret =  FUNC_WRAPPER(install_normal_kb(kb_pd,kb_pd_len,KBF_TEMP_FILE_PATH,KEYBLOCK_F_PATH),0);
    if(ret != 0)
    {
        return -1;
    }
    free_keyblock(kb_pd);
    factory_provision_str("install "KB_PD_PATH"\n");

        if (drmkey_remove_file(KB_PD_PATH) != 0)
            LOGE("[install_KB_PD_for_KISD] drmkey_remove_file() faield %s\n", KB_PD_PATH);

    factory_provision_str("remove "KB_PD_PATH"\n");
    return 0;
}

int install_KB_MIX_for_KISD(void)
{
    int ret = 0;
    unsigned char* kb_mix = NULL;
    unsigned int   kb_mix_len = 0;
    ret = FUNC_WRAPPER(read_file_to_buff(&kb_mix,&kb_mix_len,KB_MIX_PATH),0);
    if(ret != 0)
    {
        free_keyblock(kb_mix);

        return 0;
    }
    ret =  FUNC_WRAPPER(install_normal_kb(kb_mix,kb_mix_len,KBF_TEMP_FILE_PATH,KEYBLOCK_F_PATH),0);
    if(ret != 0)
    {
        free_keyblock(kb_mix);

        return -1;
    }
    free_keyblock(kb_mix);
    factory_provision_str("install "KB_MIX_PATH"\n");

    if (drmkey_remove_file(KB_MIX_PATH) != 0)
        LOGE("[install_KB_MIX_for_KISD] drmkey_remove_file() faield %s\n", KB_MIX_PATH);

    factory_provision_str("remove "KB_MIX_PATH"\n");
    return 0;

}

int install_KB_OTA_for_KISD(void)
{
    int ret = 0;
    unsigned char* kb_ota = NULL;
    unsigned int   kb_ota_len = 0;
    ret = FUNC_WRAPPER(read_file_to_buff(&kb_ota,&kb_ota_len,KB_OTA_PATH),0);
    if(ret != 0)
    {
        free_keyblock(kb_ota);

        return 0;
    }
    ret =  FUNC_WRAPPER(install_normal_kb(kb_ota,kb_ota_len,KBO_TEMP_FILE_PATH,KEYBLOCK_O_PATH),0);
    if(ret != 0)
    {
        free_keyblock(kb_ota);

        return -1;
    }
    free_keyblock(kb_ota);
    factory_provision_str("install "KB_OTA_PATH"\n");

        if (drmkey_remove_file(KB_OTA_PATH) != 0)
            LOGE("[install_KB_OTA_for_KISD] drmkey_remove_file() faield %s\n", KB_OTA_PATH);

    factory_provision_str("remove "KB_OTA_PATH"\n");
    return 0;
}

int install_KB_OTA_API(unsigned char* buff,unsigned int len)
{
    int ret = 0;
    ret = FUNC_WRAPPER(write_buff_to_file(buff,len,KB_OTA_PATH),0);
    if(ret != 0) return -1;
    return install_KB_OTA_for_KISD();
}

int install_KB_PD_API(unsigned char* buff,unsigned int len)
{
    int ret = 0;
    ret = FUNC_WRAPPER(write_buff_to_file(buff,len,KB_PD_PATH),0);
    if(ret != 0) return -1;
    return install_KB_PD_for_KISD();
}

int install_KB_MIX_API(unsigned char* buff,unsigned int len)
{
    int ret = 0;
    ret = FUNC_WRAPPER(write_buff_to_file(buff,len,KB_MIX_PATH),0);
    if(ret != 0) return -1;
    return install_KB_MIX_for_KISD();
}

int install_KB_MIX_OR_PD_API(unsigned char* buff,unsigned int len)
{
    unsigned int hasEKkb = *(unsigned int*)(buff+OFFSET_ENCKB_HEADER_HASEKKB);
    if(hasEKkb == 0)
    {
        return install_KB_PD_API(buff,len);
    }
    if(hasEKkb == 1)
    {
        return install_KB_MIX_API(buff,len);
    }
    return -10;
}

/*
static char DRMKeyTypes[][64] =
{
"WIDEVINE",
"MARLIN",
"HDCP_1X_TX",
"HDCP_2X_V1_TX",
"HDCP_2X_V1_RX",
"HDCP_2X_V2_TX",
"HDCP_2X_V2_RX",
"PLAYREADY_BGROUPCERT",
"PLAYREADY_ZGPRIV",
"PLAYREADY_KEYFILE",
"DEVICE_RSA_KEYPAIR",
"LEK"
};
*/


int query_drmkey_status(void)
{
    unsigned int  keycount;
    unsigned int  keytype[512] = {0};
    int ret;
    unsigned int i;

    ret = query_drmkey(&keycount, keytype);

    if (ret == 0)
    {
        unsigned int leng;

        LOGE("-----------@query drm key start@-----------\n");

        if(keycount > 0)
        {
            for (i=0; i<keycount; i++)
            {
                get_clearDrmkey_size(keytype[i], &leng);

                LOGE("keytype=%10d\n", keytype[i]);
            }
        }
        else if(keycount == 0)
        {
            LOGE("No key installed\n");
        }

        LOGE("-----------@query drm key end@-----------\n");

    }
    else
        return -1;

    return 0;
}


