#ifndef LZC_TRANSFER_TRANSFER_H
#define LZC_TRANSFER_TRANSFER_H

#include "TFcommon.h"
#include "Message.h"

namespace lzc {

class transfer : public listener {
public:

    void main_loop() {
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
private:
    std::mutex _recv_mutex;    
}; // class transfer
}; // namespace lzc
#endif
