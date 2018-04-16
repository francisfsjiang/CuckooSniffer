#ifndef CUCKOOSNIFFER_FTP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_FTP_DATA_PROCESSOR_HPP

#include <regex>

#include "base/data_processor.hpp"

namespace cs::ftp {

    std::vector<cs::util::File*> processor(cs::base::CollectedData* collected_data);

}


#endif //CUCKOOSNIFFER_FTP_DATA_PROCESSOR_HPP
