#ifndef CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP
#define CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP

#include "base/sniffer.hpp"

namespace cs{ namespace util {

class File;

}
}



namespace cs {
namespace ftp {

class DataSniffer : public cs::base::TCPSniffer {

public:

    virtual void on_client_payload(const Tins::TCPIP::Stream &);

    virtual void on_server_payload(const Tins::TCPIP::Stream &);

    virtual void on_connection_close(const Tins::TCPIP::Stream &);

    virtual void on_connection_terminated(
            Tins::TCPIP::Stream &,
            Tins::TCPIP::StreamFollower::TerminationReason);

    DataSniffer(Tins::TCPIP::Stream &);

    virtual ~DataSniffer();

private:

    cs::util::File* file_;
    std::string payload_;

};

}
}


#endif //CUCKOOSNIFFER_FTP_DATA_SNIFFER_HPP
