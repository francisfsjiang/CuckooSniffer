#ifndef CUCKOOSNIFFER_BASE_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_BASE_DATA_PROCESSOR_HPP

namespace cs::base {

class SnifferData;

class DataProcessor {

public:

    virtual int process(const SnifferData &) = 0;

    virtual ~DataProcessor() {};

};

}

#endif //CUCKOOSNIFFER_DATA_PROCESSOR_HPP
