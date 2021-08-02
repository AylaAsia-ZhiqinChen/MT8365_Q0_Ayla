/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#define LOG_TAG "bt_log_writer"

#include "log_writer.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/uio.h>
#include <memory>

#include <base/logging.h>

#include "mtk_util.h"
#include "osi/include/log.h"
#include "snoop_packetizer.h"

// To disable by default
#define DBG_LOG_ENABLE FALSE

#if DBG_LOG_ENABLE == TRUE
#define DBG_LOG(tag, fmt, args...) LOG_DEBUG(LOG_TAG, fmt, ##args)
#else
#define DBG_LOG(tag, fmt, args...) ((void)0)
#endif

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

std::unique_ptr<LogWriter> LogWriter::CreateInstance(WriterType type) {
   switch (type) {
   case kBtHci:
     return std::unique_ptr<LogWriter>(new BTHciSnoopLogWriter());
   case kBtFW: {
     CHECK(0);
     return nullptr;
   }
   default: {
     CHECK(0);
     return nullptr;
   }
   }
}

bool LogWriter::Open(const std::string &log_path) {
  mode_t prevmask = umask(0);
  logfile_fd_ = open(log_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, FILESYS_FILE_PERMISSION);
  if (logfile_fd_ == INVALID_FD) {
    LOG_ERROR(LOG_TAG, "%s unable to open '%s': %s", __func__,
              log_path.c_str(), strerror(errno));
    umask(prevmask);
    return false;
  }
  umask(prevmask);
  _SetPermission(log_path.c_str(), FILESYS_FILE_PERMISSION, FILESYS_USER, FILESYS_GROUP);
  return true;
}

bool LogWriter::AppendOpen(const std::string &log_path) {
  logfile_fd_ = open(log_path.c_str(), O_WRONLY | O_APPEND);
  if (logfile_fd_ == INVALID_FD) {
    LOG_ERROR(LOG_TAG, "%s unable to append open '%s': %s", __func__,
              log_path.c_str(), strerror(errno));
    return false;
  }
  _SetPermission(log_path.c_str(), FILESYS_FILE_PERMISSION, FILESYS_USER, FILESYS_GROUP);
  return true;
}

void LogWriter::Close() {
  LOG_INFO(LOG_TAG, "%s", __func__);
  if (logfile_fd_ != INVALID_FD) {
    close(logfile_fd_);
    logfile_fd_ = INVALID_FD;
  }
}

ssize_t BTHciSnoopLogWriter::Write(void *data, size_t length) const {
  if ((nullptr == data) || (0 == length) || (INVALID_FD == logfile_fd_)) {
    // This is a error handling to avoid crash BT due to log mechanism
    LOG_ERROR(LOG_TAG, "%s invalid : data: %p, length: %zu, logfile_fd_: %d",
        __func__, data, length, logfile_fd_);
    return -1;
  }

  BTSnoopParcel* parcel = static_cast<BTSnoopParcel*>(data);
  std::unique_ptr<iovec[]> iov(new iovec[length*2]);
  for (size_t i(0); i < length*2; i++) {
    if (!(i%2)) {
      iov[i].iov_base = (void*)&(parcel->header());
      iov[i].iov_len = sizeof(parcel->header());
      DBG_LOG(LOG_TAG, "%s header: "
          "len_ori: %x, len_captured: %x, flags: %x, type: %x",
          __func__,
          parcel->header().length_original,
          parcel->header().length_captured,
          parcel->header().flags, parcel->header().type);
    } else {
      iov[i].iov_base = (void*)parcel->packet().data();
      iov[i].iov_len = parcel->packet().size();
      DBG_LOG(LOG_TAG, "%s packet: %s", __func__,
              DataArrayToString<uint8_t>(parcel->packet().data(),
                  parcel->packet().size()).c_str());
      parcel++;
    }
  }
  ssize_t ret = TEMP_FAILURE_RETRY(writev(logfile_fd_, iov.get(), length*2));
  if (-1 == ret) {
    LOG_ERROR(LOG_TAG, "%s error write due to: %s",
        __func__, strerror(errno));
  }
  return ret;
}

void BTHciSnoopLogWriter::AppendHeader() const {
  ssize_t ret = TEMP_FAILURE_RETRY(write(logfile_fd_,
      kBTSnoopFileHeader_.c_str(), kBTSnoopFileHeader_.size()));
  if (-1 == ret) {
    LOG_ERROR(LOG_TAG, "%s error write due to: %s",
        __func__, strerror(errno));
  }
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
