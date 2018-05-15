#ifndef CUCKOOSNIFFER_UTIL_FUNCTION_HPP
#define CUCKOOSNIFFER_UTIL_FUNCTION_HPP

#include <vector>
#include <string>
#include <set>

#include "tins/tcp.h"

namespace Tins::TCPIP {

    class Stream;

}

namespace cs::util {

    class File;

    std::vector<std::string> split_str(const std::string, const std::string);

    std::string stream_identifier(const Tins::TCPIP::Stream &stream);

    bool is_ignore_stream(
            const Tins::TCPIP::Stream&,
            const std::set<Tins::IPv4Address>&,
            const std::set<Tins::IPv6Address>&
    );

    std::string md5(const char*, size_t);

    int submit_files(const std::vector<File*>& );
    int submit_files(const File&, const std::string& url);

}

#endif //CUCKOOSNIFFER_FUNCTION_HPP
