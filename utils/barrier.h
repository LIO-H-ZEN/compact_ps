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

// 基础的barrier 
// block()阻塞调用方，unblock()释放全部阻塞线程
class barrier : public barrier_base {
public:
    explicit barrier() {}

    void block() {
        std::unique_lock<std::mutex> ul(_mu);
        _block_cv.wait(ul, [&]{return !still_block;});
    }

    void unblock() {
        std::lock_guard<std::mutex> lg(_mu);
        still_block = false;
        _block_cv.notify_all();
    }
private:
    std::mutex _mu;
    std::condition_variable _block_cv;
    bool still_block = true;
}; // class barrier

class udf_barrier : public barrier_base {
    typedef std::function<void()> void_func;
    typedef std::function<bool()> bool_func;
public:
    explicit udf_barrier() {}    

    void block(const void_func &block_func, const bool_func &wait_func) {
        std::unique_lock<std::mutex> ul(_mu);
        block_func();
        _block_cv.wait(ul, wait_func);
    }

    void unblock(const void_func &unblock_func) {
        std::lock_guard<std::mutex> lg(_mu);
        // remember change wait_func's return
        unblock_func();
        _block_cv.notify_all();        
    }

    virtual void block() {throw std::bad_function_call();};
    virtual void unblock() {throw std::bad_function_call();};
private:
    std::mutex _mu;
    std::condition_variable _block_cv;
}; // class udf_barrier

}; // namespace lzc
#endif
