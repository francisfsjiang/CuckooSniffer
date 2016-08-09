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

    virtual void on_server_payload(const Tins::TCPIP::Stream &);

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

    std::map<uint64_t ,std::string> create_req_map_;
    uint64_t server_payload_remain_ = 0;
    util::File* server_resp_file_;

    enum COMMAND {
        CREATE=5,
        READ=8,

    };

};

}
}


#endif //CUCKOOSNIFFER_SAMBA_SNIFFER_HPP
