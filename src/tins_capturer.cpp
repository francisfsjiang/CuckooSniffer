
#include "tins_capturer.hpp"

#include <iostream>
#include <sstream>
#include <unordered_map>

#include "cuckoo_sniffer.hpp"

#include "tins/tcp_ip/stream_follower.h"
#include "tins/sniffer.h"
#include "tins/network_interface.h"

#include "threads/data_queue.hpp"
#include "sniffer_manager.hpp"
#include "threads/thread.hpp"
#include "smtp/sniffer.hpp"
#include "imap/sniffer.hpp"
#include "ftp/data_sniffer.hpp"
#include "ftp/command_sniffer.hpp"
#include "http/sniffer.hpp"
#include "samba/sniffer.hpp"
#include "util/function.hpp"
#include "util/option_parser.hpp"



std::map<int, const char*> pdu_type_to_str = {
        {Tins::PDU::RAW,                  "RAW"},
        {Tins::PDU::ETHERNET_II,          "ETHERNET_II"},
        {Tins::PDU::IEEE802_3,            "IEEE802_3"},
        {Tins::PDU::IEEE802_3,            "IEEE802_3"},
        {Tins::PDU::RADIOTAP,             "RADIOTAP"},
        {Tins::PDU::DOT11,                "DOT11"},
        {Tins::PDU::DOT11_ACK,            "DOT11_ACK"},
        {Tins::PDU::DOT11_ASSOC_REQ,      "DOT11_ASSOC_REQ"},
        {Tins::PDU::DOT11_ASSOC_RESP,     "DOT11_ASSOC_RESP"},
        {Tins::PDU::DOT11_AUTH,           "DOT11_AUTH"},
        {Tins::PDU::DOT11_BEACON,         "DOT11_BEACON"},
        {Tins::PDU::DOT11_BLOCK_ACK,      "DOT11_BLOCK_ACK"},
        {Tins::PDU::DOT11_BLOCK_ACK_REQ,  "DOT11_BLOCK_ACK_REQ"},
        {Tins::PDU::DOT11_CF_END,         "DOT11_CF_END"},
        {Tins::PDU::DOT11_DATA,           "DOT11_DATA"},
        {Tins::PDU::DOT11_CONTROL,        "DOT11_CONTROL"},
        {Tins::PDU::DOT11_DEAUTH,         "DOT11_DEAUTH"},
        {Tins::PDU::DOT11_DIASSOC,        "DOT11_DIASSOC"},
        {Tins::PDU::DOT11_END_CF_ACK,     "DOT11_END_CF_ACK"},
        {Tins::PDU::DOT11_MANAGEMENT,     "DOT11_MANAGEMENT"},
        {Tins::PDU::DOT11_PROBE_REQ,      "DOT11_PROBE_REQ"},
        {Tins::PDU::DOT11_PROBE_RESP,     "DOT11_PROBE_RESP"},
        {Tins::PDU::DOT11_PS_POLL,        "DOT11_PS_POLL"},
        {Tins::PDU::DOT11_REASSOC_REQ,    "DOT11_REASSOC_REQ"},
        {Tins::PDU::DOT11_REASSOC_RESP,   "DOT11_REASSOC_RESP"},
        {Tins::PDU::DOT11_RTS,            "DOT11_RTS"},
        {Tins::PDU::DOT11_QOS_DATA,       "DOT11_QOS_DATA"},
        {Tins::PDU::LLC,                  "LLC"},
        {Tins::PDU::SNAP,                 "SNAP"},
        {Tins::PDU::IP,                   "IP"},
        {Tins::PDU::ARP,                  "ARP"},
        {Tins::PDU::TCP,                  "TCP"},
        {Tins::PDU::UDP,                  "UDP"},
        {Tins::PDU::ICMP,                 "ICMP"},
        {Tins::PDU::BOOTP,                "BOOTP"},
        {Tins::PDU::DHCP,                 "DHCP"},
        {Tins::PDU::EAPOL,                "EAPOL"},
        {Tins::PDU::RC4EAPOL,             "RC4EAPOL"},
        {Tins::PDU::RSNEAPOL,             "RSNEAPOL"},
        {Tins::PDU::DNS,                  "DNS"},
        {Tins::PDU::LOOPBACK,             "LOOPBACK"},
        {Tins::PDU::IPv6,                 "IPv6"},
        {Tins::PDU::ICMPv6,               "ICMPv6"},
        {Tins::PDU::SLL,                  "SLL"},
        {Tins::PDU::DHCPv6,               "DHCPv6"},
        {Tins::PDU::DOT1Q,                "DOT1Q"},
        {Tins::PDU::PPPOE,                "PPPOE"},
        {Tins::PDU::STP,                  "STP"},
        {Tins::PDU::PPI,                  "PPI"},
        {Tins::PDU::IPSEC_AH,             "IPSEC_AH"},
        {Tins::PDU::IPSEC_ESP,            "IPSEC_ESP"},
        {Tins::PDU::PKTAP,                "PKTAP"},
        {Tins::PDU::MPLS,                 "MPLS"},
        {Tins::PDU::UNKNOWN,              "UNKNOWN"},
        {Tins::PDU::USER_DEFINED_PDU,     "USER_DEFINED_PDU"},
};


int CURRENT_THREAD_ID = 0;

int total_bytes = 0;

void data_callback(std::shared_ptr<cs::base::TCPSniffer> sniffer_ptr, int thread_id, const Tins::TCPIP::Stream &tcp_stream, cs::threads::DataType type) {
    LOG_TRACE << "Callback: " << (int)type;

    cs::base::PayloadType* payload_ptr = nullptr;
    if (type == cs::threads::DataType::CLIENT_PAYLOAD) {
        payload_ptr = new cs::base::PayloadType(tcp_stream.client_payload());
    }
    else if (type == cs::threads::DataType::SERVER_PAYLOAD) {
        payload_ptr = new cs::base::PayloadType(tcp_stream.server_payload());
        total_bytes += tcp_stream.server_payload().size();
        LOG_TRACE << std::string((char*)tcp_stream.server_payload().data(), tcp_stream.server_payload().size());
        LOG_TRACE << "Total server bytes: " << total_bytes;
    }
    else {
        payload_ptr = nullptr;
    }

    auto de = new cs::threads::DataEvent(
            sniffer_ptr, type, payload_ptr
    );

    cs::threads::DATA_QUEUES[thread_id]->enqueue(de);
}


std::set<Tins::IPv4Address> ignore_ipv4_address = {
        Tins::IPv4Address("127.0.0.1")
};

std::set<Tins::IPv6Address> ignore_ipv6_address = {
        Tins::IPv6Address("::1")
};

void on_new_connection(Tins::TCPIP::Stream& stream) {
    uint16_t port = stream.server_port();
    LOG_TRACE << cs::util::stream_identifier(stream) << " Get tcp stream." ;

    std::string stream_id = cs::util::stream_identifier(stream);

    cs::base::TCPSniffer* tcp_sniffer = nullptr;

    switch (port) {
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
        stream.auto_cleanup_payloads(true);
        return;
    }

    std::shared_ptr<cs::base::TCPSniffer> sniffer_ptr = std::shared_ptr<cs::base::TCPSniffer>(tcp_sniffer);

    int thread_id = CURRENT_THREAD_ID++;
    if(CURRENT_THREAD_ID >= cs::threads::THREADS_NUM) {
        CURRENT_THREAD_ID = 0;
    }


    stream.client_data_callback(
            std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, cs::threads::DataType::CLIENT_PAYLOAD)
    );

    stream.server_data_callback(
            std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, cs::threads::DataType::SERVER_PAYLOAD)
    );

    stream.stream_closed_callback(
            std::bind(&data_callback, sniffer_ptr, thread_id, std::placeholders::_1, cs::threads::DataType::CLOSE)
    );
    stream.auto_cleanup_client_data(true);
    stream.auto_cleanup_payloads(true);
    stream.auto_cleanup_server_data(true);

    cs::SNIFFER_MANAGER.append_sniffer(sniffer_ptr -> get_id(), sniffer_ptr, thread_id);
//    cs::SNIFFER_MANAGER.append_sniffer(tcp_sniffer -> get_id(), (cs::base::Sniffer*)tcp_sniffer);

}


void on_connection_terminated(Tins::TCPIP::Stream& stream, TerminationReason _) {
    std::string stream_id = cs::util::stream_identifier(stream);
    LOG_INFO << "Connection terminated " << stream_id;
    auto sniffer = cs::SNIFFER_MANAGER.get_sniffer_info(stream_id);

    data_callback(std::dynamic_pointer_cast<cs::base::TCPSniffer>(sniffer.first),  sniffer.second, stream, cs::threads::DataType::TEMINATATION);


    cs::SNIFFER_MANAGER.erase_sniffer(stream_id);
}


int main(int argc, const char* argv[]) {
    try {
        int ret = cs::util::parse_cfg(argc, argv, cs::CONFIG);
        if (ret < 0) {
            std::cerr << "Parse config failed." << std::endl;
            return ret;
        }

        cs::init_log();

        std::vector<Tins::NetworkInterface> iface_vec = Tins::NetworkInterface::all();
        for(auto iface: iface_vec) {
            auto info = iface.info();
            ignore_ipv4_address.insert(info.ip_addr);
            for(auto ipv6_prefix: info.ipv6_addrs) {
                ignore_ipv6_address.insert(ipv6_prefix.address);
            }
        }

        for(auto addr: ignore_ipv4_address)
            LOG_INFO << "Ignore IPv4 address: " << addr;
        for(auto addr: ignore_ipv6_address)
            LOG_INFO << "Ignore IPv6 address: " << addr;


        LOG_INFO << "Config:";
        for(const auto& cfg: cs::CONFIG) {
            LOG_INFO << cfg.first << " = " << cfg.second;
        }

        std::string interface_name = cs::CONFIG["interface"];

        cs::threads::start_threads(2);
        CURRENT_THREAD_ID = 0;

        Tins::SnifferConfiguration config;
//        config.set_filter("ip6 host 2001:da8:215:1660:bbb2:3d41:d32b:dc53");
        config.set_filter("tcp port 8888");
//        config.set_filter("tcp port 80");
        config.set_promisc_mode(true);
        Tins::Sniffer sniffer(interface_name, config);

        LOG_INFO << "Start sniffer on " << interface_name;

        Tins::TCPIP::StreamFollower follower;
        follower.new_stream_callback(&on_new_connection);
        follower.stream_termination_callback(on_connection_terminated);

        int num = 0;
        sniffer.sniff_loop([&](Tins::PDU& packet) {
            ++num;
            Tins::PDU* layer2_pdu = &packet;
            Tins::PDU* layer3_pdu = layer2_pdu->inner_pdu();
            Tins::PDU* layer4_pdu = layer3_pdu->inner_pdu();
            try {
                LOG_TRACE << "Packet " << packet.size() << " receiving. " << num << " , " << pdu_type_to_str[(int)(layer3_pdu->pdu_type())] << pdu_type_to_str[(int)(layer4_pdu->pdu_type())];
            }
            catch (std::exception){
                LOG_ERROR << "No layer 4 ";

            }

            switch (layer2_pdu->pdu_type()) {
                case Tins::PDU::LOOPBACK:
                case Tins::PDU::ETHERNET_II:

                    switch (layer3_pdu->pdu_type()) {
                        case Tins::PDU::IP:
                        case Tins::PDU::IPv6:

                            switch (layer4_pdu->pdu_type()) {
                                case Tins::PDU::TCP:
                                    follower.process_packet(packet);
                                    break;
                                default:
                                    break;
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            return true;
        });
    }
    catch (std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
