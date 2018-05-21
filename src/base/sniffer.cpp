#include "base/sniffer.hpp"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/function.hpp"

namespace cs::base {

    std::hash<std::string> hash_fn;

    StreamIdentifier::StreamIdentifier(const std::string& src_addr, size_t src_port, const std::string& dst_addr, size_t dst_port) {
        this->src_addr = src_addr;
        this->src_port = src_port;
        this->dst_addr = dst_addr;
        this->dst_port = dst_port;
        char buffer[100];
        sprintf(buffer, "%s%zu%s%zu\0", src_addr.data(), src_port, dst_addr.data(), dst_port);
        this->hash_key = hash_fn(std::string(buffer));
    }
    StreamIdentifier::StreamIdentifier(const std::string& src_addr, size_t src_port, const std::string& dst_addr, size_t dst_port, uint32_t hash_key) {
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

    Sniffer::~Sniffer() {
        LOG_TRACE << "Sniffer deconstructor invoked ";
    }

    const StreamIdentifier& Sniffer::get_id() {
        return stream_id_;
    }

    Sniffer::Sniffer(const StreamIdentifier& stream_id): stream_id_(stream_id){
        LOG_TRACE << "Sniffer constructor invoked ";
    }

    TCPSniffer::TCPSniffer(const StreamIdentifier& id):
            Sniffer::Sniffer(id)
    {
    }

}
