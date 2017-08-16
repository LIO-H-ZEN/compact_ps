link_libraries("${LINK_LIB_PREFIX}" gtest gtest_main glog zmq gflags boost_system "${LINK_LIB_SUFFIX}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")

add_executable(utils_queue_test utils_queue.cpp)
add_executable(utils_barrier_test utils_barrier.cpp)
