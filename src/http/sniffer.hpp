#ifndef CUCKOOSNIFFER_HTTP_SNIFFER_HPP
#define CUCKOOSNIFFER_HTTP_SNIFFER_HPP

#include "base/sniffer.hpp"

namespace cs {
namespace http {

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

    std::string data_;
};

}
}


#endif //CUCKOOSNIFFER_HTTP_SNIFFER_HPP
