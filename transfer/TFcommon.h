#ifndef LZC_TRANSFER_TFCOMMON_H
#define LZC_TRANSFER_TFCOMMON_H

#include <zmq.h>
#include <unordered_map>

#include "glog/logging.h"

#include "../utils/NoncopyableObject.h"
#include "../utils/Buffer.h"
#include "../utils/RWlock.h"
#include "../utils/String.h"
#include "../utils/EnvUtil.h"

// 忽略异常每次都继续执行
template <typename FUNC, typename... ARGS>
void ignore_signal_call(FUNC func, ARGS &&... args) {
    for (;;) {
        auto ret = func(args);
        if (ret < 0 && errno == EINTR) {
            LOG(INFO) << "ignore_signal_call ignored a error";
            continue;
        }
        break;
    }
}
 
void zmq_bind_random_port(std::string &ip, void *socket, std::string &addr, int &port) {
    for(;;) {
        addr = "";
        port = 1024 + rand() % (65536 - 1024); 
        addr = format_string(ip, ":%d", port);
        if (-1 == zmq_bind(socket, addr)) {
            continue;
        }
        break;
    } 
}
#endif
