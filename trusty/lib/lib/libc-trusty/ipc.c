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

#include <trusty_ipc.h>

#include <trusty_syscalls.h>

long port_create(const char *path, uint32_t num_recv_bufs,
                 uint32_t recv_buf_size, uint32_t flags)
{
    return _trusty_port_create(path, num_recv_bufs, recv_buf_size, flags);
}

long connect(const char *path, uint32_t flags)
{
    return _trusty_connect(path, flags);
}

long accept(uint32_t handle_id, struct uuid *peer_uuid)
{
    return _trusty_accept(handle_id, peer_uuid);
}

long close(uint32_t handle_id)
{
    return _trusty_close(handle_id);
}

long set_cookie(uint32_t handle, void *cookie)
{
    return _trusty_set_cookie(handle, cookie);
}

long handle_set_create(void)
{
    return _trusty_handle_set_create();
}

long handle_set_ctrl(uint32_t handle, uint32_t cmd, struct uevent *evt)
{
    return _trusty_handle_set_ctrl(handle, cmd, evt);
}

long wait(uint32_t handle_id, struct uevent *event, uint32_t timeout_msecs)
{
    return _trusty_wait(handle_id, event, timeout_msecs);
}

long wait_any(struct uevent *event, uint32_t timeout_msecs)
{
    return _trusty_wait_any(event, timeout_msecs);
}

long get_msg(uint32_t handle, struct ipc_msg_info *msg_info)
{
    return _trusty_get_msg(handle, msg_info);
}

long read_msg(uint32_t handle, uint32_t msg_id, uint32_t offset,
              struct ipc_msg *msg)
{
    return _trusty_read_msg(handle, msg_id, offset, msg);
}

long put_msg(uint32_t handle, uint32_t msg_id)
{
    return _trusty_put_msg(handle, msg_id);
}

long send_msg(uint32_t handle, struct ipc_msg *msg)
{
    return _trusty_send_msg(handle, msg);
}
