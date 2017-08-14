#include <limits.h>
#include "../../utils/queue.h"
#include "../../utils/common.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_queue_h, ts_queue_with_capacity) {
    ts_queue_with_capacity que(1);
    int ind1 = 1;
    int ind2 = 1;
    std::thread t1([&]{
        for(; ind1 < 10000; ++ind1)
            EXPECT_TRUE(que.push(ind1));
    });
    std::thread t2([&] {
        for(; ind2 < 10000; ++ind2) {
            int t;
            que.wait_pop(t);
            EXPECT_EQ(ind1 - 1, t);
        }
    });    
    t1.join();
    t2.join();
}

}; // namespace lzc
