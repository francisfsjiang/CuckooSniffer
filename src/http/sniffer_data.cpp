#include "http/sniffer_data.hpp"

namespace cs::http {

SnifferData::SnifferData(
        std::string &&data) :
        cs::base::SnifferData(DataType::HTTP) {
    data_ = std::move(data);
}

const std::string &SnifferData::get_data() const {
    return data_;
}

}