#ifndef CUCKOOSNIFFER_SMTP_SNIFFER_HPP
#define CUCKOOSNIFFER_SMTP_SNIFFER_HPP

#include "base/sniffer.hpp"

namespace cs::util {
    class Buffer;
}

namespace cs::smtp {

class Sniffer : public cs::base::TCPSniffer {

public:

    void on_client_payload(cs::base::PayloadVector, size_t) override;

    void on_server_payload(cs::base::PayloadVector, size_t) override;

    void on_connection_close() override;

    void on_connection_terminated(cs::base::TerminationReason) override;

    Sniffer(const cs::base::StreamIdentifier&, int);

    virtual ~Sniffer();

private:

    cs::util::Buffer* client_buffer_;
    bool start_data_;
    bool quitted_;
};

}


#endif //CUCKOOSNIFFER_SMTP_SNIFFER_HPP
