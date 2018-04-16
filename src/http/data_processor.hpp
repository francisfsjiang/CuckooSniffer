#ifndef CUCKOOSNIFFER_HTTP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_HTTP_DATA_PROCESSOR_HPP

#include <regex>

#include "base/data_processor.hpp"

namespace cs::http {

    std::vector<cs::util::File*> processor(cs::base::CollectedData* collected_data);
}


#endif //CUCKOOSNIFFER_HTTP_DATA_PROCESSOR_HPP
