#ifndef LZC_UTILS_SPIN_LOCK_H
#define LZC_UTILS_SPIN_LOCK_H

#include "Common.h"

namespace lzc {

class spinlock {
public:
    void lock() {
        while(_lock.test_and_set(std::memory_order_acquire)) {
        }
    }
    void unlock() {
        _lock.clear(std::memory_order_release);
    }
private:
    std::atomic_flag _lock = ATOMIC_FLAG_INIT; // false
}; // class spinlock
}; // namespace lzc
#endif
