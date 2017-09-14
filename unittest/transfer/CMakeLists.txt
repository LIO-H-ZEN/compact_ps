link_libraries("${LINK_LIB_PREFIX}" gtest gtest_main glog zmq gflags boost_system mpi "${LINK_LIB_SUFFIX}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")

add_executable(ut_tfcommon_test ut_tfcommon.cpp)
add_executable(ut_tfcommon2_test ut_tfcommon2.cpp)
