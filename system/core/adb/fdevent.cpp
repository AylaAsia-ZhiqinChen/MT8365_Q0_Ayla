/* http://frotznet.googlecode.com/svn/trunk/utils/fdevent.c
**
** Copyright 2006, Brian Swetland <swetland@frotz.net>
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#define TRACE_TAG FDEVENT

#include "sysdeps.h"
#include "fdevent.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <atomic>
#include <deque>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <android-base/chrono_utils.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/stringprintf.h>
#include <android-base/thread_annotations.h>
#include <android-base/threads.h>

#include "adb_io.h"
#include "adb_trace.h"
#include "adb_unique_fd.h"
#include "adb_utils.h"
#include "sysdeps/chrono.h"

#define FDE_EVENTMASK  0x00ff
#define FDE_STATEMASK  0xff00

#define FDE_ACTIVE     0x0100
#define FDE_PENDING    0x0200
#define FDE_CREATED    0x0400

struct PollNode {
  fdevent* fde;
  adb_pollfd pollfd;

  explicit PollNode(fdevent* fde) : fde(fde) {
      memset(&pollfd, 0, sizeof(pollfd));
      pollfd.fd = fde->fd.get();

#if defined(__linux__)
      // Always enable POLLRDHUP, so the host server can take action when some clients disconnect.
      // Then we can avoid leaving many sockets in CLOSE_WAIT state. See http://b/23314034.
      pollfd.events = POLLRDHUP;
#endif
  }
};

// All operations to fdevent should happen only in the main thread.
// That's why we don't need a lock for fdevent.
static auto& g_poll_node_map = *new std::unordered_map<int, PollNode>();
static auto& g_pending_list = *new std::list<fdevent*>();
static std::atomic<bool> terminate_loop(false);
static bool main_thread_valid;
static uint64_t main_thread_id;

static uint64_t fdevent_id;

static bool run_needs_flush = false;
static auto& run_queue_notify_fd = *new unique_fd();
static auto& run_queue_mutex = *new std::mutex();
static auto& run_queue GUARDED_BY(run_queue_mutex) = *new std::deque<std::function<void()>>();

void check_main_thread() {
    if (main_thread_valid) {
        CHECK_EQ(main_thread_id, android::base::GetThreadId());
    }
}

void set_main_thread() {
    main_thread_valid = true;
    main_thread_id = android::base::GetThreadId();
}

static std::string dump_fde(const fdevent* fde) {
    std::string state;
    if (fde->state & FDE_ACTIVE) {
        state += "A";
    }
    if (fde->state & FDE_PENDING) {
        state += "P";
    }
    if (fde->state & FDE_CREATED) {
        state += "C";
    }
    if (fde->state & FDE_READ) {
        state += "R";
    }
    if (fde->state & FDE_WRITE) {
        state += "W";
    }
    if (fde->state & FDE_ERROR) {
        state += "E";
    }
    return android::base::StringPrintf("(fdevent %" PRIu64 ": fd %d %s)", fde->id, fde->fd.get(),
                                       state.c_str());
}

template <typename F>
static fdevent* fdevent_create_impl(int fd, F func, void* arg) {
    check_main_thread();
    CHECK_GE(fd, 0);

    fdevent* fde = new fdevent();
    fde->id = fdevent_id++;
    fde->state = FDE_ACTIVE;
    fde->fd.reset(fd);
    fde->func = func;
    fde->arg = arg;
    if (!set_file_block_mode(fd, false)) {
        // Here is not proper to handle the error. If it fails here, some error is
        // likely to be detected by poll(), then we can let the callback function
        // to handle it.
        LOG(ERROR) << "failed to set non-blocking mode for fd " << fd;
    }
    auto pair = g_poll_node_map.emplace(fde->fd.get(), PollNode(fde));
    CHECK(pair.second) << "install existing fd " << fd;

    fde->state |= FDE_CREATED;
    return fde;
}

fdevent* fdevent_create(int fd, fd_func func, void* arg) {
    return fdevent_create_impl(fd, func, arg);
}

fdevent* fdevent_create(int fd, fd_func2 func, void* arg) {
    return fdevent_create_impl(fd, func, arg);
}

unique_fd fdevent_release(fdevent* fde) {
    check_main_thread();
    if (!fde) {
        return {};
    }

    if (!(fde->state & FDE_CREATED)) {
        LOG(FATAL) << "destroying fde not created by fdevent_create(): " << dump_fde(fde);
    }

    unique_fd result = std::move(fde->fd);
    if (fde->state & FDE_ACTIVE) {
        g_poll_node_map.erase(result.get());

        if (fde->state & FDE_PENDING) {
            g_pending_list.remove(fde);
        }
        fde->state = 0;
        fde->events = 0;
    }

    delete fde;
    return result;
}

void fdevent_destroy(fdevent* fde) {
    // Release, and then let unique_fd's destructor cleanup.
    fdevent_release(fde);
}

static void fdevent_update(fdevent* fde, unsigned events) {
    auto it = g_poll_node_map.find(fde->fd.get());
    CHECK(it != g_poll_node_map.end());
    PollNode& node = it->second;
    if (events & FDE_READ) {
        node.pollfd.events |= POLLIN;
    } else {
        node.pollfd.events &= ~POLLIN;
    }

    if (events & FDE_WRITE) {
        node.pollfd.events |= POLLOUT;
    } else {
        node.pollfd.events &= ~POLLOUT;
    }
    fde->state = (fde->state & FDE_STATEMASK) | events;
}

void fdevent_set(fdevent* fde, unsigned events) {
    check_main_thread();
    events &= FDE_EVENTMASK;
    if ((fde->state & FDE_EVENTMASK) == events) {
        return;
    }
    CHECK(fde->state & FDE_ACTIVE);
    fdevent_update(fde, events);
    D("fdevent_set: %s, events = %u", dump_fde(fde).c_str(), events);

    if (fde->state & FDE_PENDING) {
        // If we are pending, make sure we don't signal an event that is no longer wanted.
        fde->events &= events;
        if (fde->events == 0) {
            g_pending_list.remove(fde);
            fde->state &= ~FDE_PENDING;
        }
    }
}

void fdevent_add(fdevent* fde, unsigned events) {
    check_main_thread();
    CHECK(!(events & FDE_TIMEOUT));
    fdevent_set(fde, (fde->state & FDE_EVENTMASK) | events);
}

void fdevent_del(fdevent* fde, unsigned events) {
    check_main_thread();
    CHECK(!(events & FDE_TIMEOUT));
    fdevent_set(fde, (fde->state & FDE_EVENTMASK) & ~events);
}

void fdevent_set_timeout(fdevent* fde, std::optional<std::chrono::milliseconds> timeout) {
    check_main_thread();
    fde->timeout = timeout;
    fde->last_active = std::chrono::steady_clock::now();
}

static std::string dump_pollfds(const std::vector<adb_pollfd>& pollfds) {
    std::string result;
    for (const auto& pollfd : pollfds) {
        std::string op;
        if (pollfd.events & POLLIN) {
            op += "R";
        }
        if (pollfd.events & POLLOUT) {
            op += "W";
        }
        android::base::StringAppendF(&result, " %d(%s)", pollfd.fd, op.c_str());
    }
    return result;
}

static std::optional<std::chrono::milliseconds> calculate_timeout() {
    std::optional<std::chrono::milliseconds> result = std::nullopt;
    auto now = std::chrono::steady_clock::now();
    check_main_thread();

    for (const auto& [fd, pollnode] : g_poll_node_map) {
        UNUSED(fd);
        auto timeout_opt = pollnode.fde->timeout;
        if (timeout_opt) {
            auto deadline = pollnode.fde->last_active + *timeout_opt;
            auto time_left = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now);
            if (time_left < std::chrono::milliseconds::zero()) {
                time_left = std::chrono::milliseconds::zero();
            }

            if (!result) {
                result = time_left;
            } else {
                result = std::min(*result, time_left);
            }
        }
    }

    return result;
}

static void fdevent_process() {
    std::vector<adb_pollfd> pollfds;
    for (const auto& pair : g_poll_node_map) {
        pollfds.push_back(pair.second.pollfd);
    }
    CHECK_GT(pollfds.size(), 0u);
    D("poll(), pollfds = %s", dump_pollfds(pollfds).c_str());

    auto timeout = calculate_timeout();
    int timeout_ms;
    if (!timeout) {
        timeout_ms = -1;
    } else {
        timeout_ms = timeout->count();
    }

    int ret = adb_poll(&pollfds[0], pollfds.size(), timeout_ms);
    if (ret == -1) {
        PLOG(ERROR) << "poll(), ret = " << ret;
        return;
    }

    auto post_poll = std::chrono::steady_clock::now();

    for (const auto& pollfd : pollfds) {
        if (pollfd.revents != 0) {
            D("for fd %d, revents = %x", pollfd.fd, pollfd.revents);
        }
        unsigned events = 0;
        if (pollfd.revents & POLLIN) {
            events |= FDE_READ;
        }
        if (pollfd.revents & POLLOUT) {
            events |= FDE_WRITE;
        }
        if (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            // We fake a read, as the rest of the code assumes that errors will
            // be detected at that point.
            events |= FDE_READ | FDE_ERROR;
        }
#if defined(__linux__)
        if (pollfd.revents & POLLRDHUP) {
            events |= FDE_READ | FDE_ERROR;
#if !ADB_HOST
            ADBLOG("for fd %d, revents = %x\n", pollfd.fd, pollfd.revents);
#endif
        }
#endif
        auto it = g_poll_node_map.find(pollfd.fd);
        CHECK(it != g_poll_node_map.end());
        fdevent* fde = it->second.fde;

        if (events == 0) {
            // Check for timeout.
            if (fde->timeout) {
                auto deadline = fde->last_active + *fde->timeout;
                if (deadline < post_poll) {
                    events |= FDE_TIMEOUT;
                }
            }
        }

        if (events != 0) {
            CHECK_EQ(fde->fd.get(), pollfd.fd);
            fde->events |= events;
            fde->last_active = post_poll;
            D("%s got events %x", dump_fde(fde).c_str(), events);
            fde->state |= FDE_PENDING;
            g_pending_list.push_back(fde);
        }
    }
}

template <class T>
struct always_false : std::false_type {};

static void fdevent_call_fdfunc(fdevent* fde) {
    unsigned events = fde->events;
    fde->events = 0;
    CHECK(fde->state & FDE_PENDING);
    fde->state &= (~FDE_PENDING);
    D("fdevent_call_fdfunc %s", dump_fde(fde).c_str());
    std::visit(
            [&](auto&& f) {
                using F = std::decay_t<decltype(f)>;
                if constexpr (std::is_same_v<fd_func, F>) {
                    f(fde->fd.get(), events, fde->arg);
                } else if constexpr (std::is_same_v<fd_func2, F>) {
                    f(fde, events, fde->arg);
                } else {
                    static_assert(always_false<F>::value, "non-exhaustive visitor");
                }
            },
            fde->func);
}

static void fdevent_run_flush() EXCLUDES(run_queue_mutex) {
    // We need to be careful around reentrancy here, since a function we call can queue up another
    // function.
    while (true) {
        std::function<void()> fn;
        {
            std::lock_guard<std::mutex> lock(run_queue_mutex);
            if (run_queue.empty()) {
                break;
            }
            fn = run_queue.front();
            run_queue.pop_front();
        }
        fn();
    }
}

static void fdevent_run_func(int fd, unsigned ev, void* /* userdata */) {
    CHECK_GE(fd, 0);
    CHECK(ev & FDE_READ);

    char buf[1024];

    // Empty the fd.
    if (adb_read(fd, buf, sizeof(buf)) == -1) {
        PLOG(FATAL) << "failed to empty run queue notify fd";
    }

    // Mark that we need to flush, and then run it at the end of fdevent_loop.
    run_needs_flush = true;
}

static void fdevent_run_setup() {
    {
        std::lock_guard<std::mutex> lock(run_queue_mutex);
        CHECK(run_queue_notify_fd.get() == -1);
        int s[2];
        if (adb_socketpair(s) != 0) {
            PLOG(FATAL) << "failed to create run queue notify socketpair";
        }

        if (!set_file_block_mode(s[0], false) || !set_file_block_mode(s[1], false)) {
            PLOG(FATAL) << "failed to make run queue notify socket nonblocking";
        }

        run_queue_notify_fd.reset(s[0]);
        fdevent* fde = fdevent_create(s[1], fdevent_run_func, nullptr);
        CHECK(fde != nullptr);
        fdevent_add(fde, FDE_READ);
    }

    fdevent_run_flush();
}

void fdevent_run_on_main_thread(std::function<void()> fn) {
    std::lock_guard<std::mutex> lock(run_queue_mutex);
    run_queue.push_back(std::move(fn));

    // run_queue_notify_fd could still be -1 if we're called before fdevent has finished setting up.
    // In that case, rely on the setup code to flush the queue without a notification being needed.
    if (run_queue_notify_fd != -1) {
        int rc = adb_write(run_queue_notify_fd.get(), "", 1);

        // It's possible that we get EAGAIN here, if lots of notifications came in while handling.
        if (rc == 0) {
            PLOG(FATAL) << "run queue notify fd was closed?";
        } else if (rc == -1 && errno != EAGAIN) {
            PLOG(FATAL) << "failed to write to run queue notify fd";
        }
    }
}

static void fdevent_check_spin(uint64_t cycle) {
    // Check to see if we're spinning because we forgot about an fdevent
    // by keeping track of how long fdevents have been continuously pending.
    struct SpinCheck {
        fdevent* fde;
        android::base::boot_clock::time_point timestamp;
        uint64_t cycle;
    };
    static auto& g_continuously_pending = *new std::unordered_map<uint64_t, SpinCheck>();
    static auto last_cycle = android::base::boot_clock::now();

    auto now = android::base::boot_clock::now();
    if (now - last_cycle > 10ms) {
        // We're not spinning.
        g_continuously_pending.clear();
        last_cycle = now;
        return;
    }
    last_cycle = now;

    for (auto* fde : g_pending_list) {
        auto it = g_continuously_pending.find(fde->id);
        if (it == g_continuously_pending.end()) {
            g_continuously_pending[fde->id] =
                    SpinCheck{.fde = fde, .timestamp = now, .cycle = cycle};
        } else {
            it->second.cycle = cycle;
        }
    }

    for (auto it = g_continuously_pending.begin(); it != g_continuously_pending.end();) {
        if (it->second.cycle != cycle) {
            it = g_continuously_pending.erase(it);
        } else {
            // Use an absurdly long window, since all we really care about is
            // getting a bugreport eventually.
            if (now - it->second.timestamp > 300s) {
                LOG(FATAL_WITHOUT_ABORT)
                        << "detected spin in fdevent: " << dump_fde(it->second.fde);
#if defined(__linux__)
                int fd = it->second.fde->fd.get();
                std::string fd_path = android::base::StringPrintf("/proc/self/fd/%d", fd);
                std::string path;
                if (!android::base::Readlink(fd_path, &path)) {
                    PLOG(FATAL_WITHOUT_ABORT) << "readlink of fd " << fd << " failed";
                }
                LOG(FATAL_WITHOUT_ABORT) << "fd " << fd << " = " << path;
#endif
                abort();
            }
            ++it;
        }
    }
}

void fdevent_loop() {
    set_main_thread();
    fdevent_run_setup();

    uint64_t cycle = 0;
    while (true) {
        if (terminate_loop) {
            return;
        }

        D("--- --- waiting for events");

        fdevent_process();

        fdevent_check_spin(cycle++);

        while (!g_pending_list.empty()) {
            fdevent* fde = g_pending_list.front();
            g_pending_list.pop_front();
            fdevent_call_fdfunc(fde);
        }

        if (run_needs_flush) {
            fdevent_run_flush();
            run_needs_flush = false;
        }
    }
}

void fdevent_terminate_loop() {
    terminate_loop = true;
}

size_t fdevent_installed_count() {
    return g_poll_node_map.size();
}

void fdevent_reset() {
    g_poll_node_map.clear();
    g_pending_list.clear();

    std::lock_guard<std::mutex> lock(run_queue_mutex);
    run_queue_notify_fd.reset();
    run_queue.clear();

    main_thread_valid = false;
    terminate_loop = false;
}
