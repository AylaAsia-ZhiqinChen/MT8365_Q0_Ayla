/*
 * Copyright (C) 2017 The Android Open Source Project
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
#include "host/commands/ivserver/options.h"

namespace ivserver {

IVServerOptions::IVServerOptions(const std::string &shm_file_path,
                                 const std::string &qemu_socket_path,
                                 const std::string &client_socket_path)
    : shm_file_path(shm_file_path),
      qemu_socket_path(qemu_socket_path),
      client_socket_path(client_socket_path) {}

std::ostream &operator<<(std::ostream &out, const IVServerOptions &options) {
  out << "\nshm_file: " << options.shm_file_path
      << "\nqemu_socket_path: " << options.qemu_socket_path
      << "\nclient_socket_path: " << options.client_socket_path << std::endl;

  return out;
}

}  // namespace ivserver
