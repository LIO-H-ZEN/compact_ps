#ifndef LZC_TRANSFER_LISTENER_H
#define LZC_TRANSFER_LISTENER_H

#include "TFcommon.h"

namespace lzc {

class listener : public noncopyable_obj {
public:
    explicit listener(void *zmq_ctx, int thread_num) : _zmq_ctx(zmq_ctx),_thread_num(thread_num) {
        PCHECK(_rcv_socket = zmq_socket(_zmq_ctx, ZMQ_PULL));
    }

    void listen() {
        if (_rcv_ip.empty()) {
            _rcv_ip = get_local_ip();
        }
        PCHECK(_rcv_port == -1) << "local receiver can only listen once";
        zmq_bind_random_port(_rcv_ip, _rcv_socket, _rcv_addr, _rcv_port);  
        PCHECK(_rcv_port > 1024) << "receiver port not correct";
        LOG(INFO) << "receiver is binded to " << _rcv_addr;
    }     

    void listen(std::string ip) {
        _rcv_ip = ip;
        PCHECK(_rcv_port == -1) << "local receiver can only listen once";
        zmq_bind_random_port(_rcv_ip, _rcv_socket, _rcv_addr, _rcv_port);  
        PCHECK(_rcv_port > 1024) << "receiver port not correct";
        LOG(INFO) << "receiver is binded to " << _rcv_addr;
    }

    virtual main_loop() {}
    
    void service_start() {
        LOG(INFO) << "service starts " << _thread_num << " threads";
        PCHECK(_thread_num > 0);
        for (int i = 0; i < _thread_num; ++i) {
            _threads.emplace_back([this](){main_loop();});
        }
    }    
    
private:
    void *_zmq_ctx = NULL; 
    void *_rcv_socket = NULL;
    std::string _rcv_ip;
    std::string _rcv_addr;
    int _rcv_port = -1;
    std::vector<std::thread> _threads;
    int _thread_num = 0;
}; // class listener
}; // namespace lzc
#endif
