#ifndef LZC_UTILS_MPI_H
#define LZC_UTILS_MPI_H

#include "NoncopyableObject.h"
#include "Env_util.h"
#include "Common.h"

namespace lzc {

class CPS_MPI : public noncopyable_obj {
public:
    // not thread safe
    static CPS_MPI &singleton() {
        static CPS_MPI cms;
        return cms;
    }

    void initialize(int argc, char** argv) {
        MPI_Init(&argc, &argv);
        CHECK(0 == MPI_Comm_size(MPI_COMM_WORLD, &_comm_size));
        CHECK(0 == MPI_Comm_rank(MPI_COMM_WORLD, &_comm_rank));
        
        int IP_MAX_LEN = 64;
        _ip_table.assign(IP_MAX_LEN * _comm_size, 0);
        std::string ip = get_local_ip();
        strcpy(&_ip_table[_comm_rank * IP_MAX_LEN], ip.c_str());
        CHECK(0 == MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, &_ip_table[0], IP_MAX_LEN, MPI_BYTE, MPI_COMM_WORLD));  
    }

    int comm_size() {
        return _comm_size;
    }

    int comm_rank() {
        return _comm_rank;
    }

    std::vector<char> &ip_table() {
        return _ip_table;
    }
    
    void finalize() {
        MPI_Finalize();
    }
private:
    int _comm_size;
    int _comm_rank;
    std::vector<char> _ip_table;
private:
    CPS_MPI() {}
}; // class cps_mpi_singleton
}; // namespace lzc
#endif
