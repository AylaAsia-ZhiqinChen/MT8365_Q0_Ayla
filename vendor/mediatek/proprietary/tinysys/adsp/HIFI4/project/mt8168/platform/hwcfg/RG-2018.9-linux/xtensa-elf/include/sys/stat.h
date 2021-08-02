/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2010 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#if __cplusplus
extern "C" {
#endif

#include <sys/types.h>

struct stat
{
  dev_t     st_dev;
  ino_t     st_ino;
  mode_t    st_mode;
  nlink_t   st_nlink;
  uid_t     st_uid;
  gid_t     st_gid;
  dev_t     st_rdev;
  off_t     st_size;
  time_t    st_atime;
  time_t    st_mtime;
  time_t    st_ctime;
  blksize_t st_blksize;
  blkcnt_t  st_blocks;
  
  long      st_spare[4];
};

#define	S_IFMT (0xf000)
#define	S_IFDIR (0x4000)
#define	S_IFCHR (0x2000)
#define	S_IFBLK (0x6000)
#define	S_IFREG (0x8000)
#define	S_IFLNK (0xa000)
#define	S_IFSOCK (0xc000)
#define	S_IFIFO (0x1000)
#define	S_BLKSIZE (0x400)
#define	S_ISUID (0x800)
#define	S_ISGID (0x400)
#define	S_ISVTX (0x200)
#define	S_IREAD (0x100)
#define	S_IWRITE (0x80)
#define	S_IEXEC (0x40)
#define	S_ENFMT (0x400)

#define S_IRUSR (0x100)
#define S_IWUSR (0x80)
#define S_IXUSR (0x40)
#define S_IRGRP (0x20)
#define S_IWGRP (0x10)
#define S_IXGRP (0x8)
#define S_IROTH (0x4)
#define S_IWOTH (0x2)
#define S_IXOTH (0x1)
#define	S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
#define	S_IRWXG (S_IRGRP | S_IWGRP | S_IXGRP)
#define	S_IRWXO (S_IROTH | S_IWOTH | S_IXOTH)

#define	S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#define	S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#define	S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define	S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define	S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define	S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)

int    fstat(int, struct stat *);
int    lstat(const char *, struct stat *);
int    stat(const char *, struct stat *);

/* Unsupported features of POSIX stat.h  */
#if 0
    S_TYPEISMQ(buf)
    S_TYPEISSEM(buf)
    S_TYPEISSHM(buf)

int chmod(const char *, mode_t);
int fchmod(int, mode_t);
int mkdir(const char *, mode_t);
int mkfifo(const char *, mode_t);
int mknod(const char *, mode_t, dev_t);
mode_t umask(mode_t);

#endif

#if __cplusplus
}
#endif

#endif
