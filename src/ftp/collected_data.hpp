#ifndef CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP


#include <string>
#include "base/collected_data.hpp"

namespace cs {
namespace ftp {

class CollectedData: public base::CollectedData {

public:
    CollectedData(cs::util::Buffer*);

    cs::util::Buffer* server_buffer_;

};

}
}


#endif //CUCKOOSNIFFER_FTP_COLLECTED_DATA_HPP
