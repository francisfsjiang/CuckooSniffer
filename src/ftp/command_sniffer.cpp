#include "ftp/command_sniffer.hpp"

#include <regex>
#include <iostream>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "ftp/data_processor.hpp"
#include "util/function.hpp"

namespace cs::ftp {

std::map<unsigned short, std::string> CommandSniffer::data_connection_pool_ =
        std::map<unsigned short, std::string>();

void CommandSniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {
    std::string command = std::string(
            stream.client_payload().begin(),
            stream.client_payload().end()
    );
    static const std::regex get_file_command("RETR (.*)");
    std::smatch match;
    std::string caught_str;

    try {
        if (std::regex_search(command, match, get_file_command) && match.size() > 1) {
            caught_str = match.str(1);
            LOG_DEBUG << "FTP command get file command " << caught_str;
            data_connection_pool_[port_] = caught_str;
        }
    }
    catch (const std::exception& ) {
        LOG_ERROR << "FTP command sniffer regex error.";
    }

}

void CommandSniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    std::string command = std::string(
            stream.server_payload().begin(),
            stream.server_payload().end()
    );

    static const std::regex open_port_command("227 Entering Passive Mode \\(([\\d,]*)\\).");

    std::smatch match;
    std::string caught_str;

    try
    {
        if (std::regex_search(command, match, open_port_command) && match.size() > 1) {
            caught_str = match.str(1);
            std::cout << "get port command " << caught_str << std::endl;
            std::vector<std::string> vec = cs::util::split_str(caught_str, ",");
            port_ = static_cast<unsigned short>(
                    atoi(vec[4].c_str()) * 256 + atoi(vec[5].c_str())
            );
            std::cout << "port: " << port_ << std::endl;
            data_connection_pool_[port_] = "";
        }
    }
    catch (const std::exception&)
    {
        LOG_ERROR << "FTP command sniffer regex error.";
    }
}

void CommandSniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    LOG_DEBUG << id_ << " FTP connection close.";
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

void CommandSniffer::on_connection_terminated(
        Tins::TCPIP::Stream &,
        Tins::TCPIP::StreamFollower::TerminationReason) {
    LOG_DEBUG << id_ << " FTP connection terminated.";
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

CommandSniffer::CommandSniffer(Tins::TCPIP::Stream &stream) : cs::base::TCPSniffer(stream) {
    LOG_DEBUG << id_ << " Get FTP command connection.";

    stream.auto_cleanup_client_data(true);
    stream.auto_cleanup_server_data(true);
    stream.server_data_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this -> on_server_payload(tcp_stream);
            }
    );

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

std::map<unsigned short, std::string>& CommandSniffer::get_data_connection_pool() {
    return data_connection_pool_;
}

CommandSniffer::~CommandSniffer() {
}

}
