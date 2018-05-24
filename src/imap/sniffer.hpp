#ifndef CUCKOOSNIFFER_IMAP_SNIFFER_HPP
#define CUCKOOSNIFFER_IMAP_SNIFFER_HPP

#include "base/sniffer.hpp"
namespace cs::util {
    class Buffer;
}

namespace cs::imap {

class Sniffer : public cs::base::TCPSniffer {

public:

    void on_client_payload(cs::base::PayloadVector, size_t) override;

    void on_server_payload(cs::base::PayloadVector, size_t) override;

    void on_connection_close() override;

    void on_connection_terminated(cs::base::TerminationReason) override;

    Sniffer(const cs::base::StreamIdentifier&, int);

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
