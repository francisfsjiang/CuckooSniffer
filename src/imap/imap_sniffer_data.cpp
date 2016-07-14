#include "imap/imap_sniffer_data.hpp"


IMAPSnifferData::IMAPSnifferData() : SnifferData(DataType::IMAP){
}

void IMAPSnifferData::append(const std::string& data) {
    data_ += data;
}

const std::string& IMAPSnifferData::get_data() const {
    return data_;
}
