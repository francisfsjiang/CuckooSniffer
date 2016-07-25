#ifndef CUCKOOSNIFFER_IMAP_SNIFFER_DATA_HPP
#define CUCKOOSNIFFER_IMAP_SNIFFER_DATA_HPP

#include <string>
#include "base/sniffer_data.hpp"

namespace cs::imap {

class SnifferData : public cs::base::SnifferData {

public:

    SnifferData();

    void append(const std::string &);

    const std::string &get_data() const;

private:

    std::string data_;

};

}


#endif //CUCKOOSNIFFER_IMAP_SNIFFER_DATA_HPP
