#ifndef LZC_UTILS_NONCOPYABLE_OBJECT_H
#define LZC_UTILS_NONCOPYABLE_OBJECT_H

namespace lzc {
class noncopyable_obj {
public:
    noncopyable_obj() = default;
    noncopyable_obj(const noncopyable_obj &) = delete;
    noncopyable_obj &operator=(const noncopyable_obj &) = delete;
    ~noncopyable_obj() = default;
}; // class noncopyable_object

}; // namespace lzc
#endif
