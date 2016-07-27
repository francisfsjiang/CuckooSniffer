#ifndef CUCKOOSNIFFER_SAMBA_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_SAMBA_DATA_PROCESSOR_HPP

#include <regex>

#include "base/data_processor.hpp"
#include "base/collected_data.hpp"

namespace cs {
namespace samba {

class DataProcessor : public cs::base::DataProcessor {

public:

    virtual int process(const cs::base::CollectedData &);

    virtual ~DataProcessor();

};

}
}


#endif //CUCKOOSNIFFER_SAMBA_DATA_PROCESSOR_HPP
