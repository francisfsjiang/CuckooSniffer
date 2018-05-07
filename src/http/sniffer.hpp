#ifndef CUCKOOSNIFFER_HTTP_SNIFFER_HPP
#define CUCKOOSNIFFER_HTTP_SNIFFER_HPP

#include "base/sniffer.hpp"
namespace cs::util {
    class Buffer;
}

namespace cs::http {

class Sniffer : public cs::base::TCPSniffer {

public:

    virtual void on_client_payload(const cs::base::payload_type&);

    virtual void on_server_payload(const cs::base::payload_type&);

    virtual void on_connection_close();

    virtual void on_connection_terminated(cs::base::termination_reason);

    Sniffer(const std::string&);

    virtual ~Sniffer();

private:

    cs::util::Buffer* client_buffer_;
    cs::util::Buffer* server_buffer_;

};

}


#endif //CUCKOOSNIFFER_HTTP_SNIFFER_HPP
