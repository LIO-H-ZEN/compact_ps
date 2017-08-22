#include <limits.h>
#include "../../utils/Common.h"
#include "../../utils/AsynExec.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_asynexec_h, construct_asyn_feed) {
    std::mutex _mu;
    asynexec exec(3);
    exec.open();

    threadsafe_queue<int> que;
    for (int i = 0; i <= 1000; ++i) {
        que.push(i);
    }

    asynexec::task_t task = [&]{
        int ret = 0;
        while (!que.empty()) {
            int tmp = 0;
            que.wait_pop(tmp);
            ret += tmp;
        }
        std::lock_guard<std::mutex> lg(_mu);
        std::cout << ret << std::endl;
    };
    asyn_feed(task, exec);
}
}; // namespace lzc
