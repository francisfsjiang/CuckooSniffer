#ifndef _CUCKOOSNIFFER_PCPP_CAPTURER_HPP_
#define _CUCKOOSNIFFER_PCPP_CAPTURER_HPP_

#include <set>

#include "TcpReassembly.h"
#include "PcapLiveDeviceList.h"
#include "PcapFileDevice.h"
#include "PlatformSpecificUtils.h"
#include "SystemUtils.h"
#include "PcapPlusPlusVersion.h"
#include "LRUList.h"

#include "base/sniffer.hpp"


namespace cs {
    class PcpppCapturer {
    public:
        PcpppCapturer(std::string, std::string);
        ~PcpppCapturer();

        static void generate_ignore_list(std::set<std::string>&);

        int get_thread_id();

        void start();

    private:
        int current_thread_id_;
        std::map<uint32_t, std::shared_ptr<cs::base::TCPSniffer>>* sniffer_mannger_;
        pcpp::TcpReassembly* tcp_reassembly_;
        pcpp::PcapLiveDevice* dev_;

        std::string iface_;
        std::string filter_;
        bool stopped_;

//        typedef std::function<int<>>

        static void tcp_msg_ready_callback(int sideIndex, pcpp::TcpStreamData tcpData, void* userCookie);
        static void on_packet_arrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* tcpReassemblyCookie);
        static void tcp_connection_start_callback(pcpp::ConnectionData connectionData, void* userCookie);
        static void tcp_connection_end_callback(pcpp::ConnectionData connectionData, pcpp::TcpReassembly::ConnectionEndReason reason, void* userCookie);
        static void on_app_interrupted(void *cookie);

    };
}


#endif //_CUCKOOSNIFFER_PCPPP_CAPTURER_HPP_
