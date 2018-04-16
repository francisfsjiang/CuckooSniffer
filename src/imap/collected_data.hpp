#ifndef CUCKOOSNIFFER_IMAP_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_IMAP_COLLECTED_DATA_HPP

#include <string>
#include "base/collected_data.hpp"

namespace cs {
namespace imap {

class CollectedData : public cs::base::CollectedData {

public:

    CollectedData(cs::util::Buffer*);

    cs::util::Buffer* client_buffer_;
};

}
}


#endif //CUCKOOSNIFFER_IMAP_COLLECTED_DATA_HPP
