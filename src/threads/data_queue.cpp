#include "threads/data_queue.hpp"

#include "cuckoo_sniffer.hpp"

namespace cs::threads {
    using namespace cs::base;

    std::vector<DataQueue*> DATA_QUEUES;

    DataEvent::DataEvent(std::shared_ptr<cs::base::Sniffer> sniffer, DataType type, cs::base::payload_type* payload)
            :sniffer_(sniffer), type_(type), payload_(payload)
    {

    }

    DataQueue::DataQueue()
            : queue_()
            , mutex()
            , condition_var_()
    {}

    DataQueue::~DataQueue()
    {}

    void DataQueue::enqueue(DataEvent* data)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue_.push(data);
        LOG_TRACE << "Data_queue size: " << queue_.size();
        condition_var_.notify_one();
    }

    DataEvent* DataQueue::dequeue()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while(queue_.empty())
        {
            condition_var_.wait(lock);
        }
        if (!queue_.empty()) {
            DataEvent* data = queue_.front();
            queue_.pop();
            LOG_TRACE << "Data_queue size: " << queue_.size();
            return data;
        }
        else {
            return {};
        }
    }

}
