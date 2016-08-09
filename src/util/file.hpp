#ifndef CUCKOOSNIFFER_UTIL_FILE_HPP
#define CUCKOOSNIFFER_UTIL_FILE_HPP

#include <string>
#include <sstream>

namespace cs {
namespace util {

class File {

public:

    File();

    void write(const char *, size_t);

    long long int get_size();

    std::string get_md5();

    const std::string& get_mime_type() const;
    void set_mime_type(const std::string&);

    const std::string& get_name() const;
    void set_name(const std::string&);

    const std::string& get_encoding() const;
    void set_encoding(const std::string&);

    bool get_encoding_status() const;
    void set_encoding_status(bool);

private:
    std::stringstream content_;

    std::string mime_type_;
    std::string name_;
    std::string encoding_;

    bool encoded_ = false;

};

}
}

#endif //CUCKOOSNIFFER_UTIL_FILE_HPP
