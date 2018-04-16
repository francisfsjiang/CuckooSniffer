#ifndef CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP

#include <regex>

#include "base/data_processor.hpp"

namespace cs::smtp {

    std::vector<cs::util::File*> processor(cs::base::CollectedData* collected_data);

}


#endif //CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP
