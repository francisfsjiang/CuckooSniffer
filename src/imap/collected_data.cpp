#include "imap/collected_data.hpp"

namespace cs { namespace imap {

CollectedData::CollectedData() : cs::base::CollectedData(DataType::IMAP) {
}

void CollectedData::append(const std::string &data) {
    data_ += data;
}

const std::string &CollectedData::get_data() const {
    return data_;
}

}}