#include <regex>
#include <fstream>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "ftp/data_sniffer.hpp"
#include "ftp/collected_data.hpp"
#include "ftp/data_processor.hpp"
#include "ftp/command_sniffer.hpp"
#include "util/file.hpp"
#include "util/function.hpp"

namespace cs {
namespace ftp {

void DataSniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {

}

void DataSniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    std::string data = std::string(
            stream.server_payload().begin(),
            stream.server_payload().end()
    );
    payload_ += data;
}

void DataSniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    LOG_DEBUG << "FTP data size: " << payload_.size();
    cs::DATA_QUEUE.enqueue(
            new CollectedData(
                    payload_
            )
    );

    LOG_DEBUG << "FTP data connection close";
    CommandSniffer::get_data_connection_pool().erase(stream.server_port());
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

void DataSniffer::on_connection_terminated(
        Tins::TCPIP::Stream& stream,
        Tins::TCPIP::StreamFollower::TerminationReason) {
    LOG_DEBUG << id_ << " FTP data connection terminated.";
    CommandSniffer::get_data_connection_pool().erase(stream.server_port());
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

DataSniffer::DataSniffer(Tins::TCPIP::Stream &stream) : TCPSniffer(stream) {
    LOG_DEBUG << "Get FTP data connection " << id_;

    file_ = new cs::util::File();

    stream.ignore_client_data();
    stream.server_data_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this -> on_server_payload(tcp_stream);
            }
    );

    stream.stream_closed_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this -> on_connection_close(tcp_stream);
            }
    );

}


DataSniffer::~DataSniffer() {
}

}
}