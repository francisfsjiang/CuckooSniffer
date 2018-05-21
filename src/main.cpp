#include <iostream>
#include <sstream>
#include <unordered_map>

#include "cuckoo_sniffer.hpp"

#include "base/sniffer.hpp"
#include "threads/data_queue.hpp"
#include "sniffer_manager.hpp"
#include "threads/thread.hpp"
//#include "smtp/sniffer.hpp"
//#include "imap/sniffer.hpp"
//#include "ftp/data_sniffer.hpp"
//#include "ftp/command_sniffer.hpp"
//#include "samba/sniffer.hpp"
#include "http/sniffer.hpp"
#include "util/function.hpp"
#include "util/option_parser.hpp"

#include "pcpp_capturer.hpp"

int CURRENT_THREAD_ID = 0;


void data_callback(std::shared_ptr<cs::base::TCPSniffer> sniffer_ptr, int thread_id, cs::base::PayloadVector* stream_data, cs::threads::DataType type) {
    LOG_TRACE << "Callback: " << (int)type;

    cs::base::PayloadVector* payload_ptr = nullptr;
    if (type == cs::threads::DataType::CLIENT_PAYLOAD) {
        payload_ptr = stream_data;
    }
    else if (type == cs::threads::DataType::SERVER_PAYLOAD) {
        payload_ptr = stream_data;
    }
    else {
        payload_ptr = nullptr;
    }

    auto de = new cs::threads::DataEvent(
            sniffer_ptr, type, payload_ptr
    );

    cs::threads::DATA_QUEUES[thread_id]->enqueue(de);
}


std::set<std::string> ignored_ip_address = {
        "127.0.0.1",
        "::1"
};

int on_new_connection(cs::base::StreamIdentifier& stream_id) {

    LOG_TRACE << stream_id.to_string() << " Get tcp stream." ;

    cs::base::TCPSniffer* tcp_sniffer = nullptr;

    switch (stream_id.dst_port) {
//        case 25:        //SMTP
//            tcp_sniffer = new cs::smtp::Sniffer(stream);
//            break;
//        case 143:       //IMAP
//            tcp_sniffer = new cs::imap::Sniffer(stream);
//            break;
//        case 21:        //FTP
//            tcp_sniffer = new cs::ftp::CommandSniffer(stream);
//            break;
//        case 80:        //HTTP
        case 8888:        //HTTP
//            if (cs::util::is_ignore_stream(stream, ignore_ipv4_address, ignore_ipv6_address)) {
//                LOG_INFO << "Ingoring stream " << stream_id;
//                return;
//            }
            tcp_sniffer = new cs::http::Sniffer(stream_id);
            break;
//        case 445:       //SAMBA
//            tcp_sniffer = new cs::samba::Sniffer(stream);
//            break;
        default:
//            const auto& ftp_data_connection = cs::ftp::CommandSniffer::get_data_connection_pool();
//            if (ftp_data_connection.find(port) != ftp_data_connection.end()) {
//                tcp_sniffer = new cs::ftp::DataSniffer(stream);
//            }
//            else {
//                stream.auto_cleanup_payloads(true);
//                return;
//            }
            break;
    }
    if (!tcp_sniffer) {
        return -1;
    }

    std::shared_ptr<cs::base::TCPSniffer> sniffer_ptr = std::shared_ptr<cs::base::TCPSniffer>(tcp_sniffer);

    int thread_id = CURRENT_THREAD_ID++;
    if(CURRENT_THREAD_ID >= cs::threads::THREADS_NUM) {
        CURRENT_THREAD_ID = 0;
    }


            std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, cs::threads::DataType::CLIENT_PAYLOAD);

            std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, cs::threads::DataType::SERVER_PAYLOAD);

            std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, cs::threads::DataType::CLOSE);

    cs::SNIFFER_MANAGER.append_sniffer(sniffer_ptr -> get_id(), sniffer_ptr, thread_id);
//    cs::SNIFFER_MANAGER.append_sniffer(tcp_sniffer -> get_id(), (cs::base::Sniffer*)tcp_sniffer);

}


//void on_connection_terminated(const cs::base::StreamIdentifier& stream_id, Tins::TCPIP::StreamFollower::TerminationReason _) {
//    LOG_INFO << "Connection terminated " << stream_id;
//    auto sniffer = cs::SNIFFER_MANAGER.get_sniffer_info(stream_id);
//
//    data_callback(std::dynamic_pointer_cast<cs::base::TCPSniffer>(sniffer.first),  sniffer.second, stream, cs::threads::DataType::TEMINATATION);
//
//
//    cs::SNIFFER_MANAGER.erase_sniffer(stream_id);
//}


int main(int argc, const char* argv[]) {
    int ret = cs::util::parse_cfg(argc, argv, cs::CONFIG);
    if (ret < 0) {
        std::cerr << "Parse config failed." << std::endl;
        return ret;
    }

    cs::init_log();


    cs::PcpppCapturer::generate_ignore_list(ignored_ip_address);

    for(auto addr: ignored_ip_address)
        LOG_INFO << "Ignore IP address: " << addr;


    LOG_INFO << "Config:";
    for(const auto& cfg: cs::CONFIG) {
        LOG_INFO << cfg.first << " = " << cfg.second;
    }

    std::string interface_name = cs::CONFIG["interface"];

    cs::threads::start_threads(2);
    CURRENT_THREAD_ID = 0;

    std::string filter = "tcp port 8888";

    cs::PcpppCapturer pcppp_capturer(interface_name, filter);

    LOG_INFO << "Start sniffer on " << interface_name;

    pcppp_capturer.start();

}
