#ifndef CUCKOOSNIFFER_IMAP_SNIFFER_HPP
#define CUCKOOSNIFFER_IMAP_SNIFFER_HPP

#include "base/sniffer.hpp"
namespace cs::util {
    class Buffer;
}

namespace cs::imap {

class Sniffer : public cs::base::TCPSniffer {

public:

    virtual void on_client_payload(const cs::base::payload_type&);

    virtual void on_server_payload(const cs::base::payload_type&);

    virtual void on_connection_close();

    virtual void on_connection_terminated(cs::base::termination_reason);

    Sniffer(const std::string&);

    virtual ~Sniffer();

private:

    enum Status {
        NONE,
        MULTI,
        PART
    };

    Status status_ = Status::NONE;

    cs::util::Buffer* sniffer_data_ = nullptr;

};

}

#endif //CUCKOOSNIFFER_IMAP_SNIFFER_HPP
