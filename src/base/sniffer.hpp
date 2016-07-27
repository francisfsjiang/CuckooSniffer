#ifndef CUCKOOSNIFFER_BASE_SNIFFER_HPP
#define CUCKOOSNIFFER_BASE_SNIFFER_HPP

#include "tins/tcp_ip/stream_follower.h"
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"

namespace cs {
namespace base {

class Sniffer {

public:

    virtual ~Sniffer();

    const std::string &get_id();

protected:

    std::string id_;

};


class TCPSniffer : public Sniffer {

public:

    virtual void on_client_payload(const Tins::TCPIP::Stream &) = 0;

    virtual void on_server_payload(const Tins::TCPIP::Stream &) = 0;

    virtual void on_connection_close(const Tins::TCPIP::Stream &) = 0;

    virtual void on_connection_terminated(
            Tins::TCPIP::Stream &,
            Tins::TCPIP::StreamFollower::TerminationReason) = 0;

    TCPSniffer() = delete;

    TCPSniffer(Tins::TCPIP::Stream&);

    virtual ~TCPSniffer() {};

    static std::string client_endpoint(const Tins::TCPIP::Stream &stream);

    static std::string server_endpoint(const Tins::TCPIP::Stream &stream);

    static std::string stream_identifier(const Tins::TCPIP::Stream &stream);
};

}
}
#endif //CUCKOOSNIFFER_BASE_SNIFFER_HPP
