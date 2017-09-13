#include <limits.h>
#include "../../transfer/TFcommon.h"
#include "../../utils/EnvUtil.h"
#include "../../utils/Mpi.h"
#include "gtest/gtest.h"
namespace lzc {

// use mpirun -n 2 to test this below
TEST(ut_tfcommon_h, zmq_bind_random_port) {
    CPS_MPI::singleton().initialize(0, NULL);
    using namespace std::chrono_literals;
    std::string ip = get_local_ip(); 
    void *zmq_ctx = zmq_ctx_new();
    void *socket = zmq_socket(zmq_ctx, ZMQ_PULL);
    std::string addr("tcp://");
    int port;
    if (CPS_MPI::singleton().comm_rank() > 0) {
        std::this_thread::sleep_for(1s);
    }
    zmq_bind_random_port(ip, socket, addr, port);
    std::cout << addr << std::endl; 
    std::cout << port << std::endl; 
    std::this_thread::sleep_for(4s);
    auto res = split(addr, ":");
    EXPECT_EQ(atoi(res[2].c_str()), port); 
    zmq_close(socket);
    zmq_ctx_destroy(zmq_ctx);
    CPS_MPI::singleton().finalize();
}

}; // namespace lzc
