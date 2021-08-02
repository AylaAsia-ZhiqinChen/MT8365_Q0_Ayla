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

#include "app_mgmt_srv2.h"

#include <app_mgmt_test.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <trusty_ipc.h>
#include <uapi/err.h>

#define LOG_TAG    "app-mgmt-test-srv2"
#define CTRL_PORT  "com.android.trusty.appmgmt.srv2"

static handle_t ctrl_port;
static handle_t start_port;
static handle_t channel;

static bool handle_cmd(uint8_t cmd)
{
    int rc;
    bool done = false;
    bool delay = false;
    uint8_t rsp = RSP_OK;

    switch (cmd) {
    case CMD_NOP:
        break;
    case CMD_CLOSE_PORT:
        rc = close(start_port);
        if (rc != NO_ERROR) {
            TLOGI("port close failed: %d\n", rc);
            rsp = RSP_CMD_FAILED;
            done = true;
        }
        break;
    case CMD_EXIT:
        done = true;
        break;
    case CMD_DELAYED_EXIT:
        delay = true;
        done = true;
        break;
    default:
        TLOGI("Invalid cmd: %d\n", cmd);
        rsp = RSP_INVALID_CMD;
        done = true;
    }

    rc = send_rsp(channel, rsp);
    if (rc < 0) {
        TLOGI("Failed to send response: %d \n", rc);
        return true;
    } else if (delay) {
        nanosleep(0, 0, 1000000);
    }

    return done;
}

int main(void)
{
    int rc;
    uint8_t cmd;
    bool done = false;
    handle_t handle_set;
    uevent_t uevt;
    uuid_t peer_uuid;

    rc = handle_set_create();
    if (rc < 0) {
        TLOGI("failed (%d) to create handle set \n", rc);
        return rc;
    }

    handle_set = (handle_t)rc;

    rc = port_create(START_PORT,  1, 1024, IPC_PORT_ALLOW_NS_CONNECT);
    if (rc < 0) {
        TLOGI("failed (%d) to create start port\n", rc );
        return rc;
    }

    start_port = (handle_t)rc;

    uevt.handle = start_port;
    uevt.event = ~0;
    uevt.cookie = NULL;

    rc = handle_set_ctrl(handle_set, HSET_ADD, &uevt);
    if (rc < 0) {
        TLOGI("failed (%d) to add start port to handle set \n", rc);
        return rc;
    }

    rc = port_create(CTRL_PORT,  1, 1024, IPC_PORT_ALLOW_NS_CONNECT);
    if (rc < 0) {
        TLOGI("failed (%d) to create ctrl port\n", rc );
        return rc;
    }

    ctrl_port = (handle_t)rc;

    uevt.handle = ctrl_port;
    uevt.event = ~0;
    uevt.cookie = NULL;

    rc = handle_set_ctrl(handle_set, HSET_ADD, &uevt);
    if (rc < 0) {
        TLOGI("failed (%d) to add control port to handle set \n", rc);
        return rc;
    }

    while (!done) {
        rc = wait(handle_set, &uevt, -1);
        if (rc != NO_ERROR || !(uevt.event & IPC_HANDLE_POLL_READY)){
            TLOGI("Port wait failed: %d(%d)\n", rc, uevt.event);
            return rc;
        }

        rc = accept(uevt.handle, &peer_uuid);
        if (rc < 0) {
            TLOGI("Accept failed %d\n", rc );
            return rc;
        }

        channel = (handle_t)rc;

        rc = wait(channel, &uevt, -1);
        if (rc < 0 || !(uevt.event & IPC_HANDLE_POLL_MSG)) {
            TLOGI("Channel wait failed: %d(%d)\n", rc, uevt.event);
            return rc;
        }

        rc = recv_cmd(channel, &cmd);
        if (rc < 0) {
            TLOGI("recv_cmd failed: %d\n", rc);
            return rc;
        }

        done = handle_cmd(cmd);

        rc = close(channel);
        if (rc != NO_ERROR) {
            TLOGI("channel close failed: %d\n", rc);
            return rc;
        }
    }

    rc = close(ctrl_port);
    if (rc != NO_ERROR) {
        TLOGI("port close failed: %d\n", rc);
        return rc;
    }

    rc = close(handle_set);
    if (rc != NO_ERROR) {
        TLOGI("handle set close failed: %d\n", rc);
        return rc;
    }

    return 0;
}
