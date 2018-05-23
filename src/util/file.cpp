#include "util/file.hpp"

#include <cstdlib>
#include <cstring>
#include <cuckoo_sniffer.hpp>

#include "util/buffer.hpp"
#include "util/function.hpp"


namespace cs::util {

    File::File() {
        buffer_ = new Buffer();
    }

    File::File(Buffer* buffer) {
        buffer_ = buffer;
    }

    size_t File::write_to_pos(const char* data, uint64_t size, uint64_t offset) {
        return buffer_->write_to_pos(data, size, offset);
    }

    size_t File::write(const char* data, uint64_t size) {
        return buffer_->write(data ,size);
    }

    size_t File::write(const Buffer& buffer) {
        return buffer_->write(buffer.data_to_read(), buffer.size());
    }

    bool File::set_size(uint64_t size) {
        buffer_->resize(size);
        buffer_->set_reader_pos(0);
        buffer_->set_writer_pos(size);
        return false;
    }

    uint64_t File::get_size() const {
        return buffer_->size();
    }

    std::string File::get_md5() {
        return md5(buffer_->data_to_read(), buffer_->size());
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
        delete buffer_;
    }

    Buffer* File::get_buffer() const {
        return buffer_;
    }

}
