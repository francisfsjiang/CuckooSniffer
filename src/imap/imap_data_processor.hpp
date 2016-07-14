#ifndef CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP

#include <regex>

#include "data_processor.hpp"
#include "sniffer_data.hpp"

class IMAPDataProcessor: public DataProcessor{

public:

    virtual int process(const SnifferData&);

    virtual ~IMAPDataProcessor();

private:

    std::string match(const std::string&, const std::regex&);

    bool is_boundary(const std::string&, const std::string&);

};


#endif //CUCKOOSNIFFER_IMAP_DATA_PROCESSOR_HPP
