#ifndef LZC_UTILS_ASYNEXEC_H
#define LZC_UTILS_ASYNEXEC_H

#include "Common.h"
#include "Barrier.h"
#include "Queue.h"
#include "NoncopyableObject.h"

namespace lzc {
    typedef std::function<void()> task_t;
    typedef ts_queue_with_capacity<task_t> channel_t;

template <typename T, typename U>
class asynexec : public noncopyable_obj {
public:
    size_t DEFAULT_channel_capacity = std::numeric_limits<size_t>::max();

    explicit asynexec() {
        _input_channel.set_capacity(DEFAULT_channel_capacity);
        _output_channel.set_capacity(DEFAULT_channel_capacity);
    }

    explicit asynexec(int thread_num) : _thread_num(thread_num) {
        _input_channel.set_capacity(DEFAULT_channel_capacity);
        _output_channel.set_capacity(DEFAULT_channel_capacity);
    }

    explicit asynexec(int thread_num, size_t ic_size, size_t oc_size) : _thread_num(thread_num) {
        _input_channel.set_capacity(ic_size);
        _output_channel.set_capacity(oc_size);
    }

    // not thread safe
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

    // not thread safe
    void feed(const task_t &task) {
        // feed real task to channel
        for (int i = 0; i < _thread_num; ++i) {
            auto rt = task;
            _channel.push(std::move(rt));
        }
        // tasks are all pushed
        close_task_channel();
    }

    void close() {
        _input_channel.close();
        for (int i = 0; i < _thread_num; ++i) {
            ts[i].join();
        }
        _output_channel.close();
    }

    void close_task_channel() {
        _channel.close();
    }

    int get_thread_num() {
        return _thread_num;
    }

    void set_thread_num(int thread_num) {
        _thread_num = thread_num;
    }

    void push(task_t &task) {
        _channel.push(task);
    }

    bool input_empty() {
        return _input_channel.empty();
    }

    bool push_input(T &t) {
        return _input_channel.push(t);
    }

    bool wait_pop_input(T &t) {
        return _input_channel.wait_pop(t);
    }

    bool push_output(U &u) {
        return _output_channel.push(u);
    }

    bool wait_pop_output(U &u) {
        return _output_channel.wait_pop(u);
    }
private:
    int _thread_num = 0;
    channel_t _channel;
    ts_queue_with_capacity<T> _input_channel;
    ts_queue_with_capacity<U> _output_channel;
    std::vector<std::thread> ts;
}; // class asynexec
}; // namespace lzc
#endif
