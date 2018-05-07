#include "sniffer_manager.hpp"

#include "cuckoo_sniffer.hpp"
#include "base/sniffer.hpp"

namespace cs {

    SnifferManager& SNIFFER_MANAGER = SnifferManager::get_instance();

    SnifferManager SnifferManager::instance;

    SnifferManager &SnifferManager::get_instance() {
        return instance;
    }

    void SnifferManager::append_sniffer(std::string sniffer_id, std::shared_ptr<cs::base::Sniffer> sniffer_ptr, int thread_id) {
        sniffer_container[sniffer_id] = std::make_pair(sniffer_ptr, thread_id);
        LOG_DEBUG << "New sniffer " << sniffer_id << ", total: " << sniffer_container.size();
    }

    std::pair<std::shared_ptr<cs::base::Sniffer>, int> SnifferManager::get_sniffer_info(std::string sniffer_id) {
        auto search = sniffer_container.find(sniffer_id);
        if (search != sniffer_container.end()) {
            return search -> second;
        }
        else {
            return std::make_pair(nullptr, -1);
        }
    }

    void SnifferManager::erase_sniffer(std::string sniffer_id) {
        auto search = sniffer_container.find(sniffer_id);
        if (search != sniffer_container.end()) {
            sniffer_container.erase(sniffer_id);
            LOG_DEBUG << "Erase sniffer " << sniffer_id << ", total: " << sniffer_container.size();
        }
    }

    SnifferManager::SnifferManager() {
        sniffer_container.clear();
    }

}