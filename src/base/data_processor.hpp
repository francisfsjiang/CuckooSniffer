#ifndef CUCKOOSNIFFER_BASE_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_BASE_DATA_PROCESSOR_HPP

#include "collected_data.hpp"

namespace cs {
namespace base {

class CollectedData;

class DataProcessor {

public:

    virtual int process(const CollectedData &) = 0;

    virtual ~DataProcessor() {};

};

DataProcessor* get_data_processor_by_data_type(const CollectedData::DataType&);

}
}

#endif //CUCKOOCOLLECTED_DATA_PROCESSOR_HPP
