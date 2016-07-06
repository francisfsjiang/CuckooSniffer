#include <iostream>
#include <sstream>
#include "tins/tcp_ip/stream_follower.h"
#include "tins/sniffer.h"
#include "tins/packet.h"
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"

#include "sniffer_manager.hpp"


std::string client_endpoint(const Tins::TCPIP::Stream& stream) {
    std::ostringstream output;

    if (stream.is_v6()) {
        output << stream.client_addr_v6();
    }
    else {
        output << stream.client_addr_v4();
    }
    output << ":" << stream.client_port();
    return output.str();
}


std::string server_endpoint(const Tins::TCPIP::Stream& stream) {
    std::ostringstream output;
    if (stream.is_v6()) {
        output << stream.server_addr_v6();
    }
    else {
        output << stream.server_addr_v4();
    }
    output << ":" << stream.server_port();
    return output.str();
}

std::string stream_identifier(const Tins::TCPIP::Stream& stream) {
    std::ostringstream output;
    output << client_endpoint(stream) << " - " << server_endpoint(stream);
    return output.str();
}


void on_client_data(Tins::TCPIP::Stream& stream) {

    std::string data(stream.client_payload().begin(), stream.client_payload().end());

    std::cout << client_endpoint(stream) << " >> "
    << server_endpoint(stream) << ": " << std::endl << data << std::endl;
}


void on_server_data(Tins::TCPIP::Stream& stream) {
    std::string data(stream.server_payload().begin(), stream.server_payload().end());
    std::cout << server_endpoint(stream) << " >> "
    << client_endpoint(stream) << ": " << std::endl << data << std::endl;
}


void on_connection_closed(Tins::TCPIP::Stream& stream) {
    std::cout << "[+] Connection closed: " << stream_identifier(stream) << std::endl;
}


void on_new_connection(Tins::TCPIP::Stream& stream) {
    std::string stream_id = stream_identifier(stream);
    Sniffer* sniffer = SNIFFER_MANAGER.get_sniffer(stream.server_port());
        stream.client_data_callback(&on_client_data);
        stream.server_data_callback(&on_server_data);

        stream.stream_closed_callback(&on_connection_closed);

}


void on_connection_terminated(Tins::TCPIP::Stream& stream, Tins::TCPIP::StreamFollower::TerminationReason reason) {
    std::cout << "[+] Connection terminated " << stream_identifier(stream) << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        Tins::SnifferConfiguration config;
        config.set_filter("tcp port 25 || tcp port 143");
        Tins::Sniffer sniffer(argv[1], config);

        Tins::TCPIP::StreamFollower follower;
        follower.new_stream_callback(&on_new_connection);
        follower.stream_termination_callback(on_connection_terminated);

        sniffer.sniff_loop([&](Tins::PDU& packet) {
            follower.process_packet(packet);
            return true;
        });
    }
    catch (std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}