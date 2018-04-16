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

TCPSniffer::TCPSniffer(Tins::TCPIP::Stream& stream) {
    id_ = cs::util::stream_identifier(stream);
}

}
