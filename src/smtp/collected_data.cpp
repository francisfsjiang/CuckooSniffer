#include "smtp/collected_data.hpp"

namespace cs::smtp {

CollectedData::CollectedData(
        std::string &&data) :
        cs::base::CollectedData(DataType::SMTP) {
    data_ = std::move(data);
}

const std::string &CollectedData::get_data() const {
    return data_;
}

}