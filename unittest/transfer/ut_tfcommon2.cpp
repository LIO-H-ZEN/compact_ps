#include <limits.h>
#include "../../transfer/TFcommon.h"
#include "../../transfer/Message.h"
#include "gtest/gtest.h"
namespace lzc {

// use mpirun -n 2 to test this below
TEST(ut_tfcommon_h, zmq_push_once) {
    CPS_MPI::singleton().initialize(0, NULL);
    using namespace std::chrono_literals;
    if (CPS_MPI::singleton().comm_rank() == 0) {
        std::cout << "rank 0: " << std::endl; 
        int port;
        std::string addr("tcp://");
        std::string ip = get_local_ip(); 
        void *zmq_ctx = zmq_ctx_new();
        void *socket = zmq_socket(zmq_ctx, ZMQ_PULL);
        zmq_bind_random_port(ip, socket, addr, port);
        std::cout << "rank 0 server bind on: " << addr << std::endl; 
        MPI_Send(&port, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        message msg; 
        std::cout << "server start recv..." << std::endl;
        ignore_signal_call(zmq_msg_recv, &(msg.get_msg()), socket, 0);
        std::string res(msg.msg_data());
        res.resize(5);
        std::cout << res << std::endl;
        EXPECT_EQ(res, std::string("hello"));
        zmq_close(socket);
        zmq_ctx_destroy(zmq_ctx);
        std::cout << "rank 0 ended" << std::endl;
    } else {
        std::cout << "rank 1: " << std::endl; 
        void *zmq_ctx = zmq_ctx_new();
        int port = -1;
        MPI_Recv(&port, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        std::cout << port << std::endl;
        std::string addr("tcp://");
        std::string ip = get_local_ip(); 
        format_string(addr, "%s:%d", ip.c_str(), port); 
        std::cout << "try to connect on: " << addr << std::endl;
        const char *content = "hello";
        message msg(content, 5);
        zmq_push_once(addr, zmq_ctx, &(msg.get_msg()));
        zmq_ctx_destroy(zmq_ctx);
        std::cout << "rank 1 ended" << std::endl;
    }
    CPS_MPI::singleton().finalize();
}
}; // namespace lzc
