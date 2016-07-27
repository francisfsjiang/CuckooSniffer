#include "samba/collected_data.hpp"

namespace cs {
namespace samba {

CollectedData::CollectedData(
        std::string &&data) :
        cs::base::CollectedData(DataType::SAMBA) {
    data_ = std::move(data);
}

const std::string &CollectedData::get_data() const {
    return data_;
}

}
}