/* Copyright (C) 2026 Qore Technologies, s.r.o.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include <qore/Qore.h>
#include <qore/intern/QoreThreadList.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <array>
#include <thread>
#ifdef __linux__
#include <sys/resource.h>
#include <unistd.h>
#endif

// A native TLS destructor runs after q_run_thread has released its Qore TID.
// Keep it alive with a barrier so counter semantics can be checked without a race.
struct NativeCleanup {
    pthread_key_t key;
    std::mutex mutex;
    std::condition_variable condition;
    bool entered = false;
    bool released = false;
    unsigned destructors = 0;

    NativeCleanup() {
        assert(!pthread_key_create(&key, cleanup));
    }

    ~NativeCleanup() {
        assert(!pthread_key_delete(key));
    }

    static void cleanup(void* arg) {
        auto& state = *static_cast<NativeCleanup*>(arg);
        std::unique_lock<std::mutex> lock(state.mutex);
        state.entered = true;
        state.condition.notify_all();
        assert(state.condition.wait_for(lock, std::chrono::seconds(30), [&state]() {
            return state.released;
        }));
        // POSIX must finish repeated destructor passes too, not just the first callback.
        if (++state.destructors < 3) {
            assert(!pthread_setspecific(state.key, &state));
        }
    }

    static void worker(ExceptionSink*, void* arg) {
        auto& state = *static_cast<NativeCleanup*>(arg);
        assert(!pthread_setspecific(state.key, &state));
    }

    void waitForDestructor() {
        std::unique_lock<std::mutex> lock(mutex);
        assert(condition.wait_for(lock, std::chrono::seconds(30), [this]() {
            return entered;
        }));
    }

    void release() {
        std::lock_guard<std::mutex> lock(mutex);
        released = true;
        condition.notify_all();
    }
};

static void checkNativeCleanup(bool custom_stack) {
    NativeCleanup state;
    ExceptionSink xsink;
    int tid = custom_stack
        ? q_start_thread(&xsink, NativeCleanup::worker, &state, size_t(1024 * 1024), QTF_EXTERNAL_LIFECYCLE)
        : q_start_thread(&xsink, NativeCleanup::worker, &state, QTF_EXTERNAL_LIFECYCLE);
    assert(tid > 0 && !xsink);
    state.waitForDestructor();
    assert(tp_thread_counter.getCount() == 1);
    assert(thread_counter.getCount() == 0);
    state.release();
    assert(!tp_thread_counter.waitForZero(&xsink, 30000) && !xsink);
    assert(state.destructors == 3);
}

static void checkConcurrentCreation() {
    // All creators reach a native barrier before racing to start the first
    // external thread. This also exercises simultaneous reaper initialization.
    std::array<NativeCleanup, 16> states;
    std::array<std::thread, 16> creators;
    std::mutex mutex;
    std::condition_variable condition;
    unsigned ready = 0;
    for (unsigned i = 0; i < creators.size(); ++i) {
        creators[i] = std::thread([&, i]() {
            QoreForeignThreadHelper registration;
            assert(registration);
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++ready;
                condition.notify_all();
                assert(condition.wait_for(lock, std::chrono::seconds(30), [&]() {
                    return ready == creators.size();
                }));
            }
            ExceptionSink xsink;
            assert(q_start_thread(&xsink, NativeCleanup::worker, &states[i], QTF_EXTERNAL_LIFECYCLE) > 0);
            assert(!xsink);
        });
    }
    for (auto& creator : creators) {
        creator.join();
    }
    for (auto& state : states) {
        state.waitForDestructor();
    }
    assert(tp_thread_counter.getCount() == static_cast<int>(states.size()));
    for (auto& state : states) {
        state.release();
    }
    ExceptionSink xsink;
    assert(!tp_thread_counter.waitForZero(&xsink, 30000) && !xsink);
    for (auto& state : states) {
        assert(state.destructors == 3);
    }
}

static void checkFailedCreation() {
    ExceptionSink xsink;
    // An invalid stack must neither run the callback nor strand a counter entry.
    assert(q_start_thread(&xsink, NativeCleanup::worker, nullptr, size_t(1), QTF_EXTERNAL_LIFECYCLE) == -1);
    assert(xsink);
    QoreStringValueHelper error(xsink.getExceptionErr());
    assert(!strcmp(error->c_str(), "THREAD-CREATION-FAILURE"));
    xsink.clear();
    assert(tp_thread_counter.getCount() == 0);
    checkNativeCleanup(false);
}

#ifdef __linux__
class NativeCreationLimit {
public:
    NativeCreationLimit() {
        assert(!getrlimit(RLIMIT_NPROC, &original));
        struct rlimit limited = original;
        limited.rlim_cur = 1;
        assert(!setrlimit(RLIMIT_NPROC, &limited));
    }

    ~NativeCreationLimit() {
        assert(!setrlimit(RLIMIT_NPROC, &original));
    }

private:
    struct rlimit original;
};

static void checkNativeCreationFailure() {
    // RLIMIT_NPROC is per-process configuration; reducing this process's soft
    // limit leaves other developers' processes and resource limits unchanged.
    // Run this explicit mode as an unprivileged user on Linux.
    assert(geteuid() != 0);
    ExceptionSink xsink;
    int tid;
    {
        NativeCreationLimit limit;
        tid = q_start_thread(&xsink, NativeCleanup::worker, nullptr, QTF_EXTERNAL_LIFECYCLE);
    }
    assert(tid == -1 && xsink);
    QoreStringValueHelper error(xsink.getExceptionErr());
    assert(!strcmp(error->c_str(), "THREAD-CREATION-FAILURE"));
    xsink.clear();
    assert(tp_thread_counter.getCount() == 0);
    checkNativeCleanup(false);
}
#endif

int main(int argc, char** argv) {
    qore_init(QL_MIT, "UTF-8", true, QLO_DISABLE_SIGNAL_HANDLING);
#ifdef __linux__
    if (argc > 1 && !strcmp(argv[1], "--creation-failure")) {
        // First fail reaper startup, then fail a worker after the reaper exists.
        checkNativeCreationFailure();
        checkNativeCreationFailure();
    }
#endif
    if (argc == 1 || strcmp(argv[1], "--empty")) {
        checkConcurrentCreation();
        checkConcurrentCreation();
        checkNativeCleanup(false);
        checkNativeCleanup(true);
        checkFailedCreation();
    }
    qore_cleanup();
    puts("Passed native thread cleanup checks");
    return 0;
}
