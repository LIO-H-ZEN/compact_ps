#ifndef LZC_TRANSFER_TFCOMMON_H
#define LZC_TRANSFER_TFCOMMON_H

#include <zmq.h>
#include <unordered_map>

#include "glog/logging.h"

#include "../utils/NoncopyableObject.h"
#include "../utils/Buffer.h"

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

#endif
