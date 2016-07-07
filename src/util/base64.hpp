#ifndef CUCKOOSNIFFER_BASE64_HPP
#define CUCKOOSNIFFER_BASE64_HPP

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif //CUCKOOSNIFFER_BASE64_HPP
