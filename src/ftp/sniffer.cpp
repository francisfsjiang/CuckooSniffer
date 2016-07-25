#include <regex>

#include "sniffer_manager.hpp"
#include "ftp/sniffer.hpp"
#include "ftp/sniffer_data.hpp"
#include "ftp/data_processor.hpp"
#include "util/function.hpp"

namespace cs::ftp {

void Sniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {
}

void Sniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    std::string command = std::string(
            stream.server_payload().begin(),
            stream.server_payload().end()
    );


    static const std::regex open_port_command("227 Entering Passive Mode ([\\d,]*).");

    std::smatch match;
    std::string caught_str;

    try
    {
        if (std::regex_search(command, match, open_port_command) && match.size() > 1) {
            caught_str = match.str(1);
            std::cout << "get port command " << caught_str << std::endl;
            std::vector<std::string> vec = cs::util::split_str(caught_str, ",");
            unsigned short port = static_cast<unsigned short>(
                    atoi(vec[4].c_str()) * 256 + atoi(vec[5].c_str())
            );
            std::cout << "port: " << port << std::endl;
        }
    }
    catch (const std::exception&)
    {
        std::cerr << "Regex error" << std::endl;
    }

//    //TODO make this process in thread
//    SnifferData *ftp_data = new SnifferData(
//            std::string(stream.client_payload().begin(), stream.client_payload().end())
//    );
//
//    DataProcessor processor;
//    processor.process(*((SnifferData *) ftp_data));
//
//
//    delete ftp_data;
//    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

void Sniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    std::cout << "Connection Close" << std::endl;
    delete this;
}

void Sniffer::on_connection_terminated(
        Tins::TCPIP::Stream &,
        Tins::TCPIP::StreamFollower::TerminationReason) {
    std::cout << "[+] On Connection terminated " << id_ << std::endl;
    delete this;
}

Sniffer::Sniffer(Tins::TCPIP::Stream &stream) : TCPSniffer(stream) {

    stream.auto_cleanup_client_data(true);
    stream.auto_cleanup_server_data(true);
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
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

}
