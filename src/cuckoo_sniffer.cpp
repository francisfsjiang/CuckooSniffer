#include <iostream>
#include <sstream>
#include "tins/tcp_ip/stream_follower.h"
#include "tins/sniffer.h"
#include "tins/packet.h"
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"


using std::cout;
using std::cerr;
using std::endl;
using std::bind;
using std::string;
using std::to_string;
using std::ostringstream;
using std::exception;

using Tins::Sniffer;
using Tins::SnifferConfiguration;
using Tins::PDU;
using Tins::TCPIP::StreamFollower;
using Tins::TCPIP::Stream;


string client_endpoint(const Stream& stream) {
    ostringstream output;

    if (stream.is_v6()) {
        output << stream.client_addr_v6();
    }
    else {
        output << stream.client_addr_v4();
    }
    output << ":" << stream.client_port();
    return output.str();
}


string server_endpoint(const Stream& stream) {
    ostringstream output;
    if (stream.is_v6()) {
        output << stream.server_addr_v6();
    }
    else {
        output << stream.server_addr_v4();
    }
    output << ":" << stream.server_port();
    return output.str();
}

string stream_identifier(const Stream& stream) {
    ostringstream output;
    output << client_endpoint(stream) << " - " << server_endpoint(stream);
    return output.str();
}


void on_client_data(Stream& stream) {

    string data(stream.client_payload().begin(), stream.client_payload().end());

    cout << client_endpoint(stream) << " >> "
    << server_endpoint(stream) << ": " << endl << data << endl;
}


void on_server_data(Stream& stream) {
    string data(stream.server_payload().begin(), stream.server_payload().end());
    cout << server_endpoint(stream) << " >> "
    << client_endpoint(stream) << ": " << endl << data << endl;
}


void on_connection_closed(Stream& stream) {
    cout << "[+] Connection closed: " << stream_identifier(stream) << endl;
}

void on_new_connection(Stream& stream) {
    if (stream.server_port() == 25)
    {
        std::cout << "[+] New SMTP Connection" << stream_identifier(stream) << std::endl;
        stream.client_data_callback(&on_client_data);
        stream.server_data_callback(&on_server_data);

        stream.stream_closed_callback(&on_connection_closed);
    }

}


void on_connection_terminated(Stream& stream, StreamFollower::TerminationReason reason) {
    cout << "[+] Connection terminated " << stream_identifier(stream) << endl;
}

int main(int argc, char* argv[]) {
    try {
        SnifferConfiguration config;
        config.set_filter("tcp port 25 || tcp port 143");
        // WLAN
        Sniffer sniffer(argv[1], config);

        StreamFollower follower;
        follower.new_stream_callback(&on_new_connection);
        follower.stream_termination_callback(on_connection_terminated);

        sniffer.sniff_loop([&](PDU& packet) {
            follower.process_packet(packet);
            return true;
        });
    }
    catch (exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 1;
    }
}