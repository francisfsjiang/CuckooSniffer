#include "http/data.hpp"

#include "http/parser.hpp"
#include "util/buffer.hpp"

namespace cs::http {
    using namespace std;
    using namespace util;

    HTTPData::HTTPData() {
        body_ = new Buffer();
        header_ = new std::map<std::string, std::string>();
        version_ = HTTPVersion::HTTP1_1;
    }

    HTTPData::~HTTPData() {
        delete body_;
        delete header_;

    }

    void HTTPData::set_header(const std::string& field, const std::string& value) {
        header_->insert(make_pair(field, value));
    }

    void HTTPData::set_version(HTTPVersion version) {
        version_ = version;
    }

    HTTPRequest::HTTPRequest(): HTTPData::HTTPData(){
        LOG_TRACE << "HTTPRequest constructing, " << this;
    }

    void HTTPRequest::prepare(const std::string& host, HTTP_METHOD method) {
        host_ = host;
        method_ = method;
    }

    HTTPRequest::~HTTPRequest() {
        LOG_TRACE << "HTTPRequest deconstructing, " << this;
    }

    HTTP_METHOD HTTPRequest::get_method() const {
        return method_;
    }

    std::string HTTPRequest::get_url() const {
        return url_;
    }

    const map<HTTPVersion, string> HTTP_VERSION_TO_STRING{
            {HTTPVersion::HTTP1_0, "HTTP/1.0"},
            {HTTPVersion::HTTP1_1, "HTTP/1.1"},
    };

    HTTPResponse::HTTPResponse() : HTTPData::HTTPData(){
        LOG_TRACE << "HTTPResponse constructing, " << this;
        status_code_ = HTTPStatus::OK;
    }

    HTTPResponse::~HTTPResponse() {
        LOG_TRACE << "HTTPResponse deconstructing, " << this;
    }

    HTTPStatus HTTPResponse::status_code() {
        return status_code_;
    }

    Buffer* HTTPData::get_body() {
        return body_;
    }

    map<string, string>* HTTPData::get_header() {
        return new map<string, string>(*header_);
    }

    void HTTPResponse::set_status_code(HTTPStatus code) {
        status_code_ = code;
    }

}
