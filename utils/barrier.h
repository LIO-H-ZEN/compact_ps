#ifndef LZC_UTILS_BARRIER_H
#define LZC_UTILS_BARRIER_H

#include "common.h"

namespace lzc {
class barrier_base {
public:
    virtual void block() = 0;
    virtual void unblock() = 0;
    virtual ~barrier_base() {}; 
}; // class barrier_base

// 简单实现count barrier
// 在block的线程数==count时，释放所有被block的线程
class cnt_barrier : public barrier_base {
public:
    explicit cnt_barrier(int count): _count(count) {}

    void block() {
        std::unique_lock<std::mutex> ul(_mu);
        if (--_count == 0) {
            _block_cv.notify_all();
        } else {
            _block_cv.wait(ul, [&]{return _count == 0;});
        }
    }

    virtual void unblock() {throw std::bad_function_call();}
private:
    int _count = 0;
    std::mutex _mu;
    std::condition_variable _block_cv;
}; // class cnt_barrier
 
}; // namespace lzc

#endif
