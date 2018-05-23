#include "ftp/command_sniffer.hpp"

#include <regex>
#include <iostream>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/function.hpp"
#include "threads/data_queue.hpp"

namespace cs::ftp {

    using namespace std;
    using namespace cs::base;

    bool dcp_cmp::operator()(const std::pair<std::string, uint16_t> &lhs,
                                             const std::pair<std::string, uint16_t> &rhs) const {
        if (lhs.second == rhs.second) {
            return lhs.first < rhs.first;
        }
        return lhs.second < rhs.second;
    }

    CommandSniffer::DataConnectionPool CommandSniffer::data_connection_pool_ =
            CommandSniffer::DataConnectionPool();

    CommandSniffer::DataConnectionPool& CommandSniffer::get_data_connection_pool() {
        return data_connection_pool_;
    }

    static const std::regex send_file_command("STOR (.*)");
    static const std::regex recv_file_command("RETR (.*)");

    void CommandSniffer::on_client_payload(PayloadVector payload, size_t payload_size) {
        std::string command = std::string(
                (char*)payload,
                payload_size
        );
//        LOG_TRACE << "Client: " << command;
        std::smatch match;
        std::string caught_str;

        try {
            if (std::regex_search(command, match, recv_file_command) && match.size() > 1) {
                caught_str = match.str(1);
//                LOG_DEBUG << "FTP command recv file command, name " << caught_str;
                if(current_port_ != -1) {
                    data_connection_pool_.insert(std::make_pair(
                            make_pair(stream_id_.dst_addr, current_port_),
                            make_pair(string(caught_str), 0)
                    ));
                    current_port_ = -1;
                }
            }
            else if (std::regex_search(command, match, send_file_command) && match.size() > 1) {
                caught_str = match.str(1);
//                LOG_DEBUG << "FTP command send file command, name " << caught_str;
                if(current_port_ != -1) {
                    data_connection_pool_.insert(std::make_pair(
                            make_pair(stream_id_.dst_addr, current_port_),
                            make_pair(string(caught_str), -1)
                    ));
                    current_port_ = -1;
                }
            }
        }
        catch (const std::exception& ) {
            LOG_ERROR << "FTP command sniffer regex error.";
        }

    }

    static const std::regex open_port_command("227 Entering Passive Mode \\(([\\d,]*)\\)");

    void CommandSniffer::on_server_payload(PayloadVector payload, size_t payload_size) {
        std::string command = std::string(
                (char*)payload,
                payload_size
        );
//        LOG_TRACE << "Server: " << command;


        std::smatch match;
        std::string caught_str;

        try
        {
            if (std::regex_search(command, match, open_port_command) && match.size() > 1) {
                caught_str = match.str(1);
                std::vector<std::string> vec = cs::util::split_str(caught_str, ",");
                uint16_t port = static_cast<uint16_t>(
                        atoi(vec[4].c_str()) * 256 + atoi(vec[5].c_str())
                );
//                LOG_TRACE << "get port command " << caught_str << " , port: " << port;
                current_port_ = port;
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
        current_port_ = -1;
    }

    CommandSniffer::~CommandSniffer() {

    }

    void CommandSniffer::data_callback(
            cs::base::PayloadVector stream_data,
            size_t stream_data_size,
            cs::threads::DataType type
    ) {
        if (type == cs::threads::DataType::CLIENT_PAYLOAD) {
            on_client_payload(stream_data, stream_data_size);
        }
        else if (type == cs::threads::DataType::SERVER_PAYLOAD) {
            on_server_payload(stream_data, stream_data_size);
        }
        else {
            on_connection_close();
        }

    }
}
