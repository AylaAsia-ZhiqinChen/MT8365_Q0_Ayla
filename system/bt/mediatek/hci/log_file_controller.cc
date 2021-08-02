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

#define LOG_TAG "bt_log_file_controller"

#include "log_file_controller.h"

#include <errno.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <functional>
#include <set>
#include <thread>

#include <base/bind.h>
#include <base/files/file.h>
#include <base/files/file_util.h>
#include <base/logging.h>
#include <base/message_loop/message_loop.h>
#include <base/run_loop.h>
#include <base/sequenced_task_runner.h>

#include "log_tree_mgr.h"
#include "log_writer.h"
#include "logs_stats.h"
#include "osi/include/log.h"
#include "osi/include/thread.h"
#include "snoop_log_config.h"
#include "mtk_util.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

class LogFileBackupHelper {
  public:
   LogFileBackupHelper()
       : last_logging_file_duplicated_count_(0) {}
   ~LogFileBackupHelper() = default;

   bool StartNewOne(const std::string& file, LogWriter* log_writer) {
     bool is_logging = log_writer->Open(file);
     if (is_logging) {
       LOG_DEBUG(LOG_TAG, "%s: begin to logging %s",
           __func__, file.c_str());
       log_writer->AppendHeader();
     }
     return is_logging;
   }

   bool Append(const std::string& file, LogWriter* log_writer) {
     bool is_ready = log_writer->AppendOpen(file);
     if (is_ready) {
       LOG_DEBUG(LOG_TAG, "%s: be ready to append logging %s",
           __func__, file.c_str());
     }
     return is_ready;
   }

   bool Backup(const std::string& file, LogWriter* log_writer) {
     CHECK(log_writer);
     log_writer->Close();
     return Rename(file);
   }

   void UpdateLastLoggingFile(const std::string& file) {
     if (!IsDuplicatedWithLastLoggingFile(file)) {
       last_logging_file_duplicated_count_ = 0;
       base::FilePath logging_file_path(file);
       base::FilePath to_path = logging_file_path.RemoveFinalExtension();
       last_logging_file_ = to_path;
     }
   }

   bool IsDuplicatedWithLastLoggingFile(const std::string& file) const {
     base::FilePath file_path = RemoveExtension(file);
     base::FilePath last_path = RemoveExtension(last_logging_file_.value());
     return (last_path == file_path);
   }

   void AddRetryRenameFile(const std::string& file) {
     renaming_files_.insert(file);
   }

   bool IsLeftFileNeedToRename() const {
     return !renaming_files_.empty();
   }

   void RetryRename(LogFileTreeManager* files_mgr) {
     for (const auto& it : renaming_files_) {
       if (Rename(it)) {
         renaming_files_.erase(it);
         UpdateLastLoggingFile(it);
         // files_mgr->AppendNewLogFileRecord(last_logging_file().value());
       }
     }
   }

   std::string GenerateNewFileName(const std::string& file,
       const std::string& suffix) {
     base::FilePath to_path = RemoveExtension(file);
     std::string padding =
         std::to_string(++last_logging_file_duplicated_count_);
     return (to_path.AddExtension(padding).AddExtension(suffix)).value();
   }

   bool Rename(const std::string& file) const {
     base::FilePath logging_file_path(file);
     if (!base::PathExists(logging_file_path)) {
       LOG_WARN(LOG_TAG, "%s %s NOT exist!", __func__, file.c_str());
       return false;
     }
     base::FilePath to_path = logging_file_path.RemoveFinalExtension();
     // Do rename just after the file being created may cause rename hang
     // which causes thread (join) quit timeout exception
     // if (base::Move(logging_file_path, to_path)) {
     LOG_INFO(LOG_TAG, "%s try from %s to %s", __func__,
         logging_file_path.value().c_str(), to_path.value().c_str());
     if (!rename(logging_file_path.value().c_str(),
           to_path.value().c_str())) {
       LOG_WARN(LOG_TAG, "%s from %s to %s", __func__,
           logging_file_path.value().c_str(), to_path.value().c_str());
       return true;
     } else {
       LOG_ERROR(LOG_TAG, "%s failed to rename %s to %s, error(%s)", __func__,
           logging_file_path.value().c_str(), to_path.value().c_str(),
           strerror(errno));
       return (errno == ENOENT) ? true : false;
     }
   }

   base::FilePath last_logging_file() const {
     return last_logging_file_;
   }

  private:
   inline base::FilePath RemoveExtension(const std::string& file) const {
     base::FilePath file_path(file);
     base::FilePath to_path = file_path.RemoveFinalExtension();
     to_path = to_path.RemoveExtension();
     return to_path;
   }

   int last_logging_file_duplicated_count_;
   base::FilePath last_logging_file_;
   std::set<std::string> renaming_files_;
};

LogFileKeeper::LogFileKeeper(LogFileTreeManager* file_tree_mgr)
    : file_tree_mgr_(file_tree_mgr),
      log_writer_(LogWriter::CreateInstance(kBtHci)),
      log_backup_helper_(new LogFileBackupHelper()) {}

LogFileKeeper::~LogFileKeeper() {
  log_writer_.reset();
  log_backup_helper_.reset();
}

bool LogFileKeeper::MakeDir(const base::FilePath &dir) {
  if (base::PathExists(dir)) {
    LOG_INFO(LOG_TAG, "%s %s already exist.", __func__, dir.value().c_str());
    return true;
  } else {
    bool is_done(false);
    const uint8_t RETRY_TIMES(7);
    uint8_t retry(RETRY_TIMES);
    while (retry) {
      if (_MakeDir(dir.value().c_str(), FILESYS_FOLDER_PERMISSION, FILESYS_USER, FILESYS_GROUP)) {
        LOG_ERROR(LOG_TAG, "%s mkdir for %s, error! %s",
                  __func__, dir.value().c_str(), strerror(errno));
      } else {
        is_done = true;
        break;
      }
      retry--;
      sleep(1);
    }
    return is_done;
  }
}

bool LogFileKeeper::StartLogging(const base::FilePath& dir) {
  std::string logging_file = file_tree_mgr_->MakeLoggingFileName(dir.value());
  bool is_logging =
      log_backup_helper_->StartNewOne(logging_file, log_writer_.get());
  if (is_logging) {
    file_tree_mgr_->set_current_logging_file(logging_file);
  }
  return is_logging;
}

bool LogFileKeeper::AppendLogging(const base::FilePath& file_path) {
  if (!base::PathExists(file_path) || (nullptr == log_writer_)) {
    return false;
  }
  bool is_logging =
      log_backup_helper_->Append(file_path.value(), log_writer_.get());
  if (is_logging) {
    file_tree_mgr_->set_current_logging_file(file_path.value());
  }
  return is_logging;
}

ssize_t LogFileKeeper::LogData(void *parcel, size_t length) const {
  CHECK(parcel);
  return log_writer_->Write(parcel, length);
}

void LogFileKeeper::BackupLogFile(LogFileTreeManager* files_mgr) {
  if (log_backup_helper_->IsLeftFileNeedToRename()) {
    log_backup_helper_->RetryRename(files_mgr);
  }

  if (log_backup_helper_->Backup(files_mgr->current_logging_file(),
      log_writer_.get())) {
    log_backup_helper_->UpdateLastLoggingFile(
        files_mgr->current_logging_file());
    // files_mgr->AppendNewLogFileRecord(
    //    log_backup_helper_->last_logging_file().value());
  } else {
    log_backup_helper_->AddRetryRenameFile(files_mgr->current_logging_file());
  }
  std::string new_file = files_mgr->MakeLoggingFileName(
      files_mgr->current_logging_dir().value());
  if (log_backup_helper_->IsDuplicatedWithLastLoggingFile(new_file)) {
    new_file = log_backup_helper_->GenerateNewFileName(new_file,
        files_mgr->GetCurrentLoggingFileSuffix());
    LOG_INFO(LOG_TAG, "%s gen new one is %s", __func__, new_file.c_str());
  }
  if (log_backup_helper_->StartNewOne(new_file, log_writer_.get())) {
    files_mgr->set_current_logging_file(new_file);
  }
}

void LogFileKeeper::StopLogging() {
  LOG_INFO(LOG_TAG, "%s", __func__);
  log_writer_->Close();
  log_backup_helper_->Rename(file_tree_mgr_->current_logging_file());
  log_backup_helper_->UpdateLastLoggingFile(
      file_tree_mgr_->current_logging_file());
  // file_tree_mgr_->AppendNewLogFileRecord(
  //    log_backup_helper_->last_logging_file().value());
}

void LogFileKeeper::Update(LogStatistics *stat) {
  CHECK(stat);
  if (stat->current_loged_file_size() >
      SnoopLogConfig::GetInstance()->GetMaxLogSizePerFile()) {
    LOG_INFO(LOG_TAG, "%s: current_loged_file_size_(%" PRId64 ") > "
        "MaxLogSizePerFile(%" PRId64 ").",
        __func__, stat->current_loged_file_size(),
        SnoopLogConfig::GetInstance()->GetMaxLogSizePerFile());
    BackupLogFile(file_tree_mgr_);
    stat->UpdateCurrentLogedFileSize(0, false);
  }
}

class LogFileCleanRunner {
  public:
    LogFileCleanRunner() = default;
    ~LogFileCleanRunner() = default;

    static int endsWith(std::string s,std::string sub);
    static void Run(LogFileClearner* clearner, LogStatistics *stat);
};

int LogFileCleanRunner::endsWith(std::string s,std::string sub){
    return s.rfind(sub)==(s.length()-sub.length())?1:0;
}

void LogFileCleanRunner::Run(LogFileClearner* clearner,
    LogStatistics *logstat) {
  CHECK(clearner);
  CHECK(logstat);
  // log file may changed by other program, refresh the actually total log size information here
  logstat->UpdateTotalLogedSize(clearner->file_tree_mgr_->GetTotalLoggedSize(), false);
  if(logstat->total_loged_size() < SnoopLogConfig::GetInstance()->GetMaxTotalLogsSize()){
    LOG_INFO(LOG_TAG, "%s: total_loged_size_(%" PRId64 ") < "
        "MaxTotalLogsSize(%" PRId64 "). Not need delete",
        __func__, logstat->total_loged_size(),
        SnoopLogConfig::GetInstance()->GetMaxTotalLogsSize());
    return;
  }

  int64_t bytes_cleaned(0);
  std::string oldest_dirpath =  clearner->file_tree_mgr_->GetOldestLogDirRecord();
  std::string current_dirpath = (clearner->file_tree_mgr_->current_logging_dir()).value();
  if (0 == current_dirpath.compare(oldest_dirpath)) {
    LOG_INFO(LOG_TAG, "%s: Delete Dir equal current Dir: %s", __func__, current_dirpath.c_str());
    struct dirent *dirp;
    struct stat statbuf;
    memset ( &statbuf, 0, sizeof(struct stat));
    std::string delete_filepath;
    std::string scan_filename;
    std::string scan_filepath;
    time_t firstTime = std::numeric_limits<time_t>::max();
    off_t file_sizeb;
    DIR *dp = opendir(current_dirpath.c_str());

    if (NULL == dp){
      LOG_ERROR(LOG_TAG, "%s: Open dir failed", __func__);
      return;
    }
    while((dirp = readdir(dp)) != NULL) {
      // ignore . and ..
      if (strcmp(".",dirp->d_name) == 0 || strcmp("..",dirp->d_name) == 0 ) {
        continue;
      }
      scan_filename = dirp->d_name;
      scan_filepath = current_dirpath+'/'+scan_filename;
      // get file information
      if (-1 == lstat(scan_filepath.c_str(), &statbuf)){
        LOG_ERROR(LOG_TAG, "%s: stat file failed :%s", __func__, scan_filepath.c_str());
        continue;
      }
      if (firstTime > statbuf.st_mtime ) {
        LOG_DEBUG(LOG_TAG, "%s: Temp delete file name:%s size(%" PRId64 ")", __func__, scan_filepath.c_str(), statbuf.st_size);
        delete_filepath = scan_filepath;
        firstTime = statbuf.st_mtime;
        file_sizeb = statbuf.st_size;
      }
    }
    if (!delete_filepath.empty()) {
      // check if it deletes current file.
      if(endsWith(delete_filepath, ".curf")) {
        LOG_WARN(LOG_TAG, "%s: You will delete current file %s ", __func__, delete_filepath.c_str());
        closedir(dp);
        return;
      }
      if (base::DeleteFile(base::FilePath(delete_filepath), true)) {
        LOG_WARN(LOG_TAG, "%s: path %s size(%" PRId64 ") is deleted.", __func__,
            delete_filepath.c_str(), int64_t(file_sizeb));
        bytes_cleaned += int64_t(file_sizeb);
      }
      int64_t size = logstat->total_loged_size()-bytes_cleaned;
      logstat->UpdateTotalLogedSize(size > 0 ? size : 0, false);
      LOG_INFO(LOG_TAG, "%s: total_loged_size_(%" PRId64 ") "
          "MaxTotalLogsSize(%" PRId64 ").",
          __func__, logstat->total_loged_size(),
          SnoopLogConfig::GetInstance()->GetMaxTotalLogsSize());
      closedir(dp);
    }
    else {
      LOG_ERROR(LOG_TAG, "%s: No file in the dir:%s", __func__, current_dirpath.c_str());
      closedir(dp);
      return;
    }
  }
  else {
    while (0 != current_dirpath.compare(oldest_dirpath)) {
      base::FilePath file_path(clearner->file_tree_mgr_->GetOldestLogDirRecord());
      if (base::PathExists(file_path)) {
        int64_t file_size = base::ComputeDirectorySize(file_path);
        if (base::DeleteFile(file_path, true)) {
          LOG_WARN(LOG_TAG, "%s: path %s size(%" PRId64 ") is deleted.", __func__,
              file_path.value().c_str(), file_size);
          bytes_cleaned += file_size;
        }
        clearner->file_tree_mgr_->ReleaseOldestLogDirRecord();
        oldest_dirpath =  clearner->file_tree_mgr_->GetOldestLogDirRecord();
        if (bytes_cleaned >= clearner->bytes_to_be_cleaned_) {
          int64_t size = logstat->total_loged_size()-bytes_cleaned;
          logstat->UpdateTotalLogedSize(size > 0 ? size : 0, false);
          break;
        }
        // To avoid block others too much time when delete huge files
        std::this_thread::yield();
      } else
        clearner->file_tree_mgr_->ReleaseOldestLogDirRecord();
    }
  }
}

// It is shamed base::Thread with adaption issue with native exception
// Therefore, have to be back to use osi/thread
class RecycleThreadImpl {
  public:
    RecycleThreadImpl()
        : thread_(nullptr),
          message_loop_(nullptr),
          run_loop_(nullptr) {
      thread_ = thread_new("bt_log_cleaner");
      if (!thread_) {
        LOG_ERROR(LOG_TAG, "%s unable to create thread.", __func__);
      } else {
        if (!thread_set_priority(thread_, 0)) {
          LOG_ERROR(LOG_TAG, "%s unable to make thread priority.", __func__);
        }
        thread_post(thread_, RunMessageLoop, this);
      }
    }
    ~RecycleThreadImpl() {
      if (message_loop_ != nullptr) {
        message_loop_->task_runner()->PostTask(
            FROM_HERE, run_loop_->QuitClosure());
      }

      if (thread_) {
        thread_stop(thread_);
        thread_join(thread_);
        thread_free(thread_);
        thread_ = nullptr;
      }
    }

    void PostTask(LogFileClearner* clearner,
        LogStatistics *stat) {
      if (message_loop_ != nullptr) {
        message_loop_->task_runner()->PostTask(FROM_HERE,
            base::Bind(&LogFileCleanRunner::Run, clearner, stat));
      }
    }

  private:
    static void RunMessageLoop(void* context) {
      RecycleThreadImpl* impl = static_cast<RecycleThreadImpl*>(context);
      {
        impl->message_loop_ = new base::MessageLoop();
        impl->run_loop_ = new base::RunLoop();
      }

      impl->run_loop_->Run();

      {
        delete impl->message_loop_;
        impl->message_loop_ = nullptr;
        delete impl->run_loop_;
        impl->run_loop_ = nullptr;
      }
    }

    thread_t* thread_;
    base::MessageLoop* message_loop_;
    base::RunLoop* run_loop_;
};

LogFileClearner::LogFileClearner(LogFileTreeManager* file_tree_mgr,
       int64_t bytes_to_be_cleaned)
      : file_tree_mgr_(file_tree_mgr),
        bytes_to_be_cleaned_(bytes_to_be_cleaned),
        thread_(new RecycleThreadImpl()),
        cleaner_trigger_cnt_(0) {}

LogFileClearner::~LogFileClearner() {}

void LogFileClearner::Recycle(LogStatistics *stat) {
  if (thread_ && !file_tree_mgr_->IsLogDirRecordEmpty()) {
    thread_->PostTask(this, stat);
  }
}

void LogFileClearner::Update(LogStatistics *stat) {
  CHECK(stat);
  // prevent try clean for each packet
  if (cleaner_trigger_cnt_ > 1000) {
    cleaner_trigger_cnt_ = 0;
    if (stat->total_loged_size() >
        SnoopLogConfig::GetInstance()->GetMaxTotalLogsSize()) {
      LOG_INFO(LOG_TAG, "%s: total_loged_size_(%" PRId64 ") > "
        "MaxTotalLogsSize(%" PRId64 ").",
        __func__, stat->total_loged_size(),
        SnoopLogConfig::GetInstance()->GetMaxTotalLogsSize());
    Recycle(stat);
  }
  } else {
    cleaner_trigger_cnt_++;
  }
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
