#ifndef CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP

#include <regex>

#include "data_processor.hpp"
#include "sniffer_data.hpp"

class SMTPDataProcessor: public DataProcessor {

public:

    virtual int process(const SnifferData&);

    virtual ~SMTPDataProcessor();

};


#endif //CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP
