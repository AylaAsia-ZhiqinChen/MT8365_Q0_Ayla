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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trusty_ipc.h>
#include <uapi/err.h>

#define LOG_TAG    "app-mgmt-test-srv1"
#define CTRL_PORT  "com.android.trusty.appmgmt.srv1"

/* Resources are intentionally not freed to test application cleanup on exit */
int main(void)
{
    int rc;
    uint8_t cmd;
    uint8_t rsp = RSP_OK;
    handle_t ctrl_port;
    handle_t ctrl_chan;
    uevent_t uevt;
    uuid_t peer_uuid;

    rc = port_create(CTRL_PORT,  1, 1024, IPC_PORT_ALLOW_NS_CONNECT);
    if (rc < 0) {
        TLOGI("failed (%d) to create ctrl port\n", rc );
        return rc;
    }

    ctrl_port = (handle_t)rc;

    rc = wait(ctrl_port, &uevt, -1);
    if (rc != NO_ERROR || !(uevt.event & IPC_HANDLE_POLL_READY)){
        TLOGI("Port wait failed: %d(%d)\n", rc, uevt.event);
        return rc;
    }

    rc = accept(uevt.handle, &peer_uuid);
    if (rc < 0) {
        TLOGI("Accept failed %d\n", rc );
        return rc;
    }

    ctrl_chan = (handle_t) rc;
    rc = wait(ctrl_chan, &uevt, -1);
    if (rc < 0 || !(uevt.event & IPC_HANDLE_POLL_MSG)) {
        TLOGI("Channel wait failed: %d(%d)\n", rc, uevt.event);
        return rc;
    }

    rc = recv_cmd(ctrl_chan, &cmd);
    if (rc < 0) {
        TLOGI("recv_cmd failed: %d\n", rc);
        return rc;
    }

    if (cmd != CMD_EXIT) {
        TLOGI("Invalid cmd: %d\n", cmd);
        rsp = RSP_INVALID_CMD;
    }

    rc = send_rsp(ctrl_chan, rsp);
    if (rc < 0) {
        TLOGI("send_rsp failed: %d\n", rc);
        return rc;
    }

    return 0;
}
