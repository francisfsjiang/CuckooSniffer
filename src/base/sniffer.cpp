#include "base/sniffer.hpp"

#include "sniffer_manager.hpp"

namespace cs::base {

Sniffer::~Sniffer() {
    std::cout << "sniffer deconstructor invoked " << std::endl;
};

const std::string& Sniffer::get_id() {
    return id_;
}



TCPSniffer::TCPSniffer(Tins::TCPIP::Stream& stream) {
    id_ = stream_identifier(stream);
}

std::string TCPSniffer::client_endpoint(const Tins::TCPIP::Stream &stream) {
    std::ostringstream output;
    if (stream.is_v6()) {
        output << stream.client_addr_v6();
    }
    else {
        output << stream.client_addr_v4();
    }
    output << ":" << stream.client_port();
    return output.str();
}


std::string TCPSniffer::server_endpoint(const Tins::TCPIP::Stream &stream) {
    std::ostringstream output;
    if (stream.is_v6()) {
        output << stream.server_addr_v6();
    }
    else {
        output << stream.server_addr_v4();
    }
    output << ":" << stream.server_port();
    return output.str();
}

std::string TCPSniffer::stream_identifier(const Tins::TCPIP::Stream &stream) {
    std::ostringstream output;
    output << client_endpoint(stream) << "-" << server_endpoint(stream);
    return output.str();
}

}