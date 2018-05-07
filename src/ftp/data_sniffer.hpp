#ifndef CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP
#define CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP

#include "base/sniffer.hpp"

namespace cs::util {

class Buffer;

}



namespace cs::ftp {

class DataSniffer : public cs::base::TCPSniffer {

public:

    virtual void on_client_payload(const cs::base::payload_type&);

    virtual void on_server_payload(const cs::base::payload_type&);

    virtual void on_connection_close();

    virtual void on_connection_terminated(cs::base::termination_reason);

    DataSniffer(const std::string&);

    virtual ~DataSniffer();

private:

    cs::util::Buffer* buffer_;

};

}


#endif //CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP
