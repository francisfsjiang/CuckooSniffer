#include "smtp/smtp_sniffer_data.hpp"


SMTPSnifferData::SMTPSnifferData(SnifferData::DataType data_type, std::string&& data) {
    data_type_ = data_type;
    data_ = std::move(data);
}

const std::string& SMTPSnifferData::get_data() const {
    return data_;
}