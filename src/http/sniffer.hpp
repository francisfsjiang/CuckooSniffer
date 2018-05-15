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

        virtual void on_client_payload(const cs::base::PayloadType&);

        virtual void on_server_payload(const cs::base::PayloadType&);

        virtual void on_connection_close();

        virtual void on_connection_terminated(cs::base::TerminationReason);

        Sniffer(const std::string&);

        virtual ~Sniffer();

    private:

        cs::util::Buffer* client_buffer_;
        HTTPRequestParser* client_parser_;
        cs::util::Buffer* server_buffer_;
        HTTPResponseParser* server_parser_;


    };

}


#endif //CUCKOOSNIFFER_HTTP_SNIFFER_HPP
