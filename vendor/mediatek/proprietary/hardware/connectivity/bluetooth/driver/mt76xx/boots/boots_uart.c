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
 * MediaTek Inc. (C) 2014,2016~2017. All rights reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>

#include "boots.h"
#include "boots_osi.h"

//---------------------------------------------------------------------------
#define LOG_TAG             "boots_uart"

#define VCOM_PROP_NAME            "sys.usb.config"
#define VCOM_PROP_VALUE_TTYGS0   "acm"
#define VCOM_PROP_VALUE_TTYGS2   "acm_third"

#define CHECK_VCOM_DEV "/sys/class/android_usb/android0/enable"
#ifdef SPECIAL_USB_RELAY
#define OPEN_VCOM_FOR_TTYGS2 "usb_switch adb,bt"
#else
#define OPEN_VCOM_FOR_TTYGS2 \
    "echo > /sys/class/android_usb/android0/enable 0;" \
    "sleep 1;" \
    "echo > /sys/class/android_usb/android0/idVendor 0E8D;" \
    "echo > /sys/class/android_usb/android0/idProduct 2006;" \
    "echo > /sys/class/android_usb/android0/f_acm/port_index 3;" \
    "echo > /sys/class/android_usb/android0/functions mass_storage,adb,acm;" \
    "sleep 1;" \
    "echo > /sys/class/android_usb/android0/enable 1"
#endif

#define CHECK_VCOM_DEV_FOR_NEW_USB_1 "/sys/kernel/config/usb_gadget/g1/UDC"
#define CHECK_VCOM_DEV_FOR_NEW_USB_2 "/config/usb_gadget/g1/UDC"
#ifdef UDC_SSUSB    //for new USB IP SSUSB
#define OPEN_VCOM_FOR_TTYGS2_NEW_USB \
    "echo -n none > /sys/kernel/config/usb_gadget/g1/UDC;" \
    "rm /sys/kernel/config/usb_gadget/g1/configs/c.1/f*;" \
    "sleep 1;" \
    "mount -t configfs none /sys/kernel/config;" \
    "mkdir /sys/kernel/config/usb_gadget/g1;" \
    "echo -n 0x0E8D > /sys/kernel/config/usb_gadget/g1/idVendor;" \
    "echo -n 0x2010 > /sys/kernel/config/usb_gadget/g1/idProduct;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/strings/0x409;" \
    "echo -n 0123456789ABCDEF > /sys/kernel/config/usb_gadget/g1/strings/0x409/serialnumber;" \
    "echo -n Mediatek Inc. > /sys/kernel/config/usb_gadget/g1/strings/0x409/manufacturer;" \
    "echo -n rndis_adb_acm > /sys/kernel/config/usb_gadget/g1/strings/0x409/product;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/configs/c.1;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/configs/c.1/strings/0x409;" \
    "echo -n rndis_adb_acm > /sys/kernel/config/usb_gadget/g1/configs/c.1/strings/0x409/configuration;" \
    "echo -n 400 > /sys/kernel/config/usb_gadget/g1/configs/c.1/MaxPower;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/functions/ffs.adb;" \
    "mkdir /dev/usb-ffs;" \
    "chmod 775 /dev/usb-ffs -R;" \
    "mkdir /dev/usb-ffs/adb;" \
    "chmod 775 /dev/usb-ffs/adb -R;" \
    "mount -o uid=2000,gid=2000 -t functionfs adb /dev/usb-ffs/adb;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/functions/rndis.gs4;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/functions/acm.gs0;" \
    "ln -s /sys/kernel/config/usb_gadget/g1/functions/rndis.gs4 /sys/kernel/config/usb_gadget/g1/configs/c.1/f1;" \
    "ln -s /sys/kernel/config/usb_gadget/g1/functions/ffs.adb /sys/kernel/config/usb_gadget/g1/configs/c.1/f2;" \
    "ln -s /sys/kernel/config/usb_gadget/g1/functions/acm.gs0 /sys/kernel/config/usb_gadget/g1/configs/c.1/f3;" \
    "sleep 1;" \
    "echo -n '11201000.usb' > /sys/kernel/config/usb_gadget/g1/UDC;"
#else    //for MUSB
#define OPEN_VCOM_FOR_TTYGS2_NEW_USB \
    "echo -n none > /sys/kernel/config/usb_gadget/g1/UDC;" \
    "rm /sys/kernel/config/usb_gadget/g1/configs/c.1/f*;" \
    "sleep 1;" \
    "mount -t configfs none /sys/kernel/config;" \
    "mkdir /sys/kernel/config/usb_gadget/g1;" \
    "echo -n 0x0E8D > /sys/kernel/config/usb_gadget/g1/idVendor;" \
    "echo -n 0x2010 > /sys/kernel/config/usb_gadget/g1/idProduct;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/strings/0x409;" \
    "echo -n 0123456789ABCDEF > /sys/kernel/config/usb_gadget/g1/strings/0x409/serialnumber;" \
    "echo -n Mediatek Inc. > /sys/kernel/config/usb_gadget/g1/strings/0x409/manufacturer;" \
    "echo -n rndis_adb_acm > /sys/kernel/config/usb_gadget/g1/strings/0x409/product;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/configs/c.1;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/configs/c.1/strings/0x409;" \
    "echo -n rndis_adb_acm > /sys/kernel/config/usb_gadget/g1/configs/c.1/strings/0x409/configuration;" \
    "echo -n 400 > /sys/kernel/config/usb_gadget/g1/configs/c.1/MaxPower;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/functions/ffs.adb;" \
    "mkdir /dev/usb-ffs;" \
    "chmod 775 /dev/usb-ffs -R;" \
    "mkdir /dev/usb-ffs/adb;" \
    "chmod 775 /dev/usb-ffs/adb -R;" \
    "mount -o uid=2000,gid=2000 -t functionfs adb /dev/usb-ffs/adb;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/functions/rndis.gs4;" \
    "mkdir /sys/kernel/config/usb_gadget/g1/functions/acm.gs0;" \
    "ln -s /sys/kernel/config/usb_gadget/g1/functions/rndis.gs4 /sys/kernel/config/usb_gadget/g1/configs/c.1/f1;" \
    "ln -s /sys/kernel/config/usb_gadget/g1/functions/ffs.adb /sys/kernel/config/usb_gadget/g1/configs/c.1/f2;" \
    "ln -s /sys/kernel/config/usb_gadget/g1/functions/acm.gs0 /sys/kernel/config/usb_gadget/g1/configs/c.1/f3;" \
    "sleep 1;" \
    "echo -n musb-hdrc > /sys/kernel/config/usb_gadget/g1/UDC;"
#endif

//---------------------------------------------------------------------------
static int boots_uart_speed(int speed)
{
    switch (speed) {
      case 9600:
        return B9600;
      case 19200:
        return B19200;
      case 38400:
        return B38400;
      case 57600:
        return B57600;
      case 115200:
        return B115200;
      case 230400:
        return B230400;
      case 460800:
        return B460800;
      case 500000:
        return B500000;
      case 576000:
        return B576000;
      case 921600:
        return B921600;
    }
    return B57600;
}

//---------------------------------------------------------------------------
int boots_uart_init(char *dev, int speed)
{
    UNUSED(boots_btif);
    struct termios ti;
    int fd, retry = 0;
    int baudenum;
#ifndef VCOM_OPENED
    char usb_prop[128];
    char *prop_value = NULL;
    bool set_vcom_cmd = false;
#endif

    memset(&ti, 0x0, sizeof(ti));
    /* USB VCOM port */
    if (0 == speed) {
#ifndef VCOM_OPENED
        /* Set USB property to acm_third: add 1 acm port to /dev/ttyGS2 */
        if (!memcmp(dev, "/dev/ttyGS0", strlen("/dev/ttyGS0")))
            prop_value = VCOM_PROP_VALUE_TTYGS0;
        else if (!memcmp(dev, "/dev/ttyGS2", strlen("/dev/ttyGS2")))
            prop_value = VCOM_PROP_VALUE_TTYGS2;
        else {
            BPRINT_E("%s do not support VCOM port", dev);
            return -1;
        }

        if (osi_property_get(VCOM_PROP_NAME, usb_prop, NULL) < 0) {
            set_vcom_cmd = true;
        } else {
            if (strcmp(usb_prop, prop_value)) {
                if (osi_property_set(VCOM_PROP_NAME, prop_value) < 0)
                    set_vcom_cmd = true;
            }
        }

        if (set_vcom_cmd == true) {
#ifndef SPECIAL_USB_RELAY
                if (access(CHECK_VCOM_DEV, F_OK) == 0) {
#endif
                    if (osi_system(OPEN_VCOM_FOR_TTYGS2) < 0) {
                        BPRINT_E("Open default VCOM failed!");
                        return -1;
                    } else {
                        BPRINT_I("Open default VCOM successfully");
                    }
#ifndef SPECIAL_USB_RELAY
                } else {
                        BPRINT_I("Use new usb");
                }

                if (access(CHECK_VCOM_DEV_FOR_NEW_USB_1, F_OK) == 0 ||
                        access(CHECK_VCOM_DEV_FOR_NEW_USB_2, F_OK) == 0) {
                    if (osi_system(OPEN_VCOM_FOR_TTYGS2_NEW_USB) < 0) {
                        BPRINT_E("Open default VCOM failed!");
                        return -1;
                    } else {
                        BPRINT_I("Open default VCOM successfully");
                    }
                }
#endif
        }
#endif
    }

    while(1) {
        sleep(1);
        fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd >= 0)
            break;
        if (retry > 10) {
            BPRINT_E("Can't open serial port %s", dev);
            return -1;
        }
        retry++;
    }

    if (0 != speed) {
        ti.c_cflag |= CLOCAL;
        ti.c_lflag = 0;

        ti.c_cflag &= ~CRTSCTS;
        ti.c_iflag &= ~(IXON | IXOFF | IXANY);

        /* Set baudrate */
        baudenum = boots_uart_speed(speed);
        if ((baudenum == B57600) && (speed != 57600)) {
            BPRINT_E("Serial port baudrate not supported!");
            close(fd);
            return -1;
        }

        cfsetospeed(&ti, baudenum);
        cfsetispeed(&ti, baudenum);
    }

    tcflush(fd, TCIOFLUSH);

    if (tcgetattr(fd, &ti) < 0) {
        BPRINT_E("Can't get serial port setting");
        close(fd);
        return -1;
    }

    cfmakeraw(&ti);

    sleep(1);

    if (tcsetattr(fd, TCSANOW, &ti) < 0) {
        BPRINT_E("Can't set serial port setting");
        close(fd);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);

    return fd;
}

//---------------------------------------------------------------------------
static ssize_t uart_read(int fd, uint8_t *buf, size_t len)
{
    ssize_t bytesRead = 0;
    size_t bytesToRead = len;
    int ret = 0;
    struct timeval tv;
    fd_set readfd;

    tv.tv_sec = 1;  /* SECOND */
    tv.tv_usec = 0; /* USECOND */
    FD_ZERO(&readfd);

    if (fd < 0)
        return -1;

    /* Try to receive len bytes */
    while (bytesToRead > 0) {
        FD_SET(fd, &readfd);

        ret = select(fd + 1, &readfd, NULL, NULL, &tv);
        if (ret > 0) {
            bytesRead = read(fd, buf, bytesToRead);
            if (bytesRead < 0) {
                if (errno == EINTR || errno == EAGAIN)
                    continue;
                else
                    return -1;
            } else {
                bytesToRead -= bytesRead;
                buf += bytesRead;
            }
        } else if (ret == 0) {
            /* Read com port timeout */
            return -1;
        } else if ((ret == -1) && (errno == EINTR)) {
            continue;
        } else {
            return -1;
        }
    }

    return (len - bytesToRead);
}

//---------------------------------------------------------------------------
ssize_t boots_uart_write(int fd, uint8_t const *buf, size_t len)
{
    ssize_t ret = 0;
    size_t bytesToWrite = len;

    if (buf == NULL) {
        BPRINT_E("%s: buffer is NULL!", __func__);
        return -EINVAL;
    } else if (fd < 0) {
        BPRINT_E("%s: File descriptor in bad state(%d)!", __func__, fd);
        return -EBADFD;
    }

    while (bytesToWrite > 0) {
        ret = write(fd, buf, bytesToWrite);
        if (ret < 0) {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            else
                return -errno;
        }
        bytesToWrite -= ret;
        buf += ret;
    }

    return (len - bytesToWrite);
}

//---------------------------------------------------------------------------
ssize_t boots_uart_read(int fd, uint8_t *buf, size_t size)
{
    uint8_t ucHeader = 0;
    size_t u4Len = 0, pkt_len = 0;
    int count = 0;
    size_t len = size;

    if (buf == NULL) {
        BPRINT_E("%s: buffer is NULL", __func__);
        return -EINVAL;
    } else if (fd < 0) {
        BPRINT_E("%s: File descriptor in bad state(%d)!", __func__, fd);
        return -EBADFD;
    }

LOOP:
    if (uart_read(fd, &ucHeader, sizeof(ucHeader)) < 0) {
        count ++;
        if (count < 3) {
            goto LOOP;
        } else {
            return -2;
        }
    }
    buf[0] = ucHeader;
    u4Len++;

    switch (ucHeader) {
    case 0x01:      /* HCI command */
        if (uart_read(fd, buf + u4Len, 3) < 0) {
            BPRINT_E("Read command header fails");
            return -3;
        }
        u4Len += 3;
        pkt_len = (size_t)buf[3];
        if ((u4Len + pkt_len) > len) {
            BPRINT_E("Too large packet from UART! packet len %d", (int)(u4Len + pkt_len));
            return -4;
        }

        if (uart_read(fd, buf + u4Len, pkt_len) < 0) {
            BPRINT_E("Read command param fails");
            return -5;
        }
        u4Len += pkt_len;
        break;

    case 0x02:      /* ACL data */
        if (uart_read(fd, buf + u4Len, 4) < 0) {
            BPRINT_E("Read ACL header fails");
            return -6;
        }
        u4Len += 4;
        /* length 2 bytes is little endian */
        pkt_len = (((size_t)buf[4]) << 8);
        pkt_len += (size_t)buf[3];
        if ((u4Len + pkt_len) > len) {
            BPRINT_E("Too large packet from UART! packet len %d", (int)(u4Len + pkt_len));
            return -7;
        }

        if (uart_read(fd, buf + u4Len, pkt_len) < 0) {
            BPRINT_E("Read ACL data fails");
            return -8;
        }
        u4Len += pkt_len;
        break;

    case 0x03:      /* SCO data */
        if (uart_read(fd, buf + u4Len, 3) < 0) {
            BPRINT_E("Read SCO header fails");
            return -9;
        }
        u4Len += 3;
        pkt_len = (size_t)buf[3];
        if ((u4Len + pkt_len) > len) {
            BPRINT_E("Too large packet from UART! packet len %d", (int)(u4Len + pkt_len));
            return -10;
        }

        if (uart_read(fd, buf + u4Len, pkt_len) < 0) {
            BPRINT_E("Read SCO data fails");
            return -11;
        }
        u4Len += pkt_len;
        break;

    case 0x04:      /* Event */
        if (uart_read(fd, buf + u4Len, 2) < 0) {
            BPRINT_E("Read Event header fails");
            return -12;
        }
        u4Len += 2;
        pkt_len = (size_t)buf[2];
        if ((u4Len + pkt_len) > len) {
            BPRINT_E("Too large packet from UART! packet len %d", (int)(u4Len + pkt_len));
            return -13;
        }

        if (uart_read(fd, buf + u4Len, pkt_len) < 0) {
            BPRINT_E("Read Event data fails");
            return -14;
        }
        u4Len += pkt_len;
        break;

    default:    /* Filter UART garbage data */
        BPRINT_E("Invalid packet type %02x from UART", ucHeader);
        return -15;
    }

    return u4Len;
}

//---------------------------------------------------------------------------
