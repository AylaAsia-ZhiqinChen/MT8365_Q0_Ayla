#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/capability.h>
#include "private/android_filesystem_config.h"
#include <tz_cross/ta_rpmb.h>
#include <tz_cross/ta_mem.h>
#include <uree/system.h>
#include <uree/mem.h>
#include "rpmb.h"
#include "uree_rpmb.h"
#define LOG_TAG "rpmb"
#include <log/log.h>
#include "cutils/properties.h"

#define RPMB_BUF_SIZE (2*sizeof(rpmb_pkt_t)) /* 1K buffer */
#define RPMB_DEV_NODE "/dev/block/mmcblk0rpmb"
#define EMMC_CID "/sys/devices/platform/soc/11230000.mmc/mmc_host/mmc0/mmc0:0001/cid"
#define EMMC_CID2 "/sys/devices/soc/11230000.mmc/mmc_host/mmc0/mmc0:0001/cid"
#define EMMC_CID3 "/sys/devices/platform/soc/11120000.mmc/mmc_host/mmc0/mmc0:0001/cid"

/* #define DEBUG */

int get_emmc_cid(unsigned char cid[16]) {
    int fd;
    int i;
    char str[33];
#ifdef DEBUG
    char ostr[64];
    char *pstr;
#endif
    fd = open(EMMC_CID, O_RDONLY);
    if (fd < 0) {
        fd = open(EMMC_CID2, O_RDONLY);
        if (fd < 0) {
            fd = open(EMMC_CID3, O_RDONLY);
            if (fd < 0)
                return -1;
        }
    }

    if (32 != read(fd, str, 32)) {
        close(fd);
        return -2;
    }

    close(fd);
    str[32] = '\0';
#ifdef DEBUG
    sprintf(ostr, "EMMC_CID:");
    pstr = ostr + strlen("EMMC_CID:");
#endif
    for (i=15; i >= 0; i--) {
        cid[i] = (unsigned char)strtoul(&str[i*2], NULL, 16);
        str[i*2] = '\0';
#ifdef DEBUG
        sprintf(pstr, "%02x", cid[i]);
        pstr+=2;
#endif
    }
#ifdef DEBUG
    fprintf(stderr, "%s\n", ostr);
    ALOGI("%s\n", ostr);
#endif
    return 0;
}

static int __set_uid_system(void)
{
    int ret;
    struct __user_cap_header_struct caphdr;
    struct __user_cap_data_struct capdt[_LINUX_CAPABILITY_U32S_3];

    ret = prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
    if (ret != 0) return ret;

    ret = setuid(AID_SYSTEM);
    if (ret != 0) return ret;

    /* set rawio for rpmb service needed. */
    caphdr.version = _LINUX_CAPABILITY_VERSION_3;
    caphdr.pid = 0;
    memset(capdt, 0, sizeof(struct __user_cap_data_struct)*_LINUX_CAPABILITY_U32S_3);
    capdt[CAP_TO_INDEX(CAP_SYS_RAWIO)].effective = CAP_TO_MASK(CAP_SYS_RAWIO);
    capdt[CAP_TO_INDEX(CAP_SYS_RAWIO)].permitted = CAP_TO_MASK(CAP_SYS_RAWIO);
    ret = capset(&caphdr, capdt);

    return ret;
}

#define READ_COUNTER_RETRY_COUNT (10)
int main(int argc, char *argv[]) {
    TZ_RESULT tzret = TZ_RESULT_SUCCESS;
    UREE_SESSION_HANDLE mem_session;
    UREE_SESSION_HANDLE rpmb_session;
    UREE_SHAREDMEM_HANDLE rpmb_pkt_buf_handle;
    UREE_SHAREDMEM_PARAM shared_mem_param;
    rpmb_pkt_t *rpmb_pkt_buf;
    rpmb_pkt_t *rpmb_ret_buf;
    int ret;
    int fd;
    uint32_t rpmb_cmd;
    unsigned char cid[16];
    int retry_cnt = 0;

    ret = __set_uid_system();
    if (0 != ret)
    {
        ALOGE("drop to system failed! errno=%d (%s)\n", ret, strerror(errno));
    }

    if (-1 == (fd = open(RPMB_DEV_NODE, O_RDWR))) {
        ALOGE("open rpmb node %s fail! %s\n", RPMB_DEV_NODE, strerror(errno));
        return -1;
    }

    tzret = UREE_CreateSession(TZ_TA_RPMB_UUID, &rpmb_session);
    if (tzret != TZ_RESULT_SUCCESS) {
        ALOGE("create rpmb ta session fail! %s\n", TZ_GetErrorString(tzret));
        ret = -1;
        goto out1;
    }

    if (0 == get_emmc_cid(cid)) {
        UREE_RpmbInit(rpmb_session, cid, sizeof(cid));
    }

    tzret = UREE_CreateSession(TZ_TA_MEM_UUID, &mem_session);
    if (tzret != TZ_RESULT_SUCCESS) {
        ALOGE("create memory ta session fail! %s\n", TZ_GetErrorString(tzret));
        ret = -1;
        goto out2;
    }

    /* alloc memory, and register for tee use */
    rpmb_pkt_buf = (rpmb_pkt_t *)malloc(RPMB_BUF_SIZE);
    if (rpmb_pkt_buf == NULL) {
        ALOGE("memory allocation fail (%zx\n)\n", RPMB_BUF_SIZE);
        ret = -1;
        goto out3;
    }
    rpmb_ret_buf = (rpmb_pkt_t *)malloc(RPMB_BUF_SIZE);
    if (rpmb_ret_buf == NULL) {
        ALOGE("memory allocation fail (%zx\n)\n", RPMB_BUF_SIZE);
        ret = -1;
        goto out4;
    }
    shared_mem_param.buffer = rpmb_pkt_buf;
    shared_mem_param.size = RPMB_BUF_SIZE;
    tzret = UREE_RegisterSharedmem(mem_session, &rpmb_pkt_buf_handle, &shared_mem_param);
    if (tzret != TZ_RESULT_SUCCESS) {
        ALOGE("register shared memory fail! %s\n", TZ_GetErrorString(tzret));
        ret = tzret;
        goto out5;
    }

    tzret = UREE_RpmbRegWorkBuf(rpmb_session, rpmb_pkt_buf_handle, RPMB_BUF_SIZE);
    if (tzret != TZ_RESULT_SUCCESS) {
        ALOGE("register work buffer fail! %s\n", TZ_GetErrorString(tzret));
        ret = tzret;
        goto out6;
    }

read_counter_retry:

    tzret = UREE_RpmbGetWriteCounter(rpmb_session, (unsigned char *)rpmb_pkt_buf, sizeof(rpmb_pkt_t));
    if (tzret != TZ_RESULT_SUCCESS) {
        ALOGE("rpmb get write counter error 0x%x\n", tzret);
        ret = tzret;
        goto out6;
    }

    if (0 != (ret = EmmcRpmbReadCounter(fd, rpmb_pkt_buf))) {
        ALOGE("get write counter fail\n");
        goto out6;
    }

    tzret = UREE_RpmbFeedbackWriteCounter(rpmb_session, (const unsigned char*)rpmb_pkt_buf, sizeof(rpmb_pkt_t));
    if (tzret != TZ_RESULT_SUCCESS) {
        ALOGE("rpmb feedback write counter error 0x%x\n", tzret);
        if (retry_cnt < READ_COUNTER_RETRY_COUNT) {
            retry_cnt++;
            usleep(300000);  /* 300ms */
            goto read_counter_retry;
        }
        /* write counter verify fail, possibly the emmc is not yet programmed */
        /* if fail, stay in work loop, not return */
        /* return tzret; */
    }

    if (tzret == TZ_RESULT_SUCCESS)
        property_set("vendor.rpmb.ready", "1");
    else
        property_set("vendor.rpmb.ready", "0");

    /* work loop */
    do {
        ALOGV("waiting for command\n");
        tzret = UREE_RpmbWaitForCommand(rpmb_session, &rpmb_cmd);
        if (tzret != TZ_RESULT_SUCCESS) {
            ALOGE("work loop waitcmd error 0x%x (%s)\n", tzret, TZ_GetErrorString(tzret));
            sleep(1);
            /* should not happen, end the daemon */
            break;
        }

        switch (NS_RPMB_OP(rpmb_cmd)) {
            case RPMB_READ:
                ALOGD("AuthRead\n");
                EmmcRpmbAuthRead(fd, rpmb_pkt_buf, rpmb_ret_buf, NS_RPMB_SIZE(rpmb_cmd));
                memcpy(rpmb_pkt_buf, rpmb_ret_buf, NS_RPMB_SIZE(rpmb_cmd)/256*512);
            break;
            case RPMB_WRITE:
                ALOGD("AuthWrite\n");
                EmmcRpmbAuthWrite(fd, rpmb_pkt_buf, rpmb_ret_buf);
                memcpy(rpmb_pkt_buf, rpmb_ret_buf, sizeof(rpmb_pkt_t));
            break;
            case RPMB_EAGAIN:
                /* some signal interrupted the waiting process, go back for wait */
                continue;
            break;
            default:
                ALOGE("unknown cmd received: %d\n", rpmb_cmd);
                sleep(1);
            break;
        }

        ALOGV("Feedback\n");
        tzret = UREE_RpmbFeedbackResult(rpmb_session, rpmb_cmd);
        if (tzret != TZ_RESULT_SUCCESS) {
            ALOGE("work loop feedback error 0x%x (%s)\n", tzret, TZ_GetErrorString(tzret));
            sleep(1);
            /* should not happen, end the daemon */
            break;
        }
    }while (1);

out6:
    UREE_UnregisterSharedmem(mem_session, rpmb_pkt_buf_handle);
out5:
    free(rpmb_ret_buf);
out4:
    free(rpmb_pkt_buf);
out3:
    UREE_CloseSession(mem_session);
out2:
    UREE_CloseSession(rpmb_session);
out1:
    close(fd);

    return ret;
}
