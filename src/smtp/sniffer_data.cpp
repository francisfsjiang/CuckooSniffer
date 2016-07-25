#include "smtp/sniffer_data.hpp"

namespace cs::smtp {

SnifferData::SnifferData(
        std::string &&data) :
        cs::base::SnifferData(DataType::SMTP) {
    data_ = std::move(data);
}

const std::string &SnifferData::get_data() const {
    return data_;
}

}