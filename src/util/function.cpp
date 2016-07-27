#include "function.hpp"

namespace cs { namespace util {

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

}}