#include "thread.hpp"

#include <thread>

#include "cuckoo_sniffer.hpp"
#include "base/collected_data.hpp"
#include "base/data_processor.hpp"

#include "threads/data_queue.hpp"

namespace cs {
namespace threads{



void thread_init() {
    init_log_in_thread();
}

void thread_process() {
    try {
        cs::base::CollectedData* collected_data = DATA_QUEUE.dequeue();


        delete collected_data;
    }
    catch(std::exception(e)) {
        LOG_ERROR << "Thread got exception";
    }
}

void thread_loop() {
    thread_init();
    LOG_INFO << "Thread loop start.";
    while(1) {
        thread_process();
    }

}


void start_threads(int) {

}

}
}