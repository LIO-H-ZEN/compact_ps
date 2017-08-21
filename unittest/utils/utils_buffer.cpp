#include <limits.h>
#include "../../utils/common.h"
#include "../../utils/buffer.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_buffer_h, binary_buffer_test) {
    binary_buffer bb(10000);
    int i = 99;
    double d = 0.77;
    bool b = true;
    long l = 111111111111;
    bb << i;
    bb << d;
    bb << b;
    bb << l;
    int oi = -1;
    double od = -1;
    bool ob = false;
    long ol = -1;
    bb >> ol;
    bb >> ob;
    bb >> od;
    bb >> oi;
    EXPECT_EQ(oi, 99);
    EXPECT_EQ(od, 0.77);
    EXPECT_TRUE(ob);
    EXPECT_EQ(ol, 111111111111);
}
}; // namespace lzc
