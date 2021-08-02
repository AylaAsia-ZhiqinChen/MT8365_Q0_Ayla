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

#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include <base/macros.h>
#include "osi/include/log.h"

#define FILESYS_USER "bluetooth"
#define FILESYS_GROUP "log"
#define FILESYS_FOLDER_PERMISSION (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#define FILESYS_FILE_PERMISSION (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

//void _PrintPermission(const char *file_path);
void _SetPermission(const char *file_path,
    mode_t mode, const char *user_name, const char *group_name);
int _MakeDir(const char* file_path, mode_t mode, const char *user_name, const char *group_name);
bool _MoveFile(const char *from_path, const char *to_path, mode_t mode, const char *user_name, const char *group_name);

template <typename T>
std::string DataArrayToString(const T* arr, size_t array_len) {
  std::stringstream ss;
  int temp(0);
  for (size_t i(0); i < array_len; i++) {
    temp = static_cast<int>(arr[i]);
    ss << " " << std::uppercase << std::setfill('0') << std::hex << std::setw(2)
       << temp;
  }
  return ss.str();
}

template <typename T>
T StringToNumber(const std::string& str) {
  std::stringstream ss(str);
  T ret;
  ss >> ret;
  return ret;
}

template <typename T>
std::string NumberToString(T num) {
  std::ostringstream oss;
  oss << num;
  return oss.str();
}

template <typename T>
class Singleton {
 public:
  static T* GetInstance() {
    std::call_once(GetOnceFlag(), [] { instance_.reset(new T()); });
    return instance_.get();
  }

 protected:
  explicit Singleton<T>() = default;
  ~Singleton<T>() = default;

 private:
  static std::once_flag& GetOnceFlag() {
    static std::once_flag oncecall_;
    return oncecall_;
  }

  static std::unique_ptr<T> instance_;

  DISALLOW_COPY_AND_ASSIGN(Singleton);
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance_ = nullptr;

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor
