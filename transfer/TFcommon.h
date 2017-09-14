#ifndef LZC_TRANSFER_TFCOMMON_H
#define LZC_TRANSFER_TFCOMMON_H

#include <zmq.h>
#include <unordered_map>

#include "glog/logging.h"

#include "../utils/Common.h"
#include "../utils/NoncopyableObject.h"
#include "../utils/Buffer.h"
#include "../utils/RWlock.h"
#include "../utils/String.h"
#include "../utils/EnvUtil.h"
#include "../utils/SpinLock.h"
#include "../utils/AsynExec.h"
#include "../utils/EnvUtil.h"
#include "../utils/Mpi.h"

namespace lzc {

// 忽略异常每次都继续执行
template <typename FUNC, typename... ARGS>
void ignore_signal_call(FUNC func, ARGS &&... args) {
    for (;;) {
        auto ret = func(args...);
        if (ret < 0 && errno == EINTR) {
            LOG(INFO) << "ignore_signal_call ignored a error";
            continue;
        }
        break;
    }
}
 
void zmq_bind_random_port(std::string &ip, void *socket, std::string &addr, int &port) {
    std::string addr_ori(addr);
    for(;;) {
        addr = addr_ori;
        port = 1024 + rand() % (65536 - 1024); 
        format_string(addr, "%s:%d", ip.c_str(), port);
        if (-1 == zmq_bind(socket, addr.c_str())) {
            LOG(INFO) << "binding " << addr << " failed";
            LOG(INFO) << "errno: " << errno << " zmq_strerror:" << zmq_strerror(errno);
            continue;
        }
        break;
    } 
}

void zmq_push_once(const std::string &addr, void *zmq_ctx, zmq_msg_t *msg) {
    void *socket = zmq_socket(zmq_ctx, ZMQ_PUSH);
    CHECK(socket != NULL);
    ignore_signal_call(zmq_connect, socket, addr.c_str());
    ignore_signal_call(zmq_msg_send, msg, socket, 0);
    zmq_close(socket);
    LOG(INFO) << "zmq send once finished !";
}
}; // namespace lzc
#endif
