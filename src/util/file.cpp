#include "util/file.hpp"

#include <cstdlib>
#include <cstring>
#include <cuckoo_sniffer.hpp>

#include "util/function.hpp"


namespace cs {
namespace util {

File::File() {
    buffer_ = nullptr;
    buffer_end_ = 0;
    buffer_size_ = 0;
}

bool File::write_to_pos(const char* data, uint64_t size, uint64_t offset) {
    if (offset + size > buffer_size_) {
        bool ret = set_size(offset + size);
        if (!ret)
            return false;
    }
    memcpy(buffer_ + offset, data, size);
    buffer_end_ = buffer_end_ > offset + size ? buffer_end_: offset + size;
    return true;
}

bool File::write(const char* data, uint64_t size) {
    return write_to_pos(data ,size, buffer_end_);
}

bool File::set_size(uint64_t size) {
    if (size > buffer_size_) {
        char* new_buffer = nullptr;
        try {
            new_buffer = new char[size];
        }
        catch (std::bad_alloc){
            LOG_ERROR << "Alloc new buffer in file failed.";
            return false;
        }
        memcpy(new_buffer, buffer_, buffer_end_);
        buffer_size_ = size;
        delete[] buffer_;
        buffer_ = new_buffer;
    }
    return false;
}

uint64_t File::get_size() const {
    return buffer_end_;
}

std::string File::get_md5() {
    return md5(buffer_, buffer_end_);
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


File::~File() {
    if (buffer_ != nullptr)
        delete[] buffer_;
}

const char* File::get_buffer() const {
    return buffer_;
}

}
}
