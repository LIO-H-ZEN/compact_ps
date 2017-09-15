#ifndef LZC_TRANSFER_LISTENER_H
#define LZC_TRANSFER_LISTENER_H

#include "TFcommon.h"

namespace lzc {

class listener : public noncopyable_obj {
public:
    explicit listener(void *zmq_ctx, int thread_num) : _zmq_ctx(zmq_ctx),_thread_num(thread_num) {
        PCHECK(_recv_socket = zmq_socket(_zmq_ctx, ZMQ_PULL));
    }

    void set_thread_num(int thread_num) {
        _thread_num = thread_num;
    }

    void set_zmq_ctx(void *_zmq_ctx) {
        zmq_ctx = _zmq_ctx;
        PCHECK(_recv_socket = zmq_socket(_zmq_ctx, ZMQ_PULL));
    }    

    void listen() {
        if (_recv_ip.empty()) {
            _recv_ip = get_local_ip();
        }
        PCHECK(_recv_port == -1) << "local receiver can only listen once";
        zmq_bind_random_port(_recv_ip, _recv_socket, _recv_addr, _recv_port);  
        PCHECK(_recv_port > 1024) << "receiver port not correct";
        LOG(INFO) << "receiver is binded to " << _recv_addr;
    }     

    void listen(std::string ip) {
        _recv_ip = ip;
        PCHECK(_recv_port == -1) << "local receiver can only listen once";
        zmq_bind_random_port(_recv_ip, _recv_socket, _recv_addr, _recv_port);  
        PCHECK(_recv_port > 1024) << "receiver port not correct";
        LOG(INFO) << "receiver is binded to " << _recv_addr;
    }

    virtual main_loop() {}

    virtual service_finished() {}    

    void service_start() {
        LOG(INFO) << "service starts " << _thread_num << " threads";
        PCHECK(_thread_num > 0);
        for (int i = 0; i < _thread_num; ++i) {
            _threads.emplace_back([this](){main_loop();});
        }
    }    

    void service_end() {
        LOG(INFO) << "service end";
        {
            std::lock_guard<spinlock> lg(_spl);
            CHECK(service_finished()) << "service not finished... can't end!";    
        }
        // TODO still have another task to check ?
        for (int i = 0; i < _thread_num; ++i) {
            _threads[i].join();
        }
        _threads.clear();
    }    

    void *recv_socket() {
        return _recv_socket;
    }

    int recv_port() {
        return _recv_port;
    }

private:
    void *_zmq_ctx = NULL; 
    void *_recv_socket = NULL;
    std::string _recv_ip;
    std::string _recv_addr;
    int _recv_port = -1;
    std::vector<std::thread> _threads;
    int _thread_num = 0;
    spinlock _spl;
}; // class listener
}; // namespace lzc
#endif
