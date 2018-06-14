#include <iostream>
#include <sstream>
#include <unordered_map>

#include "cuckoo_sniffer.hpp"

#include "base/sniffer.hpp"
#include "threads/data_queue.hpp"
#include "sniffer_manager.hpp"
#include "threads/thread.hpp"
#include "util/function.hpp"
#include "util/option_parser.hpp"

#include "pcpp_capturer.hpp"


std::set<std::string> ignored_ip_address = {
        "127.0.0.1",
        "::1"
};


//void on_connection_terminated(const cs::base::StreamIdentifier& stream_id, TerminationReason _) {
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

//    std::string filter = "tcp port 143";
    std::string filter = "";

    cs::PcpppCapturer pcppp_capturer(interface_name, filter, ignored_ip_address);

    LOG_INFO << "Start sniffer on " << interface_name;

//    std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, std::placeholders::_2, cs::threads::DataType::CLIENT_PAYLOAD);
//
//    std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, std::placeholders::_2, cs::threads::DataType::SERVER_PAYLOAD);
//
//    std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, std::placeholders::_2, cs::threads::DataType::CLOSE);

    pcppp_capturer.start();

}
