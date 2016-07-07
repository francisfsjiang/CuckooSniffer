#ifndef CUCKOOSNIFFER_SNIFFER_HPP
#define CUCKOOSNIFFER_SNIFFER_HPP

#include "tins/tcp_ip/stream_follower.h"

#include "tins/ip_address.h"
#include "tins/ipv6_address.h"


class Sniffer {

public:

    virtual ~Sniffer() {};

    inline std::string get_id() {
        return id_;
    }

protected:

    std::string id_;

};


class TCPSniffer: public Sniffer {

public:

    virtual void on_client_payload(const Tins::TCPIP::Stream&) = 0;

    virtual void on_server_payload(const Tins::TCPIP::Stream&) = 0;

    virtual void on_connection_close(const Tins::TCPIP::Stream&) = 0;

    virtual void on_connection_terminated(
            Tins::TCPIP::Stream&,
            Tins::TCPIP::StreamFollower::TerminationReason) = 0;

    virtual ~TCPSniffer(){};

    inline static std::string client_endpoint(const Tins::TCPIP::Stream& stream) {
        std::ostringstream output;
        if (stream.is_v6()) {
            output << stream.client_addr_v6();
        }
        else {
            output << stream.client_addr_v4();
        }
        output << ":" << stream.client_port();
        return output.str();
    }


    inline static std::string server_endpoint(const Tins::TCPIP::Stream& stream) {
        std::ostringstream output;
        if (stream.is_v6()) {
            output << stream.server_addr_v6();
        }
        else {
            output << stream.server_addr_v4();
        }
        output << ":" << stream.server_port();
        return output.str();
    }

    inline static std::string stream_identifier(const Tins::TCPIP::Stream& stream) {
        std::ostringstream output;
        output << client_endpoint(stream) << "-" << server_endpoint(stream);
        return output.str();
    }
};

#include "smtp/smtp_sniffer.hpp"


#endif //CUCKOOSNIFFER_SNIFFER_HPP
