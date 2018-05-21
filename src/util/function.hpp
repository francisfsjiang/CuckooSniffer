#ifndef CUCKOOSNIFFER_UTIL_FUNCTION_HPP
#define CUCKOOSNIFFER_UTIL_FUNCTION_HPP

#include <vector>
#include <string>
#include <set>

namespace cs::base {
    class StreamIdentifier;
}

namespace cs::util {

    class File;

    std::vector<std::string> split_str(const std::string, const std::string);

    bool is_ignored_stream(
            const cs::base::StreamIdentifier&,
            const std::set<std::string>&
    );

    std::string md5(const char*, size_t);

    int submit_files(const std::vector<File*>& );
    int submit_files(const File&, const std::string& url);

}

#endif //CUCKOOSNIFFER_FUNCTION_HPP
