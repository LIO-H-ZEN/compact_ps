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

    virtual void update(int id) {}

    // not thread safe
    void add_node(int id, std::string addr) {
        CHECK(true == _id_addrs.emplace(id, addr).second);  
        CHECK(true == _id_mutexs.emplace(id, std::unique_ptr<std::mutex>(new std::mutex)).second)        
        void *socket = zmq_socket(_zmq_ctx, ZMQ_PUSH);
        CHECK(socket != NULL);
        CHECK(true == _id_sockets.emplace(id, socket).second);     
        // ignore_signal_call(zmq_connect, socket, addr.c_str()); // TODO when to connect
    }

    // not thread safe
    void remove_node(int id) {
        LOG(WARNING) << "deleting node: " << id;
        {
            auto search = _id_addrs.find(id);
            PCHECK(search != _id_addrs.end());
            _id_addrs.erase(id);
        }
        {
            auto search = _id_mutexs.find(id);
            PCHECK(search != _id_mutexs.end());
            _id_mutexs.erase(id);
        }
        {
            auto search = _id_sockets.find(id); 
            PCHECK(search != _id_sockets.end());
            PCHECK(zmq_close(search.second) == 0);
            _id_sockets.erase(search); 
        }
    }

    // thread safe
    void *get_socket(int id) {
        rlock_guard rg(&_rwlock);
        return _id_sockets[id];
    }

    // thread safe
    const std::string &get_addr(int id) {
        rlock_guard rg(&_rwlock);
        return _id_addrs[id];
    }

    // thread safe
    std::unique_ptr<std::mutex> &get_mutex(int id) {
        rlock_guard rg(&_rwlock);
        return _id_mutexs[id];
    } 

    virtual ~basic_route() {
        for (auto &socket : _id_sockets) {
            PCHECK(0 == zmq_close(socket));
        }
        PCHECK(0 == zmq_ctx_destroy(_zmq_ctx));
    }
private:
    void *_zmq_ctx = NULL;
    std::unordered_map<int, void *> _id_sockets;    
    std::unordered_map<int, std::string> _id_addrs;    
    std::unordered_map<int, std::unique_ptr<std::mutex>> _id_mutexs;    
    rwlock _rwlock; 
}; // class basic_route
}; // namespace lzc
#endif
