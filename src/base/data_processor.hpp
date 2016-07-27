#ifndef CUCKOOSNIFFER_BASE_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_BASE_DATA_PROCESSOR_HPP

namespace cs { namespace base {

class CollectedData;

class DataProcessor {

public:

    virtual int process(const CollectedData &) = 0;

    virtual ~DataProcessor() {};

};

}}

#endif //CUCKOOCOLLECTED_DATA_PROCESSOR_HPP
