#ifndef CUCKOOSNIFFER_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_DATA_PROCESSOR_HPP

class SnifferData;

class DataProcessor {

public:

    virtual int process(const SnifferData&) = 0;

    virtual ~DataProcessor() {};

};


#endif //CUCKOOSNIFFER_DATA_PROCESSOR_HPP
