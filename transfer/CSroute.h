#ifndef LZC_TRANSFER_CSROUTE_H
#define LZC_TRANSFER_CSROUTE_H

#include "Route.h"
#include "TFcommon.h"

namespace lzc {

class csroute : public basic_route {
public:

    static csroute &global_route() {
        static csroute route;
        return route;
    }    

    void add_node(bool is_server, std::string &addr) {
        wlock_guard wg(_lock); 
        int id = -1;
        if (is_server) {
            id = ++_server_num;
            _server_ids.push_back(id);
        } else {
            id = id_max_range - ++_client_num;
            _client_ids.push_back(id);
        }
        CHECK_GE(id, 0);
        add_node(id, addr); // TODO server和client这个地方都要去connect吗？
    }

    int client_num() {
        rlock_guard rg(_lock);
        return _client_num;
    }
    
    int server_num() {
        rlock_guard rg(_lock);
        return _server_num;
    }

    std::vector<int> &server_ids() {
        rlock_guard rg(_lock);
        return _server_ids; 
    }

    std::vector<int> &client_ids() {
        rlock_guard rg(_lock);
        return _client_ids; 
    }

private:
    std::vector<int> _server_ids;
    std::vector<int> _client_ids;
    int _server_num = 0;
    int _client_num = 0;
    const int id_max_range = std::numeric_limits<int>::max();
    rwlock _lock; 
}; // class csroute
}; // namespace lzc
#endif
