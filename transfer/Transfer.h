#ifndef LZC_TRANSFER_TRANSFER_H
#define LZC_TRANSFER_TRANSFER_H

#include "TFcommon.h"
#include "Message.h"

namespace lzc {

template <typename ROUTE>
class transfer : public listener {
public:
    typedef std::function<void(package &request, package &response)> request_handler_t;  // response is set in handler
    typedef std::function<void(package &response)> response_callback_t; 

    explicit transfer() : _route(global_route()) {
        set_zmq_ctx(_route);        
    }

    void set_node_id(int client_id) {
        _node_id = node_id;
    }

    void init_asynexec(int thread_num) {
        _exec.set_thread_num(thread_num);        
        _exec.open();
    }

    void main_loop() noexcept {
        package recv_pkg; 
        package_parser parser;
        for (;;) {
            {
                std::lock_guard<std::mutex> lg(_recv_mutex);
                // meta first, msg second
                ignore_signal_call(zmq_msg_recv, &(recv_pkg.meta().get_msg()), recv_socket(), ZMQ_DONTWAIT);  
                parser.set_meta(recv_pkg.meta());
                if (parser.get_msg_size() <= 0) {
                    LOG(INFO) << "received an empty msg...exit main loop...";
                    break;
                }
                PCHECK(1 == zmq_msg_more(recv_pkg.meta().get_msg()));
                ignore_signal_call(zmq_msg_recv, &(recv_pkg.msg().get_msg()), recv_socket(), ZMQ_DONTWAIT);  
                PCHECK(0 == zmq_msg_more(recv_pkg.msg().get_msg()));
                parser.set_msg(recv_pkg.msg());
            }
            if (parser.get_msg_type()) {
                LOG(INFO) << "receive a request";
                handle_request(parser);
            } else {
                LOG(INFO) << "receive a response";
                handle_response(parser);
            }
        } 
        LOG(WARNING) << "main_loop done and exit...";
    }

    // send
    void send(int dest_id, message_meta &&meta, message &&msg, response_callback_t &&rep_cb) noexcept {
        // set meta. 
        // msg_type等需要外层设置好
        size_t msg_id = ++_msg_id_cnt;  // _msg_id_cnt is atomic
        meta.set_msg_id(msg_id);
        PCHECK(_node_id >= 0) << "node id must be set before send"; 
        meta.set_node_id(_node_id);
        meta.set_msg_size(msg.size());
        
        // cache response callback 
        // rwlock
        {
            wlock_guard wg(_rwl);
            PCHECK(_rep_callback_map.emplace(msg_id, rep_cb).second);
        }
        
        // send 
        {
            std::lock_guard<std::mutex> lg(_route.get_mutex(dest_id)); // dest_id 要被发送的地址   
            package p;
            p.set_meta(meta);
            p.set_msg(msg);            
            ignore_signal_call(zmq_msg_send, &(p.meta().get_msg()), _route.get_socket(dest_id), ZMQ_SNDMORE);
            ignore_signal_call(zmq_msg_send, &(p.msg().get_msg()), _route.get_socket(dest_id), ZMQ_DONTWAIT);
        }
    }

    void handle_request(package &request) noexcept {
        package_parser parser(request);
        size_t handler_id = parser.get_handler_id(); 
        request_handler_t handler = _req_handler_map.get(handler_id);
        task_t task = [&] {
            package response;
            handler(request, response);  // handler需要构造好response
            // send
            package_parser response_parser(response);
            if (response_parser.get_msg_size() > 0) {
                send_response(std::move(response), response_parser.get_node_id());
            } else {
                LOG(INFO) << "not need to send response ";
            }    
        }; 
        _exec.push(task); 
    }

    void send_response(package &&response, size_t dest_id) noexcept {
        PCHECK(global_route().count(dest_id, basic_route::ROUTE_SOCKET)) << "dest_id not in route";
        auto socket = global_route().get_socket();
        ignore_signal_call(zmq_msg_send, &(response.meta().get_msg()), _route.get_socket    (dest_id), ZMQ_SNDMORE);
        ignore_signal_call(zmq_msg_send, &(response.msg().get_msg()), _route.get_socket(dest_id), ZMQ_DONTWAIT);
    }
    
    void handle_response(package &response) noexcept {
        package_parser parser(response);
        ponse_callback_t rct;
        size_t msg_id = parser.get_msg_id();
        {
            rlock_guard rg(_rwl); 
            auto search = _rep_callback_map.find(msg_id);
            PCHECK(search != _rep_callback_map.end()) << "not found response callback func..";
            rct = std::move(search -> second); 
            _rep_callback_map.erase(search); 
        }
        task_t task = [&]() {
            rct(response); 
        }; 
        _exec.push(task);
    }

private:
    ROUTE &_route;
    int _node_id = -2;
    atomic<size_t> _msg_id_cnt = 0;   // global msg id cnt

    std::unordered_map<int, response_callback_t> _rep_callback_map; // (msg_id, response_callback)
    rwlock _rwl;
   
    // thread-safe
    handler_map<request_handler_t> _req_handler_map; // (handler_id, request_handler) 
    std::mutex _recv_mutex;    

    asynexec<message, message> _exec; 
}; // class transfer
}; // namespace lzc
#endif
