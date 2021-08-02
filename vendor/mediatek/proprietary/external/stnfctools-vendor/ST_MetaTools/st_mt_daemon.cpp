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
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

static struct st_mt_state daemon_state;
struct st_mt_state* st_mt_g_state = &daemon_state;

static int run_as_daemon() {
  int ret;
  int fd;
  int retry = 100;

  fd = open("/dev/null", O_RDONLY);
  if (fd == -1) {
    LOG(ERROR) << StringPrintf("Error while opening /dev/null: %s",
                               strerror(errno));
    return -1;
  }

  do {
    ret = dup2(fd, STDIN_FILENO);
    retry--;
  } while (ret == -1 && errno == EINTR && retry);
  if (ret == -1) {
    LOG(ERROR) << StringPrintf("Failed to redirect STDIN to /dev/null: %s",
                               strerror(errno));
    close(fd);
    return -1;
  }

  close(fd);

  umask(0);

  signal(SIGPIPE, SIG_IGN);

  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);

  return 0;
}

static void exit_handler(int s) {
  (void)s;
  st_mt_g_state->exiting = 1;
  (void)pthread_cond_signal(&st_mt_g_state->cond);
  // not sure we are allowed to call any trace here.
  // at least we are definitely not allowed to lock mutex.
}

int main(int argc, char* argv[]) {
  int ret;
  struct sigaction sighandler;

  (void)argc;
  (void)argv;

  nfc_debug_enabled = true;
  logging::SetMinLogLevel(logging::LOG_VERBOSE);

  memset(st_mt_g_state, 0, sizeof(*st_mt_g_state));

  LOG(INFO) << StringPrintf("Starting...");

  // Initialize the state
  ret = pthread_mutex_init(&st_mt_g_state->mtx, NULL);
  if (ret != 0) {
    LOG(ERROR) << StringPrintf("Error while initialing mutex: %s",
                               strerror(ret));
    return -1;
  }
  ret = pthread_cond_init(&st_mt_g_state->cond, NULL);
  if (ret != 0) {
    LOG(ERROR) << StringPrintf("Error while initialing condvar: %s",
                               strerror(ret));
    return -1;
  }
  st_mt_g_state->servsock = -1;
  st_mt_g_state->clisock = -1;

  // "daemonize" ourselves
  if (argc > 1 && !strcmp("-f", argv[1])) {
    LOG(INFO) << StringPrintf("Run in foreground.");
  } else {
    LOG(INFO) << StringPrintf("Daemonizing...");
    ret = run_as_daemon();
    if (ret != 0) {
      LOG(ERROR) << StringPrintf("Error while daemonizing");
      return -1;
    }
  }

  // install a signal handler to stop this daemon upon exit
  sighandler.sa_handler = exit_handler;
  sigemptyset(&sighandler.sa_mask);
  sighandler.sa_flags = 0;
  //  sigaction(SIGINT, &sighandler, NULL);
  sigaction(SIGTERM, &sighandler, NULL);

  // Now start the server.
  LOG(INFO) << StringPrintf("Start server...");
  ret = st_mt_serv_start();
  if (ret != 0) {
    LOG(ERROR) << StringPrintf("Error while starting server: %s",
                               strerror(ret));
    return -1;
  }

  // this blocks until an error happens or exit condition was signaled
  st_mt_event_loop();

  // clean up sockets
  st_mt_serv_stop();

  // we are done
  LOG(INFO) << StringPrintf("Exiting now.");
  _exit(0);
};
