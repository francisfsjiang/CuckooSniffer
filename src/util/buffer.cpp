#include "util/buffer.hpp"

#include <algorithm>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>


namespace cs::util {

    std::ostream & operator<<(std::ostream &out, Buffer& buffer)
    {
        *(buffer.data_ + buffer.writer_pos_ + 1) = '\0';
        out << buffer.data();
        return out;
    }
//    std::istream & operator>>(std::istream &in, Buffer& obj)
//    {
//        in >> obj.a>> obj.b;
//        return in;
//    }

    Buffer::Buffer() {
        data_ = new char[DEFAULT_BUFFER_SIZE];
        capacity_ = DEFAULT_BUFFER_SIZE;
        writer_pos_ = 0;
        reader_pos_ = 0;
    }

    Buffer::Buffer(int len) {
        data_ = new char[len];
    }

    Buffer::Buffer(const Buffer& buffer) {
        capacity_ = buffer.capacity_;
        writer_pos_ = buffer.writer_pos_;
        reader_pos_ = buffer.reader_pos_;
        data_ = new char[capacity_];
        memcpy(data_, buffer.data_, capacity_);
    }

    size_t Buffer::resize() {
        return resize(capacity_ << 1);
    }

    size_t Buffer::resize(size_t new_capacity) {
        if(new_capacity < capacity_) {
            return capacity_;
        }
        auto new_data = new char[new_capacity];
        memcpy(new_data, data_ + reader_pos_, size());
        capacity_ = new_capacity;
        writer_pos_ = size();
        reader_pos_ = 0;

        delete[] data_;
        data_ = new_data;
        return new_capacity;
    }
    size_t Buffer::grow(size_t new_size) {
        if(new_size > capacity()) {
            size_t new_capacity = capacity();
            while (new_capacity < (SIZE_MAX >> 1) && writer_pos_ + new_size > new_capacity) {
                new_capacity <<= 1;
            }
            if (new_capacity < writer_pos_ + new_size){
//                LOG_FATAL << "Too large data to write in buffer.";
                return 0;
            }
            return resize(new_capacity);
        }
        return capacity();
    }

    Buffer::~Buffer() {
        delete[] data_;
    }

    size_t Buffer::capacity() const {
        return capacity_;
    }

    size_t Buffer::writeable_len() const {
        return capacity_ - writer_pos_;
    }

    size_t Buffer::size() const {
        return writer_pos_ - reader_pos_;
    }

    size_t Buffer::get_reader_pos() const {
        return reader_pos_;
    }

    size_t Buffer::get_writer_pos() const {
        return writer_pos_;
    }

    size_t Buffer::set_reader_pos(size_t pos){
        reader_pos_ = pos;
        return reader_pos_;
    }

    size_t Buffer::set_writer_pos(size_t pos){
        writer_pos_ = pos;
        return writer_pos_;
    }
    char* Buffer::data(){
        return data_;
    }
    char* Buffer::data_to_write() {
        return data_ + writer_pos_;
    }
    char* Buffer::data_to_read() const {
        return data_ + reader_pos_;
    }
    size_t Buffer::write_to_pos(const char* src_data, size_t len, size_t offset) {
        grow(offset + len);
        memcpy(data_ + offset, src_data, len);
        return static_cast<int>(len);
    }
    size_t Buffer::write(const char* src_data, size_t len) {
        grow(writer_pos_ + len);

        write_to_pos(src_data, len, writer_pos_);
        writer_pos_ += len;
        return static_cast<int>(len);
    }

    size_t Buffer::read(char* dst_data, size_t len) const {
        memcpy(data_, dst_data, len);
        return static_cast<int>(len);
    }

    int Buffer::shrink(size_t pos) {
        if (reader_pos_ == pos) {
            return 0;
        }
        size_t content_len = size();
        grow(pos + content_len);

        auto temp = new char[content_len];
        memcpy(temp, data_+reader_pos_, content_len);
        memcpy(data_+pos, temp, content_len);
        delete[] temp;

        reader_pos_ = pos;
        writer_pos_ = pos+content_len;
        return (int)content_len;
    }

    int Buffer::shrink() {
        return shrink(0);
    }
}
