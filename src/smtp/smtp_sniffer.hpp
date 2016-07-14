#ifndef CUCKOOSNIFFER_SMTP_SNIFFER_HPP
#define CUCKOOSNIFFER_SMTP_SNIFFER_HPP

#include "sniffer.hpp"

class SMTPSniffer: public TCPSniffer{

public:

    virtual void on_client_payload(const Tins::TCPIP::Stream&);

    virtual void on_server_payload(const Tins::TCPIP::Stream&);

    virtual void on_connection_close(const Tins::TCPIP::Stream&);

    virtual void on_connection_terminated(
            Tins::TCPIP::Stream&,
            Tins::TCPIP::StreamFollower::TerminationReason);

    SMTPSniffer(Tins::TCPIP::Stream&);
    ~SMTPSniffer();

private:

};


#endif //CUCKOOSNIFFER_SMTP_SNIFFER_HPP
