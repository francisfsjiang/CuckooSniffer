#include "http/sniffer.hpp"

#include <boost/algorithm/string/predicate.hpp>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "http/parser.hpp"
#include "http/data.hpp"
#include "http/multipart_parser.hpp"
#include "util/buffer.hpp"
#include "util/function.hpp"
#include "util/file.hpp"

namespace cs::http {


    using namespace cs::util;
    using namespace std;

    set<string> suspicious_type = {
            "application/octet-stream",
            "application/zip",
            "text/x-python",
            "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
            "application/vnd.openxmlformats-officedocument.presentationml.presentation",
    };

    void Sniffer::on_client_payload(const cs::base::PayloadType& payload) {
        client_buffer_->write(
                reinterpret_cast<const char *>(payload.data()),
//                (const char *)client_pay_load.data(),
                payload.size()
        );


        size_t ret = client_parser_->perform_parsing(*client_buffer_);
        if (ret) {
            client_buffer_->set_reader_pos(client_buffer_->get_reader_pos() + ret);
            client_buffer_->shrink();
        }
        LOG_DEBUG << "Client buffer size: " << client_buffer_->size() << ", after parsing.";

        while (true) {
            HTTPRequest* req = client_parser_->get_request();
            if (req) {
                request_url_ = req->get_header("Host") + req->get_url();
//                LOG_DEBUG << req->get_body()->size();
                if (req->get_body()->size() > 0) {
                    auto content_type = req->get_header("Content-Type");
                    LOG_DEBUG << "Content-Type: " << content_type;
                    vector<File*> file_vec;
                    if (boost::starts_with(content_type, "multipart/form-data")) {
                        auto boundary = string(content_type.data()+30, content_type.size()-30);
//                        LOG_DEBUG << "Boundary: " << boundary;
                        multipart_parse(*req->get_body(), boundary, content_type, file_vec);
                    }
                    else if (boost::starts_with(content_type, "application/x-www-form-urlencoded")) {
                        auto f = new File();
                        f->set_name(f->get_md5());
                        f->write(*req->get_body());
                        file_vec.push_back(f);
                    }
                    else {
                        LOG_ERROR << "Unsupported Content-Type.";
                    }

                    cs::util::submit_files(file_vec);
                }
                delete req;
            }
            else {
                break;
            }
        }
    }

    void Sniffer::on_server_payload(const cs::base::PayloadType& payload) {
        server_buffer_->write(
                reinterpret_cast<const char *>(payload.data()),
//                (const char *)client_pay_load.data(),
                payload.size()
        );
        size_t ret = server_parser_->perform_parsing(*server_buffer_);
        if (ret) {
            server_buffer_->set_reader_pos(server_buffer_->get_reader_pos() + ret);
            server_buffer_->shrink();
        }
        LOG_DEBUG << "Server buffer size: " << server_buffer_->size() << ", after parsing.";

        while (true) {
            HTTPResponse* resp = server_parser_->get_response();
            if (resp) {
                LOG_DEBUG << resp->get_body()->size();
                if (resp->get_body()->size() > 0) {
                    auto content_type = resp->get_header("Content-Type");
                    LOG_DEBUG << "Content-Type: " << content_type;
                    vector<File*> file_vec;
                    if (boost::starts_with(content_type, "multipart/form-data")) {
                        auto boundary = string(content_type.data()+30, content_type.size()-30);
                        multipart_parse(*resp->get_body(), boundary, content_type, file_vec);
                    }
                    else if (boost::starts_with(content_type, "application/x-www-form-urlencoded")) {

                    }
                    else {
                        LOG_ERROR << "Unsupported Content-Type.";
                    }

                    cs::util::submit_files(file_vec);
                }


                delete resp;
            }
            else {
                break;
            }
        }
    }

    void Sniffer::on_connection_close() {
        LOG_DEBUG << "HTTP server data size: " << server_buffer_->size();
        LOG_DEBUG << "HTTP client data size: " << client_buffer_->size();

        if (client_buffer_->size() == 0 && server_buffer_->size() == 0) {
            delete client_buffer_;
            delete server_buffer_;
        }
        else {
        }
        client_buffer_ = nullptr;
        server_buffer_ = nullptr;

        SNIFFER_MANAGER.erase_sniffer(id_);
    }

    void Sniffer::on_connection_terminated(
            cs::base::TerminationReason) {

        LOG_DEBUG << id_ << " HTTP connection terminated.";
        cs::SNIFFER_MANAGER.erase_sniffer(id_);
    }

    Sniffer::Sniffer(const string& stream_id) : TCPSniffer(stream_id) {

//        stream.auto_cleanup_server_data(true);
//        stream.auto_cleanup_client_data(true);
//        stream.client_data_callback(
//                [this](const Tins::TCPIP::Stream &tcp_stream) {
//                    this->on_client_payload(tcp_stream);
//                }
//        );
//
//        stream.stream_closed_callback(
//                [this](const Tins::TCPIP::Stream &tcp_stream) {
//                    this->on_connection_close(tcp_stream);
//                }
//        );

        client_buffer_ = new Buffer();
        client_parser_ = new HTTPRequestParser();
        server_buffer_ = new Buffer();
        server_parser_ = new HTTPResponseParser();

    }

    Sniffer::~Sniffer() {
        delete client_buffer_;
        delete client_parser_;
        delete server_buffer_;
        delete server_parser_;
    }

}
