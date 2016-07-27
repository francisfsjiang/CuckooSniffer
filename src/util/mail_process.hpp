#ifndef CUCKOOSNIFFER_UTIL_MAIL_PROCESSOR_HPP
#define CUCKOOSNIFFER_UTIL_MAIL_PROCESSOR_HPP

#include <vector>
#include <string>

namespace cs {
namespace util {

class File;

std::vector<File *> mail_process(const std::string &data);

}
}

#endif //CUCKOOSNIFFER_UTIL_MAIL_PROCESSOR_HPP
