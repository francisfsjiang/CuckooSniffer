#include "smtp/sniffer.hpp"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "smtp/collected_data.hpp"
#include "smtp/data_processor.hpp"

namespace cs {
namespace smtp {

void Sniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {
    LOG_TRACE << "SMTP data size :" << stream.client_payload().size();
}

void Sniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
}

void Sniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    LOG_TRACE << "SMTP data size :" << stream.client_payload().size();
    LOG_DEBUG << id_ << " " << "SMTP Connection Close" << std::endl;

    //TODO make this process in thread
    CollectedData *smtp_data = new CollectedData(
            std::string(stream.client_payload().begin(), stream.client_payload().end())
    );

    cs::smtp::DataProcessor processor;
    processor.process((CollectedData *) smtp_data);


    delete smtp_data;
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

void Sniffer::on_connection_terminated(
        Tins::TCPIP::Stream &,
        Tins::TCPIP::StreamFollower::TerminationReason) {
    LOG_DEBUG << id_ << " SMTP data connection terminated.";
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

Sniffer::Sniffer(Tins::TCPIP::Stream &stream) : TCPSniffer(stream) {

    stream.ignore_server_data();
    stream.auto_cleanup_client_data(false);
    stream.client_data_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this -> on_client_payload(tcp_stream);
            }
    );

    stream.stream_closed_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this -> on_connection_close(tcp_stream);
            }
    );

}

Sniffer::~Sniffer() {
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

}
}
