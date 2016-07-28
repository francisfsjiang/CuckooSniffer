#include "function.hpp"

#include <sstream>

#include "tins/tcp_ip/stream_follower.h"
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"

namespace cs {
namespace util {

std::vector<std::string> split_str(const std::string str, const std::string split) {
    std::string::size_type last_pos = 0, new_pos;
    std::vector<std::string> vec;
    while (1) {
        new_pos = str.find(split, last_pos);
        vec.push_back(str.substr(last_pos, new_pos - last_pos));
        if (new_pos == std::string::npos) {
            return vec;
        }
        else {
            last_pos = new_pos + 1;
        }
    }
}


std::string stream_identifier(const Tins::TCPIP::Stream &stream) {
    std::ostringstream output;
    if (stream.is_v6()) {
        output << stream.client_addr_v6()
               << ":"
               << stream.client_port()
               << "->"
               << stream.server_addr_v6()
               << ":"
               << stream.server_port();
    }
    else {
        output << stream.client_addr_v4()
               << ":"
               << stream.client_port()
               << "->"
               << stream.server_addr_v4()
               << ":"
               << stream.server_port();
    }
    return output.str();
}
}
}