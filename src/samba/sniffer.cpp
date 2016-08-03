#include "samba/sniffer.hpp"

#include <vector>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "samba/collected_data.hpp"
#include "samba/data_processor.hpp"

namespace cs {
namespace samba {

void Sniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {
    std::vector<uint8_t> vec = std::vector<uint8_t>(
            stream.client_payload().begin(),
            stream.client_payload().end()
    );
    uint32_t total_length = 0;
    if(vec[0] == 0) {
        total_length = vec[1] << 16 + vec[2] << 8 + vec[3];

    }

    //find read request

    //find write request
}

void Sniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    //find read response
    std::vector<uint8_t> vec = std::vector<uint8_t>(
            stream.server_payload().begin(),
            stream.server_payload().end()
    );
    uint32_t total_length = 0;
    if(vec[0] == 0) {
        total_length = vec[1] << 16 + vec[2] << 8 + vec[3];

    }

}

void Sniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    LOG_DEBUG << "SAMBA onnection Close";

//    CollectedData *samba_data = new CollectedData(
//            std::string(stream.client_payload().begin(), stream.client_payload().end())
//    );
//
//    DataProcessor processor;
//    processor.process(*((CollectedData *) samba_data));
//
//
//    delete samba_data;
    SNIFFER_MANAGER.erase_sniffer(id_);
}

void Sniffer::on_connection_terminated(
        Tins::TCPIP::Stream &,
        Tins::TCPIP::StreamFollower::TerminationReason) {

    LOG_DEBUG << id_ << " SAMBA connection terminated.";
    SNIFFER_MANAGER.erase_sniffer(id_);
}

Sniffer::Sniffer(Tins::TCPIP::Stream &stream) : TCPSniffer(stream) {

    stream.client_data_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this->on_client_payload(tcp_stream);
            }
    );

    stream.server_data_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this->on_server_payload(tcp_stream);
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
