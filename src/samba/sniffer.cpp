#include "samba/sniffer.hpp"

#include <vector>
#include <sstream>
#include <iomanip>
#include <codecvt>
#include <set>

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/file.hpp"
#include "samba/collected_data.hpp"
#include "samba/data_processor.hpp"

namespace cs {
namespace samba {

void Sniffer::on_client_payload(const Tins::TCPIP::Stream& stream) {
    std::vector<uint8_t> vec = std::vector<uint8_t>(stream.client_payload());
    while (vec.size() > 0) {
        vec = handle_client_NB_block(vec);
    }
}

std::vector<uint8_t> Sniffer::handle_client_NB_block(const std::vector<uint8_t>& vec) {
    uint64_t payload_len = vec.size();
    if (client_NB_block_remain_ <=0) {
        uint64_t block_size = (vec[1] << 16) + (vec[2] << 8) + vec[3];
        if (block_size <= payload_len) {
            handle_client_req(std::vector<uint8_t>(
                    vec.begin() + 4,
                    vec.begin() + 4 + block_size
            ));
            return std::vector<uint8_t>(
                    vec.begin() + 4 + block_size,
                    vec.end()
            );
        }
        else {
            client_NB_block_remain_ = block_size - (payload_len - 4);
            client_NB_block_.insert(
                    client_NB_block_.end(),
                    vec.begin() + 4,
                    vec.end()
            );
            return std::vector<uint8_t>();
        }
    }
    else {
        if (client_NB_block_remain_ <= payload_len) {
            client_NB_block_.insert(
                    client_NB_block_.end(),
                    vec.begin(),
                    vec.begin() + client_NB_block_remain_
            );
            handle_client_req(client_NB_block_);
            client_NB_block_.clear();
            uint64_t tmp = client_NB_block_remain_;
            client_NB_block_remain_ = 0;
            return std::vector<uint8_t>(
                    vec.begin() + tmp,
                    vec.end()
            );
        }
        else {
            client_NB_block_.insert(
                    client_NB_block_.end(),
                    vec.begin(),
                    vec.begin() + payload_len
            );
            client_NB_block_remain_ -= payload_len;
            return std::vector<uint8_t>();
        }

    }
}

void Sniffer::handle_client_req(const std::vector<uint8_t>& vec) {

    uint64_t header_length = vec[4] + (vec[5] << 8);
    uint64_t command = vec[12] + (vec[13] << 8);
    uint64_t flags = get_number(vec, 16, 4);

    bool resp_flag = static_cast<bool>(flags & 1); // 1 Response 0 Request
    bool DFS_op_flag = static_cast<bool>(flags & 1<<28);

    uint64_t msg_id = get_number(vec, 24, 8);

    if (resp_flag || DFS_op_flag) {
        return;
    }

    //create request
    if (command == CREATE){
        uint64_t create_req_offset = header_length;
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
        std::string file_name = get_file_name(vec, file_name_offset, file_name_len);
        if (file_name.find(':') != std::string::npos) {
            return;
        }
        create_req_file_name_.insert(
                std::make_pair(
                        msg_id,
                        file_name
                )
        );
        LOG_TRACE << "SAMBA client create req, id " << msg_id;
        LOG_TRACE << "SAMBA client create req, file name " << file_name;
    }
    //read request
    else if (command == READ ) {
        uint64_t read_req_offset = header_length;
        uint64_t read_length = get_number(vec, read_req_offset + 4, 4);
        uint64_t read_offset = get_number(vec, read_req_offset + 8, 8);
        std::string file_id = get_file_handle(vec, read_req_offset + 16);

        read_req_map_.insert(std::make_pair(
                msg_id,
                std::make_tuple(
                        file_id,
                        read_length,
                        read_offset
                )
        ));

        LOG_TRACE << "SAMBA client read req, id " << msg_id;
        LOG_TRACE << "SAMBA client read req, read length " << read_length;
        LOG_TRACE << "SAMBA client read req, read offset " << read_offset;
        LOG_TRACE << "SAMBA client read req, read file id " << file_id;

    }
        //write request
    else if (command == WRITE) {

        uint64_t write_req_offset = header_length;
        uint64_t data_offset = get_number(vec, write_req_offset + 2, 2);
        uint64_t write_len = get_number(vec, write_req_offset + 4, 4);
        uint64_t write_offset = get_number(vec, write_req_offset + 8, 8);
        std::string file_id = get_file_handle(vec, write_req_offset + 16);

        if (file_info_.find(file_id) == file_info_.end()) {
            return;
        }

        char *p_data = new char[write_len];
        memcpy(
                p_data,
                reinterpret_cast<const char *>(vec.data() + data_offset),
                sizeof(char) * write_len
        );

        write_req_map_.insert(std::make_pair(
                msg_id,
                std::make_tuple(
                        file_id,
                        write_len,
                        write_offset,
                        p_data
                )
        ));

        LOG_TRACE << "SAMBA client write req, id " << msg_id;
        LOG_TRACE << "SAMBA client write req, read length " << write_len;
        LOG_TRACE << "SAMBA client write req, read offset " << write_offset;
        LOG_TRACE << "SAMBA client write req, read file id " << file_id;
    }
    //close request
    else if (command == CLOSE) {
        uint64_t close_req_offset = header_length;
        std::string file_id = get_file_handle(vec, close_req_offset + 8);
        LOG_TRACE << "SAMBA client close req, id " << msg_id;
        LOG_TRACE << "SAMBA client close req, file id " << file_id;
        close_msg_.insert(std::make_pair(
                msg_id,
                file_id
        ));
    }
}

void Sniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    std::vector<uint8_t> vec = std::vector<uint8_t>(stream.server_payload());
    while (vec.size() > 0) {
        vec = handle_server_NB_block(vec);
    }
}

std::vector<uint8_t> Sniffer::handle_server_NB_block(const std::vector<uint8_t>& vec) {
    uint64_t payload_len = vec.size();
    if (server_NB_block_remain_ <=0) {
        uint64_t block_size = (vec[1] << 16) + (vec[2] << 8) + vec[3];
        if (block_size <= payload_len) {
            handle_server_resp(std::vector<uint8_t>(
                    vec.begin() + 4,
                    vec.begin() + 4 + block_size
            ));
            return std::vector<uint8_t>(
                    vec.begin() + 4 + block_size,
                    vec.end()
            );
        }
        else {
            server_NB_block_remain_ = block_size - (payload_len - 4);
            server_NB_block_.insert(
                    server_NB_block_.end(),
                    vec.begin() + 4,
                    vec.end()
            );
            return std::vector<uint8_t>();
        }
    }
    else {
        if (server_NB_block_remain_ <= payload_len) {
            server_NB_block_.insert(
                    server_NB_block_.end(),
                    vec.begin(),
                    vec.begin() + server_NB_block_remain_
            );
            handle_server_resp(server_NB_block_);
            server_NB_block_.clear();
            uint64_t tmp = server_NB_block_remain_;
            server_NB_block_remain_ = 0;
            return std::vector<uint8_t>(
                    vec.begin() + tmp,
                    vec.end()
            );
        }
        else {
            server_NB_block_.insert(
                    server_NB_block_.end(),
                    vec.begin(),
                    vec.begin() + payload_len
            );
            server_NB_block_remain_ -= payload_len;
            return std::vector<uint8_t>();
        }
    }
}

void Sniffer::handle_server_resp(const std::vector<uint8_t>& vec) {

    uint64_t header_length = vec[4] + (vec[5] << 8);
    uint64_t command = vec[12] + (vec[13] << 8);
    uint64_t flags = get_number(vec, 16, 4);

    bool resp_flag = static_cast<bool>(flags & 1); // 1 Response 0 Request
    bool DFS_op_flag = static_cast<bool>(flags & 1<<28);

    uint64_t msg_id = get_number(vec, 24, 8);

    if (!resp_flag || DFS_op_flag) {
        return;
    }
    //create response
    if (command == CREATE){
        if (create_req_file_name_.find(msg_id) == create_req_file_name_.end()) {
            return;
        }
        uint64_t create_resp_offset = header_length;
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

        file_info_.insert(
                std::make_pair(
                        file_id,
                        std::make_pair(
                                create_req_file_name_[msg_id],
                                file_eof
                        )
                )
        );

        LOG_TRACE << "SAMBA server create resp, id " << msg_id;
        LOG_TRACE << "SAMBA server create resp, file allloc size " << file_allocation_size;
        LOG_TRACE << "SAMBA server create resp, file eof " << file_eof;
        LOG_TRACE << "SAMBA server create resp, file id " << file_id;
    }
        //read response
    else if (command == READ ) {
        uint64_t read_resp_offset = header_length;
        uint64_t data_offset = vec[read_resp_offset + 2] + (vec[read_resp_offset + 3] << 8);
        uint64_t read_len = get_number(vec, read_resp_offset + 4, 4);
        uint64_t read_remain = get_number(vec, read_resp_offset + 8, 8);

        char *p_data = new char[read_len];
        memcpy(
                p_data,
                reinterpret_cast<const char *>(vec.data() + data_offset),
                sizeof(char) * read_len
        );

        LOG_TRACE << "SAMBA server read resp, id " << msg_id;
        LOG_TRACE << "SAMBA server read resp, read length " << read_len;

        auto iter = read_req_map_.find(msg_id);
        auto tuple = iter -> second;
        rw_result_map_.insert(std::make_pair(
                std::get<0>(tuple),  //file id
                std::make_tuple(
                        std::get<1>(tuple),  //len
                        std::get<2>(tuple),  //offset
                        p_data
                )
        ));
        read_req_map_.erase(iter);
    }
        //write response
    else if (command == WRITE) {
        uint64_t nt_success = get_number(vec, 8, 4);
        if (nt_success != 0) {
            return;
        }
        auto iter = write_req_map_.find(msg_id);
        if (iter == write_req_map_.end()) {
            return;
        }
        LOG_TRACE << "SAMBA server write resp, id " << msg_id;
        auto tuple = iter -> second;
        rw_result_map_.insert(std::make_pair(
                std::get<0>(tuple),  //file id
                std::make_tuple(
                        std::get<1>(tuple),  //len
                        std::get<2>(tuple),  //offset
                        std::get<3>(tuple)
                )
        ));
        write_req_map_.erase(iter);
    }
        //close response
    else if (command == CLOSE) {
        LOG_TRACE << "SAMBA server close resp, id " << msg_id;
        uint64_t nt_success = get_number(vec, 8, 4);
        LOG_TRACE << "SAMBA server close resp, nt success " << nt_success;
        if (nt_success != 0) {
            return;
        }

        auto iter = close_msg_.find(msg_id);
        std::string file_id = iter -> second;
        close_msg_.erase(iter);

//        LOG_TRACE << "SAMBA server close resp, file id " << file_id;
//        for (auto i: rw_result_map_) {
//            LOG_TRACE << i.first;
//        }
        combine_data(file_id);

    }
}

void Sniffer::combine_data(const std::string& file_id) {

    auto range = rw_result_map_.equal_range(file_id);
    if (range.first == rw_result_map_.end()) {
        LOG_TRACE << "SAMBA server close resp, result_map is empty.";
        return;
    }
    std::pair<std::string, uint64_t>& i_file_info = file_info_[file_id];
    std::string file_name = i_file_info.first;

    uint64_t file_size = 0;
    for (auto i = range.first; i != range.second; ++i) {
        file_size += std::get<0>(i -> second);
    }
    if (file_size > 0) {

        LOG_TRACE << "Copying " << file_name << ",size " << file_size;
        cs::util::File* file = new cs::util::File();
        file -> set_name(file_name);
        file -> set_size(file_size);

        for (auto i = range.first; i != range.second; ++i) {
            uint64_t len = std::get<0>(i -> second);
            uint64_t offset = std::get<1>(i -> second);
            char* p_data = std::get<2>(i -> second);
            LOG_TRACE << "Offset " << offset << ", len " << len;
            file -> write_to_pos(p_data, len, offset);
            delete[] p_data;
        }

        DATA_QUEUE.enqueue(new CollectedData(
                file
        ));

    }
    rw_result_map_.erase(file_id);
}


void Sniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    LOG_DEBUG << "SAMBA onnection Close";

    std::set<std::string> s;
    for (auto& iter: rw_result_map_) {
        s.insert(
                iter.first
        );
    }
    for (auto &iter: s) {
        combine_data(iter);
    }

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
