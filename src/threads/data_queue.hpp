#ifndef CUCKOOSNIFFER_THREADS_DATA_QUEUE_HPP
#define CUCKOOSNIFFER_THREADS_DATA_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

namespace cs {
namespace base {

class CollectedData;

}

namespace threads {

class DataQueue
{
public:
    DataQueue();

    ~DataQueue();

    void enqueue(cs::base::CollectedData* t);

    cs::base::CollectedData* dequeue();

private:
    std::queue<cs::base::CollectedData*> queue_;
    mutable std::mutex mutex;
    std::condition_variable condition_var_;
};


}
}

#endif //CUCKOOSNIFFER_DATA_QUEUE_HPP
