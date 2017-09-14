#ifndef LZC_TRANSFER_MESSAGE_H
#define LZC_TRANSFER_MESSAGE_H

#include "TFcommon.h" 

namespace lzc {

struct message_meta {
    size_t _node_id;   // find its addr/socket in route   TODO when set this ? (when sending?)
    size_t _handler_id;    // find its handler in handler_map
    size_t _msg_id;    // unique id for every msg
    size_t _msg_size;     // set it to _msg.size();           
    bool _is_request = true;  // request or response 

    void set_node_id(size_t node_id) { _node_id = node_id;}
    void set_handler_id(size_t handler_id) { _handler_id = handler_id;}
    void set_msg_id(size_t msg_id) { _msg_id = msg_id;}
    void set_msg_size(size_t msg_size) { _msg_size = msg_size;}
    void set_msg_type(bool is_request) { _is_request = is_request;}
}; // struct message_meta

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
    message(const char *buf, size_t size) {
        PCHECK(0 == zmq_msg_init_size(&_msg, size)); 
        memcpy(msg_data(), buf, size);
    }

    message(message_meta &mm) {
        PCHECK(0 == zmq_msg_init_size(&_msg, sizeof(message_meta)));
        memcpy(msg_data(), (char *)&mm, sizeof(message_meta));
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

    void reset(message_meta &mm) {
        CHECK(0 == zmq_msg_close(&_msg));
        PCHECK(0 == zmq_msg_init_size(&_msg, sizeof(message_meta)));
        memcpy(msg_data(), (char *)&mm, sizeof(message_meta));
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

    zmq_msg_t &get_msg() {
        return _msg;
    }

private:
    zmq_msg_t _msg;
    // ref: http://api.zeromq.org/4-1:zmq-msg-init-data
    // Example
    static void my_free (void *data, void *hint) {
        free(data);
    }   
}; // class message

// package meta and msg
class package {
public:
    explicit package() {}

    void set_meta(message_meta &mm) {
        _meta.reset(mm); 
    }

    void set_msg(binary_buffer &bb) {
        _msg.reset(std::move(bb));
    }

    message &meta() {
        return _meta;
    }

    message &msg() {
        return _msg;
    }

private:
    message _meta;
    message _msg;    
}; // class package

class package_parser {
public:
    explicit package_parser() {}

    explicit package_parser(package &&p) {
        memcpy(&meta, p.meta().msg_data(), sizeof(message_meta));        
        memcpy(bb.buffer(), p.msg().msg_data(), p.msg().size());        
    }

    void set_meta(message &m) {
        memcpy(&meta, m.msg_data(), sizeof(message_meta));        
    }

    void set_msg(message &m) {
        memcpy(bb.buffer(), m.msg_data(), m.size());        
    }
   
    // get meta attrs 
    size_t get_node_id() {return meta._node_id;}
    size_t get_handler_id() {return meta._handler_id;}
    size_t get_msg_id() {return meta._msg_id;}
    size_t get_msg_size() {return meta._msg_size;}
    bool get_msg_type() {return meta._is_request;}
private:
    message_meta meta;
    binary_buffer bb; // msg
}; // class package_parser
}; // namespace lzc
#endif
