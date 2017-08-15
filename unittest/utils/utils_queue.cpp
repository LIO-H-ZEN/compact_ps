#include <limits.h>
#include "../../utils/queue.h"
#include "../../utils/common.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(ts_queue_with_capacity, push_wait_pop) {
    using namespace std::chrono_literals;
    ts_queue_with_capacity<int> que(1);
    int ind1 = 1;
    int ind2 = 1;
    std::thread t1([&]{
        for(; ind1 < 5; ++ind1)
            EXPECT_TRUE(que.push(ind1));
    });
    std::thread t2([&] {
        for(; ind2 < 5; ++ind2) {
            int t;
            std::this_thread::sleep_for(2s);
            EXPECT_EQ(1, que.size());
            que.wait_pop(t);
            EXPECT_EQ(ind2, t);
        }
    });    
    t1.join();
    t2.join();
}

TEST(ts_queue_with_capacity, close) {
    ts_queue_with_capacity<int> que(100);
    int ind1 = 1;
    int ind2 = 1;
    std::thread t1([&]{
        for (; ind1 < 5; ++ind1) {
            EXPECT_TRUE(que.push(ind1));
        }
        que.close();
        for (; ind1 < 10; ++ind1) {
            EXPECT_FALSE(que.push(ind1));
        }
    });
    std::thread t2([&] {
        int t;
        for (; ind2 < 5; ++ind2) {
            que.wait_pop(t);
            EXPECT_EQ(ind2, t);
        }
        for (; ind2 < 10; ++ind2) {
            EXPECT_FALSE(que.wait_pop(t));
        }
    });    
    t1.join();
    t2.join();
}
}; // namespace lzc
