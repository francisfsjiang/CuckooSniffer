#ifndef CUCKOOSNIFFER_BASE_SNIFFER_HPP
#define CUCKOOSNIFFER_BASE_SNIFFER_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

namespace cs::threads {
    enum class DataType;
}

namespace cs::base {

    class StreamIdentifier {
    public:
        StreamIdentifier();
        StreamIdentifier(const std::string&, uint16_t, const std::string&, uint16_t);
        StreamIdentifier(const std::string&, uint16_t, const std::string&, uint16_t, uint32_t);
        std::string src_addr;
        uint16_t src_port;
        std::string dst_addr;
        uint16_t dst_port;
        uint32_t hash_key;

        std::string to_string() const;
    };

    typedef uint8_t* PayloadVector;
    typedef int TerminationReason;

//    class Sniffer: std::enable_shared_from_this<Sniffer> {
//
//    public:
//
//        Sniffer(const StreamIdentifier&, int);
//
//        virtual ~Sniffer();
//
//        const StreamIdentifier &get_id();
//
//        virtual void on_client_payload(PayloadVector, size_t) = 0;
//
//        virtual void on_server_payload(PayloadVector, size_t) = 0;
//
//    protected:
//
//    };


//    class TCPSniffer : public Sniffer {
    class TCPSniffer : public std::enable_shared_from_this<TCPSniffer>  {
    protected:
        StreamIdentifier stream_id_;
        int thread_id_;

    public:

        virtual void on_client_payload(PayloadVector, size_t) = 0;
        virtual void on_server_payload(PayloadVector, size_t) = 0;
        virtual void on_connection_close() = 0;
        virtual void on_connection_terminated(TerminationReason) = 0;

        const StreamIdentifier &get_id();

        TCPSniffer() = delete;

        TCPSniffer(const StreamIdentifier&, int);

        virtual ~TCPSniffer();

        virtual void data_callback(
                cs::base::PayloadVector stream_data,
                size_t stream_data_size,
                cs::threads::DataType type
        );

//        static void data_callback(
//                std::shared_ptr<cs::base::TCPSniffer> sniffer_ptr,
//                cs::base::PayloadVector stream_data,
//                size_t stream_data_size,
//                cs::threads::DataType type
//        );
    };

}

#endif //CUCKOOSNIFFER_BASE_SNIFFER_HPP
