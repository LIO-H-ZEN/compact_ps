#include <limits.h>
#include "../../utils/Common.h"
#include "../../utils/Mpi.h"
#include "gtest/gtest.h"
namespace lzc {

TEST(utils_mpi_h, initailize_test) {
    // mpirun -n 2 utils_mpi_test
    CPS_MPI::singleton().initialize(0, NULL); 
    EXPECT_EQ(CPS_MPI::singleton().comm_size(), 2); 
    auto t = CPS_MPI::singleton().ip_table();
    CPS_MPI::singleton().finalize();
    std::string ip = "172.17.0.1";    
    std::vector<char> v;
    v.assign(64 * 2, 0);
    strcpy(&v[0], ip.c_str());
    strcpy(&v[64], ip.c_str());
    for (int i = 0; i < 64*2; ++i) {
        EXPECT_EQ(v[i], t[i]);
    }
}
}; // namespace lzc
