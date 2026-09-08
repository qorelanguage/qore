/* Copyright (C) 2026 Qore Technologies, s.r.o.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 * Standalone Linux/glibc diagnostic; does not link to Qore.
 */
#include <pthread.h>
#include <sys/resource.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
static void* worker(void* p) { return p; }
int main(void) {
    assert(geteuid() != 0);
    struct rlimit saved, low;
    assert(!getrlimit(RLIMIT_NPROC, &saved));
    low = saved;
    low.rlim_cur = 1;
    pthread_t thread;
    assert(!pthread_create(&thread, 0, worker, 0));
    assert(!pthread_join(thread, 0));
    assert(!setrlimit(RLIMIT_NPROC, &low));
    size_t before = mallinfo2().uordblks;
    for (unsigned n=0; n<1000; ++n) {
        assert(pthread_create(&thread, 0, worker, 0) == EAGAIN);
    }
    size_t after = mallinfo2().uordblks;
    assert(!setrlimit(RLIMIT_NPROC, &saved));
    printf("native allocator bytes: before=%zu after=%zu delta=%zu\n", before, after, after-before);
    return 0;
}
