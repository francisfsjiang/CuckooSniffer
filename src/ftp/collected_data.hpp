#ifndef CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP


#include <string>
#include "base/collected_data.hpp"

namespace cs {
namespace ftp {

class CollectedData: public base::CollectedData {

public:

    CollectedData(const std::string&);

    const std::string& get_data() const;

private:

    std::string data_;

};

}
}


#endif //CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP
