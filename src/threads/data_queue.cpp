#include "threads/data_queue.hpp"

#include "cuckoo_sniffer.hpp"

namespace cs {
namespace threads {

DataQueue::DataQueue()
        : queue_()
        , mutex()
        , condition_var_()
{}

DataQueue::~DataQueue()
{}

void DataQueue::enqueue(cs::base::CollectedData* t)
{
    std::lock_guard<std::mutex> lock(mutex);
    queue_.push(t);
    LOG_TRACE << "Data_queue size: " << queue_.size();
    condition_var_.notify_one();
}

cs::base::CollectedData* DataQueue::dequeue()
{
    std::unique_lock<std::mutex> lock(mutex);
    while(queue_.empty())
    {
        condition_var_.wait(lock);
    }
    if (!queue_.empty()) {
        cs::base::CollectedData* val = queue_.front();
        queue_.pop();
        LOG_TRACE << "Data_queue size: " << queue_.size();
        return val;
    }
    else {
        return nullptr;
    }
}

}
}
