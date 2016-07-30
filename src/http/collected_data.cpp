#include "http/collected_data.hpp"

namespace cs {
namespace http {

CollectedData::CollectedData( const std::string &data) :
        cs::base::CollectedData(DataType::HTTP) {
    data_ = data;
}

const std::string &CollectedData::get_data() const {
    return data_;
}

}
}