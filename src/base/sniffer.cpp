#include "base/sniffer.hpp"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/function.hpp"

namespace cs::base {

    Sniffer::~Sniffer() {
        LOG_TRACE << "sniffer deconstructor invoked ";
    }

    const std::string& Sniffer::get_id() {
        return id_;
    }

    Sniffer::Sniffer(const std::string& id) {
        id_ = id;
    }

    TCPSniffer::TCPSniffer(const std::string& id):
            Sniffer::Sniffer(id)
    {
    }

}
