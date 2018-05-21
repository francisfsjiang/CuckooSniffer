#ifndef CUCKOOSNIFFER_THREADS_DATA_QUEUE_HPP
#define CUCKOOSNIFFER_THREADS_DATA_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

#include "cuckoo_sniffer.hpp"

namespace cs::base {

    class Sniffer;
    class TCPSniffer;

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
        DataEvent(std::shared_ptr<cs::base::TCPSniffer>, DataType, cs::base::PayloadVector, size_t);
        ~DataEvent();
        std::shared_ptr<cs::base::TCPSniffer> sniffer_;
        DataType type_;
        cs::base::PayloadVector payload_;
        size_t size_;

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
