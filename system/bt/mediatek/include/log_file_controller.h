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

#pragma once

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>

#include <base/files/file_path.h>

#include "log_writer.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

class LogStatistics;
class LogFileTreeManager;
class LogStatObserver {
  public:
    LogStatObserver() = default;
    virtual ~LogStatObserver() = default;

    virtual void Update(LogStatistics *stat) = 0;
};

class LogWriter;
class LogFileBackupHelper;
class LogFileKeeper : public LogStatObserver {
  public:
   explicit LogFileKeeper(LogFileTreeManager* file_tree_mgr);
   virtual ~LogFileKeeper();

   void Update(LogStatistics *stat) override;

   // TODO(Peng): Sorry to apply software workaround, to remove it
   bool MakeDir(const base::FilePath& dir);
   bool StartLogging(const base::FilePath& dir);
   bool AppendLogging(const base::FilePath& file_path);
   ssize_t LogData(void *parcel, size_t length) const;
   void StopLogging();

  private:
   void BackupLogFile(LogFileTreeManager* files_mgr);

   LogFileTreeManager* file_tree_mgr_;
   std::unique_ptr<LogWriter> log_writer_;
   std::unique_ptr<LogFileBackupHelper> log_backup_helper_;
};

class LogFileCleanRunner;
class RecycleThreadImpl;
class LogFileClearner : public LogStatObserver {
  public:
   explicit LogFileClearner(LogFileTreeManager* file_tree_mgr,
       int64_t bytes_to_be_cleaned);
   ~LogFileClearner();

   void Update(LogStatistics *stat) override;

  private:
   void Recycle(LogStatistics *stat);

   friend class LogFileCleanRunner;

   LogFileTreeManager* file_tree_mgr_;
   int64_t bytes_to_be_cleaned_;
   std::unique_ptr<RecycleThreadImpl> thread_;
   int cleaner_trigger_cnt_;
};

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
