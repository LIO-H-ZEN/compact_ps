#ifndef LZC_NODE_NODE_H
#define LZC_NODE_NODE_H

#include "../transfer/CSroute.h"

namespace lzc {

class node() {

private:
    // 初始化好路由信息
    // MPI初始化的时候会allgather ip
    // 这个需要allgather port
    void init_route() {
        
    }
private:
    std::vector<int> _port_table;
}
}; // namespace lzc
#endif
