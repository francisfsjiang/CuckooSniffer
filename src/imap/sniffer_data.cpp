#include "imap/sniffer_data.hpp"

namespace cs::imap {

SnifferData::SnifferData() : cs::base::SnifferData(DataType::IMAP) {
}

void SnifferData::append(const std::string &data) {
    data_ += data;
}

const std::string &SnifferData::get_data() const {
    return data_;
}

}