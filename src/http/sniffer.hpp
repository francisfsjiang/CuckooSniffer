#ifndef CUCKOOSNIFFER_HTTP_SNIFFER_HPP
#define CUCKOOSNIFFER_HTTP_SNIFFER_HPP

#include "base/sniffer.hpp"
namespace cs::util {
    class Buffer;
}

namespace cs::http {

    class HTTPRequestParser;
    class HTTPResponseParser;

    class Sniffer : public cs::base::TCPSniffer {

    public:

        void on_client_payload(cs::base::PayloadVector, size_t) override;

        void on_server_payload(cs::base::PayloadVector, size_t) override;

        void on_connection_close() override ;

        void on_connection_terminated(cs::base::TerminationReason) override ;

        Sniffer(const cs::base::StreamIdentifier&, int);

        ~Sniffer() override ;

    private:

        cs::util::Buffer* client_buffer_;
        HTTPRequestParser* client_parser_;
        cs::util::Buffer* server_buffer_;
        HTTPResponseParser* server_parser_;

        std::string request_url_;


    };

}


#endif //CUCKOOSNIFFER_HTTP_SNIFFER_HPP
