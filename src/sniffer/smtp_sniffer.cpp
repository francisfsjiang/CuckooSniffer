#include "smtp_sniffer.hpp"


void SMTPSniffer::on_client_payload(const Tins::TCPIP::Stream& stream) {
    std::string data(stream.client_payload().begin(), stream.client_payload().end());
    std::cout << data << std::endl;
}

void SMTPSniffer::on_server_payload(const Tins::TCPIP::Stream& stream) {
    std::string data(stream.server_payload().begin(), stream.server_payload().end());
    std::cout << data << std::endl;
}

void SMTPSniffer::on_connection_close(const Tins::TCPIP::Stream&) {
    std::cout << "Connection Close" << std::endl;
}

void SMTPSniffer::on_connection_terminated(
        Tins::TCPIP::Stream&,
        Tins::TCPIP::StreamFollower::TerminationReason) {

}

SMTPSniffer::SMTPSniffer(Tins::TCPIP::Stream& stream) {
    stream.client_data_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this->on_client_payload(tcp_stream);
            }
    );

    stream.server_data_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this->on_server_payload(tcp_stream);
            }
    );

    stream.stream_closed_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this->on_connection_close(tcp_stream);
            }
    );

}

SMTPSniffer::~SMTPSniffer() {

}
