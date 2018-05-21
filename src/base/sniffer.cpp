#include "base/sniffer.hpp"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/function.hpp"
#include "threads/thread.hpp"
#include "threads/data_queue.hpp"

namespace cs::base {

    std::hash<std::string> hash_fn;

    StreamIdentifier::StreamIdentifier(const std::string& src_addr, uint16_t src_port, const std::string& dst_addr, uint16_t dst_port) {
        this->src_addr = src_addr;
        this->src_port = src_port;
        this->dst_addr = dst_addr;
        this->dst_port = dst_port;
        char buffer[100];
        sprintf(buffer, "%s%zu%s%zu\0", src_addr.data(), src_port, dst_addr.data(), dst_port);
        this->hash_key = hash_fn(std::string(buffer));
    }
    StreamIdentifier::StreamIdentifier(const std::string& src_addr, uint16_t src_port, const std::string& dst_addr, uint16_t dst_port, uint32_t hash_key) {
        this->src_addr = src_addr;
        this->src_port = src_port;
        this->dst_addr = dst_addr;
        this->dst_port = dst_port;
        this->hash_key = hash_key;
    }

    std::string StreamIdentifier::to_string() const {
        char buffer[100];
        sprintf(buffer, "%s:%zu->%s%zu;%u\0", src_addr.data(), src_port, dst_addr.data(), dst_port, hash_key);
        return std::string(buffer);
    }

//    Sniffer::~Sniffer() {
//        LOG_TRACE << "Sniffer deconstructor invoked ";
//    }

    const StreamIdentifier& TCPSniffer::get_id() {
        return stream_id_;
    }
//
//    Sniffer::Sniffer(const StreamIdentifier& stream_id, int thread_id):
//            stream_id_(stream_id),
//            thread_id_(thread_id){
//        LOG_TRACE << "Sniffer constructor invoked ";
//    }

    TCPSniffer::TCPSniffer(const StreamIdentifier& stream_id, int thread_id):
            stream_id_(stream_id),
            thread_id_(thread_id) {
        LOG_TRACE << "Sniffer constructing, " << this;
    }

    TCPSniffer::~TCPSniffer() {
        LOG_TRACE << "Sniffer deconstructing, " << this;
    }


    void TCPSniffer::data_callback(
            cs::base::PayloadVector stream_data,
            size_t stream_data_size,
            cs::threads::DataType type
    ) {
//        LOG_TRACE << "Callback: " << (int)type;

        cs::base::PayloadVector payload_ptr = nullptr;
        if (type == cs::threads::DataType::CLIENT_PAYLOAD) {
            payload_ptr = stream_data;
        }
        else if (type == cs::threads::DataType::SERVER_PAYLOAD) {
            payload_ptr = stream_data;
        }
        else {
            payload_ptr = nullptr;
        }

        auto de = new cs::threads::DataEvent(
                shared_from_this(), type, payload_ptr, stream_data_size
        );

        cs::threads::DATA_QUEUES[thread_id_]->enqueue(de);
    }

    void TCPSniffer::data_callback(
            std::shared_ptr<cs::base::TCPSniffer> sniffer_ptr,
            cs::base::PayloadVector stream_data,
            size_t stream_data_size,
            cs::threads::DataType type
    ) {
        sniffer_ptr->data_callback(stream_data, stream_data_size, type);
    }
}
