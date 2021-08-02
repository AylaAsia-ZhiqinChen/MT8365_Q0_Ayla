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
#include <stdlib.h>
#include <memory>
#include <string>
#include <thread>

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/libs/fs/shared_fd.h"
#include "host/commands/ivserver/hald_client.h"
#include "host/commands/ivserver/vsocsharedmem_mock.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SetArgPointee;

namespace ivserver {
namespace test {

class HaldClientTest : public ::testing::Test {
 public:
  void SetUp() override {
    std::string socket_location;
    ASSERT_TRUE(GetTempLocation(&socket_location))
        << "Could not create temp file";
    LOG(INFO) << "Temp file location: " << socket_location;

    hald_server_socket_ = cvd::SharedFD::SocketLocalServer(
        socket_location.c_str(), false, SOCK_STREAM, 0666);
    test_socket_ = cvd::SharedFD::SocketLocalClient(socket_location.c_str(),
                                                    false, SOCK_STREAM);
    hald_socket_ =
        cvd::SharedFD::Accept(*hald_server_socket_, nullptr, nullptr);

    EXPECT_TRUE(hald_server_socket_->IsOpen());
    EXPECT_TRUE(test_socket_->IsOpen());
    EXPECT_TRUE(hald_socket_->IsOpen());
  }

  bool GetTempLocation(std::string* target) {
    char temp_location[] = "/tmp/hald-client-test-XXXXXX";
    mktemp(temp_location);
    *target = temp_location;
    if (target->size()) {
      cleanup_files_.emplace_back(*target);
      return true;
    }
    return false;
  }

  void TearDown() override {
    hald_socket_->Close();
    test_socket_->Close();
    hald_server_socket_->Close();

    for (const std::string& file : cleanup_files_) {
      unlink(file.c_str());
    }
  }

 protected:
  ::testing::NiceMock<VSoCSharedMemoryMock> vsoc_;

  cvd::SharedFD hald_server_socket_;
  cvd::SharedFD hald_socket_;
  cvd::SharedFD test_socket_;

 private:
  std::vector<std::string> cleanup_files_;
};

TEST_F(HaldClientTest, HandshakeTerminatedByHald) {
  std::thread thread(
      [this]() {
        auto client(HaldClient::New(vsoc_, hald_socket_));
        EXPECT_FALSE(client)
            << "Handshake should not complete when client terminates early.";
      });

  test_socket_->Close();
  thread.join();
}

TEST_F(HaldClientTest, HandshakeTerminatedByInvalidRegionSize) {
  uint16_t sizes[] = {0, VSOC_DEVICE_NAME_SZ + 1, 0xffff};

  for (uint16_t size : sizes) {
    std::thread thread([this, size]() {
      auto client(HaldClient::New(vsoc_, hald_socket_));
      EXPECT_FALSE(client) << "Handle should not be created when size is "
                           << size;
    });

    int32_t proto_version;
    EXPECT_EQ(sizeof(proto_version),
              static_cast<size_t>(test_socket_->Recv(
                  &proto_version, sizeof(proto_version), MSG_NOSIGNAL)));

    test_socket_->Send(&size, sizeof(size), MSG_NOSIGNAL);
    thread.join();
  }
}

TEST_F(HaldClientTest, FullSaneHandshake) {
  std::string temp;
  ASSERT_TRUE(GetTempLocation(&temp));
  cvd::SharedFD host_fd(
      cvd::SharedFD::Open(temp.c_str(), O_CREAT | O_RDWR, 0666));
  EXPECT_TRUE(host_fd->IsOpen());

  ASSERT_TRUE(GetTempLocation(&temp));
  cvd::SharedFD guest_fd(
      cvd::SharedFD::Open(temp.c_str(), O_CREAT | O_RDWR, 0666));
  EXPECT_TRUE(guest_fd->IsOpen());

  ASSERT_TRUE(GetTempLocation(&temp));
  cvd::SharedFD shmem_fd(
      cvd::SharedFD::Open(temp.c_str(), O_CREAT | O_RDWR, 0666));
  EXPECT_TRUE(shmem_fd->IsOpen());

  const std::string test_location("testing");
  EXPECT_CALL(vsoc_, GetEventFdPairForRegion(test_location, _, _))
      .WillOnce(DoAll(SetArgPointee<1>(host_fd), SetArgPointee<2>(guest_fd),
                      Return(true)));
  EXPECT_CALL(vsoc_, SharedMemFD()).WillOnce(ReturnRef(shmem_fd));

  std::thread thread([this]() {
    auto client(HaldClient::New(vsoc_, hald_socket_));
    EXPECT_TRUE(client);
  });

  int32_t proto_version;
  EXPECT_EQ(sizeof(proto_version),
            static_cast<size_t>(test_socket_->Recv(
                &proto_version, sizeof(proto_version), MSG_NOSIGNAL)));

  uint16_t size = test_location.size();
  EXPECT_EQ(sizeof(size), static_cast<size_t>(test_socket_->Send(
                              &size, sizeof(size), MSG_NOSIGNAL)));
  EXPECT_EQ(size, static_cast<size_t>(test_socket_->Send(test_location.data(),
                                                         size, MSG_NOSIGNAL)));

  // TODO(ender): delete this once no longer necessary. Currently, absence of
  // payload makes RecvMsgAndFDs hang forever.
  uint64_t control_data;
  struct iovec vec {
    &control_data, sizeof(control_data)
  };
  cvd::InbandMessageHeader hdr{nullptr, 0, &vec, 1, 0};
  cvd::SharedFD fds[3];

  EXPECT_GT(test_socket_->RecvMsgAndFDs<3>(hdr, MSG_NOSIGNAL, &fds), 0);
  EXPECT_TRUE(fds[0]->IsOpen());
  EXPECT_TRUE(fds[1]->IsOpen());
  EXPECT_TRUE(fds[2]->IsOpen());

  thread.join();
}

}  // namespace test
}  // namespace ivserver
