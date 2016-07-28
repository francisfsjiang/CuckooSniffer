#include "sniffer_manager.hpp"

#include "cuckoo_sniffer.hpp"
#include "base/sniffer.hpp"

namespace cs {

SnifferManager& SNIFFER_MANAGER = SnifferManager::get_instance();

SnifferManager SnifferManager::instance;

SnifferManager &SnifferManager::get_instance() {
    return instance;
}

void SnifferManager::append_sniffer(std::string sniffer_id, cs::base::Sniffer *sniffer_ptr) {
    sniffer_container[sniffer_id] = sniffer_ptr;
    LOG_DEBUG << "New sniffer " << sniffer_id << ", total: " << sniffer_container.size();
}

cs::base::Sniffer *SnifferManager::get_sniffer(std::string sniffer_id) {
    auto search = sniffer_container.find(sniffer_id);
    if (search != sniffer_container.end()) {
        return search -> second;
    }
    else {
        return nullptr;
    }
}

void SnifferManager::erase_sniffer(std::string sniffer_id) {
    auto search = sniffer_container.find(sniffer_id);
    if (search != sniffer_container.end()) {
        delete search -> second;
        sniffer_container.erase(sniffer_id);
        LOG_DEBUG << "Erase sniffer " << sniffer_id << ", total: " << sniffer_container.size();
    }
}

SnifferManager::SnifferManager() {
    sniffer_container.clear();
}

}