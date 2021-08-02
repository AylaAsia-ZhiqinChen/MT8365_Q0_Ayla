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

#define LOG_TAG "bt_mtk_util"

#include "mtk_util.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "osi/include/log.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

/*
void _PrintPermission(const char *file_path) {
  struct stat sb;
  stat(file_path, &sb);
  LOG_INFO(LOG_TAG, "%s: path[%s], uid[%s] gid[%s] mode[%o]",
    __func__, file_path, getpwuid(sb.st_uid)->pw_name, getgrgid(sb.st_gid)->gr_name, sb.st_mode);
}
*/

void _SetPermission(const char *file_path,
    mode_t mode, const char *user_name, const char *group_name) {
    struct passwd *pwd;
    struct group  *grp;
    pwd = getpwnam(user_name);
    grp = getgrnam(group_name);
    if (pwd != NULL && grp != NULL) {
      if (chown(file_path, pwd->pw_uid, grp->gr_gid) == -1) {
        LOG_INFO(LOG_TAG, "%s chown fail, path[%s], errno = %d(%s)",
            __func__, file_path, errno, strerror(errno));
      }
    }
    if (chmod(file_path, mode) == -1) {
      LOG_INFO(LOG_TAG, "%s chmod fail, path[%s], errno = %d(%s)",
        __func__, file_path, errno, strerror(errno));
    }
}

int _MakeDir(const char* dir_path, mode_t mode, const char *user_name, const char *group_name) {
  // path example: /data/debuglogger/connsyslog/bthci/CsLog_2019_0722_194436
  #define PATH_LEN 150
  char path[PATH_LEN];
  int dir_len = strlen(dir_path);

  memset(path, '\0', PATH_LEN);
  if(dir_len > (PATH_LEN - 2)) {
    LOG_ERROR(LOG_TAG, "%s fail, dir_len = %d", __func__, dir_len);
    return 1;
  }

  // add '/' in the end of string
  if(*(dir_path + dir_len - 1) == '/') {
    strncpy(path, dir_path, dir_len);
  } else {
    strncpy(path, dir_path, dir_len);
    path[dir_len] = '/';
    path[dir_len + 1] = '\0';
  }
  // go through the path string, mkdir and set permission if create new folder
  char *p = NULL;
  for (p = strchr(path + 1, '/'); p; p = strchr(p + 1, '/')) {
    *p = '\0';
    if (mkdir(path, mode) == -1) {
      if (errno != EEXIST) {
        *p = '/';
        LOG_ERROR(LOG_TAG, "%s fail, path[%s], errno = %d(%s)", __func__, path, errno, strerror(errno));
        return errno;
      }
    } else {
      _SetPermission(path, mode, user_name, group_name);
    }
    *p = '/';
  }
  return 0;
}

bool _MoveFile(const char *from_path, const char *to_path,
    mode_t mode, const char *user_name, const char *group_name) {
  int from_fd, to_fd;
  ssize_t ret_in, ret_out;
  char buffer[200];

  // open file
  from_fd = open(from_path, O_RDONLY);
  if(from_fd == -1) {
    LOG_ERROR(LOG_TAG, "%s: path[%s] open fail", __func__, from_path);
    return false;
  }
  to_fd = open(to_path, O_WRONLY | O_CREAT | O_TRUNC, mode);
  if(to_fd == -1) {
    close(from_fd);
    LOG_ERROR(LOG_TAG, "%s: path[%s] open fail", __func__, to_path);
    return false;
  }

  // copy to new file
  while((ret_in = read(from_fd, &buffer, sizeof(buffer))) > 0){
    ret_out = write(to_fd, &buffer, (ssize_t) ret_in);
    if(ret_out != ret_in){
      LOG_ERROR(LOG_TAG, "%s: path[%s] write error!", __func__, to_path);
      close (from_fd);
      close (to_fd);
      return false;
    }
  }
  close (from_fd);
  close (to_fd);

  // delete old file
  if(remove(from_path) != 0) {
    LOG_ERROR(LOG_TAG, "%s unable to delete, path[%s], errno = %d(%s)",
      __func__, from_path, errno, strerror(errno));
    return true;
  }
  return true;
}


}
}
}
}



