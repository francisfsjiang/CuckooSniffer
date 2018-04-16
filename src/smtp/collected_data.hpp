#ifndef CUCKOOSNIFFER_SMTP_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_SMTP_COLLECTED_DATA_HPP


#include <string>
#include <util/buffer.hpp>
#include "base/collected_data.hpp"

namespace cs::smtp {

class CollectedData: public cs::base::CollectedData {

public:

    CollectedData(cs::util::Buffer*);

    cs::util::Buffer* client_buffer_;
};

}


#endif //CUCKOOSNIFFER_SMTP_COLLECTED_DATA_HPP
