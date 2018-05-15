#ifndef CUCKOOSNIFFER_BASE_SNIFFER_HPP
#define CUCKOOSNIFFER_BASE_SNIFFER_HPP

#include "tins/tcp_ip/stream_follower.h"
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"

namespace cs::base {

    typedef Tins::TCPIP::Stream::payload_type PayloadType;
    typedef int TerminationReason;

    class Sniffer {

    public:

        Sniffer(const std::string&);

        virtual ~Sniffer();

        const std::string &get_id();

        virtual void on_client_payload(const PayloadType&) = 0;

        virtual void on_server_payload(const PayloadType&) = 0;

    protected:

        std::string id_;

    };


    class TCPSniffer : public Sniffer {

    public:

        virtual void on_client_payload(const PayloadType&) = 0;

        virtual void on_server_payload(const PayloadType&) = 0;

        virtual void on_connection_close() = 0;

        virtual void on_connection_terminated(TerminationReason) = 0;

        TCPSniffer() = delete;

        TCPSniffer(const std::string&);

        virtual ~TCPSniffer() {};
    };

}

#endif //CUCKOOSNIFFER_BASE_SNIFFER_HPP
