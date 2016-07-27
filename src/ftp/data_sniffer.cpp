#include <regex>
#include <fstream>

#include "sniffer_manager.hpp"
#include "ftp/data_sniffer.hpp"
#include "ftp/collected_data.hpp"
#include "ftp/data_processor.hpp"
#include "ftp/command_sniffer.hpp"
#include "util/file.hpp"
#include "util/function.hpp"

namespace cs { namespace ftp {

void DataSniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {

}

void DataSniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    std::string data = std::string(
            stream.server_payload().begin(),
            stream.server_payload().end()
    );

    std::cout << data.size() << std::endl;
    payload_ += data;
//    //TODO make this process in thread
//    CollectedData *ftp_data = new CollectedData(
//            std::string(stream.client_payload().begin(), stream.client_payload().end())
//    );
//
//    DataProcessor processor;
//    processor.process(*((CollectedData *) ftp_data));
//
//
//    delete ftp_data;
//    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

void DataSniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {

    std::cout << payload_.size() << std::endl;
    file_ -> set_size(payload_.size());
    file_ -> add_content(payload_.c_str(), payload_.size());

    std::ofstream ofs("test.dll");
    ofs.write(file_ -> buffer_, file_ -> buffer_size_);
    ofs.close();

    std::cout << "FTP data Connection Close" << std::endl;
    CommandSniffer::get_data_connection_pool().erase(stream.server_port());
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

void DataSniffer::on_connection_terminated(
        Tins::TCPIP::Stream& stream,
        Tins::TCPIP::StreamFollower::TerminationReason) {
    std::cout << "[+] On FTP Data Connection terminated " << id_ << std::endl;
    CommandSniffer::get_data_connection_pool().erase(stream.server_port());
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

DataSniffer::DataSniffer(Tins::TCPIP::Stream &stream) : TCPSniffer(stream) {
    std::cout << "get ftp data connection" << std::endl;

    file_ = new cs::util::File();
    file_ -> set_encode_status(true);

    stream.ignore_client_data();
    stream.server_data_callback(
            [this](const Tins::TCPIP::Stream& tcp_stream) {
                this -> on_server_payload(tcp_stream);
            }
    );

    stream.stream_closed_callback(
            [this](const Tins::TCPIP::Stream &tcp_stream) {
                this -> on_connection_close(tcp_stream);
            }
    );

}


DataSniffer::~DataSniffer() {
}

}}