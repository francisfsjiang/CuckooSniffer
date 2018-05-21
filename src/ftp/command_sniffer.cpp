#include "ftp/command_sniffer.hpp"

#include <regex>
#include <iostream>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/function.hpp"

namespace cs::ftp {

    using namespace std;
    using namespace cs::base;

    std::map<uint16_t, std::string> CommandSniffer::data_connection_pool_ =
            std::map<uint16_t, std::string>();

    void CommandSniffer::on_client_payload(PayloadVector payload, size_t payload_size) {
        std::string command = std::string(
                (char*)payload,
                payload_size
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

    void CommandSniffer::on_server_payload(PayloadVector payload, size_t payload_size) {
        std::string command = std::string(
                (char*)payload,
                payload_size
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
                port_ = static_cast<uint16_t>(
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

    void CommandSniffer::on_connection_close() {
        LOG_DEBUG << stream_id_.to_string() << " FTP connection close.";
    }

    void CommandSniffer::on_connection_terminated(
            TerminationReason) {
        LOG_DEBUG << stream_id_.to_string() << " FTP connection terminated.";
    }

    CommandSniffer::CommandSniffer(const cs::base::StreamIdentifier& stream_id, int thread_id):
            TCPSniffer(stream_id, thread_id) {

    }

    std::map<uint16_t, std::string>& CommandSniffer::get_data_connection_pool() {
        return data_connection_pool_;
    }

    CommandSniffer::~CommandSniffer() {
    }

}
