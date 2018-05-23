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
    using namespace std;

    void DataSniffer::on_client_payload(PayloadVector payload, size_t payload_size) {
//        LOG_DEBUG << "FTP client data size: " << buffer_->size();
        if (side_ == 1) {
            buffer_->write(
                    (char*)payload,
                    payload_size
            );
        }
    }

    void DataSniffer::on_server_payload(PayloadVector payload, size_t payload_size) {
//        LOG_DEBUG << "FTP server data size: " << buffer_->size();
        if (side_ == 0) {
            buffer_->write(
                    (char*)payload,
                    payload_size
            );
        }
    }

    void DataSniffer::on_connection_close() {
        LOG_DEBUG << "FTP data size: " << buffer_->size();
        auto f = new File();
        f->set_name(file_name_);
        f->write(*buffer_);
        submit_file_and_delete(f);

        LOG_DEBUG << "FTP data connection close";
    }

    void DataSniffer::on_connection_terminated(
            TerminationReason) {
        LOG_DEBUG << " FTP data connection terminated.";
    }

    DataSniffer::DataSniffer(const cs::base::StreamIdentifier& stream_id, int thread_id, string file_name, int side):
        TCPSniffer(stream_id, thread_id) {
        LOG_DEBUG << "FTP data connection side: " << side << " , name: " << file_name;

        buffer_ = new Buffer();
        side_ = side;
        file_name_ = file_name;
    }


    DataSniffer::~DataSniffer() {

        delete buffer_;
    }

}
