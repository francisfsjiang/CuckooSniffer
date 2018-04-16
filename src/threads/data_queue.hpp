#ifndef CUCKOOSNIFFER_THREADS_DATA_QUEUE_HPP
#define CUCKOOSNIFFER_THREADS_DATA_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <base/data_processor.hpp>

namespace cs::base {
    enum class DataType;

    class CollectedData;

}
namespace cs::threads {


    class DataQueue
    {
    public:
        DataQueue();

        ~DataQueue();

        void enqueue(base::CollectedData*);

        base::CollectedData* dequeue();

    private:
        std::queue<base::CollectedData*> queue_;
        mutable std::mutex mutex;
        std::condition_variable condition_var_;
    };


}

#endif //CUCKOOSNIFFER_DATA_QUEUE_HPP
