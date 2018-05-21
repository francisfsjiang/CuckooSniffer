#include <regex>
#include <fstream>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "ftp/data_sniffer.hpp"
#include "ftp/command_sniffer.hpp"
#include "util/file.hpp"
#include "util/buffer.hpp"
#include "util/function.hpp"

namespace cs::ftp {

    using namespace cs::util;
    using namespace cs::base;

    void DataSniffer::on_client_payload(PayloadVector payload, size_t payload_size) {

    }

    void DataSniffer::on_server_payload(PayloadVector payload, size_t payload_size) {
        buffer_->write(
                (char*)payload,
                payload_size
        );
    }

    void DataSniffer::on_connection_close() {
        LOG_DEBUG << "FTP data size: " << buffer_->size();
        auto buffer = new Buffer();

        LOG_DEBUG << "FTP data connection close";
        CommandSniffer::get_data_connection_pool().erase(stream_id_.dst_port);
    }

    void DataSniffer::on_connection_terminated(
            TerminationReason) {
        LOG_DEBUG << " FTP data connection terminated.";
        CommandSniffer::get_data_connection_pool().erase(stream_id_.dst_port);
    }

    DataSniffer::DataSniffer(const cs::base::StreamIdentifier& stream_id, int thread_id):
        TCPSniffer(stream_id, thread_id) {

        buffer_ = new Buffer();

    }


    DataSniffer::~DataSniffer() {

        delete buffer_;
    }

}
