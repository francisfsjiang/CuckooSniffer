#ifndef CUCKOOSNIFFER_HTTP_SNIFFER_HPP
#define CUCKOOSNIFFER_HTTP_SNIFFER_HPP

#include "base/sniffer.hpp"
namespace cs::util {
    class Buffer;
}

namespace cs::http {

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

    cs::util::Buffer* client_buffer_;
    cs::util::Buffer* server_buffer_;

};

}


#endif //CUCKOOSNIFFER_HTTP_SNIFFER_HPP
