#include "thread.hpp"

#include <thread>

#include "cuckoo_sniffer.hpp"
#include "base/data_processor.hpp"
#include "base/collected_data.hpp"
#include "base/sniffer.hpp"
#include "util/file.hpp"
#include "threads/data_queue.hpp"

namespace cs::threads{

    using namespace cs::base;
    using namespace cs::util;

    int THREADS_NUM = -1;

    void thread_init() {
        init_log_in_thread();
    }

    int thread_process(DataQueue* queue) {
//        try {
            DataEvent* data = queue->dequeue();

            LOG_DEBUG << "Thread " << std::this_thread::get_id() <<" get collected data.";

            if (data->type_ == DataType::CLIENT_PAYLOAD) {
                data->sniffer_->on_client_payload(*data->payload_);
            }
            else {
                data->sniffer_->on_server_payload(*data->payload_);
            }

            return 1;
//        }
//        catch(std::exception& e) {
//            LOG_ERROR << "Thread got exception";
//            return 0;
//        }
    }

    void thread_loop(DataQueue* queue) {
        thread_init();
//    LOG_INFO << "Thread loop start.";
        int ret;
        while(1) {
            ret = thread_process(queue);
            if(!ret)  {
                break;
            }
        }

    }

    std::vector<std::thread> THREADS_POOL;

    void start_threads(int threads_num) {
        THREADS_POOL.clear();
        THREADS_NUM = threads_num;

        for (int i = 0; i < threads_num; ++i) {
            auto p = new DataQueue();
            THREADS_POOL.emplace_back(
                    std::thread(thread_loop, p)
            );

            DATA_QUEUES.push_back(
                    p
            );
        }


    }

}
