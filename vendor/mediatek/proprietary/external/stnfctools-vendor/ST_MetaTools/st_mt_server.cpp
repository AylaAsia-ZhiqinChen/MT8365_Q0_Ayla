/******************************************************************************
 *
 *  Copyright (C) 2016 ST Microelectronics S.A.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include "st_mtktools.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>

// define to 1 if the server should accept multiple clients.
#define SERVER_LOOP 0

static void *server_handler(void *p) {
  int ret;

  (void)(p);

  // loop -- we will be pthread_canceled
  do {
    struct mtk_msg_hdr h;

    // wait for a client to connect.
    st_mt_g_state->clisock = accept(st_mt_g_state->servsock, NULL, NULL);
    if (st_mt_g_state->clisock < 0) {
      LOG(ERROR) << StringPrintf("Error accepting client socket: %s",
                                 strerror(errno));
      sleep(1);
      if (st_mt_g_state->exiting != 0) {
        break;
      }
      continue;
    }

    (void)pthread_mutex_lock(&st_mt_g_state->mtx);
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("CLI connected, initializing NFC stack");
    ret = st_mt_glue_startNFC();
    (void)pthread_mutex_unlock(&st_mt_g_state->mtx);
    if (ret != 0) {
      LOG(ERROR) << StringPrintf("NFC stack init failed, exiting");
      break;
    }

    // receive messages while socket is valid, and post them

    do {
      unsigned char *p = (unsigned char *)&h;
      ssize_t size, offset = 0;
      uint32_t len;  // payload length (not including header)

      // Read the command header
      do {
        size = read(st_mt_g_state->clisock, p + offset, sizeof(h) - offset);
        if (size <= 0) {
          break;
        }
        offset += size;
      } while (offset < (ssize_t)sizeof(h));
      if (size <= 0) {
        break;
      }

      // is this reasonable ?
      len = le_to_h_32(h.len_le);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Received header from client: cmd:%d len:%d",
                          le_to_h_32(h.cmd_le), len);
      if (len > MAX_MSG_SIZE) {
        break;
      }

      // ok, proceed.
      p = (unsigned char *)calloc(sizeof(h) + len, 1);
      if (!p) {
        LOG(ERROR) << StringPrintf("Malloc error");
        break;
      }

      memcpy(p, &h, offset);

      if (len) {
        do {
          size = read(st_mt_g_state->clisock, p + offset,
                      sizeof(h) + len - offset);
          if (size <= 0) {
            break;
          }
          offset += size;
        } while (offset < (ssize_t)sizeof(h) + len);
        if (size <= 0) {
          break;
        }
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Received message in full");
      }

      // full message has been read, post it.
      ret = pthread_mutex_lock(&st_mt_g_state->mtx);
      if (ret != 0) {
        LOG(ERROR) << StringPrintf("Error while locking mutex: %s",
                                   strerror(ret));
        break;
      }

      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                           &st_mt_g_state->mtx);

      while ((st_mt_g_state->recv_msg != NULL) &&
             (st_mt_g_state->exiting == 0)) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Message in fly, waiting...", __FUNCTION__);
        ret = pthread_cond_wait(&st_mt_g_state->cond, &st_mt_g_state->mtx);
        if (ret != 0) {
          LOG(ERROR) << StringPrintf("Error while waiting for cond: %s",
                                     strerror(ret));
          break;
        }
      }

      if (st_mt_g_state->recv_msg == NULL) {
        st_mt_g_state->recv_msg = (struct mtk_msg_hdr *)p;
        p = NULL;
        ret = pthread_cond_signal(&st_mt_g_state->cond);
        if (ret != 0) {
          LOG(ERROR) << StringPrintf("Warning: error while signaling cond: %s",
                                     strerror(ret));
        }
      }

      pthread_cleanup_pop(1);  // unlock mutex

      if (p) {
        LOG(ERROR) << StringPrintf("Error, did not queue new received message");
        free(p);
        break;
      }

    } while (1);  // it will break on error

    // close
    if (st_mt_g_state->clisock > 0) {
      close(st_mt_g_state->clisock);
    }
    st_mt_g_state->clisock = -1;

    // stop the stack.
    (void)pthread_mutex_lock(&st_mt_g_state->mtx);
    st_mt_glue_stopNFC();
    (void)pthread_mutex_unlock(&st_mt_g_state->mtx);

    // loop, if configured to do so.
  } while (SERVER_LOOP);

  // We are here in case of fatal error, or if the server is configured to
  // accept only 1 client.
  st_mt_g_state->exiting = 1;
  (void)pthread_cond_broadcast(&st_mt_g_state->cond);

  return NULL;
}

int st_mt_server_send(struct mtk_msg_hdr *msg) {
  unsigned char *p = (unsigned char *)msg;
  ssize_t ret;
  int offset = 0;

  if (st_mt_g_state->clisock < 0) {
    return -1;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Sending message to Tools client (cmd:%d, len:%d)",
                      le_to_h_32(msg->cmd_le), le_to_h_32(msg->len_le));

  // send over the socket
  do {
    ret = write(st_mt_g_state->clisock, p + offset,
                sizeof(struct mtk_msg_hdr) + le_to_h_32(msg->len_le) - offset);
    if (ret <= 0) {
      break;
    }
    offset += ret;
  } while (offset <
           (int)(sizeof(struct mtk_msg_hdr) + le_to_h_32(msg->len_le)));

  return (ret <= 0) ? -1 : 0;
}

int st_mt_serv_start() {
  int ret = -1;
  struct sockaddr_un server_addr;

  const char *sockname;

  // We ll create the server socket first.

  memset(&server_addr, 0, sizeof(server_addr));

  LOG(INFO) << "Cleaning previous sockets if any";
  unlink(MTK_SOCKET_NAME);
  unlink(MTK_SOCKET_NAME_EM);
  unlink(MTK_SOCKET_NAME_FM);

  st_mt_g_state->servsock = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (st_mt_g_state->servsock < 0) {
    LOG(ERROR) << StringPrintf("Error creating server socket: %s",
                               strerror(errno));
    return -1;
  }

  // bind
  server_addr.sun_family = AF_LOCAL;

#ifndef VENDOR_VERSION
  // Try to create with default name first.
  sockname = MTK_SOCKET_NAME;
  strncpy(server_addr.sun_path, MTK_SOCKET_NAME, sizeof(server_addr.sun_path));
  ret = bind(st_mt_g_state->servsock, (struct sockaddr *)&server_addr,
             sizeof(server_addr));
  if (ret < 0) {
    LOG(ERROR) << StringPrintf(
        "Error binding server socket %s: %s, trying %s...", MTK_SOCKET_NAME,
        strerror(errno), MTK_SOCKET_NAME_EM);
    strncpy(server_addr.sun_path, MTK_SOCKET_NAME_EM,
            sizeof(server_addr.sun_path));
    sockname = MTK_SOCKET_NAME_EM;
    ret = bind(st_mt_g_state->servsock, (struct sockaddr *)&server_addr,
               sizeof(server_addr));
    if (ret >= 0) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: Successfully bound to %s, assuming we are called by EM.",
          __FUNCTION__, server_addr.sun_path);
      st_mt_g_state->is_em = 1;
    }
  }
#endif
  if (ret < 0) {
    LOG(ERROR) << StringPrintf(
        "Error binding server socket %s: %s, trying %s...", MTK_SOCKET_NAME_EM,
        strerror(errno), MTK_SOCKET_NAME_FM);
    strncpy(server_addr.sun_path, MTK_SOCKET_NAME_FM,
            sizeof(server_addr.sun_path));
    sockname = MTK_SOCKET_NAME_FM;
    ret = bind(st_mt_g_state->servsock, (struct sockaddr *)&server_addr,
               sizeof(server_addr));
  }
  if (ret < 0) {
    LOG(ERROR) << StringPrintf("Error binding server socket %s: %s",
                               MTK_SOCKET_NAME_FM, strerror(errno));
    close(st_mt_g_state->servsock);
    return -1;
  }

  // Allow client processes to access this socket
  ret = chmod(sockname, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP |
                            S_IXGRP | S_IXOTH);
  if (ret < 0) {
    LOG(ERROR) << StringPrintf("Error chmod server socket: %s",
                               strerror(errno));
    close(st_mt_g_state->servsock);
    return -1;
  }

  // listen -- we ll accept only 1 client in the queue
  ret = listen(st_mt_g_state->servsock, 1);
  if (ret < 0) {
    LOG(ERROR) << StringPrintf("Error listening on server socket: %s",
                               strerror(errno));
    close(st_mt_g_state->servsock);
    return -1;
  }

  // create the handler thread
  ret =
      pthread_create(&st_mt_g_state->server_thread, NULL, server_handler, NULL);
  if (ret != 0) {
    LOG(ERROR) << StringPrintf("Error creating server handler: %s",
                               strerror(ret));
    close(st_mt_g_state->servsock);
    return -1;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "Server socket created and ready for clients: %s", sockname);
  return 0;
}

void st_mt_serv_stop() {
  int ret;

  // close the sockets
  if (st_mt_g_state->clisock >= 0) {
    shutdown(st_mt_g_state->clisock, SHUT_RDWR);
  }

  if (st_mt_g_state->servsock >= 0) {
    close(st_mt_g_state->servsock);
    st_mt_g_state->servsock = -1;
  }

  // stop the thread
  if (st_mt_g_state->server_thread) {
    //  ret = pthread_cancel(st_mt_g_state->server_thread);
    //  if (ret) {
    //      LOG(ERROR) << StringPrintf("Warning: cannot cancel server handler:
    //      %s", strerror(ret));
    //  } else {
    void *r;
    ret = pthread_join(st_mt_g_state->server_thread, &r);
    if (ret) {
      LOG(ERROR) << StringPrintf("Warning: cannot wait for server handler: %s",
                                 strerror(ret));
    }
    //  }
    memset(&st_mt_g_state->server_thread, 0,
           sizeof(st_mt_g_state->server_thread));
  }

  if (st_mt_g_state->clisock >= 0) {
    close(st_mt_g_state->clisock);
    st_mt_g_state->clisock = -1;
  }

  return;
}
