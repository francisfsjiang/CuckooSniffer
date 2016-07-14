#ifndef CUCKOOSNIFFER_IMAP_SNIFFER_HPP
#define CUCKOOSNIFFER_IMAP_SNIFFER_HPP

#include "sniffer.hpp"
#include "imap/imap_sniffer_data.hpp"

class IMAPSniffer: public TCPSniffer{

public:

    virtual void on_client_payload(const Tins::TCPIP::Stream&);

    virtual void on_server_payload(const Tins::TCPIP::Stream&);

    virtual void on_connection_close(const Tins::TCPIP::Stream&);

    virtual void on_connection_terminated(
            Tins::TCPIP::Stream&,
            Tins::TCPIP::StreamFollower::TerminationReason);

    IMAPSniffer(Tins::TCPIP::Stream&);
    ~IMAPSniffer();

private:

    enum Status {
        NONE,
        MULTI,
        PART
    };

    Status status_;

    IMAPSnifferData* sniffer_data_ = nullptr;

};


#endif //CUCKOOSNIFFER_IMAP_SNIFFER_HPP
