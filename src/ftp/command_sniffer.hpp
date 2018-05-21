#ifndef CUCKOOSNIFFER_FTP_SNIFFER_HPP
#define CUCKOOSNIFFER_FTP_SNIFFER_HPP

#include <map>

#include "base/sniffer.hpp"

namespace cs::ftp {

class CommandSniffer : public cs::base::TCPSniffer {

public:

    void on_client_payload(cs::base::PayloadVector, size_t) override;

    void on_server_payload(cs::base::PayloadVector, size_t) override;

    void on_connection_close() override;

    void on_connection_terminated(cs::base::TerminationReason) override;

    CommandSniffer(const cs::base::StreamIdentifier&, int);

    virtual ~CommandSniffer();

    static std::map<uint16_t, std::string>& get_data_connection_pool();

private:

    static std::map<uint16_t , std::string> data_connection_pool_;

    uint16_t port_;
};

}


#endif //CUCKOOSNIFFER_FTP_SNIFFER_HPP
