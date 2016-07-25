#ifndef CUCKOOSNIFFER_FTP_SNIFFER_DATA_HPP
#define CUCKOOSNIFFER_FTP_SNIFFER_DATA_HPP


#include <string>
#include "base/sniffer_data.hpp"

namespace cs::ftp {

class SnifferData: public cs::base::SnifferData {

public:

    SnifferData(std::string&&);

    const std::string& get_data() const;

private:

    std::string data_;

};

}


#endif //CUCKOOSNIFFER_FTP_SNIFFER_DATA_HPP
