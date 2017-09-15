#ifndef LZC_NODE_WORKER_H
#define LZC_NODE_WORKER_H

#include "../transfer/Transfer.h"

namespace lzc {

class worker {
public:
    worker() {} 

    void initialize() {
        init_transfer();
    }

    void finialize() {
        LOG(INFO) << "node " << CPS_MPI::singleton().comm_rank() << " exit normally";        
        // TODO still have other things to do ?
    }
protected:
    void init_transfer() {
        // 1、set listen thread num
        // 2、init_asynexec
        // 3、listen (finish bind)
        // 4、service start
        _transfer.set_thread_num(5); // TODO configurable
        _transfer.init_asynexec(3);  // TODO configurable 
        _transfer.listen(); // recv_port()  
        _transfer.service_start(); // 5 threads begins to run main_loop (waiting to recv msg) 
    }
private:
    transfer<csroute> _transfer;
}; // class worker
}; // namespace lzc
#endif
