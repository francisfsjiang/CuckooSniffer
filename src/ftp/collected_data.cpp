#include "ftp/collected_data.hpp"

namespace cs { namespace ftp {

CollectedData::CollectedData(
        std::string &&data) :
        cs::base::CollectedData(DataType::FTP) {
    data_ = std::move(data);
}

const std::string &CollectedData::get_data() const {
    return data_;
}

}}