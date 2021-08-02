/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <poll.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

#include <android-base/logging.h>
#include <android-base/stringprintf.h>

#include <android-base/unique_fd.h>
#include <time.h>

#include <cutils/fs.h>
#include <utils/Errors.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sysmacros.h>
#include <selinux/android.h>
#include <vector>
#include <sys/mount.h>
#include <utils/Timers.h>
#include <android-base/file.h>
#include <android-base/strings.h>
#include <android-base/parseint.h>
#include <pwd.h>
#include <fscrypt/fscrypt.h>

#include "Utils.h"
#include "VoldUtil.h"
#include "Ext4Crypt.h"

static void usage(char *progname);

struct selabel_handle *sehandle;

namespace android {
namespace vold {

static const char* kSgdiskPath = "/system/bin/sgdisk";
static const char* kSgdiskToken = " \t\n";

static const char* kFusePath = "/system/bin/sdcard";
//static const char* kBlkidPath = "/system/bin/blkid";

static const char* kSysfsMmcMaxMinorsDeprecated = "/sys/module/mmcblk/parameters/perdev_minors";
static const char* kSysfsMmcMaxMinors = "/sys/module/mmc_block/parameters/perdev_minors";

using android::status_t;
using android::base::ReadFileToString;
using android::base::WriteStringToFile;
using android::base::StringPrintf;

// Lock used to protect process-level SELinux changes from racing with each
// other between multiple threads.
static std::mutex kSecurityLock;

#define AID_ROOT 0 /* traditional unix root user */
#define AID_MEDIA_RW 1023        /* internal media storage write access */

int linkPrimary(int userId) {
    std::string source("/storage/emulated");
    source = StringPrintf("%s/%d", source.c_str(), userId);
    fs_prepare_dir(source.c_str(), 0755, AID_ROOT, AID_ROOT);

    std::string target(StringPrintf("/mnt/user/%d/primary", userId));
    if (TEMP_FAILURE_RETRY(unlink(target.c_str()))) {
        if (errno != ENOENT) {
            PLOG(WARNING) << "Failed to unlink " << target;
        }
    }
    LOG(DEBUG) << "Linking " << source << " to " << target;
    if (TEMP_FAILURE_RETRY(symlink(source.c_str(), target.c_str()))) {
        PLOG(WARNING) << "Failed to link";
        return -errno;
    }
    return 0;
}

namespace vfat {
static const char* kFsckPath = "/system/bin/fsck_msdos";

status_t Check(const std::string& source) {
    int pass = 1;
    int rc = 0;
    do {
        std::vector<std::string> cmd;
        cmd.push_back(kFsckPath);
        cmd.push_back("-p");
        cmd.push_back("-f");
        cmd.push_back(source);

        // Fat devices are currently always untrusted
        std::vector<std::string> output;
        rc = ForkExecvp(cmd, output, sFsckUntrustedContext);

        if (rc < 0) {
            LOG(ERROR) << "Filesystem check failed due to logwrap error";
            errno = EIO;
            return -1;
        }

        switch(rc) {
        case 0:
            LOG(INFO) << "Filesystem check completed OK";
            return 0;

        case 2:
            LOG(ERROR) << "Filesystem check failed (not a FAT filesystem)";
            errno = ENODATA;
            return -1;

        case 4:
            if (pass++ <= 3) {
                LOG(WARNING) << "Filesystem modified - rechecking (pass " << pass << ")";
                continue;
            }
            LOG(ERROR) << "Failing check after too many rechecks";
            errno = EIO;
            return -1;

        case 8:
            LOG(ERROR) << "Filesystem check failed (no filesystem)";
            errno = ENODATA;
            return -1;

        default:
            LOG(ERROR) << "Filesystem check failed (unknown exit code " << rc << ")";
            errno = EIO;
            return -1;
        }
    } while (0);

    return 0;
}

status_t Mount(const std::string& source, const std::string& target, bool ro,
        bool remount, bool executable, int ownerUid, int ownerGid, int permMask,
        bool createLost) {
    int rc;
    unsigned long flags;

    const char* c_source = source.c_str();
    const char* c_target = target.c_str();

    flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC | MS_NOATIME;

    flags |= (executable ? 0 : MS_NOEXEC);
    flags |= (ro ? MS_RDONLY : 0);
    flags |= (remount ? MS_REMOUNT : 0);

    auto mountData = android::base::StringPrintf(
            "utf8,uid=%d,gid=%d,fmask=%o,dmask=%o,shortname=mixed",
            ownerUid, ownerGid, permMask, permMask);

    rc = mount(c_source, c_target, "vfat", flags, mountData.c_str());

    if (rc && errno == EROFS) {
        LOG(ERROR) << source << " appears to be a read only filesystem - retrying mount RO";
        flags |= MS_RDONLY;
        rc = mount(c_source, c_target, "vfat", flags, mountData.c_str());
    }

    if (rc == 0 && createLost) {
        auto lost_path = android::base::StringPrintf("%s/LOST.DIR", target.c_str());
        if (access(lost_path.c_str(), F_OK)) {
            /*
             * Create a LOST.DIR in the root so we have somewhere to put
             * lost cluster chains (fsck_msdos doesn't currently do this)
             */
            if (mkdir(lost_path.c_str(), 0755)) {
                PLOG(ERROR) << "Unable to create LOST.DIR";
            }
        }
    }

    return rc;
}
}

class VolumeBase {
public:
    virtual ~VolumeBase() {};
    const std::string& getId() { return mId; };

    status_t setId(const std::string& id) {
        mId = id;
        return OK;
    };

    virtual status_t mount() = 0;
    virtual status_t create() { return OK; };

private:
    /* ID that uniquely references volume while alive */
    std::string mId;
};

class EmulatedVolume  : public VolumeBase {
public:
    EmulatedVolume(const std::string& rawPath){
        setId("emulated");
        mFusePid = 0;
        mRawPath = rawPath;
        mLabel = "emulated";
    };
    ~EmulatedVolume(){};

private:
        std::string mRawPath;
        std::string mLabel;

        std::string mFuseDefault;
        std::string mFuseRead;
        std::string mFuseWrite;

        /* PID of FUSE wrapper */
        pid_t mFusePid;

public:
    status_t mount() {
        // We could have migrated storage to an adopted private volume, so always
        // call primary storage "emulated" to avoid media rescans.
        std::string label = mLabel;

        mFuseDefault = StringPrintf("/mnt/runtime/default/%s", label.c_str());
        mFuseRead = StringPrintf("/mnt/runtime/read/%s", label.c_str());
        mFuseWrite = StringPrintf("/mnt/runtime/write/%s", label.c_str());

        if (fs_prepare_dir(mFuseDefault.c_str(), 0700, AID_ROOT, AID_ROOT) ||
                fs_prepare_dir(mFuseRead.c_str(), 0700, AID_ROOT, AID_ROOT) ||
                fs_prepare_dir(mFuseWrite.c_str(), 0700, AID_ROOT, AID_ROOT)) {
            PLOG(ERROR) << getId() << " failed to create mount points";
            return -errno;
        }

        dev_t before = GetDevice(mFuseWrite);

        if (!(mFusePid = fork())) {
            if (execl(kFusePath, kFusePath,
                    "-u", "1023", // AID_MEDIA_RW
                    "-g", "1023", // AID_MEDIA_RW
                    "-m",
                    "-w",
                    "-G",
                    "-i",
                    mRawPath.c_str(),
                    label.c_str(),
                    NULL)) {
                PLOG(ERROR) << "Failed to exec";
            }

            LOG(ERROR) << "FUSE exiting";
            _exit(1);
        }

        if (mFusePid == -1) {
            PLOG(ERROR) << getId()  << " failed to fork";
            return -errno;
        }

        nsecs_t start = systemTime(SYSTEM_TIME_BOOTTIME);
        while (before == GetDevice(mFuseWrite)) {
            LOG(VERBOSE) << "Waiting for FUSE to spin up...";
            usleep(50000); // 50ms

            nsecs_t now = systemTime(SYSTEM_TIME_BOOTTIME);
            if (nanoseconds_to_milliseconds(now - start) > 5000) {
                LOG(WARNING) << "Timed out while waiting for FUSE to spin up";
                return -ETIMEDOUT;
            }
        }
        /* sdcardfs will have exited already. FUSE will still be running */
        if (TEMP_FAILURE_RETRY(waitpid(mFusePid, nullptr, WNOHANG)) == mFusePid)
            mFusePid = 0;

        return OK;
    };

};

class PublicVolume  : public VolumeBase {
public:
    PublicVolume(dev_t device){
        mDevice = device;
        mFusePid = 0;
        setId(StringPrintf("public:%u,%u", major(device), minor(device)));
        mDevPath = StringPrintf("/dev/block/vold/%s", getId().c_str());
    };
    ~PublicVolume(){};

private:
        /* Kernel device representing partition */
        dev_t mDevice;

        std::string mRawPath;

        /* Block device path */
        std::string mDevPath;

        std::string mFuseDefault;
        std::string mFuseRead;
        std::string mFuseWrite;

        /* PID of FUSE wrapper */
        pid_t mFusePid;
        /* Filesystem type */
        std::string mFsType;
        /* Filesystem UUID */
        std::string mFsUuid;
        /* User-visible filesystem label */
        std::string mFsLabel;

public:
    status_t create() {
        return CreateDeviceNode(mDevPath, mDevice);
    }

    status_t mount() {
        ReadMetadataUntrusted(mDevPath, &mFsType, &mFsUuid, &mFsLabel);
        #if 0
        if (vfat::Check(mDevPath)) {
                LOG(ERROR) << getId() << " failed filesystem check";
                return -EIO;
         }
        #endif

        // Use UUID as stable name, if available
        std::string stableName = getId();
        if (!mFsUuid.empty()) {
            stableName = mFsUuid;
        }

        mRawPath = StringPrintf("/mnt/media_rw/%s", stableName.c_str());

        mFuseDefault = StringPrintf("/mnt/runtime/default/%s", stableName.c_str());
        mFuseRead = StringPrintf("/mnt/runtime/read/%s", stableName.c_str());
        mFuseWrite = StringPrintf("/mnt/runtime/write/%s", stableName.c_str());

        if (fs_prepare_dir(mRawPath.c_str(), 0700, AID_ROOT, AID_ROOT)) {
            PLOG(ERROR) << getId() << " failed to create mount points";
            return -errno;
        }

        if (vfat::Mount(mDevPath, mRawPath, false, false, false, AID_MEDIA_RW, AID_MEDIA_RW, 0007,
                        true)) {
            PLOG(ERROR) << getId() << " failed to mount " << mDevPath;
            return -EIO;
        }

        if (fs_prepare_dir(mFuseDefault.c_str(), 0700, AID_ROOT, AID_ROOT) ||
                fs_prepare_dir(mFuseRead.c_str(), 0700, AID_ROOT, AID_ROOT) ||
                fs_prepare_dir(mFuseWrite.c_str(), 0700, AID_ROOT, AID_ROOT)) {
            PLOG(ERROR) << getId() << " failed to create FUSE mount points";
            return -errno;
        }

        dev_t before = GetDevice(mFuseWrite);

        if (!(mFusePid = fork())) {
            if (execl(kFusePath, kFusePath,
                    "-u", "1023", // AID_MEDIA_RW
                    "-g", "1023", // AID_MEDIA_RW
                    "-U", "0",
                    mRawPath.c_str(),
                    stableName.c_str(),
                    NULL)) {
                PLOG(ERROR) << "Failed to exec";
            }
            LOG(ERROR) << "FUSE exiting";
            _exit(1);
        }

        if (mFusePid == -1) {
            PLOG(ERROR) << getId() << " failed to fork";
            return -errno;
        }

        nsecs_t start = systemTime(SYSTEM_TIME_BOOTTIME);
        while (before == GetDevice(mFuseWrite)) {
            LOG(VERBOSE) << "Waiting for FUSE to spin up...";
            usleep(50000); // 50ms

            nsecs_t now = systemTime(SYSTEM_TIME_BOOTTIME);
            if (nanoseconds_to_milliseconds(now - start) > 5000) {
                LOG(WARNING) << "Timed out while waiting for FUSE to spin up";
                return -ETIMEDOUT;
            }
        }
        /* sdcardfs will have exited already. FUSE will still be running */
        if (TEMP_FAILURE_RETRY(waitpid(mFusePid, nullptr, WNOHANG)) == mFusePid)
            mFusePid = 0;

        return OK;
    };

};


enum class Table {
    kUnknown,
    kMbr,
    kGpt,
};
static const unsigned int kMajorBlockMmc = 179;

class Disk {
private:
    /* ID that uniquely references this disk */
    std::string mId;
    /* Device path under dev */
    std::string mDevPath;
    /* Kernel device representing disk */
    dev_t mDevice;
    /* Current partitions on disk */
    std::vector<std::shared_ptr<VolumeBase>> mVolumes;

public:
    Disk(const std::string& path) {
        mDevice = 0;
        std::string tmp;
        for (int i = 0; ; i++) {
            if (!ReadFileToString(path, &tmp)) {
                 PLOG(ERROR) << "Failed to read: " << path << " i = " << i;
                 if (i >= 10 ) {
                     return;
                 }
                 usleep(500000); // 500ms
            }
            else {
                break;
            }
        }
        LOG(INFO) << path << ": '" << tmp << "'";

        auto split = android::base::Split(tmp, ": \t\n");
        auto it = split.begin();
        int major_num = 0, minor_num = 0;
        if (!android::base::ParseInt(*it, &major_num, 1)) {
            LOG(WARNING) << "Invalid major number " << *it;
            return;
        }
        ++it;
        if (!android::base::ParseInt(*it, &minor_num, 1)) {
            LOG(WARNING) << "Invalid minor number " << *it;
            return;
        }
        LOG(INFO) << "path:" << path << " (" << major_num << "," << minor_num << ")";

        mDevice = makedev(major_num, minor_num);
        mId = StringPrintf("disk:%u,%u", major(mDevice), minor(mDevice));
        mDevPath = StringPrintf("/dev/block/vold/%s", mId.c_str());
        CreateDeviceNode(mDevPath, mDevice);
    };

    ~Disk(){}   ;

    int getMaxMinors() {
        // Figure out maximum partition devices supported
        unsigned int majorId = major(mDevice);
        switch (majorId) {
            case kMajorBlockMmc: {
                // Per Documentation/devices.txt this is dynamic
                std::string tmp;
                if (!ReadFileToString(kSysfsMmcMaxMinors, &tmp) &&
                        !ReadFileToString(kSysfsMmcMaxMinorsDeprecated, &tmp)) {
                    LOG(ERROR) << "Failed to read max minors";
                    return -errno;
                }
                return std::stoi(tmp);
            }
        }

        LOG(ERROR) << "Unsupported block major type " << majorId;
        return -ENOTSUP;
    };

    void createPublicVolume(dev_t device) {
        auto vol = std::shared_ptr<VolumeBase>(new PublicVolume(device));
        mVolumes.push_back(vol);
        vol->create();
    };

    status_t readPartitions() {
        int maxMinors = getMaxMinors();
        if (maxMinors < 0) {
            return -ENOTSUP;
        }

        // Parse partition table

        std::vector<std::string> cmd;
        cmd.push_back(kSgdiskPath);
        cmd.push_back("--android-dump");
        cmd.push_back(mDevPath);

        std::vector<std::string> output;
        status_t res = ForkExecvp(cmd, output);
        if (res != OK) {
            LOG(WARNING) << "sgdisk failed to scan " << mDevPath;
            return res;
        }

        Table table = Table::kUnknown;
        bool foundParts = false;
        for (const auto& line : output) {
            auto split = android::base::Split(line, kSgdiskToken);
            auto it = split.begin();
            if (it == split.end()) continue;

            if (*it == "DISK") {
                if (++it == split.end()) continue;
                if (*it == "mbr") {
                    table = Table::kMbr;
                } else if (*it == "gpt") {
                    table = Table::kGpt;
                } else {
                    LOG(WARNING) << "Invalid partition table " << *it;
                    continue;
                }
            } else if (*it == "PART") {
                foundParts = true;

                if (++it == split.end()) continue;
                int i = 0;
                if (!android::base::ParseInt(*it, &i, 1, maxMinors)) {
                    LOG(WARNING) << "Invalid partition number " << *it;
                    continue;
                }
                dev_t partDevice = makedev(major(mDevice), minor(mDevice) + i);

                if (table == Table::kMbr) {
                    if (++it == split.end()) continue;
                    int type = 0;
                    if (!android::base::ParseInt("0x" + *it, &type)) {
                        LOG(WARNING) << "Invalid partition type " << *it;
                        continue;
                    }

                    switch (type) {
                        case 0x06:  // FAT16
                        case 0x07:  // HPFS/NTFS/exFAT
                        case 0x0b:  // W95 FAT32 (LBA)
                        case 0x0c:  // W95 FAT32 (LBA)
                        case 0x0e:  // W95 FAT16 (LBA)
                            createPublicVolume(partDevice);
                            break;
                    }
                }
            }
        }
        return OK;
    };

    void mountAllVolumes() {
        for (const auto& vol : mVolumes) {
            vol->mount();
        };
    }
};
}
}

using android::vold::EmulatedVolume;
using android::vold::Disk;

uid_t DecodeUid(const char* name) {
    if (isalpha(name[0])) {
        passwd* pwd = getpwnam(name);
        if (!pwd) {
            PLOG(ERROR) << "getpwnam failed";
            return 0;
        }

        return pwd->pw_uid;
    }
    return 0;
}

bool make_dir(const char *path, mode_t mode) {
    char* secontext = nullptr;
    if (sehandle) {
        if (!selabel_lookup(sehandle, &secontext, path, S_IFBLK)) {
            setfscreatecon(secontext);
        }
    }

    int rc = mkdir(path, mode);
    if (secontext) {
        int save_errno = errno;
        setfscreatecon(nullptr);
        freecon(secontext);
        errno = save_errno;
    }
    return rc == 0;
}

void mk_dirs_in_list() {
    struct Folder {
        const char *path;
        const mode_t mode;
        const char *uid;
        const char *gid;
    } folder_map[] = {
        { "/data/misc", 01771, "system", "misc"},
        { "/data/misc/vold", 0700, "root", "root"},
        { "/data/misc/profiles", 0771, "system", "system"},
        { "/data/misc/profiles/cur", 0771, "system", "system"},
        { "/data/misc/profiles/ref", 0771, "system", "system"},
        { "/data/vendor", 0771, "root", "root"},
        { "/data/vendor_ce", 0771, "root", "root"},
        { "/data/vendor_de", 0771, "root", "root"},
        { "/data/data", 0771, "system", "system"},
        { "/data/system", 0775, "system", "system"},
        { "/data/system/users", 0775 , "system", "system"},
        { "/data/mdlog", 0775 , "shell", "system"},
        { "/data/system_de", 0770 , "system", "system"},
        { "/data/system_ce", 0770 , "system", "system"},
        { "/data/misc_de", 01771 , "system", "misc"},
        { "/data/misc_ce", 01771 , "system", "misc"},
        { "/data/user", 0711 , "system", "system"},
        { "/data/user_de", 0711 , "system", "system"},
        { "/data/media", 0770 , "media_rw", "media_rw"},
    };

    struct Folder *cur;
    for (size_t i = 0; i < arraysize(folder_map); i++) {
        cur = &folder_map[i];
        LOG(INFO) << "try to mkdir: " << cur->path << ", mode=" << cur->mode;
        if (!make_dir(cur->path, cur->mode)) {
            /* chmod in case the directory already exists */
            if (errno == EEXIST) {
                if (fchmodat(AT_FDCWD, cur->path, cur->mode, AT_SYMLINK_NOFOLLOW) == -1) {
                    PLOG(ERROR) << "fchmodat() failed: " << cur->path << " mode:" << cur->mode;
                    continue;
                }
            } else {
                PLOG(ERROR) << "mkdir() failed: " << cur->path;
                continue;
            }
        }

        uid_t uid = DecodeUid(cur->uid);
        if (!uid && strcmp(cur->uid, "root")) {
            LOG(ERROR) << "Unable to decode UID for '" << cur->uid << "': " << cur->path;
            continue;
        }
        gid_t gid = DecodeUid(cur->gid);
        if (!gid && strcmp(cur->gid, "root")) {
            LOG(ERROR) << "Unable to decode GID for '" << cur->gid << "': " << cur->path;
            continue;
        }

        if (lchown(cur->path, uid, gid) == -1) {
            PLOG(ERROR) << "lchown failed: " << cur->path;
            continue;
        }

        /* chown may have cleared S_ISUID and S_ISGID, chmod again */
        //if (cur->mode & (S_ISUID | S_ISGID))
        {
            if (fchmodat(AT_FDCWD, cur->path, cur->mode, AT_SYMLINK_NOFOLLOW) == -1) {
               PLOG(ERROR) << "fchmodat() failed: " << cur->path << " mode:" << cur->mode;
               continue;
            }
        }
        if (e4crypt_is_native()) {
            if (e4crypt_set_directory_policy(cur->path)) {
                LOG(ERROR) << "set_policy_failed() failed: " << cur->path;
                continue;
            }
        }
    }
}

int main(int argc, char **argv) {
    setenv("ANDROID_LOG_TAGS", "*:v", 1);
    android::base::InitLogging(argv, android::base::LogdLogger(android::base::SYSTEM));

    LOG(INFO) << "storagemanagerd++";
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.size() > 0 && args[0] == "--wait") {
        // Just ignore the --wait flag
        args.erase(args.begin());
    }

    if (args.size() < 1) {
        usage(argv[0]);
        exit(5);
    }

	sehandle = selinux_android_file_context_handle();
	if (sehandle) {
		selinux_android_set_sehandle(sehandle);
	}

    fstab_default = fs_mgr_read_fstab_default();
    if (!fstab_default) {
        PLOG(ERROR) << "Failed to open default fstab";
    }

    if(args.size() >= 2 && args[0] == "volume" && args[1] == "mount_all") {

        e4crypt_unlock_user_key(0, 0, std::string("!"), std::string("!"));
        e4crypt_prepare_user_storage(std::string(""), 0, 0, android::os::IVold::STORAGE_FLAG_CE);

        android::vold::sBlkidContext = (char*)"u:r:blkid:s0";
        android::vold::sBlkidUntrustedContext = (char*)"u:r:blkid_untrusted:s0";
        android::vold::sFsckContext = (char*)"u:r:fsck:s0";
        android::vold::sFsckUntrustedContext = (char*)"u:r:fsck_untrusted:s0";

        std::shared_ptr<EmulatedVolume> mInternalEmulated = std::shared_ptr<EmulatedVolume>(new EmulatedVolume("/data/media"));
        mInternalEmulated->mount();
        android::vold::linkPrimary(0);

        auto disk = new Disk("/sys/block/mmcblk1/dev");
        disk->readPartitions();
        disk->mountAllVolumes();
    }
    else if(args.size() >= 1 && args[0] == "mk_dirs_in_list") {
        mk_dirs_in_list();
    }

    if(fstab_default) {
        fs_mgr_free_fstab(fstab_default);
    }

    LOG(INFO) << "storagemanagerd--";
    return 0;
}

static void usage(char *progname) {
    LOG(INFO) << "Usage: " << progname << " [--wait] <system> <subcommand> [args...]";
}

