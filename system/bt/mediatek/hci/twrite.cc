
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

#define LOG_TAG "bt_log_snoop_twrite"

#include "twrite.h"

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include <base/bind.h>
#include <base/logging.h>
#include <base/files/file.h>
#include <base/files/file_enumerator.h>
#include <base/files/file_util.h>
#include <base/memory/singleton.h>
#include <base/message_loop/message_loop.h>
#include <base/run_loop.h>
#include <base/sequenced_task_runner.h>
#include <base/strings/string_util.h>
#include <base/synchronization/waitable_event.h>
#include <base/threading/thread.h>
#include <chrono>
#include <utils/misc.h>

#include "bt_hci_bdroid.h"
#include "bt_log_controller.h"
#include "bt_log_tool.h"
#include "btsnoop_mem.h"
#include "hci_inbound_data_monitor.h"
#include "log_file_controller.h"
#include "log_mode.h"
#include "log_time.h"
#include "log_tree_mgr.h"
#include "logs_stats.h"
#include "module.h"
#include "mtk_stack_config.h"
#include "mtk_util.h"
#include "snoop_log_config.h"
#include "osi/include/alarm.h"
#include "osi/include/log.h"
#include "osi/include/osi.h"
#include "osi/include/properties.h"
#include "osi/include/thread.h"
#include "snoop_packetizer.h"
#include "stack_config.h"

// To disable by default
#define DBG_LOG_ENABLE FALSE

#if DBG_LOG_ENABLE == TRUE
#define DBG_LOG(tag, fmt, args...) LOG_DEBUG(LOG_TAG, fmt, ##args)
#else
#define DBG_LOG(tag, fmt, args...) ((void)0)
#endif

#if defined(BT_NET_DEBUG) && (BT_NET_DEBUG == TRUE)
void btsnoop_net_open();
void btsnoop_net_close();
void btsnoop_net_write(const void* data, size_t length);
#endif

// Module life cycle functions
static future_t* start_up(void) {
  vendor::mediatek::bluetooth::stack::BTSnoop::GetInstance()->StartUp();
  return NULL;
}

static future_t* shut_down(void) {
  vendor::mediatek::bluetooth::stack::BTSnoop::GetInstance()->Stop();
  return NULL;
}

EXPORT_SYMBOL extern const module_t mtk_btsnoop_module = {
    .name = MTK_BTSNOOP_MODULE,
    .init = NULL,
    .start_up = start_up,
    .shut_down = shut_down,
    .clean_up = NULL,
    .dependencies = {STACK_CONFIG_MODULE, NULL}};

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

const std::string kMTKLogDefaultRootPathProperty("/sdcard");
const std::string kDefaultBTSnoopRelativePath(
    "/debuglogger/connsyslog/bthci");
const std::string kBTSnoopLoggingProperty("vendor.bthcisnoop.running");

constexpr char kLogDirPrefix[] = "CsLog_";
constexpr char kLogFilePrefix[] = "BT_HCI_";
constexpr char kLogFileCurrentSuffix[] = ".curf";
constexpr char kLogFileSuffix[] = ".cfa";

std::string GetBTSnoopLogPath(const std::string& root_path) {
  return (std::string(root_path) + kDefaultBTSnoopRelativePath);
}

std::string GetBTSnoopLogInitPath() {
  // AOSP way
  if (IsPropertySet(kBluetoothSnoopRootPath)) {
    return GetPropertyValue(kBluetoothSnoopRootPath);
  } else {
    return std::string(kDefaultBtsnoopPath);
  }
}

class BTSnoopBootBackupHelper {
  public:
   BTSnoopBootBackupHelper() : is_backup_in_flight_(false) {}
   ~BTSnoopBootBackupHelper() {
     // Add mutex protection for avoid race condition
     // between data Flush and shutdown
     std::lock_guard<std::mutex> lock(data_wrapper_mutex_);
   }

   bool is_backup_in_flight() const {
     return is_backup_in_flight_;
   }

   void set_is_backup_in_flight(bool is_backup_in_flight) {
     is_backup_in_flight_ = is_backup_in_flight;
   }

   std::mutex& GetDataWrapperMutex() {
     return data_wrapper_mutex_;
   }

   void Capture(const BT_HDR* buffer, bool is_received) {
     uint64_t timestamp_us = LogTime::GetInstance()->GetEpochTime();
     btsnoop_mem_capture(buffer, timestamp_us);
     BTSnoopPacketWrapper packet(buffer, is_received, timestamp_us);
     DBG_LOG(LOG_TAG,
         "%s: is_received: %d, buffer: event: 0x%x, len: %u, data: %s",
          __func__, is_received, buffer->event, buffer->len,
          DataArrayToString<uint8_t>(buffer->data, buffer->len).c_str());
     DBG_LOG(LOG_TAG,
         "%s: wrapper: is_received: %d, buffer: event: 0x%x, len: %u, data: %s",
          __func__,
          packet.is_received(), packet.buffer()->event, packet.buffer()->len,
          DataArrayToString<uint8_t>(
              packet.buffer()->data,
              packet.buffer()->len).c_str());
     data_wrapper_.push(packet);
   }

   void Flush(const LogFileKeeper* log_file_keeper,
       SnoopPacketizer* packetizer) {
     std::vector<BTSnoopParcel> parcels;
     if (packetizer) {
       std::lock_guard<std::mutex> lock(data_wrapper_mutex_);
       while (data_wrapper_.size() > 0) {
         DBG_LOG(LOG_TAG, "%s: size %zu", __func__, data_wrapper_.size());
         parcels.push_back(
             packetizer->ConvertToParcel(data_wrapper_.front()));
         data_wrapper_.pop();
       }
     }
     if (log_file_keeper && (!parcels.empty())) {
       log_file_keeper->LogData(parcels.data(), parcels.size());
       parcels.clear();
     }
   }

   // Return the last back up one
   std::string Backup(const base::FilePath& to_path) {
     base::FilePath from_path(GetBTSnoopLogInitPath());
     LOG_DEBUG(LOG_TAG, "%s try backup from %s to %s",
         __func__, from_path.value().c_str(), to_path.value().c_str());
     if (!from_path.empty() && !to_path.empty()) {
       base::FilePath log_file(BackupBootLogs(from_path, to_path));
       if (base::PathExists(log_file)) {
         return RenameAsCurrentLoggingFile(log_file.value());
       } else {
         LOG_ERROR(LOG_TAG, "%s invalid %s for backup",
             __func__, log_file.value().c_str());
         return std::string();
       }
     } else {
       LOG_ERROR(LOG_TAG, "%s invalid %s for backup",
           __func__, to_path.value().c_str());
       return std::string();
     }
   }

   void RemoveLastTimeInitLogs(const std::string& base_dir) {
     // Don't need to do capture sync here because it is guaranteed by
     // module start up life cycle controlled by stack_manager.
     // In another word, mtk_btsnoop will be started before hci for sure
     if(base_dir.empty()) {
       // there may be something wrong in StartUp if run into here
       LOG_ERROR(LOG_TAG, "%s empty base_dir!!", __func__);
       return;
     }
     LOG_INFO(LOG_TAG, "%s base_dir: %s", __func__, base_dir.c_str());
     base::FileEnumerator file_enum(base::FilePath(base_dir), false, base::FileEnumerator::FILES);
     for (base::FilePath name = file_enum.Next(); !name.empty(); name = file_enum.Next()) {
       if (base::StartsWith(GetFileName(name), kLogFilePrefix, base::CompareCase::SENSITIVE)) {
         if (base::DeleteFile(name, false)) {
           LOG_DEBUG(LOG_TAG, "%s %s is deleted", __func__, name.value().c_str());
         }
       }
     }
   }

  private:
   base::FilePath ConvertToNewFilePath(const base::FilePath& file_path,
       const base::FilePath& logging_dir) {
     std::vector<base::FilePath::StringType> comps;
     file_path.GetComponents(&comps);
     if (comps.size() > 0) {
       return logging_dir.Append(comps[comps.size()-1]);
     } else {
       LOG_ERROR(LOG_TAG, "%s invalid %s",
           __func__, file_path.value().c_str());
       return file_path;
     }
   }

   bool MoveBootLogs(const base::FilePath& file_path,
       const base::FilePath& new_path) {
     if (!base::PathExists(file_path)) {
       LOG_WARN(LOG_TAG, "%s %s NOT exist!",
           __func__, file_path.value().c_str());
       return false;
     }
     // Do rename just after the file being created may cause rename hang
     // which causes thread (join) quit timeout exception
     LOG_DEBUG(LOG_TAG, "%s try move from %s to %s", __func__,
         file_path.value().c_str(), new_path.value().c_str());
     if (_MoveFile(file_path.value().c_str(), new_path.value().c_str(),
        FILESYS_FILE_PERMISSION, FILESYS_USER, FILESYS_GROUP)) {
       LOG_WARN(LOG_TAG, "%s %s is moved to %s.",
           __func__, file_path.value().c_str(), new_path.value().c_str());
       return true;
     } else {
       return false;
     }
   }

   std::string BackupBootLogs(const base::FilePath& from_dir,
       const base::FilePath& to_dir) {
     CHECK(!from_dir.empty());
     CHECK(!to_dir.empty());
     std::string last_init_file;
     base::FileEnumerator file_enum(base::FilePath(from_dir),
         false, base::FileEnumerator::FILES);
     for (base::FilePath name = file_enum.Next();
         !name.empty(); name = file_enum.Next()) {
       if (base::StartsWith(GetFileName(name),
           kLogFilePrefix, base::CompareCase::SENSITIVE)) {
         base::FilePath new_path = ConvertToNewFilePath(name, to_dir);
         if (MoveBootLogs(name, new_path)) {
           last_init_file = new_path.value();
         }
       }
     }
     LOG_DEBUG(LOG_TAG, "%s: the last one is %s",
         __func__, last_init_file.c_str());
     return last_init_file;
   }

   std::string GetFileName(const base::FilePath& file_path) const {
     CHECK(!file_path.empty());
     std::vector<base::FilePath::StringType> comps;
     file_path.GetComponents(&comps);
     return comps[comps.size()-1];
   }

   std::string RenameAsCurrentLoggingFile(const std::string& file) {
     base::FilePath cur_path(file);
     if (std::string(kLogFileSuffix) == cur_path.FinalExtension()) {
       base::FilePath to_path(cur_path.AddExtension(
           std::string(kLogFileCurrentSuffix)));
       if (base::Move(cur_path, to_path)) {
         LOG_WARN(LOG_TAG, "%s %s is moved to %s.",
             __func__,
             cur_path.value().c_str(),
             to_path.value().c_str());
         return to_path.value();
       } else {
         return file;
       }
     } else {
       return file;
     }
   }

   std::queue<BTSnoopPacketWrapper> data_wrapper_;
   bool is_backup_in_flight_;
   std::mutex data_wrapper_mutex_;
};

using WriteDataCallback = std::function<void(ssize_t)>;

struct BtSnoopStartUpArg {
  public:
    std::string base_dir_;
    bool is_need_append_tag_dir_;
    BTSnoopBootBackupHelper* boot_log_backup_helper_;
};

struct BtSnoopStartUpHelperArg {
  public:
    explicit BtSnoopStartUpHelperArg(base::FilePath base_dir,
        BTSnoopBootBackupHelper* boot_log_backup_helper,
        void *context)
        : base_dir_(base_dir),
          boot_log_backup_helper_(boot_log_backup_helper),
          context_(context) {}

    base::FilePath base_dir_;
    BTSnoopBootBackupHelper* boot_log_backup_helper_;
    void *context_;
};

class BtSnoopSubject {
 public:
   BtSnoopSubject() = default;
   virtual ~BtSnoopSubject() = default;

   virtual bool IsLogging() const {
     return false;
   }
   virtual void StartUp(const BtSnoopStartUpArg& arg) = 0;
   virtual void Capture(const BT_HDR* buffer, bool is_received) = 0;
   virtual void Stop() = 0;
};

class BtSnoopHelper {
  public:
   explicit BtSnoopHelper(
       LogFileKeeper* log_keeper,
       LogStatistics* log_stats,
       LogFileTreeManager* log_tree_mgr)
       : log_keeper_(log_keeper),
         log_stats_(log_stats),
         log_tree_mgr_(log_tree_mgr),
         btsnoop_packetizer_(new BtSnoopPacketizer(
             [this](const std::vector<BTSnoopParcel>& parcels,
                 base::WaitableEvent* event) {
               OnParcelsReady(parcels, event);
             },
             [this](const BTSnoopParcel& parcel) {
               OnPacketReady(parcel);
             })),
         is_logging_(false),
         thread_(nullptr),
         message_loop_(nullptr),
         run_loop_(nullptr) {}
   ~BtSnoopHelper() = default;

   static void RunMessageLoop(void* context) {
     BtSnoopStartUpHelperArg* arg =
         static_cast<BtSnoopStartUpHelperArg*>(context);
     BtSnoopHelper* impl = static_cast<BtSnoopHelper*>(arg->context_);
     {
       std::lock_guard<std::mutex> lock(impl->message_loop_mutex_);
       impl->message_loop_ = new base::MessageLoop();
       impl->run_loop_ = new base::RunLoop();
     }

     impl->message_loop_->task_runner()->PostTask(
         FROM_HERE, base::Bind(&BtSnoopHelper::InitTask,
             base::Owned(arg)));
     impl->run_loop_->Run();

     {
       std::lock_guard<std::mutex> lock(impl->message_loop_mutex_);
       delete impl->message_loop_;
       impl->message_loop_ = nullptr;
       delete impl->run_loop_;
       impl->run_loop_ = nullptr;
     }
   }

   static void InitTask(BtSnoopStartUpHelperArg* arg) {
     CHECK(arg);
     BtSnoopHelper* impl = static_cast<BtSnoopHelper*>(arg->context_);
     impl->CheckReady();
     impl->BTSnoopInitialize(arg->base_dir_, arg->boot_log_backup_helper_);
   }

   static void WriteTask(const BtSnoopHelper* impl,
       std::vector<BTSnoopParcel> parcels,
       WriteDataCallback write_cb,
       base::WaitableEvent* event) {
     CHECK(impl);
     CHECK(write_cb);

     if (!impl->is_logging()) return;

     ssize_t ret = impl->log_keeper_->LogData(parcels.data(), parcels.size());
     write_cb(ret);
     if (event) {
       event->Signal();
     }
   }

   static void ShutdownTask(BtSnoopHelper* impl) {
     CHECK(impl);
#if defined(BT_NET_DEBUG) && (BT_NET_DEBUG == TRUE)
     btsnoop_net_close();
#endif
     impl->log_keeper_->StopLogging();
     impl->is_logging_ = false;
     impl->UpdateLoggingState(impl->is_logging_);
   }

   bool is_logging() const {
     return is_logging_;
   }

   void StartUp(BtSnoopStartUpHelperArg* arg) {
     thread_ = thread_new("bt_log_snoop_helper");
     if (!thread_) {
       LOG_ERROR(LOG_TAG, "%s unable to create thread.", __func__);
     } else {
       if (!thread_set_priority(thread_, 0)) {
         LOG_ERROR(LOG_TAG, "%s unable to make thread priority.", __func__);
       }
       thread_post(thread_, RunMessageLoop, arg);
     }
   }

   void Capture(const BT_HDR* buffer, bool is_received) {
     uint64_t timestamp_us = LogTime::GetInstance()->GetEpochTime();
     btsnoop_mem_capture(buffer, timestamp_us);

     if (!is_logging_) return;

     btsnoop_packetizer_->OnDataReady(buffer, is_received, timestamp_us);
   }

   void Stop() {
     // DO NOT post FlushData() into task thread, will dead lock!
     FlushData();
     LOG_DEBUG(LOG_TAG, "%s: BtSnoopHelper Stop", __func__);
     {
       std::lock_guard<std::mutex> lock(message_loop_mutex_);
       // Error handling for messge_loop_ is still NULL during thread creation
       // but stop event is coming too soon
       if (message_loop_ != nullptr) {
         message_loop_->task_runner()->PostTask(
             FROM_HERE, base::Bind(&BtSnoopHelper::ShutdownTask, this));
         message_loop_->task_runner()->PostTask(
             FROM_HERE, run_loop_->QuitClosure());
       }
     }

     // Stop the thread to prevent Send() calls.
     if (thread_) {
       thread_stop(thread_);
       thread_join(thread_);
       thread_free(thread_);
       thread_ = NULL;
     }
   }

  private:
   void CheckReady() {
     CHECK(btsnoop_packetizer_);
     CHECK(log_keeper_);
     CHECK(log_stats_);
     CHECK(log_tree_mgr_);

     CHECK(thread_);
     CHECK(message_loop_);
     CHECK(run_loop_);
   }

   void InitLoggedDataSize(LogStatistics* log_stats,
       LogFileTreeManager* log_tree_mgr) {
     log_stats->UpdateTotalLogedSize(log_tree_mgr->GetTotalLoggedSize(), true);
     log_stats->Dump();
   }

   void BTSnoopInitialize(const base::FilePath& dir,
       BTSnoopBootBackupHelper* boot_backup_helper) {
     if (log_keeper_->MakeDir(dir)) {
       if (boot_backup_helper) {
         std::string logging_file = boot_backup_helper->Backup(dir);
         if (!logging_file.empty()) {
           is_logging_ = log_keeper_->AppendLogging(
               base::FilePath(logging_file));
         }
         if (is_logging_) {
           // set the flag before pop data cache queue to avoid race condition
           {
             std::lock_guard<std::mutex> lock(
                 boot_backup_helper->GetDataWrapperMutex());
             boot_backup_helper->set_is_backup_in_flight(false);
           }
           boot_backup_helper->Flush(log_keeper_, btsnoop_packetizer_.get());
         } else {
           // error handling here
           std::lock_guard<std::mutex> lock(
               boot_backup_helper->GetDataWrapperMutex());
           boot_backup_helper->set_is_backup_in_flight(false);
         }
       } else {
         is_logging_ = log_keeper_->StartLogging(dir);
       }
       if (is_logging_) {
         log_tree_mgr_->InitLogsFileTree(dir);
         log_tree_mgr_->set_current_logging_dir(dir);
         InitLoggedDataSize(log_stats_, log_tree_mgr_);
         UpdateLoggingState(is_logging_);
#if defined(BT_NET_DEBUG) && (BT_NET_DEBUG == TRUE)
         btsnoop_net_open();
#endif
       }
     }
   }

   void UpdateLoggingState(bool is_logging) {
     if (osi_property_set(
         kBTSnoopLoggingProperty.c_str(),
         is_logging ? "1" : "0")) {
       LOG_ERROR(LOG_TAG, "%s unable to set %s to %s.",
           __func__,
           kBTSnoopLoggingProperty.c_str(),
           is_logging ? "1" : "0");
     }
   }

   void OnParcelsReady(const std::vector<BTSnoopParcel>& parcels,
       base::WaitableEvent* event) {
     base::Closure callback = base::Bind(&WriteTask,
         this,
         std::move(parcels),
         [this](ssize_t ret) {
           if (ret) {
             log_stats_->UpdateCurrentLogedFileSize(ret + log_stats_->current_loged_file_size(), true);
             log_stats_->UpdateTotalLogedSize(ret + log_stats_->total_loged_size(), true);
           }
         },
         event);
     std::lock_guard<std::mutex> lock(message_loop_mutex_);
     if (message_loop_ != nullptr) {
       message_loop_->task_runner()->PostTask(FROM_HERE, std::move(callback));
     }
   }

   void OnPacketReady(const BTSnoopParcel& parcel) {
#if defined(BT_NET_DEBUG) && (BT_NET_DEBUG == TRUE)
     btsnoop_net_write(&parcel.header(), sizeof(parcel.header()));
     btsnoop_net_write(parcel.packet().data(), parcel.packet().size());
#endif
   }

   void FlushData() {
     base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
         base::WaitableEvent::InitialState::NOT_SIGNALED);
     btsnoop_packetizer_->OnShutdown(&event);
     // Wait for the left data are flashed into storage
     event.Wait();
   }

   LogFileKeeper* log_keeper_;
   LogStatistics* log_stats_;
   LogFileTreeManager* log_tree_mgr_;
   std::unique_ptr<SnoopPacketizer> btsnoop_packetizer_;
   bool is_logging_;

   thread_t* thread_;
   base::MessageLoop* message_loop_;
   base::RunLoop* run_loop_;

   std::mutex message_loop_mutex_;
};

class BtSnoopImpl : public BtSnoopSubject {
 public:
  BtSnoopImpl()
      : is_started_(false),
        inbound_data_monitor_(nullptr),
        log_tree_mgr_(nullptr),
        log_keeper_(nullptr),
        log_cleaner_(nullptr),
        log_stats_(nullptr),
        bt_snoop_helper_(nullptr) {}
  ~BtSnoopImpl() = default;

  void StartUp(const BtSnoopStartUpArg& arg) override {
    if (is_started_) {
      LOG_DEBUG(LOG_TAG, "%s: BtSnoopImpl ignore dup StartUp", __func__);
      return;
    }
    std::lock_guard<std::mutex> lock(btsnoop_mutex_);
    LOG_DEBUG(LOG_TAG, "%s: BtSnoopImpl StartUp", __func__);
    Setup(arg.base_dir_);
    base::FilePath current_logging_dir(arg.is_need_append_tag_dir_ ?
        log_tree_mgr_->logging_base_dir().Append(
            std::string(kLogDirPrefix) +
            LogTime::GetInstance()->GetLogTimeTag()) :
        log_tree_mgr_->logging_base_dir());
    BtSnoopStartUpHelperArg* helper_arg(new BtSnoopStartUpHelperArg(
        current_logging_dir, arg.boot_log_backup_helper_, bt_snoop_helper_.get()));
    bt_snoop_helper_->StartUp(helper_arg);
    is_started_ = true;
  }

  void Capture(const BT_HDR* buffer, bool is_received) override {
    uint64_t timestamp_us = LogTime::GetInstance()->GetEpochTime();
    btsnoop_mem_capture(buffer, timestamp_us);

    std::lock_guard<std::mutex> lock(btsnoop_mutex_);
    if ((inbound_data_monitor_ != nullptr) && is_received) {
      inbound_data_monitor_->InboundDataUpdate(buffer);
    }

    if (bt_snoop_helper_ && bt_snoop_helper_->is_logging()) {
      bt_snoop_helper_->Capture(buffer, is_received);
    }
  }

  void Stop() override {
    if (!is_started_) {
      LOG_DEBUG(LOG_TAG, "%s: BtSnoopImpl ignore dup Stop", __func__);
      return;
    }
    std::lock_guard<std::mutex> lock(btsnoop_mutex_);
    LOG_DEBUG(LOG_TAG, "%s: BtSnoopImpl Stop", __func__);
    if (bt_snoop_helper_) {
      bt_snoop_helper_->Stop();
    }
    Teardown();
    is_started_ = false;
  }

  bool IsLogging() const override {
    return (bt_snoop_helper_ && bt_snoop_helper_->is_logging());
  }

 private:
  void Setup(const std::string& base_dir) {
    log_tree_mgr_ = std::unique_ptr<LogFileTreeManager>(
        new LogFileTreeManager(base_dir,
            std::string(kLogFilePrefix),
            std::string(kLogFileSuffix),
            std::string(kLogFileCurrentSuffix)));
    log_keeper_ = std::unique_ptr<LogFileKeeper>(
        new LogFileKeeper(log_tree_mgr_.get()));
    log_cleaner_ = std::unique_ptr<LogFileClearner>(
        new LogFileClearner(log_tree_mgr_.get(),
            SnoopLogConfig::GetInstance()->GetRecycleLogsSize()));
    log_stats_ = std::unique_ptr<LogStatistics>(new LogStatistics());
    log_stats_->AppendObserver(log_keeper_.get());
    log_stats_->AppendObserver(log_cleaner_.get());
    bt_snoop_helper_ = std::unique_ptr<BtSnoopHelper>(new BtSnoopHelper(
        log_keeper_.get(), log_stats_.get(), log_tree_mgr_.get()));

    // For low power concern, don't default turn on inbound data monitor
    if (StackConfig::GetInstance()->IsInboundDataMonitorEnabled()) {
      inbound_data_monitor_.reset(
          new InboundDataMonitor(std::string(MTK_BTSNOOP_MODULE)));
      if (inbound_data_monitor_ != nullptr) {
        inbound_data_monitor_->InboundDataFilterUpdate(kInboundDataVSEFWLog, true);
      }
    }
  }

  void Teardown() {
    if (bt_snoop_helper_ != nullptr) {
      bt_snoop_helper_.reset();
      bt_snoop_helper_ = nullptr;
    }

    if (log_stats_ != nullptr) {
      log_stats_.reset();
      log_stats_ = nullptr;
    }

    if (log_keeper_ != nullptr) {
      log_keeper_.reset();
      log_keeper_ = nullptr;
    }

    if (log_cleaner_ != nullptr) {
      log_cleaner_.reset();
      log_cleaner_ = nullptr;
    }

    if (log_tree_mgr_ != nullptr) {
      log_tree_mgr_.reset();
      log_tree_mgr_ = nullptr;
    }

    if (inbound_data_monitor_ != nullptr) {
      inbound_data_monitor_.reset();
      inbound_data_monitor_ = nullptr;
    }
  }

  bool is_started_;
  std::unique_ptr<InboundDataMonitor> inbound_data_monitor_;

  std::unique_ptr<LogFileTreeManager> log_tree_mgr_;
  std::unique_ptr<LogFileKeeper> log_keeper_;
  std::unique_ptr<LogFileClearner> log_cleaner_;
  std::unique_ptr<LogStatistics> log_stats_;
  std::unique_ptr<BtSnoopHelper> bt_snoop_helper_;

  std::mutex btsnoop_mutex_;
};

static constexpr uint64_t kInitLogCancelTimeoutMs = 1000 * 60 * 5;
class BtSnoopProxy
    : public BtSnoopSubject,
      public BtLogStateObserver {
  public:
    BtSnoopProxy()
        : is_boot_logging_(true),
          is_need_backup_boot_logs_(false),
          last_logging_request_(kBtHciLogUnknown),
          btsnoop_(nullptr),
          snoop_boot_backup_helper_(new BTSnoopBootBackupHelper()),
          init_log_timer_(nullptr) {}
    ~BtSnoopProxy() = default;

    // TODO: To re-factor it if gets time with state pattern
    void OnBtLogStateChanged(const BluetoothLogSetting* setting) override {
      CHECK(setting);
      if (last_logging_request_ == setting->GetHciLogLevel()) {
        LOG_WARN(LOG_TAG, "%s ignore dup request current: %d vs last: %d.",
            __func__, setting->GetHciLogLevel(), last_logging_request_);
        return;
      }

      CancelInitLogTimer();
      if (is_boot_logging_) {
        bool is_need_boot_backup = (nullptr != btsnoop_) ? true : false;
        if (nullptr == btsnoop_) {
          SnoopLogConfig::GetInstance()->Refresh();
          btsnoop_ = std::unique_ptr<BtSnoopSubject>(new BtSnoopImpl());
        }
        LOG_DEBUG(LOG_TAG, "%s boot: new state: %s vs current state: %s.",
            __func__,
            setting->IsHciSnoopEnabled() ? "on" : "off",
            btsnoop_->IsLogging() ? "on" : "off");
        is_boot_logging_ = false;
        DoLogSwitch(setting->IsHciSnoopEnabled(), is_need_boot_backup,
            setting->GetHciLogPath());
      } else {
        LOG_DEBUG(LOG_TAG, "%s normal: new state: %s vs current state: %s.",
            __func__,
            setting->IsHciSnoopEnabled() ? "on" : "off",
            btsnoop_->IsLogging() ? "on" : "off");
        if (btsnoop_->IsLogging() !=
            setting->IsHciSnoopEnabled()) {
          DoLogSwitch(setting->IsHciSnoopEnabled(), false,
              setting->GetHciLogPath());
        } else {
          LOG_DEBUG(LOG_TAG, "%s ignore for duplicated request %s.",
              __func__,
              setting->IsHciSnoopEnabled() ? "on" : "off");
        }
      }
      last_logging_request_ = setting->GetHciLogLevel();
    }

    void StartUp(const BtSnoopStartUpArg& arg) override {
      if (!is_boot_logging_) {
        if (!is_need_backup_boot_logs_) {
          LOG_DEBUG(LOG_TAG, "%s backup already", __func__);
          btsnoop_->StartUp(arg);
        } else {
          LOG_DEBUG(LOG_TAG, "%s do backup", __func__);
          btsnoop_->Stop();
          {
            std::lock_guard<std::mutex> lock(
                snoop_boot_backup_helper_->GetDataWrapperMutex());
            snoop_boot_backup_helper_->set_is_backup_in_flight(true);
          }
          btsnoop_->StartUp(arg);
        }
      } else {
        LOG_DEBUG(LOG_TAG, "%s boot", __func__);
        if (BluetoothLogSetting::GetInstance()->IsAutoStartHciSnoop()) {
          SnoopLogConfig::GetInstance()->Refresh();
          btsnoop_ = std::unique_ptr<BtSnoopSubject>(new BtSnoopImpl());
          snoop_boot_backup_helper_->RemoveLastTimeInitLogs(arg.base_dir_);
          btsnoop_->StartUp(arg);
          if (!StackConfig::GetInstance()->IsBTSnoopEnabled()) {
            StartInitLogTimer();
          }
        }
      }
    }

    void Capture(const BT_HDR* buffer, bool is_received) override {
      if (!is_boot_logging_) {
        std::lock_guard<std::mutex> lock(
            snoop_boot_backup_helper_->GetDataWrapperMutex());
        if (snoop_boot_backup_helper_ &&
            snoop_boot_backup_helper_->is_backup_in_flight()) {
          // error handling
          if (BtSnoopPacketizer::Validate(buffer, is_received)) {
            snoop_boot_backup_helper_->Capture(buffer, is_received);
          }
        } else {
          if (btsnoop_) {
            btsnoop_->Capture(buffer, is_received);
          }
        }
      } else {
        if (btsnoop_) {
          btsnoop_->Capture(buffer, is_received);
        }
      }
    }

    void Stop() override {
      CancelInitLogTimer();
      if (btsnoop_) {
        btsnoop_->Stop();
      }
    }

  private:
    void DoLogSwitch(bool is_on, bool is_need_boot_backup,
        const std::string& root_path) {
      if (is_on) {
        BtSnoopStartUpArg arg{
          GetBTSnoopLogPath(root_path), true,
          is_need_boot_backup ? snoop_boot_backup_helper_.get() : nullptr
        };
        is_need_backup_boot_logs_ = is_need_boot_backup;
        StartUp(arg);
      } else {
        Stop();
      }
    }

    void StartInitLogTimer() {
      init_log_timer_ = alarm_new("bt_log_init_snoop_timer");
      if (init_log_timer_) {
        alarm_set(init_log_timer_, kInitLogCancelTimeoutMs,
            [](void* context) {
                CHECK(NULL != context);
                LOG_DEBUG(LOG_TAG, "%s, no log command, to stop", __func__);
                BtSnoopSubject* btsnoop =
                    static_cast<BtSnoopSubject*>(context);
                btsnoop->Stop();
            },
            this);
      }
    }

    void CancelInitLogTimer() {
      if (init_log_timer_ != nullptr) {
        LOG_INFO(LOG_TAG, "%s", __func__);
        alarm_free(init_log_timer_);
        init_log_timer_ = nullptr;
      }
    }

    bool is_boot_logging_;
    bool is_need_backup_boot_logs_;
    BtHciLogLevel last_logging_request_;
    std::unique_ptr<BtSnoopSubject> btsnoop_;
    std::unique_ptr<BTSnoopBootBackupHelper> snoop_boot_backup_helper_;
    alarm_t* init_log_timer_;
};

// It is WRONG to default initialize tons of BT snoop classes and threads
// End user build should NOT suffer that if it is not for debugging on propose
BTSnoop::BTSnoop() : btsnoop_proxy_(new BtSnoopProxy()) {}

BTSnoop* BTSnoop::GetInstance() { return base::Singleton<BTSnoop>::get(); }

void BTSnoop::StartUp() {
  BtSnoopStartUpArg arg{GetBTSnoopLogInitPath(), false, nullptr};
  btsnoop_proxy_->StartUp(arg);
  btlogtool::SetUp();
  btlogtool::AddObserver(btsnoop_proxy_.get());
  // Add observer for SnoopLogConfig
  SnoopLogConfig::GetInstance()->Setup();
}

void BTSnoop::Capture(const BT_HDR* buffer, bool is_received) {
  btsnoop_proxy_->Capture(buffer, is_received);
}

void BTSnoop::Stop() {
  // Remove observer for SnoopLogConfig
  SnoopLogConfig::GetInstance()->TearDown();
  btlogtool::RemoveObserver(btsnoop_proxy_.get());
  btlogtool::TearDown();

  btsnoop_proxy_->Stop();
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
