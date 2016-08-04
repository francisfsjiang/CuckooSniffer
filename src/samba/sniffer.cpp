#include "samba/sniffer.hpp"

#include <vector>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "samba/collected_data.hpp"
#include "samba/data_processor.hpp"

namespace cs {
namespace samba {

void Sniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {
    std::vector<uint8_t> vec = std::vector<uint8_t>(
            stream.client_payload().begin(),
            stream.client_payload().end()
    );
    uint32_t total_length = 0;
    if(vec[0] == 0) {
        total_length = (vec[1] << 16) + (vec[2] << 8) + vec[3];
        LOG_TRACE << "SAMBA client msg len " << total_length;
    }
    else {
        return;
    }

    uint32_t header_length = vec[8];
    uint32_t command = vec[16];
    uint32_t flags = 0;
    for (int i = 0, carry = 0; i < 8; ++i, carry += 8) {
        flags += vec[20+i] << carry;
    }

    bool req_resp_flag = static_cast<bool>(flags & 1); // 1 Response 2 Request

    uint64_t msg_id = 0;
    for (int i = 0, carry = 1; i < 8; ++i, carry += 8) {
        msg_id += vec[28+i] << carry;
    }
    LOG_TRACE << "SAMBA client msg header_len " << header_length;
    LOG_TRACE << "SAMBA client msg command " << command;
    LOG_TRACE << "SAMBA client msg req flag " << req_resp_flag;
    LOG_TRACE << "SAMBA client msg id " << msg_id;

    //find read request

    if (command == READ && req_resp_flag == 0) {
        uint64_t read_request_offset = header_length;
        uint64_t read_length = 0;
        for (int i = 0, carry = 1; i < 4; ++i, carry += 8) {
            read_length += vec[read_request_offset + 4 + i] << carry;
        }
        uint64_t read_offset = 0;
        for (int i = 0, carry = 1; i < 8; ++i, carry += 8) {
            read_offset += vec[read_request_offset + 8 + i] << carry;
        }
        std::string file_id = std::string(
                reinterpret_cast<char*>(vec.data() + read_request_offset + 16),
                16
        );
        LOG_TRACE << "SAMBA client msg read req, read length " << read_length;
        LOG_TRACE << "SAMBA client msg read req, read offset " << read_offset;
        LOG_TRACE << "SAMBA client msg read req, read file id " << file_id;

    }

    //find write request
//    else if (command == WIRTE){
//
//    }
}

void Sniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    //find read response
    std::vector<uint8_t> vec = std::vector<uint8_t>(
            stream.server_payload().begin(),
            stream.server_payload().end()
    );
    uint32_t total_length = 0;
    if(vec[0] == 0) {
        total_length = (vec[1] << 16) + (vec[2] << 8) + vec[3];
        LOG_TRACE << "SAMBA server msg len " << total_length;
    }
    else {
        return;
    }

    uint32_t header_length = vec[8];
    uint32_t command = vec[16];
    uint32_t flags = 0;
    for (int i = 0, carry = 0; i < 8; ++i, carry += 8) {
        flags += vec[20+i] << carry;
    }

    bool req_resp_flag = static_cast<bool>(flags & 1); // 1 Response 2 Request

    uint64_t msg_id = 0;
    for (int i = 0, carry = 1; i < 8; ++i, carry += 8) {
        msg_id += vec[28+i] << carry;
    }
    LOG_TRACE << "SAMBA server msg header_len " << header_length;
    LOG_TRACE << "SAMBA server msg command " << command;
    LOG_TRACE << "SAMBA server msg flag " << req_resp_flag;
    LOG_TRACE << "SAMBA server msg id" << msg_id;

}

void Sniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    LOG_DEBUG << "SAMBA onnection Close";

//    CollectedData *samba_data = new CollectedData(
//            std::string(stream.client_payload().begin(), stream.client_payload().end())
//    );
//
//    DataProcessor processor;
//    processor.process(*((CollectedData *) samba_data));
//
//
//    delete samba_data;
    SNIFFER_MANAGER.erase_sniffer(id_);
}

void Sniffer::on_connection_terminated(
        Tins::TCPIP::Stream &,
        Tins::TCPIP::StreamFollower::TerminationReason) {

    LOG_DEBUG << id_ << " SAMBA connection terminated.";
    SNIFFER_MANAGER.erase_sniffer(id_);
}

Sniffer::Sniffer(Tins::TCPIP::Stream &stream) : TCPSniffer(stream) {

    stream.client_data_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this->on_client_payload(tcp_stream);
            }
    );

    stream.server_data_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this->on_server_payload(tcp_stream);
            }
    );

    stream.stream_closed_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this->on_connection_close(tcp_stream);
            }
    );

}

Sniffer::~Sniffer() {

}

}
}
