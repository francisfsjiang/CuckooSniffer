#include "http/parser.hpp"

#include <string>

#include "cuckoo_sniffer.hpp"
#include "http/parser/http_parser.h"
#include "util/buffer.hpp"
#include "http/data.hpp"

namespace cs::http {
    using namespace cs::util;

#define HTTP_STATUS_GEN(num, name, string) { num , #string },
    const std::map<unsigned int, std::string> HTTP_STATUS_TO_DESCRIPTION = {
            HTTP_STATUS_MAP(HTTP_STATUS_GEN)
    };
#undef HTTP_STATUS_GEN

    http_parser_type http_parser_convertor(HTTP_TYPE t) {
        switch (t) {
            case HTTP_TYPE::REQUEST:
                return HTTP_REQUEST;
            case HTTP_TYPE::RESPONSE:
                return HTTP_RESPONSE;
            default:
                return HTTP_BOTH;
        }
    }

    const http_parser_settings* HTTPParser::parser_settings_ = new http_parser_settings{
            on_message_begin,
            on_url,
            on_status,
            on_header_field,
            on_header_value,
            on_header_complete,
            on_body,
            on_message_complete,
            on_chunk_header,
            on_chunk_complete
    };


    HTTPParser::HTTPParser(HTTP_TYPE type) {
//        LOG_DEBUG << "http-parser version: " << http_parser_version();
        parser_ = new http_parser();
        type_ = type;
        http_parser_init(parser_, http_parser_convertor(type));
        parser_->data = this;
        header_complete_ = false;
        message_complete_ = false;

        last_field_ = "";
    }

    HTTPParser::~HTTPParser() {
        delete parser_;
    }

    bool HTTPParser::complete() {
        return message_complete_;
    }



    size_t HTTPParser::perform_parsing(const Buffer& buffer) {
        LOG_TRACE << buffer.data_to_read();
        size_t ret = http_parser_execute(parser_, parser_settings_, buffer.data_to_read(), buffer.size());
        LOG_TRACE << http_errno_name(http_errno(parser_->http_errno));
        LOG_TRACE << http_errno_description(http_errno(parser_->http_errno));
        return ret;
    }

    int HTTPParser::on_message_begin(http_parser* parser) {
//        LOG_TRACE << "on_message_begin";
        auto p = static_cast<HTTPParser*>(parser->data);


        if (p->type_ == HTTP_TYPE::REQUEST) {
            p->current_data_ = new HTTPRequest;
            auto req = static_cast<HTTPRequestParser*>(p);
            req->current_request()->method_ = (HTTP_METHOD)(parser->method);
        }
        else {
            p->current_data_ = new HTTPResponse;
            auto resp = static_cast<HTTPResponseParser*>(p);
        }

        return 0;
    }

    int HTTPParser::on_url(http_parser* parser, const char* data, size_t size) {
//        LOG_TRACE << "on_url";
        auto req = static_cast<HTTPRequestParser*>(parser->data);
        req->current_request()->url_ += std::string(data, size);
        return 0;

    }

    int HTTPParser::on_status(http_parser* parser, const char* data, size_t size) {
//        LOG_TRACE << "on_status";
        auto resp = static_cast<HTTPResponseParser*>(parser->data);
        std::string status_code(data, size);
        int code = parser->status_code;
        resp->current_response()->set_status_code(HTTPStatus(code));

        return 0;

    }

    int HTTPParser::on_header_field(http_parser* parser, const char* data, size_t size) {
//        LOG_TRACE << "on_header_field";
        auto p = static_cast<HTTPParser*>(parser->data);
        p->last_field_ += std::string(data, size);
        return 0;

    }

    int HTTPParser::on_header_value(http_parser* parser, const char* data, size_t size) {
//        LOG_TRACE << "on_header_value";
        auto p = static_cast<HTTPParser*>(parser->data);
        auto iter = p->current_data_->header_->find(p->last_field_);
        if (iter != p->current_data_->header_->end()) {
            iter->second += std::string(data, size);
        }
        else {
            p->current_data_->header_->insert(std::make_pair(p->last_field_, std::string(data, size)));
        }
        p->last_field_ = "";
        return 0;

    }

    int HTTPParser::on_header_complete(http_parser* parser) {
//        LOG_TRACE << "on_header_complete";
        auto p = static_cast<HTTPParser*>(parser->data);
        p->header_complete_ = true;
        return 0;

    }

    int HTTPParser::on_body(http_parser* parser, const char* data, size_t size) {
        LOG_TRACE << "on_body, " << size;
        auto p = static_cast<HTTPParser*>(parser->data);
        p->current_data_->body_->write(data, size);
        LOG_TRACE << p->current_data_->body_->size();
        return 0;

    }

    int HTTPParser::on_message_complete(http_parser* parser) {
//        LOG_TRACE << "on_message_complete";
        auto p = static_cast<HTTPParser*>(parser->data);

        HTTPVersion version;
        if(parser->http_major == 2) {
            version = HTTPVersion::HTTP2_0;
        }
        else if (parser->http_minor == 1){
            version = HTTPVersion::HTTP1_1;
        }
        else {
            version = HTTPVersion::HTTP1_0;
        }
        p->current_data_->set_version(version);

        p->completed_data_.push(p->current_data_);
        p->current_data_ = nullptr;

        return 0;

    }

    int HTTPParser::on_chunk_header(http_parser* parser) {
        LOG_TRACE << "on_chunk_header";
        return 0;

    }

    int HTTPParser::on_chunk_complete(http_parser* parser) {
        LOG_TRACE << "on_chunk_complete";
        return 0;
    }

    HTTPRequestParser::HTTPRequestParser():
            HTTPParser::HTTPParser(HTTP_TYPE::REQUEST)
    {
    }

    HTTPRequest* HTTPRequestParser::get_request() {
        if (!completed_data_.empty()) {
            auto p = completed_data_.front();
            completed_data_.pop();
            return static_cast<HTTPRequest*>(p);
        }
        return nullptr;
    }

    HTTPResponseParser::HTTPResponseParser():
            HTTPParser::HTTPParser(HTTP_TYPE::RESPONSE)
    {
    }

    HTTPResponse* HTTPResponseParser::get_response() {
        if (!completed_data_.empty()) {
            auto p = completed_data_.front();
            completed_data_.pop();
            return static_cast<HTTPResponse*>(p);
        }
        return nullptr;
    }

    HTTPRequest* HTTPRequestParser::current_request() {
        return static_cast<HTTPRequest*>(current_data_);
    }

    HTTPResponse* HTTPResponseParser::current_response() {
        return static_cast<HTTPResponse*>(current_data_);
    }

}