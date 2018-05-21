#include "sniffer_manager.hpp"

#include "cuckoo_sniffer.hpp"
#include "base/sniffer.hpp"


namespace cs {

    SnifferManager& SNIFFER_MANAGER = SnifferManager::get_instance();

    SnifferManager SnifferManager::instance;

    SnifferManager &SnifferManager::get_instance() {
        return instance;
    }

    void SnifferManager::append_sniffer(const cs::base::StreamIdentifier& stream_id, std::shared_ptr<cs::base::Sniffer> sniffer_ptr, int thread_id) {
        sniffer_container.insert(
                std::make_pair(
                        stream_id,
                        std::make_pair(sniffer_ptr, thread_id
                        )
                )
        );
        LOG_DEBUG << "New sniffer " << stream_id.to_string() << ", total: " << sniffer_container.size();
    }

    std::pair<std::shared_ptr<cs::base::Sniffer>, int> SnifferManager::get_sniffer_info(const cs::base::StreamIdentifier& sniffer_id) {
        std::lock_guard<std::mutex> lock(lock_);
        auto search = sniffer_container.find(sniffer_id);
        if (search != sniffer_container.end()) {
            return search -> second;
        }
        else {
            return std::make_pair(nullptr, -1);
        }
    }

    void SnifferManager::erase_sniffer(const cs::base::StreamIdentifier& sniffer_id) {
        std::lock_guard<std::mutex> lock(lock_);
        auto search = sniffer_container.find(sniffer_id);
        if (search != sniffer_container.end()) {
            sniffer_container.erase(sniffer_id);
            LOG_DEBUG << "Erasing sniffer " << sniffer_id.to_string() << ", total: " << sniffer_container.size();
        }
    }

    SnifferManager::SnifferManager() {
        sniffer_container.clear();
    }

    bool SnifferManager::sniffer_container_cmp::operator()(const cs::base::StreamIdentifier& lhs, const cs::base::StreamIdentifier& rhs) const {
        return lhs.hash_key < rhs.hash_key; // NB. intentionally ignores y
    }
}