#include "util/file.hpp"

#include <cstdlib>
#include <cstring>

namespace cs::util {

File::File() {
    encoded_ = false;
}

void File::set_size(size_t size) {
    buffer_size_ = size;
    buffer_ = new char[size];
    buffer_pos_ = 0;
}
void File::add_content(const char* from, size_t size) {
    memcpy(buffer_ + buffer_pos_, from, size);
    buffer_pos_ += size;
}

void File::add_content(const std::string &str) {
    content_ += str;
}

const std::string &File::get_content() const {
    return content_;
}


const std::string &File::get_mime_type() const {
    return mime_type_;
}

void File::set_mime_type(const std::string& str) {
    mime_type_ = str;
}

const std::string &File::get_name() const {
    return name_;
}

void File::set_name(const std::string& str) {
    name_ = str;
}

const std::string &File::get_encoding() const {
    return encoding_;
}

bool File::get_encode_status() const {
    return encoded_;
}

void File::set_encode_status(bool flag) {
    encoded_ = flag;
}

}
