
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <tz_cross/trustzone.h>
#include <pthread.h>
#include <kree/tz_mod.h>
#include "uree/system.h"
#include "uree/mem.h"
#include <unistd.h>

#include <tz_cross/ta_mem.h>
#include <errno.h>
#include <string.h>
#include <private/android_filesystem_config.h>
#define LOG_TAG "uree.so"
#include <log/log.h>

static int uree_fd = -1;
pthread_mutex_t uree_init_mutex = PTHREAD_MUTEX_INITIALIZER;

static TZ_RESULT UREE_OpenFd(void) {
    int fd;
    int ret = TZ_RESULT_SUCCESS;

    pthread_mutex_lock(&uree_init_mutex);

    if (uree_fd >= 0)
        goto out;

    // Open TZ device.
    fd = open("/dev/" TZ_DEV_NAME, O_RDWR);
    if (fd < 0 ) {
        int num_gids, i;
        gid_t *gids;
        char *strbuf, *pstr;

        ret = TZ_RESULT_ERROR_GENERIC;

        ALOGE("%s open fd fail, err = %s\n", __FUNCTION__, strerror(errno));
        ALOGE("uid=%d, gid=%d\n", getuid(), getgid());
        strbuf = malloc(512);
        if (strbuf == NULL)
            goto out;

        num_gids = getgroups(0, NULL);
        if (num_gids > 0) {
            gids = malloc(num_gids*sizeof(gid_t));
            if(gids != NULL) {
                getgroups(num_gids, gids);
                pstr = strbuf;
                pstr += snprintf(pstr, 512, "groups={%d", gids[0]);
                for(i=1;i<num_gids;i++)
                    pstr += snprintf(pstr, 512-(pstr-strbuf), " %d", gids[i]);
                free(gids);
            }
        }
        ALOGE("%s}\n", strbuf);

        snprintf(strbuf, 512, "/proc/%d/cmdline", getpid());
        fd = open(strbuf, O_RDONLY);
        if (fd >= 0) {
            i = read(fd, strbuf, 512);
            close(fd);
            strbuf[i-1] = '\0';
            ALOGE("cmdline=%s\n", strbuf);
        }
        free(strbuf);
        ALOGE("permission granted for UID %d(SYSTEM), GID %d(DRMRPC)\n",
                AID_SYSTEM, AID_DRMRPC);
    }
    else
        uree_fd = fd;

out:
    pthread_mutex_unlock(&uree_init_mutex);
    return ret;
}

static TZ_RESULT UREE_CloseFd(void) {
    if(uree_fd >= 0)
        close(uree_fd);

    uree_fd = -1;
    return TZ_RESULT_SUCCESS;
}

static int __attribute__((destructor)) uree_fini(void)
{
    UREE_CloseFd();
    return 0;
}

static TZ_RESULT UREE_CreateSession_Body(const char *ta_uuid,
                             UREE_SESSION_HANDLE *pHandle, const char *tag) {
    TZ_RESULT ret;
    struct kree_session_tag_cmd_param param;
    int iret;

    if (uree_fd < 0) {
        ret = UREE_OpenFd();
        if (ret != TZ_RESULT_SUCCESS)
            return ret;
    }

    if (!pHandle || !ta_uuid)
        return TZ_RESULT_ERROR_BAD_PARAMETERS;

    param.data = (uint64_t)(unsigned long)ta_uuid;
    param.tag = (uint64_t)(unsigned long)tag;
    if (tag != NULL) {
        param.tag_size = strlen(tag);
    } else {
        param.tag_size = 0;
    }
    iret = ioctl(uree_fd, MTEE_CMD_OPEN_SESSION_WITH_TAG, &param);
    if (iret) {
        ALOGE("%s: ioctl ret %d, err %d (%s))\n", __FUNCTION__, iret, errno, strerror(errno) );
        return TZ_RESULT_ERROR_GENERIC;
    }
    if (param.ret != TZ_RESULT_SUCCESS)
        return param.ret;
    *pHandle = param.handle;

    return TZ_RESULT_SUCCESS;
}

TZ_RESULT UREE_CreateSession(const char *ta_uuid,
                             UREE_SESSION_HANDLE *pHandle) {
    return UREE_CreateSession_Body(ta_uuid, pHandle, NULL);
}

TZ_RESULT UREE_CreateSessionWithTag(const char *ta_uuid,
                             UREE_SESSION_HANDLE *pHandle, const char *tag) {
    return UREE_CreateSession_Body(ta_uuid, pHandle, tag);
}

TZ_RESULT UREE_CloseSession(UREE_SESSION_HANDLE handle) {
    struct kree_session_cmd_param param;
    int iret;

    if (uree_fd < 0) {
        ALOGE("%s: invalid fd %d\n", __FUNCTION__, uree_fd);
        return TZ_RESULT_ERROR_GENERIC;
    }

    param.handle = handle;
    iret = ioctl(uree_fd, MTEE_CMD_CLOSE_SESSION, &param);
    if (iret) {
        ALOGE("%s: ioctl ret %d, err %d (%s))\n", __FUNCTION__, iret, errno, strerror(errno) );
        return TZ_RESULT_ERROR_GENERIC;
    }
    return param.ret;
}

TZ_RESULT UREE_TeeServiceCall(UREE_SESSION_HANDLE handle, uint32_t command,
                              uint32_t paramTypes, MTEEC_PARAM param[4]) {
    struct kree_tee_service_cmd_param cparam;
    int iret;

    if (uree_fd < 0) {
        ALOGE("%s invalid fd %d\n", __FUNCTION__, uree_fd);
        return TZ_RESULT_ERROR_GENERIC;
    }

    cparam.handle = handle;
    cparam.command = command;
    cparam.paramTypes = paramTypes;
    cparam.param = (uint64_t)(unsigned long)param;
    iret = ioctl(uree_fd, MTEE_CMD_TEE_SERVICE, &cparam);
    if (iret) {
        ALOGE("%s: ioctl ret %d, err %d (%s))\n", __FUNCTION__, iret, errno, strerror(errno) );
        return TZ_RESULT_ERROR_GENERIC;
    }
    return cparam.ret;
}

#include "tz_cross/tz_error_strings.h"

const char *TZ_GetErrorString(TZ_RESULT res) {
    return _TZ_GetErrorString(res);
}

static TZ_RESULT UREE_RegisterSharedmem_Helper(UREE_SESSION_HANDLE session,
    UREE_SHAREDMEM_HANDLE *shm_handle, UREE_SHAREDMEM_PARAM *param,
    const char *tag)
{
    struct kree_sharedmemory_tag_cmd_param p;
    int iret;

    if (uree_fd < 0) {
        return TZ_RESULT_ERROR_ACCESS_DENIED;
    }

    p.session = session;
    p.buffer = (uint64_t)(unsigned long)param->buffer;
    p.size = param->size;
    p.command = TZCMD_MEM_SHAREDMEM_REG;
    p.mem_handle = 0;
    p.control = 0;  // for write
    p.tag = (unsigned long long)(unsigned long)tag;
    if (tag == NULL)
        p.tag_size = 0;
    else
        p.tag_size = strlen(tag);
    iret = ioctl(uree_fd, MTEE_CMD_SHM_REG_WITH_TAG, &p);
    if (iret) {
        ALOGE("%s: ioctl ret %d, err %d (%s))\n", __FUNCTION__, iret, errno, strerror(errno) );
        return TZ_RESULT_ERROR_GENERIC;
    }

    *shm_handle = p.mem_handle;

    return TZ_RESULT_SUCCESS;
}

TZ_RESULT UREE_RegisterSharedmem(UREE_SESSION_HANDLE session,
    UREE_SHAREDMEM_HANDLE *shm_handle, UREE_SHAREDMEM_PARAM *param) {
    return UREE_RegisterSharedmem_Helper(session, shm_handle, param, NULL);
}

TZ_RESULT UREE_RegisterSharedmemWithTag(UREE_SESSION_HANDLE session,
    UREE_SHAREDMEM_HANDLE *shm_handle, UREE_SHAREDMEM_PARAM *param,
    const char *tag) {
    return UREE_RegisterSharedmem_Helper(session, shm_handle, param, tag);
}

TZ_RESULT UREE_UnregisterSharedmem(UREE_SESSION_HANDLE session,
                                   UREE_SHAREDMEM_HANDLE shm_handle) {
    struct kree_sharedmemory_cmd_param p;
    int iret;

    if (uree_fd < 0) {
        return TZ_RESULT_ERROR_ACCESS_DENIED;
    }

    p.session = session;
    p.buffer = (uint64_t)(unsigned long)NULL;
    p.size = 0;
    p.command = TZCMD_MEM_SHAREDMEM_UNREG;
    p.mem_handle = shm_handle;
    p.control = 0;  // for write
    iret = ioctl(uree_fd, MTEE_CMD_SHM_UNREG, &p);
    if (iret) {
        ALOGE("%s: ioctl ret %d, err %d (%s))\n", __FUNCTION__, iret, errno, strerror(errno) );
        return TZ_RESULT_ERROR_GENERIC;
    }

    return TZ_RESULT_SUCCESS;
}

static TZ_RESULT UREE_RegisterSharedmem_ReadOnly_Helper(
    UREE_SESSION_HANDLE session,
    UREE_SHAREDMEM_HANDLE *shm_handle, UREE_SHAREDMEM_PARAM *param,
    const char *tag)
{
    struct kree_sharedmemory_tag_cmd_param p;
    int iret;

    if (uree_fd < 0) {
        return TZ_RESULT_ERROR_ACCESS_DENIED;
    }

    p.session = session;
    p.buffer = (uint64_t)(unsigned long)param->buffer;
    p.size = param->size;
    p.command = TZCMD_MEM_SHAREDMEM_REG;
    p.mem_handle = 0;
    p.control = 1;  // for read only
    p.tag = (unsigned long long)(unsigned long)tag;
    if (tag == NULL)
        p.tag_size = 0;
    else
        p.tag_size = strlen(tag);
    iret = ioctl(uree_fd, MTEE_CMD_SHM_REG_WITH_TAG, &p);
    if (iret) {
        ALOGE("%s: ioctl ret %d, err %d (%s))\n", __FUNCTION__, iret, errno, strerror(errno) );
        return TZ_RESULT_ERROR_GENERIC;
    }

    *shm_handle = p.mem_handle;

    return TZ_RESULT_SUCCESS;
}

TZ_RESULT UREE_RegisterSharedmem_ReadOnly(
    UREE_SESSION_HANDLE session,
    UREE_SHAREDMEM_HANDLE *shm_handle, UREE_SHAREDMEM_PARAM *param)
{
     return UREE_RegisterSharedmem_ReadOnly_Helper(session, shm_handle, param, NULL);
}

TZ_RESULT UREE_RegisterSharedmem_ReadOnlyWithTag(
    UREE_SESSION_HANDLE session,
    UREE_SHAREDMEM_HANDLE *shm_handle, UREE_SHAREDMEM_PARAM *param,
    const char *tag)
{
     return UREE_RegisterSharedmem_ReadOnly_Helper(session, shm_handle, param, tag);
}

TZ_RESULT UREE_UnregisterSharedmem_ReadOnly(UREE_SESSION_HANDLE session,
        UREE_SHAREDMEM_HANDLE shm_handle) {
    struct kree_sharedmemory_cmd_param p;
    int iret;

    if (uree_fd < 0) {
        return TZ_RESULT_ERROR_ACCESS_DENIED;
    }

    p.session = session;
    p.buffer = (uint64_t)(unsigned long)NULL;
    p.size = 0;
    p.command = TZCMD_MEM_SHAREDMEM_UNREG;
    p.mem_handle = shm_handle;
    p.control = 1;  // for read only
    iret = ioctl(uree_fd, MTEE_CMD_SHM_UNREG, &p);
    if (iret) {
        ALOGE("%s: ioctl ret %d, err %d (%s))\n", __FUNCTION__, iret, errno, strerror(errno) );
        return TZ_RESULT_ERROR_GENERIC;
    }

    return TZ_RESULT_SUCCESS;
}




