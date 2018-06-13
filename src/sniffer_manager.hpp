#ifndef CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
#define CUCKOOSNIFFER_SNIFFER_MANAGER_HPP

#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <set>
#include <queue>


namespace cs::base {
    class TCPSniffer;
}

namespace cs {

    typedef uint32_t FlowKeyType;
    typedef std::shared_ptr<cs::base::TCPSniffer> SnifferPtr;
    typedef std::pair<int, FlowKeyType> PQueueType;

    class SnifferManager {

    public:
        SnifferManager();
        ~SnifferManager();


        SnifferPtr find(FlowKeyType);

        void insert(FlowKeyType, SnifferPtr);

        void close_and_erase(FlowKeyType);

        void handle_time_out();

    private:

        void put_into_bucket();

        const int TIMEOUT_VAL = 60;


        std::map<FlowKeyType, std::pair<SnifferPtr, int>> sniffer_container_;

        std::function<bool(const PQueueType& left, const PQueueType& right)> cmp_ =
                [](const PQueueType& left, const PQueueType& right) { return left.first > right.first;};
        std::priority_queue<PQueueType, std::vector<PQueueType>, decltype(cmp_)> timeout_queue_;

        int get_current_time();

    };

}


#endif //CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
