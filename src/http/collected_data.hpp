#ifndef CUCKOOSNIFFER_HTTP_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_HTTP_COLLECTED_DATA_HPP


#include <string>
#include "base/collected_data.hpp"

namespace cs { namespace http {

class CollectedData: public cs::base::CollectedData {

public:

    CollectedData(std::string&&);

    const std::string& get_data() const;

private:

    std::string data_;

};

}}


#endif //CUCKOOSNIFFER_HTTP_COLLECTED_DATA_HPP
