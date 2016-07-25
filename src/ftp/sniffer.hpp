#ifndef CUCKOOSNIFFER_FTP_SNIFFER_HPP
#define CUCKOOSNIFFER_FTP_SNIFFER_HPP

#include "sniffer.hpp"

namespace cs::ftp {

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

    const std::map<unsigned short, std::string>& get_data_connection_pool();

private:
    static std::map<unsigned short, std::string> data_connection_pool_;

};

}


#endif //CUCKOOSNIFFER_FTP_SNIFFER_HPP