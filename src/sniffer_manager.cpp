#include "sniffer_manager.hpp"

#include <sys/time.h>

#include "cuckoo_sniffer.hpp"
#include "base/sniffer.hpp"
#include "threads/data_queue.hpp"

namespace cs {

    using namespace std;
    using namespace cs::threads;

    SnifferManager::SnifferManager()
    {

    }


    SnifferManager::~SnifferManager() {

    }

    SnifferPtr SnifferManager::find(FlowKeyType key) {
        auto iter = sniffer_container_.find(key);
        if (iter == sniffer_container_.end())
            return nullptr;

        iter->second.second = get_current_time();
        return iter->second.first;
    }

    void SnifferManager::insert(FlowKeyType key, SnifferPtr ptr) {
        int id= get_current_time();
        sniffer_container_.insert(make_pair(
                key,
                make_pair(ptr, id)
        ));

    }

    void SnifferManager::handle_time_out() {
        int current_time = get_current_time();
        int out_time = current_time - TIMEOUT_VAL;
//        LOG_TRACE << "Handling timeout. Current time: " << current_time;
        while(!timeout_queue_.empty()) {
            if (timeout_queue_.top().first < out_time) {
                FlowKeyType key = timeout_queue_.top().second;
                auto sniffer_info_ptr = sniffer_container_.find(key);
                if(sniffer_info_ptr != sniffer_container_.end()) {
                    if (sniffer_info_ptr->second.second < out_time) {
                        LOG_TRACE << "Timeout, closing sniffer " << key;
                        close_and_erase(key);
                    }
                    else {
                        timeout_queue_.push(make_pair(sniffer_info_ptr->first, key));
                    }
                }
                timeout_queue_.pop();
            }
            else {
                break;
            }
        }
//        LOG_TRACE << "Handling timeout finish.";
    }

    void SnifferManager::close_and_erase(FlowKeyType key) {
        auto iter = sniffer_container_.find(key);
        if (iter == sniffer_container_.end())
            return;

        iter->second.first->data_callback(nullptr, 0, DataType::CLOSE);
        LOG_TRACE << "Ref num : " << iter->second.first.use_count();
        sniffer_container_.erase(iter);
    }


    int SnifferManager::get_current_time() {
        struct timeval t;
        gettimeofday(&t, NULL);
        return t.tv_sec;
    }
}
