#ifndef CUCKOOSNIFFER_IMAP_SNIFFER_DATA_HPP
#define CUCKOOSNIFFER_IMAP_SNIFFER_DATA_HPP


#include <string>
#include "sniffer_data.hpp"

class IMAPSnifferData: public SnifferData {

public:

    IMAPSnifferData();

    void append(const std::string&);

    const std::string& get_data() const;

private:

    std::string data_;

};


#endif //CUCKOOSNIFFER_IMAP_SNIFFER_DATA_HPP
