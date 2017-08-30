#ifndef LZC_UTILS_RWLOCK_H
#define LZC_UTILS_RWLOCK_H
// 读写锁  //封装pthread中的pthread_rwlock_t // 同时写了个guard
#include "Common.h"
#include "pthread.h"

namespace lzc {

class rwlock {
public:
    rwlock() {
        pthread_rwlock_init(&_lock, NULL);
    }
    ~rwlock() {
        pthread_rwlock_destroy(&_lock);
    }
    void wait_read_lock() {
        pthread_rwlock_rdlock(&_lock);
    }
    void wait_write_lock() {
        pthread_rwlock_wrlock(&_lock);
    }
    void unlock() {
        pthread_rwlock_unlock(&_lock);
    }
private:
    pthread_rwlock_t _lock;
}; // class rwlock

class rlock_guard {
public:
    rlock_guard(rwlock &l) : _lock(&l) {
        _lock -> wait_read_lock();
    }
    ~rlock_guard() {
        _lock -> unlock();
    }
private:
    rwlock* _lock;    
}; //class rlock_guard

class wlock_guard {
public:
    wlock_guard(rwlock &l) : _lock(&l) {
        _lock -> wait_write_lock();
    }
    ~wlock_guard() {
        _lock -> unlock();
    }
private:
    rwlock* _lock;    
}; // class wlock_guard
}; // namespace lzc
#endif
