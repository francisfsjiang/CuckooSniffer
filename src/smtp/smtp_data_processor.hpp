#ifndef CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP

#include <regex>

#include "sniffer_data.hpp"

class SMTPDataProcessor: public SnifferData {

public:

    virtual int process(const SnifferData&);

    virtual ~SMTPDataProcessor();

private:

    std::string match(const std::string&, const std::regex&);

    bool is_boundary(const std::string&, const std::string&);

};


#endif //CUCKOOSNIFFER_SMTP_DATA_PROCESSOR_HPP
