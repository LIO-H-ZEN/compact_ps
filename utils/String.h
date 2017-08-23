#ifndef LZC_UTILS_STRING_H
#define LZC_UTILS_STRING_H

#include "Common.h"

namespace lzc {

void trim(std::string &str) {
    if (str.empty()) {
        return ;
    }
    size_t st = str.find_first_not_of(" \t\r\n");
    size_t ed = str.find_last_not_of(" \t\r\n");
    str.erase(0, st);
    str.erase(ed - st + 1);
}

std::vector<std::string> split(std::string &str, const std::string &sep) {
    std::vector<std::string> ret;
    size_t pos = str.find_first_of(sep);
    if (pos == std::string::npos) {
        ret.push_back(str);
        return std::move(ret);
    } 
    size_t lens = str.size() - 1;
    size_t st = 0;
    while (pos < lens && pos != std::string::npos) {
        ret.push_back(str.substr(st, pos - st));
        st = pos + 1;
        pos = str.find_first_of(sep, st);
    }
    if (pos == lens) {
        ret.push_back(str.substr(st, pos - st));
        ret.push_back(std::string(""));
    } else if (pos == std::string::npos) {
        ret.push_back(str.substr(st, lens - st + 1));
    }
    return std::move(ret);
}

std::pair<std::string, std::string> kv_split(std::string &str, const std::string &sep) {
    std::pair<std::string, std::string> ret;
    size_t pos = str.find_first_of(sep);
    size_t lens = str.size() - 1;
    if (pos == std::string::npos || pos == lens) {
        ret.first = str;
        ret.second = std::string("");
    } else {
        ret.first = str.substr(0, pos);
        ret.second = str.substr(pos + 1, lens - pos);
    }
    return std::move(ret);
}

bool headswith(const std::string &str, const std::string &head) {
    size_t lens = head.size();
    if (str.substr(0, lens) == head) {
        return true;
    } else {
        return false;
    }
}

template<typename... ARGS>
void format_string(std::string &ret, const std::string &format, ARGS... args) {
    const char* cf = format.c_str();
    size_t prefix_len = ret.size();
    int args_len = snprintf(NULL, 0, cf, args...);
    ret.resize(prefix_len + args_len + 1);
    CHECK(snprintf(&ret[prefix_len], prefix_len + args_len, cf, args...) == args_len);
    ret.resize(prefix_len + args_len);
}

}; // namespace lzc
#endif
