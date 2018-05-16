#include <string>
#include <iostream>
#include <stdexcept>
#include <boost/regex.hpp>
#include "tins/tcp_ip/stream_follower.h"
#include "tins/sniffer.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::exception;

using boost::regex;
using boost::match_results;

using Tins::Packet;
using Tins::Sniffer;
using Tins::SnifferConfiguration;
using Tins::TCPIP::Stream;
using Tins::TCPIP::StreamFollower;

// This example captures and follows TCP streams seen on port 80. It will
// wait until both the client and server send data and then apply a regex
// to both payloads, extrating some information and printing it.

// Don't buffer more than 3kb of data in either request/response
const size_t MAX_PAYLOAD = 3 * 1024;
// The regex to be applied on the request. This will extract the HTTP
// method being used, the request's path and the Host header value.
regex request_regex("([\\w]+) ([^ ]+).+\r\nHost: ([\\d\\w\\.-]+)\r\n");
// The regex to be applied on the response. This finds the response code.
regex response_regex("HTTP/[^ ]+ ([\\d]+)");

void on_server_data(Stream& stream) {
}

void on_client_data(Stream& stream) {
}

void on_close(Stream& stream) {
    std::cout << stream.client_payload().size() << std::endl;
    std::cout << stream.server_payload().size() << std::endl;
    std::cout << "Connection close" << std::endl;
}

void on_new_connection(Stream& stream) {
    std::cout << "New connection" << std::endl;
    stream.client_data_callback(&on_client_data);
    stream.server_data_callback(&on_server_data);
    stream.stream_closed_callback(&on_close);
    // Don't automatically cleanup the stream's data, as we'll manage
    // the buffer ourselves and let it grow until we see a full request
    // and response
    stream.auto_cleanup_payloads(false);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <interface>" << endl;
        return 1;
    }

    try {
        // Construct the sniffer configuration object
        SnifferConfiguration config;
        // Only capture TCP traffic sent from/to port 80
        config.set_filter("tcp port 8888");
        // Construct the sniffer we'll use
        Sniffer sniffer(argv[1], config);

        cout << "Starting capture on interface " << argv[1] << endl;

        // Now construct the stream follower
        StreamFollower follower;
        // We just need to specify the callback to be executed when a new
        // stream is captured. In this stream, you should define which callbacks
        // will be executed whenever new data is sent on that stream
        // (see on_new_connection)
        follower.new_stream_callback(&on_new_connection);
        // Now start capturing. Every time there's a new packet, call
        // follower.process_packet
        int num = 0;
        sniffer.sniff_loop([&](Packet& packet) {
            std::cout << packet.pdu()->size() << ", total num: " << num << std::endl;
            ++num;
            follower.process_packet(packet);
            return true;
        });
    }
    catch (exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 1;
    }
}