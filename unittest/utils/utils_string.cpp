#include <limits.h>
#include "../../utils/Common.h"
#include "../../utils/String.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_string_h, string_trim_test) {
    std::string str = " \t \r \nhello \r\tworld\n\r\t ";
    trim(str);
    EXPECT_EQ(str, std::string("hello \r\tworld"));
}

TEST(utils_string_h, string_split_test) {
    std::string s1 = "123";
    std::string s2 = "123@";
    std::string s3 = "123@456";
    std::string s4 = "123@456@";
    std::string sep = "@";
    auto r1 = split(s1, sep);
    auto r2 = split(s2, sep);
    auto r3 = split(s3, sep);
    auto r4 = split(s4, sep);
    EXPECT_EQ(r1.size(), 1);
    EXPECT_EQ(r1[0], std::string("123"));
    EXPECT_EQ(r2.size(), 2);
    EXPECT_EQ(r2[0], std::string("123"));
    EXPECT_EQ(r2[1], std::string(""));
    EXPECT_EQ(r3.size(), 2);
    EXPECT_EQ(r3[0], std::string("123"));
    EXPECT_EQ(r3[1], std::string("456"));
    EXPECT_EQ(r4.size(), 3);
    EXPECT_EQ(r4[0], std::string("123"));
    EXPECT_EQ(r4[1], std::string("456"));
    EXPECT_EQ(r4[2], std::string(""));
}

TEST(utils_string_h, string_kv_split_test) {
    std::string str = "label:1.0";
    std::string sep = ":";
    auto res = kv_split(str, sep);
    EXPECT_EQ(res.first, std::string("label"));
    EXPECT_EQ(res.second, std::string("1.0"));
}

TEST(utils_string_h, string_headswith_test) {
    std::string str = "123 5678";
    std::string head = "123 ";
    EXPECT_TRUE(headswith(str, head));
}

TEST(utils_string_h, string_format_string_test) {
    std::string prefix = "http";
    std::string format = "://%d.%d.%d.%d";
    std::vector<int> args = {1,2,3,4};
    format_string(prefix, format, args[0], args[1], args[2], args[3]);
    EXPECT_EQ(prefix, std::string("http://1.2.3.4"));
}
}; // namespace lzc
