#include <sniffer_manager.hpp>
#include "smtp_sniffer.hpp"
#include "smtp_sniffer_data.hpp"
#include "smtp_data_processor.hpp"


void SMTPSniffer::on_client_payload(const Tins::TCPIP::Stream& stream) {
}

void SMTPSniffer::on_server_payload(const Tins::TCPIP::Stream& stream) {
}

void SMTPSniffer::on_connection_close(const Tins::TCPIP::Stream& stream) {
    std::cout << "Connection Close" << std::endl;

    //TODO make this process in thread
    SMTPSnifferData* smtp_data = new SMTPSnifferData(
            SnifferData::DataType::SMTP,
            std::string(stream.client_payload().begin(), stream.client_payload().end())
    );

    SMTPDataProcessor processor;
    processor.process(*((SnifferData*)smtp_data));


    delete smtp_data;
    SNIFFER_MANAGER.erase_sniffer(id_);
}

void SMTPSniffer::on_connection_terminated(
        Tins::TCPIP::Stream&,
        Tins::TCPIP::StreamFollower::TerminationReason) {

    std::cout << "[+] On Connection terminated " << id_ << std::endl;
}

SMTPSniffer::SMTPSniffer(Tins::TCPIP::Stream& stream) {
    stream.ignore_server_data();
    stream.auto_cleanup_client_data(false);
    stream.client_data_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this->on_client_payload(tcp_stream);
            }
    );

    stream.stream_closed_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this->on_connection_close(tcp_stream);
            }
    );

}

SMTPSniffer::SMTPSniffer(const std::string& id) {
    id_ = id;
}

SMTPSniffer::~SMTPSniffer() {

}
