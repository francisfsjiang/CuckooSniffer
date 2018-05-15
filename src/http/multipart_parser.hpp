#ifndef _CUCKOOSNIFFER_MULTIPART_PARSER_HPP_
#define _CUCKOOSNIFFER_MULTIPART_PARSER_HPP_

#include <vector>
#include <string>
#include <map>

namespace cs::util {
    class File;
    class Buffer;
}

namespace cs::http {

    int multipart_parse(const cs::util::Buffer&, const std::string&, const std::string&, std::vector<cs::util::File*>&);

    int parse_parameters(const std::string&, std::string&, std::map<std::string, std::string>&);
}


#endif //_CUCKOOSNIFFER_MULTIPART_PARSER_HPP_
