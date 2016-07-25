#ifndef CUCKOOSNIFFER_SMTP_SNIFFER_DATA_HPP
#define CUCKOOSNIFFER_SMTP_SNIFFER_DATA_HPP


#include <string>
#include "base/sniffer_data.hpp"

namespace cs::smtp {

class SnifferData: public cs::base::SnifferData {

public:

    SnifferData(std::string&&);

    const std::string& get_data() const;

private:

    std::string data_;

};

}


#endif //CUCKOOSNIFFER_SMTP_SNIFFER_DATA_HPP
