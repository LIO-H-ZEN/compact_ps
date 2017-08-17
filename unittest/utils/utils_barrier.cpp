#include <limits.h>
#include "../../utils/common.h"
#include "../../utils/queue.h"
#include "../../utils/barrier.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_barrier_h, cnt_barrier) {
    std::vector<std::thread> ts(5);
    threadsafe_queue<int> que;
    cnt_barrier cb(5);
    for (int i = 0; i < 4; ++i) {
        ts[i] = std::thread([&](int i){
            std::cout << "thread id: " << i << " started" << std::endl;
            cb.block();
            que.push(i);
            std::cout << "thread id: " << i << " ended" << std::endl;
        }, i);
    }
    ts[4] = std::thread([&]{
        std::cout << "thread id: " << 4 << " started" << std::endl;
        que.push(4);
        cb.block();
        std::cout << "thread id: " << 4 << " ended" << std::endl;
    });
    int first = -1;
    que.wait_pop(first);
    EXPECT_EQ(4, first);    
    for (int i = 0; i < 5; ++i) {
        ts[i].join();
    }
}

TEST(utils_barrier_h, barrier) {
    std::vector<std::thread> ts(5);
    threadsafe_queue<int> que;
    barrier b;
    for (int i = 0; i < 4; ++i) {
        ts[i] = std::thread([&](int i){
            std::cout << "thread id: " << i << " started" << std::endl;
            b.block();
            que.push(i);
            std::cout << "thread id: " << i << " ended" << std::endl;
        }, i);
    }
    ts[4] = std::thread([&]{
        std::cout << "thread id: " << 4 << "started" << std::endl;
        que.push(4);
        b.unblock();
        std::cout << "thread id: " << 4 << "ended" << std::endl;
    });
    int first = -1;
    que.wait_pop(first);
    EXPECT_EQ(4, first);    
    for (int i = 0; i < 5; ++i) {
        ts[i].join();
    }
}

TEST(utils_barrier_h, udf_barrier) {
    std::vector<std::thread> ts(5);
    threadsafe_queue<int> que;
    udf_barrier b;
    bool ret = false;
    for (int i = 0; i < 4; ++i) {
        ts[i] = std::thread([&](int i){
            std::cout << "thread id: " << i << " started" << std::endl;
            b.block([&]{std::cout << i << " is block..." << std::endl;}, [&]{return ret;});
            que.push(i);
            std::cout << "thread id: " << i << " ended" << std::endl;
        }, i);
    }
    ts[4] = std::thread([&]{
        std::cout << "thread id: " << 4 << "started" << std::endl;
        que.push(4);
        b.unblock([&]{ret = true;});
        std::cout << "thread id: " << 4 << "ended" << std::endl;
    });
    int first = -1;
    que.wait_pop(first);
    EXPECT_EQ(4, first);    
    for (int i = 0; i < 5; ++i) {
        ts[i].join();
    }
}

}; // namespace lzc
