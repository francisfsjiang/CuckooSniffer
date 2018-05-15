#ifndef _CUCKOOSNIFFER_HTTP_DATA_HPP_
#define _CUCKOOSNIFFER_HTTP_DATA_HPP_

#include <string>
#include <map>

#include "http/parser.hpp"

namespace cs::util {
    class Buffer;
}

namespace cs::http {


    class HTTPData {
    public:
        HTTPData();
        ~HTTPData();
        HTTP_TYPE type;
        HTTPVersion version_;
        util::Buffer* body_;
        std::map<std::string, std::string>* header_;

        void set_header(const std::string&, const std::string&);
        std::string get_header(const std::string&);
        void set_version(HTTPVersion);
        util::Buffer* get_body();
    };


    class HTTPRequest: public HTTPData{
        friend class HTTPParser;
    private:
        HTTP_METHOD method_;
        std::string host_;
        std::string url_;

    public:
        HTTPRequest();
        ~HTTPRequest();

        void prepare(const std::string&, HTTP_METHOD);

        int write_post_data(const char*, size_t);

        HTTP_METHOD get_method() const;
        std::string get_url() const;

    };

    class HTTPResponse: public HTTPData{
        friend class HTTPParser;
    private:
        HTTPStatus status_code_;
        std::map<std::string, std::string>* header_;

    public:
        HTTPResponse();
        ~HTTPResponse();

        HTTPStatus status_code();
        void set_status_code(HTTPStatus);

    };
}


#endif //_CUCKOOSNIFFER_HTTP_REQUEST_HPP_
