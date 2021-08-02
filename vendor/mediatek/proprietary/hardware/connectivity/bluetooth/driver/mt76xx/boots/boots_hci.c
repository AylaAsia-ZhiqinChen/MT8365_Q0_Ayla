/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2016~2017. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "boots.h"

#define LOG_TAG "boots_hci"

#define HCI_VENDOR_PKT      0xff

#define SOL_HCI         0
#define HCI_FILTER      2

#define HCI_MAX_DEV    1
#define BTPROTO_HCI    1

#define HCIDEVUP        _IOW('H', 201, int)
#define HCIDEVDOWN      _IOW('H', 202, int)
#define HCIDEVRESET     _IOW('H', 203, int)
#define HCIDEVRESTAT    _IOW('H', 204, int)
#define HCIGETDEVLIST   _IOR('H', 210, int)
#define HCISETRAW       _IOW('H', 220, int)

#define HCI_FLT_TYPE_BITS    31
#define HCI_FLT_EVENT_BITS    63

#define HCI_COMMAND_PKT        0x01
#define HCI_ACLDATA_PKT        0x02
#define HCI_SCODATA_PKT        0x03
#define HCI_EVENT_PKT        0x04
#define HCI_VENDOR_PKT        0xff

#define EVT_INQUIRY_COMPLETE        0x01
#define EVT_INQUIRY_RESULT        0x02
#define EVT_CONN_COMPLETE        0x03
#define EVT_CONN_REQUEST        0x04
#define EVT_DISCONN_COMPLETE        0x05
#define EVT_CMD_STATUS             0x0F
#define EVT_AUTH_COMPLETE        0x06
#define EVT_REMOTE_NAME_REQ_COMPLETE    0x07
#define EVT_READ_REMOTE_FEATURES_COMPLETE    0x0B
#define EVT_READ_REMOTE_VERSION_COMPLETE    0x0C
#define EVT_CMD_COMPLETE         0x0E
#define EVT_RETURN_LINK_KEYS        0x15
#define EVT_PIN_CODE_REQ        0x16
#define EVT_LINK_KEY_REQ        0x17
#define EVT_LINK_KEY_NOTIFY        0x18
#define EVT_INQUIRY_RESULT_WITH_RSSI    0x22
#define EVT_READ_REMOTE_EXT_FEATURES_COMPLETE    0x23
#define EVT_EXTENDED_INQUIRY_RESULT    0x2F
#define EVT_IO_CAPABILITY_REQUEST    0x31
#define EVT_IO_CAPABILITY_RESPONSE    0x32
#define EVT_USER_CONFIRM_REQUEST    0x33
#define EVT_USER_PASSKEY_REQUEST    0x34
#define EVT_REMOTE_OOB_DATA_REQUEST    0x35
#define EVT_SIMPLE_PAIRING_COMPLETE    0x36
#define EVT_USER_PASSKEY_NOTIFY        0x3B
#define EVT_KEYPRESS_NOTIFY        0x3C
#define EVT_REMOTE_HOST_FEATURES_NOTIFY    0x3D
#define EVT_LE_META_EVENT    0x3E/* HCI device flags */

#define EVT_ENCRYPT_CHANGE  0x08
#define EVT_HARDWARE_ERROR  0x10
#define EVT_FLUSH_OCCURRED  0x11
#define EVT_ROLE_CHANGE     0x12
#define EVT_NUM_COMP_PKTS   0x13
#define EVT_MODE_CHANGE     0x14
#define EVT_SYNC_CONN_COMPLETE  0x2C
#define EVT_SYNC_CONN_CHANGED   0x2D
#define EVT_LINK_SUPERVISION_TIMEOUT_CHANGED  0x38

enum {
    HCI_UP,
    HCI_INIT,
    HCI_RUNNING,

    HCI_PSCAN,
    HCI_ISCAN,
    HCI_AUTH,
    HCI_ENCRYPT,
    HCI_INQUIRY,

    HCI_RAW,

    HCI_SETUP,
    HCI_AUTO_OFF,
    HCI_MGMT,
    HCI_PAIRABLE,
    HCI_SERVICE_CACHE,
    HCI_LINK_KEYS,
    HCI_DEBUG_KEYS,

    HCI_RESET,
};

typedef unsigned short sa_family_t;


struct hci_filter {
    uint32_t type_mask;
    uint32_t event_mask[2];
    uint16_t opcode;
};

struct sockaddr_hci {
    sa_family_t     hci_family;
    unsigned short  hci_dev;
    unsigned short  hci_channel;
};

static struct dev_info {
    int id;
    int sk;

    int already_up;
} devs;

typedef struct {
    uint8_t b[6];
} __attribute__((__packed__)) bdaddr_t;

struct hci_dev_req {
    uint16_t dev_id;
    uint32_t dev_opt;
};

struct hci_dev_list_req {
    uint16_t dev_num;
    struct hci_dev_req dev_req[0];  /* hci_dev_req structures */
};

struct hci_dev_stats {
    uint32_t err_rx;
    uint32_t err_tx;
    uint32_t cmd_tx;
    uint32_t evt_rx;
    uint32_t acl_tx;
    uint32_t acl_rx;
    uint32_t sco_tx;
    uint32_t sco_rx;
    uint32_t byte_rx;
    uint32_t byte_tx;
};

struct hci_dev_info {
    uint16_t dev_id;
    char  name[8];

    bdaddr_t bdaddr;

    uint32_t flags;
    uint8_t  type;

    uint8_t  features[8];

    uint32_t pkt_type;
    uint32_t link_policy;
    uint32_t link_mode;

    uint16_t acl_mtu;
    uint16_t acl_pkts;
    uint16_t sco_mtu;
    uint16_t sco_pkts;

    struct hci_dev_stats stat;
};

static int commPort = 0;

//---------------------------------------------------------------------------
static inline int hci_test_bit(int nr, void *addr)
{
    return *((uint32_t *) addr + (nr >> 5)) & (1 << (nr & 31));
}

static inline void hci_set_bit(int nr, void *addr)
{
    *((uint32_t *) addr + (nr >> 5)) |= (1 << (nr & 31));
}

static inline void hci_filter_clear(struct hci_filter *f)
{
    memset(f, 0, sizeof(*f));
}

static inline void hci_filter_set_ptype(int t, struct hci_filter *f)
{
    hci_set_bit((t == HCI_VENDOR_PKT) ? 0 : (t & HCI_FLT_TYPE_BITS), &f->type_mask);
}

#if 0
static inline void hci_filter_all_events(struct hci_filter *f)
{
    memset((void *) f->event_mask, 0xff, sizeof(f->event_mask));
}
#endif

static inline void hci_filter_set_event(int e, struct hci_filter *f)
{
    hci_set_bit((e & HCI_FLT_EVENT_BITS), &f->event_mask);
}

//---------------------------------------------------------------------------
int hci_open_dev(int dev_id)
{
    struct sockaddr_hci a;
    int dd, err;

    /* Create HCI socket */
    dd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (dd < 0)
        return dd;

    BPRINT_D("sock = %x", dd);
    /* Bind socket to the HCI device */
    memset(&a, 0, sizeof(a));
    a.hci_family = AF_BLUETOOTH;
    a.hci_dev = dev_id;
    /* 1 means HCI_USER_CHANNEL */
    a.hci_channel = 1;
    if (bind(dd, (struct sockaddr *) &a, sizeof(a)) < 0)
        goto failed;

#if 0 /* No need for 3.18 */
    if (ioctl(dd, HCISETRAW, 1) < 0)
    {
        BPRINT_E("Can't set HCI RAW: %s (%d)", strerror(errno), errno);
        goto failed;
    }
#endif
    return dd;

failed:
    err = errno;
    close(dd);
    errno = err;

    return -1;
}

//---------------------------------------------------------------------------
int hci_close_dev(int dd)
{
    BPRINT_D("hci_close_dev %d", dd);
    return close(dd);
}

//---------------------------------------------------------------------------
static struct dev_info *init_dev_info(int index, int sk, int already_up)
{
    struct dev_info *dev = &devs;

    memset(dev, 0, sizeof(*dev));

    dev->id = index;
    dev->sk = sk;
    commPort = sk;
    BPRINT_D("%s: commPort = %d", __func__, commPort);
    dev->already_up = already_up;
    return dev;
}

//---------------------------------------------------------------------------
static void start_hci_dev(int index)
{
    struct dev_info *dev = &devs;
    struct hci_filter flt;

    BPRINT_I("Listening for HCI events on hci%d", index);

    /* Set filter */
    hci_filter_clear(&flt);

    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_set_ptype(HCI_ACLDATA_PKT, &flt);
    hci_filter_set_ptype(HCI_SCODATA_PKT, &flt);
   // hci_filter_all_events(&flt);

    hci_filter_set_event(EVT_CMD_STATUS, &flt);
    hci_filter_set_event(EVT_CMD_COMPLETE, &flt);
    hci_filter_set_event(EVT_PIN_CODE_REQ, &flt);
    hci_filter_set_event(EVT_LINK_KEY_REQ, &flt);
    hci_filter_set_event(EVT_LINK_KEY_NOTIFY, &flt);
    hci_filter_set_event(EVT_RETURN_LINK_KEYS, &flt);
    hci_filter_set_event(EVT_IO_CAPABILITY_REQUEST, &flt);
    hci_filter_set_event(EVT_IO_CAPABILITY_RESPONSE, &flt);
    hci_filter_set_event(EVT_USER_CONFIRM_REQUEST, &flt);
    hci_filter_set_event(EVT_USER_PASSKEY_REQUEST, &flt);
    hci_filter_set_event(EVT_REMOTE_OOB_DATA_REQUEST, &flt);
    hci_filter_set_event(EVT_USER_PASSKEY_NOTIFY, &flt);
    hci_filter_set_event(EVT_KEYPRESS_NOTIFY, &flt);
    hci_filter_set_event(EVT_SIMPLE_PAIRING_COMPLETE, &flt);
    hci_filter_set_event(EVT_AUTH_COMPLETE, &flt);
    hci_filter_set_event(EVT_REMOTE_NAME_REQ_COMPLETE, &flt);
    hci_filter_set_event(EVT_READ_REMOTE_VERSION_COMPLETE, &flt);
    hci_filter_set_event(EVT_READ_REMOTE_FEATURES_COMPLETE, &flt);
    hci_filter_set_event(EVT_REMOTE_HOST_FEATURES_NOTIFY, &flt);
    hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
    hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
    hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
    hci_filter_set_event(EVT_EXTENDED_INQUIRY_RESULT, &flt);
    hci_filter_set_event(EVT_READ_REMOTE_EXT_FEATURES_COMPLETE, &flt);
    hci_filter_set_event(EVT_CONN_REQUEST, &flt);
    hci_filter_set_event(EVT_CONN_COMPLETE, &flt);
    hci_filter_set_event(EVT_DISCONN_COMPLETE, &flt);
    hci_filter_set_event(EVT_ENCRYPT_CHANGE, &flt);
    hci_filter_set_event(EVT_HARDWARE_ERROR, &flt);
    hci_filter_set_event(EVT_FLUSH_OCCURRED, &flt);
    hci_filter_set_event(EVT_ROLE_CHANGE, &flt);
    hci_filter_set_event(EVT_NUM_COMP_PKTS, &flt);
    hci_filter_set_event(EVT_MODE_CHANGE, &flt);
    hci_filter_set_event(EVT_SYNC_CONN_COMPLETE, &flt);
    hci_filter_set_event(EVT_SYNC_CONN_CHANGED, &flt);
    hci_filter_set_event(EVT_LINK_SUPERVISION_TIMEOUT_CHANGED, &flt);
    hci_filter_set_event(EVT_LE_META_EVENT, &flt);

    if (setsockopt(dev->sk, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
        BPRINT_E("Can't set filter on hci%d: %s (%d)", index, strerror(errno), errno);
        return;
    }

#if 0
    if (fcntl(dev->sk, F_SETFL, fcntl(dev->sk, F_GETFL, 0) | O_NONBLOCK) < 0) {
        BPRINT_E("Can't set non blocking mode: %s (%d)", strerror(errno), errno);
        return;
    }
#endif
}

//---------------------------------------------------------------------------
static struct dev_info *init_device(int index, int already_up)
{
    struct dev_info *dev;
    struct hci_dev_req dr;
    int dd;

    BPRINT_D("%s: hci = %d, already_up = %d", __func__, index, already_up);

    /* Hard code for hci0 */
    index = 0;
    BPRINT_I("open hci%d", index);
    dd = hci_open_dev(index);
    if (dd < 0) {
        BPRINT_E("Unable to open hci%d: %s (%d)", index,
                        strerror(errno), errno);
        return NULL;
    }


    dev = init_dev_info(index, dd, already_up);
    start_hci_dev(index);

    /* Avoid forking if nothing else has to be done */
    if (already_up)
        return dev;

    /* Do initialization in the separate process */
    memset(&dr, 0, sizeof(dr));
    dr.dev_id = index;

    /* Start HCI device */
    /* If we use HCI_CHANNEL_RAW, we must be send HCIDEVUP to bluez stack */
    /*
    while (ioctl(dd, HCIDEVUP, index) < 0 && errno != EALREADY) {
    BPRINT_E("Can't init device hci%d: %s (%d)",
                index, strerror(errno), errno);
          sleep(1);
    }*/
    return dev;

    /*
fail:
    hci_close_dev(dd);
    return NULL;
    */
}

//---------------------------------------------------------------------------
static int init_known_adapters(int ctl)
{
    struct hci_dev_list_req *dl;
    struct hci_dev_req *dr;
    int i, err;
    size_t req_size;

    req_size = HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t);

    dl = (struct hci_dev_list_req *)malloc(req_size);
    if (!dl) {
        BPRINT_E("Can't allocate devlist buffer");
        return -1;
    }

    dr = dl->dev_req;

    while (1) {
        dl->dev_num = HCI_MAX_DEV;
        if (ioctl(ctl, HCIGETDEVLIST, dl) < 0) {
            err = -errno;
            BPRINT_E("Can't get device list: %s (%d)", strerror(-err), -err);
            free(dl);
            return -1;
        }

        if (dl->dev_num == 0) {
            BPRINT_E("No BT adapter. return error....");
            free(dl);
            return -1;
            //usleep(300000);
        } else {
            break;
        }
    }

    BPRINT_D("dl->dev_num = %d", dl->dev_num);

    for (i = 0; i < dl->dev_num; i++, dr++) {
        struct dev_info *dev;
        int already_up;

        already_up = hci_test_bit(HCI_UP, &dr->dev_opt);
        BPRINT_D("already_up=%d", already_up);

        dev = init_device(dr->dev_id, already_up);
        if (dev == NULL)
            continue;

        if (!dev->already_up)
            continue;

        //device_event(HCI_DEV_UP, dr->dev_id);
        BPRINT_D("HCI dev %d up", dr->dev_id);
    }
    free(dl);
    return 0;
}

//---------------------------------------------------------------------------
int mtk_hciops_setup(void)
{
    int sock, err;
    int count = 3, ret = 0;

    while (count-- > 0) {
        /* Create and bind HCI socket */
        sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
        if (sock < 0) {
            err = -errno;
            BPRINT_E("Can't open HCI socket: %s (%d)", strerror(-err), -err);
            return err;
        }
        BPRINT_D("sock = %d", sock);

        /* Set filter */
        ret = init_known_adapters(sock);
        hci_close_dev(sock);
        if (!ret)
            break;
        (void)usleep(300000);
    }

    if (0 != ret)
    {
        BPRINT_E("init_known_adapters failed : %d", ret);
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int mtk_hci_enable(char *arg, void *func_cb)
{
    UNUSED(boots_btif);
    UNUSED(arg);
    UNUSED(func_cb);

    if (mtk_hciops_setup() < 0) {
        BPRINT_E("adapter_ops_setup failed");
        return -1;
    }
    else {
        return commPort;
    }
}

//---------------------------------------------------------------------------
static void stop_hci_dev(int index)
{
    struct dev_info *dev = &devs;

    if (dev->sk < 0)
        return;

    BPRINT_D("Stopping hci%d event socket %d", index, dev->sk);

    hci_close_dev(dev->sk);
    init_dev_info(index, -1, dev->already_up);
}

#define ENABLE_BT_WAKEUP _IOW('H', 3, int)
#define DISABLE_BT_WAKEUP _IOW('H', 2, int)
#define START_BT_WAKEUP _IOW('H', 4, int)
#define STOP_BT_WAKEUP _IOW('H', 5, int)

int mtk_wakeup_op(int cmd)
{
    int ret=0;
    switch (cmd)
    {
        case 0:
            ret = ioctl(commPort, DISABLE_BT_WAKEUP, 0);
            break;
        case 1:
            ret = ioctl(commPort, ENABLE_BT_WAKEUP, 0);
            break;
        case 2:
            ret = ioctl(commPort, START_BT_WAKEUP, 0);
            break;
        case 3:
            ret = ioctl(commPort, STOP_BT_WAKEUP, 0);
            break;
        default:
            break;
    }
    return ret;
}

//---------------------------------------------------------------------------
void mtk_hciops_teardown(void)
{
    stop_hci_dev(0);
}

//---------------------------------------------------------------------------
int mtk_hci_disable(int bt_fd)
{
    UNUSED(bt_fd);

    if (commPort >= 0) {
        mtk_hciops_teardown();
        commPort = -1;
    }
    return 0x00;
}

#if 0
void main(int argc, char *argv[])
{
    mtk_bt_hci_disable(mtk_bt_hci_enable(0, NULL));
}
#endif

