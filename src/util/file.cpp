#include "util/file.hpp"

#include <cstdlib>
#include <cstring>

#include "util/function.hpp"


namespace cs {
namespace util {

File::File() {
    encoded_ = false;
}

void File::write(const char *from, size_t size) {
    content_.write(from, size);
}

long long int File::get_size() {
    return content_.tellp();
}

std::string File::get_md5() {
    std::string s = content_.str();
    return md5(s.c_str(), s.size());
}

const std::string& File::get_mime_type() const {
    return mime_type_;
}

void File::set_mime_type(const std::string& mime_type) {
    mime_type_ = mime_type;
}

const std::string& File::get_name() const {
    return name_;
}

void File::set_name(const std::string& name) {
    name_ = name;
}

const std::string& File::get_encoding() const {
    return encoding_;
}

void File::set_encoding(const std::string& encoding) {
    encoding_ = encoding;
}

bool File::get_encoding_status() const {
    return encoded_;
}

void File::set_encoding_status(bool flag) {
    encoded_ = flag;
}

}
}
