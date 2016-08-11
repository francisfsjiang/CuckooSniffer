#ifndef CUCKOOSNIFFER_SAMBA_SNIFFER_HPP
#define CUCKOOSNIFFER_SAMBA_SNIFFER_HPP

#include "base/sniffer.hpp"

namespace cs {

namespace util{

class File;

}

namespace samba {

class Sniffer : public cs::base::TCPSniffer {

public:

    virtual void on_client_payload(const Tins::TCPIP::Stream &);
    std::vector<uint8_t> handle_client_NB_block(const std::vector<uint8_t>&);
    void handle_client_req(const std::vector<uint8_t>&);

    virtual void on_server_payload(const Tins::TCPIP::Stream &);
    std::vector<uint8_t> handle_server_NB_block(const std::vector<uint8_t>&);
    void handle_server_resp(const std::vector<uint8_t>&);

    virtual void on_connection_close(const Tins::TCPIP::Stream &);

    virtual void on_connection_terminated(
            Tins::TCPIP::Stream &,
            Tins::TCPIP::StreamFollower::TerminationReason);

    Sniffer(Tins::TCPIP::Stream &);

    virtual ~Sniffer();

private:

    std::string get_file_handle(const std::vector<uint8_t>&, uint64_t);
    std::string get_file_name(const std::vector<uint8_t>&, uint64_t, uint64_t);
    uint64_t get_number(const std::vector<uint8_t>&, uint64_t, uint64_t);

    std::map<uint64_t, std::string> create_req_file_name_;
    std::map<uint64_t, std::string> close_msg_;

    std::map<std::string, std::pair<std::string, uint64_t> > file_info_;

    std::map<uint64_t, std::tuple<std::string, uint64_t, uint64_t> > read_req_map_;

    std::map<uint64_t, std::tuple<std::string, uint64_t, uint64_t, char*> > write_req_map_;

    std::multimap<std::string, std::tuple<uint64_t, uint64_t, char*> > rw_result_map_;

    uint64_t server_NB_block_remain_ = 0;
    std::vector<uint8_t> server_NB_block_;
    uint64_t client_NB_block_remain_ = 0;
    std::vector<uint8_t> client_NB_block_;

    enum COMMAND {
        CREATE=5,
        CLOSE=6,
        READ=8,
        WRITE=9

    };

};

}
}


#endif //CUCKOOSNIFFER_SAMBA_SNIFFER_HPP
