// Copyright 2026 Qore Technologies, s.r.o.
// Regression for inline/heap bigint copies and allocator ownership in the vendored jsoncons.
#include <jsoncons/utility/bigint.hpp>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

static void check(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

struct AllocationState {
    size_t live_words = 0;
    size_t allocations = 0;
    bool fail = false;
};

template <typename T>
struct TrackedAllocator {
    using value_type = T;
    AllocationState* state;

    explicit TrackedAllocator(AllocationState* state = nullptr) : state(state) {
    }
    template <typename U>
    TrackedAllocator(const TrackedAllocator<U>& other) : state(other.state) {
    }
    T* allocate(size_t count) {
        if (!state || state->fail) {
            throw std::bad_alloc();
        }
        T* value = std::allocator<T>{}.allocate(count);
        state->live_words += count;
        ++state->allocations;
        return value;
    }
    void deallocate(T* value, size_t count) noexcept {
        std::allocator<T>{}.deallocate(value, count);
        state->live_words -= count;
    }
    template <typename U>
    bool operator==(const TrackedAllocator<U>& other) const noexcept {
        return state == other.state;
    }
    template <typename U>
    bool operator!=(const TrackedAllocator<U>& other) const noexcept {
        return !(*this == other);
    }
};

// Keep a separately compiled copy operation: GCC's -Og diagnostic occurred in this constructor.
jsoncons::bigint copyBigint(const jsoncons::bigint& source) {
    return source;
}

int main() {
    try {
        const char* values[] = {
            "0", "-1", "340282366920938463463374607431768211455",
            "123456789012345678901234567890123456789012345678901234567890",
            "-123456789012345678901234567890123456789012345678901234567890",
        };
        for (const char* text : values) {
            jsoncons::bigint source(text);
            jsoncons::bigint copied = copyBigint(source);
            check(copied.to_string() == text, "default allocator copy changed the value");
            copied += jsoncons::bigint(1);
            check(source.to_string() == text, "copy mutation changed the source");
            check(copied != source, "copy mutation was lost");

            AllocationState state;
            using Allocator = TrackedAllocator<uint64_t>;
            using Bigint = jsoncons::basic_bigint<Allocator>;
            {
                Bigint tracked(text, Allocator(&state));
                Bigint copy(tracked);
                check(copy.get_allocator().state == &state, "copy lost the allocator state");
                check(copy.to_string() == text, "stateful allocator copy changed the value");
                if (state.live_words) {
                    size_t live_before = state.live_words;
                    state.fail = true;
                    bool threw = false;
                    try {
                        Bigint failed(tracked);
                    } catch (const std::bad_alloc&) {
                        threw = true;
                    }
                    state.fail = false;
                    check(threw, "heap copy did not propagate allocation failure");
                    check(state.live_words == live_before, "failed copy leaked storage");
                    check(tracked.to_string() == text, "failed copy changed the source");
                }
            }
            check(state.live_words == 0, "copy storage was not released through its allocator");
        }
        std::puts("bigint copies: inline, heap, sign, allocator state and allocation failure passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
