/******************************************************************************
 *
 *  Copyright 2014 Google, Inc.
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

#define LOG_TAG "bt_osi_semaphore"

#include "osi/include/semaphore.h"

#include <base/logging.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <time.h>

#include "osi/include/allocator.h"
#include "osi/include/log.h"
#include "osi/include/osi.h"
#if defined(MTK_COMMON) && (MTK_COMMON == TRUE) //reconnect too quick
#include "osi/include/semaphore.h"
#endif

#if !defined(EFD_SEMAPHORE)
#define EFD_SEMAPHORE (1 << 0)
#endif

#if defined(MTK_COMMON) && (MTK_COMMON == TRUE)
struct semaphore_t;
#else
struct semaphore_t {
  int fd;
};
#endif
semaphore_t* semaphore_new(unsigned int value) {
  semaphore_t* ret = static_cast<semaphore_t*>(osi_malloc(sizeof(semaphore_t)));
  ret->fd = eventfd(value, EFD_SEMAPHORE);
  if (ret->fd == INVALID_FD) {
    LOG_ERROR(LOG_TAG, "%s unable to allocate semaphore: %s", __func__,
              strerror(errno));
    osi_free(ret);
    ret = NULL;
  }
  return ret;
}

void semaphore_free(semaphore_t* semaphore) {
  if (!semaphore) return;

  if (semaphore->fd != INVALID_FD) close(semaphore->fd);
  osi_free(semaphore);
}

void semaphore_wait(semaphore_t* semaphore) {
  CHECK(semaphore != NULL);
  CHECK(semaphore->fd != INVALID_FD);

  eventfd_t value;
  if (eventfd_read(semaphore->fd, &value) == -1)
    LOG_ERROR(LOG_TAG, "%s unable to wait on semaphore: %s", __func__,
              strerror(errno));
}

#if defined(MTK_COMMON) && (MTK_COMMON == TRUE)
void semaphore_wait_timeout(semaphore_t* semaphore, long usec) {
  CHECK(semaphore != NULL);
  CHECK(semaphore->fd != INVALID_FD);

  int retval;
  eventfd_t value;
  struct timeval tv;
  fd_set rfds;

  FD_ZERO(&rfds);
  FD_SET(semaphore->fd, &rfds);

  tv.tv_sec = usec / 1000000;
  tv.tv_usec = usec % 1000000;

  retval = select(semaphore->fd + 1, &rfds, NULL, NULL, &tv);
  if (retval == -1) {
    LOG_ERROR(LOG_TAG, "%s select return fail for semaphore fd: %s", __func__,
    strerror(errno));
  }
  else if (retval == 0) {
    LOG_ERROR(LOG_TAG, "%s select say no data is available , and timeout", __func__);
  }
  else if (retval > 0) {
    LOG_ERROR(LOG_TAG, "%s select say data is available now", __func__);

  if (eventfd_read(semaphore->fd, &value) == -1)
    LOG_ERROR(LOG_TAG, "%s unable to wait on semaphore: %s", __func__,
              strerror(errno));
  }

}
#endif

bool semaphore_try_wait(semaphore_t* semaphore) {
  CHECK(semaphore != NULL);
  CHECK(semaphore->fd != INVALID_FD);

  int flags = fcntl(semaphore->fd, F_GETFL);
  if (flags == -1) {
    LOG_ERROR(LOG_TAG, "%s unable to get flags for semaphore fd: %s", __func__,
              strerror(errno));
    return false;
  }
  if (fcntl(semaphore->fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    LOG_ERROR(LOG_TAG, "%s unable to set O_NONBLOCK for semaphore fd: %s",
              __func__, strerror(errno));
    return false;
  }

  bool rc = true;
  eventfd_t value;
  if (eventfd_read(semaphore->fd, &value) == -1) rc = false;

  if (fcntl(semaphore->fd, F_SETFL, flags) == -1)
    LOG_ERROR(LOG_TAG, "%s unable to restore flags for semaphore fd: %s",
              __func__, strerror(errno));
  return rc;
}

void semaphore_post(semaphore_t* semaphore) {
  CHECK(semaphore != NULL);
  CHECK(semaphore->fd != INVALID_FD);

  if (eventfd_write(semaphore->fd, 1ULL) == -1)
    LOG_ERROR(LOG_TAG, "%s unable to post to semaphore: %s", __func__,
              strerror(errno));
}

int semaphore_get_fd(const semaphore_t* semaphore) {
  CHECK(semaphore != NULL);
  CHECK(semaphore->fd != INVALID_FD);
  return semaphore->fd;
}
