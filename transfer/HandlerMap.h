#ifndef LZC_TRANSFER_HANDLER_H
#define LZC_TRANSFER_HANDLER_H

#include "TFcommon.h"

namespace lzc {

template <typename HANDLER>

class msg2handler : public noncopyable_obj {
public:

    // all api is thread-safe

    void add(size_t id, HANDLER h) {
        wlock_guard wg(&_lock);
        PCHECK(true == _msg_handler.emplace(id, h)) << "callback should be registerd only once";  
    }

    void remove(size_t id) {
        wlock_guard wg(&_lock);
        auto search = _msg_handler.find(id);
        PCHECK(search != _msg_handler.end());
        _msg_handler.erase(id);
    }

    HANDLER &get(size_t id) {
        rlock_guard rg(&_lock);
        return _msg_handler[id];
    }
private:
    rwlock _lock;
    std::unordered_map<size_t, HANDLER> _msg_handler; 
}; // class msg2handler
}; // namespace lzc
#endif
