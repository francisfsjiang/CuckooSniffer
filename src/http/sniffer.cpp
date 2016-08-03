#include "http/sniffer.hpp"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "http/collected_data.hpp"
#include "http/data_processor.hpp"

namespace cs {
namespace http {

void Sniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {
    data_ += std::string(
            stream.client_payload().begin(),
            stream.client_payload().end()
    );
}

void Sniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
}

void Sniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    LOG_DEBUG << "HTTP data size: " << data_.size();

    CollectedData *http_data = new CollectedData(
            data_
    );
    cs::DATA_QUEUE.enqueue(http_data);

    SNIFFER_MANAGER.erase_sniffer(id_);
}

void Sniffer::on_connection_terminated(
        Tins::TCPIP::Stream &,
        Tins::TCPIP::StreamFollower::TerminationReason) {

    LOG_DEBUG << id_ << " HTTP connection terminated.";
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

Sniffer::Sniffer(Tins::TCPIP::Stream &stream) : TCPSniffer(stream) {

    stream.ignore_server_data();
    stream.auto_cleanup_client_data(true);
    stream.client_data_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this->on_client_payload(tcp_stream);
            }
    );

    stream.stream_closed_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this->on_connection_close(tcp_stream);
            }
    );

}

Sniffer::~Sniffer() {

}

}
}
