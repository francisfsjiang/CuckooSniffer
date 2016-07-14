#include <iostream>
#include <sstream>

#include "tins/tcp_ip/stream_follower.h"
#include "tins/sniffer.h"
#include "tins/packet.h"

#include "sniffer_manager.hpp"
#include "sniffer.hpp"



void on_new_connection(Tins::TCPIP::Stream& stream) {
    std::string stream_id = TCPSniffer::stream_identifier(stream);
    TCPSniffer* tcp_sniffer;
    switch (stream.server_port()) {
        case 25:                            //SMTP
            tcp_sniffer = new SMTPSniffer(stream);
            break;
        default:
            return;
    }

    SNIFFER_MANAGER.append_sniffer(stream_id, tcp_sniffer);

}


void on_connection_terminated(Tins::TCPIP::Stream& stream, Tins::TCPIP::StreamFollower::TerminationReason reason) {
    std::cout << "[+] Connection terminated " << TCPSniffer::stream_identifier(stream) << std::endl;
    std::string stream_id = TCPSniffer::stream_identifier(stream);
    ((TCPSniffer*)SNIFFER_MANAGER.get_sniffer(stream_id)) ->on_connection_terminated(stream, reason);
    SNIFFER_MANAGER.erase_sniffer(TCPSniffer::stream_identifier(stream));
}


int main(int argc, char* argv[]) {
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