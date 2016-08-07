#include "samba/sniffer.hpp"

#include <vector>
#include <sstream>
#include <iomanip>
#include <codecvt>

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

    uint32_t header_length = vec[8] + (vec[9] << 8);
    uint32_t command = vec[16] + (vec[17] << 8);
    uint32_t flags = 0;
    for (int i = 0, carry = 0; i < 4; ++i, carry += 8) {
        flags += vec[20+i] << carry;
    }

    bool resp_flag = static_cast<bool>(flags & 1); // 1 Response 0 Request
    bool DFS_op_flag = static_cast<bool>(flags & 1<<28);

    uint64_t msg_id = 0;
    for (int i = 0, carry = 0; i < 8; ++i, carry += 8) {
        msg_id += vec[28+i] << carry;
    }
//    LOG_TRACE << "SAMBA client msg header_len " << header_length;
//    LOG_TRACE << "SAMBA client msg command " << command;
//    LOG_TRACE << "SAMBA client msg req flag " << resp_flag;

    if (resp_flag || DFS_op_flag) {
        return;
    }

    //find create request
    else if (command == CREATE){

        uint64_t read_request_offset = header_length + 4;
        //check file_attributes
        if (vec[read_request_offset + 28] != 0x80) {
            return;
        }

        uint64_t file_name_offset = vec[read_request_offset + 44] + (vec[read_request_offset + 45] << 8);
        uint64_t file_name_len = vec[read_request_offset+ 46] + (vec[read_request_offset + 47] << 8);
        LOG_TRACE << "SAMBA client create msg";
        LOG_TRACE << "SAMBA client msg id " << msg_id;
        LOG_TRACE << "SAMBA client msg create req, file name offset " << file_name_offset;
        LOG_TRACE << "SAMBA client msg create req, file name len " << file_name_len;
        std::string file_name = get_file_name(vec, file_name_offset + 4, file_name_len);
        LOG_TRACE << "SAMBA client msg read req, file name " << file_name;
    }
    //find read request
    else if (command == READ ) {
        uint64_t read_request_offset = header_length + 4;
        uint64_t read_length = 0;
        for (int i = 0, carry = 0; i < 4; ++i, carry += 8) {
            read_length += vec[read_request_offset + 4 + i] << carry;
        }
        uint64_t read_offset = 0;
        for (int i = 0, carry = 1; i < 8; ++i, carry += 8) {
            read_offset += vec[read_request_offset + 8 + i] << carry;
        }
        std::string file_id = get_file_handle(vec, read_request_offset + 16);

        LOG_TRACE << "SAMBA client read msg";
        LOG_TRACE << "SAMBA client msg id " << msg_id;
        LOG_TRACE << "SAMBA client msg read req, read length " << read_length;
        LOG_TRACE << "SAMBA client msg read req, read offset " << read_offset;
        LOG_TRACE << "SAMBA client msg read req, read file id " << file_id;

    }
}


std::string Sniffer::get_file_handle(const std::vector<uint8_t> vec, uint64_t offset) {

    std::ostringstream ostr;
    ostr << std::right << std::hex << std::setfill('0');
    for (uint64_t i = offset + 3;  i >= offset; --i) {
        ostr << std::setw(2) << static_cast<int>(vec[i]);
    }
    ostr << "-";
    for (uint64_t i = offset + 5;  i >= offset + 4; --i) {
        ostr << std::setw(2) << static_cast<int>(vec[i]);
    }
    ostr << "-";
    for (uint64_t i = offset + 7;  i >= offset + 6; --i) {
        ostr << std::setw(2) << static_cast<int>(vec[i]);
    }
    ostr << "-";
    for (uint64_t i = offset + 8; i <= offset + 9; ++i) {
        ostr << std::setw(2) << static_cast<int>(vec[i]);
    }
    ostr << "-";
    for (uint64_t i = offset + 10; i <= offset + 15; ++i) {
        ostr << std::setw(2) << static_cast<int>(vec[i]);
    }
    return ostr.str();
}

std::string Sniffer::get_file_name(const std::vector<uint8_t> vec, uint64_t offset, uint64_t len) {
    std::vector<wchar_t> tmp;
    for (uint64_t i = offset; i < offset + len; i += 2) {
        tmp.push_back(vec[i] + (vec[i + 1] << 8));
    }
    std::wstring ws = std::wstring(tmp.begin(), tmp.end());
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cov;
    return cov.to_bytes(ws);
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

    uint32_t header_length = vec[8] + (vec[9] << 8);
    uint32_t command = vec[16] + (vec[17] << 8);
    uint32_t flags = 0;
    for (int i = 0, carry = 0; i < 4; ++i, carry += 8) {
        flags += vec[20+i] << carry;
    }

    bool resp_flag = static_cast<bool>(flags & 1); // 1 Response 0 Request

    uint64_t msg_id = 0;
    for (int i = 0, carry = 0; i < 8; ++i, carry += 8) {
        msg_id += vec[28+i] << carry;
    }
//    LOG_TRACE << "SAMBA server msg header_len " << header_length;
//    LOG_TRACE << "SAMBA server msg command " << command;
//    LOG_TRACE << "SAMBA server msg flag " << req_resp_flag;
//    LOG_TRACE << "SAMBA server msg id" << msg_id;

    if (!resp_flag) {
        return;
    }

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
