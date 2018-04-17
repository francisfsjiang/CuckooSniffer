#include "thread.hpp"

#include <thread>

#include "cuckoo_sniffer.hpp"
#include "base/data_processor.hpp"
#include "base/collected_data.hpp"
#include "util/file.hpp"
#include "threads/data_queue.hpp"

namespace cs::threads{

    using namespace cs::base;
    using namespace cs::util;


    void thread_init() {
        init_log_in_thread();
    }

    int thread_process() {
        try {
            CollectedData* data = DATA_QUEUE.dequeue();

            LOG_DEBUG << "Thread " << std::this_thread::get_id() <<" get collected data.";

            ProcessorFunc f = ProcessorRouter[data->get_data_type()];

            LOG_DEBUG << (int)(ProcessorRouter.find(data->get_data_type()) != ProcessorRouter.end());

            std::vector<cs::util::File*> v = f(data);

            return 1;
        }
        catch(std::exception& e) {
            LOG_ERROR << "Thread got exception";
            return 0;
        }
    }

    void thread_loop() {
        thread_init();
//    LOG_INFO << "Thread loop start.";
        int ret;
        while(1) {
            ret = thread_process();
            if(!ret)  {
                break;
            }
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
