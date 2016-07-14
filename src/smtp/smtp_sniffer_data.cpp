#include "smtp/smtp_sniffer_data.hpp"


SMTPSnifferData::SMTPSnifferData(
    std::string&& data) :
    SnifferData(DataType::SMTP)
 {
    data_ = std::move(data);
}

const std::string& SMTPSnifferData::get_data() const {
    return data_;
}
