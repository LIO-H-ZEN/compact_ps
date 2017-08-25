#include <limits.h>
#include "../../utils/Common.h"
#include "../../utils/Env_util.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_env_util_h, get_local_ip_test) {
    std::cout << get_local_ip() << std::endl;
    EXPECT_EQ(get_local_ip(), std::string("172.17.0.1"));
}
}; // namespace lzc
