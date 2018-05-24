#include "smtp/sniffer.hpp"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/buffer.hpp"
#include "util/function.hpp"
#include "util/mail_process.hpp"

namespace cs::smtp {

    using namespace cs::util;
    using namespace cs::base;

    void Sniffer::on_client_payload(PayloadVector payload, size_t payload_size) {
        LOG_TRACE << "SMTP client data size :" << payload_size;
        client_buffer_->write(reinterpret_cast<char*>(payload), payload_size);
    }

    void Sniffer::on_server_payload(PayloadVector payload, size_t payload_size) {
        LOG_TRACE << "SMTP server data size :" << payload_size;
    }

    void Sniffer::on_connection_close() {
        LOG_DEBUG << stream_id_.to_string() << " " << "SMTP Connection Close" << std::endl;

        auto file_vec = mail_process(std::string(
                client_buffer_->data_to_read(),
                client_buffer_->size()
        ));

        submit_files_and_delete(file_vec);

    }

    void Sniffer::on_connection_terminated(
            TerminationReason) {
        LOG_DEBUG << stream_id_.to_string() << " SMTP data connection terminated.";
    }

    Sniffer::Sniffer(const cs::base::StreamIdentifier& stream_id, int thread_id) : TCPSniffer(stream_id, thread_id) {

        client_buffer_ = new Buffer();

    }

    Sniffer::~Sniffer() {
        delete client_buffer_;
    }

}
