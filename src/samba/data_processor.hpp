#ifndef CUCKOOSNIFFER_SAMBA_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_SAMBA_DATA_PROCESSOR_HPP

#include <regex>

#include "base/data_processor.hpp"

namespace cs::samba {

    std::vector<cs::util::File*> processor(cs::base::CollectedData* collected_data);

}


#endif //CUCKOOSNIFFER_SAMBA_DATA_PROCESSOR_HPP
