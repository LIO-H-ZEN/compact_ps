#ifndef LZC_UTILS_BUFFER_H
#define LZC_UTILS_BUFFER_H

#include "Common.h"
#include "NoncopyableObject.h"

namespace lzc {
class basic_buffer : public noncopyable_obj {
public:
    explicit basic_buffer() {
        _start = new char[_capacity];  
        _end = _cur = _start; 
    }

    explicit basic_buffer(size_t capacity) : _capacity(capacity) {
        _start = new char[_capacity];  
        _end = _cur = _start; 
    }

    void clear() {
        _start = nullptr;
        _cur = _start;
        _end = _start;
    }

    ~basic_buffer() {
        if (_start) {
            delete _start;
            _start = nullptr;
            _capacity = 0;
        }
    }

    size_t size() {
        return _cur - _start;
    }

    char *buffer() {
        return _start;
    }

    bool can_forward(size_t lens) {
        if (_cur - _start + lens < _capacity) {
            return true;
        } else {
            return false;
        }
    } 

    bool can_backward(size_t lens) {
        if (_cur - (char*)(_start + lens) >= 0) {
            return true;
        } else {
            return false;
        }
    }

    void forward(size_t lens) {
        _cur += lens;
    }

    void backward(size_t lens) {
        _cur -= lens;
    }    

    char* get_cursor() {
        return _cur;
    }

private:
    size_t _capacity = 8196;
    char* _start;
    char* _end;
    char* _cur;
}; // class basic_buffer

class binary_buffer : public basic_buffer {
public:
    explicit binary_buffer(size_t capacity) : basic_buffer(capacity) {}

#define BUFFER_OPERATOR_OVERRIDE(X)                            \
    void operator<<(const X &x) {                                    \
        put_raw<X>(x, sizeof(X));                                     \
    }                                                                \
    void operator>>(X &x) {                                          \
        get_raw<X>(x, sizeof(X));                                     \
    }                                                   
    BUFFER_PATTERN_5(int16_t, uint16_t, int32_t, uint32_t, int64_t);
    BUFFER_PATTERN_2(float, double);
    BUFFER_PATTERN_1(bool);
    BUFFER_PATTERN_1(byte_t);
    BUFFER_PATTERN_1(size_t);
#undef BUFFER_OPERATOR_OVERRIDE

private:
    template <typename T>
    void put_raw(const T &t, size_t size) {
        CHECK(can_forward(size));
        memcpy(get_cursor(), &t, size);
        forward(size);
    }

    template <typename T>
    void get_raw(T &t, size_t size) {
        CHECK(can_backward(size));
        memcpy(&t, get_cursor() - size, size);
        backward(size);
    }
}; // class binary_buffer
}; // namespace lzc
#endif
