#include <limits.h>
#include "../../utils/Common.h"
#include "../../utils/SpinLock.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_spinlock_h, all_test) {
    spinlock sl;
    std::vector<std::thread> vec;
    for (int i = 0; i < 10; ++i) {
        vec.emplace_back([&](int ind){
            sl.lock();
            std::cout << "thread:" << ind << std::endl;
            std::cout << ind << "ends" << std::endl;
            sl.unlock(); 
        }, i);
    } 
    for (int i = 0; i < 10; ++i) {
        vec[i].join();
    }
}
}; // namespace lzc
