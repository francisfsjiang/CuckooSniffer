#ifndef CUCKOOSNIFFER_HTTP_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_HTTP_COLLECTED_DATA_HPP


#include <string>
#include "base/collected_data.hpp"

namespace cs {
namespace http {

class CollectedData: public cs::base::CollectedData {

public:
    CollectedData(cs::util::Buffer*, cs::util::Buffer*);

    cs::util::Buffer* client_buffer_;
    cs::util::Buffer* server_buffer_;

};

}
}


#endif //CUCKOOSNIFFER_HTTP_COLLECTED_DATA_HPP
