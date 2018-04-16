#ifndef CUCKOOSNIFFER_UTIL_BASE64_HPP
#define CUCKOOSNIFFER_UTIL_BASE64_HPP

#include <string>

namespace cs::util {

std::string base64_encode(unsigned char const *, unsigned int len);

std::string base64_decode(std::string const &s);

}

#endif //CUCKOOSNIFFER_UTIL_BASE64_HPP
