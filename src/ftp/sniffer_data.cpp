#include "ftp/sniffer_data.hpp"

namespace cs::ftp {

SnifferData::SnifferData(
        std::string &&data) :
        cs::base::SnifferData(DataType::FTP) {
    data_ = std::move(data);
}

const std::string &SnifferData::get_data() const {
    return data_;
}

}