#ifndef CUCKOOSNIFFER_UTIL_OPTION_PARSER_HPP
#define CUCKOOSNIFFER_UTIL_OPTION_PARSER_HPP

#include <string>
#include <map>

namespace cs {
namespace util {

int parse_cfg(int&, const char** &, std::map<std::string, std::string>&);

}
}

#endif //CUCKOOSNIFFER_UTIL_OPTION_PARSER_HPP
