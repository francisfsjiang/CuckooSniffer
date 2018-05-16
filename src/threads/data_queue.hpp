#ifndef CUCKOOSNIFFER_THREADS_DATA_QUEUE_HPP
#define CUCKOOSNIFFER_THREADS_DATA_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

#include "cuckoo_sniffer.hpp"

namespace cs::base {

    class Sniffer;

}
namespace cs::threads {


    enum class DataType: int {
        SERVER_PAYLOAD,
        CLIENT_PAYLOAD,
        CLOSE,
        TEMINATATION
    };

    class DataEvent {
    public:
        DataEvent(std::shared_ptr<cs::base::Sniffer>, DataType, cs::base::PayloadType*);
        ~DataEvent();
        std::shared_ptr<cs::base::Sniffer> sniffer_;
        DataType type_;
        cs::base::PayloadType* payload_;

    };


    class DataQueue {
    public:
        DataQueue();

        ~DataQueue();

        void enqueue(DataEvent*);

        DataEvent* dequeue();

    private:
        std::queue<DataEvent*> queue_;
        mutable std::mutex mutex;
        std::condition_variable condition_var_;
    };


    extern std::vector<DataQueue*> DATA_QUEUES;

}

#endif //CUCKOOSNIFFER_DATA_QUEUE_HPP
