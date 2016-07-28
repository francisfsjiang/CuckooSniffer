#ifndef CUCKOOSNIFFER_UTIL_FUNCTION_HPP
#define CUCKOOSNIFFER_UTIL_FUNCTION_HPP

#include <vector>
#include <string>

namespace Tins {
namespace TCPIP {
class Stream;
}
}

namespace cs {
namespace util {

std::vector<std::string> split_str(const std::string, const std::string);

std::string stream_identifier(const Tins::TCPIP::Stream &stream);

}
}

#endif //CUCKOOSNIFFER_FUNCTION_HPP
