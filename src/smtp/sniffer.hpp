#ifndef CUCKOOSNIFFER_SMTP_SNIFFER_HPP
#define CUCKOOSNIFFER_SMTP_SNIFFER_HPP

#include "base/sniffer.hpp"

namespace cs::smtp {

class Sniffer : public cs::base::TCPSniffer {

public:

    virtual void on_client_payload(const cs::base::PayloadType&);

    virtual void on_server_payload(const cs::base::PayloadType&);

    virtual void on_connection_close();

    virtual void on_connection_terminated(cs::base::TerminationReason);

    Sniffer(const std::string&);

    virtual ~Sniffer();

private:

};

}


#endif //CUCKOOSNIFFER_SMTP_SNIFFER_HPP
