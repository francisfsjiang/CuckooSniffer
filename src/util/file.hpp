#ifndef CUCKOOSNIFFER_UTIL_FILE_HPP
#define CUCKOOSNIFFER_UTIL_FILE_HPP


#include <string>

class File {

public:

    File(
            const std::string&,
            const std::string&,
            const std::string&
    );

    void add_content(const std::string& str);



private:
public:
    const std::string &get_content() const;
    const std::string &get_mime_type() const;
    const std::string &get_name() const;
    const std::string &get_encoding() const;

private:
    std::string content_;
    std::string mime_type_;
    std::string name_;
    std::string encoding_;

};


#endif //CUCKOOSNIFFER_UTIL_FILE_HPP
