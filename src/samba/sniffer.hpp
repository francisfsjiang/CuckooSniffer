#ifndef CUCKOOSNIFFER_SAMBA_SNIFFER_HPP
#define CUCKOOSNIFFER_SAMBA_SNIFFER_HPP

#include "sniffer.hpp"

namespace cs::samba {

class Sniffer : public cs::base::TCPSniffer {

public:

    virtual void on_client_payload(const Tins::TCPIP::Stream &);

    virtual void on_server_payload(const Tins::TCPIP::Stream &);

    virtual void on_connection_close(const Tins::TCPIP::Stream &);

    virtual void on_connection_terminated(
            Tins::TCPIP::Stream &,
            Tins::TCPIP::StreamFollower::TerminationReason);

    Sniffer(Tins::TCPIP::Stream &);

    ~Sniffer();

private:

};

}


#endif //CUCKOOSNIFFER_SAMBA_SNIFFER_HPP
