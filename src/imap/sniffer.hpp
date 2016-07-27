#ifndef CUCKOOSNIFFER_IMAP_SNIFFER_HPP
#define CUCKOOSNIFFER_IMAP_SNIFFER_HPP

#include "base/sniffer.hpp"
#include "imap/collected_data.hpp"

namespace cs { namespace imap {

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

    enum Status {
        NONE,
        MULTI,
        PART
    };

    Status status_ = Status::NONE;

    CollectedData *sniffer_data_ = nullptr;

};

}}

#endif //CUCKOOSNIFFER_IMAP_SNIFFER_HPP
