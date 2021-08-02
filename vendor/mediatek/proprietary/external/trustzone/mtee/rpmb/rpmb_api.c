#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/ioctl.h"
#include "linux/mmc/ioctl.h"
#include "linux/major.h"
#include "tz_cross/ta_rpmb.h"
#include "rpmb.h"
#include <errno.h>
#include <string.h>
#define LOG_TAG "rpmb"
#include <log/log.h>

/* #define DEBUG */

static void ReverseEndian(void *data, size_t size) {
    unsigned int i;
    char tmp;
    char *swp = (char *)data;
#ifdef DEBUG
    char buf[64] = {'\0'};
    fprintf(stderr, "\nrvbuf @%p, size:%zx", data, size);
    sprintf(buf, "\nrvbuf@%p, size:%zx", data, size);
#endif
    for (i = 0 ; i< (size/2); ++i) {
        tmp = swp[i];
        swp[i] = swp[size-1-i];
        swp[size-1-i] = tmp;
#ifdef DEBUG
        if (i%16 == 0) {
            fprintf(stderr, "\n");
            ALOGD("%s", buf);
        }
        fprintf(stderr, "%02x ", swp[i]);
        sprintf(buf+3*(i%16), "%02x ", swp[i]);
#endif
    }
#ifdef DEBUG
    while (i < size) {
        if (i%16 == 0) {
            fprintf(stderr, "\n");
            ALOGD("%s", buf);
        }
        fprintf(stderr, "%02x ", swp[i]);
        sprintf(buf+3*(i%16), "%02x ", swp[i]);
        i++;
    }
    if (i%16 != 1)
        ALOGD("%s", buf);
#endif
}

int EmmcRpmbProgramKey(int fd, rpmb_pkt_t *ps_rpmb_pkt, rpmb_pkt_t *ps_res_pkt) {
    int ret;
    struct mmc_ioc_cmd *idata;
    struct mmc_ioc_multi_cmd *pmcmd;

    pmcmd = calloc(sizeof(struct mmc_ioc_multi_cmd) +
                   sizeof(struct mmc_ioc_cmd)*2, 1);
    if(pmcmd == NULL) {
        ALOGE("memory allocation failed!");
        return -1;
    }

    idata = pmcmd->cmds;
    pmcmd->num_of_cmds = 2;

    memset(&idata[0], 0, sizeof(struct mmc_ioc_cmd)*2);
    idata[0].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[0].opcode = MMC_WRITE_MULTIPLE_BLOCK;
    idata[0].arg = 0;
    idata[0].blksz = 512;
    idata[0].blocks = 1;
    idata[0].write_flag = 1<<31;

    ps_rpmb_pkt->u2ReqResp = 0x1;
    ReverseEndian(ps_rpmb_pkt, sizeof(rpmb_pkt_t));
    mmc_ioc_cmd_set_data(idata[0], ps_rpmb_pkt);

    memset(ps_res_pkt, 0, sizeof(rpmb_pkt_t));
    idata[1].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[1].opcode = MMC_READ_MULTIPLE_BLOCK;
    idata[1].arg = 0;
    idata[1].blksz = 512;
    idata[1].blocks = 1;
    idata[1].write_flag = 0;
    mmc_ioc_cmd_set_data(idata[1], ps_res_pkt);

    ALOGD("%s: write_multiple_block, then read multiple_block\n", __FUNCTION__);
    if ((ret=ioctl(fd, MMC_IOC_MULTI_CMD, pmcmd)) != 0) {
        ALOGE("%s(ioc_mul_cmd): ioctl ret %d, errno=%d (%s)", __FUNCTION__, ret, errno, strerror(errno));
        free(pmcmd);
        return ret;
    }
    free(pmcmd);

    ReverseEndian(ps_res_pkt, sizeof(rpmb_pkt_t));

    if (ps_res_pkt->u2Result != 0) {
        ALOGE("%s: program fail, result=0x%x", __FUNCTION__, ps_res_pkt->u2Result);
        return ps_res_pkt->u2Result;
    }

    return ret;
}

int EmmcRpmbReadCounter(int fd, rpmb_pkt_t *ps_rpmb_pkt) {
    struct mmc_ioc_cmd *idata;
    struct mmc_ioc_multi_cmd *pmcmd;
    rpmb_pkt_t *ps_resp_pkt;
    int ret;

    pmcmd = calloc(sizeof(struct mmc_ioc_multi_cmd) +
                   sizeof(struct mmc_ioc_cmd)*2, 1);
    if(pmcmd == NULL) {
        ALOGE("memory allocation failed!");
        return -1;
    }

    idata = pmcmd->cmds;
    pmcmd->num_of_cmds = 2;

    memset(&idata[0], 0, sizeof(struct mmc_ioc_cmd)*2);
    idata[0].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[0].opcode = MMC_WRITE_MULTIPLE_BLOCK;
    idata[0].arg = 0;
    idata[0].blksz = 512;
    idata[0].blocks = 1;
    idata[0].write_flag = 1;

    ps_rpmb_pkt->u2ReqResp = 0x2;
    ReverseEndian(ps_rpmb_pkt, sizeof(rpmb_pkt_t));
    mmc_ioc_cmd_set_data(idata[0], ps_rpmb_pkt);

    ps_resp_pkt = calloc(sizeof(rpmb_pkt_t), 1);
    if (ps_resp_pkt == NULL) {
        ALOGE("memory allocation failed!");
        free(pmcmd);
        return -1;
    }

    idata[1].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[1].opcode = MMC_READ_MULTIPLE_BLOCK;
    idata[1].arg = 0;
    idata[1].blksz = 512;
    idata[1].blocks = 1;
    idata[1].write_flag = 0;

    mmc_ioc_cmd_set_data(idata[1], ps_resp_pkt);

    ALOGD("%s: write_multiple_block, then read_multiple_block\n", __FUNCTION__);
    if ((ret=ioctl(fd, MMC_IOC_MULTI_CMD, pmcmd)) != 0) {
        ALOGE("%s(ioc_mul_cmd): ioctl ret %d, errno=%d (%s)", __FUNCTION__, ret, errno, strerror(errno));
        free(ps_resp_pkt);
        free(pmcmd);
        return ret;
    }

    memcpy(ps_rpmb_pkt, ps_resp_pkt, sizeof(rpmb_pkt_t));
    ReverseEndian(ps_rpmb_pkt, sizeof(rpmb_pkt_t));

    free(ps_resp_pkt);
    free(pmcmd);
    return 0;
}


int EmmcRpmbAuthWrite(int fd, rpmb_pkt_t *pwrite_rpmb_pkt, rpmb_pkt_t *result_pkt) {
    struct mmc_ioc_cmd *idata;
    struct mmc_ioc_multi_cmd *pmcmd;
    rpmb_pkt_t *ps_readreq_pkt;
    int ret;
    int idx, blkcnt;

    pmcmd = calloc(sizeof(struct mmc_ioc_multi_cmd) +
                   sizeof(struct mmc_ioc_cmd)*3, 1);
    if(pmcmd == NULL) {
        ALOGE("memory allocation failed!");
        return -1;
    }

    idata = pmcmd->cmds;
    pmcmd->num_of_cmds = 3;

    memset(&idata[0], 0, sizeof(struct mmc_ioc_cmd)*3);
    idata[0].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[0].opcode = MMC_WRITE_MULTIPLE_BLOCK;
    idata[0].arg = 0;
    idata[0].blksz = 512;
    idata[0].blocks = pwrite_rpmb_pkt->u2BlockCount;
    idata[0].write_flag = 1<<31;

    blkcnt = pwrite_rpmb_pkt->u2BlockCount;
    for (idx=0; idx < blkcnt; idx++)
        ReverseEndian(&pwrite_rpmb_pkt[idx], sizeof(rpmb_pkt_t));
    mmc_ioc_cmd_set_data(idata[0], pwrite_rpmb_pkt);

    /* send read result request */
    ps_readreq_pkt = calloc(sizeof(rpmb_pkt_t), 1);
    if (ps_readreq_pkt == NULL) {
        free(pmcmd);
        ALOGE("memory allocation failed!");
        return -1;
    }
    memset(ps_readreq_pkt, 0, sizeof(rpmb_pkt_t));
    ps_readreq_pkt->u2ReqResp = 0x5;

    idata[1].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[1].opcode = MMC_WRITE_MULTIPLE_BLOCK;
    idata[1].arg = 0;
    idata[1].blksz = 512;
    idata[1].blocks = 1;
    idata[1].write_flag = 1;

    ReverseEndian(ps_readreq_pkt, sizeof(rpmb_pkt_t));
    mmc_ioc_cmd_set_data(idata[1], ps_readreq_pkt);

    /* read back the result */
    memset(result_pkt, 0, sizeof(rpmb_pkt_t));

    idata[2].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[2].opcode = MMC_READ_MULTIPLE_BLOCK;
    idata[2].arg = 0;
    idata[2].blksz = 512;
    idata[2].blocks = 1;
    idata[2].write_flag = 0;

    mmc_ioc_cmd_set_data(idata[2], result_pkt);

    ALOGD("%s: write_multiple_blockx2, then read_multiple_block\n", __FUNCTION__);
    if ((ret=ioctl(fd, MMC_IOC_MULTI_CMD, pmcmd)) != 0) {
        ALOGE("%s(ioc_mul_cmd): ioctl ret %d, errno=%d (%s)", __FUNCTION__, ret, errno, strerror(errno));
        free(ps_readreq_pkt);
        free(pmcmd);
        return ret;
    }

    ReverseEndian(result_pkt, sizeof(rpmb_pkt_t));

    free(ps_readreq_pkt);
    free(pmcmd);
    return 0;
}

int EmmcRpmbAuthRead(int fd, rpmb_pkt_t *pread_rpmb_pkt, rpmb_pkt_t *result_pkt, unsigned int size) {
    int cnt = (size + RPMB_DATA_LENGTH - 1)/RPMB_DATA_LENGTH;
    struct mmc_ioc_cmd *idata;
    struct mmc_ioc_multi_cmd *pmcmd;
    int idx;
    int ret;

    pmcmd = calloc(sizeof(struct mmc_ioc_multi_cmd) +
                   sizeof(struct mmc_ioc_cmd)*2, 1);
    if(pmcmd == NULL) {
        ALOGE("memory allocation failed!");
        return -1;
    }

    idata = pmcmd->cmds;
    pmcmd->num_of_cmds = 2;

    memset(&idata[0], 0, sizeof(struct mmc_ioc_cmd)*2);

    /* send read result request */
    idata[0].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[0].opcode = MMC_WRITE_MULTIPLE_BLOCK;
    idata[0].arg = 0;
    idata[0].blksz = 512;
    idata[0].blocks = 1;
    idata[0].write_flag = 1;

    pread_rpmb_pkt->u2ReqResp = 0x0004;
    ReverseEndian(pread_rpmb_pkt, sizeof(rpmb_pkt_t));
    mmc_ioc_cmd_set_data(idata[0], pread_rpmb_pkt);

    memset(result_pkt, 0, sizeof(rpmb_pkt_t));
    /* read back the result */
    idata[1].flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata[1].opcode = MMC_READ_MULTIPLE_BLOCK;
    idata[1].arg = 0;
    idata[1].blksz = 512;
    idata[1].blocks = cnt;
    idata[1].write_flag = 0;

    mmc_ioc_cmd_set_data(idata[1], result_pkt);

    ALOGD("%s: write_multiple_block, then read_multiple_block\n", __FUNCTION__);
    if ((ret=ioctl(fd, MMC_IOC_MULTI_CMD, pmcmd)) != 0) {
        ALOGE("%s(ioc_mul_cmd): ioctl ret %d, errno=%d (%s)", __FUNCTION__, ret, errno, strerror(errno));
        free(pmcmd);
        return ret;
    }

    for (idx=0; idx < cnt; idx++)
        ReverseEndian(&result_pkt[idx], sizeof(rpmb_pkt_t));

    free(pmcmd);
    return 0;
}

