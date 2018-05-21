#ifndef CUCKOOSNIFFER_BASE_SNIFFER_HPP
#define CUCKOOSNIFFER_BASE_SNIFFER_HPP

#include <cstdint>
#include <vector>
#include <string>

namespace cs::base {

    class StreamIdentifier {
    public:
        StreamIdentifier(const std::string&, size_t, const std::string&, size_t);
        StreamIdentifier(const std::string&, size_t, const std::string&, size_t, uint32_t);
        std::string src_addr;
        size_t src_port;
        std::string dst_addr;
        size_t dst_port;
        uint32_t hash_key;

        std::string to_string() const;
    };

    typedef std::vector<uint8_t> PayloadVector;
    typedef int TerminationReason;

    class Sniffer {

    public:

        Sniffer(const StreamIdentifier&);

        virtual ~Sniffer();

        const StreamIdentifier &get_id();

        virtual void on_client_payload(const PayloadVector&) = 0;

        virtual void on_server_payload(const PayloadVector&) = 0;

    protected:

        StreamIdentifier stream_id_;

    };


    class TCPSniffer : public Sniffer {

    public:

        virtual void on_client_payload(const PayloadVector&) = 0;

        virtual void on_server_payload(const PayloadVector&) = 0;

        virtual void on_connection_close() = 0;

        virtual void on_connection_terminated(TerminationReason) = 0;

        TCPSniffer() = delete;

        TCPSniffer(const StreamIdentifier&);

        virtual ~TCPSniffer() {};
    };

}

#endif //CUCKOOSNIFFER_BASE_SNIFFER_HPP
