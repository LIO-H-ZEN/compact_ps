#ifndef LZC_TRANSFER_HANDLER_H
#define LZC_TRANSFER_HANDLER_H

#include "TFcommon.h"

namespace lzc {

// 记录了msg_id和对应的handler的map封装的相关api

template <typename HANDLER>
class handler_map : public noncopyable_obj {
public:

    // all api is thread-safe

    void add(size_t id, HANDLER h) {
        wlock_guard wg(&_lock);
        PCHECK(true == _handler_map.emplace(id, h)) << "callback should be registerd only once";  
    }

    void remove(size_t id) {
        wlock_guard wg(&_lock);
        auto search = _handler_map.find(id);
        PCHECK(search != _handler_map.end());
        _handler_map.erase(id);
    }

    HANDLER &get(size_t id) {
        rlock_guard rg(&_lock);
        return _handler_map[id];
    }
private:
    rwlock _lock;
    std::unordered_map<size_t, HANDLER> _handler_map; 
}; // class msg2handler
}; // namespace lzc
#endif
