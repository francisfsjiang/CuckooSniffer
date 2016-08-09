#ifndef CUCKOOSNIFFER_HTTP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_HTTP_DATA_PROCESSOR_HPP

#include <regex>

#include "base/data_processor.hpp"
#include "base/collected_data.hpp"

namespace cs {
namespace http {

class DataProcessor : public cs::base::DataProcessor {

public:

    virtual int process(cs::base::CollectedData*);

    virtual ~DataProcessor();

};

}
}


#endif //CUCKOOSNIFFER_HTTP_DATA_PROCESSOR_HPP
