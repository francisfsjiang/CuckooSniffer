#ifndef CUCKOOSNIFFER_UTIL_FILE_HPP
#define CUCKOOSNIFFER_UTIL_FILE_HPP

#include <string>

namespace cs::util {

class File {

public:

    File();

    void add_content(const std::string &str);

    const std::string &get_content() const;

    void set_size(size_t);
    void add_content(const char*, size_t);

    const std::string &get_mime_type() const;
    void set_mime_type(const std::string&);

    const std::string &get_name() const;
    void set_name(const std::string&);

    const std::string &get_encoding() const;
    void set_encoding(const std::string&);

    bool get_encode_status() const;
    void set_encode_status(bool);

    char* buffer_;
    size_t buffer_size_;
private:

    size_t buffer_pos_;

    std::string content_;
    std::string mime_type_;
    std::string name_;
    std::string encoding_;

    bool encoded_ = false;

};

}

#endif //CUCKOOSNIFFER_UTIL_FILE_HPP
