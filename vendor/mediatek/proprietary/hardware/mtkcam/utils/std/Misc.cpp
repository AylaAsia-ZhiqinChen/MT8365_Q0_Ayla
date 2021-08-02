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
 * MediaTek Inc. (C) 2010. All rights reserved.
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

#define LOG_TAG "MtkCam/Utils"
//
#include "MyUtils.h"
using namespace android;
//
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/resource.h>
//
#include <cutils/properties.h>
#include <utils/Mutex.h> // android::Mutex
//
#include <cassert> // assert
#include <libladder.h>
#include <mtkcam/utils/std/ULog.h>

CAM_ULOG_DECLARE_MODULE_ID(MOD_UTILITY);

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_ULOGMV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_ULOGMD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_ULOGMI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_ULOGMW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_ULOGME("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)


namespace NSCam {
namespace Utils {
/******************************************************************************
 *
 ******************************************************************************/


/******************************************************************************
 * @brief dump call stack
 ******************************************************************************/
void
dumpCallStack(char const* prefix)
{
    std::string callstack;
    UnwindCurThreadBT(&callstack);
    std::string tag = std::string(LOG_TAG);
    if(prefix) {
        tag += std::string("/");
        tag += std::string(prefix);
    }
    __android_log_write(ANDROID_LOG_INFO, tag.c_str(), callstack.c_str());
}


/******************************************************************************
 *
 ******************************************************************************/
static bool do_mkdir(char const*const path, unsigned int const mode)
{
    struct stat st;
    //
    if  ( 0 != ::stat(path, &st) )
    {
        //  Directory does not exist.
        if  ( 0 != ::mkdir(path, mode) && EEXIST != errno )
        {
            MY_LOGE("fail to mkdir [%s]: %d[%s]", path, errno, ::strerror(errno));
            return  false;
        }
    }
    else if ( ! S_ISDIR(st.st_mode) )
    {
        MY_LOGE("!S_ISDIR");
        return  false;
    }
    //
    return  true;
}


/******************************************************************************
 * @brief make all directories in path.
 *
 * @details
 * @note
 *
 * @param[in] path: a specified path to create.
 * @param[in] mode: the argument specifies the permissions to use, like 0777
 *                 (the same to that in mkdir).
 *
 * @return
 * -    true indicates success
 * -    false indicates failure
 *****************************************************************************/
bool
makePath(char const*const path, unsigned int const mode)
{
    bool ret = true;
    char*copypath = strdup(path);
    if(copypath == NULL)
        return false;
    char*pp = copypath;
    char*sp;
    while ( ret && 0 != (sp = strchr(pp, '/')) )
    {
        if (sp != pp)
        {
            *sp = '\0';
            ret = do_mkdir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (ret) {
        ret = do_mkdir(path, mode);
    }
    free(copypath);
    return  ret;
}


/******************************************************************************
 * @brief save the buffer to the file
 *
 * @details
 *
 * @note
 *
 * @param[in] fname: The file name
 * @param[in] buf: The buffer to save
 * @param[in] size: The size in bytes to save
 *
 * @return
 * -   true indicates success
 * -   false indicates failure
 ******************************************************************************/
bool
saveBufToFile(
    char const*const    fname,
    unsigned char*const buf,
    unsigned int const  size
)
{
    int nw, cnt = 0;
    uint32_t written = 0;

    MY_LOGD("opening file [%s]", fname);
    int fd = ::open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd < 0) {
        MY_LOGE("failed to create file [%s]: %s", fname, ::strerror(errno));
        return false;
    }

    MY_LOGD("writing %d bytes to file [%s]", size, fname);
    while (written < size) {
        nw = ::write(fd,
                     buf + written,
                     size - written);
        if (nw < 0) {
            MY_LOGE("failed to write to file [%s]: %s", fname, ::strerror(errno));
            break;
        }
        written += nw;
        cnt++;
    }
    MY_LOGD("done writing %d bytes to file [%s] in %d passes", size, fname, cnt);
    ::close(fd);
    return true;
}


/******************************************************************************
 * @brief load the file to the buffer
 *
 * @details
 *
 * @note
 *
 * @param[in] fname: The file name
 * @param[out] buf: The output buffer
 * @param[in] capacity: The buffer capacity in bytes
 *
 * @return
 * -   The loaded size in bytes.
 ******************************************************************************/
unsigned int
loadFileToBuf(
    char const*const    fname,
    unsigned char*const buf,
    unsigned int const  capacity
)
{
    int nr, cnt = 0;
    uint32_t readCnt = 0;
    unsigned int size = capacity;

    MY_LOGD("opening file [%s]", fname);
    int fd = ::open(fname, O_RDONLY);
    if (fd < 0) {
        MY_LOGE("failed to create file [%s]: %s", fname, strerror(errno));
        return readCnt;
    }
    //
    if (size == 0) {
        int temp = ::lseek(fd, 0, SEEK_END);
        if(temp>=0)
        {
            size = (unsigned int)temp;
        }
        ::lseek(fd, 0, SEEK_SET);
    }
    //
    MY_LOGD("read %d bytes from file [%s]", size, fname);
    while (readCnt < size) {
        nr = ::read(fd,
                    buf + readCnt,
                    size - readCnt);
        if (nr < 0) {
            MY_LOGE("failed to read from file [%s]: %s",
                        fname, strerror(errno));
            break;
        }
        readCnt += nr;
        cnt++;
    }
    MY_LOGD("done reading %d bytes to file [%s] in %d passes", size, fname, cnt);
    ::close(fd);

    return readCnt;
}


/******************************************************************************
 * set the thread policy & priority
 ******************************************************************************/
bool
setThreadPriority(int policy, int priority)
{
    struct sched_param sched_p;
    //
    ::sched_getparam(0, &sched_p);
    if (policy == SCHED_OTHER) {
        sched_p.sched_priority = 0;
        ::sched_setscheduler(0, policy, &sched_p);
        ::setpriority(PRIO_PROCESS, 0, priority);   //  Note: "priority" is nice value.
        MY_LOGV("tid(%d) policy(SCHED_OTHER:%d) priority(%d)", ::gettid(), policy, priority);
    } else {
        sched_p.sched_priority = priority;          //  Note: "priority" is real-time priority.
        ::sched_setscheduler(0, policy, &sched_p);
        MY_LOGV("tid(%d) policy(Real-Time:%d) priority(%d)", ::gettid(), policy, priority);
    }

    return true;
}


/******************************************************************************
 * get the thread policy & priority
 ******************************************************************************/
bool
getThreadPriority(int& policy, int& priority)
{
    policy = ::sched_getscheduler(0);
    if (policy == SCHED_OTHER) {
        // a conventional process has only the static priority
        priority = ::getpriority(PRIO_PROCESS, 0);
        MY_LOGV("tid(%d) policy(SCHED_OTHER:%d) priority(%d)", ::gettid(), policy, priority);
    } else {
        // a real-time process has both the static priority and real-time priority.
        struct sched_param sched_p;
        ::sched_getparam(0, &sched_p);
        priority = sched_p.sched_priority;
        MY_LOGV("tid(%d) policy(Real-Time:%d) priority(%d)", ::gettid(), policy, priority);
    }

    return true;
}


/******************************************************************************
 * lock w/o throwing exception
 ******************************************************************************/
template<class T>
bool lockNoexcept(std::unique_lock<T>& lk) noexcept
{
    try { lk.lock(); }
    catch (std::exception&) {
        MY_LOGE("exception thrown during invoke unique_lock<MUTEX_T>::lock");
        assert(0);
        return false;
    }
    return true;
}

template<>
bool lockNoexcept<std::mutex>(
        std::unique_lock<std::mutex>& lk
    ) noexcept
{
    return lockNoexcept<std::mutex>(lk);
}

template<>
bool lockNoexcept<android::Mutex>(
        std::unique_lock<android::Mutex>& lk
    ) noexcept
{
    return lockNoexcept<android::Mutex>(lk);
}


/******************************************************************************
 *
 ******************************************************************************/
};  // namespace Utils
};  // namespace NSCam

static int32_t determinePersistLogLevel()
{
    int32_t level = ::property_get_int32("persist.vendor.mtk.camera.log_level", -1);
    CAM_ULOGMD("###### get camera log property =%d", level);
    if  (-1 == level) {
        level = MTKCAM_LOG_LEVEL_DEFAULT;
    }
    return level;
}

__BEGIN_DECLS
static int32_t gLogLevel = determinePersistLogLevel();
int mtkcam_testLog(char const* /*tag*/, int prio)
{
    switch (prio)
    {
        case 'V':       return (gLogLevel>=4);
        case 'D':       return (gLogLevel>=3);
        case 'I':       return (gLogLevel>=2);
        case 'W':       return (gLogLevel>=1);
        case 'E':       return (1);
        default:        break;
    }
    return 0;
}

void setLogLevelToEngLoad(bool is_camera_on_off_timing, bool set_log_level_to_eng, int logCount)
{
#if (((1 == MTKCAM_USER_DEBUG_LOAD) && (0 == MTKCAM_USER_DBG_LOG_OFF)) || (1 == MTKCAM_ENG_LOAD))
    char value[16];
    if(is_camera_on_off_timing)
    {
        int32_t mtk_internal = ::property_get_int32("ro.vendor.mtklog_internal", -1);
        if(set_log_level_to_eng)
        {
            if(logCount == -1)
            {
                sprintf(value,"%d",MTKCAM_ANDROID_LOG_MUCH_COUNT);
            }
            else
            {
                sprintf(value,"%d",logCount);
            }
            if(mtk_internal == 1)
            {
                property_set("vendor.logmuch.value",value);
                MY_LOGI("###### set log level to %s",value);
            }
            else
            {
                MY_LOGI("[enter camera]not mtk_internal_project (%d), no need to change log level",
                    mtk_internal);
            }
        }
        else
        {
            if(mtk_internal == 1)
            {
                property_set("vendor.logmuch.value","0");
                MY_LOGI("###### set log level to default");
            }
            else
            {
                MY_LOGI("[exit camera]not mtk_internal_project (%d), no need to change log level",
                    mtk_internal);
            }
        }
    }
#endif
}

__END_DECLS

