#include "threads/data_queue.hpp"

#include "cuckoo_sniffer.hpp"

namespace cs::threads {
    using namespace cs::util;
    using namespace cs::base;

    DataQueue::DataQueue()
            : queue_()
            , mutex()
            , condition_var_()
    {}

    DataQueue::~DataQueue()
    {}

    void DataQueue::enqueue(CollectedData* data)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue_.push(data);
        LOG_TRACE << "Data_queue size: " << queue_.size();
        condition_var_.notify_one();
    }

    CollectedData* DataQueue::dequeue()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while(queue_.empty())
        {
            condition_var_.wait(lock);
        }
        if (!queue_.empty()) {
            CollectedData* data = queue_.front();
            queue_.pop();
            LOG_TRACE << "Data_queue size: " << queue_.size();
            return data;
        }
        else {
            return {};
        }
    }

}
