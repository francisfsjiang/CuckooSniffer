#ifndef CUCKOOSNIFFER_UTIL_MAIL_PROCESSOR_HPP
#define CUCKOOSNIFFER_UTIL_MAIL_PROCESSOR_HPP

#include "util/file.hpp"

namespace cs::util {

std::vector<File *> mail_process(const std::string &data);

}

#endif //CUCKOOSNIFFER_UTIL_MAIL_PROCESSOR_HPP
