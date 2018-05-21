#include "thread.hpp"

#include <thread>

#include "cuckoo_sniffer.hpp"
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
        DataEvent *data = queue->dequeue();

//        LOG_DEBUG << "Thread " << std::this_thread::get_id() << " get collected data. " << data->size_;

        auto sniffer = data->sniffer_;

        switch (data->type_) {
            case DataType::CLIENT_PAYLOAD:
                sniffer->on_client_payload(data->payload_, data->size_);
                break;
            case DataType::SERVER_PAYLOAD:
                sniffer->on_server_payload(data->payload_, data->size_);
                break;
            case DataType::CLOSE:
                sniffer->on_connection_close();
                break;
            case DataType::TEMINATATION:
                sniffer->on_connection_terminated(0);
                break;
        }
        delete data;
        return 1;
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
