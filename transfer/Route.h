#ifndef LZC_TRANSFER_ROUTE_H
#define LZC_TRANSFER_ROUTE_H

#include "TFcommon.h"

namespace lzc {
// 维护着id和addr，mutex，socket的关系及相关的操作API
class basic_route : public noncopyable_obj {
public:
    explicit basic_route() {
        _zmq_ctx = zmq_ctx_new();
        CHECK(NULL != _zmq_ctx);
    }
    // not thread safe
    void add_node(int id, std::string addr) {
        CHECK(true == _id_addrs.emplace(id, addr).second);  
        CHECK(true == _id_mutexs.emplace(id, std::unique_ptr<std::mutex>(new std::mutex)).second)        
        void *socket = zmq_socket(_zmq_ctx, ZMQ_PUSH);
        CHECK(socket != NULL);
        CHECK(true == _id_sockets.emplace(id, socket).second);     
        // ignore_signal_call(zmq_connect, socket, addr.c_str());
    }

    void remove_node() {

    }
    ~basic_route() {

    }
private:
    void *_zmq_ctx = NULL;
    std::unordered_map<int, void *> _id_sockets;    
    std::unordered_map<int, std::string> _id_addrs;    
    std::unordered_map<int, std::unique_ptr<std::mutex>> _id_mutexs;    
}; // class basic_route
}; // namespace lzc
#endif
