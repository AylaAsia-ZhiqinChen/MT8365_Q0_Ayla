/*
 * Copyright (C) 2018 The Android Open Source Project
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

#include <app_mgmt_test.h>

#include <uapi/err.h>

int send_rsp(handle_t channel, uint8_t rsp)
{
    uint8_t tx_buffer[1];
    iovec_t tx_iov = {tx_buffer, 1};
    ipc_msg_t tx_msg = {1, &tx_iov, 0, NULL};
    int rc = 0;

    tx_buffer[0] = rsp;
    rc = send_msg(channel, &tx_msg);
    if (rc < 1) {
        TLOGI("send_msg failed: %d\n", rc);
        return -1;
    }

    return 0;
}

int recv_cmd(handle_t channel, uint8_t *cmd)
{
    int rc;
    uint8_t rx_buffer[MAX_CMD_LEN];
    iovec_t rx_iov = {rx_buffer, MAX_CMD_LEN};
    ipc_msg_t rx_msg = {1, &rx_iov, 0, NULL};
    ipc_msg_info_t msg_inf;

    rc = get_msg(channel, &msg_inf);
    if (rc != NO_ERROR) {
        TLOGI("get_msg failed: %d\n", rc);
        return rc;
    }

    if (msg_inf.len == 0 || msg_inf.len > MAX_CMD_LEN) {
        TLOGI("Invalid cmd info. len:%d\n", msg_inf.len);
        return -1;
    }

    rc = read_msg(channel, msg_inf.id, 0, &rx_msg);
    if ((size_t)rc != msg_inf.len) {
        TLOGI("read_msg failed: %d\n", rc);
        return -1;
    }

    rc = put_msg(channel, msg_inf.id);
    if (rc != NO_ERROR) {
        TLOGI("put_msg failed: %d\n", rc);
        return rc;
    }

    *cmd = ((uint8_t *)(rx_msg.iov->base))[0];

    return 0;
}
