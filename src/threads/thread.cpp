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

//        LOG_DEBUG << "Thread get collected data.";

        cs::base::DataProcessor* processor =
                cs::base::get_data_processor_by_data_type(
                        collected_data->get_data_type()
                );

        processor -> process(collected_data);

        delete collected_data;
    }
    catch(std::exception(e)) {
//        LOG_ERROR << "Thread got exception";
    }
}

void thread_loop() {
    thread_init();
//    LOG_INFO << "Thread loop start.";
    while(1) {
        thread_process();
    }

}

std::vector<std::thread> threads_vec;

void start_threads(int threads_num) {

    for (int i = 0; i < threads_num; ++i) {
        threads_vec.push_back(
                std::thread(thread_loop)
        );
    }


}

}
}