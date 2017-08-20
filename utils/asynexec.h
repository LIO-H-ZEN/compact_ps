#ifndef LZC_UTILS_ASYNEXEC_H
#define LZC_UTILS_ASYNEXEC_H

#include "common.h"
#include "barrier.h"
#include "queue.h"
#include "noncopyable_object.h"

namespace lzc {
class asynexec : public noncopyable_obj {
public:
    typedef std::function<void()> task_t;
    typedef ts_queue_with_capacity<task_t> channel_t;

    explicit asynexec() {}

    explicit asynexec(int thread_num) : _thread_num(thread_num) {}

    void open() {
        for (int i = 0; i < _thread_num; ++i) {
            ts.emplace_back([&](int i){
                std::cout << "thread "  << i << " started"  << std::endl;
                task_t thread_task;
                bool valid = false;
                while ((valid = _channel.wait_pop(thread_task))) {
                    if (!valid) {
                        break;
                    } else {
                        thread_task();
                    }
                }
            std::cout << "thread "  << i << " ended"  << std::endl;
            }, i);
        }
    } 

    void close() {
        _channel.close();
        for (int i = 0; i < _thread_num; ++i) {
            ts[i].join();
        }
    }

    int get_thread_num() {
        return _thread_num;
    }

    void set_thread_num(int thread_num) {
        _thread_num = thread_num;
    }

    void push(task_t task) {
        _channel.push(task);
    }

private:
    int _thread_num = 0;
    channel_t _channel;
    std::vector<std::thread> ts;

}; // class asynexec


void asyn_feed(const asynexec::task_t &task, asynexec &exec) {
    barrier b;
    std::atomic<int> thread_num(exec.get_thread_num());         
    // define real task
    asynexec::task_t rtask = [&task, &thread_num, &b]{
        task();
        if (--thread_num == 0) {
            b.unblock();
        }
    };    
    // feed real task to channel
    for (int i = 0; i < thread_num; ++i) {
        auto rt = rtask;
        exec.push(std::move(rt));
    }
    // tasks are all pushed
    exec.close();
    // wait until all threads finished proceding task()
    b.block();
}
}; // namespace lzc
#endif
