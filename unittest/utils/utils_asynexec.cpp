#include <limits.h>
#include "../../utils/Common.h"
#include "../../utils/AsynExec.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_asynexec_h, only_input_channel) {
    int tn = 3;
    asynexec<int, int> exec(tn);
    exec.open();

    for (int i = 0; i <= 1000; ++i) {
        exec.push_input(i);
    }

    task_t task = [&]{
        int ret = 0;
        while (!exec.input_empty()) {
            int tmp = 0;
            exec.wait_pop_input(tmp);
            ret += tmp;
        }
        exec.push_output(ret);
    };
    exec.feed2all(task);

    exec.close();
    
    int tmp = -1;
    int res = 0;
    for (int i = 0; i < tn; ++i) {
        exec.wait_pop_output(tmp);
        std::cout << tmp << std::endl;
        res += tmp; 
    }
    EXPECT_EQ(500500, res);
}

}; // namespace lzc
