#ifndef CUCKOOSNIFFER_FTP_SNIFFER_HPP
#define CUCKOOSNIFFER_FTP_SNIFFER_HPP

#include <map>

#include "base/sniffer.hpp"

namespace cs::ftp {
    struct dcp_cmp{
        bool operator()(const std::pair<std::string, uint16_t>& lhs, const std::pair<std::string, uint16_t>& rhs) const;
    };

class CommandSniffer : public cs::base::TCPSniffer {

public:

    void on_client_payload(cs::base::PayloadVector, size_t) override;

    void on_server_payload(cs::base::PayloadVector, size_t) override;

    void on_connection_close() override;

    void on_connection_terminated(cs::base::TerminationReason) override;

    CommandSniffer(const cs::base::StreamIdentifier&, int);

    virtual ~CommandSniffer();

    void data_callback(
            cs::base::PayloadVector stream_data,
            size_t stream_data_size,
            cs::threads::DataType type
    ) override;



    typedef std::map<
            std::pair<std::string, uint16_t>,
            std::pair<std::string, int>,
            dcp_cmp> DataConnectionPool;

    static DataConnectionPool data_connection_pool_;

    static DataConnectionPool& get_data_connection_pool();
private:

    int current_port_;
};

}


#endif //CUCKOOSNIFFER_FTP_SNIFFER_HPP
