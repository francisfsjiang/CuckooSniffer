#include <iostream>
#include <sstream>

#include "tins/tcp_ip/stream_follower.h"
#include "tins/sniffer.h"
#include "tins/packet.h"

#include "cuckoo_sniffer.hpp"

void on_new_connection(Tins::TCPIP::Stream& stream) {
    cs::base::TCPSniffer* tcp_sniffer = nullptr;
    switch (stream.server_port()) {
        case 25:                            //SMTP
            tcp_sniffer = new cs::smtp::Sniffer(stream);
            break;
        case 143:                           //IMAP
            tcp_sniffer = new cs::imap::Sniffer(stream);
            break;
//        case 21:
//            tcp_sniffer = new FTPSniffer(stream);
        default:
            stream.auto_cleanup_payloads(true);
            return;
    }

    cs::SNIFFER_MANAGER.append_sniffer(tcp_sniffer -> get_id(), tcp_sniffer);

}


void on_connection_terminated(Tins::TCPIP::Stream& stream, Tins::TCPIP::StreamFollower::TerminationReason reason) {
    std::cout << "[+] Connection terminated " << cs::base::TCPSniffer::stream_identifier(stream) << std::endl;
    std::string stream_id = cs::base::TCPSniffer::stream_identifier(stream);
    ((cs::base::TCPSniffer*)cs::SNIFFER_MANAGER.get_sniffer(stream_id)) ->on_connection_terminated(stream, reason);
    cs::SNIFFER_MANAGER.erase_sniffer(cs::base::TCPSniffer::stream_identifier(stream));
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: CuckooSniffer <device name>" << std::endl;
        return 0;
    }
    try {
        Tins::SnifferConfiguration config;
        config.set_filter("tcp port 25 || tcp port 143");
        config.set_promisc_mode(true);
        Tins::Sniffer sniffer(argv[1], config);

        Tins::TCPIP::StreamFollower follower;
        follower.new_stream_callback(&on_new_connection);
        follower.stream_termination_callback(on_connection_terminated);

        sniffer.sniff_loop([&](Tins::PDU& packet) {
            Tins::PDU* layer2_pdu = &packet;
            Tins::PDU* layer3_pdu = layer2_pdu->inner_pdu();
            Tins::PDU* layer4_pdu = layer3_pdu->inner_pdu();
            switch (layer2_pdu->pdu_type()) {
                case Tins::PDU::ETHERNET_II:

                    switch (layer3_pdu->pdu_type()) {
                        case Tins::PDU::IP:

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