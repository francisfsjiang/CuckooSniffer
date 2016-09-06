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

class File;

std::vector<std::string> split_str(const std::string, const std::string);

std::string stream_identifier(const Tins::TCPIP::Stream &stream);

std::string md5(const char*, size_t);

int submit_file(const File&, const char* url);

}
}

#endif //CUCKOOSNIFFER_FUNCTION_HPP
