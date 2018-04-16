#ifndef CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP

#include <regex>

#include "base/data_processor.hpp"

namespace cs::imap {

    std::vector<cs::util::File*> processor(cs::base::CollectedData* collected_data);

}


#endif //CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP
