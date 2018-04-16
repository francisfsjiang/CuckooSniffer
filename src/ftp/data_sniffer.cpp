#include <regex>
#include <fstream>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "ftp/data_sniffer.hpp"
#include "ftp/data_processor.hpp"
#include "ftp/collected_data.hpp"
#include "ftp/command_sniffer.hpp"
#include "util/file.hpp"
#include "util/buffer.hpp"
#include "util/function.hpp"

namespace cs::ftp {

    using namespace cs::util;

    void DataSniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {

    }

    void DataSniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
        auto& pay_load = stream.server_payload();
        buffer_->write(
                reinterpret_cast<const char*>(pay_load.data()),
                pay_load.size()
        );
    }

    void DataSniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
        LOG_DEBUG << "FTP data size: " << buffer_->size();
        auto buffer = new Buffer();
        cs::DATA_QUEUE.enqueue(new cs::ftp::CollectedData(buffer));

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

        buffer_ = new Buffer();

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
