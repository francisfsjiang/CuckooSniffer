#ifndef CUCKOOSNIFFER_UTIL_FILE_HPP
#define CUCKOOSNIFFER_UTIL_FILE_HPP

#include <string>
#include <sstream>

namespace cs {
namespace util {

class File {

public:

    File();

    bool write(const char*, uint64_t);
    bool write_to_pos(const char*, uint64_t, uint64_t);

    bool set_size(uint64_t);
    uint64_t get_size() const;

    std::string get_md5();

    const std::string& get_mime_type() const;
    void set_mime_type(const std::string&);

    const std::string& get_name() const;
    void set_name(const std::string&);


    ~File();

    const char* get_buffer() const;

private:
    char* buffer_;

    uint64_t buffer_end_;
    uint64_t buffer_size_;

    std::string mime_type_;
    std::string name_;


};

}
}

#endif //CUCKOOSNIFFER_UTIL_FILE_HPP
