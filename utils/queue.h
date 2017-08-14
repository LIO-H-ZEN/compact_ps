// 20170813 by liuzhaocheng
#ifndef LZC_CPS_UTILS_QUEUE_H

#include "common.h"

namespace lzc {

// simple threadsafe queue.
// 没有capacity,所以没有满这种情况

template <class T>
class threadsafe_queue {
private:
    std::queue<std::shared_ptr<T>> _que;
    std::condition_variable _cv;
    mutable std::mutex _mu; 

public:
    explicit threadsafe_queue() {}    

    bool empty() {
        std::lock_guard<std::mutex> lg(_mu);
        return _que.empty();
    }

    size_t size() {
        std::lock_guard<std::mutex> lg(_mu);
        return _que.size();
    }    
    
    void push(T &t) {
        std::shared_ptr<T> data(std::make_shared<T>(std::move(t)));
        std::lock_guard<std::mutex> lg(_mu);
        _que.push(std::move(data));
        _cv.notify_one(); 
    }

    bool try_pop(T &ret) {
        std::lock_guard<std::mutex> lg(_mu);
        if (_que.empty()) {
            return false;
        }
        ret = std::move(*_que.front());
        _que.pop();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lg(_mu);
        if (_que.empty()) {
            return std::make_shared<T>();
        }
        std::shared_ptr<T> ret = _que.front();
        _que.pop();
        return ret;
    }

    void wait_pop(T& ret) {
        std::unique_lock<std::mutex> ul(_mu);
        _cv.wait(ul, [this]{return !_que.empty();});
        ret = std::move(*_que.front());
        _que.pop();
    }

    std::shared_ptr<T> wait_pop() {
        std::unique_lock<std::mutex> ul(_mu);
        _cv.wait(ul, [this]{return !_que.empty();});
        std::shared_ptr<T> ret = _que.front();
        _que.pop();
    }

}; // class threadsafe_queue

template <typename T>
class ts_queue_with_capacity {
private:
    size_t _capacity = std::numeric_limits<size_t>::max();
    std::queue<std::shared_ptr<T>> _que;
    mutable std::mutex _mu;
    std::condition_variable _full_cv;
    std::condition_variable _empty_cv;
    // 如果closed，那么只能读不能写入了
    bool _closed = false;

public:
    explicit ts_queue_with_capacity(size_t capacity): _capacity(capacity) {}

    explicit ts_queue_with_capacity() {}

    // not thread safe
    void set_capacity(size_t capacity) {
        this -> _capacity = capacity;
    }    

    // not thread safe
    size_t get_capacity() {
        return _capacity;
    }

    bool empty() {
        std::lock_guard<std::mutex> lg(_mu);
        return _que.empty();
    }

    size_t size() {
        std::lock_guard<std::mutex> lg(_mu);
        return _que.size();
    }    

    bool is_closed() {
        std::lock_guard<std::mutex> lg(_mu);
        return _closed;
    }

    void close() {
        std::lock_guard<std::mutex> lg(_mu);
        _closed = true;
    }

    bool push(T &t) {
        std::unique_lock<std::mutex> ul(_mu); 
        _full_cv.wait(ul, [this]{return !_que.size() >= _capacity || _closed;});
        if (_closed) {
            return false;
        } 
        std::shared_ptr<T> data(std::make_shared<T>(std::move(t)));
        _que.push(std::move(data)); 
        _empty_cv.notify_one();
        return true;
    }

    bool try_pop(T &ret) {
        std::lock_guard<std::mutex> lg(_mu);
        if (_que.empty()) {
            return false;
        }
        ret = std::move(*_que.front());
        _que.pop();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lg(_mu);
        if (_que.empty()) {
            return std::make_shared<T>();
        }
        std::shared_ptr<T> ret = _que.front();
        _que.pop();
        return ret;
    }

    void wait_pop(T& ret) {
        std::unique_lock<std::mutex> ul(_mu);
        _empty_cv.wait(ul, [this]{return !_que.empty();});
        ret = std::move(*_que.front());
        _que.pop();
        _full_cv.notify_one();
    }

    std::shared_ptr<T> wait_pop() {
        std::unique_lock<std::mutex> ul(_mu);
        _empty_cv.wait(ul, [this]{return !_que.empty();});
        std::shared_ptr<T> ret = _que.front();
        _que.pop();
        _full_cv.notify_one();
    }

    
}; // class ts_queue_with_capacity
}; // namespace lzc
#endif
