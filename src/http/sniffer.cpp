#include "http/sniffer.hpp"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "http/data_processor.hpp"
#include "http/collected_data.hpp"
#include "util/buffer.hpp"

namespace cs::http {

    using namespace cs::util;

    void Sniffer::on_client_payload(const cs::base::payload_type& payload) {
        client_buffer_->write(
                reinterpret_cast<const char *>(payload.data()),
//                (const char *)client_pay_load.data(),
                payload.size()
        );
    }

    void Sniffer::on_server_payload(const cs::base::payload_type& payload) {
        server_buffer_->write(
                reinterpret_cast<const char *>(payload.data()),
//                (const char *)client_pay_load.data(),
                payload.size()
        );
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
            Tins::TCPIP::StreamFollower::TerminationReason) {

        LOG_DEBUG << id_ << " HTTP connection terminated.";
        cs::SNIFFER_MANAGER.erase_sniffer(id_);
    }

    Sniffer::Sniffer(const std::string& stream_id) : TCPSniffer(stream_id) {

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

        server_buffer_ = new Buffer();
        client_buffer_ = new Buffer();

    }

    Sniffer::~Sniffer() {

    }

}
