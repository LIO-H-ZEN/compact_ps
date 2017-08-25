#ifndef LZC_CPS_UTILS_COMMON_H
#define LZC_CPS_UTILS_COMMON_H

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <cstdio>
#include <stdint.h>
#include <string>
#include <utility>

#include "glog/logging.h"
#include "mpi.h"

// typedef 
typedef unsigned char byte_t;

// macro
#define BUFFER_PATTERN_1(X) BUFFER_OPERATOR_OVERRIDE(X)
#define BUFFER_PATTERN_2(X, args...) BUFFER_OPERATOR_OVERRIDE(X) BUFFER_PATTERN_1(args)
#define BUFFER_PATTERN_3(X, args...) BUFFER_OPERATOR_OVERRIDE(X) BUFFER_PATTERN_2(args)
#define BUFFER_PATTERN_4(X, args...) BUFFER_OPERATOR_OVERRIDE(X) BUFFER_PATTERN_3(args)
#define BUFFER_PATTERN_5(X, args...) BUFFER_OPERATOR_OVERRIDE(X) BUFFER_PATTERN_4(args)
#define BUFFER_PATTERN_6(X, args...) BUFFER_OPERATOR_OVERRIDE(X) BUFFER_PATTERN_5(args)

#endif
