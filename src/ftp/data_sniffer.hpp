#ifndef CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP
#define CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP

#include "base/sniffer.hpp"

namespace cs::util {
    class Buffer;
}

namespace cs::ftp {

class DataSniffer : public cs::base::TCPSniffer {

public:

    void on_client_payload(cs::base::PayloadVector, size_t) override;

    void on_server_payload(cs::base::PayloadVector, size_t) override;

    void on_connection_close() override;

    void on_connection_terminated(cs::base::TerminationReason) override;

    DataSniffer(const cs::base::StreamIdentifier&, int, std::string, int);

    virtual ~DataSniffer();

private:

    cs::util::Buffer* buffer_;
    int side_;
    std::string file_name_;

};

}


#endif //CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP
