#ifndef LZC_TRANSFER_MESSAGE_H
#define LZC_TRANSFER_MESSAGE_H

#include "TFcommon.h" 

namespace lzc {
// 基本上就是对于zmq_msg的api封装
class message {
public:
    message() {
        CHECK(0 == zmq_msg_init(&_msg));
    }
    // move
    message(basic_buffer &&bb) {
        CHECK(0 == zmq_msg_init_data(&_msg, bb.buffer(), bb.size(), my_free, NULL));
        // 切断bb对于这段内存的使用
        bb.clear();
    }
    // cp
    message(char *buf, size_t size) {
        zmq_msg_init_size(&_msg, size); 
        memcpy(msg_data(), buf, size);
    }

    ~message() {
        CHECK(0 == zmq_msg_close(&_msg));
    }  

    void reset() {
        CHECK(0 == zmq_msg_close(&_msg));
        CHECK(0 == zmq_msg_init(&_msg));
    }

    void reset(basic_buffer &&bb) {
        CHECK(0 == zmq_msg_close(&_msg));
        CHECK(0 == zmq_msg_init_data(&_msg, bb.buffer(), bb.size(), my_free, NULL)); 
        bb.clear();
    }

    size_t size() {
        return zmq_msg_size(&_msg);
    }

    bool empty() {
        return size() <= 0;
    }

    char *msg_data() {
        return (char *)zmq_msg_data(&_msg); 
    }
private:
    zmq_msg_t _msg;
    // ref: http://api.zeromq.org/4-1:zmq-msg-init-data
    // Example
    void my_free (void *data, void *hint) {
        free(data);
    }   
}; // class message
}; // namespace lzc
#endif
