/*
 * Copyright (C) 2007 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mount.h>
#include <unistd.h>
#include <log/log.h>
#include "mounts.h"


typedef struct {
    MountedVolume *volumes;
    int volumes_allocd;
    int volume_count;
} MountsState;

static MountsState g_mounts_state = {
    NULL,   // volumes
    0,      // volumes_allocd
    0       // volume_count
};

static inline void
free_volume_internals(const MountedVolume *volume, int zero)
{
    free((char *)volume->device);
    free((char *)volume->mount_point);
    free((char *)volume->filesystem);
    free((char *)volume->flags);
    if (zero) {
        memset((void *)volume, 0, sizeof(*volume));
    }
}

#define PROC_MOUNTS_FILENAME   "/proc/mounts"

int
scan_mounted_volumes()
{
    char *buf = NULL;
    const char *bufp;
    int fd = -1, len, len2, numv = 32;
    ssize_t nbytes = 0;

    if (g_mounts_state.volumes == NULL) {
        MountedVolume *volumes = malloc(numv * sizeof(*volumes));
        if (volumes == NULL) {
            errno = ENOMEM;
            return -1;
        }
        g_mounts_state.volumes = volumes;
        g_mounts_state.volumes_allocd = numv;
        memset(volumes, 0, numv * sizeof(*volumes));
    } else {
        /* Free the old volume strings.
         */
        int i;
        for (i = 0; i < g_mounts_state.volume_count; i++) {
            free_volume_internals(&g_mounts_state.volumes[i], 1);
        }
    }
    g_mounts_state.volume_count = 0;

    /* Open and read the file contents.
     */
    fd = open(PROC_MOUNTS_FILENAME, O_RDONLY);
    if (fd < 0)
        goto bail;

    /* Since /proc/mounts cannot be seeked to determine length,
     * we use try to read more return value of read() equal buffer length
     */
    len = 2048;
    buf = malloc(len + 1);
    if (!buf) {
        ALOGE("Fail to allocate %d bytes memory for reading " PROC_MOUNTS_FILENAME "\n", len);
        goto bail;
    }

    do {
        len2 = read(fd, buf + nbytes, len - nbytes);
        if (len2 < 0) {
            goto bail;
        } else if (len2 == len - nbytes) {
            nbytes += len2;
            len *= 2;
            buf = realloc(buf, len+1);
            if (!buf) {
                ALOGE("Fail to allocate %d bytes memory for reading " PROC_MOUNTS_FILENAME "\n", len);
                goto bail;
            }
        } else if (len2 < len - nbytes) {
            nbytes += len2;
            break;
        } else if (len2 > len - nbytes) {
           goto bail;
        }
    } while (1);
    ALOGE("Length of " PROC_MOUNTS_FILENAME " is %zd\n", nbytes);
    buf[nbytes] = '\0';

    /* Parse the contents of the file, which looks like:
     *
     *     # cat /proc/mounts
     *     rootfs / rootfs rw 0 0
     *     /dev/pts /dev/pts devpts rw 0 0
     *     /proc /proc proc rw 0 0
     *     /sys /sys sysfs rw 0 0
     *     /dev/block/mtdblock4 /system yaffs2 rw,nodev,noatime,nodiratime 0 0
     *     /dev/block/mtdblock5 /data yaffs2 rw,nodev,noatime,nodiratime 0 0
     *     /dev/block/mmcblk0p1 /sdcard vfat rw,sync,dirsync,fmask=0000,dmask=0000,codepage=cp437,iocharset=iso8859-1,utf8 0 0
     *
     * The zeroes at the end are dummy placeholder fields to make the
     * output match Linux's /etc/mtab, but don't represent anything here.
     */
    bufp = buf;
    while (nbytes > 0) {
        char device[64];
        char mount_point[64];
        char filesystem[64];
        char flags[128];
        int matches;

        /* %as is a gnu extension that malloc()s a string for each field.
         */
        matches = sscanf(bufp, "%63s %63s %63s %127s",
                device, mount_point, filesystem, flags);

        if (matches == 4) {
            device[sizeof(device)-1] = '\0';
            mount_point[sizeof(mount_point)-1] = '\0';
            filesystem[sizeof(filesystem)-1] = '\0';
            flags[sizeof(flags)-1] = '\0';

            if (g_mounts_state.volume_count >= numv) {
                numv *= 2;
                g_mounts_state.volumes =
                    realloc(g_mounts_state.volumes, numv * sizeof(MountedVolume));
                if (g_mounts_state.volumes == NULL) {
                    errno = ENOMEM;
                    goto bail;
                }
                g_mounts_state.volumes_allocd = numv;
                memset(g_mounts_state.volumes + numv/2, 0, numv/2 * sizeof(MountedVolume));
            }

            MountedVolume *v =
                    &g_mounts_state.volumes[g_mounts_state.volume_count++];
            v->device = strdup(device);
            v->mount_point = strdup(mount_point);
            v->filesystem = strdup(filesystem);
            v->flags = strdup(flags);
        } else {
            printf("matches was %d on <<%.40s>>\n", matches, bufp);
        }

        /* Eat the line.
         */
        while (nbytes > 0 && *bufp != '\n') {
            bufp++;
            nbytes--;
        }
        if (nbytes > 0) {
            bufp++;
            nbytes--;
        }
    }

    close(fd);
    free(buf);

    return 0;

bail:
//TODO: free the strings we've allocated.
    g_mounts_state.volume_count = 0;
    if (fd >= 0)
        close(fd);
    if (buf)
        free(buf);
    return -1;
}

const MountedVolume *
find_mounted_volume_by_device(const char *device)
{
    if (g_mounts_state.volumes != NULL) {
        int i;
        for (i = 0; i < g_mounts_state.volume_count; i++) {
            MountedVolume *v = &g_mounts_state.volumes[i];
            /* May be null if it was unmounted and we haven't rescanned.
             */
            if (v->device != NULL) {
                if (strcmp(v->device, device) == 0) {
                    return v;
                }
            }
        }
    }
    return NULL;
}

const MountedVolume *
find_mounted_volume_by_mount_point(const char *mount_point)
{
    if (g_mounts_state.volumes != NULL) {
        int i;
        for (i = 0; i < g_mounts_state.volume_count; i++) {
            MountedVolume *v = &g_mounts_state.volumes[i];
            /* May be null if it was unmounted and we haven't rescanned.
             */
            if (v->mount_point != NULL) {
                if (strcmp(v->mount_point, mount_point) == 0) {
                    return v;
                }
            }
        }
    }
    return NULL;
}

int
unmount_mounted_volume(const MountedVolume *volume)
{
    /* Intentionally pass NULL to umount if the caller tries
     * to unmount a volume they already unmounted using this
     * function.
     */
    sync();
    int ret = umount(volume->mount_point);
    if (ret == 0) {
        sync();
        free_volume_internals(volume, 1);
    }
    return ret;
}
