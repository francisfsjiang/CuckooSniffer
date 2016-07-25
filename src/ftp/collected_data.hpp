#ifndef CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP


#include <string>
#include "base/collected_data.hpp"

namespace cs::ftp {

class CollectedData: public cs::base::CollectedData {

public:

    CollectedData(std::string&&);

    const std::string& get_data() const;

private:

    std::string data_;

};

}


#endif //CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP