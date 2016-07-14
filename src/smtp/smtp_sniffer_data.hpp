#ifndef CUCKOOSNIFFER_SMTP_SNIFFER_DATA_HPP
#define CUCKOOSNIFFER_SMTP_SNIFFER_DATA_HPP


#include <string>
#include "sniffer_data.hpp"

class SMTPSnifferData: public SnifferData {

public:

    SMTPSnifferData(std::string&&);

    const std::string& get_data() const;

private:

    std::string data_;

};


#endif //CUCKOOSNIFFER_SMTP_SNIFFER_DATA_HPP
