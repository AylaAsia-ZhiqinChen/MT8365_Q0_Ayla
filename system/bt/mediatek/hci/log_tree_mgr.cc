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

#define LOG_TAG "bt_log_tree_mgr"

#include "log_tree_mgr.h"

#include <inttypes.h>
#include <stdint.h>
#include <unistd.h>

#include <algorithm>
#include <vector>

#include <base/files/file.h>
#include <base/files/file_enumerator.h>
#include <base/files/file_util.h>
#include <base/strings/string_tokenizer.h>
#include <base/logging.h>

#include "log_time.h"
#include "logs_stats.h"
#include "mtk_util.h"
#include "osi/include/log.h"
#include "osi/include/properties.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

constexpr char kFileTree[] = "/file_tree.txt";

// Refine Init log tree flow, add a new function to handle this case
void LogFileTreeManager::InitLogsFileTree(const base::FilePath& current_dir) {
  // return when mtklog not start
  if (current_dir == base::FilePath(kDefaultBtsnoopPath)){
    LOG_INFO(LOG_TAG, "%s: ignore %s", __func__, current_dir.value().c_str());
    return;
  }
  if (!base::IsDirectoryEmpty(logging_base_dir_)){
    // scan all folder under basedir
    LOG_INFO(LOG_TAG, "%s: scan dir %s", __func__, logging_base_dir_.value().c_str());
    base::FileEnumerator file_enum(logging_base_dir_,false, base::FileEnumerator::DIRECTORIES);
    for (base::FilePath name = file_enum.Next(); !name.empty(); name = file_enum.Next()) {
       logs_tree_.push_back(name.value());
    }
    std::sort(logs_tree_.begin(), logs_tree_.end());

    // create file_tree.txt
    if(access(file_tree_path_.value().c_str(), F_OK ) == 0) {
      if(remove(file_tree_path_.value().c_str()) != 0) {
          LOG_ERROR(LOG_TAG, "%s unable to delete, path[%s], errno = %d(%s)",
            __func__, file_tree_path_.value().c_str(), errno, strerror(errno));
      }
    }
    int tree_fd;
    tree_fd = open(file_tree_path_.value().c_str(),
        O_WRONLY | O_CREAT | O_TRUNC, FILESYS_FILE_PERMISSION);
    if (tree_fd == -1) {
      LOG_ERROR(LOG_TAG, "%s unable to open, path[%s], errno = %d(%s)",
        __func__, file_tree_path_.value().c_str(), errno, strerror(errno));
      return;
    }
    // write file tree to file_tree.txt
    for (unsigned long i = 0; i < logs_tree_.size(); i++){
      std::string tree_path_record = logs_tree_[i] +  std::string("\r");
      write(tree_fd, tree_path_record.c_str(), tree_path_record.size());
    }
    close(tree_fd);
    // set permission
    _SetPermission(file_tree_path_.value().c_str(), FILESYS_FILE_PERMISSION, FILESYS_USER, FILESYS_GROUP);
  } else {
    LOG_ERROR(LOG_TAG, "%s: basedir :%s empty",__func__,logging_base_dir_.value().c_str());
    return;
  }
  Dump();
  LOG_INFO(LOG_TAG, "%s: file load done", __func__);
}

LogFileTreeManager::LogFileTreeManager(const std::string& log_base_dir,
    const std::string& log_file_prefix,
    const std::string& log_file_suffix,
    const std::string& log_file_current_suffix)
    : logging_base_dir_(log_base_dir),
      file_tree_path_(log_base_dir+std::string(kFileTree)),
      kLogFilePrefix_(log_file_prefix),
      kLogFileSuffix_(log_file_suffix),
      kLogFileCurrentSuffix_(log_file_current_suffix) {

}

LogFileTreeManager::~LogFileTreeManager() {
  // RefreshLogsRecordsTree();
}

std::string LogFileTreeManager::MakeLoggingFileName(const std::string &dir) const {
  std::string log_path = dir
      + std::string("/") + kLogFilePrefix_
      + LogTime::GetInstance()->GetLogTimeTag()
      + kLogFileSuffix_
      + kLogFileCurrentSuffix_;
  return log_path;
}

std::string LogFileTreeManager::GetCurrentLoggingFileSuffix() const {
  return kLogFileSuffix_ + kLogFileCurrentSuffix_;
}

int64_t LogFileTreeManager::GetTotalLoggedSize() const {
  if (!base::PathExists(logging_base_dir_)) {
    return 0;
  } else {
    if (base::IsDirectoryEmpty(logging_base_dir_)) {
      return 0;
    } else {
      //Anyway, it's not a good idea to recording the total size!
      //
      // ComputeDirectorySize() may time consuming,
      // even to post it in a separate thread,
      // still block too much for instant BT on/off stress test, so bad :(
      // Put it as a backup solution
      //
      std::chrono::time_point<std::chrono::steady_clock> ts = std::chrono::steady_clock::now();
      int64_t ret = base::ComputeDirectorySize(logging_base_dir_);
      int time_delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ts).count();
      LOG_INFO(LOG_TAG, "%s: Compute:%s cost %dms", __func__, logging_base_dir_.value().c_str(), time_delta);
      return ret;
    }
  }
}

void LogFileTreeManager::Dump() const {
  // dump part of the tree
  #define DUMP_SIZE 20
  LOG_INFO(LOG_TAG, "%s: [0] %s", __func__, logs_tree_.front().c_str());
  int last = static_cast<int>(logs_tree_.size());
  int begin = (last > DUMP_SIZE ? last - DUMP_SIZE : 0);
  for (int i = begin; i < last; i++){
    LOG_INFO(LOG_TAG, "%s: [%d] %s", __func__, i, logs_tree_[i].c_str());
  }
/*
  for (const auto& it : logs_records_) {
    LOG_INFO(LOG_TAG, "%s: logs_records_: %s", __func__, it.c_str());
  }
*/
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
