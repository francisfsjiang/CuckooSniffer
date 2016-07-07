#include "file.hpp"


File::File(
        const std::string& mime_type,
        const std::string& name,
        const std::string& encoding
) :
        mime_type_(mime_type),
        name_(name),
        encoding_(encoding)
{

}


void File::add_content(const std::string &str) {
    content_ += str;
}

const std::string& File::get_content() const {
    return content_;
}

const std::string& File::get_mime_type() const {
    return mime_type_;
}

const std::string& File::get_name() const {
    return name_;
}

const std::string& File::get_encoding() const {
    return encoding_;
}
