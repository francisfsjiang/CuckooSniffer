#include "samba/sniffer.hpp"

#include <vector>
#include <sstream>
#include <iomanip>
#include <codecvt>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/file.hpp"
#include "samba/collected_data.hpp"
#include "samba/data_processor.hpp"

namespace cs {
namespace samba {

void Sniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {
    if (stream.client_payload().size() <= 0) {
        return;
    }
    std::vector<uint8_t> vec = std::vector<uint8_t>(
            stream.client_payload().begin(),
            stream.client_payload().end()
    );
    uint64_t total_length = 0;
    if(vec[0] == 0) {
        total_length = (vec[1] << 16) + (vec[2] << 8) + vec[3];
        LOG_TRACE << "SAMBA client req len " << total_length;
    }
    else {
        return;
    }

    uint64_t header_length = vec[8] + (vec[9] << 8);
    uint64_t command = vec[16] + (vec[17] << 8);
    uint64_t flags = get_number(vec, 20, 4);
//    for (int i = 0, carry = 0; i < 4; ++i, carry += 8) {
//        flags += vec[20+i] << carry;
//    }

    bool resp_flag = static_cast<bool>(flags & 1); // 1 Response 0 Request
    bool DFS_op_flag = static_cast<bool>(flags & 1<<28);

    uint64_t msg_id = get_number(vec, 28, 8);
//    for (int i = 0, carry = 0; i < 8; ++i, carry += 8) {
//        msg_id += vec[28+i] << carry;
//    }

    if (resp_flag || DFS_op_flag) {
        return;
    }

    //find create request
    if (command == CREATE){

        uint64_t create_req_offset = header_length + 4;
        //check file_attributes
        if (vec[create_req_offset + 28] != 0x80) {
            return;
        }
        uint64_t access_mask = get_number(vec, create_req_offset + 24, 4);
        if (!(access_mask&1)) {
            return;
        }

        uint64_t file_name_offset = vec[create_req_offset + 44] + (vec[create_req_offset + 45] << 8);
        uint64_t file_name_len = vec[create_req_offset+ 46] + (vec[create_req_offset + 47] << 8);
        std::string file_name = get_file_name(vec, file_name_offset + 4, file_name_len);
        if (file_name.find(':') != std::string::npos) {
            return;
        }
        create_req_map_.insert(std::make_pair(msg_id, file_name));
        LOG_TRACE << "SAMBA client create req, id " << msg_id;
        LOG_TRACE << "SAMBA client create req, file name " << file_name;
    }
    //find read request
    else if (command == READ ) {
        uint64_t read_req_offset = header_length + 4;
        uint64_t read_length = get_number(vec, read_req_offset + 4, 4);
//        for (int i = 0, carry = 0; i < 4; ++i, carry += 8) {
//            read_length += vec[read_req_offset + 4 + i] << carry;
//        }
        uint64_t read_offset = get_number(vec, read_req_offset + 8, 8);
//        for (int i = 0, carry = 1; i < 8; ++i, carry += 8) {
//            read_offset += vec[read_req_offset + 8 + i] << carry;
//        }
        std::string file_id = get_file_handle(vec, read_req_offset + 16);

        LOG_TRACE << "SAMBA client read req, id " << msg_id;
        LOG_TRACE << "SAMBA client read req, read length " << read_length;
        LOG_TRACE << "SAMBA client read req, read offset " << read_offset;
        LOG_TRACE << "SAMBA client read req, read file id " << file_id;

    }
}


void Sniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    if (stream.server_payload().size() <= 0) {
        return;
    }

    LOG_TRACE << "SAMBA server payload len " << stream.server_payload().size();
    uint64_t payload_len = stream.server_payload().size();

    if (server_payload_remain_ > 0) {
        server_resp_file_ -> write(
                reinterpret_cast<const char*>(stream.server_payload().data()),
                payload_len
        );
        server_payload_remain_ -= payload_len;
        LOG_TRACE << "SAMBA server read resp, file remain " << server_payload_remain_;
        LOG_TRACE << "SAMBA server read resp, file size " << server_resp_file_ -> get_size();

        if (server_payload_remain_ == 0) {
            CollectedData* collected_data = new CollectedData(server_resp_file_);
            DATA_QUEUE.enqueue(collected_data);
            server_resp_file_ = nullptr;
        }

        return;
    }


    //find read response
    std::vector<uint8_t> vec = std::vector<uint8_t>(
            stream.server_payload().begin(),
            stream.server_payload().end()
    );
    uint64_t total_length = 0;
    if(vec[0] == 0) {
        total_length = (vec[1] << 16) + (vec[2] << 8) + vec[3];
        LOG_TRACE << "SAMBA server resp len " << total_length;
    }
    else {
        return;
    }

    uint64_t header_length = vec[8] + (vec[9] << 8);
    uint64_t command = vec[16] + (vec[17] << 8);
    uint64_t flags = get_number(vec, 20, 4);

    bool resp_flag = static_cast<bool>(flags & 1); // 1 Response 0 Request
    bool DFS_op_flag = static_cast<bool>(flags & 1<<28);

    uint64_t msg_id = get_number(vec, 28, 8);
    LOG_TRACE << "SAMBA server msg id" << msg_id;

    if (!resp_flag || DFS_op_flag) {
        return;
    }
    //find create response
    if (command == CREATE){
        if (create_req_map_.find(msg_id) == create_req_map_.end()) {
            return;
        }
        uint64_t create_resp_offset = header_length + 4;
        uint64_t struct_size = get_number(vec, create_resp_offset, 2);
        if (struct_size < 80) {
            return;
        }

        uint64_t file_attributes = get_number(vec, create_resp_offset + 56, 4);
        bool is_dir = static_cast<bool>(file_attributes & (1 << 4));
        if (is_dir) {
            return;
        }


        uint64_t file_allocation_size = get_number(vec, create_resp_offset + 40, 8);
        uint64_t file_eof = get_number(vec, create_resp_offset + 48, 8);

        std::string file_id = get_file_handle(vec, create_resp_offset + 64);

        LOG_TRACE << "SAMBA server create resp, id " << msg_id;
        LOG_TRACE << "SAMBA server create resp, file allloc size " << file_allocation_size;
        LOG_TRACE << "SAMBA server create resp, file eof " << file_eof;
        LOG_TRACE << "SAMBA server create resp, file id " << file_id;
    }
        //find read response
    else if (command == READ ) {
        uint64_t read_resp_offset = header_length + 4;
        uint64_t data_offset = vec[read_resp_offset + 2] + (vec[read_resp_offset + 3] << 8);
        uint64_t read_len = get_number(vec, read_resp_offset + 4, 4);
        uint64_t read_remain = get_number(vec, read_resp_offset + 8, 8);

        char *p_data = reinterpret_cast<char *>(vec.data() + data_offset + 4);

        LOG_TRACE << "SAMBA server read resp, id " << msg_id;
        LOG_TRACE << "SAMBA server read resp, read length " << read_len;
        LOG_TRACE << "SAMBA server read resp, read remain " << read_remain;

        server_resp_file_ = new cs::util::File();
        server_payload_remain_ = read_len - (payload_len - data_offset - 4);
        server_resp_file_->write(p_data, payload_len - data_offset - 4);
        LOG_TRACE << "SAMBA server read resp, file remain " << server_payload_remain_;
        LOG_TRACE << "SAMBA server read resp, file size " << server_resp_file_ -> get_size();
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

    server_payload_remain_ = 0;

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

std::string Sniffer::get_file_handle(const std::vector<uint8_t>& vec, uint64_t offset) {

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

std::string Sniffer::get_file_name(const std::vector<uint8_t>& vec, uint64_t offset, uint64_t len) {
    std::vector<wchar_t> tmp;
    for (uint64_t i = offset; i < offset + len; i += 2) {
        tmp.push_back(vec[i] + (vec[i + 1] << 8));
    }
    std::wstring ws = std::wstring(tmp.begin(), tmp.end());
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cov;
    return cov.to_bytes(ws);
}

uint64_t Sniffer::get_number(const std::vector<uint8_t>& vec, uint64_t offset, uint64_t len) {
    uint64_t ret = 0, s = offset, t = offset + len;
    for (uint64_t i = s, carry = 0; i < t; ++i, carry += 8) {
        ret += vec[i] << carry;
    }
    return ret;
}


}
}
