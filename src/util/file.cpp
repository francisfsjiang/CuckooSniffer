#include "file.hpp"

namespace cs::util {

File::File(
        const std::string &name,
        const std::string &mime_type,
        const std::string &encoding
) :
        name_(name),
        mime_type_(mime_type),
        encoding_(encoding) {

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

const std::string &File::get_name() const {
    return name_;
}

const std::string &File::get_encoding() const {
    return encoding_;
}

}