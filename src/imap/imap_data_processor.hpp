#ifndef CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP

#include <regex>

#include "data_processor.hpp"
#include "sniffer_data.hpp"

class IMAPDataProcessor: public DataProcessor{

public:

    virtual int process(const SnifferData&);

    virtual ~IMAPDataProcessor();

};


#endif //CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP
