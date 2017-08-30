#include <limits.h>
#include "../../utils/Common.h"
#include "../../utils/RWlock.h"
#include "../../utils/AsynExec.h"
#include "../../utils/Queue.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_rwlock_h, all_test) {
    using namespace std::chrono_literals;
    asynexec<int, int> exec(10);
    exec.open();

    rwlock lock;
    for (int i = 0; i < 11; ++i) {
        exec.push_input(i);
    }  

    task_t f = [&]() {
        int flag = -1;
        exec.wait_pop_input(flag);
        if (flag == 5) {
            wlock_guard wg(lock);
            std::cout << "write" << std::endl;
            std::this_thread::sleep_for(2s);
        } else {
            rlock_guard rg(lock);
            std::cout << "read" << std::endl;
            std::this_thread::sleep_for(2s);
        }
    };
    exec.feed(f);
    exec.close();
}
}; // namespace lzc
