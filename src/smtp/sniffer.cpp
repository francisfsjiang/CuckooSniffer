#include "smtp/sniffer.hpp"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/buffer.hpp"
#include "base/data_processor.hpp"
#include "smtp/collected_data.hpp"
#include "smtp/data_processor.hpp"

namespace cs::smtp {

    using namespace cs::util;
    using namespace cs::base;

    void Sniffer::on_client_payload(const cs::base::PayloadType& payload) {
        LOG_TRACE << "SMTP data size :" << payload.size();
    }

    void Sniffer::on_server_payload(const cs::base::PayloadType& payload) {
    }

    void Sniffer::on_connection_close() {
        LOG_TRACE << "SMTP data size :" << stream.client_payload().size();
        LOG_DEBUG << id_ << " " << "SMTP Connection Close" << std::endl;

        //TODO make this process in threa
        auto& client_pay_load = stream.client_payload();
        auto client_buffer = new Buffer();
        client_buffer->write(
                reinterpret_cast<const char *>(client_pay_load.data()),
//                (const char *)client_pay_load.data(),
                client_pay_load.size()
        );

//        cs::DATA_QUEUE.enqueue(new cs::smtp::CollectedData(client_buffer));

        cs::SNIFFER_MANAGER.erase_sniffer(id_);
    }

    void Sniffer::on_connection_terminated(
            Tins::TCPIP::Stream &,
            Tins::TCPIP::StreamFollower::TerminationReason) {
        LOG_DEBUG << id_ << " SMTP data connection terminated.";
        cs::SNIFFER_MANAGER.erase_sniffer(id_);
    }

    Sniffer::Sniffer(const string& id) : TCPSniffer(id) {

        stream.ignore_server_data();
        stream.auto_cleanup_client_data(false);
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

    Sniffer::~Sniffer() {
        cs::SNIFFER_MANAGER.erase_sniffer(id_);
    }

}
