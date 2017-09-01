#ifndef LZC_TRANSFER_ROUTE_H
#define LZC_TRANSFER_ROUTE_H

#include "TFcommon.h"

namespace lzc {

class basic_route : public noncopyable_obj {
public:
    explicit basic_route() {
        _zmq_ctx = zmq_ctx_new();
    }
    void add_node() {

    }
    void remove_node() {

    }
private:
    void *_zmq_ctx = NULL;
    
}; // class basic_route
}; // namespace lzc
#endif
