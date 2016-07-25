#ifndef CUCKOOSNIFFER_FTP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_FTP_DATA_PROCESSOR_HPP

#include <regex>

#include "base/data_processor.hpp"
#include "base/sniffer_data.hpp"

namespace cs::ftp {

class DataProcessor : public cs::base::DataProcessor {

public:

    virtual int process(const cs::base::SnifferData &);

    virtual ~DataProcessor();

};

}


#endif //CUCKOOSNIFFER_FTP_DATA_PROCESSOR_HPP
